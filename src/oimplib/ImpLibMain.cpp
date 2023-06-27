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

#include "ObjTypes.h"
#include "ObjFactory.h"
#include "ObjIeee.h"
#include "ObjSymbol.h"
#include "ObjExpression.h"
#include "LibManager.h"
#include "CmdSwitch.h"
#include "Utils.h"
#include "ImpLibMain.h"
#include "DefFile.h"
#include "DLLExportReader.h"

#include <cctype>
#include <iostream>
#include <algorithm>
int main(int argc, char** argv)
{
    ImpLibMain librarian;
    try
    {
        return librarian.Run(argc, argv);
    }
    catch (std::runtime_error e)
    {
        std::cout << e.what() << std::endl;
    }
    catch (ObjIeeeBinary::SyntaxError e)
    {
        std::cout << e.what() << std::endl;
    }
    return 1;
}

CmdSwitchParser ImpLibMain::SwitchParser;
CmdSwitchBool ImpLibMain::CDLLSwitch(SwitchParser, 'C', false);
CmdSwitchBool ImpLibMain::ShowHelp(SwitchParser, '?', false, {"help"});
CmdSwitchBool ImpLibMain::caseSensitiveSwitch(SwitchParser, 'c', true);
CmdSwitchOutput ImpLibMain::OutputFile(SwitchParser, 'o', ".a");
CmdSwitchFile ImpLibMain::File(SwitchParser, '@');
const char* ImpLibMain::helpText =
    "[options] outputfile [+ files] [- files] [* files]\n"
    "\n"
    "/c-            Case insensitive library\n"
    "/oxxx          Set output file name\n"
    "/C             Legacy, unused\n"
    "/V, --version  Show version and date\n"
    "/!, --nologo   No logo\n"
    "/?, --help     This text\n"
    "@xxx           Read commands from file\n"
    "\n"
    "outputfile can be a library, object, or def file\n"
    "the input files can be objects, def files, or dll files\n"
    "\n"
    "Time: " __TIME__ "  Date: " __DATE__;
const char* ImpLibMain::usageText = "[options] outputfile [+ files] [- files] [* files]";

ImpLibMain::~ImpLibMain() {}

void ImpLibMain::AddFile(LibManager& librarian, const char* arg)
{
    const char* p = arg;
    if (p[0] == '+')
    {
        if (p[1] == '-')
        {
            mode = REPLACE;
            p += 2;
        }
        else
        {
            mode = ADD;
            p++;
        }
    }
    else if (p[0] == '-')
    {
        if (p[1] == '+')
        {
            mode = REPLACE;
            p += 2;
        }
        else
        {
            mode = REMOVE;
            p++;
        }
    }
    else if (p[0] == '*')
    {
        mode = EXTRACT;
        p++;
    }
    if (*p)
    {
        std::string name = p;
        switch (mode)
        {
            case ADD:
                modified = true;
                {
                    std::string inputFile = arg;
                    int n = inputFile.find_last_of('.');
                    if (n != std::string::npos && (n == 0 || inputFile[n - 1] != '.'))
                    {
                        std::string ext = inputFile.substr(n);
                        transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                        if (ext == ".def")
                        {
                            DefFile defFile(inputFile);
                            if (!defFile.Read())
                            {
                                std::cout << "Def file '" << inputFile << "' is missing or in wrong format" << std::endl;
                                return;
                            }
                            defFile.SetFileName(inputFile);
                            librarian.ReplaceFile(*DefFileToObjFile(defFile));
                        }
                        else if (ext == ".dll")
                        {
                            DLLExportReader dllFile(inputFile);
                            switch (dllFile.Read())
                            {
                                case 0:
                                    break;
                                case 1:
                                    std::cout << "Dll file '" << inputFile << "' is missing" << std::endl;
                                    break;
                                case 2:
                                    std::cout << "Dll file '" << inputFile << "' is invalid format" << std::endl;
                                    break;
                                case 3:
                                    std::cout << "Dll file '" << inputFile << "' exports are missing or invalid" << std::endl;
                                    break;
                                    return;
                            }
                            librarian.ReplaceFile(*DllFileToObjFile(dllFile));
                        }
                        else
                        {
                            addFiles.Add(inputFile);
                        }
                    }
                    else
                    {
                        addFiles.Add(inputFile);
                    }
                }
                break;
            case REMOVE:
                modified = true;
                changed = true;
                librarian.RemoveFile(name);
                break;
            case REPLACE:
                modified = true;
                changed = true;
                replaceFiles.Add(name);
                break;
            case EXTRACT:
                librarian.ExtractFile(name);
                break;
        }
    }
}
std::string ImpLibMain::GetInputFile(int argc, char** argv, bool& def)
{
    std::string name;
    def = false;
    if (argc == 3)
    {
        name = argv[2];
    }
    else if (File.GetCount() == 2)
    {
        name = File.GetValue()[1];
    }
    else
    {
        std::cout << "Must be exactly one input file" << std::endl;
        return "";
    }
    int npos = name.find_last_of(".");
    if (npos == std::string::npos)
    {
        name = "";
    }
    else
    {
        std::string ext = name.substr(npos);
        transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        if (ext == ".def")
            def = true;
        else if (ext != ".dll")
            name = "";
    }
    if (name == "")
    {
        std::cout << "Invalid input file" << std::endl;
        return "";
    }
    return name;
}
int ImpLibMain::HandleDefFile(const std::string& outputFile, int argc, char** argv)
{
    bool def;
    std::string inputFile = GetInputFile(argc, argv, def);

    if (inputFile == "")
        return 1;
    if (def)
    {
        // def to def is basically a copy operation lol...  but it will reformat
        // the file :)
        DefFile defFile(inputFile);
        if (!defFile.Read())
            return 1;
        defFile.SetFileName(outputFile);
        defFile.Write();
    }
    else
    {
        DLLExportReader dllFile(inputFile);
        if (dllFile.Read())
            return 1;
        DefFile defFile(outputFile);
        int npos = inputFile.find_last_of(CmdFiles::DIR_SEP);
        if (npos == std::string::npos)
            npos = 0;
        else
            npos++;
        defFile.SetLibraryName(inputFile.substr(npos));
        for (auto& item : dllFile)
        {
            DefFile::Export* exp = new DefFile::Export;
            exp->id = item->name;
            exp->ord = item->ordinal;
            defFile.Add(exp);
        }
        defFile.Write();
    }
    return 0;
}
void ImpLibMain::CreateImportEntry(DefFile& def, ObjFile* obj, const char *externalName, DefFile::Export* exp)
{
        ObjImportSymbol* p = objectData.back()->factory.MakeImportSymbol(externalName);
        p->SetExternalName(exp->entry);
        p->SetByOrdinal(exp->byOrd);
        p->SetOrdinal(exp->ord);
        if (!exp->module.empty())
            p->SetDllName(exp->module);
        else
            p->SetDllName(def.GetLibraryName());
        obj->Add(p);
}
ObjFile* ImpLibMain::DefFileToObjFile(DefFile& def)
{
    objectData.push_back(std::make_unique<ObjectData>());
    ObjFile* obj = new ObjFile(def.GetLibraryName());
    for (auto it = def.ExportBegin(); it != def.ExportEnd(); ++it)
    {
        CreateImportEntry(def, obj, (*it)->id.c_str(), (*it).get());
        CreateImportEntry(def, obj, ("_" + (*it)->id).c_str(), (*it).get());
        int n = (*it)->id.find('@');
        if (n != 0 && n != std::string::npos)
        {
            auto id = (*it)->id.substr(0, n);
            CreateImportEntry(def, obj, id.c_str(), (*it).get());
            if (id[0] == '_')
            {
               CreateImportEntry(def, obj, id.substr(1, id.size()-1).c_str(), (*it).get());
            }
        }
    }
    return obj;
}
void ImpLibMain::CreateDLLImportEntry(ObjFile* obj, const char *dllName, const char *externalName, DLLExport* exp)
{
    ObjImportSymbol* p = objectData.back()->factory.MakeImportSymbol(externalName);
    p->SetExternalName(exp->name);
    p->SetByOrdinal(exp->byOrd);
    p->SetOrdinal(exp->ordinal);
    p->SetDllName(dllName);
    obj->Add(p);
}
ObjFile* ImpLibMain::DllFileToObjFile(DLLExportReader& dll)
{
    objectData.push_back(std::make_unique<ObjectData>());
    std::string name = dll.GetName();
    int npos = name.find_last_of('\\');
    if (npos == std::string::npos)
        npos = name.find_last_of('/');
    if (npos != std::string::npos)
        name.erase(0, npos + 1);
    ObjFile* obj = new ObjFile(name);
    for (auto&& exp : dll)
    {
        CreateDLLImportEntry(obj, name.c_str(), exp->name.c_str(), exp.get());
        CreateDLLImportEntry(obj, name.c_str(), ("_" + exp->name).c_str(), exp.get());
        int n = exp->name.find('@');
        if (n != 0 && n != std::string::npos)
        {
            auto id = exp->name.substr(0, n);
            CreateDLLImportEntry(obj, name.c_str(), id.c_str(), exp.get());
            if (id[0] == '_')
            {
               CreateDLLImportEntry(obj, name.c_str(), id.substr(1, id.size()-1).c_str(), exp.get());
            }
        }
    }
    return obj;
}
int ImpLibMain::HandleObjFile(const std::string& outputFile, int argc, char** argv)
{
    bool def;
    std::unique_ptr<ObjFile> obj;
    std::string inputFile = GetInputFile(argc, argv, def);
    if (inputFile == "")
        return 1;

    if (def)
    {
        DefFile defFile(inputFile);
        if (!defFile.Read())
            return 1;
        obj.reset(DefFileToObjFile(defFile));
    }
    else
    {
        DLLExportReader dllFile(inputFile);
        if (dllFile.Read())
            return 1;
        obj.reset(DllFileToObjFile(dllFile));
    }
    ObjIeeeIndexManager im1;
    ObjFactory fact1(&im1);
    ObjIeee ieee(outputFile.c_str());
    FILE* stream = fopen(outputFile.c_str(), "wb");
    if (stream != nullptr)
    {
        ieee.Write(stream, obj.get(), &fact1);
        fclose(stream);
        return 0;
    }
    return 1;
}
int ImpLibMain::HandleLibrary(const std::string& outputFile, int argc, char** argv)
{
    // only get here if it is a library
    LibManager librarian(outputFile, false, caseSensitiveSwitch.GetValue());
    if (librarian.IsOpen())
        if (!librarian.LoadLibrary())
        {
            std::cout << outputFile << " is not a library" << std::endl;
            return 1;
        }
    for (int i = 2; i < argc; i++)
        AddFile(librarian, argv[i]);
    for (int i = 1; i < File.GetCount(); i++)
        AddFile(librarian, File.GetValue()[i]);
    for (auto it = addFiles.FileNameBegin(); it != addFiles.FileNameEnd(); ++it)
    {
        librarian.AddFile((*it));
    }
    for (auto it = replaceFiles.FileNameBegin(); it != replaceFiles.FileNameEnd(); ++it)
    {
        librarian.ReplaceFile((*it));
    }
    if (modified)
        switch (librarian.SaveLibrary())
        {
            case LibManager::CANNOT_CREATE:
                std::cout << "Cannot open library file for 'write' access" << std::endl;
                return 1;
            case LibManager::CANNOT_WRITE:
                std::cout << "Error while writing library file" << std::endl;
                return 1;
            case LibManager::CANNOT_READ:
                std::cout << "Error while reading input files" << std::endl;
                return 1;
            default:
                break;
        }
    return 0;
}
int ImpLibMain::Run(int argc, char** argv)
{
    Utils::banner(argv[0]);
    Utils::SetEnvironmentToPathParent("ORANGEC");
    CmdSwitchFile internalConfig(SwitchParser);
    std::string configName = Utils::QualifiedFile(argv[0], ".cfg");
    std::fstream configTest(configName, std::ios::in);
    if (!configTest.fail())
    {
        configTest.close();
        if (!internalConfig.Parse(configName.c_str()))
            Utils::fatal("Corrupt configuration file");
    }
    if (!SwitchParser.Parse(&argc, argv))
    {
        Utils::usage(argv[0], usageText);
    }
    if (ShowHelp.GetExists())
        Utils::usage(argv[0], helpText);
    if (argc < 2 && File.GetCount() < 3)
    {
        Utils::usage(argv[0], usageText);
    }

    // setup
    ObjString outputFile = argv[1];
    size_t n = outputFile.find_last_of('.');
    if (n == std::string::npos)
    {
        outputFile += ".l";
    }
    else if (n != std::string::npos)
    {
        std::string ext = outputFile.substr(n);
        transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        if (ext == ".def")
        {
            std::cout << "Creating " << outputFile << std::endl;
            if (HandleDefFile(outputFile, argc, argv))
            {
                std::cout << "Missing or invalid input file" << std::endl;
                return 1;
            }
            else
            {
                return 0;
            }
        }
        else if (ext == ".o")
        {
            std::cout << "Creating " << outputFile << std::endl;
            if (HandleObjFile(outputFile, argc, argv))
            {
                std::cout << "Missing or invalid input file" << std::endl;
                return 1;
            }
            else
            {
                return 0;
            }
        }
        else if (ext != ".l" && ext != ".a" && ext != ".lib")
        {
            std::cout << outputFile << " is an invalid output file type" << std::endl;
            return 1;
        }
    }
    std::cout << "Modifying " << outputFile << std::endl;
    return HandleLibrary(outputFile, argc, argv);
}
