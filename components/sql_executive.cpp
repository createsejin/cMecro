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
    bool debug_sql{true};

    int open_mecro_data() {
        const auto rc = sqlite3_open_v2("./mecro_data.db", &mecro_data,
            SQLITE_OPEN_READWRITE, nullptr);
        if (rc != SQLITE_OK) {
            std::cerr << "Failed to open mecro_data.db database \u25A1" << std::endl;
            sqlite3_close_v2(mecro_data);
            return rc;
            }
        if (debug_sql)
            cout << "Opened mecro_data.db database successfully \u25A0" << endl;
        return rc;
    }

    int begin_transaction(sqlite3* db) {
        char* errmsg;
        if (debug_sql)
            cout << "Begin transaction." << endl;
        const auto rc = sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, &errmsg);
        if (rc != SQLITE_OK) {
            std::cerr << "Failed to begin transaction: " << errmsg << " \u25A1" << std::endl;
            sqlite3_free(errmsg);
            sqlite3_close_v2(db);
            return rc;
        }
        sqlite3_free(errmsg);
        return rc;
    }

    auto prepare_stmt(sqlite3* db, const char* sql) -> sqlite3_stmt* {
        sqlite3_stmt* stmt;
        const auto rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            std::cerr << "Failed to prepare statement: "<< sqlite3_errmsg(db) <<" \u25A1" << std::endl;
            sqlite3_close_v2(db);
            return nullptr;
        }
        return stmt;
    }
    int step_stmt(sqlite3* db, sqlite3_stmt* stmt) {
        const auto rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            std::cerr << "Failed to execute SQL statement: "<< sqlite3_errmsg(db) <<" \u25A1" << std::endl;
            sqlite3_finalize(stmt);
            sqlite3_close_v2(db);
            return rc;
        }
        return rc;
    }
    int reset_stmt(sqlite3* db, sqlite3_stmt* stmt) {
        const auto rc = sqlite3_reset(stmt);
        if (rc != SQLITE_OK) {
            std::cerr << "Failed to reset the statement: "<< sqlite3_errmsg(db) <<" \u25A1" << std::endl;
            sqlite3_finalize(stmt);
            sqlite3_close_v2(db);
            return rc;
        }
        return rc;
    }

    int sql_bind_int(sqlite3* db, sqlite3_stmt* stmt, const int pos, const int value) {
        const auto rc = sqlite3_bind_int(stmt, pos, value);
        if (rc != SQLITE_OK) {
            std::cerr << "Failed to bind key value: " << sqlite3_errmsg(db) << " \u25A1" << std::endl;
            sqlite3_finalize(stmt);
            sqlite3_close_v2(db);
            return rc;
        }
        return rc;
    }
    int sql_bind_text(sqlite3* db, sqlite3_stmt* stmt, const int pos, const char* text) {
        const auto rc = sqlite3_bind_text(stmt, pos, text, -1, SQLITE_STATIC);
        if (rc != SQLITE_OK) {
            std::cerr << "Failed to bind key name: " << sqlite3_errmsg(db) << " \u25A1" << std::endl;
            sqlite3_finalize(stmt);
            sqlite3_close_v2(db);
            return rc;
        }
        return rc;
    }
    int commit_or_rollback(sqlite3* db) {
        if (db == nullptr) return 1;
        char* errmsg;
        while (true) {
            string input;
            cout << "Do you want to commit or rollback? (commit/rollback)" << endl;
            cout << "cmd> ";
            std::getline(std::cin, input);
            const auto pos = input.find("cmd>");
            if (pos != std::string::npos) {
                input.erase(pos, 5);
            }
            // command를 lower case로 변환
            ranges::transform(input.begin(), input.end(), input.begin(),
                [](const unsigned char c) {
                return std::tolower(c);
            });
            if (input == "commit") {
                const auto rc = sqlite3_exec(db, "COMMIT;", nullptr, nullptr, &errmsg);
                if (rc != SQLITE_OK) {
                    std::cerr << "Failed to commit transaction: " << errmsg << " \u25A1" << std::endl;
                    sqlite3_free(errmsg);
                    return rc;
                }
                cout << "Committed transaction. \u25A0" << endl;
                break;
            }
            if (input == "rollback") {
                const auto rc = sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, &errmsg);
                if (rc != SQLITE_OK) {
                    std::cerr << "Failed to rollback transaction: " << errmsg << " \u25A1" << std::endl;
                    sqlite3_free(errmsg);
                    return rc;
                }
                cout << "Rolled back transaction. \u25A0" << endl;
                break;
            }
            std::cerr << "Unknown command. \u25A1" << std::endl;
        }
        return 0;
    }

    void insert_key_code() {
        if (open_mecro_data() != 0) return;

        // BEGIN TRANSACTION
        begin_transaction(mecro_data);

        const auto* sql = "INSERT INTO key_code(key_value, key_name) VALUES(?, ?);";
        sqlite3_stmt* stmt = prepare_stmt(mecro_data, sql); if (stmt == nullptr) return;

        key_patterns::KeyCodeManager& key_code_manager = key_patterns::KeyCodeManager::getInstance();
        const auto& key_code_map = key_code_manager.get_keyCode_name_map();

        for (const auto& [key, value] : key_code_map) {
            // Bind the key_value parameter
            sql_bind_int(mecro_data, stmt, 1, static_cast<int>(key));
            // Bind the key_name parameter
            sql_bind_text(mecro_data, stmt, 2, value.c_str());
            // Execute the statement
            step_stmt(mecro_data, stmt);
            // Reset the statement for the next iteration
            reset_stmt(mecro_data, stmt);
        }
        // Finalize the statement
        sqlite3_finalize(stmt);
        if (debug_sql)
            cout << "Inserted key codes successfully \u25A0" << endl << endl;

        if (debug_sql)
            cout << "Result of the table key_code:" << endl;
        // Execute SELECT * FROM key_code;
        sql = "SELECT * FROM key_code;";
        stmt = prepare_stmt(mecro_data, sql); if (stmt == nullptr) return;
        cout << "pk | key_value | key_name" << endl;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            cout << sqlite3_column_int(stmt, 0)<< " | " << sqlite3_column_int(stmt, 1) << " | "
            << sqlite3_column_text(stmt, 2) << endl;
        }
        // Finalize the statement
        sqlite3_finalize(stmt);

        // Ask user to commit or rollback
        commit_or_rollback(mecro_data);

        sqlite3_close_v2(mecro_data);
    }
}