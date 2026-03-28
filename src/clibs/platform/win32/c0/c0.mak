#  Software License Agreement
#  
#      Copyright(C) 1994-2025 David Lindauer, (LADSoft)
#  
#      This file is part of the Orange C Compiler package.
#  
#      The Orange C Compiler package is free software: you can redistribute it and/or modify
#      it under the terms of the GNU General Public License as published by
#      the Free Software Foundation, either version 3 of the License, or
#      (at your option) any later version.
#  
#      The Orange C Compiler package is distributed in the hope that it will be useful,
#      but WITHOUT ANY WARRANTY; without even the implied warranty of
#      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#      GNU General Public License for more details.
#  
#      You should have received a copy of the GNU General Public License
#      along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
#  
#      contact information:
#          email: TouchStone222@runbox.com <David Lindauer>
#  

C0OBJECTS = .\c0Main.o .\c0wMain.o .\c0WinMain.o .\c0wWinMain.o .\c0DllMain.o .\c0static.o .\c0wstatic.o .\c0vars.o
c0Objects: $(C0OBJECTS)

.\c0Main.o: ..\c0\c0Main.c
	$(CC) $(STDINCLUDE) /c $(CFLAGS) -o$@ $^
.\c0wMain.o: ..\c0\c0wMain.c
	$(CC) $(STDINCLUDE) /c $(CFLAGS) -o$@ $^
.\c0WinMain.o: ..\c0\c0WinMain.c
	$(CC) $(STDINCLUDE) /c $(CFLAGS) -o$@ $^
.\c0wWinMain.o: ..\c0\c0wWinMain.c
	$(CC) $(STDINCLUDE) /c $(CFLAGS) -o$@ $^
.\c0DllMain.o: ..\c0\c0DllMain.c
	$(CC) $(STDINCLUDE) /c $(CFLAGS) -o$@ $^
.\c0static.o: .\c0static.c
	$(CC) $(STDINCLUDE) /c $(CFLAGS) -o$@ $^
.\c0wstatic.o: .\c0wstatic.c
	$(CC) $(STDINCLUDE) /c $(CFLAGS) -o$@ $^
.\c0vars.o: ..\c0\c0vars.c
	$(CC) $(STDINCLUDE) /c $(CFLAGS) -o$@ $^
