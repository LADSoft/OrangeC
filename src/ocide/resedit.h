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

#define UNDO_MAX 10

typedef struct
{
    short reserved;
    short resType;
    short resCount;
} ICOCURSORHDR;

typedef struct
{
    unsigned char width;
    unsigned char height;
    unsigned char colors;
    unsigned char unused;
    short hotspotX;
    short hotspotY;
    int DIBSize;
    int DIBOffset;
} ICOCURSORDESC, *PICOCURSORDESC;

typedef struct _imagedata
{
    struct _imagedata* next;
    struct _imagedata* undo;
    enum
    {
        FT_BMP,
        FT_ICO,
        FT_CUR
    } type;
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
    unsigned char* DIBInfo;
    DWORD rgbScreen;
} IMAGEDATA;

typedef struct _rubber
{
    enum
    {
        RT_LINE,
        RT_RECTANGLE,
        RT_ELLIPSE
    } type;
    int x;
    int y;
    int width;
    int height;
    int bmpwidth;
    int bmpheight;
    HBITMAP hbmpRubber;
    HDC hdcRubber;
} RUBBER;

#define RGB_WHITE RGB(255, 255, 255)
#define RGB_BLACK RGB(0, 0, 0)
