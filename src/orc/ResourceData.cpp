/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
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
#include "ResourceData.h"
#include "RCFile.h"
#include "ResFile.h"

#include <stdexcept>
void ResourceData::WriteRes(ResFile &resFile) 
{ 
    if (data) 
        resFile.WriteData(data, len); 
}
void ResourceData::ReadRC(RCFile &rcFile)
{
    if (rcFile.IsKeyword())
    {
        int maxLen = 1024;
        data = new unsigned char [maxLen];
        rcFile.NeedBegin();
        while (rcFile.IsNumber())
        {
            if (len == maxLen)
            {
                maxLen += 1024;
                unsigned char *hold = data;
                data = new unsigned char [maxLen];
                memcpy(data, hold, maxLen - 1024);
                delete [] hold;
            }
            data[len++] = rcFile.GetNumber() & 0xff;
            rcFile.SkipComma();
        }
        rcFile.NeedEnd();
    }
    else
    {
        std::string name = rcFile.GetFileName();
        std::fstream in(name.c_str(), std::ios::in | std::ios::binary);
        if (!in.fail())
        {
            in.seekg(0, std::ios::end);
            len = in.tellg();
            in.seekg(0, std::ios::beg);
            data = new unsigned char[len];
            in.read((char *)data, len);
        }
        else
            throw new std::runtime_error(std::string("Could not open file '") + name + "'");
    }
}
