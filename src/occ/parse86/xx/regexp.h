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
typedef struct re_match {
    struct re_match *next;
    enum { M_CHAR, M_ANY, M_ZERO, M_ONE, M_START, M_END, M_MATCH } type;
    int ch;
    int rl, rh;
    unsigned char *regexp;
} RE_MATCH;

typedef struct re_context {
    RE_MATCH *root;
    RE_MATCH **current;
    RE_MATCH *last;
    unsigned char *wordChars;
    char *beginning;
    int flags;
    int matchOffsets[10][2];
    int matchStack[10];
    int matchCount;
    int matchStackTop;
    int m_so;
    int m_eo;
} RE_CONTEXT;

#define SET_BYTES (256/8)
#define SETBIT(a, x)  a[(x & 255)/8] |= (1 << (x & 7))
#define CLRBIT(a, x)  a[(x & 255)/8] &= ~(1 << (x & 7))
#define TSTBIT(a,x)  (!!(a[(x & 255)/8] & (1 << (x & 7))))

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

RE_CONTEXT *re_init();
int re_matches();
void re_free();
