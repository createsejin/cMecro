PRAGMA foreign_keys = OFF;

CREATE TABLE IF NOT EXISTS temp_table(
        pk INTEGER,
        menu_id INTEGER NOT NULL UNIQUE,
        menu_name TEXT NOT NULL,
        comment TEXT,
        PRIMARY KEY (pk AUTOINCREMENT)
);

INSERT INTO temp_table (menu_id, menu_name)
SELECT menu_id, menu_name FROM menus;

DROP TABLE menus;
ALTER TABLE temp_table RENAME TO menus;

PRAGMA foreign_keys = ON;
PRAGMA foreign_key_check;