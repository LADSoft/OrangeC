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

const char* IeeeMain::usageText = "input file[s]\n";
const char* IeeeMain::helpText =
    "input file[s]"
    "\n"
    ".o extension means read binary convert to ascii"
    ".oa extension means read ascii convert to binary"
    "Time: " __TIME__ "  Date: " __DATE__;

int main(int argc, char** argv)
{
    IeeeMain ieeeMain;
    try
    {
        return ieeeMain.Run(argc, argv);
    }
    catch (ObjIeeeBinary::SyntaxError e)
    {
        std::cout << e.what() << std::endl;
    }
}
void IeeeMain::usage(const char* prog_name, const char* text, int retcode)
{
    fprintf(stderr, "\nusage: %s %s", Utils::ShortName(prog_name), text);
    exit(retcode);
}
int IeeeMain::Run(int argc, char** argv)
{
    int rv = 0;
    Utils::banner(argv[0]);
    if (argc < 2)
    {
        usage(argv[0], usageText, 2);
    }

    CmdFiles files(argv + 1, false);

    for (auto it = files.FileNameBegin(); it != files.FileNameEnd(); ++it)
    {
        if ((*it).find(".oa") != std::string::npos)
        {
            FILE* fil = fopen((*it).c_str(), "r");
            if (fil)
            {
                std::string othername = (*it).substr(0, (*it).size() - 1);
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
                            Utils::fatal("Cannot write %s", othername.c_str());
                        }
                    }
                    else
                    {
                        Utils::fatal("Cannot read %s", (*it).c_str());
                    }
                    fclose(outfile);
                }
                else
                {
                    Utils::fatal("Cannot open '%s' for write", othername.c_str());
                }
                fclose(fil);
            }
            else
            {
                std::cout << "Cannot open " << *it << std::endl;
                rv = 1;
            }
        }
        else if ((*it).find(".o") != std::string::npos)
        {
            FILE* fil = fopen((*it).c_str(), "rb");
            if (fil)
            {
                std::string othername = (*it) + "a";
                ObjIeeeIndexManager index_manager;
                ObjFactory factory(&index_manager);
                FILE* outfile = fopen(othername.c_str(), "w");
                if (outfile)
                {
                    ObjIeeeBinary input_obj((*it).c_str());
                    ObjIeeeAscii output_obj((*it).c_str());
                    ObjFile* finput_obj = input_obj.Read(fil, ObjIeee::eAll, &factory);
                    if (finput_obj)
                    {
                        output_obj.SetTranslatorName(input_obj.GetTranslatorName());
                        output_obj.SetStartAddress(finput_obj, input_obj.GetStartAddress());
                        if (!output_obj.Write(outfile, finput_obj, &factory))
                        {
                            Utils::fatal("Cannot write %s", othername.c_str());
                        }
                    }
                    else
                    {
                        Utils::fatal("Cannot read %s", (*it).c_str());
                    }
                    fclose(outfile);
                }
                else
                {
                    Utils::fatal("Cannot open '%s' for write", othername.c_str());
                }
                fclose(fil);
            }
            else
            {
                std::cout << "Cannot open " << *it << std::endl;
                rv = 1;
            }
        }
        else
        {
            std::cout << "Cannot process " << *it << std::endl;
            rv = 1;
        }
    }
    return rv;
}
