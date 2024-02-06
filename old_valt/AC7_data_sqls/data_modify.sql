-- before modify
PRAGMA foreign_keys = OFF;
DROP VIEW parts_view_classified;
DROP VIEW parts_using;
DROP VIEW parts_final_data;
DROP VIEW same_parameterLabels;
DROP VIEW final_union_parts;

CREATE TABLE "temp_table01" (
	pk	INTEGER,
	purpose	TEXT,
	ParameterLabel	TEXT UNIQUE,
	OperationType	TEXT,
	calculed_value	REAL,
	union_partsID	INTEGER,
	union_parts_name TEXT,
	time_stamp TIMESTAMP,	
	PRIMARY KEY("pk" AUTOINCREMENT)
);
INSERT INTO temp_table01 (purpose, ParameterLabel, OperationType, calculed_value, union_partsID, union_parts_name)
SELECT purpose, ParameterLabel, OperationType, calculed_value, union_partsID, union_parts_name FROM parameterLabels;
DROP TABLE parameterLabels;
ALTER TABLE temp_table01 RENAME TO parameterLabels;

-- modify table : Database Structure에서 긁어올것.
-- CREATE TABLE "temp_table01" (
-- 	"pk"	INTEGER,
-- 	"purpose"	TEXT,
-- 	"ParameterLabel"	TEXT UNIQUE,
-- 	"OperationType"	TEXT,
-- 	"calculed_value"	REAL,
-- 	"union_partsID"	INTEGER,
-- 	"union_parts_name"	TEXT,
-- 	PRIMARY KEY("pk" AUTOINCREMENT)
-- );
-- INSERT INTO temp_table01 (purpose, ParameterLabel, OperationType, calculed_value, union_partsID, union_parts_name)
-- SELECT purpose, ParameterLabel, OperationType, calculed_value, union_partsID, union_parts_name FROM parameterLabels;
-- DROP TABLE parameterLabels;
-- ALTER TABLE temp_table01 RENAME TO parameterLabels;

-- after modify
PRAGMA foreign_keys = ON;

-- CREATE VIEW
CREATE VIEW parts_view_classified AS
SELECT A.pk, A.partsID, A.parts_name, A.category, A.capability, B.level, A.purpose, B.choice
FROM parts A
JOIN parts_level B
ON A.pk = B.parts_pk
ORDER BY A.purpose, A.partsID, B.level;

CREATE VIEW parts_using AS
SELECT pk, partsID, parts_name, category, capability, purpose
FROM parts_view_classified
WHERE choice = 1;

CREATE VIEW parts_final_data AS
-- parts_final_data view SELECT SQL + row_number for purpose
SELECT A.partsID, A.parts_name, B.category, B.capability, B.purpose, 
A.slot, 
row_number() OVER (PARTITION BY B.purpose ORDER BY A.partsID) - 1 AS slot_num,
A.capability as actual_capa, A.ParameterLabel, substr(A.OperationType, 26) AS OperationType, ParameterValue,
C.union_partsID, C.union_parts_name
FROM parts_capability A
JOIN parts_using B
ON A.partsID = B.partsID
JOIN merged_parts C
ON A.partsID = C.merged_partsID
ORDER BY B.purpose, A.partsID;

-- same ParameterLabel들의 values들을 곱하거나 더하기 위한 VIEW
CREATE VIEW same_parameterLabels AS
SELECT row_number() OVER () AS row, partsID, parts_name, purpose,
-- 같은 purpose를 가진 리스트 중에서 ParameterLabel이 겹치는 항목에 파티션된 row_number를 부여
row_number() OVER (PARTITION BY purpose, ParameterLabel ORDER BY partsID) - 1 AS same_para,
	-- 파티션된 ParameterLabel 그룹의 count가 2를 넘으면 1(true)를 반환, 아니면 0(false)
	CASE
		WHEN count(*) OVER (PARTITION BY ParameterLabel) >= 2 THEN 1
		ELSE 0
	END AS is_same,
ParameterLabel, OperationType, 
ParameterValue,
union_partsID, union_parts_name
FROM parts_final_data;

CREATE VIEW final_union_parts AS
SELECT pk, purpose, 
	row_number() OVER (PARTITION BY purpose ORDER BY pk) - 1 AS slot,
	ParameterLabel, OperationType, calculed_value, union_partsID, union_parts_name
FROM parameterLabels;

-- FOREIGN KEY CHECK
PRAGMA foreign_key_check;
VACUUM;
PRAGMA optimize;