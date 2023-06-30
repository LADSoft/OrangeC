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

#include "ObjTypes.h"
#include "CmdSwitch.h"
#include "ResourceContainer.h"
#define PEHEADER_ONLY
#include "PEHeader.h"
#include <string>
#include <vector>
#include <memory>
#include <ctime>
#include <cstring>

class ObjFile;
class ObjFactory;
class PEObject;
class PEExportObject;

class dlPeMain
{
  public:
    dlPeMain() :
        mode(CONSOLE),
        exportObject(nullptr),
        startAddress(0),
        fileAlign(0),
        objectAlign(0),
        imageBase(0),
        importThunkVA(0),
        importCount(0),
        heapCommit(0),
        heapSize(0),
        stackCommit(0),
        stackSize(0),
        file(nullptr),
        stubSize(0)
    {
        memset(&header, 0, sizeof(header));
    }
    ~dlPeMain() {}

    int Run(int argc, char** argv);
    enum Mode
    {
        UNKNOWN,
        CONSOLE,
        GUI,
        DLL
    };
    static unsigned TimeStamp() { return (unsigned)timeStamp; }

  protected:
    void ReadValues();
    bool LoadImports(ObjFile* file);
    bool ReadSections(const std::string& path);
    std::string GetOutputName(char* infile) const;
    void ParseOutResourceFiles(int* argc, char** argv);
    bool ParseOutDefFile(int* argc, char** argv);
    void InitHeader(unsigned headerSize, ObjInt endVa);
    bool LoadStub(const std::string& exeName);
    void WriteStub(std::fstream& out);
    void PadHeader(std::fstream& out);
    bool GetMode();

  private:
    static CmdSwitchParser SwitchParser;
    static CmdSwitchBool ShowHelp;
    static CmdSwitchString modeSwitch;
    static CmdSwitchString stubSwitch;
    static CmdSwitchString outputFileSwitch;
    static CmdSwitchString DebugFile;
    static CmdSwitchBool FlatExports;
    static CmdSwitchBool Verbose;
    static CmdSwitchCombineString OutputDefFile;
    static CmdSwitchCombineString OutputImportLibrary;
    static const char* usageText;
    static const char* helpText;
    ObjInt fileAlign;
    ObjInt objectAlign;
    ObjInt imageBase;
    ObjInt importThunkVA;
    ObjInt importCount;
    ObjInt heapCommit;
    ObjInt heapSize;
    ObjInt stackCommit;
    ObjInt stackSize;
    ObjFile* file;
    std::string outputName;
    std::deque<std::unique_ptr<PEObject>> objects;
    ResourceContainer resources;
    std::string defFile;
    enum Mode mode;
    int stubSize;
    std::unique_ptr<char[]> stubData;
    PEHeader header;
    ObjInt startAddress;
    std::unique_ptr<ObjFactory> factory;
    PEExportObject* exportObject;
    static unsigned char defaultStubData[];
    static int defaultStubSize;
    static int osMajor;
    static int osMinor;
    static int userMajor;
    static int userMinor;
    static int subsysMajor;
    static int subsysMinor;
    static int subsysOverride;
    static int dllFlags;
    static time_t timeStamp;
};
#endif