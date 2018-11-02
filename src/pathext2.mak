ifeq "$(PATHEXT2)" ""
ifeq "$(COMPILER)" "gcc-linux"
PATHEXT2=/#
else
QQQ:=\#
AA:=AA
ifeq "$(AA)" "AA" # will be true for mingW32-make
PATHEXT2=\\#
else
PATHEXT2=\#
endif
endif
endif
export PATHEXT2