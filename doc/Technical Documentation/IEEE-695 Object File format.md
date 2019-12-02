# IEEE-695 Object File Format

This document documents the ieee695 format as used by LADSoft tools.

## 1.1.1. Introduction 

This documentation gives the usage of the IEEE695 object files as used by the LADSoft tools.  This is an ASCII rendition of IEEE695, but extends the functionality of the standard. 

Note that most numbers used in the file format are HEX-ASCII. But some
numbers, such as dates and the CO record identifiers, are decimal.

## 2.1.1. Brief guide to linker format 

Each record in the relocatable object file starts with a two-letter command and ends with a period. Symbols with periods in them may be embedded in the text so the location of the ending period is context-sensitive. 
Control characters such as CR/LF can be used for readability and are generally assumed by the linker to occur after each period character at the end of a record. 

Checksums are kept as running additions of the ascii value of the characters, excluding any control characters that may be embedded.



## 2.2.1 Variable types 

IEEE695 defines the following types of variables. In each case the '#' 
indicates an index that is associated with the variable. 

* `I#`:    public variable 
* `N#`:    local variable (ladsoft tools assume this is FILE global) 
* `X#`:    external variable



In addition IEEE695 defines a variable that can be used to index into 
a relocatable section: 

* `R#`:    relocatable section


The IEEE695 spec also defines: 

* `L#`:    the low limit of a section. 

This is the address at which the section is relocated to and is only valid 
if an absolute object file is output. 


* `P`:     the current value of the program counter. 

This is only valid in the LR (fixup) records.  It is derived from the low limit 
of the section and the data previously encountered in the sections `LD` and `LR` records. 



The ladsoft tools also define the following extra variable types: 

* `A#`:    an auto (stacked) variable 
* `E#`:    a variable in a register 
* `Y#`:    a segment value (e.g. for 8086 segmentation)



## 2.2.2 Expressions 

In some cases such as in fixups or `LR` Records, an expression is used to specify 
an address or offset. An expression is a comma-delimited sequence of constants, 
the operators '+','-'.'*','/', and internal variables.  The elements of an expression 
are in postfix notation, that is the expression: 

```
R23,8,+
```



indicates a location that is 8 bytes beyond the beginning of section 23. 

An expression such as: 

```
P,424,+
```

can be used to generate pc-relative offsets.

## 2.2.3 Strings 

when a string such as a name or comment text is placed into the output file, 
it is preceded by a two-digit hexadecimal number that gives the length of the 
string. This implicitly limits strings and variable names to 255 characters, 
but allows characters that would normally be record internal delimiters such as
',' and '.' to be embedded in the string. 

For example to create a public variable with the name 'apple' one would
do the following: 

```
NI3,05apple.
```




## 3.1.1 Linker record definitions


## 3.2.1 Module control


This section defines various control records used by the linker to qualify the 
file contents.


## 3.2.2 Module Begin record

The module begin record has the following format:

```
MBtranslator,filename.
```


* `translator` is a string which identifies the translator (for example
'Ladsoft C Compiler')

* `filename` is a string which gives the name of the source file.


## 3.2.3 Module End Record

The module end record has the format:

```
ME.
```


Data after this record will be discarded by all object module processing programs.


## 3.2.4 Date and Time Record

The date and time record gives the time at which the object file was generated.

It has the format:
```
DTYYYYMMDDHHMMSS.
```

Where all values are in decimal.


## 3.2.5 Architecture Definition Record

The architecture definition record gives basic information about the 
processor architecture this file is targeted for. It has the format:

```
ADbpmau,maus,endian.
```

* `bpmau` is the bits per machine architectural unit.  This is
usually 8 to denote a byte-oriented architecture.

* `maus` is the maximum number of machine architectural units the
processor deals with.  The LADSoft tools define this to be the
maximimum number of machine architectural units in an address. 
For example on the 68K this would be either 2 or 4 depending on the
processor type and compiler settings.

* `endian` gives the endianness.  It is 'L' for little endian, 'B'
for big endian.  Endianness is used for example when a tool needs
to convert an `LR` record to an `LD` record when turning the object file
into absolute format.


## 3.2.6 Checksum record. 

The checksum record gives a sanity check to indicate the file has not 
been modified up to this point.  Checksums can be placed anywhere; 
additionally a translator can choose not to generate them.  The format of a
checksum record is:

```
CSxx.
```


When a checksum record is encountered the two hex digits after it give
the sum, modulo 128, of all non-control characters after the '.' of the
last checksum record (or start of file) and up to and including the '`S`'
of the current checksum record.   (Since it ignores control characters any
formatting such as line feeds, carriage returns, or tabs are not counted)



## 3.2.7 Start Address Record

The start address record gives the start address of the program.  The linker will not allow more than one input module to have a start address; however a start address is totally optional and its presence is only useful when some type of loader is present to load the program and execute it.  It has the format:
```
ASG,expr.
```

The expression can be an absolute number for non-relocatable files, or
a standard IEEE expression for relocatable files.


## 3.3.1 Sections

This section defines the records that deal with defining sections of data.


## 3.3.2 Section Type Record

The section definition record is the initial definition of a section.  It 
gives attributes for the section, and its name.  The linker may use the 
attributes and name, with possibly some other input, to combine and relocate 
sections when generating the output.  It has the format:

```
ST#,attributes,name.
```


Attributes are separated by a comma and are as follows: 

* `'A'`: an absolute section not subject to relocation.
  * When this attribute is present an ASL record must be present in the input file telling exactly where the section is located. 

* `'B'`: a section made up of bit values
  * This attribute is reserved for processors like the C167 or 8051. 

* `'C'`: A common section. 
  * Two common sections with the same name are overlayed on each other at link time. 
  * Non-common sections normally follow one another. 

* `'E'`: an equal section
  * Two equal sections with the same name must have exactly the same contents and attributes when 'E' is present in the attribute list 

* `'M'`: maximum section
  * if there are two sections with the same name, take the largest one. 

* `'R'`: readonly (ROM) section 
* `'S'`: a non-common section
  * This is assumed without `C` or `M` or `E`
* `'U'`: unique section. 
  * No other section can have the same name as a section with this attribute

* `'W'`: readwrite (RAM) section 
* `'X'`: executable section 
* `'Z':` section to be zeroed out

Generally data will not be provided with the SB,LR,LD records and the runtime 
system will use the size of the section to initialize it. (e.g. C language 
uninitialized variables).  



In addition there are attributes which consider ordering sections with
the same
name. 

* `'N'`: 'now' order sections with this attribute first 
* `(no qualifier)`: order sections with this attribute next 
* `'P'`: 'postpone' order sections with this attribute last



* `name`
is the name of a section, and is used by the linker for combining
sections.


## 3.3.3 Section Alignment Record

The section alignment record gives the minimum alignment required by a section.  
For example 68K processors require code to be aligned on two-byte boundaries, 
and often data as well.  It has the format:

```
SA#,alignment.
```



* `#` is the index of the section as defined in the `ST` record
* `alignment` is the desired section alignment.  It must be a power of 2.

When the Section Type record is present without a Section Alignment
record, the section alignment is assumed to be 1.


## 3.3.4 Assign Symbol record

The assign size record gives the size when used with a section.  Even though 
the size is implied by the `LD` and `LR` records, this record is required to allow 
the linker to do relocations without having read in the `LD`/`LR` records.  It has the
format:

```
ASS#,size.
```

* `#` is the index of the section as defined in the `ST` record.
* `size` is the number of
mau's worth of data in the section.


## 3.3.5 Assign Location record

The assign location record gives the absolute location a section 
should be loaded at.  It would be used for example by a tool that 
converted the object file into one of the HEX file formats.  It 
must be present for if the section has the '`A`' attribute, but 
otherwise should not be present.  It has the format:

```
ASL#,location.
```



* `#` is the index of the section as defined in the `ST` record
* `location` is an address constantspecifying the location of the section.


## 3.4.1 Section data


This section defines the records that are used to populate sections with data.


## 3.4.2 Section Begin. 

The section begin record indicates the start of data for a section.  It has the following format:

```
SB#.
```


* `#` is the index of the section as defined in the `ST` record

Subsequent `LD` and `LR` records, along with debug comment records, will
refer to this section.  No load address is specified in any of the
data records; the load address is implied by the final section location
and preceding `LD` and `LR` records in this section.


## 3.4.3 Load Data record

The load data record loads pure data without modification.  It is a hex-dump 
of arbitrary length, but it is recommended that the data portion of each record 
not be longer than 64 ascii characters for readability.  It has the format:

```
LDxxxxxxxxxxxxxxxxxxxxxxxx.
```


When the `AD` record specifies `maus=8` (byte architecture) the `LD` record loads bytes of data, with each byte described by two ASCII characters.


## 3.4.4 Load Relocation record

The load relocation record indicates the target address and source size for a relocation (fixup).  
It has the format:

```
LR(expr,size).
```


The expression can be any valid expression referencing symbols or
section relative addresses, and generally calculates a relocated
address. It could also be a constant, however, normally constants are
going to be embedded directly in the LD statements.

The size gives the size in `maus` of the expression when it is resolved
to an absolute format, e.g. the number of maus of space that this `LR`
record reserves.



## 3.5.1 Variable declarations 

This section defines the records used in declaring variables.


## 3.5.2 Name record

The name record associates an internal variable index with a name.  This is 
the record that creates the variable index.  It has the format:
```
N$#,name.
```
(variable indexes will subsequently be used in `LR` records or sometimes in comment records)

* `$` is the type of variable:
  * `'A'`    auto 
  * `'E'`    register 
  * `'I'`    public 
  * `'N'`    local 
  * `'X'`    external
* `#` is the index to assign
* `name` is a string specifying the programmer's name for the variable.


## 3.5.3 Assign Symbol record

The assign symbol record assigns the location for a variable or procedure, 
for example by using a section-relative expression.  It has the format:

```
AS$#,expr.
```



* `$` is the type of variable:
  * `'I'` is a public variable 
  * `'N'` is a local variable 
* `#` is the index assigned with the Name record
* `expr` is an expression indicating the location of the variable or
procedure


## 3.5.3 Assign Type record

The assign type record assigns the type
of a variable.  It has the format:

```
AT$#,T%.
```


* `$` is the type of variable:
  * `'A'`    auto 
  * `'E'`    register 
  * `'I'`    public 
  * `'N'`    local 
  * `'X'`    external


additionally there is a command  `ATT` which is used to construct new types.  
This is further defined in the section on debug information.


* `#` is the index assigned with the Name record
* `%` is the type index.  The type index can either be a built-in type or a type assigned with the `ATT` record.  It may not however be a type constructor.


## 3.6.1 COMMENT records 

The IEEE695 standard defines comment style records with the intent of using 
them for some type of human-readable text that can be embedded in the object 
file for example to use by tools which read the object file. 

However, the LADSoft tools reserve most comment records for extensions
to the object file format.  One example use is for link pass separators.

A comment record has the following format:

```
CO#,string.
```


* `#` is the comment record type in decimal

* `string` is any textual data associated with the comment record.  This string does have a length field, and in this toolset sometimes other strings with length fields are nested within it.

The following sections outline the comment records in use.



## 3.6.2 Comment records 1-99 

A general comment inserted by the
translator and discarded by other tools. For example: `CO1,16this is a cool program.`



## 3.6.3 Pass separators

There are three link pass separators in the LADSOFT format. The given text 
for each separator can either be left blank or arbitrarily chosen; tools rely on the
index.  Following are the defined pass separators:

```
CO100,09sometext.
```
The make pass separator. Make programs will quit processing after this. (optional) 

```
CO101,09some text. 
```
The link pass separator. All declarations are
done; two pass linkers can expect only loadable data beyond this point.


```
CO102,09some text.
```
The browse info separator. Linker does not need
any info after this for relocation. (optional)

## 3.6.4 Windows DLL support 

There are two types of comment records for Windows DLL support.  These 
include the import and export symbols.

For an export symbol the record is as follows:

```
CO200,$$type,name,qualifer.
```


* `$$` is the length of the string up to the '.' 
* `type` = O or N for Ordinal or by Name 
* `name` is the name of the symbol being exported 
* `qualifier` is either the ordinal or the exported name. 

The names in this record are also strings which are further qualified
with lengths.

For an import symbol the record is as follows:

```
C0201,O,$$name,qualifier,dllname.
```


* `$$` is the length of the string up to the '.' 
* `type` = O or N for Ordinal or by Name 
* `name` is the name of the symbol being exported 
* `qualifier` is either the ordinal or the exported name. 
* `dllname` is the name of the dll to reference.

The names in this record are also strings which are further qualified
with lengths.

## 3.6.5 Source File Information Record 

The Source file information record gives information about a source or 
include file.  It can be used for example by a make program to auto-make 
a project, or by a debugger to reference line number information.  It has the format:

```
C0300,$$index,name,time.
```


* `$$` is the length of the following up to the '.' 
* `index` is the source or header file index we are assigning, 
* `name` is the name of the file, this is a string which is further qualified by a length.
* `time` is the time the file was last modified in YYYYMMDDHHMMSS format.



## 3.6.6 Debug Information records 

Comment records with index 400-405 are for debug information, and appear intermixed with 
the LD and LR records. More information is found in the debug information section.



## 3.6.7 Browse Information record

Comment record index 500 is browse information for an editor.  More information is found 
in the browse information section.


## 3.6.8 Librarian object module record

Comment record index 600 is reserved by
the librarian as a marker before each object module.  More
information is found in the librarian section.

## 4.1.1 Debug info 

For debugging, a program is assumed to have program global variables, 
file global variables, functions, blocks within functions, and local variables 
within blocks. For global variables, some basic debug information can be 
extracted e.g. from the `N$`, `ASI`, `ASN` and the `AT$#` declarations 
given above. 

In addition to declaring types and locations, the debug information has
to give locations at which local variables are 'live'. This debug
information is interspersed with the `LD` and `LR` records for a section
define the layout of local data. It is handled with COMMENT-style
records. Note that all type and debug information is not specified well
by the IEEE695 spec and this information is heavily tools specific. 


## 4.2.1 Derived Type record

The derived type record allows construction of new types such as structures and unions.  
It has the format:

```
ATT#,T%,type expression.
```


* `#`: is the type index assigned by the translator, which must be >= 1024
* `%`: some type constructor such as a structure or union designator.
  * the type expression depends on the constructor used for the type, but is typically a base type, or a list of other types.  

Type information is in the file header with other information about variables. 
It is further specified for each constructor below:


* `0`: undefined 
* `1`: pointer: `ATT$,T1,T#.` 

* `$` is the translator-defined type of the pointer.
* `#` is the base type index


* `2`: function:  `ATT$,T2,TR,#,argument list.` 
* `$` is the translator defined type of the function 
* `R` is the type index of the return value 
* `#` is a hexadecimal value giving the linkage: 
  * `0`= unadorned label (assembler) 
  * `1` = C style function call 
  * `2` = stdcall style function call
  * `3` = pascall style function call
  * `4` = C++ object method 

The argument list is a comma-delimited list of
`A#`, which gives the parameters 

the `#` gives the references to an `NA` and `ATA` records for the variable.

When an argument list is present, it will
be interpreted according to implied rules about alignment of arguments.
These rules are architecture-dependent, but for a 32-bit architecture
the data will be a multiple of four bytes. in the case of integers it
will be implicitly cast to (DWORD) and aligned as such according to the
little/big endianness of the processor as assigned in the AD record. 



* `3`: typedef: `ATT$,T3,T#.`
* `$` is the translator-assigned type 
* `#` is the base type we are equating it to 

* `4`: `ATT$,T4,list of base types.`
* `$` is the translator-defined type.

The list of base types has comma-delimited elements as follows: 
```
T#,name,offset 
```
where:
* `#` is the base type 
* `name` is the name of the field
* `offset` is the offset from the beginning of the data for the field.

The last element of a structure reference may be a reference to a
'field' record to continue the list of elements in which case it doesn't need 
a name and offset. 

* `5`: union `ATT$,T5,list of base types.`
see the definition of the structure type. offset will normally be 0. 

* `6`: array `ATT$,T6,TB,TI,base,top.`
* `$` is the translator-defined type 
* `TB` is the type of the elements in the array 
* `TI` is the type of the indexes of the array (usually T42/Int for C languages) 
* `base` is the hexadecimal rendition of the low limit to the array index 
* `top` is the hexadecimal rendition of the high limit to the array index (inclusive) 

multiple array records can be cascaded together to make a
multi-dimensional array.  In this case the first record will be
the left-hand index in the declaration (assuming C-style declarations) 

* `7`: enumeration: `ATT$,T7,list of base types.` 
see the definition of the structure type. offset will be the index of the enumerated value. 

* `8`: field list continuator: `ATT$,T8,list of base types.` 
see the definition of the structure type. this can be used at the end of a structure, 
union, or enumerated constructor to continue the sequence of members.



## 4.2.2 Base types 

A variety of base types are pre-defined for debugger interoperabilty; 
other types are built by combining the type constructors with these base types 
and with other translator-assigned types.

The type indexes for the base types are as follows: 
* `32`: void 
* `33`: pointer to void 
* `34`: boolean 
* `35`: bit 

* `40`: char 
* `41`: short 
* `42`: int 
* `43`: long 
* `44`: long long 

* `48`: unsigned char 
* `49`: unsigned short 
* `50`: unsigned int 
* `51`: unsigned long 
* `52`: unsigned long long 

* `56`: pointer to char 
* `57`: pointer to short 
* `58`: pointer to int 
* `59`: pointer to long 
* `60`: pointer to long long 

* `64`: pointer to unsigned char 
* `65`: pointer to unsigned short 
* `66`: pointer to unsigned int 
* `67`: pointer to unsigned long 
* `68`: pointer to unsigned long long 

* `72`: float 
* `73`: double 
* `74`: long double 

* `80`: float (imaginary) 
* `81`: double (imaginary) 
* `82`: long double (imaginary) 

* `88`: float (complex) 
* `89`: double (complex) 
* `90`: long double (complex) 

* `96`: pointer to float 
* `97`: pointer to double 
* `98`: pointer to long double 

* `104`: pointer to float (imaginary) 
* `105`: pointer to double (imaginary) 
* `106`: pointer to long double (imaginary) 

* `112`: pointer to float (complex)
* `113`: pointer to double (complex) 
* `114`: pointer to long double(complex) 

* `115-1023`: reserved


## 4.3.1 Local variables and other debug information

The records in this section are used to determine scope of local variables, 
and line number information


## 4.3.2 Declare variable in scope

These records declare the beginning of scope for a local variable.  They have 
the following formats:
```
CO400,$$A#.
CO400,$$E#. 
CO400,$$N#.
```

When `N#` variables are present they must have function scope, not file scope.
(e.g. a local static variable defined in a function)


## 4.3.3 Declare source file line number

This record specifies the relationship between a line of code in the
source file, and executable code in the object file.

```
CO401,$$index,line.
```



* `index` is the file index given in the CO300 record
* `line` is a line number, given in decimal.


## 4.3.4 Block declarations

The block declarations serve to locate the end of scope for variables declared in a block.

```
CO402.
```

block is beginning here. 

```
CO403.
```

 block is ending here. 

variables defined after a CO402 go out of scope when the program counter hits the matching CO403. Blocks can be nested. Hitting a CO402 is equivalent to putting a block on a stack, hitting a 403 pops the most recent CO402 and all 
variables defined since then.

## 4.3.5 Function declarations

The function declarations serve to locate the start and end of scope for 
function parameters.

For a function start:

```
CO404,I#.
```
```
CO404,N#.
``` 


For a function end.


```
CO405,I#.
```
```
CO405,N#.
``` 

The I and N variables specified must be declared with a type
derived with the function constructor type.

## 5.1.1 Browse info 

Browse info gives a cross-reference of symbols to file/line number pairs. 
It is independent of code generation. It can be generated by the translator, 
or tacked on after the fact. A separate browse file translator takes the 
information for the files in a program and generates a browse file database.  

The browse information is given as comment record type 500.  It has the following format:

```
C0500,subtype,qualifiers,file index,linenumber,name.
```


* `subtype` gives the type of browse information.
  * `0`: This is for preprocessor definitions like #defines. 
  * `1`: variable: this is some type of program variable. 
  * `2`: file start: this is a file start 
  * `3`: function start: this is a function start 
  * `4`: function end: this is the end of a function 
  * `5`: block start: this is the start of a block 
  * `6`: block end: this is the end of a block 

* `qualifiers` are used for variable and function names as follows: 
  * `0`: program global 
  * `1`: file global 
  * `2`: external 
  * `3`: local variable 
  * `4`: function prototype

* `file index` gives the index assigned by the CO300 record

* `line number` references the line within the given file and is given in decimal.


* `name` is the name of the symbol this record defines


## 6.1.1 librarian support 

The librarian uses a binary format, however, the actual data in the file is a streamed version
of the object modules which make up the library.

It is beyond the scope of this document to discuss the library format in detail.