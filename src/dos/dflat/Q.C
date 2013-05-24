#include <dos.h>
#define vad(x,y) ((y)*(SCREENWIDTH*2)+(x)*2 + video_page * 16)
#define poke(a,b,c)  (*((short  far*)MK_FP((a),(b))) = (int)(c))
#define SCREENWIDTH  (peekb(__seg0040,0x4a) & 255)
#define peekb(a,b)   (*((char far*)MK_FP((a),(b))))
short video_address;
int video_page;
short __seg0040;
void near vpoke(short far *vp, int c);
main()
{
	int ch;
	int xc,yc;
	vpoke(MK_FP(video_address, vad(xc, yc)), ch);
	poke(video_address, vad(xc, yc), ch);
}
