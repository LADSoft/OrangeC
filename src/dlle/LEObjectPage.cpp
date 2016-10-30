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
#include "LEObjectPage.h"
#include "LEObject.h"
#include "LEHeader.h"
#include <string.h>
unsigned LEObjectPage::CountPages()
{
    int n = 0;
    for (auto obj : objects)
    {
        n += ObjectAlign(4096, obj->GetInitSize())/4096;
    }
    return n;
}
unsigned LXObjectPage::CountPages()
{
    int n = 0;
    for (auto obj : objects)
    {
        n += ObjectAlign(4096, obj->GetSize())/4096;
    }
    return n;
}
void ObjectPage::Write(std::fstream &stream)
{
    stream.write((char *)data, size);
}
void LEObjectPage::Setup()
{
    pages = CountPages();
    size = pages *sizeof(PageData);
    data = new unsigned char[size];
    memset(data, 0, size);
    PageData *p = (PageData *)data;
    for (int i = 1; i <= pages; i++)
    {
        p->flags = LE_OPF_ENUMERATED ;
        p->high_offs = i/65536 ;
        p->med_offs = i/256 ;
        p->low_offs = i ;
        p++;
    }
}
void LXObjectPage::Setup()
{
    pages = CountPages();
    size = pages *sizeof(PageData);
    data = new unsigned char[size];
    
    PageData *p = (PageData *)data;
    int k = 1;
    for (auto obj : objects)
    {
        for (int size = 0; size < obj->GetSize(); size += 4096)
        {
            if (size >= obj->GetInitSize())
            {
                p->flags = LX_OPF_ZERO;
                p->data_offset = 0;
            }
            else
            {
                p->flags = LX_OPF_ENUMERATED;
                p->data_offset = k++;
            }
            p->data_size = 4096;
            p++;
        }
    }
}
