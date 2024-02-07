//
// Created by creat on 2024-02-07.
//

#include "sql_executive.h"
#include "../lib/sqlite3.h"
#include <iostream>
sqlite3* mecro_data;

int open_mecro_data() {
    if (sqlite3_open_v2("./mecro_data.db", &mecro_data,
        SQLITE_OPEN_READWRITE, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to open mecro_data.db database \u25A1" << std::endl;
        sqlite3_close_v2(mecro_data);
        return 1;
    }
    return 0;
}
void close_mecro_data() {
    sqlite3_close_v2(mecro_data);
}