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
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 * 
 */

#ifndef TokenNode_h
#define TokenNode_h

#include <deque>
#include <string>

class TokenNode
{
public:
    TokenNode(std::string Str, int ol) : type(tk_string), optionLevel(ol), bytes(NULL), used(false), coding(-1), values(NULL), eos(0)
                                         { str = new std::string(Str); id = tn_next++; tokenList.push_back(this); 
                                            std::map<std::string, int>::iterator it = tokenTable.find(Str);
                                            if (it == tokenTable.end())
                                            {
                                                tokenTable[Str] = token = tk_next++;
                                            }
                                            else
                                            {
                                                token = it->second;
                                            }
                                        }
    TokenNode(Register *Reg, int ol) : type(tk_reg), reg(Reg), optionLevel(ol), bytes(NULL), used(false), coding(-1), values(NULL), eos(0)
                                        { id = tn_next++; tokenList.push_back(this); }
    TokenNode(RegClass *rClass, int ol) : type(tk_regclass), regClass(rClass), optionLevel(ol), bytes(NULL), used(false), coding(-1), values(NULL), eos(0)
                                        { id = tn_next++; tokenList.push_back(this); }
    TokenNode(AddressClass *AddrClass, int ol) : type(tk_addrclass), addrClass(AddrClass), optionLevel(ol), bytes(NULL), used(false), coding(-1), values(NULL), eos(0)
                                        { id = tn_next++; tokenList.push_back(this); }
    TokenNode(Opcode *OpCode, int ol) : type(tk_opcode), opcode(OpCode), optionLevel(ol), bytes(NULL), used(false), coding(-1), values(NULL), eos(0)
                                        { id = tn_next++; tokenList.push_back(this); }
    TokenNode(Number *nNumber, int ol) : type(tk_number), number(nNumber), optionLevel(ol), bytes(NULL), used(false), coding(-1), values(NULL), eos(0)
                                        { id = tn_next++; tokenList.push_back(this); }
    ~TokenNode() { if (bytes) delete bytes; }
    void SetBytes(BYTE *data, int n)
    {
        bytes = new BYTE[n];
        memcpy(bytes, data, n);
    }
    std::string name;
    std::deque<TokenNode *> branches;
    int optionLevel;
    BYTE *bytes;
    int coding;
    int eos;
    std::map<std::string, std::string> *values;
    enum tk_type { tk_string, tk_number, tk_reg, tk_regclass, tk_addrclass, tk_opcode } type;
    std::string *str;
    union
    {
        void *opaque;
        int token;
        Register *reg;
        RegClass *regClass;
        AddressClass *addrClass;
        Opcode *opcode;
        Number *number;
    } ;
    enum { 
        TOKEN_BASE = 0,
        REGISTER_BASE = 1000,
        OPCODE_BASE = 2000
    };
    int id;
    int tokenId;
    bool used;
    static int tn_next;
    static int tk_next;
    static std::deque<TokenNode *> tokenList;
    static std::map<std::string, int> tokenTable;
} ;
#endif