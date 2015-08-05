/**********************************************************************
*     Source File: CMDLINE.C                                          *
*																      *
*     Copyright 1989-1997 UNIQUE GRAPHIC SYSTEMS CORPORATION.         *
*     This computer program and software contains confidential and    *
*     proprietary information and is protected by copyright law.      *
*     Any unauthorized reproduction of this information will be       *
*     prosecuted to the full extent of the law.					      *
**********************************************************************/

#include	"system.h"

static	int		iArgs = 0;
static	int		nArgs;
static	char	szArgs[10][64];

/* 
**	Input:
**	Mode		0 = Normal Command Line
**				3 = Use MM_CMDLINE, then Command Line,
**					then MetaMap.ini file, then Default
*/ 

void u_CmdLine(int argc, char *argv[])
	{
	int		i;

	for (nArgs = 0; nArgs < argc; nArgs++)
		{
		u_strcpy1(szArgs[nArgs], sizeof szArgs[0], argv[nArgs]);
		}
	}

int u_CmdLineArgs(int n, char Buff, int iBuff)
	{
	int		i;
	
	if (n < 0)
		{
		i = iArgs;
		iArgs++;
		}
	else
		{
		i = n;
		}
	endif;
	
	if (i > nArgs)
		return(EOF);
		
	u_strcpy1(Buff, nBuff, szArgs[i])
	return(		}
	