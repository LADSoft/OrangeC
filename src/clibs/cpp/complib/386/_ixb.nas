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
[export @_InitializeException$qpvpv]
[export @_RundownException$qv]
%endif
[global @_InitializeException$qpvpv]
[global @_RundownException$qv]

[extern ___cppexceptionhandle]

SECTION code CLASS=CODE USE32
@_InitializeException$qpvpv:
    mov eax,[esp+4] ; pointer to xception block
    mov ecx,[fs:0]      ; next pointer in chain
    mov [eax],ecx
    mov DWORD [eax+4], ___cppexceptionhandle
    lea ecx,[esp + 12] ; pointer to what ESP should be when a catch block is entered
    mov [eax + 8],ecx
    mov ecx,[esp+8]  ; function xcept block
    mov edx, eax
    sub edx, [ecx + 4] ; offset from start of xcept block to where EBP would be
    mov [eax+12], edx   ; pointer to ebp
    mov [eax+16], ecx   ; function xception block
    mov DWORD [eax + 20], 0 ; constructor progress
    mov DWORD [eax + 24], 0 ; throw flags
    mov [fs:0],eax
    ret
@_RundownException$qv:
    mov ecx,[fs:0]      ; has to be ECX to avoid killing the function return value
    mov ecx,[ecx]
    mov [fs:0],ecx
    ret
    