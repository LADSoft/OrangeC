/* Software License Agreement
 *
 *     Copyright(C) 1994-2022 David Lindauer, (LADSoft)
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

#include "PipeArbitrator.h"
#include "Utils.h"

#ifdef HAVE_UNISTD_H
#    include <unistd.h>
#else
#    include <io.h>
extern "C" char* getcwd(char*, int);
#endif

PipeArbitrator::~PipeArbitrator()
{
    if (main >= 3)
        close(main);
}

void PipeArbitrator::Init(const std::string& pipeName)
{
    if (pipeName.size())
    {
        int fds;
        if (Utils::NamedPipe(&fds, pipeName))
        {
            main = fds;
        }
    }
}

int PipeArbitrator::OpenFile(const std::string& fileName)
{
    if (main >= 0)
    {
        Utils::PipeWrite(main, fileName);
        std::string val = Utils::PipeRead(main);
        int fds;
        if (Utils::NamedPipe(&fds, val))
        {
            return fds;
        }
    }
    return -1;
}
