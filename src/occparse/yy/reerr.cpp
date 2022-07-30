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

#include <stdio.h>
#include <string.h>
#include <ctype.h>
extern int a;
#define BUFSIZE 256

void process(char* name)
{
    FILE* fil = fopen(name, "r");
    int count = 0;
    if (!fil)
        printf("cannot open %s\n", name);
    else
    {
        printf("\n");
        while (!feof(fil))
        {
            char buf[BUFSIZE], *p, data[BUFSIZE], *q;
            buf[0] = 0;
            fgets(buf, BUFSIZE, fil);
            p = buf;
            while ((p = strstr(p, "ERR_")))
            {
                q = data;
                while (isalnum(*p) || *p == '_')
                    *q++ = *p++;
                *q = 0;
                printf("#define %s %d\n", data, count++);
            }
        }
        fclose(fil);
    }
}
int main(int argc, char** argv)
{
    int i;
    for (i = 1; i < argc; i++)
        process(argv[i]);
    return 0;
}
