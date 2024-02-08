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
        bool debug1 {true};
        const char* database_path {"./mecro_data.db"};
        std::ifstream DBfile;
        std::streampos s_size{};
        std::vector<char> buffer;
        sqlite3* memoryDB {nullptr};
        SQLManager();
        ~SQLManager();
    public:
        static SQLManager& getInstance();
        SQLManager(const SQLManager&) = delete;
        SQLManager& operator=(const SQLManager&) = delete;
        SQLManager(SQLManager&&) = delete;
        SQLManager& operator=(SQLManager&&) = delete;

        void check_and_close_memoryDB() const;
        void get_buffer();
        void open_memoryDB();

        void testdb003() const;
    };
    //void insert_key_code();
    void testdb000();
    void testdb001();
    void testdb002();
}
#endif //SQL_EXECUTIVE_H
