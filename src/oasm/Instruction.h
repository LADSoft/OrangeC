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

#ifndef Instruction_h
#define Instruction_h

#include "Label.h"
#include "AsmExpr.h"
#include <vector>
#include <string>
#include <memory>

class Fixup;
class AsmFile;

typedef std::vector<std::shared_ptr<Fixup>> FixupContainer;
class Instruction
{
  public:
    enum iType
    {
        LABEL,
        DATA,
        CODE,
        ALIGN,
        RESERVE,
        ALT
    };
    Instruction(std::shared_ptr<Label>& lbl);
    Instruction(void* data, int Size, bool isData = false);
    Instruction(int aln);
    Instruction(int Repeat, int Size);
    Instruction(void* data);
    virtual ~Instruction();

    void RepRemoveCancellations(std::shared_ptr<AsmExprNode> exp, bool commit, int& count, Section* sect[], bool sign[], bool plus);
    void* GetAltData() const { return altdata; }
    std::shared_ptr<Label> GetLabel() const { return label; }
    bool IsLabel() { return type == LABEL; }
    int GetType() const { return type; }
    void Optimize(Section* sect, int pc, bool doErrors);
    void SetOffset(int Offs) { offs = Offs; }
    int GetOffset() const { return offs; }
    void SetFill(int fv) { fill = fv; }
    int GetFill() const { return fill; }
    void SetFillWidth(int fv) { fillWidth = fv; }
    int GetFillWidth() const { return fillWidth; }
    int const GetSize()
    {
        if (type == ALIGN)
        {
            int n = size - offs % size;
            if (n == size)
                return 0;
            return n;
        }
        else if (type == RESERVE)
            return size * repeat;
        return size;
    }
    unsigned fill = 0;
    unsigned fillWidth = 1;
    int GetRepeat() const { return repeat; }
    int GetNext(Fixup& fixup, unsigned char* buf);
    void Rewind() { pos = fpos = 0; }
    static bool ParseSectionAttrib(AsmFile* file);
    void Add(std::shared_ptr<Fixup> fixup);
    unsigned char* GetBytes() const { return data.get(); }
    FixupContainer* GetFixups();
    static void SetBigEndian(bool be) { bigEndian = be; }
    std::unique_ptr<unsigned char[]> LoadData(bool isCode, unsigned char* data, size_t size);
    bool Lost() const { return lost; }

  private:
    enum iType type;
    std::shared_ptr<Label> label;
    int size;
    int offs;
    int pos;
    int fpos;
    int repeat, xrepeat;
    void* altdata;
    bool lost;

    FixupContainer fixups;
    std::unique_ptr<unsigned char[]> data;
    static bool bigEndian;
};
#endif