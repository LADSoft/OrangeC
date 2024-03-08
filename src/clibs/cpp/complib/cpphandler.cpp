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

#include <windows.h>
#include <stdlib.h>
#include <exception>
#include <deque>
#include "_rtti.h"
#include <stdio.h>
#include <time.h>
#undef errno
#include <wchar.h>
#include <locale.h>
#include "libp.h"

extern "C" void __global_unwind(void*, void*);
extern "C" void __call_terminate();
extern "C" void _RTL_FUNC __call_unexpected(std::exception_ptr *e);

extern "C" LONG ___xceptionhandle(PEXCEPTION_RECORD p, void* record, PCONTEXT context, void* param);

std::nested_exception::nested_exception() noexcept : captured(std::current_exception()) 
{ 
}
std::nested_exception::~nested_exception() __NOTHROW
{
}
std::exception_ptr::~exception_ptr()
{
    decrement();
    if (exc && !exc->referenceCount)
    {
        auto temp = exc;
        exc = nullptr;
        FreeExceptionObject(temp);
    }
}

struct __thrownExceptions
{
    struct __thrownExceptions *next;
    struct __thrownExceptions *prev;
    std::exception_ptr exception;
    struct __thrownExceptions* Add(const std::exception_ptr& exc)
    {
        auto rv = (__thrownExceptions*) calloc(1, sizeof(__thrownExceptions));
        if (this)
        {
             rv->prev = this;
             next = rv;
        }
        new (&rv->exception) std::exception_ptr(exc);
        return rv;
    }
    struct __thrownExceptions* Remove()
    {
        if (this)
        {
            auto rv = this->prev;
            if (rv)
            	rv->next = nullptr;
            (&exception)->~exception_ptr();
            free(this);
            return rv;
        }
        return nullptr;
    }
};
struct __thrownExceptionList
{
    struct __thrownExceptions* first;
    struct __thrownExceptions* last; 
    int count;
    int size() { return count; }
    std::exception_ptr& back() { return last->exception; }
    void push_back(const std::exception_ptr&exc) { last = last->Add(exc); if (!count) { first = last; } count++; } 
    void pop_back() { if (count) { last = last->Remove(); count--; if (!count)first = nullptr; } }
};
thread_local __thrownExceptionList thrownExceptions;
thread_local XCTAB* currentFrame;

static void uninstantiate(std::__exceptionInternal* exc, bool base);

static inline std::__exceptionInternal* std::__getxc(const std::exception_ptr& p)
{
    return p.exc;
}
// called when the reference count on an exception_ptr reaches zero...
void std::exception_ptr::FreeExceptionObject(std::__exceptionInternal* exc)
{
     // the exception_ptr object will be deleted either on catch/rethrow
     // or when a user exception_ptr object is destroyed
     uninstantiate(exc, false);
     uninstantiate(exc, true);
     free(exc);
}
int  _RTL_FUNC std::uncaught_exceptions() noexcept
{
     int rv = 0;
     for (auto t = thrownExceptions.first; t; t = t->next)
         if (__getxc(t->exception)->inprocess)
             rv++;
     return rv;
}
bool _RTL_FUNC std::uncaught_exception() 
{
     return uncaught_exceptions() != 0;
}

std::exception_ptr _RTL_FUNC std::current_exception() noexcept
{
    if (thrownExceptions.size())
    {
       return thrownExceptions.back();
    }
    return nullptr;
}


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
                if (!catchBlock || (catchBlock->startOffs <= blk->startOffs && blk->endOffs < catchBlock->endOffs))
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
        auto xc = __getxc(thrownExceptions.back());
        while (*list)
        {
            size_t offs;
            if (matchXT(*list, xc->thrownxt, &offs, FALSE))
                return TRUE;
            list++;
        }
        return FALSE;
    }
}
static void instantiate(std:: __exceptionInternal* exc, BYTE* src, BYTE* dest)
{
    if (exc->cons)
    {
        for (int i = 0; i < exc->elems; i++)
        {
            void (*xx)(void*, void*) = (void (*)(void*, void*))exc->cons;
            (*xx)(dest, src);
            dest += exc->thrownxt->size;
            src += exc->thrownxt->size;
        }
    }
    else
    {
        memcpy(dest, src, exc->elems * exc->thrownxt->size);
    }
}
static BOOL matchBlock(XCTAB* record, PEXCEPTION_RECORD p, PCONTEXT context)
{
    XCEPT* candidate = NULL;
    XCEPTHEAD* head = record->xceptBlock;
    XCEPT* blk = (XCEPT*)(head + 1);
    THROWREC* throwRec = head->throwRecord;
    XCTAB* orig = (XCTAB*)p->ExceptionInformation[0];
    auto xc = __getxc(thrownExceptions.back());
    size_t offs = 0;
    while (blk->flags)
    {
        if (blk->flags & XD_CL_TRYBLOCK)
        {
            if (blk->startOffs <= record->funcIndex && record->funcIndex < blk->endOffs)
            {
                if (matchXT(blk->xt, xc->thrownxt, &offs, TRUE))
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
        __global_unwind(0, record);
        destroy(record, candidate);
        record->eip = candidate->trylabel;
        record->funcIndex = candidate->endOffs;
        record->throwninstance = xc->obj = malloc(xc->thrownxt->size * xc->elems);
        if (!xc->obj)
            __call_terminate();
        instantiate(xc, (BYTE*)xc->baseinstance, (BYTE*)xc->obj);
        xc->instance = (char*)xc->obj + offs;
        xc->inprocess = false;
        currentFrame = record;
        context->Eip = (DWORD)candidate->trylabel;
        context->Ebp = record->ebp;
        context->Esp = record->esp;
        return TRUE;
    }
    return FALSE;
}
__export extern "C" LONG __cppexceptionhandle(PEXCEPTION_RECORD p, void* record, PCONTEXT context, void* param)
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
        {
            // not using auto construction so that the exception block for this function
            // won't have a reference to this is a destructor
            char hold[sizeof(std::exception_ptr)];
            std::exception_ptr* e = new(hold) std::exception_ptr;
            __call_unexpected(e);
            if (canThrow((XCTAB*)record, p, context) || __getxc(*e)->thrownxt == typeid(std::bad_exception).tpp)
            {
                auto e1 = thrownExceptions.back();
                thrownExceptions.pop_back();
                std::__exceptionInternal* xc = __getxc(thrownExceptions.back());
                uninstantiate(xc,false);
                delete xc;
                thrownExceptions.pop_back();
                thrownExceptions.push_back(e1);
            }
        }
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
    std::__exceptionInternal* cls = (std::__exceptionInternal*)calloc(1, sizeof(std::__exceptionInternal));
    cls->elems = arraySize;
    cls->inprocess = true;
    cls->thrownxt = xceptBlock;
    cls->cons = cons;
    void *temp;
    asm mov eax,[gs:0]
    asm mov [temp],eax
    cls->thrd = temp;
    cls->baseinstance = malloc(cls->thrownxt->size * cls->elems);
    if (!cls->baseinstance)
        __call_terminate();
  
    instantiate(cls, (BYTE*)instance, (BYTE*)cls->baseinstance);
    // not using auto construction so that the exception block for this function
    // won't have a reference to this is a destructor
    char hold[sizeof(std::exception_ptr)];
    std::exception_ptr*p = new (hold) std::exception_ptr(cls);
    thrownExceptions.push_back(*p);
    p->~p();	
    RaiseException(OUR_CPP_EXC_CODE, EXCEPTION_CONTINUABLE, 1, (ULONG_PTR*)&params[0]);
}
static void uninstantiate(std::__exceptionInternal* exc, bool base)
{
    if (exc->obj && exc->thrownxt->destructor && !(exc->thrownxt->flags & (XD_POINTER | XD_ARRAY | XD_REF)))
    {
        BYTE* baseptr = (BYTE *)(base ? exc->baseinstance : exc->obj);
        if (baseptr)
        {
            BYTE* ptr = baseptr + exc->thrownxt->size * (exc->elems - 1);
            for (int i = 0; i < exc->elems; i++)
            {
                (exc->thrownxt->destructor)(ptr);
                ptr -= exc->thrownxt->size;
            }
            free(baseptr);
         }
    }
    exc->obj = nullptr;
}
void _RTL_FUNC _RethrowException(void* r)
{
    if (thrownExceptions.size() == 0)
        __call_terminate();
    XCTAB* record = (XCTAB*)r;
    ULONG_PTR params[1];
    __asm mov eax, [record];
    __asm mov [fs:0], eax;
    uninstantiate(__getxc(thrownExceptions.back()), false);
    params[0] = (ULONG_PTR)record;
    RaiseException(OUR_CPP_EXC_CODE, EXCEPTION_CONTINUABLE, 1, (ULONG_PTR*)&params[0]);
}
void _RTL_FUNC _CatchCleanup(void* r)
{
    if (thrownExceptions.size() == 0)
        __call_terminate();
    XCTAB* record = (XCTAB*)r;
    ULONG_PTR params[1];
    __asm mov eax, [record];
    __asm mov [fs:0], eax;
    thrownExceptions.pop_back();
}

[[noreturn]] void _RTL_FUNC std::rethrow_exception(std::exception_ptr arg)
{
    ULONG_PTR params[1];
    void *thrd;
    XCTAB* cur;
    asm mov eax,[gs:0]
    asm mov [thrd], eax
    if (thrd != __getxc(arg)->thrd || thrownExceptions.size() == 0)
    {
        asm mov eax,[fs:0]
        asm mov [cur], eax

        auto cls = __getxc(arg);
        params[0] = (ULONG_PTR)cur;
        thrownExceptions.push_back(arg);
    }
    else
    {
        auto e1 = thrownExceptions.back();
        thrownExceptions.pop_back();
        thrownExceptions.push_back(arg);
        XCTAB* record = currentFrame;
        __asm mov eax, [record];
        __asm mov [fs:0], eax;
        params[0] = (ULONG_PTR)currentFrame;
    }
    uninstantiate(__getxc(arg), false);
    RaiseException(OUR_CPP_EXC_CODE, EXCEPTION_CONTINUABLE, 1, (ULONG_PTR*)&params[0]);
}

std::__exceptionInternal* _RTL_FUNC std::__make_exception_ptr(void* instance, int arraySize, void* cons, void* xceptBlock_in)
{
    std::__exceptionInternal* cls = (std::__exceptionInternal*)calloc(1, sizeof(std::__exceptionInternal));
    cls->elems = arraySize;
    cls->inprocess = false;
    cls->thrownxt = (RTTI*) xceptBlock_in;
    cls->cons = cons;
    cls->baseinstance = malloc(cls->thrownxt->size * cls->elems);
    void *temp;
    asm mov eax,[gs:0]
    asm mov [temp],eax
    cls->thrd = temp;
    if (!cls)
        __call_terminate();

    instantiate(cls, (BYTE*)instance, (BYTE*)cls->baseinstance);
    return (void*) cls;
}

[[noreturn]] void std::nested_exception::rethrow_nested() const
{
    if (!captured || !thrownExceptions.size())
        __call_terminate();
    auto cls = __getxc(captured);
    uninstantiate(cls, false);
    thrownExceptions.pop_back();
    thrownExceptions.push_back(captured);
    ULONG_PTR params[1];
    params[0] = (ULONG_PTR)currentFrame;
    RaiseException(OUR_CPP_EXC_CODE, EXCEPTION_CONTINUABLE, 1, (ULONG_PTR*)&params[0]);
}
