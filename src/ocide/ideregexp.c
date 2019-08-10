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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include "utype.h"
#include "regexp.h"

static int insert(context, type, ch, regexp) unsigned char* regexp;
int ch;
int type;
RE_CONTEXT* context;
{
    *context->current = calloc(sizeof(RE_MATCH), 1);
    if (!*context->current)
    {
        if (regexp)
            free(regexp);
        return FALSE;
    }
    (*context->current)->type = type;
    if (context->flags & RE_F_INSENSITIVE)
    {
        if (!regexp)
        {
            regexp = calloc(SET_BYTES, 1);
            if (!regexp)
            {
                free(*context->current);
                *context->current = NULL;
                return FALSE;
            }
            SETBIT(regexp, ch);
            if (isupper(ch))
                ch = tolower(ch);
            else
                ch = toupper(ch);
            SETBIT(regexp, ch);
            ch = 0;
        }
    }
    (*context->current)->ch = ch;
    (*context->current)->regexp = regexp;
    (*context->current)->rl = -1;
    (*context->current)->rh = -1;
    return TRUE;
}
static int GetClass(array, ch, not) unsigned char* array;
char** ch;
int not;
{
    char name[256], *p = name;
    unsigned char found[SET_BYTES];
    int i;
    memset(found, 0, sizeof(found));
    (*ch)++;
    while (**ch && (**ch != ':'))
        *p++ = *(*ch)++;
    if (*(*ch)++ != ':')
        return FALSE;
    if (*(*ch)++ != ']')
        return FALSE;
    *p = 0;

    if (!strcmp(name, "alpha"))
    {
        for (i = 0; i < 128; i++)
            if (isalpha(i))
                SETBIT(found, i);
    }
    else if (!strcmp(name, "upper"))
    {
        for (i = 0; i < 128; i++)
            if (isupper(i))
                SETBIT(found, i);
    }
    else if (!strcmp(name, "lower"))
    {
        for (i = 0; i < 128; i++)
            if (islower(i))
                SETBIT(found, i);
    }
    else if (!strcmp(name, "digit"))
    {
        for (i = 0; i < 128; i++)
            if (isdigit(i))
                SETBIT(found, i);
    }
    else if (!strcmp(name, "alnum"))
    {
        for (i = 0; i < 128; i++)
            if (isalnum(i))
                SETBIT(found, i);
    }
    else if (!strcmp(name, "xdigit"))
    {
        for (i = 0; i < 128; i++)
            if (isxdigit(i))
                SETBIT(found, i);
    }
    else if (!strcmp(name, "space"))
    {
        for (i = 0; i < 128; i++)
            if (isspace(i))
                SETBIT(found, i);
    }
    else if (!strcmp(name, "print"))
    {
        for (i = 0; i < 128; i++)
            if (isprint(i))
                SETBIT(found, i);
    }
    else if (!strcmp(name, "punct"))
    {
        for (i = 0; i < 128; i++)
            if (ispunct(i))
                SETBIT(found, i);
    }
    else if (!strcmp(name, "graph"))
    {
        for (i = 0; i < 128; i++)
            if (isgraph(i))
                SETBIT(found, i);
    }
    else if (!strcmp(name, "cntrl"))
    {
        for (i = 0; i < 128; i++)
            if (iscntrl(i))
                SETBIT(found, i);
    }
    else if (!strcmp(name, "blank"))
    {
        for (i = 0; i < 128; i++)
            if (i == ' ' || i == '\t')
                SETBIT(found, i);
    }
    else
        return FALSE;
    for (i = 0; i < SET_BYTES; i++)
        if (not)
            array[i] &= ~found[i];
        else
            array[i] |= found[i];
    return TRUE;
}
static unsigned char* GetRegexp(RE_CONTEXT* context, char** ch)
{
    int not = FALSE;
    unsigned char* rv = calloc(SET_BYTES, 1);
    if (!rv)
        return FALSE;
    if (*(*ch)++ != '[')
    {
        free(rv);  // memleak pointed out by cppcheck
        return FALSE;
    }
    if ((**ch) == '^')
    {
        not = TRUE;
        memset(rv, 255, SET_BYTES / 2);
        (*ch)++;
    }
    if ((**ch) == '-')
    {
        int n = '-';
        if (not)
            CLRBIT(rv, n);
        else
            SETBIT(rv, n);
        (*ch)++;
    }
    while (**ch && **ch != ']')
    {
        int n = *(*ch)++;
        if (n == '[' && **ch == ':')
        {
            if (!GetClass(rv, ch, not))
            {
                free(rv);
                return NULL;
            }
            continue;
        }
        if (n == '\\')
        {
            n = *(*ch)++;
            if (!n)
            {
                free(rv);
                return NULL;
            }
        }
        if (not)
            CLRBIT(rv, n);
        else
            SETBIT(rv, n);
        if (context->flags & RE_F_INSENSITIVE)
        {
            if (isupper(n))
                n = tolower(n);
            else
                n = toupper(n);
            if (not)
                CLRBIT(rv, n);
            else
                SETBIT(rv, n);
        }
        if ((**ch) == '-')
        {
            int m = *++(*ch);
            if (!m)
            {
                free(rv);
                return NULL;
            }
            if (m == '\\')
                m = *++(*ch);
            if (!m)
            {
                free(rv);
                return NULL;
            }
            if (context->flags & RE_F_INSENSITIVE)
            {
                n = toupper(n);
                m = toupper(m);
            }
            if (m < n)
            {
                free(rv);
                return NULL;
            }
            while (++n < m)
            {
                if (not)
                    CLRBIT(rv, n);
                else
                    SETBIT(rv, n);
                if (context->flags & RE_F_INSENSITIVE)
                {
                    int n1 = tolower(n);
                    if (not)
                        CLRBIT(rv, n1);
                    else
                        SETBIT(rv, n1);
                }
            }
        }
    }
    if (*(*ch)++ == 0)
    {
        free(rv);
        return NULL;
    }
    return rv;
}
static int getSpecial(ch) char** ch;
{
    int n = *(*ch)++;
    switch (n)
    {
        case 'b':
            return RE_M_WORD;
        case 'B':
            return RE_M_IWORD;
        case 'w':
            return RE_M_BWORD;
        case 'W':
            return RE_M_EWORD;
        case '<':
            return RE_M_WORDCHAR;
        case '>':
            return RE_M_NONWORDCHAR;
        case '`':
            return RE_M_BBUFFER;
        case '\'':
            return RE_M_EBUFFER;
        default:
            return n;
    }
}
static int GetMatch(context, ch) RE_CONTEXT* context;
char** ch;
{

    unsigned char* regexp;
    if (context->flags & RE_F_REGULAR)
    {
        switch (**ch)
        {
            int n;
            case '.':
                if (!insert(context, M_ANY, 0, NULL))
                    return FALSE;
                (*ch)++;
                break;
            case '|':
                if (!context->last || context->last->type != M_CHAR)
                    return FALSE;
                (*ch)++;
                if (**ch == '\\')
                {
                    (*ch)++;
                    n = getSpecial(ch);
                    regexp = NULL;
                }
                else if (**ch == '[')
                {
                    regexp = GetRegexp(context, ch);
                    if (!regexp)
                        return FALSE;
                    n = 0;
                }
                else
                {
                    regexp = NULL;
                    n = *(*ch)++;
                }
                if (regexp)
                {
                    if (!context->last->regexp)
                    {
                        SETBIT(regexp, context->last->ch);
                        context->last->ch = 0;
                        context->last->regexp = regexp;
                    }
                    else
                    {
                        int i;
                        for (i = 0; i < SET_BYTES; i++)
                            context->last->regexp[i] |= regexp[i];
                        free(regexp);
                    }
                }
                else
                {
                    if (!context->last->regexp)
                    {
                        context->last->regexp = calloc(SET_BYTES, 1);
                        if (!context->last->regexp)
                            return FALSE;
                        SETBIT(context->last->regexp, context->last->ch);
                        context->last->ch = 0;
                    }
                    SETBIT(context->last->regexp, n);
                }
                break;
            case '*':
                if (!context->last || context->last->type != M_CHAR)
                    return FALSE;
                (*ch)++;
                context->last->type = M_ZERO;
                break;
            case '+':
                if (!context->last || context->last->type != M_CHAR)
                    return FALSE;
                (*ch)++;
                context->last->type = M_ONE;
                break;
            case '?':
                if (!context->last || context->last->type != M_CHAR)
                    return FALSE;
                (*ch)++;
                context->last->rl = 0;
                context->last->rh = 1;
                break;
            case '[':
                regexp = GetRegexp(context, ch);
                if (!regexp)
                    return FALSE;
                if (!insert(context, M_CHAR, 0, regexp))
                {
                    return FALSE;
                }
                break;
            case '^':
                if (!insert(context, M_CHAR, RE_M_SOL, NULL))
                    return FALSE;

                (*ch)++;
                break;
            case '$':
                if (!insert(context, M_CHAR, RE_M_EOL, NULL))
                    return FALSE;
                (*ch)++;
                break;
            case '\\':
                (*ch)++;
                if (**ch == '{')
                {
                    int n1, n2;
                    if (!context->last)
                        return FALSE;
                    (*ch)++;
                    if (!isdigit(**ch))
                    {
                        return FALSE;
                    }
                    n1 = n2 = atoi(*ch);
                    while (isdigit(**ch))
                        (*ch)++;
                    if (**ch == ',')
                    {
                        (*ch)++;
                        if (!isdigit(**ch))
                        {
                            return FALSE;
                        }
                        n2 = atoi(*ch);
                        while (isdigit(**ch))
                            (*ch)++;
                    }
                    if (**ch != '\\' || (*ch)[1] != '}')
                        return FALSE;
                    (*ch) += 2;
                    if (n2 < n1)
                        return FALSE;
                    context->last->rl = n1;
                    context->last->rh = n2;
                }
                else if (**ch == '(')
                {
                    (*ch)++;
                    if (context->matchCount >= 10)
                        return FALSE;
                    context->matchStack[context->matchStackTop++] = context->matchCount;
                    if (!insert(context, M_START, context->matchCount++, NULL))
                        return FALSE;
                }
                else if ((**ch) == ')')
                {
                    (*ch)++;
                    if (context->matchStackTop == 0)
                        return FALSE;
                    if (!insert(context, M_END, context->matchStack[--context->matchStackTop], NULL))
                        return FALSE;
                }
                else if (isdigit(**ch))
                {
                    if (!insert(context, M_MATCH, *(*ch)++ - '0', NULL))
                        return FALSE;
                }
                else
                {
                    n = getSpecial(ch);
                    if (!insert(context, M_CHAR, n, NULL))
                        return FALSE;
                }
                break;
            default:
                if (!insert(context, M_CHAR, *(*ch)++, NULL))
                    return FALSE;
                break;
        }
    }
    else
    {
        if (!insert(context, M_CHAR, *(*ch)++, NULL))
            return FALSE;
    }
    return TRUE;
}

void re_free(context) RE_CONTEXT* context;
{
    while (context->root)
    {
        RE_MATCH* cur = context->root;
        context->root = context->root->next;
        if (cur->regexp)
            free(cur->regexp);
        free(cur);
    }
    free(context);
}

RE_CONTEXT* re_init(param, flags, word) char* param;
int flags;
char* word;
{

    RE_CONTEXT* context = calloc(1, sizeof(RE_CONTEXT));
    if (!context)
        return NULL;
    if (!word)
        word = "[a-zA-Z0-9_]";
    context->last = NULL;
    context->current = &context->root;
    context->wordChars = GetRegexp(context, &word);
    context->flags = flags;
    if (!context->wordChars)
    {
        free(context);
        return NULL;
    }
    while (*param)
    {
        if (!GetMatch(context, &param))
        {
            re_free(context);
            return NULL;
        }
        if (*context->current)
        {
            context->last = (*context->current);
            context->current = &(*context->current)->next;
        }
    }
    if (context->matchStackTop)
    {
        re_free(context);
        return NULL;
    }
    return context;
}
int matchesSpecial(RE_CONTEXT* context, int sp, char** ch)
{
    switch (sp)
    {
        case RE_M_WORD:
            if (TSTBIT(context->wordChars, **ch))
                if (!TSTBIT(context->wordChars, (*ch)[-1]))
                {
                    return TRUE;
                }
            if (*ch != context->beginning)
                if (!TSTBIT(context->wordChars, **ch))
                    if (TSTBIT(context->wordChars, (*ch)[-1]))
                    {
                        return TRUE;
                    }
            return FALSE;
        case RE_M_IWORD:
            if (TSTBIT(context->wordChars, **ch))
                if (TSTBIT(context->wordChars, (*ch)[-1]))
                {
                    return TRUE;
                }
            return FALSE;
        case RE_M_BWORD:
            if (TSTBIT(context->wordChars, **ch))
                if (!TSTBIT(context->wordChars, (*ch)[-1]))
                {
                    return TRUE;
                }
            return FALSE;
        case RE_M_EWORD:
            if (*ch != context->beginning)
                if (!TSTBIT(context->wordChars, **ch))
                    if (TSTBIT(context->wordChars, (*ch)[-1]))
                    {
                        return TRUE;
                    }
            return FALSE;
        case RE_M_WORDCHAR:
            if (TSTBIT(context->wordChars, **ch))
            {
                (*ch)++;
                return TRUE;
            }
            return FALSE;
        case RE_M_NONWORDCHAR:
            if (!TSTBIT(context->wordChars, **ch))
            {
                (*ch)++;
                return TRUE;
            }
            return FALSE;
        case RE_M_BBUFFER:
            return *ch == context->beginning;
        case RE_M_EBUFFER:
            return !**ch;
        case RE_M_SOL:
            if (*ch == context->beginning)
                return TRUE;
            return ((*ch)[-1] == '\n');
        case RE_M_EOL:
            if (**ch == '\n' || !**ch)
            {
                if (*ch)
                    (*ch)++;
                return TRUE;
            }
            return FALSE;
        default:
            return FALSE;
    }
}
int matchesChar(context, ch) RE_CONTEXT* context;
char** ch;
{
    int n = **ch, i;
    if ((*context->current)->regexp)
    {
        if (TSTBIT((*context->current)->regexp, n))
        {
            (*ch)++;
            return TRUE;
        }
        for (i = RE_M_WORD; i < RE_M_END; i++)
            if (TSTBIT((*context->current)->regexp, i))
                return matchesSpecial(context, i, ch);
    }
    else
    {
        if ((*context->current)->ch == n)
        {
            (*ch)++;
            return TRUE;
        }
        return matchesSpecial(context, (*context->current)->ch, ch);
    }
    return FALSE;
}
int matchesRange(context, string) RE_CONTEXT* context;
char** string;
{
    int i;
    if ((*context->current)->rl < 0 || (*context->current)->rh < 0)
        return matchesChar(context, string);
    if (*string != context->beginning)
    {
        char* old = *string;
        (*string)--;
        if (matchesChar(context, string))
        {
            *string = old;
            return FALSE;
        }
        *string = old;
    }
    for (i = 0; i < (*context->current)->rl; i++)
    {
        if (!matchesChar(context, string))
            return FALSE;
    }
    for (i = (*context->current)->rl; i < (*context->current)->rh; i++)
        if (!matchesChar(context, string))
            return TRUE;
    return !matchesChar(context, string);
}
int isMatched(context, string) RE_CONTEXT* context;
char* string;
{
    char* old = string;
    context->current = &context->root;
    while (*context->current)
    {
        switch ((*context->current)->type)
        {
            case M_CHAR:
                if (!matchesRange(context, &string))
                    return -1;
                break;
            case M_ANY:
                if (!isgraph(*string++))
                    return -1;
                break;
            case M_ONE:
                if (!matchesChar(context, &string))
                    return -1;
                /* fallthrough*/
            case M_ZERO:
                while (matchesChar(context, &string))
                    ;
                break;
            case M_START:
                context->matchOffsets[(*context->current)->ch][0] = string - context->beginning;
                break;
            case M_END:
                context->matchOffsets[(*context->current)->ch][1] = string - context->beginning;
                break;
            case M_MATCH:
                if (context->matchOffsets[(*context->current)->ch][1] <= 0)
                    return -1;
                else
                {
                    int i;
                    int begin = context->matchOffsets[(*context->current)->ch][0];
                    int end = context->matchOffsets[(*context->current)->ch][1];
                    for (i = begin; i < end; i++)
                    {
                        int n1 = context->beginning[i];
                        int n2 = *string++;
                        if (context->flags & RE_F_INSENSITIVE)
                        {
                            n1 = toupper(n1);
                            n2 = toupper(n2);
                        }
                        if (n1 != n2)
                            return -1;
                    }
                }
                break;
            default:
                break;
        }
        context->current = &(*context->current)->next;
    }
    return string - old;
}
int re_matches(context, text, start, end) RE_CONTEXT* context;
char* text;
int start;
int end;
{
    int i;
    int direction = 1;
    char* old;
    context->beginning = text;
    old = text += start;
    if (start == end)
        return FALSE;
    else if (start > end)
    {
        int n = start;
        start = end;
        end = n;
        direction = -1;
    }
    memset(context->matchOffsets, 0, sizeof(context->matchOffsets));
    while (text >= context->beginning + start && text <= context->beginning + end)
    {
        int n;
        if ((n = isMatched(context, text)) != -1)
        {
            if (!(context->flags & RE_F_WORD))
            {
                for (i = 0; i < 10; i++)
                {
                    context->matchOffsets[i][0] -= old - context->beginning;
                    context->matchOffsets[i][1] -= old - context->beginning;
                }
                context->m_so = text - old;
                context->m_eo = context->m_so + n;
                return TRUE;
            }
            else
            {
                if (text == context->beginning || !TSTBIT(context->wordChars, text[-1]))
                    if (!TSTBIT(context->wordChars, text[n]))
                    {
                        for (i = 0; i < 10; i++)
                        {
                            context->matchOffsets[i][0] -= old - context->beginning;
                            context->matchOffsets[i][1] -= old - context->beginning;
                        }
                        context->m_so = text - old;
                        context->m_eo = context->m_so + n;
                        return TRUE;
                    }
            }
        }
        text += direction;
    }
    return FALSE;
}
