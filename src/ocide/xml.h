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

#ifndef _XML_H
#define _XML_H

#define MAX_XML_TEXT_LEN 0x800
#define MAX_XML_NAME_LEN 0x80
struct xmlAttr
{
    struct xmlAttr* next;
    char* name;
    char* value;
};

struct xmlNode
{
    struct xmlNode* next;
    char* elementType;
    char* textData;
    struct xmlAttr* attribs;
    struct xmlNode* children;
    struct xmlNode* parent;
};

void xmlFree(struct xmlNode* Node);
struct xmlNode* xmlReadFile(FILE* in);
void xmlWriteFile(FILE* out, struct xmlNode* root);
char* xmlConvertString(char* s, int attr);

#endif  //_XML_H