/* Software License Agreement
 * 
 *     Copyright(C) 1994-2024 David Lindauer, (LADSoft)
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
 * 
 */

#pragma once
#define BRS_DEFINE 0
#define BRS_VARIABLE 1
#define BRS_STARTFILE 2
#define BRS_STARTFUNC 3
#define BRS_ENDFUNC 4
#define BRS_BLOCKSTART 5
#define BRS_BLOCKEND 6
#define BRS_PROTOTYPE 7
#define BRS_USAGE 8

#define BRF_EXTERNAL 2
#define BRF_STATIC 1
#define BRF_TYPE 5

namespace Optimizer
{
typedef struct browseFile
{
    struct browseFile* next;
    const char* name;
    int filenum;
} BROWSEFILE;
typedef struct browseInfo
{
    struct browseInfo* next;
    int type;
    const char* name;
    int filenum;
    int lineno;
    int charpos;
    int flags;
} BROWSEINFO;
}  // namespace Optimizer