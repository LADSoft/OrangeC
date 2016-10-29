/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
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
#ifndef OBJIO_H
#define OBJIO_H

#include <stdio.h>
#include <vector>
#include "ObjTypes.h"

class ObjFile ;
class ObjFactory;
class ObjExpression;

class ObjIOBase
{
public:
    enum eParseType { eMake, eLink, eBrowse, eAll };
    ObjIOBase(const ObjString Name, bool CaseSensitive) : name(Name), translatorName(""),bitsPerMAU(8),
                MAUS(4), debugInfo(true), startAddress(NULL), caseSensitive(CaseSensitive), absolute(false) {}
    virtual ~ObjIOBase() { }
    ObjString &GetName() { return name; }
    virtual bool Write(FILE *fil, ObjFile *File, ObjFactory *Factory) = 0;
    virtual ObjFile *Read(FILE *fil, eParseType ParseType, ObjFactory *Factory) = 0;
    ObjString GetTranslatorName() { return translatorName; }
    void SetTranslatorName(ObjString TranslatorName ) { translatorName = TranslatorName; }
    ObjInt GetBitsPerMAU() { return bitsPerMAU; }
    void SetBitsPerMAU(ObjInt BitsPerMAU) { bitsPerMAU = BitsPerMAU; }
    ObjInt GetMAUS() { return MAUS; }
    void SetMAUS(ObjInt maus) { MAUS = maus; }
    bool GetDebugInfoFlag() const { return debugInfo; }
    void SetDebugInfoFlag(bool DebugInfo) { debugInfo = DebugInfo; }
    bool GetCaseSensitiveFlag() const { return caseSensitive; }
    void SetCaseSensitiveFlag(bool CaseSensitive) { caseSensitive = CaseSensitive; }
    ObjExpression *GetStartAddress() { return startAddress; }
    ObjFile *GetStartFile() { return startFile; }
    void SetStartAddress(ObjFile *file, ObjExpression *address) { startFile = file, startAddress = address; }
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
    ObjExpression *startAddress;
    ObjFile *startFile;
};

class ObjIOContainer
{
    typedef std::vector<ObjIOBase *> IOBaseContainer;
public:
    static ObjIOContainer *Instance()
    {
        if (!_instance)
        {
            _instance = new ObjIOContainer();
        }
        return _instance;
    }
    virtual void Add(ObjIOBase *ObjIO) ;
    virtual void Remove(ObjIOBase *ObjIO) ;
    
    typedef IOBaseContainer::iterator IOBaseIterator;
    typedef IOBaseContainer::const_iterator const_IOBaseIterator;

    IOBaseIterator IOBaseBegin() { return ioModules.begin(); }
    IOBaseIterator IOBaseEnd() { return ioModules.end(); }
protected:
    ObjIOContainer() { }
    ~ObjIOContainer() { }
    static ObjIOContainer *_instance;
    IOBaseContainer ioModules;
};

#endif
