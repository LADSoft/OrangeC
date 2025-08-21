all: test

test: testb testa

testa: a/testa
	$(MAKE) -C a test

testb: b/testb
	$(MAKE) -C b test

b/testb:
	$(MAKE) -C b
a/testa:
	$(MAKE) -C a
.PHONY: clean
clean:
	$(MAKE) -C b clean
	$(MAKE) -C a clean
	rm -f hello hello.o
