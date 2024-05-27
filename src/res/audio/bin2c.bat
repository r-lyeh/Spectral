for %%i in (*.wav) do (
	sox %%i -r 22050 -b 16 -c 1 -e signed-integer wav%%~ni.wav
	bin2c.exe wav%%~ni.wav %%~ni wav%%~ni
)

del wav*.
del wav*.wav
