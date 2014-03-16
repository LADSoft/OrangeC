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
#define STRICT
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <float.h>
#include "helpid.h"
#include "header.h"
//extern int __argc;
//extern char **__argv;
extern HWND hwndFrame;

typedef struct _list {
    struct _list *next;
    char *data;
} LIST;
LIST *nameList;
static LPSTR lpszSlotName = "\\\\.\\mailslot\\ocide_mailslot"; 
static HANDLE ghMailSlot;
static CRITICAL_SECTION critical;
static BOOL started;
int SendFileName(char *msg)
{
    DWORD cbWritten;
    HANDLE hFile = CreateFile(lpszSlotName,
        GENERIC_WRITE, 
        FILE_SHARE_READ, /* required to write to a mailslot */ 
        (LPSECURITY_ATTRIBUTES) NULL, 
        OPEN_EXISTING, 
        FILE_ATTRIBUTE_NORMAL, 
        (HANDLE) NULL); 
     
    if (hFile == INVALID_HANDLE_VALUE) { 
        return FALSE;
    } 
     
    WriteFile(hFile, 
        msg, 
        (DWORD) strlen(msg) + 1,
        &cbWritten, 
        (LPOVERLAPPED) NULL); 
     
    CloseHandle(hFile); 
    return TRUE; 
} 
void PassFilesToInstance(void)
{
    int argc = __argc;
    char **argv = __argv;
    AllowSetForegroundWindow(ASFW_ANY);
//	argv = CmdLineToC(&argc, GetCommandLineA());
    if (argv)
    {
        if (argc <= 1)
        {
            SendFileName("###TOP###");
        }
        else
        {
            int i;
            for (i=1; i < argc; i++)
            {
                char buf[260], *p = buf;
                strcpy(buf, argv[i]);
                while (isspace(*p)) p++;
                    if (*p != '/' && *p != '-')
                    {
                        abspath(p, NULL);
                        SendFileName(p);
                    }
            }
        }
    }
}
int RetrieveInstanceFile(DWINFO *info)
{
    int rv = 0;
    if (nameList)
    {
        LIST *l ;
        char *p;
        EnterCriticalSection(&critical);
        l = nameList;
        nameList = nameList->next;
        LeaveCriticalSection(&critical);
        if (started)
        {
            strncpy(info->dwName, l->data, 260);
            info->dwName[259] = 0;
            p = strrchr(info->dwName, '\\');
            if (!p)
                p = info->dwName;
            else
                p++;
            strcpy(info->dwTitle, p);		
            info->dwLineNo = -1;
            info->logMRU = FALSE;
            info->newFile = FALSE;
            rv = 1;
        }
        free(l->data);
        free(l);		
    }
    return rv;
}
int msThread(void *aa)
{
    HANDLE hMailSlot = (HANDLE)aa;
    InitializeCriticalSection(&critical);
    started = TRUE;
    while (TRUE)
    {
        char buffer[260], *name;
        DWORD cbRead;
        DWORD fResult = ReadFile(hMailSlot, 
            buffer, 
            sizeof(buffer), 
            &cbRead, 
            (LPOVERLAPPED) NULL); 

        if (!fResult) { 
            started = FALSE;
            DeleteCriticalSection(&critical);			
            return FALSE; 
        } 
        SetForegroundWindow(hwndFrame);
        if (strcmp(buffer,"###TOP###"))
        {
            name = strdup(buffer);
            if (name)
            {
                LIST *l = calloc(sizeof(LIST), 1);
                if (l)
                {
                    l->data = name;
                    EnterCriticalSection(&critical);
                    l->next = nameList;
                    nameList = l;	
                    LeaveCriticalSection(&critical);
                }
                else
                    free(name);
            }
        }
    }
    return TRUE; 
}
int StartInstanceComms(void)
{
    DWORD id;
    HANDLE hMailSlot = CreateMailslot(lpszSlotName, 
        0,                         /* no maximum message size         */ 
        MAILSLOT_WAIT_FOREVER,     /* no time-out for read operations */ 
        (LPSECURITY_ATTRIBUTES) NULL); /* no security attributes      */ 
 
    if (hMailSlot == INVALID_HANDLE_VALUE) { 
        return FALSE; 
    } 
    ghMailSlot = hMailSlot;
    _beginthread((BEGINTHREAD_FUNC)msThread, 0, (LPVOID)hMailSlot);
    
    return TRUE; 
}
void StopInstanceComms(void)
{
    CloseHandle(ghMailSlot);
}