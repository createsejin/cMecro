//
// Created by creat on 2024-02-07.
//
#include "sql_executive.h"
#include "key_patterns.h"

#include <iostream>

using namespace std;

namespace sql_executive
{
    bool debug_sql{true};
    const char* mecro_data_path = "./mecro_data.db";

    static auto openDB_as_memoryDB(const char* file_path) -> sqlite3* {
        ifstream file(file_path, ios::binary | ios::ate);
        const streamsize size {file.tellg()};
        file.seekg(0, ios::beg);

        vector<char> buffer(size);
        if (!file.read(buffer.data(), size)) {
            cerr << "Falied to read database file" << endl;
            return nullptr;
        }
        sqlite3* memdb;
        auto rc = sqlite3_open_v2(":memory:", &memdb,
       SQLITE_OPEN_READWRITE, nullptr);
        if (rc != SQLITE_OK) {
            cerr << "Failed to open SQLite memory database. \u25A1" << endl;
            return nullptr;
       }
        // 3. sqlite3_deserialize 함수를 사용하여 이 메모리 버퍼를 SQLite 데이터베이스로 변환한다.
        rc = sqlite3_deserialize(memdb, "main", reinterpret_cast<unsigned char*>(buffer.data()),
            size, static_cast<sqlite3_int64>(buffer.size()),
            SQLITE_DESERIALIZE_RESIZEABLE);
        if (rc != SQLITE_OK) {
            cerr << "Failed to deserialize the memory database. \u25A1" << endl;
            sqlite3_close_v2(memdb);
            return nullptr;
        }
        cout << "Opened memory database successfully. \u25A0" << endl;
        return memdb;
    }

    SQLManager::SQLManager() {
        get_buffer();
        open_memoryDB();
    }

    SQLManager::~SQLManager() {
        check_and_close_memoryDB();
    }

    SQLManager& SQLManager::getInstance() {
        static SQLManager instance;
        return instance;
    }

    void SQLManager::check_and_close_memoryDB() const {
        if (memoryDB != nullptr) {
            sqlite3_close_v2(memoryDB);
            if (debug_sql)
                cout << "Closed memory database \u25A0" << endl;
        } else cerr << "Memory DB not opened \u25A1" << endl;
    }

    void SQLManager::get_buffer() {
        ifstream file(database_path, ios::binary | ios::ate);
        const auto size = file.tellg();
        s_size = size;
        file.seekg(0, ios::beg);

        vector<char> buffer(size);
        if (!file.read(buffer.data(), size)) {
            cerr << "Falied to read database file" << endl;
            return;
        }
        //DBfile = std::move(file);
        this->buffer = std::move(buffer);
    }

    void SQLManager::open_memoryDB() {
        sqlite3* db;
        auto rc = sqlite3_open_v2(":memory:", &db,
       SQLITE_OPEN_READWRITE, nullptr);
        if (rc != SQLITE_OK) {
            cerr << "Failed to open SQLite memory database. \u25A1" << endl;
            return;
        }
        // 3. sqlite3_deserialize 함수를 사용하여 이 메모리 버퍼를 SQLite 데이터베이스로 변환한다.
        rc = sqlite3_deserialize(db, "main", reinterpret_cast<unsigned char*>(buffer.data()),
            s_size, static_cast<sqlite3_int64>(buffer.size()),
            SQLITE_DESERIALIZE_RESIZEABLE);
        if (rc != SQLITE_OK) {
            cerr << "Failed to deserialize the memory database. \u25A1" << endl;
            sqlite3_close_v2(db);
            return;
        }
        if (debug1)
            cout << "Opened memory database successfully. \u25A0" << endl;
    }

    auto open_database(const char* file_path, const int flag) -> sqlite3* {
        sqlite3* db;
        const auto rc = sqlite3_open_v2(file_path, &db,
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
    auto get_input() -> string {
        string input;
        std::getline(std::cin, input);
        const auto pos = input.find("sql>");
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

    void save_to_diskDB_from_memoryDB(const char* file_path, sqlite3* memory_db) {
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
                cout << "Saved the memory database to disk. \u25A0" << endl;
                sqlite3_close_v2(disk_db);
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

    // void insert_key_code() {
    //     // BEGIN TRANSACTION
    //     begin_transaction(memory_db);
    //
    //     const auto* sql = "INSERT INTO key_codes(key_value, key_name) VALUES(?, ?);";
    //     sqlite3_stmt* stmt = prepare_stmt(memory_db, sql); if (stmt == nullptr) return;
    //
    //     key_patterns::KeyCodeManager& key_code_manager = key_patterns::KeyCodeManager::getInstance();
    //     const auto& key_code_map = key_code_manager.get_keyCode_name_map();
    //
    //     for (const auto& [key, value] : key_code_map) {
    //         // Bind the key_value parameter
    //         sql_bind_int(memory_db, stmt, 1, static_cast<int>(key));
    //         // Bind the key_name parameter
    //         sql_bind_text(memory_db, stmt, 2, value.c_str());
    //         // Execute the statement
    //         step_stmt(memory_db, stmt);
    //         // Reset the statement for the next iteration
    //         reset_stmt(memory_db, stmt);
    //     }
    //     // Finalize the statement
    //     sqlite3_finalize(stmt);
    //     if (debug_sql)
    //         cout << "Inserted key codes successfully \u25A0" << endl << endl;
    //
    //     if (debug_sql)
    //         cout << "Result of the table key_code:" << endl;
    //     // Execute SELECT * FROM key_code;
    //     sql = "SELECT * FROM key_codes;";
    //     stmt = prepare_stmt(memory_db, sql); if (stmt == nullptr) return;
    //     cout << "pk | key_value | key_name" << endl;
    //     while (sqlite3_step(stmt) == SQLITE_ROW) {
    //         cout << sqlite3_column_int(stmt, 0)<< " | " << sqlite3_column_int(stmt, 1) << " | "
    //         << sqlite3_column_text(stmt, 2) << endl;
    //     }
    //     // Finalize the statement
    //     sqlite3_finalize(stmt);
    //
    //     // Ask user to commit or rollback
    //     commit_or_rollback(memory_db);
    //     save_to_diskDB_from_memoryDB(mecro_data_path, memory_db);
    // }

    void testdb000() {
        sqlite3* memoryDB = openDB_as_memoryDB(mecro_data_path);
        const auto* sql = "SELECT * FROM single_actions;";
        sqlite3_stmt* stmt = prepare_stmt(memoryDB, sql);
        cout << "pk | key_value | key_name" << endl;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            cout << sqlite3_column_int(stmt, 0)<< " | " << sqlite3_column_int(stmt, 1) << " | "
            << sqlite3_column_text(stmt, 2) << endl;
        }
        sqlite3_finalize(stmt);
        sqlite3_close_v2(memoryDB);
        cout << "Closed memory database." << endl;
    }

    using Row = std::tuple<int, int, std::string>;
    int test_call(void* data, const int argc, char* argv[], char* azColName[]) {
        auto* rows = static_cast<std::vector<Row>*>(data);
        rows->emplace_back(std::stoi(argv[0]), std::stoi(argv[1]), argv[2]);
        return 0;
    }

    void testdb001() {
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
    }

    void testdb002() {
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
}