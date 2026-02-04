; This file is heavily inspired by/compiled from https://www.strchr.com/sse2_optimised_strlen
; The original code is public domain, this usage of it is, due to that basis, kept as public domain.
; It's compiled with CL with minor modifications such as explicitly laying out my no-ops
; These no-ops are taken from stack overflow on NASM no-ops https://stackoverflow.com/a/48254809
%define nop1 nop                                                     ; just a nop, included for completeness
%define nop2 db 0x66, 0x90                                           ; 66 NOP
%define nop3 db 0x0F, 0x1F, 0x00                                     ;    NOP DWORD ptr [EAX]
%define nop4 db 0x0F, 0x1F, 0x40, 0x00                               ;    NOP DWORD ptr [EAX + 00H]
%define nop5 db 0x0F, 0x1F, 0x44, 0x00, 0x00                         ;    NOP DWORD ptr [EAX + EAX*1 + 00H]
%define nop6 db 0x66, 0x0F, 0x1F, 0x44, 0x00, 0x00                   ; 66 NOP DWORD ptr [EAX + EAX*1 + 00H]
%define nop7 db 0x0F, 0x1F, 0x80, 0x00, 0x00, 0x00, 0x00             ;    NOP DWORD ptr [EAX + 00000000H]
%define nop8 db 0x0F, 0x1F, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00       ;    NOP DWORD ptr [EAX + EAX*1 + 00000000H]
%define nop9 db 0x66, 0x0F, 0x1F, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00 ; 66 NOP DWORD ptr [EAX + EAX*1 + 00000000H]
%ifdef __BUILDING_LSCRTL_DLL
[export _strlen]
%endif
[global _strlen]
SECTION code CLASS=CODE USE32
_strlen:
        mov     eax, [esp+4]
        xor     edx, edx
        test    al, 15
        je      SHORT hot_loop_prep
        nop6
alignment_loop:
        mov     cl, [eax]
        inc     eax
        test    cl, cl
        je      easy_exit
        inc     edx
        test    al, 15
        jne     alignment_loop
hot_loop_prep:
        movaps  xmm0, [eax]
        xorps   xmm1, xmm1
        pcmpeqb xmm0, xmm1
        pmovmskb ecx, xmm0
        test    ecx, ecx
        jne     sse_exit
        sub     eax, edx
hot_loop:
        movaps  xmm0, [eax+edx+16]
        add     edx, 16
        pcmpeqb xmm0, xmm1
        pmovmskb ecx, xmm0
        test    ecx, ecx
        je      SHORT hot_loop
sse_exit:
        bsf     eax, ecx
        add     eax, edx
        ret
easy_exit:
        mov     eax, edx
        ret
