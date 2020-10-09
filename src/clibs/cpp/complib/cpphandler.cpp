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
 *     As a special exception, if other files instantiate templates or
 *     use macros or inline functions from this file, or you compile
 *     this file and link it with other works to produce a work based
 *     on this file, this file does not by itself cause the resulting
 *     work to be covered by the GNU General Public License. However
 *     the source code for this file must still be made available in
 *     accordance with section (3) of the GNU General Public License.
 *
 *     This exception does not invalidate any other reasons why a work
 *     based on this file might be covered by the GNU General Public
 *     License.
 *
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 *
 */

#include <windows.h>
#include <stdlib.h>
#include "_rtti.h"
#include <stdio.h>
#include <time.h>
#undef errno
#include <wchar.h>
#include <locale.h>
#include "libp.h"

extern "C" void __global_unwind(void*, void*);
extern "C" void __call_terminate();
extern "C" void __call_unexpected();

extern "C" LONG ___xceptionhandle(PEXCEPTION_RECORD p, void* record, PCONTEXT context, void* param);

#define CAUGHT 1
#define THROWN 2

int ___xcflags;

bool _RTL_FUNC uncaught_exception() { return ___xcflags == THROWN; }

// have to do the destroys in reverse order...
static void destroyone(XCTAB* record, XCEPT* blk, XCEPT* catchBlock)
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
                    RTTI* base = blk->xt;
                    while (base->flags & XD_ARRAY)
                    {
                        BYTE* elempos = (BYTE*)base->name;
                        while (*elempos)
                            elempos++;
                        elempos++;
                        elems *= *(int*)elempos;
                        base = base->base;
                    }
                    if (base->flags & XD_REF)
                        base = base->base;
                    if (!(base->flags & XD_POINTER) && base->destructor)
                    {
                        BYTE* thisptr = blk->ebpoffs + (BYTE*)record->ebp + (elems - 1) * base->size;
                        for (int i = 0; i < elems; i++)
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
static void destroy(XCTAB* record, XCEPT* catchBlock)
{
    XCEPTHEAD* head = record->xceptBlock;
    XCEPT* blk = (XCEPT*)(head + 1);
    destroyone(record, blk, catchBlock);
}
static size_t recurseMatchXT(RTTI* xt, RTTI* thrown, size_t offs)
{
    if (!(thrown->flags & XD_ARRAY))
    {
        RTTISTRUCT* rttis = (RTTISTRUCT*)((char*)thrown + (unsigned)&thrown->name - (unsigned)thrown + strlen(thrown->name) + 1);
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
static BOOL matchXT(RTTI* xt, RTTI* thrown, size_t* offs, BOOL recurse)
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
static BOOL canThrow(XCTAB* record, PEXCEPTION_RECORD p, PCONTEXT context)
{
    XCEPTHEAD* head = record->xceptBlock;
    THROWREC* throwRec = head->throwRecord;
    if (!throwRec)
        return TRUE;  // unspecified
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
        RTTI** list = &throwRec->thrownClasses;
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
static void instantiate(XCTAB* record, void* dest1, void* src1)
{
    if (record->cons)
    {
        BYTE* src = (BYTE*)src1;
        BYTE* dest = (BYTE*)dest1;
        for (int i = 0; i < record->elems; i++)
        {
            void (*xx)(void*, void*) = (void (*)(void*, void*))record->cons;
            (*xx)(dest, src);
            dest += record->thrownxt->size;
            src += record->thrownxt->size;
        }
    }
    else
    {
        memcpy(dest1, src1, record->elems * record->thrownxt->size);
    }
}
static BOOL matchBlock(XCTAB* record, PEXCEPTION_RECORD p, PCONTEXT context)
{
    XCEPT* candidate = NULL;
    XCEPTHEAD* head = record->xceptBlock;
    XCEPT* blk = (XCEPT*)(head + 1);
    THROWREC* throwRec = head->throwRecord;
    XCTAB* orig = (XCTAB*)p->ExceptionInformation[0];
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
        record->instance = (char*)record->throwninstance + offs;
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
extern "C" LONG __cppexceptionhandle(PEXCEPTION_RECORD p, void* record, PCONTEXT context, void* param)
{
    if (p->ExceptionFlags & 2)  // unwinding
    {
        destroy((XCTAB*)record, NULL);
        return 1;  // continue search
    }
    else if (p->ExceptionCode == OUR_CPP_EXC_CODE)
    {
        if (matchBlock((XCTAB*)record, p, context))
        {
            return 0;  // continue execution
        }
        else if (canThrow((XCTAB*)record, p, context))
        {
            return 1;  // continue search
        }
        XCEPTHEAD* head = ((XCTAB*)record)->xceptBlock;
        THROWREC* throwRec = head->throwRecord;
        // abort if can't throw
        if (throwRec->flags & XD_DYNAMICXC)
            __call_unexpected();
        else
            __call_terminate();
        return 0;
    }
    return ___xceptionhandle(p, record, context, param);
}
void _RTL_FUNC _ThrowException(void* irecord, void* instance, int arraySize, void* cons, void* xceptBlock_in)
{
    XCTAB* record = (XCTAB*)irecord;
    RTTI* xceptBlock = (RTTI*)xceptBlock_in;
    ULONG_PTR params[1];
    params[0] = (ULONG_PTR)record;
    if (record->flags & THROWN)  // in case of nested throws
        __call_terminate();
    ___xcflags = record->flags = THROWN;
    record->elems = arraySize;
    record->baseinstance = malloc(xceptBlock->size * arraySize);
    record->thrownxt = xceptBlock;
    record->cons = cons;
    if (!record->baseinstance)
        __call_terminate();

    instantiate(record, record->baseinstance, instance);
    RaiseException(OUR_CPP_EXC_CODE, EXCEPTION_CONTINUABLE, 1, (DWORD*)&params[0]);
}

void uninstantiate(XCTAB* record, void* instance)
{
    if (record->thrownxt->destructor && !(record->thrownxt->flags & (XD_POINTER | XD_ARRAY | XD_REF)))
    {
        BYTE* ptr = (BYTE*)instance + record->thrownxt->size * (record->elems - 1);
        for (int i = 0; i < record->elems; i++)
        {
            (*record->thrownxt->destructor)(ptr);
            ptr -= record->thrownxt->size;
        }
    }
    free(instance);
}
void _RTL_FUNC _RethrowException(void* r)
{
    XCTAB* record = (XCTAB*)r;
    ULONG_PTR params[1];
    __asm mov eax, [record];
    __asm mov [fs:0], eax;
    if (!(record->flags & CAUGHT))
        __call_terminate();
    uninstantiate(record, record->throwninstance);
    params[0] = (ULONG_PTR)record;
    RaiseException(OUR_CPP_EXC_CODE, EXCEPTION_CONTINUABLE, 1, (DWORD*)&params[0]);
}
void _RTL_FUNC _CatchCleanup(void* r)
{
    XCTAB* record = (XCTAB*)r;
    ULONG_PTR params[1];
    __asm mov eax, [record];
    __asm mov [fs:0], eax;
    if (!(record->flags & CAUGHT))
        __call_terminate();
    ___xcflags = record->flags = 0;
    uninstantiate(record, record->throwninstance);
    uninstantiate(record, record->baseinstance);
}
