#include "afx.h"
#ifdef TARGET_OS_WINDOWS
#    include <windows.h>
#endif

static char rundir[LINE_MAX];

char* getrundir(void) { return rundir; }

int main(int argc, char* argv[])
{
    char buf[260];
    if (argc == 1)
        help();

    atexit(clear_all);

#ifdef TARGET_OS_WINDOWS
    GetModuleFileNameA(NULL, buf, sizeof(buf));
#else
    strcpy(buf, argv[0]);
#endif
    fn_split(buf);
    strcpy(rundir, fn_path());
#if DEBUG
    printf("*** Run from '%s'\n", getrundir());
#endif
    /*
        strcat( strcpy( cfgfile, fn_name() ), ".cfg" );
    */
    /*
    ** EnvVar, config filename, argv, argc
    */
    parse_params("OCL", "OCL.cfg", argv, argc);

    if (get_make_opt(OPT_COMPILE))
        compile();
    if (get_make_opt(OPT_ASSEMBLY))
        assembly();
    xrc();
    if (get_make_opt(OPT_LINKING))
        linking();
    return 0;
}