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

// Software License Agreement
//
//     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
//
//     This file is part of the Orange C Compiler package.
//
//     The Orange C Compiler package is free software: you can redistribute it and/or modify
//     it under the terms of the GNU General Public License as published by
//     the Free Software Foundation, either version 3 of the License, or
//     (at your option) any later version, with the addition of the
//     Orange C "Target Code" exception.
//
//     The Orange C Compiler package is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU General Public License for more details.
//
//     You should have received a copy of the GNU General Public License
//     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
//
//     contact information:
//         email: TouchStone222@runbox.com <David Lindauer>
//
#include <stdio.h>
#include <windows.h>
void process(char* path)
{
    FILE* fil = fopen(path, "r");
    FILE* out = fopen("out.txt", "w");

    while (1)
    {
        char buf[1024];
        buf[0] = 0;
        fgets(buf, 1024, fil);
        if (!buf[0])
            break;
        if (strstr(buf, "#include"))
        {
            char* p = strchr(buf, '"');
            if (p)
            {
                while (*++p && *p != '"')
                    *p = tolower(*p);
            }
        }
        fputs(buf, out);
    }
    fclose(out);
    fclose(fil);
    unlink(path);
    MoveFile("out.txt", path);
}
void recurse(char* path)
{
    HANDLE hand;
    WIN32_FIND_DATA data;
    char root[MAX_PATH], search[MAX_PATH];
    strcpy(root, path);
    if (root[strlen(root) - 1] != '\\')
        strcat(root, "\\");
    sprintf(search, "%s*.c", root);
    hand = FindFirstFile(search, &data);
    if (hand != INVALID_HANDLE_VALUE)
    {
        do
        {
            sprintf(search, "%s%s", root, data.cFileName);
            if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if (data.cFileName[0] != '.' && stricmp(data.cFileName, "test"))
                {
                    recurse(search);
                }
            }
            else
            {
                process(search);
            }
        } while (FindNextFile(hand, &data));
        FindClose(hand);
    }
}
int main(int argc, char** argv)
{

    char buf[MAX_PATH];
    getcwd(buf, MAX_PATH);
    printf("%s\n", buf);
    recurse(buf);
}
