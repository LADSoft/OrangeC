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

typedef struct re_match
{
    struct re_match* next;
    enum
    {
        M_CHAR,
        M_ANY,
        M_ZERO,
        M_ONE,
        M_START,
        M_END,
        M_MATCH
    } type;
    int ch;
    int rl, rh;
    unsigned char* regexp;
} RE_MATCH;

typedef struct re_context
{
    RE_MATCH* root;
    RE_MATCH** current;
    RE_MATCH* last;
    unsigned char* wordChars;
    char* beginning;
    int flags;
    int matchOffsets[10][2];
    int matchStack[10];
    int matchCount;
    int matchStackTop;
    int m_so;
    int m_eo;
} RE_CONTEXT;

#define SET_BYTES (256 / 8)
#define SETBIT(a, x) a[(x & 255) / 8] |= (1 << (x & 7))
#define CLRBIT(a, x) a[(x & 255) / 8] &= ~(1 << (x & 7))
#define TSTBIT(a, x) (!!(a[(x & 255) / 8] & (1 << (x & 7))))

#define RE_F_INSENSITIVE 1
#define RE_F_WORD 2
#define RE_F_REGULAR 4

#define RE_M_WORD 128
#define RE_M_IWORD 129
#define RE_M_BWORD 130
#define RE_M_EWORD 131
#define RE_M_WORDCHAR 132
#define RE_M_NONWORDCHAR 133
#define RE_M_BBUFFER 134
#define RE_M_EBUFFER 135
#define RE_M_SOL 136
#define RE_M_EOL 137
#define RE_M_END 138

RE_CONTEXT* re_init();
int re_matches();
void re_free();
