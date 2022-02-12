/* Software License Agreement
 *
 *     Copyright(C) 1994-2022 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 *
 */

#include "ResFile.h"
#include "Resource.h"
#include "Font.h"
#include "Icon.h"
#include "Cursor.h"
#include "StringTable.h"
#ifdef WIN32
#    include <windows.h>
#endif
#include <fstream>

ResFile::~ResFile() {}
void ResFile::Mark()
{
    Align();
    base = stream.tellp();
}
void ResFile::MarkHeader() { hdrSize = (size_t)stream.tellp() - base; }
void ResFile::Release()
{
    size_t pos = stream.tellp();
    stream.seekp(base);
    WriteDWord(pos - base - hdrSize);
    WriteDWord(hdrSize);
    stream.seekp(pos);
}
void ResFile::Align()
{
    size_t pos = stream.tellp();
    if (pos % 4)
    {
        int size = 4 - pos % 4;
        int n = 0;
        stream.write((char*)&n, size);
    }
}
void ResFile::WriteByte(int byte)
{
    char c = (char)byte;
    stream.write(&c, 1);
}

void ResFile::WriteWord(int word)
{
    WriteByte(word & 0xff);
    WriteByte(word >> 8);
}

void ResFile::WriteDWord(int dword)
{
    WriteByte(dword & 0xff);
    WriteByte(dword >> 8);
    WriteByte(dword >> 16);
    WriteByte(dword >> 24);
}
void ResFile::WriteData(const unsigned char* data, int len) { stream.write((char*)data, len); }
void ResFile::WriteString(const std::wstring& str)
{
    int n = str.size();
#ifdef WIN32
    // The string actually isn't unicode, it is
    // code-page based.
    // the MENU font doesn't support code pages very well...
    // Convert to unicode.
    {
        int i;
        char buf[256];
        wchar_t wbuf[256], *p = wbuf;
        for (i = 0; i < n; i++)
            buf[i] = str[i];
        buf[i] = 0;

        wbuf[MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, buf, strlen(buf), wbuf, 256)] = 0;
        while (*p)
            WriteWord(*p++);
    }
#else
    // writing word by word for portability
    for (int i = 0; i < n; i++)
        WriteWord(str[i]);
#endif
    WriteWord(0);
}

bool ResFile::Write(const std::string& name)
{
    stream.open(name, std::ios::out | std::ios::binary);
    if (stream.is_open())
    {
        for (auto& res : resources)
        {
            res->WriteRes(*this);
        }
        for (auto& res : strings)
        {
            res->WriteRes(*this);
        }
        // the alignment rules for end of file seem strange
        // we are just going to align on a dword boundary like gnu rc does
        Align();
        stream.close();
        bool rv = !stream.fail();
        return rv;
    }
    return false;
}

void ResFile::Reset()
{
    Icon::Reset();
    Cursor::Reset();
    StringTable::Reset();
}
void ResFile::Add(Resource* th)
{
    std::unique_ptr<Resource> temp(th);
    if (th->GetType() == Resource::eString)
        strings.insert(std::move(temp));
    else
        resources.push_back(std::move(temp));
}
