/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2011, David Lindauer, (LADSoft).
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
#include "ObjFactory.h"
#include "ObjIeee.h"
#ifdef XXXXX
#include "LinkPartition.h"
#include "LinkAttribs.h"
#include "LinkExpression.h"
#include "LinkLibrary.h"
#include "LinkManager.h"
#include "LinkMap.h"
#include "LinkOverlay.h"
#include "LinkPartition.h"
#include "LinkRegion.h"
#include "LinkRegionFileSpec.h"
#include "LinkRemapper.h"
#include "LinkTokenizer.h"
#include "CmdSwitch.h"
#endif
#include <cstdio>
#include <strstream>
#include <iomanip>

ObjSection *codeSection;

ObjType *BuildFunction(ObjFile &file, ObjFactory &factory)
{
    ObjType *rettype = factory.MakeType(ObjType::ePUInt);
    ObjFunction *rv = factory.MakeFunction("MyFunc", rettype);
    file.Add(rv);
    ObjType *argType = factory.MakeType(ObjType::eInt);
    ObjSymbol *s1 = factory.MakeAutoSymbol("auto1");
    s1->SetBaseType(argType);
    rv->Add(s1);
    file.Add(s1);
    s1 = factory.MakeAutoSymbol("auto2");
    s1->SetBaseType(argType);
    rv->Add(s1);
    file.Add(s1);
    
    return rv;
}

ObjType *BuildType(ObjFile &file, ObjFactory &factory)
{
    ObjType *rv = factory.MakeType(ObjType::eStruct);
    rv->SetName("mystruct");
    ObjType *base = factory.MakeType(ObjType::eUInt);
    ObjType *index = factory.MakeType(ObjType::eChar);
    ObjType *array = factory.MakeType(ObjType::eArray);
    array->SetBase(4);
    array->SetTop(8);
    array->SetBaseType(base);
    array->SetIndexType(index);
    file.Add(array);
    ObjType *ptr = factory.MakeType(ObjType::ePointer, base); 
    file.Add(ptr);
    ObjField *f = factory.MakeField("field1", ptr, 0);
    rv->Add(f);
    f = factory.MakeField("field2", array, 3);
    rv->Add(f);
    ObjType *function = BuildFunction(file, factory);
    f = factory.MakeField("field3", function, 4);
    rv->Add(f);
    f = factory.MakeField("field4", base, 5);
    rv->Add(f);
    f = factory.MakeField("field5", base, 6);
    rv->Add(f);
    f = factory.MakeField("field6", base, 7);
    rv->Add(f);
    f = factory.MakeField("field7", base, 8);
    rv->Add(f);
    f = factory.MakeField("field8", base, 9);
    rv->Add(f);
    f = factory.MakeField("field9", base, 10);
    rv->Add(f);
    f = factory.MakeField("field10", base, 11);
    rv->Add(f);
    f = factory.MakeField("field11", base, 12);
    rv->Add(f);
    f = factory.MakeField("field12", base, 13);
    rv->Add(f);
    file.Add(rv);
    return rv ; 
}
void BuildBrowse(ObjFile &file, ObjFactory &factory, ObjLineNo *l)
{
    ObjBrowseInfo *b = factory.MakeBrowseInfo(ObjBrowseInfo::eFuncStart, ObjBrowseInfo::eExternal,
                                              l, "hi dave");
    file.Add(b);	
    l = factory.MakeLineNo(l->GetFile(), 27);
    b = factory.MakeBrowseInfo(ObjBrowseInfo::eVariable, ObjBrowseInfo::eStatic,
                                              l, "hi dave");
    file.Add(b);	
}
ObjFile *MakeData(ObjFactory &factory)
{
    ObjFile *rv = factory.MakeFile("test.dat");
    std::time_t x = std::time(0);
    rv->SetFileTime(*std::localtime(&x));
    ObjSourceFile *sf = factory.MakeSourceFile("test.dat");
    rv->Add(sf);
    ObjSourceFile *sf1 = factory.MakeSourceFile("q.dat");
    rv->Add(sf1);
    ObjSection *sect = factory.MakeSection("code");
    codeSection = sect;
    rv->Add(sect);
    ObjSymbol *s1 = factory.MakePublicSymbol("public");
    rv->Add(s1);
    ObjSymbol *s1a = factory.MakePublicSymbol("2public");
    s1a->SetBaseType(BuildType(*rv, factory));
    rv->Add(s1a);
    ObjSymbol *s2 = factory.MakeLocalSymbol("local");
    s2->SetBaseType(factory.MakeType(ObjType::eInt));
    rv->Add(s2);
    ObjSymbol *s3 = factory.MakeAutoSymbol("auto");
    s3->SetOffset(factory.MakeExpression(-8));
    rv->Add(s3);
    ObjSymbol *s4 = factory.MakeExternalSymbol("extern");
    rv->Add(s4);
    ObjMemory *mem = factory.MakeData((ObjByte *)"\x44\x22\x33\x11", 4);
    ObjLineNo *l = factory.MakeLineNo(sf, 3);
    mem->Add(factory.MakeDebugTag(l));
    sect->Add(mem);
    mem = factory.MakeData((ObjByte *)"\xaa\xbb\x33", 3);
    mem->Add(factory.MakeDebugTag(s1));
    sect->Add(mem);
    mem = factory.MakeData((ObjByte *)"\xf1\x9a\x33", 3);
    mem->Add(factory.MakeDebugTag(s2));
    l = factory.MakeLineNo(sf1, 4);
    mem->Add(factory.MakeDebugTag(l));
    sect->Add(mem);
    mem = factory.MakeData((ObjByte *)"\x32\x17\x33", 3);
    mem->Add(factory.MakeDebugTag(s3));
    sect->Add(mem);
    ObjExpression *left = factory.MakeExpression(s4);
    ObjExpression *right = factory.MakeExpression(7);
    mem = factory.MakeFixup(factory.MakeExpression(ObjExpression::eAdd, left, right), 4);
    sect->Add(mem);
    mem = factory.MakeData((ObjByte *)"\x55\x44\x33", 3);
    mem->Add(factory.MakeDebugTag(s4));
    mem->Add(factory.MakeDebugTag(s1a));
    sect->Add(mem);
    BuildBrowse(*rv, factory, l);
    
    ObjExportSymbol *es = factory.MakeExportSymbol("export");
    es->SetExternalName("myexport");
    es->SetByOrdinal(false);
    rv->Add(es);
    ObjExportSymbol *es1 = factory.MakeExportSymbol("export1");
    es1->SetOrdinal(500);
    es1->SetByOrdinal(true);
    rv->Add(es1);
    ObjImportSymbol *is = factory.MakeImportSymbol("import");
    is->SetExternalName("myimport");
    is->SetDllName("mydll");
    rv->Add(is);
    ObjImportSymbol *is1 = factory.MakeImportSymbol("import1");
    is1->SetByOrdinal(true);
    is1->SetOrdinal(512);
    is1->SetDllName("mydll");
    rv->Add(is1);
    return rv;
}
int main()
{
    ObjIeeeIndexManager im;
    ObjFactory f(&im);
    
    ObjFile *fi = MakeData(f);
    fi->ResolveSymbols(&f);
    std::fstream a("hi.txt", std::fstream::trunc | std::fstream::out);
    
    ObjIeee i("hi");
    i.SetTranslatorName("Link32");
    i.SetDebugInfoFlag(true);
    ObjSection *sect = codeSection;
    ObjExpression *left = f.MakeExpression(sect);
    ObjExpression *right = f.MakeExpression(10);
    ObjExpression *sa = f.MakeExpression(ObjExpression::eAdd, left, right);
    i.SetStartAddress(sa);
    
    i.Write(a, fi, &f);
    a.close();
//	ObjFile *fi = i->Read(a, ObjIeee::eAll, f);

    ObjIeeeIndexManager im1;
    ObjFactory fact1(&im1);
    std::fstream b("hi.txt", std::fstream::in);
    ObjIeee i1("hi");
    ObjFile *fi1 = i.Read(b, ObjIeee::eAll, &fact1);
    a.close();
    
    std::fstream c("hi2.txt", std::fstream::trunc | std::fstream::out);
    ObjIeee i2("hi");
    i.SetTranslatorName("Link32");
    i.SetDebugInfoFlag(true);
    i.Write(c, fi1, &fact1);
    c.close();
    
}
