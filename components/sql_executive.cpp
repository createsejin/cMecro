//
// Created by creat on 2024-02-07.
//
#include "sql_executive.h"
#include "commander.h"
#include "key_patterns.h"

using namespace std;

namespace sql_executive
{
    bool debug_sql{true};
    const char* mecro_data_path = "./mecro_data.db";

    SQLManager::SQLManager()
    : DBfile(database_path, ios::binary | ios::ate),
        s_size(DBfile.tellg()), buffer(get_buffer()), memoryDB(open_memoryDB()), diskDB(nullptr)
    {
        if (memoryDB == nullptr) {
            cerr << "Failed to open memory database \u25A1" << endl;
        }
    }

    SQLManager::~SQLManager() {
        check_and_close_memoryDB();
    }

    void SQLManager::check_and_close_memoryDB() const {
        if (memoryDB != nullptr) {
            sqlite3_close_v2(memoryDB);
            if (debug_sql)
                cout << "Closed memory database \u25A0" << endl;
        } else cerr << "Memory DB not opened \u25A1" << endl;
    }

    auto SQLManager::get_buffer() -> std::vector<char> {
        std::vector<char> buffer(s_size);
        DBfile.seekg(0, ios::beg);
        if (!DBfile.read(buffer.data(), s_size)) {
            cerr << "Falied to read database file" << endl;
        }
        return buffer;
    }

    auto SQLManager::open_memoryDB() -> sqlite3* {
        sqlite3* db;
        auto rc = sqlite3_open_v2(":memory:", &db,
       SQLITE_OPEN_READWRITE, nullptr);
        if (rc != SQLITE_OK) {
            cerr << "Failed to open SQLite memory database. \u25A1" << endl;
            return nullptr;
        }
        // 3. sqlite3_deserialize 함수를 사용하여 이 메모리 버퍼를 SQLite 데이터베이스로 변환한다.
        rc = sqlite3_deserialize(db, "main", reinterpret_cast<unsigned char*>(buffer.data()),
            s_size, static_cast<sqlite3_int64>(buffer.size()),
            SQLITE_DESERIALIZE_RESIZEABLE);
        if (rc != SQLITE_OK) {
            cerr << "Failed to deserialize the memory database. \u25A1" << endl;
            sqlite3_close_v2(db);
            return nullptr;
        }
        if (debug1)
            cout << "Opened memory database successfully. \u25A0" << endl;
        return db;
    }

    // ReSharper disable once CppDFAConstantParameter
    auto SQLManager::open_diskDB(const int flag) -> sqlite3* {
        sqlite3* db;
        const auto rc = sqlite3_open_v2(database_path, &db,
            flag, nullptr);
        if (rc != SQLITE_OK) {
            std::cerr << "Failed to open mecro_data.db database \u25A1" << std::endl;
            sqlite3_close_v2(db);
            return nullptr;
        }
        if (debug_sql)
            cout << "Opened " << database_path << " database successfully \u25A0" << endl;
        return db;
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

    // ReSharper disable once CppDFAConstantParameter
    static auto prepare_stmt(sqlite3* db, const char* sql) -> sqlite3_stmt* {
        sqlite3_stmt* stmt;
        const auto rc = sqlite3_prepare_v3(db, sql, -1, SQLITE_PREPARE_PERSISTENT,
            &stmt, nullptr);
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
            const string prompt{"sql> "};
            cout << "Do you want to commit or rollback? (commit/rollback)" << endl;
            cout << prompt;
            const auto args {std::move(commander::get_args_from_input(prompt))};
            if (args.empty()) {
                cout << "input command." << endl;
                continue;
            }
            const string_view input {args[0]};
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

    void SQLManager::save_to_diskDB_from_memoryDB(const bool asking) {
        diskDB = open_diskDB(SQLITE_OPEN_READWRITE);
        if (diskDB == nullptr || memoryDB == nullptr) return;
        if (!asking) {
            // ReSharper disable once CppDFAUnreachableCode
            auto* backup = sqlite3_backup_init(diskDB, "main",
                                               memoryDB, "main");
            if (backup) {
                sqlite3_backup_step(backup, -1);
                sqlite3_backup_finish(backup);
                cout << "Saved the memory database to disk. \u25A0" << endl;
            } else {
                cerr << "Failed to backup the memory database to disk. \u25A1" << endl;
            }
            sqlite3_close_v2(diskDB);
            return;
        }
        while (true) {
            const string prompt{"sql> "};
            cout << "Do you want to save the memory database to disk? (yes/no)" << endl;
            cout << prompt;
            const auto args {std::move(commander::get_args_from_input(prompt))};
            if (args.empty()) {
                cout << "input command." << endl;
                continue;
            }
            const string_view input {args[0]};
            if (input == "yes" || input == "y") {
                auto* backup = sqlite3_backup_init(diskDB, "main",
                    memoryDB, "main");
                if (backup) {
                    sqlite3_backup_step(backup, -1);
                    sqlite3_backup_finish(backup);
                } else {
                    cerr << "Failed to backup the memory database to disk. \u25A1" << endl;
                    sqlite3_close_v2(diskDB);
                    break;
                }
                sqlite3_close_v2(diskDB);
                cout << "Saved the memory database to disk. \u25A0" << endl;
                break;
            }
            if (input == "no" || input == "n") {
                sqlite3_close_v2(diskDB);
                cout << "Did not save the memory database to disk. \u25A0" << endl;
                break;
            }
            std::cerr << "Unknown command. \u25A1" << std::endl;
        }
    }

    /*void insert_key_code() {
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
        save_to_diskDB_from_memoryDB(mecro_data_path, memory_db);
    }*/

    using Row = std::tuple<int, int, std::string>;
    int test_call(void* data, const int argc, char* argv[], char* azColName[]) {
        auto* rows = static_cast<std::vector<Row>*>(data);
        rows->emplace_back(std::stoi(argv[0]), std::stoi(argv[1]), argv[2]);
        return 0;
    }

    /*void testdb001() {
        ifstream file(mecro_data_path, ios::binary | ios::ate);
        const streamsize size {file.tellg()};
        file.seekg(0, ios::beg);

        vector<char> buffer(size);
        if (!file.read(buffer.data(), size)) {
            cerr << "Falied to read database file" << endl;
            return;
        }
        sqlite3* memdb;
        auto rc = sqlite3_open_v2(":memory:", &memdb,
       SQLITE_OPEN_READWRITE, nullptr);
        if (rc != SQLITE_OK) {
            cerr << "Failed to open SQLite memory database. \u25A1" << endl;
            return;
        }
        // 3. sqlite3_deserialize 함수를 사용하여 이 메모리 버퍼를 SQLite 데이터베이스로 변환한다.
        rc = sqlite3_deserialize(memdb, "main", reinterpret_cast<unsigned char*>(buffer.data()),
            size, static_cast<sqlite3_int64>(buffer.size()),
            SQLITE_DESERIALIZE_RESIZEABLE);
        if (rc != SQLITE_OK) {
            cerr << "Failed to deserialize the memory database. \u25A1" << endl;
            sqlite3_close_v2(memdb);
            return;
        }
        cout << "Opened memory database successfully. \u25A0" << endl;
        char* errmsg;
        const auto* sql = "SELECT * FROM single_actions;";
        std::vector<Row> rows;
        rc = sqlite3_exec(memdb, sql, test_call, &rows, &errmsg);
        if (rc != SQLITE_OK) {
            std::cerr << "SQL error: " << errmsg << " \u25A1" << std::endl;
            sqlite3_free(errmsg);
            sqlite3_close_v2(memdb);
            return;
        }
        std::cout << "Operation done successfully. \u25A0" << std::endl;
        cout << "pk | key_value | key_name" << endl;
        for (const auto& [one, two, three] : rows) {
            cout << one << " | " << two << " | " << three << endl;
        }
        sqlite3_close_v2(memdb);
    }*/

    /*void testdb002() {
        ifstream file(mecro_data_path, ios::binary | ios::ate);
        const streamsize size {file.tellg()};
        file.seekg(0, ios::beg);

        vector<char> buffer(size);
        if (!file.read(buffer.data(), size)) {
            cerr << "Falied to read database file" << endl;
            return;
        }
        sqlite3* memdb;
        auto rc = sqlite3_open_v2(":memory:", &memdb,
       SQLITE_OPEN_READWRITE, nullptr);
        if (rc != SQLITE_OK) {
            cerr << "Failed to open SQLite memory database. \u25A1" << endl;
            return;
        }
        // 3. sqlite3_deserialize 함수를 사용하여 이 메모리 버퍼를 SQLite 데이터베이스로 변환한다.
        rc = sqlite3_deserialize(memdb, "main", reinterpret_cast<unsigned char*>(buffer.data()),
            size, static_cast<sqlite3_int64>(buffer.size()),
            SQLITE_DESERIALIZE_RESIZEABLE);
        if (rc != SQLITE_OK) {
            cerr << "Failed to deserialize the memory database. \u25A1" << endl;
            sqlite3_close_v2(memdb);
            return;
        }
        cout << "Opened memory database successfully. \u25A0" << endl;

        const auto* sql = "SELECT * FROM single_actions;";
        sqlite3_stmt* stmt = prepare_stmt(memdb, sql);

        cout << "pk | key_value | key_name" << endl;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            cout << sqlite3_column_int(stmt, 0)<< " | " << sqlite3_column_int(stmt, 1) << " | "
            << sqlite3_column_text(stmt, 2) << endl;
        }
        // Finalize the statement
        sqlite3_finalize(stmt);
        sqlite3_close_v2(memdb);
    }*/

    void SQLManager::insert_key_code() {
        begin_transaction(memoryDB);

        const auto* sql = "INSERT INTO key_codes(key_value, key_name) VALUES(?, ?);";
        sqlite3_stmt* stmt = prepare_stmt(memoryDB, sql);

        key_patterns::KeyCodeManager& key_code_manager = key_patterns::KeyCodeManager::getInstance();
        const auto& key_code_map = key_code_manager.get_keyCode_name_map();

        for (const auto& [key, value] : key_code_map) {
            // Bind the key_value parameter
            sql_bind_int(memoryDB, stmt, 1, static_cast<int>(key));
            // Bind the key_name parameter
            sql_bind_text(memoryDB, stmt, 2, value.c_str());
            // Execute the statement
            step_stmt(memoryDB, stmt);
            // Reset the statement for the next iteration
            reset_stmt(memoryDB, stmt);
        }
        sqlite3_finalize(stmt);
        if (debug1)
            cout << "Inserted key codes successfully \u25A0" << endl << endl;

        if (debug1)
            cout << "Result of the table key_code: " << endl;
        // Execute SELECT * FROM key_code;
        sql = "SELECT * FROM key_codes;";
        stmt = prepare_stmt(memoryDB, sql); if (stmt == nullptr) return;
        cout << "pk | key_value | key_name" << endl;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            cout << sqlite3_column_int(stmt, 0)<< " | " << sqlite3_column_int(stmt, 1) << " | "
            << sqlite3_column_text(stmt, 2) << endl;
        }
        // Finalize the statement
        sqlite3_finalize(stmt);
        commit_or_rollback(memoryDB);
        save_to_diskDB_from_memoryDB(true);
    }

    void SQLManager::testdb003() const {
        const auto* sql = "SELECT * FROM single_actions;";
        sqlite3_stmt* stmt = prepare_stmt(memoryDB, sql);

        cout << "pk | key_value | key_name" << endl;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            cout << sqlite3_column_int(stmt, 0)<< " | " << sqlite3_column_int(stmt, 1) << " | "
            << sqlite3_column_text(stmt, 2) << endl;
        }
        // Finalize the statement
        sqlite3_finalize(stmt);
    }

    std::unique_ptr<SQLManager> sql_manager = std::make_unique<SQLManager>();
}