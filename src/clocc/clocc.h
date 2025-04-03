/* Software License Agreement
 *
 *     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 *
 *
 */

class clocc
{
  public:
    int Run(int argc, char** argv);

  protected:
    std::string SanitizeExtension(std::string fileName, std::string ext);
    int LinkOptions(std::string& args);

  private:
    static const char* usageText;
    static const char* helpText;

    static CmdSwitchParser SwitchParser;
    /*
/O1 maximum optimizations (favor space) /O2 maximum optimizations (favor speed)
/Od disable optimizations (default)
/Os favor code space                    /Ot favor code speed
/Ox optimizations (favor speed)         /Oy[-] enable frame pointer omission

/Fe<file> name executable file
/Fm[file] name map file                 /Fo<file> name object file
/Fi[file] name preprocessed file
/Fd: <file> name .PDB file              /Fe: <file> name executable file
/Fm: <file> name map file               /Fo: <file> name object file
/Fi: <file> name preprocessed file

/D<name>{=|#}<text> define macro
/E preprocess to stdout                 /EP preprocess to stdout, no #line
/P preprocess to file
/U<name> remove predefined macro
/I<dir> add to include search path

/std:<c++14|c++17|c++20|c++latest> C++ standard version
c++14 - ISO/IEC 14882:2014 (default)
c++17 - ISO/IEC 14882:2017
c++20 - ISO/IEC 14882:2020
c++latest - latest draft standard (feature set subject to change)
/Ze enable extensions (default)         /Za disable extensions
/Zi enable debugging information
/std:<c11|c17> C standard version
c11 - ISO/IEC 9899:2011
c17 - ISO/IEC 9899:2018

@<file> options response file           /?, /help print this help message
/c compile only, no link
/J default char type is unsigned
/MP[n] use up to 'n' processes for compilation
/nologo suppress copyright message      /showIncludes show include file names
/Tc<source file> compile file as .c     /Tp<source file> compile file as .cpp
/TC compile all files as .c             /TP compile all files as .cpp

/LD Create .DLL
/F<num> set stack size
/link [linker options and libraries]    /MD link with MSVCRT.LIB
/MT link with LIBCMT.LIB                /MDd link with MSVCRTD.LIB debug lib
/MTd link with LIBCMTD.LIB debug lib

/Wall enable all warnings               /w   disable all warnings
/W<n> set warning level (default n=1)
/WX treat warnings as errors            /WL enable one line diagnostics
/wd<n> disable warning n                /we<n> treat warning n as an error
/wo<n> issue warning n once             /w<l><n> set warning level 1-4 for n
*/

    static CmdSwitchBool prm_compileonly;
    static CmdSwitchBool prm_debug;
    static CmdSwitchBool prm_verbose;
    static CmdSwitchCombineString prm_define;
    static CmdSwitchCombineString prm_cinclude;
    static CmdSwitchCombineString prm_optimize;
    static CmdSwitchCombineString prm_undefine;
    static CmdSwitchBool prmDll;
    static CmdSwitchString prmOutputFile;
    static CmdSwitchBool prmPreprocessToStdout;
    static CmdSwitchBool prmPreprocessToFile;
    static CmdSwitchString prmStandard;
    static CmdSwitchCombineString prmExtensions;
    static CmdSwitchBool prmCharTypeIsUnsigned;
    static CmdSwitchBool prmShowIncludes;
    static CmdSwitchString prmCompileAs;
    static CmdSwitchCombineString prmLinkOptions;
    static CmdSwitchString prmLinkWithMSVCRT;
    static CmdSwitchCombineString prmWarningSetup;
    static CmdSwitchBool RuntimeObjectOverflow;
    static CmdSwitchBool RuntimeUninitializedVariable;
};