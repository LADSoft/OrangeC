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
#include "Bitmap.h"
#include "RCFile.h"
#include "ResourceData.h"
#include "ResFile.h"
#include <windows.h>

#include <stdexcept>

#define BITMAP_SIG 0x4d42 // BM
static const int SKIP = 14;

Bitmap::~Bitmap() 
{  
    delete data; 
}

void Bitmap::WriteRes(ResFile &resFile) 
{ 
    Resource::WriteRes(resFile); 
    if (data) 
        data->WriteRes(resFile); 
    resFile.Release(); 
}
void Bitmap::SetData(ResourceData *rdata) {

        delete data;
    data = rdata;
}
void Bitmap::ReadRC(RCFile &rcFile)
{
    resInfo.SetFlags( (resInfo.GetFlags() &~ResourceInfo::Discardable) | ResourceInfo::Pure);
    resInfo.ReadRC(rcFile, false);
    ResourceData *rd = new ResourceData;
    rd->ReadRC(rcFile);
    if (rd->GetLen() <= sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) )
    {
        delete rd;
        throw new std::runtime_error("invalid bitmap file");
    }
    else
    {
        LPBITMAPFILEHEADER f = (LPBITMAPFILEHEADER)(rd->GetData());
        LPBITMAPINFOHEADER p = (LPBITMAPINFOHEADER)(rd->GetData() + sizeof(BITMAPFILEHEADER));
        if (f->bfType != BITMAP_SIG
            || f->bfSize != rd->GetLen()
            || f->bfOffBits < sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)
            || f->bfOffBits >= rd->GetLen()
            || p->biSize != sizeof(BITMAPINFOHEADER))
        {
            delete rd;
            throw new std::runtime_error("invalid bitmap file");
        }
        // borland sets the size explicitly for RGB images that don't have a size
        // even though windows docs explicitly say it is not required.
        // and I figure there must be a good reason so I'm doing it too...
        if (p->biSizeImage == 0 && p->biCompression == BI_RGB)
        {
            int n = ((((p->biWidth * p->biBitCount) + 31) & ~31) >> 3) * p->biHeight;
            p->biSizeImage = n;
        }
        data = new ResourceData((unsigned char *)p, rd->GetLen() - sizeof(BITMAPFILEHEADER));
        delete rd;
    }
    rcFile.NeedEol();
}
