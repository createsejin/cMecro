-- Implement calculated values of Parameter Label values
-- 요 아래의 built-in math function들은 python의 기본 패키지로는 절대로 작동할리가 없다.
-- 그러나 트리거로 구현해놓으면 DB 내부적으로는 시스템의 sqlite3.dll을 토대로 작동하기 때문에 문제없이 실행된다.
-- 그러나 그것이 가능하려면 당연히 built-in function을 enable화 한 미리 컴파일된 dll 파일이 반드시 시스템상에 필요하다.
-- 당연히 이 파일이 여기 DB Browser 내부에 쓰이는 sqlite3.dll도 바꿔야한다. 
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

/*
The built-in math functions enable sqlite3:
1. get source code as an amalgamation of sqlite3 from the official site
https://www.sqlite.org/download.html
2. Extract it appropriate place
3. You need to also the MinGw-w64 complier for c source code of sqlite3.c
https://sourceforge.net/projects/mingw-w64/
3. Extract it to C:/
4. You need to add the environment path in the windows system. So that can call the compiler anywhere.
5. Right-click on Start, then click the 'System'
6. Go to the advanced system setting
7. Go to environmental variables
8. In system variables, select the 'Path' and click 'edit'.
9. 'create new' and add the path:
C:\mingw64\bin
and click the 'confirm' button.
9. Now, open cmd and input 'gcc', and if you got the appropriate errors, you did well.
10. Go back to the place where you've extracted the sqlite3 source code file and click the windows file explorer's file path and input 'cmd'
11. The cmd is open in your path, then input the following command:
gcc -DSQLITE_ENABLE_MATH_FUNCTIONS -shared -o sqlite3.dll sqlite3.c
11. Then you can get compiled sqlite3.dll file. Get a copy and paste it into your application or DB Browser's sqlite3.dll path, and replace it.
12. Open your DB in DB Browser, input the following query, and run:
SELECT typeof(sin(1)), typeof(cos(1)), typeof(sqrt(4));
12. If you get three real, then you succeed!
13. If you want to change dll for your windows 64-bit system, copy and paste complied sqlite3.dll file to the following directory:
C:\Windows\SysWOW64
*/
SELECT typeof(sin(1)), typeof(cos(1)), typeof(sqrt(4));

SELECT *
FROM same_parameterLabels
WHERE OperationType = "POT_RATE";

SELECT purpose, is_same, ParameterLabel, OperationType, sum(ParameterValue) as calculed_value, union_partsID, union_parts_name FROM same_parameterLabels
WHERE OperationType = "POT_OFFSET"
GROUP BY ParameterLabel;

-- validation process: pk comparison
SELECT pk
FROM parts INDEXED BY idx_parts_main_data
WHERE partsID = 168;

SELECT pk
FROM parts INDEXED BY idx_parts_main_data
WHERE parts_name = "기관포 자동";

-- 그냥 파이썬으로 구현해라. 아니면 WITH RECURSIVE로 구현하면 된다. 그걸로 반복문을 만들 수 있기 때문이다.
--> 이것을 SQL문으로 구현해버렸다! 위 query를 참조하자. 
SELECT partsID, parts_name, purpose, same_para, is_same, ParameterLabel, OperationType, 
ParameterValue, next_same_para, next_is_same, before_value, base_value, calculed_value,
union_partsID, union_parts_name FROM
(SELECT row, partsID, parts_name, purpose, same_para, is_same, ParameterLabel, OperationType, 
ParameterValue,
next_same_para, next_is_same, before_value, base_value, 
	CASE
		WHEN next_same_para > 0 AND next_is_same = 1 AND OperationType = "POT_RATE" THEN before_value * base_value
		WHEN next_same_para > 0 AND next_is_same = 1 AND OperationType = "POT_OFFSET" THEN before_value + base_value
		ELSE NULL
	END AS calculed_value,
union_partsID, union_parts_name FROM
(SELECT t.row, t.partsID, t.parts_name, t.purpose, t.same_para, t.is_same, t.ParameterLabel, 
	t.OperationType, t.ParameterValue,
	(SELECT same_para FROM same_parameterLabels WHERE row = t.row + 1) AS next_same_para,
	(SELECT is_same FROM same_parameterLabels WHERE row = t.row + 1) AS next_is_same,
	B.ParameterValue as before_value,
	CASE
		WHEN t.same_para = 0 AND t.is_same = 1 THEN t.ParameterValue
		ELSE NULL
	END AS base_value, 	
	t.union_partsID, t.union_parts_name
FROM same_parameterLabels t
-- self JOIN
LEFT JOIN ( 
	-- self subquery
	SELECT * 
	FROM same_parameterLabels) B 
ON B.row = (
	SELECT C.row
	FROM (
		-- self subquery
		SELECT *
		FROM same_parameterLabels
	) C
	WHERE C.row = t.row - 1
)));


SELECT same_para, is_same
FROM same_parameterLabels
WHERE row = 15;

SELECT parts_name
FROM parts INDEXED BY idx_parts_main_data
WHERE parts_name REGEXP "투"
ORDER BY parts_name
LIMIT 10;

SELECT parts_name
FROM parts INDEXED BY idx_parts_main_data
WHERE partsID = 10;

SELECT purpose, slot, ParameterLabel, OperationType, calculed_value, union_partsID, union_parts_name
FROM final_union_parts;

SELECT pk, purpose, 
	row_number() OVER (PARTITION BY purpose ORDER BY pk) - 1 AS slot,
	ParameterLabel, OperationType, calculed_value, union_partsID, union_parts_name
FROM parameterLabels;

-- 실제 python에서 producting을 진행하기 위한 SELECT문
SELECT purpose, same_para, is_same, ParameterLabel, OperationType, ParameterValue, union_partsID, union_parts_name
FROM same_parameterLabels;

-- same ParameterLabel들의 values들을 곱하거나 더하기 위한 SELECT
SELECT partsID, parts_name, purpose,
-- 같은 purpose를 가진 리스트 중에서 ParameterLabel이 겹치는 항목에 파티션된 row_number를 부여
row_number() OVER (PARTITION BY purpose, ParameterLabel ORDER BY partsID) - 1 AS same_para,
	-- 파티션된 ParameterLabel 그룹의 count가 2를 넘으면 1(true)를 반환, 아니면 0(false)
	CASE
		WHEN count(*) OVER (PARTITION BY ParameterLabel) >= 2 THEN 1
		ELSE 0
	END AS is_same,
ParameterLabel, OperationType, 
ParameterValue, union_partsID, union_parts_name
FROM parts_final_data;

-- union parts 들을 모두 캠페인 모드 전용으로 만들기 위한 SELECT문
SELECT union_partsID, union_parts_name
FROM merged_parts
GROUP BY union_partsID;

-- parts_final_data로부터 json 파일에 삽입하기 위한 SELECT문
SELECT purpose, slot_num, ParameterLabel, OperationType, ParameterValue, union_partsID, union_parts_name
FROM parts_final_data;

-- parts_final_data view SELECT SQL + row_number for purpose
SELECT A.partsID, A.parts_name, B.category, B.capability, B.purpose, 
A.slot, 
row_number() OVER (PARTITION BY B.purpose ORDER BY A.partsID) - 1 AS slot_num,
A.capability as actual_capa, 
-- 같은 purpose를 가진 리스트 중에서 ParameterLabel이 겹치는 항목에 파티션된 row_number를 부여
row_number() OVER (PARTITION BY B.purpose, A.ParameterLabel ORDER BY A.partsID) - 1 AS same_para,
A.ParameterLabel, substr(A.OperationType, 26) AS OperationType, ParameterValue,
C.union_partsID, C.union_parts_name
FROM parts_capability A
JOIN parts_using B
ON A.partsID = B.partsID
JOIN merged_parts C
ON A.partsID = C.merged_partsID
ORDER BY B.purpose, A.partsID;

SELECT partsID
FROM parts_final_data
GROUP BY partsID;

-- union parts의 slot 수 구하기
SELECT purpose, count(purpose), union_partsID, union_parts_name
FROM parts_final_data
GROUP BY purpose;

-- parts_final_data view SELECT SQL
SELECT A.partsID, A.parts_name, B.category, B.capability, B.purpose, 
A.slot, A.capability as actual_capa, A.ParameterLabel, substr(A.OperationType, 26), ParameterValue,
C.union_partsID, C.union_parts_name
FROM parts_capability A
JOIN parts_using B
ON A.partsID = B.partsID
JOIN merged_parts C
ON A.partsID = C.merged_partsID
ORDER BY B.purpose, A.partsID;

SELECT A.pk as parts_pk, A.partsID, A.parts_name, B.slot, B.parameter, B.type, B.value
FROM parts A
JOIN parts_capa_temp B
ON A.partsID = B.partsID;

SELECT * FROM parts_capa_temp;

-- 실제 사용될 merge될 파츠들의 partsID
SELECT pk, partsID, parts_name
FROM parts
WHERE partsID IN (
SELECT partsID FROM parts_using
ORDER BY partsID ASC);

-- 같은 이름을 가진 prifix_name 중에서 level_v가 가장 높은 pk만 선택하고 이름이 "None Object"인 애들 제외 
SELECT pk
FROM (
SELECT pk, prifix_name, max(level_v)
FROM parts_level
WHERE NOT prifix_name = "None Object"
GROUP BY prifix_name)
-- SELECT UNION
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
WHERE choice = 1 AND NOT prifix_name = "None Object";

-- 같은 이름을 가진 prifix_name 중에서 level_v가 가장 높은 pk만 선택
SELECT pk FROM
(SELECT pk, prifix_name, max(level_v)
FROM parts_level
GROUP BY prifix_name);

-- substr 후, TEXT를 CAST로 INTEGER로 convert
SELECT pk, CAST(substr(level, 4) AS INTEGER) AS level_v
FROM parts_level;

-- parts_name에서 Lv.\d 요소 제거
SELECT 
	CASE
		WHEN parts_name REGEXP "Lv.1" THEN replace(parts_name, "Lv.1", "")
		WHEN parts_name REGEXP "Lv.2" THEN replace(parts_name, "Lv.2", "")
		WHEN parts_name REGEXP "Lv.3" THEN replace(parts_name, "Lv.3", "")
		ELSE parts_name
	END
FROM parts;

-- CASE와 REGEXP를 사용하여 해당되는 열에 level result를 출력.
SELECT partsID, parts_name, category, capability, purpose,
	CASE 
		WHEN parts_name REGEXP "Lv.1" THEN "Lv.1"
		WHEN parts_name REGEXP "Lv.2" THEN "LV.2"
		WHEN parts_name REGEXP "Lv.3" THEN "Lv.3"
		ELSE "Lv.1"
	END AS level
FROM parts;

-- REGEXP를 사용하여 정규표현식으로 데이터 찾기
SELECT partsID, parts_name, capability
FROM parts
WHERE parts_name REGEXP "Lv.1";

-- LIKE 조건을 두 개 이상 쓰는 SQL
SELECT *
FROM parts
WHERE capability LIKE "%특수미사일%"
OR
capability LIKE "%특수 미사일%";

SELECT partsID, parts_name, capability
FROM parts
WHERE capability LIKE "%표준미사일%";

SELECT * FROM
(SELECT partsID
FROM parts
WHERE capability LIKE "%EML%");

SELECT partsID, parts_name, capability
FROM parts
WHERE capability LIKE "%TLS%"
OR
capability LIKE "%PLSL%";