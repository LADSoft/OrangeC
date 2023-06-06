/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
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
    {"(", kw::openpa},
    {")", kw::closepa},
    {"+", kw::plus},
    {"-", kw::minus},
    {"!", kw::lnot},
    {"~", kw::bcompl},
    {"*", kw::star},
    {"/", kw::divide},
    {"%", kw::mod},
    {"<<", kw::leftshift},
    {">>", kw::rightshift},
    {">", kw::gt},
    {"<", kw::lt},
    {">=", kw::geq},
    {"<=", kw::leq},
    {"==", kw::eq},
    {"!=", kw::ne},
    {"|", kw::bor},
    {"&", kw::band},
    {"^", kw::bxor},
    {"||", kw::lor},
    {"&&", kw::land},
    {"?", kw::hook},
    {":", kw::colon},
    {",", kw::comma},
    {"{", kw::BEGIN},  // openbr},
    {"}", kw::END},    // closebr},
    {";", kw::semi},
    {"ACCELERATORS", kw::ACCELERATORS},
    {"ALT", kw::ALT},
    {"ASCII", kw::ASCII},
    {"AUTO3STATE", kw::AUTO3STATE},
    {"AUTOCHECKBOX", kw::AUTOCHECKBOX},
    {"AUTORADIOBUTTON", kw::AUTORADIOBUTTON},
    {"BEGIN", kw::BEGIN},
    {"BITMAP", kw::TBITMAP},
    {"BLOCK", kw::BLOCK},
    {"CAPTION", kw::CAPTION},
    {"CHARACTERISTICS", kw::CHARACTERISTICS},
    {"CHECKBOX", kw::CHECKBOX},
    {"CHECKED", kw::CHECKED},
    {"CLASS", kw::CLASS},
    {"COMBOBOX", kw::COMBOBOX},
    {"CONTROL", kw::CONTROL},
    {"CTEXT", kw::CTEXT},
    {"CURSOR", kw::CURSOR},
    {"DEFPUSHBUTTON", kw::DEFPUSHBUTTON},
    {"DIALOG", kw::DIALOG},
    {"DIALOGEX", kw::DIALOGEX},
    {"DISCARDABLE", kw::DISCARDABLE},
    {"DLGINCLUDE", kw::DLGINCLUDE},
    {"EDITTEXT", kw::EDITTEXT},
    {"END", kw::END},
    {"EXSTYLE", kw::EXSTYLE},
    {"FILEDATE", kw::FILEDATE},
    {"FILEFLAGSMASK", kw::FILEFLAGSMASK},
    {"FILEFLAGS", kw::FILEFLAGS},
    {"FILEOS", kw::FILEOS},
    {"FILESUBTYPE", kw::FILESUBTYPE},
    {"FILETYPE", kw::FILETYPE},
    {"FILEVERSION", kw::FILEVERSION},
    {"FIXED", kw::FIXED},
    {"FONT", kw::FONT},
    {"GRAYED", kw::GRAYED},
    {"GROUPBOX", kw::GROUPBOX},
    {"HELP", kw::HELP},
    {"ICON", kw::ICON},
    {"IMPURE", kw::IMPURE},
    {"INACTIVE", kw::INACTIVE},
    {"ITALIC", kw::ITALIC},
    {"LANGUAGE", kw::LANGUAGE},
    {"LISTBOX", kw::LISTBOX},
    {"LOADONCALL", kw::LOADONCALL},
    {"LTEXT", kw::LTEXT},
    {"MENU", kw::MENU},
    {"MENUEX", kw::MENUEX},
    {"MENUBARBREAK", kw::MENUBARBREAK},
    {"MENUBREAK", kw::MENUBREAK},
    {"MENUITEM", kw::MENUITEM},
    {"MESSAGETABLE", kw::MESSAGETABLE},
    {"MOVEABLE", kw::MOVEABLE},
    {"NOINVERT", kw::NOINVERT},
    {"NONDISCARDABLE", kw::NONDISCARDABLE},
    {"NOT", kw::bcompl},
    {"POPUP", kw::POPUP},
    {"PRELOAD", kw::PRELOAD},
    {"PRODUCTVERSION", kw::PRODUCTVERSION},
    {"PURE", kw::PURE},
    {"PUSHBUTTON", kw::PUSHBUTTON},
    {"RADIOBUTTON", kw::RADIOBUTTON},
    {"RCDATA", kw::RCDATA},
    {"RCINCLUDE", kw::RCINCLUDE},
    {"RTEXT", kw::RTEXT},
    {"SCROLLBAR", kw::SCROLLBAR},
    {"SEPARATOR", kw::SEPARATOR},
    {"SHIFT", kw::SHIFT},
    {"STATE3", kw::STATE3},
    {"STRINGTABLE", kw::STRINGTABLE},
    {"STYLE", kw::STYLE},
    {"VALUE", kw::VALUE},
    {"VERSION", kw::VERSION},
    {"VERSIONINFO", kw::VERSIONINFO},
    {"VIRTKEY", kw::VIRTKEY},
    {"WEIGHT", kw::WEIGHT},

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
