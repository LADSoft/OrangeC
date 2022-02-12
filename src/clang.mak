# Software License Agreement
# 
#     Copyright(C) 1994-2022 David Lindauer, (LADSoft)
# 
#     This file is part of the Orange C Compiler package.
# 
#     The Orange C Compiler package is free software: you can redistribute it and/or modify
#     it under the terms of the GNU General Public License as published by
#     the Free Software Foundation, either version 3 of the License, or
#     (at your option) any later version.
# 
#     The Orange C Compiler package is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#     GNU General Public License for more details.
# 
#     You should have received a copy of the GNU General Public License
#     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
# 
#     contact information:
#         email: TouchStone222@runbox.com <David Lindauer>
# 

ifeq "$(COMPILER)" "CLANG"

UCRTPATH=C:\Program Files (x86)\Windows Kits\10\Lib\10.0.10240.0\ucrt\x86
CLANG_PATH := c:\program files\llvm
OBJ_IND_PATH := clang

CPP_deps = $(notdir $(CPP_DEPENDENCIES:.cpp=.obj))
C_deps = $(notdir $(C_DEPENDENCIES:.c=.obj))
ASM_deps = $(notdir $(ASM_DEPENDENCIES:.nas=.obj))
TASM_deps = $(notdir $(TASM_DEPENDENCIES:.asm=.obj))
RES_deps = $(notdir $(RC_DEPENDENCIES:.rc=.res))

MAIN_DEPENDENCIES = $(MAIN_FILE:.cpp=.obj)
ifeq "$(MAIN_DEPENDENCIES)" "$(MAIN_FILE)"
MAIN_DEPENDENCIES = $(MAIN_FILE:.c=.obj)
endif

LLIB_DEPENDENCIES := $(notdir $(filter-out $(addsuffix .obj,$(EXCLUDE)) $(MAIN_DEPENDENCIES), $(CPP_deps) $(C_deps) $(ASM_deps) $(TASM_deps)))



CC="$(CLANG_PATH)\bin\clang-cl"
PCC="$(CLANG_PATH)\bin\clang++"
CCFLAGS = /O2 /EHs /c /nologo -m32 -Wno-deprecated-declarations -Wno-potentially-evaluated-expression -Wno-switch

LINK=link.exe
LFLAGS=/LTCG:incremental /nologo /NXCOMPAT /DYNAMICBASE /MACHINE:x86 /OPT:REF /SAFESEH  /OPT:ICF /TLBID:1

LIBEXE=lib.exe
LIBFLAGS=/MACHINE:x86 /LTCG /nologo
LIB_EXT:=.lib
LIB_PREFIX:=
TASM=$(COMPILER_PATH)\bin\tasm32

ASM=nasm
ASMFLAGS = -fwin32

RC=rc.exe
RCFLAGS = ./r

ifneq "$(INCLUDES)" ""
CINCLUDES:=$(addprefix /I,$(INCLUDES))
endif
DEFINES := $(addprefix /D,$(DEFINES))
DEFINES := $(subst @, ,$(DEFINES))
LIB_DEPENDENCIES := $(foreach file,$(addsuffix $(LIB_EXT),$(LIB_DEPENDENCIES)), $(_LIBDIR)\$(file))

CFLAGS := $(CCFLAGS) $(CINCLUDES) $(DEFINES) /DMICROSOFT /DWIN32
CXXFLAGS := $(CCFLAGS) $(CINCLUDES) $(DEFINES) /DMICROSOFT /DWIN32

ifeq "$(TARGET)" "GUI"
LFLAGS:= $(LFLAGS) /SUBSYSTEM:WINDOWS
else
LFLAGS:= $(LFLAGS) /MANIFEST /SUBSYSTEM:CONSOLE /MANIFESTUAC:"level='asInvoker' uiAccess = 'false'"
endif

COMPLIB:=$(COMPLIB) "kernel32.lib" "user32.lib" "gdi32.lib" "winspool.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "odbc32.lib" "odbccp32.lib" "uxtheme.lib" "comctl32.lib" "msimg32.lib"

vpath %.obj $(_OUTPUTDIR)
vpath %.lib $(_LIBDIR)
vpath %.res $(_OUTPUTDIR)

%.obj: %.cpp
	$(CC) $(CXXFLAGS) -Fo$(_OUTPUTDIR)/$@ $^

%.obj: %.c
	$(CC) $(CFLAGS) -Fo$(_OUTPUTDIR)/$@ $^

%.obj: %.asm
	$(TASM) /ml /zi /i$(INCLUDE) $(ADEFINES) $^, $(_OUTPUTDIR)/$@

%.obj: %.nas
	$(ASM) $(ASMFLAGS) -o$(_OUTPUTDIR)/$@ $^

%.res: %.rc
	$(RC) -i$(RCINCLUDE) $(RCFLAGS) -fo$(_OUTPUTDIR)/$@ $^

$(_LIBDIR)\$(NAME)$(LIB_EXT): $(LLIB_DEPENDENCIES)
#	-del $()_LIBDIR)\$(NAME)$(LIB_EXT) 2> $(NULLDEV)
	$(LIBEXE) $(LIBFLAGS) /OUT:$(_LIBDIR)\$(NAME)$(LIB_EXT) $(addprefix $(_OUTPUTDIR)\,$(LLIB_DEPENDENCIES)) 


$(NAME).exe: $(MAIN_DEPENDENCIES) $(LIB_DEPENDENCIES) $(NAME)$(LIB_EXT) $(_LIBDIR)\$(NAME)$(LIB_EXT) $(RES_deps)
	$(LINK) $(TYPE) $(LFLAGS) $(COMPLIB) /LIBPATH:"$(UCRTPATH)" /LIBPATH:"$(VCINSTALLDIR)\lib" /OUT:$@ $(_LIBDIR)\$(NAME)$(LIB_EXT) $(LIB_DEPENDENCIES) $(addprefix /DEF:,$(DEF_DEPENDENCIES)) $(addprefix $(_OUTPUTDIR)\,$(RES_deps)) $(addprefix $(_OUTPUTDIR)\,$(MAIN_DEPENDENCIES))


%.exe: %.c
	$(CC) $(CFLAGS) -nologo $^

%.exe: %.cpp
	$(CC) $(CXXFLAGS) -nologo $^

endif