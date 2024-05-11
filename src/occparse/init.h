#pragma once
/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
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

namespace Parser
{
typedef struct _startups_
{
    struct _startups_* next;
    char* name;
    int prio;
} STARTUP;

typedef struct _alias_
{
    struct _alias_* next;
    char* name;
    char* alias;
} ALIAS;

typedef struct _dyninit_
{
    struct _dyninit_* next;
    SYMBOL* sp;
    std::list<Initializer*>* init;
} DYNAMIC_Initializer;

extern bool initializingGlobalVar;
extern int ignore_global_init;


void init_init(void);
void CreateInlineConstructor(SYMBOL* sym);
void CreateInlineDestructor(SYMBOL* sym);
void dumpLits(void);
void dumpStartups(void);
void insert_file_constructor(SYMBOL* sym);
void insertDynamicInitializer(SYMBOL* sym, std::list<Initializer*>* init);
void insertDynamicDestructor(SYMBOL* sym, std::list<Initializer*>* init);
int dumpMemberPtr(SYMBOL* sym, Type* membertp, bool make_label);
int dumpInit(SYMBOL* sym, Initializer* init);
void dumpInitGroup(SYMBOL* sym, Type* tp);
void dumpInitializers(void);
void insertInitSym(SYMBOL* sym);
Initializer* initInsert(std::list<Initializer*>** pos, Type* tp, EXPRESSION* exp, int offset, bool noassign);
ExpressionNode referenceTypeError(Type* tp, EXPRESSION* exp);
EXPRESSION* getThisNode(SYMBOL* sym);
void RecalculateVariableTemplateInitializers(std::list<Initializer*>::iterator& ilbegin, std::list<Initializer*>::iterator& ilend, std::list<Initializer*>** out, Type* tp, int offset);
void ReplaceVarRef(EXPRESSION** exp, SYMBOL* name, SYMBOL* newName);
void ReplaceVarRef(EXPRESSION** exp, SYMBOL* name, EXPRESSION* newName);

LexList* initType(LexList* lex, SYMBOL* funcsp, int offset, StorageClass sc, std::list<Initializer*>** init, std::list<Initializer*>** dest, Type* itype,
    SYMBOL* sym, bool arrayMember, int flags);

LexList* initialize(LexList* lex, SYMBOL* funcsp, SYMBOL* sym, StorageClass storage_class_in, bool asExpression, bool inTemplate,
                    int flags);
}  // namespace Parser