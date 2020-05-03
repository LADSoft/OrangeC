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
        INITIALIZER* init;
    } DYNAMIC_INITIALIZER;

    extern bool initializingGlobalVar;
    extern int ignore_global_init;

    void init_init(void);
    EXPRESSION* stringlit(STRING* s);
    int genstring(STRING* str);
    void dumpLits(void);
    void dumpStartups(void);
    void insertDynamicInitializer(SYMBOL* sym, INITIALIZER* init);
    void insertDynamicDestructor(SYMBOL* sym, INITIALIZER* init);
    int dumpMemberPtr(SYMBOL* sym, TYPE* membertp, bool make_label);
    int dumpInit(SYMBOL* sym, INITIALIZER* init);
    bool IsConstWithArr(TYPE* tp);
    void dumpInitGroup(SYMBOL* sym, TYPE* tp);
    void dumpInitializers(void);
    void insertInitSym(SYMBOL* sym);
    INITIALIZER* initInsert(INITIALIZER** pos, TYPE* tp, EXPRESSION* exp, int offset, bool noassign);
    enum e_node referenceTypeError(TYPE* tp, EXPRESSION* exp);
    EXPRESSION* createTemporary(TYPE* tp, EXPRESSION* val);
    EXPRESSION* msilCreateTemporary(TYPE* tp, EXPRESSION* val);
    EXPRESSION* getThisNode(SYMBOL* sym);
    LEXEME* initType(LEXEME* lex, SYMBOL* funcsp, int offset, enum e_sc sc, INITIALIZER** init, INITIALIZER** dest, TYPE* itype,
        SYMBOL* sym, bool arrayMember, int flags);
    bool checkconstexprfunc(EXPRESSION* node);
    bool IsConstantExpression(EXPRESSION* node, bool allowParams, bool allowFunc);
    bool InitVariableMatches(SYMBOL* left, SYMBOL* right);
    void RecalculateVariableTemplateInitializers(INITIALIZER** in, INITIALIZER*** out, TYPE* tp, int offset);
    LEXEME* initialize(LEXEME* lex, SYMBOL* funcsp, SYMBOL* sym, enum e_sc storage_class_in, bool asExpression, int flags);
}