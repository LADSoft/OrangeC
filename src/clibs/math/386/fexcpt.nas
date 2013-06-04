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
[export _feclearexcept]
[export _fegetexceptflag]
[export _feraiseexcept]
[export _fesetexceptflag]
[export _fetestexcept]
%endif
[global _feclearexcept]
[global _fegetexceptflag]
[global _feraiseexcept]
[global _fesetexceptflag]
[global _fetestexcept]
[global feflagshold]
SECTION data CLASS=DATA USE32

feflagshold dw    0
section code CLASS=CODE USE32

_feclearexcept:
    mov ax,[esp+4]
    not ax
    or ax,0ff00h
    sub esp,28
    fnstenv [esp]
    and word [esp+4],ax
    fldenv [esp]
    add esp,28
    and [feflagshold],ax
    sub eax,eax
    ret
_fegetexceptflag:
    fnstsw ax
    and ax, 0ffh
    and ax, word [esp+ 8]
    or  ax,[feflagshold]
    mov ecx, [esp+4]
    mov word [ecx],ax
    sub eax,eax
    ret
_fesetexceptflag:
    mov ax,[esp+8]
    mov ecx,[esp+4]
    and ax,[ecx]
    and ax,0ffh
    mov [feflagshold],ax
    sub eax,eax
    ret
_feraiseexcept:
    mov eax,[esp+4]
    and ax,0ffh
    sub esp,28
    fnstenv [esp]
    or word [esp+4],ax
    fldenv [esp]
    fwait
    add esp,28
    sub eax,eax
    ret
_fetestexcept:
    fstsw ax
    and ax,[esp+4]
    and eax,0ffh
    or ax,[feflagshold]
    ret
