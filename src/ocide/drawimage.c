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
#include <commctrl.h>
#include <richedit.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include "header.h"
#include "rc.h"
#include "rcgui.h"
#include "img.h"

extern HWND hwndSrcTab;
extern HWND hwndClient;
extern HINSTANCE hInstance;
extern PROJECTITEM *workArea;
extern char szimageClassName[];

static char *szImageDrawClassName = "xccImageDrawClass";
static char *szUntitled = "Image";



static void InsertImgProperties(HWND lv, struct resRes *data);
static void GetImgPropText(char *buf, HWND lv, struct resRes *data, int row);
static HWND ImgPropStartEdit(HWND lv, int row, struct resRes *data);
static void ImgPropEndEdit(HWND lv, int row, HWND editWnd, struct resRes *data);
struct propertyFuncs imgFuncs = 
{ 
    InsertImgProperties,
    GetImgPropText,
    ImgPropStartEdit,
    ImgPropEndEdit
} ;

static void InsertImgProperties(HWND lv, struct resRes *data)
{
    PropSetGroup(lv, 101, L"General Characteristics");
    PropSetItem(lv, 0, 101, "Resource Identifier");
    PropSetItem(lv, 1, 101, "File");
    PropSetItem(lv, 2, 101, "Full Path");
}
static void GetImgPropText(char *buf, HWND lv, struct resRes *data, int row)
{
    buf[0] = 0;
    switch (row)
    {
        case 0:
            FormatResId(buf, &data->resource->id);
            break;
        case 1:
        {
            PROJECTITEM *pj = GetResProjectFile(data->hTreeItem);
            strcpy(buf, relpath(data->resource->filename, pj->realName));
        }
            break;
        case 2:
            strcpy(buf, data->resource->filename);
            break;
    }
}
static HWND ImgPropStartEdit(HWND lv, int row, struct resRes *data)
{
    if (row == 1 || row == 0)
    {
        HWND rv = PropGetHWNDText(lv);
        if (rv)
        {
            char buf[256];
            GetImgPropText(buf, lv, data, row);
            SendMessage(rv, WM_SETTEXT, 0, (LPARAM)buf);
        }
        return rv;
    }
    return NULL;
}
static void ImgPropEndEdit(HWND lv, int row, HWND editWnd, struct resRes *data)
{
    if (row == 1 || row == 0)
    {
        char buf[256];
        char buf1[256];
        SendMessage(editWnd, WM_GETTEXT, sizeof(buf), (LPARAM)buf);
        GetImgPropText(buf1, lv, data, row);
        if (strcmp(buf, buf1)) //FIXME strcmp check!
        {
            switch(row)
            {
                case 0:
                    PropSetIdName(data, buf, &data->resource->id.u.id, NULL);
                    break;                
                case 1:
                    PropSetExp(data, buf, &data->resource->info.language_high);
                    break;
            }
            ResSetDirty(data);
        }
        DestroyWindow(editWnd);
        ListView_SetItemText(lv, row, 1, LPSTR_TEXTCALLBACK);
        SetFocus(data->activeHwnd);
    }
}


int LoadImageToDC(HWND hwndParent, IMAGEDATA *res)
{
    static DWORD monoBmpColorTable[] = {
        RGB(0, 0, 0),
        RGB(255, 255, 255)
    };
    LPBITMAPINFO lpbi;
    INT bitCount;
    DWORD colorTableBytes;
    DWORD bits;
    LPBYTE DIBBits;
    HBITMAP hbmMono;
    HBITMAP hbmpImage;
    PBITMAPINFO pbi;

    if (!res->bits)
        return FALSE;
    lpbi = (LPBITMAPINFO)res->bitmapInfo;
    bitCount = lpbi->bmiHeader.biBitCount;

    if (!CreateImageDC(hwndParent, res))
        return FALSE;

    if (!(hbmMono = CreateBitmap(res->width, res->height, 1, 1, NULL)))
        return FALSE;

    if (bitCount == 24)
        colorTableBytes = 0;
    else
        colorTableBytes = (1 << bitCount) * sizeof(RGBQUAD);
        
        
    DIBBits = res->bits;
    bits = (((((DWORD)res->width * bitCount) + 31) & 0xffffffe0) >> 3) * res->height;

    pbi = calloc(1, res->DIBSize);
    memcpy(pbi, lpbi, res->DIBSize);

    pbi->bmiHeader.biHeight = res->height;
    pbi->bmiHeader.biSizeImage = bits;

    /*
     * Set up the XOR mask
     */
    if (res->saveColors < 8)
    {
        // must be two...
        HDC cdc; //FIXME cdc uninit !
        SetDIBits(cdc, hbmMono, 0, res->height, 
                  DIBBits, pbi, DIB_RGB_COLORS);
        cdc = CreateCompatibleDC(res->hdcImage);
        hbmpImage = SelectObject(cdc, hbmMono);
        BitBlt(res->hdcImage, 0, 0, res->width, res->height, cdc, 0, 0, SRCCOPY);
        hbmpImage = SelectObject(cdc, hbmpImage);
        DeleteDC(cdc);
    }
    else
    {
        hbmpImage = SelectObject(res->hdcImage, hbmMono);
        SetDIBits(res->hdcImage, hbmpImage, 0, res->height, 
                  DIBBits, pbi, DIB_RGB_COLORS);
        SelectObject(res->hdcImage, hbmpImage);
    }

    /* and the and mask */    
    if (res->type != FT_BMP) {
        DIBBits += bits;

        colorTableBytes = 2 * sizeof(RGBQUAD);

        /* set up the copy of the bitmap info structure
         * to have a monochrom bitmap
         */
        pbi->bmiHeader.biBitCount = 1;
        pbi->bmiHeader.biSizeImage =
                res->height * ((((DWORD)res->width + 31) & 0xffffffe0) >> 3);
        pbi->bmiHeader.biClrImportant = 0;
        pbi->bmiHeader.biClrUsed = 0;
        memcpy(pbi->bmiColors, &monoBmpColorTable[0],
                colorTableBytes); //FIXME ?

        /* move the bits into the and mask */
        hbmpImage = SelectObject(res->hdcAndMask, hbmMono);
        SetDIBits(res->hdcAndMask, hbmpImage, 0, res->height, 
                    DIBBits, pbi, DIB_RGB_COLORS);
        SelectObject(res->hdcAndMask, hbmpImage);

        /* make the viewable image */
        CombineImageDC(res);
    }

    res->imageDirty = FALSE;
    DeleteObject(hbmMono);
    free(pbi);
    return TRUE;

}




/************************************************************************
* ImageSave
*
* Saves the state of the current image into the image list (if it
* is dirty).
*
* History:
*
************************************************************************/

void SaveImage(IMAGEDATA *res)
{
    INT bitCount;
    DWORD colorTableBytes;
    DWORD XORBits;
    DWORD ANDBits;
    LPBYTE DIBInfo;
    DWORD DIBSize;
    DWORD bitsSize;
    LPBITMAPINFOHEADER lpbih;
    LPBYTE lpBits,lpBPtr;
    HBITMAP hbmMono;
    HBITMAP hbmpImage;
    
    if (!res->imageDirty)
        return;

    if (res->type != FT_BMP)
        SplitImageDC(res);

    /*
     * Create a temporary bitmap.
     */
    if (!(hbmMono = CreateBitmap(res->width, res->height, 1, 1, NULL)))
        return;

    switch (res->saveColors) {
        case 2:
            bitCount = 1;
            break;
        case 8:
            bitCount = 3;
            break;
        case 16:
            bitCount = 4;
            break;
        case 256:
            bitCount = 8;
            break;
        case 0:
            bitCount = 24;
            break;
    }

    colorTableBytes = (DWORD)res->saveColors * sizeof(RGBQUAD);
    XORBits = ((((res->width * bitCount) + 31)
            & 0xffffffe0) >> 3) * res->height;

    switch (res->type) {
        case FT_BMP:
            ANDBits = 0;
            break;

        case FT_ICO:
        case FT_CUR:
            ANDBits = res->height *
                    (((res->width + 31) & 0xffffffe0) >> 3);
            break;
    }

    DIBSize = sizeof(BITMAPINFOHEADER) + colorTableBytes;
    bitsSize = XORBits + ANDBits;

    if (res->type == FT_BMP)
    {
        if (!(DIBInfo = rcAlloc(DIBSize))) {
            DeleteObject(hbmMono);
            return;
        }
        if (!(lpBPtr = rcAlloc(bitsSize))) {
            DeleteObject(hbmMono);
            free(DIBInfo);
            return;
        }
    }
    else
    {
        if (!(DIBInfo = rcAlloc(DIBSize + bitsSize))) {
            DeleteObject(hbmMono);
            return;
        }
        lpBPtr = DIBInfo + DIBSize;
    }

    lpbih = (LPBITMAPINFOHEADER)DIBInfo;

    if (res->type != FT_BMP) {
        /* get the and bits */
        lpBits = lpBPtr + XORBits;

        lpbih->biSize          = sizeof(BITMAPINFOHEADER);
        lpbih->biWidth         = res->width;
        lpbih->biHeight        = res->height;
        lpbih->biPlanes        = 1;
        lpbih->biBitCount      = 1;
        lpbih->biCompression   = BI_RGB;
        lpbih->biSizeImage     = ANDBits;
        lpbih->biXPelsPerMeter = 0;
        lpbih->biYPelsPerMeter = 0;
        lpbih->biClrImportant  = 0;
        lpbih->biClrUsed       = 0;

        hbmpImage = SelectObject(res->hdcAndMask, hbmMono);
        GetDIBits(res->hdcAndMask, hbmpImage, 0, res->height, lpBits,
                (LPBITMAPINFO)lpbih, DIB_RGB_COLORS);
        SelectObject(res->hdcAndMask, hbmpImage);
    }

    /*
     * XOR mask
     */
    lpbih->biSize          = sizeof(BITMAPINFOHEADER);
    lpbih->biWidth         = res->width;
    lpbih->biHeight        = res->height;
    lpbih->biPlanes        = 1;
    lpbih->biBitCount      = bitCount;
    lpbih->biCompression   = BI_RGB;
    lpbih->biSizeImage     = XORBits;
    lpbih->biXPelsPerMeter = 0;
    lpbih->biYPelsPerMeter = 0;
    lpbih->biClrImportant  = 0;
    lpbih->biClrUsed       = 0;

    lpBits = lpBPtr;

    if (res->saveColors < 8 && res->colors > res->saveColors)
    {
        // must be two...
        HDC cdc = CreateCompatibleDC(res->hdcImage);
        hbmpImage = SelectObject(cdc, hbmMono);
        BitBlt(cdc, 0, 0, res->width, res->height, res->hdcImage, 0, 0, SRCCOPY);
        hbmpImage = SelectObject(cdc, hbmpImage);
        GetDIBits(cdc, hbmpImage, 0, res->height, lpBits,
                (LPBITMAPINFO)lpbih, DIB_RGB_COLORS);
        DeleteDC(cdc);

    }
    else
    {
        hbmpImage = SelectObject(res->hdcImage, hbmMono);
        GetDIBits(res->hdcImage, hbmpImage, 0, res->height, lpBits,
                (LPBITMAPINFO)lpbih, DIB_RGB_COLORS);
        SelectObject(res->hdcImage, hbmpImage);
    }

    /*
     * For icons and cursors, we have a few extra steps.
     */
    if (res->type != FT_BMP) {
        lpbih->biHeight *= 2;
        lpbih->biSizeImage = XORBits + ANDBits;

        CombineImageDC(res);
    }

    rcFree(res->bitmapInfo);
    if (res->type == FT_BMP)
    {
        rcFree(res->bits);
    }
    switch (res->type)
    {
        case FT_BMP:
        {
            BITMAP_ *bm = res->origData;
            bm->headerData = DIBInfo;
            bm->pixelData = lpBPtr;
            bm->headerSize = DIBSize;
            bm->pixelSize = bitsSize;
        }
            break;
        case FT_ICO:
        {
            ICONDATA *ic = res->origData;
            ic->data = DIBInfo;
            ic->bytes = DIBSize + bitsSize;
            ic->width = res->width;
            ic->height = res->height;
            ic->colorcount = res->saveColors;
        }
            break;
        case FT_CUR:
        {
            CURSORDATA *cc = res->origData;
            cc->data = DIBInfo;
            cc->bytes = DIBSize + bitsSize;
            cc->width = res->width;
            cc->height = res->height;
            cc->xhotspot = res->hotspotX;
            cc->yhotspot = res->hotspotY;
            cc->colorcount = 0;
        }
            break;
    }

    res->bits = lpBPtr;
    res->bitmapInfo = DIBInfo;
    res->DIBSize = DIBSize + bitsSize;

    res->fileDirty = TRUE;
    res->imageDirty = FALSE;

    DeleteObject(hbmMono);
}
IMAGEDATA *NewImage(HWND hwndParent, IMAGEDATA *res)
{
    IMAGEDATA *id;
    id = calloc(1, sizeof(IMAGEDATA));
    if (!id)
    {
        return NULL;
    }
    id->colors = res->colors;
    id->saveColors = res->saveColors;
    id->height = res->height;
    id->width = res->width; 
    id->type = res->type;
    id->hotspotX = res->hotspotX;
    id->hotspotY = res->hotspotY;
    id->imageDirty = TRUE;
    id->rgbScreen = RGB_DEFAULT_BACKGROUND;
        
    if (!CreateImageDC(hwndParent, id))
    {
        free(id);
        return NULL;
    }
    id->imageDirty = TRUE;
    return id;
}
static int validDIB(
    LPBITMAPINFO DIB,
    DWORD DIBSize,
    int bitmap)
{
    DWORD ANDMaskBytes;
    DWORD XORMaskBytes;
    DWORD colorTableBytes;
    DWORD height;

    if (DIBSize < sizeof(BITMAPINFOHEADER))
        return FALSE;

    if (DIB->bmiHeader.biSize != sizeof(BITMAPINFOHEADER))
        return FALSE;

    if (DIB->bmiHeader.biPlanes != 1)
        return FALSE;

    if (DIB->bmiHeader.biBitCount != 1 &&
            DIB->bmiHeader.biBitCount != 4 &&
            DIB->bmiHeader.biBitCount != 8 &&
            DIB->bmiHeader.biBitCount != 24)
        return FALSE;

    if (bitmap) {
        height = DIB->bmiHeader.biHeight;
        ANDMaskBytes = 0;
    }
    else {
        height = DIB->bmiHeader.biHeight / 2;
        ANDMaskBytes = (((DIB->bmiHeader.biWidth + 31) & 0xffffffe0) >> 3) *
                height;
    }
    if (DIB->bmiHeader.biBitCount == 24)
        colorTableBytes = 0;
    else
        colorTableBytes = (1 << DIB->bmiHeader.biBitCount) * sizeof(RGBQUAD);
    XORMaskBytes = ((((DIB->bmiHeader.biWidth * DIB->bmiHeader.biBitCount) +
            31) & 0xffffffe0) >> 3) * height;

    if (DIB->bmiHeader.biSizeImage &&
            DIB->bmiHeader.biSizeImage != XORMaskBytes + ANDMaskBytes &&
            DIB->bmiHeader.biSizeImage != XORMaskBytes)
        return FALSE;

    if (DIBSize != sizeof(BITMAPINFOHEADER) + colorTableBytes +
            XORMaskBytes + ANDMaskBytes)
        return FALSE;

    return TRUE;
}

static IMAGEDATA *LoadBitmapImage(HWND hwndParent, struct resRes *imageData)
{
    BITMAP_ *bitmapData = imageData->resource->u.bitmap;
    DWORD DIBSize;
    BITMAPINFOHEADER bih;
    INT colors;
    IMAGEDATA *id;

    DIBSize = bitmapData->headerSize + bitmapData->pixelSize;
    bih = *(BITMAPINFOHEADER *)(bitmapData->headerData);
    if (!validDIB( &bih, DIBSize, TRUE))
    {
        return NULL;
    }
    /* maximum image = 256 * 256 */
    if (bih.biWidth > 1024 || bih.biHeight > 1024) {
        return NULL;
    }

    switch (bih.biBitCount) {
        case 1:
            colors = 2;
            break;
        case 3:
            colors = 8;
            break;
        case 4:
            colors = 16;
            break;

        case 8:
            colors = 256;
            break;

        case 24:
            colors = 0;
            break;                
        default:
            return NULL;
    }
    id = calloc(1, sizeof(IMAGEDATA));
    if (!id)
    {
        return NULL;
    }
    id->bits = bitmapData->pixelData;
    id->bitmapInfo = bitmapData->headerData;
    id->type = FT_BMP;
    id->height = bih.biHeight;
    id->width = bih.biWidth;
    id->colors = id->saveColors = colors;
    id->DIBSize = DIBSize;
    id->rgbScreen = RGB_DEFAULT_BACKGROUND; /* not used for bitmaps, but let's initialize anyway */
    id->origData = bitmapData;

    if (!LoadImageToDC(hwndParent, id))
    {
        free(id);
        return NULL;
    }
    return id;
}
static IMAGEDATA *LoadCursorImage(HWND hwndParent, struct resRes * imageData, int icon)
{
    CURSORDATA *cursors = imageData->resource->u.cursor->cursors;
    int imageCount = imageData->resource->u.cursor->count;
    int i;
    LPBITMAPINFO lpBitmapInfo;
    int resType;
    int colorTableBytes;
    IMAGEDATA *id, *head =0, **tail = &head;
        resType = FT_CUR;
    /*
     * Get number of images in the file.
     */
    for (i=0; i < imageCount; i++, cursors++)
    {    
        id = calloc(1,sizeof(IMAGEDATA));
        if (!id)
        {
            while (head)
            {
                IMAGEDATA *next = head->next;
                free(head);
                head = next;
            }
            return FALSE;
        }
        *tail = id;
        tail = &(*tail)->next;
        id->bitmapInfo = cursors->data;
        id->type = resType;
        id->height = cursors->height;
        id->width = cursors->width;
        id->colors = cursors->colorcount == 8 ? 8 : 0;
        id->DIBSize = cursors->bytes;
        id->hotspotX = cursors->xhotspot;
        id->hotspotY = cursors->yhotspot;
        id->rgbScreen = RGB_DEFAULT_BACKGROUND;
        id->origData = cursors;

        lpBitmapInfo = id->bitmapInfo;
        if (id->colors == 0)
        {
            id->colors = 1 << lpBitmapInfo->bmiHeader.biBitCount ;
        }
        id->saveColors = id->colors;
        if (id->colors < 8)
            id->colors = 8;

        if (lpBitmapInfo->bmiHeader.biBitCount == 24)
            colorTableBytes = 0;
        else
            colorTableBytes = (1 << lpBitmapInfo->bmiHeader.biBitCount) * sizeof(RGBQUAD);
        id->bits = cursors->data + sizeof(BITMAPINFOHEADER) + colorTableBytes;

        if (!validDIB(lpBitmapInfo, id->DIBSize, FALSE) || !LoadImageToDC(hwndParent, id)) {
            while (head)
            {
                IMAGEDATA *next = head->next;
                free(head);
                head = next;
            }
            return FALSE;
        }
    }
    return head;
}
static IMAGEDATA *LoadIconImage(HWND hwndParent, struct resRes * imageData, int icon)
{
    ICONDATA *icons = imageData->resource->u.icon->icons;
    int imageCount = imageData->resource->u.icon->count;
    int i;
    LPBITMAPINFO lpBitmapInfo;
    int resType;
    int colorTableBytes;
    IMAGEDATA *id, *head =0, **tail = &head;

    resType = FT_ICO;
    /*
     * Get number of images in the file.
     */
    for (i=0; i < imageCount; i++, icons++)
    {    
        id = calloc(1,sizeof(IMAGEDATA));
        if (!id)
        {
            while (head)
            {
                IMAGEDATA *next = head->next;
                free(head);
                head = next;
            }
            return FALSE;
        }
        *tail = id;
        tail = &(*tail)->next;
        id->bitmapInfo = icons->data;
        id->type = resType;
        id->height = icons->height;
        id->width = icons->width;
        id->colors = icons->colorcount == 8 ? 8 : 0;
        id->DIBSize = icons->bytes;
        id->rgbScreen = RGB_DEFAULT_BACKGROUND;
        id->origData = icons;

        lpBitmapInfo = id->bitmapInfo;
        if (id->colors == 0)
        {
            id->colors = 1 << lpBitmapInfo->bmiHeader.biBitCount ;
        }
        id->saveColors = id->colors;
        if (id->colors < 8)
            id->colors = 8;
        
        if (lpBitmapInfo->bmiHeader.biBitCount == 24)
            colorTableBytes = 0;
        else
            colorTableBytes = (1 << lpBitmapInfo->bmiHeader.biBitCount) * sizeof(RGBQUAD);
        id->bits = icons->data + sizeof(BITMAPINFOHEADER) + colorTableBytes;

        if (!validDIB(lpBitmapInfo, id->DIBSize, FALSE) || !LoadImageToDC(hwndParent, id)) {
            while (head)
            {
                IMAGEDATA *next = head->next;
                free(head);
                head = next;
            }
            return FALSE;
        }
    }
    return head;
}
static void DoPaint(HWND hwnd, HDC hDc, IMAGEDATA *data, RECT *r)
{
    BitBlt(hDc, (r->right - r->left - data->width)/2,
           (r->bottom - r->top - data->height)/2, data->width, data->height,
           data->hdcImage,0,0,SRCCOPY);
}
static IMAGEDATA * CreateImages(HWND hwnd, struct resRes *imageData)
{
    IMAGEDATA *rv = NULL;
    switch(imageData->resource->itype)
    {
        case RESTYPE_CURSOR:
            rv = LoadCursorImage(hwnd, imageData, FALSE);
            break;
        case RESTYPE_BITMAP:
            rv = LoadBitmapImage(hwnd, imageData);
            break;
        case RESTYPE_ICON:
            rv = LoadIconImage(hwnd, imageData, TRUE);
            break;
    }
    return rv;
}
LRESULT CALLBACK ImageDrawProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam)
{
    RECT r;
    LPCREATESTRUCT createStruct;
    struct resRes *imageData;
    switch (iMessage)
    {
        case WM_MDIACTIVATE:
            if ((HWND)lParam == hwnd)
            {
                doMaximize();
            }
            break;
        case WM_SETFOCUS:
            imageData = (struct resRes *)GetWindowLong(hwnd, 0);
            SetFocus(imageData->gd.childWindow);
            break;
        case WM_CREATE:
            GetClientRect(hwnd, &r);
            createStruct = (LPCREATESTRUCT)lParam;
            imageData = (struct resRes *)((LPMDICREATESTRUCT)(createStruct->lpCreateParams))->lParam;
            SetWindowLong(hwnd, 0, (long)imageData);
            imageData->activeHwnd = hwnd;
            imageData->gd.images = CreateImages(hwnd, imageData);
            if (!imageData->gd.images)
            {
                ExtendedMessageBox("Error", 0, "Could not load image");
                return -1;
            }
            imageData->gd.childWindow = CreateWindowEx(0, szimageClassName, "Image Editor",
                WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
                CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                hwnd, NULL, hInstance, (void *)imageData);
            break;
        case WM_RESCOMMIT:
            imageData = (struct resRes *)GetWindowLong(hwnd, 0);
        {
            IMAGEDATA *res = imageData->gd.images;
            ResGetHeap(workArea, imageData);
            while (res)
            {
                SaveImage(res);
                res = res->next;
            }
        }
            break;
        case EM_CANUNDO:
            imageData = (struct resRes *)GetWindowLong(hwnd, 0);
            return SendMessage(imageData->gd.childWindow, iMessage, wParam, lParam);
        case WM_CLOSE:
            SendMessage(hwndSrcTab, TABM_REMOVE, 0, (LPARAM)hwnd);
            SendMessage(hwnd, WM_RESCOMMIT, 0, 0);
            break;
        case WM_DESTROY:
            imageData = (struct resRes *)GetWindowLong(hwnd, 0);
            imageData->activeHwnd = NULL;
            while (imageData->gd.images)
            {
                IMAGEDATA *next = imageData->gd.images->next;
                FreeImageUndo(imageData->gd.images);
                DeleteImageDC(imageData->gd.images);
                free(imageData->gd.images);
                imageData->gd.images = next;
            }
            break;
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDBLCLK:
            SetFocus(hwnd);
            return 0;
#if 0
        case WM_PAINT:
        
            imageData = (struct resRes *)GetWindowLong(hwnd, 0);
            GetClientRect(hwnd, &r);
            dc = BeginPaint(hwnd, &paint);
            DoPaint(hwnd, dc, imageData->gd.images, &r);
            EndPaint(hwnd, &paint);
            break;
#endif
        case WM_SIZE:
            imageData = (struct resRes *)GetWindowLong(hwnd, 0);
            MoveWindow(imageData->gd.childWindow, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
            break;
        default:
            break;
    }
    return DefMDIChildProc(hwnd, iMessage, wParam, lParam);
}

void RegisterImageDrawWindow(void)
{
    WNDCLASS wc;
    memset(&wc, 0, sizeof(wc));
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = &ImageDrawProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof(void*) * 2;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = 0;
    wc.lpszClassName = szImageDrawClassName;
    RegisterClass(&wc);

    imageInit();
    RegisterImageWindows();    
}
void CreateImageDrawWindow(struct resRes *info)
{
    char name[512];
    int maximized;
    HWND hwnd;
    switch (info->resource->itype)
    {
        case RESTYPE_CURSOR:
            sprintf(name, "%s - %s", "CURSOR", info->name);
            break;
        case RESTYPE_ICON:
            sprintf(name, "%s - %s", "ICON", info->name);
            break;
        case RESTYPE_BITMAP:
        default:
            sprintf(name, "%s - %s", "BITMAP", info->name);
            break;
    }
    SendMessage(hwndClient, WM_MDIGETACTIVE, 0, (LPARAM) &maximized);
    hwnd = CreateMDIWindow(szImageDrawClassName, name, WS_VISIBLE |
           WS_CHILD | WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | MDIS_ALLCHILDSTYLES | 
        WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
        WS_SIZEBOX | (PropGetInt(NULL, "TABBED_WINDOWS") ? WS_MAXIMIZE : WS_SYSMENU),
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hwndClient, hInstance, 
        (LPARAM)info); 
    if (hwnd)
    {
        SendMessage(hwndSrcTab, TABM_ADD, (WPARAM)name, (LPARAM)hwnd);
        if (info->resource->changed)
            ResRewriteTitle(info, TRUE);
    }
}