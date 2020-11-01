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
#define CT_NONE 0
#define CT_CONS 1
#define CT_DEST 2
namespace Parser
{

extern int inDefaultParam;
extern char deferralBuf[100000];
extern SYMBOL* enumSyms;
extern STRUCTSYM* structSyms;
extern int expandingParams;
extern int noSpecializationError;
extern Optimizer::LIST* deferred;
extern int structLevel;
extern Optimizer::LIST* openStructs;
extern int parsingTrailingReturn;


void declare_init(void);
void InsertGlobal(SYMBOL* sp);
void WeedExterns(void);
const char* AnonymousName(void);
const char* AnonymousTypeName(void);
SYMBOL* SymAlloc(void);
SYMBOL* makeID(enum e_sc storage_class, TYPE* tp, SYMBOL* spi, const char* name);
SYMBOL* makeUniqueID(enum e_sc storage_class, TYPE* tp, SYMBOL* spi, const char* name);
void addStructureDeclaration(STRUCTSYM* decl);
void addTemplateDeclaration(STRUCTSYM* decl);
void dropStructureDeclaration(void);
SYMBOL* getStructureDeclaration(void);
void InsertSymbol(SYMBOL* sp, enum e_sc storage_class, enum e_lk linkage, bool allowDups);
LEXEME* tagsearch(LEXEME* lex, char* name, SYMBOL** rsp, HASHTABLE** table, SYMBOL** strSym_out, NAMESPACEVALUELIST** nsv_out,
                  enum e_sc storage_class);
LEXEME* get_type_id(LEXEME* lex, TYPE** tp, SYMBOL* funcsp, enum e_sc storage_class, bool beforeOnly, bool toErr, bool inUsing);
SYMBOL* calculateStructAbstractness(SYMBOL* top, SYMBOL* sp);
void calculateStructOffsets(SYMBOL* sp);
void resolveAnonymousUnions(SYMBOL* sp);
LEXEME* innerDeclStruct(LEXEME* lex, SYMBOL* funcsp, SYMBOL* sp, bool inTemplate, enum e_ac defaultAccess, bool isfinal,
                        bool* defd);
void sizeQualifiers(TYPE* tp);
LEXEME* parse_declspec(LEXEME* lex, enum e_lk* linkage, enum e_lk* linkage2, enum e_lk* linkage3);
LEXEME* getQualifiers(LEXEME* lex, TYPE** tp, enum e_lk* linkage, enum e_lk* linkage2, enum e_lk* linkage3, bool* asFriend);
LEXEME* getBeforeType(LEXEME* lex, SYMBOL* funcsp, TYPE** tp, SYMBOL** spi, SYMBOL** strSym, NAMESPACEVALUELIST** nsv,
                      bool inTemplate, enum e_sc storage_class, enum e_lk* linkage, enum e_lk* linkage2, enum e_lk* linkage3,
                      bool asFriend, int consdest, bool beforeOnly, bool funcptr);
LEXEME* getBasicType(LEXEME* lex, SYMBOL* funcsp, TYPE** tp, SYMBOL** strSym_out, bool inTemplate, enum e_sc storage_class,
                     enum e_lk* linkage_in, enum e_lk* linkage2_in, enum e_lk* linkage3_in, enum e_ac access, bool* notype,
                     bool* defd, int* consdest, bool* templateArg, bool isTypedef, bool templateErr, bool inUsing);
void injectThisPtr(SYMBOL* sp, HASHTABLE* syms);
bool intcmp(TYPE* t1, TYPE* t2);
LEXEME* getFunctionParams(LEXEME* lex, SYMBOL* funcsp, SYMBOL** spin, TYPE** tp, bool inTemplate, enum e_sc storage_class);
LEXEME* getDeferredData(LEXEME* lex, LEXEME** savePos, bool braces);

LEXEME* declare(LEXEME* lex, SYMBOL* funcsp, TYPE** tprv, enum e_sc storage_class, enum e_lk defaultLinkage, BLOCKDATA* block,
                bool needsemi, int asExpression, bool inTemplate, enum e_ac access);
}  // namespace Parser