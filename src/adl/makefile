# This makefile is for Borland make (IBM PC/80386+)

#		*Translator Definitions*
# Comment this next line out to generate the 68k compiler
# leave it in to generate the 386 compiler
#i386 = -Di386

CC = \bcc55\bin\bcc32 -c -v -DBORLANDC -w- /I..\util
ASM = TASM /zi /ml /m2
LIB = tlib
LINK = \bcc55\bin\ilink32 -Gn -Gi
INCLUDEPATH = C:\bc45\INCLUDE;..\include

.path.cpp=.\;..\obj;..\lib;..\util
.cpp.obj:
  $(CC) $*
#		*List Macros*

EXE_dependencies = \
		CmdSwitch.obj \
		CmdFiles.obj \
		Utils.obj \
		xml.obj \
		GenParser.obj \
		Loader.obj \
		Tokenizer.obj \
		ADLMain.obj
#		*Explicit Rules*
adl.exe: $(EXE_dependencies)
  $(LINK) /Tpe /ap /c/v/s/L\bcc55\lib @&&|
c0x32.obj $(EXE_dependencies)
adl.exe
adl.map
import32.lib cw32.lib
|
