/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
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

#include "InputFile.h"
#include "Errors.h"
#include "PipeArbitrator.h"
#include "UTF8.h"

#ifdef HAVE_UNISTD_H
#    include <unistd.h>
#    include <fcntl.h>
#else
#    include <io.h>
extern "C" char* _getcwd(char*, int);
#endif

std::set<std::string> InputFile::fileNameCache;

InputFile::~InputFile()
{
    if (streamid >= 3)
        _close(streamid);
    CheckErrors();
}
bool InputFile::Open()
{
    if (piper.HasPipe())
    {
        streamid = piper.OpenFile(*name);
    }
    else if ((*name)[0] == '-')
    {
        streamid = 0;
    }
    else
        streamid = _open(name->c_str(), 0);  // readonly
    if (streamid >= 0)
        CheckUTF8BOM();
    return streamid >= 0;
}

void InputFile::CheckErrors()
{
    if (inComment)
        Errors::Warning("Non-terminated comment started in line " + Errors::ToNum(commentLine));
    if (endedWithoutEOL)
        Errors::TrivialWarning("File not terminated with End Of Line character");
}
bool InputFile::GetLine(std::string& line)
{
    char buf[LINE_WIDTH];
    while (inComment)
    {
        int m = ReadLine(buf);
        if (!m)
            return false;
        m = StripComment(buf);
        if (m)
        {
            buf[m] = 0;
            break;
        }
    }
    line = std::string(buf);
    return true;
}
std::string InputFile::GetErrorName(bool full, const std::string& name)
{
    return name;
#if 0
    if (full)
    {
        return name;
    }
    else
    {
        const char* p = name.c_str();
        const char* q = strrchr(p, '\\');
        if (!q)
        {
            q = strrchr(p, ':');
            if (q)
                q++;
            else
                q = p;
        }
        else
        {
            q++;
        }
        return std::string(q);
    }
#endif
}
bool InputFile::ReadString(char* s, int len)
{
    char* olds = s;
    if (streamid < 0)
    {
        *s = 0;
        inputLen = 0;
        return false;
    }
    while (true)
    {
        while (inputLen > 0)
        {
            int ch;
            if (ucs2BOM)
            {
                ch = *((unsigned short*)bufPtr);
                bufPtr += 2;
                inputLen -= 2;
            }
            else
            {
                ch = *bufPtr++;
                inputLen--;
            }
            if (ch == 0x1a)
            {
                *s = 0;
                inputLen = 0;
                return s != olds;
            }
            if (ch != '\r')
            {

                if (ch < 128 || !ucs2BOM)
                {
                    *s++ = ch;
                    len--;
                }
                else
                {
                    int l = UTF8::Encode(s, ch);
                    s += l;
                    len -= l;
                }
                if (ch == '\n' || len < 4)
                {
                    *s = 0;
                    return true;
                }
            }
        }
        inputLen = _read(streamid, inputBuffer, sizeof(inputBuffer));
        bufPtr = inputBuffer;
        if (inputLen <= 0)
        {
            *s = 0;
            inputLen = 0;
            return s != olds;
        }
    }
}
bool InputFile::ReadLine(char* line)
{
    line[0] = 0;
    lineno++;
    errlineno++;
    ReadString(line, LINE_WIDTH);
    int n = strlen(line);
    if (n)
    {
        if (line[n - 1] != '\n')
            endedWithoutEOL = true;
        else
            line[n - 1] = 0;
        return true;
    }
    return false;
}
void InputFile::CheckUTF8BOM()
{
    static unsigned char BOM[] = {0xef, 0xbb, 0xbf};
    static unsigned char BOM2[] = {0xff, 0xfe};  // only LE version at this time...
    unsigned char buf[4];
    int l;
    if (4 == (l = _read(streamid, buf, 4)))
    {
        utf8BOM = !memcmp(BOM, buf, 3);
        if (utf8BOM)
        {
            buf[0] = buf[3];
            l = 1;
        }
        else
        {
            ucs2BOM = !memcmp(BOM2, buf, 2);
            if (ucs2BOM)
            {
                buf[0] = buf[2];
                buf[1] = buf[3];
                l = 2;
            }
        }
    }
    if (l > 0)
    {
        memcpy(inputBuffer, buf, inputLen = l);
        bufPtr = inputBuffer;
    }
}
