/* ------ fixhelp.c ------ */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "htree.h"
#define FIXHELP
#include "helpbox.h"

static FILE *helpfp;
static char hline [160];

static struct helps *FirstHelp;
static struct helps *LastHelp;
static struct helps *ThisHelp;
static char **Argv;

static void WriteText(char *);

/* ---- compute the displayed length of a help text line --- */
static int HelpLength(char *s)
{
    int len = strlen(s);
    char *cp = strchr(s, '[');
    while (cp != NULL)    {
        len -= 4;
        cp = strchr(cp+1, '[');
    }
    cp = strchr(s, '<');
    while (cp != NULL)    {
        char *cp1 = strchr(cp, '>');
        if (cp1 != NULL)
            len -= (int) (cp1-cp)+1;
        cp = strchr(cp1, '<');
    }
    return len;
}

int FindHelp(char *nm)
{
	int hlp = 0;
	struct helps *thishelp = FirstHelp;
	if (!nm)
		return -1;
	while (thishelp != NULL)	{
		if (strcmp(nm, thishelp->hname) == 0)
			break;
		hlp++;
		thishelp = thishelp->NextHelp;
	}
	return thishelp ? hlp : -1;
}

main(int argc, char *argv[])
{
    char *cp;
	int HelpCount = 0;
	long where;

	if (argc < 2)
		return -1;
	Argv = argv;
    if ((helpfp = OpenHelpFile(argv[1], "r+b")) == NULL)
        return -1;


    *hline = '\0';
    while (*hline != '<')    {
        if (GetHelpLine(hline) == NULL)    {
            fclose(helpfp);
            return -1;
        }
    }
    while (*hline == '<')   {
        if (strncmp(hline, "<end>", 5) == 0)
            break;

        /* -------- parse the help window's text name ----- */
        if ((cp = strchr(hline, '>')) != NULL)    {
            ThisHelp = calloc(1, sizeof(struct helps));
            if (FirstHelp == NULL)
            	FirstHelp = ThisHelp;
            *cp = '\0';
            ThisHelp->hname=malloc(strlen(hline+1)+1);
            strcpy(ThisHelp->hname, hline+1);

            HelpFilePosition(&ThisHelp->hptr, &ThisHelp->bit);
            if (GetHelpLine(hline) == NULL)
                break;

            /* ------- build the help linked list entry --- */
            while (*hline == '[')    {
                HelpFilePosition(&ThisHelp->hptr,
                                            &ThisHelp->bit);
				/* ------ parse a comment ----- */
                if (strncmp(hline, "[*]", 3) == 0)    {
				    ThisHelp->comment=malloc(strlen(hline+3)+1);
            		strcpy(ThisHelp->comment, hline+3);
                    if (GetHelpLine(hline) == NULL)
                        break;
					continue;
				}
                /* ---- parse the <<prev button pointer ---- */
                if (strncmp(hline, "[<<]", 4) == 0)    {
                    char *cp = strchr(hline+4, '<');
                    if (cp != NULL)    {
                        char *cp1 = strchr(cp, '>');
                        if (cp1 != NULL)    {
                            int len = (int) (cp1-cp);
                            ThisHelp->PrevName=calloc(1,len);
                            strncpy(ThisHelp->PrevName,
                                cp+1,len-1);
                        }
                    }
                    if (GetHelpLine(hline) == NULL)
                        break;
                    continue;
                }
                /* ---- parse the next>> button pointer ---- */
                else if (strncmp(hline, "[>>]", 4) == 0)    {
                    char *cp = strchr(hline+4, '<');
                    if (cp != NULL)    {
                        char *cp1 = strchr(cp, '>');
                        if (cp1 != NULL)    {
                            int len = (int) (cp1-cp);
                            ThisHelp->NextName=calloc(1,len);
                            strncpy(ThisHelp->NextName,
                                            cp+1,len-1);
                        }
                    }
                    if (GetHelpLine(hline) == NULL)
                        break;
                    continue;
                }
                else
                    break;
            }
            ThisHelp->hheight = 0;
            ThisHelp->hwidth = 0;
            ThisHelp->NextHelp = NULL;

            /* ------ append entry to the linked list ------ */
            if (LastHelp != NULL)
                LastHelp->NextHelp = ThisHelp;
            LastHelp = ThisHelp;
			HelpCount++;
        }
        /* -------- move to the next <helpname> token ------ */
        if (GetHelpLine(hline) == NULL)
            strcpy(hline, "<end>");
        while (*hline != '<')    {
            ThisHelp->hwidth =
                max(ThisHelp->hwidth, HelpLength(hline));
            ThisHelp->hheight++;
            if (GetHelpLine(hline) == NULL)
                strcpy(hline, "<end>");
        }
    }
	/* --- append the help structures to the file --- */
	fseek(helpfp, 0L, SEEK_END);
	where = ftell(helpfp);
	ThisHelp = FirstHelp;
	fwrite(&HelpCount, sizeof(int), 1, helpfp);
	while (ThisHelp != NULL)	{
		ThisHelp->nexthlp = FindHelp(ThisHelp->NextName);
		ThisHelp->prevhlp = FindHelp(ThisHelp->PrevName);
		WriteText(ThisHelp->hname);
		WriteText(ThisHelp->comment);
		fwrite(&ThisHelp->hptr, sizeof(short)*5+sizeof(long), 1, helpfp);
		ThisHelp = ThisHelp->NextHelp;
	}
	fwrite(&where, sizeof(long), 1, helpfp);
    fclose(helpfp);
	return 0;
}

void BuildFileName(char *fn, const char *fname, const char *ext)
{
	strcpy(fn, Argv[1]);
	strcat(fn, ext);
}

static void WriteText(char *text)
{
	char *np = text ? text : "";
	short len = strlen(np);
	fwrite(&len, sizeof(short), 1, helpfp);
	if (len)
		fwrite(np, len+1, 1, helpfp);
}
