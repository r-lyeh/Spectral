if not exist ZXDB.sqlite (

if not exist sqlite3.exe (cl sqlite3.c shell.c /O2 /MT || exit /b)
if exist *.obj del *.obj

where /q unzip.exe || (echo cannot find unzip.exe in path && exit /b)
where /q python.exe || (echo cannot find python.exe in path && exit /b)
where /q sqlite3.exe || (echo cannot find sqlite3.exe in path && exit /b)

rd /q /s ZXDB >nul 2>nul
git clone --depth 1 https://github.com/ZXDB/ZXDB && ^
pushd ZXDB && ^
unzip ZXDB_mysql.sql.zip && ^
python scripts\ZXDB_to_SQLite.py && ^
popd && ^
move ZXDB\ZXDB_sqlite.sql && ^
rd /q /s ZXDB >nul 2>nul

if exist ZXDB_sqlite.sql (
echo.|sqlite3 -init ZXDB_sqlite.sql ZXDB.sqlite
del ZXDB_sqlite.sql
)

)

rem scripts\ZXDB_help_search.sql

rem sqlite3 ZXDB.sqlite
rem sqlite> .headers on
rem sqlite> .excel
rem sqlite> select * from aliases limit 20;
rem sqlite> select * from entries where title like 'myst%';
