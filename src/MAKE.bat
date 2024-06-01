@echo off

for /f "tokens=1,* delims= " %%a in ("%*") do set ALL_BUT_FIRST=%%b

if "%1"=="-h" (
    echo make [dev^|opt^|rel] [compiler-flags]
    exit /b
)

if "%1"=="test" (
    call make opt -DPRINTER -DTESTS %ALL_BUT_FIRST% || goto error
    pause

    rem Z80------------------------------------------

    rem ..\spectral "tests\z80\rak\z80test-1.0\z80ccf.tap"
    rem ..\spectral "tests\z80\rak\z80test-1.0\z80doc.tap"
    rem ..\spectral "tests\z80\rak\z80test-1.0\z80docflags.tap"
    rem ..\spectral "tests\z80\rak\z80test-1.0\z80flags.tap"
    rem ..\spectral "tests\z80\rak\z80test-1.0\z80full.tap"
    rem ..\spectral "tests\z80\rak\z80test-1.0\z80memptr.tap"
    rem ..\spectral "tests\z80\rak\z80test-1.2a\z80ccf.tap"
    rem ..\spectral "tests\z80\rak\z80test-1.2a\z80ccfscr.tap"
    rem ..\spectral "tests\z80\rak\z80test-1.2a\z80doc.tap"
    rem ..\spectral "tests\z80\rak\z80test-1.2a\z80docflags.tap"
    rem ..\spectral "tests\z80\rak\z80test-1.2a\z80flags.tap"
    rem ..\spectral "tests\z80\rak\z80test-1.2a\z80full.tap"
    rem ..\spectral "tests\z80\rak\z80test-1.2a\z80memptr.tap"
    rem ..\spectral "tests\z80\woodster\z80tests.tap"
    rem ..\spectral "tests\z80\zexall\zexall.tap"
    rem ..\spectral "tests\z80\zexall\zexall2.tap"
    rem ..\spectral "tests\z80\zexall\zexbit.tap"
    rem ..\spectral "tests\z80\zexall\zexdoc.tap"
    rem ..\spectral "tests\z80\zexall\zexfix.tap"

    rem ..\spectral "tests\z80\redcode\Z80 XCF Flavor.tap"

    rem https://github.com/redcode/Z80/wiki/Z80-Block-Flags-Test
    rem ..\spectral "tests\z80\z80bltst.tap"

    rem https://github.com/redcode/Z80/wiki/Z80-INT-Skip
    ..\spectral "tests\z80\int_skip.tap"

    exit /b
)

if "%1"=="dev" (
    cl zx.c sys_window.cc /Fe..\Spectral.exe /Zi %ALL_BUT_FIRST% || goto error

    tasklist /fi "ImageName eq remedybg.exe" 2>NUL | find /I "exe">NUL || start remedybg -q -g ..\Spectral.exe

    exit /b
)

if "%1"=="opt" (
    rem do not use /O1 or /O2 below. ayumi drums will be broken in AfterBurner.dsk otherwise
    call make nil /Ox /MT /DNDEBUG /GL /GF /arch:AVX2 %ALL_BUT_FIRST% || goto error
    exit /b
)

if "%1"=="rel" (
    call make opt -Dmain=WinMain -DNDEBUG=3 %ALL_BUT_FIRST% || goto error
    where /q upx.exe && upx ..\spectral.exe 
    exit /b 1
)

@echo on
cl zx.c sys_window.cc /FeSpectral.exe %ALL_BUT_FIRST% || goto error

@echo off
where /Q ResourceHacker.exe && ResourceHacker.exe -open Spectral.exe -save ..\Spectral.exe -action addskip -res res\img\noto_1f47b.ico -mask ICONGROUP,MAINICON,0 || move /y Spectral.exe ..\Spectral.exe

del ..\*.ilk 1> nul 2> nul
del ..\*.pdb 1> nul 2> nul
del *.obj 1>nul 2>nul
del *.ilk 1>nul 2>nul
del *.pdb 1>nul 2>nul
del *.exe 1>nul 2>nul

exit /b 0

:error
@copy , ,, >nul
