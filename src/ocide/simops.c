/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2012, David Lindauer, (LADSoft).
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
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    contact information:
        email: TouchStone222@runbox.com <David Lindauer>
*/

#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <ctype.h>
#include "operands.h"
#include "header.h"
#include "utype.h"

BOOL useim;

void SegmentDisplay(char *buffer, long value)
{
    sprintf(buffer, "0x0x%x", value);
}

//-------------------------------------------------------------------------

void FormatValue(char *buffer, OPERAND *record, uint segs, uint type)
{
    char buf[256];
    DEBUG_INFO *dbg;
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
            sprintf(buffer, "0x%x", (uint)record->address);
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

void AddSymbol(OPERAND *record, uint type)
{
    record++;
    type++;
}
