## OLink Specification Files

 
 A specification file indicates the combination and ordering of sections of code and data.  The specification file consists of one or more **Partitions**, which are independent units of code and/or data.  
 
 Each partition holds one or more **Overlays**.  Overlays are units of code and/or data which may share the same location in memory; for example a processor with a small memory address can use bank switching to move different units of code in and out of the address range as necessary.  These different units of code would however all share the same address, so the overlay mechanism gives a way of relocating multiple units of code to the same address.
 
 Each overlay holds one or more **Regions**; a region is what specifies which sections get combined.  For example the region **code** takes all sections named **code** from the input file and concatenates them together.  Multiple regions can be concatenated one after another within an overlay.  To support the overlay mechanism, each region may further be qualified with file names, so that you can specify that sections named **code** from one group of files go in one region, and sections named **code** from another group of files go in another region.  These regions could be placed in different overlays to help with things like the bank-switch mechanism indicated above.
 
 Partitions, overlays, and regions can be given attributes to specify things like an absolute address, a maximum size, alignment, and so forth.
 
 You can also define labels within partitions and overlays similar to assignment statements in a high-level-language.  Each label has an associated expression, which is calculated and then used as a value for the label.  These values become globals to the linker, and are treated the same as the address obtained when declaring a global variable in the assembler and compiler.  Code in the object files can reference these labels as if they were externals.  
 
 Further, expressions used in defining a label or attribute could use another label which is not defined in the specification file; this might be defined for example somewhere in the code, or in a [command-line definition](OLink%20Command%20Line%20Options.md).  It is especially useful to define such labels in a command line definition, as a way to customize the specification file without rewriting it.  For example, if two peices of hardware share the same source code but are linked at different base addresses, one might write a single linker specification file, referencing the base address as a label.  Then a linker command-line definition could be used to resolve the specific address the code is linked for.
 
 The following will be used as an example:
 
 
>     partition
>     {
>         overlay {
>            region {} data [align=4];
>            region {} bss.
>         } RAM [addr=0x0000, size=0x4000];
>     } DATA;
 
>     partition
>     {
>         overlay {
>            region {} code;
>            region {} const;
>         } ROM;
>     } CODE [addr=0xf000, size=0x1000];
 
This defines two partitions, in this case one is for data nad one is for code.  The first partition is named DATA and consists of two groups of sections.   First all sections named **data** are concatenated together, then all sections named **bss** follow after that.  This partition is defined with attributes to start address 0, and extend for 16K.  If the actual size of the partition is greater than 16K, an error will be generated.  In this case the overlay is named RAM; this overlay name is what is visible to ROM-generation tools such as **DLHEX**.
 
 The second partition is named CODE and also consists of two groups of sections; first all sections named **code** are concatenated together, followed by all sections named **const**.  This partition starts at address 0xf000, and extends for 4K.  In this case the overlay name visible to **DLHEX** or other executable-generation tools is ROM.
 
 
 In the first partition, an **align=4** attribute is declared on the **data** region.  This means that each data section put into the region will be aligned on a four-byte boundary when it is loaded from its corresponding file.  (Note:  if assembly language code specifies a more stringent bound such as align = 8, that will be used instead).
 
 In the following:
 
>     partition
>     {
>        overlay {
>            region { bank1a.o bank1b.o bank1c.o } code;
>        } BANK1;
>        overlay {
>            region { bank2a.o bank2b.o bank2c.o } code;
>        } BANK2;
>        overlay {
>            region { bank3a.o bank3b.o bank3c.o } code;
>        } BANK3;
>     } CODE [addr = 0xe000, size = 0x1000];
 
 Three banks of code have been defined, each of which starts at address 0xe000 and extends for 4K.  Each region references sections named **code**, however, file names are specifically specified for each region, so that only **code** sections from specific files will be included while processing the region.  For example in the overlay BANK1, only files **bank1a.o**, **bank1b.o**, and **bank1c.o** will contributed to the contents of the region.
 
 Wildcards may be used in the file specification, so that the above could be written:
 
>     partition
>     {
>         overlay {
>            region { bank1*.o } code;
>        } BANK1;
>        overlay {
>            region { bank2*.o } code;
>        } BANK2;
>         overlay {
>            region { bank3*.o } code;
>        } BANK3;
>     } CODE [addr = 0xe000, size = 0x1000];
 
 
 
 In the following:
 
>     partition
>     {
>         overlay {
>            RAMSTART=$
>            region {} data [align=4];
>            region {} bss;
>            RAMEND=$
>         } RAM [addr=0x0000, size=0x4000];
>     } DATA;
 
 The labels RAMSTART and RAMEND have been defined.  The '$' in the expression indicates to use the address at the location the label is specified, so these definitions effectively define labels at the beginning and ending of the overlay.  As indicated before these define global variables, so an x86 assembler program such as the following could be used to set all data in these regions to zero:
 
>     extern RAMSTART, RAMEND
>         mov edi, RAMSTART
>         mov ecx,RAMEND-RAMSTART
>         mov al, 0
>         cld
>         rep stosb
 
 
 Expressions may be more complex, consisting of add, subtract, multiply, divide and parenthesis.  As a simple example the above example can be rewritten to define a size:
 
>     partition
>     {
>         overlay {
>            RAMSTART=$
>            region {} data [align=4];
>            region {} bss;
>            RAMSIZE = $-RAMSTART
>         } RAM [addr=0x0000, size=0x4000];
>     } DATA;
 
 
 
 Labels or expressions may be used in attributes, for example:
 
>     partition
>     {
>         overlay {
>            RAMSTART=$
>            region {} data [align=4];
>            region {} bss.
>            RAMSIZE = $-RAMSTART
>         } RAM [addr=RAMBASE, size=0x4000];
>     } DATA;
 
 Here the base address is defined in terms of a label RAMBASE.  But RAMBASE is not defined anywhere in the specification file, so it has to be pulled from the linker's table of globals.  In this case we might define it on the linker command line as follows:
 
>     OLink /DRAMBASE=0x7000 /smyspec.spc ...
 
 
 Labels don't have to include '$' in the expression, although it is often useful.  For example:
 
>     MYLABEL=0x44000+2000
 
 is valid.
 
 Note that when using target configurations, the default specification files use these types of declarations, but the target configuration gives default values to use.  For example the default value for RAMBASE in a hex file is 0x10000, when used with the default linker specification file that is used for binary and hex file output.  But such values can be overridden on the command line; if it is desirable to use the default specification file but RAMBASE is 0x8000 for the specific hardware in question one might use OLink as follows:
 
>     OLink /T:M3 /DRAMBASE=0x8000 ...

### Attributes

 Partitions, overlays, and regions can be attributed with one or more attributes.  The attributes are comma delimited, and enclosed in braces.  They occur after the name of the partition or overlay, or after the section specified by a region.
 
 The attributes are as follows:
 
    

|Attribute|Meaning|Default Value for Partitions|Default Value for Overlays|Default Value for Regions|
|--- |--- |--- |--- |--- |
|ADDR|Address|0, or end of previous partition|partition address|overlay address or end of previous region|
|SIZE|Absolute size|unassigned|partition size|unassigned|
|MAXSIZE|absolute size may vary up to this amount|unassigned|partition maxsize|unassigned|
|ROUNDSIZE|absolute size may vary, but will be rounded up to the next multiple of this|1|partition roundsize|unassigned|
|FILL|fill value used when absolute size does not match size of data included in region|0|partition fill|overlay fill|
|ALIGN|minimum alignment of region|1|partition alignment|overlay alignment|
|VIRTUAL|base address for linking the region, when base address does not match the  ADDR attribute|unassigned|partition virtual attribute|virtual address of overlay, or end of previous region|


### Complex region specifiers

 Usually the region statement is used to specify a specific section name such as code:
 
>     region { } code;
 
 But sometimes it is useful to be able to combine multiple sections in a single region with the **or** operator
 
>     region {} code | const;
 
 However this is different making two regions, one for **code** and one for **const**.  The difference is that in this case code and const regions may be intermixed; whereas in the other case all the code sections would be combined together, separately from all the const regions.
 
 
 Wildcards may be used in the region name:
 
>     region {} code*
 
 matches the sections name code1, code2, code123, and so forth.
 
 And for example
 
>     region {} *
 
 matches ALL sections.  There are two wildcard characters:  **\*** matches a sequence of characters, whereas **?** matches a single character.
 
 Other times you want to do a catch all which gets all sections except for a select section or group of sections.
 
>     region {} * & !(code*)
 
 This uses the **and** operator and the **not** operator to select all sections which do not start with the four letters 'code'.
 
 A region can be named with any potentially complex expression involving section names and these operators, to match various combinations of sections.