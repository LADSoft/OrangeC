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
#include <commctrl.h>
#include <stdio.h>
#include <float.h>
#include <string.h>
#include <stdlib.h>
#include <setjmp.h>
#include <stdarg.h>
#include <time.h>
#include <richedit.h>
#include "header.h"

static char *outputBuffer;
static int outputLength;
static int outputPos;

void WriteChar(char ch)
{
   if (outputPos >= outputLength)
   {
       outputBuffer = realloc(outputBuffer, outputLength *2);
       outputLength *= 2;
   }
   outputBuffer[outputPos++] = ch;
}
void WriteString(const char *fmt, ...)
{
    char buf[16000];
    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);
    char *p = buf;
    while (*p)
        WriteChar(*p++);   
}
//#define TEST
extern jmp_buf errjump;
void StringWToA(char* a, WCHAR* w, int l)
{
    while (l--)
        *a++ = (char)*w++;
    *a = 0;
}
static FILE* DoOpen(char* name, char* mode)
{
    FILE* rv;
#ifdef TEST
    char realname[256];
    char* p = strrchr(name, '\\');
    if (!p)
        p = strrchr(name, ':');
    if (p)
        p++;
    else
        p = name;
    sprintf(realname, ".\\temp\\%s", p);
    name = realname;
#endif
    rv = fopen(name, mode);
    if (!rv)
        fatal("Cannot open output file %s", name);
    return rv;
}
static void WriteCursorFile(RESOURCE* res)
{
    if (res->changed)
    {
        CURSORDATA* cursors = res->u.cursor->cursors;
        int n = res->u.cursor->count;
        int offset = 6 + n * 16;
        int val;
        int i;
        FILE* fil;
        for (i = 0; i < n; i++)
        {
            cursors[i].offset = offset;
            offset += cursors[i].bytes;
        }
        fil = DoOpen(res->filename, "wb");
        if (!fil)
            fatal("Could not open %s for write", res->filename);
        val = 0;  // filler
        fwrite(&val, 1, 2, fil);
        val = 2;  // this is a cursor file
        fwrite(&val, 1, 2, fil);
        val = n;  // number of objects
        fwrite(&val, 1, 2, fil);
        for (i = 0; i < n; i++)
        {
            char buf[16];
            buf[0] = cursors[i].width;
            buf[1] = cursors[i].height;
            buf[2] = cursors[i].colorcount;
            buf[3] = cursors[i].xtra;
            *(short*)(buf + 4) = cursors[i].xhotspot;
            *(short*)(buf + 6) = cursors[i].yhotspot;
            *(int*)(buf + 8) = cursors[i].bytes;
            *(int*)(buf + 12) = cursors[i].offset;
            fwrite(buf, 1, 16, fil);
        }
        for (i = 0; i < n; i++)
        {
            fwrite(cursors[i].data, 1, cursors[i].bytes, fil);
        }
        fclose(fil);
    }
}
static void WriteIconFile(RESOURCE* res)
{
    if (res->changed)
    {
        ICONDATA* icons = res->u.icon->icons;
        int n = res->u.cursor->count;
        int offset = 6 + n * 16;
        int val;
        int i;
        FILE* fil;
        for (i = 0; i < n; i++)
        {
            icons[i].offset = offset;
            offset += icons[i].bytes;
        }
        fil = DoOpen(res->filename, "wb");
        if (!fil)
            fatal("Could not open %s for write", res->filename);
        val = 0;  // filler
        fwrite(&val, 1, 2, fil);
        val = 1;  // this is an icon file
        fwrite(&val, 1, 2, fil);
        val = n;  // number of objects
        fwrite(&val, 1, 2, fil);
        for (i = 0; i < n; i++)
        {
            char buf[16];
            buf[0] = icons[i].width;
            buf[1] = icons[i].height;
            buf[2] = icons[i].colorcount;
            buf[3] = icons[i].xtra;
            *(short*)(buf + 4) = icons[i].planes;
            *(short*)(buf + 6) = icons[i].bits;
            *(int*)(buf + 8) = icons[i].bytes;
            *(int*)(buf + 12) = icons[i].offset;
            fwrite(buf, 1, 16, fil);
        }
        for (i = 0; i < n; i++)
        {
            fwrite(icons[i].data, 1, icons[i].bytes, fil);
        }
        fclose(fil);
    }
}
static void WriteBitmapFile(RESOURCE* res)
{
    if (res->changed)
    {
        BITMAP_* bd = res->u.bitmap;
        char data[14];
        FILE* fil;
        memset(data, 0, sizeof(data));
        data[0] = 'B';
        data[1] = 'M';
        *(int*)(data + 2) = 14 + bd->headerSize + bd->pixelSize;
        *(int*)(data + 10) = 14 + bd->headerSize;
        fil = DoOpen(res->filename, "wb");
        if (!fil)
            fatal("Could not open %s for write", res->filename);
        fwrite(data, 1, 14, fil);
        fwrite(bd->headerData, 1, bd->headerSize, fil);
        fwrite(bd->pixelData, 1, bd->pixelSize, fil);
        fclose(fil);
    }
}
static void WriteMessageTableFile(RESOURCE* res)
{
    if (res->changed)
    {
        FILE* fil = DoOpen(res->filename, "wb");
        if (!fil)
            fatal("Could not open %s for write", res->filename);
        fwrite(res->u.data.data, 1, res->u.data.length, fil);
        fclose(fil);
    }
}
static void WriteUserDataFile(RESOURCE* res)
{
    if (res->changed)
    {
        FILE* fil = DoOpen(res->filename, "wb");
        if (!fil)
            fatal("Could not open %s for write", res->filename);
        fwrite(res->u.rcdata->u.buffer.data, 1, res->u.rcdata->u.buffer.length, fil);
        fclose(fil);
    }
}
static void WriteFontFile(RESOURCE* res)
{
    if (res->changed)
    {
        FILE* fil = DoOpen(res->filename, "wb");
        if (!fil)
            fatal("Could not open %s for write", res->filename);
        fwrite(res->u.font->data, 1, res->u.font->length, fil);
        fclose(fil);
    }
}
static void WriteExpInternal(EXPRESSION* p, int n)
{
    // well -1 is the outer layer, well this tries to omit the outer parenthesis
    // but it isn't perfect
    if (n == -1)
        n = p->type;
    switch (p->type)
    {
        case hook:
            WriteChar('(');
            WriteExpInternal(p->left, hook);
            WriteString(" ? ");
            WriteExpInternal(p->right->left, hook);
            WriteString(" : ");
            WriteExpInternal(p->left->left, hook);
            WriteChar(')');
            break;
        case land:
            WriteExpInternal(p->left, land);
            WriteString(" && ");
            WriteExpInternal(p->right, land);
            break;
        case lor:
            if (n != lor)
                WriteChar('(');
            WriteExpInternal(p->left, lor);
            WriteString(" || ");
            WriteExpInternal(p->right, lor);
            if (n != lor)
                WriteChar(')');
            break;
        case or:
            if (n != or)
                WriteChar('(');
            WriteExpInternal(p->left, or);
            WriteString(" | ");
            WriteExpInternal(p->right, or);
            if (n != or)
                WriteChar(')');
            break;
        case uparrow:
            if (n != uparrow)
                WriteChar('(');
            WriteExpInternal(p->left, uparrow);
            WriteString(" ^ ");
            WriteExpInternal(p->right, uparrow);
            if (n != uparrow)
                WriteChar(')');
            break;
        case and:
            if (n != and)
                WriteChar('(');
            WriteExpInternal(p->left, and);
            WriteString(" & ");
            WriteExpInternal(p->right, and);
            if (n != and)
                WriteChar(')');
            break;
        case eq:
            if (n != eq)
                WriteChar('(');
            WriteExpInternal(p->left, eq);
            WriteString(" == ");
            WriteExpInternal(p->right, eq);
            if (n != eq)
                WriteChar(')');
            break;
        case neq:
            if (n != eq)
                WriteChar('(');
            WriteExpInternal(p->left, eq);
            WriteString(" != ");
            WriteExpInternal(p->right, eq);
            if (n != eq)
                WriteChar(')');
            break;
        case lt:
            if (n != lt)
                WriteChar('(');
            WriteExpInternal(p->left, lt);
            WriteString(" < ");
            WriteExpInternal(p->right, lt);
            if (n != lt)
                WriteChar(')');
            break;
        case gt:
            if (n != lt)
                WriteChar('(');
            WriteExpInternal(p->left, lt);
            WriteString(" > ");
            WriteExpInternal(p->right, lt);
            if (n != lt)
                WriteChar(')');
            break;
        case leq:
            if (n != lt)
                WriteChar('(');
            WriteExpInternal(p->left, lt);
            WriteString(" <= ");
            WriteExpInternal(p->right, lt);
            if (n != lt)
                WriteChar(')');
            break;
        case geq:
            if (n != lt)
                WriteChar('(');
            WriteExpInternal(p->left, lt);
            WriteString(" >= ");
            WriteExpInternal(p->right, lt);
            if (n != lt)
                WriteChar(')');
            break;
        case lshift:
            if (n != lshift)
                WriteChar('(');
            WriteExpInternal(p->left, lshift);
            WriteString(" << ");
            WriteExpInternal(p->right, lshift);
            if (n != lshift)
                WriteChar(')');
            break;
        case rshift:
            if (n != rshift)
                WriteChar('(');
            WriteExpInternal(p->left, rshift);
            WriteString(" >> ");
            WriteExpInternal(p->right, rshift);
            if (n != rshift)
                WriteChar(')');
            break;
        case plus:
            if (n != plus)
                WriteChar('(');
            WriteExpInternal(p->left, plus);
            WriteString(" + ");
            WriteExpInternal(p->right, plus);
            if (n != plus)
                WriteChar(')');
            break;
        case minus:
            if (p->right)
            {
                if (n != minus)
                    WriteChar('(');
                WriteExpInternal(p->left, plus);
                WriteString(" - ");
                WriteExpInternal(p->right, plus);
                if (n != plus)
                    WriteChar(')');
            }
            else
            {
                WriteChar('-');
                WriteExpInternal(p->left, minus);
            }
            break;
        case star:
            if (n != star)
                WriteChar('(');
            WriteExpInternal(p->left, star);
            WriteString(" * ");
            WriteExpInternal(p->right, star);
            if (n != star)
                WriteChar(')');
            break;
        case divide:
            if (n != star)
                WriteChar('(');
            WriteExpInternal(p->left, star);
            WriteString(" / ");
            WriteExpInternal(p->right, star);
            if (n != star)
                WriteChar(')');
            break;
        case modop:
            if (n != star)
                WriteChar('(');
            WriteExpInternal(p->left, star);
            WriteString(" %% ");
            WriteExpInternal(p->right, star);
            if (n != star)
                WriteChar(')');
            break;
        case not:
            WriteChar('!');
            WriteExpInternal(p->left, not);
            break;
        case kw_not:
            WriteString(" NOT ");
            WriteExpInternal(p->left, not);
            break;
        case compl:
            WriteChar('~');
            WriteExpInternal(p->left, compl);
            break;
        case e_int:
            if (p->rendition)
                WriteString("%s", p->rendition);
            else
                WriteString("%d", p->val);
            break;
        default:
            fatal("internal error");
            break;
    }
}
static void WriteExp(EXPRESSION* exp)
{
    if (!exp)
        WriteString("0");
    else
        WriteExpInternal(exp, -1);
}
static void WriteCommentLines(LIST* lines)
{
    while (lines)
    {
        WriteString("%s", lines->data);
        lines = lines->next;
    }
}
static void WriteAString(char* string, int len)
{
    WriteChar('\"');
    while (len--)
    {
        unsigned char ch = *string++;
        if (ch < 0x20)
        {
            WriteChar('\\');
            switch (ch)
            {
                case 0:
                    WriteChar('0');
                    break;
                case '\a':
                    WriteChar('a');
                    break;
                case '\b':
                    WriteChar('b');
                    break;
                case '\f':
                    WriteChar('f');
                    break;
                case '\n':
                    WriteChar('n');
                    break;
                case '\r':
                    WriteChar('r');
                    break;
                case '\t':
                    WriteChar('t');
                    break;
                default:
                    WriteString("x%02X", ch);
                    break;
            }
        }
        else
        {
            switch (ch)
            {
                case '\'':
                    WriteChar('\\');
                    WriteChar('\'');
                    break;
                case '\"':
                    WriteChar('\\');
                    WriteChar('\"');
                    break;
                case '\\':
                    WriteChar('\\');
                    WriteChar('\\');
                    break;
                default:
                    WriteChar(ch);
                    break;
            }
        }
    }
    WriteChar('\"');
}
static void WriteWString(WCHAR* str, int len)
{
    char buffer[1000];
    char* p = buffer;
    int ilen = len;
    while (ilen--)
        *p++ = (char)*str++;
    *p = 0;
    WriteAString(buffer, len);
}
static void WriteQuotedResId(IDENT* id)
{
    if (id->symbolic)
    {
        if (id->origName)
        {
            char buf[256];
            StringWToA(buf, id->origName, wcslen(id->origName));
            if (buf[0] && buf[strlen(buf) - 1] == '\n')
                buf[strlen(buf) - 1] = 0;
            WriteString("%s ", buf);
        }
        else
        {
            WriteWString(id->u.n.symbol, id->u.n.length);
        }
    }
    else
    {
        WriteExp(id->u.id);
        WriteChar(' ');
    }
}
static void WriteResId(IDENT* id)
{
    if (id->symbolic)
    {
        if (id->origName)
        {
            char buf[256];
            StringWToA(buf, id->origName, wcslen(id->origName));
            WriteString("%s ", buf);
        }
        else
        {
            char buf[256];
            StringWToA(buf, id->u.n.symbol, id->u.n.length);
            WriteString("%s ", buf);
        }
    }
    else
    {
        WriteExp(id->u.id);
        WriteChar(' ');
    }
}
static void WriteFileName(char* rel, char* name)
{
    char buf[MAX_PATH];
    strcpy(buf, relpath(name, rel));

#ifdef TEST
    char* p = strrchr(name, '\\');
    if (!p)
        p = strrchr(name, ':');
    if (p)
        p++;
    else
        p = name;
    name = p;
#endif
    WriteString("\"%s\"", buf);
}
static void WriteMemFlags(int flags)
{
    if (flags & MF_MOVEABLE)
        WriteString("MOVEABLE ");
    if (flags & MF_PURE)
        WriteString("PURE ");
    if (flags & MF_IMPURE)
        WriteString("IMPURE ");
    if (flags & MF_PRELOAD)
        WriteString("PRELOAD ");
    if (flags & MF_DISCARDABLE)
        WriteString("DISCARDABLE ");
    if (flags & MF_NONDISCARDABLE)
        WriteString("NONDISCARDABLE ");
}
static void WriteSecondaryCharacteristics(CHARACTERISTICS* info)
{
    if (info->language_low)
    {
        WriteString("LANGUAGE ");
        WriteExp(info->language_low);
        if (info->language_high)
        {
            WriteString(", ");
            WriteExp(info->language_high);
        }
        WriteString("\n");
    }
    if (info->version)
    {
        WriteString("VERSION ");
        WriteExp(info->version);
        WriteString("\n");
    }
    if (info->characteristics)
    {
        WriteString("CHARACTERISTICS ");
        WriteExp(info->characteristics);
        WriteString("\n");
    }
}
static void WriteMenuFlags(MENUITEM* item)
{
    if (item->flags & MI_GRAYED)
        WriteString("GRAYED ");
    if (item->flags & MI_INACTIVE)
        WriteString("INACTIVE ");
    if (item->flags & MI_CHECKED)
        WriteString("CHECKED ");
    if (item->flags & MI_MENUBARBREAK)
        WriteString("MENUBARBREAK ");
    if (item->flags & MI_MENUBREAK)
        WriteString("MENUBREAK ");
    if (item->flags & MI_HELP)
        WriteString("HELP ");
    if (item->flags & MI_SEPARATOR)
        WriteString("SEPARATOR ");
}
static void WriteLanguage(RESOURCE* res)
{
    WriteString("LANGUAGE ");
    WriteExp(res->info.language_low);
    WriteString(", ");
    WriteExp(res->info.language_high);
    WriteString("\n");
}
static void WriteUserData(char* rel, RESOURCE* res)
{
    WriteUserDataFile(res);
    WriteResId(&res->type);
    WriteMemFlags(res->info.memflags);
    WriteFileName(rel, res->filename);
    WriteString("\n");
}
static void WriteCursor(char* rel, RESOURCE* res)
{
    WriteCursorFile(res);
    WriteString("CURSOR ");
    WriteMemFlags(res->info.memflags);
    WriteFileName(rel, res->filename);
    WriteString("\n");
}
static void WriteBitmap(char* rel, RESOURCE* res)
{
    WriteBitmapFile(res);
    WriteString("BITMAP ");
    WriteMemFlags(res->info.memflags);
    WriteFileName(rel, res->filename);
    WriteString("\n");
}
static void WriteIcon(char* rel, RESOURCE* res)
{
    WriteIconFile(res);
    WriteString("ICON ");
    WriteMemFlags(res->info.memflags);
    WriteFileName(rel, res->filename);
    WriteString("\n");
}
static void WriteFont(char* rel, RESOURCE* res)
{
    WriteFontFile(res);
    WriteString("FONT ");
    WriteMemFlags(res->info.memflags);
    WriteFileName(rel, res->filename);
    WriteString("\n");
}
static void WriteMessageTable(char* rel, RESOURCE* res)
{
    WriteMessageTableFile(res);
    WriteString("MESSAGETABLE ");
    WriteMemFlags(res->info.memflags);
    WriteFileName(rel, res->filename);
    WriteString("\n");
}
static void WriteDLGInclude(char* rel, RESOURCE* res)
{
    WriteString("DLGINCLUDE ");
    WriteMemFlags(res->info.memflags);
    WriteFileName(rel, (char*)res->u.data.data);
    WriteString("\n");
}
static void WriteResString(RESOURCE* res)
{
    STRINGS* strings;
    WriteString("STRINGTABLE ");
    WriteMemFlags(res->info.memflags);
    WriteString("\n{\n");
    strings = res->u.stringtable;
    while (strings)
    {
        WriteString("\t");
        WriteExp(strings->id);
        WriteString(", ");
        WriteWString(strings->string, strings->length);
        WriteString("\n");
        strings = strings->next;
    }
    WriteString("}\n");
}
static void WriteAccelerator(RESOURCE* res)
{
    ACCELERATOR* acc;
    WriteString("ACCELERATORS ");
    WriteMemFlags(res->info.memflags);
    WriteString("\n");
    WriteSecondaryCharacteristics(&res->info);
    WriteString("BEGIN\n");
    acc = res->u.accelerator;
    while (acc)
    {
        WriteChar('\t');
        if (acc->key)
        {
            WriteExp(acc->key);
            WriteString(", ");
        }
        else if (acc->skey < 0x20)
        {
            WriteString("\"^%c\", ", acc->skey + 0x40);
        }
        else
        {
            WriteString("\"%c\", ", acc->skey);
        }
        WriteExp(acc->id);

        if (acc->flags)
        {
            WriteString(", ");
            if (acc->flags & ACC_VIRTKEY)
                WriteString("VIRTKEY ");
            else
                WriteString("ASCII ");
            if (acc->flags & ACC_NOINVERT)
                WriteString("NOINVERT ");
            if (acc->flags & ACC_SHIFT)
                WriteString("SHIFT ");
            if (acc->flags & ACC_CONTROL)
                WriteString("CONTROL ");
            if (acc->flags & ACC_ALT)
                WriteString("ALT ");
        }
        WriteString("\n");
        acc = acc->next;
    }
    WriteString("END\n");
}
static void WriteRCData(char* rel, RESOURCE* res)
{
    WriteString("RCDATA ");
    WriteMemFlags(res->info.memflags);
    if (res->filename)
    {
        WriteUserDataFile(res);
        WriteFileName(rel, res->filename);
        WriteString("\n");
    }
    else
    {
        RCDATA* data = res->u.rcdata;
        WriteString("\nBEGIN\n");
        while (data)
        {
            WriteString("\t");
            if (data->type == RCDATA_STRING)
            {
                WriteAString((char*)data->u.string.s, data->u.string.length);
            }
            else if (data->type == RCDATA_WORD)
            {
                WriteString("%d", data->u.word);
            }
            else if (data->type == RCDATA_DWORD)
            {
                WriteString("%dL", data->u.dword);
            }
            WriteString("\n");

            data = data->next;
        }
        WriteString("END\n");
    }
}
static void WriteVersion(RESOURCE* res)
{
    VERSIONINFO* v = res->u.versioninfo;
    struct variable* var = res->u.versioninfo->var;
    struct ver_stringinfo* strings;
    struct ver_varinfo* vars;
    struct ver_varlangchar* langchars;
    WriteString("VERSIONINFO\n");
    WriteString("FILEVERSION %d, %d, %d, %d\n", v->fixed->file_version_ms >> 16, v->fixed->file_version_ms & 0xffff,
            v->fixed->file_version_ls >> 16, v->fixed->file_version_ls & 0xffff);
    WriteString("PRODUCTVERSION %d, %d, %d, %d\n", v->fixed->product_version_ms >> 16,
            v->fixed->product_version_ms & 0xffff, v->fixed->product_version_ls >> 16, v->fixed->product_version_ls & 0xffff);
    if (v->fixed->file_flags_mask)
    {
        WriteString("FILEFLAGSMASK ");
        WriteExp(v->fixed->file_flags_mask);
        WriteString("\n");
    }
    if (v->fixed->file_flags)
    {
        WriteString("FILEFLAGS ");
        WriteExp(v->fixed->file_flags);
        WriteString("\n");
    }
    if (v->fixed->file_os)
    {
        WriteString("FILEOS ");
        WriteExp(v->fixed->file_os);
        WriteString("\n");
    }
    if (v->fixed->file_type)
    {
        WriteString("FILETYPE ");
        WriteExp(v->fixed->file_type);
        WriteString("\n");
    }
    if (v->fixed->file_subtype)
    {
        WriteString("FILESUBTYPE ");
        WriteExp(v->fixed->file_subtype);
        WriteString("\n");
    }
    WriteString("BEGIN\n");
    while (var)
    {
        switch (var->type)
        {
            case VERINFO_STRING:
                WriteString("\tBLOCK \"StringFileInfo\"\n");
                WriteString("\tBEGIN\n");
                WriteString("\t\tBLOCK ");
                WriteWString(var->u.string.language, wcslen(var->u.string.language));
                WriteString("\n\t\tBEGIN\n");
                strings = var->u.string.strings;
                while (strings)
                {
                    WriteString("\t\t\tVALUE ");
                    WriteWString(strings->key, wcslen(strings->key));
                    WriteString(", ");
                    WriteWString(strings->value, strings->length);
                    WriteString("\n");
                    strings = strings->next;
                }
                WriteString("\t\tEND\n");
                WriteString("\tEND\n");
                break;
            case VERINFO_VAR:
                WriteString("\tBLOCK \"VarFileInfo\"\n");
                WriteString("\tBEGIN\n");
                vars = var->u.var.var;
                while (vars)
                {
                    WriteString("\t\tVALUE ");
                    WriteWString(vars->key, wcslen(vars->key));
                    langchars = vars->intident;
                    while (langchars)
                    {
                        WriteString(", ");
                        WriteExp(langchars->language);
                        if (langchars->charset)
                        {
                            WriteString(", ");
                            WriteExp(langchars->charset);
                        }
                        langchars = langchars->next;
                    }
                    WriteString("\n");
                    vars = vars->next;
                }
                WriteString("\tEND\n");
                break;
        }
        var = var->next;
    }
    WriteString("END\n");
}
static void WriteIndent(int indent)
{
    int i;
    for (i = 0; i < indent; i++)
        WriteChar('\t');
}
static void WritePopup(MENUITEM* items, int extended, int indent)
{
    WriteIndent(indent++);
    WriteString("BEGIN\n");
    while (items)
    {
        WriteCommentLines(items->prevLines);
        if (items->popup)
        {
            WriteIndent(indent);
            WriteString("POPUP ");
        }
        else
        {
            WriteIndent(indent);
            WriteString("MENUITEM ");
        }
        if (items->text)
        {
            WriteWString(items->text, wcslen(items->text));
        }
        else if (extended)
        {
            WriteString("\"\"");
        }
        if (items->id)
        {
            WriteString(", ");
            WriteExp(items->id);
        }
        if (!extended)
        {
            if (items->flags)
            {
                if (items->text || items->id)
                    WriteString(", ");
                WriteMenuFlags(items);
            }
        }
        else
        {
            if (items->type)
            {
                if (!items->id)
                    WriteString(",");
                WriteString(", ");
                WriteExp(items->type);
            }
            if (items->state)
            {
                if (!items->id)
                    WriteString(",");
                if (!items->type)
                    WriteString(",");
                WriteString(", ");
                WriteExp(items->state);
            }
            if (items->help)
            {
                if (!items->id)
                    WriteString(",");
                if (!items->type)
                    WriteString(",");
                if (!items->state)
                    WriteString(",");
                WriteString(", ");
                WriteExp(items->help);
            }
        }
        WriteString("\n");
        if (items->popup)
        {
            WritePopup(items->popup, extended, indent);
        }
        items = items->next;
    }
    WriteIndent(--indent);
    WriteString("END\n");
}
static void WriteMenu(RESOURCE* res, BOOL extended)
{
    MENU* m = res->u.menu;
    if (extended)
    {
        WriteString("MENUEX ");
    }
    else
    {
        WriteString("MENU ");
    }
    WriteMemFlags(res->info.memflags);
    WriteString("\n");
    WriteSecondaryCharacteristics(&res->info);
    WritePopup(m->items, extended, 0);
}
static void WriteControl(CONTROL* control, int extended)
{
    int generic = control->generic;
    BOOL writeText = TRUE;
    WCHAR* class = NULL;
    WriteCommentLines(control->prevLines);
    if (!generic)
    {
        if (!control->class.symbolic)
        {
            switch (control->class.u.id->val)
            {
                case CTL_EDIT:
                    WriteString("EDITTEXT ");
                    writeText = FALSE;
                    break;
                case CTL_LISTBOX:
                    WriteString("LISTBOX ");
                    writeText = FALSE;
                    break;
                case CTL_SCROLLBAR:
                    WriteString("SCROLLBAR ");
                    writeText = FALSE;
                    break;
                case CTL_COMBOBOX:
                    WriteString("COMBOBOX ");
                    writeText = FALSE;
                    break;
                case CTL_BUTTON:
                    if (control->style->val & BS_AUTO3STATE)
                    {
                        WriteString("AUTO3STATE ");
                    }
                    else if (control->style->val & BS_AUTOCHECKBOX)
                    {
                        WriteString("AUTOCHECKBOX ");
                    }
                    else if (control->style->val & BS_AUTORADIOBUTTON)
                    {
                        WriteString("AUTORADIOBUTTON ");
                    }
                    else if (control->style->val & BS_CHECKBOX)
                    {
                        WriteString("CHECKBOX ");
                    }
                    else if (control->style->val & BS_DEFPUSHBUTTON)
                    {
                        WriteString("DEFPUSHBUTTON ");
                    }
                    else if (control->style->val & BS_GROUPBOX)
                    {
                        WriteString("GROUPBOX ");
                    }
                    else if (control->style->val & BS_PUSHBUTTON)
                    {
                        WriteString("PUSHBUTTON ");
                    }
                    else if (control->style->val & BS_RADIOBUTTON)
                    {
                        WriteString("RADIOBUTTON ");
                    }
                    else if (control->style->val & BS_3STATE)
                    {
                        WriteString("STATE3 ");
                    }
                    else
                    {
                        class = L"button";
                        generic = TRUE;
                    }
                    break;
                case CTL_STATIC:
                    if (control->style->val & SS_ICON)
                    {
                        WriteString("ICON ");
                    }
                    else if (control->style->val & SS_CENTER)
                    {
                        WriteString("TEXT ");
                    }
                    else if (control->style->val & SS_RIGHT)
                    {
                        WriteString("RTEXT ");
                    }
                    else if (control->style->val & SS_LEFT)
                    {
                        WriteString("LTEXT ");
                    }
                    else
                    {
                        class = L"static";
                        generic = TRUE;
                    }
                    break;
                default:
                    generic = TRUE;
                    break;
            }
        }
        else
        {
            switch (control->class.u.id->val)
            {
                case CTL_EDIT:
                    class = L"edit";
                    break;
                case CTL_LISTBOX:
                    class = L"listbox";
                    break;
                case CTL_SCROLLBAR:
                    class = L"scrollbar";
                    break;
                case CTL_COMBOBOX:
                    class = L"combobox";
                    break;
                case CTL_BUTTON:
                    class = L"button";
                    break;
                case CTL_STATIC:
                    class = L"static";
                    break;
                default:
                    generic = TRUE;
                    break;
            }
            generic = TRUE;
        }
    }
    else
    {
        if (!control->class.symbolic)
        {
            switch (control->class.u.id->val)
            {
                case CTL_EDIT:
                    class = L"edit";
                    break;
                case CTL_LISTBOX:
                    class = L"listbox";
                    break;
                case CTL_SCROLLBAR:
                    class = L"scrollbar";
                    break;
                case CTL_COMBOBOX:
                    class = L"combobox";
                    break;
                case CTL_BUTTON:
                    class = L"button";
                    break;
                case CTL_STATIC:
                    class = L"static";
                    break;
            }
        }
    }
    if (generic)
    {
        WriteString("CONTROL ");
    }
    if ((writeText && control->text) || generic)
    {
        if (control->text)
            WriteQuotedResId(control->text);
        else
            WriteString("\"\"");
        WriteString(", ");
    }
    WriteExp(control->id);
    WriteString(", ");
    if (generic)
    {
        if (class)
            WriteWString(class, wcslen(class));
        else
            WriteQuotedResId(&control->class);
        WriteString(", ");
        WriteExp(control->style);
        WriteString(", ");
    }
    WriteExp(control->x);
    WriteString(", ");
    WriteExp(control->y);
    WriteString(", ");
    WriteExp(control->width);
    WriteString(", ");
    WriteExp(control->height);
    if (!generic && control->style)
    {
        WriteString(", ");
        WriteExp(control->style);
    }
    if (extended && control->exstyle)
    {
        WriteString(", ");
        WriteExp(control->exstyle);
    }
    if (control->help)
    {
        WriteString(", ");
        WriteExp(control->help);
    }
    WriteChar('\n');
}
static void WriteDialogSettings(DIALOG* dlg, CHARACTERISTICS* info, BOOL extended)
{
    if (info->language_low)
    {
        WriteString("LANGUAGE ");
        WriteExp(info->language_low);
        if (info->language_high)
        {
            WriteString(", ");
            WriteExp(info->language_high);
        }
        WriteString("\n");
    }
    if (info->version)
    {
        WriteString("VERSION ");
        WriteExp(info->version);
        WriteString("\n");
    }
    if (info->characteristics)
    {
        WriteString("CHARACTERISTICS ");
        WriteExp(info->characteristics);
        WriteString("\n");
    }
    if (dlg->style)
    {
        WriteString("STYLE ");
        WriteExp(dlg->style);
        WriteString("\n");
    }
    if (extended && dlg->exstyle)
    {
        WriteString("EXSTYLE ");
        WriteExp(dlg->exstyle);
        WriteString("\n");
    }
    if (dlg->menu)
    {
        WriteString("MENU ");
        WriteResId(dlg->menu);
        WriteString("\n");
    }
    if (dlg->pointsize)
    {
        WriteString("FONT ");
        WriteExp(dlg->pointsize);
        WriteString(", ");
        WriteWString(dlg->font, wcslen(dlg->font));
        if (extended)
        {
            if (dlg->ex.weight || dlg->ex.italic || dlg->ex.charset)
            {
                WriteString(", ");
                WriteExp(dlg->ex.weight);
            }
            if (dlg->ex.italic || dlg->ex.charset)
            {
                WriteString(", ");
                WriteExp(dlg->ex.italic);
            }
            if (dlg->ex.charset)
            {
                WriteString(", ");
                WriteExp(dlg->ex.charset);
            }
        }
        WriteString("\n");
    }
    if (dlg->caption)
    {
        WriteString("CAPTION ");
        WriteWString(dlg->caption, wcslen(dlg->caption));
        WriteString("\n");
    }
    if (dlg->class)
    {
        WriteString("CLASS ");
        WriteQuotedResId(dlg->class);
        WriteString("\n");
    }
    if (dlg->ex.help)
    {
        WriteString("HELP ");
        WriteExp(dlg->ex.help);
        WriteString("\n");
    }
}
static void WriteDialog(RESOURCE* res, BOOL extended)
{
    DIALOG* d = res->u.dialog;
    CONTROL* c = d->controls;
    if (extended)
    {
        WriteString("DIALOGEX ");
    }
    else
    {
        WriteString("DIALOG ");
    }
    if (res->info.memflags)
    {
        WriteMemFlags(res->info.memflags);
        WriteString(", ");
    }
    WriteExp(d->x);
    WriteString(", ");
    WriteExp(d->y);
    WriteString(", ");
    WriteExp(d->width);
    WriteString(", ");
    WriteExp(d->height);
    WriteString("\n");
    WriteDialogSettings(d, &res->info, extended);
    WriteString("BEGIN\n");
    while (c)
    {
        WriteControl(c, extended);
        c = c->next;
    }
    WriteString("END\n");
}
static void WriteRes(char* rel, RESOURCE* res)
{
    WriteCommentLines(res->prevLines);

    if (res->itype == RESTYPE_PLACEHOLDER)
        return;

    if (res->type.symbolic || (res->itype != RESTYPE_STRING && res->itype != RESTYPE_LANGUAGE))
        WriteResId(&res->id);
    if (res->type.symbolic)
    {
        WriteUserData(rel, res);
    }
    else
    {
        switch (res->itype)
        {
            case RESTYPE_LANGUAGE:
                WriteLanguage(res);
                break;
            case RESTYPE_CURSOR:
                WriteCursor(rel, res);
                break;
            case RESTYPE_BITMAP:
                WriteBitmap(rel, res);
                break;
            case RESTYPE_ICON:
                WriteIcon(rel, res);
                break;
            case RESTYPE_MENU:
                WriteMenu(res, res->extended);
                break;
            case RESTYPE_DIALOG:
                WriteDialog(res, res->extended);
                break;
            case RESTYPE_STRING:
                WriteResString(res);
                break;
            case RESTYPE_FONT:
                WriteFont(rel, res);
                break;
            case RESTYPE_ACCELERATOR:
                WriteAccelerator(res);
                break;
            case RESTYPE_RCDATA:
                WriteRCData(rel, res);
                break;
            case RESTYPE_MESSAGETABLE:
                WriteMessageTable(rel, res);
                break;
            case RESTYPE_VERSION:
                WriteVersion(res);
                break;
            case RESTYPE_DLGINCLUDE:
                WriteDLGInclude(rel, res);
                break;
            default:
                WriteUserData(rel, res);
                break;
        }
    }
}
static void MarkDef(RESOURCE_DATA* select, EXPRESSION* id)
{
    if (id && id->rendition)
    {
        SYM* s = search(id->rendition, &select->syms);
        if (s)
            s->marked = TRUE;
    }
}
static void MarkAcceleratorDefinitions(RESOURCE_DATA* select, ACCELERATOR* accel)
{
    while (accel)
    {
        MarkDef(select, accel->id);
        accel = accel->next;
    }
}
static void MarkStringDefinitions(RESOURCE_DATA* select, STRINGS* strings)
{
    while (strings)
    {
        MarkDef(select, strings->id);
        strings = strings->next;
    }
}
static void MarkControlDefinitions(RESOURCE_DATA* select, DIALOG* dlg)
{
    CONTROL* controls = dlg->controls;
    while (controls)
    {
        MarkDef(select, controls->id);
        controls = controls->next;
    }
}
static void MarkMenuDefinitions(RESOURCE_DATA* select, MENUITEM* items)
{
    while (items)
    {
        MarkDef(select, items->id);
        if (items->popup)
            MarkMenuDefinitions(select, items->popup);
        items = items->next;
    }
}
static void MarkNewDefinitions(RESOURCE_DATA* select)
{
    RESOURCE* res;
    for (res = select->resources; res; res = res->next)
    {
        if (!res->id.symbolic && res->id.u.id)
            MarkDef(select, res->id.u.id);
        switch (res->itype)
        {
            case RESTYPE_ACCELERATOR:
                MarkAcceleratorDefinitions(select, res->u.accelerator);
                break;
            case RESTYPE_STRING:
                MarkStringDefinitions(select, res->u.stringtable);
                break;
            case RESTYPE_DIALOG:
                MarkControlDefinitions(select, res->u.dialog);
                break;
            case RESTYPE_MENU:
                MarkMenuDefinitions(select, res->u.menu->items);
                break;
            default:
                break;
        }
    }
}
static void WriteDefinitionSet(SYM* definitions, BOOL weed)
{
    while (definitions)
    {
        if (!weed || definitions->marked)
        {
            char buf[2048];
            int i;
            DEFSTRUCT* d = (DEFSTRUCT*)definitions->value.s;
            WriteCommentLines(definitions->lines);
            WriteString("#define ");
            WriteString("%s", definitions->name);
            if (d->argcount)
            {
                WriteChar('(');
                for (i = 0; i < d->argcount; i++)
                {
                    StringWToA(buf, d->args[i], wcslen(d->args[i]));
                    WriteString("%s", buf);
                    if (i + 1 < d->argcount)
                        WriteString(", ");
                }
                WriteChar(')');
            }
            WriteChar(' ');
            StringWToA(buf, d->string, wcslen(d->string));
            WriteString("%s", buf);
            WriteChar('\n');
        }
        definitions = definitions->xref;
    }
}
static void WriteDefines(RESOURCE_DATA* select)
{
    SYM* definitions;
    WriteString("#ifdef __IDE_RC_INVOKED\n");
    WriteString("#define __NEXT_CONTROL_ID\t%d\n", select->nextControlId);
    WriteString("#define __NEXT_MENU_ID\t\t%d\n", select->nextMenuId);
    WriteString("#define __NEXT_RESOURCE_ID\t%d\n", select->nextResourceId);
    WriteString("#define __NEXT_STRING_ID\t%d\n", select->nextStringId);
    WriteString("#endif\n");
    definitions = select->newDefinitions;
    while (definitions)
    {
        definitions->marked = FALSE;
        definitions = definitions->xref;
    }
    MarkNewDefinitions(select);
    WriteDefinitionSet(select->headerDefinitions, FALSE);
    WriteDefinitionSet(select->newDefinitions, TRUE);
}
void WriteResources(char* fileName, RESOURCE_DATA* select)
{
    RESOURCE* res = select->resources;
    if (setjmp(errjump))
        return;
    while (res)
    {
#ifdef TEST
        res->changed = TRUE;
#else
        if (res->changed)
            break;
#endif
        res = res->next;
    }
#ifndef TEST
    if (res)
#endif
    {
        outputPos = 0;
        outputLength = 200;
        outputBuffer = (char *)malloc(outputLength);
        for (res = select->resources; res; res = res->next)
        {
            if (!res->deleted)
                WriteRes(fileName, res);
        }
        WriteChar(0);
        if (!WriteFileData(fileName, outputBuffer, select->fileFlags))
            fatal("Could not write file");
        free(outputBuffer);
        if (select->resourceIdFile)
        {
            outputPos = 0;
            outputLength = 200;
            outputBuffer = (char *)malloc(outputLength);
            WriteDefines(select);
            WriteChar(0);
            if (!WriteFileData(select->resourceIdFile, outputBuffer, select->fileFlags))
                fatal("Could not write file");
            free(outputBuffer);
        }
    }
}