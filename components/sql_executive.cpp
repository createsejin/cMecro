//
// Created by creat on 2024-02-07.
//
#include "sql_executive.h"
#include "key_patterns.h"
#include "../lib/sqlite3.h"
#include <iostream>
#include <fstream>
using namespace std;

namespace sql_executive
{
    bool debug_sql{true};

    auto open_db_as_memory_db(const char* file_path) -> sqlite3* {
        ifstream file(file_path, ios::binary | ios::ate);
        const streamsize size {file.tellg()};
        file.seekg(0, ios::beg);

        vector<char> buffer(size);
        if (!file.read(buffer.data(), size)) {
            cerr << "Falied to read database file" << endl;
            return nullptr;
        }
        sqlite3* memory_db;
        if (sqlite3_open_v2(":memory:", &memory_db,
       SQLITE_OPEN_READWRITE, nullptr) != SQLITE_OK) {
            cerr << "Failed to open SQLite memory database" << endl;
            return nullptr;
       }
        // 3. sqlite3_deserialize 함수를 사용하여 이 메모리 버퍼를 SQLite 데이터베이스로 변환한다.
        sqlite3_deserialize(memory_db, "main", reinterpret_cast<unsigned char*>(buffer.data()),
            size, static_cast<sqlite3_int64>(buffer.size()),
            SQLITE_DESERIALIZE_RESIZEABLE);
        return memory_db;
    }
    sqlite3* memory_db = open_db_as_memory_db(".\\mecro_data.db");

    auto open_database(const char* file_name, const int flag) -> sqlite3* {
        sqlite3* db;
        const auto rc = sqlite3_open_v2(file_name, &db,
            flag, nullptr);
        if (rc != SQLITE_OK) {
            std::cerr << "Failed to open mecro_data.db database \u25A1" << std::endl;
            sqlite3_close_v2(db);
            return nullptr;
            }
        if (debug_sql)
            cout << "Opened mecro_data.db database successfully \u25A0" << endl;
        return db;
    }
    //sqlite3* mecro_data = open_database("./mecro_data.db", SQLITE_OPEN_READWRITE);

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
    auto get_input() -> string {
        string input;
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
        return input;
    }
    int commit_or_rollback(sqlite3* db) {
        if (db == nullptr) return 1;
        char* errmsg;
        while (true) {
            cout << "Do you want to commit or rollback? (commit/rollback)" << endl;
            cout << "sql> ";
            string input {get_input()};
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

    void save_to_disk_db(const char* file_path) {
        sqlite3* disk_db = open_database(file_path, SQLITE_OPEN_READWRITE);
        if (disk_db == nullptr) return;
        while (true) {
            cout << "Do you want to save the memory database to disk? (yes/no)" << endl;
            cout << "sql> ";
            string input {get_input()};
            if (input == "yes" || input == "y") {

                sqlite3_backup* backup = sqlite3_backup_init(disk_db, "main",
                    memory_db, "main");
                if (backup) {
                    sqlite3_backup_step(backup, -1);
                    sqlite3_backup_finish(backup);
                }
                sqlite3_close_v2(disk_db);
                cout << "Saved the memory database to disk. \u25A0" << endl;
                break;
            }
            if (input == "no" || input == "n") {
                cout << "Did not save the memory database to disk. \u25A0" << endl;
                sqlite3_close_v2(disk_db);
                break;
            }
            std::cerr << "Unknown command. \u25A1" << std::endl;
        }
    }

    void insert_key_code() {

        // BEGIN TRANSACTION
        begin_transaction(memory_db);

        const auto* sql = "INSERT INTO key_codes(key_value, key_name) VALUES(?, ?);";
        sqlite3_stmt* stmt = prepare_stmt(memory_db, sql); if (stmt == nullptr) return;

        key_patterns::KeyCodeManager& key_code_manager = key_patterns::KeyCodeManager::getInstance();
        const auto& key_code_map = key_code_manager.get_keyCode_name_map();

        for (const auto& [key, value] : key_code_map) {
            // Bind the key_value parameter
            sql_bind_int(memory_db, stmt, 1, static_cast<int>(key));
            // Bind the key_name parameter
            sql_bind_text(memory_db, stmt, 2, value.c_str());
            // Execute the statement
            step_stmt(memory_db, stmt);
            // Reset the statement for the next iteration
            reset_stmt(memory_db, stmt);
        }
        // Finalize the statement
        sqlite3_finalize(stmt);
        if (debug_sql)
            cout << "Inserted key codes successfully \u25A0" << endl << endl;

        if (debug_sql)
            cout << "Result of the table key_code:" << endl;
        // Execute SELECT * FROM key_code;
        sql = "SELECT * FROM key_codes;";
        stmt = prepare_stmt(memory_db, sql); if (stmt == nullptr) return;
        cout << "pk | key_value | key_name" << endl;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            cout << sqlite3_column_int(stmt, 0)<< " | " << sqlite3_column_int(stmt, 1) << " | "
            << sqlite3_column_text(stmt, 2) << endl;
        }
        // Finalize the statement
        sqlite3_finalize(stmt);

        // Ask user to commit or rollback
        commit_or_rollback(memory_db);
        // Close the database
        sqlite3_close_v2(memory_db);
    }

    void insert_key_patterns() {

    }
}