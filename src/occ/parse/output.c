/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
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

*/
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "compiler.h"

extern FILE *outputFile;

void oinit(void)
{
}

/*-------------------------------------------------------------------------*/

void oflush(FILE *file)
{
    fflush(file);
    oinit();
}

/*-------------------------------------------------------------------------*/

void oputc(int ch, FILE *file)
{

    if (file)
        fputc(ch,file);
}

void beputc(int ch)
{
        fputc(ch,outputFile);
}

/*-------------------------------------------------------------------------*/
void owrite(char *buf, size_t size, int n, FILE *fil)
{
    if (fil)
        fwrite(buf,size,n,fil);
}
void beWrite(char *buf, size_t size)
{
    fwrite(buf, 1, size, outputFile);
}
/*-------------------------------------------------------------------------*/
void oprintf(FILE *file, char *format, ...)
{
    if (file) {
        va_list arg;
        va_start(arg, format);
        vfprintf(file, format, arg);
        va_end(arg);
    }
}
void bePrintf(char *format, ...)
{
        va_list arg;
        va_start(arg, format);
        vfprintf(outputFile, format, arg);
        va_end(arg);
}
