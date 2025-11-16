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

namespace Parser
{
template <class T>
class SymbolProperty
{
public:
    void set(SYMBOL* symbol, T val)
    {
        // check for builtins
        if (symbol->sb && symbol->sb->origdeclfile)
        {
            if (val)
            {
                properties[symbol] = val;
            }
            else
            {
                properties.erase(symbol);
            }
        }
    }
   T& get(SYMBOL* symbol)
    {
       // check for builtins..
       if (symbol->sb && symbol->sb->origdeclfile)
       {
           auto it = properties.find(symbol);
           if (it != properties.end())
               return it->second;
       }
        return null;
    }
    void clear()
    {
        properties.clear();
    }
private:
    struct hasher
    {

        size_t operator()(const SYMBOL* sym)  const
        {
            size_t hash = 0;
            hash = hash_string(hash, sym->name);
            hash = hash_string(hash, sym->sb->origdeclfile);
            hash = hash + (std::hash<int>()(sym->sb->origdeclline) << 1);
            return hash;
        }
    private:
        static size_t hash_string(unsigned orig, const char*str)
        {
            while(*str)
            {
                orig = (orig << 8) + (orig << 2) + (orig << 1) + (unsigned char)*str++;
            }
            return orig;
        }
    };
    struct eq
    {
        bool operator()(const SYMBOL* left, const SYMBOL* right)  const
        {
            if (left->sb->origdeclline != right->sb->origdeclline)
                return false;
            int n = strcmp(left->sb->origdeclfile, right->sb->origdeclfile);
            if (n == -0)
            {
                n = strcmp(left->name, right->name);
                if (n == 0 )
                    return true;
            }
            return false;
        }

    };
    T null = T();
    std::unordered_map<SYMBOL*, T, hasher, eq> properties;
};

inline SymbolProperty<LexemeStream*> bodyTokenStreams;
inline SymbolProperty<LexemeStream*> initTokenStreams;
inline SymbolProperty<LexemeStream*> noExceptTokenStreams;
inline SymbolProperty<SYMBOL*> functionDefinitions;

}  // namespace Parser