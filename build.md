Building Orange C
-----------------

These instructions are for building on WIN32.  I actually build on Windows 11 64-bit.

Following these instructions will build the compiler, but will not rebuild it with itself.   If you need to do that try using build.bat.

You will need either Visual Studio, CLANG, or MINGW to compile this set of applications.   For help support, C# tools version 4.0 and .NET framework 4.5 need to be installed.  Additionally, with the exception of the C# code this package currently will compile and link itself.

The current compilers being tested with are:

VISUAL STUDIO 2022
CLANG 15.0.1
MINGW64 8.1.0

You will also need 7-zip and inno setup, to do a complete build including making the distribution files.

When you install these programs install them to their default directories, or alternately edit the associated .MAK files to set the paths.  (note: I use the latest 64-bit version of 7-zip).  Make sure the compiler you choose is set in the PATH enironment variable before doing a build.

The distribution directories are meant to go in a subdirectory.  I name mine OrangeC and put it in the root, then put the SRC and DOC directories in there.  Later, the build steps may create other directories such as a BIN directory and an INCLUDE directory.  

You may have limited success using a deeply nested directory as the root.   Orange C has a path size limitation of 260 characters, and the path to some of the library files is nested very deeply.   Try using C:\orangec or c:\workingdir\orangec

There is a master makefile, and then there are multiple *.mak files which perform various subtasks.  In particulare there is a platform-specific make file for each compiler, and a makefile called 'treetop.mak' which performs most common tasks.

Note that treetop.mak does not automatically check dependencies for .h files.   That is something I will add at a later date.

You can change the version number for the compiler by editing `SRC\VERSION.H` then rebuilding everything.

Building orange C happens in three steps.  First, the executables are built.  Then, the C Runtime library is built.  After that, the distribution release files are built.

To build the executables the easiest thing to do is install Visual studio 2022.  Then you can either build the release version from the IDE, or from the command line.

Before starting a build set the environment variable ORANGEC to the root of the repository (usually c:\orangec)
Note that ORANGEC has to be set to the root of the sources you are trying to build; change it if you have a release version of
orangec installed.    This will also be part of making sure the correct executables are being run.


You can also add ORANGEC to the path.   Generally, you might not have to set the variable ORANGEC as the tools create it internally for basic operations, but, some of the make files still rely on it being set.

If you want to use VC to compile the executables, install the command line build environment (using VCVARS32.bat) and type:

    omake

or alternately

    ms.bat

You need to be in the src directory for build steps like this to work (usually \orangec\src or /c/orangec/src for msys2)

The first time you build various steps have to be taken to bootstrap the build.   For a first time build or if you want to make sure the entire thing gets built, cd to the source directory and type:

    omake fullbuild

This builds the tool chain, then builds the run time library, then creates an installer and a couple of zip files used for releases.  Note: on a first time build it can take a while to compile all the programs and libraries so get a cup of coffee!

you can do:

    omake fullbuild ORANGEC_ONLY=YES

and it won't compile the DOS libraries or package the results.

to compile with MINGW64:

    omake fullbuild COMPILER=MINGW64

to compile with CLANG:

    omake fullbuild COMPILER=CLANG

if you don't specify a compiler it defaults to MSVC (cl.exe)

If you want to build the executable files you would cd to the src directory and type

    omake exebuild

which builds a library for each tool then links the libraries into executable files.   Then it copies all the files into the \orangec\bin directory along with various configuration files that are necessary 

To recompile orange C with the current version of itself:

    omake COMPILER=OCC exebuild

note if you want to build the executables without putting them in the /orangec/bin directory you can just use omake without any switches

    omake

or if you want to build the sources with a specific compiler:

    omake COMPILER=OCC

to build the sources with a specific compiler and then put the exes in the /orangec/bin directory:

    omake COMPILER=OCC exebuild

if you've previously built with fullbuild to set up the environment, you can just build with no command line switches then type:

    omake copyexes

and it will copy the exe files to the bin directory.

If you want to build the runtime libraries you can type:

    omake librarybuild

and it will build the runtime library and move the include and library files to their proper place

again you can specify -ORANGEC_ONLY=YES to not build the DOS or OCCIL portions of the library.

If you want to clean up object files type:

    omake clean

which cleans up all the output files for the specific target (in this case msvc)

to clean up all output directories in the distribution type

    omake dist-clean

If you want to make a distribution after seperately building the EXE and LIBRARY files:

    omake distribute

which copies all files to the distribution directories and creates the distribution packages into `\orangec\dist`.  If you build instead from Visual Studio IDE or using MSBUILD you need to include /DMS:

    omake MS=YES distribute

which copies the Visual Studio binaries from `\orangec\src\release` instead of the binaries that are built by the make program in each destination directory.

after distributing you can rebuild the zip file:

    omake zip

or with 7z:

    omake zip7z

which build the zip files and put them in the /orangec/dist directory

To compile with debug information set the WITHDEBUG environment variable:

    omake WITHDEBUG=YES fullbuild

note that mostly, mingw32-make will perform all these tasks.   So it is possible to replace 'omake' with mingw32-make.

we have also tested the build steps on msys2 using the mingw compiler.

`omake` has an added function called 'tree' make.   Basically this mode is like a normal make mode, except it looks for a file called 'treetop.mak' in successively higher directories until it finds it.   Then it sets a couple of environment variables related to where it found `treetop.mak` and the current working directory, and invokes `treetop.mak`.

You invoke treetop.mak by using:

    omake /T

from either the source directory or one of the tools directories.   This means you can compile either the entire package, or you can compile each of the tools individually, depending on what path you have cd'd to.  In fact, `omake /T` is exactly what happens when you type `omake tools`.

