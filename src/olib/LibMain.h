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

#ifndef LIBMAIN_H
#define LIBMAIN_H

#include "LibManager.h"
#include "CmdFiles.h"

class LibMain
{
    enum Mode
    {
        ADD = 0,
        REPLACE = 1,
        EXTRACT = 2,
        REMOVE = 3,
    };

  public:
    LibMain() : mode(ADD), modified(false), changed(false) {}
    ~LibMain() {}
    int Run(int argc, char** argv);
    void AddFile(LibManager& librarian, const char* arg);
    const ObjString& GetOutputFile(int argc, char** argv);

  private:
    Mode mode;
    bool modified;
    bool changed;

    CmdFiles addFiles;
    CmdFiles replaceFiles;

    static CmdSwitchParser SwitchParser;
    static CmdSwitchBool ShowHelp;
    static CmdSwitchBool caseSensitiveSwitch;
    static CmdSwitchOutput OutputFile;
    static CmdSwitchFile File;
    static CmdSwitchBool noExport;

    static const char* usageText;
    static const char* helpText;
};
#endif