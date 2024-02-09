//
// Created by creat on 2024-02-07.
//

#ifndef SQL_EXECUTIVE_H
#define SQL_EXECUTIVE_H
#include "../lib/sqlite3.h"
#include <fstream>
#include <vector>

namespace sql_executive
{
    class SQLManager {
        bool debug1;
        const char* database_path;
        std::ifstream DBfile;
        std::streampos s_size;
        std::vector<char> buffer;
        sqlite3* memoryDB;
        SQLManager();
        ~SQLManager();
    public:
        static SQLManager& getInstance();
        SQLManager(const SQLManager&) = delete;
        SQLManager& operator=(const SQLManager&) = delete;
        SQLManager(SQLManager&&) = delete;
        SQLManager& operator=(SQLManager&&) = delete;

        void check_and_close_memoryDB() const;
        auto get_buffer() -> std::vector<char>;
        auto open_memoryDB() -> sqlite3*;

        void testdb003() const;
    };
    //void insert_key_code();
    void testdb000();
    void testdb001();
    void testdb002();
}
#endif //SQL_EXECUTIVE_H
