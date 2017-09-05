/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2011, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
f    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "be.h"
#include "DotNetPELib.h"
#include <vector>

using namespace DotNetPELib;

extern "C" {

	extern int nextLabel;
	extern SYMBOL *theCurrentFunc;
	extern PELib *peLib;
	extern int startlab, retlab;
	extern int MSILLocalOffset;
	extern TYPE stdint;
	extern EXPRESSION *objectArray_exp;
	extern MethodSignature *argsCtor;
	extern MethodSignature *argsNextArg;
	extern MethodSignature *argsUnmanaged;
	extern MethodSignature *ptrBox;
	extern MethodSignature *ptrUnbox;
    extern MethodSignature *concatStr;
    extern MethodSignature *concatObj;
    extern MethodSignature *toStr;
	extern Type *systemObject;
	extern Method *currentMethod;
	extern std::vector<Local *> localList;
	extern DataContainer *mainContainer;
}
#define MAX_ALIGNS 50
MethodSignature *FindPInvokeWithVarargs(std::string name, std::list<Param *>::iterator begin, std::list<Param *>::iterator end, size_t size);
extern std::multimap<std::string, MethodSignature *> pInvokeReferences;

struct swlist
{
    struct swlist *next;
    int lab;
};
extern int uniqueId;

static int fstackid;
static int inframe;
static int switch_deflab;
static LLONG_TYPE switch_range, switch_case_count, switch_case_max;
static IMODE *switch_ip;
static Operand *switch_ip_a;
static enum {swm_enumerate, swm_compactstart, swm_compact, swm_tree} switch_mode;
static int switch_lastcase;
static int *switchTreeLabels, *switchTreeBranchLabels ;
static LLONG_TYPE *switchTreeCases;
static int switchTreeLabelCount;
static int switchTreePos;
static int returnCount;
static int hookCount;
static int stackpos = 0;

static void box(IMODE *im);

MethodSignature *LookupArrayMethod(Type *cls, std::string name);

Type * GetType(TYPE *tp, BOOLEAN commit, BOOLEAN funcarg = false, BOOLEAN pinvoke = false);
Type * GetStringType(int type);
Value *GetLocalData(SYMBOL *sp);
Value *GetParamData(std::string name);
Value *GetFieldData(SYMBOL *sp);
MethodSignature *GetMethodSignature(SYMBOL *sp);
void LoadLocals(SYMBOL *sp);
void LoadParams(SYMBOL *sp);
BOOLEAN qualifiedStruct(SYMBOL *sp);
Value *GetStringFieldData(int i, int type);
Value *GetStructField(SYMBOL *sp);

void include_start(char *name, int num)
{
}

void increment_stack(void)
{
    ++stackpos;
}
void decrement_stack(void)
{
    --stackpos;
}
Instruction *gen_code(Instruction::iop op, Operand *operand)
{
    Instruction *i = peLib->AllocateInstruction(op, operand);
    currentMethod->AddInstruction(i);
    return i;
}
void oa_gen_label(int labno)
/*
 *      add a compiler generated label to the peep list.
 */
{
    char buf[256];
    sprintf(buf, "L_%d_%x", labno, uniqueId);
    Instruction *i = peLib->AllocateInstruction(Instruction::i_label, peLib->AllocateOperand(buf));
    currentMethod->AddInstruction(i);
}

Operand *make_constant(int sz, EXPRESSION *exp)
{
    Operand *operand = NULL;
    if (isintconst(exp))
    {
        operand = peLib->AllocateOperand((longlong)exp->v.i, Operand::any);
    }
    else if (isfloatconst(exp))
    {
        double a;
        FPFToDouble((unsigned char *)&a, &exp->v.f);
        operand = peLib->AllocateOperand(a, Operand::any);
    }
    else if (exp->type == en_structelem)
    {
        if (exp->v.sp->linkage2 == lk_property)
        {
            operand = peLib->AllocateOperand(GetFieldData(exp->v.sp));
            operand->Property(true);
        }
        else
        {
            Value *field = GetStructField(exp->v.sp);
            operand = peLib->AllocateOperand(field);
        }
    }
    else if (exp->type == en_c_string)
    {
        // dotnetpelib currently doesn't support wide characters
        char value[50000];
        int pos = 0;
        int i;
        exp->string->refCount--;
        for (i =0; i < exp->string->size && pos < 49999; i++)
        {
            SLCHAR *str = exp->string->pointers[i];
            int j;
            for (j=0; j < str->count && pos < 49999; j++, pos++)
                value[pos] = str->str[j];
        }
        value[pos] = 0;
        // operand is a string
        operand = peLib->AllocateOperand(value, true);
    }
    else if (exp->type == en_labcon)
    {
        char lbl[256];
        sprintf(lbl, "L_%d_%x", exp->v.i, uniqueId);
        Value *field = GetStringFieldData(exp->v.i, exp->altdata);
        operand = peLib->AllocateOperand(field);
    }
    else if (exp->type == en_auto)
    {
        operand = peLib->AllocateOperand(GetLocalData(exp->v.sp));
    }
    else if (isfunction(exp->v.sp->tp))
    {
        operand = peLib->AllocateOperand(peLib->AllocateMethodName(GetMethodSignature(exp->v.sp)));
    }
    else
    {
        operand = peLib->AllocateOperand(GetFieldData(exp->v.sp));
        if (exp->v.sp->linkage2 == lk_property)
            operand->Property(true);
    }
    return operand;
} 
/*-------------------------------------------------------------------------*/
BOOLEAN isauto(EXPRESSION *ep)
{
    if (ep->type == en_auto)
        return TRUE;
    if (ep->type == en_add || ep->type == en_structadd)
        return isauto(ep->left) || isauto(ep->right);
    if (ep->type == en_sub)
        return isauto(ep->left);
    return FALSE;
}
void oa_gen_vtt(VTABENTRY *vt, SYMBOL *func)
{
}
void oa_gen_vc1(SYMBOL *func)
{
}
void oa_gen_importThunk(SYMBOL *func)
{
}
Operand *getCallOperand(QUAD *q)
{
    EXPRESSION *en = GetSymRef(q->dc.left->offset);

    Operand *operand;
    MethodSignature *sig;
    if (q->dc.left->mode == i_immed)
    {
        SYMBOL *sp = en->v.sp;
        if (!msil_managed(sp))
        {
            sig = GetMethodSignature(sp);
            INITLIST *valist = ((FUNCTIONCALL *)q->altdata)->arguments;
            int n = sig->ParamCount();
            while (n-- && valist)
                valist = valist->next;
            while (valist)
            {
                sig->AddVarargParam(peLib->AllocateParam("", GetType(valist->tp, TRUE, true, true)));
                valist = valist->next;
            }
            MethodSignature *oldsig = FindPInvokeWithVarargs(sig->Name(), sig->vbegin(), sig->vend(), sig->VarargParamCount());
            if (oldsig)
            {
                sig = oldsig;
            }
            else
            {
                pInvokeReferences.insert(std::pair<std::string, MethodSignature *>(sig->Name(), sig));
            }
        }
        else
        {
            if (sp->msil)
                sig = ((Method *)sp->msil)->Signature();
            else
                sig = GetMethodSignature(sp);
        }
    }
    else
    {
        SYMBOL *sp = ((FUNCTIONCALL *)q->altdata)->sp;
       sig = GetMethodSignature(sp);
        sig->SetName(""); // for calli instruction
    }
    operand = peLib->AllocateOperand(peLib->AllocateMethodName(sig));
    return operand;
}
Operand *getOperand(IMODE *oper)
{
    Operand *rv = NULL;
    switch(oper->mode)
    {
        case i_immed:
            if (oper && oper->mode == i_immed && oper->offset->type == en_msil_array_access)
            {
                Type *tp = GetType(oper->offset->v.msilArray->tp, TRUE);
                if (tp->ArrayLevel() == 1)
                {
                    Instruction::iop instructions[] = {
                            Instruction::i_ldelem, Instruction::i_ldelem, Instruction::i_ldelem, Instruction::i_ldelem_u1, 
                            Instruction::i_ldelem_u2, Instruction::i_ldelem_i1, Instruction::i_ldelem_u1, Instruction::i_ldelem_i2, Instruction::i_ldelem_u2, Instruction::i_ldelem_i4, Instruction::i_ldelem_u4, Instruction::i_ldelem_i8, Instruction::i_ldelem_u8, Instruction::i_ldelem_i, Instruction::i_ldelem_i,
                            Instruction::i_ldelem_r4, Instruction::i_ldelem_r8, Instruction::i_ldelem, Instruction::i_ldelem
                    };
                    gen_code(instructions[tp->GetBasicType()], NULL);
                    decrement_stack();
                }
                else
                {
                    MethodSignature *sig = LookupArrayMethod(tp, "Get");
                    Operand *operand = peLib->AllocateOperand(peLib->AllocateMethodName(sig));
                    gen_code(Instruction::i_call, operand);
                    int n = tp->ArrayLevel();
                    for (int i=0; i < n; i++)
                        decrement_stack();
                }
            }
            else
            {
                rv = make_constant(oper->size, oper->offset);
            }
            break;
        case i_direct:
        {
            if (oper->offset->type == en_structelem)
            {
                if (oper->offset->v.sp->linkage2 == lk_property)
                {
                    rv = peLib->AllocateOperand(GetFieldData(oper->offset->v.sp));
                    rv->Property(true);
                }
                else
                {
                    Value *field = GetStructField(oper->offset->v.sp);
                    rv = peLib->AllocateOperand(field);
                }
            }
            else
            {
                EXPRESSION *en = GetSymRef(oper->offset);
                SYMBOL *sp = NULL;
                if (en)
                {
                    sp = en->v.sp;
                }
                else if (oper->offset->type == en_tempref)
                {
                    sp = (SYMBOL *)oper->offset->right;
                }
                if (sp)
                {
                    if (sp->storage_class == sc_auto || sp->storage_class == sc_register || sp->storage_class == sc_parameter)
                    {
                        rv = peLib->AllocateOperand(GetLocalData(sp));
                    }
                    else
                    {
                        rv = peLib->AllocateOperand(GetFieldData(sp));
                        if (sp->linkage2 == lk_property)
                            rv->Property(true);
                    }
                }
                else if (oper->offset->type != en_tempref)
                {
                    if (oper->offset == objectArray_exp)
                    {
                        Type *oa = peLib->AllocateType(Type::object, 0);
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

void load_ind(int sz)
{
    Instruction::iop op;
    switch(sz)
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
        case ISZ_ADDR:
        {
            Operand *oper = currentMethod->LastInstruction()->GetOperand();
            // check for __va_arg__ on a pointer type
            if (oper && oper->OperandType() == Operand::t_value && typeid(*oper->GetValue()) == typeid(MethodName))
            {
                MethodSignature *test = ((MethodName *)oper->GetValue())->Signature();
                if (test->GetContainer() == ptrUnbox->GetContainer())
                    if (test->Name() == ptrUnbox->Name())
                        return;
            }
            op = Instruction::i_ldind_u4;
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
    }
    gen_code(op, NULL);

}
void store_ind(IMODE *im)
{
    int sz = im->size;
    Instruction::iop op;
    Operand *operand = nullptr;
    if (sz < 0)
        sz = - sz;
    switch(sz)
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
            op = Instruction::i_stind_i4;
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
        case ISZ_OBJECT:
            op = Instruction::i_stobj;
            operand = peLib->AllocateOperand(peLib->AllocateValue("", GetType(im->offset->v.sp->tp, TRUE, 0, 0)));
            break;
    }
    gen_code(op, operand);
    decrement_stack();
    decrement_stack();

}
void load_arithmetic_constant(int sz, Operand *operand)
{
    Instruction::iop op;
    int sz1 = sz < 0 ? - sz : sz;
    switch(sz1)
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
void load_constant(int sz, EXPRESSION *exp)
{
    int sz1;
    Instruction::iop op;
    sz1 = sz;
    if (sz < 0)
        sz1 = - sz;
    Operand *operand = make_constant(sz1, exp);
    switch(sz1)
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
void gen_load(IMODE *im, Operand *dest)
{
    if (dest && dest->OperandType() == Operand::t_string)
    {
        gen_code(Instruction::i_ldstr, dest);
        increment_stack();
        return;
    }
    if (im->mode == i_ind)
    {
        if (im->fieldname)
        {
            EXPRESSION *offset = (EXPRESSION *)im->vararg;
            if (qualifiedStruct(offset->v.sp->parentClass))
            {
                if (offset->v.sp->linkage2 == lk_property)
                {
                    Property *p = static_cast<Property *>(offset->v.sp->msil);
                    p->CallGet(*peLib, currentMethod);
                }
                else
                {
                    Value *field = GetStructField(offset->v.sp);
                    Operand *operand = peLib->AllocateOperand(field);
                    gen_code(Instruction::i_ldfld, operand);
                }
            }
            else
            {
                load_ind(im->size);
            }
        }
        else
        {
            load_ind(im->size);
        }
        return;
    }
    if (!dest)
        return;
    switch(dest->OperandType())
    {

        case Operand::t_int:
        case Operand::t_real:
            load_arithmetic_constant(im->size, dest);
            break;
        case Operand::t_value:
            if (typeid(*dest->GetValue()) == typeid(Local))
            {
                Local *l = static_cast<Local *>(dest->GetValue());
                Type *t = l->GetType();
                bool address = t->GetBasicType() == Type::cls && (t->GetClass()->Flags().Flags() & Qualifiers::Value);
                if (im->mode == i_immed && (!im->msilObject || address))
                    gen_code(Instruction::i_ldloca, dest);
                else
                    gen_code(Instruction::i_ldloc, dest);
                increment_stack();
            }
            else if (typeid(*dest->GetValue()) == typeid(Param))
            {
                Param *p = static_cast<Param *>(dest->GetValue());
                Type *t = p->GetType();
                bool address = t->GetBasicType() == Type::cls && (t->GetClass()->Flags().Flags() & Qualifiers::Value);
                if (im->mode == i_immed && (!im->msilObject || address))
                    gen_code(Instruction::i_ldarga, dest);
                else
                    gen_code(Instruction::i_ldarg, dest);
                increment_stack();
            }
            else if (typeid(*dest->GetValue()) == typeid(MethodName))
            {
                gen_code(Instruction::i_ldftn, dest);
                increment_stack();
            }
            else // fieldname
            {
                if (dest->Property())
                {
                    Property *p = reinterpret_cast<Property *>(static_cast<FieldName *>(dest->GetValue())->GetField());
                    p->CallGet(*peLib, currentMethod);
                    increment_stack();
                }
                else if (im->offset->type == en_structelem)
                {
                    FieldName *f = static_cast<FieldName *>(dest->GetValue());
                    Type *t = f->GetField()->FieldType();
                    bool address = t->GetBasicType() == Type::cls && (t->GetClass()->Flags().Flags() & Qualifiers::Value);
                    if (im->mode == i_immed && (!im->msilObject || address))
                        gen_code(Instruction::i_ldflda, dest);
                    else
                        gen_code(Instruction::i_ldfld, dest);
                }
                else
                {
                    FieldName *f = static_cast<FieldName *>(dest->GetValue());
                    Type *t = f->GetField()->FieldType();
                    bool address = t->GetBasicType() == Type::cls && (t->GetClass()->Flags().Flags() & Qualifiers::Value);
                    if (im->mode == i_immed && (!im->msilObject || address))
                        gen_code(Instruction::i_ldsflda, dest);
                    else
                        gen_code(Instruction::i_ldsfld, dest);
                    increment_stack();
                }
            }
            break;
    }
}
void gen_store(IMODE *im, Operand *dest)
{
    if (im->mode == i_ind)
    {
        if (im->fieldname)
        {
            EXPRESSION *offset = (EXPRESSION *)im->vararg;
            if (qualifiedStruct(offset->v.sp->parentClass))
            {
                if (offset->v.sp->linkage2 == lk_property)
                {
                    Property *p = static_cast<Property *>(offset->v.sp->msil);
                    p->CallSet(*peLib, currentMethod);
                    decrement_stack();
                    decrement_stack();
                }
                else
                {
                    Value *field = GetStructField(offset->v.sp);
                    Operand *operand = peLib->AllocateOperand(field);
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
        else // fieldname
        {
            if (dest->Property())
            {
                Property *p = reinterpret_cast<Property *>(static_cast<FieldName *>(dest->GetValue())->GetField());
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
void gen_convert(Operand *dest, IMODE *im, int sz)
{
    Instruction::iop op;
    switch(sz)
    {
        case ISZ_UNATIVE:
            op = Instruction::i_conv_u;
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
            op = Instruction::i_conv_u4;
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
        case ISZ_STRING:
        {
            box(im);
            MethodSignature *sig = toStr;
            Operand *ap = peLib->AllocateOperand(peLib->AllocateMethodName(sig));
            gen_code(Instruction::i_call, ap);
            return;
        }
    }
    gen_code(op, NULL);
}
void gen_branch(Instruction::iop op, int label, BOOLEAN decrement)
{
    char lbl[256];
    sprintf(lbl, "L_%d_%x", label, uniqueId);
    Operand *operand = peLib->AllocateOperand(lbl);
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

extern "C" void asm_expressiontag(QUAD *q)
{
    if (!q->dc.v.label)
    {
        // expression tags can be nested...
        int n = 1;
        q = q->back;
        while (n && (q->dc.opcode == i_line || q->dc.opcode == i_expressiontag))
        {
            if (q->dc.opcode == i_expressiontag)
                if (q->dc.v.label)
                    n--;
                else
                    n++;
            q = q->back;
        }
        if (n)
        {

            gen_code(Instruction::i_pop, NULL);
            decrement_stack();
        }
    }
}
extern "C" void asm_tag(QUAD *q)
{
    if (q->beforeGosub)
    {
        QUAD *find = q;
        while (find && find->dc.opcode != i_gosub)
            find = find->fwd;
        if (find)
        {
            FUNCTIONCALL *params = (FUNCTIONCALL *)find->altdata;
            if (msil_managed(params->sp) || find->dc.left->mode != i_immed)
            {
                if (params->vararg)
                {
                    if (strcmp(params->sp->name, "__va_arg__"))
                    {
                        if (find->nullvararg)
                            gen_code(Instruction::i_ldnull, NULL);
                        else
                            gen_code(Instruction::i_ldloc, peLib->AllocateOperand(localList[objectArray_exp->v.sp->offset]));
                    }
                }
            }
        }
    }
}
extern "C" void asm_line(QUAD *q)               /* line number information and text */
{
    char buf[10000];
    LINEDATA *ld = (LINEDATA *)q->dc.left;
    sprintf(buf, "Line %d: %s", ld->lineno, ld->line);
    Instruction *i = peLib->AllocateInstruction(Instruction::i_comment, buf);
    currentMethod->AddInstruction(i);
}
extern "C" void asm_blockstart(QUAD *q)               /* line number information and text */
{
}
extern "C" void asm_blockend(QUAD *q)               /* line number information and text */
{
}
extern "C" void asm_varstart(QUAD *q)               /* line number information and text */
{
}
extern "C" void asm_func(QUAD *q)               /* line number information and text */
{
}
extern "C" void asm_passthrough(QUAD *q)        /* reserved */
{
}
extern "C" void asm_datapassthrough(QUAD *q)        /* reserved */
{
}
extern "C" void asm_label(QUAD *q)              /* put a label in the code stream */
{
    oa_gen_label(q->dc.v.label);
}
extern "C" void asm_goto(QUAD *q)               /* unconditional branch */
{
    if (q->dc.opcode == i_goto)
        gen_branch(Instruction::i_br, q->dc.v.label, FALSE);
    else
    {
        // i don't know if this is kosher in the middle of a function...
        gen_code(Instruction::i_tail_, 0);
        gen_code(Instruction::i_calli, 0);
    }

}
BoxedType *boxedType(int isz)
{
    static Type::BasicType names[] = { Type::u32, Type::u32, Type::u8, Type::u8,
        Type::u16, Type::u16, Type::u16, Type::u32, Type::unative, Type::u32, Type::u32,
        Type::u64, Type::u32, Type::u32, Type::u16, Type::u16, Type::r32, Type::r64,
        Type::r64, Type::r32, Type::r64, Type::r64,
    };
    static Type::BasicType mnames[] = { Type::i32, Type::i32, Type::i8, Type::i8,
        Type::i16, Type::i16, Type::i16, Type::i32, Type::inative, Type::i32, Type::i32,
        Type::i64, Type::i32, Type::i32, Type::i16, Type::i16, Type::r32, Type::r64,
        Type::r64, Type::r32, Type::r64, Type::r64,
    };
    if (isz == ISZ_OBJECT)
        return NULL;
    Type::BasicType n;
    if (isz == ISZ_STRING)
        n = Type::string;
    else
        n = isz < 0 ? mnames[-isz] : names[isz];
    return peLib->AllocateBoxedType(n);
}
void box(IMODE *im)
{
    BoxedType *type = boxedType(im->size);
    if (type)
    {
        Operand *operand = peLib->AllocateOperand(peLib->AllocateValue("", type));
        gen_code(Instruction::i_box, operand);
    }
}
void unbox(int val)
{
    static Type::BasicType typeNames[] = { Type::i8, Type::i8, Type::i8, Type::i8, Type::u8,
        Type::i16, Type::i16, Type::u16, Type::u16, Type::i32, Type::i32, Type::inative, Type::i32, Type::u32, Type::unative, Type::i32, Type::u32,
        Type::i64, Type::u64, Type::r32, Type::r64, Type::r64, Type::r32, Type::r64, Type::r64 };
    Operand *op1 = peLib->AllocateOperand(peLib->AllocateValue("", peLib->AllocateType(typeNames[val], 0)));
    if (op1)
        gen_code(Instruction::i_unbox, op1);
}
// this implementation won't handle varag functions nested in other varargs...
extern "C" void asm_parm(QUAD *q)               /* push a parameter*/
{
    if (q->vararg)
    {
        if (q->dc.left->size == ISZ_ADDR)
        {
            Operand *operand = peLib->AllocateOperand(peLib->AllocateMethodName(ptrBox));
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
    else if (q->valist && q->valist->type == en_l_p)
    {
        QUAD *find = q;
        while (find && find->dc.opcode != i_gosub)
            find = find->fwd;
        if (find)
        {
            FUNCTIONCALL *params = (FUNCTIONCALL *)find->altdata;
            if (!msil_managed(params->sp))
            {
                Operand *operand = peLib->AllocateOperand(peLib->AllocateMethodName(argsUnmanaged));
                gen_code (Instruction::i_callvirt, operand);
            }
        }
    }
}
extern "C" void asm_parmblock(QUAD *q)          /* push a block of memory */
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
        Instruction *i = currentMethod->LastInstruction();
        if (i->OpCode() == Instruction::i_ldloc || i->OpCode() == Instruction::i_ldarg || i->OpCode() == Instruction::i_ldsfld)
        {
            return;
        }
        // no it is a member of a structure, we have to load it
        Type *tp = GetType((TYPE *)q->altdata, TRUE);
        gen_code(Instruction::i_ldobj, peLib->AllocateOperand(peLib->AllocateValue("", tp)));
    }
}
extern "C" void asm_parmadj(QUAD *q)            /* adjust stack after function call */
{
    int i;
    int n = beGetIcon(q->dc.left) - beGetIcon(q->dc.right);
    if (n > 0)
        for (i=0; i < n; i++)
            decrement_stack();
    else if (n < 0)
        increment_stack();
}
static BOOLEAN bltin_gosub(QUAD *q)
{
	EXPRESSION *en = GetSymRef(q->dc.left->offset);

	Operand *operand;
	if (q->dc.left->mode == i_immed)
	{
		SYMBOL *sp = en->v.sp;
		if (!strcmp(sp->name, "__va_start__"))
		{
			EXPRESSION *en = GetSymRef(q->dc.left->offset);
			en->v.sp->genreffed = FALSE;
			Operand *op1 = peLib->AllocateOperand(GetParamData("__va_list__"));
			gen_code(Instruction::i_ldarg, op1);
			op1 = peLib->AllocateOperand(peLib->AllocateMethodName(argsCtor));
			gen_code(Instruction::i_newobj, op1);
			return TRUE;
		}
		else if (!strcmp(sp->name, "__va_arg__"))
		{
			EXPRESSION *en = GetSymRef(q->dc.left->offset);
			en->v.sp->genreffed = FALSE;
			FUNCTIONCALL *func = (FUNCTIONCALL *)q->altdata;
			TYPE *tp = en->v.sp->tp;
			if (func->arguments->next)
				tp = func->arguments->next->tp;
			Operand *operand = peLib->AllocateOperand(peLib->AllocateMethodName(argsNextArg));
			// the function pushes both an arglist val and a type to cast to on the stack
			// remove the type to cast to.
			currentMethod->RemoveLastInstruction();
			gen_code(Instruction::i_callvirt, operand);
			if (ispointer(tp))
			{
				Operand *operand = peLib->AllocateOperand(peLib->AllocateMethodName(ptrUnbox));
				gen_code(Instruction::i_call, operand);
			}
			else if (!isstructured(tp) && !isarray(tp))
			{
                EXPRESSION *exp = func->arguments->next->exp;
                unbox(basetype(exp->v.sp->tp)->type);
			}
			return TRUE;
		}
	}
    return FALSE;
}
extern "C" void asm_gosub(QUAD *q)              /* normal gosub to an immediate label or through a var */
{
    if (q->dc.left->mode == i_immed)
    {
        if (!bltin_gosub(q))
        {
			Operand *ap = getCallOperand(q);
            if (!strcmp(q->dc.left->offset->v.sp->name, ".ctor"))
            {
                gen_code(Instruction::i_newobj, ap);
                increment_stack();
            }
            else
            {
                gen_code(Instruction::i_call, ap);
            }
        }
    }
    else
    {
		Operand *ap = getCallOperand(q);
		gen_code(Instruction::i_calli, ap);
        decrement_stack();
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
extern "C" void asm_fargosub(QUAD *q)           /* far version of gosub */
{
}
extern "C" void asm_trap(QUAD *q)               /* 'trap' instruction - the arg will be an immediate # */
{
}
extern "C" void asm_int(QUAD *q)                /* 'int' instruction(QUAD *q) calls a labeled function which is an interrupt */
{
}
/* left will be a constant holding the number of bytes to pop
 * e.g. the parameters will be popped in stdcall or pascal type functions
 */
extern "C" void asm_ret(QUAD *q)                /* return from subroutine */
{
    gen_code(Instruction::i_ret, NULL);    
}
/* left will be a constant holding the number of bytes to pop
 * e.g. the parameters will be popped in stdcall or pascal type functions
 */
extern "C" void asm_fret(QUAD *q)                /* far return from subroutine */
{
}
/*
 * this can be either a fault or iret return
 * for processors that char, the 'left' member will have an integer
 * value that is TRUE for an iret or false or a fault ret
 */
extern "C" void asm_rett(QUAD *q)               /* return from trap or int */
{
}
extern "C" void asm_add(QUAD *q)                /* evaluate an addition */
{
    if (q->dc.right->offset && q->dc.right->offset->type == en_structelem)
    {
        // the 'add' for a structure offset had to remain until now because it 
        // held the left and right side both in the expression trees...   
        // it is ignored for 'direct' access by used as a field address for 
        // immed access
        if (q->dc.right->mode == i_immed)
        {
            Operand *ap = getOperand(q->dc.right);
            gen_load(q->dc.right, ap);
        }

    }
    // only time we generate add for non arithmetic types is for strings
    else if (q->dc.left->size == ISZ_STRING || q->dc.right->size == ISZ_STRING ||
            q->dc.left->size == ISZ_OBJECT && q->dc.right->size == ISZ_OBJECT)
    {
        MethodSignature *sig;
        if (q->dc.left->size == q->dc.right->size && q->dc.left->size == ISZ_STRING)
        {
            sig = concatStr;
        }
        else
        {
            sig = concatObj;
        }
        Operand *ap = peLib->AllocateOperand(peLib->AllocateMethodName(sig));
        gen_code(Instruction::i_call, ap);
        decrement_stack();
    }
    else
    {
        decrement_stack();
        gen_code(Instruction::i_add, NULL);
    }
}
extern "C" void asm_sub(QUAD *q)                /* evaluate a subtraction */
{
    decrement_stack();
    gen_code(Instruction::i_sub, NULL);
}
extern "C" void asm_udiv(QUAD *q)               /* unsigned division */
{
    decrement_stack();
    gen_code(Instruction::i_div_un, NULL);
}
extern "C" void asm_umod(QUAD *q)               /* unsigned modulous */
{
    decrement_stack();
    gen_code(Instruction::i_rem_un, NULL);
}
extern "C" void asm_sdiv(QUAD *q)               /* signed division */
{
    decrement_stack();
    gen_code(Instruction::i_div, NULL);
}
extern "C" void asm_smod(QUAD *q)               /* signed modulous */
{
    decrement_stack();
    gen_code(Instruction::i_rem, NULL);
}
extern "C" void asm_muluh(QUAD *q)
{
    EXPRESSION *en = intNode(en_c_i, 32);
    Operand *ap = make_constant(ISZ_UINT, en);
    gen_code(Instruction::i_mul,NULL);
    gen_code(Instruction::i_ldc_i4, ap);
    gen_code(Instruction::i_shr_un, NULL);
    decrement_stack();
}
extern "C" void asm_mulsh(QUAD *q)
{
    EXPRESSION *en = intNode(en_c_i, 32);
    Operand *ap = make_constant(ISZ_UINT, en);
    gen_code(Instruction::i_mul,NULL);
    gen_code(Instruction::i_ldc_i4, ap);
    gen_code(Instruction::i_shr, NULL);
    decrement_stack();
}
extern "C" void asm_mul(QUAD *q)               /* signed multiply */
{
    decrement_stack();
    gen_code(Instruction::i_mul, NULL);
}
extern "C" void asm_lsr(QUAD *q)                /* unsigned shift right */
{
    decrement_stack();
    gen_code(Instruction::i_shr_un, NULL);
}
extern "C" void asm_lsl(QUAD *q)                /* signed shift left */
{
    decrement_stack();
    gen_code(Instruction::i_shl, NULL);
}
extern "C" void asm_asr(QUAD *q)                /* signed shift right */
{
    decrement_stack();
    gen_code(Instruction::i_shr, NULL);
}
extern "C" void asm_neg(QUAD *q)                /* negation */
{
    gen_code(Instruction::i_neg, NULL);
}
extern "C" void asm_not(QUAD *q)                /* complement */
{
    gen_code(Instruction::i_not, NULL);
}
extern "C" void asm_and(QUAD *q)                /* binary and */
{
    decrement_stack();
    gen_code(Instruction::i_and, NULL);
}
extern "C" void asm_or(QUAD *q)                 /* binary or */
{
    decrement_stack();
    gen_code(Instruction::i_or, NULL);
}
extern "C" void asm_eor(QUAD *q)                /* binary exclusive or */
{
    decrement_stack();
    gen_code(Instruction::i_xor, NULL);
}
extern "C" void asm_setne(QUAD *q)              /* evaluate a = b != c */
{
    gen_code(Instruction::i_ceq, NULL);
    gen_code(Instruction::i_ldc_i4_1, NULL);
    gen_code(Instruction::i_xor, NULL);
    increment_stack();
    decrement_stack();
    decrement_stack();
}
extern "C" void asm_sete(QUAD *q)               /* evaluate a = b == c */
{
    gen_code(Instruction::i_ceq, NULL);
    decrement_stack();
}
extern "C" void asm_setc(QUAD *q)               /* evaluate a = b U< c */
{
    gen_code(Instruction::i_clt_un, NULL);
    decrement_stack();
}
extern "C" void asm_seta(QUAD *q)               /* evaluate a = b U> c */
{
    gen_code(Instruction::i_cgt_un, NULL);
    decrement_stack();
}
extern "C" void asm_setnc(QUAD *q)              /* evaluate a = b U>= c */
{
    gen_code(Instruction::i_clt_un, NULL);
    gen_code(Instruction::i_ldc_i4_1, NULL);
    gen_code(Instruction::i_xor, NULL);
    increment_stack();
    decrement_stack();
    decrement_stack();
}
extern "C" void asm_setbe(QUAD *q)              /* evaluate a = b U<= c */
{
    gen_code(Instruction::i_cgt_un, NULL);
    gen_code(Instruction::i_ldc_i4_1, NULL);
    gen_code(Instruction::i_xor, NULL);
    increment_stack();
    decrement_stack();
    decrement_stack();
}
extern "C" void asm_setl(QUAD *q)               /* evaluate a = b S< c */
{
    gen_code(Instruction::i_clt, NULL);
    decrement_stack();
}
extern "C" void asm_setg(QUAD *q)               /* evaluate a = b s> c */
{
    gen_code(Instruction::i_cgt, NULL);
    decrement_stack();
}
extern "C" void asm_setle(QUAD *q)              /* evaluate a = b S<= c */
{
    gen_code(Instruction::i_cgt, NULL);
    gen_code(Instruction::i_ldc_i4_1, NULL);
    gen_code(Instruction::i_xor, NULL);
    increment_stack();
    decrement_stack();
    decrement_stack();
}
extern "C" void asm_setge(QUAD *q)              /* evaluate a = b S>= c */
{
    gen_code(Instruction::i_clt, NULL);
    gen_code(Instruction::i_ldc_i4_1, NULL);
    gen_code(Instruction::i_xor, NULL);
    increment_stack();
    decrement_stack();
    decrement_stack();
}
extern "C" void asm_assn(QUAD *q)               /* assignment */
{
    Operand *ap;

    if (q->ans && q->ans->mode == i_immed && q->ans->offset->type == en_msil_array_access)
    {
        Type *tp = GetType(q->ans->offset->v.msilArray->tp, TRUE);
        if (tp->ArrayLevel() == 1)
        {
            Instruction::iop instructions[] = {
                    Instruction::i_stelem, Instruction::i_stelem, Instruction::i_stelem, Instruction::i_stelem_i1, 
                    Instruction::i_stelem_i2, Instruction::i_stelem_i1, Instruction::i_stelem_i1, Instruction::i_stelem_i2, Instruction::i_stelem_i2, Instruction::i_stelem_i4, Instruction::i_stelem_i4, Instruction::i_stelem_i8, Instruction::i_stelem_i8, Instruction::i_stelem_i, Instruction::i_stelem_i,
                    Instruction::i_stelem_r4, Instruction::i_stelem_r8, Instruction::i_stelem, Instruction::i_stelem
            };
            gen_code(instructions[tp->GetBasicType()], NULL);
            decrement_stack();
            decrement_stack();
            decrement_stack();
        }
        else
        {
            MethodSignature *sig = LookupArrayMethod(tp, "Set");
            Operand *operand = peLib->AllocateOperand(peLib->AllocateMethodName(sig));
            gen_code(Instruction::i_call, operand);
            int n = tp->ArrayLevel();
            for (int i=0; i < n+2; i++)
                decrement_stack();
        }
        return;
    }
    else if (q->dc.left && q->dc.left->mode == i_immed && q->dc.left->offset->type == en_msil_array_init)
    {
        if (!isarray(basetype(q->dc.left->offset->v.tp)->btp))
        {
            Type *tp = boxedType(q->ans->size);
            Operand *operand = peLib->AllocateOperand(peLib->AllocateValue("", tp));
            gen_code(Instruction::i_newarr, operand);

        }
        else
        {
            Type *tp = GetType(q->dc.left->offset->v.tp, TRUE);
            MethodSignature *sig = LookupArrayMethod(tp, ".ctor");
            Operand *operand = peLib->AllocateOperand(peLib->AllocateMethodName(sig));
            gen_code(Instruction::i_newobj, operand);
            int n = tp->ArrayLevel();
            for (int i=0; i < n-1; i++)
                decrement_stack();
        }
    }
    else
    {
        // don't generate if it is a placeholder ind...
        if (q->ans->mode == i_direct && !(q->temps & TEMP_ANS) && q->ans->offset->type == en_auto)
        {
            if (objectArray_exp && q->ans->offset->v.sp == objectArray_exp->v.sp)
            {
                // assign to object array, call the ctor here
                // count is already on the stack
                Operand *ap = peLib->AllocateOperand(peLib->AllocateValue("", systemObject));
                gen_code(Instruction::i_newarr, ap);
                ap = getOperand(q->ans);
                gen_store(q->ans, ap);
                return;
            }
        }
        ap = getOperand(q->dc.left);
        gen_load(q->dc.left, ap);
        if (q->dc.left->size != 0 && q->dc.left->size != q->ans->size)
        {
            gen_convert(ap, q->dc.left, q->ans->size);
        }
    }
    ap = getOperand(q->ans);
    gen_store(q->ans, ap);
    if (q->ans->retval)
        returnCount++;
    if (q->hook)
        hookCount++;
}
extern "C" void asm_genword(QUAD *q)            /* put a byte or word into the code stream */
{
}
void compactgen(Instruction *i, int lab)
{
    char buf[256];
    sprintf(buf, "L_%d_%x", lab, uniqueId);
    i->AddCaseLabel(buf);
}
void bingen(int lower, int avg, int higher)
{
    int nelab = beGetLabel;
    if (switchTreeBranchLabels[avg] !=  0)
        oa_gen_label(switchTreeBranchLabels[avg]);
    gen_load(switch_ip, switch_ip_a);
    load_constant(switch_ip->size, intNode(en_c_i, switchTreeCases[avg]));
    gen_branch(Instruction::i_beq,  switchTreeLabels[avg], TRUE);
    if (avg == lower)
    {
        gen_branch(Instruction::i_br, switch_deflab, FALSE);
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
        gen_load(switch_ip, switch_ip_a);
        load_constant(switch_ip->size, intNode(en_c_i, switchTreeCases[avg]));
        if (switch_ip->size < 0)
            gen_branch(Instruction::i_bgt, lab, TRUE);
        else
            gen_branch(Instruction::i_bgt_un, lab, TRUE);
        bingen(lower, avg1, avg);
        if (avg + 1 < higher)
            bingen(avg + 1, avg2, higher);
    }
}

extern "C" void asm_coswitch(QUAD *q)           /* switch characteristics */
{
    Instruction::iop op;
     switch_deflab = q->dc.v.label;
    switch_range = q->dc.right->offset->v.i;
    switch_case_max = switch_case_count = q->ans->offset->v.i;
    switch_ip = q->dc.left;
    switch_ip_a = getOperand(switch_ip);
    if (switch_ip->size == ISZ_ULONGLONG || switch_ip->size == - ISZ_ULONGLONG || switch_case_max <= 5)
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
        if (!switchTreeLabelCount || switchTreeLabelCount  < switch_case_max)
        {
            free(switchTreeCases);
            free(switchTreeLabels);
            free(switchTreeBranchLabels);
            switchTreeLabelCount = (switch_case_max + 1024) & ~1023;
            switchTreeCases = (LLONG_TYPE *)calloc(switchTreeLabelCount, sizeof (LLONG_TYPE));
            switchTreeLabels = (int *)calloc(switchTreeLabelCount, sizeof (int));
            switchTreeBranchLabels = (int *)calloc(switchTreeLabelCount, sizeof (int));
        }
        switchTreePos = 0;
        memset(switchTreeBranchLabels, 0, sizeof(int) * switch_case_max);
    }
}
extern "C" void asm_swbranch(QUAD *q)           /* case characteristics */
{
    static Instruction *swins;
    ULLONG_TYPE swcase = q->dc.left->offset->v.i;
    int labin = q->dc.v.label, lab;
    if (switch_case_count == 0)
    {
/*		diag("asm_swbranch, count mismatch"); in case only a default */
        return;
    }

    if (switch_mode == swm_compactstart)
    {
        gen_load(switch_ip, switch_ip_a);
        if (swcase != 0)
        {
            load_constant(switch_ip->size, intNode(en_c_i, swcase));
            gen_code(Instruction::i_sub, NULL);
            decrement_stack();
        }
        swins = gen_code(Instruction::i_switch, NULL);
        gen_branch(Instruction::i_br, switch_deflab, FALSE);
    }
    switch(switch_mode)
    {
        int lab;
        case swm_enumerate:
        default:

            gen_load(switch_ip, switch_ip_a);
            load_constant(switch_ip->size, intNode(en_c_i, swcase));
            gen_branch(Instruction::i_beq, labin, TRUE);
            if (-- switch_case_count == 0)
            {
                gen_branch(Instruction::i_br, switch_deflab, FALSE);
            }
            break ;
        case swm_compact:
            while(switch_lastcase < swcase)
            {
                compactgen(swins, switch_deflab);
                switch_lastcase++;
            }
            // fall through
        case swm_compactstart:
            compactgen(swins, labin);
            switch_lastcase = swcase + 1;
            switch_mode = swm_compact;
            -- switch_case_count;
            if (!switch_case_count)
                decrement_stack();
            break ;
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
            break ;
    }
    
}
extern "C" void asm_dc(QUAD *q)                 /* unused */
{
}
extern "C" void asm_assnblock(QUAD *q)          /* copy block of memory*/
{
    gen_code(Instruction::i_cpblk, 0);
    decrement_stack();
    decrement_stack();
    decrement_stack();
}
extern "C" void asm_clrblock(QUAD *q)           /* clear block of memory */
{
    // the 'value' field is loaded by examine_icode...
    gen_code(Instruction::i_initblk, 0);
    decrement_stack();
    decrement_stack();
    decrement_stack();
}
extern "C" void asm_jc(QUAD *q)                 /* branch if a U< b */
{
    gen_branch(Instruction::i_blt_un, q->dc.v.label, TRUE);
}
extern "C" void asm_ja(QUAD *q)                 /* branch if a U> b */
{
    gen_branch(Instruction::i_bgt_un, q->dc.v.label, TRUE);
    
}
extern "C" void asm_je(QUAD *q)                 /* branch if a == b */
{
    if (q->dc.right->mode == i_immed && isconstzero(&stdint, q->dc.right->offset))
        gen_branch(Instruction::i_brfalse, q->dc.v.label, TRUE);
    else
        gen_branch(Instruction::i_beq, q->dc.v.label, TRUE);
    
}
extern "C" void asm_jnc(QUAD *q)                /* branch if a U>= b */
{
    gen_branch(Instruction::i_bge_un, q->dc.v.label, TRUE);
    
}
extern "C" void asm_jbe(QUAD *q)                /* branch if a U<= b */
{
    gen_branch(Instruction::i_ble_un, q->dc.v.label, TRUE);
    
}
extern "C" void asm_jne(QUAD *q)                /* branch if a != b */
{
    if (q->dc.right->mode == i_immed && isconstzero(&stdint, q->dc.right->offset))
        gen_branch(Instruction::i_brtrue, q->dc.v.label, TRUE);
    else
        gen_branch(Instruction::i_bne_un, q->dc.v.label, TRUE);
    
}
extern "C" void asm_jl(QUAD *q)                 /* branch if a S< b */
{
    gen_branch(Instruction::i_blt, q->dc.v.label, TRUE);

}
extern "C" void asm_jg(QUAD *q)                 /* branch if a S> b */
{
    gen_branch(Instruction::i_bgt, q->dc.v.label, TRUE);

}
extern "C" void asm_jle(QUAD *q)                /* branch if a S<= b */
{
    gen_branch(Instruction::i_ble, q->dc.v.label, TRUE);
    
}
extern "C" void asm_jge(QUAD *q)                /* branch if a S>= b */
{
    gen_branch(Instruction::i_bge, q->dc.v.label, TRUE);
    
}
extern "C" void asm_cppini(QUAD *q)             /* cplusplus initialization (historic)*/
{
    (void)q;    
}
/*
 * function prologue.  left has a constant which is a bit mask
 * of registers to push.  It also has a flag indicating whether frames
 * are absolutely necessary
 *
 * right has the number of bytes to allocate on the stack
 */
extern "C" void asm_prologue(QUAD *q)           /* function prologue */
{
    stackpos = 0;
    returnCount = 0;
    hookCount = 0;
    LoadLocals(theCurrentFunc);
    LoadParams(theCurrentFunc);
    for (int i=0; i < localList.size(); i++)
        currentMethod->AddLocal(localList[i]);
}
/*
 * function epilogue, left holds the mask of which registers were pushed
 */
extern "C" void asm_epilogue(QUAD *q)           /* function epilogue */
{
    if (basetype(theCurrentFunc->tp)->btp->type != bt_void)
        stackpos--;
    if (returnCount)
        stackpos -= returnCount -1;
    stackpos -= hookCount/2;
    if (stackpos != 0)
        diag("asm_epilogue: stack mismatch");
}
/*
 * in an interrupt handler, push the current context
 */
extern "C" void asm_pushcontext(QUAD *q)        /* push register context */
{
}
/*
 * in an interrupt handler, pop the current context
 */
extern "C" void asm_popcontext(QUAD *q)         /* pop register context */
{
}
/*
 * loads a context, e.g. for the loadds qualifier
 */
extern "C" void asm_loadcontext(QUAD *q)        /* load register context (e.g. at interrupt level ) */
{
    
}
/*
 * unloads a context, e.g. for the loadds qualifier
 */
extern "C" void asm_unloadcontext(QUAD *q)        /* load register context (e.g. at interrupt level ) */
{
    
}
extern "C" void asm_tryblock(QUAD *q)			 /* try/catch */
{
}
extern "C" void asm_stackalloc(QUAD *q)         /* allocate stack space - positive value = allocate(QUAD *q) negative value deallocate */
{
}
extern "C" void asm_loadstack(QUAD *q)			/* load the stack pointer from a var */
{
}
extern "C" void asm_savestack(QUAD *q)			/* save the stack pointer to a var */
{
}
extern "C" void asm_functail(QUAD *q, int begin, int size)	/* functail start or end */
{
}
extern "C" void asm_atomic(QUAD *q)
{
}
QUAD * leftInsertionPos(QUAD *head, IMODE *im)
{
    QUAD *rv = head;
    head = head->back;
    while (head && head->dc.opcode != i_block)
    {
        if (head->temps & TEMP_ANS)
        {
            if (im->offset->v.sp->value.i == head->ans->offset->v.sp->value.i)
            {
                rv = head;
                if (!(head->temps & TEMP_LEFT))
                    break;
                im = head->dc.left;
                if (im->offset->v.sp->pushedtotemp)
                    break;
            }
        }
        head = head->back;
    }
    return rv;
}
int examine_icode(QUAD *head)
{
    int parmIndex = 0;
    IMODE *fillinvararg = NULL;
    while (head)
    {
        if (head->dc.opcode == i_gosub)
        {
            if (fillinvararg)
                fillinvararg->offset->v.i = parmIndex;
            if (!parmIndex && ((FUNCTIONCALL *)head->altdata)->vararg)
                head->nullvararg = TRUE;
            fillinvararg = NULL;
            parmIndex = 0;
        }
        if (head->dc.opcode != i_block && head->dc.opcode != i_blockend 
            && head->dc.opcode != i_dbgblock && head->dc.opcode != i_dbgblockend && head->dc.opcode != i_var
            && head->dc.opcode != i_label && head->dc.opcode != i_line && head->dc.opcode != i_passthrough
            && head->dc.opcode != i_func && head->dc.opcode != i_gosub && head->dc.opcode != i_parmadj
            && head->dc.opcode != i_ret && head->dc.opcode != i_varstart && head->dc.opcode != i_parmblock
            && head->dc.opcode != i_coswitch && head->dc.opcode != i_swbranch
            && head->dc.opcode != i_expressiontag)
        {
            if (head->dc.opcode == i_muluh || head->dc.opcode == i_mulsh)
            {
                int sz = head->dc.opcode == i_muluh ? ISZ_ULONGLONG : - ISZ_ULONGLONG;
                IMODE *ap = InitTempOpt(sz, sz);
                QUAD *q = (QUAD *)Alloc(sizeof(QUAD));
                q->dc.opcode = i_assn;
                q->ans = ap;
                q->temps = TEMP_ANS;
                q->dc.left = head->dc.left;
                head->dc.left = ap;
                head->temps |= TEMP_LEFT;
                InsertInstruction(head->back, q);
                head->dc.right->size = sz;
            }
            if (head->dc.left && head->dc.left->mode == i_immed && head->dc.opcode != i_assn)
            {
                IMODE *ap = InitTempOpt(head->dc.left->size, head->dc.left->size);
                QUAD *q = (QUAD *)Alloc(sizeof(QUAD)), *t;
                q->dc.opcode = i_assn;
                q->ans = ap;
                q->temps = TEMP_ANS;
                q->dc.left = head->dc.left;
                head->dc.left = ap;
                head->temps |= TEMP_LEFT;
                t = head;
                if (head->temps & TEMP_RIGHT)
                    t = leftInsertionPos(head, head->dc.right);
                InsertInstruction(t->back, q);
            }
            /*
            if (head->dc.opcode == i_clrblock)
            {
                // insert the value to clear it to, e.g. zero
                IMODE *ap = InitTempOpt(head->dc.right->size, head->dc.right->size);
                QUAD *q = (QUAD *)Alloc(sizeof(QUAD));
                q->alwayslive = TRUE;
                q->dc.opcode = i_assn;
                q->ans = ap;
                q->temps = TEMP_ANS;
                q->dc.left = (IMODE *)Alloc(sizeof(IMODE));
                q->dc.left->mode = i_immed;
                q->dc.left->offset = intNode(en_c_i, 0);
                InsertInstruction(head->back, q);
            }
            */
            if (head->dc.right && head->dc.right->offset->type == en_structelem)
            {
                // by definition this is an add node...
                if (!qualifiedStruct(head->dc.right->offset->v.sp->parentClass))
                {
                    head->dc.right-> mode = i_immed;
                    head->dc.right->offset = intNode(en_c_i, head->dc.right->offset->v.sp->offset);
                }
            }
            if (head->dc.right && head->dc.right->mode == i_immed)
            {
                if (head->dc.opcode != i_je && head->dc.opcode != i_jne || !isconstzero(&stdint, head->dc.right->offset))
                {
                    if (head->dc.right->offset->type != en_structelem)
                    {
                        IMODE *ap = InitTempOpt(head->dc.right->size, head->dc.right->size);
                        QUAD *q = (QUAD *)Alloc(sizeof(QUAD));
                        q->dc.opcode = i_assn;
                        q->ans = ap;
                        q->temps = TEMP_ANS;
                        q->dc.left = head->dc.right;
                        head->dc.right = ap;
                        head->temps |= TEMP_RIGHT;
                        InsertInstruction(head->back, q);
                    }
                }
            }
            if (head->vararg)
            {
                // handle varargs... this won't work in the case of nested vararg funcs 
                QUAD *q = (QUAD *)Alloc(sizeof(QUAD));
                QUAD *q1 = (QUAD *)Alloc(sizeof(QUAD));
                IMODE *ap = InitTempOpt(ISZ_ADDR, ISZ_ADDR);
                IMODE *ap1 = (IMODE *)Alloc(sizeof(IMODE));
                IMODE *ap2 = InitTempOpt(-ISZ_UINT, -ISZ_UINT);
                IMODE *ap3 = make_immed(-ISZ_UINT, parmIndex++);
                QUAD *prev = head;
                ap1->offset = objectArray_exp;
                ap1->mode = i_direct;
                ap1->size = ISZ_ADDR;
                while (prev->back && !prev->back->varargPrev)
                    prev = prev->back;
                if (parmIndex - 1 == 0)
                {
                    // this is for the initialization of the object array
                    QUAD *q2 = (QUAD *)Alloc(sizeof(QUAD));
                    QUAD *q3 = (QUAD *)Alloc(sizeof(QUAD));
                    IMODE *ap4 = InitTempOpt(ISZ_ADDR, ISZ_ADDR);
                    IMODE *ap5 = (IMODE *)Alloc(sizeof(IMODE));
                    IMODE *ap6 = (IMODE *)Alloc(sizeof(IMODE));
                    ap6->offset = objectArray_exp;
                    ap6->mode = i_direct;
                    ap6->size = ISZ_ADDR;
                    ap5->mode = i_immed;
                    ap5->size = ISZ_UINT;
                    ap5->offset = intNode(en_c_i, 0);
                    fillinvararg = ap5;
                    q2->dc.opcode = i_assn;
                    q2->ans = ap4;
                    q2->dc.left = ap5;
                    q2->temps = TEMP_ANS;
                    q3->dc.opcode = i_assn;
                    q3->ans = ap6;
                    q3->dc.left = ap4;
                    q3->temps = TEMP_LEFT;
                    InsertInstruction(prev->back, q2);
                    InsertInstruction(prev->back, q3);
                }
                // this is to load this param into the object array
                // it inserts the params need by the stelem.ref
                // the stelem.ref and any boxing are done later...
                q->dc.opcode = i_assn;
                q->ans = ap;
                q->dc.left = ap1;
                q->temps = TEMP_ANS;
                q->alwayslive = TRUE;
                q1->dc.opcode = i_assn;
                q1->ans = ap2;
                q1->dc.left = ap3;
                q1->temps = TEMP_ANS;
                q1->alwayslive = TRUE;
                InsertInstruction(prev->back, q);
                InsertInstruction(prev->back, q1);
            }
        }
        head = head->fwd;
    }
	return 0;
}
