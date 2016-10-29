/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
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
#ifndef ppExpr_h
#define ppExpr_h

#include <string>

#include "token.h"
class ppDefine;

typedef L_INT PPINT;

enum kw
{
    openpa, closepa, plus, minus, lnot, bcompl, star, divide, mod,
    leftshift, rightshift, gt, lt, geq, leq, eq, ne,
    bor, bxor, band, land, lor, hook, colon, comma
};
class ppExpr
{
public:
    ppExpr(bool isunsignedchar) : define(NULL),
        unsignedchar(isunsignedchar) { InitHash(); }
    ~ppExpr() { }

	void SetParams(ppDefine *Define)
	{
		define = Define;
	}
    PPINT Eval(std::string &line);
    std::string GetString() { return tokenizer->GetString(); }
    void SetDefine (ppDefine *Define) { define = Define; }
    static KeywordHash *GetHash() { return &hash; }
protected:
    PPINT primary(std::string &line);
    PPINT unary(std::string &line);
    PPINT multiply(std::string &line);
    PPINT add(std::string &line);
    PPINT shift(std::string &line);
    PPINT relation(std::string &line);	
    PPINT equal(std::string &line);
    PPINT and_(std::string &line);
    PPINT xor_(std::string &line);
    PPINT or_(std::string &line);
    PPINT logicaland(std::string &line);
    PPINT logicalor(std::string &line);
    PPINT conditional(std::string &line);
    PPINT comma_(std::string &line);

private:
    static void InitHash();
    bool unsignedchar;
    ppDefine *define;
    Tokenizer *tokenizer;
    const Token *token;
    static KeywordHash hash;
    static bool initted;
} ;
#endif