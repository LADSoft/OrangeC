NAME=ocl
MAIN_FILE=CL386.c
INCLUDE=
C_DEPENDENCIES=$(wildcard *.c)
LIB_DEPENDENCIES=

LIBRARY: $(OBJECT)\$(NAME).lib
EXEFILE: $(NAME).exe

include $(CPPMAKE)

DISTRIBUTE: copyexe
	copy ocl.cfg $(DISTBIN)
	copy ocl.lic $(DISTLIC)