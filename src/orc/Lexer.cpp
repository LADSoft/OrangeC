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
        hash["{"] = BEGIN;  // openbr;
        hash["}"] = END;    // closebr;
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
    tokenizer = new Tokenizer("", &hash);
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
