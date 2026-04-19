/* Software License Agreement
 *
 *     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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
 *
 */

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "be.h"
#include "winmode.h"
#include "Utils.h"
#include "ToolChain.h"
#include "config.h"
#include "ildata.h"
#ifdef HAVE_UNISTD_H
#    include <unistd.h>
#endif

#define TEMPFILE "$$$OCC.TMP"
namespace occmsil
{
static std::list<std::string> objlist, reslist, rclist;
static char outFileName[260];

bool IsCompilerSource(const char* buffer)
{
    bool found = false;
    static std::list<std::string> acceptedExtensions = {".c", ".cc", ".cpp", ".cxx"};
    for (auto& str : acceptedExtensions)
    {
        if (Utils::HasExt(buffer, str.c_str()) || Utils::iequal(buffer, str.c_str()))
        {
            found = true;
            break;
        }
    }
    return found;
}
static void InsertFile(std::list<std::string>& target, const std::string& name, const std::string& as)
{
    int index = name.find_last_of("#");
    if (index >= 0)
        return;
    if (!outFileName[0])
    {
        const char* ext = Optimizer::cparams.prm_targettype == DLL ? ".dll" : ".exe";
        Utils::StrCpy(outFileName, Optimizer::outputFileName.c_str());
        if (outFileName[0] && outFileName[strlen(outFileName) - 1] == '\\')
        {
            // output file is a path specification rather than a file name
            // just add our name and ext
            Utils::StrCat(outFileName, name.c_str());
            Utils::StripExt(outFileName);
            Utils::AddExt(outFileName, ext);
        }
        else if (outFileName[0] == 0)  // no output file specified, put the output wherever the input was...
        {
            Utils::StrCpy(outFileName, name.c_str());
            char* p = (char*)strrchr(outFileName, '\\');
            char* q = (char*)strrchr(outFileName, '/');
            if (q > p)
                p = q;
            if (p)
                Utils::StrCpy(outFileName, p + 1);
            Utils::StripExt(outFileName);
            if (sizeof(outFileName) - 1 - strlen(outFileName) >= strlen(ext))
                Utils::AddExt(outFileName, ext);
        }
    }
    for (auto&& i : target)
    {

        if (Utils::iequal(i, name))
            return;
    }
    target.push_back(name);
}

/*-------------------------------------------------------------------------*/

int InsertExternalFile(const std::string& name)
{
    int index = name.find_last_of('#');
    if (index < 0)
        return 1;  // shouldn't process, this is a safety net.
    std::string fileToCompile = name.substr(0, index);
    std::string as = name.substr(index + 1);
    std::string stem = fileToCompile;
    if (!as.empty() && (Utils::HasExt(stem.c_str(), as.c_str()) ||
                        (IsCompilerSource(fileToCompile.c_str()) && IsCompilerSource(("a" + as).c_str()))))
    {
        index = stem.find_last_of('.');
        stem = stem.substr(0, index);
    }

    char buf[260];
    const char* p;

    if (!as.empty())
    {
        if (Utils::iequal(as, ".c"))
        {
            InsertFile(objlist, stem + ".ilo", as);
            return 1;
        }
        else if (Utils::iequal(as, ".rc"))
        {
            InsertFile(reslist, stem + ".res", as);
            InsertFile(rclist, fileToCompile, as);
            return 1;
        }
        else if (Utils::iequal(as, ".res"))
        {
            InsertFile(reslist, fileToCompile, as);
            return 1;
        }
        else if (Utils::iequal(as, ".ilo"))
        {
            InsertFile(objlist, fileToCompile, as);
            return 1;
        }
    }
    // no extenstion or an unknown one, just use the name intact (sans directory specification)
    index = fileToCompile.find_last_of("\\");
    if (index < 0)
        index = fileToCompile.find_last_of("/");
    if (index > 0)
        fileToCompile = fileToCompile.substr(index + 1);
    InsertFile(objlist, fileToCompile, as);

    return 0; /* compiler should process it*/
}

/*-------------------------------------------------------------------------*/

void InsertOutputFileName(const char* name) { Utils::StrCpy(outFileName, name); }

/*-------------------------------------------------------------------------*/
static std::list<std::string> objPosition;
void GetOutputFileName(char* name, int len, char* path, int len2, bool obj)
{
    if (obj)
    {
        char* p;
        if (objPosition.empty())
            objPosition = objlist;
        if (objPosition.empty())
            Utils::Fatal("Cannot get object file name");
        Utils::StrCpy(name, len, outFileName);
        p = (char*)strrchr(name, '\\');
        if (!p)
            p = name;
        else
            p++;
        Utils::StrCpy(p, len - (p - name), (char*)objPosition.front().c_str());
        Utils::StrCpy(path, len2, name);
    }
    else
    {
        path[0] = 0;
        Utils::StrCpy(name, len, outFileName);
        if (!objlist.empty() && name[0] && name[strlen(name) - 1] == '\\')
        {
            Utils::StrCat(name, len, objlist.front().c_str());
            Utils::StripExt(name);
            Utils::StrCat(name, len, ".exe");
            Utils::StrCpy(path, len2, outFileName);
        }
    }
}
void NextOutputFileName()
{
    if (!objPosition.empty())
        objPosition.pop_front();
}
int RunExternalFiles() { return 0; }
}  // namespace occmsil