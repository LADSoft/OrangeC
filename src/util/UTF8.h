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

#ifndef UTF8_h
#define UTF8_h

#include <string>

class UTF8
{
  public:
    static bool IsAlpha(int);
    static bool IsAlnum(int);
    static bool IsAlpha(const char*);
    static bool IsAlnum(const char*);
    static int Decode(const char*);
    static int Encode(char*, int);
    static int CharSpan(const char*);
    static int Span(const char*);
    static int ToUpper(int val);
    static std::string ToUpper(const std::string& val);

  private:
    static int alpha[];
    static int alnum[];
    static int upper[][2];
};
#endif