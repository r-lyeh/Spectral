cl run.c /O2 /MT /DNDEBUG /GL /GF /arch:AVX2 -Dmain=WinMain %* && rem /link /SUBSYSTEM:WINDOWS

rem mt.exe -manifest run.exe.manifest -outputresource:run.exe;1
ResourceHacker.exe -open run.exe -save ..\spectral.exe -action addskip -res noto_1f47b.ico -mask ICONGROUP,MAINICON,0 
upx ..\spectral.exe

del *.obj
del *.ilk
del *.pdb
del *.exe
