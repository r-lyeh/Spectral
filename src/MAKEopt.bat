@rem do not use /O1 or /O2 below. ayumi drums will be broken in AfterBurner.dsk otherwise
@call make /Ox /MT /DNDEBUG /GL /GF /arch:AVX2 %*
