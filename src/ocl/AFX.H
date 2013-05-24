#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <process.h>

#if !defined(DEBUG)
#  define DEBUG 0
#endif

#include "..\version.h"

typedef struct {
	int temp;
	char fname[1];
} XFILE;

#define SIZE_BLOCK 64

typedef struct {
	int size;
	int count;
	XFILE **array;
} AFILE;

#define PATH_DELIM	';'
#define DIR_DELIM	'\\'
#define CC_DELIM	' '

#define LINE_MAX 256

/* make options */
#define OPT_COMPILE		0x01
#define OPT_ASSEMBLY	0x02
#define OPT_LINKING		0x04

/* options */
#define OPT_ERRFILE 	0x001
#define OPT_CPPFILE 	0x002
#define OPT_LSTFILE 	0x004
#define OPT_EXTEND  	0x008
#define OPT_MAPFILE 	0x010
#define OPT_NODEFLIB	0x020
#define OPT_DEBUG		0x040
#define OPT_KEEPGEN 	0x080
#define OPT_KEEPRSP 	0x100
#define OPT_EXCEPTION	0x200
#define OPT_QUIET		0x400
#define OPT_C99         0x800

/* assemblers */
#define OPT_NASM 		1
#define OPT_TASM        2
#define OPT_TASMX       3
#define OPT_TASM32      4
#define OPT_MASM        5
#define OPT_ML          6
#define OPT_WASM        7
#define OPT_LASM        8
#define OPT_LASM32      9
/* linkers */
#define OPT_VALX 		1
#define OPT_TLINK       2
#define OPT_LINK        3
#define OPT_WLINK       4
#define OPT_OLINK		5

/* dos extenders */
#define OPT_PMODE 		1
#define OPT_PMODEW      2
#define OPT_DOS4G       3
#define OPT_WDOSX       4
#define OPT_DOS32A      5
#define OPT_ZRDX        6
#define OPT_CAUSEWAY	7
#define OPT_W32CON      8
#define OPT_W32GUI      9
#define OPT_W32DLL     10
#define OPT_HXDOS      11
#define OPT_WHXDOS	   12
#define OPT_D3X		   13

#define RESPONSE "OCL.$$$"

extern int afile_add(AFILE *afile,char *fname,int temp);
extern int afile_cnt(AFILE *afile);
extern char *afile_fname(AFILE *afile,int index);
extern void afile_deltemp(AFILE *afile);
extern void afile_del(AFILE *afile);
extern char *getrundir(void );
extern void fn_split(char *fname);
extern char *fn_drive(void );
extern char *fn_dir(void );
extern char *fn_name(void );
extern char *fn_ext(void );
extern char *fn_path(void );
extern char *fn_make(char *fext);
extern char *fn_temp(char *fext);
extern char *xfind(char *fname,char *rundir);
extern void make_path(char *fname,char *dir,char *buf);
extern int get_assembler(void );
extern int get_linker(void );
extern int get_dosex(void );
extern int get_option(int what);
extern int get_make_opt(int what);
extern char *get_cc_opt(void );
extern char *get_inc_path(void );
extern char *get_lib_path(void );
extern char *get_syslib_path(void );
extern void option(char *opt);
extern int last_char(char *str);
extern char *add2path(char *path,char *add,int delim);
extern void clear_all(void );
extern int add_lib_file(char *file);
extern int lib_file_cnt(void );
extern char *lib_file(int index);
extern int add_rc_file(char *file);
extern int rc_file_cnt(void );
extern char *rc_file(int index);
extern int add_res_file(char *file);
extern int add_res_tmpfile(char *file);
extern void del_res_tmpfiles(void );
extern int res_file_cnt(void );
extern char *res_file(int index);
extern int add_obj_file(char *file);
extern int add_obj_tmpfile(char *file);
extern int obj_file_cnt(void );
extern char *obj_file(int index);
extern void del_obj_tmpfiles(void );
extern int add_asm_file(char *file);
extern int add_asm_tmpfile(char *file);
extern int asm_file_cnt(void );
extern char *asm_file(int index);
extern void del_asm_tmpfiles(void );
extern int add_cpp_file(char *file);
extern void clear_asm_files(void );
extern int cpp_file_cnt(void );
extern char *cpp_file(int index);
extern char *get_exe_name(void );
extern void set_exe_name(char *name);
extern void source(char *file);
extern void help(void );
extern void parse_params(char *env,char *cfgfile,char *argv[],int argc);
extern void parse_param_line(char *s);
extern void parse_param_file(char *file);
extern void parse_param(char *param);
extern XFILE *xfile_new(char *file,int istempfile);
extern void xfile_del(XFILE *xfile);
extern int xfile_temp(XFILE *xfile);
extern char *xfile_fname(XFILE *xfile);
extern void compile(void );
extern void assembly(void );
extern void linking(void );
extern void xrc(void ) ;