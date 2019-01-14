include ../pathext2.mak
NAME=ocl
MAIN_FILE=cl386.c
INCLUDES=
C_DEPENDENCIES=$(wildcard *.c) winsystem.c
LIB_DEPENDENCIES=
H_FILES=$(wildcard *.h)

vpath %.c ..$(PATHEXT2)util

include ../redirect.mak

DISTRIBUTE: copyexe
	copy ocl.cfg $(DISTBIN)
