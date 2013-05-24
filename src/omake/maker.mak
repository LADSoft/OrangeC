$(info hi dave)
CC=cc386
CFLAGS = /9 /c

ASM=nasm
ASMFLAGS = -fobj
$(OBJECT)\%.obj: %.c
	$(CC) $(CFLAGS) $(BUILDING_DLL) -o$@ $^

$(OBJECT)\%.obj: %.nas
	$(ASM) $(ASMFLAGS) $(BUILDING_DLL) -o$@ $^

C_deps = $(C_DEPENDENCIES:.obj=.c)
ASM_deps = $(ASM_DEPENDENCIES:.obj=.nas)

$(DEPENDENCIES) = $(C_deps) $(ASM_deps)