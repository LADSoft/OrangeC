FILES=$(wildcard bzip2*.c)
FILES2=$(wildcard inc*.cpp)
all: gen.exe $(FILES:.c=.ods) $(FILES2:.cpp=.ods)

gen.exe: gen.c
	occ gen.c
	gen

%.ods: %.c
	occpr --nologo /Q /w /z$(ORANGEC)\include $^
%.ods: %.cpp
	occpr --nologo /Q /w /z$(ORANGEC)\include $^