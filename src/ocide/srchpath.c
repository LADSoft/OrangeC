/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2012, David Lindauer, (LADSoft).
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
/*
 * Search for a file along a path list
 */
#include <stdio.h>
#include <string.h>
/*
 * Pull the next path off the path search list
 */
static char *parsepath(char *path, char *buffer)
{
    char *pos = path;

    /* Quit if hit a ';' */
    while (*pos)
    {
        if (*pos == ';')
        {
            pos++;
            break;
        }
        *buffer++ =  *pos++;
    }
    *buffer = 0;

    /* Return a null pointer if no more data */
    if (*pos)
        return (pos);

    return (0);
}

/*
 * For each library:
 * Search local directory and all directories in the search path
 *  until it is found or run out of directories
 */
FILE *searchPathI(char *string, char *searchPath, char *mode)
{
    FILE *in;
    char *newpath = searchPath;

    /* Search local path */
    in = fopen((char*)string, mode);
    if (in)
    {
        return (in);
    }
    else
    {
        /* If no path specified we search along the search path */
        if (string[0] != '\\' && string[1] != ':')
        {
            char buffer[200];
            while (newpath)
            {
                /* Create a file name along this path */
                newpath = parsepath(newpath, buffer);
                if (buffer[strlen(buffer) - 1] != '\\')
                    strcat(buffer, "\\");
                strcat(buffer, (char*)string);

                /* Check this path */
                in = fopen(buffer, mode);
                if (in)
                {
                    strcpy(string, buffer);
                    return (in);
                }
            }
        }
    }
    return (0);
}
