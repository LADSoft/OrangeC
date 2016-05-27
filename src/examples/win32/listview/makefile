#
#  Generated make file for project C:\tools\CC\examples\windows\listview\listview.prj
#
all: listview.exe 

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
# Build rules for target listview.exe
#
#
# Options
#

CFLAGS_LISTVIEW_EXE =  /c
AFLAGS_LISTVIEW_EXE = -fieee -Fls -g
RCFLAGS_LISTVIEW_EXE = 
LFLAGS_LISTVIEW_EXE =  /c /T:GUI32
LIBFLAGS_LISTVIEW_EXE = 

INCLUDE_LISTVIEW_EXE = 
LIBS_LISTVIEW_EXE = "clwin.l" "climp.l" 
START_LISTVIEW_EXE = "c0pe.o" 
#
# Rules
#

LISTVIEW_EXE =  \
	xml.o  \
	listview.o 



listview.exe : $(LISTVIEW_EXE) listview.res 
	$(LINK) $(LFLAGS_LISTVIEW_EXE) -olistview @&&|
$(START_LISTVIEW_EXE) $(LISTVIEW_EXE) $(LIBS_LISTVIEW_EXE) listview.res
|

xml.o : xml.c xml.h 
	$(CC) @&&|
$(CFLAGS_LISTVIEW_EXE) -I$(INCLUDE_LISTVIEW_EXE)
 $(DEFINES_LISTVIEW_EXE)
 xml.c
|

listview.o : listview.c Resource.h xml.h 
	$(CC) @&&|
$(CFLAGS_LISTVIEW_EXE) -I$(INCLUDE_LISTVIEW_EXE)
 $(DEFINES_LISTVIEW_EXE)
 listview.c
|

listview.res : listview.rc Resource.h 
	$(RC) $(RCFLAGS_LISTVIEW_EXE) -i$(INCLUDE_LISTVIEW_EXE) @&&|
$(DEFINES_LISTVIEW_EXE) listview.rc
|

