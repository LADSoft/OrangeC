FILES=$(wildcard bzip2*.c)
FILES2=$(wildcard inc*.cpp)
all: gen.exe $(FILES:.c=.ods) $(FILES2:.cpp=.ods)

gen.exe: gen.c
	occ /! gen.c
	gen

clean:
	del *.ods

%.ods: %.c
	occpr --nologo /Q /w /E999 /z$(ORANGEC)\include\c;$(ORANGEC)\include\c\win32 $^
%.ods: %.cpp
	occpr --nologo /Q /w /E999 /z$(ORANGEC)\include\c;$(ORANGEC)\include\c\win32 /Z$(ORANGEC)\include;$(ORANGEC)\include\c;$(ORANGEC)\include\c\win32 $^