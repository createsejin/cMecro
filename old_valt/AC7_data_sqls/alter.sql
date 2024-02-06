
-- DROP TABLE parts_level;

CREATE TABLE "parts_level" ( 
"pk" INTEGER, 
"parts_pk" INTEGER, 
"partsID" INTEGER, 
"level" TEXT, 
"choice" INTEGER DEFAULT 0,
FOREIGN KEY("parts_pk") d
REFERENCES "parts"("pk"), 
PRIMARY KEY("pk" AUTOINCREMENT) );


ALTER TABLE parts
DROP COLUMN level;



