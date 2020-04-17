/* Software License Agreement
 * 
 *     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
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

#ifndef DLLExportReader_H
#define DLLExportReader_H

#include <string>
#include <fstream>
#include <deque>

struct DLLExport
{
    DLLExport(const std::string &Name, int ord, bool ByOrd) : name(Name), ordinal(ord), byOrd(ByOrd) { }
    std::string name;
    int ordinal;
    bool byOrd;
};

class DLLExportReader
{
public:
    DLLExportReader(const std::string &fname) : name(fname) { }
    ~DLLExportReader();
    bool Read() { bool rv = FindDLL(); if (rv) rv = LoadExports(); return rv; }

    std::string Name() { return name; }
    typedef std::deque<DLLExport *>::iterator iterator;
    iterator begin() { return exports.begin(); }
    iterator end() { return exports.end(); }
protected:
    bool doExports(std::fstream &in, int phys, int rva);
    bool FindDLL();
    bool LoadExports();
private:
    std::string name;
    std::deque<DLLExport *> exports;
};
#endif
