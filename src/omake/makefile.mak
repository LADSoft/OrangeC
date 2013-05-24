NAME := emake
IPATH := ..\obj;..\util
CPP_files = *.cpp ..\util\*.cpp
####################
CC = \bcc55\bin\bcc32 -c -v -DBORLANDC -w- /I$(IPATH)
ASM = TASM /zi /ml /m2
LIB = tlib
LINK = \bcc55\bin\ilink32 -Gn -Gi

vpath %.cpp $(IPATH:;= )

%.obj: %.cpp
	$(CC) $^

CPP_dependencies = $(wildcard $(CPP_files))
EXE_dependencies := $(notdir $(CPP_dependencies:.cpp=.obj))

#		*Explicit Rules*
$(NAME).exe: $(EXE_dependencies)
	$(LINK) /Tpe /ap /c/v/s/L\bcc55\lib @&&|
c0x32.obj $^
$(NAME).exe
$(NAME).map
import32.lib cw32.lib
|
