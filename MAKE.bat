#!/bin/bash 2>nul || goto :windows
# run `sh MAKE.bat` to compile Spectral

# linux + osx -----------------------------------------------------------------
cd `dirname $0`

# update to latest ------------------------------------------------------------
git reset --hard HEAD~1
git pull

#echo $*

if [ "$(uname)" != "Darwin" ]; then

# setup (ArchLinux) ----------------------------------------------------------
[ ! -f ".setup" ] && sudo pacman -Sy && sudo pacman -Sy --noconfirm gcc && echo>.setup
# setup (Debian, Ubuntu, etc)
[ ! -f ".setup" ] && sudo apt-get -y update && sudo apt-get -y install gcc upx libx11-dev gcc libgl1-mesa-dev libasound2-dev mesa-common-dev libudev-dev && echo>.setup

# compile -------------------------------------------------------------------- do not use -O3 below. zxdb cache will contain 0-byte files otherwise.
gcc src/app.c -I src -o ./Spectral.linux -O2 -DNDEBUG=3 -D_GNU_SOURCE -Wno-unused-result -Wno-unused-value -Wno-format -Wno-multichar -Wno-pointer-sign -Wno-string-plus-int -Wno-empty-body -lm -lX11 -lGL -lasound -lpthread -ludev $* || exit
upx -9 Spectral.linux
#src/res/embed.linux Spectral.linux @SpectralEmBeDdEd
#src/res/embed.linux Spectral.linux src/res/zxdb/Spectral.db.gz
#src/res/embed.linux Spectral.linux @SpectralEmBeDdEd
cat Spectral.linux src/res/embed src/res/zxdb/Spectral.db.gz src/res/embed > Spectral.linux

fi

if [ "$(uname)" = "Darwin" ]; then

# compile --------------------------------------------------------------------
export SDKROOT=$(xcrun --show-sdk-path)
gcc -ObjC src/app.c -I src -o ./Spectral.osx -O3 -DNDEBUG=3 -Wno-unused-result -Wno-unused-value -Wno-format -Wno-multichar -Wno-pointer-sign -Wno-string-plus-int -Wno-empty-body -Wno-dangling-else -framework cocoa -framework iokit -framework CoreFoundation -framework CoreAudio -framework AudioToolbox -framework OpenGL -lm $* || exit
#src/res/embed.osx Spectral.osx @SpectralEmBeDdEd
#src/res/embed.osx Spectral.osx src/res/zxdb/Spectral.db.gz
#src/res/embed.osx Spectral.osx @SpectralEmBeDdEd
cat Spectral.osx src/res/embed src/res/zxdb/Spectral.db.gz src/res/embed > Spectral.osx

# embed icon and make .app
test -d Spectral.app && rm -rf Spectral.app
sh src/res/img/appify.sh --script ./Spectral.osx --name "Spectral" --icons src/res/img/noto_1f47b_1k.icns

# make .dmg from .app
test -f Spectral.dmg && rm Spectral.dmg
sh src/res/img/create-dmg/create-dmg \
    --volname "Spectral" \
    --window-pos 200 120 \
    --window-size 800 400 \
    --icon Spectral 200 190 \
    --app-drop-link 600 185 \
    --background src/res/img/dmg_background0.png \
    --no-internet-enable Spectral.dmg Spectral.app

fi

exit

:windows

@echo off

for /f "tokens=1,* delims= " %%a in ("%*") do set ALL_FROM_2ND=%%b

if "%1"=="" (
    make rel
)

if "%1"=="-h" (
    echo make [bug^|dev^|opt^|rel] [compiler-flags]
    exit /b
)

if "%1"=="test" (
    call make opt -DPRINTER -DTESTS %ALL_FROM_2ND% || goto error
    pause

    rem Z80------------------------------------------

    rem spectral "tests\z80\rak\z80test-1.0\z80ccf.tap"
    rem spectral "tests\z80\rak\z80test-1.0\z80doc.tap"
    rem spectral "tests\z80\rak\z80test-1.0\z80docflags.tap"
    rem spectral "tests\z80\rak\z80test-1.0\z80flags.tap"
    rem spectral "tests\z80\rak\z80test-1.0\z80full.tap"
    rem spectral "tests\z80\rak\z80test-1.0\z80memptr.tap"
    rem spectral "tests\z80\rak\z80test-1.2a\z80ccf.tap"
    rem spectral "tests\z80\rak\z80test-1.2a\z80ccfscr.tap"
    rem spectral "tests\z80\rak\z80test-1.2a\z80doc.tap"
    rem spectral "tests\z80\rak\z80test-1.2a\z80docflags.tap"
    rem spectral "tests\z80\rak\z80test-1.2a\z80flags.tap"
    rem spectral "tests\z80\rak\z80test-1.2a\z80full.tap"
    rem spectral "tests\z80\rak\z80test-1.2a\z80memptr.tap"
    rem spectral "tests\z80\woodster\z80tests.tap"
    rem spectral "tests\z80\zexall\zexall.tap"
    rem spectral "tests\z80\zexall\zexall2.tap"
    rem spectral "tests\z80\zexall\zexbit.tap"
    rem spectral "tests\z80\zexall\zexdoc.tap"
    rem spectral "tests\z80\zexall\zexfix.tap"

    rem spectral "tests\z80\redcode\Z80 XCF Flavor.tap"

    rem https://github.com/redcode/Z80/wiki/Z80-Block-Flags-Test
    rem spectral "tests\z80\z80bltst.tap"

    rem https://github.com/redcode/Z80/wiki/Z80-INT-Skip
    spectral "tests\z80\int_skip.tap"

    exit /b
)

if "%1"=="tidy" (
    del *.obj
    del *.exe
    del *.pdb
    del *.ilk
    del *.zip
    del src\res\zxdb\*.db
    del src\res\zxdb\*.exe
    del src\res\zxdb\*.sqlite
    exit /b
)

if "%1"=="dev" (
    call make nil /Zi %ALL_FROM_2ND% || goto error
    copy /b/y Spectral.exe+src\res\embed+src\res\zxdb\Spectral.db.gz+src\res\embed Spectral.exe > nul

    exit /b
)

if "%1"=="bug" (
    call make dev /fsanitize=address %ALL_FROM_2ND% || goto error

    tasklist /fi "ImageName eq remedybg.exe" 2>NUL | find /I "exe">NUL || (where /q remedybg.exe && start remedybg -q -g Spectral.exe)

    exit /b
)

if "%1"=="opt" (
    rem /dynamicdeopt 
    rem do not use /O1 or /O2 below. ayumi drums will be broken in AfterBurner.dsk otherwise (not anymore?)
    rem do not use /O2 below. +3 FDC may behave weirdly otherwise (AfterBurner.dsk/GNG.dsk)
    rem false positives: +1 (vs19) .. +4 (vs22) - secureage (bc of DNDEBUG and optimization flags lol)
    call make nil /Ox /MT /DNDEBUG /GL /GF %ALL_FROM_2ND% || goto error
    rem false positives: +12
    rem where /q upx.exe && upx Spectral.exe

    rem false positives: +2 - crowdstrike falcon, cylance
    copy /b/y Spectral.exe+src\res\embed SpectralNoZXDB.exe > nul
    rem false positives: +1 - microsoft (defender)
    copy /b/y SpectralNoZXDB.exe+src\res\zxdb\Spectral.db.gz+src\res\embed Spectral.exe > nul

    exit /b
)

if "%1"=="rel" (
    taskkill /f /im Spectral.exe > nul 2> nul
    call make opt -Dmain=WinMain -DNDEBUG=3 %ALL_FROM_2ND% || goto error

    del *.ilk 1> nul 2> nul
    del *.pdb 1> nul 2> nul
    del *.obj 1>nul 2>nul
    del *.ilk 1>nul 2>nul
    del *.pdb 1>nul 2>nul

    exit /b
)

where /q cl.exe || call "%VS170COMNTOOLS%/../../VC/Auxiliary/Build/vcvarsx86_amd64.bat" >nul 2>nul
where /q cl.exe || call "%VS160COMNTOOLS%/../../VC/Auxiliary/Build/vcvarsx86_amd64.bat" >nul 2>nul
where /q cl.exe || call "%VS150COMNTOOLS%/../../VC/Auxiliary/Build/vcvarsx86_amd64.bat" >nul 2>nul
where /q cl.exe || call "%VS140COMNTOOLS%/../../VC/bin/x86_amd64/vcvarsx86_amd64.bat" >nul 2>nul
where /q cl.exe || call "%VS120COMNTOOLS%/../../VC/bin/x86_amd64/vcvarsx86_amd64.bat" >nul 2>nul
where /q cl.exe || call "%ProgramFiles%/microsoft visual studio/2022/community/VC/Auxiliary/Build/vcvarsx86_amd64.bat" >nul 2>nul
where /q cl.exe || call "%ProgramFiles(x86)%/microsoft visual studio/2019/community/VC/Auxiliary/Build/vcvarsx86_amd64.bat" >nul 2>nul
where /q cl.exe || call "%ProgramFiles(x86)%/microsoft visual studio/2017/community/VC/Auxiliary/Build/vcvarsx86_amd64.bat" >nul 2>nul

setlocal enableDelayedExpansion
if "%cc%"=="" (
set "cc=clang-cl -Wno-multichar -Wno-unused-value -Wno-macro-redefined -Wno-implicit-function-declaration -Wno-deprecated-declarations -Wno-empty-body -Wno-pointer-sign -Wno-dangling-else -Wno-string-plus-int"
(where /q clang-cl || set "cc=cl" >nul 2>nul)
)

rem X86 use /arch:SSE2 to maximize performance
rem X64 do not use /arch:AVX2 to maximize compatibility. see issue #4
if "%__DOTNET_PREFERRED_BITNESS%"=="32" (set ARCH=/arch:SSE2) else (set ARCH=/arch:AVX)

echo !cc! src\app.c src\sys_window.cc -I src /FeSpectral.exe !ARCH! %ALL_FROM_2ND%
     !cc! src\app.c src\sys_window.cc -I src /FeSpectral.exe !ARCH! %ALL_FROM_2ND% || goto error


for /F "skip=1 delims=" %%F in ('
    wmic PATH Win32_LocalTime GET Day^,Month^,Year /FORMAT:TABLE
') do (
    for /F "tokens=1-3" %%L in ("%%F") do (
        set today=0%%L
        set month=0%%M
        set year=%%N
    )
)

rem where /q rcedit-x64 || (git clone https://github.com/electron/rcedit && pushd rcedit && git checkout 28a1319 && rc src\rcedit.rc && cl /Fercedit-x64 src\*.res src\*.c* /EHsc version.lib && copy *.exe /y .. && popd)
rem where /q rcedit-x64 || (git clone https://github.com/electron/rcedit/ && pushd rcedit && cmake . && msbuid rcedit.sln && popd)
rem where /q rcedit-x64 || (
rem git clone https://github.com/electron/rcedit && pushd rcedit && git checkout 28a1319
rem echo cmake_minimum_required^(VERSION 3.15^) > CMakeLists.txt
rem echo set^(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded"^) >> CMakeLists.txt
rem echo project^(rcedit^) >> CMakeLists.txt
rem echo add_executable^(rcedit src/main.cc src/rescle.cc src/rcedit.rc^) >> CMakeLists.txt
rem echo target_link_libraries^(rcedit version.lib^) >> CMakeLists.txt
rem cmake . && msbuild rcedit.sln
rem copy /y Debug\rcedit.exe ..\rcedit-x64.exe
rem popd
rem )

where /q rcedit-x64 || curl -LO https://github.com/electron/rcedit/releases/download/v2.0.0/rcedit-x64.exe
where /q rcedit-x64 && rcedit-x64 "Spectral.exe" --set-file-version "!year!.!month!.!today!.!today!!month!"
where /q rcedit-x64 && rcedit-x64 "Spectral.exe" --set-product-version "1.05 Spectral"
where /q rcedit-x64 && rcedit-x64 "Spectral.exe" --set-icon src\res\img\noto_1f47b.ico
rem where /q rcedit-x64 && rcedit-x64 "Spectral.exe" --set-version-string "version" "value"
rem where /q rcedit-x64 && rcedit-x64 "Spectral.exe" --set-resource-string "version" "value"

rem where /Q ResourceHacker.exe && ResourceHacker.exe -open Spectral.exe -save Spectral.exe -action addskip -res src\res\img\noto_1f47b.ico -mask ICONGROUP,MAINICON,0

if "%__DOTNET_PREFERRED_BITNESS%"=="32" (
    move /y Spectral.exe SpectralX86.exe
)

exit /b 0

:error
@copy , ,, >nul
