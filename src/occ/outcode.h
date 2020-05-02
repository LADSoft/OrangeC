#pragma once
/* Software License Agreement
 *
 *     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
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
extern int segAligns[MAX_SEGS];
extern int dbgblocknum;

void * operator new(std::size_t n) throw(std::bad_alloc);
void operator delete(void * p) throw();
void omfInit(void);
void dbginit(void);
void outcode_file_init(void);
void outcode_func_init(void);
void omf_globaldef(SimpleSymbol* sym);
void omf_put_extern(SimpleSymbol* sym, int code);
void omf_put_impfunc(SimpleSymbol* sym, const char* file);
void omf_put_expfunc(SimpleSymbol* sym);
void omf_put_includelib(const char* name);
void Release(void);
void output_obj_file(void);
void compile_start(char* name);
void outcode_enterseg(int seg);
void InsertInstruction(Instruction* ins);
void outcode_gen_strlab(SimpleSymbol* sym);
void InsertLabel(int lbl);
void emit(void* data, int len);
void outcode_dump_muldivval(void);
void outcode_genref(SimpleSymbol* sym, int offset);
void outcode_gen_labref(int n);
void outcode_gen_labdifref(int n1, int n2);
void outcode_gensrref(SimpleSymbol* sym, int val);
void outcode_genstorage(int len);
void outcode_genfloat(FPF* val);
void outcode_gendouble(FPF* val);
void outcode_genlongdouble(FPF* val);
void outcode_genstring(char* string, int len);
void outcode_genbyte(int val);
void outcode_genword(int val);
void outcode_genlong(int val);
void outcode_genlonglong(long long val);
void outcode_align(int size);
void outcode_put_label(int lab);
void outcode_start_virtual_seg(SimpleSymbol* sym, int data);
void outcode_end_virtual_seg(SimpleSymbol* sym);
void InsertAttrib(ATTRIBDATA* ad);
void InsertLine(LINEDATA* linedata);
void InsertVarStart(SimpleSymbol* sym);
void InsertFunc(SimpleSymbol* sym, int start);
void InsertBlock(int start);
void AddFixup(Instruction* newIns, OCODE* ins, const std::list<Numeric*>& operands);
void outcode_diag(OCODE* ins, const char* str);
void outcode_AssembleIns(OCODE* ins);
void outcode_gen(OCODE* peeplist);
