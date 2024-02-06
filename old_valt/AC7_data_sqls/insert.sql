DELETE FROM parameterLabels;
INSERT INTO parameter_values (ParameterLabel_pk, ParameterLabel, OperationType, ParameterValue)
SELECT B.pk, A.ParameterLabel, A.OperationType, A.ParameterValue
FROM same_parameterLabels A
JOIN parameterLabels B
ON B.ParameterLabel = A.ParameterLabel;


INSERT INTO parameterLabels (ParameterLabel, OperationType, union_partsID, union_parts_name)
SELECT ParameterLabel, OperationType, union_partsID, union_parts_name
FROM same_parameterLabels
GROUP BY ParameterLabel;
UPDATE parameterLabels
SET purpose = (
	SELECT purpose
	FROM parts_final_data A
	WHERE A.ParameterLabel = parameterLabels.ParameterLabel);

-- exception_choice INSERT
INSERT INTO exception_choice (parts_pk, level_pk, partsID, parts_name, choice)
SELECT A.parts_pk, A.pk, A.partsID, B.parts_name, 0 as choice
FROM parts_level A
JOIN parts B
ON A.parts_pk = B.pk
WHERE A.partsID = 133;

INSERT INTO merged_parts (merged_partsID, merged_part_name, category, merged_capability, purpose)
SELECT partsID, parts_name, category, capability, purpose
FROM parts_using
WHERE partsID >= 83 AND partsID <= 84;

INSERT INTO parts_capability (parts_pk, partsID, parts_name, slot, ParameterLabel, OperationType, ParameterValue)
SELECT A.pk as parts_pk, A.partsID, A.parts_name, B.slot, B.parameter, B.type, B.value
FROM parts A
JOIN parts_capa_temp B
ON A.partsID = B.partsID
WHERE B.pk = 279;

INSERT INTO parts_capa_temp (partsID, slot, parameter, type, value)
VALUES (?, ?, ?, ?, ?);

INSERT INTO parts_capability (parts_pk, partsID, parts_name, capa_num, ParameterLabel, OperationType, ParameterValue)
VALUES (10, 10, "ECU 소프트웨어 업데이트 Lv.2", 1, "EPartsParameterList::PA_HighGSpeedStartDec", "RATE", 0.8);


-- 데이터 rows를 parts_level에 INSERT 한다.
INSERT INTO parts_level (parts_pk, partsID, prifix_name, level)
SELECT pk, partsID,
	CASE
		WHEN parts_name REGEXP "Lv.1" THEN replace(parts_name, "Lv.1", "")
		WHEN parts_name REGEXP "Lv.2" THEN replace(parts_name, "Lv.2", "")
		WHEN parts_name REGEXP "Lv.3" THEN replace(parts_name, "Lv.3", "")
		ELSE parts_name
	END AS prifix_name,
	CASE 
		WHEN parts_name REGEXP "Lv.1" THEN "Lv.1"
		WHEN parts_name REGEXP "Lv.2" THEN "LV.2"
		WHEN parts_name REGEXP "Lv.3" THEN "LV.3"
		ELSE "Lv.1"
	END AS level
FROM parts;

-- CASE와 REGEXP를 사용하여 parts_level에 들어갈 데이터셋을 만든다.
SELECT pk, partsID,
	CASE 
		WHEN parts_name REGEXP "Lv.1" THEN 0
		WHEN parts_name REGEXP "Lv.2" THEN 1
		ELSE 1
	END AS choice
FROM parts;