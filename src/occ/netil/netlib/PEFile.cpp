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
#include "PEFile.h"
namespace DotNetPELib 
{
void PEWriter::pool::Ensure(size_t newSize) {
    if (size + newSize > max)
    {
        if (size + newSize < max * 2)
            max = max * 2;
        else
            max = (max + newSize) * 2;
        base = (unsigned char *)realloc(base, max);
    }
}
size_t PEWriter::AddTableEntry(TableEntryBase *entry)
{
    int n = entry->Index();
    int rv = tables[n].size();
    tables[n].push_back(entry);
    return rv;
}
void PEWriter::AddMethod(PEMethod *method)
{
    if (method->flags & PEMethod::EntryPoint)
    {
        if (entryPoint)
            throw new PELibError(PELibError::MultipleEntryPoints);
        entryPoint = method->methodDef | (tMethodDef << 24);
    }
}
size_t PEWriter::HashString(std::string utf8)
{

    std::map<std::string, size_t>::iterator it = stringMap.find(utf8);
    if (it != stringMap.end())
        return it->second;
    strings.Ensure(utf8.size() + 1);
    size_t rv = strings.size;
    memcpy(strings.base + strings.size, utf8.c_str(), utf8.size() + 1);
    strings.size += utf8.size() + 1;
    stringMap[utf8] = rv;
    return rv;
}
size_t PEWriter::HashUS(std::wstring str)
{
    int flag = 0;
    us.Ensure(str.size() * 2 + 5);
    size_t rv = us.size;
    int blobLen = str.size() * 2 + 1;
    if (blobLen < 0x80)
    {
        us.base[us.size++] = blobLen;
    }
    else if (blobLen <= 0x3fff)
    {
        us.base[us.size++] = (blobLen >> 8) | 0x80;
        us.base[us.size++] = blobLen & 0x7f;
    }
    else
    {
        blobLen &= 0x1fffffff;
        us.base[us.size++] = (blobLen >> 24) | 0xc0;
        us.base[us.size++] = (blobLen >> 16);
        us.base[us.size++] = (blobLen >> 8);
        us.base[us.size++] = (blobLen >> 0);
    }
    for (int i=0; i < str.size(); i++)
    {
        int n = str[i];
        if (n & 0xff00)
            flag = 1;
        else if (n <= 8 || n >= 0x0e && n < 0x20 || n == 0x27 || n == 0x2d || n == 0x7f)
            flag = 1;
        us.base[us.size++] = n & 0xff;
        us.base[us.size++] = n >> 8;
    }
    us.base[us.size++] = flag;
    return rv;
}
size_t PEWriter::HashGUID(unsigned char *Guid)
{
    if (guid.size == 1)
        guid.size = 0;
    guid.Ensure(128/8);
    size_t rv = guid.size;
    memcpy(guid.base + rv , Guid, 128/8);
    guid.size += 128/8;
    return (rv / (128/8) + 1);

}
size_t PEWriter::HashBlob(unsigned char *blobData, size_t blobLen)
{
    int xcount = 0;
    blob.Ensure(blobLen + 4);
    size_t rv = blob.size;
    if (blobLen < 0x80)
    {
        blob.base[blob.size++] = blobLen;
    }
    else if (blobLen <= 0x3fff)
    {
        blob.base[blob.size++] = (blobLen >> 8) | 0x80;
        blob.base[blob.size++] = blobLen & 0x7f;
    }
    else
    {
        blobLen &= 0x1fffffff;
        blob.base[blob.size++] = (blobLen >> 24) | 0xc0;
        blob.base[blob.size++] = (blobLen >> 16);
        blob.base[blob.size++] = (blobLen >> 8);
        blob.base[blob.size++] = (blobLen >> 0);
    }
    memcpy(blob.base + blob.size, blobData, blobLen);
    blob.size += blobLen;
    return rv;
}
size_t PEWriter::RVABytes(unsigned char *bytes, size_t dataLen)
{
    if (rva.size == 1)
        rva.size = 0;
    // align
    int pos = rva.size;
    if (pos % dataLen)
        pos += (dataLen - pos % dataLen);
    rva.Ensure(dataLen + pos - rva.size);
    size_t rv = rva.size;
    rva.size += pos - rva.size + dataLen;
    memcpy(rva.base + pos, bytes, dataLen);
    return rv;
}
}