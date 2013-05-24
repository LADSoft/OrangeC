#			D3X DOS-Extender examples makefile
#					
#				  (c) GrayWolf

all: ex1.exe ex2.exe coff.exe le.exe

ex1.exe:
	nasm -fobj ex1.asm
	alink ex1.obj
	stubx -s ex1.exe
	del ex1.obj

ex2.exe:
	tasm /m /ml /p /q /t /z ex2.asm
	tlink /3 /c /d /x ex2.obj
	stubx -s ex2.exe
	del ex2.obj

coff.exe:
	gcc -o coff.exe -Wall -s -nostdlib coff.S
	stubx -s coff.exe

le.exe:
	wasm le.asm
	wlink f le.obj
	stubx -s le.exe
	del le.obj
