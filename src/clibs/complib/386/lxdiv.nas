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
[export __LXDIVS]
[export __LXDIVU]
[export __LXMODS]
[export __LXMODU]
%endif
[global __LXDIVS]
[global __LXDIVU]
[global __LXMODS]
[global __LXMODU]
SECTION code CLASS=CODE USE32
__LXDIVS:
                xor     ecx, ecx
                jmp     short join
__LXDIVU:
                mov     ecx, 1
                jmp     short join
__LXMODS:
                mov     ecx, 2
                jmp     short join
__LXMODU:

                mov     ecx, 3

join:
                push    ebp
                mov     ebp, esp
                push    esi
                push    edi
                push    ebx
                mov     edi, ecx
                mov     eax, [ebp+8]
                mov     edx, [ebp+12]
                mov     ebx, [ebp+16]
                mov     ecx, [ebp+20]
                or      ecx, ecx
        jnz	loc_1101A
                or      edx, edx
        jz	near loc_1107F
                or      ebx, ebx
        jz	near loc_1107F

loc_1101A:
                test    edi, 1
        jnz	loc_1103C
                or      edx, edx
        jns	loc_1102E
                neg     edx
                neg     eax
                sbb     edx, 0
                or      edi, 0Ch

loc_1102E:
                or      ecx, ecx
        jns	loc_1103C
                neg     ecx
                neg     ebx
                sbb     ecx, 0
                xor     edi, 4

loc_1103C: 
                mov     ebp, ecx
                mov     ecx, 40h ; ' '
                push    edi
                xor     edi, edi
                xor     esi, esi

loc_11046:
                shl     eax, 1
                rcl     edx, 1
                rcl     esi, 1
                rcl     edi, 1
                cmp     edi, ebp
        jb	loc_1105D
        ja	loc_11058
                cmp     esi, ebx
        jb	loc_1105D

loc_11058:
                sub     esi, ebx
                sbb     edi, ebp
                inc     eax

loc_1105D:
        loop	loc_11046
                pop     ebx
                test    ebx, 2
        jz	loc_1106C
                mov     eax, esi
                mov     edx, edi
                shr     ebx, 1

loc_1106C: 
                test    ebx, 4
        jz	loc_11079
                neg     edx
                neg     eax
                sbb     edx, 0

loc_11079: 
                pop     ebx
                pop     edi
                pop     esi
                pop     ebp
                ret     16
loc_1107F: 
                div     ebx
                test    edi, 2
        jz	loc_11088
                xchg    eax, edx

loc_11088: 
                xor     edx, edx
        jmp	short loc_11079
