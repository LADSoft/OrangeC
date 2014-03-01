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
#include "RCData.h"
#include "RCFile.h"
#include "ResFile.h"
#include "ResourceData.h"
#include <windows.h>
RCData::~RCData()
{
    for (iterator it = begin(); it != end(); ++it)
    {
        ResourceData *r = (*it);
        delete r;
    }
    data.clear();
}
void RCData::WriteRes(ResFile &resFile)
{
    Resource::WriteRes(resFile);
    for (iterator it = begin(); it != end(); ++it)
    {
        ResourceData *r = (*it);
        r->WriteRes(resFile);
    }
    resFile.Release();
}
void RCData::ReadRC(RCFile &rcFile)
{
    resInfo.SetFlags((resInfo.GetFlags() & ~ResourceInfo::Discardable) | ResourceInfo::Pure);
    resInfo.ReadRC(rcFile, false);

    const Token * t = rcFile.GetToken();
    if (t->GetKeyword() != Lexer::BEGIN)
    {
        ResourceData *rd = new ResourceData;
        rd->ReadRC(rcFile);
        data.push_back(rd);
    }
    else
    {
        rcFile.NeedBegin();
        bool done = false;
        bool concatwide = false;
        while (!done)
        {
            t = rcFile.GetToken();
            if (t->IsNumeric())
            {
                int n = t->GetInteger();				
                if (t->GetNumericType() == Token::t_int || t->GetNumericType() == Token::t_unsignedint)
                {
                    ResourceData *rd = new ResourceData((unsigned char *)&n, 2);
                    data.push_back(rd);
                }
                else
                {
                    ResourceData *rd = new ResourceData((unsigned char *)&n, 4);
                    data.push_back(rd);
                }
            }
            else if (t->IsString())
            {
                if (t->IsWide() || concatwide)
                {
                    // this is being done for portability
                    int n = t->GetString().size();
                    WORD *p = new WORD[n];
                    for (int i=0; i < n; i++)
                        p[i] = t->GetString()[i];
                    ResourceData *rd = new ResourceData((unsigned char *)p, n * sizeof(WORD));
                    data.push_back(rd);
                    delete[] p;
                    concatwide = true;
                }
                else
                {
                    std::string name = rcFile.CvtString(t->GetString());
                    ResourceData *rd = new ResourceData((unsigned char *)name.c_str(), t->GetString().size());
                    data.push_back(rd);
                }
            }
            else
                done = true;
            if (!done)
            {
                rcFile.NextToken();
                if (rcFile.IsKeyword())
                    if (rcFile.GetToken()->GetKeyword() == Lexer::comma)
                        concatwide = false;
                rcFile.SkipComma();
            }
        }
        rcFile.NeedEnd();
    }
}
