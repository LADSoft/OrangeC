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
#include "DotNetPELib.h"

namespace DotNetPELib
{

    char *Type::typeNames[] = {
        "", "", "void", "int8", "uint8", "int16", "uint16", "int32", "uint32", "int64", "uint64", "int",
        "float32", "float64", "object", "object []", "string" 
    };    
    char *BoxedType::typeNames[] = { "", "", "", "Int8", "UInt8",
        "Int16", "UInt16", "Int32", "UInt32",
        "Int64", "UInt64", "Int", "Float", "Double"
    };
    bool Type::ILSrcDump(PELib &peLib)
    {
        if (fullName.size())
        {
            peLib.Out() << fullName;
        }
        else if (tp == cls)
        {
            peLib.Out() << "'" << Qualifiers::GetName("", typeRef, "\xf8") << "'";
        }
        else if (tp == method)
        {
            peLib.Out() << "method ";
            methodRef->ILSrcDump(peLib, false, true, true);
        }
        else
        {
            peLib.Out() << typeNames[tp];
        }
        for (int i=0; i < pointerLevel; i++)
            peLib.Out() << " *";
        return true;
    }
    bool BoxedType::ILSrcDump(PELib &peLib)
    {
        peLib.Out() << "[mscorlib]System." << typeNames[tp];
    }
}
