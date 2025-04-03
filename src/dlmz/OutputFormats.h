/* Software License Agreement
 *
 *     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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

#ifndef OutputFormats_h
#define OutputFormats_h

#include <fstream>
#include <deque>
#include <map>
#include <memory>
#include "MZHeader.h"
class ObjFile;
class ObjExpression;
class OutFile
{
  public:
    OutFile() : size(0), startSeg(0), startOffs(0), stackSeg(0), stackOffs(0) {}
    virtual ~OutFile() {}
    virtual bool ReadSections(ObjFile* file, ObjExpression* start) = 0;
    virtual bool Write(std::fstream& stream) = 0;

  protected:
    std::unique_ptr<unsigned char[]> data;
    int size;
    int startSeg, startOffs;
    int stackSeg, stackOffs;
};
class Tiny : public OutFile
{
  public:
    Tiny() : OutFile() {}
    virtual bool ReadSections(ObjFile* file, ObjExpression* start);
    virtual bool Write(std::fstream& stream);
};
class Real : public OutFile
{
  public:
    Real() : OutFile() { memset(&header, 0, sizeof(header)); }
    virtual bool ReadSections(ObjFile* file, ObjExpression* start);
    virtual bool Write(std::fstream& stream);

  protected:
    void GetSectionBalance(ObjExpression* n, int& add, int& sub, bool positive);
    bool Balanced(ObjExpression* n, bool skipping);
    void WriteHeader(std::fstream& stream);
    int GetFixupOffset(ObjExpression* fixup, int sbase, int pc);
    int GetFirstSeg(ObjExpression* exp);
    struct Fixup
    {
        Fixup() : seg(0), off(0) {}
        Fixup(int Seg, int Off) : seg(Seg), off(Off) {}
        int seg;
        int off;
    };
    MZHeader header;
    std::deque<Fixup> fixups;
};

#endif