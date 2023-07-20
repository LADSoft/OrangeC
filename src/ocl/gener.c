#include "afx.h"

#ifdef TARGET_OS_WINDOWS
#    define system(x) winsystem(x)
extern int winsystem(const char*);
#endif

static char args[LINE_MAX];

static void wasm(void);
static void lasm(void);
static void lasm32(void);
static void masm(void);
static void ml(void);
static void nasm(void);
static void tasm(void);
static void tasmx(void);
static void tasm32(void);
static void link(void);
static void tlink(void);
static void valx(void);
static void wlink(void);
static void olink(void);

static void exec(char* program, char* param)
{
    static char* args[3];
    int retcode;

    assert(program != NULL);
    assert(param != NULL);
#if DEBUG
    printf("*** Search '%s'\n", program);
#endif
    fn_split(program);

    args[0] = xfind(fn_make(".exe"), getrundir());
    args[1] = param;
    args[2] = NULL;

#if DEBUG
    printf("*** Run '%s' '%s'\n", args[0], args[1]);
#endif
    /*
    retcode = spawnvp( P_WAIT, args[0], (void *)args );
    */
    {
        char buf[256];
        sprintf(buf, "\"%s\" %s", args[0], args[1]);
        retcode = system(buf);
    }

    if (retcode > 0)
    {
        fprintf(stderr, "\n'%s' exitcode = %d\n", program, retcode);
        if (!get_option(OPT_KEEPRSP))
            unlink(RESPONSE);
        exit(retcode);
    }
    else if (retcode < 0)
    {
        perror(program);
        if (!get_option(OPT_KEEPRSP))
            unlink(RESPONSE);
        exit(1);
    }
}

static FILE* response(char* fname)
{
    FILE* fp;

    assert(fname != NULL);
    unlink(fname);
    fp = fopen(fname, "wt");
    if (!fp)
    {
        perror(fname);
        exit(1);
    }
    return fp;
}

void compile(void)
{
    int i;
    char buf[256];
    if (cpp_file_cnt() > 0)
    {
        FILE* fp = response(RESPONSE);
        int sys = get_dosex();
        fprintf(fp, " /W%c %s%ci %ce %cl %cA %s \"/I%s\"",
                (sys == OPT_W32CON || sys == OPT_W32GUI || sys == OPT_W32DLL
                     ? 'c'
                     : 'a'),  // we aren't being too specific on the /W switch
                              // since the compiler isn't spawning the linker directly
                (get_option(OPT_C99) ? "/9 " : ""), (get_option(OPT_CPPFILE) ? '+' : '-'), (get_option(OPT_ERRFILE) ? '+' : '-'),
                (get_option(OPT_LSTFILE) ? '+' : '-'), (get_option(OPT_EXTEND) ? '+' : '-'),
                //( get_option( OPT_EXCEPTION) ? '+' : '-' ),
                get_cc_opt(), get_inc_path());
        for (i = 0; i < cpp_file_cnt(); i++)
            fprintf(fp, " %s", cpp_file(i));
        fclose(fp);
        // this is being done the hard way because of a vagary in the compiler:
        // it cannot handle /S in a response file.  I'll fix it later...
        if (get_assembler() == OPT_NASM)
            sprintf(buf, " /S ");
        else if (get_assembler() == OPT_WASM || get_assembler() == OPT_LASM || get_assembler() == OPT_MASM ||
                 get_assembler() == OPT_ML)
            sprintf(buf, " -C+M ");
        else
            sprintf(buf, " /c ");
        strcat(buf, "@" RESPONSE);
        exec("occ", buf);
        if (!get_option(OPT_KEEPRSP))
            unlink(RESPONSE);
    }
}

void assembly(void)
{

    switch (get_assembler())
    {
        case OPT_NASM:
            nasm();
            break;
        case OPT_TASM:
            tasm();
            break;
        case OPT_TASMX:
            tasmx();
            break;
        case OPT_TASM32:
            tasm32();
            break;
        case OPT_MASM:
            masm();
            break;
        case OPT_ML:
            ml();
            break;
        case OPT_WASM:
            wasm();
            break;
        case OPT_LASM:
            lasm();
            break;
        case OPT_LASM32:
            lasm32();
            break;
        default:
            fprintf(stderr, "Unknown assembler, use NASN...\n");
            nasm();
            break;
    }
    if (!get_option(OPT_KEEPGEN))
        del_asm_tmpfiles();
}

void linking(void)
{
    switch (get_linker())
    {
        case OPT_VALX:
            valx();
            break;
        case OPT_TLINK:
            tlink();
            break;
        case OPT_LINK:
            link();
            break;
        case OPT_WLINK:
            wlink();
            break;
        case OPT_OLINK:
            olink();
            break;
        default:
            fprintf(stderr, "Unknown linker, use VALX...\n");
            valx();
            break;
    }

    if (!get_option(OPT_KEEPGEN))
        del_obj_tmpfiles();
}

/*
**	Assembler
*/

void wasm(void)
{
    int i;

    if (asm_file_cnt() > 0)
    {
        for (i = 0; i < asm_file_cnt(); i++)
        {
            fn_split(asm_file(i));
            sprintf(args, "-4ps -fpi87 -q -mf %s -fo=%s -fe=%s %s", get_option(OPT_DEBUG) ? "/d1" : "", fn_temp(".obj"),
                    get_option(OPT_LSTFILE) ? fn_temp(".lst") : "NUL", asm_file(i));
            exec("WASM", args);
        }
    }
}

static void lasmz(char* lasm_name);

void lasm(void) { lasmz("LASM"); }

void lasm32(void) { lasmz("LASM32"); }

void lasmz(char* lasm_name)
{
    int i;

    if (asm_file_cnt() > 0)
    {
        for (i = 0; i < asm_file_cnt(); i++)
        {
            fn_split(asm_file(i));
            sprintf(args, "/G1 /O /S /U /Y %s /FO%s /L%s %s", get_option(OPT_DEBUG) ? "/ZH" : "", fn_temp(".obj"),
                    get_option(OPT_LSTFILE) ? fn_temp(".lst") : "NUL", asm_file(i));
            exec(lasm_name, args);
        }
    }
}

void masm(void)
{
    int i;

    if (asm_file_cnt() > 0)
    {
        for (i = 0; i < asm_file_cnt(); i++)
        {
            fn_split(asm_file(i));
            sprintf(args, "/t /Ml %s %s,%s,%s,NUL", get_option(OPT_DEBUG) ? "/Zi" : "", asm_file(i), fn_temp(".obj"),
                    get_option(OPT_LSTFILE) ? fn_temp(".lst") : "NUL");
            exec("MASM", args);
        }
    }
}

void ml(void)
{
    int i;

    if (asm_file_cnt() > 0)
    {
        for (i = 0; i < asm_file_cnt(); i++)
        {
            fn_split(asm_file(i));
            sprintf(args, "/nologo /c /Cp %s /Fo%s /Fl%s %s", get_option(OPT_DEBUG) ? "/Zi /Zd" : "", fn_temp(".obj"),
                    get_option(OPT_LSTFILE) ? fn_temp(".lst") : "NUL", asm_file(i));
            exec("ML", args);
        }
    }
}

void nasm(void)
{
    int i;

    if (asm_file_cnt() > 0)
    {
        for (i = 0; i < asm_file_cnt(); i++)
        {
            if (get_option(OPT_LSTFILE))
            {
                strcat(args, "-l ");
            }
            strcat(args, asm_file(i));
            exec("OASM", args);
        }
    }
}

static void tasmz(char*);

void tasm(void) { tasmz("TASM"); }

void tasmx(void) { tasmz("TASMX"); }

void tasm32(void) { tasmz("TASM32"); }

void tasmz(char* tasm_name)
{
    FILE* fp;
    int i;

    if (asm_file_cnt() > 0)
    {
        fp = response(RESPONSE);
        for (i = 0; i < asm_file_cnt(); i++)
        {
            fprintf(fp, "/t /ml /m /z%c", get_option(OPT_DEBUG) ? 'i' : 'n');
            fn_split(asm_file(i));
            fprintf(fp, " %s,%s,%s,NUL;\n", asm_file(i), fn_temp(".obj"), get_option(OPT_LSTFILE) ? fn_temp(".lst") : "NUL");
        }
        fclose(fp);
        exec(tasm_name, "@" RESPONSE);
        if (!get_option(OPT_KEEPRSP))
            unlink(RESPONSE);
    }
}

void olink(void)
{
    int i;
    FILE* fp;
    char* c0 = "c0pm.o";
    char* c0d = "c0pmd.o";
    char* cl = "cldos.l";
    char* ci = NULL;
    char* lnk_op = "/T:PM";

    if ((obj_file_cnt() + lib_file_cnt()) > 0)
    {
        int sys = get_dosex();
        switch (sys)
        {
            case OPT_W32CON:
                c0d = c0 = "c0xpe.o";
                cl = "clwin.l";
                ci = "climp.l";
                lnk_op = "/T:CON32";
                break;
            case OPT_W32GUI:
                c0d = c0 = "c0pe.o";
                cl = "clwin.l";
                ci = "climp.l";
                lnk_op = "/T:GUI32";
                break;
            case OPT_W32DLL:
                c0d = c0 = "c0dpe.o";
                cl = "clwin.l";
                ci = "climp.l";
                lnk_op = "/T:DLL32";
                break;
            case OPT_DOS32A:
                c0d = "c0watd.o";
                c0 = "c0wat.o";
                lnk_op = "/T:DOS32;sstub32a.exe";
                break;
            case OPT_PMODEW:
                c0d = "c0watd.o";
                c0 = "c0wat.o";
                lnk_op = "/T:DOS32;spmodew.exe";
                break;
            case OPT_DOS4G:
                c0d = "c0watd.o";
                c0 = "c0wat.o";
                lnk_op = "/T:DOS32;s4gstub.exe";
                break;
            case OPT_WDOSX:
                c0d = "c0watd.o";
                c0 = "c0wat.o";
                lnk_op = "/T:DOS32;swdosxle.exe";
                break;
            case OPT_ZRDX:
                c0d = "c0watd.o";
                c0 = "c0wat.o";
                lnk_op = "/T:DOS32;szrdx.exe";
                break;
            case OPT_CAUSEWAY:
                c0d = "c0watd.o";
                c0 = "c0wat.o";
                lnk_op = "/T:DOS32;scwstub.exe";
                break;
            case OPT_HXDOS:
                c0d = c0 = "c0xpe.o";
                cl = "CLWIN.L";
                ci = "CLIMP.L";
                lnk_op = "/T:CON32;sdpmist32.bin";
                break;
            case OPT_WHXDOS:
                c0d = "c0hxd.o";
                c0 = "c0hx.o";
                lnk_op = "/T:CON32;shdld32.bin /DOBJECTALIGN=10000";
                break;
            case OPT_D3X:
                c0d = "c0watd.o";
                c0 = "c0wat.o";
                lnk_op = "/T:DOS32;sd3xw.exe";
                break;
            default:
                fprintf(stderr, "VALX use only with PMODE!\n");
            case OPT_PMODE:
                break;
        }
        fp = response(RESPONSE);
        fprintf(fp, "-c%s", get_option(OPT_MAPFILE) ? " -m" : "");
        fprintf(fp, " -o%s%s", get_exe_name(), get_option(OPT_MAPFILE) ? get_exe_name() : "");
        if (lnk_op)
        {
            fprintf(fp, " %s", lnk_op);
        }
        if (!get_option(OPT_NODEFLIB))
        {
            if (get_option(OPT_DEBUG))
                fprintf(fp, " \"%s\\%s\"", get_syslib_path(), c0d);
            else
                fprintf(fp, " \"%s\\%s\"", get_syslib_path(), c0);
        }
        for (i = 0; i < obj_file_cnt(); i++)
            fprintf(fp, " %s", obj_file(i));
        for (i = 0; i < lib_file_cnt(); i++)
            fprintf(fp, " %s", lib_file(i));
        if (!get_option(OPT_NODEFLIB))
        {
            if (cl)
            {
                fprintf(fp, " \"%s\\%s\"", get_syslib_path(), cl);
            }
            if (ci)
            {
                fprintf(fp, " \"%s\\%s\"", get_syslib_path(), ci);
            }
        }
        for (i = 0; i < res_file_cnt(); i++)
            fprintf(fp, " %s", res_file(i));
        fclose(fp);
        exec("olink", "@" RESPONSE);
        if (!get_option(OPT_KEEPRSP))
            unlink(RESPONSE);
        if (!get_option(OPT_KEEPGEN))
        {
            del_res_tmpfiles();
        }
        if (sys == OPT_WHXDOS)
        {
            exec("PATCHPE", get_exe_name());
        }
    }
}
void link(void)
{
    int i;
    FILE* fp;

    if ((obj_file_cnt() + lib_file_cnt()) > 0)
    {
        if (get_dosex() != OPT_PMODE)
            fprintf(stderr, "LINK use only with PMODE!\n");
        sprintf(args, "/NOIGNORECASE %s @" RESPONSE " ", get_option(OPT_MAPFILE) ? "/MAP" : "");
        fp = response(RESPONSE);
        if (!get_option(OPT_NODEFLIB))
        {
            if (get_option(OPT_DEBUG))
                fprintf(fp, "%s\\C0DOSD.OBJ", get_syslib_path());
            else
                fprintf(fp, "%s\\C0DOS.OBJ", get_syslib_path());
        }
        for (i = 0; i < obj_file_cnt(); i++)
            fprintf(fp, " %s", obj_file(i));
        fprintf(fp, "\n%s\n%s\n", get_exe_name(), get_option(OPT_MAPFILE) ? get_exe_name() : "NUL");
        if (!get_option(OPT_NODEFLIB))
            fprintf(fp, "%s\\CLDOS.LIB", get_syslib_path());
        for (i = 0; i < lib_file_cnt(); i++)
            fprintf(fp, " %s", lib_file(i));
        fprintf(fp, "\n");
        fclose(fp);
        exec("LINK", args);
        if (!get_option(OPT_KEEPRSP))
            unlink(RESPONSE);
    }
}

void tlink(void)
{
    int i;
    FILE* fp;

    if ((obj_file_cnt() + lib_file_cnt()) > 0)
    {
        if (get_dosex() != OPT_PMODE)
            fprintf(stderr, "TLINK use only with PMODE!\n");
        fp = response(RESPONSE);
        fprintf(fp, "/3/c/d%s%s", get_option(OPT_MAPFILE) ? "/m/l/s" : "/x", get_option(OPT_DEBUG) ? "/v" : "");
        if (lib_file_cnt() > 0)
            fprintf(fp, "/L%s ", get_lib_path());
        if (!get_option(OPT_NODEFLIB))
        {
            if (get_option(OPT_DEBUG))
                fprintf(fp, "%s\\C0DOSD.OBJ", get_syslib_path());
            else
                fprintf(fp, "%s\\C0DOS.OBJ", get_syslib_path());
        }
        for (i = 0; i < obj_file_cnt(); i++)
            fprintf(fp, " %s", obj_file(i));
        fprintf(fp, ",%s,%s,", get_exe_name(), get_option(OPT_MAPFILE) ? get_exe_name() : "NUL");
        if (!get_option(OPT_NODEFLIB))
            fprintf(fp, "%s\\CLDOS.LIB", get_syslib_path());
        for (i = 0; i < lib_file_cnt(); i++)
            fprintf(fp, " %s", lib_file(i));
        fprintf(fp, "\n");
        fclose(fp);
        exec("TLINK", "@" RESPONSE);
        if (!get_option(OPT_KEEPRSP))
            unlink(RESPONSE);
    }
}

void valx(void)
{
    int i;
    FILE* fp;
    char* c0 = "C0DOS.OBJ";
    char* c0d = "C0DOSD.OBJ";
    char* cl = "CLDOS.LIB";
    char* ci = NULL;
    char* dosx = NULL;
    char* lnk_op = "/NCI /32";

    if ((obj_file_cnt() + lib_file_cnt()) > 0)
    {
        int sys = get_dosex();
        switch (sys)
        {
            case OPT_W32CON:
                c0d = c0 = "C0xwin.obj";
                cl = "CLWIN.LIB";
                ci = "CLIMP.LIB";
                lnk_op = "/NCI /PE";
                break;
            case OPT_W32GUI:
                c0d = c0 = "C0win.obj";
                cl = "CLWIN.LIB";
                ci = "CLIMP.LIB";
                lnk_op = "/NCI /PE /WIN";
                break;
            case OPT_W32DLL:
                c0d = c0 = "C0dwin.obj";
                cl = "CLWIN.LIB";
                ci = "CLIMP.LIB";
                lnk_op = "/NCI /PE /BDL";
                break;
            case OPT_DOS32A:
                c0d = "c0doswd.obj";
                c0 = "c0dosw.obj";
                lnk_op = "/NCI /LE";
                dosx = "stub32a.exe";
                break;
            case OPT_PMODEW:
                c0d = "c0doswd.obj";
                c0 = "c0dosw.obj";
                lnk_op = "/NCI /LE";
                dosx = "pmodew.exe";
                break;
            case OPT_DOS4G:
                c0d = "c0doswd.obj";
                c0 = "c0dosw.obj";
                lnk_op = "/NCI /LE";
                dosx = "4gstub.exe";
                break;
            case OPT_WDOSX:
                c0d = "c0doswd.obj";
                c0 = "c0dosw.obj";
                lnk_op = "/NCI /LE";
                dosx = "wdosxle.exe";
                break;
            case OPT_ZRDX:
                c0d = "c0doswd.obj";
                c0 = "c0dosw.obj";
                lnk_op = "/NCI /LE";
                dosx = "zrdx.exe";
                break;
            case OPT_CAUSEWAY:
                c0d = "c0doswd.obj";
                c0 = "c0dosw.obj";
                lnk_op = "/NCI /LE";
                dosx = "cwstub.exe";
                break;
            case OPT_HXDOS:
                c0d = c0 = "C0xwin.obj";
                cl = "CLWIN.LIB";
                ci = "CLIMP.LIB";
                lnk_op = "/NCI /PE";
                dosx = "dpmist32.bin";
                break;
            case OPT_WHXDOS:
                c0d = c0 = "C0doswhx.obj";
                lnk_op = "/NCI /PE";
                dosx = "hdld32.bin";
                break;
            case OPT_D3X:
                c0d = "c0doswd.obj";
                c0 = "c0dos.obj";
                lnk_op = "/NCI /LE";
                dosx = "d3xw.exe";
                break;
            default:
                fprintf(stderr, "VALX use only with PMODE!\n");
            case OPT_PMODE:
                break;
        }
        fp = response(RESPONSE);
        fprintf(fp, "-NOCA -USE32 %s ", get_option(OPT_MAPFILE) ? "-MAP" : "-NOMAP");
        if (lnk_op)
            fprintf(fp, "%s ", lnk_op);
        if (dosx)
            fprintf(fp, "-STB:(%s\\%s) ", get_syslib_path(), dosx);
        if (!get_option(OPT_NODEFLIB))
        {
            if (get_option(OPT_DEBUG))
                fprintf(fp, "\"%s\\%s\"", get_syslib_path(), c0d);
            else
                fprintf(fp, "\"%s\\%s\"", get_syslib_path(), c0);
        }
        for (i = 0; i < obj_file_cnt(); i++)
            fprintf(fp, " %s", obj_file(i));
        fprintf(fp, ",%s,%s,", get_exe_name(), get_option(OPT_MAPFILE) ? get_exe_name() : "NUL");
        if (!get_option(OPT_NODEFLIB))
            fprintf(fp, "\"%s\\%s\"", get_syslib_path(), cl);
        if (ci)
            fprintf(fp, " \"%s\\%s\"", get_syslib_path(), ci);
        for (i = 0; i < lib_file_cnt(); i++)
            fprintf(fp, " %s", lib_file(i));
        fprintf(fp, "\n");
        for (i = 0; i < res_file_cnt(); i++)
            fprintf(fp, " %s", res_file(i));
        fprintf(fp, "\n\n");
        fclose(fp);
        exec("VALX", "@" RESPONSE);
        if (!get_option(OPT_KEEPRSP))
            unlink(RESPONSE);
        if (!get_option(OPT_KEEPGEN))
        {
            del_res_tmpfiles();
        }
        if (sys == OPT_WHXDOS)
        {
            exec("PATCHPE", get_exe_name());
        }
    }
}

void wlink(void)
{
    int i;
    char* dosx;
    FILE* fp;

    if ((obj_file_cnt() + lib_file_cnt()) > 0)
    {
        switch (get_dosex())
        {
            case OPT_PMODEW:
                dosx = "pmodew.exe";
                break;
            case OPT_DOS4G:
                dosx = "4gs.exe";
                break;
            case OPT_WDOSX:
                dosx = "wdosxle.exe";
                break;
            case OPT_DOS32A:
                dosx = "dos32a.exe";
                break;
            case OPT_ZRDX:
                dosx = "zrdx.exe";
                break;
            case OPT_CAUSEWAY:
                dosx = "cwstub.exe";
                break;
            case OPT_D3X:
                dosx = "d3xw.exe";
                break;
            case OPT_PMODE:

            default:
                fprintf(stderr, "WLINK NO use with PMODE!\n");
                dosx = "4gs.exe";
                break;
        }
        fp = response(RESPONSE);
        fprintf(fp,
                "op osname=CC386[%s]\n"
                "format os2 le\n"
                "op nod\n"
                "op quiet\n"
                "op stub=%s\\%s\n"
                "name %s\n",
                dosx, get_syslib_path(), dosx, get_exe_name());
        if (get_option(OPT_MAPFILE))
            fprintf(fp, "op map\n");
        if (get_option(OPT_DEBUG))
            fprintf(fp, "debug all \nop symf\n");
        if (!get_option(OPT_NODEFLIB))
            fprintf(fp, "file %s\\C0DOSW.OBJ\n", get_syslib_path());
        for (i = 0; i < obj_file_cnt(); i++)
            fprintf(fp, "file %s\n", obj_file(i));
        if (!get_option(OPT_NODEFLIB))
            fprintf(fp, "library %s\\CLDOS.LIB\n", get_syslib_path());
        for (i = 0; i < lib_file_cnt(); i++)
            fprintf(fp, "library %s\n", lib_file(i));
        fclose(fp);
        exec("WLINK", "@" RESPONSE);
        if (!get_option(OPT_KEEPRSP))
            unlink(RESPONSE);
    }
}
void xrc(void)
{
    int i;

    if (rc_file_cnt() > 0)
    {
        for (i = 0; i < rc_file_cnt(); i++)
        {
            sprintf(args, "/r \"/i%s\" ", get_inc_path());
            strcat(args, rc_file(i));
            exec("orc", args);
        }
    }
}
