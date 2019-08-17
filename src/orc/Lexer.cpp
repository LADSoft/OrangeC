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

KeywordHash Lexer::hash = {
    {"(", openpa},
    {")", closepa},
    {"+", plus},
    {"-", minus},
    {"!", lnot},
    {"~", bcompl},
    {"*", star},
    {"/", divide},
    {"%", mod},
    {"<<", leftshift},
    {">>", rightshift},
    {">", gt},
    {"<", lt},
    {">=", geq},
    {"<=", leq},
    {"==", eq},
    {"!=", ne},
    {"|", bor},
    {"&", band},
    {"^", bxor},
    {"||", lor},
    {"&&", land},
    {"?", hook},
    {":", colon},
    {",", comma},
    {"{", BEGIN},  // openbr},
    {"}", END},    // closebr},
    {"ACCELERATORS", ACCELERATORS},
    {"ALT", ALT},
    {"ASCII", ASCII},
    {"AUTO3STATE", AUTO3STATE},
    {"AUTOCHECKBOX", AUTOCHECKBOX},
    {"AUTORADIOBUTTON", AUTORADIOBUTTON},
    {"BEGIN", BEGIN},
    {"BITMAP", TBITMAP},
    {"BLOCK", BLOCK},
    {"CAPTION", CAPTION},
    {"CHARACTERISTICS", CHARACTERISTICS},
    {"CHECKBOX", CHECKBOX},
    {"CHECKED", CHECKED},
    {"CLASS", CLASS},
    {"COMBOBOX", COMBOBOX},
    {"CONTROL", CONTROL},
    {"CTEXT", CTEXT},
    {"CURSOR", CURSOR},
    {"DEFPUSHBUTTON", DEFPUSHBUTTON},
    {"DIALOG", DIALOG},
    {"DIALOGEX", DIALOGEX},
    {"DISCARDABLE", DISCARDABLE},
    {"DLGINCLUDE", DLGINCLUDE},
    {"EDITTEXT", EDITTEXT},
    {"END", END},
    {"EXSTYLE", EXSTYLE},
    {"FILEDATE", FILEDATE},
    {"FILEFLAGSMASK", FILEFLAGSMASK},
    {"FILEFLAGS", FILEFLAGS},
    {"FILEOS", FILEOS},
    {"FILESUBTYPE", FILESUBTYPE},
    {"FILETYPE", FILETYPE},
    {"FILEVERSION", FILEVERSION},
    {"FIXED", FIXED},
    {"FONT", FONT},
    {"GRAYED", GRAYED},
    {"GROUPBOX", GROUPBOX},
    {"HELP", HELP},
    {"ICON", ICON},
    {"IMPURE", IMPURE},
    {"INACTIVE", INACTIVE},
    {"ITALIC", ITALIC},
    {"LANGUAGE", LANGUAGE},
    {"LISTBOX", LISTBOX},
    {"LOADONCALL", LOADONCALL},
    {"LTEXT", LTEXT},
    {"MENU", MENU},
    {"MENUEX", MENUEX},
    {"MENUBARBREAK", MENUBARBREAK},
    {"MENUBREAK", MENUBREAK},
    {"MENUITEM", MENUITEM},
    {"MESSAGETABLE", MESSAGETABLE},
    {"MOVEABLE", MOVEABLE},
    {"NOINVERT", NOINVERT},
    {"NONDISCARDABLE", NONDISCARDABLE},
    {"NOT", bcompl},
    {"POPUP", POPUP},
    {"PRELOAD", PRELOAD},
    {"PRODUCTVERSION", PRODUCTVERSION},
    {"PURE", PURE},
    {"PUSHBUTTON", PUSHBUTTON},
    {"RADIOBUTTON", RADIOBUTTON},
    {"RCDATA", RCDATA},
    {"RCINCLUDE", RCINCLUDE},
    {"RTEXT", RTEXT},
    {"SCROLLBAR", SCROLLBAR},
    {"SEPARATOR", SEPARATOR},
    {"SHIFT", SHIFT},
    {"STATE3", STATE3},
    {"STRINGTABLE", STRINGTABLE},
    {"STYLE", STYLE},
    {"VALUE", VALUE},
    {"VERSION", VERSION},
    {"VERSIONINFO", VERSIONINFO},
    {"VIRTKEY", VIRTKEY},
    {"WEIGHT", WEIGHT},

};

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
