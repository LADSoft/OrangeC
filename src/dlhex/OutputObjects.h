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

#ifndef OUTPUTOBJECTS_H
#define OUTPUTOBJECTS_H

#include <fstream>
#include <string>
#include "ObjTypes.h"
class OutputObject
{
  public:
    OutputObject() {}
    virtual ~OutputObject() {}

    virtual int Write(std::fstream& stream, char* data, int line, int firstAddress) = 0;
    virtual int Pad(std::fstream& stream, ObjInt addr, ObjInt size, int padChar);
    virtual int WriteHeader(std::fstream& stream) = 0;
    virtual int WriteTrailer(std::fstream& stream) = 0;
    virtual std::ios::openmode GetOpenFlags() = 0;
};

class IntelOutputObject : public OutputObject
{
  public:
    IntelOutputObject(int Stype) : stype(Stype) {}
    virtual ~IntelOutputObject() {}

    virtual int Write(std::fstream& stream, char* data, int line, int firstAddress);
    virtual int WriteHeader(std::fstream& stream);
    virtual int WriteTrailer(std::fstream& stream);
    virtual std::ios::openmode GetOpenFlags() { return std::ios::out; }

  protected:
    void putrecord(std::fstream& stream, unsigned char* data, int datalen, int offset);
    void putulba(std::fstream& stream, int address);

  private:
    int stype;
};

class MotorolaOutputObject : public OutputObject
{
  public:
    MotorolaOutputObject(const std::string& Name, int Type) : type(Type), name(Name) {}
    virtual ~MotorolaOutputObject() {}

    virtual int Write(std::fstream& stream, char* data, int line, int firstAddress);
    virtual int WriteHeader(std::fstream& stream);
    virtual int WriteTrailer(std::fstream& stream);
    virtual std::ios::openmode GetOpenFlags() { return std::ios::out; }

  protected:
    void putdatarec(std::fstream& stream, unsigned char* data, int datalen, long offset);
    void putendrec(std::fstream& stream);
    void putheaderrec(std::fstream& stream);

  private:
    int type;
    const std::string& name;
};
class BinaryOutputObject : public OutputObject
{
  public:
    BinaryOutputObject() {}
    virtual ~BinaryOutputObject() {}

    virtual int Write(std::fstream& stream, char* data, int line, int firstAddress);
    virtual int WriteHeader(std::fstream&) { return 0; }
    virtual int WriteTrailer(std::fstream&) { return 0; }
    virtual std::ios::openmode GetOpenFlags() { return std::ios::out | std::ios::binary; }
};

#endif