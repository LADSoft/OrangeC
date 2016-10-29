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
#define UNDO_MAX 10

typedef struct {
    short reserved;
    short resType;
    short resCount;
} ICOCURSORHDR;

typedef struct {
    unsigned char width;
    unsigned char height;
    unsigned char colors;
    unsigned char unused;
    short hotspotX;
    short hotspotY;
    int DIBSize;
    int DIBOffset;
} ICOCURSORDESC, *PICOCURSORDESC;

typedef struct _imagedata {
    struct _imagedata *next;
    struct _imagedata *undo;
    enum { FT_BMP, FT_ICO, FT_CUR } type;
    int imageDirty : 1;
    int fileDirty : 1;
    int height;
    int width;
    int colors;
    int hotspotX;
    int hotspotY;
    int DIBSize;
    HBITMAP hbmpImage;
    HDC hdcImage;
    HBITMAP hbmpAndMask;
    HDC hdcAndMask;
    unsigned char *DIBInfo;
    DWORD rgbScreen;
} IMAGEDATA;

typedef struct _rubber
{
    enum { RT_LINE, RT_RECTANGLE, RT_ELLIPSE } type;
    int x;
    int y;
    int width;
    int height;
    int bmpwidth ;
    int bmpheight;
    HBITMAP hbmpRubber;
    HDC hdcRubber;
} RUBBER;

#define RGB_WHITE RGB(255,255,255)
#define RGB_BLACK RGB(0,0,0)
