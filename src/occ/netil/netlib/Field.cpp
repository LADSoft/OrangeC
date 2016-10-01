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
#include <iomanip>
namespace DotNetPELib
{
    void Field::AddEnumValue(longlong Value, ValueSize Size)
    {
        if (mode == None)
        {
            mode = Enum;
            enumValue = Value;
            size = Size;
        }
    }
    void Field::AddInitializer(unsigned char *bytes, int len)
    {
        if (mode == None)
        {
            mode = Bytes;
            byteValue = bytes;
            byteLength = len;
        }
    }
    bool Field::ILSrcDumpTypeName(PELib &peLib, Field::ValueSize size)
    {
        switch (size)
        {
            case Field::i8:
                peLib.Out() << " int8";
                break;
            case Field::i16:
                peLib.Out() << " int16";
                break;
            case Field::i32:
            default:
                peLib.Out() << " int32";
                break;
            case Field::i64:
                peLib.Out() << " int64";
                break;
        }
        return true;
    }
    bool Field::ILSrcDump(PELib &peLib)
    {
        peLib.Out() << ".field";
        flags.ILSrcDumpBeforeFlags(peLib);
        flags.ILSrcDumpAfterFlags(peLib);
        peLib.Out() << " ";
        type->ILSrcDump(peLib);
        peLib.Out() << " '" << name << "'";
        switch (mode)
        {
            case None:
                break;
            case Enum:
                peLib.Out() << " = ";
                ILSrcDumpTypeName(peLib, size);
                peLib.Out() << "(" << (int)enumValue << ")";

                break;
            case Bytes:
                if (byteValue && byteLength)
                {
                    peLib.Out() << " at $" << name << std::endl;
                    peLib.Out() << ".data $" << name << " = bytearray (" << std::endl << std::hex ;
                    int i;
                    for (i=0; i < byteLength; i++)
                    {
                        peLib.Out()  << std::setw(2) << std::setfill('0') << (int)byteValue[i] << " ";
                        if (i %8 == 7 && i != byteLength-1) 
                            peLib.Out() << std::endl << "\t";
                    }
                    peLib.Out() << ")" << std::dec;
                }
                break;
        }
        peLib.Out() << std::endl;
        return true;
    }
}