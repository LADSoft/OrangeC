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

#ifndef OBJIO_H
#define OBJIO_H

#include <cstdio>
#include <vector>
#include "ObjTypes.h"

class ObjFile;
class ObjFactory;
class ObjExpression;

class ObjIOBase
{
  public:
    // clang-format off
    enum eParseType { eMake, eLink, eBrowse, eAll };
    // clang-format on
    ObjIOBase(const ObjString& Name, bool CaseSensitive) :
        name(Name),
        translatorName(""),
        bitsPerMAU(8),
        MAUS(4),
        debugInfo(true),
        startAddress(nullptr),
        startFile(nullptr),
        caseSensitive(CaseSensitive),
        absolute(false)
    {
    }
    virtual ~ObjIOBase() {}
    ObjString& GetName() { return name; }
    virtual bool Write(FILE* fil, ObjFile* File, ObjFactory* Factory) = 0;
    virtual ObjFile* Read(FILE* fil, eParseType ParseType, ObjFactory* Factory) = 0;
    ObjString GetTranslatorName() { return translatorName; }
    void SetTranslatorName(ObjString& TranslatorName) { translatorName = TranslatorName; }
    void SetTranslatorName(ObjString&& TranslatorName) { translatorName = TranslatorName; }
    ObjInt GetBitsPerMAU() { return bitsPerMAU; }
    void SetBitsPerMAU(ObjInt BitsPerMAU) { bitsPerMAU = BitsPerMAU; }
    ObjInt GetMAUS() { return MAUS; }
    void SetMAUS(ObjInt maus) { MAUS = maus; }
    bool GetDebugInfoFlag() const { return debugInfo; }
    void SetDebugInfoFlag(bool DebugInfo) { debugInfo = DebugInfo; }
    bool GetCaseSensitiveFlag() const { return caseSensitive; }
    void SetCaseSensitiveFlag(bool CaseSensitive) { caseSensitive = CaseSensitive; }
    ObjExpression* GetStartAddress() { return startAddress; }
    ObjFile* GetStartFile() { return startFile; }
    void SetStartAddress(ObjFile* file, ObjExpression* address) { startFile = file, startAddress = address; }
    void SetAbsolute(bool flag) { absolute = flag; }
    bool GetAbsolute() const { return absolute; }
    virtual std::string GetErrorQualifier() { return ""; }

  protected:
    std::string name;
    ObjString translatorName;
    ObjInt bitsPerMAU;
    ObjInt MAUS;
    bool debugInfo;
    bool caseSensitive;
    bool absolute;
    ObjExpression* startAddress;
    ObjFile* startFile;
};

class ObjIOContainer
{
    typedef std::vector<ObjIOBase*> IOBaseContainer;

  public:
    static ObjIOContainer* Instance()
    {
        if (!_instance)
        {
            _instance = new ObjIOContainer();
        }
        return _instance;
    }
    virtual void Add(ObjIOBase* ObjIO);
    virtual void Remove(ObjIOBase* ObjIO);

    typedef IOBaseContainer::iterator iterator;
    typedef IOBaseContainer::const_iterator const_iterator;

    iterator begin() { return ioModules.begin(); }
    iterator end() { return ioModules.end(); }

  protected:
    ObjIOContainer() {}
    ~ObjIOContainer() {}
    static ObjIOContainer* _instance;
    IOBaseContainer ioModules;
};

#endif
