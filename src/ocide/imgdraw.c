#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <richedit.h>
#include "img.h"
#include "header.h"

static int ClipboardFormat;

void DeleteImageDC(IMAGEDATA* res);
void ClearImageDC(IMAGEDATA* res);
void SaveImage(IMAGEDATA* res);
void DrawLine(HDC hDC, POINT start, POINT end);
void FreeImageResource(IMAGEDATA* res);

void imageInit() { ClipboardFormat = RegisterClipboardFormat("xccIconFormat"); }

static void limit_undo(IMAGEDATA* res)
{
    int i;
    res = res->undo;
    for (i = 0; i < IMAGE_UNDO_MAX && res; i++)
    {
        res = res->undo;
    }
    if (res)
    {
        IMAGEDATA* undo = res->undo;
        res->undo = 0;
        while (undo)
        {
            res = undo->undo;
            DeleteImageDC(undo);
            free(undo);
            undo = res;
        }
    }
}
void ResizeImage(HWND hwndParent, IMGDATA* p, IMAGEDATA* res, int width, int height, BOOL stretch)
{
    int maxx, maxy;
    IMAGEDATA* undo;

    maxx = width > res->width ? res->width : width;
    maxy = height > res->height ? res->height : height;
    undo = calloc(1, sizeof(IMAGEDATA));
    if (!undo)
        return;
    *undo = *res;
    undo->next = NULL;

    limit_undo(res);
    res->undo = undo;

    res->hdcAndMask = res->hdcImage = 0;
    res->hbmpAndMask = res->hbmpImage = 0;
    res->width = width;
    res->height = height;
    CreateImageDC(hwndParent, res);
    if (stretch)
    {
        StretchBlt(res->hdcImage, 0, 0, res->width, res->height, res->undo->hdcImage, 0, 0, res->undo->width, res->undo->height,
                   SRCCOPY);
    }
    else
    {
        BitBlt(res->hdcImage, 0, 0, maxx, maxy, res->undo->hdcImage, 0, 0, SRCCOPY);
    }
    if (!res->imageDirty)
    {
        res->imageDirty = TRUE;
        ResSetDirty(p->resource);
    }
}
void InsertImageUndo(HWND hwndParent, IMAGEDATA* res)
{
    IMAGEDATA* undo;
    undo = calloc(1, sizeof(IMAGEDATA));
    if (!undo)
        return;
    *undo = *res;
    undo->next = NULL;

    limit_undo(res);
    res->undo = undo;

    res->hdcAndMask = res->hdcImage = 0;
    res->hbmpAndMask = res->hbmpImage = 0;
    CreateImageDC(hwndParent, res);
    BitBlt(res->hdcImage, 0, 0, res->width, res->height, res->undo->hdcImage, 0, 0, SRCCOPY);
}
void FreeImageUndo(IMAGEDATA* res)
{
    IMAGEDATA* old = res;
    res = res->undo;
    while (res)
    {
        IMAGEDATA* next = res->undo;
        DeleteImageDC(res);
        free(res);
        res = next;
    }
    old->undo = NULL;
}
void DirtyImageUndo(IMAGEDATA* res)
{
    IMAGEDATA* undoList = res->undo;
    while (undoList)
    {
        undoList->imageDirty = TRUE;
        undoList = undoList->next;
    }
}
void ApplyImageUndo(IMGDATA* p, IMAGEDATA* res)
{
    IMAGEDATA* oldundo;
    if (!res->undo)
        return;
    oldundo = res->undo;
    if (oldundo)
    {
        oldundo->next = res->next;
        DeleteImageDC(res);
        *res = *(oldundo);
        free(oldundo);
        if (!res->imageDirty)
        {
            ResSetClean(p->resource);
        }
        else
        {
            ResSetDirty(p->resource);
        }
    }
}
HBITMAP LocalCreateBitmap(HDC dc, int width, int height, int colors)
{
    BITMAPINFOHEADER bmih;

    if (colors == 2)
    {
        return CreateBitmap(width, height, 1, 1, NULL);
    }
    else
    {

        bmih.biSize = sizeof(BITMAPINFOHEADER);
        bmih.biWidth = width;
        bmih.biHeight = height;
        bmih.biPlanes = 1;
        switch (colors)
        {
            case 16:
                bmih.biBitCount = 4;
                break;
            case 256:
                bmih.biBitCount = 8;
                break;
            case 0:
                bmih.biBitCount = 24;
                break;
        }
        bmih.biCompression = 0;
        bmih.biSizeImage = 0;
        bmih.biXPelsPerMeter = 0;
        bmih.biYPelsPerMeter = 0;
        bmih.biClrUsed = 0;
        bmih.biClrImportant = 0;

        return CreateDIBitmap(dc, &bmih, 0, NULL, NULL, 0);
    }
}

int CreateImageDC(HWND hwndParent, IMAGEDATA* res)
{
    HDC hdcParent;

    DeleteImageDC(res);

    hdcParent = GetDC(hwndParent);

    if (!(res->hdcImage = CreateCompatibleDC(hdcParent)))
    {
        return FALSE;
    }
    if (!(res->hbmpImage = LocalCreateBitmap(hdcParent, res->width, res->height, res->colors)))
    {
        ReleaseDC(hwndParent, res->hdcImage);
        return FALSE;
    }

    if (res->type == FT_ICO || res->type == FT_CUR)
    {
        if (!(res->hdcAndMask = CreateCompatibleDC(hdcParent)))
        {
            ReleaseDC(hwndParent, res->hdcImage);
            return FALSE;
        }

        if (!(res->hbmpAndMask = LocalCreateBitmap(hdcParent, res->width, res->height, 2)))
        {
            ReleaseDC(hwndParent, res->hdcImage);
            DeleteDC(res->hdcAndMask);
            return FALSE;
        }

        SelectObject(res->hdcAndMask, res->hbmpAndMask);
    }

    SelectObject(res->hdcImage, res->hbmpImage);

    ClearImageDC(res);

    return TRUE;
}

void DeleteImageDC(IMAGEDATA* res)
{
    if (res->hdcImage)
    {
        DeleteDC(res->hdcImage);
        res->hdcImage = NULL;
        DeleteObject(res->hbmpImage);
        res->hbmpImage = NULL;
    }

    if (res->hdcAndMask)
    {
        DeleteDC(res->hdcAndMask);
        res->hdcAndMask = NULL;
        DeleteObject(res->hbmpAndMask);
        res->hbmpAndMask = NULL;
    }
}

void ClearImageDC(IMAGEDATA* res)
{
    PatBlt(res->hdcImage, 0, 0, res->width, res->height, WHITENESS);

    if (res->hdcAndMask)
        PatBlt(res->hdcAndMask, 0, 0, res->width, res->height, BLACKNESS);
}

void RecoverAndMask(IMAGEDATA* res)
{
    PatBlt(res->hdcAndMask, 0, 0, res->width, res->height, BLACKNESS);

    SetBkColor(res->hdcImage, res->rgbScreen);
    BitBlt(res->hdcAndMask, 0, 0, res->width, res->height, res->hdcImage, 0, 0, SRCINVERT);
    //    SetBkColor(res->hdcImage, res->rgbScreen ^ 0xffffff);
    //    BitBlt(res->hdcAndMask, 0, 0, res->width, res->height, res->hdcImage, 0, 0, SRCPAINT);
}
void SplitImageDC(IMAGEDATA* res)
{
    HBITMAP hbmTemp;
    HDC hdcTemp;
    HBITMAP hbmOld;

    /*
     *rebuild the and mask
     */
    RecoverAndMask(res);
    /*
     * for the mask bits
     */
    hdcTemp = CreateCompatibleDC(res->hdcImage);
    hbmTemp = CreateCompatibleBitmap(res->hdcImage, res->width, res->height);
    hbmOld = SelectObject(hdcTemp, hbmTemp);

    /*
     * colorize the mask with the screen color
     */
    SetBkColor(hdcTemp, res->rgbScreen);
    BitBlt(hdcTemp, 0, 0, res->width, res->height, res->hdcAndMask, 0, 0, SRCCOPY);

    /*
     * xor the screen colors against the image to recover the true xor mask
     */
    BitBlt(res->hdcImage, 0, 0, res->width, res->height, hdcTemp, 0, 0, SRCINVERT);

    SelectObject(hdcTemp, hbmOld);
    DeleteDC(hdcTemp);
    DeleteObject(hbmTemp);
}

void CombineImageDC(IMAGEDATA* res)
{
    HBITMAP hbmTemp;
    HDC hdcTemp;
    HBITMAP hbmOld;
    HBRUSH hbrOld;
    HBRUSH hbrScreen = CreateSolidBrush(res->rgbScreen);

    /*
     * image DC has an xor mask, make a copy
     */
    hdcTemp = CreateCompatibleDC(res->hdcImage);
    hbmTemp = CreateCompatibleBitmap(res->hdcImage, res->width, res->height);
    hbmOld = SelectObject(hdcTemp, hbmTemp);
    BitBlt(hdcTemp, 0, 0, res->width, res->height, res->hdcImage, 0, 0, SRCCOPY);

    /*
     * image DC is now the current screen color
     */
    hbrOld = SelectObject(res->hdcImage, hbrScreen);
    PatBlt(res->hdcImage, 0, 0, res->width, res->height, PATCOPY);
    SelectObject(res->hdcImage, hbrOld);

    /*
     * reconstruct the display image
     */
    BitBlt(res->hdcImage, 0, 0, res->width, res->height, res->hdcAndMask, 0, 0, SRCAND);
    BitBlt(res->hdcImage, 0, 0, res->width, res->height, hdcTemp, 0, 0, SRCINVERT);

    SelectObject(hdcTemp, hbmOld);
    DeleteDC(hdcTemp);
    DeleteObject(hbmTemp);
    DeleteObject(hbrScreen);
}

void ChangeImageSize(IMAGEDATA* res, int width, int height)
{
    HBITMAP hbmTemp;
    HDC hdcTemp;

    /* create a temporary dc */
    hdcTemp = CreateCompatibleDC(res->hdcImage);
    hbmTemp = LocalCreateBitmap(res->hdcImage, width, height, res->colors);
    SelectObject(hdcTemp, hbmTemp);

    /* blit into temporary DC */
    BitBlt(hdcTemp, 0, 0, res->width, res->height, res->hdcImage, 0, 0, SRCCOPY);

    res->width = width;
    res->height = height;
    DeleteDC(res->hdcImage);
    DeleteObject(res->hbmpImage);
    res->hdcImage = hdcTemp;
    res->hbmpImage = hbmTemp;
}
void ChangeImageColorCount(IMAGEDATA* res, int colors)
{
    HBITMAP hbmTemp;
    HDC hdcTemp;

    /* create a temporary dc */
    hdcTemp = CreateCompatibleDC(res->hdcImage);
    hbmTemp = LocalCreateBitmap(res->hdcImage, res->width, res->height, colors);

    /* blit into temporary DC */
    BitBlt(hdcTemp, 0, 0, res->width, res->height, res->hdcImage, 0, 0, SRCCOPY);

    res->colors = colors < 16 ? 16 : colors;
    res->saveColors = colors == 0 ? colors : res->colors;
    DeleteDC(res->hdcImage);
    DeleteObject(res->hbmpImage);
    res->hdcImage = hdcTemp;
    res->hbmpImage = hbmTemp;
}

int CopyImageToClipboard(HWND hwnd, IMAGEDATA* res, RECT* pick)
{
    HBITMAP hBitmap;
    HDC hDC;
    HANDLE hOldObj;
    int cxPick, cyPick;
    HANDLE hClipboardMem = NULL;

    cxPick = pick->right - pick->left;
    cyPick = pick->bottom - pick->top;

    hDC = CreateCompatibleDC(res->hdcImage);
    hBitmap = LocalCreateBitmap(res->hdcImage, cxPick, cyPick, res->colors);

    hOldObj = SelectObject(hDC, hBitmap);
    BitBlt(hDC, 0, 0, cxPick, cyPick, res->hdcImage, pick->left, pick->top, SRCCOPY);
    SelectObject(hDC, hOldObj);

    if (res->type != FT_BMP)
    {
        LPSTR lpBytes;
        HBITMAP hAndBitmap;
        RecoverAndMask(res);
        hAndBitmap = CreateCompatibleBitmap(res->hdcAndMask, cxPick, cyPick);
        hDC = CreateCompatibleDC(res->hdcAndMask);

        hOldObj = SelectObject(hDC, hAndBitmap);
        BitBlt(hDC, 0, 0, cxPick, cyPick, res->hdcAndMask, pick->left, pick->top, SRCCOPY);
        SelectObject(hDC, hOldObj);

        hClipboardMem = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, (DWORD)((cxPick + 31) >> 3) * cyPick + sizeof(DWORD));
        if (!hClipboardMem)
        {
            DeleteObject(hAndBitmap);
            DeleteObject(hBitmap);
            DeleteDC(hDC);
            return FALSE;
        }

        lpBytes = (LPSTR)GlobalLock(hClipboardMem);

        *((DWORD FAR*)lpBytes) = res->rgbScreen;

        GetBitmapBits(hAndBitmap, (DWORD)((cxPick + 31) >> 3) * cyPick, (LPSTR)lpBytes + sizeof(DWORD));

        DeleteObject(hAndBitmap);
        GlobalUnlock(hClipboardMem);
    }

    if (!OpenClipboard(hwnd))
    {
        GlobalFree(hClipboardMem);
        DeleteObject(hBitmap);
        DeleteDC(hDC);
        return (FALSE);
    }
    /* should process WM_DESTROYCLIPBOARD */
    EmptyClipboard();

    if (res->type != FT_BMP)
    {
        if (!SetClipboardData(ClipboardFormat, hClipboardMem))
        {
            GlobalFree(hClipboardMem);
            DeleteObject(hBitmap);
            DeleteDC(hDC);
            CloseClipboard();
            return (FALSE);
        }
    }
    if (!SetClipboardData(CF_BITMAP, hBitmap))
    {
        DeleteObject(hBitmap);
        DeleteDC(hDC);
        CloseClipboard();
        return (FALSE);
    }

    CloseClipboard();
    DeleteDC(hDC);

    return TRUE;
}

/* make a new IMAGEDATA with the value on the clipboard */
/* match gives data about DCs for matching the currently selected image */
IMAGEDATA* PasteImageFromClipboard(HWND hwnd, IMAGEDATA* match)
{
    INT cxClip;
    INT cyClip;
    INT colors;

    DWORD rgbClipScreen = 0;
    HBITMAP hbmpClipAnd = NULL;
    HBITMAP hbmpImage, hbmpDest;
    HBITMAP hOldImage;
    HDC hdcDest, hdcSource;
    IMAGEDATA* rv;
    BITMAP bmClip;

    if (!OpenClipboard(hwnd))
    {
        return NULL;
    }

    if (!(hbmpImage = GetClipboardData(CF_BITMAP)))
    {
        CloseClipboard();
        return NULL;
    }
    /* get size of bitmap */
    GetObject(hbmpImage, sizeof(BITMAP), (LPSTR)&bmClip);

    cxClip = (INT)bmClip.bmWidth;
    cyClip = (INT)bmClip.bmHeight;

    switch (bmClip.bmBitsPixel)
    {
        case 1:
            colors = 2;
            break;
        case 4:
            colors = 16;
            break;
        case 8:
            colors = 256;
            break;
        case 24:
        case 32:
            colors = 0;
            break;
        default:
            CloseClipboard();
            return NULL;
    }
    /* copy the clipboard bitmap */
    hdcSource = CreateCompatibleDC(match->hdcImage);
    hdcDest = CreateCompatibleDC(match->hdcImage);
    hbmpDest = CreateCompatibleBitmap(match->hdcImage, cxClip, cyClip);
    hOldImage = SelectObject(hdcSource, hbmpImage);
    SelectObject(hdcDest, hbmpDest);
    BitBlt(hdcDest, 0, 0, cxClip, cyClip, hdcSource, 0, 0, SRCCOPY);
    SelectObject(hdcSource, hOldImage);
    SelectObject(hdcDest, hOldImage);
    DeleteDC(hdcSource);

    /* copy and mask from clipboard if it exists */
    if (IsClipboardFormatAvailable(ClipboardFormat))
    {
        HANDLE hClipData = GetClipboardData(ClipboardFormat);
        LPBYTE lpClipData = (LPBYTE)GlobalLock(hClipData);
        rgbClipScreen = *((DWORD FAR*)lpClipData);

        hbmpClipAnd = CreateBitmap(cxClip, cyClip, (BYTE)1, (BYTE)1, (LPSTR)lpClipData + sizeof(DWORD));
        GlobalUnlock(hClipData);
    }
    CloseClipboard();

    /* make an object to store the data in */
    rv = calloc(1, sizeof(IMAGEDATA));
    if (!rv)
    {
        DeleteObject(hbmpClipAnd);
        DeleteObject(hbmpDest);
        DeleteDC(hdcDest);
        return NULL;
    }

    /* everything ok, fill in our structure and return it */
    rv->colors = colors < 16 ? 16 : colors;
    rv->saveColors = colors == 0 ? colors : rv->colors;
    rv->width = cxClip;
    rv->height = cyClip;
    rv->hbmpAndMask = hbmpClipAnd;
    rv->hdcAndMask = CreateCompatibleDC(match->hdcAndMask);
    rv->hbmpImage = hbmpDest;
    rv->hdcImage = hdcDest;
    rv->type = match->type;
    rv->rgbScreen = rgbClipScreen;

    SelectObject(rv->hdcAndMask, rv->hbmpAndMask);
    SelectObject(rv->hdcImage, rv->hbmpImage);
    return rv;
}
/* return true if there is valid data on the clipboard */
int Pastable(HWND hwnd)
{
    HBITMAP hbmpImage;
    BITMAP bmClip;
    if (!OpenClipboard(hwnd))
    {
        return FALSE;
    }

    if (!(hbmpImage = GetClipboardData(CF_BITMAP)))
    {
        CloseClipboard();
        return FALSE;
    }
    /* get size of bitmap */
    GetObject(hbmpImage, sizeof(BITMAP), (LPSTR)&bmClip);

    CloseClipboard();

    switch (bmClip.bmBitsPixel)
    {
        case 1:
        case 4:
        case 8:
        case 24:
            return TRUE;
        default:
            return FALSE;
    }
}
RUBBER* CreateRubberBand(IMAGEDATA* res, int type, int filltype, POINT pt)
{
    RUBBER* rv = calloc(1, sizeof(RUBBER));
    LOGPEN lpen;
    LOGBRUSH lbrush;
    HPEN pen;
    HBRUSH brush;
    if (!rv)
        return NULL;

    rv->hbmpRubberBorder = CreateBitmap(res->width, res->height, 1, 1, NULL);
    if (!rv->hbmpRubberBorder)
    {
        free(rv);
        return NULL;
    }
    rv->hdcRubberBorder = CreateCompatibleDC(res->hdcImage);
    if (!rv->hdcRubberBorder)
    {
        DeleteObject(rv->hbmpRubberBorder);
        free(rv);
        return NULL;
    }
    SelectObject(rv->hdcRubberBorder, rv->hbmpRubberBorder);

    rv->hbmpRubberFill = CreateBitmap(res->width, res->height, 1, 1, NULL);
    if (!rv->hbmpRubberFill)
    {
        DeleteObject(rv->hbmpRubberBorder);
        DeleteObject(rv->hdcRubberBorder);
        free(rv);
        return NULL;
    }
    rv->hdcRubberFill = CreateCompatibleDC(res->hdcImage);
    if (!rv->hdcRubberFill)
    {
        DeleteObject(rv->hbmpRubberFill);
        DeleteObject(rv->hbmpRubberBorder);
        DeleteObject(rv->hdcRubberBorder);
        free(rv);
        return NULL;
    }
    SelectObject(rv->hdcRubberFill, rv->hbmpRubberFill);

    pen = CreatePen(PS_SOLID, 0, 0);
    pen = SelectObject(res->hdcImage, pen);
    GetObject(pen, sizeof(lpen), &lpen);
    pen = SelectObject(res->hdcImage, pen);
    DeleteObject(pen);

    brush = CreateSolidBrush(0);
    brush = SelectObject(res->hdcImage, brush);
    GetObject(brush, sizeof(lbrush), &lbrush);
    brush = SelectObject(res->hdcImage, brush);
    DeleteObject(brush);

    lpen.lopnColor = filltype == FT_BORDER || filltype == FT_HOLLOW ? RGB_WHITE : RGB_BLACK;
    pen = CreatePenIndirect(&lpen);
    DeleteObject(SelectObject(rv->hdcRubberBorder, pen));

    lbrush.lbColor = RGB_BLACK;
    brush = CreateBrushIndirect(&lbrush);
    DeleteObject(SelectObject(rv->hdcRubberBorder, brush));

    lpen.lopnColor = RGB_BLACK;
    pen = CreatePenIndirect(&lpen);
    DeleteObject(SelectObject(rv->hdcRubberFill, pen));

    lbrush.lbColor = filltype == FT_BORDER || filltype == FT_FILL ? RGB_WHITE : RGB_BLACK;
    brush = CreateBrushIndirect(&lbrush);
    DeleteObject(SelectObject(rv->hdcRubberFill, brush));

    rv->type = type;
    rv->filltype = filltype;
    rv->x = pt.x;
    rv->y = pt.y;
    rv->width = 0;
    rv->height = 0;
    rv->bmpwidth = res->width;
    rv->bmpheight = res->height;
    return rv;
}
void DeleteRubberBand(IMGDATA* p, RUBBER* r)
{
    DeleteDC(r->hdcRubberBorder);
    DeleteObject(r->hbmpRubberBorder);
    DeleteDC(r->hdcRubberFill);
    DeleteObject(r->hbmpRubberFill);
    free(r);
    SendMessage(p->hwndStatus, SB_SETTEXT, 3, (LPARAM) "");
}
static void printRubberSize(IMGDATA* p, int w, int h)
{
    char buf[256];
    sprintf(buf, "%d x %d", w / p->zoom, h / p->zoom);
    SendMessage(p->hwndStatus, SB_SETTEXT, 3, (LPARAM)buf);
}
void RubberBand(IMGDATA* p, RUBBER* r, POINT pt)
{
    POINT start;
    r->width = pt.x - r->x;
    r->height = pt.y - r->y;

    printRubberSize(p, r->width, r->height);

    PatBlt(r->hdcRubberBorder, 0, 0, r->bmpwidth, r->bmpheight, BLACKNESS);
    PatBlt(r->hdcRubberFill, 0, 0, r->bmpwidth, r->bmpheight, BLACKNESS);
    switch (r->type)
    {
        case RT_LINE:
            start.x = r->x;
            start.y = r->y;
            DrawLine(r->hdcRubberBorder, start, pt);
            break;
        case RT_RECTANGLE:
            Rectangle(r->hdcRubberBorder, r->x, r->y, r->x + r->width + 1, r->y + r->height + 1);
            Rectangle(r->hdcRubberFill, r->x, r->y, r->x + r->width + 1, r->y + r->height + 1);
            break;
        case RT_ROUNDEDRECTANGLE:
            RoundRect(r->hdcRubberBorder, r->x, r->y, r->x + r->width + 1, r->y + r->height + 1, 7, 7);
            RoundRect(r->hdcRubberFill, r->x, r->y, r->x + r->width + 1, r->y + r->height + 1, 7, 7);
            break;
        case RT_ELLIPSE:
            Ellipse(r->hdcRubberBorder, r->x, r->y, r->x + r->width + 1, r->y + r->height + 1);
            Ellipse(r->hdcRubberFill, r->x, r->y, r->x + r->width + 1, r->y + r->height + 1);
            break;
    }
}
void InstantiateRubberBand(IMGDATA* p, IMAGEDATA* res, RUBBER* r)
{
    HPEN pen;
    HBRUSH brush;
    LOGPEN lpen;
    switch (r->type)
    {
        POINT start, end;
        case RT_LINE:
            start.x = r->x;
            start.y = r->y;
            end.x = r->x + r->width;
            end.y = r->y + r->height;
            DrawLine(res->hdcImage, start, end);
            break;
        case RT_RECTANGLE:
        case RT_ROUNDEDRECTANGLE:
        case RT_ELLIPSE:
            SetBkColor(res->hdcImage, RGB_WHITE);
            SetTextColor(res->hdcImage, RGB_BLACK);
            BitBlt(res->hdcImage, 0, 0, res->width, res->height, r->hdcRubberFill, 0, 0, DSPDxax);
            pen = CreatePen(PS_SOLID, 0, 0);
            pen = SelectObject(res->hdcImage, pen);
            GetObject(pen, sizeof(lpen), &lpen);
            pen = SelectObject(res->hdcImage, pen);
            DeleteObject(pen);
            brush = CreateSolidBrush(lpen.lopnColor);
            brush = SelectObject(res->hdcImage, brush);
            BitBlt(res->hdcImage, 0, 0, res->width, res->height, r->hdcRubberBorder, 0, 0, DSPDxax);
            brush = SelectObject(res->hdcImage, brush);
            DeleteObject(brush);
            break;
    }
    if (!res->imageDirty)
    {
        res->imageDirty = TRUE;
        ResSetDirty(p->resource);
    }
}
void DrawPoint(IMGDATA* p, IMAGEDATA* res, POINT pt)
{
    MoveToEx(res->hdcImage, pt.x, pt.y, NULL);
    LineTo(res->hdcImage, pt.x + 1, pt.y);

    if (!res->imageDirty)
    {
        res->imageDirty = TRUE;
        ResSetDirty(p->resource);
    }
}
/* we have our own line draw based on bresenham,
 * because LineTo does not fill in the end point
 */
void DrawLine(HDC hDC, POINT start, POINT end)
{
    POINT current = start;
    int deltax = end.x - start.x;
    int deltay = end.y - start.y;
    int incrementx = deltax > 0 ? 1 : -1;
    int incrementy = deltay > 0 ? 1 : -1;
    int d;
    int done = FALSE;
    if (deltax < 0)
        deltax = -deltax;
    if (deltay < 0)
        deltay = -deltay;
    if (deltax > deltay)
        d = 2 * deltay - deltax;
    else
        d = 2 * deltax - deltay;
    while (!done)
    {
        MoveToEx(hDC, current.x, current.y, NULL);
        LineTo(hDC, current.x + 1, current.y);
        if (deltax >= deltay)
        {
            if (d < 0)
                d = d + 2 * deltay;
            else
            {
                d = d + 2 * (deltay - deltax);
                current.y += incrementy;
            }
            current.x += incrementx;
        }
        else
        {
            if (d < 0)
                d = d + 2 * deltax;
            else
            {
                d = d + 2 * (deltax - deltay);
                current.x += incrementx;
            }
            current.y += incrementy;
        }
        if (end.x > start.x)
        {
            if (current.x > end.x)
                done = TRUE;
        }
        else if (current.x < end.x)
            done = TRUE;
        if (end.y > start.y)
        {
            if (current.y > end.y)
                done = TRUE;
        }
        else if (current.y < end.y)
            done = TRUE;
    }
}

void Fill(IMGDATA* p, IMAGEDATA* res, POINT pt)
{
    DWORD color;
    color = GetPixel(res->hdcImage, pt.x, pt.y);
    /* now flood fill the image */
    ExtFloodFill(res->hdcImage, pt.x, pt.y, color, FLOODFILLSURFACE);
    if (!res->imageDirty)
    {
        res->imageDirty = TRUE;
        ResSetDirty(p->resource);
    }
}
void FreeImageResource(IMAGEDATA* res)
{
    //    while (res)
    //    {
    //        IMAGEDATA *next = res->next;
    //        FreeImageUndo(res);
    //        DeleteImageDC(res);
    //        free(res);
    //        res = next;
    //    }
}
