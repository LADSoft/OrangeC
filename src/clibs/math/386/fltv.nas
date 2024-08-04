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
[export __tiny_ldbl]
[export __max_ldbl]
[export __max_dbl]
[export __max_flt]
%endif
[GLOBAL __tiny_ldbl]
[GLOBAL __max_ldbl]
[GLOBAL __max_dbl]
[GLOBAL __max_flt]

[SECTION data CLASS=DATA USE32]

__tiny_ldbl     db      000H, 000H, 000H, 000H, 000H, 000H, 000H, 080H, 001H, 000H
__max_ldbl     db      0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FEH, 07FH
__max_dbl      db      0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0EFH, 07FH
__max_flt      db      0FFH, 0FFH, 07FH, 07FH

