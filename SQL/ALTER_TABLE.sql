PRAGMA foreign_keys = OFF;

CREATE TABLE IF NOT EXISTS temp_table(
   pk INTEGER,
   pattern_id INTEGER NOT NULL UNIQUE,
   pattern_name TEXT NOT NULL UNIQUE,
   blocking INTEGER NOT NULL DEFAULT 0,
   pattern_type TEXT NOT NULL DEFAULT 'NORMAL',
   PRIMARY KEY (pk AUTOINCREMENT)
);

-- INSERT INTO temp_table (key_value, key_name)
-- SELECT key_value, key_name FROM key_codes;

DROP TABLE key_patterns;
ALTER TABLE temp_table RENAME TO key_patterns;

PRAGMA foreign_keys = ON;
PRAGMA foreign_key_check;