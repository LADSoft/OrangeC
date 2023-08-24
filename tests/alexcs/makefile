FILES =\
exception.o \
generator.o \
main.o \
parser.o \
scanner.o \
statement.o \
statement_base.o \
sym_table.o \
syntax_node.o \
syntax_node_base.o \

CXXFLAGS= /c -fstack-protector-all /DWIN32_LEAN_AND_MEAN /!
	
test: cs.exe
	cs -G test.pas > q1.out
	fc /b test.cmpx q1.out

clean:
	$(CLEAN)

cs.exe: $(FILES)
	olink /c /! /T:CON32 /mx /o$@ @&&|
c0xpe.o $(FILES) clwin.l climp.l
|