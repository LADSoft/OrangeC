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

%ifdef __BUILDING_LSCRTL_DLL
[export __ultof]
[export __ulltof]
[export __lltof]
%endif
[global __ultof]
[global __ulltof]
[global __lltof]
[extern __fdchsmask]
SECTION data CLASS=DATA USE32 ALIGN=8
    temp    dd 0,0;
SECTION code CLASS=CODE USE32
load:
    shr eax,cl
    and eax,edx
    cvtsi2sd xmm1, eax
    mov al,cl
    add al,ch
    cbw
    cwde
    add eax, 03ffh
    shl eax,20
    mov [temp+4],eax
    mulsd xmm1,[temp]
    ret
__ultof:
    mov eax,[esp+4]
    sub esp,8
    movsd [esp],xmm1
    push eax
    mov cx,0010h
    mov edx,0ffffh
    call load
    movsd xmm0, xmm1
    pop eax

    mov cl, 0
    call load
    addsd xmm0, xmm1
    movsd xmm1,[esp]
    add esp,8
    ret 4
__lltof:
    mov eax,[esp+4]
    mov edx,[esp+8]
    push edx
    or edx,edx
    jns noneg
    neg eax
    adc edx,0
    neg edx
noneg:
    push edx
    push eax
    call __ultof ; pops eax off the stack...
    pop eax
    pop edx
    sub esp,8
    movsd [esp],xmm1
    push edx
    push eax
    mov cx,2000h
    mov edx,0ffffh
    call load
    addsd xmm0, xmm1
    pop eax
    mov cl, 10h
    mov edx,07fffh
    call load
    addsd xmm0, xmm1
    pop edx
    or edx,edx
    jns nochs
       xorpd xmm0, [__fdchsmask]
nochs:
    movsd xmm1,[esp]
    add esp,8
    ret 8
__ulltof:
    push dword [esp+4]
    call __ultof
    mov eax,[esp+8]
    sub esp,8
    movsd [esp],xmm1
    push eax
    mov ecx,2000h
    mov edx, 0ffffh
    call load
    addsd xmm0, xmm1
    pop eax
    mov cl, 10h
    call load
    addsd xmm0, xmm1
    movsd xmm1,[esp]
    add esp,8
    ret 8