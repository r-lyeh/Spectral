cl zx.c sys_window.cc %* || exit /b && rem /link /SUBSYSTEM:WINDOWS

@echo off
rem mt.exe -manifest zx.exe.manifest -outputresource:zx.exe;1
ResourceHacker.exe -open zx.exe -save ..\spectral.exe -action addskip -res res\img\noto_1f47b.ico -mask ICONGROUP,MAINICON,0 || exit /b

del *.obj 1>nul 2>nul
del *.ilk 1>nul 2>nul
del *.pdb 1>nul 2>nul
del *.exe 1>nul 2>nul
