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
#ifndef VersionInfo_h
#define VersionInfo_h

#include "Resource.h"
#include <deque>
#include <string>

class RCFile;
class ResFile;

class InternalVerInfo
{
public:
    enum
    {
        String, Var
    };
    InternalVerInfo(int Type) : type(Type) { }
    int GetType() const { return type; }
    virtual void WriteRes(ResFile &resFile) = 0;
    virtual bool ReadRC(RCFile &rcFile) = 0;
	virtual ~InternalVerInfo() {}
private:
    int type;
};

class StringVerInfo : public InternalVerInfo
{
public:
    struct Info
    {
        Info(std::wstring Key, std::wstring Value) : key(Key), value(Value) { }
        std::wstring key;
        std::wstring value;
    };
    StringVerInfo(std::wstring Language) : InternalVerInfo(String), language(Language) { }
    virtual void WriteRes(ResFile &resFile);
    virtual bool ReadRC(RCFile &rcFile);
    std::wstring GetLanguage() const { return language; }
    void Add(std::wstring key, std::wstring value) { strings.push_back(Info(key, value)); }
    typedef std::deque<Info>::iterator iterator;
    iterator begin() { return strings.begin(); }
    iterator end() { return strings.end(); }
private:
    std::wstring language;
    std::deque<Info> strings;
};
class VarVerInfo : public InternalVerInfo
{
public:
    struct Info
    {
        
        Info(std::wstring Key) : key(Key) { }
        Info(const Info &Old);
        std::wstring key;
        std::deque<int> languages;
        typedef std::deque<int>::iterator iterator;
        typedef std::deque<int>::const_iterator const_iterator;
        iterator begin() { return languages.begin(); }
        iterator end() { return languages.end(); }
        const_iterator begin() const { return languages.begin(); }
        const_iterator end() const{ return languages.end(); }
    };
    VarVerInfo() : InternalVerInfo(Var) { }
    virtual void WriteRes(ResFile &resFile);
    virtual bool ReadRC(RCFile &rcFile);
    typedef std::deque<Info>::iterator iterator;
    iterator begin() { return info.begin(); }
    iterator end() { return info.end(); }
private:
    std::deque<Info> info;
};
class VersionInfo : public Resource
{
public:
    VersionInfo(const ResourceId &Id, const ResourceInfo &info)
        : Resource(eVersion, Id, info), fixed(false),
        fileVersionMS(0), fileVersionLS(0),
        productVersionMS(0), productVersionLS(0),
        fileFlagsMask(0), fileFlags(0),
        fileOS(0), fileType(0), fileSubType(0),
        fileDateMS(0), fileDateLS(0) { }
    virtual ~VersionInfo();
    virtual void WriteRes(ResFile &resFile);
    virtual bool ReadRC(RCFile &rcFile);
    
    void SetFixed(bool flag) { fixed = flag; }
    bool GetFixed() const { return fixed; }
    void Add(InternalVerInfo *v) { varInfo.push_back(v); }
    typedef std::deque<InternalVerInfo *>::iterator iterator;
    iterator begin() { return varInfo.begin(); }
    iterator end() { return varInfo.end(); }
    void SetFileVersion(unsigned high, unsigned low) { fileVersionMS = high, fileVersionLS = low; }
    unsigned GetFileVersionMS() const { return fileVersionMS; }
    unsigned GetFileVersionLS() const { return fileVersionLS; }
    void SetProductVersion(unsigned high, unsigned low) { productVersionMS = high, productVersionLS = low; }
    unsigned GetProductVersionMS() const { return productVersionMS; }
    unsigned GetProductVersionLS() const { return productVersionLS; }
    void SetFileFlagsMask(unsigned mask) { fileFlagsMask = mask; }
    unsigned GetFileFlagsMask() const { return fileFlagsMask; }
    void SetFileFlags(unsigned flags) { fileFlags = flags; }
    unsigned GetFileFlags() const { return fileFlags; }
    void SetFileOS(unsigned OS) { fileOS = OS; }
    unsigned GetFileOS() const { return fileOS; }
    void SetFileType(unsigned Type) { fileType = Type; }
    unsigned GetFileType() const { return fileType; }
    void SetFileSubType(unsigned SubType) { fileSubType = SubType; }
    unsigned GetFileSubType() const { return fileSubType; }
    void SetFileDate(unsigned high, unsigned low) { fileDateMS = high, fileDateLS = low; }
    unsigned GetFileDateMS() const { return fileDateMS; }
    unsigned GetFileDateLS() const { return fileDateLS; }
private:
    bool fixed;
    unsigned fileVersionMS;
    unsigned fileVersionLS;
    unsigned productVersionMS;
    unsigned productVersionLS;
    unsigned fileFlagsMask;
    unsigned fileFlags;
    unsigned fileOS;
    unsigned fileType;
    unsigned fileSubType;
    unsigned fileDateMS;
    unsigned fileDateLS;
    std::deque<InternalVerInfo *> varInfo;
};
#endif
