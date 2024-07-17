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

#include "IeeeMain.h"
#include <string>
#include "CmdFiles.h"
#include "ObjFactory.h"
#include "ObjIeee.h"
#include "Utils.h"
#include "ToolChain.h"

const char* IeeeMain::usageText = "input file[s]";
const char* IeeeMain::helpText =
    "input file[s]"
    "\n"
    ".o extension means read binary convert to ascii"
    ".oa extension means read ascii convert to binary"
    "Time: " __TIME__ "  Date: " __DATE__;

CmdSwitchParser IeeeMain::SwitchParser;

int main(int argc, char** argv)
{
    IeeeMain ieeeMain;
    try
    {
        return ieeeMain.Run(argc, argv);
    }
    catch (std::domain_error e)
    {
        std::cout << e.what() << std::endl;
    }
}
int IeeeMain::Run(int argc, char** argv)
{
    int rv = 0;
    auto files = ToolChain::StandardToolStartup(SwitchParser, argc, argv, usageText, helpText);
    if (files.size() < 2)
        ToolChain::Usage(usageText, 2);
    for (int i = 1; i < files.size(); i++)
    {
        if (files[i].find(".oa") != std::string::npos)
        {
            FILE* fil = fopen(files[i].c_str(), "r");
            if (fil)
            {
                std::string othername = files[i].substr(0, files[i].size() - 1);
                ObjIeeeIndexManager index_manager;
                ObjFactory factory(&index_manager);
                FILE* outfile = fopen(othername.c_str(), "wb");
                if (outfile)
                {
                    ObjIeeeAscii input_obj(othername.c_str());
                    ObjIeeeBinary output_obj(othername.c_str());
                    ObjFile* finput_obj = input_obj.Read(fil, ObjIeee::eAll, &factory);
                    if (finput_obj)
                    {
                        output_obj.SetTranslatorName(input_obj.GetTranslatorName());
                        output_obj.SetStartAddress(finput_obj, input_obj.GetStartAddress());
                        if (!output_obj.Write(outfile, finput_obj, &factory))
                        {
                            Utils::Fatal("Cannot write %s", othername.c_str());
                        }
                    }
                    else
                    {
                        Utils::Fatal("Cannot read %s", files[i].c_str());
                    }
                    fclose(outfile);
                }
                else
                {
                    Utils::Fatal("Cannot open '%s' for write", othername.c_str());
                }
                fclose(fil);
            }
            else
            {
                std::cout << "Cannot open " << files[i] << std::endl;
                rv = 1;
            }
        }
        else if (files[i].find(".o") != std::string::npos)
        {
            FILE* fil = fopen(files[i].c_str(), "rb");
            if (fil)
            {
                std::string othername = files[i] + "a";
                ObjIeeeIndexManager index_manager;
                ObjFactory factory(&index_manager);
                FILE* outfile = fopen(othername.c_str(), "w");
                if (outfile)
                {
                    ObjIeeeBinary input_obj(files[i].c_str());
                    ObjIeeeAscii output_obj(files[i].c_str());
                    ObjFile* finput_obj = input_obj.Read(fil, ObjIeee::eAll, &factory);
                    if (finput_obj)
                    {
                        output_obj.SetTranslatorName(input_obj.GetTranslatorName());
                        output_obj.SetStartAddress(finput_obj, input_obj.GetStartAddress());
                        if (!output_obj.Write(outfile, finput_obj, &factory))
                        {
                            Utils::Fatal("Cannot write %s", othername.c_str());
                        }
                    }
                    else
                    {
                        Utils::Fatal("Cannot read %s", files[i].c_str());
                    }
                    fclose(outfile);
                }
                else
                {
                    Utils::Fatal("Cannot open '%s' for write", othername.c_str());
                }
                fclose(fil);
            }
            else
            {
                std::cout << "Cannot open " << files[i] << std::endl;
                rv = 1;
            }
        }
        else
        {
            std::cout << "Cannot process " << files[i] << std::endl;
            rv = 1;
        }
    }
    return rv;
}
