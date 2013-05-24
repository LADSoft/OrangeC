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
