cl zx.c sys_window.cc %* || MAKEerr && rem /link /SUBSYSTEM:WINDOWS

@echo off
rem mt.exe -manifest zx.exe.manifest -outputresource:zx.exe;1
where /Q ResourceHacker.exe && ResourceHacker.exe -open zx.exe -save ..\spectral.exe -action addskip -res res\img\noto_1f47b.ico -mask ICONGROUP,MAINICON,0 || move /y zx.exe ..\spectral.exe

del *.obj 1>nul 2>nul
del *.ilk 1>nul 2>nul
del *.pdb 1>nul 2>nul
del *.exe 1>nul 2>nul
