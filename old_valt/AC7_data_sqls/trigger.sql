DROP TRIGGER update_exception_choice;
DROP TRIGGER insert_exception_choice;
DROP TRIGGER delete_exception_choice;

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