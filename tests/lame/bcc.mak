CFILES=$(wildcard *.c)
all:
	bcc32 $(CFILES)