char *msga = "hi david";
char *msgb = "hello";
char *msgc = "hi david2";
main()
{
	char lbuf[200];
	int i;
	printf("strcpy: '%s'\n",STRCPY(lbuf,msga));
	printf("strcpy: '%s'\n",lbuf);
	printf("strcat: '%s'\n",STRCAT(lbuf,msgb));
	printf("strcat: '%s'\n",lbuf);
	printf("strlen: %d\n",STRLEN(msga));
	printf("strcmp: %d\n",STRCMP(msga,msgb));
	printf("strcmp: %d\n",STRCMP(msgb,msga));
	printf("strcmp: %d\n",STRCMP(msga,msga));
	printf("strcmp: %d\n",STRCMP(msga,msgc));
	printf("strcmp: %d\n",STRCMP(msgc,msga));
	printf("strchr: %s\n",STRCHR(msga,'d'));
	printf("strrchr: %s\n",STRRCHR(msgb,'l'));
	for (i=0; i<200; i++) lbuf[i] = 0;
	for (i=0; i<10; i++) lbuf[i] = 0x33;
	printf("strncpy: %s\n",STRNCPY(lbuf,msga,3));
	printf("strncat: %s\n",STRNCAT(lbuf,msgb,4));
	for (i=0; i<200; i++) lbuf[i] = 0x34;
	STRNCPY(lbuf,msgb,10);
	for (i=0; i<200; i++) printf("%x",lbuf[i]);
	printf("\n");
	printf("strncmp: %d\n",STRNCMP(msga,msgb,1));
	printf("strncmp: %d\n",STRNCMP(msga,msgb,2));
	printf("strncmp: %d\n",STRNCMP(msgb,msga,2));
}