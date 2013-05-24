/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2009, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    contact information:
        email: TouchStone222@runbox.com <David Lindauer>
*/
#include <windows.h>
#include <stdio.h>
int main(int argc, char **argv)
{
    HANDLE handle;
    WIN32_FIND_DATA blk;
    if (argc != 3)
    {
        printf("wrong number of args");
        return 1;
    }
    memset(&blk, 0, sizeof(blk));
    if ((handle = FindFirstFile(argv[1], &blk)) != INVALID_HANDLE_VALUE)
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
                strcpy(existingFile, argv[1]);
                p = strrchr(existingFile, '\\');
                if (p)
                    p++;
                else
                    p = existingFile;
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