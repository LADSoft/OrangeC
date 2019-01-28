#include "afx.h"
#include <io.h>

static AFILE asm_files;
static AFILE lib_files;
static AFILE cpp_files;
static AFILE obj_files;
static AFILE rc_files;
static AFILE res_files;

void clear_all(void)
{
    afile_del(&cpp_files);
    afile_del(&asm_files);
    afile_del(&obj_files);
    afile_del(&lib_files);
    afile_del(&rc_files);
    afile_del(&res_files);
}

int add_lib_file(char* file)
{
    assert(file != NULL);
    return afile_add(&lib_files, file, 0);
}

int lib_file_cnt(void) { return afile_cnt(&lib_files); }

char* lib_file(int index) { return afile_fname(&lib_files, index); }

int add_rc_file(char* file)
{
    assert(file != NULL);
    return afile_add(&rc_files, file, 0);
}

int rc_file_cnt(void) { return afile_cnt(&rc_files); }

char* rc_file(int index) { return afile_fname(&rc_files, index); }

int add_res_file(char* file)
{
    assert(file != NULL);
    return afile_add(&res_files, file, 0);
}

int add_res_tmpfile(char* file)
{
    assert(file != NULL);
    return afile_add(&res_files, file, 1);
}
int res_file_cnt(void) { return afile_cnt(&res_files); }

char* res_file(int index) { return afile_fname(&res_files, index); }

void del_res_tmpfiles(void) { afile_deltemp(&res_files); }

int add_obj_file(char* file)
{
    assert(file != NULL);
    return afile_add(&obj_files, file, 0);
}

int add_obj_tmpfile(char* file)
{
    assert(file != NULL);
    return afile_add(&obj_files, file, 1);
}

int obj_file_cnt(void) { return afile_cnt(&obj_files); }

char* obj_file(int index) { return afile_fname(&obj_files, index); }

void del_obj_tmpfiles(void) { afile_deltemp(&obj_files); }

int add_asm_file(char* file)
{
    assert(file != NULL);
    return afile_add(&asm_files, file, 0);
}

int add_asm_tmpfile(char* file)
{
    assert(file != NULL);
    return afile_add(&asm_files, file, 1);
}

int asm_file_cnt(void) { return afile_cnt(&asm_files); }

char* asm_file(int index) { return afile_fname(&asm_files, index); }

void del_asm_tmpfiles(void) { afile_deltemp(&asm_files); }

int add_cpp_file(char* file)
{
    assert(file != NULL);
    return afile_add(&cpp_files, file, 0);
}

void clear_asm_files(void) { afile_del(&asm_files); }

int cpp_file_cnt(void) { return afile_cnt(&cpp_files); }

char* cpp_file(int index) { return afile_fname(&cpp_files, index); }

static char exe_file[LINE_MAX];
/* static char err_file[ LINE_MAX ]; */

char* get_exe_name(void) { return exe_file; }

void set_exe_name(char* name)
{
    assert(name != NULL);
    strcpy(exe_file, name);
}

static void one_source(char* file);
static void sources(char* fpath, char* fname);

void one_source(char* file)
{
    assert(file != NULL);

    fn_split(file);

    if (!exe_file[0])
    {
        strcpy(exe_file, fn_name());
        strcat(exe_file, ".exe");
    }

    if (stricmp(fn_ext(), ".l") == 0)
    {
        if (!add_lib_file(file))
            fprintf(stderr, "File '%s' already add!\n", file);
        return;
    }
    if (stricmp(fn_ext(), ".o") == 0)
    {
        if (!add_obj_file(file))
            fprintf(stderr, "File '%s' already add!\n", file);
        return;
    }
    if (stricmp(fn_ext(), ".asm") == 0)
    {
        if (!add_asm_file(file))
            fprintf(stderr, "File '%s' already add!\n", file);
        if (!add_obj_tmpfile(fn_temp(".o")))
            fprintf(stderr, "File '%s' already add!\n", file);
        return;
    }
    if (stricmp(fn_ext(), ".res") == 0)
    {
        if (!add_res_file(file))
            fprintf(stderr, "File '%s' already add!\n", file);
        return;
    }
    if (stricmp(fn_ext(), ".rc") == 0)
    {
        if (!add_rc_file(file))
            fprintf(stderr, "File '%s' already add!\n", file);
        if (!add_res_tmpfile(fn_temp(".res")))
            fprintf(stderr, "File '%s' already add!\n", file);
        return;
    }
    if (!add_cpp_file(file))
        fprintf(stderr, "File '%s' already add!\n", file);
    if (!add_asm_tmpfile(fn_temp(".asm")))
        fprintf(stderr, "File '%s' already add!\n", file);
    if (!add_obj_tmpfile(fn_temp(".o")))
        fprintf(stderr, "File '%s' already add!\n", file);
}

void source(char* file)
{
    static char findpath[_MAX_PATH];
    static char findname[_MAX_PATH];

    fn_split(file);
    /*
        _splitpath( file, disk, path, name, ext );
    */
    if (strchr(file, '?') || strchr(file, '*'))
    { /* Check for '*' and '?' */
        _makepath(findpath, fn_drive(), fn_dir(), NULL, NULL);
        _makepath(findname, NULL, NULL, fn_name(), fn_ext());
        sources(findpath, findname);
    }
    else
        one_source(file);
}

#if defined(__TURBOC__)

#    include <dir.h>
#    include <dos.h>

static void sources(char* fpath, char* fname)
{
    int fpath_len = strlen(fpath);
    struct ffblk fb;
    if (!findfirst(strcat(fpath, fname), &fb, FA_ARCH))
        do
        {
            fpath[fpath_len] = '\0';
            one_source(strcat(fpath, fb.ff_name));
        } while (!findnext(&fb));
}

#else

#    include <dos.h>
static void sources(char* fpath, char* fname)
{
    int fpath_len = strlen(fpath);
    struct _finddata_t fb;
    long t = _findfirst(strcat(fpath, fname), &fb);
    if (t)
        do
        {
            fpath[fpath_len] = '\0';
            one_source(strcat(fpath, fb.name));
            printf("found %s\n", fpath);
        } while (_findnext(t, &fb));

    if (t)
        _findclose(t);
}
/*
static void sources( char *fpath, char *fname ) {
    int fpath_len = strlen( fpath );
    struct find_t fb;
    if (!_dos_findfirst( strcat( fpath, fname), _A_NORMAL, &fb ) )
    do {
        fpath[ fpath_len ] = '\0';
        one_source( strcat( fpath, fb.name) );
        printf( "found %s\n", fpath );
    } while( ! _dos_findnext( &fb ) );
}
*/
#endif
