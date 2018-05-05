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

%.o: %.cpp
	occ /c /! -o$@ $^

test: cs.exe
	cs -G test.pas > q1.out
	fc /b test.cmpx q1.out

cs.exe: $(FILES)
	olink /c /! /T:CON32 /mx /o$@ @&&|
c0xpe.o $(FILES) clwin.l climp.l
|