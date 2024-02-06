

-- 같은 테이블의 서브쿼리로는 UPDATE할 수 없다.
/*
UPDATE parameterLabels AS A
SET slot = B.slot
FROM (
    SELECT purpose, row_number() OVER (PARTITION BY purpose ORDER BY ParameterLabel) - 1 AS slot
    FROM parameterLabels
    GROUP BY purpose
) AS B
WHERE A.purpose = B.purpose;
*/

UPDATE parameterLabels
SET purpose = (
	SELECT purpose
	FROM parts_final_data A
	-- 다른 테이블의 서브 쿼리로부터 상응하는 값을 가져오기 위해서는 반드시 아래처럼 condition clue를 
	-- 우변 좌변 모두 table.value 형식으로 정확히 표기해야한다.
	-- 또한 상단의 UPDATE되는 테이블에는 A, B 등을 지정할 수 없다.
	WHERE A.ParameterLabel = parameterLabels.ParameterLabel);

-- pd.df로부터 upadate
UPDATE parameterLabels
SET calculed_value = ?
WHERE ParameterLabel = ?;

UPDATE parts_level
SET choice = (
SELECT choice
FROM exception_choice);

UPDATE merged_parts
SET parts_pk = (
  SELECT A.pk
  FROM parts A
  WHERE A.partsID = merged_partsID
);

UPDATE merged_parts
SET union_partsID = 48, union_parts_name = "고성능 작약"
WHERE purpose = "폭탄 성능";

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

-- level에서 level 값을 추출해 INTEGER 값으로 업데이트
UPDATE parts_level
SET level_v = CAST(substr(level, 4) AS INTEGER);

-- 서브쿼리로 서로 다른 테이블의 partsID를 매치시켜 UPDATE
UPDATE parts_capability
SET parts_name = (
  SELECT A.parts_name
  FROM parts A
  WHERE A.partsID = parts_capability.partsID
);

--
UPDATE merged_parts
SET union_partsID = 48, union_parts_name = "램제트식 추진 장치(SPW) Lv.1"
WHERE purpose = "특수미사일 성능";

-- 특정 칼럼의 특정 텍스트를 다른 텍스트로 대치(replacement)
UPDATE parts
SET capability = replace(capability, '위력 향상', '위력 증가');

-- 특정 범위 purpose 일괄 변경 SQL
UPDATE parts
SET purpose = "기체 성능"
WHERE partsID >= 93 AND partsID <= 97;

-- LIKE 매치에서 찾은 모든 partsID들과 매치되는 데이터들의 purpose를 원하는 값으로 변경
UPDATE parts
SET purpose = "기관포 성능"
WHERE partsID IN 
(SELECT partsID
FROM parts
WHERE capability LIKE "%기관포%");