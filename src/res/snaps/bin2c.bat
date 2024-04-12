for %%i in (4*.rom;1*.rom)      do bin2c.exe %%i %%~ni rom%%~ni
for %%i in (*.z80;*.sna;*.scr;p*.rom) do bin2c.exe %%i %%~ni    %%~ni
