-- SQLite Archive 파일에서 원하는 데이터를 압축 해제
CREATE TABLE memdb.uncompressed AS
SELECT name, mode, datetime(mtime,'unixepoch'), sqlar_uncompress(data,sz) as uncompressed_data
FROM sqlar
WHERE name='test_db_000.db';

-- 압축 해제한 데이터를 사용하여 `test_table1` 테이블을 조회
ATTACH DATABASE 'file::memory:?cache=shared' AS memdb;
CREATE TABLE memdb.test_table1 AS SELECT * FROM test_db_000.test_table1;
SELECT * FROM memdb.test_table1;