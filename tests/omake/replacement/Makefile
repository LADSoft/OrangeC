C_DEPENDENCIES = $(wildcard *.c)
C_DEPS = $(C_DEPENDENCIES:.c=.o)

test: $(C_DEPS)
	$(CC) $(C_DEPS) -o $@

clean:
	rm -f $(C_DEPS)
	rm -f test
