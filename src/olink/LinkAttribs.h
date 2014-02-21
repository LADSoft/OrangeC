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
#ifndef LINKATTRIBS_H
#define LINKATTRIBS_H

#include "Objtypes.h"
#include "LinkExpression.h"

class LinkAttribs
{
    public:
        LinkAttribs() : address(NULL), align(NULL), maxSize(NULL), 
                roundSize(NULL), size(NULL), virtualOffset(NULL), fill(NULL) {}
        ~LinkAttribs();
        LinkAttribs &operator =(const LinkAttribs &Attribs);
        ObjInt GetAddress()
            { if (address) return address->Eval(0); else return 0; }
        void SetAddress(LinkExpression * Address)
            {  delete address; address = Address; }
        ObjInt GetAlign()
            { if (align) return align->Eval(0); else return 1; }
        void SetAlign(LinkExpression * Align) 
            {  delete align; align = Align; }
        ObjInt GetMaxSize()
            { if (maxSize) return maxSize->Eval(0); else return 0; }
        void SetMaxSize(LinkExpression * Size) 
            {  delete maxSize; maxSize = Size; }
        ObjInt GetSize()
            { if (size) return size->Eval(0); else return 0; }
        void SetSize(LinkExpression * Size) 
            {  delete size; size = Size; }
        ObjInt GetRoundSize()
            { if (roundSize) return roundSize->Eval(0); else return 0; }
        void SetRoundSize(LinkExpression * RoundSize) 
            { delete roundSize; roundSize = RoundSize; }
        ObjInt GetVirtualOffset()
            { if (virtualOffset) return virtualOffset->Eval(0); else return 0; }
        void SetVirtualOffset(LinkExpression * VirtualOffset) 
            {  delete virtualOffset; virtualOffset = VirtualOffset; }
        ObjInt GetFill() { if (fill) return fill->Eval(0); else return 0; }
        void SetFill(LinkExpression *Fill)
            { delete fill; fill = Fill; }
        bool GetAddressSpecified() { return address != NULL; }
        bool GetVirtualOffsetSpecified() { return virtualOffset != NULL; }
        bool GetHasFill() { return fill != NULL; }
    private:
        LinkExpression *address;
        LinkExpression *align;
        LinkExpression *maxSize;
        LinkExpression *roundSize;
        LinkExpression *virtualOffset;
        LinkExpression *size;
        LinkExpression *fill;
} ;

#endif
