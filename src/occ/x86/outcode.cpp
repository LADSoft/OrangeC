/* Software License Agreement
 *
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the
 *     Orange C "Target Code" exception.
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

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "be.h"

#include "Label.h"
#include "Fixup.h"
#include "Instruction.h"
#include "Section.h"
#include <map>

#include <new>
void * operator new(std::size_t n) throw(std::bad_alloc)
{
    return (void *)Alloc(n);
}
void operator delete(void * p) throw()
{
    // noop
}

#define FULLVERSION

extern "C" ARCH_ASM* chosenAssembler;
extern "C" MULDIV* muldivlink;
extern "C" ASMNAME oplst[];
extern "C" enum e_sg oa_currentSeg;
extern "C" DBGBLOCK* DbgBlocks[];
extern "C" SYMBOL* theCurrentFunc;
extern "C" int fastcallAlias;

LIST* includedFiles;

static char* segnames[] = { 0,         "code",     "data",     "bss",        "string",     "const",
"tls",     "cstartup", "crundown", "tlsstartup", "tlsrundown", "codefix",
"datafix", "lines",    "types",    "symbols",    "browse" };


int segAlignsDefault[] = { 1, 2, 8, 8, 2, 8, 8, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
int segAligns[sizeof(segAlignsDefault) / sizeof(segAlignsDefault[0])];

static int virtualSegmentNumber;
static int lastIncludeNum;

static std::map<int, Label *> labelMap;

static std::vector<Section *> virtuals;

static Section *sections[MAX_SEGS];

static Section *currentSection;

extern "C" void adjustUsesESP();
void outcode_file_init(void)
{
    int i;
    adjustUsesESP();
    lastIncludeNum = 0;
    virtualSegmentNumber = MAX_SEGS;
    labelMap.clear();
    memset(sections, 0, sizeof(sections));
    virtuals.clear();
    currentSection = 0;
}

/*-------------------------------------------------------------------------*/

void outcode_func_init(void)
{
    int i;
#define NOP 0x90

}

void compile_start(char* name)
{
    LIST* newItem = (LIST *)beGlobalAlloc(sizeof(LIST));
    newItem->data = beGlobalAlloc(strlen(name) + 1);
    strcpy((char *)newItem->data, name);

    includedFiles = newItem;
    oa_ini();
    o_peepini();
    omfInit();
}
void include_start(char* name, int num)
{
    if (num > lastIncludeNum)
    {
        LIST* newItem = (LIST *)beGlobalAlloc(sizeof(LIST));
        newItem->data = beGlobalAlloc(strlen(name) + 1);
        strcpy((char *)newItem->data, name);

        if (!includedFiles)
            includedFiles = newItem;
        else
        {
            LIST* l = includedFiles;
            while (l->next)
                l = l->next;
            l->next = newItem;
        }
        lastIncludeNum = num;
    }
}
/*-------------------------------------------------------------------------*/

void outcode_enterseg(e_sg seg)
{
    if (!sections[seg])
    {
        sections[seg] = new Section(segnames[seg], seg);
    }
    currentSection = sections[seg];
}

void InsertInstruction(Instruction *ins)
{
}
static Label *GetLabel(int lbl)
{
    Label *l = labelMap[lbl];
    if (!l)
    {
        char buf[256];
        sprintf(buf, "L_%d", lbl);
        std::string name = buf;
        l = new Label(name, labelMap.size(), 0);//currentSection()->GetSect());
        labelMap[lbl] = l;
    }
    return l;
}
void InsertLabel(int lbl)
{
    Label *l = GetLabel(lbl);
    //l->SetSect(currentSection()->GetSect());
    Instruction *newIns = new Instruction(l);
    InsertInstruction(newIns);
}

void InsertLine(LINEDATA* linedata) {  }
void InsertVarStart(SYMBOL* sp) { }
void InsertFunc(SYMBOL* sp, int start) { }
void InsertBlock(int start) { }

void emit(void* data, int len)
{
    Instruction *newIns = new Instruction((unsigned char *)data, len, true);
    InsertInstruction(newIns);
}
void emit(void *data, int len, Fixup *fixup, int fixofs)
{
    Instruction *newIns = new Instruction((unsigned char *)data, len, true);
    newIns->Add(fixup);
    fixup->SetInsOffs(fixofs);
    InsertInstruction(newIns);

}
void emit(Label *label)
{
    Instruction *newIns = new Instruction(label);
    InsertInstruction(newIns);
}

void emit(int size)
{
    //size < 0 = align > 0 = reserve
    if (size > 0)
    {
        Instruction *newIns = new Instruction(size, 1);
        InsertInstruction(newIns);
    }
    else
    {
        if (size < 0)
        {
            size = -size;
            Instruction*newIns = new Instruction(size);
            InsertInstruction(newIns);
        }
    }
}
/*-------------------------------------------------------------------------*/

Fixup * gen_symbol_fixup(SYMBOL* pub, int offset, bool PC)
{
    char buf[4096];
    beDecorateSymName(buf, pub);
    AsmExprNode *expr = new AsmExprNode(buf);
    if (offset)
    {
        AsmExprNode *expr1 = new AsmExprNode(offset);
        expr = new AsmExprNode(AsmExprNode::ADD, expr, expr1);
    }
    Fixup *f = new Fixup(expr, 4, !!PC, PC ? 4 : 0);
    return f;
}
Fixup *gen_label_fixup(int lab, int offset, bool PC)
{
    Label *l = GetLabel(lab);
    AsmExprNode *expr = new AsmExprNode(l->GetName());
    if (offset)
    {
        AsmExprNode *expr1 = new AsmExprNode(offset);
        expr = new AsmExprNode(AsmExprNode::ADD, expr, expr1);
    }
    Fixup *f = new Fixup(expr, 4, !!PC, PC ? 4 : 0);
    return f;
}
Fixup *gen_threadlocal_fixup(SYMBOL* tls, SYMBOL* base, int offset)
{
    char buf[4096];
    beDecorateSymName(buf, base);
    AsmExprNode *expr = new AsmExprNode(buf);
    beDecorateSymName(buf, tls);
    AsmExprNode *expr1 = new AsmExprNode(buf);
    expr = new AsmExprNode(AsmExprNode::SUB, expr1, expr);
    if (offset)
    {
        expr1 = new AsmExprNode(offset);
        expr = new AsmExprNode(AsmExprNode::ADD, expr, expr1);
    }
    Fixup *f = new Fixup(expr, 4, false);
    return f;

}
Fixup *gen_diff_fixup(int lab1, int lab2)
{
    Label *l = GetLabel(lab1);
    AsmExprNode *expr = new AsmExprNode(l->GetName());
    l = GetLabel(lab2);
    AsmExprNode *expr1 = new AsmExprNode(l->GetName());
    expr = new AsmExprNode(AsmExprNode::SUB, expr, expr1);
    Fixup *f = new Fixup(expr, 4, false);
    return f;

}
void outcode_dump_muldivval(void)
{
    MULDIV* v = muldivlink;
    UBYTE buf[10];
    while (v)
    {
        oa_align(8);
        InsertLabel(v->label);
        if (v->size == 0)
        {
            *(int*)buf = v->value;
            emit(buf, 4);
        }
        else if (v->size == ISZ_FLOAT || v->size == ISZ_IFLOAT)
        {
            FPFToFloat(buf, &v->floatvalue);
            emit(buf, 4);
        }
        else if (v->size == ISZ_DOUBLE || v->size == ISZ_IDOUBLE)
        {
            FPFToDouble(buf, &v->floatvalue);
            emit(buf, 8);
        }
        else
        {
            FPFToLongDouble(buf, &v->floatvalue);
            emit(buf, 10);
        }
        v = v->next;
    }
}

/*-------------------------------------------------------------------------*/

void outcode_genref(SYMBOL* sp, int offset)
{
    Fixup *f = gen_symbol_fixup(sp, offset, false);
    int i = 0;
    emit(&i, 4, f,0);

}

/*-------------------------------------------------------------------------*/

void outcode_gen_labref(int n)
{
    Fixup *f = gen_label_fixup(n, 0, false);
    int offset = 0;
    emit(&offset, 4, f,0);
}

/* the labels will already be resolved well enough by this point */
void outcode_gen_labdifref(int n1, int n2)
{
    Fixup *f = gen_diff_fixup(n1, n2);
    int offset = 0;
    emit(&offset, 4, f,0);
}

/*-------------------------------------------------------------------------*/

void outcode_gensrref(SYMBOL* sp, int val)
{
    Fixup *f = gen_symbol_fixup(sp, 0, false);
    char buf[8] = {};
    buf[1] = val;

    emit(&buf, 6, f,2);

}

/*-------------------------------------------------------------------------*/

void outcode_genstorage(int len)
{
    emit(len);
}

/*-------------------------------------------------------------------------*/

void outcode_genfloat(FPFC* val)
{
    UBYTE buf[4];
    FPFToFloat(buf, val);
    emit(buf, 4);
}

/*-------------------------------------------------------------------------*/

void outcode_gendouble(FPFC* val)
{
    UBYTE buf[8];
    FPFToDouble(buf, val);
    emit(buf, 8);
}

/*-------------------------------------------------------------------------*/

void outcode_genlongdouble(FPFC* val)
{
    UBYTE buf[10];
    FPFToLongDouble(buf, val);
    emit(buf, 10);
}

/*-------------------------------------------------------------------------*/

void outcode_genstring(LCHAR* string, int len)
{
    int i;
    for (i = 0; i < len; i++)
        outcode_genbyte(string[i]);
}

/*-------------------------------------------------------------------------*/

void outcode_genbyte(int val)
{
    char v = (char)val;
    emit(&v, 1);
}

/*-------------------------------------------------------------------------*/

void outcode_genword(int val)
{
    short v = (short)val;
    emit( &v, 2);
}

/*-------------------------------------------------------------------------*/

void outcode_genlong(int val) { emit(&val, 4); }

/*-------------------------------------------------------------------------*/

void outcode_genlonglong(LLONG_TYPE val)
{
    emit(&val, 8);
}

/*-------------------------------------------------------------------------*/

void outcode_align(int size)
{
    emit(-size);
}

/*-------------------------------------------------------------------------*/

void outcode_put_label(int lab)
{
    InsertLabel(lab);
}

/*-------------------------------------------------------------------------*/

void outcode_start_virtual_seg(SYMBOL* sp, int data)
{
    char buf[4096];
    if (data)
        strcpy(buf, "vsd@");
    else
        strcpy(buf, "vsc@");
    beDecorateSymName(buf + 3 + sp->decoratedName != @, sp);
    Section *sect = new Section()
    VIRTUAL_LIST* x;
    x = beGlobalAlloc(sizeof(VIRTUAL_LIST));
    x->sp = sp;
    x->seg = beGlobalAlloc(sizeof(EMIT_TAB));
    x->data = data;
    sp->value.i = virtualSegmentNumber++;
    if (virtualFirst)
        virtualLast = virtualLast->next = x;
    else
        virtualFirst = virtualLast = x;

}

/*-------------------------------------------------------------------------*/

void outcode_end_virtual_seg(SYMBOL* sp)
{
    outcode_enterseg(oa_currentSeg);
}

/*-------------------------------------------------------------------------*/

int resolveoffset(EXPRESSION* n, int* resolved)
{
    int rv = 0;
    if (n)
    {
        switch (n->type)
        {
            case en_sub:
                rv += resolveoffset(n->left, resolved);
                rv -= resolveoffset(n->right, resolved);
                break;
            case en_add:
                // case en_addstruc:
                rv += resolveoffset(n->left, resolved);
                rv += resolveoffset(n->right, resolved);
                break;
            case en_c_ll:
            case en_c_ull:
            case en_c_i:
            case en_c_c:
            case en_c_uc:
            case en_c_u16:
            case en_c_u32:
            case en_c_l:
            case en_c_ul:
            case en_c_ui:
            case en_c_wc:
            case en_c_s:
            case en_c_us:
            case en_absolute:
            case en_c_bool:
                rv += n->v.i;
                break;
            case en_auto:
                if (n->v.sp->storage_class == sc_parameter && fastcallAlias)
                {
                    if (!isstructured(basetype(theCurrentFunc->tp)->btp) || n->v.sp->offset != chosenAssembler->arch->retblocksize)
                    {

                        int m = n->v.sp->offset - fastcallAlias * chosenAssembler->arch->parmwidth;
                        if (m >= chosenAssembler->arch->retblocksize)
                        {
                            rv += m;
                        }
                    }
                    else
                    {
                        rv += n->v.sp->offset;
                    }
                }
                else
                {
                    rv += n->v.sp->offset;
                }
                break;
            case en_labcon:
            case en_global:
            case en_pc:
            case en_threadlocal:
                *resolved = 0;
                break;
            default:
                diag("Unexpected node type in resolveoffset");
                break;
        }
    }
    return rv;
}
AsmExprNode *MakeFixup(EXPRESSION *offset)
{
    int resolved = 1;
    int n = resolveoffset(offset, &resolved);
    if (!resolved)
    {
        AsmExprNode *rv;
        if (offset->type == en_sub && offset->left->type == en_threadlocal)
        {
            EXPRESSION * node = GetSymRef(offset->left);
            EXPRESSION *node1 = GetSymRef(offset->right);
            std::string name = node->v.sp->name;
            AsmExprNode *left = new AsmExprNode(name);
            name = node1->v.sp->name;
            AsmExprNode *right = new AsmExprNode(name);
            rv = new AsmExprNode(AsmExprNode::SUB, left, right);
        }
        else
        {
            EXPRESSION *node = GetSymRef(offset);
            std::string name = node->v.sp->decoratedName;
            rv = new AsmExprNode(name);
            
        }
        if (n)
        {
            rv = new AsmExprNode(AsmExprNode::ADD, rv, new AsmExprNode(n));
        }
        return rv;
    }
    else
    {
        return new AsmExprNode(n);
    }
}
void AddFixup(Instruction *newIns, OCODE *ins, const std::list<Numeric*>& operands)
{
    AMODE *amodes[4] = { ins->oper1, ins->oper2, ins->oper3 };
    AMODE **p = &amodes[0];
    if (ins->opcode == op_dd)
    {
        int resolved = 1;
        int n = resolveoffset( ins->oper1->offset, &resolved);
        if (!resolved)
        {
            memcpy(newIns->GetData(), &n, 4);
            AsmExprNode *expr = MakeFixup(ins->oper1->offset);
            Fixup *f = new Fixup(expr, 4, false);
            newIns->Add(f);
        }
    }
    else
    {
        for (auto operand : operands)
        {
            if (operand->used && operand->size)
            {
                if (newIns->Lost() && operand->pos)
                    operand->pos -= 8;
                int n = operand->relOfs;
                if (n < 0)
                    n = -n;
                Fixup* f = new Fixup(operand->node, (operand->size + 7) / 8, operand->relOfs != 0, n, operand->relOfs > 0);
                f->SetInsOffs((operand->pos + 7) / 8);
                newIns->Add(f);
                p++;
            }
        }
    }
}

void outcode_diag(OCODE *ins, char *str)
{
    char buf[512];
    sprintf(buf, "Assembly instruction %s: %s", opcodeTable[ins->opcode], str);
    diag(buf);
}
/*-------------------------------------------------------------------------*/

void outcode_AssembleIns(OCODE* ins)
{
    if (ins->opcode >= op_aaa)
    {
        Instruction *newIns = nullptr;
        std::list<Numeric*> operands;
        asmError err = InstructionParser::GetInstance()->GetInstruction(ins, newIns, operands);
        switch (err)
        {

        case AERR_NONE:
            AddFixup(newIns, ins, operands);
            InsertInstruction(newIns);
            ins->ins = newIns;
            break;
        case AERR_SYNTAX:
            outcode_diag(ins, "Syntax error while parsing instruction");
            break;
        case AERR_OPERAND:
            outcode_diag(ins, "Unknown operand");
            break;
        case AERR_BADCOMBINATIONOFOPERANDS:
            outcode_diag(ins, "Bad combination of operands");
            break;
        case AERR_UNKNOWNOPCODE:
            outcode_diag(ins, "Unrecognized opcode");
            break;
        case AERR_INVALIDINSTRUCTIONUSE:
            outcode_diag(ins, "Invalid use of instruction");
            break;
        default:
            outcode_diag(ins, "unknown assembler error");
            break;

        }
    }
    else
    {
        switch (ins->opcode)
        {
        case op_label:
            InsertLabel((int)ins->oper1);
            return;
        case op_line:
        case op_blockstart:
        case op_blockend:
        case op_varstart:
            break;
        case op_genword:
            outcode_genbyte(ins->oper1->offset->v.i);
            return;
            break;
        case op_dd:
        {
            int i = 0;
            Instruction *newIns = new Instruction(&i, 4, true);
            const std::list<Numeric*> operands;
            AddFixup(newIns, ins, operands);
            InsertInstruction(newIns);
            //reserve 4
            break;
        }
        default:
            break;
        }
    }
}

/*-------------------------------------------------------------------------*/


void outcode_gen(OCODE* peeplist)
{
    OCODE* head = peeplist;
    outcode_func_init();
    while (head)
    {
        outcode_AssembleIns(head);
        head = head->fwd;
    }
}
