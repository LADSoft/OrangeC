%.o: %.cpp
	occ -c /C-E $<

VPATH := som

CPP_DEPENDENCIES=$(wildcard *.cpp)
TEMP = $(CPP_DEPENDENCIES:.cpp=.o)

all: main.exe
		main

main.exe: $(TEMP) random.o
	occ -pl-mx /omain.exe $^

clean: