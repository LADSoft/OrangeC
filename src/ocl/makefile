NAME=ocl
MAIN_FILE=CL386.c
INCLUDES=
C_DEPENDENCIES=$(wildcard *.c) winsystem.c
LIB_DEPENDENCIES=

vpath %.c ..\util

redirect:
	@$(MAKE) /T

DISTRIBUTE: copyexe
	copy ocl.cfg $(DISTBIN)
	copy ocl.lic $(DISTLIC)