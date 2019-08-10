/* Software License Agreement
 *
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
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
 */

#include "opcodes.h"

static OPTABLE base0[] = {{0x00fc, 0x0000, "add", OP_REGRMREG, 2, VAL_ALL},
                          {0x00e7, 0x0006, "push", OP_SEG35, 1, VAL_ALL},
                          {0x00fe, 0x0004, "add", OP_ACCIMM, 1, VAL_ALL},
                          {0x00e7, 0x0007, "pop", OP_SEG35, 1, VAL_ALL},
                          {0x00fc, 0x0008, "or", OP_REGRMREG, 2, VAL_ALL},
                          {0x00fe, 0x000c, "or", OP_ACCIMM, 1, VAL_ALL},
                          {0x00fc, 0x0010, "adc", OP_REGRMREG, 2, VAL_ALL},
                          {0x00fe, 0x0014, "adc", OP_ACCIMM, 1, VAL_ALL},
                          {0x00fc, 0x0018, "sbb", OP_REGRMREG, 2, VAL_ALL},
                          {0x00fe, 0x001c, "sbb", OP_ACCIMM, 1, VAL_ALL},
                          {0, 0, 0, 0, 0, 0}};

static OPTABLE base1[] = {{0x00fc, 0x0020, "and", OP_REGRMREG, 2, VAL_ALL},
                          {0x00fe, 0x0024, "and", OP_ACCIMM, 1, VAL_ALL},
                          {0x00ff, 0x0027, "daa", OP_CODEONLY, 1, VAL_ALL},
                          {0x00fc, 0x0028, "sub", OP_REGRMREG, 2, VAL_ALL},
                          {0x00fe, 0x002c, "sub", OP_ACCIMM, 1, VAL_ALL},
                          {0x00ff, 0x002f, "das", OP_CODEONLY, 1, VAL_ALL},
                          {0x00fc, 0x0030, "xor", OP_REGRMREG, 2, VAL_ALL},
                          {0x00fe, 0x0034, "xor", OP_ACCIMM, 1, VAL_ALL},
                          {0x00ff, 0x0037, "aaa", OP_CODEONLY, 1, VAL_ALL},
                          {0x00fc, 0x0038, "cmp", OP_REGRMREG, 2, VAL_ALL},
                          {0x00fe, 0x003c, "cmp", OP_ACCIMM, 1, VAL_ALL},
                          {0x00ff, 0x003f, "aas", OP_CODEONLY, 1, VAL_ALL},
                          {0, 0, 0, 0, 0, 0}};

static OPTABLE base2[] = {{0x00f8, 0x0040, "inc", OP_WREG02, 1, VAL_ALL},
                          {0x00f8, 0x0048, "dec", OP_WREG02, 1, VAL_ALL},
                          {0x00f8, 0x0050, "push", OP_WREG02, 1, VAL_ALL},
                          {0x00f8, 0x0058, "pop", OP_WREG02, 1, VAL_ALL},
                          {0, 0, 0, 0, 0, 0}};

static OPTABLE base3[] = {{0x00ff, 0x0060, "pusha", OP_CODEONLY, 1, VAL_NO8086},
                          {0x00ff, 0x0061, "popa", OP_CODEONLY, 1, VAL_NO8086},
                          {0x00ff, 0x0062, "bound", OP_WORDREGRM, 2, VAL_NO8086},
                          {0x00ff, 0x0063, "arpl", OP_WORDRMREG, 2, VAL_286},
                          {0x00ff, 0x0064, "repnc", OP_CODEONLY, 1, VAL_VX},
                          {0x00ff, 0x0065, "repc", OP_CODEONLY, 1, VAL_VX},
                          {0xc0fe, 0xc066, "fp02", OP_CODEONLY, 2, VAL_VX},
                          {0x00fe, 0x0066, "fp02", OP_CODEONLY, 2, VAL_VX},
                          {0x00fd, 0x0068, "push", OP_IMM, 1, VAL_NO8086},
                          {0x00fd, 0x0069, "imul", OP_IMUL, 2, VAL_NO8086},
                          {0x00ff, 0x006c, "insb", OP_CODEONLY, 1, VAL_NO8086},
                          {0x00ff, 0x006d, "ins", OP_CODEONLY, 1, VAL_NO8086},
                          {0x00ff, 0x006e, "outsb", OP_CODEONLY, 1, VAL_NO8086},
                          {0x00ff, 0x006f, "outs", OP_CODEONLY, 1, VAL_NO8086},
                          {0x00ff, 0x0070, "jo", OP_SHORTBRANCH, 2, VAL_ALL},
                          {0x00ff, 0x0071, "jno", OP_SHORTBRANCH, 2, VAL_ALL},
                          {0x00ff, 0x0072, "jb", OP_SHORTBRANCH, 2, VAL_ALL},
                          {0x00ff, 0x0073, "jae", OP_SHORTBRANCH, 2, VAL_ALL},
                          {0x00ff, 0x0074, "jz", OP_SHORTBRANCH, 2, VAL_ALL},
                          {0x00ff, 0x0075, "jnz", OP_SHORTBRANCH, 2, VAL_ALL},
                          {0x00ff, 0x0076, "jbe", OP_SHORTBRANCH, 2, VAL_ALL},
                          {0x00ff, 0x0077, "ja", OP_SHORTBRANCH, 2, VAL_ALL},
                          {0x00ff, 0x0078, "js", OP_SHORTBRANCH, 2, VAL_ALL},
                          {0x00ff, 0x0079, "jns", OP_SHORTBRANCH, 2, VAL_ALL},
                          {0x00ff, 0x007a, "jpe", OP_SHORTBRANCH, 2, VAL_ALL},
                          {0x00ff, 0x007b, "jpo", OP_SHORTBRANCH, 2, VAL_ALL},
                          {0x00ff, 0x007c, "jl", OP_SHORTBRANCH, 2, VAL_ALL},
                          {0x00ff, 0x007d, "jge", OP_SHORTBRANCH, 2, VAL_ALL},
                          {0x00ff, 0x007e, "jle", OP_SHORTBRANCH, 2, VAL_ALL},
                          {0x00ff, 0x007f, "jg", OP_SHORTBRANCH, 2, VAL_ALL},
                          {0, 0, 0, 0, 0, 0}};

static OPTABLE base4[] = {{0x38fc, 0x0080, "add", OP_RMIMMSIGNED, 2, VAL_ALL},
                          {0x38fe, 0x0880, "or", OP_RMIMMSIGNED, 2, VAL_ALL},
                          {0x38fc, 0x0880, "or", OP_RMIMMSIGNED, 2, VAL_386},
                          {0x38fc, 0x1080, "adc", OP_RMIMMSIGNED, 2, VAL_ALL},
                          {0x38fc, 0x1880, "sbb", OP_RMIMMSIGNED, 2, VAL_ALL},
                          {0x38fe, 0x2080, "and", OP_RMIMMSIGNED, 2, VAL_ALL},
                          {0x38fc, 0x2080, "and", OP_RMIMMSIGNED, 2, VAL_386},
                          {0x38fc, 0x2880, "sub", OP_RMIMMSIGNED, 2, VAL_ALL},
                          {0x38fe, 0x3080, "xor", OP_RMIMMSIGNED, 2, VAL_ALL},
                          {0x38fc, 0x3080, "xor", OP_RMIMMSIGNED, 2, VAL_386},
                          {0x38fc, 0x3880, "cmp", OP_RMIMMSIGNED, 2, VAL_ALL},
                          {0x00fe, 0x0084, "test", OP_REGRM, 2, VAL_ALL},
                          {0x00fe, 0x0086, "xchg", OP_REGRMBW, 2, VAL_ALL},
                          {0x00fc, 0x0088, "mov", OP_REGRMREG, 2, VAL_ALL},
                          {0x20fd, 0x008c, "mov", OP_SEGRMSEG, 2, VAL_ALL},
                          {0x00fd, 0x008c, "mov", OP_SEGRMSEG, 2, VAL_386},
                          {0x00ff, 0x008d, "lea", OP_WORDREGRM, 2, VAL_ALL},
                          {0x38ff, 0x008f, "pop", OP_NOSTRICTRM, 2, VAL_ALL},
                          {0x00ff, 0x0090, "nop", OP_CODEONLY, 1, VAL_ALL},
                          {0x00f8, 0x0090, "xchg", OP_ACCREG02, 1, VAL_ALL},
                          {0x00ff, 0x0098, "cbw", OP_CODEONLY, 1, VAL_ALL},
                          {0x00ff, 0x0099, "cwd", OP_CODEONLY, 1, VAL_ALL},
                          {0x00ff, 0x009a, "call", OP_SEGBRANCH, 5, VAL_ALL},
                          {0x00ff, 0x009b, "wait", OP_CODEONLY, 1, VAL_ALL},
                          {0x00ff, 0x009c, "pushf", OP_INSWORDSIZE, 1, VAL_ALL},
                          {0x00ff, 0x009d, "popf", OP_INSWORDSIZE, 1, VAL_ALL},
                          {0x00ff, 0x009e, "sahf", OP_CODEONLY, 1, VAL_ALL},
                          {0x00ff, 0x009f, "lahf", OP_CODEONLY, 1, VAL_ALL},
                          {0, 0, 0, 0, 0, 0}};

static OPTABLE base5[] = {{0x00fe, 0x00a0, "mov", OP_ACCABS, 3, VAL_ALL},
                          {0x00fe, 0x00a2, "mov", OP_ABSACC, 3, VAL_ALL},
                          {0x00ff, 0x00a5, "movs", OP_INSWORDSIZE, 1, VAL_ALL},
                          {0x00ff, 0x00a7, "cmps", OP_INSWORDSIZE, 1, VAL_ALL},
                          {0x00fe, 0x00a8, "test", OP_ACCIMM, 1, VAL_ALL},
                          {0x00ff, 0x00ab, "stos", OP_INSWORDSIZE, 1, VAL_ALL},
                          {0x00ff, 0x00ad, "lods", OP_INSWORDSIZE, 1, VAL_ALL},
                          {0x00ff, 0x00af, "scas", OP_INSWORDSIZE, 1, VAL_ALL},
                          {0x00ff, 0x00a4, "movsb", OP_CODEONLY, 1, VAL_ALL},
                          {0x00ff, 0x00a6, "cmpsb", OP_CODEONLY, 1, VAL_ALL},
                          {0x00ff, 0x00aa, "stosb", OP_CODEONLY, 1, VAL_ALL},
                          {0x00ff, 0x00ac, "lodsb", OP_CODEONLY, 1, VAL_ALL},
                          {0x00ff, 0x00ae, "scasb", OP_CODEONLY, 1, VAL_ALL},
                          {0x00f0, 0x00b0, "mov", OP_ACCIMMB3, 1, VAL_ALL},
                          {0, 0, 0, 0, 0, 0}};

static OPTABLE base6[] = {{0x38fe, 0x00c0, "rol", OP_RMSHIFT, 2, VAL_NO8086},
                          {0x38fe, 0x08c0, "ror", OP_RMSHIFT, 2, VAL_NO8086},
                          {0x38fe, 0x10c0, "rcl", OP_RMSHIFT, 2, VAL_NO8086},
                          {0x38fe, 0x18c0, "rcr", OP_RMSHIFT, 2, VAL_NO8086},
                          {0x38fe, 0x20c0, "shl", OP_RMSHIFT, 2, VAL_NO8086},
                          {0x38fe, 0x28c0, "shr", OP_RMSHIFT, 2, VAL_NO8086},
                          {0x38fe, 0x38c0, "sar", OP_RMSHIFT, 2, VAL_NO8086},
                          {0x00ff, 0x00c2, "ret", OP_RET, 3, VAL_ALL},
                          {0x00ff, 0x00c3, "ret", OP_CODEONLY, 1, VAL_ALL},
                          {0x00ff, 0x00c4, "les", OP_WORDREGRM, 2, VAL_ALL},
                          {0x00ff, 0x00c5, "lds", OP_WORDREGRM, 2, VAL_ALL},
                          {0x38fe, 0x00c6, "mov", OP_RMIMM, 2, VAL_ALL},
                          {0x00ff, 0x00c8, "enter", OP_ENTER, 4, VAL_NO8086},
                          {0x00ff, 0x00ca, "retf", OP_RET, 3, VAL_ALL},
                          {0x00ff, 0x00cb, "retf", OP_CODEONLY, 1, VAL_ALL},
                          {0x00ff, 0x00cc, "int", OP_INTR, 1, VAL_ALL},
                          {0x00ff, 0x00cd, "int", OP_INTR, 2, VAL_ALL},
                          {0x00ff, 0x00ce, "into", OP_CODEONLY, 1, VAL_ALL},
                          {0x00ff, 0x00cf, "iret", OP_CODEONLY, 1, VAL_ALL},
                          {0x00ff, 0x00c9, "leave", OP_CODEONLY, 1, VAL_NO8086},
                          {0x38fc, 0x00d0, "rol", OP_RMSHIFT, 2, VAL_ALL},
                          {0x38fc, 0x08d0, "ror", OP_RMSHIFT, 2, VAL_ALL},
                          {0x38fc, 0x10d0, "rcl", OP_RMSHIFT, 2, VAL_ALL},
                          {0x38fc, 0x18d0, "rcr", OP_RMSHIFT, 2, VAL_ALL},
                          {0x38fc, 0x20d0, "shl", OP_RMSHIFT, 2, VAL_ALL},
                          {0x38fc, 0x28d0, "shr", OP_RMSHIFT, 2, VAL_ALL},
                          {0x38fc, 0x38d0, "sar", OP_RMSHIFT, 2, VAL_ALL},
                          {0xffff, 0x0ad4, "aam", OP_CODEONLY, 2, VAL_ALL},
                          {0xffff, 0x0ad5, "aad", OP_CODEONLY, 2, VAL_ALL},
                          {0x00ff, 0x00d7, "xlat", OP_CODEONLY, 1, VAL_ALL},
                          {0, 0, 0, 0, 0, 0}};

static OPTABLE base7[] = {{0x00ff, 0x00e0, "loopnz", OP_SHORTBRANCH, 2, VAL_ALL},
                          {0x00ff, 0x00e1, "loopz", OP_SHORTBRANCH, 2, VAL_ALL},
                          {0x00ff, 0x00e2, "loop", OP_SHORTBRANCH, 2, VAL_ALL},
                          {0x00ff, 0x00e3, "jcxz", OP_SHORTBRANCH, 2, VAL_ALL},
                          {0x00fe, 0x00e4, "in", OP_PORTACCPORT, 2, VAL_ALL},
                          {0x00fe, 0x00e6, "out", OP_PORTACCPORT, 2, VAL_ALL},
                          {0x00ff, 0x00e8, "call", OP_BRANCH, 3, VAL_ALL},
                          {0x00ff, 0x00e9, "jmp", OP_BRANCH, 3, VAL_ALL},
                          {0x00ff, 0x00ea, "jmp", OP_SEGBRANCH, 5, VAL_ALL},
                          {0x00ff, 0x00eb, "jmp", OP_SHORTBRANCH, 2, VAL_ALL},
                          {0x00fe, 0x00ec, "in", OP_ACCDX, 1, VAL_ALL},
                          {0x00fe, 0x00ee, "out", OP_DXACC, 1, VAL_ALL},
                          {0x00ff, 0x00f0, "lock", OP_CODEONLY, 1, VAL_ALL},
                          {0x00ff, 0x00f2, "repnz", OP_CODEONLY, 1, VAL_ALL},
                          {0x00ff, 0x00f3, "repz", OP_CODEONLY, 1, VAL_ALL},
                          {0x00ff, 0x00f4, "hlt", OP_CODEONLY, 1, VAL_ALL},
                          {0x00ff, 0x00f5, "cmc", OP_CODEONLY, 1, VAL_ALL},
                          {0x38fe, 0x00f6, "test", OP_RMIMM, 2, VAL_ALL},
                          {0x38fe, 0x10f6, "not", OP_RM, 2, VAL_ALL},
                          {0x38fe, 0x18f6, "neg", OP_RM, 2, VAL_ALL},
                          {0x38fe, 0x20f6, "mul", OP_RM, 2, VAL_ALL},
                          {0x38fe, 0x28f6, "imul", OP_RM, 2, VAL_ALL},
                          {0x38fe, 0x30f6, "div", OP_RM, 2, VAL_ALL},
                          {0x38fe, 0x38f6, "idiv", OP_RM, 2, VAL_ALL},
                          {0x00ff, 0x00f8, "clc", OP_CODEONLY, 1, VAL_ALL},
                          {0x00ff, 0x00f9, "stc", OP_CODEONLY, 1, VAL_ALL},
                          {0x00ff, 0x00fa, "cli", OP_CODEONLY, 1, VAL_ALL},
                          {0x00ff, 0x00fb, "sti", OP_CODEONLY, 1, VAL_ALL},
                          {0x00ff, 0x00fc, "cld", OP_CODEONLY, 1, VAL_ALL},
                          {0x00ff, 0x00fd, "std", OP_CODEONLY, 1, VAL_ALL},
                          {0x38fe, 0x00fe, "inc", OP_RM, 2, VAL_ALL},
                          {0x38fe, 0x08fe, "dec", OP_RM, 2, VAL_ALL},
                          {0x38ff, 0x10ff, "call", OP_RM, 2, VAL_ALL},
                          {0x38ff, 0x18ff, "call", OP_FARRM, 2, VAL_ALL},
                          {0x38ff, 0x20ff, "jmp", OP_RM, 2, VAL_ALL},
                          {0x38ff, 0x28ff, "jmp", OP_FARRM, 2, VAL_ALL},
                          {0x38ff, 0x30ff, "push", OP_NOSTRICTRM, 2, VAL_ALL},
                          {0, 0, 0, 0, 0, 0}};

static OPTABLE baseVX[] = {{0x00ff, 0x0020, "add4s", OP_CODEONLY, 1, VAL_VX}, {0x00ff, 0x0022, "sub4s", OP_CODEONLY, 1, VAL_VX},
                           {0x00ff, 0x0026, "cmp4s", OP_CODEONLY, 1, VAL_VX}, {0x38ff, 0x0028, "rol4", OP_RM, 2, VAL_VX},
                           {0x38ff, 0x002a, "ror4", OP_RM, 2, VAL_VX},        {0xc0ff, 0xc031, "ins", OP_REGMOD, 2, VAL_VX},
                           {0xc0ff, 0xc033, "ext", OP_REGMOD, 2, VAL_VX},     {0xf8ff, 0xc039, "ins", OP_MODIMM, 3, VAL_VX},
                           {0xf8ff, 0xc03b, "ext", OP_MODIMM, 3, VAL_VX},     {0x38f6, 0x0010, "test1", OP_RMSHIFTB3, 2, VAL_VX},
                           {0x38f6, 0x0012, "clr1", OP_RMSHIFTB3, 2, VAL_VX}, {0x38f6, 0x0014, "set1", OP_RMSHIFTB3, 2, VAL_VX},
                           {0x38f6, 0x0016, "not1", OP_RMSHIFTB3, 2, VAL_VX}, {0, 0, 0, 0, 0, 0}};

static OPTABLE base386[] = {{0x00fe, 0x00be, "movsx", OP_MIXEDREGRM, 2, VAL_386},
                            {0x00fe, 0x00b6, "movzx", OP_MIXEDREGRM, 2, VAL_386},
                            {0x00f7, 0x00a0, "push", OP_SEG35, 1, VAL_386},
                            {0x00f7, 0x00a1, "pop", OP_SEG35, 1, VAL_386},
                            {0x00ff, 0x00b4, "lfs", OP_WORDREGRM, 2, VAL_386},
                            {0x00ff, 0x00b5, "lgs", OP_WORDREGRM, 2, VAL_386},
                            {0x00ff, 0x00b2, "lss", OP_WORDREGRM, 2, VAL_386},
                            {0x00ff, 0x0006, "clts", OP_CODEONLY, 1, VAL_286},
                            {0x00ff, 0x00af, "imul", OP_WORDREGRM, 2, VAL_386},
                            {0x00ff, 0x00a4, "shld", OP_REGRMSHIFT, 3, VAL_386},
                            {0x00ff, 0x00a5, "shld", OP_REGRMSHIFT, 2, VAL_386},
                            {0x00ff, 0x00ac, "shrd", OP_REGRMSHIFT, 3, VAL_386},
                            {0x00ff, 0x00ad, "shrd", OP_REGRMSHIFT, 2, VAL_386},
                            {0x00ff, 0x00bc, "bsf", OP_WORDREGRM, 2, VAL_386},
                            {0x00ff, 0x00bd, "bsr", OP_WORDREGRM, 2, VAL_386},
                            {0x00ff, 0x00a3, "bt", OP_WORDRMREG, 2, VAL_386},
                            {0x00ff, 0x00ab, "bts", OP_WORDRMREG, 2, VAL_386},
                            {0x00ff, 0x00b3, "btr", OP_WORDRMREG, 2, VAL_386},
                            {0x00ff, 0x00bb, "btc", OP_WORDRMREG, 2, VAL_386},
                            {0x38ff, 0x20ba, "bt", OP_BITNUM, 3, VAL_386},
                            {0x38ff, 0x28ba, "bts", OP_BITNUM, 3, VAL_386},
                            {0x38ff, 0x30ba, "btr", OP_BITNUM, 3, VAL_386},
                            {0x38ff, 0x38ba, "btc", OP_BITNUM, 3, VAL_386},
                            {0x00ff, 0x00a7, "ibts", OP_CODEONLY, 1, VAL_386},
                            {0x00ff, 0x00a6, "xbts", OP_CODEONLY, 1, VAL_386},
                            {0x00ff, 0x0080, "jo", OP_BRANCH, 3, VAL_386},
                            {0x00ff, 0x0081, "jno", OP_BRANCH, 3, VAL_386},
                            {0x00ff, 0x0082, "jb", OP_BRANCH, 3, VAL_386},
                            {0x00ff, 0x0083, "jnb", OP_BRANCH, 3, VAL_386},
                            {0x00ff, 0x0084, "je", OP_BRANCH, 3, VAL_386},
                            {0x00ff, 0x0085, "jne", OP_BRANCH, 3, VAL_386},
                            {0x00ff, 0x0086, "jna", OP_BRANCH, 3, VAL_386},
                            {0x00ff, 0x0087, "ja", OP_BRANCH, 3, VAL_386},
                            {0x00ff, 0x0088, "js", OP_BRANCH, 3, VAL_386},
                            {0x00ff, 0x0089, "jns", OP_BRANCH, 3, VAL_386},
                            {0x00ff, 0x008a, "jp", OP_BRANCH, 3, VAL_386},
                            {0x00ff, 0x008b, "jnp", OP_BRANCH, 3, VAL_386},
                            {0x00ff, 0x008c, "jl", OP_BRANCH, 3, VAL_386},
                            {0x00ff, 0x008d, "jge", OP_BRANCH, 3, VAL_386},
                            {0x00ff, 0x008e, "jle", OP_BRANCH, 3, VAL_386},
                            {0x00ff, 0x008f, "jg", OP_BRANCH, 3, VAL_386},
                            {0x38ff, 0x0090, "seto", OP_NOSTRICTRM, 2, VAL_386},
                            {0x38ff, 0x0091, "setno", OP_NOSTRICTRM, 2, VAL_386},
                            {0x38ff, 0x0092, "setb", OP_NOSTRICTRM, 2, VAL_386},
                            {0x38ff, 0x0093, "setnb", OP_NOSTRICTRM, 2, VAL_386},
                            {0x38ff, 0x0094, "setz", OP_NOSTRICTRM, 2, VAL_386},
                            {0x38ff, 0x0095, "setnz", OP_NOSTRICTRM, 2, VAL_386},
                            {0x38ff, 0x0096, "setbe", OP_NOSTRICTRM, 2, VAL_386},
                            {0x38ff, 0x0097, "seta", OP_NOSTRICTRM, 2, VAL_386},
                            {0x38ff, 0x0098, "sets", OP_NOSTRICTRM, 2, VAL_386},
                            {0x38ff, 0x0099, "setns", OP_NOSTRICTRM, 2, VAL_386},
                            {0x38ff, 0x009a, "setp", OP_NOSTRICTRM, 2, VAL_386},
                            {0x38ff, 0x009b, "setnp", OP_NOSTRICTRM, 2, VAL_386},
                            {0x38ff, 0x009c, "setl", OP_NOSTRICTRM, 2, VAL_386},
                            {0x38ff, 0x009d, "setge", OP_NOSTRICTRM, 2, VAL_386},
                            {0x38ff, 0x009e, "setle", OP_NOSTRICTRM, 2, VAL_386},
                            {0x38ff, 0x009f, "setg", OP_NOSTRICTRM, 2, VAL_386},
                            {0xc0fd, 0xc020, "mov", OP_386REG, 2, VAL_386},
                            {0xc0fd, 0xc021, "mov", OP_386REG, 2, VAL_386},
                            {0xc0fd, 0xc024, "mov", OP_386REG, 2, VAL_386},
                            {0x00ff, 0x0002, "lar", OP_WORDREGRM, 2, VAL_286},
                            {0x00ff, 0x0003, "lsl", OP_WORDREGRM, 2, VAL_286},
                            {0x38ff, 0x1001, "lgdt", OP_NOSTRICTRM, 2, VAL_286},
                            {0x38ff, 0x1801, "lidt", OP_NOSTRICTRM, 2, VAL_286},
                            {0x38ff, 0x1000, "lldt", OP_NOSTRICTRM, 2, VAL_286},
                            {0x38ff, 0x3001, "lmsw", OP_NOSTRICTRM, 2, VAL_286},
                            {0x38ff, 0x1800, "ltr", OP_NOSTRICTRM, 2, VAL_286},
                            {0x38ff, 0x0001, "sgdt", OP_NOSTRICTRM, 2, VAL_286},
                            {0x38ff, 0x0801, "sidt", OP_NOSTRICTRM, 2, VAL_286},
                            {0x38ff, 0x0000, "sldt", OP_NOSTRICTRM, 2, VAL_286},
                            {0x38ff, 0x2001, "smsw", OP_NOSTRICTRM, 2, VAL_286},
                            {0x38ff, 0x0800, "str", OP_NOSTRICTRM, 2, VAL_286},
                            {0x38ff, 0x2000, "verr", OP_NOSTRICTRM, 2, VAL_286},
                            {0x38ff, 0x2800, "verw", OP_NOSTRICTRM, 2, VAL_286},
                            {0, 0, 0, 0, 0, 0}};
static OPTABLE floats[] = {
    /* single byte commands */
    {0xffff, 0xd0d9, "fnop", OP_CODEONLY, 2, VAL_FLOAT},
    {0xffff, 0xe0d9, "fchs", OP_CODEONLY, 2, VAL_FLOAT},
    {0xffff, 0xe1d9, "fabs", OP_CODEONLY, 2, VAL_FLOAT},
    {0xffff, 0xe4d9, "ftst", OP_CODEONLY, 2, VAL_FLOAT},
    {0xffff, 0xe5d9, "fxam", OP_CODEONLY, 2, VAL_FLOAT},
    {0xffff, 0xe8d9, "fld1", OP_CODEONLY, 2, VAL_FLOAT},
    {0xffff, 0xe9d9, "fldl2t", OP_CODEONLY, 2, VAL_FLOAT},
    {0xffff, 0xead9, "fldl2e", OP_CODEONLY, 2, VAL_FLOAT},
    {0xffff, 0xebd9, "fldpi", OP_CODEONLY, 2, VAL_FLOAT},
    {0xffff, 0xecd9, "fldlg2", OP_CODEONLY, 2, VAL_FLOAT},
    {0xffff, 0xedd9, "fldln2", OP_CODEONLY, 2, VAL_FLOAT},
    {0xffff, 0xeed9, "fldz", OP_CODEONLY, 2, VAL_FLOAT},
    {0xffff, 0xf0d9, "f2xm1", OP_CODEONLY, 2, VAL_FLOAT},
    {0xffff, 0xf1d9, "fyl2x", OP_CODEONLY, 2, VAL_FLOAT},
    {0xffff, 0xf2d9, "fptan", OP_CODEONLY, 2, VAL_FLOAT},
    {0xffff, 0xf3d9, "fpatan", OP_CODEONLY, 2, VAL_FLOAT},
    {0xffff, 0xf5d9, "fprem1", OP_CODEONLY, 2, VAL_FLOAT386},
    {0xffff, 0xf4d9, "fxtract", OP_CODEONLY, 2, VAL_FLOAT},
    {0xffff, 0xf6d9, "fdecstp", OP_CODEONLY, 2, VAL_FLOAT},
    {0xffff, 0xf7d9, "fincstp", OP_CODEONLY, 2, VAL_FLOAT},
    {0xffff, 0xf8d9, "fprem", OP_CODEONLY, 2, VAL_FLOAT},
    {0xffff, 0xf9d9, "fyl2xp1", OP_CODEONLY, 2, VAL_FLOAT},
    {0xffff, 0xfad9, "fsqrt", OP_CODEONLY, 2, VAL_FLOAT},
    {0xffff, 0xfbd9, "fsincos", OP_CODEONLY, 2, VAL_FLOAT386},
    {0xffff, 0xfcd9, "frndint", OP_CODEONLY, 2, VAL_FLOAT},
    {0xffff, 0xfdd9, "fscale", OP_CODEONLY, 2, VAL_FLOAT},
    {0xffff, 0xfed9, "fsin", OP_CODEONLY, 2, VAL_FLOAT386},
    {0xffff, 0xffd9, "fcos", OP_CODEONLY, 2, VAL_FLOAT386},
    {0xffff, 0xe9da, "fucompp", OP_CODEONLY, 2, VAL_FLOAT386},
    {0xffff, 0xe0db, "feni", OP_CODEONLY, 2, VAL_FLOAT},
    {0xffff, 0xe1db, "fdisi", OP_CODEONLY, 2, VAL_FLOAT},
    {0xffff, 0xe2db, "fclex", OP_CODEONLY, 2, VAL_FLOAT},
    {0xffff, 0xe3db, "finit", OP_CODEONLY, 2, VAL_FLOAT},
    {0xffff, 0xe4db, "fsetpm", OP_CODEONLY, 2, VAL_FLOAT386},
    {0xffff, 0xd9de, "fcompp", OP_CODEONLY, 2, VAL_FLOAT},

    /* Group 1, RM 3 */
    {0xf8ff, 0xc0d9, "fld", OP_FLOATRM, 2, VAL_FLOAT},
    {0xf8ff, 0xc8d9, "fxch", OP_FLOATRM, 2, VAL_FLOAT},
    {0xf8fb, 0xd8d9, "fstp", OP_FLOATRM, 2, VAL_FLOAT},
    {0xc0ff, 0xc0d9, "esc", OP_ESC, 2, VAL_ALL},

    /* Group 1, RM0-2 */
    {0x38ff, 0x20d9, "fldenv", OP_FLOATNOPTR, 2, VAL_FLOAT},
    {0x38ff, 0x28d9, "fldcw", OP_FLOATNOPTR, 2, VAL_FLOAT},
    {0x38ff, 0x30d9, "fstenv", OP_FLOATNOPTR, 2, VAL_FLOAT},
    {0x38ff, 0x38d9, "fstcw", OP_FLOATNOPTR, 2, VAL_FLOAT},

    /* Group 5, RM3 */
    {0xf8ff, 0xc0dd, "ffree", OP_FLOATRM, 2, VAL_FLOAT},
    {0xf8ff, 0xd0dd, "fst", OP_FLOATRM, 2, VAL_FLOAT},
    {0xf8ff, 0xe0dd, "fucom", OP_FLOATRM, 2, VAL_FLOAT386},
    {0xf8ff, 0xe8dd, "fucomp", OP_FLOATRM, 2, VAL_FLOAT386},

    {0xc0ff, 0xc0dd, "esc", OP_ESC, 2, VAL_ALL},

    /* Group 5, RM0-2 */
    {0x38ff, 0x20dd, "frstor", OP_FLOATNOPTR, 2, VAL_FLOAT},
    {0x38ff, 0x30dd, "fsave", OP_FLOATNOPTR, 2, VAL_FLOAT},
    {0x38ff, 0x38dd, "fstsw", OP_FLOATNOPTR, 2, VAL_FLOAT},

    /* Group 3 & 7*/
    {0xc0fb, 0xc0db, "esc", OP_ESC, 2, VAL_FLOAT},
    {0x38ff, 0x28db, "fld", OP_SIZEFLOATRM, 2, VAL_FLOAT},
    {0x38ff, 0x38db, "fstp", OP_SIZEFLOATRM, 2, VAL_FLOAT},

    /* Group 7 */
    {0x38ff, 0x20df, "fbld", OP_SIZEFLOATRM, 2, VAL_FLOAT},
    {0x38ff, 0x28df, "fild", OP_SIZEFLOATRM, 2, VAL_FLOAT},
    {0x38ff, 0x30df, "fbstp", OP_SIZEFLOATRM, 2, VAL_FLOAT},
    {0x38ff, 0x38df, "fistp", OP_SIZEFLOATRM, 2, VAL_FLOAT},

    /* Math, group 0,2,4,6 special RM 3*/
    {0xc0ff, 0xc0da, "esc", OP_ESC, 2, VAL_ALL},
    {0xf8ff, 0xc0de, "fadd", OP_FLOATMATH, 2, VAL_FLOAT},
    {0xf8ff, 0xc8de, "fmul", OP_FLOATMATH, 2, VAL_FLOAT},
    {0xf8ff, 0xd0de, "fcom", OP_FLOATRM, 2, VAL_FLOAT},
    {0xf8ff, 0xd8de, "esc", OP_ESC, 2, VAL_ALL},
    {0xf0ff, 0xe0de, "fsub", OP_FLOATMATH, 2, VAL_FLOAT},
    {0xf0ff, 0xf0de, "fdiv", OP_FLOATMATH, 2, VAL_FLOAT},

    /* Math, other */
    {0x38fb, 0x00d8, "fadd", OP_FLOATMATH, 2, VAL_FLOAT},
    {0x38fb, 0x08d8, "fmul", OP_FLOATMATH, 2, VAL_FLOAT},
    {0x38fb, 0x10d8, "fcom", OP_FLOATRM, 2, VAL_FLOAT},
    {0x38fb, 0x18d8, "fcomp", OP_FLOATRM, 2, VAL_FLOAT},
    {0x30fb, 0x20d8, "fsub", OP_FLOATMATH, 2, VAL_FLOAT},
    {0x30fb, 0x30d8, "fdiv", OP_FLOATMATH, 2, VAL_FLOAT},
    {0x38fb, 0x00da, "fiadd", OP_FLOATMATH, 2, VAL_FLOAT},
    {0x38fb, 0x08da, "fimul", OP_FLOATMATH, 2, VAL_FLOAT},
    {0x38fb, 0x10da, "ficom", OP_FLOATRM, 2, VAL_FLOAT},
    {0x38fb, 0x18da, "ficomp", OP_FLOATRM, 2, VAL_FLOAT},
    {0x30fb, 0x20da, "fisub", OP_FLOATMATH, 2, VAL_FLOAT},
    {0x30fb, 0x30da, "fidiv", OP_FLOATMATH, 2, VAL_FLOAT},

    /* groups 1, 3, 5, 7 */
    /* keep the follwing from going into error, RM3 */
    {0xe0f9, 0xc0d9, "esc", OP_ESC, 2, VAL_ALL},
    {0x38fb, 0x00d9, "fld", OP_FLOATRM, 2, VAL_FLOAT},
    {0x38fb, 0x10d9, "fst", OP_FLOATRM, 2, VAL_FLOAT},
    {0x38fb, 0x18d9, "fstp", OP_FLOATRM, 2, VAL_FLOAT},
    {0x38fb, 0x00db, "fild", OP_FLOATRM, 2, VAL_FLOAT},
    {0x38fb, 0x10db, "fist", OP_FLOATRM, 2, VAL_FLOAT},
    {0x38fb, 0x18db, "fistp", OP_FLOATRM, 2, VAL_FLOAT},

    /* Catch- all */
    {0x00f8, 0x00d8, "esc", OP_ESC, 2, VAL_ALL},
    {0, 0, 0, 0, 0, 0}};
static OPTABLE* indexes[] = {base0, base1, base2, base3, base4, base5, base6, base7};

OPTABLE* FindOpcode(BYTE* stream, uint validator)
{
    uint opcode;
    OPTABLE* table;

    if (*stream == 0x0f)
    {
        stream++;
        if (validator & VAL_VX)
            table = baseVX;
        else
            table = base386;
    }
    else if ((*stream & 0xf8) == 0xd8)
        table = floats;
    else
        table = indexes[*stream >> 5];

    opcode = *((uint*)stream);
    while (table->mask)
    {
        if ((validator & table->validator) && ((table->mask & opcode) == table->compare))
            return (table);
        table++;
    }
    return (0);
}
