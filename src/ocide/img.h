#define IMAGE_UNDO_MAX 10

#define RGB_WHITE RGB(255, 255, 255)
#define RGB_BLACK RGB(0, 0, 0)
#define RGB_DEFAULT_BACKGROUND RGB(127, 191, 255) /* light blue */

#define DSPDxax 0x00E20746L

#define MAX_COLORS 28

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
    int withAlpha : 1;
    int height;
    int width;
    int colors;
    int saveColors;
    int hotspotX;
    int hotspotY;
    int DIBSize;
    HBITMAP hbmpImage;
    HDC hdcImage;
    HBITMAP hbmpAndMask;
    HDC hdcAndMask;
    unsigned char* bits;
    BITMAPINFO* bitmapInfo;
    DWORD rgbScreen;
    void* origData;
} IMAGEDATA;

typedef struct _rubber
{
    enum
    {
        RT_LINE,
        RT_RECTANGLE,
        RT_ROUNDEDRECTANGLE,
        RT_ELLIPSE
    } type;
    enum
    {
        FT_HOLLOW,
        FT_BORDER,
        FT_FILL
    } filltype;
    int x;
    int y;
    int width;
    int height;
    int bmpwidth;
    int bmpheight;
    HBITMAP hbmpRubberBorder;
    HDC hdcRubberBorder;
    HBITMAP hbmpRubberFill;
    HDC hdcRubberFill;
} RUBBER;

#define POLY_POINTS_MAX 20
typedef struct _imgdata
{
    SHORT cbImgData; /* needed since we are passing this through CreateWindow
                      * as an lparam
                      */
    int drawMode;
    int lastDrawMode;

    int captured : 1;
    int mouseInClient : 1;
    int right : 1;  // right button
    int polyStarted : 1;
    int selected : 1;  // is it selected
    int skipSelect : 1;
    int sizehorz : 1;
    int sizevert : 1;
    int showFontTB : 1;
    int shownCaret : 1;
    // for scrolling
    int showX, showY;
    int maxX, maxY;

    int lineWidth;
    int eraseWidth;
    int fillType;
    int brushType;
    int polyPointCount;
    char textBuf[512];
    int textLen;

    HFONT drawFont;
    HCURSOR cursor;
    POINT cursorPos;
    POINT polyOrigin;
    POINT polyPoints[POLY_POINTS_MAX + 1];
    RECT selectRect;
    POINT selectMovePos, selectStartPos;
    POINT sizePos;

    HDC selectDC;
    HBITMAP selectbmp;

    IMAGEDATA* res;
    IMAGEDATA* resList;
    RUBBER* rubber;

    HWND hwndSelf;
    HWND hwndStatus;
    HWND hwndControlToolbar;
    HWND hwndColorToolbar;
    HWND hwndSelectToolbar;
    HWND hwndColorSelectToolbar;
    HWND hwndScreenColorToolbar;
    HWND hwndControl;
    HWND hwndAux;
    HWND hwndColor, hwndColor2, hwndScreenColor;
    HWND hwndFontTB, hwndFontCombo, hwndPointSizeCombo;
    HWND hwndImageTab, hwndWorkspace, hwndDrawArea;

    HWND hwndEraseToolbar;
    HWND hwndFillToolbar;
    HWND hwndLineToolbar;
    HWND hwndZoomToolbar;
    HWND hwndBrushToolbar;
    HWND hwndSelectedToolbar;

    RECT controlTbSize, colorTbSize, selectTbSize, screenColorTbSize;

    DWORD screenColor;

    DWORD currentLeftColor;
    DWORD currentRightColor;

    DWORD currentPalette[MAX_COLORS];
    int zoom;
    struct resRes* resource;

} IMGDATA;

#include "imgdraw.p"
