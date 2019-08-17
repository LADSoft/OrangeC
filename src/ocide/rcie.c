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

/*
 * Evaluate an expression which should be known at compile time.
 * This uses recursive descent.  It is roughly analogous to the
 * primary expression handler except it returns a value rather than
 * an enode list
 */
#include <string.h>
#include <limits.h>
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <richedit.h>
#include <stdio.h>
#include <ctype.h>

#include "header.h"

extern int lastch;
extern WCHAR* lptr;
extern int cantnewline;
extern enum e_sym lastst;
extern char lastid[];
extern long ival;
extern TABLE defsyms;
extern SYM* typequal;
extern TABLE lsyms;
extern TABLE tagtable;
extern IFSTRUCT* ifs;
extern char intstring[50];

EXPRESSION* ReadExp(void);
int Eval(EXPRESSION*);
static EXPRESSION* iecondop(void);
void freeExpr(EXPRESSION* r)
{
    if (r)
    {
        freeExpr(r->left);
        freeExpr(r->right);
        rcFree(r);
    }
}
EXPRESSION* ReadExpFromString(WCHAR* id)
{
    EXPRESSION* expr;
    WCHAR* p;
    cantnewline++;
    p = --lptr;
    lptr = id;
    getch();
    getsym();
    expr = iecondop();
    lptr = p;
    getch();
    cantnewline--;
    return expr;
}
EXPRESSION* InternalLookup(char* id, int translate)
{
    WCHAR buf[2048], *p = buf;
    WCHAR nbuf[2048];
    EXPRESSION* expr = NULL;
    while (isspace(*id))
        id++;
    while (*id)
        *p++ = *id++;
    *p++ = '\n';
    *p = 0;
    wcscpy(nbuf, buf);
    defcheck(buf);
    if (wcscmp(nbuf, buf) || (!isalpha(buf[0]) && buf[0] != '_'))
    {
        expr = ReadExpFromString(translate ? nbuf : buf);
    }
    return expr;
}
EXPRESSION* LookupWithTranslation(char* id) { return InternalLookup(id, TRUE); }
static int rcLookup(char* id)
{
    int rv = 0;
    EXPRESSION* expr = InternalLookup(id, FALSE);
    if (expr)
    {
        rv = Eval(expr);
        freeExpr(expr);
    }
    return rv;
}

static EXPRESSION* ieprimary(void)
/*
 * PRimary integer
 *    id
 *    iconst
 *    (cast )intexpr
 *    (intexpr)
 */
{
    EXPRESSION* temp = 0;
    if (lastst == rceol)
        getsym();
    if (lastst == iconst || lastst == lconst || lastst == iuconst || lastst == luconst)
    {
        temp = rcAlloc(sizeof(EXPRESSION));
        temp->type = e_int;
        temp->rendition = rcStrdup(intstring);
        temp->val = ival;
        getsym();
        return temp;
    }
    else if (lastst == cconst)
    {
        char buf[10];
        buf[0] = '\'';
        if (lastch < 0x20)
        {
            buf[1] = '\\';
            buf[3] = 0;
            switch (lastch)
            {
                case 0:
                    buf[2] = '0';
                    break;
                case '\a':
                    buf[2] = 'a';
                    break;
                case '\b':
                    buf[2] = 'b';
                    break;
                case '\f':
                    buf[2] = 'f';
                    break;
                case '\n':
                    buf[2] = 'n';
                    break;
                case '\r':
                    buf[2] = 'r';
                    break;
                case '\t':
                    buf[2] = 't';
                    break;
                default:
                    sprintf(buf + 2, "x%02X", lastch);
                    break;
            }
            strcat(buf, "'");
        }
        else
        {
            buf[1] = lastch;
            buf[2] = '\'';
            buf[3] = 0;
        }
        temp = rcAlloc(sizeof(EXPRESSION));
        temp->type = e_int;
        temp->rendition = rcStrdup(buf);
        temp->val = ival;
        getsym();
        return temp;
    }
    else if (lastst == ident)
    {
        int val = rcLookup(lastid);
        char* s = rcStrdup(lastid);
        temp = rcAlloc(sizeof(EXPRESSION));
        temp->type = e_int;
        temp->val = val;
        temp->rendition = s;
        getsym();
        return temp;
    }
    else if (lastst == openpa)
    {
        getsym();
        temp = ReadExp();
        needpunc(closepa);
        return (temp);
    }
    getsym();
    return 0;
}

/*
 * Integer unary
 *   - unary
 *   ! unary
 *   ~unary
 *   primary
 */
static EXPRESSION* ieunary(void)
{
    EXPRESSION* temp;
    switch (lastst)
    {
        case minus:
        case not:
        case kw_not:
        case compl:
            temp = rcAlloc(sizeof(EXPRESSION));
            temp->type = lastst;
            getsym();
            temp->left = ieunary();
            break;
        case plus:
            getsym();
            temp = ieprimary();
            break;
        default:
            temp = ieprimary();
            break;
    }
    return (temp);
}

//-------------------------------------------------------------------------

static EXPRESSION* iemultops(void)
/* Multiply ops */
{
    EXPRESSION *val1 = ieunary(), *val2, *temp;
    while (val1 && (lastst == star || lastst == divide || lastst == modop))
    {
        long oper = lastst;
        getsym();
        val2 = ieunary();
        temp = rcAlloc(sizeof(EXPRESSION));
        temp->left = val1;
        temp->right = val2;
        temp->type = oper;
        val1 = temp;
    }
    return (val1);
}

//-------------------------------------------------------------------------

static EXPRESSION* ieaddops(void)
/* Add ops */
{
    EXPRESSION *val1 = iemultops(), *val2, *temp;
    while (val1 && (lastst == plus || lastst == minus))
    {
        long oper = lastst;
        getsym();
        val2 = iemultops();
        temp = rcAlloc(sizeof(EXPRESSION));
        temp->left = val1;
        temp->right = val2;
        temp->type = oper;
        val1 = temp;
    }
    return (val1);
}

//-------------------------------------------------------------------------

static EXPRESSION* ieshiftops(void)
/* Shift ops */
{
    EXPRESSION *val1 = ieaddops(), *val2, *temp;
    while (val1 && (lastst == lshift || lastst == rshift))
    {
        long oper = lastst;
        getsym();
        val2 = ieaddops();
        temp = rcAlloc(sizeof(EXPRESSION));
        temp->left = val1;
        temp->right = val2;
        temp->type = oper;
        val1 = temp;
    }
    return (val1);
}

//-------------------------------------------------------------------------

static EXPRESSION* ierelation(void)
/* non-eq relations */
{
    EXPRESSION *val1 = ieshiftops(), *val2, *temp;
    while (val1 && (lastst == lt || lastst == gt || lastst == leq || lastst == geq))
    {
        long oper = lastst;
        getsym();
        val2 = ieshiftops();
        temp = rcAlloc(sizeof(EXPRESSION));
        temp->left = val1;
        temp->right = val2;
        temp->type = oper;
        val1 = temp;
    }
    return (val1);
}

//-------------------------------------------------------------------------

static EXPRESSION* ieequalops(void)
/* eq relations */
{
    EXPRESSION *val1 = ierelation(), *val2, *temp;
    while (val1 && (lastst == eq || lastst == neq))
    {
        long oper = lastst;
        getsym();
        val2 = ierelation();
        temp = rcAlloc(sizeof(EXPRESSION));
        temp->left = val1;
        temp->right = val2;
        temp->type = oper;
        val1 = temp;
    }
    return (val1);
}

//-------------------------------------------------------------------------

static EXPRESSION* ieandop(void)
/* and op */
{
    EXPRESSION *val1 = ieequalops(), *val2, *temp;
    while (val1 && lastst == and)
    {
        getsym();
        val2 = ieequalops();
        temp = rcAlloc(sizeof(EXPRESSION));
        temp->left = val1;
        temp->right = val2;
        temp->type = and;
        val1 = temp;
    }
    return (val1);
}

//-------------------------------------------------------------------------

static EXPRESSION* iexorop(void)
/* xor op */
{
    EXPRESSION *val1 = ieandop(), *val2, *temp;
    while (val1 && lastst == uparrow)
    {
        getsym();
        val2 = ieandop();
        temp = rcAlloc(sizeof(EXPRESSION));
        temp->left = val1;
        temp->right = val2;
        temp->type = uparrow;
        val1 = temp;
    }
    return (val1);
}

//-------------------------------------------------------------------------

static EXPRESSION* ieorop(void)
/* or op */
{
    EXPRESSION *val1 = iexorop(), *val2, *temp;
    while (val1 && lastst == or)
    {
        getsym();
        val2 = iexorop();
        temp = rcAlloc(sizeof(EXPRESSION));
        temp->left = val1;
        temp->right = val2;
        temp->type = or ;
        val1 = temp;
    }
    return (val1);
}

//-------------------------------------------------------------------------

static EXPRESSION* ielandop(void)
/* logical and op */
{
    EXPRESSION *val1 = ieorop(), *val2, *temp;
    while (val1 && lastst == land)
    {
        getsym();
        val2 = ieorop();
        temp = rcAlloc(sizeof(EXPRESSION));
        temp->left = val1;
        temp->right = val2;
        temp->type = land;
        val1 = temp;
    }
    return (val1);
}

//-------------------------------------------------------------------------

static EXPRESSION* ielorop(void)
/* logical or op */
{
    EXPRESSION *val1 = ielandop(), *val2, *temp;
    while (val1 && lastst == lor)
    {
        getsym();
        val2 = ielandop();
        temp = rcAlloc(sizeof(EXPRESSION));
        temp->left = val1;
        temp->right = val2;
        temp->type = lor;
        val1 = temp;
    }
    return (val1);
}

//-------------------------------------------------------------------------

static EXPRESSION* iecondop(void)
/* Hook op */
{
    EXPRESSION *val1 = ielorop(), *val2, *val3, *temp;
    if (val1 && lastst == hook)
    {
        getsym();
        val2 = ielorop();
        needpunc(colon);
        val3 = iecondop();
        temp = rcAlloc(sizeof(EXPRESSION));
        temp->left = val1;
        temp->right = rcAlloc(sizeof(EXPRESSION));
        temp->right->left = val2;
        temp->right->right = val3;
        val1 = temp;
    }
    return (val1);
}

//-------------------------------------------------------------------------

EXPRESSION* ReadExp(void)
{
    EXPRESSION* rv = iecondop();
    if (!rv)
    {
        generror(ERR_NEEDCONST, 0);
    }
    return rv;
}
EXPRESSION* ReadExpOr(EXPRESSION* p)
{
    EXPRESSION* rv;
    if (!p)
        return ReadExp();

    rv = rcAlloc(sizeof(EXPRESSION));
    rv->type = or ;
    rv->left = p;
    rv->right = ReadExp();
    return ReadExp();
}

int Eval(EXPRESSION* p)
{
    if (!p)
        return 0;
    switch (p->type)
    {
        case hook:
            if (Eval(p->left))
                return Eval(p->right->left);
            else
                return Eval(p->right->right);
        case land:
            return Eval(p->left) && Eval(p->right);
        case lor:
            return Eval(p->left) || Eval(p->right);
        case or:
            return Eval(p->left) | Eval(p->right);
        case uparrow:
            return Eval(p->left) ^ Eval(p->right);
        case and:
            return Eval(p->left) & Eval(p->right);
        case eq:
            return Eval(p->left) == Eval(p->right);
        case neq:
            return Eval(p->left) != Eval(p->right);
        case lt:
            return Eval(p->left) < Eval(p->right);
        case gt:
            return Eval(p->left) > Eval(p->right);
        case leq:
            return Eval(p->left) <= Eval(p->right);
        case geq:
            return Eval(p->left) >= Eval(p->right);
        case lshift:
            return Eval(p->left) << Eval(p->right);
        case rshift:
            return ((unsigned)Eval(p->left)) >> Eval(p->right);
        case plus:
            return Eval(p->left) + Eval(p->right);
        case minus:
            if (p->right)
                return Eval(p->left) - Eval(p->right);
            else
                return 0 - Eval(p->left);
        case star:
            return Eval(p->left) * Eval(p->right);
        case divide:
        {
            int n = Eval(p->right);
            if (n)
                return Eval(p->left) / Eval(p->right);
            else
                return INT_MAX;
        }
        case modop:
        {
            int n = Eval(p->right);
            if (n)
                return Eval(p->left) % Eval(p->right);
            else
                return 0;
        }
        case not:
            return !Eval(p->left);
        case compl:
            return ~Eval(p->left);
        case e_int:
            return p->val;
        default:
            fatal("internal error");
            break;
    }

    return 0;
}

int intexpr(void)
{
    EXPRESSION* rv = ReadExp();
    if (rv)
    {
        int n = Eval(rv);
        freeExpr(rv);
        return n;
    }
    else
    {
        return 0;
    }
}
