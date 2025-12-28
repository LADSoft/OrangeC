DIRS = replacement multi_file basic_test
ifdef $(SHELL)
DIRS += linux
endif
.PHONY: test clean $(DIRS)
test: $(DIRS)
$(DIRS):
	$(MAKE) -C $@