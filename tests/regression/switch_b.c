#include <stdio.h>

int main (const int argc, char **argv) {
	char suffix[3] = "";    // works with = {0}
	char name[10] = "";
	char *yytext = "SWITCH B     ";
	int yyleng;
	
	yyleng = 8;

	if (yytext[yyleng-2] == ' ' && isdigit((unsigned char)yytext[yyleng-1])) {
		/* SWITCH 0  to SWITCH 9 */
		suffix[0] = yytext[yyleng-1];
	} else if (isdigit((unsigned char)yytext[yyleng-2])) {
		/* SWITCH 00 to SWITCH 99 */
		suffix[0] = yytext[yyleng-2];
		suffix[1] = yytext[yyleng-1];
	} else {
		suffix[0] = yytext[yyleng-1];
	}
	strncpy(name, yytext, 6);
	strcat(name, "_");
	strcat(name, suffix);

	printf("result: '%s' (test was: '%s', suffix was: '%s')\n", name, yytext, suffix);
	
	return 0;
}