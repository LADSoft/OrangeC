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
#include <commctrl.h>
#include <stdio.h>
#include <float.h>
#include <string.h>
#include <stdlib.h>
#include <setjmp.h>
#include <time.h>
#include "header.h"

//#define TEST
extern jmp_buf errjump;
void StringWToA(char *a, WCHAR *w, int l)
{
    while (l--)
        *a++ = (char) *w++;
    *a = 0;
}
static FILE * DoOpen(char *name, char *mode)
{
    FILE *rv;
#ifdef TEST
    char realname[256];
    char *p = strrchr(name, '\\');
    if (!p)
        p = strrchr(name, ':');
    if (p)
        p ++;
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
static void WriteCursorFile(RESOURCE *res)
{
    if (res->changed)
    {
        CURSORDATA *cursors = res->u.cursor->cursors;
        int n = res->u.cursor->count;
        int offset = 6 + n*16;
        int val;
        int i;
        FILE *fil;
        for (i=0 ; i < n; i++)
        {
            cursors[i].offset = offset;
            offset += cursors[i].bytes;
        }
        fil = DoOpen(res->filename, "wb");
        if (!fil)
            fatal("Could not open %s for write" , res->filename);
        val = 0; // filler
        fwrite(&val, 1, 2, fil);
        val = 2; // this is a cursor file
        fwrite(&val, 1, 2, fil); 
        val = n; // number of objects
        fwrite(&val, 1, 2, fil);
        for (i=0 ; i < n; i++)
        {
            char buf[16];
            buf[0] = cursors[i].width;
            buf[1] = cursors[i].height;
            buf[2] = cursors[i].colorcount;
            buf[3] = cursors[i].xtra;
            *(short *)(buf + 4) = cursors[i].xhotspot;
            *(short *)(buf + 6) = cursors[i].yhotspot;
            *(int *)(buf + 8) = cursors[i].bytes;
            *(int *)(buf + 12) = cursors[i].offset;
            fwrite(buf, 1, 16, fil);
        }
        for (i=0 ; i < n; i++)
        {
            fwrite(cursors[i].data, 1, cursors[i].bytes, fil);
        }
        fclose(fil);
    }
}
static void WriteIconFile(RESOURCE *res)
{   
    if (res->changed)
    {
        ICONDATA *icons = res->u.icon->icons;
        int n = res->u.cursor->count;
        int offset = 6 + n*16;
        int val;
        int i;
        FILE *fil;
        for (i=0 ; i < n; i++)
        {
            icons[i].offset = offset;
            offset += icons[i].bytes;
        }
        fil = DoOpen(res->filename, "wb");
        if (!fil)
            fatal("Could not open %s for write" , res->filename);
        val = 0; // filler
        fwrite(&val, 1, 2, fil);
        val = 1; // this is an icon file
        fwrite(&val, 1, 2, fil); 
        val = n; // number of objects
        fwrite(&val, 1, 2, fil);
        for (i=0 ; i < n; i++)
        {
            char buf[16];
            buf[0] = icons[i].width;
            buf[1] = icons[i].height;
            buf[2] = icons[i].colorcount;
            buf[3] = icons[i].xtra;
            *(short *)(buf + 4) = icons[i].planes;
            *(short *)(buf + 6) = icons[i].bits;
            *(int *)(buf + 8) = icons[i].bytes;
            *(int *)(buf + 12) = icons[i].offset;
            fwrite(buf, 1, 16, fil);
        }
        for (i=0 ; i < n; i++)
        {
            fwrite(icons[i].data, 1, icons[i].bytes, fil);
        }
        fclose(fil);
    }
}
static void WriteBitmapFile(RESOURCE *res)
{
    if (res->changed)
    {
        BITMAP_ *bd = res->u.bitmap;
        char data[14];
        FILE *fil;
        memset(data, 0, sizeof(data));
        data[0] = 'B';
        data[1] = 'M';
        *(int *)(data + 2) = 14 + bd->headerSize + bd->pixelSize;
        *(int *)(data + 10) = 14 + bd->headerSize;
        fil = DoOpen(res->filename, "wb");
        if (!fil)
            fatal("Could not open %s for write" , res->filename);
        fwrite(data, 1, 14, fil);
        fwrite(bd->headerData, 1, bd->headerSize, fil);
        fwrite(bd->pixelData, 1, bd->pixelSize, fil);
        fclose(fil);
    }
}
static void WriteMessageTableFile(RESOURCE *res)
{
    if (res->changed)
    {
        FILE *fil = DoOpen(res->filename, "wb");
        if (!fil)
            fatal("Could not open %s for write" , res->filename);
        fwrite(res->u.data.data, 1, res->u.data.length, fil);
        fclose(fil);
    }
}
static void WriteUserDataFile(RESOURCE *res)
{
    if (res->changed)
    {
        FILE *fil = DoOpen(res->filename, "wb");
        if (!fil)
            fatal("Could not open %s for write" , res->filename);
        fwrite(res->u.rcdata->u.buffer.data, 1, res->u.rcdata->u.buffer.length, fil);
        fclose(fil);
    }
}
static void WriteFontFile(RESOURCE *res)
{
    if (res->changed)
    {
        FILE *fil = DoOpen(res->filename, "wb");
        if (!fil)
            fatal("Could not open %s for write" , res->filename);
        fwrite(res->u.font->data, 1, res->u.font->length, fil);
        fclose(fil);
    }
}
static void WriteExpInternal(FILE *outputFile, EXPRESSION *p, int n)
{
    // well -1 is the outer layer, well this tries to omit the outer parenthesis
    // but it isn't perfect
    if (n == -1)
        n = p->type;
    switch(p->type)
    {
        case hook:
            fputc('(', outputFile);
            WriteExpInternal(outputFile, p->left, hook);
            fprintf(outputFile, " ? ");
            WriteExpInternal(outputFile, p->right->left, hook);
            fprintf(outputFile, " : ");
            WriteExpInternal(outputFile, p->left->left, hook);
            fputc(')', outputFile);
            break;            
        case land:
            WriteExpInternal(outputFile, p->left, land);
            fprintf(outputFile, " && ");            
            WriteExpInternal(outputFile, p->right, land);
            break;
        case lor:
            if (n != lor)
                fputc('(', outputFile);
            WriteExpInternal(outputFile, p->left, lor);
            fprintf(outputFile, " || ");            
            WriteExpInternal(outputFile, p->right, lor);
            if (n != lor)
                fputc(')', outputFile);
            break;
        case or:
            if (n != or)
                fputc('(', outputFile);
            WriteExpInternal(outputFile, p->left, or);
            fprintf(outputFile, " | ");            
            WriteExpInternal(outputFile, p->right, or);
            if (n != or)
                fputc(')', outputFile);
            break;
        case uparrow:
            if (n != uparrow)
                fputc('(', outputFile);
            WriteExpInternal(outputFile, p->left, uparrow);
            fprintf(outputFile, " ^ ");            
            WriteExpInternal(outputFile, p->right, uparrow);
            if (n != uparrow)
                fputc(')', outputFile);
            break;
        case and:
            if (n != and)
                fputc('(', outputFile);
            WriteExpInternal(outputFile, p->left, and);
            fprintf(outputFile, " & ");            
            WriteExpInternal(outputFile, p->right, and);
            if (n != and)
                fputc(')', outputFile);
            break;
        case eq:
            if (n != eq)
                fputc('(', outputFile);
            WriteExpInternal(outputFile, p->left, eq);
            fprintf(outputFile, " == ");            
            WriteExpInternal(outputFile, p->right, eq);
            if (n != eq)
                fputc(')', outputFile);
            break;
        case neq:
            if (n != eq)
                fputc('(', outputFile);
            WriteExpInternal(outputFile, p->left, eq);
            fprintf(outputFile, " != ");            
            WriteExpInternal(outputFile, p->right, eq);
            if (n != eq)
                fputc(')', outputFile);
            break;
        case lt:
            if (n != lt)
                fputc('(', outputFile);
            WriteExpInternal(outputFile, p->left, lt);
            fprintf(outputFile, " < ");            
            WriteExpInternal(outputFile, p->right, lt);
            if (n != lt)
                fputc(')', outputFile);
            break;
        case gt:
            if (n != lt)
                fputc('(', outputFile);
            WriteExpInternal(outputFile, p->left, lt);
            fprintf(outputFile, " > ");            
            WriteExpInternal(outputFile, p->right, lt);
            if (n != lt)
                fputc(')', outputFile);
            break;
        case leq:
            if (n != lt)
                fputc('(', outputFile);
            WriteExpInternal(outputFile, p->left, lt);
            fprintf(outputFile, " <= ");            
            WriteExpInternal(outputFile, p->right, lt);
            if (n != lt)
                fputc(')', outputFile);
            break;
        case geq:
            if (n != lt)
                fputc('(', outputFile);
            WriteExpInternal(outputFile, p->left, lt);
            fprintf(outputFile, " >= ");            
            WriteExpInternal(outputFile, p->right, lt);
            if (n != lt)
                fputc(')', outputFile);
            break;
        case lshift:
            if (n != lshift)
                fputc('(', outputFile);
            WriteExpInternal(outputFile, p->left, lshift);
            fprintf(outputFile, " << ");            
            WriteExpInternal(outputFile, p->right, lshift);
            if (n != lshift)
                fputc(')', outputFile);
            break;
        case rshift:
            if (n != rshift)
                fputc('(', outputFile);
            WriteExpInternal(outputFile, p->left, rshift);
            fprintf(outputFile, " >> ");            
            WriteExpInternal(outputFile, p->right, rshift);
            if (n != rshift)
                fputc(')', outputFile);
            break;
        case plus:
            if (n != plus)
                fputc('(', outputFile);
            WriteExpInternal(outputFile, p->left, plus);
            fprintf(outputFile, " + ");            
            WriteExpInternal(outputFile, p->right, plus);
            if (n != plus)
                fputc(')', outputFile);
            break;
        case minus:
            if (p->right)
            {
                if (n != minus)
                    fputc('(', outputFile);
                WriteExpInternal(outputFile, p->left, plus);
                fprintf(outputFile, " - ");            
                WriteExpInternal(outputFile, p->right, plus);
                if (n != plus)
                    fputc(')', outputFile);
            }
            else
            {
                fputc('-', outputFile);
                WriteExpInternal(outputFile, p->left, minus);
            }
            break;
        case star:
            if (n != star)
                fputc('(', outputFile);
            WriteExpInternal(outputFile, p->left, star);
            fprintf(outputFile, " * ");            
            WriteExpInternal(outputFile, p->right, star);
            if (n != star)
                fputc(')', outputFile);
            break;
        case divide:
            if (n != star)
                fputc('(', outputFile);
            WriteExpInternal(outputFile, p->left, star);
            fprintf(outputFile, " / ");            
            WriteExpInternal(outputFile, p->right, star);
            if (n != star)
                fputc(')', outputFile);
            break;
        case modop:
            if (n != star)
                fputc('(', outputFile);
            WriteExpInternal(outputFile, p->left, star);
            fprintf(outputFile, " %% ");            
            WriteExpInternal(outputFile, p->right, star);
            if (n != star)
                fputc(')', outputFile);
            break;
        case not:
            fputc('!', outputFile);
            WriteExpInternal(outputFile, p->left, not);
            break;
        case kw_not:
            fprintf(outputFile, " NOT ");
            WriteExpInternal(outputFile, p->left, not);
            break;
        case compl:
            fputc('~', outputFile);
            WriteExpInternal(outputFile, p->left, compl);
            break;
        case e_int:
            if (p->rendition)
                fprintf(outputFile,"%s", p->rendition);
            else
                fprintf(outputFile,"%d", p->val);
            break;
        default:
            fatal("internal error");
            break;
    }
}
static void WriteExp(FILE *outputFile, EXPRESSION *exp)
{
    if (!exp)
        fprintf(outputFile, "0");
    else
        WriteExpInternal(outputFile, exp, -1);
}
static void WriteCommentLines(FILE *outputFile, LIST *lines)
{
    while (lines)
    {
        fprintf(outputFile, "%s", lines->data);
        lines = lines->next;
    }
}
static void WriteAString(FILE *outputFile, char *string, int len)
{
    fputc('\"', outputFile);
    while (len--)
    {
        unsigned char ch = *string++;
        if (ch < 0x20)
        {
            fputc('\\', outputFile);
            switch(ch)
            {
                case 0:
                    fputc('0', outputFile);
                    break;
                case '\a':
                    fputc('a', outputFile);
                    break;
                case '\b':
                    fputc('b', outputFile);
                    break;
                case '\f':
                    fputc('f', outputFile);
                    break;
                case '\n':
                    fputc('n', outputFile);
                    break;
                case '\r':
                    fputc('r', outputFile);
                    break;
                case '\t':
                    fputc('t', outputFile);
                    break;
                default:
                    fprintf(outputFile, "x%02X", ch);
                    break;
            }
        }
        else
        {
            switch(ch)
            {
                case '\'':
                    fputc('\\', outputFile);
                    fputc('\'', outputFile);
                    break;
                case '\"':
                    fputc('\\', outputFile);
                    fputc('\"', outputFile);
                    break;
                case '\\':
                    fputc('\\', outputFile);
                    fputc('\\', outputFile);
                    break;
                default:
                    fputc(ch, outputFile);
                    break;
            }
        }
    }        
    fputc('\"', outputFile);
}
static void WriteWString(FILE *outputFile, WCHAR *str, int len)
{
    char buffer[1000];
    char *p = buffer;
    int ilen = len;
    while (ilen--)
        *p++ = (char)*str++;
    *p = 0;
    WriteAString(outputFile, buffer, len);
}
static void WriteQuotedResId(FILE *outputFile, IDENT *id)
{
    if (id->symbolic)
    {
        if (id->origName)
        {
            char buf[256];
            StringWToA(buf, id->origName, wcslen(id->origName));
            if (buf[0] && buf[strlen(buf)-1] == '\n')
                buf[strlen(buf)-1] = 0;
            fprintf(outputFile, "%s ", buf);
        }
        else
        {
            WriteWString(outputFile, id->u.n.symbol, id->u.n.length);
        }
    }
    else
    {
        WriteExp(outputFile, id->u.id);
        fputc(' ', outputFile);
    }
}
static void WriteResId(FILE *outputFile, IDENT *id)
{
    if (id->symbolic)
    {
        if (id->origName)
        {
            char buf[256];
            StringWToA(buf, id->origName, wcslen(id->origName));
            fprintf(outputFile, "%s ", buf);
        }
        else
        {
                char buf[256];
                StringWToA(buf, id->u.n.symbol, id->u.n.length);
                fprintf(outputFile, "%s ", buf);
        }
    }
    else
    {
        WriteExp(outputFile, id->u.id);
        fputc(' ', outputFile);
    }
}
static void WriteFileName(FILE *outputFile, char *rel, char *name)
{
    char buf[MAX_PATH];
    strcpy(buf, relpath(name, rel));
    
#ifdef TEST
    char *p = strrchr(name , '\\');
    if (!p)
        p = strrchr(name, ':');
    if (p)
        p ++;
    else 
        p = name;
    name = p;
#endif
    fprintf(outputFile, "\"%s\"", buf);
}
static void WriteMemFlags(FILE *outputFile, int flags)
{
    if (flags & MF_MOVEABLE)
        fprintf(outputFile, "MOVEABLE ");
    if (flags & MF_PURE)
        fprintf(outputFile, "PURE ");
    if (flags & MF_IMPURE)
        fprintf(outputFile, "IMPURE ");
    if (flags & MF_PRELOAD)
        fprintf(outputFile, "PRELOAD ");
    if (flags & MF_DISCARDABLE)
        fprintf(outputFile, "DISCARDABLE ");
    if (flags & MF_NONDISCARDABLE)
        fprintf(outputFile, "NONDISCARDABLE ");
}
static void WriteSecondaryCharacteristics(FILE *outputFile, CHARACTERISTICS *info)
{
    if (info->language_low)
    {
        fprintf(outputFile, "LANGUAGE ");
        WriteExp(outputFile, info->language_low);
        if (info->language_high)
        {
            fprintf(outputFile, ", ");
            WriteExp(outputFile, info->language_high);
        }
        fprintf(outputFile, "\n");
    }
    if (info->version)
    {
        fprintf(outputFile, "VERSION ");
        WriteExp(outputFile, info->version);
        fprintf(outputFile, "\n");
    }
    if (info->characteristics)
    {
        fprintf(outputFile, "CHARACTERISTICS ");
        WriteExp(outputFile, info->characteristics);
        fprintf(outputFile, "\n");
    }
}
static void WriteMenuFlags(FILE *outputFile, MENUITEM *item)
{
    if (item->flags & MI_GRAYED)
        fprintf(outputFile, "GRAYED ");
    if (item->flags & MI_INACTIVE)
        fprintf(outputFile, "INACTIVE ");
    if (item->flags & MI_CHECKED)
        fprintf(outputFile, "CHECKED ");
    if (item->flags & MI_MENUBARBREAK)
        fprintf(outputFile, "MENUBARBREAK ");
    if (item->flags & MI_MENUBREAK)
        fprintf(outputFile, "MENUBREAK ");
    if (item->flags & MI_HELP)
        fprintf(outputFile, "HELP ");
    if (item->flags & MI_SEPARATOR)
        fprintf(outputFile, "SEPARATOR ");
}
static void WriteLanguage(FILE *outputFile, RESOURCE *res)
{
    fprintf(outputFile, "LANGUAGE ");
    WriteExp(outputFile, res->info.language_low);
    fprintf(outputFile, ", ");
    WriteExp(outputFile, res->info.language_high);
    fprintf(outputFile, "\n");
}
static void WriteUserData(FILE *outputFile, char *rel, RESOURCE *res)
{
    WriteUserDataFile(res);
    WriteResId(outputFile, &res->type);
    WriteMemFlags(outputFile, res->info.memflags);
    WriteFileName(outputFile, rel, res->filename);
    fprintf(outputFile, "\n");
}
static void WriteCursor(FILE *outputFile, char *rel, RESOURCE *res)
{
    WriteCursorFile(res);
    fprintf(outputFile, "CURSOR ");
    WriteMemFlags(outputFile, res->info.memflags);
    WriteFileName(outputFile, rel, res->filename);
    fprintf(outputFile, "\n");
}
static void WriteBitmap(FILE *outputFile, char *rel, RESOURCE *res)
{
    WriteBitmapFile(res);
    fprintf(outputFile, "BITMAP ");
    WriteMemFlags(outputFile, res->info.memflags);
    WriteFileName(outputFile, rel, res->filename);
    fprintf(outputFile, "\n");
}
static void WriteIcon(FILE *outputFile, char *rel, RESOURCE *res)
{
    WriteIconFile(res);
    fprintf(outputFile, "ICON ");
    WriteMemFlags(outputFile, res->info.memflags);
    WriteFileName(outputFile, rel, res->filename);
    fprintf(outputFile, "\n");
}
static void WriteFont(FILE *outputFile, char *rel, RESOURCE *res)
{
    WriteFontFile(res);
    fprintf(outputFile, "FONT ");
    WriteMemFlags(outputFile, res->info.memflags);
    WriteFileName(outputFile, rel, res->filename);
    fprintf(outputFile, "\n");
}
static void WriteMessageTable(FILE *outputFile, char *rel, RESOURCE *res)
{
    WriteMessageTableFile(res);
    fprintf(outputFile, "MESSAGETABLE ");
    WriteMemFlags(outputFile, res->info.memflags);
    WriteFileName(outputFile, rel, res->filename);
    fprintf(outputFile, "\n");
}
static void WriteDLGInclude(FILE *outputFile, char *rel, RESOURCE *res)
{
    fprintf(outputFile, "DLGINCLUDE ");
    WriteMemFlags(outputFile, res->info.memflags);
    WriteFileName(outputFile, rel, (char *)res->u.data.data);
    fprintf(outputFile, "\n");
}
static void WriteString(FILE *outputFile, RESOURCE *res)
{
    STRINGS *strings;
    fprintf(outputFile, "STRINGTABLE ");
    WriteMemFlags(outputFile, res->info.memflags);
    fprintf(outputFile, "\n{\n");
    strings = res->u.stringtable;
    while (strings)
    {
        fprintf(outputFile, "\t");
        WriteExp(outputFile, strings->id);
        fprintf(outputFile, ", ");
        WriteWString(outputFile, strings->string, strings->length);
        fprintf(outputFile, "\n");
        strings = strings->next;
    }
    fprintf(outputFile, "}\n");
}
static void WriteAccelerator(FILE *outputFile, RESOURCE *res)
{
    ACCELERATOR *acc;
    fprintf(outputFile, "ACCELERATORS ");
    WriteMemFlags(outputFile, res->info.memflags);
    fprintf(outputFile, "\n");
    WriteSecondaryCharacteristics(outputFile, &res->info);
    fprintf(outputFile, "BEGIN\n");
    acc = res->u.accelerator;
    while (acc)
    {
        fputc('\t', outputFile);
        if (acc->key)
        {
            WriteExp(outputFile, acc->key);
            fprintf(outputFile, ", ");
        }
        else if (acc->skey < 0x20)
        {
            fprintf(outputFile, "\"^%c\", ", acc->skey + 0x40);
        }
        else
        {
            fprintf(outputFile, "\"%c\", ", acc->skey);
        }
        WriteExp(outputFile, acc->id);
        
        if (acc->flags)
        {
            fprintf(outputFile, ", ");
            if (acc->flags & ACC_VIRTKEY)
                fprintf(outputFile, "VIRTKEY ");
            else
                fprintf(outputFile, "ASCII ");
            if (acc->flags & ACC_NOINVERT)
                fprintf(outputFile, "NOINVERT ");
            if (acc->flags & ACC_SHIFT)
                fprintf(outputFile, "SHIFT ");
            if (acc->flags & ACC_CONTROL)
                fprintf(outputFile, "CONTROL ");
            if (acc->flags & ACC_ALT)
                fprintf(outputFile, "ALT ");
        }        
        fprintf(outputFile, "\n");
        acc = acc->next;
        
    }
    fprintf(outputFile, "END\n");
}
static void WriteRCData(FILE *outputFile, char *rel, RESOURCE *res)
{
    fprintf(outputFile, "RCDATA ");
    WriteMemFlags(outputFile, res->info.memflags);
    if (res->filename)
    {
        WriteUserDataFile(res);
        WriteFileName(outputFile, rel, res->filename);
        fprintf(outputFile, "\n");
    }
    else
    {
        RCDATA *data = res->u.rcdata;
        fprintf(outputFile, "\nBEGIN\n");
        while (data)
        {
            fprintf(outputFile, "\t");
            if (data->type == RCDATA_STRING)
            {
                WriteAString(outputFile, (char *)data->u.string.s, data->u.string.length);
            }
            else if (data->type == RCDATA_WORD)
            {
                fprintf(outputFile, "%d", data->u.word);
            }
            else if (data->type == RCDATA_DWORD)
            {
                fprintf(outputFile, "%dL", data->u.dword);
            }
            fprintf(outputFile, "\n");
                
            data = data->next;
        }
        fprintf(outputFile, "END\n");    
    }
}
static void WriteVersion(FILE *outputFile, RESOURCE *res)
{
    VERSIONINFO *v= res->u.versioninfo;
    struct variable *var = res->u.versioninfo->var;
    struct ver_stringinfo *strings;
    struct ver_varinfo *vars;
    struct ver_varlangchar *langchars;
    fprintf(outputFile, "VERSIONINFO\n");
    fprintf(outputFile, "FILEVERSION %d, %d, %d, %d\n",v->fixed->file_version_ms >> 16,
            v->fixed->file_version_ms &0xffff,v->fixed->file_version_ls >> 16,
            v->fixed->file_version_ls &0xffff);
    fprintf(outputFile, "PRODUCTVERSION %d, %d, %d, %d\n",v->fixed->product_version_ms >> 16,
            v->fixed->product_version_ms &0xffff,v->fixed->product_version_ls >> 16,
            v->fixed->product_version_ls &0xffff);
    if (v->fixed->file_flags_mask)
    {
        fprintf(outputFile, "FILEFLAGSMASK ");
        WriteExp(outputFile, v->fixed->file_flags_mask);
        fprintf(outputFile, "\n");
    }
    if (v->fixed->file_flags)
    {
        fprintf(outputFile, "FILEFLAGS");
        WriteExp(outputFile, v->fixed->file_flags);
        fprintf(outputFile, "\n");
    }
    if (v->fixed->file_os)
    {
        fprintf(outputFile, "FILEOS ");
        WriteExp(outputFile, v->fixed->file_os);
        fprintf(outputFile, "\n");
    }
    if (v->fixed->file_type)
    {
        fprintf(outputFile, "FILETYPE ");
        WriteExp(outputFile, v->fixed->file_type);
        fprintf(outputFile, "\n");
    }
    if (v->fixed->file_subtype)
    {
        fprintf(outputFile, "FILESUBTYPE ");
        WriteExp(outputFile, v->fixed->file_subtype);
        fprintf(outputFile, "\n");
    }
    fprintf(outputFile, "BEGIN\n");
    while (var)
    {
        switch(var->type)
        {
            case VERINFO_STRING:
                fprintf(outputFile, "\tBLOCK \"StringFileInfo\"\n");
                fprintf(outputFile, "\tBEGIN\n");
                fprintf(outputFile, "\t\tBLOCK ");
                WriteWString(outputFile, var->u.string.language, wcslen(var->u.string.language));
                fprintf(outputFile, "\n\t\tBEGIN\n");
                strings = var->u.string.strings;
                while (strings)
                {
                    fprintf(outputFile, "\t\t\tVALUE ");
                    WriteWString(outputFile, strings->key, wcslen(strings->key));
                    fprintf(outputFile, ", ");
                    WriteWString(outputFile, strings->value, strings->length);
                    fprintf(outputFile, "\n");
                    strings = strings->next;
                }
                fprintf(outputFile, "\t\tEND\n");
                fprintf(outputFile, "\tEND\n");
                break;
            case VERINFO_VAR:
                fprintf(outputFile, "\tBLOCK \"VarFileInfo\"\n");
                fprintf(outputFile, "\tBEGIN\n");
                vars = var->u.var.var;
                while (vars)
                {
                    fprintf(outputFile, "\t\tVALUE ");
                    WriteWString(outputFile, vars->key, wcslen(vars->key));
                    langchars = vars->intident;
                    while (langchars)
                    {
                        fprintf(outputFile, ", ");
                        WriteExp(outputFile, langchars->language);
                        if (langchars->charset)
                        {
                            fprintf(outputFile, ", ");
                            WriteExp(outputFile, langchars->charset);
                        }
                        langchars = langchars->next;
                    }   
                    fprintf(outputFile, "\n");
                    vars = vars->next;
                }
                fprintf(outputFile, "\tEND\n");
                break;
        }
        var = var->next;
    }
    fprintf(outputFile, "END\n");
}
static void WriteIndent(FILE *outputFile, int indent)
{
    int i;
    for (i=0; i < indent; i++)
        fputc('\t', outputFile);
}
static void WritePopup(FILE *outputFile, MENUITEM *items, int extended, int indent)
{
    WriteIndent(outputFile, indent++);
    fprintf(outputFile, "BEGIN\n");
    while (items)
    {
        WriteCommentLines(outputFile, items->prevLines);
        if (items->popup)
        {
            WriteIndent(outputFile, indent);
            fprintf(outputFile, "POPUP ");
        }
        else
        {
            WriteIndent(outputFile, indent);
            fprintf(outputFile, "MENUITEM ");
        }
        if (items->text)
        {
            WriteWString(outputFile, items->text, wcslen(items->text));
        }
        else if (extended)
        {
            fprintf(outputFile, "\"\"");
        }
        if (items->id)
        {
            fprintf(outputFile, ", ");
            WriteExp(outputFile, items->id);
        }
        if (!extended)
        {
            if (items->flags)
            {
                if (items->text || items->id)
                    fprintf(outputFile, ", ");
                WriteMenuFlags(outputFile, items);
            }
        }
        else
        {
            if (items->type)
            {
                if (!items->id)
                    fprintf(outputFile, ",");
                fprintf(outputFile, ", ");
                WriteExp(outputFile, items->type);
            }
            if (items->state)
            {
                if (!items->id)
                    fprintf(outputFile, ",");
                if (!items->type)
                    fprintf(outputFile, ",");
                fprintf(outputFile, ", ");
                WriteExp(outputFile, items->state);
            }
            if (items->help)
            {
                if (!items->id)
                    fprintf(outputFile, ",");
                if (!items->type)
                    fprintf(outputFile, ",");
                if (!items->state)
                    fprintf(outputFile, ",");
                fprintf(outputFile, ", ");
                WriteExp(outputFile, items->help);
            }
        }
        fprintf(outputFile, "\n");
        if (items->popup)
        {
            WritePopup(outputFile, items->popup, extended, indent);
        }
        items = items->next;
    }
    WriteIndent(outputFile, --indent);
    fprintf(outputFile, "END\n");
}
static void WriteMenu(FILE *outputFile, RESOURCE *res)
{
    MENU *m = res->u.menu;
    if (m->extended)
    {
        fprintf(outputFile, "MENUEX ");
    }
    else
    {
        fprintf(outputFile, "MENU ");
    }   
    WriteMemFlags(outputFile, res->info.memflags);
    fprintf(outputFile, "\n");
    WriteSecondaryCharacteristics(outputFile, &res->info);
    WritePopup(outputFile, m->items, m->extended, 0);
}
static void WriteControl(FILE *outputFile, CONTROL *control, int extended)
{
    int generic = control->generic;
    BOOL writeText = TRUE;
    WCHAR *class = NULL;
    WriteCommentLines(outputFile, control->prevLines);
    if (!generic)
    {
        if (!control->class.symbolic)
        {
            switch(control->class.u.id->val)
            {
                case CTL_EDIT:
                    fprintf(outputFile, "EDITTEXT ");
                    writeText = FALSE;
                    break;
                case CTL_LISTBOX:
                    fprintf(outputFile, "LISTBOX ");
                    writeText = FALSE;
                    break;
                case CTL_SCROLLBAR:
                    fprintf(outputFile, "SCROLLBAR ");
                    writeText = FALSE;
                    break;
                case CTL_COMBOBOX:
                    fprintf(outputFile, "COMBOBOX ");
                    writeText = FALSE;
                    break;
                case CTL_BUTTON:
                    if (control->style->val & BS_AUTO3STATE)
                    {
                        fprintf(outputFile, "AUTO3STATE ");
                    }
                    else if (control->style->val & BS_AUTOCHECKBOX)
                    {
                        fprintf(outputFile, "AUTOCHECKBOX ");
                    }
                    else if (control->style->val & BS_AUTORADIOBUTTON)
                    {
                        fprintf(outputFile, "AUTORADIOBUTTON ");
                    }
                    else if (control->style->val & BS_CHECKBOX)
                    {
                        fprintf(outputFile, "CHECKBOX ");
                    }
                    else if (control->style->val & BS_DEFPUSHBUTTON)
                    {
                        fprintf(outputFile, "DEFPUSHBUTTON ");
                    }
                    else if (control->style->val & BS_GROUPBOX)
                    {
                        fprintf(outputFile, "GROUPBOX ");
                    }
                    else if (control->style->val & BS_PUSHBUTTON)
                    {
                        fprintf(outputFile, "PUSHBUTTON ");
                    }
                    else if (control->style->val & BS_RADIOBUTTON)
                    {
                        fprintf(outputFile, "RADIOBUTTON ");
                    }
                    else if (control->style->val & BS_3STATE)
                    {
                        fprintf(outputFile, "STATE3 ");
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
                        fprintf(outputFile, "ICON ");
                    }
                    else if (control->style->val & SS_CENTER)
                    {
                        fprintf(outputFile, "TEXT ");
                    }
                    else if (control->style->val & SS_RIGHT)
                    {
                        fprintf(outputFile, "RTEXT ");
                    }
                    else if (control->style->val & SS_LEFT)
                    {
                        fprintf(outputFile, "LTEXT ");
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
            switch(control->class.u.id->val)
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
            switch(control->class.u.id->val)
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
        fprintf(outputFile, "CONTROL ");
        
    }
    if ((writeText && control->text) || generic)
    {
		if (control->text)
	        WriteQuotedResId(outputFile, control->text);
		else
			fprintf(outputFile, "\"\"");
        fprintf(outputFile, ", ");
    }
    WriteExp(outputFile, control->id);
    fprintf(outputFile, ", ");
    if (generic)
    {
        if (class)
            WriteWString(outputFile, class, wcslen(class));
        else
            WriteQuotedResId(outputFile, &control->class);
        fprintf(outputFile, ", ");
        WriteExp(outputFile, control->style);
        fprintf(outputFile, ", ");
    }
    WriteExp(outputFile, control->x);
    fprintf(outputFile, ", ");
    WriteExp(outputFile, control->y);
    fprintf(outputFile, ", ");
    WriteExp(outputFile, control->width);
    fprintf(outputFile, ", ");
    WriteExp(outputFile, control->height);
    if (!generic && control->style)
    {
        fprintf(outputFile, ", ");
        WriteExp(outputFile, control->style);
    }
    if (control->exstyle)
    {
        fprintf(outputFile, ", ");
        WriteExp(outputFile, control->exstyle);
    }
    if (control->help)
    {
        fprintf(outputFile, ", ");
        WriteExp(outputFile, control->help);
    }
    fputc('\n', outputFile);    
}
static void WriteDialogSettings(FILE *outputFile, DIALOG *dlg, CHARACTERISTICS *info)
{
    if (info->language_low)
    {
        fprintf(outputFile, "LANGUAGE ");
        WriteExp(outputFile, info->language_low);
        if (info->language_high)
        {
            fprintf(outputFile, ", ");
            WriteExp(outputFile, info->language_high);
        }       
        fprintf(outputFile, "\n");
    }
    if (info->version)
    {
        fprintf(outputFile, "VERSION ");
        WriteExp(outputFile, info->version);
        fprintf(outputFile, "\n");
    }
    if (info->characteristics)
    {
        fprintf(outputFile, "CHARACTERISTICS ");
        WriteExp(outputFile, info->characteristics);
        fprintf(outputFile, "\n");
    }
    if (dlg->style)
    {
        fprintf(outputFile, "STYLE ");
        WriteExp(outputFile, dlg->style);
        fprintf(outputFile, "\n");
    }
    if (dlg->exstyle)
    {
        fprintf(outputFile, "EXSTYLE ");
        WriteExp(outputFile, dlg->exstyle);
        fprintf(outputFile, "\n");
    }
    if (dlg->menu)
    {
        fprintf(outputFile, "MENU ");
        WriteResId(outputFile, dlg->menu);
        fprintf(outputFile, "\n");
    }
    if (dlg->pointsize)
    {
        fprintf(outputFile, "FONT ");
        WriteExp(outputFile, dlg->pointsize);
        fprintf(outputFile, ", ");
        WriteWString(outputFile, dlg->font, wcslen(dlg->font));
        if (dlg->ex.weight || dlg->ex.italic || dlg->ex.charset)
        {
            fprintf(outputFile, ", ");
            WriteExp(outputFile, dlg->ex.weight);
        }
        if (dlg->ex.italic || dlg->ex.charset)
        {
            fprintf(outputFile, ", ");
            WriteExp(outputFile, dlg->ex.italic);
        }
        if (dlg->ex.charset)
        {
            fprintf(outputFile, ", ");
            WriteExp(outputFile, dlg->ex.charset);
        }
        fprintf(outputFile, "\n");
    }
    if (dlg->caption)
    {
        fprintf(outputFile, "CAPTION ");
        WriteWString(outputFile, dlg->caption, wcslen(dlg->caption));
        fprintf(outputFile, "\n");
    }
    if (dlg->class)
    {
        fprintf(outputFile, "CLASS ");
        WriteQuotedResId(outputFile, dlg->class);
        fprintf(outputFile, "\n");
    }
    if (dlg->ex.help)
    {
        fprintf(outputFile, "HELP ");
        WriteExp(outputFile, dlg->ex.help);
        fprintf(outputFile, "\n");
    }
}
static void WriteDialog(FILE *outputFile, RESOURCE *res)
{
    DIALOG *d = res->u.dialog;
    CONTROL *c = d->controls;
    if (d->ex.extended)
    {
        fprintf(outputFile, "DIALOGEX ");
    }
    else
    {
        fprintf(outputFile, "DIALOG ");
    }
    if (res->info.memflags)
    {
        WriteMemFlags(outputFile, res->info.memflags);
        fprintf(outputFile, ", ");
    }
    WriteExp(outputFile, d->x);
    fprintf(outputFile, ", ");
    WriteExp(outputFile, d->y);
    fprintf(outputFile, ", ");
    WriteExp(outputFile, d->width);
    fprintf(outputFile, ", ");
    WriteExp(outputFile, d->height);
    fprintf(outputFile, "\n");
    WriteDialogSettings(outputFile, d, &res->info);
    fprintf(outputFile, "BEGIN\n");
    while (c)
    {
        WriteControl(outputFile, c, d->ex.extended);
        c = c->next;
    }
    fprintf(outputFile, "END\n");
}
static void WriteRes(FILE *outputFile, char *rel, RESOURCE *res)
{
    WriteCommentLines(outputFile, res->prevLines);

    if (res->itype == RESTYPE_PLACEHOLDER)
        return;    
        
    if (res->type.symbolic || (res->itype != RESTYPE_STRING && res->itype != RESTYPE_LANGUAGE))
        WriteResId(outputFile, &res->id);
    if (res->type.symbolic)
    {
        WriteUserData(outputFile, rel, res);
    }
    else 
    {
        switch(res->itype)
        {
            case RESTYPE_LANGUAGE:
                WriteLanguage(outputFile, res);
                break;
            case RESTYPE_CURSOR:
                WriteCursor(outputFile, rel, res);
                break;
            case RESTYPE_BITMAP:
                WriteBitmap(outputFile, rel, res);
                break;
            case RESTYPE_ICON:
                WriteIcon(outputFile, rel, res);
                break;
            case RESTYPE_MENU:
                WriteMenu(outputFile, res);
                break;
            case RESTYPE_DIALOG:
                WriteDialog(outputFile, res);
                break;
            case RESTYPE_STRING:
                WriteString(outputFile, res);
                break;
            case RESTYPE_FONT:
                WriteFont(outputFile, rel, res);
                break;
            case RESTYPE_ACCELERATOR:
                WriteAccelerator(outputFile, res);
                break;
            case RESTYPE_RCDATA:
                WriteRCData(outputFile, rel, res);
                break;
            case RESTYPE_MESSAGETABLE:
                WriteMessageTable(outputFile, rel, res);
                break;
            case RESTYPE_VERSION:
                WriteVersion(outputFile, res);
                break;
            case RESTYPE_DLGINCLUDE:
                WriteDLGInclude(outputFile, rel, res);
                break;
            default:
                WriteUserData(outputFile, rel, res);
                break;
        }
    }
}
static void MarkDef(RESOURCE_DATA *select, EXPRESSION *id)
{
    if (id && id->rendition)
    {
        SYM *s = search(id->rendition, &select->syms);
        if (s)
            s->marked = TRUE;
    }
}
static void MarkAcceleratorDefinitions(RESOURCE_DATA *select, ACCELERATOR *accel)
{
    while (accel)
    {
        MarkDef(select, accel->id);
        accel = accel->next;
    }
}
static void MarkStringDefinitions(RESOURCE_DATA *select, STRINGS *strings)
{
    while (strings)
    {
        MarkDef(select, strings->id);
        strings = strings->next;
    }
}
static void MarkControlDefinitions(RESOURCE_DATA *select, DIALOG *dlg)
{
    CONTROL *controls = dlg->controls;
    while (controls)
    {
        MarkDef(select, controls->id);
        controls = controls->next;
    }
}
static void MarkMenuDefinitions(RESOURCE_DATA *select, MENUITEM *items)
{
    while (items)
    {
        MarkDef(select, items->id);
        if (items->popup)
            MarkMenuDefinitions(select, items->popup);
        items = items->next;
    }
}
static void MarkNewDefinitions(RESOURCE_DATA *select)
{
    RESOURCE *res;
    for (res = select->resources; res; res = res->next)
    {
        if (!res->id.symbolic && res->id.u.id)
            MarkDef(select, res->id.u.id);
        switch(res->itype)
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
static void WriteDefinitionSet(FILE *outputFile, SYM *definitions, BOOL weed)
{
    while (definitions)
    {
        if (!weed || definitions->marked)
        {
            char buf[2048];
            int i;
            DEFSTRUCT *d = (DEFSTRUCT *)definitions->value.s;
            WriteCommentLines(outputFile, definitions->lines);
            fprintf(outputFile, "#define ");
            fprintf(outputFile, "%s", definitions->name);
            if (d->argcount)
            {
                fputc('(', outputFile);
                for (i = 0; i < d->argcount; i++)
                {
                    StringWToA(buf,d->args[i],wcslen(d->args[i]));
                    fprintf(outputFile, "%s", buf);
                    if (i +1 < d->argcount)
                        fprintf(outputFile, ", ");
                }
                fputc(')', outputFile);
            }
            fputc(' ', outputFile);
            StringWToA(buf, d->string,wcslen(d->string));
            fprintf(outputFile, "%s", buf);
            fputc('\n', outputFile);
        }
        definitions = definitions->xref;
    }
}
static void WriteDefines(FILE *outputFile, RESOURCE_DATA *select)
{
    SYM *definitions;
    fprintf(outputFile, "#ifdef __IDE_RC_INVOKED\n");
    fprintf(outputFile, "#define __NEXT_CONTROL_ID\t%d\n", select->nextControlId);
    fprintf(outputFile, "#define __NEXT_MENU_ID\t\t%d\n", select->nextMenuId);
    fprintf(outputFile, "#define __NEXT_RESOURCE_ID\t%d\n", select->nextResourceId);    
    fprintf(outputFile, "#define __NEXT_STRING_ID\t%d\n", select->nextStringId);
    fprintf(outputFile, "#endif\n");
    definitions = select->newDefinitions;
    while (definitions)
    {
        definitions->marked = FALSE;
        definitions = definitions->xref;
    }
    MarkNewDefinitions(select);
    WriteDefinitionSet(outputFile, select->headerDefinitions, FALSE);
    WriteDefinitionSet(outputFile, select->newDefinitions, TRUE);
}
void WriteResources(char *fileName, RESOURCE_DATA *select)
{
    RESOURCE *res = select->resources;
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
        FILE *fil = DoOpen(fileName,"w");
        if (!fil)
            fatal("Could not write file");
        for (res = select->resources; res; res = res->next)
        {
            if (!res->deleted)
                WriteRes(fil, fileName, res);
        }
        fclose(fil);
        if (select->resourceIdFile)
        {
            fil = DoOpen(select->resourceIdFile,"w");
            if (!fil)
                fatal("Could not write file");
            WriteDefines(fil, select);
            fclose(fil);
            
        }
    }
}