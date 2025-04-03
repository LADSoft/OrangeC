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

#pragma once

namespace occx86
{
extern int skipsize;
extern int addsize;

/*      variable initialization         */

extern char segregs[];

extern MULDIV* muldivlink;
extern enum Optimizer::e_sg oa_currentSeg; /* Current seg */
extern int newlabel;
extern int needpointer;

void oa_ini(void);
void oa_nl(void);
void outop(const char* name);
void putop(enum e_opcode op, AMODE* aps, AMODE* apd, int nooptx);
void oa_putconst(int op, int sz, Optimizer::SimpleExpression* offset, bool doSign);
void oa_putlen(int l);
void putsizedreg(const char* string, int reg, int size);
void pointersize(int size);
void putseg(int seg, int usecolon);
int islabeled(Optimizer::SimpleExpression* n);
void oa_putamode(int op, int szalt, AMODE* ap);
void oa_put_code(OCODE* cd);
void oa_gen_strlab(Optimizer::SimpleSymbol* sym);
void oa_put_label(int lab);
void oa_put_string_label(int lab, int type);
void oa_genfloat(enum Optimizer::e_gt type, FPF* val);
void oa_genstring(char* str, int len);
void oa_genint(enum Optimizer::e_gt type, long long val);
void oa_genaddress(unsigned long long val);
void oa_gensrref(Optimizer::SimpleSymbol* sym, int val, int type);
void oa_genref(Optimizer::SimpleSymbol* sym, int offset);
void oa_genpcref(Optimizer::SimpleSymbol* sym, int offset);
void oa_genstorage(int nbytes);
void oa_gen_labref(int n);
void oa_gen_labdifref(int n1, int n2);
void oa_exitseg(enum Optimizer::e_sg seg);
void oa_enterseg(enum Optimizer::e_sg seg);
void oa_gen_virtual(Optimizer::SimpleSymbol* sym, int data);
void oa_gen_endvirtual(Optimizer::SimpleSymbol* sym);
void oa_align(int size);
void oa_setalign(int code, int data, int bss, int constant);
long queue_muldivval(long number);
long queue_large_const(unsigned constant[], int count);
long queue_floatval(FPF* number, int size);
void dump_muldivval(void);
void oa_header(const char* filename, const char* compiler_version);
void oa_trailer(void);
void oa_localdef(Optimizer::SimpleSymbol* sym);
void oa_localstaticdef(Optimizer::SimpleSymbol* sym);
void oa_globaldef(Optimizer::SimpleSymbol* sym);
void oa_output_alias(char* name, char* alias);
void oa_put_extern(Optimizer::SimpleSymbol* sym, int code);
void oa_put_impfunc(Optimizer::SimpleSymbol* sym, const char* file);
void oa_put_expfunc(Optimizer::SimpleSymbol* sym);
void oa_output_includelib(const char* name);
void oa_end_generation(void);
}  // namespace occx86