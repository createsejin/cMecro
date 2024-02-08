//
// Created by creat on 2024-02-07.
//
#include "sql_executive.h"
#include "key_patterns.h"
#include "../lib/sqlite3.h"
#include <iostream>
using namespace std;

namespace sql_executive
{
    sqlite3* mecro_data;

    int open_mecro_data() {
        if (sqlite3_open_v2("./mecro_data.db", &mecro_data,
            SQLITE_OPEN_READWRITE, nullptr) != SQLITE_OK) {
            std::cerr << "Failed to open mecro_data.db database \u25A1" << std::endl;
            sqlite3_close_v2(mecro_data);
            return 1;
            }
        cout << "Opened mecro_data.db database successfully \u25A0" << endl;
        return 0;
    }

    void insert_key_code() {
        if (open_mecro_data() != 0) return;

        // BEGIN TRANSACTION
        char* errmsg;
        if (sqlite3_exec(mecro_data, "BEGIN TRANSACTION;", nullptr, nullptr, &errmsg) != SQLITE_OK) {
            std::cerr << "Failed to begin transaction \u25A1" << std::endl;
            sqlite3_close_v2(mecro_data);
            return;
        }

        const auto* sql = "INSERT INTO key_code(key_value, key_name) VALUES(?, ?);";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(mecro_data, sql, -1, &stmt, nullptr) != SQLITE_OK) {
            std::cerr << "Failed to prepare statement \u25A1" << std::endl;
            sqlite3_close_v2(mecro_data);
            return;
        }
        key_patterns::KeyCodeManager& key_code_manager = key_patterns::KeyCodeManager::getInstance();
        const auto& key_code_map = key_code_manager.get_keyCode_name_map();
        for (const auto&[key, value] : key_code_map) {
            // Bind the key_value parameter
            if (sqlite3_bind_int(stmt, 1, static_cast<int>(key)) != SQLITE_OK) {
                std::cerr << "Failed to bind key value \u25A1" << std::endl;
                sqlite3_finalize(stmt);
                sqlite3_close_v2(mecro_data);
                return;
            }
            // Bind the key_name parameter
            if (sqlite3_bind_text(stmt, 2, value.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) {
                std::cerr << "Failed to bind key name \u25A1" << std::endl;
                sqlite3_finalize(stmt);
                sqlite3_close_v2(mecro_data);
                return;
            }
            // Execute the statement
            if (sqlite3_step(stmt) != SQLITE_DONE) {
                std::cerr << "Failed to execute SQL statement \u25A1" << std::endl;
                sqlite3_finalize(stmt);
                sqlite3_close_v2(mecro_data);
                return;
            }
            // Reset the statement for the next iteration
            if (sqlite3_reset(stmt) != SQLITE_OK) {
                std::cerr << "Failed to reset the statement \u25A1" << std::endl;
                sqlite3_finalize(stmt);
                sqlite3_close_v2(mecro_data);
                return;
            }
        }
        // Finalize the statement
        sqlite3_finalize(stmt);
        cout << "Inserted key codes successfully \u25A0" << endl << endl;

        cout << "Result of the table key_code:" << endl;
        // Execute SELECT * FROM key_code;
        sql = "SELECT * FROM key_code;";
        if (sqlite3_prepare_v2(mecro_data, sql, -1, &stmt, nullptr) != SQLITE_OK) {
            std::cerr << "Failed to prepare statement \u25A1" << std::endl;
            sqlite3_close_v2(mecro_data);
            return;
        }
        cout << "pk | key_value | key_name" << endl;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            cout << sqlite3_column_int(stmt, 0)<< " | " << sqlite3_column_int(stmt, 1) << " | "
            << sqlite3_column_text(stmt, 2) << endl;
        }
        // Finalize the statement
        sqlite3_finalize(stmt);

        // Ask user to commit or rollback
        while (true) {
            string input;
            cout << "Do you want to commit or rollback? (commit/rollback)" << endl;
            cout << "cmd> ";
            std::getline(std::cin, input);
            const auto pos = input.find("cmd>");
            if (pos != std::string::npos) {
                input.erase(pos, 5);
            }
            if (input == "commit") {
                if (sqlite3_exec(mecro_data, "COMMIT;", nullptr, nullptr, &errmsg) != SQLITE_OK) {
                    std::cerr << "Failed to commit transaction \u25A1" << std::endl;
                    sqlite3_free(errmsg);
                }
                cout << "Committed transaction \u25A0" << endl;
                break;
            }
            if (input == "rollback") {
                if (sqlite3_exec(mecro_data, "ROLLBACK;", nullptr, nullptr, &errmsg) != SQLITE_OK) {
                    std::cerr << "Failed to rollback transaction \u25A1" << std::endl;
                    sqlite3_free(errmsg);
                }
                cout << "Rolled back transaction \u25A0" << endl;
                break;
            }
            std::cerr << "Unknown command \u25A1" << std::endl;
        }
        sqlite3_close_v2(mecro_data);
    }
}