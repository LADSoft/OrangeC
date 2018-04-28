#include <windows.h>
#include <string.h>
#include <stdio.h>

char *hdr[200];
char *names[] = {
".cpp", ".c", ".mak",".asm",".nas",".h",".p",".inc", 0
};
int in_ext(char *p)
{
	char **q = names;
	while (*q)
		if (!stricmp(*q++, p))
			return TRUE;
	return FALSE;
}
char *stristr(char *str1, char *str2)
{
    int l = strlen(str2);
    while (*str1)
    {
        char *str3 = str1, *str4 = str2;
        while (*str3 && *str4)
        {
            if (toupper(*str3) != toupper(*str4))
                break;
            str3++, str4++;
        }
        if (!*str4)
            return str1;
        str1++;
    }
    return 0;
}

int hasCopyright(char *path)
{
	int rv = FALSE;
	FILE *fil = fopen(path,"r");
	if (fil)
	{
		fseek(fil, 0, SEEK_END);
		int n = ftell(fil);
		fseek(fil, 0, SEEK_SET);
		char *p = (char *)malloc(n+1);
                if (p)
		{
			fread(p,1,n,fil);
			p[n] = 0;
			if (stristr(p, "(BSD LICENSE)"))
				rv = TRUE;
			free(p);
		}
		fclose(fil);
	}
	return rv;
}
void eraseBlock(char *buf)
{
	char *p = strstr(buf, "*/");
	if (p)
	{
		p += 2;
		while (isspace(*p))
			p++;
		strcpy(buf, p);
	}
}
void eraseSingle(char *buf, char sep)
{
	char *p = buf;
	while (1)
	{
		while (*p && isspace(*p))
			p++;
		if (*p != sep)
			break;
		p = strchr(p, '\n');
		if (!p)
		{
			printf("file too small");
			exit(1);
		}
	}
	if (p)
	{
		strcpy(buf, p);
	}
}
void addBlock(char *buf)
{
	memmove(buf + 2000, buf, strlen(buf));
	memset(buf,0, 2000);
	char **p = hdr;
	sprintf(buf, "/* %s", *p++);
	while (*p)
		sprintf(buf + strlen(buf), " * %s", *p++);
        sprintf(buf+ strlen(buf), " */\n\n");
	strcpy(buf + strlen(buf), buf + 2000);
}
void addSingle(char *buf, char sep)
{
	memmove(buf + 2000, buf, strlen(buf));
	memset(buf,0, 2000);
	char **p = hdr;
	while (*p)
		sprintf(buf + strlen(buf), "%c %s", sep, *p++);
        sprintf(buf+ strlen(buf), "\n");
	strcpy(buf + strlen(buf), buf + 2000);
}
void doReplace(char *p)
{
	char *q = p;
	while (isspace(*q)) q++;
	char n = q[0];
	switch(n)
	{
		case '/':
			eraseBlock(p);
			break;
		case '#':
		case ';':
			eraseSingle(p, n);
			break;
	}
	switch(n)
	{
		case '/':
			addBlock(p);
			break;
		case '#':
		case ';':
			addSingle(p,n);
			break;
	}
}
void replaceCopyright(char *path)
{
	FILE *fil = fopen(path,"r");
	if (fil)
	{
		fseek(fil, 0, SEEK_END);
		int n = ftell(fil);
		fseek(fil, 0, SEEK_SET);
		char *p = (char *)malloc(n+10000);
                if (p)
		{
			int n1 = fread(p,1,n,fil);
			memset(p+n1, 0, 10000 + n - n1);
			fclose (fil);
			doReplace(p);
			fil = fopen(path, "w");
			fprintf(fil, "%s", p);
			free(p);
		}
		fclose(fil);
	}
}
void process(char *path)
{
	char *p = strrchr(path, '.');
	if (p && in_ext(p) || strlen(path) >= 8 && stristr(path, "makefile") == path + strlen(path) - 8)
	{
		if (hasCopyright(path))
		{
			printf("%s\n", path);
			replaceCopyright(path);
		}
	}
}
void recurse(char *path)
{
    HANDLE hand;
    WIN32_FIND_DATA data;
    char root[MAX_PATH], search[MAX_PATH];
    strcpy(root, path);
    if (root[strlen(root)-1] != '\\')
        strcat(root, "\\");
    sprintf(search, "%s*.*", root);
    hand = FindFirstFile(search, &data);
    if (hand != INVALID_HANDLE_VALUE)
    {
        do
        {
            sprintf(search,"%s%s",root, data.cFileName);
            if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if (data.cFileName[0] != '.' && stricmp(data.cFileName, "test"))
		{
                    recurse(search);
		}
            }
            else
            {
                process(search);
            }
        } while (FindNextFile(hand, &data));
        FindClose(hand);
    }
          
                 
}
void LoadHdr()
{
	char **p = hdr;
	FILE *fil = fopen("gnu.hdr","r");
	if (fil)
	{
		while (!feof(fil))
		{
		char buf[2048];
		buf[0] = 0;
		fgets(buf, 2048, fil);
		if (buf[0])
			*p++ = strdup(buf);
		}
	}
}
int main(int argc, char **argv)
{

    char buf[MAX_PATH];
    getcwd(buf, MAX_PATH);
    printf("%s\n", buf);
    LoadHdr();
    recurse(buf);
}
