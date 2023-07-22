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

#pragma once

typedef unsigned LCHAR;

namespace Parser
{
	struct sym;
};
namespace Optimizer
{
typedef unsigned BITINT;

#define LLONG_FORMAT_SPECIFIER "%lld"

typedef unsigned long ADDRESS;

typedef unsigned char UBYTE;

typedef struct _list_
{
    struct _list_* next;
    void* data;
} LIST;

typedef struct _ilist_
{
    struct _ilist_* next;
    int data;
} ILIST;

typedef struct _linedata
{
    struct _linedata *stmtNext;
    const char* line;
    const char* file;
    int lineno;
    int fileindex;
} LINEDATA;

typedef struct _runtimeData
{
    struct Parser::sym *runtimeSymOrig;
    void *runtimeSym;
    const char *fileName;
    const char *varName;
    int lineno : 24;
    int asStore : 1;
} RUNTIMEDATA;

typedef struct
{
    LCHAR* str;
    int count;
} SLCHAR;

enum e_mo
{
    mo_relaxed,
    mo_consume,
    mo_acquire,
    mo_release,
    mo_acq_rel,
    mo_seq_cst
};

enum e_ao
{
    ao_init,
    ao_flag_set_test,
    ao_flag_clear,
    ao_thread_fence,
    ao_signal_fence,
    ao_load,
    ao_store,
    ao_fetch_modify,
    ao_modify_fetch,
    ao_cmpxchgweak,
    ao_cmpxchgstrong,
    ao_kill_dependency
};

#define ATOMIC_FLAG_SPACE Optimizer::sizeFromISZ(ISZ_UINT)
}  // namespace Optimizer