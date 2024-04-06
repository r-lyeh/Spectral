call make /O2 /MT /DNDEBUG /GL /GF /arch:AVX2 %*
upx ..\spectral.exe
