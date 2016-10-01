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
    bool Operand::isnanorinf()
    {
        // little endian architectures only
        longlong check;
        *(double *)&check = floatValue;
        // infinity or nan, or denormal... (exponent all ones or all zeros)
        check >>= 32;
        check &= 0x7ff00000;
        return check == 0x7ff00000 || check == 0;
    }
    bool Operand::ILSrcDump(PELib &peLib)
    {
        switch (type)
        {
            case t_none: // no operand, nothing to display
                break;
            case t_value:
                refValue->ILSrcDump(peLib);
                break;
            case t_int:
                peLib.Out() << intValue;
                break;
            case t_real:
                if (isnanorinf())
                {
                    unsigned char buf[8];
                    int sz1, i;
                    if (sz == r4)
                    {
                        sz1 = 4;
                        *(float *)buf = floatValue;
                    } 
                    else
                    {
                        sz1 = 8;
                        *(double *)buf = floatValue;
                    }

                    peLib.Out() << "(" << std::hex ;
                    for (i=0; i < sz1; i++)
                    {
                        peLib.Out() << std::setw(2) << std::setfill('0') << (int)buf[i] << " ";
                    }
                    peLib.Out() << ")" << std::dec;
                }
                else
                {
                    peLib.Out() << floatValue;
                }
                break;
            case t_string:
                peLib.Out() << "\"" << stringValue << "\"";
                break;
            case t_label:
                peLib.Out() << stringValue;
                break;
        }
        return true;
    }
}