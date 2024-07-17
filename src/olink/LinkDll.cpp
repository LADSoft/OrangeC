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

#include "LinkDll.h"
#include "PEHeader.h"
#include "MZHeader.h"
#include "Utils.h"
#include "ToolChain.h"
#include <algorithm>
#include <fstream>
#include <deque>
#ifdef HAVE_UNISTD_H
#    include <unistd.h>
#else
#    include <io.h>
#endif

#include <cstdlib>
struct TempFileDeleter
{
    ~TempFileDeleter()
    {
        for (auto q : list)
            unlink(q.c_str());
    }
    void Add(const std::string& name) { list.push_back(name); }

  private:
    std::deque<std::string> list;
};

TempFileDeleter deleter;
void LinkDll::LoadDll()
{
    if (name.length() < 5)
        return;
    std::string data = name;
    std::transform(data.begin(), data.end(), data.begin(), [](unsigned char c) { return tolower(c); });
    if (data.substr(data.size() - 4) == ".dll")
    {
        auto fil = fopen(name.c_str(), "rb");
        if (fil)
            fclose(fil);
        if (!fil)
        {
            auto nm = Utils::FindOnPath(name, libPath);
            if (nm.size() == 0)
            {
                char* p = getenv("PATH");
                if (!p)
                    return;
                nm = Utils::FindOnPath(name, p);
                if (nm.size() == 0)
                    return;
            }
            name = nm;
        }
        std::fstream in(name, std::ios::in | std::ios::binary);
        if (!in.is_open())
            return;  // can't find file
        if (!in.fail())
        {
            MZHeader mzh;
            in.read((char*)&mzh, sizeof(mzh));
            if (!in.fail() && mzh.signature == MZ_SIGNATURE)
            {

                // seek to the position where the offset to the PEHeader is stored
                in.seekg(mzh.n_header_paragraphs * 16 - 4);
                int pos;
                in.read((char*)&pos, sizeof(pos));
                if (!in.fail())
                {
                    in.seekg(pos);
                    if (!in.fail())
                    {
                        PEHeader peh;
                        in.read((char*)&peh, sizeof(peh));
                        if (!in.fail() && peh.signature == PESIG)
                        {
                            if (peh.flags & PE_FILE_LIBRARY)
                            {
                                isDll = true;
                                if (peh.flags & PE_FILE_32BIT)
                                    matchesArchitecture = true;
                            }
                        }
                    }
                }
            }
        }
    }
}
std::unique_ptr<LinkLibrary> LinkDll::LoadLibrary(bool isstdcall)
{
    auto rv = std::unique_ptr<LinkLibrary>(nullptr);
    std::string fileName;
    auto fil = Utils::TempName(fileName);
    if (fil)
    {
        fileName += ".l";
        deleter.Add(fileName);
        fclose(fil);
        std::string C = isstdcall ? "" : "-C";
        if (!ToolChain::ToolInvoke("oimplib", nullptr, " -! %s \"%s\" \"%s\"", C.c_str(), fileName.c_str(), name.c_str()))
        {
            rv = std::make_unique<LinkLibrary>(fileName, caseSensitive);
            if (rv)
            {
                if (rv->IsOpen())
                {
                    if (!rv->Load())
                    {
                        rv.release();
                    }
                }
                else
                {
                    rv.release();
                }
            }
        }
    }
    return rv;
}
