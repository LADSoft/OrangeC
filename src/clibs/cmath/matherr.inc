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

%define DOMAIN 1
%define SING 2
%define OVERFLOW 3
%define UNDERFLOW 4
%define TLOSS 5
%define STACKFAULT 7

[extern poptemp]
[extern large_t]
[extern wrapcomplex]
[extern trigdomain]
[extern clearmath]
[extern wrapmath]
[extern ___matherr]
[extern sinularerr]
[extern mnegerr]
[extern mnegeerr]
[extern domainerr]
[extern checkedscale]
[extern checkinvalid]
[extern overflow]
[extern invalid]

%macro popone 0
	fstp st0
%endmacro
%macro poptwo 0
	fstp st0
	fstp st0
%endmacro
