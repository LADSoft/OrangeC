/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
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

*/
#include "compiler.h"
void displayLexeme(LEXEME *lex)
{
            char buf[256];
            LCHAR *w;
            switch(lex->type)
            {
                case l_i:
                    printf("int constant: " LLONG_FORMAT_SPECIFIER"\n", lex->value.i);
                    break;
                case l_ui:
                    printf("unsigned int constant: " LLONG_FORMAT_SPECIFIER"\n", lex->value.i);
                    break;
                case l_l:
                    printf("long constant: " LLONG_FORMAT_SPECIFIER"\n", lex->value.i);
                    break;
                case l_ul:
                    printf("unsigned long constant: " LLONG_FORMAT_SPECIFIER"\n", lex->value.i);
                    break;
                case l_ll:
                    printf("long long constant: " LLONG_FORMAT_SPECIFIER"\n", lex->value.i);
                    break;
                case l_ull:
                    printf("unsigned long long constant: " LLONG_FORMAT_SPECIFIER"\n", lex->value.i);
                    break;
                case l_f:
                    FPFToString(buf, &lex->value.f);
                    printf("float constant: %s\n", buf);
                    break;
                case l_d:
                    FPFToString(buf, &lex->value.f);
                    printf("double constant: %s\n", buf);
                    break;
                case l_ld:
                    FPFToString(buf, &lex->value.f);
                    printf("long double constant: %s\n", buf);
                    break ;
                case l_astr:
                    printf("ascii string: ");
                    
                case l_wstr:
                    if (lex->type == l_wstr)
                        printf("wide string: ");
                    w = lex->value.s.w;
                    while (*w)
                        fputc(*w++, stdout);
                    fputc('\n', stdout);
                    break;
                case l_achr:
                    printf("ascii char: ");
                case l_wchr:
                    if (lex->type == l_wchr)
                        printf("wide char: ");
                    fputc((int)lex->value.i, stdout);
                    break;
                case l_id:
                    printf("id: %s\n", lex->value.s.a);
                    break;
                case l_kw:
                    printf("kw: %s\n", lex->kw->name);
                    break;
                default:
                    printf("***** unknown token\n");
                    break;
            }
}