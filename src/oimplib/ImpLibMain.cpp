/* Software License Agreement
 * 
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the 
 *     Orange C "Target Code" exception.
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
#include "DllExportReader.h"

#include <ctype.h>
#include <iostream>

int main(int argc, char **argv)
{
    ImpLibMain librarian;
    return librarian.Run(argc, argv);
}

CmdSwitchParser ImpLibMain::SwitchParser;

CmdSwitchBool ImpLibMain::caseSensitiveSwitch(SwitchParser, 'c', true);
CmdSwitchBool ImpLibMain::CDLLSwitch(SwitchParser, 'C');
CmdSwitchOutput ImpLibMain::OutputFile(SwitchParser, 'o', ".a");
CmdSwitchFile ImpLibMain::File(SwitchParser, '@');
const char *ImpLibMain::usageText = "[options] outputfile [+ files] [- files] [* files]\n"
            "\n"
            "/c-            Case insensitive library\n"
            "/oxxx          Set output file name\n"
            "/C             C language compatibility\n"
            "/V, --version  Show version and date\n"
            "/!             No logo\n"
            "@xxx           Read commands from file\n"
            "\n"
            "outputfile can be a library, object, or def file\n"
            "the input files can be objects, def files, or dll files\n"
            "\n"
            "Time: " __TIME__ "  Date: " __DATE__;

ImpLibMain::~ImpLibMain()
{
    for (auto obj : objectData)
        delete obj;
    objectData.clear();
}
void ImpLibMain::AddFile(LibManager &librarian, const char *arg)
{
    const char *p = arg;
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
        switch(mode)
        {
            case ADD:
                modified = true;
                {
                    std::string inputFile = arg;
                    int n = inputFile.find_last_of('.');
                    if (n != std::string::npos && (n == 0 || inputFile[n-1] != '.'))
                    {
                        std::string ext = inputFile.substr(n);
                        for (int i=0; i < ext.size(); ++i)
                        {
                            ext[i] = tolower(ext[i]);
                        }
                        if (ext == ".def")
                        {
                            DefFile defFile(inputFile, CDLLSwitch.GetValue());
                            if (!defFile.Read())
                            {
                                std::cout << "Def file '" << inputFile.c_str() << "' is missing or in wrong format" << std::endl;
                                return;
                            }
                            defFile.SetFileName(inputFile);
                            librarian.AddFile(*DefFileToObjFile(defFile));
                        }
                        else if (ext == ".dll")
                        {
                            DLLExportReader dllFile(inputFile);
                            if (!dllFile.Read())
                            {
                                std::cout << "Dll file '" << inputFile.c_str() << "' is missing or in wrong format" << std::endl;
                                return;
                            }
                            librarian.AddFile(*DllFileToObjFile(dllFile));
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
std::string ImpLibMain::GetInputFile(int argc, char **argv, bool &def)
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
        for (int i=0; i < ext.size(); i++)
            ext[i] = toupper(ext[i]);
        if (ext == ".DEF")
            def = true;
        else if (ext != ".DLL")
            name = "";
    }
    if (name == "")
    {
        std::cout << "Invalid input file" << std::endl;
        return "";
    }
    return name;
}
int ImpLibMain::HandleDefFile(const std::string &outputFile, int argc, char **argv)
{
    bool def;
    std::string inputFile = GetInputFile(argc, argv, def);
    if (inputFile == "")
        return 1;
    if (def)
    {
        // def to def is basically a copy operation lol...  but it will reformat
        // the file :)
        DefFile defFile(inputFile, CDLLSwitch.GetValue());
        if (!defFile.Read())
            return 1;
        defFile.SetFileName(outputFile);
        defFile.Write();
    }
    else
    {
        DLLExportReader dllFile(inputFile);
        if (!dllFile.Read())
            return 1;
        DefFile defFile(outputFile, CDLLSwitch.GetValue());
        int npos = inputFile.find_last_of(CmdFiles::DIR_SEP);
        if (npos == std::string::npos)
            npos = 0;
        else
            npos++;
        defFile.SetLibraryName(inputFile.substr(npos));
        for (auto item : dllFile)
        {
            DefFile::Export *exp = new DefFile::Export;
            exp->id = item->name;
            exp->ord = item->ordinal;
            defFile.Add(exp);
        }
        defFile.Write();
    }
    return 0;
    
}
ObjFile *ImpLibMain::DefFileToObjFile(DefFile &def)
{
    ObjectData *od = new ObjectData;
    ObjFile *obj = new ObjFile(def.GetLibraryName());
    for (DefFile::ExportIterator it = def.ExportBegin(); it != def.ExportEnd(); ++it)
    {
        ObjImportSymbol *p = od->factory.MakeImportSymbol((CDLLSwitch.GetValue() ? "_" : "") + (*it)->id);
        p->SetExternalName((*it)->entry);
        p->SetByOrdinal((*it)->byOrd);
        p->SetOrdinal((*it)->ord);
        if ((*it)->module.size())
            p->SetDllName((*it)->module);
        else
            p->SetDllName(def.GetLibraryName());
        obj->Add(p);		
    }
    return obj;
}
ObjFile *ImpLibMain::DllFileToObjFile(DLLExportReader &dll)
{
    std::string name = dll.GetName();
    int npos = name.find_last_of('\\');
    if (npos != std::string::npos)
        name.erase(0, npos + 1);
    ObjectData *od = new ObjectData;
    ObjFile *obj = new ObjFile(name);
    for (auto exp : dll)
    {
        ObjImportSymbol *p = od->factory.MakeImportSymbol((CDLLSwitch.GetValue() ? "_" : "" ) + exp->name);
        p->SetExternalName(exp->name);
        p->SetByOrdinal(exp->byOrd);
        p->SetOrdinal(exp->ordinal);
        p->SetDllName(name);
        obj->Add(p);		
    }
    return obj;
}
int ImpLibMain::HandleObjFile(const std::string &outputFile, int argc, char **argv)
{
    bool def;
    ObjFile *obj = nullptr;
    std::string inputFile = GetInputFile(argc, argv, def);
    if (inputFile == "")
        return 1;
    if (def)
    {
        DefFile defFile(inputFile, CDLLSwitch.GetValue());
        if (!defFile.Read())
            return 1;
        obj = DefFileToObjFile(defFile);
    }
    else
    {
        DLLExportReader dllFile(inputFile);
        if (!dllFile.Read())
            return 1;
        obj = DllFileToObjFile(dllFile);
    }
    ObjIeeeIndexManager im1;
    ObjFactory fact1(&im1);
    ObjIeee ieee(outputFile.c_str());
    FILE *stream = fopen(outputFile.c_str(), "wb");
    if (stream != nullptr)
    {
        ieee.Write(stream, obj, &fact1);
        fclose(stream);
        return 0;
    }
    else
    {
        return 1;
    }
}
int ImpLibMain::HandleLibrary(const std::string &outputFile, int argc, char **argv)
{
    // only get here if it is a library
    LibManager librarian(outputFile, caseSensitiveSwitch.GetValue());
    if (librarian.IsOpen())
        if (!librarian.LoadLibrary())
        {
            std::cout << outputFile.c_str() << " is not a library" << std::endl;
            return 1;
        }
    for (int i= 2; i < argc; i++)
        AddFile(librarian, argv[i]);
    for (int i = 1; i < File.GetCount(); i++)
        AddFile(librarian, File.GetValue()[i]);
    for (CmdFiles::FileNameIterator it = addFiles.FileNameBegin(); it != addFiles.FileNameEnd(); ++it)
    {
        librarian.AddFile(*(*it));
    }
    for (CmdFiles::FileNameIterator it = replaceFiles.FileNameBegin(); it != replaceFiles.FileNameEnd(); ++it)
    {
        librarian.ReplaceFile(*(*it));
    }
    if (modified)
        if (!librarian.SaveLibrary())
        {
	    FILE *fil = fopen((outputFile + "probe").c_str(), "w");
	    if (!fil)
                std::cout << "Cannot create library file" << std::endl;
            else 
                std::cout << "Error writing library file" << std::endl;
            if (fil)
            {
                fclose(fil);
                remove((outputFile + "probe").c_str());
            }
            return 1;
        }
    return 0;
}
int ImpLibMain::Run(int argc, char **argv)
{
    Utils::banner(argv[0]);
    Utils::SetEnvironmentToPathParent("ORANGEC");
    CmdSwitchFile internalConfig(SwitchParser);
    std::string configName = Utils::QualifiedFile(argv[0], ".cfg");
    std::fstream configTest(configName.c_str(), std::ios::in);
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
        for (int i=0; i < ext.size(); ++i)
        {
            ext[i] = tolower(ext[i]);
        }
        if (ext == ".def")
        {
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
        else if (ext != ".l")
        {
            std::cout << outputFile.c_str() << " is an invalid output file type" << std::endl;
            return 1;
        }
    }
    return HandleLibrary(outputFile, argc, argv);
}
