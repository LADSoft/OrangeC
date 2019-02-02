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

#ifndef Lexer_h
#define Lexer_h

#include "Token.h"

class PreProcessor;

class Lexer
{
  public:
    enum
    {
        openpa = 1,
        closepa,
        plus,
        minus,
        lnot,
        bcompl,
        star,
        divide,
        mod,
        leftshift,
        rightshift,
        gt,
        lt,
        geq,
        leq,
        eq,
        ne,
        bor,
        bxor,
        band,
        land,
        lor,
        hook,
        colon,
        comma,
        openbr,
        closebr,
        // must be after punctuation
        ACCELERATORS,
        ALT,
        ASCII,
        AUTO3STATE,
        AUTOCHECKBOX,
        AUTORADIOBUTTON,
        BEGIN,
        TBITMAP,
        BLOCK,
        CAPTION,
        CHARACTERISTICS,
        CHECKBOX,
        CHECKED,
        CLASS,
        COMBOBOX,
        CONTROL,
        CTEXT,
        CURSOR,
        DEFPUSHBUTTON,
        DIALOG,
        DIALOGEX,
        DISCARDABLE,
        DLGINCLUDE,
        EDITTEXT,
        END,
        EXSTYLE,
        FILEDATE,
        FILEFLAGSMASK,
        FILEFLAGS,
        FILEOS,
        FILESUBTYPE,
        FILETYPE,
        FILEVERSION,
        FIXED,
        FONT,
        GRAYED,
        GROUPBOX,
        HELP,
        ICON,
        IMPURE,
        INACTIVE,
        ITALIC,
        LANGUAGE,
        LISTBOX,
        LOADONCALL,
        LTEXT,
        MENU,
        MENUEX,
        MENUBARBREAK,
        MENUBREAK,
        MENUITEM,
        MESSAGETABLE,
        MOVEABLE,
        NOINVERT,
        NONDISCARDABLE,
        POPUP,
        PRELOAD,
        PRODUCTVERSION,
        PURE,
        PUSHBUTTON,
        RADIOBUTTON,
        RCDATA,
        RCINCLUDE,
        RTEXT,
        SCROLLBAR,
        SEPARATOR,
        SHIFT,
        STATE3,
        STRINGTABLE,
        STYLE,
        VALUE,
        VERSION,
        VERSIONINFO,
        VIRTKEY,
        WEIGHT,
    };
    Lexer(PreProcessor& PP) : pp(PP), atEol(false), atEof(false), tokenizer(nullptr), token(nullptr)
    {
        InitTokenizer();
        NextToken();
    }
    ~Lexer() {}

    std::string GetRestOfLine();
    const Token* GetToken() { return token; }
    void NextToken();
    void Reset(const std::string& line)
    {
        tokenizer->Reset(line);
        NextToken();
    }
    bool AtEol() { return atEol; }
    bool AtEof() { return atEof; }

  protected:
    void InitTokenizer();

  private:
    PreProcessor& pp;
    bool atEol;
    bool atEof;
    Tokenizer* tokenizer;
    const Token* token;
    static KeywordHash hash;
};
#endif  // Lexer_h