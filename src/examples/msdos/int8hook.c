#include <dpmi.h>

volatile int vv ;
static DPMI_REGS regs ;
static UWORD dsSel ;
void _interrupt func()
{
        asm push ds
        asm mov ds,cs:[dsSel]
        vv++ ;
        dpmi_simulate_proc_int_frame(&regs) ;
        asm pop ds
}
main()
{
        SELECTOR cssel ;
        asm mov [cssel],cs
        asm mov [dsSel],ds
        regs.h.flags = 0x46 ;
        regs.h.ip = *(short *)(8 * 4) ;
        regs.h.cs = *(short *)(8 * 4 + 2) ;
        dpmi_set_protected_interrupt(8,cssel,(ULONG)func) ;
        while (!kbhit())
                printf("%d\n",vv) ;
}