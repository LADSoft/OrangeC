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

#ifndef Listing_h
#define Listing_h

class Label;
class Instruction;

#include <fstream>
#include <deque>
#include <memory>
static const int AddressWidth = 4;
static const int Bytes = 8;

class ListedLine
{
  public:
    ListedLine(std::shared_ptr<Instruction>& Ins, int LineNo) : lineno(LineNo), label(nullptr), ins(Ins) {}
    ListedLine(std::shared_ptr<Label>& lbl, int LineNo) : lineno(LineNo), label(lbl), ins(nullptr) {}
    int lineno;
    std::shared_ptr<Label> label;
    std::shared_ptr<Instruction> ins;
};
class Listing
{
  public:
    Listing();
    ~Listing();
    void Add(std::shared_ptr<Instruction>& ins, int lineno, bool inMacro) { list.push_back(std::make_unique<ListedLine>(ins, lineno)); }
    void Add(std::shared_ptr<Label>& lbl, int lineno, bool inMacro) { list.push_back(std::make_unique<ListedLine>(lbl, lineno)); }
    bool Write(std::string& listingName, std::string& inName, bool listMacros);
    void SetBigEndian(bool flag) { bigEndian = flag; }

  protected:
    void ListLine(std::fstream& out, std::string& line, ListedLine* cur, bool macro);

  private:
    std::deque<std::unique_ptr<ListedLine>> list;
    std::string blanks;
    std::string zeros;
    bool bigEndian;
};

#endif