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

#ifndef ResourceInfo_h
#define ResourceInfo_h

class RCFile;
class ResFile;

class ResourceInfo
{
  public:
    enum
    {
        Moveable = 0x10,
        Pure = 0x20,
        Preload = 0x40,
        LoadOnCall = 0x0,
        Discardable = 0x1000
    };
    ResourceInfo(int Language) : language(Language), memoryFlags(0), characteristics(0), version(0) {}
    void SetFlags(unsigned flags) { memoryFlags = flags; }
    unsigned GetFlags() const { return memoryFlags; }
    void SetLanguage(int Language) { language = Language; }
    int GetLanguage() const { return language; }
    void SetCharacteristics(unsigned chars) { characteristics = chars; }
    unsigned GetCharacteristics() const { return characteristics; }
    void SetVersion(unsigned Version) { version = Version; }
    unsigned GetVersion() const { return version; }

    void WriteRes(ResFile& resFile);
    void ReadRC(RCFile& rcFile, bool withSecondary);

  private:
    unsigned memoryFlags;
    int language;
    unsigned characteristics;
    unsigned version;
};

#endif