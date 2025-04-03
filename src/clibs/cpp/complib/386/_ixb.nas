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
[export @_InitializeException.qpvpv]
[export @_RundownException.qv]
%endif
[global @_InitializeException.qpvpv]
[global @_RundownException.qv]

[extern ___cppexceptionhandle]

SECTION code CLASS=CODE USE32
@_InitializeException.qpvpv:
    push eax
    push ecx
    push edx
    mov eax,[esp+4 + 12] ; pointer to xception block
    mov ecx,[fs:0]      ; next pointer in chain
    mov [eax],ecx
    mov DWORD [eax+4], ___cppexceptionhandle
    lea ecx,[esp + 12 + 12] ; pointer to what ESP should be when a catch block is entered
    mov [eax + 8],ecx
    mov ecx,[esp+8 + 12]  ; function xcept block
    mov edx, eax
    sub edx, [ecx + 4] ; offset from start of xcept block to where EBP would be
    mov [eax+12], edx   ; pointer to ebp
    mov [eax+16], ecx   ; function xception block
    mov DWORD [eax + 20], 0 ; constructor progress
    mov [fs:0],eax
    pop edx
    pop ecx
    pop eax
    ret
@_RundownException.qv:
    mov ecx,[fs:0]      ; has to be ECX to avoid killing the function return value
    mov ecx,[ecx]
    mov [fs:0],ecx
    ret
    
