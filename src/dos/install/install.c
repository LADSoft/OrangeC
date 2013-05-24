/* --------------- memopad.c ----------- */

#include "dflat.h"
#include "idconst.h"
#include "..\..\version.h"
#include <stdarg.h>

extern DBOX dosDialog;
extern DBOX NTDialog;
char DFlatApplication[] = "DosInstall";


static int InstallProc(WINDOW, MESSAGE, PARAM, PARAM);
void PrintMessage(char *msg, ...);
static BOOL isWindowsNT(void);
static BOOL GetAutoexecPath(char *path);
static void ModifyAutoexec(char *path, char *installPath);
static void RunNTPathInstall(char *path);
static int unzipDistribution(char *installPath);
static int install(void);

static WINDOW text;
void main(int argc, char *argv[])
{
    WINDOW wnd;
    if (!init_messages())
		return;
    Argv = argv;
	if (!LoadConfig())
		cfg.ScreenLines = SCREENHEIGHT;
    wnd = CreateWindow(APPLICATION,
                        "Orange C Install Utility Version " PRODUCT_STRING_VERSION,
                        0, 0, -1, -1,
                        NULL,
                        NULL,
                        InstallProc,
                        MOVEABLE  |
                        SIZEABLE  |
                        HASBORDER |
						MINMAXBOX |
                        HASSTATUSBAR
                        );

    SendMessage(wnd, SETFOCUS, TRUE, 0);
	PostMessage(wnd, COMMAND, ID_START, 0);
    while (dispatch_message())
        ;
}
/* ------- window processing module for the
                    memopad application window ----- */
static int InstallProc(WINDOW wnd,MESSAGE msg,PARAM p1,PARAM p2)
{
	int rtn;
	RECT rc;
    switch (msg)    {
		case CREATE_WINDOW:
		    rtn = DefaultWndProc(wnd, msg, p1, p2);
			rc = WindowRect(wnd);
			rc = AdjustRectangle(wnd, rc);
		    text = CreateWindow(TEXTBOX,
		                isWindowsNT() ? "Install Progress - Windows NT/2000/XP" : "Install Progress - DOS" ,
		                0, 0, rc.bt-rc.tp, rc.rt-rc.lf,
		                NULL, wnd, NULL,
		                MINMAXBOX  |
		                CONTROLBOX |
		                VSCROLLBAR |
		                HSCROLLBAR |
		                MOVEABLE   |
		                HASBORDER  |
		                SIZEABLE   |
		                MULTILINE);
			return rtn;
        case COMMAND:
            switch ((int)p1)    {
				case ID_EXIT:	
					break;
				case ID_START:
					install();
					PostMessage(wnd, COMMAND, ID_EXIT, 0);
					return TRUE;
                default:
                    break;
            }
        default:
            break;
    }
    return DefaultWndProc(wnd, msg, p1, p2);
}
void PrepFileMenu(void *w, struct Menu *mnu)
{
}

void PrepSearchMenu(void *w, struct Menu *mnu)
{
}

void PrepEditMenu(void *w, struct Menu *mnu)
{
}
void PrintMessage(char *msg,...)
{
	char buf[1024];
	va_list aa;
	va_start(aa, msg);
	vsprintf(buf, msg, aa);
	va_end(aa);
	SendMessage(text, ADDTEXT, (PARAM)buf, 0);
	SendMessage(text, PAINT, 0, 0);
	handshake();
}
BOOL isWindowsNT(void)
{
	char *p = getenv("OS");
	return p && (strcmp(p,"Windows_NT") == 0);
}
BOOL ScanConfig(char *path, char *config, char *autoexec)
{
	FILE *fil ;
	char buf[256];
	sprintf(buf,"C:\\%s", config);
	fil = fopen(buf, "rt");
	if (!fil)
	{
		sprintf(buf,"C:\\%s", autoexec);
		if (!access(autoexec, 0))
		{
			strcpy(path, buf);
			return TRUE;
		}
		return FALSE;
	}
	while (!feof(fil))
	{
		char buf[1024], *p;
		buf[0] = 0;
		fgets(buf, 1024, fil);
		p = buf;
		while (isspace(*p))
			p++;
		if (!strnicmp(p, "SHELL",3))
		{
			p = strstr(buf,"/P:");
			if (p)
			{
				p += 3;
				while (*p && !isspace(*p))
					*path++ = *p++;
				*path = 0;
				fclose(fil);
				return TRUE;
			}
		}
	}
	fclose(fil);
	sprintf(buf,"C:\\%s", autoexec);
	strcpy(path, buf);
	return TRUE;
}
BOOL GetAutoexecPath(char *buf)
{
	if (!ScanConfig(buf, "config.sys", "autoexec.bat"))
		if (!ScanConfig(buf, "fdconfig.sys", "fdauto.bat"))
			if (!ScanConfig(buf, "dconfig.sys", "drauto.bat"))
			{
				strcpy(buf, "C:\\autoexec.bat");
				return FALSE;
			}
	return TRUE;
}
void ModifyAutoExec(char *path, char *installPath)
{
	FILE *fil ;
	char apath[260], *p = apath;
	if (path[1] == ':')
	{
		*p++ = *path++;
		*p++ = *path++;
	}
	else
	{
		*p++ = 'c';
		*p++ = ':';
	}
	if (path[0] != '\\')
		*p++ = '\\';
	strcpy(p, path);
		
	fil = fopen(apath,"wta");
	fprintf(fil,"REM\n");
	fprintf(fil, "REM Beginning of LADSoft Orange C installation\n");
	fprintf(fil,"REM\n");
	fprintf(fil, "set ORANGEC=%s\n", installPath);
    fprintf(fil, "set ORANGECDOS=%s\n", installPath);
	fprintf(fil, "path %s\\bin;%s\\lib;%%PATH%%\n", installPath, installPath);
    fprintf(fil, "set HDPMI=32\n");
    fprintf(fil, "set DPMILDR=8\n");
	fprintf(fil,"REM\n");
	fprintf(fil, "REM End of LADSoft Orange C installation\n");
	fprintf(fil,"REM\n");
	fclose(fil);
}
void MakeLSetEnv(char *path, char *installPath)
{
	FILE *fil ;
	char apath[260], *p = apath;
	if (path[1] == ':')
	{
		*p++ = *path++;
		*p++ = *path++;
	}
	else
	{
		*p++ = 'c';
		*p++ = ':';
	}
	if (path[0] != '\\')
		*p++ = '\\';
	strcpy(p, path);
		
	fil = fopen(apath,"wta");
	fprintf(fil,"@echo off\n");
	fprintf(fil,"REM\n");
	fprintf(fil, "REM Beginning of LADSoft Orange C installation\n");
	fprintf(fil,"REM\n");
	fprintf(fil, "if \"%%1\"==\"/u\" goto unset\n");
	fprintf(fil, "if \"%%1\"==\"/U\" goto unset\n");
//	fprintf(fil, "set DUMMY798=%s;%s\\bin;%%PATH%%1234567890\n",installPath, installPath);
//	fprintf(fil, "if \"%%DUMMY798%%\"==\"\"  %%COMSPEC%% /k %%0 /e:4096\n");
//	fprintf(fil, "set DUMMY798=\n");
	fprintf(fil, "set ORANGEC=%s\n", installPath);
	fprintf(fil, "set ORANGECDOS=%s\n", installPath);
    fprintf(fil, "set HDPMI=32\n");
    fprintf(fil, "set DPMILDR=8\n");
	fprintf(fil, "set PATHBAK=%%PATH%%\n");
	fprintf(fil, "path %s\\BIN;%s\\LIB;%%PATH%%\n", installPath, installPath);
	fprintf(fil, "goto end\n");
	fprintf(fil, ":unset\n");

	fprintf(fil, "if \"%%ORANGEC%%\"==\"\" goto end\n");
	fprintf(fil, "path %%PATHBAK%%\n");
	fprintf(fil, "set ORANGEC=\n");
	fprintf(fil, "set ORANGECDOS=\n");
	fprintf(fil, "set PATHBAK=\n");
    fprintf(fil, "set HDPMI=\n");
    fprintf(fil, "set DPMILDR=\n");
	fprintf(fil, ":end\n");
	fprintf(fil,"REM\n");
	fprintf(fil, "REM End of LADSoft Orange C installation\n");
	fprintf(fil,"REM\n");
	fclose(fil);
}
void RunNTPathInstall(char *path)
{
	char buf[256];
	sprintf(buf,"%s\\bin\\ntsetenv.exe LADSOFT %s > nul", path, path);
	system(buf);
	sprintf(buf,"%s\\bin\\ntsetenv.exe /A PATH %s\\bin > nul", path, path);
	system(buf);
}
int unzipDistribution(char *installPath)
{
    PrintMessage("Unzipping main distribution");
	return unzip(Argv[0], installPath);
}
int unzipHX(char *installPath)
{
    char source[256];
    char dest[256];
    PrintMessage("Unzipping HXDOS");
    sprintf(source,"%s\\addon\\hx.zip", installPath);
    sprintf(dest,"%s\\bin", installPath);
    return unzip(source, dest);
}
static int DlgInstall(WINDOW wnd,MESSAGE msg,PARAM p1,PARAM p2)
{
    switch (msg)    {
        case CREATE_WINDOW:    {
            int rtn = DefaultWndProc(wnd, msg, p1, p2);
            DBOX *db = wnd->extension;
            WINDOW cwnd = ControlWindow(db, ID_INSTALLPATH);
            SendMessage(cwnd, SETTEXTLENGTH, 200, 0);
            return rtn;
        }
        case INITIATE_DIALOG:
	        PutItemText(wnd, ID_INSTALLPATH, "C:\\OrangeC");
            break;
        default:
            break;
    }
    return DefaultWndProc(wnd, msg, p1, p2);
}
int install(void)
{
	char *installPath, *smsg;
	char curpath[260];
	DBOX *dialog ;
	BOOL modifyPath = FALSE;
	PrintMessage("Prompting for configuration");
	if (isWindowsNT())
		dialog = &NTDialog;
	else
		dialog = &dosDialog;
	if (!DialogBox(NULL, dialog, TRUE, DlgInstall))    {
		ErrorMessage("Install canceled");
		return FALSE;
	}
    installPath = GetEditBoxText(dialog, ID_INSTALLPATH);
    modifyPath = CheckBoxSetting(dialog,ID_MODIFYAUTO);

	getcwd(curpath, sizeof(curpath));
	if (!installPath[0])
	{
		ErrorMessage("No path specified");
		return FALSE;
	}
	if (installPath[strlen(installPath)-1] == '\\')
		installPath[strlen(installPath)-1] = 0;
	PrintMessage("Installing files to %s", installPath);
	if (unzipDistribution(installPath) || unzipHX(installPath))
	{
		PrintMessage("Install failed");
		ErrorMessage("Install failed");
		return FALSE;
	}
	else
	{
		if (installPath[strlen(installPath)-1] == '\\')
			installPath[strlen(installPath)-1] = 0;
		strcat(curpath,"\\occvars.bat");
		unlink(curpath);
		MakeLSetEnv(curpath, installPath);
		if (modifyPath)
		{
			if (isWindowsNT())
			{
				PrintMessage("Modifying registry");
				RunNTPathInstall(installPath);
			}
			else
			{
				char path[260];
				BOOL t = GetAutoexecPath(path);
				PrintMessage("Modifying %s startup batch file", path);
				ModifyAutoExec(path, installPath);
			}
			smsg = "Install succeeded\nA reboot is required to set the environment";
		}
		else
		{
			PrintMessage("Not modifying startup files");
			smsg = "Install succeeded\n'occvars.bat' has been created with required setup changes";
		}
		GenericMessage(NULL, "Success", smsg, 1, MessageBoxProc,
			Ok, NULL, ID_OK, 0, TRUE);
		return TRUE;
	}
}
