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
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    contact information:
        email: TouchStone222@runbox.com <David Lindauer>
*/
#ifndef OBJUTIL_H
#define OBJUTIL_H

#include <string>
#include "ObjTypes.h"
class ObjUtil
{
public:
    static ObjString ToHex(ObjInt value, int width = 0);
    static ObjString ToDecimal(ObjInt value, int width = 0);
    static ObjInt ObjUtil::FromHex(const char *data, int *pos=NULL, int width=0)
    {
        return FromInt(data, pos, width, 16);
    }
    static ObjInt ObjUtil::FromDecimal(const char *data, int *pos=NULL, int width=0)
    {
        return FromInt(data, pos, width, 10);
    }
    static ObjInt ObjUtil::FromBinary(const char *data, int *pos=NULL, int width=0)
    {
        return FromInt(data, pos, width, 2);
    }
    static ObjInt ObjUtil::FromOctal(const char *data, int *pos=NULL, int width=0)
    {
        return FromInt(data, pos, width, 8);
    }
private:
    static ObjInt ObjUtil::FromInt(const char *data, int *pos, int width, int radix);
};
#endif