##OASM Directives

 
 Directives are used to indicate the assembler should interpret the statement as something other than an instruction to the processor.  For example they can be used to define data, or to create seperate sections for grouping code and/or data.  
 
 Natively, directives are always enclosed in brackets, for example to define a byte of data:
 
> myvar \[db   44\]
 
 However, the directives are redefined with default [multiline macros](Multiline%20Macro%20Extensions.html), so that the brackets are not necessary when typing code:
 
> myvar db 44
 
 This documentation describes the macro version of the directives.
 
 Some of the macro redefinitions of the directives are simply a mapping from the non-bracket version to the bracketized version.  Other macros are more complex, adding behavior.  For example the macros for psuedo-structures define the structure size, and keep track of the current section and switch back to it when the end of structure macro is encountered.
 
 There are several types of directives.  
 
* Data Definition directives define the value of a variable, either in terms of a numeric value or as a label (address) defined elsewhere in the assembler.  
   
* Data Reservation directives reserve space for data, and optionally initialize that space with default values.   
   
* Label Directives give some attribute to a label, such as defining how it will be viewed from outside the module.  
   
* Section Directives group related code or data in such a way that it can be combined with other modules.  
   
* The EQU directive allows definition of a label in terms of a constant value.  
   
* The INCBIN directive allows the possibility of directly importing binary data into the current section.  
   
* The TIMES directive allows a very simple form of repetitive code generation.  
   
* Psuedo-structure directives allow you to define structured data in a very basic way.


###Data Definition Directives

 Data Definition directives define the value of a variable.  There are several types of values that can be defined.  The first is a number.  The number can sometimes be an integer, other times floating point, depending on the directive.  Another type is a character or string.  Another type of value is use of a label, or the difference between labels.  Another type sometimes useful in real mode programming is a segment, which is very loosely a reference to a section.  The reference can be made either by saying the section name, or using the SEG operator on a label to extract its section.
 
 There are five Data Definition Directives:
* db - define byte
   
* dw - define word
   
* dd - define dword
   
* dq - define quadword
   
* dt - define tbyte
  
 Table 1 cross-references the various directives along with the data types can be defined with each.
 
     

|colheader |colheader |colheader |colheader |colheader |colheader |
|--- |--- |--- |--- |--- |--- |
||integer|floating point|character/string|label|segment|
|db|yes|no|yes|no|no|
|dw|yes|no|yes|yes|yes|
|dd|yes|yes|yes|yes|no|
|dq|no|yes|no|no|no|
|dt|no|yes|no|no|no|   

  
  Table 1 - directives and data types
  
 In 16-bit mode, it makes sense to use dw with labels, whereas in 32-bit mode, it makes sense to use dd with labels.
 
 Some examples follow:
 
> mylab:
     db 44
     dw   0234h
     dd  9999
     dd  43.72
     dd   mylab
>>     dq   19.21e17
>     dt  0.001 ;
 
 Multiple values may be specified per line for these directives; for example:
 
> mylab:
>     db   "hello world",13, 10, 0
>     dq   44.7,33,2.19.8


###Data Reservation Directives

 Data Reservation directives reserve space for data, and optionally give an initial value to load the space with.  For example:
 
>     resb   64
 
 reserves space for 64 bytes of data.  This data will default to zeros in the section it is defined it.  However, an alternative form of the directive specifies an initial value.  Thus:
 
>     resb   64,'a'
 
 fills the space with lower case 'a' values.
 
 There are five data reservation directives:
 
* resb - reserve bytes
   
* resw - reserve words
   
* resd - reserve dwords
   
* resq - reserve quadwords
   
* rest - reserve tbytes
  As an example:
 
> mylab   db   "hello world"
>         resb mylab + 80 - $, '.'
 
 defines the string "hello world", then adds enough dots on the end to make up an 80-character buffer.
 
 Generally, the type of data that can be defined in the optional argument to one of the Data Reservation directives is the same as for the corresponding Data Definition directive.


###Label Directives

 Label Directives bestow some type of attribute to a label.  Generally these attributes center around visibility of the label - is it visible to some entity outside the current OAsm assembly session or not?  For example the 'global' and 'extern' directives bestow attributes that allow the linker to resolve references to a label when the references are made in one source code file but the label is defined in a different source code file.  Some of these directives require the label to be defined in the same file as the directive occurs; and others require the label to be defined elsewhere.
 
 The Label Directives are as follows:
 
* global - the label is defined in this file, but the linker may use it to resolve references in other files
   
* extern - the label is not defined in this file, the linker should find it elsewhere
   
* export - the label is defined in this file, and will become a DLL or EXE export that Windows can use to resolve against other executables
   
* import - the label is not defined in this file, it will be imported from some other DLL or EXE file by windows


###Global Directive

 Each use of the global directive can assign the 'global' attribute to one or more labels.  When a label is global, it has a presence that extends beyond the current file, and the linker may resolve references to the variable to it, when those references are made in other files.  Those references would typically have been made by use of the 'extern' directive.  For example:
 
>     global puterror
>    
> puterror:
>     mov   eax,errmsg
>     call strput
>     ret
> errmsg   db   "this is an error",0
 
> strput:
>     ....
 
 creates a function 'puterror' which is visible to other files during linkage.  Global may be used with multiple labels:
 
>     global mylab, strput


###Extern Directive

 Each use of the extern directive can assign the 'external' attribute to one or more labels.  When a label is external, the definition is not found in the file currently being assembled.  The purpose of the directive is to give the assembler and linker a hint that it should search elsewhere for the definition.
 
 In the above example, if 'strput' was defined in a different file from the definition of puterror you might write the following:
 
>     global puterror
>     extern strput
   
> puterror:
>     mov   eax,errmsg
>     call strput
>     ret
> errmsg   db   "this is an error",0
 
 As with the global directive, extern can be used with multiple symbols:
 
>     global puterror
>     extern strput, numberput
   
> puterror:
>     push eax
     mov   eax,errmsg
>     call strput
     pop eax
     call numberput
>     ret
> errmsg   db   "this is error number: ",0


###Export Directive

 The export directive defines a symbol that can be used by the windows operating system during program load.  For example a DLL might use it to declare a function that is to be available to other executable files.  Unlike the global and external directives, 'export' can only be used to change the attributes of one variable at a time.  For example in the above examples adding the line:
 
>     export puterror
 
 would create an export symbol named 'puterror' which windows could then resolve to an import reference in another executable file at load time.  Another form of the export directive can be used to change the visible name of the exported function:
 
>     export puterror Error
 
 would export the function puterror, but other executables would see it as being named Error.


###Import Directive
 

 The import directive is used to signify that the label is exported from some other executable or DLL file, and that windows should load that executable or DLL so that the label can be resolved.  As with export there are two versions of the directive:
 
>     import   ExitProcess Kernel32.dll
 
>     ...
     push    0
>     call    ExitProcess
>     ...
 
 indicates that the DLL kernel32.dll should be loaded so that a reference to the ExitProcess API call can be resolved.  It might be useful to rename ExitProcess to Quit if that is easier to remember and type:
 
>     import ExitProcess Kernel32.dll Quit
 
>     ...
     push   0
>     call   Quit
>     ...


###Section Directives

 Section directives help arrange data and code.  In 16-bit code they are often essential, as a section in **OAsm** maps more or less directly to a segment in the real-mode architecture (depending on the exact definitions given in the linker specification file); and large programs simply won't fit within a single section.  The section directive would then be used to partition the various code and data into segments some way that makes sense based on the application.
 
 While sections aren't quite as necessary in 32-bit code, it is still customary to partition code into various sections to make it more manageable.  In general, the linker will combine the code and data that is generated from assembling multiple source files so that similar types of code or data will appear close together in the output file.  This is usually done based on section NAME, for example a section named CODE in one object file will be combined with sections named CODE in other object files, and similarly sections named DATA will be combined together, before the CODE and DATA sections are themselves combined together to make a part of the executable.  Section names are generally arbitrary, however, there are some conventions made specifically in the linker specifications for windows programs.
 
 Another use for section directives is to define absolute addresses.  This is similar to using EQU to give a value to a label, but is more generic and allows use of Data Reservation directives so the assembler can calculate offsets based on the amount of data reserved.  This use of section directives helps define the psuedo-struct mechanism.
 
 The three section directives are:
 
* section - define a names section
* absolute - start an absolute section
* align - align code or data to a specific boundary


###Section Directive

 The Section directive switches to a new section.  If this is the first time the section is used, the section directive may also specify attributes for the section.  For example:
 
> section code
 
 switches to a section named code.  Various attributes may be specified, such as section alignment and word size of section.  Some other attributes are parsed for compatibility with x86 assemblers, but are not currently used.  The attributes are:
 
* ALIGN=xxx - set alignment of section within EXE file
     
* CLASS=xxx - set class name.  This attribute is ignored by this assembler
     
* STACK=xxx - this section is a stack section.  This attribute is ignored by this assembler
     
* USE16 - this section uses 16-bit addressing modes and data
     
* USE32 - this section uses 32-bit addressing modes and data
  As an example of a simple 32-bit program
 
> section code ALIGN=2 USE32
> extern Print
> ..start:
>     mov eax,helloWorld
>     call Print
>     ret
 
> section data ALIGN=4 USE32
> helloWorld   db   "Hello World",0
 
 Note that for convenience, 'segment' is defined as an alias for 'section'.  So you could write:
 
> segment code USE32
 
 to start a section named code, if you prefer.


###Absolute Directive

 The absolute directive is used to switch out of sections where data can be emitted, into an **absolute** section with the specified origin.  It is called absolute because these labels never get relocated by the linker; they are essentially constants.  Labels defined in an absolute section will get the value of the program counter within the section, at the time the section is defined.  But these labels get a constant value, that is not relocated by the linker.  For example:
 
> absolute 0
 
> lbl1:
>     resb   4
> lbl2:
>     resw 3
> lbl3:
 
 creates an absolute section based at absolute zero, and defines three labels.  These labels will have the following values based on the space that has been reserved:
 
> lbl1:   0
> lbl2:   4 \* 1 = 4
> lbl3:   4 + 3 \* 2 = 10
 
 Note that in the definition of this section, we did not attempt to create data or code, we only reserved space.  In general attempting to generate code or data in an absolute section will cause an error.


###Align Directive
 

  The Align directive aligns data to a specific boundary.  For example:
 
> align 4
 
 inserts enough zeroed bytes of data to align the current section to the beginning of a four-byte boundary.  Note that the section attributes still need to be set to have the same alignment or better, either in the section directive or in the linker specification file, so that the linker will honor the alignment when relocating the section.


###The EQU Directive

 The EQU directive allows a label to be given some value.  This value must be calculable at the time the EQU directive is encountered and must resolve to a constant.  However, the value itself can involve the differences between relative constructs, e.g. the difference between the current program counter and a label defined earlier in the section is a valid expression for EQU.
 
 When the value is a difference between relative constructs, care must be taken that the branch optimization does not change the value after it has been calculated.  For more information, see the section on expressions.
 
 For example:
 
> four     EQU 4         ; value of the label 'four' is 4
> mylab    resb   64
> size     EQU $-mylab   ; value of the label 'size' is 64


###The TIMES Directive

 The Times directive is a primitive form of repetitive programming.  It takes as operands an instruction or directive, and a count of the number of times to repeat the instruction or directive.  As such its functionality can often be performed more efficiently with a Data Reservation directive.  It is also much more limited than the %rep group of preprocessor directives.  Times is available primarily for nasm compatibility.
 
 For example the earlier example from the Data Reservation section could be alternatively written:
 
 
> mylab   db   "hello world"
>         times mylab + 80 - $ \[db '.'\]
 
 Here the native form of the db directive is used, since macro substitution is not available in this context.  Times could also be used for timing:
 
>     times 4 NOP
 
 another use for times sometimes found in NASM programs is to align data:
 
>     times ($$-$)%4 \[db 0\]


###The INCBIN Directive
 

 The Incbin directive allows the import of a binary file into the current section.  For example it could be used to copy a graphics resource such as a bitmap or font verbatim into the current section.  Other uses include things like importing help text from a text file, or importing a table such as a CRC table that has been pre-generated by some other program.
 
 The basic form of incbin is:
 
> incbin "filename"
 
 where filename is the name of the file to import.  In this form, all data from the beginning to the end of the file will be imported.  Another form starts importing at a specific offset and goes to the end:
 
> incbin "Filename", 100
 
 starts importing at the 100th byte in the file.  Still another form lets you specify the length of data to import:
 
> incbin "Filename", 96, 16
 
 imports 16 bytes, starting at offset 96 within the file.


###Psuedo-Structures
 

 Structures aren't really a construct supported by the assembler, however, clever macro definitions allow definition of structure-like entities.  For example, consider the following:
 
> struc   astruc
> s1   resb 4
> s2   resw 3
> s3   resd 5
> s4   resb 1
> endstruc
 
 This defines the following label values similar to how they were defined in an absolute section:
 
> s1: 0
> s2: 4 \* 1 = 4
> s3: 4 + 3 \* 2 = 10
> s4: 10 + 5 \* 4 = 30
 
 The structure mechanism also defines astruc as 0, and it defines the size of the struct 'astruc\_size' as 30 + 1 = 31.
 
 To access the member of a structure one might use something like:
 
> mov   eax,\[ebx + s3\]
 
 The structure mechanism could just as well be done with absolute sections or EQU statements (except that a side effect of the structure mechanism is to define a size).  But a little more more interestingly, if you introduce local labels and remember that a local label can be accessed from anywhere if its fully qualified name is specified you might write:
 
> struct astruc
> .s1   resb 4
> .s2   resw 3
> .s3   resd 5
> .s4   resb 1
> endstruc
 
 This lets you qualify the name and use:
 
> mov    eax,\[ebx + astruc.s3\]
 
 However you need to be careful with this.  Structures aren't really part of the assembler, but are instead an extension provided by built-in macros.  So you can't make an instance of a structure and then use a period to qualify the instance name with a structure member.  For example:
 
> mystruc   resb   astruc\_size,0
> mov       eax,\[mystruc.s3\]
 
 is not valid, because the label 'mystruc.s3' does not exist.  The move would have to be changed to something like:
 
> mov eax,\[mystruc + astruc.s3\]
 
 
 