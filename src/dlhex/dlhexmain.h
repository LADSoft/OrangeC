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
#include <vector>
#include "CmdSwitch.h"
#include "ObjFile.h"
#include "ObjFactory.h"
#include <memory>
class dlHexMain
{
    struct Section
    {
        Section(ObjInt Address = 0, ObjInt Size = 0) : address(Address), size(Size) {}
        ObjInt address;
        ObjInt size;
        std::unique_ptr<char[]> data;
    };

  public:
    dlHexMain() : outputMode(eBinary), extraMode(0) {}

    int Run(int argc, char** argv);
    void GetSectionNames(std::vector<std::string>& names, ObjFile* file);
    void GetInputSections(const std::vector<std::string>& names, ObjFile* file, ObjFactory* factory);
    bool ReadSections(const std::string& path);
    bool GetOutputMode();
    std::string GetOutputName(char* infile) const;

  private:
    static CmdSwitchParser SwitchParser;
    static CmdSwitchBool ShowHelp;
    static CmdSwitchString modeSwitch;
    static CmdSwitchString sectionsSwitch;
    static CmdSwitchHex padSwitch;
    static CmdSwitchString outputFileSwitch;
    static CmdSwitchString DebugFile;
    static const char* usageText;
    static const char* helpText;

    std::vector<std::unique_ptr<Section>> sections;
    enum OutputMode
    {
        eBinary,
        eMotorola,
        eIntel
    } outputMode;
    int extraMode;
};
#endif