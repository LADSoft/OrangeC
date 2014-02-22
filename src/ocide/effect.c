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
#include <windows.h>
#include <ctype.h>

#define DSPDxax   0x00E20746L

#define RGBBLACK     RGB(0,0,0)
#define RGBWHITE     RGB(255,255,255)

typedef HANDLE HTHEME;
HTHEME hTheme;

HTHEME __stdcall OpenThemeData(HWND hwnd, LPWSTR pszClassList);
void __stdcall CloseThemeData(HTHEME hTheme);
COLORREF __stdcall GetThemeSysColor(HTHEME hTheme, int iColorId);

VOID FAR PASCAL ChangeBitmapColorDC(HDC hdcBM, LPBITMAP lpBM, COLORREF rgbOld,
    COLORREF rgbNew);

void InitTheme(HWND hwnd)
{
//    hTheme = OpenThemeData(hwnd, L"Edit");
                
}
void DestructTheme(void)
{
//    if (hTheme)
//        CloseThemeData(hTheme);
}
DWORD RetrieveSysColor(DWORD iColorId)
{
    return GetSysColor(iColorId);
}
HBRUSH RetrieveSysBrush(DWORD iColorId)
{
    return CreateSolidBrush(GetSysColor(iColorId));
}
/*************************************************************************
 *
 * ChangeBitmapColor()
 *
 *    This function translates the colors in a bitmap from one
 *    to another.
 *
 * Parameters:
 *
 *    HBITMAP hbmSrc  - Bitmap to process
 *    COLORREF rgbOld - Source color
 *    COLORREF rgbNew - Destination color
 *
 * Return value: none.
 *
 * History:   Date      Author      Reason
 *            6/10/91   CKindel     Created
 *            1/28/92   MarkBad     Split "Graying" functionality out into
 *                                    separate API, added palette param
 *
 *************************************************************************/

VOID ChangeBitmapColor(HBITMAP hbmSrc, COLORREF rgbOld, COLORREF rgbNew)
{
    HDC hDC;
    HDC hdcMem;
    BITMAP bmBits;

    if (hDC = GetDC(NULL))
    {
        if (hdcMem = CreateCompatibleDC(hDC))
        {
            //
            // Get the bitmap struct needed by ChangeBitmapColorDC()
            //
            GetObject(hbmSrc, sizeof(BITMAP), (LPSTR) &bmBits);

            //
            // Select our bitmap into the memory DC
            //
            hbmSrc = SelectObject(hdcMem, hbmSrc);

            //
            // Translate the sucker
            //
            ChangeBitmapColorDC(hdcMem, &bmBits, rgbOld, rgbNew);

            //
            // Unselect our bitmap before deleting the DC
            //
            hbmSrc = SelectObject(hdcMem, hbmSrc);

            DeleteDC(hdcMem);
        }

        ReleaseDC(NULL, hDC);
    }
} /* ChangeBitmapColor()  */


/*************************************************************************
 *
 * ChangeBitmapColorDC()
 *
 * This function makes all pixels in the given DC that have the
 * color rgbOld have the color rgbNew.  This function is used by
 * ChangeBitmapColor().
 *
 * Parameters:
 *
 * HDC hdcBM        - Memory DC containing bitmap
 * LPBITMAP lpBM    - Long pointer to bitmap structure from hdcBM
 * COLORREF rgbOld  - Source color
 * COLORREF rgbNew  - Destination color
 *
 * Return value: none.
 *
 * History:   Date      Author      Reason
 *            6/10/91   CKindel     Created
 *            1/23/92   MarkBad     Added big nifty comments which explain
 *                                  how this works, split bitmap graying 
 *                                  code out
 *
 *************************************************************************/

VOID FAR PASCAL ChangeBitmapColorDC(HDC hdcBM, LPBITMAP lpBM, COLORREF rgbOld,
    COLORREF rgbNew)
{
    HDC hdcMask;
    HBITMAP hbmMask, hbmOld;
    HBRUSH hbrOld;

    if (!lpBM)
        return ;

    //
    // if the bitmap is mono we have nothing to do
    //

    if (lpBM->bmPlanes == 1 && lpBM->bmBitsPixel == 1)
        return ;

    //
    // To perform the color switching, we need to create a monochrome 
    // "mask" which is the same size as our color bitmap, but has all 
    // pixels which match the old color (rgbOld) in the bitmap set to 1.  
    // 
    // We then use the ROP code "DSPDxax" to Blt our monochrome 
    // bitmap to the color bitmap.  "D" is the Destination color 
    // bitmap, "S" is the source monochrome bitmap, and "P" is the 
    // selected brush (which is set to the replacement color (rgbNew)).  
    // "x" and "a" represent the XOR and AND operators, respectively.
    // 
    // The DSPDxax ROP code can be explained as having the following 
    // effect:
    // 
    // "Every place the Source bitmap is 1, we want to replace the 
    // same location in our color bitmap with the new color.  All 
    // other colors we leave as is."
    // 
    // The truth table for DSPDxax is as follows:
    // 
    //       D S P Result
    //       - - - ------
    //       0 0 0   0
    //       0 0 1   0
    //       0 1 0   0
    //       0 1 1   1
    //       1 0 0   1
    //       1 0 1   1
    //       1 1 0   0
    //       1 1 1   1
    // 
    // (Even though the table is assuming monochrome D (Destination color), 
    // S (Source color), & P's (Pattern color), the results apply to color 
    // bitmaps also). 
    //
    // By examining the table, every place that the Source is 1 
    // (source bitmap contains a 1), the result is equal to the 
    // Pattern at that location.  Where S is zero, the result equals 
    // the Destination.  
    // 
    // See Section 11.2 (page 11-4) of the "Reference -- Volume 2" for more
    // information on the Termary Raster Operation codes.
    //
    if (hbmMask = CreateBitmap(lpBM->bmWidth, lpBM->bmHeight, 1, 1, NULL))
    {
        if (hdcMask = CreateCompatibleDC(hdcBM))
        {
            //
            // Select th mask bitmap into the mono DC
            //
            hbmOld = SelectObject(hdcMask, hbmMask);

            // 
            // Create the brush and select it into the source color DC --
            // this is our "Pattern" or "P" color in our DSPDxax ROP.
            //

            hbrOld = SelectObject(hdcBM, CreateSolidBrush(rgbNew));

            // 
            // To create the mask, we will use a feature of BitBlt -- when
            // converting from Color to Mono bitmaps, all Pixels of the
            // background colors are set to WHITE (1), and all other pixels
            // are set to BLACK (0).  So all pixels in our bitmap that are
            // rgbOld color, we set to 1.
            //

            SetBkColor(hdcBM, rgbOld);
            BitBlt(hdcMask, 0, 0, lpBM->bmWidth, lpBM->bmHeight, hdcBM, 0, 0,
                SRCCOPY);

            //
            // Where the mask is 1, lay down the brush, where it is 0, leave
            // the destination.
            //

            SetBkColor(hdcBM, RGBWHITE);
            SetTextColor(hdcBM, RGBBLACK);

            BitBlt(hdcBM, 0, 0, lpBM->bmWidth, lpBM->bmHeight, hdcMask, 0, 0,
                DSPDxax);

            SelectObject(hdcMask, hbmOld);

            hbrOld = SelectObject(hdcBM, hbrOld);
            DeleteObject(hbrOld);

            DeleteDC(hdcMask);
        }
        else
            return ;

        DeleteObject(hbmMask);
    }
    else
        return ;
} /* ChangeBitmapColorDC()  */
#define TBASE(x) (((x) * 6)/10)
#define TTRANS(x) (((x) * 4)/10)
void Tint(HBITMAP hBmpSrc, COLORREF dest)
{
    BITMAP bmBits;
    int i;
    int bbase = TBASE(dest & 0xff);
    int gbase = TBASE((dest >> 8) & 0xff);
    int rbase = TBASE((dest >> 16) & 0xff);
    GetObject(hBmpSrc, sizeof(BITMAP), (LPSTR) &bmBits);
    if (bmBits.bmBitsPixel != 32)
        return;
    for (i=0; i < bmBits.bmWidth * bmBits.bmHeight; i++)
    {
        DWORD rgb = *(((DWORD *)bmBits.bmBits)+i);
        DWORD transp = rgb >> 24;
        DWORD blue = rgb & 0xff;
        DWORD green = (rgb >> 8) & 0xff;
        DWORD red = (rgb >> 16) & 0xff;
        // first convert to gray
        int gray = (int)((red * .3) + (green * .59) + (blue * .11));
        // now tint the gray
        gray = TTRANS(gray);
        red = gray + rbase;
        green = gray + gbase;
        blue = gray + bbase;
        *(((DWORD *)bmBits.bmBits)+i) = (transp << 24) | RGB(red, green, blue);
    }
}
void ConvertToGray(HBITMAP hBmpSrc) // assumes a 32-bit bitmap
{
    BITMAP bmBits;
    int i;
    GetObject(hBmpSrc, sizeof(BITMAP), (LPSTR) &bmBits);
    if (bmBits.bmBitsPixel != 32)
        return;
    for (i=0; i < bmBits.bmWidth * bmBits.bmHeight; i++)
    {
        DWORD rgb = *(((DWORD *)bmBits.bmBits)+i);
        DWORD transp = rgb >> 24;
        DWORD blue = rgb & 0xff;
        DWORD green = (rgb >> 8) & 0xff;
        DWORD red = (rgb >> 16) & 0xff;
        int gray = (int)((red * .3) + (green * .59) + (blue * .11));
        *(((DWORD *)bmBits.bmBits)+i) = (transp << 24) | RGB(gray, gray, gray);
    }
}
HBITMAP ConvertToTransparent(HBITMAP hBmpSrc, DWORD color)
{
    HDC hDC, hdcMem ,hdcMem32;
    HBITMAP hOldBmp;
    BITMAP bmBits;
    HBITMAP hnewBmp = hBmpSrc;
    BITMAPINFO bmiNew;
    VOID *pvBitsNew;
    if (GetVersion() > 6) // windows 7?
    {
        GetObject(hBmpSrc, sizeof(BITMAP), (LPSTR) &bmBits);
        if (hDC = GetDC(NULL))
        {
            if (hdcMem = CreateCompatibleDC(hDC))
            {
                if (hdcMem32 = CreateCompatibleDC(hDC))
                {
                    int x,y;
                    DWORD *bits;
                    ZeroMemory(&bmiNew, sizeof(BITMAPINFO));
                
                    bmiNew.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                    bmiNew.bmiHeader.biWidth = bmBits.bmWidth;
                    bmiNew.bmiHeader.biHeight = bmBits.bmHeight;
                    bmiNew.bmiHeader.biPlanes = 1;
                    bmiNew.bmiHeader.biBitCount = 32;         // four 8-bit components 
                    bmiNew.bmiHeader.biCompression = BI_RGB;
                    bmiNew.bmiHeader.biSizeImage = bmBits.bmWidth, bmBits.bmHeight * 4; //FIXME ?
                
                    hnewBmp = CreateDIBSection(hdcMem32, &bmiNew, DIB_RGB_COLORS, &pvBitsNew, NULL, 0x0);
                    SelectObject(hdcMem32, hnewBmp);
    
                    hOldBmp = SelectObject(hdcMem, hBmpSrc);
                    bits = (DWORD *)pvBitsNew;
                    for (x=0; x< bmBits.bmWidth; x++)
                        for (y=0; y < bmBits.bmHeight; y++)
                        {
                            DWORD rgb = GetPixel(hdcMem, x, bmBits.bmHeight - y-1);
                            if (rgb == color)
                            {
                                bits[y * bmBits.bmWidth + x] = 0;
                            }
                            else
                            {
                                rgb = ((rgb & 0xff) << 16) + ((rgb >> 16) & 0xff) + (rgb & 0xff00);
                                bits[y * bmBits.bmWidth + x] = rgb | (0xff << 24);
                            }
                        }
                    hOldBmp = SelectObject(hdcMem, hOldBmp);
                    DeleteDC(hdcMem32);
                }
                DeleteDC(hdcMem);
            }
            ReleaseDC(NULL, hDC);
        }
        DeleteObject(hBmpSrc);
    }
    return hnewBmp;
}

HBITMAP CopyBitmap(HWND wnd, HBITMAP src, int x, int y, int width, int height)
{
    HDC hDC = GetDC(wnd);
    HDC sDC = CreateCompatibleDC(hDC);
    HDC dDC = CreateCompatibleDC(hDC);
    int cx = width;//GetSystemMetrics(SM_CXMENUCHECK);
    int cy = height; //GetSystemMetrics(SM_CYMENUCHECK);
    HBITMAP dest = CreateCompatibleBitmap(hDC, cx, cy), old;
    SelectObject(sDC, src);
    old = SelectObject(dDC, dest);
    SetStretchBltMode(sDC, COLORONCOLOR);
    StretchBlt(dDC, 0, 0, cx, cy, sDC, x, y, width, height, SRCCOPY);
//	BitBlt(dDC, 0, 0, width, height, sDC, x, y, PATCOPY);
    SelectObject(dDC, old);
    DeleteObject(sDC);
    DeleteObject(dDC);
    ReleaseDC(wnd,hDC);
    return dest;
}
