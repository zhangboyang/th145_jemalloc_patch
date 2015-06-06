gcc -O2 -g -Wall -shared -o patch.dll patch.c
gcc -O2 -g -Wall -shared -o rmalloc.dll rmalloc.c
gcc -Os -g -Wall -o shellcode.exe shellcode.c
pause
strip patch.dll
strip rmalloc.dll
strip shellcode.exe
pause
