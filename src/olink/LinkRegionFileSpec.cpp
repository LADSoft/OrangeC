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
#include "LinkRegionFileSpec.h"
#include <cstring>
#include <vector>
#include <fstream>
LinkRegionFileSpecContainer::LinkRegionFileSpecContainer(const ObjString &Spec)
{
    const char *data = Spec.c_str();
    int start=0, end;
    while (data[start])
    {
        if (data[start] == '*')
            specs.push_back(new LinkRegionFileSpec(LinkRegionFileSpec::eStar)), start++;
        else if (data[start] == '?')
            specs.push_back(new LinkRegionFileSpec(LinkRegionFileSpec::eQuestionMark)), start++;
        else {
            end = start;
            while (data[end] && data[end] != '*' && data[end] != '?')
            {
                end++;
            }
            specs.push_back(new LinkRegionFileSpec(LinkRegionFileSpec::eSpan, Spec.substr(start, end)));
            start = end;
        }		
    }
}
LinkRegionFileSpecContainer::~LinkRegionFileSpecContainer()
{
    for (auto spec : specs)
        delete spec;
}
bool LinkRegionFileSpecContainer::Matches(const ObjString &Spec)
{
    ObjString working = Spec;
    for (auto spec : specs)
    for (auto it = specs.begin(); it != specs.end(); ++it)
    {
        switch ((*it)->GetType())
        {
            case LinkRegionFileSpec::eStar:
            {
                ++it;
                if (it != specs.end())
                {
                    if ((*it)->GetType() != LinkRegionFileSpec::eSpan)
                        return false;
                    unsigned pos = working.find((*it)->GetSpan());
                    if (pos == ObjString::npos)
                        return false;
                    working = working.substr(pos + (*it)->GetSpan().size());
                }
                else
                    return true;
                break;
            }
            case LinkRegionFileSpec::eQuestionMark:
                if (working.size() < 1)
                    return false;
                working = working.substr(1);
                break;
            default:
            {
                int pos = working.find((*it)->GetSpan());
                if (pos != 0)
                    return false;
                working = working.substr((*it)->GetSpan().size());
                break;
            }
        }
    }
    return true;
}
