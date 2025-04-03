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
[export @__GetTypeInfo.qpvpv]
%endif
[global @__GetTypeInfo.qpvpv]


%ifdef __BUILDING_LSCRTL_DLL
        export @__GetTypeInfo.qpvpv
%endif
extern @std@type_info@.bdtr.qv

SECTION data CLASS=DATA USE32
@std@type_info_dtr:
    dd  0,0,0 ; pointer to xceptiontable goes here
    dd  @std@type_info@.bdtr.qv
    
SECTION code CLASS=CODE USE32
        global @__GetTypeInfo.qpvpv
@__GetTypeInfo.qpvpv:
        mov     eax,[ esp + 4] ; rv;
        mov     edx,[ esp + 8] ; tpid
        mov     dword [eax],@std@type_info_dtr + 12
        mov     dword [eax + 4], edx
        ret
