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
    char *Qualifiers::qualifierNames[] =
    { 
        "nested", "public", "private", "static", "explicit", "ansi", "sealed", "enum",
        "value", "sequential", "auto", "literal", "valuetype", "hidebysig", "preservesig", "specialname",
        "rtspecialname", "cil", "managed"
    };
    int Qualifiers::afterFlags = Qualifiers::PreserveSig | Qualifiers::CIL | Qualifiers::Managed;
    bool Qualifiers::ILSrcDumpBeforeFlags(PELib &peLib)
    {
        int n = ~afterFlags & flags;
        for (int i=0; i < 32; i++)
            if (n & ( 1 << i))
                peLib.Out() << " " << qualifierNames[i];
    }
    bool Qualifiers::ILSrcDumpAfterFlags(PELib &peLib)
    {
        int n = afterFlags & flags;
        for (int i=0; i < 32; i++)
            if (n & ( 1 << i))
                peLib.Out() << " " << qualifierNames[i];
    }
    void Qualifiers::ReverseNamePrefix(std::string &rv, DataContainer *parent, int &pos, bool type)
    {
        if (parent)
        {
            ReverseNamePrefix(rv, parent->GetParent(), pos, type);
            if (pos != 0)
            {
                rv += parent->GetName();
                if (pos == 2)
                    rv += type ? "\xf8" : "/";
                else
                    rv += '.';
            }
            pos++;
        }

    }
    std::string Qualifiers::GetNamePrefix(DataContainer *parent, bool type)
    {
        std::string rv;
        if (parent)
        {
            int pos = 0;
            ReverseNamePrefix(rv, parent, pos, type);
        }
        return rv;
    }
    std::string Qualifiers::GetName(std::string root, DataContainer *parent, bool type)
    {
        std::string rv = GetNamePrefix(parent, type);
        if (rv.size())
        {
            rv = rv.substr(0, rv.size()-1);
        }
        if (root.size())
        {
            if (rv.size())
                rv += "::";
            rv += "'" + root + "'";
        }
        return rv;
    }
}