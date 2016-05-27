#
#  Generated make file for project C:\tools\CC\examples\windows\xmlview\xmlview.prj
#
all: xmlview.exe 

#
# Tools Definitions
#

CC=OCC
LINK=OLINK
ASM=ONASM
RC=ORC
LIB=OLIB

#
# Default paths
#
INSTALLDIR=C:\orangec
SYSTEMDIR=C:\WINDOWS\System32
WINDIR=C:\WINDOWS
#
# Build rules for target xmlview.exe
#
#
# Options
#

CFLAGS_XMLVIEW_EXE =  /c
AFLAGS_XMLVIEW_EXE = -fieee -Fls -g
RCFLAGS_XMLVIEW_EXE =
LFLAGS_XMLVIEW_EXE =  /c /T:GUI32
LIBFLAGS_XMLVIEW_EXE = 

INCLUDE_XMLVIEW_EXE = 
LIBS_XMLVIEW_EXE = "$(INSTALLDIR)\lib\clwin.l" "$(INSTALLDIR)\lib\climp.l" 
START_XMLVIEW_EXE = "$(INSTALLDIR)\lib\c0pe.o" 
#
# Rules
#

XMLVIEW_EXE =  \
	xml.o  \
	xmlview.o 



xmlview.exe : $(XMLVIEW_EXE) xmlview.res 
	$(LINK) $(LFLAGS_XMLVIEW_EXE) -oxmlview @&&|
$(START_XMLVIEW_EXE) $(XMLVIEW_EXE) $(LIBS_XMLVIEW_EXE) xmlview.res
|

xml.o : xml.c xml.h 
	$(CC) @&&|
$(CFLAGS_XMLVIEW_EXE) -I$(INCLUDE_XMLVIEW_EXE)
 $(DEFINES_XMLVIEW_EXE)
 xml.c
|

xmlview.o : xmlview.c Resource.h xml.h 
	$(CC) @&&|
$(CFLAGS_XMLVIEW_EXE) -I$(INCLUDE_XMLVIEW_EXE)
 $(DEFINES_XMLVIEW_EXE)
 xmlview.c
|

xmlview.res : xmlview.rc Resource.h 
	$(RC) $(RCFLAGS_XMLVIEW_EXE) -i$(INCLUDE_XMLVIEW_EXE) @&&|
$(DEFINES_XMLVIEW_EXE) xmlview.rc
|

