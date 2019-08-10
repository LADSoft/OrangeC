/* Software License Agreement
 *
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
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
std::string InputFile::GetErrorName(bool full, std::string& name)
{
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
}
bool InputFile::ReadLine(char* line)
{
    line[0] = 0;
    lineno++;
    fgets(line, LINE_WIDTH, file);
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
    unsigned char buf[3];
    if (3 == fread(buf, 1, 3, file))
    {
        utf8BOM = !memcmp(BOM, buf, 3);
        if (utf8BOM)
            return;
    }
    fseek(file, 0, SEEK_SET);
}