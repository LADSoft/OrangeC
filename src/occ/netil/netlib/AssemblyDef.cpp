/*
    Software License Agreement (BSD License)

    Copyright (c) 2016, David Lindauer, (LADSoft).
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
#include "PEFile.h"
namespace DotNetPELib
{
    bool AssemblyDef::ILHeaderDump(PELib &peLib)
    {
        peLib.Out() << ".assembly ";
        if (external_)
            peLib.Out() << "extern ";
        peLib.Out() << name_ << "{" << std::endl;
        if (major_ || minor_ || build_ || revision_ )
            peLib.Out() << "\t.ver " << major_ << ":" << minor_ << ":" << build_ << ":" << revision_ << std::endl;
        for (int i = 0; i < 8; i++)
        {
            if (publicKeyToken_[i])
            {
                peLib.Out() << "\t.publickeytoken = (";
                for (int i = 0; i < 8; i++)
                {
                    peLib.Out() << std::hex << (int)(unsigned char)publicKeyToken_[i] << " ";
                }
                peLib.Out() << ")" << std::endl;
                break;
            }
        }
        peLib.Out() << "}" << std::endl;
        return true;

    }
    bool AssemblyDef::PEHeaderDump(PELib &peLib)
    {
        size_t nameIndex = peLib.PEOut().HashString(name_);
        TableEntryBase *table;
        if (external_)
        {
            size_t blobIndex = 0;
            for (int i = 0; i < 8; i++)
            {
                if (publicKeyToken_[i])
                {
                    blobIndex = peLib.PEOut().HashBlob(publicKeyToken_, 8);
                    break;
                }
            }
            table = new AssemblyRefTableEntry(PA_None, major_, minor_, build_, revision_, nameIndex, blobIndex);
        }
        else
        {
            table = new AssemblyDefTableEntry(PA_None, 0, 0, 0, 0, nameIndex);
        }
        peIndex_ = peLib.PEOut().AddTableEntry(table);
        return true;
    }
}