/* Software License Agreement
 *
 *     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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
 *
 */

#pragma once
#include <array>
#include "Utils.h"
namespace Parser
{
    class TemplateHashContext
    {
    public:
        static const int DataSize = 16;
        typedef std::array<unsigned char, DataSize> DataType;
        bool Computed = false;
        unsigned str32 = 5381;
        unsigned sum32 = 0;
        unsigned len32 = 0;
        unsigned h32 = 0;
        // we need a 'value' for hashing but the entire body of these hashes can be very long
        // so we compress it to get a value.
        void Input(const unsigned char* data, int len)
        {
            len32 += len;
            for (int i = 0; i < len; i++, data++)
            {
                sum32 += *data;
                str32 = (str32 << 5) + str32 + *data;
            }
            h32 += len32 + str32 + sum32;
        }
        void Reset()
        {
            Computed = false;
        }
        void Result()
        {
            Computed = true;
        }
        void Result(DataType& result)
        {
            *(reinterpret_cast<unsigned*>(&result.front()) + 0) = h32;
            *(reinterpret_cast<unsigned*>(&result.front()) + 1) = len32;
            *(reinterpret_cast<unsigned*>(&result.front()) + 2) = sum32;
            *(reinterpret_cast<unsigned*>(&result.front()) + 3) = str32;
        }
    };
    void templateHashInit();
    SYMBOL* LookupTemplateClass(TemplateHashContext& context, SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* params);
    SYMBOL* LookupGeneratedTemplateClass(TemplateHashContext& context, SYMBOL* sym);
    void RegisterTemplateClass(TemplateHashContext& context, SYMBOL* cls);
    SYMBOL* LookupTemplateFunction(TemplateHashContext& context, SYMBOL* sym, std::list<SYMBOL*>* gather, CallSite* callSite);
    void RegisterTemplateFunction(TemplateHashContext& context, SYMBOL* func);
}  // namespace Parser