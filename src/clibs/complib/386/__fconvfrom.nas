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
[export __ftoi]
[export __ftoull]
[export __ftoll]
%endif
[global __ftoi]
[global __ftoull]
[global __ftoll]
[extern __fdchsmask]
[extern __fzero]
[extern _fesetexceptflag]
SECTION data CLASS=DATA USE32 ALIGN=8
ranges  dq  0.0, 255.0
        dq  -128.0, 128.0
        dq  0.0, 65536.0
        dq  -32768.0, 32767.0
        dq  0.0,4294967296.0
        dq  -2147483648.0,2147483648.0
        dq  0.0,18446744073709551616.0
        dq  -9223372036854775808.0, 9223372036854775808.0
divmap  dd  0,42f00000h
        dd  0,41f00000h
        dd  0,40f00000h
temp    dd  1 ; //FE_INVALID
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

__ftoi:
        mov ecx,[esp +12]
        movsd xmm0,[esp+4]
        call checkrange
        jc reti
        sub esp,8
        movsd [esp], xmm1
        mov edx,2
        call unload
        shl eax,16
        cvttsd2si edx, xmm0
        or eax,edx
        mov edx,ecx
        movsd xmm1,[esp]
        add esp,8
reti:
        ret 12
__ftoll:
        mov ecx,7
        movsd xmm0,[esp+4]
        call checkrange      
        jc retll
        sub esp,8
        movsd [esp], xmm1
        ucomisd xmm0,[__fzero]
        mov ecx, 0
        jae noneg
        inc ecx
        xorpd xmm0,[__fdchsmask]
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
        cvttsd2si edx, xmm0
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
retll:
        ret 8
__ftoull:
        mov ecx,6
        movsd xmm0,[esp+4]
        call checkrange      
        jc retull

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
        cvttsd2si edx, xmm0
        or eax,edx
        mov edx,ecx
        movsd xmm1,[esp]
        add esp,8
retull:
        ret 8
        
checkrange:
    add ecx,ecx
    ucomisd xmm0, [ranges + ecx * 8];
    jc err
    ucomisd xmm0, [ranges + 8 + ecx * 8]
    jb ok
err:
    push 1                      ; FE_INVALID
    push dword temp
    call _fesetexceptflag
    pop ecx
    pop ecx
    sub eax,eax
    sub edx,edx
ok:
    cmc
    ret