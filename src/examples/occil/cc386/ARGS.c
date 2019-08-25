/* 
CC386 C Compiler
Copyright 1994-2011 David Lindauer.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

This program is derived from the cc68k complier by 
Matthew Brandt (mailto::mattb@walkingdog.net) 

You may contact the author of this derivative at:

mailto::camille@bluegrass.net
 */
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "cmdline.h"
/*
 * This module handles command line arguments.  The main program defines
 * an array specifying what arguments are allowed and what the activity
 * should be for each argument and then calls parse_args giving it
 * argv and argc as arguments.  Parse-args dispatches any arguments it
 * finds to the action routine and then deletes the argument it found
 * from argv[]
 */
/*
 * The main program must define this array.  It customizes the types of
 * arguments that will be allowed
 *
 */
extern ARGLIST ArgList[];
static BOOL use_case; /* Gets set for case sensitivity */
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

/*
 * ompare two characters, ignoring case if necessary
 */
static BOOL cmatch(char t1, char t2)
{
    if (use_case)
        return (t1 == t2);

    return (toupper(t1) == toupper(t2));
}

/* Routine scans a string to see if any of the characters match
 *  the arguments, then dispatches to the action routine if so.
 */
/* Callbacks of the form
 *   void boolcallback( char selectchar, BOOL value)
 *   void switchcallback( char selectchar, BOOL value)  ;; value always true
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
                    if (string[0] == ARG_SEPTRUE || string[0] == '/')
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

/*
 * Main parse routine.  Scans for '-', then scan for arguments and
 * delete from the argv[] array if so.
 */
BOOL parse_args(int *argc, char *argv[], BOOL case_sensitive)
{
    int pos = 0;

    BOOL retval = TRUE;
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
                        fprintf(stderr, "Invalid Arg: %s\n", argv[pos]);
                        done = TRUE;
                        retval = FALSE;
                        break;
                    case ARG_NOARG:
                        /* Missing the arg for a CONCAT type, spit the error */
                        fprintf(stderr, "Missing string for Arg %s\n",
                            argv[pos]);
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
