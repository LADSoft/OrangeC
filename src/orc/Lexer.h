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
#ifndef Lexer_h
#define Lexer_h

#include "Token.h"


class PreProcessor;

class Lexer
{
public:
    enum
    {
        openpa=1, closepa, plus, minus, not, compl, star, divide, mod,
        leftshift, rightshift, gt, lt, geq, leq, eq, ne,
        or, xor, and, land, lor, hook, colon, comma,
        openbr, closebr,
        // must be after punctuation
        ACCELERATORS, ALT, ASCII, AUTO3STATE, 
        AUTOCHECKBOX, AUTORADIOBUTTON, BEGIN, TBITMAP, 
        BLOCK, CAPTION, CHARACTERISTICS, CHECKBOX, 
        CHECKED, CLASS, COMBOBOX, CONTROL, 
        CTEXT, CURSOR, DEFPUSHBUTTON, DIALOG, 
        DIALOGEX, DISCARDABLE, DLGINCLUDE, EDITTEXT, 
        END, EXSTYLE, FILEDATE, FILEFLAGSMASK, 
        FILEFLAGS, FILEOS, FILESUBTYPE, FILETYPE, 
        FILEVERSION, FIXED, FONT, GRAYED, 
        GROUPBOX, HELP, ICON, IMPURE, 
        INACTIVE, ITALIC, LANGUAGE, LISTBOX, 
        LOADONCALL, LTEXT, MENU, MENUEX, 
        MENUBARBREAK, MENUBREAK, MENUITEM, MESSAGETABLE, 
        MOVEABLE, NOINVERT , NONDISCARDABLE, 
        POPUP, PRELOAD, PRODUCTVERSION, PURE, 
        PUSHBUTTON, RADIOBUTTON, RCDATA, RCINCLUDE, 
        RTEXT, SCROLLBAR, SEPARATOR, SHIFT, 
        STATE3, STRINGTABLE, STYLE, VALUE, 
        VERSION, VERSIONINFO, VIRTKEY, WEIGHT, 
    };
    Lexer(PreProcessor &PP) : pp(PP), atEol(false), atEof(false), tokenizer(NULL), token(NULL) 
        { InitHash(); InitTokenizer(); NextToken(); }
    ~Lexer() { }

    std::string GetRestOfLine();
    const Token *GetToken() { return token; }
    void NextToken();
    void Reset(const std::string &line) { tokenizer->Reset(line); NextToken(); }
    bool AtEol() { return atEol; }
    bool AtEof() { return atEof; }
protected:
    void InitHash();
    void InitTokenizer();
    
private:
    PreProcessor &pp;
    bool atEol;
    bool atEof;
    Tokenizer *tokenizer;
    const Token *token;
    static KeywordHash hash;
    static bool hashInitted;
} ;
#endif Lexer_h