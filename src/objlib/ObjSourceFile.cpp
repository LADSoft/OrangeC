/* Software License Agreement
 * 
 *     Copyright(C) 1994-2024 David Lindauer, (LADSoft)
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

#include "ObjSourceFile.h"
#ifndef HAVE_UNISTD_H
#    include <io.h>
#    include <fcntl.h>
#endif
#include <ctime>
#include <sys/stat.h>
void ObjSourceFile::RetrieveFileTime(const ObjString& name)
{
#ifdef TARGET_OS_WINDOWS
    //    struct stat xxstat;
    //    stat(const_cast<char *>(name.c_str()), &xxstat);
    //	struct tm *tmx = localtime(&xxstat.st_mtime);
    //    if (tmx)
    //    	fileTime = *tmx;
    _finddata_t data;
    int handle = _findfirst(const_cast<char*>(name.c_str()), &data);
    if (handle != -1)
    {
        _findclose(handle);
        struct tm* tmx = localtime(&data.time_write);
        fileTime = *tmx;
    }
#endif
}
