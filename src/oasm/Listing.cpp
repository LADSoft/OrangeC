/* Software License Agreement
 * 
 *     Copyright(C) 1994-2024 David Lindauer, (LADSoft)
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

#include "Listing.h"
#include "Label.h"
#include "Instruction.h"
#include "Fixup.h"
#include "Utils.h"
#include <sstream>
#include <iomanip>
#include <fstream>

Listing::Listing() : bigEndian(false)
{
    for (int i = 0; i < AddressWidth * 2 + 2 + Bytes * 3 + 1; i++)
        blanks += " ";
    for (int i = 0; i < AddressWidth; i++)
        zeros += "00";
}
Listing::~Listing() {}
void Listing::ListLine(std::fstream& out, std::string& line, ListedLine* cur, bool macro)
{
    std::string outputLine;
    if (!cur)
    {
        outputLine = blanks + line;
        out << outputLine.c_str() << std::endl;
    }
    else
    {
        if (cur->label)
        {
            std::string ss = Utils::NumberToStringHex(cur->label->GetOffset()->ival);
            if (ss.size() < AddressWidth * 2)
                outputLine += zeros.substr(0, AddressWidth * 2 - ss.size());
            outputLine += ss + std::string("  ") + blanks.substr(0, Bytes * 3 + 1) + line;
            out << outputLine.c_str() << std::endl;
        }
        else if (cur->ins->GetType() == Instruction::ALIGN)
        {
            outputLine = blanks + line;
            out << outputLine.c_str() << std::endl;
        }
        else if (cur->ins->IsLabel())
        {
            std::string ss = Utils::NumberToStringHex(cur->ins->GetOffset());
            if (ss.size() < AddressWidth * 2)
                outputLine += zeros.substr(0, AddressWidth * 2 - ss.size());
            outputLine += ss + std::string(": ") + blanks.substr(0, Bytes * 3 + 1) + line;
            out << outputLine.c_str() << std::endl;
        }
        else
        {
            int size = cur->ins->GetSize() / cur->ins->GetRepeat();
            unsigned char* buf = cur->ins->GetBytes();
            std::string ss = Utils::NumberToStringHex(cur->ins->GetOffset());
            if (ss.size() < AddressWidth * 2)
                outputLine += zeros.substr(0, AddressWidth * 2 - ss.size());
            if (macro)
            {
                outputLine += ss + "* ";
            }
            else
            {
                outputLine += ss + ": ";
            }
            FixupContainer* fixups = cur->ins->GetFixups();
            int z = 0;
            bool first = true;
            int top = size;
            if (fixups->size() > z)
                top = (*fixups)[z]->GetInsOffs();
            for (int i = 0; i < size; i++)
            {
                if (i < top)
                {
                    if (outputLine.size() - AddressWidth * 2 - 2 > Bytes * 3 - 3)
                    {
                        if (first)
                        {
                            outputLine += blanks.substr(0, AddressWidth * 2 + 2 + Bytes * 3 + 1 - outputLine.size());
                            outputLine += line;
                            first = false;
                        }
                        out << outputLine.c_str() << std::endl;
                        outputLine = blanks.substr(0, 10);
                    }
                    ss = Utils::NumberToStringHex(buf[i]);
                    if (ss.size() == 1)
                        ss = std::string("0") + ss;
                    outputLine += ss + " ";
                }
                else
                {
                    int fsize = (*fixups)[z]->GetSize();
                    if (outputLine.size() - AddressWidth * 2 - 2 > Bytes * 3 - 1 - fsize * 2)
                    {
                        if (first)
                        {
                            outputLine += blanks.substr(0, AddressWidth * 2 + 2 + Bytes * 3 + 1 - outputLine.size());
                            outputLine += line;
                            first = false;
                        }
                        out << outputLine.c_str() << std::endl;
                        outputLine = blanks.substr(0, 10);
                    }
                    //                    if (bigEndian)
                    {
                        for (int j = 0; j < fsize; j++)
                        {
                            ss = Utils::NumberToStringHex(buf[j + i]);
                            if (ss.size() == 1)
                                ss = std::string("0") + ss;
                            outputLine += ss;
                        }
                        i += fsize - 1;
                    }
                    /*
                    else
                    {
                        for (int j=fsize-1; j >= 0; j--)
                        {
                            ss = Utils::NumberToStringHex(buf[fsize-j+i]);
                            if (ss.size() == 1)
                                ss = std::string("0") + ss;
                            outputLine += ss;
                        }
                        i += fsize-1;
                    }
                    */
                    if ((*fixups)[z]->IsResolved())
                    {
                        outputLine += " ";
                    }
                    else if ((*fixups)[z]->GetExpr()->GetType() == AsmExprNode::DIV)
                    {
                        outputLine += "s ";
                    }
                    else
                    {
                        outputLine += "r ";
                    }
                    top = size;
                    if (fixups->size() > ++z)
                        top = (*fixups)[z]->GetInsOffs();
                }
            }
            if (cur->ins->GetRepeat() != 1)
            {
                if (outputLine[outputLine.size() - 2] == ' ')
                    outputLine = outputLine.substr(0, outputLine.size() - 2);
                else
                    outputLine = outputLine.substr(0, outputLine.size() - 1);
                outputLine += '+';
            }
            if (outputLine.size() - AddressWidth * 2 - 2 != 0)
            {
                if (first)
                {
                    outputLine += blanks.substr(0, AddressWidth * 2 + 2 + Bytes * 3 + 1 - outputLine.size());
                    outputLine += line;
                    first = false;
                }
                out << outputLine.c_str() << std::endl;
            }
        }
    }
}
bool Listing::Write(std::string& listingName, std::string& inName, bool listMacros)
{
    std::fstream in(inName.c_str(), std::ios::in);
    if (!in.is_open())
        return false;
    std::fstream out(listingName.c_str(), std::ios::out);
    if (!out.is_open())
    {
        Utils::Fatal(std::string(std::string("Could not open ") + listingName.c_str() + " for write.").c_str());
        return false;
    }
    std::vector<std::string> lines;
    int lineno = 1;
    ListedLine* cur = nullptr;
    if (list.size())
    {
        cur = list.front().release();
        list.pop_front();
    }
    while (!in.eof())
    {
        char buf[4000];
        while ((!cur || (cur && lineno < cur->lineno)) && !in.eof())
        {
            in.getline(buf, 4000);
            std::string bufs = buf;
            lineno++;
            lines.push_back(bufs);
            ListLine(out, bufs, nullptr, false);
        }
        if (cur && cur->lineno == lineno)
        {
            in.getline(buf, 4000);
            std::string bufs = buf;
            lineno++;
            lines.push_back(bufs);
            ListLine(out, bufs, cur, false);
            if (list.size())
            {
                cur = list.front().release();
                list.pop_front();
            }
            else
            {
                cur = nullptr;
            }
        }
        if (cur && cur->lineno < lineno)
        {
            in.getline(buf, 4000);
            std::string bufs = buf;
            lineno++;
            lines.push_back(bufs);
            ListLine(out, bufs, nullptr, false);
        }
        while (cur && cur->lineno < lineno)
        {
            if (listMacros)
                ListLine(out, lines[cur->lineno - 1], cur, true);
            if (list.size())
            {
                cur = list.front().release();
                list.pop_front();
            }
            else
            {
                cur = nullptr;
            }
        }
    }
    return true;
}
