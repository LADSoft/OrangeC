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
#ifndef __DIRECT_H
#define __DIRECT_H

#ifndef __STDDEF_H
#include <stddef.h>
#endif


#ifdef __cplusplus
namespace __STD_NS__ {
extern "C" {
#endif
struct _diskfree_t
{
    unsigned total_clusters;
    unsigned avail_clusters;
    unsigned sectors_per_cluster;
    unsigned bytes_per_sector;
};

int     _RTL_FUNC _IMPORT _chdrive(int __drive);
char *  _RTL_FUNC _IMPORT _getdcwd(int __drive, char *__buf, int __len);
unsigned _RTL_FUNC _IMPORT _getdiskfree(unsigned, struct _diskfree_t *);
int     _RTL_FUNC _IMPORT _getdrive(void);
unsigned long _RTL_FUNC _IMPORT _getdrives(void);

int _RTL_FUNC _IMPORT chdir(const char *);
char * _RTL_FUNC _IMPORT getcwd(char *, int);
int _RTL_FUNC _IMPORT mkdir(const char *);
int _RTL_FUNC _IMPORT rmdir(const char *);

int _RTL_FUNC _IMPORT _chdir(const char *);
char * _RTL_FUNC _IMPORT _getcwd(char *, int);
int _RTL_FUNC _IMPORT _mkdir(const char *);
int _RTL_FUNC _IMPORT _rmdir(const char *);


#ifdef __cplusplus
} ;
};
#endif

#endif /* __DIRECT_H */
#if defined(__cplusplus) && !defined(__USING_CNAME__) && !defined(__DIRECT_H_USING_LIST)
#define __DIRECT_H_USING_LIST
    using __STD_NS_QUALIFIER _chdrive;
    using __STD_NS_QUALIFIER _getdcwd;
    using __STD_NS_QUALIFIER _getdrive;
    using __STD_NS_QUALIFIER _getdrives;
    using __STD_NS_QUALIFIER _getdiskfree;
    using __STD_NS_QUALIFIER chdir;
    using __STD_NS_QUALIFIER _chdir;
    using __STD_NS_QUALIFIER getcwd;
    using __STD_NS_QUALIFIER _getcwd;
    using __STD_NS_QUALIFIER mkdir;
    using __STD_NS_QUALIFIER _mkdir;
    using __STD_NS_QUALIFIER rmdir;
    using __STD_NS_QUALIFIER _rmdir;
    
#endif
