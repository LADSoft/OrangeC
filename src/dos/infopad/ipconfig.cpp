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
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "ipconfig.h"
#include "xml.h"
std::string InfoPadCfg::GetFileName()
{
    char buf[MAX_PATH];
    strcpy(buf, __argv[0]);
    char *p = strrchr(buf, '\\');
    if (p)
        p++;
    else
        p = buf;
    strcpy(p, CFGFILE);
    return buf;
}
void InfoPadCfg::Load()
{
    std::string name = GetFileName();
    FILE *fil = fopen(name.c_str(), "r");
    if (fil)
    {
        xmlNode *node = xmlReadFile(fil);
        if (node)
        {
            if (!strcmp(node->elementType, "INFOPADCFG"))
            {
                node = node->children;
                while (node)
                {
                    xmlAttr *attribs = node->attribs;
                    if (!strcmp(node->elementType, "AUTOFORMAT"))
                    {
                        while (attribs)
                        {
                            if (!strcmp(attribs->name, "format"))
                            {
                                toAutoFormat = !!atoi(attribs->value);
                            }
                            else if (!strcmp(attribs->name, "indent"))
                            {
                                toAutoIndent = !!atoi(attribs->value);
                            }
                            else if (!strcmp(attribs->name, "colorize"))
                            {
                                toColorize = !!atoi(attribs->value);
                            }
                            attribs = attribs->next;
                        }
                    }
                    else if (!strcmp(node->elementType, "TABS"))
                    {
                        while (attribs)
                        {
                            if (!strcmp(attribs->name, "asSpaces"))
                            {
                                tabsAsSpaces = !!atoi(attribs->value);
                            }
                            else if (!strcmp(attribs->name, "indent"))
                            {
                                tabIndent = atoi(attribs->value);
                            }
                            attribs = attribs->next;
                        }
                    }
                    else if (!strcmp(node->elementType, "GENERAL"))
                    {
                        while (attribs)
                        {
                            if (!strcmp(attribs->name, "parenMatch"))
                            {
                                toMatchParenthesis = !!atoi(attribs->value);
                            }
                            else if (!strcmp(attribs->name, "backupFiles"))
                            {
                                toBackupFiles = !!atoi(attribs->value);
                            }
                            else if (!strcmp(attribs->name, "showLineNumbers"))
                            {
                                toShowLineNumbers = !!atoi(attribs->value);
                            }
                            attribs = attribs->next;
                        }
                    }
                    else if (!strcmp(node->elementType, "FILEMRU"))
                    {
                        xmlNode *child = node->children;
                        while (child)
                        {
                            attribs = child->attribs;
                            while (attribs)
                            {
                                if (!strcmp(attribs->name, "title"))
                                {
                                    fileMRU.Add(child->textData, attribs->value, true);
                                    break;
                                }
                                attribs = attribs->next;
                            }
                            child = child->next;
                        }
                    }
                    node = node->next;
                }
            }
            xmlFree(node);
        }
        fclose(fil);
    }
}
void InfoPadCfg::Save()
{
    std::string name = GetFileName();
    FILE *fil = fopen(name.c_str(), "w");
    if (fil)
    {
        fprintf(fil, "<INFOPADCFG>\n");
        fprintf(fil, "\t<AUTOFORMAT format=\"%d\" indent=\"%d\" colorize=\"%d\"/>\n", !!toAutoFormat, !!toAutoIndent, !!toColorize);
        fprintf(fil, "\t<TABS asSpaces=\"%d\" indent=\"%d\"/>\n", !!tabsAsSpaces, tabIndent);
        fprintf(fil, "\t<GENERAL parenMatch=\"%d\" backupFiles=\"%d\" showLineNumbers=\"%d\"/>\n", !!toMatchParenthesis, !!toBackupFiles, !!toShowLineNumbers);
        fprintf(fil, "\t<FILEMRU>\n");
        for (int i=0; i < MRU::MAX_MRU; i++)
        {
            MRUItem *p = fileMRU.Get(i);
            if (p)
            {
                fprintf(fil, "\t\t<ITEM title=\"%s\">%s</ITEM>\n", p->title, p->name);
            }
        }
        fprintf(fil, "\t</FILEMRU>\n");
        fprintf(fil, "</INFOPADCFG>\n");
        fclose(fil);
    }
}
