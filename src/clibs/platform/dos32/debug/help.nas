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

%include  "version.asi"
%include  "prints.ase" 
%include  "mtrap.ase" 
%include  "input.ase"

    global  Help

    segment code USE32
;
; help command entry point
;
Help:
    call	Message
    db	13,10,"Debug/386 Version "
    db	30h + VERID/10,'.',30h + VERID % 10,9,9
    db	"Copyright (c) LADsoft",13,10
    db	"a  [addr]                      - assemble",10,13
    db	"b  [d][#]                      - Show breakpoint(s)",13,10
    db	"b  [d][-]#,addr[,r/w/x[,len]]  - set or clear a hdwe breakpoint",13,10
    db	"b  [-]#,addr                   - set or clear a breakpoint",13,10
    db	"c  start,end,dest              - compare memory",10,13
    db	"d  [bwd]  [start [,end]]       - dump memory",13,10
    db	"e  [bwd]  start [,list]        - examine memory",13,10
    db	"f  start,end [,val]            - fill memory",13,10
    db	"g  [=start] [,break]           - run from start (or pos) to break",13,10
    db	"h  val1 [+-*/] val2            - hex arithmetic",13,10
    db	"i  port                        - read from port",13,10
    db	"m  start,end,dest              - move memory",13,10
    db	"o  port,val                    - write to port",13,10
    db	"p  [r][count]                  - step over/out",13,10
    db	"q                              - quit",13,10
    db	"r  [reg[:val]]                 - show/modify regs",13,10
    db  "rf                             - show/modify flags",13,10
    db	"rm [m#]                        - show/modify mmx regs",13,10
    db	"rn [s][# val]                  - show/modify fp regs/status",13,10
    db	"s  start,end [,list]           - search for a byte pattern",13,10
    db	"t  [count]                     - step into",13,10
    db	"u  [start [,end]]              - unassemble",13,10
    db  "/	instruction                 - execute an instruction", 13, 10
    db	"?                              - this help"
    db	0
    clc
    ret	
