/* Software License Agreement
 *
 *     Copyright(C) 1994-2022 David Lindauer, (LADSoft)
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

enum class kw
{
    openpa,
    closepa,
    plus,
    minus,
    lnot,
    bcompl,
    star,
    divide,
    mod,
    leftshift,
    rightshift,
    gt,
    lt,
    geq,
    leq,
    eq,
    ne,
    bor,
    bxor,
    band,
    land,
    lor,
    hook,
    colon,
    comma,
    ellipses = 100,

    DEFINE = 200,
    UNDEF,
    PRAGMA,
    ERROR,
    LINE,
    INCLUDE,
    INCLUDE_NEXT,
    IF,
    ELIF,
    IFDEF,
    IFNDEF,
    ELSE,
    ENDIF,
    IFIDN,
    ELIFIDN,
    IFIDNI,
    ELIFIDNI,
    IFID,
    ELIFID,
    IFNUM,
    ELIFNUM,
    IFSTR,
    ELIFSTR,
    IFNIDN,
    ELIFNIDN,
    IFNIDNI,
    ELIFNIDNI,
    IFNID,
    ELIFNID,
    IFNNUM,
    ELIFNNUM,
    IFNSTR,
    ELIFNSTR,
    ELIFDEF,
    ELIFNDEF,
    ASSIGN,
    REP,
    EXITREP,
    ENDREP,
    MACRO,
    ENDMACRO,
    ROTATE,
    PUSH,
    POP,
    REPL,
    IFCTX,
    ELIFCTX,
    IFNCTX,
    ELIFNCTX,
    IDEFINE,
    IASSIGN,
    IMACRO,
    WARNING,
    UNKNOWN = 10000
};
