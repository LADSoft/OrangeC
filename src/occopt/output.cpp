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

#include <cstdio>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <cstdarg>

namespace Optimizer
{
FILE* outputFile;
FILE* browseFile;

void oinit(void) {}

/*-------------------------------------------------------------------------*/

void oflush(FILE* file)
{
    fflush(file);
    oinit();
}

/*-------------------------------------------------------------------------*/

void oputc(int ch, FILE* file)
{

    if (file)
        fputc(ch, file);
}

void beputc(int ch) { fputc(ch, outputFile); }

/*-------------------------------------------------------------------------*/
void owrite(const char* buf, size_t size, int n, FILE* fil)
{
    if (fil)
        fwrite(buf, size, n, fil);
}
void beWrite(const char* buf, size_t size) { fwrite(buf, 1, size, outputFile); }
/*-------------------------------------------------------------------------*/
void oprintf(FILE* file, const char* format, ...)
{
    if (file)
    {
        va_list arg;
        va_start(arg, format);
        vfprintf(file, format, arg);
        va_end(arg);
    }
}
void bePrintf(const char* format, ...)
{
    va_list arg;
    va_start(arg, format);
    vfprintf(outputFile, format, arg);
    va_end(arg);
}
void beRewind(void) { fseek(outputFile, 0, SEEK_SET); }
}  // namespace Optimizer