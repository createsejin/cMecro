CREATE TABLE IF NOT EXISTS "missions" (
	"PID"	INTEGER,
	"mission_number"	INTEGER NOT NULL UNIQUE,
	"mission_name"	TEXT NOT NULL UNIQUE,
	"operation_name"	TEXT,
	"operation_objective"	TEXT,
	"S_rank_time"	TEXT,
	"S_rank_score"	INTEGER,
	"appropriate_airframe_ID"	INTEGER,
	"mission_info_dir"	TEXT,
	PRIMARY KEY("PID" AUTOINCREMENT)
);
CREATE TABLE IF NOT EXISTS "parts_level" (
	"pk"	INTEGER,
	"parts_pk"	INTEGER,
	"partsID"	INTEGER,
	"prifix_name"	TEXT,
	"level"	TEXT,
	"level_v"	INTEGER,
	"choice"	INTEGER DEFAULT 0,
	PRIMARY KEY("pk" AUTOINCREMENT),
	FOREIGN KEY("parts_pk") REFERENCES "parts"("pk")
);
CREATE TABLE IF NOT EXISTS "merged_parts" (
	"pk"	INTEGER,
	"parts_pk"	INTEGER,
	"merged_partsID"	INTEGER,
	"merged_part_name"	TEXT,
	"category"	TEXT,
	"merged_capability"	TEXT,
	"purpose"	TEXT,
	"union_partsID"	INTEGER,
	"union_parts_name"	INTEGER,
	PRIMARY KEY("pk" AUTOINCREMENT),
	FOREIGN KEY("parts_pk") REFERENCES "parts"("pk")
);
CREATE TABLE IF NOT EXISTS "parts" (
	"pk"	INTEGER,
	"partsID"	INTEGER UNIQUE,
	"parts_name"	TEXT,
	"category"	TEXT,
	"capability"	TEXT,
	"purpose"	TEXT,
	PRIMARY KEY("pk" AUTOINCREMENT)
);
CREATE TABLE IF NOT EXISTS "parts_capa_temp" (
	"pk"	INTEGER,
	"partsID"	INTEGER,
	"slot"	INTEGER,
	"parameter"	TEXT,
	"type"	TEXT,
	"value"	REAL,
	PRIMARY KEY("pk" AUTOINCREMENT)
);
CREATE TABLE IF NOT EXISTS "exception_choice" (
	"pk"	INTEGER,
	"parts_pk"	INTEGER,
	"level_pk"	INTEGER,
	"partsID"	INTEGER UNIQUE,
	"parts_name"	TEXT,
	"choice"	INTEGER DEFAULT 0,
	FOREIGN KEY("level_pk") REFERENCES "parts_level"("pk"),
	FOREIGN KEY("parts_pk") REFERENCES "parts"("pk"),
	PRIMARY KEY("pk" AUTOINCREMENT)
);
CREATE TABLE IF NOT EXISTS "parts_capability" (
	"pk"	INTEGER,
	"parts_pk"	INTEGER,
	"partsID"	INTEGER,
	"parts_name"	TEXT,
	"slot"	INTEGER,
	"capability"	TEXT,
	"ParameterLabel"	TEXT,
	"OperationType"	TEXT,
	"ParameterValue"	BLOB,
	PRIMARY KEY("pk"),
	FOREIGN KEY("parts_pk") REFERENCES "parts"("pk")
);
CREATE TABLE IF NOT EXISTS "parameter_values" (
	"pk"	INTEGER,
	"ParameterLabel_pk"	INTEGER,
	"is_same"	INTEGER,
	"ParameterLabel"	TEXT,
	"OperationType"	TEXT,
	"ParameterValue"	INTEGER,
	FOREIGN KEY("ParameterLabel_pk") REFERENCES "parameterLabels"("pk"),
	PRIMARY KEY("pk" AUTOINCREMENT)
);
CREATE TABLE IF NOT EXISTS "parameterLabels" (
	"pk"	INTEGER,
	"purpose" TEXT,
	"ParameterLabel"	TEXT UNIQUE,
	"OperationType"	TEXT,
	"calculed_value"	REAL,
	"union_partsID"	INTEGER,
	"union_parts_name"	TEXT,
	PRIMARY KEY("pk" AUTOINCREMENT)
);
CREATE INDEX "idx_ParameterLabels" ON "parts_capability" (
	"ParameterLabel",
	"OperationType",
	"partsID"
);
CREATE INDEX "idx_parts_main_data" ON "parts" (
	"pk"	ASC,
	"partsID"	ASC,
	"parts_name"	ASC
);
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
CREATE TRIGGER update_exception_choice
AFTER UPDATE ON exception_choice
BEGIN
	UPDATE parts_level
	SET choice = (
	SELECT A.choice
	FROM exception_choice A
	WHERE A.parts_pk = parts_level.pk
	)
	WHERE pk IN (
	SELECT level_pk
	FROM exception_choice
	);  
	-- exception_choice에 있는 view들만 업데이트 한다.
	SELECT * FROM parts_view_classified
	WHERE partsID IN (SELECT partsID FROM exception_choice);
	SELECT * FROM parts_using
	WHERE partsID IN (SELECT partsID FROM exception_choice);
	SELECT * FROM parts_final_data
	WHERE partsID IN (SELECT partsID FROM exception_choice);
	SELECT * FROM same_parameterLabels
	WHERE partsID IN (SELECT partsID FROM exception_choice);

	-- parameterLabels modify
	DELETE FROM parameterLabels;	
	INSERT INTO parameterLabels (purpose, ParameterLabel, OperationType, calculed_value, union_partsID, union_parts_name)
	WITH log_values AS (
		SELECT purpose, ParameterLabel, OperationType, LOG(ParameterValue) as log_value,
			union_partsID, union_parts_name
		FROM (
			SELECT *
			FROM same_parameterLabels
			WHERE OperationType = "POT_RATE"
		)
	)
	SELECT purpose, ParameterLabel, OperationType, POWER(10, SUM(log_value)) as calculed_value,
		union_partsID, union_parts_name
	FROM log_values
	GROUP BY ParameterLabel
	UNION
	SELECT purpose, ParameterLabel, OperationType, SUM(ParameterValue) as calculed_value, union_partsID, union_parts_name FROM same_parameterLabels
	WHERE OperationType = "POT_OFFSET"
	GROUP BY ParameterLabel;
END;
CREATE TRIGGER insert_exception_choice
AFTER INSERT ON exception_choice
BEGIN
	UPDATE parts_level
	SET choice = (
	SELECT A.choice
	FROM exception_choice A
	WHERE A.parts_pk = parts_level.pk
	)
	WHERE pk IN (
	SELECT level_pk
	FROM exception_choice
	);
	SELECT * FROM parts_view_classified
	WHERE partsID IN (SELECT partsID FROM exception_choice);
	SELECT * FROM parts_using
	WHERE partsID IN (SELECT partsID FROM exception_choice);
	SELECT * FROM parts_final_data
	WHERE partsID IN (SELECT partsID FROM exception_choice);
	SELECT * FROM same_parameterLabels
	WHERE partsID IN (SELECT partsID FROM exception_choice);

	-- parameterLabels modify
	DELETE FROM parameterLabels;	
	INSERT INTO parameterLabels (purpose, ParameterLabel, OperationType, calculed_value, union_partsID, union_parts_name)
	WITH log_values AS (
		SELECT purpose, ParameterLabel, OperationType, LOG(ParameterValue) as log_value,
			union_partsID, union_parts_name
		FROM (
			SELECT *
			FROM same_parameterLabels
			WHERE OperationType = "POT_RATE"
		)
	)
	SELECT purpose, ParameterLabel, OperationType, POWER(10, SUM(log_value)) as calculed_value,
		union_partsID, union_parts_name
	FROM log_values
	GROUP BY ParameterLabel
	UNION
	SELECT purpose, ParameterLabel, OperationType, SUM(ParameterValue) as calculed_value, union_partsID, union_parts_name FROM same_parameterLabels
	WHERE OperationType = "POT_OFFSET"
	GROUP BY ParameterLabel;
END;
CREATE TRIGGER delete_exception_choice
AFTER DELETE ON exception_choice
BEGIN
	-- 아래 조건을 모두 만족하는 애들만 choice
	UPDATE parts_level
	SET choice = 1
	WHERE pk IN (
	-- 같은 이름을 가진 prifix_name 중에서 level_v가 가장 높은 pk만 선택하고 이름이 "None Object"인 애들 제외 
	SELECT pk
	FROM (
	SELECT pk, prifix_name, max(level_v)
	FROM parts_level
	WHERE NOT prifix_name = "None Object"
	GROUP BY prifix_name)
	UNION
	-- prifix_name 그룹에서 level_v의 Max값이 1인 prifix_name들 중에서 이름이 "None Object"인 애들 제외
	SELECT pk FROM
	(SELECT pk, prifix_name, level,
		CASE
			WHEN max(level_v) = 1 THEN 1
			ELSE 0
		END AS choice
	FROM parts_level
	GROUP BY prifix_name)
	WHERE choice = 1 AND NOT prifix_name = "None Object");
	
	UPDATE parts_level
	  SET choice = (
		SELECT A.choice
		FROM exception_choice A
		WHERE A.parts_pk = parts_level.pk
	  )
	  WHERE pk IN (
		SELECT level_pk
		FROM exception_choice
	  );
	  
	SELECT * FROM parts_view_classified;
	SELECT * FROM parts_using;
	SELECT * FROM parts_final_data;
	SELECT * FROM same_parameterLabels;
	
	-- parameterLabels modify
	DELETE FROM parameterLabels;	
	INSERT INTO parameterLabels (purpose, ParameterLabel, OperationType, calculed_value, union_partsID, union_parts_name)
	WITH log_values AS (
		SELECT purpose, ParameterLabel, OperationType, LOG(ParameterValue) as log_value,
			union_partsID, union_parts_name
		FROM (
			SELECT *
			FROM same_parameterLabels
			WHERE OperationType = "POT_RATE"
		)
	)
	SELECT purpose, ParameterLabel, OperationType, POWER(10, SUM(log_value)) as calculed_value,
		union_partsID, union_parts_name
	FROM log_values
	GROUP BY ParameterLabel
	UNION
	SELECT purpose, ParameterLabel, OperationType, SUM(ParameterValue) as calculed_value, union_partsID, union_parts_name FROM same_parameterLabels
	WHERE OperationType = "POT_OFFSET"
	GROUP BY ParameterLabel;
END;
/* No STAT tables available */
