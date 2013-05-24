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