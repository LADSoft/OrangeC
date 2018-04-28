/* Software License Agreement
 * 
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the 
 *     Orange C "Target Code" exception.
 * 
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 */

#ifndef ResourceData_h
#define ResourceData_h

#include <stdlib.h>
#include <string>
#include <string.h>

class RCFile;
class ResFile;

class ResourceData
{
public:
    ResourceData() : len(0), data(nullptr), pos(0) { }
    ResourceData(const unsigned char *rdata, size_t len): len(0), data(nullptr), pos(0) 
    {
        SetData(rdata, len);
    }
    ~ResourceData() { delete data; }
    
    size_t GetLen() const { return len; }
    const unsigned char *GetData() const { return data; }
    void SetData(const unsigned char *rdata, size_t rlen)
    {
        delete data;
        len = rlen;
        data = new unsigned char[len];
        memcpy(data, rdata, len);
    }
    unsigned GetByte() { return data[pos++]; }
    unsigned GetWord() { int n = data[pos] + (data[pos +1] << 8); pos+=2; return n; }
    unsigned GetDWord() { int n = data[pos] + (data[pos +1] << 8) + (data[pos +2] << 24) + (data[pos +3] << 24); pos+=4; return n; }
    bool PastEnd() { return pos >= len; }
    void WriteRes(ResFile &resFile);
    void ReadRC(RCFile &rcFile);
    unsigned char operator [] (int val) { return data[val]; }		
private:
    int len;
    unsigned char *data;
    int pos;
} ;

#endif
