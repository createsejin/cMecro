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
        bool debug1;
        const char* database_path;
        std::ifstream DBfile;
        std::streampos s_size;
        std::vector<char> buffer;
        sqlite3* memoryDB;

    public:
        SQLManager();
        ~SQLManager();
        void check_and_close_memoryDB() const;
        auto get_buffer() -> std::vector<char>;
        auto open_memoryDB() -> sqlite3*;

        void testdb003() const;
    };
    extern std::unique_ptr<SQLManager> sql_manager;

    //void insert_key_code();
    //void testdb000();
    //void testdb001();
    //void testdb002();
}
#endif //SQL_EXECUTIVE_H
