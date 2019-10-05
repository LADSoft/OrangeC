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
 
#include <windows.h>
#include <stdio.h>
int main(int argc, char **argv)
{
    char filename[MAX_PATH], *f;
    HANDLE handle;
    WIN32_FIND_DATA blk;
    if (argc != 3)
    {
        printf("wrong number of args");
        return 1;
    }
    strcpy(filename, argv[1]);
    while ((f = strchr(filename, '%')))
           *f = '*';
    memset(&blk, 0, sizeof(blk));
    if ((handle = FindFirstFile(filename, &blk)) != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (!(blk.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                char newFile[MAX_PATH];
                char existingFile[MAX_PATH], *p;
                strcpy(newFile, argv[2]);
                if (newFile[strlen(newFile)-1] != '\\')
                    strcat(newFile, "\\");
                strcat(newFile, blk.cFileName);
                strcpy(existingFile, filename);
                p = strrchr(existingFile, '\\');
                if (p)
                    p++;
                else
                    p = existingFile ;
                strcpy(p, blk.cFileName);
                printf("copying %s to %s\n", existingFile, newFile);
                CopyFile(existingFile, newFile, FALSE);
            }
        } while (FindNextFile(handle, &blk));
        FindClose(handle);
        printf("Ok");
        return 0;
    }
    printf("No files");
    return 1;
}