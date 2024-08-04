/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2024 David Lindauer, (LADSoft)
 *  
 *      This file is part of the Orange C Compiler package.
 *  
 *      The Orange C Compiler package is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *  
 *      The Orange C Compiler package is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *  
 *      You should have received a copy of the GNU General Public License
 *      along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *  
 *      contact information:
 *          email: TouchStone222@runbox.com <David Lindauer>
 *  
 */

#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys\stat.h>

char* _RTL_FUNC mktemp(char* __template)
{
    char* s = strstr(__template, "XXXXXX");
    if (s && s == __template + strlen(__template) - 6)
    {
        memcpy(s, "AA.AAA", 6);
        while (1)
        {
            int fil = open(__template, O_RDONLY);
            if (fil == -1)
            {
                return __template;
            }
            close(fil);
            if (++s[5] > 'Z')
            {
                s[5] = 'A';
                if (++s[4] > 'Z')
                {
                    s[4] = 'A';
                    if (++s[3] > 'Z')
                    {
                        s[3] = 'A';
                        if (++s[1] > 'Z')
                        {
                            s[1] = 'A';
                            if (++s[0] > 'Z')
                                return 0; /* man that is a lot of temp files if they get here */
                        }
                    }
                }
            }
        }
    }
    return 0;
}
char* _RTL_FUNC _mktemp(char* __template) { return mktemp(__template); }
int _RTL_FUNC mkstemp(char* __template)
{
    if (mktemp(__template) == NULL)
        return -1;
    return open(__template, O_RDWR | O_CREAT | O_EXCL, S_IWRITE);
}
int _RTL_FUNC _mkstemp(char* __template) { return mkstemp(__template); }
