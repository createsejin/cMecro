//
// Created by creat on 2024-02-07.
//
#ifndef SQL_EXECUTIVE_H
#define SQL_EXECUTIVE_H
#include "../lib/sqlite3.h"
#include <fstream>
#include <vector>
#include <memory>

namespace sql_executive
{
    class SQLManager {
        inline static bool debug1 {true};
        inline static const char* database_path {"./mecro_data.db"};
        std::ifstream DBfile;
        std::streampos s_size;
        std::vector<char> buffer;
        sqlite3* memoryDB; // dynamic allocation
        sqlite3* diskDB;
        void check_and_close_memoryDB() const;
        auto get_buffer() -> std::vector<char>;
        auto open_memoryDB() -> sqlite3*;
        static auto open_diskDB(int flag) -> sqlite3*;
        void save_to_diskDB_from_memoryDB(bool asking = false);
    public:
        SQLManager();
        ~SQLManager();

        void insert_key_code();
        void testdb003() const;
    };
    extern std::unique_ptr<SQLManager> sql_manager;
}
#endif //SQL_EXECUTIVE_H
