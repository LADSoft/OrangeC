/*
	Software License Agreement (BSD License)
	
	Copyright (c) 1997-2013, David Lindauer, (LADSoft).
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
#include <stdio.h>
#include <stdlib.h>
#include <exception>
#include "_rtti.h"

#pragma startup __init_cppdata 119
#pragma rundown __rundown_cppdata 6

namespace __dls {

const char _RTL_DATA *__dls_NoNamedException = "No Named Exception" ;
const char _RTL_DATA *__dls_BadException = "Bad Exception" ;

} ;

namespace std {
   _RTL_FUNC exception::~exception ()  throw ()
   {
   }  
   _RTL_FUNC bad_exception::~bad_exception ()  throw()
   {
   }
} ;

void _RTL_FUNC terminate()
{
   abort() ;
}
static CPPDATA * getCPPData()
{
    CPPDATA *rv;
    asm mov eax,fs:[4]
    asm mov eax,[eax-4]
    asm mov [rv], eax;
    return rv;
}
extern "C" void _RTL_FUNC __call_terminate()
{
   try {
      std::flushall() ;
      CPPDATA * data = getCPPData();
      (*data->term)();
   } catch (...) {
   } 
   abort() ;
}
void _RTL_FUNC unexpected()
{
   __call_terminate() ;
}
terminate_handler _RTL_FUNC set_terminate(terminate_handler __t)
{
    CPPDATA * data = getCPPData();
    terminate_handler *rv = (terminate_handler)data->term;
    if (__t)
    {
       data->term = __t;
    }
    return rv;
}
unexpected_handler _RTL_FUNC set_unexpected(unexpected_handler __u)
{
    CPPDATA * data = getCPPData();
    unexpected_handler *rv = (unexpected_handler)data->term;
    if (__u)
    {
       CPPDATA * data = getCPPData();
       data->unexpected = __u;
    }
    return rv;
}
extern "C" void _RTL_FUNC __call_unexpected()
{
    CPPDATA * data = getCPPData();
    (*data->unexpected)();
}
extern "C" void _RTL_FUNC __init_cppdata()
{
   CPPDATA *block ;
   block = (CPPDATA *)malloc(sizeof(CPPDATA)) ;
   if (!block)
      exit(1) ;
   asm mov eax,fs:[4]
   asm mov edx,[block]
   asm mov [eax-4],edx
   block->term = terminate ;
   block->unexpected = unexpected ;
}
extern "C" void _RTL_FUNC __rundown_cppdata()
{
    CPPDATA *block = getCPPData();
    free(block) ;
}