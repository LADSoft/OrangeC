/* Software License Agreement
 *
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
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
    if (mainRead >= 3)
        close(mainRead);
    if (mainWrite >= 3)
        close(mainWrite);
}

void PipeArbitrator::Init(const std::string& pipeName)
{
    if (pipeName.size())
    {
        int fds[2];
        if (Utils::NamedPipe(fds, pipeName))
        {
            mainRead = fds[0];
            mainWrite = fds[1];
        }
    }
}

int PipeArbitrator::OpenFile(const std::string& fileName)
{
    if (mainRead >= 0)
    {
        Utils::PipeWrite(mainWrite, fileName);
        std::string val = Utils::PipeRead(mainRead);
        int fds[2];
        if (Utils::NamedPipe(fds, val))
        {
            close(fds[1]);
            return fds[0];
        }
    }
    return -1;
}
