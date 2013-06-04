LIBRARY:
EXEFILE: mkobj ocpp.exe

mkobj:
	-mkdir $(OBJECT)

OBJECT=objectb55
OBJ=.obj
MYCC = bcc32 -c -v
MYCCFLAGS = /I$(SRCPATH) /DCPREPROCESSOR /c /DUSE_LONGLONG /DBORLAND
MYASM = nasm -fobj
MYLIB = tlib32
MYLINK = ilink32 -Gn -Gi -v -m

SRCPATH=.\;..\parse;..\middle

vpath %.c $(SRCPATH)
vpath %.o $(OBJECT)
vpath %.obj $(OBJECT)

%$(OBJ): %.c
	$(MYCC) $(MYCCFLAGS) -o$@ $^

#.path.c = .\;..\preproc;..\middle

NAME = ocpp
EXE_dependencies = \
	ppmain$(OBJ) \
	osutil$(OBJ) \
	memory$(OBJ) \
	symtab$(OBJ) \
	ccerr$(OBJ) \
	preproc$(OBJ) \
	ppexpr$(OBJ)	

#		*Explicit Rules*
$(NAME).exe: $(EXE_dependencies)
	$(MYLINK) $(LINKFLAGS)  @&&|
c0x32.obj $(addprefix $(OBJECT)\,$(EXE_dependencies)),ocpp.exe,ocpp.map, cw32.lib import32.lib
|
	$(PEPATCH) $(NAME).exe

DISTRIBUTE:
	copy $(RELEASEPATH)\ocpp.exe $(DISTBIN)
	$(RESTUB) $(DISTBIN)\ocpp.exe $(STUB)
#	upx $(DISTBIN)\ocpp.exe
#	$(RENSEG) $(DISTBIN)\ocpp.exe
	copy target.cfg $(DISTBIN)\ocpp.cfg
