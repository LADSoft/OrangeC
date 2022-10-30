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

The build may not work completely if you don't put orangec in the root of `C:`. So for example `C:\ORANGEC\SRC` is where all the source files go.  If you want to change it go into src/treetop.mak and change the definition of DISTROOT.  You may have to fiddle with other makefiles if you do change it.

There is a master makefile, and then there are multiple *.mak files which perform various subtasks.  In particulare there is a platform-specific make file for each compiler, and a makefile called 'treetop.mak' which performs most common tasks.

Note that treetop.mak does not automatically check dependencies for .h files.   That is something I will add at a later date.

You can change the version number by editing `SRC\VERSION.H` then rebuilding everything.

Building orange C happens in three steps.  First, the executables are built.  Then, the C Runtime library is built.  After that, the distribution release files are built.

To build the executables the easiest thing to do is install Visual studio 2033.  Then you can either build the release version from the IDE, or from the command line.

If you want to use VC to compile the executables, install the command line build environment (using VCVARS32.bat) and type:

    omake

or alternately

    ms.bat

The first time you build various steps have to be taken to bootstrap the build.   For a first time build or if you want to make sure the entire thing gets built, cd to the source directory and type:

    omake fullbuild

This builds the tool chain, then builds the run time library, then creates an installer and a couple of zip files used for releases.  Note: on a first time build it can take a while to compile all the programs and libraries so get a cup of coffee!

you can do:

    omake fullbuild -DORANGEC_ONLY=YES

and it won't compile the DOS libraries, IDE-related code, or the occil runtime libraries or package the results

to compile with MINGW64:

    omake fullbuild -DCOMPILER=MINGW64

to compile with CLANG:

    omake fullbuild -DCOMPILER=CLANG

if you don't specify a compiler it defaults to MSVC (cl.exe)

If you want to build the executable files you would cd to the src directory and type

    omake exebuild

which builds a library for each tool then links the libraries into executable files.   Then it copies all the files into the \orangec\bin directory along with various configuration files that are necessary 

To recompile orange C with the current version of itself:

    omake -DCOMPILER=OCC exebuild

note if you want to build the executables without putting them in the /orangec/bin directory you can just use omake without any switches

    omake

or if you want to build the sources with a specific compiler:

    omake -DCOMPILER=OCC

to build the sources with a specific compiler and then put the exes in the /orangec/bin directory:

    omake -DCOMPILER=OCC exebuild

If you want to build the runtime libraries you can type:

    omake librarybuild

and it will build the runtime library and move the include and library files to their proper place

again you can specify -ORANGEC_ONLY=YES to not build the DOS or OCCIL portions of the library.

If you want to clean up object files type:

    omake clean

which cleans up all the output files for the specific target (in this case msvc)

If you want to make a distribution after seperately building the EXE and LIBRARY files:

    omake distribute

which copies all files to the distribution directories and creates the distribution packages into `\orangec\dist`.  If you build instead from Visual Studio IDE or using MSBUILD you need to include /DMS:

    omake /DMS distribute

which copies the Visual Studio binaries from `\orangec\src\release` instead of the binaries that are built by the make program in each destination directory.

after distributing you can rebuild the zip file:

    omake zip

or with 7z:

    omake zip7z

which build the zip files and put them in the /orangec/dist directory

`omake` has an added function called 'tree' make.   Basically this mode is like a normal make mode, except it looks for a file called 'treetop.mak' in successively higher directories until it finds it.   Then it sets a couple of environment variables related to where it found `treetop.mak` and the current working directory, and invokes `treetop.mak`.

You invoke treetop.mak by using:

    omake /T

from either the source directory or one of the tools directories.   This means you can compile either the entire package, or you can compile each of the tools individually, depending on what path you have cd'd to.  In fact, `omake /T` is exactly what happens when you type `omake tools`.
