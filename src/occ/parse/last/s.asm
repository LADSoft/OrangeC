    .386p
    ifdef ??version
    if    ??version GT 500H
    .mmx
    endif
    endif
    model flat
    ifndef	??version
    ?debug	macro
    endm
    endif
    ?debug	S "s.cpp"
    ?debug	T "s.cpp"
_TEXT	segment dword public use32 'CODE'
_TEXT	ends
_DATA	segment dword public use32 'DATA'
_DATA	ends
_BSS	segment dword public use32 'BSS'
_BSS	ends
DGROUP	group	_BSS,_DATA
_TEXT	segment dword public use32 'CODE'
_TEXT	ends
    ?debug	D "c:\Bcc55\include\_nfile.h" 10459 8224
    ?debug	D "c:\Bcc55\include\_null.h" 10459 8224
    ?debug	D "c:\Bcc55\include\_defs.h" 10459 8224
    ?debug	D "c:\Bcc55\include\_stddef.h" 10459 8224
    ?debug	D "c:\Bcc55\include\stdio.h" 10459 8224
    ?debug	D "s.cpp" 16249 20548
    end
