
char* text =
"config.cxx_under_test           = \"occ.exe\"\n"
"config.test_retry_attempts      = 5\n"
"config.std                      = \"c++14\"\n"
"#config.project_obj_root         = \"..\obj\"\n"
"#config.libcxx_src_root          = \"..\\\"\n"
"#config.libcxx_obj_root          = \"..\bin\"\n"
"#config.cxx_library_root         = \"c:/orangec/lib\"\n"
"config.enable_exceptions        = True\n"
"config.enable_experimental      = False\n"
"config.enable_filesystem        = False\n"
"config.enable_rtti              = True\n"
"config.enable_shared            = False\n"
"config.enable_32bit             = False\n"
"#config.cxx_abi                  = \"\"\n"
"config.use_sanitizer            = None\n"
"#config.sanitizer_library        = \"\"\n"
"#config.abi_library_path         = \"\"\n"
"#config.configuration_variant    = \"\"\n"
"#config.host_triple              = \"\"\n"
"config.target_triple            = \"x86_64-pc-windows-msvc\"\n"
"config.use_target               = False\n"
"#config.sysroot                  = \"\"\n"
"#config.gcc_toolchain            = False\n"
"#config.generate_coverage        = False\n"
"#config.target_info              = \"\"\n"
"#config.test_linker_flags        = \"\"\n"
"#config.test_compiler_flags      = \"\"\n"
"\n"
"#config.executor                 = \"\"\n"
"#config.llvm_unwinder            = False\n"
"#config.builtins_library         = \"\"\n"
"config.has_libatomic            = True\n"
"config.use_libatomic            = True\n"
"config.debug_build              = False\n"
"#config.libcxxabi_shared         = False\n"
"#config.cxx_ext_threads          = False\n"
"config.pstl_src_root            = \"\"\n"
"config.pstl_obj_root            = \"\"\n"
"#config.libcxx_gdb               = \"\"\n"
"\n"
"# Code signing\n"
"#config.llvm_codesign_identity   = None\n"
"\n"
"# Let the main config do the real work.\n"
"config.loaded_site_config = True\n"
"lit_config.load_config(config, \"%s/src/clibs/cpp/libcxx/test/lit.cfg\")\n"
"\n";

#include <stdio.h>

int main()
{
	char *s = getenv("ORANGEC");
        if (s)
            printf(text, s);
}
