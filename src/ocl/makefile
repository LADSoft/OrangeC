NAME=ocl
MAIN_FILE=CL386.c
INCLUDES=
C_DEPENDENCIES=$(wildcard *.c)
LIB_DEPENDENCIES=

redirect:
	@$(MAKE) /T

DISTRIBUTE: copyexe
	copy ocl.cfg $(DISTBIN)
	copy ocl.lic $(DISTLIC)