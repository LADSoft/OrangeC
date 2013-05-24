/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2012, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    contact information:
        email: TouchStone222@runbox.com <David Lindauer>
*/
//
// WARGS.CPP
//
// Handle command line arguments
//
// Two basic routines:
//   1) translate windows ASCIIZ string to a list of C-Style ASCIIZ strings
//   2) parse the resulting list and dispatch action routines based
//      on switch values
//
#define STRICT
#include <windows.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "wargs.h"
/*
 * This module handles command line arguments.  The main program defines
 * an array specifying what arguments are allowed and what the activity
 * should be for each argument and then calls parse_args giving it
 * argv and argc as arguments.  Parse-args dispatches any arguments it
 * finds to the action routine and then deletes the argument it found
 * from argv[]
 */
// ==============================================================
//
/*
 * The main program must define this array.  It customizes the types of
 * arguments that will be allowed
 *
 */
extern ARGLIST ArgList[];

static int use_case; /* Gets set for case sensitivity */

// ==============================================================
//
/* 
 * Function that unlinks the argument from che argv[] chain
 */
static void remove_arg(int pos, int *count, char *list[])
{
    int i;

    /* Decrement length of list */
    (*count)--;

    /* move items down */
    for (i = pos; i <  *count; i++)
        list[i] = list[i + 1];

}

// ==============================================================
//
/*
 * ompare two characters, ignoring case if necessary
 */
static int cmatch(char t1, char t2)
{
    if (use_case)
        return (t1 == t2);

    return (toupper(t1) == toupper(t2));
}

// ==============================================================
//
/* Routine scans a string to see if any of the characters match
 *  the arguments, then dispatches to the action routine if so.
 */
/* Callbacks of the form
 *   void intcallback( char selectchar, int value)
 *   void switchcallback( char selectchar, int value)  ;; value always true
 *   void stringcallback( char selectchar, char *string)
 */
static int scan_args(char *string, int index, char *arg)
{
    int i =  - 1;
    while (ArgList[++i].id)
    {
        switch (ArgList[i].mode)
        {
            case ARG_SWITCHSTRING:
                if (cmatch(string[index], ArgList[i].id))
                {
                    (*ArgList[i].routine)(string[index], &string[index]);
                    return (ARG_NEXTCHAR);
                }
                break;
            case ARG_SWITCH:
                if (cmatch(string[index], ArgList[i].id))
                {
                    (*ArgList[i].routine)(string[index], (char*)TRUE);
                    return (ARG_NEXTCHAR);
                }
                break;
            case ARG_BOOL:
                if (cmatch(string[index], ArgList[i].id))
                {
                    if (string[0] == ARG_SEPTRUE)
                        (*ArgList[i].routine)(string[index], (char*)TRUE);
                    else
                        (*ArgList[i].routine)(string[index], (char*)FALSE);
                    return (ARG_NEXTCHAR);
                }
                break;
            case ARG_CONCATSTRING:
                if (cmatch(string[index], ArgList[i].id))
                {
                    (*ArgList[i].routine)(string[index], string + index + 1);
                    return (ARG_NEXTARG);
                }
                break;
            case ARG_NOCONCATSTRING:
                if (cmatch(string[index], ArgList[i].id))
                {
                    if (!arg)
                        return (ARG_NOARG);
                    (*ArgList[i].routine)(string[index], arg);
                    return (ARG_NEXTNOCAT);
                }
                break;
        }
    }
    return (ARG_NOMATCH);
}

// ==============================================================
//
/*
 * Main parse routine.  Scans for '-', then scan for arguments and
 * delete from the argv[] array if so.
 */
int parse_args(int *argc, char *argv[], int case_sensitive)
{
    int pos = 0;

    int retval = TRUE;
    use_case = case_sensitive;

    while (++pos <  *argc)
    {
        if ((argv[pos][0] == ARG_SEPSWITCH) || (argv[pos][0] == ARG_SEPFALSE) 
            || (argv[pos][0] == ARG_SEPTRUE))
        {
            int argmode;
            int index = 1;
            int done = FALSE;
            do
            {
                /* Scan the present arg */
                if (pos <  *argc - 1)
                    argmode = scan_args(argv[pos], index, argv[pos + 1]);
                else
                    argmode = scan_args(argv[pos], index, 0);

                switch (argmode)
                {
                    case ARG_NEXTCHAR:
                        /* If it was a char, go to the next one */
                        if (!argv[pos][++index])
                            done = TRUE;
                        break;
                    case ARG_NEXTNOCAT:
                        /* Otherwise if it was a nocat, remove the extra arg */
                        remove_arg(pos, argc, argv);
                        /* Fall through to NEXTARG */
                    case ARG_NEXTARG:
                        /* Just a next arg, go do it */
                        done = TRUE;
                        break;
                    case ARG_NOMATCH:
                        /* No such arg, spit an error  */
//                        fprintf(stderr, "Invalid Arg: %s\n", argv[pos]);
                        done = TRUE;
                        retval = FALSE;
                        break;
                    case ARG_NOARG:
                        /* Missing the arg for a CONCAT type, spit the error */
//                        fprintf(stderr, "Missing string for Arg %s\n",
//                            argv[pos]);
                        done = TRUE;
                        retval = FALSE;
                        break;
                };

            }
            while (!done);
            /* We'll always get rid of the present arg
             * And back up one
             */
            remove_arg(pos--, argc, argv);
        }
    }
    return (retval);
}
