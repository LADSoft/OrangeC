ifeq "$(PATHEXT2)" ""
ifeq "$(COMPILER)" "gcc-linux"
PATHEXT2=/#
else
PATHEXT2=\\#
endif
endif
export PATHEXT2