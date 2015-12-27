/*
	Software License Agreement (BSD License)
	
	Copyright (c) 1997-2013, David Lindauer, (LADSoft).
	All rights reserved.
	
	Redistribution and use of this software in source and binary forms, with or without modification, are
	permitted provided that the following conditions are met:
	
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
	
	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
	WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
	PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
	ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
	TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
	ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <windows.h>
#include <stdlib.h>
#include "_rtti.h"
#include <stdio.h>

#define OUR_CPP_EXC_CODE 0xEEFABCD

extern "C" void __global_unwind(void *, void *);
extern "C" void __call_terminate() ;

extern "C" LONG ___xceptionhandle(PEXCEPTION_RECORD p, void *record, PCONTEXT context, void *param) ;

#define CAUGHT 1
#define THROWN 2

int ___xcflags;

bool _RTL_FUNC uncaught_exception()
{
    return ___xcflags == THROWN;
}


// have to do the destroys in reverse order...
static void destroyone(XCTAB *record, XCEPT *blk, XCEPT *catchBlock)
{
    if (blk->flags)
    {
        destroyone(record, blk + 1, catchBlock);
        if (blk->flags & XD_CL_PRIMARY)
        {
            if (blk->startOffs <= record->funcIndex && record->funcIndex < blk->endOffs)
            {
                if (!catchBlock || (catchBlock->startOffs <= blk->endOffs && blk->endOffs < catchBlock->endOffs))
                {
                    // call destructor
                    int elems = 1;
                    RTTI *base = blk->xt;
                    while (base-> flags & XD_ARRAY)
                    {
                        BYTE *elempos = (BYTE *)base->name;
                        while (*elempos)
                            elempos++;
                        elempos++;
                        elems *= *(int *)elempos;
                        base = base->base;
                    }
                    if (base->flags & XD_REF)
                        base = base->base;
                    if (!(base->flags & XD_POINTER) && base->destructor)
                    {
                        BYTE *thisptr = blk->ebpoffs + (BYTE *)record->ebp  + (elems-1) * base->size;
                        for (int i=0; i < elems; i++)
                        {
                            (*base->destructor)(thisptr);
                            thisptr -= base->size;
                        }
                    }
                }
            }
        }
    }
}
static void destroy(XCTAB *record, XCEPT *catchBlock)
{
    XCEPTHEAD *head = record->xceptBlock;
    XCEPT *blk = (XCEPT *)(head+1);
    destroyone(record, blk, catchBlock);
}
static size_t recurseMatchXT(RTTI *xt, RTTI *thrown, size_t offs)
{
    if (!(thrown->flags & XD_ARRAY))
    {
        RTTISTRUCT *rttis = (RTTISTRUCT*)((char *)thrown + (unsigned)&thrown->name - (unsigned)thrown + strlen(thrown->name)+1);
        while (rttis->flags)
        {
            if ((rttis->flags & XD_CL_BASE) && !(rttis->flags & XD_CL_VIRTUAL))
            {
                int offs1;
                if (rttis->xt == xt)
                {
                    return offs + rttis->offset;
                }
                offs1 = recurseMatchXT(xt, rttis->xt, offs + rttis->offset);
                if (offs1 != (size_t)-1)
                    return offs1;
                    
            }
            rttis++; 
        }
    }
    return (size_t)-1;
}
static BOOL matchXT(RTTI *xt, RTTI *thrown, size_t *offs, BOOL recurse)
{
    *offs = 0;
    if (!xt)
        return TRUE;
    if (xt == thrown)
        return TRUE;
    if (xt->flags & XD_REF)
    {
        if (xt->base == thrown)
            return TRUE;
    }
    if (recurse)
    {
        *offs = recurseMatchXT(xt, thrown, 0);
        if (*offs != (size_t)-1)
            return TRUE;
    }
    return FALSE;       
}
static BOOL canThrow(XCTAB *record, PEXCEPTION_RECORD p, PCONTEXT context)
{
    XCEPTHEAD *head = record->xceptBlock;
    THROWREC *throwRec = head->throwRecord;
    if (!throwRec)
        return TRUE; // unspecified
    if (throwRec->flags & XD_NOXC)
    {
        return FALSE;
    }
    else if (throwRec->flags & XD_ALLXC)
    {
        return TRUE;
    }
    else
    {
        RTTI **list = &throwRec->thrownClasses;
        while (*list)
        {
            size_t offs;
            if (matchXT(*list, record->thrownxt, &offs, FALSE))
                return TRUE;
            list++;
        }
        return FALSE;
    }
}
static void instantiate(XCTAB *record, void *dest1 , void *src1)
{
    if (record->cons)
    {
        BYTE *src = (BYTE *)src1;
        BYTE *dest = (BYTE *)dest1;
        for (int i=0; i < record->elems; i++)
        {
            void (*xx)(void *, void *) = (void (*)(void *, void *))record->cons;
            (*xx)(dest, src);
            dest += record->thrownxt->size;
            src += record->thrownxt->size;
        }
    }
    else
    {
        memcpy(record->throwninstance, record->baseinstance, record->elems * record->thrownxt->size);
    }
}
static BOOL matchBlock(XCTAB *record, PEXCEPTION_RECORD p, PCONTEXT context)
{
    XCEPT *candidate = NULL;
    XCEPTHEAD *head = record->xceptBlock;
    XCEPT *blk = (XCEPT *)(head+1);
    THROWREC *throwRec = head->throwRecord;
    XCTAB *orig = (XCTAB *)p->ExceptionInformation[0];
    size_t offs = 0;
    while (blk->flags)
    {
        if (blk->flags & XD_CL_TRYBLOCK)
        {
            if (blk->startOffs <= record->funcIndex && record->funcIndex < blk->endOffs)
            {
                if (matchXT(blk->xt, orig->thrownxt, &offs, TRUE))
                {
                    if (candidate)
                    {
                        // check for inner throw
                        if (blk->startOffs > candidate->startOffs)
                            candidate = blk;
                    }
                    else
                    {
                        candidate = blk;
                    }
                }
            }
        }
        blk++;
    }
    if (candidate)
    {
        record->baseinstance = orig->baseinstance;
        record->thrownxt = orig->thrownxt;
        record->elems = orig->elems;
        record->cons = orig->cons;
        record->eip = candidate->trylabel;
        ___xcflags = record->flags = CAUGHT;
        record->throwninstance = malloc(record->thrownxt->size * record->elems);
        record->instance = (char *)record->throwninstance + offs;
        if (!record->instance)
            __call_terminate();
        instantiate(record, record->throwninstance, record->baseinstance);
        __global_unwind(0, record);
        destroy(record, candidate);
        record->funcIndex = candidate->endOffs;
        context->Eip = (DWORD)candidate->trylabel;
        context->Ebp = record->ebp;
        context->Esp = record->esp;
        return TRUE;
    }
    return FALSE;
}
extern "C" LONG __cppexceptionhandle(PEXCEPTION_RECORD p, void *record, PCONTEXT context, void *param)
{
    if (p->ExceptionFlags & 2) // unwinding
    {
       destroy((XCTAB *)record, NULL);
       return 1; // continue search
    }
	else if (p->ExceptionCode == OUR_CPP_EXC_CODE) 
    {
        if (matchBlock((XCTAB *)record, p, context))
        {
            return 0; // continue execution
        }
        else if (canThrow((XCTAB *)record, p, context))
        {
            return 1; // continue search    
        }
        else
        {
            // abort if can't throw
            context->Eip = (DWORD)__call_terminate;
            context->Ebp = ((XCTAB *)record)->ebp;
            context->Esp = ((XCTAB *)record)->esp;
            return 0;
        }
	}
    return ___xceptionhandle(p, record, context, param);
}
void _ThrowException(void *irecord,void *instance,int arraySize,void *cons,void *xceptBlock_in)
{
    XCTAB *record = (XCTAB *)irecord;
    RTTI *xceptBlock = (RTTI *)xceptBlock_in;
    ULONG_PTR params[1];
    params[0] = (ULONG_PTR)record;
    if (record->flags & THROWN) // in case of nested throws
        __call_terminate();
    ___xcflags = record->flags = THROWN;
    record->elems = arraySize;
    record->baseinstance = malloc(xceptBlock->size * arraySize);
    record->thrownxt = xceptBlock;
    record->cons = cons;
    if (!record->baseinstance)
        __call_terminate();
        
    instantiate(record,record->baseinstance, instance);
	RaiseException(OUR_CPP_EXC_CODE,EXCEPTION_CONTINUABLE,1,(DWORD *)&params[0]) ;
}

void uninstantiate(XCTAB *record, void *instance)
{
    if (record->thrownxt->destructor)
    {
        BYTE *ptr = (BYTE *)instance + record->thrownxt->size * (record->elems - 1);
        for (int i=0; i < record->elems; i++)
        {
            (*record->thrownxt->destructor)(ptr);
            ptr -= record->thrownxt->size;
        }        
    }
    free(instance);
}
void _RethrowException(void *r)
{
    XCTAB *record = (XCTAB *)r;
    ULONG_PTR params[1];
    asm mov eax,[record]
    asm mov [fs:0],eax
    if (!(record->flags & CAUGHT))
        __call_terminate();
    uninstantiate(record, record->throwninstance);    
    params[0] = (ULONG_PTR)record;
	RaiseException(OUR_CPP_EXC_CODE,EXCEPTION_CONTINUABLE,1,(DWORD *)&params[0]) ;    
}
void _CatchCleanup(void *r)
{
    XCTAB *record = (XCTAB *)r;
    ULONG_PTR params[1];
    asm mov eax,[record]
    asm mov [fs:0],eax
    if (!(record->flags & CAUGHT))
        __call_terminate();
    ___xcflags = record->flags = 0;
    uninstantiate(record, record->throwninstance);    
    uninstantiate(record, record->baseinstance);
}
