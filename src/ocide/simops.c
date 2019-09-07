/* Software License Agreement
 *
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
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
#include <commctrl.h>
#include <stdio.h>
#include <ctype.h>
#include <richedit.h>
#include "operands.h"
#include "header.h"
#include "utype.h"

BOOL useim;

void SegmentDisplay(char* buffer, long value) { sprintf(buffer, "0x%x", value); }

//-------------------------------------------------------------------------

void FormatValue(char* buffer, OPERAND* record, uint segs, uint type)
{
    char buf[4096];
    DEBUG_INFO* dbg;
    buf[0] = 0;
    if (record->address)
        segs++;
    FindGlobalSymbol(&dbg, record->address, buf, NULL);
    switch (type)
    {
        case SY_SIGNEDOFS:
            if (record->address < 0)
                sprintf(buffer, "-0x%x", (BYTE)ABS(record->address));
            else
                sprintf(buffer, "+0x%x", (BYTE)record->address);
            break;
        case SY_WORDOFS:
            sprintf(buffer, "+0x%lx", record->address);
            break;
        case SY_BYTEOFS:
            sprintf(buffer, "+0x%x", (BYTE)record->address);
            break;
        case SY_ABSOLUTE:
            if (buf[0])
                sprintf(buffer, "0x%lx:%s", record->address, buf);
            else
                sprintf(buffer, "0x%lx", record->address);
            break;
        case SY_SIGNEDIMM:
            if (record->address < 0)
                sprintf(buffer, "-0x%x", ABS(record->address));
            else
                sprintf(buffer, "+0x%x", record->address);
            break;
        case SY_WORDIMM:
            if (useim)
                sprintf(buffer, "0x%lx", record->address);
            else
                sprintf(buffer, "offset 0x%lx", record->address);
            break;
        case SY_BYTEIMM:
            sprintf(buffer, "0x%x", (BYTE)(record->address));
            break;
        case SY_PORT:
            sprintf(buffer, "0x%x", (BYTE)record->address);
            break;
        case SY_INTR:
            sprintf(buffer, "0x%x", (BYTE)record->address);
            break;
        case SY_RETURN:
            sprintf(buffer, "0x%x", (ushort)record->address);
            break;
        case SY_ABSBRANCH:
            if (buf[0])
                sprintf(buffer, "0x%lx:%s", record->address, buf);
            else
                sprintf(buffer, "0x%lx", record->address);
            break;
        case SY_LONGBRANCH:
            if (buf[0])
                sprintf(buffer, "0x%lx:%s", record->address, buf);
            else
                sprintf(buffer, "0x%lx", record->address);
            break;
        case SY_SHORTBRANCH:
            if (buf[0])
                sprintf(buffer, "0x%lx:%s", record->address, buf);
            else
                sprintf(buffer, "0x%lx", record->address);
            break;
        case SY_SHIFT:
            sprintf(buffer, "0x%x", (BYTE)record->address);
            break;
        case SY_SEGMENT:
            SegmentDisplay(buffer, record->segment);
            break;
    }
}

//-------------------------------------------------------------------------

void AddSymbol(OPERAND* record, uint type)
{
    record++;
    type++;
}
