#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "be.h"

int dbgblocknum;

void omf_dump_browsefile(BROWSEFILE *brf)
{
}
void debug_outputtypedef(SYMBOL *sp)
{
}
void dbginit(void)
{
}
void omfInit(void)
{
   memcpy(segAligns, segAlignsDefault, sizeof(segAligns));
}
void omf_dump_browsedata(BROWSEINFO* bri)
{
}   
void omf_globaldef(SYMBOL* sp)
{
}
void omf_put_extern(SYMBOL* sp, int code)
{
}
void omf_put_impfunc(SYMBOL* sp, char* file)
{
}
void omf_put_expfunc(SYMBOL* sp)
{
}
void omf_put_includelib(char* name)
{
}
void link_FileTime(char* file, char* buf)
{
}
void link_DebugMarker(void)
{
}
void link_LibMod(void)
{
}
void link_Files(void)
{
}
void link_Segs(void)
{
}
void link_putext(SYMBOL* sp)
{
}

void link_putimport(SYMBOL* sp)
{
}
void link_putpub(SYMBOL* sp, char sel)
{
}
void link_putexport(SYMBOL* sp)
{
}

void output_obj_file(void)
{
}

