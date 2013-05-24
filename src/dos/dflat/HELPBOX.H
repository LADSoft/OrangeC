/* --------- helpbox.h ----------- */

#ifndef HELPBOX_H
#define HELPBOX_H

/* --------- linked list of help text collections -------- */
struct helps {
    char *hname;
	char *comment;
    long hptr;
    short bit;
    short hheight;
    short hwidth;
	short nexthlp;
	short prevhlp;
    void *hwnd;
	char *PrevName;
	char *NextName;
#ifdef FIXHELP
	struct helps *NextHelp;
#endif
};

#endif

