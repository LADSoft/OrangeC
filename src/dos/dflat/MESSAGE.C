/* --------- message.c ---------- */

#include "dflat.h"
#define TIMERSTACKSIZE 64 * 1024

static int px = -1, py = -1;
static int pmx = -1, pmy = -1;
static int mx, my;
static int handshaking = 0;
static BOOL CriticalError;
static DPMI_MEM_HANDLE timerMemhand;
static SELECTOR dsSel;
static SELECTOR oldss;
static int oldsp;
static BOOL hasints;

static unsigned char timerLast[6];
static unsigned char keyboardLast[6];
static char *stackbot;
BOOL AllocTesting = FALSE;
jmp_buf AllocError;
BOOL AltDown = FALSE;

/* ---------- event queue ---------- */
static struct events    {
    MESSAGE event;
    int mx;
    int my;
} EventQueue[MAXMESSAGES];

/* ---------- message queue --------- */
static struct msgs {
    WINDOW wnd;
    MESSAGE msg;
    PARAM p1;
    PARAM p2;
} MsgQueue[MAXMESSAGES];

static int EventQueueOnCtr;
static int EventQueueOffCtr;
static int EventQueueCtr;

static int MsgQueueOnCtr;
static int MsgQueueOffCtr;
static int MsgQueueCtr;

static int lagdelay = FIRSTDELAY;

static int keyportvalue;	/* for watching for key release */

WINDOW CaptureMouse;
WINDOW CaptureKeyboard;
static BOOL NoChildCaptureMouse;
static BOOL NoChildCaptureKeyboard;

static int doubletimer = -1;
static int delaytimer  = -1;
static int clocktimer  = -1;
char time_string[] = "         ";

static WINDOW Cwnd;

static void _interrupt newkeyboard(void)
{
	asm push ds
	asm push es
	asm mov ds, cs:[dsSel]
	asm mov es, cs:[dsSel]
	keyportvalue = inp(KEYBOARDPORT);
	asm pushfd
	asm call fword [keyboardLast];
	asm pop es
	asm pop ds
}

/* ------- timer interrupt service routine ------- */
static void _interrupt newtimer(void)
{
    asm push ds
    asm push es
    asm mov ax,cs:[dsSel] ;
    asm mov ds,ax
    asm mov es,ax
	asm mov ecx,[stackbot] ;
	asm add	ecx,TIMERSTACKSIZE-4 ;
	asm mov	[oldss],ss 
	asm mov	[oldsp],esp
	asm mov	ss,ax ;
	asm mov	esp,ecx

    if (timer_running(doubletimer))
        countdown(doubletimer);
    if (timer_running(delaytimer))
        countdown(delaytimer);
    if (timer_running(clocktimer))
        countdown(clocktimer);
	asm pushfd
	asm call fword [timerLast];
	asm mov ss,[oldss]
	asm mov esp,[oldsp]
    asm pop es
    asm pop ds
}
static char ermsg[] = "Error accessing drive x";

/* -------- test for critical errors --------- */
int TestCriticalError(void)
{
    int rtn = 0;
    if (CriticalError)    {
        rtn = 1;
        CriticalError = FALSE;
        if (TestErrorMessage(ermsg) == FALSE)
            rtn = 2;
    }
    return rtn;
}

/* ------ critical error interrupt service routine ------ */
static void _interrupt newcrit(IREGS ir)
{
	asm push ds
	asm push es
	asm mov ds, cs:[dsSel]
	asm mov es, cs:[dsSel]
    if (!(ir.ax & 0x8000))     {
        ermsg[sizeof(ermsg) - 2] = (ir.ax & 0xff) + 'A';
        CriticalError = TRUE;
    }
    ir.ax = 0;
	asm pop es
	asm pop ds
}

void StopMsg(void)
{
	if (hasints)
	{
        dpmi_set_protected_interrupt(TIMER,*(SELECTOR *)((unsigned char *)&timerLast + 4),*(ULONG *)&timerLast) ;
		dpmi_free_memory(timerMemhand) ;
        dpmi_set_protected_interrupt(KEYBOARDVECT,*(SELECTOR *)((unsigned char *)&keyboardLast + 4),*(ULONG *)&keyboardLast) ;
	    hasints = FALSE;
	}
	ClearClipboard();
	ClearDialogBoxes();
	ConsoleRundown();
    hide_mousecursor();
}

/* ------------ initialize the message system --------- */
BOOL init_messages(void)
{
	asm mov [dsSel],ds
	AllocTesting = TRUE;
//	if (setjmp(AllocError) != 0)	{ // this is very bad...
//		StopMsg();
//		return FALSE;
//	}
    resetmouse();
	set_mousetravel(0, SCREENWIDTH-1, 0, SCREENHEIGHT-1);
	ConsoleInit();
    px = py = -1;
    pmx = pmy = -1;
    mx = my = 0;
    CaptureMouse = CaptureKeyboard = NULL;
    NoChildCaptureMouse = FALSE;
    NoChildCaptureKeyboard = FALSE;
    MsgQueueOnCtr = MsgQueueOffCtr = MsgQueueCtr = 0;
    EventQueueOnCtr = EventQueueOffCtr = EventQueueCtr = 0;
    if (!hasints)    {
        SELECTOR cssel, dssel ;
		ULONG linear;
        asm mov [cssel],cs
		asm mov [dssel],ds
		dpmi_get_memory(&stackbot,&timerMemhand, TIMERSTACKSIZE) ;
		dpmi_get_sel_base(&linear, dssel);
		stackbot -= linear;
		dpmi_get_protected_interrupt(((unsigned char *)&timerLast + 4), &timerLast, TIMER) ;
        dpmi_set_protected_interrupt(TIMER,cssel,(ULONG)newtimer) ;

		dpmi_get_protected_interrupt(((unsigned char *)&keyboardLast + 4), &keyboardLast, KEYBOARDVECT) ;
        dpmi_set_protected_interrupt(KEYBOARDVECT,cssel,(ULONG)newkeyboard) ;

	    dpmi_set_protected_interrupt(CRIT,cssel,(ULONG)newcrit) ;
		hasints = TRUE;
    }
    PostMessage(NULL,START,0,0);
    lagdelay = FIRSTDELAY;
	return TRUE;
}

/* ----- post an event and parameters to event queue ---- */
static void PostEvent(MESSAGE event, int p1, int p2)
{
    if (EventQueueCtr != MAXMESSAGES)    {
        EventQueue[EventQueueOnCtr].event = event;
        EventQueue[EventQueueOnCtr].mx = p1;
        EventQueue[EventQueueOnCtr].my = p2;
        if (++EventQueueOnCtr == MAXMESSAGES)
            EventQueueOnCtr = 0;
        EventQueueCtr++;
    }
}

/* ------ collect mouse, clock, and keyboard events ----- */
static void near collect_events(void)
{
    static int ShiftKeys = 0;
	int sk;
    struct tm *now;
    static BOOL flipflop = FALSE;
    int hr;

    /* -------- test for a clock event (one/second) ------- */
    if (timed_out(clocktimer))    {
        /* ----- get the current time ----- */
        time_t t = time(NULL);
        now = localtime(&t);
        hr = now->tm_hour > 12 ?
             now->tm_hour - 12 :
             now->tm_hour;
        if (hr == 0)
            hr = 12;
        sprintf(time_string, "%2d:%02d", hr, now->tm_min);
        strcpy(time_string+5, now->tm_hour > 11 ? "pm " : "am ");
        /* ------- blink the : at one-second intervals ----- */
        if (flipflop)
            *(time_string+2) = ' ';
        flipflop ^= TRUE;
        /* -------- reset the timer -------- */
        set_timer(clocktimer, 1);
        /* -------- post the clock event -------- */
        PostEvent(CLOCKTICK, FP_SEG(time_string), FP_OFF(time_string));
    }

    /* --------- keyboard events ---------- */
    if ((sk = getshift()) != ShiftKeys)    {
        ShiftKeys = sk;
        /* ---- the shift status changed ---- */
        PostEvent(SHIFT_CHANGED, sk, 0);
    	if (sk & ALTKEY)
			AltDown = TRUE;
    }

    /* ---- build keyboard events for key combinations that
        BIOS doesn't report --------- */
    if (sk & ALTKEY)	{
        if (keyportvalue == 14)    {
			AltDown = FALSE;
			waitforkeyboard();
            PostEvent(KEYBOARD, ALT_BS, sk);
        }
        if (keyportvalue == 83)    {
			AltDown = FALSE;
			waitforkeyboard();
            PostEvent(KEYBOARD, ALT_DEL, sk);
        }
	}
    if (sk & CTRLKEY)	{
		AltDown = FALSE;
        if (keyportvalue == 82)    {
			waitforkeyboard();
            PostEvent(KEYBOARD, CTRL_INS, sk);
        }
	}
    /* ----------- test for keystroke ------- */
    if (keyhit())    {
        static int cvt[] = {SHIFT_INS,END,DN,PGDN,BS,'5',
                        FWD,HOME,UP,PGUP};
        int c = getkey();
		AltDown = FALSE;
        /* -------- convert numeric pad keys ------- */
        if (sk & (LEFTSHIFT | RIGHTSHIFT))    {
            if (c >= '0' && c <= '9')
                c = cvt[c-'0'];
            else if (c == '.' || c == DEL)
                c = SHIFT_DEL;
            else if (c == INS)
                c = SHIFT_INS;
        }
		if (c != '\r' && (c < ' ' || c > 127))
			clearBIOSbuffer();
        /* ------ post the keyboard event ------ */
        PostEvent(KEYBOARD, c, sk);
    }
    /* ------------ test for mouse events --------- */
    if (button_releases())    {
        /* ------- the button was released -------- */
		AltDown = FALSE;
        doubletimer = DOUBLETICKS;
        PostEvent(BUTTON_RELEASED, mx, my);
        disable_timer(delaytimer);
    }
    get_mouseposition(&mx, &my);
    if (mx != px || my != py)  {
        px = mx;
        py = my;
        PostEvent(MOUSE_MOVED, mx, my);
    }
    if (rightbutton())	{
		AltDown = FALSE;
        PostEvent(RIGHT_BUTTON, mx, my);
	}
    if (leftbutton())    {
		AltDown = FALSE;
        if (mx == pmx && my == pmy)    {
            /* ---- same position as last left button ---- */
            if (timer_running(doubletimer))    {
                /* -- second click before double timeout -- */
                disable_timer(doubletimer);
                PostEvent(DOUBLE_CLICK, mx, my);
            }
            else if (!timer_running(delaytimer))    {
                /* ---- button held down a while ---- */
                delaytimer = lagdelay;
                lagdelay = DELAYTICKS;
                /* ---- post a typematic-like button ---- */
                PostEvent(LEFT_BUTTON, mx, my);
            }
        }
        else    {
            /* --------- new button press ------- */
            disable_timer(doubletimer);
            delaytimer = FIRSTDELAY;
            lagdelay = DELAYTICKS;
            PostEvent(LEFT_BUTTON, mx, my);
            pmx = mx;
            pmy = my;
        }
    }
    else
        lagdelay = FIRSTDELAY;
}

/* ----- post a message and parameters to msg queue ---- */
void PostMessage(WINDOW wnd, MESSAGE msg, PARAM p1, PARAM p2)
{
    if (MsgQueueCtr != MAXMESSAGES)    {
        MsgQueue[MsgQueueOnCtr].wnd = wnd;
        MsgQueue[MsgQueueOnCtr].msg = msg;
        MsgQueue[MsgQueueOnCtr].p1 = p1;
        MsgQueue[MsgQueueOnCtr].p2 = p2;
        if (++MsgQueueOnCtr == MAXMESSAGES)
            MsgQueueOnCtr = 0;
        MsgQueueCtr++;
    }
}

/* --------- send a message to a window ----------- */
int SendMessage(WINDOW wnd, MESSAGE msg, PARAM p1, PARAM p2)
{
    int rtn = TRUE, x, y;

#ifdef INCLUDE_LOGGING
	LogMessages(wnd, msg, p1, p2);
#endif
    if (wnd != NULL)
        switch (msg)    {
            case PAINT:
            case BORDER:
                /* ------- don't send these messages unless the
                    window is visible -------- */
                if (isVisible(wnd))
	                rtn = (*wnd->wndproc)(wnd, msg, p1, p2);
                break;
            case RIGHT_BUTTON:
            case LEFT_BUTTON:
            case DOUBLE_CLICK:
            case BUTTON_RELEASED:
                /* --- don't send these messages unless the
                    window is visible or has captured the mouse -- */
                if (isVisible(wnd) || wnd == CaptureMouse)
	                rtn = (*wnd->wndproc)(wnd, msg, p1, p2);
                break;
            case KEYBOARD:
            case SHIFT_CHANGED:
                /* ------- don't send these messages unless the
                    window is visible or has captured the keyboard -- */
                if (!(isVisible(wnd) || wnd == CaptureKeyboard))
	                break;
            default:
                rtn = (*wnd->wndproc)(wnd, msg, p1, p2);
                break;
        }
    /* ----- window processor returned true or the message was sent
        to no window at all (NULL) ----- */
    if (rtn != FALSE)    {
        /* --------- process messages that a window sends to the
            system itself ---------- */
        switch (msg)    {
            case STOP:
				StopMsg();
                break;
            /* ------- clock messages --------- */
            case CAPTURE_CLOCK:
				if (Cwnd == NULL)
	                set_timer(clocktimer, 0);
				wnd->PrevClock = Cwnd;
                Cwnd = wnd;
                break;
            case RELEASE_CLOCK:
                Cwnd = wnd->PrevClock;
				if (Cwnd == NULL)
	                disable_timer(clocktimer);
                break;
            /* -------- keyboard messages ------- */
            case KEYBOARD_CURSOR:
                if (wnd == NULL)
                    cursor((int)p1, (int)p2);
                else if (wnd == inFocus)
                    cursor(GetClientLeft(wnd)+(int)p1,
                                GetClientTop(wnd)+(int)p2);
                break;
            case CAPTURE_KEYBOARD:
                if (p2)
                    ((WINDOW)p2)->PrevKeyboard=CaptureKeyboard;
                else
                    wnd->PrevKeyboard = CaptureKeyboard;
                CaptureKeyboard = wnd;
                NoChildCaptureKeyboard = (int)p1;
                break;
            case RELEASE_KEYBOARD:
				if (wnd != NULL)	{
					if (CaptureKeyboard == wnd || (int)p1)
	                	CaptureKeyboard = wnd->PrevKeyboard;
					else	{
						WINDOW twnd = CaptureKeyboard;
						while (twnd != NULL)	{
							if (twnd->PrevKeyboard == wnd)	{
								twnd->PrevKeyboard = wnd->PrevKeyboard;
								break;
							}
							twnd = twnd->PrevKeyboard;
						}
						if (twnd == NULL)
							CaptureKeyboard = NULL;
					}
                	wnd->PrevKeyboard = NULL;
				}
				else
					CaptureKeyboard = NULL;
                NoChildCaptureKeyboard = FALSE;
                break;
            case CURRENT_KEYBOARD_CURSOR:
                curr_cursor(&x, &y);
                *(int*)p1 = x;
                *(int*)p2 = y;
                break;
            case SAVE_CURSOR:
                savecursor();
                break;
            case RESTORE_CURSOR:
                restorecursor();
                break;
            case HIDE_CURSOR:
                normalcursor();
                hidecursor();
                break;
            case SHOW_CURSOR:
                if (p1)
                    set_cursor_type(0x0607);
                else
                    set_cursor_type(0x0106);
                unhidecursor();
                break;
			case WAITKEYBOARD:
				waitforkeyboard();
				break;
            /* -------- mouse messages -------- */
			case RESET_MOUSE:
				resetmouse();
				set_mousetravel(0, SCREENWIDTH-1, 0, SCREENHEIGHT-1);
				break;
            case MOUSE_INSTALLED:
                rtn = mouse_installed();
                break;
			case MOUSE_TRAVEL:	{
				RECT rc;
				if (!p1)	{
        			rc.lf = rc.tp = 0;
        			rc.rt = SCREENWIDTH-1;
        			rc.bt = SCREENHEIGHT-1;
				}
				else 
					rc = *(RECT *)p1;
				set_mousetravel(rc.lf, rc.rt, rc.tp, rc.bt);
				break;
			}
            case SHOW_MOUSE:
                show_mousecursor();
                break;
            case HIDE_MOUSE:
                hide_mousecursor();
                break;
            case MOUSE_CURSOR:
                set_mouseposition((int)p1, (int)p2);
                break;
            case CURRENT_MOUSE_CURSOR:
                get_mouseposition((int*)p1,(int*)p2);
                break;
            case WAITMOUSE:
                waitformouse();
                break;
            case TESTMOUSE:
                rtn = mousebuttons();
                break;
            case CAPTURE_MOUSE:
                if (p2)
                    ((WINDOW)p2)->PrevMouse = CaptureMouse;
                else
                    wnd->PrevMouse = CaptureMouse;
                CaptureMouse = wnd;
                NoChildCaptureMouse = (int)p1;
                break;
            case RELEASE_MOUSE:
				if (wnd != NULL)	{
					if (CaptureMouse == wnd || (int)p1)
	                	CaptureMouse = wnd->PrevMouse;
					else	{
						WINDOW twnd = CaptureMouse;
						while (twnd != NULL)	{
							if (twnd->PrevMouse == wnd)	{
								twnd->PrevMouse = wnd->PrevMouse;
								break;
							}
							twnd = twnd->PrevMouse;
						}
						if (twnd == NULL)
							CaptureMouse = NULL;
					}
                	wnd->PrevMouse = NULL;
				}
				else
					CaptureMouse = NULL;
                NoChildCaptureMouse = FALSE;
                break;
            default:
                break;
        }
    }
    return rtn;
}

static RECT VisibleRect(WINDOW wnd)
{
	RECT rc = WindowRect(wnd);
	if (!TestAttribute(wnd, NOCLIP))	{
		WINDOW pwnd = GetParent(wnd);
		RECT prc;
		prc = ClientRect(pwnd);
		while (pwnd != NULL)	{
			if (TestAttribute(pwnd, NOCLIP))
				break;
			rc = subRectangle(rc, prc);
			if (!ValidRect(rc))
				break;
			if ((pwnd = GetParent(pwnd)) != NULL)
				prc = ClientRect(pwnd);
		}
	}
	return rc;
}

/* ----- find window that mouse coordinates are in --- */
static WINDOW inWindow(WINDOW wnd, int x, int y)
{
	WINDOW Hit = NULL;
	while (wnd != NULL)	{
		if (isVisible(wnd))	{
			WINDOW wnd1;
			RECT rc = VisibleRect(wnd);
			if (InsideRect(x, y, rc))
				Hit = wnd;
			if ((wnd1 = inWindow(LastWindow(wnd), x, y)) != NULL)
				Hit = wnd1;
			if (Hit != NULL)
				break;
		}
		wnd = PrevWindow(wnd);
	}
	return Hit;
}

static WINDOW MouseWindow(int x, int y)
{
    /* ------ get the window in which a
                    mouse event occurred ------ */
    WINDOW Mwnd = inWindow(ApplicationWindow, x, y);
    /* ---- process mouse captures ----- */
    if (CaptureMouse != NULL)	{
        if (NoChildCaptureMouse ||
				Mwnd == NULL 	||
					!isAncestor(Mwnd, CaptureMouse))
            Mwnd = CaptureMouse;
	}
	return Mwnd;
}

void handshake(void)
{
	handshaking++;
	dispatch_message();
	--handshaking;
}

/* ---- dispatch messages to the message proc function ---- */
BOOL dispatch_message(void)
{
    WINDOW Mwnd, Kwnd;
    /* -------- collect mouse and keyboard events ------- */
    collect_events();
    /* --------- dequeue and process events -------- */
    while (EventQueueCtr > 0)  {
        struct events ev;
			
		ev = EventQueue[EventQueueOffCtr];
        if (++EventQueueOffCtr == MAXMESSAGES)
            EventQueueOffCtr = 0;
        --EventQueueCtr;

        /* ------ get the window in which a
                        keyboard event occurred ------ */
        Kwnd = inFocus;

        /* ---- process keyboard captures ----- */
        if (CaptureKeyboard != NULL)
            if (Kwnd == NULL ||
                    NoChildCaptureKeyboard ||
						!isAncestor(Kwnd, CaptureKeyboard))
                Kwnd = CaptureKeyboard;

        /* -------- send mouse and keyboard messages to the
            window that should get them -------- */
        switch (ev.event)    {
            case SHIFT_CHANGED:
            case KEYBOARD:
				if (!handshaking)
	                SendMessage(Kwnd, ev.event, ev.mx, ev.my);
                break;
            case LEFT_BUTTON:
				if (!handshaking)	{
		        	Mwnd = MouseWindow(ev.mx, ev.my);
                	if (!CaptureMouse ||
                        	(!NoChildCaptureMouse &&
								isAncestor(Mwnd, CaptureMouse)))
                    	if (Mwnd != inFocus)
                        	SendMessage(Mwnd, SETFOCUS, TRUE, 0);
                	SendMessage(Mwnd, LEFT_BUTTON, ev.mx, ev.my);
				}
                break;
            case BUTTON_RELEASED:
            case DOUBLE_CLICK:
            case RIGHT_BUTTON:
				if (handshaking)
					break;
            case MOUSE_MOVED:
		        Mwnd = MouseWindow(ev.mx, ev.my);
                SendMessage(Mwnd, ev.event, ev.mx, ev.my);
                break;
            case CLOCKTICK:
                SendMessage(Cwnd, ev.event, ev.mx, ev.my);
				break;
            default:
                break;
        }
    }
    /* ------ dequeue and process messages ----- */
    while (MsgQueueCtr > 0)  {
        struct msgs mq;

		mq = MsgQueue[MsgQueueOffCtr];
        if (++MsgQueueOffCtr == MAXMESSAGES)
            MsgQueueOffCtr = 0;
        --MsgQueueCtr;
        SendMessage(mq.wnd, mq.msg, mq.p1, mq.p2);
        if (mq.msg == ENDDIALOG)
			return FALSE;
        if (mq.msg == STOP)	{
		    PostMessage(NULL, STOP, 0, 0);
			return FALSE;
		}
    }
    return TRUE;
}


