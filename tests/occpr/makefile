FILES=$(wildcard bzip2*.c)
FILES2=$(wildcard inc*.cpp)
all: gen.exe $(FILES:.c=.ods) $(FILES2:.cpp=.ods)

gen.exe: gen.c
	occ /! gen.c
	gen

clean:
	$(CLEAN)

%.ods: %.c
	occpr --nologo /Q /w /E999 /z$(ORANGEC)\include\c $^
%.ods: %.cpp
	occpr --nologo /Q /w /E999 /z$(ORANGEC)\include\c /Z$(ORANGEC)\include;$(ORANGEC)\include\c $^