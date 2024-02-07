PRAGMA foreign_keys = OFF;

CREATE TABLE IF NOT EXISTS temp_table (
    pk INTEGER,
    action_index INTEGER NOT NULL UNIQUE,
    action_name TEXT NOT NULL UNIQUE,
    PRIMARY KEY (pk AUTOINCREMENT)
);

INSERT INTO temp_table (action_index, action_name)
SELECT action_index, action_name FROM single_actions;
DROP TABLE single_actions;
ALTER TABLE temp_table RENAME TO single_actions;

PRAGMA foreign_keys = ON;
PRAGMA foreign_key_check;