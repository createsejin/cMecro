CREATE TABLE IF NOT EXISTS single_actions (
    single_actions_pk INTEGER,
    action_index INTEGER NOT NULL UNIQUE,
    action_name TEXT NOT NULL UNIQUE,
    PRIMARY KEY (single_actions_pk AUTOINCREMENT)
);

CREATE TABLE IF NOT EXISTS key_codes(
    pk INTEGER,
    key_value INTEGER NOT NULL UNIQUE,
    key_name TEXT NOT NULL UNIQUE,
    PRIMARY KEY (pk AUTOINCREMENT)
);

CREATE TABLE IF NOT EXISTS key_patterns(
    pk INTEGER,
    pattern_id INTEGER NOT NULL UNIQUE,
    pattern_name TEXT NOT NULL UNIQUE,
    blocking INTEGER NOT NULL DEFAULT 0,
    pattern_type TEXT NOT NULL DEFAULT 'NORMAL',
    PRIMARY KEY (pk AUTOINCREMENT)
);

CREATE TABLE IF NOT EXISTS combine_keys(
    pk INTEGER,
    pattern_pk INTEGER NOT NULL,
    slot INTEGER NOT NULL DEFAULT 1,
    key_code_pk INTEGER NOT NULL,
    PRIMARY KEY (pk AUTOINCREMENT),
    FOREIGN KEY (pattern_pk) REFERENCES key_pattern(pk),
    FOREIGN KEY (key_code_pk) REFERENCES key_code(pk)
);

CREATE TABLE IF NOT EXISTS menus(
    pk INTEGER,
    menu_id INTEGER NOT NULL UNIQUE AUTOINCREMENT,
    menu_name TEXT NOT NULL,
    PRIMARY KEY (pk AUTOINCREMENT)
);

CREATE TABLE IF NOT EXISTS menu_nodes(
    pk INTEGER,
    parent_menu_pk INTEGER NOT NULL,
    child_menu_pk INTEGER NOT NULL,
    menu_order INTEGER NOT NULL DEFAULT 0,
    PRIMARY KEY (pk AUTOINCREMENT),
    FOREIGN KEY (parent_menu_pk) REFERENCES menus(pk),
    FOREIGN KEY (child_menu_pk) REFERENCES menus(pk)
);