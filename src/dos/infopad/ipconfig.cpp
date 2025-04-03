/* Software License Agreement
 * 
 *     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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
