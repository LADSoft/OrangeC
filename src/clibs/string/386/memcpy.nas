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
[export _memcpy]
[export _memmove]
%endif
[global _memcpy]
[global _memmove]
SECTION code CLASS=CODE USE32
; memcpy_basic
_memcpy_basic:
; Line 2: inline void* memcpy_basic(void* dest, const void* src, size_t n) 
        push    ebx
        push    esi
        push    edi
        mov     esi,dword [esp+0ch+0ch]
        mov     edx,dword [esp+08h+0ch]
        mov     eax,dword [esp+04h+0ch]
; Line 4:     for (size_t i = 0; i < n; i++) 
        xor     ebx,ebx
        cmp     ebx,esi
        jnc     L_6
L_4:
; Line 5:     { 
; Line 6:         ((char*)dest)[i] = ((const char*)src)[i]; 
        mov     cl,byte [edx+ebx]
        mov     byte [eax+ebx],cl
; Line 7:     } 
        inc     ebx
L_5:
        cmp     ebx,esi
        jc      L_4
L_6:
; Line 8:     return dest; 
; Line 9: } 
        pop     edi
        pop     esi
        pop     ebx
        ret

; memcpy_repmovs
_memcpy_repmovs:
; Line 39: inline void* memcpy_repmovs(void* s1, const void* s2, size_t sz) 
        cmp     dword [esp+0ch],0100h
        jl      _memcpy_basic
        push    esi
        push    edi
; Line 48:     __asm 
; Line 49:     { 
        mov     esi,dword [esp+08h+08h]
; Line 50:         mov esi, [s2]; 
; Line 51:         mov edi, [s1]; 
        mov     edi,dword [esp+04h+08h]
; Line 52:         mov ecx, [sz]; 
        mov     ecx,dword [esp+0ch+08h]
; Line 53:         cld; 
        cld
; Line 54:         rep movsb; 
        rep movsb
; Line 55:     } 
; Line 56:     return s1; 
        mov     eax,dword [esp+04h+08h]
; Line 57: } 
        pop     edi
        pop     esi
        ret
; memcpy
_memmove:
_memcpy:
; Line 59: void* _RTL_FUNC memcpy(void* dest, const void* src, size_t sz) 
        sub     esp,byte 014h
;       dest location: 04h
;       src location:  08h
;       size location: 0ch
;       Stack size: 4 * 4 + 14h = 24h
        push    ebx
        push    ebp
        push    esi
        push    edi
L_78:
        mov     ebx,dword [esp+0ch+024h]
        mov     eax,dword [esp+08h+024h]
        mov     dword [esp-010h+024h],eax
        mov     edi,dword [esp+04h+024h]
; Line 62:     if (((uintptr_t)src) < ((uintptr_t)dest) && ((uintptr_t)dest) < (((uintptr_t)src) + sz)) 
        mov     eax,edi
        mov     esi,edi
        mov     edx,dword [esp-010h+024h]
        mov     ecx,dword [esp-010h+024h]
        cmp     ecx,esi
        jnc     L_81
        mov     ecx,edx
        add     ecx,ebx
        mov     dword [esp-014h+024h],ecx
        mov     ebp,eax
        mov     esi,ebx
        cmp     ebp,dword [esp-014h+024h]
        jnc     L_81
; Line 63:     { 
; Line 64:         size_t distance_beyond = ((uintptr_t)src) + sz - ((uintptr_t)dest); 
        add     edx,esi
        mov     ecx,eax
        sub     ecx,edx
        neg     ecx
; Line 65:         size_t inverse_distance_beyond = sz - distance_beyond; 
        sub     esi,ecx
; Line 66:         memmove_backwards_basic((char*)dest + inverse_distance_beyond, dest, distance_beyond); 
        mov     ebx,eax
        add     ebx,esi
; Line 12:     while (sz > 0) 
L_100:
        cmp     ecx,byte 00h
        jbe     L_97
L_96:
; Line 13:     { 
; Line 14:         sz--; 
        dec     ecx
        mov     dl,byte [eax+ecx]
        mov     byte [ebx+ecx],dl
; Line 15:         ((char*)s1)[sz] = ((const char*)s2)[sz]; 
L_98:
        cmp     ecx,byte 00h
        ja      L_96
L_97:
; Line 16:     } 
; Line 17:     return s1; 
; Line 18: } 
; Line 68:         return memcpy_repmovs(dest, src, inverse_distance_beyond); 
        push    esi
        push    dword [esp-010h+028h]
        push    edi
        call    _memcpy_repmovs ; memcpy_repmovs
        add     esp,byte 0ch
        jmp     L_79
; Line 69:     } 
L_81:
; Line 70:     else 
; Line 71:     { 
; Line 74:         return memcpy_repmovs(dest, src, sz); 
        push    ebx
        push    dword [esp-010h+028h]
        push    edi
        call    _memcpy_repmovs ; memcpy_repmovs
        add     esp,byte 0ch
; Line 75:     } 
L_86:
L_79:
        pop     edi
        pop     esi
        pop     ebp
        pop     ebx
        add     esp,byte 014h
        ret
