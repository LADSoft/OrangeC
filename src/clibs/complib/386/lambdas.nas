;  Software License Agreement
;  
;      Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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

%ifdef __BUILDING_LSCRTL_DLL
[export @__lambdaCallS.qpv]
[export @__lambdaPtrCallS.qpvpv]
%endif
[global @__lambdaCallS.qpv]
[global @__lambdaPtrCallS.qpvpv]

; frame in
;    args
;    this
;    struct ptr ; discarded on return
;    rv1
;    function ; discarded on return
;    rv2
;
; frame out
;     args
;     rv2
;     rv1
;     this
;    struct ptr
SECTION code CLASS=CODE USE32



@__lambdaCallS.qpv:
    pop ecx
    xchg ecx,[esp + 12]
    xchg ecx,[esp+4]
    xchg ecx,[esp+8]
    xchg ecx,[esp]
    call ecx
    mov ecx,[esp]
    xchg ecx,[esp + 8];
    xchg ecx,[esp + 4]
    xchg ecx,[esp + 12]
    jmp ecx

; frame in
;    args
;    struct ptr
;    rv1
;    function ; discarded on return
;    this
;    rv2
;
; frame out
;     args
;     rv1
;     rv2
;     this
;    struct ptr
@__lambdaPtrCallS.qpvpv:
    pop ecx
    xchg ecx,[esp + 12]
    xchg ecx,[esp + 0]
    xchg ecx,[esp + 4]
    call ecx
    xchg ecx,[esp + 4]
    xchg ecx,[esp + 0]
    xchg ecx,[esp + 12]
    jmp ecx
    