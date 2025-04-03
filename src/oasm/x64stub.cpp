/* Software License Agreement
 *
 *     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 *
 *
 */

#include "Instruction.h"
#include "InstructionParser.h"
#include "x64Operand.h"
#include "x64Parser.h"
#include "x64Instructions.h"
#include "AsmFile.h"
#include "Section.h"
#include "Fixup.h"
#include "AsmLexer.h"
#include <fstream>
#include <iostream>
#include "Errors.h"

std::set<std::string> InstructionParser::attPotentialExterns;

const char* Lexer::preDataIntel =
    "%define __SECT__\n"
    "%imacro	pdata1	0+ .native\n"
    "	[db %1]\n"
    "%endmacro\n"
    "%imacro	.word	0+ .native\n"
    "	[dw %1]\n"
    "%endmacro\n"
    "%imacro	pdata2	0+ .native\n"
    "	[dw %1]\n"
    "%endmacro\n"
    "%imacro	pdata4	0+ .native\n"
    "	[dd %1]\n"
    "%endmacro\n"
    "%imacro	.long	0+ .native\n"
    "	[dd %1]\n"
    "%endmacro\n"
    "%imacro	pdata8	0+ .native\n"
    "	[dq %1]\n"
    "%endmacro\n"
    "%imacro	.byte	0+ .native\n"
    "	[db %1]\n"
    "%endmacro\n"
    "%imacro	db	0+ .native\n"
    "	[db %1]\n"
    "%endmacro\n"
    "%imacro	resb 0+ .native\n"
    "	[resb %1]\n"
    "%endmacro\n"
    "%imacro	dw	0+ .native\n"
    "	[dw %1]\n"
    "%endmacro\n"
    "%imacro	resw 0+ .native\n"
    "	[resw %1]\n"
    "%endmacro\n"
    "%imacro	dd	0+ .native\n"
    "	[dd %1]\n"
    "%endmacro\n"
    "%imacro	resd 0+ .native\n"
    "	[resd %1]\n"
    "%endmacro\n"
    "%imacro	dq	0+ .native\n"
    "	[dq %1]\n"
    "%endmacro\n"
    "%imacro	resq 0+ .native\n"
    "	[resq %1]\n"
    "%endmacro\n"
    "%imacro	dt	0+ .native\n"
    "	[dt %1]\n"
    "%endmacro\n"
    "%imacro	rest 0+ .native\n"
    "	[rest %1]\n"
    "%endmacro\n"
    "%imacro	section 0+ .nolist\n"
    "%define __SECT__ [section %1]\n"
    "__SECT__\n"
    "%endmacro\n"
    "%imacro	segment 0+ .nolist\n"
    "%define __SECT__ [section %1]\n"
    "__SECT__\n"
    "%endmacro\n"
    "%imacro	absolute 0+ .nolist\n"
    "%define __SECT__ [absolute %1]\n"
    "__SECT__\n"
    "%endmacro\n"
    "%imacro	.code 0 .nolist\n"
    "%define __SECT__ [section code]\n"
    "__SECT__\n"
    "%endmacro\n"
    "%imacro	.pdata 0 .nolist\n"
    "%define __SECT__ [section pdata]\n"
    "__SECT__\n"
    "%endmacro\n"
    "%imacro	.bss 0 .nolist\n"
    "%define __SECT__ [section bss]\n"
    "__SECT__\n"
    "%endmacro\n"
    "%imacro	align 0+ .nolist\n"
    "	[align %1]\n"
    "%endmacro\n"
    "%imacro	.align 0+ .nolist\n"
    "	[galign %1]\n"
    "%endmacro\n"
    "%imacro	import 0+ .nolist\n"
    "	[import %1]\n"
    "%endmacro\n"
    "%imacro	export 0+ .nolist\n"
    "	[export %1]\n"
    "%endmacro\n"
    "%imacro	global 0+ .nolist\n"
    "	[global %1]\n"
    "%endmacro\n"
    "%imacro	.globl 0+ .nolist\n"
    "	[global %1]\n"
    "%endmacro\n"
    "%imacro	extern 0+ .nolist\n"
    "	[extern %1]\n"
    "%endmacro\n"
    "%imacro equ 0+ .native\n"
    "	[equ	%1]\n"
    "%endmacro\n"
    "%imacro incbin 0+ .nolist\n"
    "	[incbin	%1]\n"
    "%endmacro\n"
    "%imacro times 0+ .native\n"
    "	[times %1]\n"
    "%endmacro\n"
    "%imacro struc 1 .nolist\n"
    "%push struc\n"
    "%define %$name %1\n"
    "[absolute 0]\n"
    "%{$name}:\n"
    "%endmacro\n"
    "%imacro endstruc 0 .nolist\n"
    "%{$name}_SIZE EQU $-%$name\n"
    "%pop\n"
    "__SECT__\n"
    "%endmacro\n";

const char* Lexer::preDataGas =
    "%define __SECT__\n"
    "%macro	.comm	2 .native\n"
    "%define __SECT__ [section %1 virtual]\n"
    "__SECT__\n"
    "    [resb %2]\n"
    "%endmacro\n"
    "%macro	.lcomm	2 .native\n"
    "%define __BSS_SECT__ [section bss]\n"
    "__BSS_SECT__\n"
    "%define __SECT__ [section %1 virtual]\n"
    "__SECT__\n"
    "    [resb %2]\n"
    "%endmacro\n"
    "%macro	.word	0+ .native\n"
    "	[dw %1]\n"
    "%endmacro\n"
    "%macro	.byte	0+ .native\n"
    "	[db %1]\n"
    "%endmacro\n"
    "%macro	.2byte	0+ .native\n"
    "	[dw %1]\n"
    "%endmacro\n"
    "%macro	.hword	0+ .native\n"
    "	[dw %1]\n"
    "%endmacro\n"
    "%macro	.short	0+ .native\n"
    "	[dw %1]\n"
    "%endmacro\n"
    "%macro	.4byte	0+ .native\n"
    "	[dd %1]\n"
    "%endmacro\n"
    "%macro	.int	0+ .native\n"
    "	[dd %1]\n"
    "%endmacro\n"
    "%macro	.long	0+ .native\n"
    "	[dd %1]\n"
    "%endmacro\n"
    "%macro	.8byte	0+ .native\n"
    "	[dq %1]\n"
    "%endmacro\n"
    "%macro	.quad	0+ .native\n"
    "	[dq %1]\n"
    "%endmacro\n"
    "%macro	.ascii	0+ .native\n"
    "	[string 1,%1]\n"
    "%endmacro\n"
    "%macro	.string	0+ .native\n"
    "	[string 1,%1]\n"
    "   [db 0]\n"
    "%endmacro\n"
    "%macro	.string8 0+ .native\n"
    "	[string 1,%1]\n"
    "   [db 0]\n"
    "%endmacro\n"
    "%macro	.string16 0+ .native\n"
    "	[string 2,%1]\n"
    "   [dw 0]\n"
    "%endmacro\n"
    "%macro	.string32 0+ .native\n"
    "	[string 4,%1]\n"
    "   [dd 0]\n"
    "%endmacro\n"
    "%macro	.balign	0+ .native\n"
    "	[balign 1,%1]\n"
    "%endmacro\n"
    "%macro	.balignw 0+ .native\n"
    "	[balign 2,%1]\n"
    "%endmacro\n"
    "%macro	.balignl 0+ .native\n"
    "	[balign 4,%1]\n"
    "%endmacro\n"
    "%macro	.p2align 0+ .native\n"
    "	[p2align 1,%1]\n"
    "%endmacro\n"
    "%macro	.p2alignw 0+ .native\n"
    "	[p2align 2,%1]\n"
    "%endmacro\n"
    "%macro	.p2alignl 0+ .native\n"
    "	[p2align 4,%1]\n"
    "%endmacro\n"
    "%macro	.float 0+ .native\n"
    "	[single %1]\n"
    "%endmacro\n"
    "%macro	.single 0+ .native\n"
    "	[single %1]\n"
    "%endmacro\n"
    "%macro	.double 0+ .native\n"
    "	[double %1]\n"
    "%endmacro\n"
    "%macro	.asciz	0+ .native\n"
    "	[string 1, %1]\n"
    "   [db 0]\n"
    "%endmacro\n"
    "%macro .equ 0+ .native\n"
    "	[set	%1]\n"
    "%endmacro\n"
    "%macro .set 0+ .native\n"
    "	[set	%1]\n"
    "%endmacro\n"
    "%macro .equiv 0+ .native\n"
    "	[set	%1]\n"
    "%endmacro\n"
    "%macro .eqv 0+ .native\n"
    "   [eqv  %1]\n"
    "%endmacro\n"
    "%macro	.extern 0+ .nolist\n"
    "	[extern %1]\n"
    "%endmacro\n"
    "%macro .incbin 0+ .nolist\n"
    "	[incbin	%1]\n"
    "%endmacro\n"
    "%macro	.section 0+ .nolist\n"
    "%define __SECT__ [gsection %1]\n"
    "__SECT__\n"
    "%endmacro\n"
    "%macro	.subsection 1 .nolist\n"
    "    [subsection %1]\n"
    "%endmacro\n"
    "%macro	.text 0-1 .nolist\n"
    "%define __SECT__ [text %1]\n"
    "__SECT__\n"
    "%endmacro\n"
    "%macro	.data 0-1 .nolist\n"
    "%define __SECT__ [data %1]\n"
    "__SECT__\n"
    "%endmacro\n"
    "%macro	.pushsection 0+ .nolist\n"
    "    [pushsection %1]\n"
    "%endmacro\n"
    "%macro	.popsection 0 .nolist\n"
    "    [popsection]\n"
    "%endmacro\n"
    "%macro	.previous 0 .nolist\n"
    "    [previous]\n"
    "%endmacro\n"

    "%macro	.align 0+ .nolist\n"
    "	[galign 1, %1]\n"
    "%endmacro\n"
    "%macro	.globl 0+ .nolist\n"
    "	[global %1]\n"
    "%endmacro\n"
    "%macro .abort 0 .native\n"
    "	[abort]\n"
    "%endmacro\n"
    "%macro .ABORT 0 .native\n"
    "	[abort]\n"
    "%endmacro\n"
    "%macro .err 0 .native\n"
    "	[error]\n"
    "%endmacro\n"
    "%macro .error 0+ .native\n"
    "	[error %1]\n"
    "%endmacro\n"
    "%macro .warning 0+ .native\n"
    "	[warning %1]\n"
    "%endmacro\n"
    "%macro .fail 0+ .native\n"
    "	[fail %1]\n"
    "%endmacro\n"
    "%macro .include 1\n"
    "    %include %1\n"
    "%endmacro\n"
    "%macro .if 1 .native\n"
    "    %if %1\n"
    "%endmacro\n"
    "%macro .ifdef 0+ .native\n"
    "    [unknowndirective \".ifdef\"]\n"
    "%endmacro\n"
    "%macro .else 0 .native\n"
    "    %else\n"
    "%endmacro\n"
    "%macro .endif 0 .native\n"
    "    %endif\n"
    "%endmacro\n"
    "%macro .skip 0+ .native\n"
    "    [space %1]\n"
    "%endmacro\n"
    "%macro .space 0+ .native\n"
    "    [space %1]\n"
    "%endmacro\n"
    "%macro .zero 1 .native\n"
    "    [space %1]\n"
    "%endmacro\n"
    "%macro .nops 0+ .native\n"
    "    [nops %1]\n"
    "%endmacro\n"
    "%macro .fill 0+ .native\n"
    "    [fill %1]\n"
    "%endmacro\n"
    "%macro .rept 1 .native\n"
    "    %rep %1\n"
    "%endmacro\n"
    "%macro .endr 0 .native\n"
    "    %endrep\n"
    "%endmacro\n"
    "%macro	.dc.a	0+ .native\n"
    "	[dd %1]\n"
    "%endmacro\n"
    "%macro	.dc.b	0+ .native\n"
    "	[db %1]\n"
    "%endmacro\n"
    "%macro	.dc.d	0+ .native\n"
    "	[double %1]\n"
    "%endmacro\n"
    "%macro	.dc.l	0+ .native\n"
    "	[dd %1]\n"
    "%endmacro\n"
    "%macro	.dc.s	0+ .native\n"
    "	[single %1]\n"
    "%endmacro\n"
    "%macro	.dc.w	0+ .native\n"
    "	[dw %1]\n"
    "%endmacro\n"
    "%macro	.dc.x	0+ .native\n"
    "	[dt %1]\n"
    "%endmacro\n"
    "%macro	.dc	0+ .native\n"
    "	[dw %1]\n"
    "%endmacro\n"
    "%macro	.dcb.a	1-2 0\n"
    "   %rep %1\n"
    "	[dd %2]\n"
    "   %endrep\n"
    "%endmacro\n"
    "%macro	.dcb.b	1-2 0\n"
    "   %rep %1\n"
    "	[db %2]\n"
    "   %endrep\n"
    "%endmacro\n"
    "%macro	.dcb.d	1-2 0\n"
    "   %rep %1\n"
    "	[double %2]\n"
    "   %endrep\n"
    "%endmacro\n"
    "%macro	.dcb.l	1-2 0\n"
    "   %rep %1\n"
    "	[dd %2]\n"
    "   %endrep\n"
    "%endmacro\n"
    "%macro	.dcb.s	1-2 0\n"
    "   %rep %1\n"
    "	[single %2]\n"
    "   %endrep\n"
    "%endmacro\n"
    "%macro	.dcb.w	1-2 0\n"
    "   %rep %1\n"
    "	[dw %2]\n"
    "   %endrep\n"
    "%endmacro\n"
    "%macro	.dcb.x	1-2 0\n"
    "   %rep %1\n"
    "	[dt %2]\n"
    "   %endrep\n"
    "%endmacro\n"
    "%macro	.dcb	1-2 0\n"
    "   %rep %1\n"
    "	[dw %2]\n"
    "   %endrep\n"
    "%endmacro\n"
    "%macro	.ds.a	1-2 0\n"
    "   %rep %1\n"
    "	[dd %2]\n"
    "   %endrep\n"
    "%endmacro\n"
    "%macro	.ds.b	1-2 0\n"
    "   %rep %1\n"
    "	[db %2]\n"
    "   %endrep\n"
    "%endmacro\n"
    "%macro	.ds.d	1-2 0\n"
    "   %rep %1\n"
    "	[dq %2]\n"
    "   %endrep\n"
    "%endmacro\n"
    "%macro	.ds.l	1-2 0\n"
    "   %rep %1\n"
    "	[dd %2]\n"
    "   %endrep\n"
    "%endmacro\n"
    "%macro	.ds.p	1-2 0\n"
    "   %rep %1\n"
    "	[dq %2]\n"
    "   [dd 0]\n"
    "   %endrep\n"
    "%endmacro\n"
    "%macro	.ds.s	1-2 0\n"
    "   %rep %1\n"
    "	[dd %2]\n"
    "   %endrep\n"
    "%endmacro\n"
    "%macro	.ds.w	1-2 0\n"
    "   %rep %1\n"
    "	[dw %2]\n"
    "   %endrep\n"
    "%endmacro\n"
    "%macro	.ds.x	1-2 0\n"
    "   %rep %1\n"
    "	[dq %2]\n"
    "   [dd 0]\n"
    "   %endrep\n"
    "%endmacro\n"
    "%macro	.ds	1-2 0\n"
    "   %rep %1\n"
    "	[dw %2]\n"
    "   %endrep\n"
    "%endmacro\n"
    "%macro .def 0+ .nolist\n"
    "%if 0\n"
    "%endmacro\n"
    "%macro .endef 0+ .nolist\n"
    "%endif\n"
    "%endmacro\n"
    "%macro .struc 1 .nolist\n"
    "[absolute %1]\n"
    "%endmacro\n"
    "%macro .offset 1 .nolist\n"
    "[absolute %1]\n"
    "%endmacro\n"
    "%macro    .eject 0 .nolist\n"
    "    [eject]\n"
    "%endmacro\n"
    "%macro    .print 1 .nolist\n"
    "    [print %1]\n"
    "%endmacro\n"
    "%macro    .type 0+ .native\n"
    "%endmacro\n"
    "%macro .func 0+ .native\n"
    "%endmacro\n"
    "%macro .endfunc 0 .native\n"
    "%endmacro\n"
    "%macro .weak 0+ .nolist\n"
    "    [unknowndirective \".weak\"]\n"
    "%endmacro\n"
    "%macro .weakref 0+ .nolist\n"
    "    [unknowndirective \".weakref\"]\n"
    "%endmacro\n"
    "%macro .title 0+ .nolist\n"
    "%endmacro\n"
    "%macro .sbttl 0+ .nolist\n"
    "%endmacro\n"
    "%macro .file 0+ .nolist\n"
    "%endmacro\n"
    "%macro .line 0+ .nolist\n"
    "%endmacro\n"
    "%macro .ln 0+ .nolist\n"
    "%endmacro\n"
    "%macro .loc 0+ .nolist\n"
    "%endmacro\n"
    "%macro .version 0+ .nolist\n"
    "    [unknowndirective \".version\"]\n"
    "%endmacro\n"
    "%macro .symver 0+ .nolist\n"
    "%endmacro\n"
    "%macro .tag 0+ .nolist\n"
    "%endmacro\n"
    "%macro .macro 0+ .nolist\n"
    "    [unknowndirective \".macro\"]\n"
    "%endmacro\n"
    "%macro .endm 0+ .nolist\n"
    "    [unknowndirective \".endm\"]\n"
    "%endmacro\n"
    "%macro .purgem 0+ .nolist\n"
    "    [unknowndirective \".purgem\"]\n"
    "%endmacro\n"
    "%macro .local 0+ .nolist\n"
    "    [unknowndirective \".local\"]\n"
    "%endmacro\n"
    "%macro .altmacro 0+ .nolist\n"
    "    [unknowndirective \".altmacro\"]\n"
    "%endmacro\n"
    "%macro .noaltmacro 0+ .nolist\n"
    "    [unknowndirective \".noaltmacro\"]\n"
    "%endmacro\n"
    "%macro .exitm 0+ .nolist\n"
    "    [unknowndirective \".exitm\"]\n"
    "%endmacro\n"
    "%macro .irp 0+ .nolist\n"
    "    [unknowndirective \".irp\"]\n"
    "%endmacro\n"
    "%macro .irpc 0+ .nolist\n"
    "    [unknowndirective \".irpc\"]\n"
    "%endmacro\n"
    "%macro .bundle_align_mode 0+ .nolist\n"
    "    [unknowndirective \".bundle_align_mode\"]\n"
    "%endmacro\n"
    "%macro .bundle_lock 0+ .nolist\n"
    "    [unknowndirective \".bundle_lock\"]\n"
    "%endmacro\n"
    "%macro .bundle_unlock 0+ .nolist\n"
    "    [unknowndirective \".bundle_unlock\"]\n"
    "%endmacro\n"
    "%macro .vtable_entry 0+ .nolist\n"
    "    [unknowndirective \".vtable_entry\"]\n"
    "%endmacro\n"
    "%macro .vtable_inherit 0+ .nolist\n"
    "    [unknowndirective \".vtable_inherit\"]\n"
    "%endmacro\n"
    "%macro .cfi_sections 0+ .nolist\n"
    "    [unknowndirective \".cfi_Section::sections\"]\n"
    "%endmacro\n"
    "%macro .cfi_startproc 0+ .nolist\n"
    "    [unknowndirective \".cfi_startproc\"]\n"
    "%endmacro\n"
    "%macro .cfi_endproc 0+ .nolist\n"
    "    [unknowndirective \".cfi_endproc\"]\n"
    "%endmacro\n"
    "%macro .cfi_personality 0+ .nolist\n"
    "    [unknowndirective \".cfi_personality\"]\n"
    "%endmacro\n"
    "%macro .cfi_personality_id 0+ .nolist\n"
    "    [unknowndirective \".cfi_personality_id\"]\n"
    "%endmacro\n"
    "%macro .cfi_fde_data 0+ .nolist\n"
    "    [unknowndirective \".cfi_fde_data\"]\n"
    "%endmacro\n"
    "%macro .cfi_lsda 0+ .nolist\n"
    "    [unknowndirective \".cfi_lsda\"]\n"
    "%endmacro\n"
    "%macro .cfi_inline_lsda 0+ .nolist\n"
    "    [unknowndirective \".cfi_inline_lsda\"]\n"
    "%endmacro\n"
    "%macro .cfi_def_cfa 0+ .nolist\n"
    "    [unknowndirective \".cfi_def_cfa\"]\n"
    "%endmacro\n"
    "%macro .cfi_def_cfa_register 0+ .nolist\n"
    "    [unknowndirective \".cfi_def_cfa_register\"]\n"
    "%endmacro\n"
    "%macro .cfi_def_cfa_offset 0+ .nolist\n"
    "    [unknowndirective \".cfi_def_cfa_offset\"]\n"
    "%endmacro\n"
    "%macro .cfi_adjust_cfa_offset 0+ .nolist\n"
    "    [unknowndirective \".cfi_adjust_cfa_offset\"]\n"
    "%endmacro\n"
    "%macro .cfi_offset 0+ .nolist\n"
    "    [unknowndirective \".cfi_offset\"]\n"
    "%endmacro\n"
    "%macro .cfi_val_offset 0+ .nolist\n"
    "    [unknowndirective \".cfi_val_offset\"]\n"
    "%endmacro\n"
    "%macro .cfi_rel_offset 0+ .nolist\n"
    "    [unknowndirective \".cfi_rel_offset\"]\n"
    "%endmacro\n"
    "%macro .cfi_register 0+ .nolist\n"
    "    [unknowndirective \".cfi_register\"]\n"
    "%endmacro\n"
    "%macro .cfi_restore 0+ .nolist\n"
    "    [unknowndirective \".cfi_restore\"]\n"
    "%endmacro\n"
    "%macro .cfi_undefined 0+ .nolist\n"
    "    [unknowndirective \".cfi_undefined\"]\n"
    "%endmacro\n"
    "%macro .cfi_same_value 0+ .nolist\n"
    "    [unknowndirective \".cfi_same_value\"]\n"
    "%endmacro\n"
    "%macro .cfi_remember_state 0+ .nolist\n"
    "    [unknowndirective \".cfi_remember_state\"]\n"
    "%endmacro\n"
    "%macro .cfi_restore_state 0+ .nolist\n"
    "    [unknowndirective \".cfi_restore_state\"]\n"
    "%endmacro\n"
    "%macro .cfi_return_column 0+ .nolist\n"
    "    [unknowndirective \".cfi_return_column\"]\n"
    "%endmacro\n"
    "%macro .cfi_signal_frame 0+ .nolist\n"
    "    [unknowndirective \".cfi_signal_frame\"]\n"
    "%endmacro\n"
    "%macro .cfi_window_save 0+ .nolist\n"
    "    [unknowndirective \".cfi_window_save\"]\n"
    "%endmacro\n"
    "%macro .cfi_escape 0+ .nolist\n"
    "    [unknowndirective \".cfi_escape\"]\n"
    "%endmacro\n"
    "%macro .cfi_val_encoded_addr 0+ .nolist\n"
    "    [unknowndirective \".cfi_val_encoded_addr\"]\n"
    "%endmacro\n"
    "%macro .linkonce 0+ .nolist\n"
    "    [unknowndirective \".linkonce\"]\n"
    "%endmacro\n"
    "%macro .octa 0+ .nolist\n"
    "    [unknowndirective \".octa\"]\n"
    "%endmacro\n"
    "%macro .org 0+ .nolist\n"
    "    [unknowndirective \".org\"]\n"
    "%endmacro\n"
    "%macro .reloc 0+ .nolist\n"
    "    [unknowndirective \".reloc\"]\n"
    "%endmacro\n"
    "%macro .sleb128 0+ .nolist\n"
    "    [unknowndirective \".sleb128\"]\n"
    "%endmacro\n"
    "%macro .uleb128 0+ .nolist\n"
    "    [unknowndirective \".uleb128\"]\n"
    "%endmacro\n"
    "%macro .gnu_attributes 0+ .nolist\n"
    "%endmacro\n"
    "%macro .hidden 0+ .nolist\n"
    "%endmacro\n"
    "%macro .internal 0+ .nolist\n"
    "%endmacro\n"
    "%macro .protected 0+ .nolist\n"
    "%endmacro\n"
    "%macro .desc 0+ .nolist\n"
    "%endmacro\n"
    "%macro .dim 0+ .nolist\n"
    "%endmacro\n"
    "%macro .end 0+ .nolist\n"
    "%endmacro\n"
    "%macro .ident 0+ .nolist\n"
    "%endmacro\n"
    "%macro .lflags 0+ .nolist\n"
    "%endmacro\n"
    "%macro .list 0+ .nolist\n"
    "%endmacro\n"
    //    "%macro .nolist 0+ .nolist\n"
    //    "%endmacro\n"
    "%macro .psize 0+ .nolist\n"
    "%endmacro\n"
    "%macro .loc_mark_labels 0+ .nolist\n"
    "%endmacro\n"
    "%macro .mri 0+ .nolist\n"
    "%endmacro\n"
    "%macro .scl 0+ .nolist\n"
    "%endmacro\n"
    "%macro .size 0+ .nolist\n"
    "%endmacro\n"
    "%macro .stabd 0+ .nolist\n"
    "%endmacro\n"
    "%macro .stabn 0+ .nolist\n"
    "%endmacro\n"
    "%macro .stabs 0+ .nolist\n"
    "%endmacro\n"
    "%macro .val 0+ .nolist\n"
    "%endmacro\n";

int InstructionParser::processorMode = 32;

bool IsSymbolCharRoutine(const char* data, bool startOnly);

InstructionParser* InstructionParser::GetInstance() { return static_cast<InstructionParser*>(new x64Parser()); }

void Instruction::RepRemoveCancellations(std::shared_ptr<AsmExprNode> exp, bool commit, int& count, Section* sect[], bool sign[],
                                         bool plus)
{
    if (exp->GetType() == AsmExprNode::LABEL)
    {
        std::shared_ptr<AsmExprNode> num = AsmExpr::GetEqu(exp->label);
        if (!num)
        {
            auto s = AsmFile::GetLabelSection(exp->label);
            if (s)
            {
                if (commit)
                {
                    exp->SetType(AsmExprNode::IVAL);
                    exp->ival = s->Lookup(exp->label)->second;
                }
                else
                {
                    sect[count] = s.get();
                    sign[count++] = plus;
                }
            }
        }
        else
            RepRemoveCancellations(num, commit, count, sect, sign, plus);
    }
    else if (exp->GetType() == AsmExprNode::BASED)
    {
        if (!commit)
        {
            sect[count] = exp->GetSection().get();
            sign[count++] = plus;
        }
        else
        {
            exp->SetType(AsmExprNode::IVAL);
        }
    }
    if (exp->GetLeft())
    {
        RepRemoveCancellations(exp->GetLeft(), commit, count, sect, sign, plus);
    }
    if (exp->GetRight())
    {
        RepRemoveCancellations(exp->GetRight(), commit, count, sect, sign, exp->GetType() == AsmExprNode::SUB ? !plus : plus);
    }
}

void Instruction::Optimize(Section* sect, int pc, bool last)
{
    unsigned char* pdata = data.get();
    if (!pdata)
        return;
    if (pdata && size >= 3 && pdata[0] == 0x0f && pdata[1] == 0x0f && (pdata[2] == 0x9a || pdata[2] == 0xea))
    {
        if (fixups.size() != 1)
        {
            std::cout << "Far branch cannot be resolved" << std::endl;
            memmove(pdata, pdata + 2, size - 2);
            size -= 2;
        }
        else
        {
            std::shared_ptr<Fixup> f = fixups[0];
            std::shared_ptr<AsmExprNode> expr = AsmExpr::Eval(f->GetExpr(), pc);
            if (expr->IsAbsolute())
            {
                memmove(pdata, pdata + 1, size - 1);
                size -= 3;
                f->SetInsOffs(f->GetInsOffs() - 1);
                pdata[0] = 0x0e;  // push cs
                int n = pdata[f->GetInsOffs() - 1];
                if (n == 0xea)  // far jmp
                    n = 0xe9;   // jmp
                else
                    n = 0xe8;  // call
                pdata[f->GetInsOffs() - 1] = n;
                f->SetRel(true);
                f->SetRelOffs(f->GetSize());
                f->SetAdjustable(true);
            }
            else
            {
                memmove(pdata, pdata + 2, size - 2);
                size -= 2;
                f->SetInsOffs(f->GetInsOffs() - 2);
                std::shared_ptr<AsmExprNode> n =
                    std::make_shared<AsmExprNode>(AsmExprNode::DIV, f->GetExpr(), std::make_shared<AsmExprNode>(16));
                fixups.push_back(std::make_shared<Fixup>(n, 2, false));
                f = fixups.back();
                f->SetInsOffs(size - 2);
            }
        }
    }
    if (type == CODE || type == DATA || type == RESERVE)
    {
        for (auto& fixup : fixups)
        {
            Section* sect[10];
            bool sign[10];
            int count = 0;
            bool canceled = false;
            std::shared_ptr<AsmExprNode> expr = fixup->GetExpr();
            if (!fixup->IsCanceled())
            {
                RepRemoveCancellations(expr, false, count, sect, sign, true);
                if (count == 2 && sect[0] == sect[1] && sign[0] == !sign[1])
                {
                    canceled = true;
                    RepRemoveCancellations(expr, true, count, sect, sign, true);
                }
                fixup->SetCanceled(true);
            }
            expr = AsmExpr::Eval(expr, pc);
            if (fixup->GetExpr()->IsAbsolute() || canceled || (fixup->IsRel() && expr->GetType() == AsmExprNode::IVAL))
            {
                int n = fixup->GetSize() * 8;
                int p = fixup->GetInsOffs();
                if (fixup->GetSize() < 4 || (expr->GetType() == AsmExprNode::IVAL && fixup->GetSize() == 4))
                {
                    int o;
                    bool error = false;
                    if (expr->GetType() == AsmExprNode::IVAL)
                        o = expr->ival;
                    else
                        o = (long long)expr->fval;
                    if (fixup->IsRel())
                    {
                        o -= size + pc;
                        if (fixup->IsAdjustable())
                        {
                            if (type == CODE)
                            {
                                if (p >= 1 && pdata[p - 1] == 0xe9)
                                {
                                    if (o <= 127 && o >= -128 - ((n / 8) - 1))
                                    {
                                        pdata[p - 1] = 0xeb;
                                        size -= fixup->GetSize() - 1;
                                        fixup->SetSize(1);
                                        fixup->SetRelOffs(1);
                                        n = 8;
                                    }
                                }
                                else if (p >= 2 && ((pdata[p - 1] & 0xf0) == 0x80) && pdata[p - 2] == 0x0f)
                                {
                                    if (o <= 127 && o >= -128 - (n / 8))
                                    {
                                        pdata[p - 2] = pdata[p - 1] - 0x80 + 0x70;
                                        size -= fixup->GetSize();
                                        fixup->SetInsOffs(p = fixup->GetInsOffs() - 1);
                                        fixup->SetSize(1);
                                        fixup->SetRelOffs(1);
                                        n = 8;
                                    }
                                }
                            }
                        }
                        int t = o >> (n - 1);
                        if (t != 0 && t != -1)
                        {
                            error = true;
                            if (!fixup->IsResolved())
                            {
                                if (last)
                                {
                                    Errors::IncrementCount();
                                    std::cout << "Error " << fixup->GetFileName().c_str() << "(" << fixup->GetErrorLine() << "):"
                                              << "Branch out of range" << std::endl;
                                }
                            }
                        }
                    }
                    else
                    {
                        if (n < 32)
                        {
                            int t = o >> (n);
                            if (t != 0 && t != -1)
                            {
                                error = true;
                                if (!fixup->IsResolved())
                                {
                                    if (last)
                                    {
                                        //                                        Errors::IncrementCount();
                                        std::cout << "Warning " << fixup->GetFileName() << "(" << fixup->GetErrorLine() << "):"
                                                  << "Value out of range" << std::endl;
                                    }
                                }
                            }
                        }
                    }
                    if (!error)
                        fixup->SetResolved();
                    if (bigEndian)
                    {
                        // never get here on an x86
                        for (int i = n - 8; i >= 0; i -= 8)
                            pdata[p + i / 8] = o >> i;
                    }
                    else
                    {
                        for (int i = 0; i < n; i += 8)
                            pdata[p + i / 8] = o >> i;
                    }
                }
                else if (expr->GetType() == AsmExprNode::IVAL || expr->GetType() == AsmExprNode::FVAL)
                {
                    if (expr->GetType() == AsmExprNode::IVAL && n != 80)
                    {
                        fixup->SetResolved();
                        if (n == 32)
                            *(unsigned*)(pdata + p) = expr->ival;
                        else
                            *(unsigned long long*)(pdata + p) = expr->ival;
                    }
                    else
                    {
                        FPF o;
                        fixup->SetResolved();
                        if (expr->GetType() == AsmExprNode::FVAL)
                            o = expr->fval;
                        else
                            o = expr->ival;
                        switch (n)
                        {
                            case 32:
                                o.ToFloat(pdata + p);
                                break;
                            case 64:
                                o.ToDouble(pdata + p);
                                break;
                            case 80:
                                o.ToLongDouble(pdata + p);
                                break;
                        }
                    }
                }
            }
        }
    }
}
std::string InstructionParser::RewriteATTArg(const std::string& line)
{
    size_t i = line.find_first_not_of("\t\v \r\n");
    if (i == std::string::npos)
        return "";
    std::string seg;
    if (line[i] == '%')
    {
        seg = line.substr(i + 1);
        if (seg.size() > 2 && seg[2] == ':')
        {
            seg = seg.substr(0, 3);
            i += 4;
        }
        else
        {
            if (seg.size() >= 2 && seg.substr(0, 2) == "st")
            {
                if (seg == "st")
                    return "st0";
                else if (seg[2] == '(' && seg[4] == ')')
                    return "st" + seg.substr(3, 1);
            }
            return seg;
        }
    }
    if (line[i] == '$')
    {
        for (i = i + 1; i < line.size(); i++)
            if (!isspace(line[i]))
                break;
        if (i < line.size() && (line[i] == '-' || isdigit(line[i])))
        {
            return line.substr(i);
        }
        std::string name = line.substr(i);
        if (!name.empty())
        {
            attPotentialExterns.insert(name);
        }
        return "dword " + name;
    }
    else
    {
        std::string name;
        int npos = line.find_first_of("(", i);
        if (npos == std::string::npos)
        {
            name = line.substr(i, npos - i);
            if (!name.empty())
            {
                attPotentialExterns.insert(name);
            }
            return "[" + seg + name + "]";
        }
        name = line.substr(i, npos - i);
        if (!name.empty())
        {
            attPotentialExterns.insert(name);
        }
        std::string primary, secondary, times;
        i = npos + 1;
        for (; i < line.size(); i++)
            if (!isspace(line[i]))
                break;
        int j = line.size() - 1;
        while (j > i && isspace(line[j]))
            j--;
        if (line[j] == ')')
        {
            std::vector<std::string> splt;
            Split(line.substr(i, j - i), splt);

            switch (splt.size())
            {
                case 1:
                    if (splt[0] != "" && splt[0][0] == '%')
                    {
                        primary = splt[0].substr(1);
                        break;
                    }
                    // error
                    return line;
                case 2:
                    if (splt[0] == "")
                    {
                        if (splt[1] == "1")
                        {
                            return "[" + seg + name + "]";
                        }
                    }
                    else if (splt[0][0] == '%')
                    {
                        primary = splt[0].substr(1);
                        if (splt[1] != "" && splt[1][0] == '%')
                        {
                            secondary = splt[1].substr(1);
                            break;
                        }
                    }
                    // error
                    return line;
                case 3:
                    if (splt[0] != "")
                    {
                        if (splt[0][0] == '%')
                            primary = splt[0].substr(1);
                        else
                            return "[" + seg + name + line.substr(i) + "]";
                    }
                    if (splt[1] != "")
                    {
                        if (splt[1][0] == '%')
                            secondary = splt[1].substr(1);
                        else
                            return "[" + seg + name + line.substr(i) + "]";
                    }
                    if (splt[2] != "")
                    {
                        times = splt[2];
                    }
                    break;
                default:
                    // error
                    return line;
            }
            if (secondary.size())
            {
                if (primary.size())
                    primary += "+";
                primary += secondary;
                if (times.size())
                {
                    primary += "*" + times;
                }
            }
            if (name.size())
            {
                attPotentialExterns.insert(name);
                primary += "+" + name;
            }
            primary = "[" + seg + primary + "]";
            return primary;
        }
        // error
        return line;
    }
}
void InstructionParser::PreprendSize(std::string& val, int sz)
{
    const char* str = "";
    switch (sz)
    {
        case 1:
            str = "byte ";
            break;
        case 2:
            str = "word ";
            break;
        case 4:
        case -4:
            str = "dword ";
            break;
        case 8:
        case -8:
            str = "qword ";
            break;
        case -10:
            str = "tword ";
            break;
    }
    val = std::string(str) + val;
}

std::string InstructionParser::ParsePreInstruction(const std::string& op, bool doParse)
{
    std::string parse = op;
    while (true)
    {
        int npos = parse.find_first_not_of("\t\v \n\r");
        if (npos != std::string::npos)
            parse = parse.substr(npos);
        if (!parse.size() || parse[0] != '{')
            break;
        npos = parse.find_first_of("}");
        if (npos == std::string::npos)
            break;
        Errors::Warning("Unknown qualifier " + parse.substr(0, npos + 1));

        parse = parse.substr(npos + 1);
    }
    return parse;
}

std::string InstructionParser::RewriteATT(int& op, const std::string& line, int& size1, int& size2)
{
    switch (op)
    {
        case op_call:
        case op_jmp: {
            int npos = line.find_first_not_of("\t\v \r\n");
            if (npos != std::string::npos)
                if (line[npos] == '*')
                {
                    if (line.find_first_of("%") != std::string::npos)
                    {
                        return "[" + RewriteATTArg(line.substr(npos + 1)) + "]";
                    }
                    else
                    {
                        std::string name = line.substr(npos + 1);
                        if (!name.empty())
                        {
                            attPotentialExterns.insert(name);
                        }
                        return "[" + name + "]";
                    }
                }
            if (!line.empty())
            {
                attPotentialExterns.insert(line);
            }
            return line;
        }
        case 10000:  // lcall
            op = op_call;
            if (!line.empty())
            {
                attPotentialExterns.insert(line);
            }
            return "far [" + line + "]";
        case 10001:  // ljmp
            op = op_jmp;
            if (!line.empty())
            {
                attPotentialExterns.insert(line);
            }
            return "far [" + line + "]";
        case op_ja:
        case op_jae:
        case op_jb:
        case op_jbe:
        case op_jc:
        case op_jcxz:
        case op_jecxz:
        case op_je:
        case op_jg:
        case op_jge:
        case op_jl:
        case op_jle:
        case op_jna:
        case op_jnae:
        case op_jnb:
        case op_jnbe:
        case op_jnc:
        case op_jne:
        case op_jng:
        case op_jnge:
        case op_jnl:
        case op_jnle:
        case op_jno:
        case op_jnp:
        case op_jns:
        case op_jnz:
        case op_jo:
        case op_jp:
        case op_jpe:
        case op_jpo:
        case op_js:
        case op_jz:
            if (!line.empty())
            {
                attPotentialExterns.insert(line);
            }
            return line;
        case op_lods:
        case op_stos:
        case op_movs:
        case op_cmps:
        case op_scas:
        case op_ins:
        case op_outs:
            if (op == op_movs)
                op++;
            switch (size1)
            {
                case 1:
                    op += 1;
                    break;
                case 2:
                    op += 2;
                    break;
                case 4:
                case -4:
                    op += 3;
                    break;
                case 8:
                case -8:
                    op += 4;
                    break;
            }
            return line;
        case op_push:
        case op_pop:
            if (size1 == 0)
                size1 = processorMode / 8;
            break;
    }
    std::vector<std::string> splt, splt2;
    // split arguments out into multiple strings
    Split(line, splt);
    // process each arg
    for (auto s : splt)
    {
        splt2.push_back(RewriteATTArg(s));
    }
    if (size1 == 4)  // 'l'
    {
        if (op >= op_f2xm1 && op <= op_fyl2xp1)
        {
            size1 = -8;
        }
    }

    if (size2 && splt2[0].find_first_of("[") != std::string::npos)
    {
        PreprendSize(splt2[0], size1);
    }
    else if (size1)
    {
        if (splt2.size() == 1)
        {
            if (splt2[0].find_first_of("[") != std::string::npos)
                PreprendSize(splt2[0], size1);
        }
        // the other combination has a destination which MUST BE a reg so we don't consider it.
        else
        {
            if (splt2[1].find_first_of("[") != std::string::npos)
                if (splt[0].find_first_of("%") == std::string::npos)
                    PreprendSize(splt2[1], size1);
        }
    }

    if (op >= op_f2xm1 && op <= op_fyl2xp1 && splt.size() == 2)
    {
        // as swaps the sense of some of the floating point instructions
        // and gcc depends on this...
        if (splt2[1].size() > 2 && splt2[1].substr(0, 2) == "st" && splt2[1] != "st0")
        {
            switch (op)
            {
                case op_fsub:
                    op = op_fsubr;
                    break;
                case op_fsubp:
                    op = op_fsubrp;
                    break;
                case op_fsubr:
                    op = op_fsub;
                    break;
                case op_fsubrp:
                    op = op_fsubp;
                    break;
                case op_fdiv:
                    op = op_fdivr;
                    break;
                case op_fdivp:
                    op = op_fdivrp;
                    break;
                case op_fdivr:
                    op = op_fdiv;
                    break;
                case op_fdivrp:
                    op = op_fdivp;
                    break;
            }
        }
    }
    if (splt2.size() == 1)
    {
        switch (op)
        {
            case op_shl:
            case op_shr:
            case op_sal:
            case op_sar:
            case op_rcl:
            case op_rcr:
            case op_rol:
            case op_ror:
                splt2.push_back("");
                splt2[1] = splt2[0];
                splt2[0] = "1";
                break;
        }
    }
    std::string rv;
    // reverse
    for (int i = splt2.size() - 1; i >= 0; i--)
    {
        if (rv.size())
            rv += ",";
        rv += splt2[i];
    }
    return rv;
}

int x64Parser::DoMath(char op, int left, int right)
{
    switch (op)
    {
        case '!':
            return -left;
        case '~':
            return ~left;
        case '+':
            return left + right;
        case '-':
            return left - right;
        case '>':
            return left >> right;
        case '<':
            return left << right;
        case '&':
            return left & right;
        case '|':
            return left | right;
        case '^':
            return left ^ right;
        default:
            return left;
    }
}
void x64Parser::Setup(Section* sect)
{
    if (sect->beValues[0] == 0)
        sect->beValues[0] = processorMode;  // 32 bit mode is the default
    Setprocessorbits(sect->beValues[0]);
}

bool x64Parser::ParseSection(AsmFile* fil, Section* sect)
{
    bool rv = false;
    if (fil->GetKeyword() == kw::USE16)
    {
        sect->beValues[0] = 16;
        fil->NextToken();
        rv = true;
    }
    else if (fil->GetKeyword() == kw::USE32)
    {
        sect->beValues[0] = 32;
        fil->NextToken();
        rv = true;
    }
    else if (fil->GetKeyword() == kw::USE64)
    {
        sect->beValues[0] = 64;
        fil->NextToken();

        rv = true;
    }
    return rv;
}
bool x64Parser::ParseDirective(AsmFile* fil, Section* sect)
{
    bool rv = false;
    if (fil->GetKeyword() == kw::BITS)
    {
        fil->NextToken();
        if (fil->IsNumber())
        {
            int n = static_cast<const NumericToken*>(fil->GetToken())->GetInteger();
            if (n == 16 || n == 32 || n == 64)
            {
                sect->beValues[0] = n;
                Setprocessorbits(sect->beValues[0]);
                rv = true;
            }
            fil->NextToken();
        }
    }
    return rv;
}