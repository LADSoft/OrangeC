                              /* Imgdraw.c */

void DeleteImageDC(IMAGEDATA *res); /* PROTOTYPE */
void ResizeImage(HWND hwndParent, IMGDATA *p, IMAGEDATA *res, int width, int height, BOOL stretch);
void InsertImageUndo(HWND hwndParent, IMAGEDATA *res);
void FreeImageUndo(IMAGEDATA *res);
void DirtyImageUndo(IMAGEDATA *res);
void ApplyImageUndo(IMGDATA *p, IMAGEDATA *res);
HBITMAP LocalCreateBitmap(HDC dc, int width, int height, int colors);
int CreateImageDC(HWND hwndParent, IMAGEDATA *res);
void DeleteImageDC(IMAGEDATA *res);
void ClearImageDC(IMAGEDATA *res);
void RecoverAndMask(IMAGEDATA *res);
void SplitImageDC(IMAGEDATA *res);
void CombineImageDC(IMAGEDATA *res);
void ChangeImageSize(IMAGEDATA *res, int width, int height);
void ChangeImageColorCount(IMAGEDATA *res, int colors);
int CopyImageToClipboard(HWND hwnd, IMAGEDATA *res, RECT *pick);
IMAGEDATA *PasteImageFromClipboard(HWND hwnd, IMAGEDATA *match);
int Pastable(HWND hwnd);
RUBBER *CreateRubberBand(IMAGEDATA *res, int type, int filltype, POINT pt);
void DeleteRubberBand(IMGDATA *p, RUBBER *r);
void RubberBand(IMGDATA *p, RUBBER *r, POINT pt);
void InstantiateRubberBand(IMGDATA *p, IMAGEDATA *res, RUBBER *r);
void DrawPoint(IMGDATA *p, IMAGEDATA *res, POINT pt);
void DrawLine(HDC hDC, POINT start, POINT end);
void Fill(IMGDATA *p, IMAGEDATA *res, POINT pt);
void FreeImageResource(IMAGEDATA *res);
