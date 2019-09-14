.PHONY: all, compile
all: compile
	tester.exe help @var
compile:
	gcc tester.cpp -o tester.exe
