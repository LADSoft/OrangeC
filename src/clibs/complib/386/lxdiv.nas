;Software License Agreement (BSD License)
;
;Copyright (c) 1997-2008, David Lindauer, (LADSoft).
;All rights reserved.
;
;Redistribution and use of this software in source and binary forms, with or without modification, are
;permitted provided that the following conditions are met:
;
;* Redistributions of source code must retain the above
;  copyright notice, this list of conditions and the
;  following disclaimer.
;
;* Redistributions in binary form must reproduce the above
;  copyright notice, this list of conditions and the
;  following disclaimer in the documentation and/or other
;  materials provided with the distribution.
;
;* Neither the name of LADSoft nor the names of its
;  contributors may be used to endorse or promote products
;  derived from this software without specific prior
;  written permission of LADSoft.
;
;THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
;WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
;PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
;ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
;LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
;INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
;TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
;ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
