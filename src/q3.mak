all: a1.exe

a1.exe: code\a1.c
	occ /c $< /otarget\objs\  

#aa
