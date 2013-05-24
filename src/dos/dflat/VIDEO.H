/* ---------------- video.h ----------------- */

#ifndef VIDEO_H
#define VIDEO_H

#include "rect.h"

void getvideo(RECT, void *);
void storevideo(RECT, void *);
extern unsigned video_mode;
extern unsigned video_page;
void wputch(WINDOW, int, int, int);
unsigned int GetVideoChar(int, int);
void PutVideoChar(int, int, int);
void get_videomode(void);
void wputs(WINDOW, void *, int, int);
void twputs(WINDOW, void *, int, int);
void scroll_window(WINDOW, RECT, int);

#define clr(fg,bg) ((fg)|((bg)<<4))
#define vad(x,y) ((y)*(SCREENWIDTH*2)+(x)*2 + video_page * 16)
#define ismono() (video_mode == 7)
#define istext() (video_mode < 4)
#define videochar(x,y) (GetVideoChar(x,y) & 255)

#endif
