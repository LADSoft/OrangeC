#pragma startup memstart 4
#pragma rundown memdelete 4

static void memdelete(void)
{
	rpmalloc_finalize();
}
static void memstart(void)
{
	rpmalloc_initialize();
}