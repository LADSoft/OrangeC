NAME=ocl
MAIN_FILE=CL386.c
INCLUDES=
C_DEPENDENCIES=$(wildcard *.c) winsystem.c
LIB_DEPENDENCIES=
H_FILES=$(wildcard *.h)

vpath %.c ..\util

include ..\redirect.mak

DISTRIBUTE: copyexe
	copy ocl.cfg $(DISTBIN)
