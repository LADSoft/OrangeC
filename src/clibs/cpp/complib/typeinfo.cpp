/*
	Software License Agreement (BSD License)
	
	Copyright (c) 1997-2008, David Lindauer, (LADSoft).
	All rights reserved.
	
	Redistribution and use of this software in source and binary forms, with or without modification, are
	permitted provided that the following conditions are met:
	
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
	
	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
	WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
	PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
	ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
	TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
	ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <typeinfo>
#include <string.h>
#include <stdio.h>
#include "_rtti.h"

extern void _RTL_FUNC __undecorate(char *buf, char *name) ;
namespace std {

_RTL_FUNC type_info::type_info(const type_info &in)
{
    this->tpp = in.tpp;
}
type_info &  _RTL_FUNC type_info::operator=(const type_info &in)
{
    return *this ;
}

_RTL_FUNC type_info::~type_info()
{
}

bool         _RTL_FUNC type_info::operator==(const type_info  &in) const
{
             if (this->tpp == in.tpp )
               return true ;
             RTTI *s1 = (RTTI *)this->tpp ;
             RTTI *s2 = (RTTI *)in.tpp ;
             if (s1->flags & s2->flags & XD_POINTER)
               if (s1->destructor == s2->destructor)
                  return true ;
             return false ;
}
bool         _RTL_FUNC type_info::operator!=(const type_info  &in) const
{
             return !(*this == in) ;
}

bool         _RTL_FUNC type_info::before(const type_info  &in) const
{
    RTTI *left = (RTTI *)this->tpp;
    RTTI *right = (RTTI *)in.tpp;
    return (strcmp(left->name, right->name) > 0);
}

const char * _RTL_FUNC type_info::name() const
{
    RTTI *ths = (RTTI *)this->tpp;
    return ths->name;
}

} ;
