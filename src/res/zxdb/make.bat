@echo off

rem checks. compile if needed
where /q python.exe || (echo cannot find python.exe in path && exit /b)
where /q sqlite3.exe || (cl sqlite3.c shell.c /MT || exit /b)
where /q zxdb2txt.exe || (cl zxdb2txt.c sqlite3.c /MT /Ox /Oy || exit /b)
if exist *.obj del *.obj
if exist Z*.sql* del Z*.sql*

rem clone
rd /q /s ZXDB >nul 2>nul
git clone --depth 1 https://github.com/ZXDB/ZXDB && ^
python -m zipfile -e ZXDB\ZXDB_mysql.sql.zip . && ^
python ZXDB\scripts\ZXDB_to_SQLite.py && ^
rd /q /s ZXDB >nul 2>nul

rem generate full zxdb, 133 mib
if not exist ZXDB_sqlite.sql (echo Cannot find ZXDB_sqlite.sql && exit /b)
echo.|sqlite3 -init ZXDB_sqlite.sql  ZXDB_FULL.sqlite

rem generate lite zxdb, 55 mib
copy /y ZXDB_FULL.sqlite ZXDB.sqlite
echo.|sqlite3 -init ZXDB_trim.script ZXDB.sqlite

rem clean up
del ZXDB_mysql.sql
del ZXDB_sqlite.sql

rem prompt user
echo We may convert the SQLite database into a custom DB file now.
echo If you convert the DB now, next Spectral build will use this custom DB backend ^(1MiB overhead^).
echo If you skip the conversion now, next Spectral build will include and use the SQLite backend ^(56MiB overhead^).
echo The conversion is painfully slow, though.
choice /C YN /M "Convert? "

rem convert if requested
if "%errorlevel%"=="1" (
zxdb2txt 0..65535 > spectral.db && python -m gzip --best spectral.db && echo Ok!
)
