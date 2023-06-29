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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <climits>
#include "be.h"
#include "DotNetPELib.h"
#include <vector>
#include <cassert>
#include "config.h"
#include "ildata.h"
#include "occil.h"
#include "MsilProcess.h"
#include "ildata.h"
#include "msilInit.h"
#include "symfuncs.h"
#include "OptUtils.h"
#include "using.h"
#include "ioptimizer.h"
#include "Delegate.h"
#include "Utils.h"
#include "gen.h"

using namespace DotNetPELib;
namespace occmsil
{
#define MAX_ALIGNS 50
MethodSignature* FindPInvokeWithVarargs(std::string name, std::list<Param*>::iterator begin, std::list<Param*>::iterator end,
                                        size_t size);

struct swlist
{
    struct swlist* next;
    int lab;
};

static int fstackid;
static int inframe;
static int switch_deflab;
static long long switch_range, switch_case_count, switch_case_max;
static Optimizer::IMODE* switch_ip;
static Operand* switch_ip_a;
static enum { swm_enumerate, swm_compactstart, swm_compact, swm_tree } switch_mode;
static int switch_lastcase;
static int *switchTreeLabels, *switchTreeBranchLabels;
static long long* switchTreeCases;
static int switchTreeLabelCount;
static int switchTreePos;
static int returnCount;
static int hookCount;
static int stackpos = 0;

void box(Optimizer::IMODE* im);

void include_start(char* name, int num) {}

void increment_stack(void) { ++stackpos; }
void decrement_stack(void) { --stackpos; }
Instruction* gen_code(Instruction::iop op, Operand* operand)
{
    Instruction* i = peLib->AllocateInstruction(op, operand);
    currentMethod->AddInstruction(i);
    return i;
}
void msil_oa_gen_label(int labno)
/*
 *      add a compiler generated label to the peep list.
 */
{
    char buf[256];
    sprintf(buf, "L_%d_%x", labno, uniqueId);
    Instruction* i = peLib->AllocateInstruction(Instruction::i_label, peLib->AllocateOperand(buf));
    currentMethod->AddInstruction(i);
}

Operand* make_constant(int sz, Optimizer::SimpleExpression* exp)
{
    Operand* operand = NULL;
    if (isintconst(exp))
    {
        operand = peLib->AllocateOperand((longlong)exp->i, Operand::any);
    }
    else if (isfloatconst(exp))
    {
        double a;
        exp->f.ToDouble((unsigned char*)&a);
        operand = peLib->AllocateOperand(a, Operand::any);
    }
    else if (exp->type == Optimizer::se_structelem)
    {
        if (exp->sp->isproperty)
        {
            operand = peLib->AllocateOperand(GetFieldData(exp->sp));
            operand->Property(true);
        }
        else
        {
            Value* field = GetStructField(exp->sp);
            operand = peLib->AllocateOperand(field);
        }
    }
    else if (exp->type == Optimizer::se_labcon)
    {
        Value* field = GetStringFieldData(exp->i, exp->altData->i);
        operand = peLib->AllocateOperand(field);
    }
    else if (exp->type == Optimizer::se_string)
    {
        // dotnetpelib currently doesn't support wide characters
        char value[50000], *p = value;
        value[0] = 0;
        if (exp->astring.str)
        {
            int i;
            for (i = 0; i < exp->astring.len; i++)
            {
                *p++ = exp->astring.str[i];
            }
            *p = 0;
        }
        // operand is a string
        operand = peLib->AllocateOperand(value, true);
    }
    else if (exp->type == Optimizer::se_labcon)
    {
        Value* field = GetStringFieldData(exp->i, exp->altData->i);
        operand = peLib->AllocateOperand(field);
    }
    else if (exp->type == Optimizer::se_auto)
    {
        operand = peLib->AllocateOperand(GetLocalData(exp->sp));
    }
    else if (exp->sp->tp->type == Optimizer::st_func)
    {
        operand = peLib->AllocateOperand(peLib->AllocateMethodName(GetMethodSignature(exp->sp)));
    }
    else
    {
        operand = peLib->AllocateOperand(GetFieldData(exp->sp));
        if (exp->sp->isproperty)
            operand->Property(true);
    }
    return operand;
}
/*-------------------------------------------------------------------------*/
bool isauto(Optimizer::SimpleExpression* ep)
{
    if (ep->type == Optimizer::se_auto)
        return true;
    if (ep->type == Optimizer::se_add)
        return isauto(ep->left) || isauto(ep->right);
    if (ep->type == Optimizer::se_sub)
        return isauto(ep->left);
    return false;
}
void msil_oa_gen_vtt(int dataOffset, Optimizer::SimpleSymbol* func) {}
void msil_oa_gen_vc1(Optimizer::SimpleSymbol* func) {}
void msil_oa_gen_importThunk(Optimizer::SimpleSymbol* func) {}
Operand* getCallOperand(Optimizer::QUAD* q, bool& virt)
{
    Optimizer::SimpleExpression* en = GetSymRef(q->dc.left->offset);

    Operand* operand;
    MethodSignature* sig;
    if (q->dc.left->mode == Optimizer::i_immed)
    {
        Optimizer::SimpleSymbol* sp = en->sp;
        if (!msil_managed(sp))
        {
            sig = GetMethodSignature(sp);
            Optimizer::ArgList* valist = q->altargs;
            int n = sig->ParamCount();
            while (n-- && valist)
                valist = valist->next;
            while (valist)
            {
                sig->AddVarargParam(peLib->AllocateParam("", GetType(valist->tp, true, true, true)));
                valist = valist->next;
            }
            MethodSignature* oldsig = FindPInvokeWithVarargs(sig->Name(), sig->vbegin(), sig->vend(), sig->VarargParamCount());
            if (oldsig)
            {
                sig = oldsig;
            }
            else
            {
                pInvokeReferences.insert(std::pair<std::string, MethodSignature*>(sig->Name(), sig));
            }
        }
        else
        {
            if (sp->storage_class == Optimizer::scc_virtual)
                virt = true;
            if (sp->msil)
            {

                sig = ((Method*)sp->msil)->Signature();
            }
            else
            {
                sig = GetMethodSignature(sp);
            }
        }
    }
    else
    {
        Optimizer::SimpleSymbol* sp = q->altsp;
        sig = GetMethodSignature(sp->tp, false);
        sig->SetName("");  // for calli instruction
    }
    operand = peLib->AllocateOperand(peLib->AllocateMethodName(sig));
    return operand;
}
Operand* getOperand(Optimizer::IMODE* oper)
{
    Operand* rv = NULL;
    switch (oper->mode)
    {
        case Optimizer::i_immed:
            if (oper && oper->mode == Optimizer::i_immed && oper->offset->type == Optimizer::se_msil_array_access)
            {
                Type* tp = GetType(oper->offset->msilArrayTP, true);
                if (tp->ArrayLevel() == 1)
                {
                    Operand* operand = NULL;
                    Instruction::iop instructions[] = {
                        Instruction::i_ldelem,    Instruction::i_ldelem,    Instruction::i_ldelem,    Instruction::i_ldelem_u1,
                        Instruction::i_ldelem_u2, Instruction::i_ldelem_i1, Instruction::i_ldelem_u1, Instruction::i_ldelem_i2,
                        Instruction::i_ldelem_u2, Instruction::i_ldelem_i4, Instruction::i_ldelem_u4, Instruction::i_ldelem_i8,
                        Instruction::i_ldelem_u8, Instruction::i_ldelem_i,  Instruction::i_ldelem_i,  Instruction::i_ldelem_r4,
                        Instruction::i_ldelem_r8, Instruction::i_ldelem,    Instruction::i_ldelem};
                    if (instructions[tp->GetBasicType()] == Instruction::i_ldelem)
                    {
                        operand = peLib->AllocateOperand(peLib->AllocateValue("", GetType(oper->offset->msilArrayTP->btp, true)));
                    }
                    gen_code(instructions[tp->GetBasicType()], operand);
                    decrement_stack();
                }
                else
                {
                    MethodSignature* sig = LookupArrayMethod(tp, "Get");
                    Operand* operand = peLib->AllocateOperand(peLib->AllocateMethodName(sig));
                    gen_code(Instruction::i_call, operand);
                    int n = tp->ArrayLevel();
                    for (int i = 0; i < n; i++)
                        decrement_stack();
                }
            }
            else
            {
                rv = make_constant(oper->size, oper->offset);
            }
            break;
        case Optimizer::i_direct: {
            if (oper->offset->type == Optimizer::se_structelem)
            {
                if (oper->offset->sp->isproperty)
                {
                    rv = peLib->AllocateOperand(GetFieldData(oper->offset->sp));
                    rv->Property(true);
                }
                else
                {
                    Value* field = GetStructField(oper->offset->sp);
                    rv = peLib->AllocateOperand(field);
                }
            }
            else
            {
                Optimizer::SimpleExpression* en = GetSymRef(oper->offset);
                Optimizer::SimpleSymbol* sp = NULL;
                if (en && en->type != Optimizer::se_labcon)
                {
                    sp = en->sp;
                }
                else if (oper->offset->type == Optimizer::se_tempref)
                {
                    sp = (Optimizer::SimpleSymbol*)oper->offset->right;
                }
                if (sp)
                {
                    if (sp->storage_class == Optimizer::scc_auto || sp->storage_class == Optimizer::scc_register ||
                        sp->storage_class == Optimizer::scc_parameter)
                    {
                        rv = peLib->AllocateOperand(GetLocalData(sp));
                    }
                    else
                    {
                        rv = peLib->AllocateOperand(GetFieldData(sp));
                        if (sp->isproperty)
                            rv->Property(true);
                    }
                }
                else if (oper->offset->type == Optimizer::se_labcon)
                {
                    Value* field = GetStringFieldData(oper->offset->i, oper->offset->altData->i);
                    rv = peLib->AllocateOperand(field);
                }
                else if (oper->offset->type != Optimizer::se_tempref)
                {
                    if (oper->offset->type == Optimizer::se_auto && oper->offset->sp->msilObjectArray)
                    {
                        Type* oa = peLib->AllocateType(Type::object, 0);
                        oa->ArrayLevel(1);
                        rv = peLib->AllocateOperand(peLib->AllocateValue("", oa));
                    }
                    else
                    {
                        diag("Invalid load node");
                    }
                }
            }
            break;
        }
    }
    return rv;
}

void load_ind(Optimizer::IMODE* im)
{
    int sz = im->size;
    Instruction::iop op;
    Operand* operand = nullptr;
    switch (sz)
    {
        case ISZ_BOOLEAN:
        case ISZ_UCHAR:
            op = Instruction::i_ldind_u1;
            break;
        case -ISZ_UCHAR:
            op = Instruction::i_ldind_i1;
            break;
        case ISZ_USHORT:
        case ISZ_WCHAR:
        case ISZ_U16:
            op = Instruction::i_ldind_u2;
            break;
        case -ISZ_USHORT:
            op = Instruction::i_ldind_i2;
            break;
        case ISZ_UINT:
        case ISZ_ULONG:
        case ISZ_U32:
        case ISZ_UNATIVE:
            op = Instruction::i_ldind_u4;
            break;
        case -ISZ_UINT:
        case -ISZ_ULONG:
        case -ISZ_UNATIVE:
            op = Instruction::i_ldind_i4;
            break;
        case ISZ_ULONGLONG:
            op = Instruction::i_ldind_u8;
            break;
        case -ISZ_ULONGLONG:
            op = Instruction::i_ldind_i8;
            break;
        case ISZ_ADDR: {
            Operand* oper = currentMethod->LastInstruction()->GetOperand();
            // check for __va_arg__ on a pointer type
            if (oper && oper->OperandType() == Operand::t_value && typeid(*oper->GetValue()) == typeid(MethodName))
            {
                MethodSignature* test = ((MethodName*)oper->GetValue())->Signature();
                if (test->GetContainer() == ptrUnbox->GetContainer())
                    if (test->Name() == ptrUnbox->Name())
                        return;
            }
            op = Instruction::i_ldind_i;
            break;
        }
        /* */
        case ISZ_FLOAT:
            op = Instruction::i_ldind_r4;
            break;
        case ISZ_DOUBLE:
        case ISZ_LDOUBLE:
            op = Instruction::i_ldind_r8;
            break;

        case ISZ_IFLOAT:
            op = Instruction::i_ldind_r4;
            break;
        case ISZ_IDOUBLE:
        case ISZ_ILDOUBLE:
            op = Instruction::i_ldind_r8;
            break;

        case ISZ_CFLOAT:
        case ISZ_CDOUBLE:
        case ISZ_CLDOUBLE:
            break;
        case ISZ_OBJECT:
            op = Instruction::i_ldobj;
            operand = peLib->AllocateOperand(peLib->AllocateValue("", GetType(im->offset->sp->tp, true, 0, 0)));
            break;
        case ISZ_STRING:
            op = Instruction::i_ldobj;
            operand = peLib->AllocateOperand(peLib->AllocateValue("", peLib->AllocateType(Type::string, 0)));
            break;
    }
    gen_code(op, operand);
}
void store_ind(Optimizer::IMODE* im)
{
    int sz = im->size;
    Instruction::iop op;
    Operand* operand = nullptr;
    if (sz < 0)
        sz = -sz;
    switch (sz)
    {
        case ISZ_BOOLEAN:
        case ISZ_UCHAR:
            op = Instruction::i_stind_i1;
            break;
        case ISZ_USHORT:
        case ISZ_WCHAR:
        case ISZ_U16:
            op = Instruction::i_stind_i2;
            break;
        case ISZ_UINT:
        case ISZ_ULONG:
        case ISZ_U32:
        case ISZ_UNATIVE:
            op = Instruction::i_stind_i4;
            break;
        case ISZ_ULONGLONG:
            op = Instruction::i_stind_i8;
            break;
        case ISZ_ADDR:
            op = Instruction::i_stind_i;
            break;
        /* */
        case ISZ_FLOAT:
            op = Instruction::i_stind_r4;
            break;
        case ISZ_DOUBLE:
        case ISZ_LDOUBLE:
            op = Instruction::i_stind_r8;
            break;

        case ISZ_IFLOAT:
            op = Instruction::i_stind_r4;
            break;
        case ISZ_IDOUBLE:
        case ISZ_ILDOUBLE:
            op = Instruction::i_stind_r8;
            break;

        case ISZ_CFLOAT:
        case ISZ_CDOUBLE:
        case ISZ_CLDOUBLE:
            break;
        case ISZ_STRING:
            op = Instruction::i_stobj;
            operand = peLib->AllocateOperand(peLib->AllocateValue("", peLib->AllocateType(Type::string, 0)));
            break;
        case ISZ_OBJECT:
            op = Instruction::i_stobj;
            operand = peLib->AllocateOperand(peLib->AllocateValue("", GetType(im->offset->sp->tp, true, 0, 0)));
            break;
    }
    gen_code(op, operand);
    decrement_stack();
    decrement_stack();
}

void load_arithmetic_constant(int sz, Operand* operand)
{
    Instruction::iop op;
    int sz1 = sz < 0 ? -sz : sz;
    switch (sz1)
    {
        case 0:
        case ISZ_BOOLEAN:
        case ISZ_UCHAR:
        case ISZ_USHORT:
        case ISZ_WCHAR:
        case ISZ_U16:
        case ISZ_UINT:
        case ISZ_ULONG:
        case ISZ_U32:
        case ISZ_UNATIVE:
            op = Instruction::i_ldc_i4;
            break;
        case ISZ_ULONGLONG:
            op = Instruction::i_ldc_i8;
            break;
        case ISZ_ADDR:
            op = Instruction::i_ldc_i4;
            break;
        /* */
        case ISZ_FLOAT:
            op = Instruction::i_ldc_r4;
            break;
        case ISZ_DOUBLE:
        case ISZ_LDOUBLE:
            op = Instruction::i_ldc_r8;
            break;

        case ISZ_IFLOAT:
            op = Instruction::i_ldc_r4;
            break;
        case ISZ_IDOUBLE:
        case ISZ_ILDOUBLE:
            op = Instruction::i_ldc_r8;
            break;

        case ISZ_CFLOAT:
        case ISZ_CDOUBLE:
        case ISZ_CLDOUBLE:
            break;
    }
    gen_code(op, operand);
    increment_stack();
}
void load_constant(int sz, Optimizer::SimpleExpression* exp)
{
    int sz1;
    Instruction::iop op;
    sz1 = sz;
    if (sz < 0)
        sz1 = -sz;
    Operand* operand = make_constant(sz1, exp);
    switch (sz1)
    {
        case 0:
        case ISZ_BOOLEAN:
        case ISZ_UCHAR:
        case ISZ_USHORT:
        case ISZ_WCHAR:
        case ISZ_U16:
        case ISZ_UINT:
        case ISZ_ULONG:
        case ISZ_U32:
        case ISZ_UNATIVE:
            op = Instruction::i_ldc_i4;
            break;
        case ISZ_ULONGLONG:
            op = Instruction::i_ldc_i8;
            break;
        case ISZ_ADDR:
            op = Instruction::i_ldc_i4;
            break;
        /* */
        case ISZ_FLOAT:
            op = Instruction::i_ldc_r4;
            break;
        case ISZ_DOUBLE:
        case ISZ_LDOUBLE:
            op = Instruction::i_ldc_r8;
            break;

        case ISZ_IFLOAT:
            op = Instruction::i_ldc_r4;
            break;
        case ISZ_IDOUBLE:
        case ISZ_ILDOUBLE:
            op = Instruction::i_ldc_r8;
            break;

        case ISZ_CFLOAT:
        case ISZ_CDOUBLE:
        case ISZ_CLDOUBLE:
            break;
    }
    gen_code(op, operand);
    increment_stack();
}
void gen_load(Optimizer::IMODE* im, Operand* dest, bool retval)
{
    if (dest && dest->OperandType() == Operand::t_string)
    {
        gen_code(Instruction::i_ldstr, dest);
        increment_stack();
        return;
    }
    if (im->mode == Optimizer::i_ind)
    {
        if (im->fieldname)
        {
            Optimizer::SimpleExpression* offset = (Optimizer::SimpleExpression*)im->vararg;
            if (qualifiedStruct(offset->sp->parentClass))
            {
                if (offset->sp->isproperty)
                {
                    void* result;
                    peLib->Find(offset->sp->msil, &result);
                    Property* p = static_cast<Property*>(result);
                    p->CallGet(*peLib, currentMethod);
                }
                else
                {
                    Value* field = GetStructField(offset->sp);
                    Operand* operand = peLib->AllocateOperand(field);
                    gen_code(Instruction::i_ldfld, operand);
                }
            }
            else
            {
                load_ind(im);
            }
        }
        else
        {
            load_ind(im);
        }
        return;
    }
    if (!dest)
        return;
    switch (dest->OperandType())
    {

        case Operand::t_int:
        case Operand::t_real:
            load_arithmetic_constant(im->size, dest);
            break;
        case Operand::t_value:
            if (typeid(*dest->GetValue()) == typeid(Local))
            {
                Local* l = static_cast<Local*>(dest->GetValue());
                Type* t = l->GetType();
                bool address =
                    t->GetBasicType() == Type::cls && (t->GetClass()->Flags().Flags() & Qualifiers::Value) && !t->ArrayLevel();
                if (im->mode == Optimizer::i_immed && (!im->msilObject || address) && !retval)
                    gen_code(Instruction::i_ldloca, dest);
                else
                    gen_code(Instruction::i_ldloc, dest);
                increment_stack();
            }
            else if (typeid(*dest->GetValue()) == typeid(Param))
            {
                Param* p = static_cast<Param*>(dest->GetValue());
                Type* t = p->GetType();
                bool address =
                    t->GetBasicType() == Type::cls && (t->GetClass()->Flags().Flags() & Qualifiers::Value) && !t->ArrayLevel();
                if (im->mode == Optimizer::i_immed && (!im->msilObject || address) && !retval)
                    gen_code(Instruction::i_ldarga, dest);
                else
                    gen_code(Instruction::i_ldarg, dest);
                increment_stack();
            }
            else if (typeid(*dest->GetValue()) == typeid(MethodName))
            {
                if (Optimizer::delegateforfuncptr)
                {
                    gen_code(Instruction::i_ldnull, nullptr);
                }
                gen_code(Instruction::i_ldftn, dest);
                if (Optimizer::delegateforfuncptr)
                {
                    Operand* ctor;
                    Operand* operand = GetDelegateAllocator(im->offset->sp->tp, ctor);
                    gen_code(Instruction::i_newobj, ctor);
                    gen_code(Instruction::i_call, operand);
                }
                increment_stack();
            }
            else  // fieldname
            {
                if (dest->Property())
                {
                    Property* p = reinterpret_cast<Property*>(static_cast<FieldName*>(dest->GetValue())->GetField());
                    p->CallGet(*peLib, currentMethod);
                }
                else if (im->offset->type == Optimizer::se_structelem)
                {
                    FieldName* f = static_cast<FieldName*>(dest->GetValue());
                    Type* t = f->GetField()->FieldType();
                    bool address =
                        t->GetBasicType() == Type::cls && (t->GetClass()->Flags().Flags() & Qualifiers::Value) && !t->ArrayLevel();
                    if (im->mode == Optimizer::i_immed && (!im->msilObject || address) && !retval)
                    {
                        gen_code(Instruction::i_ldflda, dest);
                    }
                    else
                    {
                        gen_code(Instruction::i_ldfld, dest);
                    }
                }
                else
                {
                    FieldName* f = static_cast<FieldName*>(dest->GetValue());
                    Type* t = f->GetField()->FieldType();
                    bool address =
                        t->GetBasicType() == Type::cls && (t->GetClass()->Flags().Flags() & Qualifiers::Value) && !t->ArrayLevel();
                    if (im->mode == Optimizer::i_immed && (!im->msilObject || address) && !retval)
                        gen_code(Instruction::i_ldsflda, dest);
                    else
                        gen_code(Instruction::i_ldsfld, dest);
                    increment_stack();
                }
            }
            break;
    }
}
void gen_store(Optimizer::IMODE* im, Operand* dest)
{
    if (im->mode == Optimizer::i_ind)
    {
        if (im->fieldname)
        {
            Optimizer::SimpleExpression* offset = (Optimizer::SimpleExpression*)im->vararg;
            if (qualifiedStruct(offset->sp->parentClass))
            {
                if (offset->sp->isproperty)
                {
                    void* result;
                    peLib->Find(offset->sp->msil, &result);
                    Property* p = static_cast<Property*>(result);
                    p->CallSet(*peLib, currentMethod);
                    decrement_stack();
                    decrement_stack();
                }
                else
                {
                    Value* field = GetStructField(offset->sp);
                    Operand* operand = peLib->AllocateOperand(field);
                    gen_code(Instruction::i_stfld, operand);
                    decrement_stack();
                    decrement_stack();
                }
            }
            else
            {
                store_ind(im);
            }
        }
        else
        {
            store_ind(im);
        }
        return;
    }
    if (!dest)
        return;
    switch (dest->OperandType())
    {
        case Operand::t_value:
            if (typeid(*dest->GetValue()) == typeid(Local))
            {
                gen_code(Instruction::i_stloc, dest);
                decrement_stack();
            }
            else if (typeid(*dest->GetValue()) == typeid(Param))
            {
                gen_code(Instruction::i_starg, dest);
                decrement_stack();
            }
            else  // fieldname
            {
                if (dest->Property())
                {
                    Property* p = reinterpret_cast<Property*>(static_cast<FieldName*>(dest->GetValue())->GetField());
                    p->CallSet(*peLib, currentMethod);
                    decrement_stack();
                }
                else
                {
                    gen_code(Instruction::i_stsfld, dest);
                    decrement_stack();
                }
            }
            break;
    }
}
void gen_convert(Operand* dest, Optimizer::IMODE* im, int sz)
{
    Instruction::iop op;
    switch (sz)
    {
        case ISZ_UNATIVE:
            op = Instruction::i_conv_u;
            break;
        case -ISZ_UNATIVE:
            op = Instruction::i_conv_i;
            break;
        case ISZ_BOOLEAN:
        case ISZ_UCHAR:
            op = Instruction::i_conv_u1;
            break;
        case -ISZ_UCHAR:
            op = Instruction::i_conv_i1;
            break;
        case ISZ_USHORT:
        case ISZ_WCHAR:
        case ISZ_U16:
            op = Instruction::i_conv_u2;
            break;
        case -ISZ_USHORT:
            op = Instruction::i_conv_i2;
            break;
        case ISZ_UINT:
        case ISZ_ULONG:
        case ISZ_U32:
            op = Instruction::i_conv_u4;
            break;
        case -ISZ_UINT:
        case -ISZ_ULONG:
            op = Instruction::i_conv_i4;
            break;
        case ISZ_ULONGLONG:
            op = Instruction::i_conv_u8;
            break;
        case -ISZ_ULONGLONG:
            op = Instruction::i_conv_i8;
            break;
        case ISZ_ADDR:
            op = Instruction::i_conv_i;
            break;
        /* */
        case ISZ_FLOAT:
            op = Instruction::i_conv_r4;
            break;
        case ISZ_DOUBLE:
        case ISZ_LDOUBLE:
            op = Instruction::i_conv_r8;
            break;

        case ISZ_IFLOAT:
            op = Instruction::i_conv_r4;
            break;
        case ISZ_IDOUBLE:
        case ISZ_ILDOUBLE:
            op = Instruction::i_conv_r8;
            break;

        case ISZ_CFLOAT:
        case ISZ_CDOUBLE:
        case ISZ_CLDOUBLE:
            break;
        case ISZ_OBJECT:
            box(im);
            return;
        case ISZ_STRING: {
            box(im);
            MethodSignature* sig = toStr;
            Operand* ap = peLib->AllocateOperand(peLib->AllocateMethodName(sig));
            gen_code(Instruction::i_call, ap);
            return;
        }
        case ISZ_TOINT: {
            MethodSignature* sig = toInt;
            Operand* ap = peLib->AllocateOperand(peLib->AllocateMethodName(sig));
            gen_code(Instruction::i_call, ap);
            return;
        }
        case ISZ_TOVOIDSTAR: {
            MethodSignature* sig = toVoidStar;
            Operand* ap = peLib->AllocateOperand(peLib->AllocateMethodName(sig));
            gen_code(Instruction::i_call, ap);
            return;
        }
    }
    gen_code(op, NULL);
}
void gen_branch(Instruction::iop op, int label, bool decrement)
{
    char lbl[256];
    sprintf(lbl, "L_%d_%x", label, uniqueId);
    Operand* operand = peLib->AllocateOperand(lbl);
    gen_code(op, operand);
    if (decrement)
    {
        switch (op)
        {
            case Instruction::i_br:
            case Instruction::i_br_s:
                break;
            case Instruction::i_brtrue:
            case Instruction::i_brtrue_s:
            case Instruction::i_brfalse:
            case Instruction::i_brfalse_s:
                decrement_stack();
                break;
            default:
                decrement_stack();
                decrement_stack();
                break;
        }
    }
}

void asm_expressiontag(Optimizer::QUAD* q)
{
    if (!q->dc.v.label)
    {
        // expression tags can be nested...
        int n = 1;
        q = q->back;
        while (n && (q->dc.opcode == Optimizer::i_line || q->dc.opcode == Optimizer::i_expressiontag))
        {
            if (q->dc.opcode == Optimizer::i_expressiontag)
            {
                if (q->dc.v.label)
                    n--;
                else
                    n++;
            }
            q = q->back;
        }
        if (n)
        {

            gen_code(Instruction::i_pop, NULL);
            decrement_stack();
        }
    }
}
void asm_tag(Optimizer::QUAD* q) {}
void asm_line(Optimizer::QUAD* q) /* line number information and text */
{
    char buf[10000];
    Optimizer::LINEDATA* ld = (Optimizer::LINEDATA*)q->dc.left;
    sprintf(buf, "Line %d: %s", ld->lineno, ld->line);
    Instruction* i = peLib->AllocateInstruction(Instruction::i_comment, buf);
    currentMethod->AddInstruction(i);
}
void asm_blockstart(Optimizer::QUAD* q) /* line number information and text */ {}
void asm_blockend(Optimizer::QUAD* q) /* line number information and text */ {}
void asm_varstart(Optimizer::QUAD* q) /* line number information and text */ {}
void asm_func(Optimizer::QUAD* q) /* line number information and text */ {}
void asm_passthrough(Optimizer::QUAD* q) /* reserved */ {}
void asm_datapassthrough(Optimizer::QUAD* q) /* reserved */ {}
void asm_label(Optimizer::QUAD* q) /* put a label in the code stream */ { msil_oa_gen_label(q->dc.v.label); }
void asm_goto(Optimizer::QUAD* q) /* unconditional branch */
{
    if (q->dc.opcode == Optimizer::i_goto)
        gen_branch(Instruction::i_br, q->dc.v.label, false);
    else
    {
        // i don't know if this is kosher in the middle of a function...
        gen_code(Instruction::i_tail_, 0);
        gen_code(Instruction::i_calli, 0);
    }
}
static Type::BasicType names[] = {
    Type::u32, Type::u32, Type::u8,  Type::u8,  Type::u16, Type::u16, Type::u16, Type::u32, Type::unative, Type::u32, Type::u32,
    Type::u64, Type::u32, Type::u32, Type::u16, Type::u16, Type::r32, Type::r64, Type::r64, Type::r32,     Type::r64, Type::r64,
};
static Type::BasicType mnames[] = {
    Type::i32, Type::i32, Type::i8,  Type::i8,  Type::i16, Type::i16, Type::i16, Type::i32, Type::inative, Type::i32, Type::i32,
    Type::i64, Type::i32, Type::i32, Type::i16, Type::i16, Type::r32, Type::r64, Type::r64, Type::r32,     Type::r64, Type::r64,
};
BoxedType* boxedType(int isz)
{
    Type::BasicType n;
    if (isz == ISZ_OBJECT)
        n = Type::object;  // to support newarr object[]
    else if (isz == ISZ_STRING)
        n = Type::string;
    else if (isz == ISZ_TOINT)
        n = Type::inative;
    else if (isz == ISZ_TOVOIDSTAR)
        n = Type::inative;
    else
        n = isz < 0 ? mnames[-isz] : names[isz];
    return peLib->AllocateBoxedType(n);
}
void box(Optimizer::IMODE* im)
{
    BoxedType* type = boxedType(im->size);
    if (type)
    {
        Operand* operand = peLib->AllocateOperand(peLib->AllocateValue("", type));
        gen_code(Instruction::i_box, operand);
    }
}
void unbox(int isz)
{
    Type::BasicType n = isz < 0 ? mnames[-isz] : names[isz];
    Operand* op1 = peLib->AllocateOperand(peLib->AllocateValue("", peLib->AllocateType(n, 0)));
    if (op1)
        gen_code(Instruction::i_unbox, op1);
}
void set_xxx(Instruction::iop ins)
{
    int lab1 = beGetLabel;
    int lab2 = beGetLabel;
    gen_branch(ins, lab1, true);
    gen_code(Instruction::i_ldc_i4_0, nullptr);
    gen_branch(Instruction::i_br, lab2, true);
    msil_oa_gen_label(lab1);
    gen_code(Instruction::i_ldc_i4_1, nullptr);
    msil_oa_gen_label(lab2);
    increment_stack();
}
// this implementation won't handle varag functions nested in other varargs...
void asm_parm(Optimizer::QUAD* q) /* push a parameter*/
{
    if (q->vararg)
    {
        if (q->dc.left->size == ISZ_ADDR)
        {
            Operand* operand = peLib->AllocateOperand(peLib->AllocateMethodName(ptrBox));
            gen_code(Instruction::i_call, operand);
        }
        else if (q->dc.left->size != ISZ_OBJECT)
        {
            box(q->dc.left);
        }
        gen_code(Instruction::i_stelem_ref, NULL);
        decrement_stack();
        decrement_stack();
    }
    else if (q->valist)
    {
        Optimizer::QUAD* find = q;
        while (find && find->dc.opcode != Optimizer::i_gosub)
            find = find->fwd;
        if (find)
        {
            if (!msil_managed(find->altsp))
            {
                Operand* operand = peLib->AllocateOperand(peLib->AllocateMethodName(argsUnmanaged));
                gen_code(Instruction::i_callvirt, operand);
            }
        }
    }
}
void asm_parmblock(Optimizer::QUAD* q) /* push a block of memory */
{
    if (q->vararg)
    {
        gen_code(Instruction::i_stelem_ref, NULL);
        decrement_stack();
        decrement_stack();
    }
    else
    {
        // have to see if it was already loaded...
        Instruction* i = currentMethod->LastInstruction();
        if (i->OpCode() == Instruction::i_ldloc || i->OpCode() == Instruction::i_ldarg || i->OpCode() == Instruction::i_ldsfld)
        {
            return;
        }
        if (i->OpCode() == Instruction::i_call || i->OpCode() == Instruction::i_calli || i->OpCode() == Instruction::i_callvirt)
        {
            return;
        }
        // no it is a member of a structure, we have to load it
        Type* tp = GetType(q->alttp, true);
        gen_code(Instruction::i_ldobj, peLib->AllocateOperand(peLib->AllocateValue("", tp)));
    }
}
void asm_parmadj(Optimizer::QUAD* q) /* adjust stack after function call */
{
    int i;
    int n = beGetIcon(q->dc.left) - beGetIcon(q->dc.right);
    if (n > 0)
        for (i = 0; i < n; i++)
            decrement_stack();
    else if (n < 0)
        increment_stack();
}
static bool bltin_gosub(Optimizer::QUAD* q)
{
    Optimizer::SimpleExpression* en = GetSymRef(q->dc.left->offset);

    Operand* operand;
    if (q->dc.left->mode == Optimizer::i_immed)
    {
        Optimizer::SimpleSymbol* sp = en->sp;
        if (!strcmp(sp->name, "__va_start__"))
        {
            Optimizer::SimpleExpression* en = GetSymRef(q->dc.left->offset);
            Operand* op1 = peLib->AllocateOperand(GetParamData("__va_list__"));
            gen_code(Instruction::i_ldarg, op1);
            op1 = peLib->AllocateOperand(peLib->AllocateMethodName(argsCtor));
            gen_code(Instruction::i_newobj, op1);
            return true;
        }
        else if (!strcmp(sp->name, "__va_arg__"))
        {
            Optimizer::SimpleExpression* en = GetSymRef(q->dc.left->offset);
            Optimizer::ArgList* args = q->altargs;
            Optimizer::SimpleType* tp = en->sp->tp;
            if (args->next)
                tp = args->next->tp;
            Operand* operand = peLib->AllocateOperand(peLib->AllocateMethodName(argsNextArg));
            // the function pushes both an arglist val and a type to cast to on the stack
            // remove the type to cast to.
            currentMethod->RemoveLastInstruction();
            gen_code(Instruction::i_callvirt, operand);
            if (tp->type == Optimizer::st_pointer)
            {
                Operand* operand = peLib->AllocateOperand(peLib->AllocateMethodName(ptrUnbox));
                gen_code(Instruction::i_call, operand);
            }
            else if (tp->type != Optimizer::st_struct && tp->type != Optimizer::st_union && !tp->isarray)
            {
                Optimizer::SimpleExpression* exp = args->next->exp;
                unbox(exp->sp->tp->sizeFromType);
            }
            return true;
        }
    }
    return false;
}
void asm_gosub(Optimizer::QUAD* q) /* normal gosub to an immediate label or through a var */
{
    if (q->dc.left->mode == Optimizer::i_immed)
    {
        if (!bltin_gosub(q))
        {
            bool virt = false;
            Operand* ap = getCallOperand(q, virt);
            if (!strcmp(q->dc.left->offset->sp->name, ".ctor"))
            {
                gen_code(Instruction::i_newobj, ap);
                increment_stack();
            }
            else if (virt)
            {
                gen_code(Instruction::i_callvirt, ap);
            }
            else
            {
                gen_code(Instruction::i_call, ap);
            }
        }
    }
    else
    {
        if (Optimizer::delegateforfuncptr)
        {
            gen_code(Instruction::i_call, GetDelegateInvoker(q->altsp));
            if (q->altsp->tp->btp->btp->type == Optimizer::st_void)
                gen_code(Instruction::i_pop, nullptr);
            else if (q->altsp->tp->btp->btp->type != Optimizer::st_pointer)
                gen_convert(nullptr, q->dc.left, q->altsp->tp->btp->btp->sizeFromType);
        }
        else
        {
            bool virt = false;
            Operand* ap = getCallOperand(q, virt);
            gen_code(Instruction::i_calli, ap);
            decrement_stack();
        }
    }
    if (q->novalue == -3)
    {
        returnCount++;
    }
    else if (q->novalue && q->novalue != -1)
    {
        gen_code(Instruction::i_pop, NULL);
        decrement_stack();
    }
}
void asm_fargosub(Optimizer::QUAD* q) /* far version of gosub */ {}
void asm_trap(Optimizer::QUAD* q) /* 'trap' instruction - the arg will be an immediate # */ {}
void asm_int(Optimizer::QUAD* q) /* 'int' instruction(Optimizer::QUAD *q) calls a labeled function which is an interrupt */ {}
/* left will be a constant holding the number of bytes to pop
 * e.g. the parameters will be popped in stdcall or pascal type functions
 */
void asm_ret(Optimizer::QUAD* q) /* return from subroutine */ { gen_code(Instruction::i_ret, NULL); }
/* left will be a constant holding the number of bytes to pop
 * e.g. the parameters will be popped in stdcall or pascal type functions
 */
void asm_fret(Optimizer::QUAD* q) /* far return from subroutine */ {}
/*
 * this can be either a fault or iret return
 * for processors that char, the 'left' member will have an integer
 * value that is true for an iret or false or a fault ret
 */
void asm_rett(Optimizer::QUAD* q) /* return from trap or int */ {}
void asm_add(Optimizer::QUAD* q) /* evaluate an addition */
{
    if (q->dc.right->offset && q->dc.right->offset->type == Optimizer::se_structelem)
    {
        // the 'add' for a structure offset had to remain until now because it
        // held the left and right side both in the expression trees...
        // it is ignored for 'direct' access by used as a field address for
        // immed access
        if (q->dc.right->mode == Optimizer::i_immed)
        {
            Operand* ap = getOperand(q->dc.right);
            gen_load(q->dc.right, ap, false);
        }
    }
    // only time we generate add for non arithmetic types is for strings
    else if (q->dc.left->size == ISZ_STRING || q->dc.right->size == ISZ_STRING ||
             (q->dc.left->size == ISZ_OBJECT && q->dc.right->size == ISZ_OBJECT))
    {
        MethodSignature* sig;
        if (q->dc.left->size == q->dc.right->size && q->dc.left->size == ISZ_STRING)
        {
            sig = concatStr;
        }
        else
        {
            sig = concatObj;
        }
        Operand* ap = peLib->AllocateOperand(peLib->AllocateMethodName(sig));
        gen_code(Instruction::i_call, ap);
        decrement_stack();
    }
    else
    {
        decrement_stack();
        gen_code(Instruction::i_add, NULL);
    }
}
void asm_sub(Optimizer::QUAD* q) /* evaluate a subtraction */
{
    decrement_stack();
    gen_code(Instruction::i_sub, NULL);
}
void asm_udiv(Optimizer::QUAD* q) /* unsigned division */
{
    decrement_stack();
    gen_code(Instruction::i_div_un, NULL);
}
void asm_umod(Optimizer::QUAD* q) /* unsigned modulous */
{
    decrement_stack();
    gen_code(Instruction::i_rem_un, NULL);
}
void asm_sdiv(Optimizer::QUAD* q) /* signed division */
{
    decrement_stack();
    gen_code(Instruction::i_div, NULL);
}
void asm_smod(Optimizer::QUAD* q) /* signed modulous */
{
    decrement_stack();
    gen_code(Instruction::i_rem, NULL);
}
void asm_muluh(Optimizer::QUAD* q)
{
    Optimizer::SimpleExpression* en = Optimizer::simpleIntNode(Optimizer::se_i, 32);
    Operand* ap = make_constant(ISZ_UINT, en);
    gen_code(Instruction::i_mul, NULL);
    gen_code(Instruction::i_ldc_i4, ap);
    gen_code(Instruction::i_shr_un, NULL);
    decrement_stack();
}
void asm_mulsh(Optimizer::QUAD* q)
{
    Optimizer::SimpleExpression* en = Optimizer::simpleIntNode(Optimizer::se_i, 32);
    Operand* ap = make_constant(ISZ_UINT, en);
    gen_code(Instruction::i_mul, NULL);
    gen_code(Instruction::i_ldc_i4, ap);
    gen_code(Instruction::i_shr, NULL);
    decrement_stack();
}
void asm_mul(Optimizer::QUAD* q) /* signed multiply */
{
    decrement_stack();
    gen_code(Instruction::i_mul, NULL);
}
void asm_lsr(Optimizer::QUAD* q) /* unsigned shift right */
{
    decrement_stack();
    gen_code(Instruction::i_shr_un, NULL);
}
void asm_lsl(Optimizer::QUAD* q) /* signed shift left */
{
    decrement_stack();
    gen_code(Instruction::i_shl, NULL);
}
void asm_asr(Optimizer::QUAD* q) /* signed shift right */
{
    decrement_stack();
    gen_code(Instruction::i_shr, NULL);
}
void asm_neg(Optimizer::QUAD* q) /* negation */ { gen_code(Instruction::i_neg, NULL); }
void asm_not(Optimizer::QUAD* q) /* complement */ { gen_code(Instruction::i_not, NULL); }
void asm_and(Optimizer::QUAD* q) /* binary and */
{
    decrement_stack();
    gen_code(Instruction::i_and, NULL);
}
void asm_or(Optimizer::QUAD* q) /* binary or */
{
    decrement_stack();
    gen_code(Instruction::i_or, NULL);
}
void asm_eor(Optimizer::QUAD* q) /* binary exclusive or */
{
    decrement_stack();
    gen_code(Instruction::i_xor, NULL);
}
void asm_setne(Optimizer::QUAD* q) /* evaluate a = b != c */ { set_xxx(Instruction::i_bne_un); }
void asm_sete(Optimizer::QUAD* q) /* evaluate a = b == c */ { set_xxx(Instruction::i_beq); }
void asm_setc(Optimizer::QUAD* q) /* evaluate a = b U< c */ { set_xxx(Instruction::i_blt); }
void asm_seta(Optimizer::QUAD* q) /* evaluate a = b U> c */ { set_xxx(Instruction::i_bgt); }
void asm_setnc(Optimizer::QUAD* q) /* evaluate a = b U>= c */ { set_xxx(Instruction::i_bge); }
void asm_setbe(Optimizer::QUAD* q) /* evaluate a = b U<= c */ { set_xxx(Instruction::i_ble); }
void asm_setl(Optimizer::QUAD* q) /* evaluate a = b S< c */ { set_xxx(Instruction::i_blt); }
void asm_setg(Optimizer::QUAD* q) /* evaluate a = b s> c */ { set_xxx(Instruction::i_bgt); }
void asm_setle(Optimizer::QUAD* q) /* evaluate a = b S<= c */ { set_xxx(Instruction::i_ble); }
void asm_setge(Optimizer::QUAD* q) /* evaluate a = b S>= c */ { set_xxx(Instruction::i_bge); }
void asm_assn(Optimizer::QUAD* q) /* assignment */
{
    Operand* ap;

    if (q->ans && q->ans->mode == Optimizer::i_immed && q->ans->offset->type == Optimizer::se_msil_array_access)
    {
        Type* tp = GetType(q->ans->offset->msilArrayTP, true);
        if (tp->ArrayLevel() == 1)
        {
            Operand* operand = NULL;
            Instruction::iop instructions[] = {
                Instruction::i_stelem,    Instruction::i_stelem,    Instruction::i_stelem,    Instruction::i_stelem_i1,
                Instruction::i_stelem_i2, Instruction::i_stelem_i1, Instruction::i_stelem_i1, Instruction::i_stelem_i2,
                Instruction::i_stelem_i2, Instruction::i_stelem_i4, Instruction::i_stelem_i4, Instruction::i_stelem_i8,
                Instruction::i_stelem_i8, Instruction::i_stelem_i,  Instruction::i_stelem_i,  Instruction::i_stelem_r4,
                Instruction::i_stelem_r8, Instruction::i_stelem,    Instruction::i_stelem};
            if (instructions[tp->GetBasicType()] == Instruction::i_stelem)
            {
                operand = peLib->AllocateOperand(peLib->AllocateValue("", GetType(q->ans->offset->msilArrayTP->btp, true)));
            }
            gen_code(instructions[tp->GetBasicType()], operand);
            decrement_stack();
            decrement_stack();
            decrement_stack();
        }
        else
        {
            MethodSignature* sig = LookupArrayMethod(tp, "Set");
            Operand* operand = peLib->AllocateOperand(peLib->AllocateMethodName(sig));
            gen_code(Instruction::i_call, operand);
            int n = tp->ArrayLevel();
            for (int i = 0; i < n + 2; i++)
                decrement_stack();
        }
        return;
    }
    else if (q->dc.left && q->dc.left->mode == Optimizer::i_immed && q->dc.left->offset->type == Optimizer::se_msil_array_init)
    {
        if (q->dc.left->offset->tp->btp->isarray)
        {
            Type* tp = boxedType(q->ans->size);
            Operand* operand = peLib->AllocateOperand(peLib->AllocateValue("", tp));
            gen_code(Instruction::i_newarr, operand);
        }
        else
        {
            Type* tp = GetType(q->dc.left->offset->tp, true);
            MethodSignature* sig = LookupArrayMethod(tp, ".ctor");
            Operand* operand = peLib->AllocateOperand(peLib->AllocateMethodName(sig));
            gen_code(Instruction::i_newobj, operand);
            int n = tp->ArrayLevel();
            for (int i = 0; i < n - 1; i++)
                decrement_stack();
        }
    }
    else if (q->dc.left && q->dc.left->mode == Optimizer::i_immed &&
             (q->dc.left->size == ISZ_OBJECT || q->dc.left->size == ISZ_STRING) &&
             ((q->dc.left->offset->type == Optimizer::se_i || q->dc.left->offset->type == Optimizer::se_ui) &&
              q->dc.left->offset->i == 0))
    {
        gen_code(Instruction::i_ldnull, NULL);
        increment_stack();
    }
    else
    {
        Optimizer::SimpleType* tp;
        // don't generate if it is a placeholder ind...
        if (q->ans->mode == Optimizer::i_direct && !(q->temps & TEMP_ANS) && q->ans->offset->type == Optimizer::se_auto)
        {
            if (q->ans->offset->sp->msilObjectArray)
            {
                // assign to object array, call the ctor here
                // count is already on the stack
                Operand* ap = peLib->AllocateOperand(peLib->AllocateValue("", systemObject));
                gen_code(Instruction::i_newarr, ap);
                ap = getOperand(q->ans);
                gen_store(q->ans, ap);
                return;
            }
        }
        ap = getOperand(q->dc.left);
        if (q->blockassign)
        {
            tp = q->alttp;
            GetType(tp, true, false, false);
            if (tp->sp->msil)
            {
                void* result;
                peLib->Find(tp->sp->msil, &result);
                Class* c = static_cast<Class*>(result);
                if (c->Flags().Flags() & Qualifiers::Value)
                    if (!currentMethod->LastInstruction()->IsCall() ||
                        static_cast<MethodName*>(currentMethod->LastInstruction()->GetOperand()->GetValue())
                            ->Signature()
                            ->ReturnType()
                            ->PointerLevel())
                        gen_code(Instruction::i_ldobj,
                                 peLib->AllocateOperand(peLib->AllocateValue("", GetType(tp, true, false, false))));
                switch (q->ans->offset->type)
                {
                    case Optimizer::se_global:
                    case Optimizer::se_auto:
                    case Optimizer::se_pc:
                    case Optimizer::se_labcon:  // DAL fix
                        q->ans->mode = (enum Optimizer::i_adr)q->oldmode;
                        break;
                    default:
                        gen_code(Instruction::i_stobj,
                                 peLib->AllocateOperand(peLib->AllocateValue("", GetType(tp, true, false, false))));
                        decrement_stack();
                        decrement_stack();
                        if (q->hook)
                            hookCount++;
                        return;
                }
            }
        }
        else
        {
            gen_load(q->dc.left, ap, q->ans->retval);
            if (q->dc.left->size != 0 && q->dc.left->size != q->ans->size)
            {
                gen_convert(ap, q->dc.left, q->ans->size);
            }
        }
    }
    ap = getOperand(q->ans);
    if (Optimizer::delegateforfuncptr)
    {
        Optimizer::SimpleExpression* en = GetSymRef(q->ans->offset);
        Optimizer::SimpleSymbol* sp = NULL;
        if (en && en->type != Optimizer::se_labcon)
        {
            sp = en->sp;
        }
        else if (q->ans->offset->type == Optimizer::se_tempref)
        {
            sp = (Optimizer::SimpleSymbol*)q->ans->offset->right;
        }
        if (sp)
        {
            if (sp->tp->type == Optimizer::st_pointer && sp->tp->btp->type == Optimizer::st_func)
            {
                gen_load(q->ans, ap, q->ans->retval);
                gen_code(Instruction::i_call, peLib->AllocateOperand(peLib->AllocateMethodName(delegateFreer)));
            }
        }
    }
    gen_store(q->ans, ap);
    if (q->ans->retval)
        returnCount++;
    if (q->hook)
        hookCount++;
}
void asm_genword(Optimizer::QUAD* q) /* put a byte or word into the code stream */ {}
void compactgen(Instruction* i, int lab)
{
    char buf[256];
    sprintf(buf, "L_%d_%x", lab, uniqueId);
    i->AddCaseLabel(buf);
}
void bingen(int lower, int avg, int higher)
{
    int nelab = beGetLabel;
    if (switchTreeBranchLabels[avg] != 0)
        msil_oa_gen_label(switchTreeBranchLabels[avg]);
    gen_load(switch_ip, switch_ip_a, false);
    load_constant(switch_ip->size, Optimizer::simpleIntNode(Optimizer::se_i, switchTreeCases[avg]));
    gen_branch(Instruction::i_beq, switchTreeLabels[avg], true);
    if (avg == lower)
    {
        gen_branch(Instruction::i_br, switch_deflab, false);
    }
    else
    {
        int avg1 = (lower + avg) / 2;
        int avg2 = (higher + avg + 1) / 2;
        int lab;
        if (avg + 1 < higher)
            lab = switchTreeBranchLabels[avg2] = beGetLabel;
        else
            lab = switch_deflab;
        gen_load(switch_ip, switch_ip_a, false);
        load_constant(switch_ip->size, Optimizer::simpleIntNode(Optimizer::se_i, switchTreeCases[avg]));
        if (switch_ip->size < 0)
            gen_branch(Instruction::i_bgt, lab, true);
        else
            gen_branch(Instruction::i_bgt_un, lab, true);
        bingen(lower, avg1, avg);
        if (avg + 1 < higher)
            bingen(avg + 1, avg2, higher);
    }
}

void asm_coswitch(Optimizer::QUAD* q) /* switch characteristics */
{
    Instruction::iop op;
    switch_deflab = q->dc.v.label;
    switch_range = q->dc.right->offset->i;
    switch_case_max = switch_case_count = q->ans->offset->i;
    switch_ip = q->dc.left;
    switch_ip_a = getOperand(switch_ip);
    if (switch_ip->size == ISZ_ULONGLONG || switch_ip->size == -ISZ_ULONGLONG || switch_case_max <= 5)
    {
        switch_mode = swm_enumerate;
    }
    else if (switch_case_max * 10 / switch_range > 8)
    {
        switch_mode = swm_compactstart;
    }
    else
    {
        switch_mode = swm_tree;
        if (!switchTreeLabelCount || switchTreeLabelCount < switch_case_max)
        {
            free(switchTreeCases);
            free(switchTreeLabels);
            free(switchTreeBranchLabels);
            switchTreeLabelCount = (switch_case_max + 1024) & ~1023;
            switchTreeCases = (long long*)calloc(switchTreeLabelCount, sizeof(long long));
            switchTreeLabels = (int*)calloc(switchTreeLabelCount, sizeof(int));
            switchTreeBranchLabels = (int*)calloc(switchTreeLabelCount, sizeof(int));
        }
        switchTreePos = 0;
        memset(switchTreeBranchLabels, 0, sizeof(int) * switch_case_max);
    }
}
void asm_swbranch(Optimizer::QUAD* q) /* case characteristics */
{
    static Instruction* swins;
    unsigned long long swcase = q->dc.left->offset->i;
    int labin = q->dc.v.label, lab;
    if (switch_case_count == 0)
    {
        /*		diag("asm_swbranch, count mismatch"); in case only a default */
        return;
    }

    if (switch_mode == swm_compactstart)
    {
        gen_load(switch_ip, switch_ip_a, false);
        if (swcase != 0)
        {
            load_constant(switch_ip->size, Optimizer::simpleIntNode(Optimizer::se_i, swcase));
            gen_code(Instruction::i_sub, NULL);
            decrement_stack();
        }
        swins = gen_code(Instruction::i_switch, NULL);
        gen_branch(Instruction::i_br, switch_deflab, false);
    }
    switch (switch_mode)
    {
        int lab;
        case swm_enumerate:
        default:

            gen_load(switch_ip, switch_ip_a, false);
            load_constant(switch_ip->size, Optimizer::simpleIntNode(Optimizer::se_i, swcase));
            gen_branch(Instruction::i_beq, labin, true);
            if (--switch_case_count == 0)
            {
                gen_branch(Instruction::i_br, switch_deflab, false);
            }
            break;
        case swm_compact:
            while (switch_lastcase < swcase)
            {
                compactgen(swins, switch_deflab);
                switch_lastcase++;
            }
            // fall through
        case swm_compactstart:
            compactgen(swins, labin);
            switch_lastcase = swcase + 1;
            switch_mode = swm_compact;
            --switch_case_count;
            if (!switch_case_count)
                decrement_stack();
            break;
        case swm_tree:
            switchTreeCases[switchTreePos] = swcase;
            switchTreeLabels[switchTreePos++] = labin;
            if (--switch_case_count == 0)
            {
                bingen(0, switch_case_max / 2, switch_case_max);
                increment_stack();
                increment_stack();
                decrement_stack();
                decrement_stack();
            }
            break;
    }
}
void asm_dc(Optimizer::QUAD* q) /* unused */ {}
void asm_assnblock(Optimizer::QUAD* q) /* copy block of memory*/
{
    gen_code(Instruction::i_cpblk, 0);
    decrement_stack();
    decrement_stack();
    decrement_stack();
}
void asm_clrblock(Optimizer::QUAD* q) /* clear block of memory */ { asm_initobj(q); }
void asm_initblock(Optimizer::QUAD* q) /* clear block of memory */
{
    // the 'value' field is loaded by examine_icode...
    gen_code(Instruction::i_initblk, 0);
    decrement_stack();
    decrement_stack();
    decrement_stack();
}
void asm_initobj(Optimizer::QUAD* q)
{
    auto operand = peLib->AllocateOperand(peLib->AllocateValue("", GetType(q->dc.left->offset->sp->tp, true)));
    gen_code(Instruction::i_initobj, operand);
    decrement_stack();
}
void asm_sizeof(Optimizer::QUAD* q)
{
    auto type = GetType(q->dc.left->offset->tp, true);
    type->ShowType();
    auto operand = peLib->AllocateOperand(peLib->AllocateValue("", type));
    gen_code(Instruction::i_sizeof, operand);
    increment_stack();
}
void asm_cmpblock(Optimizer::QUAD* q)
{
    assert(0);  // atomic support not implemented
}
void asm_jc(Optimizer::QUAD* q) /* branch if a U< b */ { gen_branch(Instruction::i_blt_un, q->dc.v.label, true); }
void asm_ja(Optimizer::QUAD* q) /* branch if a U> b */ { gen_branch(Instruction::i_bgt_un, q->dc.v.label, true); }
void asm_je(Optimizer::QUAD* q) /* branch if a == b */
{
    if (q->dc.right->mode == Optimizer::i_immed &&
        ((q->dc.right->offset->type == Optimizer::se_i || q->dc.right->offset->type == Optimizer::se_ui) &&
         q->dc.right->offset->i == 0))
    {
        gen_code(Instruction::i_conv_i4, nullptr);
        gen_branch(Instruction::i_brfalse, q->dc.v.label, true);
    }
    else
    {
        gen_branch(Instruction::i_beq, q->dc.v.label, true);
    }
}
void asm_jnc(Optimizer::QUAD* q) /* branch if a U>= b */ { gen_branch(Instruction::i_bge_un, q->dc.v.label, true); }
void asm_jbe(Optimizer::QUAD* q) /* branch if a U<= b */ { gen_branch(Instruction::i_ble_un, q->dc.v.label, true); }
void asm_jne(Optimizer::QUAD* q) /* branch if a != b */
{
    if (q->dc.right->mode == Optimizer::i_immed &&
        ((q->dc.right->offset->type == Optimizer::se_i || q->dc.right->offset->type == Optimizer::se_ui) &&
         q->dc.right->offset->i == 0))
    {
        gen_code(Instruction::i_conv_i4, nullptr);
        gen_branch(Instruction::i_brtrue, q->dc.v.label, true);
    }
    else
    {
        gen_branch(Instruction::i_bne_un, q->dc.v.label, true);
    }
}
void asm_jl(Optimizer::QUAD* q) /* branch if a S< b */ { gen_branch(Instruction::i_blt, q->dc.v.label, true); }
void asm_jg(Optimizer::QUAD* q) /* branch if a S> b */ { gen_branch(Instruction::i_bgt, q->dc.v.label, true); }
void asm_jle(Optimizer::QUAD* q) /* branch if a S<= b */ { gen_branch(Instruction::i_ble, q->dc.v.label, true); }
void asm_jge(Optimizer::QUAD* q) /* branch if a S>= b */ { gen_branch(Instruction::i_bge, q->dc.v.label, true); }
void asm_cppini(Optimizer::QUAD* q) /* cplusplus initialization (historic)*/ { (void)q; }
/*
 * function prologue.  left has a constant which is a bit mask
 * of registers to push.  It also has a flag indicating whether frames
 * are absolutely necessary
 *
 * right has the number of bytes to allocate on the stack
 */
void asm_prologue(Optimizer::QUAD* q) /* function prologue */
{
    stackpos = 0;
    returnCount = 0;
    hookCount = 0;
    LoadLocals(Optimizer::functionVariables);
    LoadParams(currentFunction, Optimizer::functionVariables, paramList);
    for (int i = 0; i < localList.size(); i++)
        currentMethod->AddLocal(localList[i]);
}
/*
 * function epilogue, left holds the mask of which registers were pushed
 */
void asm_epilogue(Optimizer::QUAD* q) /* function epilogue */
{
    if (currentFunction->tp->btp->type != Optimizer::st_void)
        stackpos--;
    if (returnCount)
        stackpos -= returnCount - 1;
    stackpos -= hookCount / 2;
    // if (stackpos != 0)
    //    diag("asm_epilogue: stack mismatch");
}
/*
 * in an interrupt handler, push the current context
 */
void asm_pushcontext(Optimizer::QUAD* q) /* push register context */ {}
/*
 * in an interrupt handler, pop the current context
 */
void asm_popcontext(Optimizer::QUAD* q) /* pop register context */ {}
/*
 * loads a context, e.g. for the loadds qualifier
 */
void asm_loadcontext(Optimizer::QUAD* q) /* load register context (e.g. at interrupt level ) */ {}
/*
 * unloads a context, e.g. for the loadds qualifier
 */
void asm_unloadcontext(Optimizer::QUAD* q) /* load register context (e.g. at interrupt level ) */ {}
void asm_tryblock(Optimizer::QUAD* q) /* try/catch */ {}
void asm_seh(Optimizer::QUAD* q) /* windows seh */
{
    bool begin = !!(q->sehMode & 0x80);
    Instruction::iseh mode;
    switch (q->sehMode & 15)
    {
        case 1:
            mode = Instruction::seh_try;
            break;
        case 2:
            mode = Instruction::seh_catch;
            break;
        case 3:
            mode = Instruction::seh_fault;
            break;
        case 4:
            mode = Instruction::seh_finally;
            break;
        default:
            return;
    }
    if (begin)
    {
        if (mode == Instruction::seh_catch)
        {
            if (q->dc.left)
            {
                Instruction* i = peLib->AllocateInstruction(mode, true, GetType(q->dc.left->offset->sp->tp, true));
                currentMethod->AddInstruction(i);
            }
            else
            {
                Instruction* i = peLib->AllocateInstruction(mode, true, systemObject);
                currentMethod->AddInstruction(i);
            }
        }
        else
        {
            Instruction* i = peLib->AllocateInstruction(mode, true);
            currentMethod->AddInstruction(i);
        }
    }
    switch (mode)
    {
        case Instruction::seh_try:
            if (!begin)
                gen_branch(Instruction::i_leave, q->dc.v.label, false);
            break;
        case Instruction::seh_catch:
            if (!begin)
            {
                if (q->dc.left)
                {
                    gen_code(Instruction::i_ldnull, NULL);
                    gen_code(Instruction::i_stloc, getOperand(q->dc.left));
                }
                gen_branch(Instruction::i_leave, q->dc.v.label, false);
            }
            else
            {
                if (q->dc.left)
                    gen_code(Instruction::i_stloc, getOperand(q->dc.left));
                else
                    gen_code(Instruction::i_pop, NULL);
            }
            break;
        case Instruction::seh_fault:
            if (!begin)
                gen_code(Instruction::i_endfault, NULL);
            break;
        case Instruction::seh_finally:
            if (!begin)
                gen_code(Instruction::i_endfinally, NULL);
            break;
        default:
            return;
    }
    if (!begin)
    {
        Instruction* i = peLib->AllocateInstruction(mode, false);
        currentMethod->AddInstruction(i);
    }
}
void asm_stackalloc(
    Optimizer::QUAD* q) /* allocate stack space - positive value = allocate(Optimizer::QUAD *q) negative value deallocate */
{
}
void asm_loadstack(Optimizer::QUAD* q) /* load the stack pointer from a var */ {}
void asm_savestack(Optimizer::QUAD* q) /* save the stack pointer to a var */ {}
void asm_functail(Optimizer::QUAD* q, int begin, int size) /* functail start or end */ {}
void asm_atomic(Optimizer::QUAD* q) {}
void asm_nop(Optimizer::QUAD* q) { gen_code(Instruction::i_nop, 0); }
}  // namespace occmsil