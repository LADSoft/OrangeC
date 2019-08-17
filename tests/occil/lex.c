#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int isalpha(int c);
int isdigit(int c);

#define  SIZETOKEN	1000
#define  OPERATOR2 	"== != <= >= |= += -= *= /= >> << ++ -- && || -> "

int  nToken, fToken;
char *Token[SIZETOKEN];

int isAlpha(int c)
{
	return (isalpha(c) || c == '_');
}

int isAlNum(int c)
{
	return isAlpha(c) || isdigit(c);
}

char *strdup (const char *s) {
    char *d = (char*)malloc (strlen (s) + 1);   
    if (d == NULL) return NULL;         
    strcpy (d,s);                       
    return d;                          
}

void lex(const char *srcfile)
{
	char linebuf[256], buf[128], op2[4], *p, *pBgn;
	FILE *fpSrc = fopen(srcfile, "r");
	if (fpSrc == NULL) 
		printf("file '%s' can't be opened", srcfile);
	while (fgets(linebuf, sizeof(linebuf), fpSrc) != NULL)
	{
		for (p = linebuf; *p != '\0'; )
		{
			if (*p <= ' ')
			{
				p++;
				continue;
			}
			if (p[0] == '/' && p[1] == '/') break;
			pBgn = p;
			if (*p == '"')
			{
				for (p++; *p != '\0' && *p != '"'; p++)
				{
					if (*p == '\\' && strchr("rn\"t", p[1]) != NULL)  // '\r','\n','\"','\t'
					{
						*p = p[1] == 'r' ? '\r' : p[1] == 'n' ? '\n' : p[1] == '"' ? '\"' : '\t';
						strcpy(p + 1, p + 2);
					}
					else if (*p == '\\')
					{
						p++;
					}
				}
				if (*p++ != '"') 
					printf("Expected '\"' <%s>", pBgn);
			}
			else if (isdigit(*p))
			{
				strtoul(p, &p, 10); 
			}
			else if (strncmp(pBgn, "char*", 5) == 0)  	// char*
			{
				p += 5;
			}
			else if (isAlpha(*p))
			{
				for (p++; *p != '\0' && isAlNum(*p); ) 
					p++;
			}
			else
			{
				sprintf(op2, "%c%c ", p[0], p[1]);
				p += strstr(OPERATOR2, op2) != NULL ? 2 : 1;
			}
			sprintf(buf, "%.*s", p - pBgn, pBgn);
			if (nToken == SIZETOKEN)
				printf("Token is to big");
			Token[nToken++] = strdup(buf);
			if (fToken) 
				printf(strchr(";{}", *buf) != NULL ? "%s\n" : "%s ", buf);
		} 
	}
	fclose(fpSrc);
}

int main()
{
  int i = 0;
  char* fileName = "test.c";
  lex(fileName);
  
  for(i=0;i<nToken;i++)
    printf("%s\n", Token[i]);
  
  return 0;
}