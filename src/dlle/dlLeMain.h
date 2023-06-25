/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
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

#ifndef DLHEXMAIN_H
#define DLHEXMAIN_H

#include <string>
#include <fstream>
#include <deque>
#include "ObjTypes.h"
#include "CmdSwitch.h"
#include "LEHeader.h"
#include <memory>
#include <cstring>
class ObjFile;
class ObjFactory;
class LEObject;
class LEFixup;
class ObjectPage;
class ResidentNameTable;

class dlLeMain
{
  public:
    dlLeMain() :
        mode(eLe),
        rnt(nullptr),
        factory(nullptr),
        file(nullptr),
        startAddress(0),
        stackSize(0),
        fixups(0),
        objectPages(0),
        stubSize(0)
    {
        memset(&header, 0, sizeof(header));
    }
    ~dlLeMain() {}

    int Run(int argc, char** argv);
    enum Mode
    {
        UNKNOWN,
        eLe,
        eLx
    };

  protected:
    bool GetMode();
    void ReadValues();
    bool ReadSections(const std::string& path, const std::string& exeName);
    std::string GetOutputName(char* infile) const;
    void InitHeader();
    bool LoadStub(const std::string& exeName);
    void WriteStub(std::fstream& out);

  private:
    static CmdSwitchParser SwitchParser;
    static CmdSwitchBool ShowHelp;
    static CmdSwitchString modeSwitch;
    static CmdSwitchString stubSwitch;
    static CmdSwitchString outputFileSwitch;
    static CmdSwitchString DebugFile;
    static const char* usageText;
    static const char* helpText;

    ObjInt stackSize;
    std::string outputName;
    std::deque<LEObject*> objects;
    LEFixup* fixups;
    ObjectPage* objectPages;
    ResidentNameTable* rnt;
    enum Mode mode;
    int stubSize;
    std::unique_ptr<char[]> stubData;
    LEHeader header;
    static unsigned fileVersion;
    ObjFactory* factory;
    ObjFile* file;
    ObjInt startAddress;
};
#endif