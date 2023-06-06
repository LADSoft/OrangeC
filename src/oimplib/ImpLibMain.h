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

class ObjFile;
class DefFile;
class DLLExportReader;

class ImpLibMain
{
    enum Mode
    {
        ADD = 0,
        REPLACE = 1,
        EXTRACT = 2,
        REMOVE = 3,
    };

  public:
    ImpLibMain() : mode(ADD), modified(false), changed(false) {}
    ~ImpLibMain();
    int Run(int argc, char** argv);
    void AddFile(LibManager& librarian, const char* arg);
    const ObjString& GetOutputFile(int argc, char** argv);
    std::string GetInputFile(int argc, char** argv, bool& def);
    ObjFile* DefFileToObjFile(DefFile& def);
    ObjFile* DllFileToObjFile(DLLExportReader& dll);
    int HandleObjFile(const std::string& outputFile, int argc, char** argv);
    int HandleDefFile(const std::string& outputFile, int argc, char** argv);
    int HandleLibrary(const std::string& outputFile, int argc, char** argv);

  private:
    struct ObjectData
    {
        ObjectData() : factory(&iml) {}
        ObjIeeeIndexManager iml;
        ObjFactory factory;
    };
    std::deque<std::unique_ptr<ObjectData>> objectData;
    Mode mode;
    bool modified;
    bool changed;

    CmdFiles addFiles;
    CmdFiles replaceFiles;

    static CmdSwitchParser SwitchParser;
    static CmdSwitchBool caseSensitiveSwitch;
    static CmdSwitchOutput OutputFile;
    static CmdSwitchFile File;
    static CmdSwitchBool CDLLSwitch;

    static const char* usageText;
};
#endif