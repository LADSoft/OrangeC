; Software License Agreement
; 
;     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
; 
;     This file is part of the Orange C Compiler package.
; 
;     The Orange C Compiler package is free software: you can redistribute it and/or modify
;     it under the terms of the GNU General Public License as published by
;     the Free Software Foundation, either version 3 of the License, or
;     (at your option) any later version, with the addition of the 
;     Orange C "Target Code" exception.
; 
;     The Orange C Compiler package is distributed in the hope that it will be useful,
;     but WITHOUT ANY WARRANTY; without even the implied warranty of
;     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;     GNU General Public License for more details.
; 
;     You should have received a copy of the GNU General Public License
;     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
; 
;     contact information:
;         email: TouchStone222@runbox.com <David Lindauer>
; 

%ifdef __ftoBUILDING_LSCRTL_DLL
[export __ftoul]
[export __ftoull]
[export __ftoll]
%endif
[global __ftoul]
[global __ftoull]
[global __ftoll]
[extern __fdchsmask]
[extern __fzero]
SECTION data CLASS=DATA USE32 ALIGN=8
divmap  dd  0,42f00000h
        dd  0,41f00000h
        dd  0,40f00000h
SECTION code CLASS=CODE USE32
unload:
        movsd   xmm1,xmm0
        divsd xmm0, [divmap + edx*8]
        cvttsd2si eax,xmm0
        cvtsi2sd xmm0,eax

        mulsd xmm0, [divmap + edx*8]
        subsd xmm1, xmm0
        movsd xmm0,xmm1
        ret

__ftoul:
        sub esp,8
        movsd [esp], xmm1
        mov edx,2
        call unload
        shl eax,16
        cvtsd2si edx, xmm0
        or eax,edx
        mov edx,ecx
        movsd xmm1,[esp]
        add esp,8
        ret
__ftoll:
        sub esp,8
        movsd [esp], xmm1
        ucomisd xmm0,[__fzero]
        mov ecx, 0
        ja noneg
        inc ecx
        xorps xmm0,[__fdchsmask]
noneg:
        push ecx
        sub edx,edx
        call unload
        mov ecx,eax
        shl ecx,16
        inc edx
        call unload
        or ecx,eax
        inc edx
        call unload
        shl eax,16
        cvtsd2si edx, xmm0
        or eax,edx
        mov edx,ecx
        pop ecx
        or  ecx,ecx
        jz nochs
        neg eax
        adc edx, 0
        neg edx

nochs:
        movsd xmm1,[esp]
        add esp,8
        ret
__ftoull:
        sub esp,8
        movsd [esp], xmm1
        sub edx,edx
        call unload
        mov ecx,eax
        shl ecx,16
        inc edx
        call unload
        or ecx,eax
        inc edx
        call unload
        shl eax,16
        cvtsd2si edx, xmm0
        or eax,edx
        mov edx,ecx
        movsd xmm1,[esp]
        add esp,8
        ret