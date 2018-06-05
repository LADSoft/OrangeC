## OCC Code Generation Parameters

### /Cparams specifies code generation parameters
 
Params is a list of parameters, seperated by + and - symbols.  Use the + symbol to indicate a parameter is to be turned on, 
the minus symbol that the parameter is to be turned off.  The default states of the various parameters are opposite what is 
shown here; i.e. this lists how to change the default state to something else.
 
     +d    display diagnostics
 
Displays memory diagnostics, and some indication of what internal errors have occurred
 
     -b    merge BSS with initialized data 
         
Normally there are two segments used, one for initialized data and one for uninitialized data.  This prevents the OS 
from having to load uninitialized data from a file; instead the program just zeroes it during startup.  This switch
merges the two sections into one initialized data section.
 
     -l    don't put C source in the ASM file
 
When an ASM file output option is specified, this will create an ASM file without cross-referencing the assembly code 
to the lines of the source file.
 
     -m    don't use leading underscores
 
Normal C language procedure is to prepend each identifier with an underscore.  If you want to use the compiler to 
generate function and variable names without an underscore use this switch.  However, doing so will create an 
incompatibility with the run time libraries and you won't be able to link.
 
     +r    reverse order of bit operations
 
Normally bit fields are allocated from least significant bit to most significant bit of whatever size variable is being 
used.  Use this switch to reverse the order, however this may create incompatibilities with the libraries which result 
in code bugs.
 
     +s    align stack
 
This switch causes OCC to emit code to align the stack to 16-byte boundaries.  This is useful to speed up operations 
that involve loading and storing double-precision floating point values to auto variables.  By default, the run-time 
libraries cause main() or WinMain() to execute with an aligned stack.
 
     +F    use FLAT model in ASM file
 
When using ASM file, select FLAT model.
 
     +I    use Microsoft-style imports
 
Normally the linker creates a thunk table with jump addresses that jump indirectly through the import table.  This 
allows basic C code to compile and link.  However some linkers do not support this and instead need the compiler to
call indirectly through the import table rather than to a thunk table.  Use this switch to generate code compatible 
with these linkers.
 
     +M    generate MASM assembler file
 
     +N    generate NASM assembler file for OMF
 
     +NX    generate generic NASM assembler file
 
     +R    use the far keyword to create far pointers or far procedure frames
 
     +S    add stack checking code
         
This switch adds calls to the run-time library's stack checking code.  If the stack overruns, an error is generated.
 
     +T    generate TASM assembler file
 
     +U    do not assume DS == SS
 
     +Z    add profiler calls
     
This switch adds calls to a profiler module at the beginning and ending of each compiled function.  This is DOS 
compatibility; the WIN32 profiler module does not exist at present.
 
for example:

    OCC /C+NX+Z myfile.c

generates generic NASM assembler module, with profiler calls inserted.