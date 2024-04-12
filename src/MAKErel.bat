call makeopt -Dmain=WinMain -DFLAGS=FLAGS_REL %*
del ..\*.ilk
del ..\*.pdb

upx ..\spectral.exe
