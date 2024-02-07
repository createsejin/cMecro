CREATE TABLE IF NOT EXISTS temp_table (
    single_actions_pk INTEGER,
    action_index INTEGER NOT NULL UNIQUE,
    action_name TEXT NOT NULL UNIQUE,
    PRIMARY KEY (single_actions_pk AUTOINCREMENT)
);

CREATE TABLE IF NOT EXISTS key_code(
    pk INTEGER,
    key_value INTEGER NOT NULL UNIQUE,
    key_name TEXT NOT NULL UNIQUE,
    PRIMARY KEY (pk AUTOINCREMENT)
);