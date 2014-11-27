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
#include "RCFile.h"
#include "PreProcessor.h"

#include <stdexcept>

KeywordHash Lexer::hash;	
bool Lexer::hashInitted;

void Lexer::InitHash()
{
    if (!hashInitted)
    {
        hash["("] = openpa;
        hash[")"] = closepa;
        hash["+"] = plus;
        hash["-"] = minus;
        hash["!"] = lnot;
        hash["~"] = bcompl;
        hash["*"] = star;
        hash["/"] = divide;
        hash["%"] = mod;
        hash["<<"] = leftshift;
        hash[">>"] = rightshift;
        hash[">"] = gt;
        hash["<"] = lt;
        hash[">="] = geq;
        hash["<="] = leq;
        hash["=="] = eq;
        hash["!="] = ne;
        hash["|"] = bor;
        hash["&"] = band;
        hash["^"] = bxor;
        hash["||"] = lor;
        hash["&&"] = land;
        hash["?"] = hook;
        hash[":"] = colon;
        hash[","] = comma;
        hash["{"] = BEGIN; //openbr;
        hash["}"] = END; // closebr;
        hash["ACCELERATORS"] = ACCELERATORS;
        hash["ALT"] = ALT;
        hash["ASCII"] = ASCII;
        hash["AUTO3STATE"] = AUTO3STATE;
        hash["AUTOCHECKBOX"] = AUTOCHECKBOX;
        hash["AUTORADIOBUTTON"] = AUTORADIOBUTTON;
        hash["BEGIN"] = BEGIN;
        hash["BITMAP"] = TBITMAP;
        hash["BLOCK"] = BLOCK;
        hash["CAPTION"] = CAPTION;
        hash["CHARACTERISTICS"] = CHARACTERISTICS;
        hash["CHECKBOX"] = CHECKBOX;
        hash["CHECKED"] = CHECKED;
        hash["CLASS"] = CLASS;
        hash["COMBOBOX"] = COMBOBOX;
        hash["CONTROL"] = CONTROL;
        hash["CTEXT"] = CTEXT;
        hash["CURSOR"] = CURSOR;
        hash["DEFPUSHBUTTON"] = DEFPUSHBUTTON;
        hash["DIALOG"] = DIALOG;
        hash["DIALOGEX"] = DIALOGEX;
        hash["DISCARDABLE"] = DISCARDABLE;
        hash["DLGINCLUDE"] = DLGINCLUDE;
        hash["EDITTEXT"] = EDITTEXT;
        hash["END"] = END;
        hash["EXSTYLE"] = EXSTYLE;
        hash["FILEDATE"] = FILEDATE;
        hash["FILEFLAGSMASK"] = FILEFLAGSMASK;
        hash["FILEFLAGS"] = FILEFLAGS;
        hash["FILEOS"] = FILEOS;
        hash["FILESUBTYPE"] = FILESUBTYPE;
        hash["FILETYPE"] = FILETYPE;
        hash["FILEVERSION"] = FILEVERSION;
        hash["FIXED"] = FIXED;
        hash["FONT"] = FONT;
        hash["GRAYED"] = GRAYED;
        hash["GROUPBOX"] = GROUPBOX;
        hash["HELP"] = HELP;
        hash["ICON"] = ICON;
        hash["IMPURE"] = IMPURE;
        hash["INACTIVE"] = INACTIVE;
        hash["ITALIC"] = ITALIC;
        hash["LANGUAGE"] = LANGUAGE;
        hash["LISTBOX"] = LISTBOX;
        hash["LOADONCALL"] = LOADONCALL;
        hash["LTEXT"] = LTEXT;
        hash["MENU"] = MENU;
        hash["MENUEX"] = MENUEX;
        hash["MENUBARBREAK"] = MENUBARBREAK;
        hash["MENUBREAK"] = MENUBREAK;
        hash["MENUITEM"] = MENUITEM;
        hash["MESSAGETABLE"] = MESSAGETABLE;
        hash["MOVEABLE"] = MOVEABLE;
        hash["NOINVERT"] = NOINVERT;
        hash["NONDISCARDABLE"] = NONDISCARDABLE;
        hash["NOT"] = bcompl;
        hash["POPUP"] = POPUP;
        hash["PRELOAD"] = PRELOAD;
        hash["PRODUCTVERSION"] = PRODUCTVERSION;
        hash["PURE"] = PURE;
        hash["PUSHBUTTON"] = PUSHBUTTON;
        hash["RADIOBUTTON"] = RADIOBUTTON;
        hash["RCDATA"] = RCDATA;
        hash["RCINCLUDE"] = RCINCLUDE;
        hash["RTEXT"] = RTEXT;
        hash["SCROLLBAR"] = SCROLLBAR;
        hash["SEPARATOR"] = SEPARATOR;
        hash["SHIFT"] = SHIFT;
        hash["STATE3"] = STATE3;
        hash["STRINGTABLE"] = STRINGTABLE;
        hash["STYLE"] = STYLE;
        hash["VALUE"] = VALUE;
        hash["VERSION"] = VERSION;
        hash["VERSIONINFO"] = VERSIONINFO;
        hash["VIRTKEY"] = VIRTKEY;
        hash["WEIGHT"] = WEIGHT;
        hashInitted = true;
    }
}
void Lexer::InitTokenizer()
{
    tokenizer = new Tokenizer("",  &hash);
    tokenizer->SetCaseInsensitive(true);
}
std::string Lexer::GetRestOfLine()
{
    std::string rv;
    if (token)
        rv = token->GetChars();
    rv += tokenizer->GetString();
    tokenizer->Reset("");
    return rv;
}
void Lexer::NextToken()
{
    atEol = false;
    if (!token || !atEof)
    {
        if (tokenizer)
        {
            bool done = false;
            while (!done)
            {
                token = tokenizer->Next();
                if (token->IsEnd())
                {
                    atEol = true;
                    std::string line;
                    if (!pp.GetLine(line))
                    {
                        atEof = true;
                        done = true;
                    }
                    else
                    {
                        tokenizer->Reset(line);
                    }
                }
                else
                {
                    done = true;
                }
            }
        }
    }
}
