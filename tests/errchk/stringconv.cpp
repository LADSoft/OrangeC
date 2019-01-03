int aa(char *);
int ab(const char *);

void a1()
{
	char *rr = "hi";
	const char *ss = "hi";
        aa("hi");
        ab("hi");
        rr="hi";
        ss="hi";      
}