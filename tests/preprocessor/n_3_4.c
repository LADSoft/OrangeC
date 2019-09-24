/* n_3_4.c: Handling of comment and <backslash><newline>.   */

/* 3.4: Comment and <backslash><newline> in #error line.    */
#error  Message of first physical line. \
    Message of second physical and first logical line.  /*
    this comment splices the lines
    */  Message of forth physical and third logical line.

