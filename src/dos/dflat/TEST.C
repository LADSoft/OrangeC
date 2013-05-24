/* --------------- memopad.c ----------- */

#include "dflat.h"

extern DBOX PrintSetup;

char DFlatApplication[] = "MemoPad";

static char Untitled[] = "Untitled";
static int wndpos;

static int MemoPadProc(WINDOW, MESSAGE, PARAM, PARAM);
static void NewFile(WINDOW);
static void SelectFile(WINDOW);
static void PadWindow(WINDOW, char *);
static void OpenPadWindow(WINDOW, char *);
static void LoadFile(WINDOW);
static void PrintPad(WINDOW);
static void SaveFile(WINDOW, int);
static void DeleteFile(WINDOW);
static int EditorProc(WINDOW, MESSAGE, PARAM, PARAM);
static char *NameComponent(char *);
static int PrintSetupProc(WINDOW, MESSAGE, PARAM, PARAM);
static void FixTabMenu(void);

#define CHARSLINE 80
#define LINESPAGE 66

void main(int argc, char *argv[])
{
    WINDOW wnd;
    if (!init_messages())
		return;
    Argv = argv;
	if (!LoadConfig())
		cfg.ScreenLines = SCREENHEIGHT;
    wnd = CreateWindow(APPLICATION,
                        "D-Flat MemoPad " VERSION,
                        0, 0, -1, -1,
                        &MainMenu,
                        NULL,
                        MemoPadProc,
                        MOVEABLE  |
                        SIZEABLE  |
                        HASBORDER |
						MINMAXBOX |
                        HASSTATUSBAR
                        );

    LoadHelpFile(DFlatApplication);
    SendMessage(wnd, SETFOCUS, TRUE, 0);
    while (argc > 1)    {
        PadWindow(wnd, argv[1]);
        --argc;
        argv++;
    }
    while (dispatch_message())
        ;
}
/* ------ open text files and put them into editboxes ----- */
static void PadWindow(WINDOW wnd, char *FileName)
{
    int ax, criterr = 1;
    struct ffblk ff;
    char path[MAXPATH+1];
    char *cp;

    CreatePath(path, FileName, FALSE, FALSE);
    cp = path+strlen(path);
    CreatePath(path, FileName, TRUE, FALSE);
    while (criterr == 1)    {
        ax = findfirst(path, &ff, 0);
        criterr = TestCriticalError();
    }
    while (ax == 0 && !criterr)    {
        strcpy(cp, ff.ff_name);
        OpenPadWindow(wnd, path);
        ax = findnext(&ff);
    }
}
/* ------- window processing module for the
                    memopad application window ----- */
static int MemoPadProc(WINDOW wnd,MESSAGE msg,PARAM p1,PARAM p2)
{
	int rtn;
    switch (msg)    {
		case CREATE_WINDOW:
		    rtn = DefaultWndProc(wnd, msg, p1, p2);
			if (cfg.InsertMode)
				SetCommandToggle(&MainMenu, ID_INSERT);
			if (cfg.WordWrap)
				SetCommandToggle(&MainMenu, ID_WRAP);
			FixTabMenu();
			return rtn;
        case COMMAND:
            switch ((int)p1)    {
                case ID_NEW:
                    NewFile(wnd);
                    return TRUE;
                case ID_OPEN:
                    SelectFile(wnd);
                    return TRUE;
                case ID_SAVE:
                    SaveFile(inFocus, FALSE);
                    return TRUE;
                case ID_SAVEAS:
                    SaveFile(inFocus, TRUE);
                    return TRUE;
                case ID_DELETEFILE:
                    DeleteFile(inFocus);
                    return TRUE;
				case ID_PRINTSETUP:
					DialogBox(wnd, &PrintSetup, TRUE, PrintSetupProc);
					return TRUE;
                case ID_PRINT:
                    PrintPad(inFocus);
                    return TRUE;
				case ID_EXIT:	
					if (!YesNoBox("Exit Memopad?"))
						return FALSE;
					break;
				case ID_WRAP:
			        cfg.WordWrap = GetCommandToggle(&MainMenu, ID_WRAP);
    	            return TRUE;
				case ID_INSERT:
			        cfg.InsertMode = GetCommandToggle(&MainMenu, ID_INSERT);
    	            return TRUE;
				case ID_TAB2:
					cfg.Tabs = 2;
					FixTabMenu();
                    return TRUE;
				case ID_TAB4:
					cfg.Tabs = 4;
					FixTabMenu();
                    return TRUE;
				case ID_TAB6:
					cfg.Tabs = 6;					
					FixTabMenu();
                    return TRUE;
				case ID_TAB8:
					cfg.Tabs = 8;
					FixTabMenu();
                    return TRUE;
                default:
                    break;
            }
            break;
        default:
            break;
    }
    return DefaultWndProc(wnd, msg, p1, p2);
}
/* --- The New command. Open an empty editor window --- */
static void NewFile(WINDOW wnd)
{
    OpenPadWindow(wnd, Untitled);
}
/* --- The Open... command. Select a file  --- */
static void SelectFile(WINDOW wnd)
{
    char FileName[15];
    if (OpenFileDialogBox("*.*", FileName))    {
        /* --- see if the document is already in a window --- */
        WINDOW wnd1 = FirstWindow(wnd);
        while (wnd1 != NULL)    {
            if (stricmp(FileName, wnd1->extension) == 0)    {
                SendMessage(wnd1, SETFOCUS, TRUE, 0);
                SendMessage(wnd1, RESTORE, 0, 0);
                return;
            }
            wnd1 = NextWindow(wnd1);
        }
        OpenPadWindow(wnd, FileName);
    }
}

/* --- open a document window and load a file --- */
static void OpenPadWindow(WINDOW wnd, char *FileName)
{
    static WINDOW wnd1 = NULL;
	WINDOW wwnd;
    struct stat sb;
    char *Fname = FileName;
    char *ermsg;
    if (strcmp(FileName, Untitled))    {
        if (stat(FileName, &sb))    {
            ermsg = DFmalloc(strlen(FileName)+20);
            strcpy(ermsg, "No such file as\n");
            strcat(ermsg, FileName);
            ErrorMessage(ermsg);
            free(ermsg);
            return;
        }
        Fname = NameComponent(FileName);
    }
	wwnd = WatchIcon();
    wndpos += 2;
    if (wndpos == 20)
        wndpos = 2;
    wnd1 = CreateWindow(EDITBOX,
                Fname,
                (wndpos-1)*2, wndpos, 10, 40,
                NULL, wnd, EditorProc,
                SHADOW     |
                MINMAXBOX  |
                CONTROLBOX |
                VSCROLLBAR |
                HSCROLLBAR |
                MOVEABLE   |
                HASBORDER  |
                SIZEABLE   |
                MULTILINE
    );
    if (strcmp(FileName, Untitled))    {
        wnd1->extension = DFmalloc(strlen(FileName)+1);
        strcpy(wnd1->extension, FileName);
        LoadFile(wnd1);
    }
	SendMessage(wwnd, CLOSE_WINDOW, 0, 0);
    SendMessage(wnd1, SETFOCUS, TRUE, 0);
}
/* --- Load the notepad file into the editor text buffer --- */
static void LoadFile(WINDOW wnd)
{
    char *Buf = NULL;
	int recptr = 0;
    FILE *fp;

    if ((fp = fopen(wnd->extension, "rt")) != NULL)    {
		while (!feof(fp))	{
			handshake();
			Buf = DFrealloc(Buf, recptr+150);
			memset(Buf+recptr, 0, 150);
        	fgets(Buf+recptr, 150, fp);
			recptr += strlen(Buf+recptr);
		}
        fclose(fp);
		if (Buf != NULL)	{
	        SendMessage(wnd, SETTEXT, (PARAM) Buf, 0);
		    free(Buf);
		}
    }
}

static int LineCtr;
static int CharCtr;

/* ------- print a character -------- */
static void PrintChar(FILE *prn, int c)
{
	int i;
    if (c == '\n' || CharCtr == cfg.RightMargin)	{
		fputs("\r\n", prn);
		LineCtr++;
		if (LineCtr == cfg.BottomMargin)	{
    		fputc('\f', prn);
			for (i = 0; i < cfg.TopMargin; i++)
	    		fputc('\n', prn);
			LineCtr = cfg.TopMargin;
		}
		CharCtr = 0;
		if (c == '\n')
			return;
	}
	if (CharCtr == 0)	{
		for (i = 0; i < cfg.LeftMargin; i++)	{
			fputc(' ', prn);
			CharCtr++;
		}
	}
	CharCtr++;
    fputc(c, prn);
}

/* --- print the current notepad --- */
static void PrintPad(WINDOW wnd)
{
	if (*cfg.PrinterPort)	{
		FILE *prn;
		if ((prn = fopen(cfg.PrinterPort, "wt")) != NULL)	{
			long percent;
			BOOL KeepPrinting = TRUE;
		    unsigned char *text = GetText(wnd);
			unsigned oldpct = 100, cct = 0, len = strlen(text);
			WINDOW swnd = SliderBox(20, GetTitle(wnd), "Printing");
    		/* ------- print the notepad text --------- */
			LineCtr = CharCtr = 0;
			while (KeepPrinting && *text)	{
				PrintChar(prn, *text++);
				percent = ((long) ++cct * 100) / len;
				if ((int) percent != oldpct)	{
					oldpct = (int) percent;
					KeepPrinting = SendMessage(swnd, PAINT, 0, oldpct);
				}
    		}
			if (KeepPrinting)
				/* ---- user did not cancel ---- */
				if (oldpct < 100)
					SendMessage(swnd, PAINT, 0, 100);
   			/* ------- follow with a form feed? --------- */
   			if (YesNoBox("Form Feed?"))
       			fputc('\f', prn);
			fclose(prn);
		}
		else
			ErrorMessage("Cannot open printer file");
	}
	else
		ErrorMessage("No printer selected");
}

/* ---------- save a file to disk ------------ */
static void SaveFile(WINDOW wnd, int Saveas)
{
    FILE *fp;
    if (wnd->extension == NULL || Saveas)    {
        char FileName[15];
        if (SaveAsDialogBox("*.*", NULL, FileName))    {
            if (wnd->extension != NULL)
                free(wnd->extension);
            wnd->extension = DFmalloc(strlen(FileName)+1);
            strcpy(wnd->extension, FileName);
            AddTitle(wnd, NameComponent(FileName));
            SendMessage(wnd, BORDER, 0, 0);
        }
        else
            return;
    }
    if (wnd->extension != NULL)    {
        WINDOW mwnd = MomentaryMessage("Saving the file");
        if ((fp = fopen(wnd->extension, "wt")) != NULL)    {
            fwrite(GetText(wnd), strlen(GetText(wnd)), 1, fp);
            fclose(fp);
            wnd->TextChanged = FALSE;
        }
        SendMessage(mwnd, CLOSE_WINDOW, 0, 0);
    }
}
/* -------- delete a file ------------ */
static void DeleteFile(WINDOW wnd)
{
    if (wnd->extension != NULL)    {
        if (strcmp(wnd->extension, Untitled))    {
            char *fn = NameComponent(wnd->extension);
            if (fn != NULL)    {
                char msg[30];
                sprintf(msg, "Delete %s?", fn);
                if (YesNoBox(msg))    {
                    unlink(wnd->extension);
                    SendMessage(wnd, CLOSE_WINDOW, 0, 0);
                }
            }
        }
    }
}
/* ------ display the row and column in the statusbar ------ */
static void ShowPosition(WINDOW wnd)
{
    char status[30];
    sprintf(status, "Line:%4d  Column: %2d",
        wnd->CurrLine, wnd->CurrCol);
    SendMessage(GetParent(wnd), ADDSTATUS, (PARAM) status, 0);
}
/* ----- window processing module for the editboxes ----- */
static int EditorProc(WINDOW wnd,MESSAGE msg,PARAM p1,PARAM p2)
{
    int rtn;
    switch (msg)    {
        case SETFOCUS:
			if ((int)p1)	{
				wnd->InsertMode = GetCommandToggle(&MainMenu, ID_INSERT);
				wnd->WordWrapMode = GetCommandToggle(&MainMenu, ID_WRAP);
			}
            rtn = DefaultWndProc(wnd, msg, p1, p2);
            if ((int)p1 == FALSE)
                SendMessage(GetParent(wnd), ADDSTATUS, 0, 0);
            else 
                ShowPosition(wnd);
            return rtn;
        case KEYBOARD_CURSOR:
            rtn = DefaultWndProc(wnd, msg, p1, p2);
            ShowPosition(wnd);
            return rtn;
        case COMMAND:
			switch ((int) p1)	{
				case ID_HELP:
	                DisplayHelp(wnd, "MEMOPADDOC");
    	            return TRUE;
				case ID_WRAP:
					SendMessage(GetParent(wnd), COMMAND, ID_WRAP, 0);
					wnd->WordWrapMode = cfg.WordWrap;
    	            return TRUE;
				case ID_INSERT:
					SendMessage(GetParent(wnd), COMMAND, ID_INSERT, 0);
					wnd->InsertMode = cfg.InsertMode;
					SendMessage(NULL, SHOW_CURSOR, wnd->InsertMode, 0);
    	            return TRUE;
				default:
					break;
            }
            break;
        case CLOSE_WINDOW:
            if (wnd->TextChanged)    {
                char *cp = DFmalloc(25+strlen(GetTitle(wnd)));
                SendMessage(wnd, SETFOCUS, TRUE, 0);
                strcpy(cp, GetTitle(wnd));
                strcat(cp, "\nText changed. Save it?");
                if (YesNoBox(cp))
                    SendMessage(GetParent(wnd),
                        COMMAND, ID_SAVE, 0);
                free(cp);
            }
            wndpos = 0;
            if (wnd->extension != NULL)    {
                free(wnd->extension);
                wnd->extension = NULL;
            }
            break;
        default:
            break;
    }
    return DefaultWndProc(wnd, msg, p1, p2);
}
/* -- point to the name component of a file specification -- */
static char *NameComponent(char *FileName)
{
    char *Fname;
    if ((Fname = strrchr(FileName, '\\')) == NULL)
        if ((Fname = strrchr(FileName, ':')) == NULL)
            Fname = FileName-1;
    return Fname + 1;
}

static char *ports[] = {
	"Lpt1",	"Lpt2",	"Lpt3",
	"Com1",	"Com2",	"Com3",	"Com4",
 	 NULL
};

static int PrintSetupProc(WINDOW wnd, MESSAGE msg, PARAM p1, PARAM p2)
{
	int rtn, i = 0, mar;
	char marg[10];
	WINDOW cwnd;
    switch (msg)    {
		case CREATE_WINDOW:
		    rtn = DefaultWndProc(wnd, msg, p1, p2);
			PutItemText(wnd, ID_PRINTERPORT, cfg.PrinterPort);
			while (ports[i] != NULL)
				PutComboListText(wnd, ID_PRINTERPORT, ports[i++]);
			for (mar = CHARSLINE; mar >= 0; --mar)	{
				sprintf(marg, "%3d", mar);
				PutItemText(wnd, ID_LEFTMARGIN, marg);
				PutItemText(wnd, ID_RIGHTMARGIN, marg);
			}
			for (mar = LINESPAGE; mar >= 0; --mar)	{
				sprintf(marg, "%3d", mar);
				PutItemText(wnd, ID_TOPMARGIN, marg);
				PutItemText(wnd, ID_BOTTOMMARGIN, marg);
			}
			cwnd = ControlWindow(&PrintSetup, ID_LEFTMARGIN);
			SendMessage(cwnd, LB_SETSELECTION,
				CHARSLINE-cfg.LeftMargin, 0);
			cwnd = ControlWindow(&PrintSetup, ID_RIGHTMARGIN);
			SendMessage(cwnd, LB_SETSELECTION,
				CHARSLINE-cfg.RightMargin, 0);
			cwnd = ControlWindow(&PrintSetup, ID_TOPMARGIN);
			SendMessage(cwnd, LB_SETSELECTION,
				LINESPAGE-cfg.TopMargin, 0);
			cwnd = ControlWindow(&PrintSetup, ID_BOTTOMMARGIN);
			SendMessage(cwnd, LB_SETSELECTION,
				LINESPAGE-cfg.BottomMargin, 0);
			return rtn;
		case COMMAND:
			if ((int) p1 == ID_OK && (int) p2 == 0)	{
				GetItemText(wnd, ID_PRINTERPORT, cfg.PrinterPort, 4);
				cwnd = ControlWindow(&PrintSetup, ID_LEFTMARGIN);
				cfg.LeftMargin = CHARSLINE -
					SendMessage(cwnd, LB_CURRENTSELECTION, 0, 0);
				cwnd = ControlWindow(&PrintSetup, ID_RIGHTMARGIN);
				cfg.RightMargin = CHARSLINE -
					SendMessage(cwnd, LB_CURRENTSELECTION, 0, 0);
				cwnd = ControlWindow(&PrintSetup, ID_TOPMARGIN);
				cfg.TopMargin = LINESPAGE -
					SendMessage(cwnd, LB_CURRENTSELECTION, 0, 0);
				cwnd = ControlWindow(&PrintSetup, ID_BOTTOMMARGIN);
				cfg.BottomMargin = LINESPAGE -
					SendMessage(cwnd, LB_CURRENTSELECTION, 0, 0);
			}
			break;
        default:
            break;
	}
    return DefaultWndProc(wnd, msg, p1, p2);
}

static void FixTabMenu(void)
{
	char *cp = GetCommandText(&MainMenu, ID_TABS);
	if (cp != NULL)	{
		cp = strchr(cp, '(');
		if (cp != NULL)	{
			*(cp+1) = cfg.Tabs + '0';
			if (GetClass(inFocus) == POPDOWNMENU)
				SendMessage(inFocus, PAINT, 0, 0);
		}
	}
}

void PrepFileMenu(void *w, struct Menu *mnu)
{
	WINDOW wnd = w;
	DeactivateCommand(&MainMenu, ID_SAVE);
	DeactivateCommand(&MainMenu, ID_SAVEAS);
	DeactivateCommand(&MainMenu, ID_DELETEFILE);
	DeactivateCommand(&MainMenu, ID_PRINT);
	if (wnd != NULL && GetClass(wnd) == EDITBOX) {
		if (isMultiLine(wnd))	{
			ActivateCommand(&MainMenu, ID_SAVE);
			ActivateCommand(&MainMenu, ID_SAVEAS);
			ActivateCommand(&MainMenu, ID_DELETEFILE);
			ActivateCommand(&MainMenu, ID_PRINT);
		}
	}
}

void PrepSearchMenu(void *w, struct Menu *mnu)
{
	WINDOW wnd = w;
	DeactivateCommand(&MainMenu, ID_SEARCH);
	DeactivateCommand(&MainMenu, ID_REPLACE);
	DeactivateCommand(&MainMenu, ID_SEARCHNEXT);
	if (wnd != NULL && GetClass(wnd) == EDITBOX) {
		if (isMultiLine(wnd))	{
			ActivateCommand(&MainMenu, ID_SEARCH);
			ActivateCommand(&MainMenu, ID_REPLACE);
			ActivateCommand(&MainMenu, ID_SEARCHNEXT);
		}
	}
}

void PrepEditMenu(void *w, struct Menu *mnu)
{
	WINDOW wnd = w;
	DeactivateCommand(&MainMenu, ID_CUT);
	DeactivateCommand(&MainMenu, ID_COPY);
	DeactivateCommand(&MainMenu, ID_CLEAR);
	DeactivateCommand(&MainMenu, ID_DELETETEXT);
	DeactivateCommand(&MainMenu, ID_PARAGRAPH);
	DeactivateCommand(&MainMenu, ID_PASTE);
	DeactivateCommand(&MainMenu, ID_UNDO);
	if (wnd != NULL && GetClass(wnd) == EDITBOX) {
		if (isMultiLine(wnd))	{
			if (TextBlockMarked(wnd))	{
				ActivateCommand(&MainMenu, ID_CUT);
				ActivateCommand(&MainMenu, ID_COPY);
				ActivateCommand(&MainMenu, ID_CLEAR);
				ActivateCommand(&MainMenu, ID_DELETETEXT);
			}
			ActivateCommand(&MainMenu, ID_PARAGRAPH);
			if (!TestAttribute(wnd, READONLY) &&
						Clipboard != NULL)
				ActivateCommand(&MainMenu, ID_PASTE);
			if (wnd->DeletedText != NULL)
				ActivateCommand(&MainMenu, ID_UNDO);
		}
	}
}