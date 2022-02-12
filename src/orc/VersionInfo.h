/* Software License Agreement
 *
 *     Copyright(C) 1994-2022 David Lindauer, (LADSoft)
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

#ifndef VersionInfo_h
#define VersionInfo_h

#include "Resource.h"
#include <deque>
#include <string>
#include <memory>

class RCFile;
class ResFile;

class InternalVerInfo
{
  public:
    enum
    {
        String,
        Var
    };
    InternalVerInfo(int Type) : type(Type) {}
    int GetType() const { return type; }
    virtual void WriteRes(ResFile& resFile) = 0;
    virtual void ReadRC(RCFile& rcFile) = 0;
    virtual ~InternalVerInfo() {}

  private:
    int type;
};

class StringVerInfo : public InternalVerInfo
{
  public:
    struct Info
    {
        Info() {}
        Info(std::wstring Key, std::wstring Value) : key(Key), value(Value) {}
        std::wstring key;
        std::wstring value;
    };
    StringVerInfo(std::wstring Language) : InternalVerInfo(String), language(Language) {}
    virtual void WriteRes(ResFile& resFile);
    virtual void ReadRC(RCFile& rcFile);
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
        Info() {}

        Info(std::wstring Key) : key(Key) {}
        Info(const Info& Old);
        std::wstring key;
        std::deque<int> languages;
        typedef std::deque<int>::iterator iterator;
        typedef std::deque<int>::const_iterator const_iterator;
        iterator begin() { return languages.begin(); }
        iterator end() { return languages.end(); }
        const_iterator begin() const { return languages.begin(); }
        const_iterator end() const { return languages.end(); }
    };
    VarVerInfo() : InternalVerInfo(Var) {}
    virtual void WriteRes(ResFile& resFile);
    virtual void ReadRC(RCFile& rcFile);
    typedef std::deque<Info>::iterator iterator;
    iterator begin() { return info.begin(); }
    iterator end() { return info.end(); }

  private:
    std::deque<Info> info;
};
class VersionInfo : public Resource
{
  public:
    VersionInfo(const ResourceId& Id, const ResourceInfo& info) :
        Resource(eVersion, Id, info),
        fixed(false),
        fileVersionMS(0),
        fileVersionLS(0),
        productVersionMS(0),
        productVersionLS(0),
        fileFlagsMask(0),
        fileFlags(0),
        fileOS(0),
        fileType(0),
        fileSubType(0),
        fileDateMS(0),
        fileDateLS(0)
    {
    }
    virtual ~VersionInfo();
    virtual void WriteRes(ResFile& resFile);
    virtual void ReadRC(RCFile& rcFile);

    void SetFixed(bool flag) { fixed = flag; }
    bool GetFixed() const { return fixed; }
    void Add(InternalVerInfo* v);
    typedef std::deque<std::unique_ptr<InternalVerInfo>>::iterator iterator;
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
    std::deque<std::unique_ptr<InternalVerInfo>> varInfo;
};
#endif
