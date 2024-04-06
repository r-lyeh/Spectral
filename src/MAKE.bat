cl run.c %* || exit /b && rem /link /SUBSYSTEM:WINDOWS

rem mt.exe -manifest run.exe.manifest -outputresource:run.exe;1
ResourceHacker.exe -open run.exe -save ..\spectral.exe -action addskip -res img\noto_1f47b.ico -mask ICONGROUP,MAINICON,0 

del *.obj
del *.ilk
del *.pdb
del *.exe
