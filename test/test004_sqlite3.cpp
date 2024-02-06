//
// Created by bae on 2024-01-29.
//
#include "test004_sqlite3.h"
#include "../lib/sqlite3.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <tuple>

using namespace std;

using Row = std::tuple<std::string, int>;

static int callback(void* data, const int argc, char* argv[], char* azColName[]) {
    auto* rows = static_cast<std::vector<Row>*>(data);
    rows->emplace_back(argv[0], std::stoi(argv[1]));
    return 0;
}

int test00::test000() {
    sqlite3* db;
    sqlite3* memdb;
    char* zErrMsg = nullptr;
    int rc = sqlite3_open_v2("./test_db_000.db", &db,
        SQLITE_OPEN_READWRITE, nullptr);
    if (rc) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << " \u25A1" << std::endl;
        return rc;
    }
    // 메모리 데이터베이스를 생성합니다.
    if (sqlite3_open_v2("file::memory:?cache=shared", &memdb,
        SQLITE_OPEN_READWRITE, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to open SQLite memory database \u25A1" << std::endl;
        sqlite3_close_v2(db);
        return 1;
    }

    std::vector<Row> rows;

    const auto* sql = "SELECT * FROM test_table1;";
    rc = sqlite3_exec(db, sql, callback, &rows, &zErrMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << zErrMsg << " \u25A1" << std::endl;
        sqlite3_free(zErrMsg);
        sqlite3_close_v2(db);
        sqlite3_close_v2(memdb);
        return rc;
    }
    std::cout << "Operation done successfully \u25A0" << std::endl;
    cout << "one | two" << endl;
    for (const auto& [one, two] : rows) {
        cout << one << " | " << two << endl;
    }

    // 메모리 데이터베이스의 내용을 디스크에 있는 데이터베이스 파일로 복사합니다.
    // 그런데 이렇게 하면 메모리 db는 아무것도 없으므로 이렇게 하면 복사된 파일이 빈 파일이 되버린다.
    sqlite3_backup* backup = sqlite3_backup_init(db, "main",
        memdb, "main");
    if (backup) {
        sqlite3_backup_step(backup, -1);
        sqlite3_backup_finish(backup);
    }

    sqlite3_close_v2(db);
    sqlite3_close_v2(memdb);
    return 0;
}

int test00::test001() {
    // 1. 데이터베이스 파일을 읽어서 메모리 버퍼에 저장한다.
    ifstream file(LR"|(.\test_db_001.db)|", ios::binary | ios::ate);
    const streamsize size = file.tellg();
    file.seekg(0, ios::beg);

    vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) {
        cerr << "Falied to read database file" << endl;
        return 1;
    }

    // 2. 메모리 데이터베이스를 연다.
    sqlite3* memdb;
    if (sqlite3_open_v2(":memory:", &memdb,
        SQLITE_OPEN_READWRITE, nullptr) != SQLITE_OK) {
        cerr << "Failed to open SQLite memory database" << endl;
        return 1;
    }
    // 3. sqlite3_deserialize 함수를 사용하여 이 메모리 버퍼를 SQLite 데이터베이스로 변환한다.
    sqlite3_deserialize(memdb, "main", reinterpret_cast<unsigned char*>(buffer.data()),
        size, static_cast<sqlite3_int64>(buffer.size()),
        SQLITE_DESERIALIZE_RESIZEABLE);

    // 3. 데이터베이스에서 데이터를 읽어오는 쿼리를 실행한다.
    vector<Row> rows;
    char* zErrMsg = nullptr;
    const auto* sql = "SELECT * FROM test_table1;";
    const int rc = sqlite3_exec(memdb, sql, callback, &rows, &zErrMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << zErrMsg << endl;
        sqlite3_free(zErrMsg);
        sqlite3_close_v2(memdb);
        return rc;
    }
    // 데이터를 출력한다.
    cout << "one | two" << endl;
    for (const auto& [one, two] : rows) {
        cout << one << " | " << two << endl;
    }

    // 원본 disk db를 연다.
    sqlite3* disk_db;
    if (sqlite3_open_v2("./test_db_001.db", &disk_db,
        SQLITE_OPEN_READWRITE, nullptr) != SQLITE_OK) {
        cerr << "Failed to open SQLite disk database" << endl;
        sqlite3_close_v2(memdb);
        return 1;
    }
    // 4. 메모리 데이터베이스의 내용을 디스크에 있는 데이터베이스 파일로 복사한다.
    sqlite3_backup* backup = sqlite3_backup_init(disk_db, "main",
        memdb, "main");
    if (backup) {
        sqlite3_backup_step(backup, -1);
        sqlite3_backup_finish(backup);
    }

    // 연 db들을 close 해준다.
    sqlite3_close_v2(memdb);
    sqlite3_close_v2(disk_db);
    cout << "test end" << endl;
    return 0;
}