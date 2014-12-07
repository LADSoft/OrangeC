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
    for (int i=0; i < AddressWidth * 2 + 2 + Bytes * 3 + 1; i++)
        blanks += " ";
    for (int i=0; i < AddressWidth; i++)
        zeros += "00";
}
Listing::~Listing()
{
    while (list.size())
    {
        ListedLine *p = list.front();
        list.pop_front();
        delete p;
    }
}
void Listing::ListLine(std::fstream &out, std::string &line, ListedLine *cur, bool macro)
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
            std::string ss = Utils::NumberToStringHex( cur->label->GetOffset()->ival);
            if (ss.size() < AddressWidth *2)
                outputLine += zeros.substr(0, AddressWidth *2 - ss.size());
            outputLine += ss + std::string("  ")+ blanks.substr(0, Bytes *3 + 1) + line;
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
            if (ss.size() < AddressWidth *2)
                outputLine += zeros.substr(0, AddressWidth *2 - ss.size());
            outputLine += ss + std::string(": ")+ blanks.substr(0, Bytes *3 + 1) + line;
            out << outputLine.c_str() << std::endl;
        }
        else
        {
            int size = cur->ins->GetSize() / cur->ins->GetRepeat();
            unsigned char *buf = cur->ins->GetBytes();
            std::string ss = Utils::NumberToStringHex(cur->ins->GetOffset());
            if (ss.size() < AddressWidth *2)
                outputLine += zeros.substr(0, AddressWidth *2 - ss.size());
            if (macro)
            {
                outputLine += ss + "* ";
            }
            else
            {
                outputLine += ss + ": ";
            }
            FixupContainer *fixups = cur->ins->GetFixups();
            int z = 0;
            bool first = true;
            int top = size;
            if (fixups->size() > z)
                top = (*fixups)[z]->GetInsOffs();
            for (int i=0; i < size; i++)
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
                    if (bigEndian)
                    {
                        for (int j=0; j < fsize; j++)
                        {
                            ss = Utils::NumberToStringHex(buf[i]);
                            if (ss.size() == 1)
                                ss = std::string("0") + ss;
                            outputLine += ss;
                        }
                    }
                    else
                    {
                        for (int j=fsize-1; j >= 0; j--)
                        {
                            ss = Utils::NumberToStringHex(buf[i]);
                            if (ss.size() == 1)
                                ss = std::string("0") + ss;
                            outputLine += ss;
                        }
                        i += fsize-1;
                    }
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
                outputLine+= '+';
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
bool Listing::Write(std::string &listingName, std::string &inName, bool listMacros)
{
    std::fstream in(inName.c_str(), std::ios::in);
    if (in == NULL)
        return false;
    std::fstream out(listingName.c_str(), std::ios::out);
    if (out == NULL)
    {
        Utils::fatal(std::string(std::string("Could not open ") + listingName.c_str() + " for write.").c_str());
        return false;
    }
    std::vector<std::string> lines;
    int lineno = 1;
    ListedLine *cur = NULL;
    if (list.size())
    {
        cur = list.front();
        list.pop_front();
    }
    while (!in.eof())
    {
        char buf[4000];
        while ((!cur  || (cur && lineno < cur->lineno)) && !in.eof())
        {
            in.getline(buf, 4000);
            std::string bufs = buf;
            lineno++;
            lines.push_back(bufs);
            ListLine(out, bufs, NULL, false);
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
                cur = list.front();
                list.pop_front();
            }
            else
            {
                cur = NULL;
            }
        }
        if (cur && cur->lineno < lineno)
        {
            in.getline(buf, 4000);
            std::string bufs = buf;
            lineno++;
            lines.push_back(bufs);
            ListLine(out, bufs, NULL, false);
        }
        while (cur && cur->lineno < lineno)
        {
            if (listMacros)
                ListLine(out, lines[cur->lineno-1], cur, true);
            if (list.size())
            {
                cur = list.front();
                list.pop_front();
            }
            else
            {
                cur = NULL;
            }
        }
    }
    return true;
}
