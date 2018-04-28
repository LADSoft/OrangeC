cd ..\lib
imake 
imake -fmakefile.pe
cd ..\test
imake -B
test > q
imake -fmakefile.pe -B
test > r
