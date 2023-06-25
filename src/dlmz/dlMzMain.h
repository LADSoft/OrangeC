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

#ifndef dlMzMAIN_H
#define dlMzMAIN_H

#include <string>
#include <vector>
#include "CmdSwitch.h"
#include <memory>

class ObjFile;
class OutFile;

class dlMzMain
{
  public:
    dlMzMain() : mode(UNKNOWN), file(nullptr) {}
    ~dlMzMain() {}

    int Run(int argc, char** argv);

  private:
    enum Mode
    {
        UNKNOWN,
        TINY,
        REAL
    } mode;
    bool GetMode();
    bool ReadSections(const std::string& path);
    std::string GetOutputName(char* infile) const;
    static CmdSwitchParser SwitchParser;
    static CmdSwitchBool ShowHelp;
    static CmdSwitchString outputFileSwitch;
    static CmdSwitchString modeSwitch;
    static CmdSwitchString DebugFile;
    static const char* usageText;
    static const char* helpText;

    ObjFile* file;
    std::unique_ptr<OutFile> data;
};
#endif