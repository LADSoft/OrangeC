;  Software License Agreement
;  
;      Copyright(C) 1994-2024 David Lindauer, (LADSoft)
;  
;      This file is part of the Orange C Compiler package.
;  
;      The Orange C Compiler package is free software: you can redistribute it and/or modify
;      it under the terms of the GNU General Public License as published by
;      the Free Software Foundation, either version 3 of the License, or
;      (at your option) any later version.
;  
;      The Orange C Compiler package is distributed in the hope that it will be useful,
;      but WITHOUT ANY WARRANTY; without even the implied warranty of
;      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;      GNU General Public License for more details.
;  
;      You should have received a copy of the GNU General Public License
;      along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
;  
;      contact information:
;          email: TouchStone222@runbox.com <David Lindauer>
;  

%include "matherr.inc"
%ifdef __BUILDING_LSCRTL_DLL
[export _scalbn]
[export _scalbln]
[export _scalbnf]
[export _scalblnf]
[export _scalbnl]
[export _scalblnl]
%endif
[global _scalbn]
[global _scalbln]
[global _scalbnf]
[global _scalblnf]
[global _scalbnl]
[global _scalblnl]
SECTION data CLASS=DATA USE32
nm	db	"scalbn",0

SECTION code CLASS=CODE USE32
_scalbnf:
_scalblnf:
    lea     ecx,[esp+8]
    fild    dword [ecx]
    lea     edx,[esp+4]
    fld     dword [edx]
    sub dl,dl
    jmp short scalbn
_scalbn:
_scalbln:
_scalbnl:
_scalblnl:
    lea     ecx,[esp+12]
    fild    dword [ecx]
    lea     edx,[esp+4]
    fld     qword [edx]
    mov dl,1
scalbn:
    call    checkedscale
    jc      wrapmath
    fxch
    popone
    jmp     wrapmath
