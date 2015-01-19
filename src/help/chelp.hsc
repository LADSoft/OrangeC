HelpScribble project file.
16
YNQ`bsg-113363
0
2
C language help


LADSoft
FALSE


1
BrowseButtons()
0
FALSE

FALSE
TRUE
16777215
0
32768
32896
255
TRUE
TRUE
TRUE
FALSE
150
50
600
500
TRUE
FALSE
1
FALSE
FALSE
Contents
%s Contents
Index
%s Index
Previous
Next
FALSE

49
1
Scribble1
Introduction
introduction; index; welcome



Writing



FALSE
16
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\lang1033\b\fs32 Introduction\f1 
\par 
\par \cf0\b0\f0\fs20 The compiler is responsible for processing a textual representation of algorithms and data, and changing it into a form that can be processed by the computer.  In the process, other tools such as linkers and librarians are used to manage large code bases and apply specific transformations required for an operating system to load the program.  This documentation will primarily focus on aspects of the C language, and will generally avoid discussion of such other tools.  Requirements imposed by operating systems will also be outside the scope, as will various extensions to the language which are available in this compiler.  This document goes to some effort to distinguish the difference between the 1999 version of the standard and previous versions.  Finally, this documentation is not meant to accurately reflect the terminology in the standards.  Certain leeway has been taken for explanatory purposes.
\par 
\par Before code is compiled, it is preprocessed.  The \cf2\strike preprocessor\cf3\strike0\{linkID=20\}\cf0  strips \cf2\strike comments\cf3\strike0\{linkID=140\}\cf0 , determines which code is to be \cf2\strike conditionally compiled\cf3\strike0\{linkID=100\}\cf0  and which code is to be ignored, and allows for \cf2\strike\f1 macro expansion\cf3\strike0\{linkID=90\}\cf0\f0 .  There are also compiler-specific instructions called \cf2\strike pragmas\cf3\strike0\{linkID=110\}\cf0  which are handled by the preprocessor, as well as other miscellaneous source-code control functions.
\par 
\par At the outermost level, the compiler processes \cf2\strike declarations\cf3\strike0\{linkID=30\}\cf0 .  A declaration declares some type of data.  This can be a \cf2\strike variable\cf3\strike0\{linkID=40\}\cf0  to store values in, a definition of an abstract \cf2\strike type\cf3\strike0\{linkID=180\}\cf0 , or a \cf2\strike function\cf3\strike0\{linkID=60\}\cf0  declaration.  
\par 
\par Simple abstract types can be used to rename basic types.  More complex abstract types can define pointers or arrays, or gather simpler types together in a structure or union.  In general, an abstract type is always based on some combination of built-in types and other abstract types.
\par 
\par A function can be further broken down into \cf2\strike statements\cf3\strike0\{linkID=70\}\cf0 , which are a sequential listing of actions to take, and \cf2\strike expressions\cf3\strike0\{linkID=80\}\cf0 , which evaluate some mathematical quantity and optionally assign the result into a variable.
\par 
\par \cf1\b\f1 
\par }
20
Scribble20
Preprocessor
preprocessor



Writing



FALSE
14
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Preprocessor\cf0\b0\fs24 
\par 
\par \fs20 The preprocessor controls the flow of source code into the compiler.  It starts by stripping \cf2\strike comments\cf3\strike0\{linkID=140\}\cf0  out of the code, replacing \cf2\strike trigraphs\cf3\strike0\{linkID=155\}\cf0  with their equivalent, and \cf2\strike concatenating lines\cf3\strike0\{linkID=150\}\cf0  as necessary.  As it executes it evaluates various preprocessor directives, and performs \cf2\strike macro expansion\cf3\strike0\{linkID=90\}\cf0 .  Macro expansion is a primary use for the preprocessor.  It allows a textual substitution of one sequence of characters with another, and in this respect is similar to an editor search-and-replace function.  The textual substitution is however somewhat more powerful, allowing definition of macros as pseudo-functions with arguments.
\par 
\par Preprocessor directives occur when the first non-space character on a line is '#'.  The directive extends to the end of the current line, unless \cf2\strike line concatenation\cf3\strike0\{linkID=150\}\cf0  is used.
\par 
\par The most often-used preprocessor directives are the one used for \cf2\strike including\cf3\strike0\{linkID=130\}\cf0  the contents of another source or header file, and the ones used for macro expansion.  
\par 
\par Another often-used set of preprocessor directives involves \cf2\strike conditional compilation\cf3\strike0\{linkID=100\}\cf0 .  Conditional statements evaluated by the preprocessor are used to programmatically determine what part of the source code the compiler sees, and what part it ignores.
\par 
\par There are a couple of preprocessor directives which control part of the text of the \cf2\strike diagnostic messages\cf3\strike0\{linkID=120\}\cf0  issued by the compiler.  Finally, there is a preprocessor \cf2\strike pragma\cf3\strike0\{linkID=110\}\cf0  directive, which is used to relay compiler or platform-specific information to the compiler.\fs24 
\par }
30
Scribble30
Declarations
declarations; scope; linkage; extern; static



Writing



FALSE
32
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil\fcharset0 Courier New;}{\f2\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Declarations\cf0\b0\fs24 
\par 
\par \fs20 The compiler starts by looking for declarations.  A declaration creates some type of data, such as a \cf2\strike variable\cf3\strike0\{linkID=40\}\cf0  or \cf2\strike function\cf3\strike0\{linkID=60\}\cf0 .  It can also be used to create an abstract \cf2\strike type\cf3\strike0\{linkID=180\}\cf0 , which is itself later used to create variables, data, or other types.
\par 
\par Declarations that are not types have a scope and a linkage.  The scope of a declaration can be file, parameter, block scoped, or member.  The first declaration read by the compiler is always file scoped.  The linkage can be static, global, auto, register, extern, or member.   An addition found in the 1999 standard is an inline linkage that may be used with function declarations.
\par 
\par A file-scoped declaration can be used anywhere in the part of the file after the declaration is encountered, or in subsequently included files.  A parameter declaration is one of the listed parameters in a function.  A block-scoped declaration is somewhere within a function body.  A member declaration is a member of a structure or union type.
\par 
\par The linkage of a declaration determines some aspects of the storage for the declaration.  Static linkage persists, but is not visible outside the file or function it is declared in.  Global linkage is visible in other modules.  External linkage means no space is allocated; this is a directive to the linker that the space allocation will be found in another file.  Auto linkage is only found in function bodies; it means the declaration is allocated in non-persistent storage, each time the function is called.  This allows for recursive functions.  Register linkage is like auto linkage except it serves as a hint to the compiler that the value should be stored in a register.  Most modern compilers will silently ignore register linkage and calculate an optimal use for registers. Member linkage means the declaration exists within the scope of a structure or union definition.  A record of the amount of space is required but no actual space will be allocated until the type is used to create a variable.
\par 
\par As an example:
\par 
\par \f1 extern int moduleScopeExternalLinkage;
\par static int moduleScopeStaticLinkage;
\par 
\par int moduleScopeGlobalLinkage;
\par 
\par int moduleScopeExternalLinkedFunction();
\par 
\par struct _mystructTag \{
\par \tab int memberScopeMemberLinkage;
\par \};
\par int moduleScopeGlobalLinkedFunction(int parameterScopeAutoLinkage)
\par \{
\par \tab static int blockScopedStaticLinkage;
\par \tab int blockScopedAutoLinkage;
\par \}
\par 
\par \f2 In C, declarations inside a function body\f0  were originally restricted to\f2  the beginning of a block.  However, the 1999 version of the standard relaxed this restriction and allowed declarations anywhere within a block.
\par }
40
Scribble40
Variables
variables; extern; static; auto; register



Writing



FALSE
40
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Variables\cf0\b0\fs24 
\par 
\par \fs20 Variable declarations define some type of data.  The data may consist of some numeric value, a character, a string, an enumeration, or an entity of some aggregate type such as a structure or union.
\par 
\par The variable declaration has three parts: a base \cf2\strike type\cf3\strike0\{linkID=50\}\cf0 , qualifiers, and a variable name.  Optionally, the variable can be assigned an \cf2\strike initial value\cf3\strike0\{linkID=45\}\cf0  as part of the declaration. 
\par 
\par The base type must be specified within a function body, but may be omitted at file scope.  When this is done, the base type defaults to 'int'.  Note that the 1999 standard disallows this defaulting of the base type to int.  
\par 
\par Qualifiers are optional and specify things like whether the variable is a pointer or array.  The variable name is used throughout the code as a reference to the allocated storage.
\par 
\par If a variable is not declared with an initial value, the initial value depends on the scope of the variable.  File scoped uninitialized variables are always initialized to zero before the program runs.  Block scoped uninitialized variables are considered to hold an undefined value.  Parameter scoped variables are generally initialized by the function call and may only become uninitialized in the absence of prototypes.  Member scoped variables are simply type declarations, and have no value until the enclosing structure or union type is used to create a variable.
\par 
\par A simple declaration is as follows:
\par 
\par \f1 int myInteger;
\par 
\par \f0 This declaration has different scope depending on where it is found.  For example if it is at file scope, this is a global declaration, visible from other modules.  If it is inside a function, its scope is the remainder of the block which it is found in.  If it is declared as part of a structure, its scope is the same as the instance of the structure it is referenced from.
\par 
\par Some scopes may be explicitly specified.  For example:
\par 
\par \f1 static int myPersistentStaticInteger;
\par extern int myExternalInteger;
\par auto int myAutoInteger;
\par register int myRegisterInteger;
\par 
\par \f0 The first is persistent storage.  It has static linkage, meaning it cannot be seen outside the function or file it is declared in.
\par The second is a reference to a variable found in another module.
\par The third is an explicit way of declaring variables within a function.  Normally the 'auto' keyword is omitted, and in the 1999 version of the standard its use is deprecated.
\par The fourth is a way of hinting that the variable should go in a register.  It may only be used within a function.\f1 
\par 
\par \f0 The above definitions simply define storage for an integral variable of standard size.  Several values may be declared on the same line:
\par 
\par \f1 int myInt1, myInt2, myInt3;
\par \f0 
\par When declaring several variables on the same line, type modifiers may be specified with each variable.  For example with the following there is one basic type but each declared variable has different modifiers, making the variables actually have different types.
\par 
\par \f1 int myInt, *myIntPointer, myIntArray[10], volatile tickCount;\f0 
\par }
45
Scribble45
Initial Value
initialization; initial value



Writing



FALSE
107
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Initial Value\cf0\b0\f1\fs20 
\par 
\par \f0 When a variable is declared at file or block scope, it may be given an initial value.  For example:
\par 
\par int treeCount = 5;
\par 
\par declares the variable treeCount and gives it an initial value of 5.   If this variable is an automatic variable, the value is reinitialized every time the statement is encountered, e.g. each time through a looping \cf2\strike control statement\cf3\strike0\{linkID=210\}\cf0  in which it is \cf2\strike declared\cf3\strike0\{linkID=40\}\cf0  and initialized.   If it isn't an automatic variable, the variable is preinitialized exactly once, before the program starts. 
\par 
\par Note that any uninitialized automatic variable has an undefined value until something is assigned to it.  However, uninitialized variables that are not auto variables are automatically preinitialized to zero, exactly once, before the program starts.
\par 
\par Initial values for automatic variables may consist of any expression of compatible type.  In that sense an initializer for an automatic variable is similar to the combination of an assignment statement and a declaration.  However initializers for file scoped variables variables must be \cf2\strike constant values\cf3\strike0\{linkID=190\}\cf0 , or \cf2\strike expressions\cf3\strike0\{linkID=80\}\cf0  that may be evaluated to a constant value at compile time.
\par 
\par Const-qualified integers are treated specially by the compiler:
\par 
\par \f2 int const NumberOfStations = 10;
\par 
\par \f0 is not assigned any storage, but instead the value ten is used wherever NumberOfStations is referenced.  This is similar to macro substitution of constants, but has the advantage that the compiler knows the type of the constant.
\par 
\par Pointers may be initialized at either file or block scope:
\par 
\par \f2 int myInteger;
\par int *p = &myInteger;
\par 
\par \f0 Arrays may be initialized as long as they are not automatic variables.  Note: in the 1999 standard automatic variables of type array may be initialized via \cf2\strike type casting\cf3\strike0\{linkID=330\}\cf0 .\f2 
\par \f0 
\par \f2 int myArray[5] = \{ 1, 2, 3, 4, 5 \};
\par 
\par \f1 In this case the outer set of braces is mandatory, however, if an array of multiple dimensions is initialized internal braces may optionally be omitted.\f0   This is generally not a good idea, because the array initializer does not need to completely define the array contents.  Values between the last content and the end of the array are initialized to zero.  For example these declare the same data:
\par 
\par \f2 int myArray[5] = \{ 1, 2 \};
\par int myArray2[5] = \{ 1, 2, 0, 0, 0 \};
\par 
\par \f0 An array may be declared with the last dimension left empty if an initialization is going to occur, or in an external statement.  If such an array is initialized, its size reflects the number of elements actually initialized, in the following example space is allocated for three elements.
\par 
\par \f2 extern int myArray[];
\par int myArray[] = \{ 1, 2, 3\};
\par 
\par \f0 In the 1999 version of the standard, array values may be specified in an arbitrary order, e.g. these declare the same data:
\par 
\par \f2 int myArray[5] = \{ [4] = 4, [2] = 2, [1] = 1 \};
\par int myArray2[5] = \{ 0, 1, 2, 0, 4 \};
\par 
\par \f0 Here the number in braces is the index to be initialized.  In the case that one value is specified in an arbitrary order and the next is not, the second specification occurs at the next linear point in the array.  e.g. for the following the values referenced by the 3rd and 4th indexes are set to 2 and 7 respectively.  Unspecified elements are set to zero.
\par 
\par \f2 int myArray[5] = \{ [2] = 2, 7 \};
\par 
\par \f0 Structures may be initialized as long as they are not automatic variables.  Note: in the 1999 standard automatic variables of type structure or union may be initialized via \cf2\strike type casting\cf3\strike0\{linkID=330\}\cf0 .
\par 
\par \f2 struct _fruit \{
\par \tab enum _type \{apple, orange\}; type;
\par \tab enum _color \{red, green, yellow\} color;
\par \tab int size;
\par \};
\par 
\par struct _fruit myApple = \{ apple, red, 4 \};
\par \f0 
\par If any structure initializations is missing members at the end, they are filled with zeros.  The following declare the same data:
\par \f2 
\par struct _fruit myApple = \{apple, red \};
\par struct _fruit myApple2 = \{apple, red, 0\};
\par \f0 
\par When an initialization occurs for a variable with a type that has nested structures, the inner braces are not required, however, it is a good idea to use them because it clarifies what is going on.  And again, since they structure initializer does not need to enumerate all elements there could be problems if an element is skipped and there aren't braces.
\par 
\par In the 1999 standard, structure elements may be arbitrarily specified, in any order in an initialization:
\par 
\par \f2 struct _fruit myApple = \{ .color = red, .size = 2, .type = apple \};
\par \f0 
\par In the case that one value is specified in an arbitrary order and the next is not, the second specification occurs at the next listed element in the array, e.g. the following specifies the color and size.  Again, unspecified elements are set to zero.
\par 
\par \f2 struct _fruit_myApple = \{.color = red, 4 \};
\par 
\par \f0 Unions are initialized like structures, however, all values in the union share the same space, so only one value may be stored at a time.  Generally another mechanism is used to determine which member is being used, such as wrapping the union in a structure.  For example:
\par 
\par \f2 union value \{
\par \tab int i;
\par \tab float f;
\par \tab complex c
\par \tab imaginary i;
\par \};
\par 
\par \f0 declares a union which can be one of several types of numbers.
\par 
\par When initializing a union variable, only one member may be initialized.  The first member listed  in the union definition is the member that will be initialized:
\par 
\par \f2 union value myValue = \{ 4 \};
\par 
\par \f0 In the 1999 standard, any one of the members may be selected to be initialized by using the same nomenclature as declaring structure values explicitly:
\par 
\par \f2 union value myValue = \{ .f = 4.3e20 \};
\par \f0 
\par It is possible to assign a value to a function pointer which is not an automatic variable:
\par 
\par \f2 int myFunc();
\par int (*myPtr)() = myFunc;
\par 
\par \f0 Arrays of these may be declared:
\par 
\par \f2 int (*myPtrArray[]) = \{ func1, func2, func3, func4 \};
\par 
\par \f0 A\f1  string pointer is a special case.  It may be initialized as follows:
\par 
\par \f2 char *myGreeting = \{ "Hello World" \};
\par 
\par \f1 which allocates a string, then initializes the myGreeting variable with a pointer to it.\f0   The braces are not required in this case.  Also, such initializes may occur for automatic variables.
\par }
50
Scribble50
Types
basic types; types; modifiers; qualifiers; char; short; int; unsigned; long; long long; float; double; long double; complex; imaginary; bool; function pointer; variable length array; VLA; const; volatile; restrict



Writing



FALSE
122
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil\fcharset0 Courier New;}{\f2\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Types\cf0\b0\fs24 
\par 
\par \fs20 All declared variables must have a type.  A type is used to govern the kinds of operations that are valid for the variable, as well as to provide hints to the compiler how to reserve space and process usage of the variable.  For example arrays may be indexed, pointers may be dereferenced, integers may have operations applied to them, and the compiler may need to generate different types of code for integers and floating point variables.
\par 
\par The compiler comes with a set of basic types.  The basic types can be extended through \cf2\strike aggregation\cf3\strike0\{linkID=170\}\cf0 , by adding modifiers, or with the \cf2\strike typedef keyword\cf3\strike0\{linkID=180\}\cf0 .
\par 
\par The basic types may hold differing amounts of storage depending on a compiler and platform.  This basically means that some times can store larger numbers and or have greater precision than other types.  Following is a list of basic types, with examples of using modifiers to create new types.
\par 
\par Character types:
\par \f1 char
\par unsigned char
\par 
\par \f2 Integer types:
\par \f1 short
\par unsigned short
\par int
\par unsigned int
\par long
\par unsigned long
\par \f2 
\par Floating point types\f0 :\f2 
\par \f1 float
\par double
\par long double
\par 
\par \f2 Another type, \f1 void\f2 , is used to signify that a function returns no value, or to declare a pointer to a generic (non typed) entity.
\par 
\par In addition the 1999 standard introduces the \f1 _Bool\f2  \f0 keyword\f2 , the \f1 _Complex\f2  \f0 keyword\f2 , and optionally the \f1 _Imaginary\f2  \f0 keyword\f2 .\f0   These types are spelled with an underscore to keep from name clashes with C++ compilers, but generally a header is included to make them more readable.  A further pair of integer types is also defined:\f1 long long \f2 and\f1  unsigned long long.
\par \f0 
\par For example the system header stdbool.h declares the \f1 bool\f0  type, along with \f1 true\f0  and \f1 false\f0 , while the system header complex.h declares \f1 complex\f0  and \f1 imaginary\f0 . The types added by the 1999 standard are thus:
\par 
\par Boolean types:
\par \f1 bool
\par 
\par \f2 Integer types:
\par \f1 long long
\par unsigned long long
\par 
\par \f2 Imaginary Types\f0 :\f2 
\par \f1 float imaginary
\par double imaginary
\par long double imaginary
\par 
\par \f2 Complex Types\f0 :\f2 
\par \f1 float complex
\par double complex
\par long double complex
\par 
\par \f0 Each type may be modified with one or both of the \f1 const\f0  or \f1 volatile\f0  keywords.  \f1 const\f0  tells the compiler variables of the type won't change once initialized.  \f1 volatile\f0  tells the compiler the variables of the type may change due to external circumstances, and the compiler should be careful with it.  
\par 
\par The 1999 standard also adds a modifier \f1 restrict\f0  which may be used with pointers with parameter scope.  This modifier is used to indicate that the pointer will not point to any value that another parameter which is a pointer points to.  It helps compilers choose a better optimization for code within the function.
\par 
\par For example:
\par 
\par \f1 unsigned short const
\par double volatile
\par \f0 
\par Additionally, types may be modified with pointer or array specifiers.  
\par 
\par A pointer is an indirect reference to another variable.  Dereferencing a pointer references the value of the other variable.  For example:
\par 
\par \f1 int *
\par 
\par \f2 is a pointer type;
\par 
\par \f1 double **
\par 
\par \f2 is a pointer to a pointer\f0 , \f2 and so forth.
\par 
\par \f0 A special case is a pointer to char:
\par 
\par \f1 char *
\par 
\par \f0 which is often used in the run-time library to denote a string of characters.  Generally, such strings will be terminated by a null character.
\par \f2 
\par \f0 An array is a linear list of variables of the specified type.  For example:\f2 
\par \f1 
\par int somename[5];
\par 
\par \f2 declares somename \f0 with a single\f2  \f0 dimension 5, e.g. it is \f2 a linear list of 5 integers.\f0   The dimension on the array ranges from 0 to 4, or one less than the dimension specifier.  These specifiers may be mixed, for example:
\par 
\par \f1 long double **somename2[3][2];
\par 
\par const\f0 , \f1 volatile\f0 , and \f1 restrict\f0  may be freely mixed with pointers, In which case the specifier goes with the type to the left.  In the following example \f1 const\f0  goes with the long double, or *** somename.  And \f1 volatile\f0  goes with long double ** or **somename.
\par 
\par \f1 long double const *volatile** somename;
\par 
\par \f0 Sometimes an array declaration can be left with the last dimension unspecified:
\par 
\par \f1 int somename[];
\par 
\par \f0 This can happen when \cf2\strike initializing\cf3\strike0\{linkID=45\}\cf0  the array, or if the array is declared with external linkage.  In the 1999 standard, it can also happen for arrays that are at the end of \cf2\strike structure\cf3\strike0\{linkID=170\}\cf0  definitions.  In this case the structure can be arbitrarily extended, if memory has been allocated for it.  This is useful for storing variable-length character strings, for example.
\par 
\par In the 1999 standard, arrays are further extended for the possibility of Variable Length Arrays, or VLAs.  VLAs don't have a fixed size, but instead have a size which is generated on the fly by evaluating each dimension as an expression.  For example:
\par 
\par \f1 void myFunc(int m)
\par \{
\par \tab int somename[m];
\par \}
\par 
\par \f0 Such variables may only be placed in block or parameter scope.  At block scope, the dimension may be any expression, but at parameter scope, the dimension has to be one of the previously defined parameters.  For example:
\par 
\par \f1 void myFunc(int m, int arr[m]);
\par \f0 
\par In special cases it may not be convenient to declare the size of the array as an earlier parameter.  In such cases a special syntax is used:
\par 
\par \f1 void myFunc(int arr[*]);
\par 
\par \f0 In this case the array has size equivalent to whatever is passed, and it is up to the author of the function to find a way to learn how long it is.  A pointer to a variable length array may be declared:
\par 
\par \f1 int (*arr_ptr)[m];
\par 
\par \f2 a variable length array may be typedef\f0 'd, in which case the typedef has a fixed size based on the size of the VLA as it would have been if it had been declared at the time of the typedef:
\par 
\par \f1 typedef int VLA_XXX[m];
\par \f2 
\par \f0 Another often-used type is the pointer to function.   The below is a pointer to a function which has one parameter of type\f1  double\f0  and returns an \f1 int\f0 .
\par 
\par \f1 int (*functionPtr)(double x);\f0 
\par }
60
Scribble60
Functions
functions; prototypes; variadic; ellipses; body; inline



Writing



FALSE
75
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Functions\cf0\b0\fs24 
\par 
\par \fs20 A function declaration is an entity capable of doing some type of sequential processing.  It has a prototype which declares the name, return type, and parameters, and optionally a body.  The prototype declares any return type, as well as the list of parameters to the function.  The body is composed of \cf2\strike declarations\cf3\strike0\{linkID=30\}\cf0 , \cf2\strike statements\cf3\strike0\{linkID=70\}\cf0 , and \cf2\strike expressions\cf3\strike0\{linkID=80\}\cf0  and defines actions to be taken when the function is invoked from another function.  In C there is a root function \cf2\strike main()\cf3\strike0\{linkID=200\} \cf0  which gets executed automatically when the program starts.  Other functions are then invoked to make up the program.
\par 
\par The return type in a function body can be any basic or abstract \cf2\strike\f1 type\cf3\strike0\{linkID=50\}\cf0\f0 , except another function or an array.  The return value of a function can however be a pointer to another function.
\par 
\par Each parameter in the parameter list is declared similarly to any other declaration  A subtle difference is that in a normal declaration, the comma separator allows multiple variables with the same basic type to be declared, whereas in parameter lists the comma separator simply separates one parameter from the next.
\par 
\par This documentation will not attempt to explain the original K&R style of function prototypes, although it is supported by this compiler.  Prototypes are used for two purposes.  First, a prototype may be immediately followed by a function body to declare the execution of the function.  Second, a prototype may be a standalone entity used by other files so that the compiler may check for type mismatches.
\par 
\par An example of a function prototype is:
\par 
\par \f2 int getNumberOfCars(struct list *cars);
\par 
\par \f0 which has a single parameter which is a pointer to a list of cars, and returns a count of items on the list.  likewise the following has two parameters:
\par 
\par \f2 int displayFruit(char *name, struct fruit *fruit);
\par 
\par \f1 whereas
\par 
\par \f2 void ShutdownDrive(void);
\par 
\par \f0 is a function which has no parameters and returns no value.
\par 
\par \f2 struct fruit getLastEatenFruit();
\par 
\par \f0 is a function that isn't fully prototyped, but returns a fruit structure.
\par 
\par \f2 int printf(char *s, ...);
\par 
\par \f0 declares a function whose first parameter is a character string, and the ellipses(...) indicates an unspecified (variadic) list of parameters following it.  The method for determining the number of passed arguments is dependent on the purpose of the function.  For example the argument list could be null terminated, or the first unspecified argument could be a count, or the string could somehow specify the number of arguments.  This function returns some integer value.  The macros in stdarg.h can be used to extract the unspecified arguments, if necessary.
\par 
\par A function may optionally have a body.  Assuming \f2 struct list\f0  has a member \f2 next\f0  which points to the next linked item in the list we could write:
\par 
\par \f2 #include <stdlib.h> /* definition of NULL */
\par int getNumberOfCars(struct list *cars)
\par \{
\par \tab int returnValue = 0;
\par \tab while (cars != NULL)
\par \tab\{
\par \tab\tab returnValue = returnValue + 1;
\par \tab\tab cars = cars->next;
\par \tab\}
\par \tab return returnValue;
\par \}
\par 
\par \f1 Note that in C, parameters are reserved separate space from the original passed variable.  This means they can be used freely as local variables, without modifying the original value.  However if a pointer or array is \f0 passed and then \f1 dereferenced, it \f0 may\f1  be used to modify the same data the original pointer or array would access when dereferenced.\f0   A statement can be prevented from accidentally modifying such data by use of the \f2 const\f0  keyword in the parameter declaration.
\par 
\par In the 1999 standard, function declarations may be qualified with the \f2 inline\f0  linkage type.  For example:
\par 
\par \f2 inline int incrementBirdCount()
\par \{
\par \tab return ++birds;
\par \}
\par \f1 
\par \f0 If the compiler can, it will take such functions and instead of making them external standalone entities it will simply compile them in line in the current function.  For example:
\par 
\par \f2 while (watchingBirds())
\par \{
\par \tab if (seenBird)
\par \tab\tab printf("I've seen %d birds", incrementBirdCount());
\par \}
\par \f0 
\par will be as if the compiler was compiling:
\par 
\par \f2 while (watchingBirds())
\par \{
\par \tab if (seenBird)
\par \tab\tab printf("I've seen %d birds", ++birds);
\par \}
\par \f0 
\par However, the inline qualifier is not specified to absolutely require such behavior.  A compiler is free to take inline functions that are too complex and compile them as separate entities.  Also, even when a compiler honors the inline linkage, the fact that the function is recompiled in line every time it is used means the overall program may get a little bigger.  In general, inline functions should be kept simple because of these factors.\f2 
\par }
70
Scribble70
Statements
statements; blocks



Writing



FALSE
37
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Statements\cf0\b0\fs24 
\par 
\par \fs20 A statement is the a basic unit of execution.  Statements can be blocks containing other statements, evaluate arbitrary \cf2\strike\f1 expressions\cf3\strike0\{linkID=80\}\cf0\f0 , assign values to \cf2\strike\f1 variables\cf3\strike0\{linkID=40\}\cf0\f0 , or \cf2\strike\f1 transfer control\cf3\strike0\{linkID=210\}\cf0\f0  to a different place.
\par 
\par A block is statement, which is a list of other statements.  It is most often used in conjunction with control statements, to group the actions to be taken as a result of the control statement.  A block is surrounded by braces.  For example:
\par 
\par \f2 if (bSwitchedOn)
\par \{
\par \tab countOfOnSwitches++;
\par \tab TurnOnLight();
\par \tab TurnOnFan();
\par \}
\par 
\par \f1 In this case \f2 countOfOnSwitches++\f1  is an \cf2\strike expression\cf3\strike0\{linkID=80\}\cf0  being evaluated.  This particular expression has a side effect\f0  which makes it useful.  \f1  \f0 E\f1 valuating an expression such as \f2 b+5\f1  without an assignment \f0 could be done although it would not result in any type of meaningful result.  \f2 TurnOnLight()\f0  and \f2 TurnOnFan()\f0  are also expressions being evaluated.\f1 
\par 
\par \f0 Historically, any number of \cf2\strike declarations\cf3\strike0\{linkID=30\}\cf0  may occur within a function body, but they must occur at the beginning of a block.  The 1999 standard relaxes this constraint and allows declarations to occur anywhere within a function body.  However in some cases, as with \cf2\strike VLAs\cf3\strike0\{linkID=50\}\cf0 , certain statements such as goto are not allowed to branch to a point within a block after the variable has been declared.  And in any case using goto to branch around declarations with \cf2\strike initializers\cf3\strike0\{linkID=45\}\cf0  is problematic as the initializers will not be executed.\f1 
\par 
\par \f0 A value may be assigned to a variable within a statement or declaration at block scope:
\par 
\par \f2 int result, a = 7, b=3+a;
\par result = b + 5;
\par 
\par \f0 Pointers may be dereferenced as part of an assignment:
\par 
\par \f2 char *p = &myString;
\par (*p) = 'C';
\par 
\par \f1 Note that assignments themselves return values, so that:
\par 
\par \f2 int a,b,c = 5 ;
\par 
\par a = b = c;
\par 
\par \f1 sets both a and b to be equal to 5.
\par }
80
Scribble80
Expressions
Expressions



Writing



FALSE
8
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Expressions\cf0\b0\fs24 
\par 
\par \fs20 Expressions are used to perform arithmetic operations.  Built-in \cf2\strike operators\cf3\strike0\{linkID=320\}\cf0 , combined with \cf2\strike variables\cf3\strike0\{linkID=40\}\cf0  and \cf2\strike constants\cf3\strike0\{linkID=190\}\cf0 , may be use to evaluate an equation and store the result in a variable.  Expressions may also be use to form the selector in a \cf2\strike control statement\cf3\strike0\{linkID=210\}\cf0 , or to create an \cf2\strike initializer\cf3\strike0\{linkID=45\}\cf0  for a variable.
\par 
\par An expression has a \cf2\strike type\cf3\strike0\{linkID=50\}\cf0 .  It may be converted from one type to another using \cf2\strike type casting\cf3\strike0\{linkID=330\}\cf0 .  Type casting may be used for example to truncate an integer or affect the precision of a floating point value.\fs24 
\par }
90
Scribble90
Macro Expansion
macro; expansion; macro expansion; preprocessor; preprocessor macro expansion; #define; #undef



Writing



FALSE
88
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Macro Expansion\f1\fs20 
\par 
\par 
\par \cf0\b0\f0 Macro substitution is a text-based substitution of one set of characters with another.  Once a macro is defined, and any instance of the text of the macro name is replaced with the text of its definition prior to compilation. This is similar to the search-and-replace function of an editor.  However the macro expansion done by the preprocessor is somewhat more substantial.  It allows that a macro can be defined with arguments, which themselves will be replaced in the text to be substituted.  A preprocessor directive is used to define a macro, as follows:
\par 
\par \tab\f2 #define RED 3
\par \f0 
\par defines the macro RED to be substituted with the text '3'.  For example if a later \cf2\strike statement\cf3\strike0\{linkID=70\}\cf0  is like this:
\par 
\par \f2\tab appleColor = RED;\f0 
\par 
\par The preprocessor would expand the statement to:
\par 
\par \f2\tab appleColor = 3;\f0 
\par 
\par before handing it over to the compiler.  This is a common way to give untyped constants names.  Note: often macro names are written in upper case like this.
\par 
\par A more complex function-like macro might look like this:
\par 
\par \f2\tab #define MUL(left, right) left * right
\par 
\par \f0 so that one could write:
\par 
\par \f2\tab result = MUL(dollars, 100);
\par \f0 
\par and the preprocessor would expand the statement to:
\par 
\par \f2\tab result = dollars * 100;
\par 
\par \f0 before handing it to the compiler.
\par 
\par Note that the definition of MUL above is not quite adequate, because the preprocessor will faithfully substitute any text given in an argument to the macro argument.  For example if one wrote:
\par 
\par \f2\tab result = MUL(a+b, c+d) * e;
\par 
\par \f0 the preprocessor would expand it to:
\par 
\par \f2\tab result = a+b * c+d * e;
\par 
\par \f0 which does not have the desired meaning because of \cf2\strike operator precedence\cf3\strike0\{linkID=160\}\cf0 .  To be safe macros taking arguments should use parenthesis liberally, for example a better definition of MUL would be:
\par 
\par \f2\tab #define MUL(left, right) ((left) * (right))
\par 
\par \f0 and the expansion of the above statement would be:
\par 
\par \f2\tab result = ((a+b) * (c+d)) * e;
\par 
\par \f0 which is the desired result.
\par 
\par Macros may be undefined again.  For example:
\par 
\par \f2\tab #undef\tab RED
\par 
\par \f0 will cause subsequent uses of RED to not be replaced.
\par 
\par In the 1999 C standard, macros have been extended to allow variable-length argument lists.  For example:
\par 
\par \f2\tab #define printerror(text, ...)  fprintf(stdout, text, __VA_ARGS__);
\par \f0 
\par could be used as:
\par 
\par \f2\tab printerror("%d",errno);
\par 
\par \f0 to generate:
\par 
\par \f2\tab fprintf(stdout, "%d", errno);
\par 
\par \f1 In this case \f0 ... means there is an unspecified number of arguments to follow, and \f1 __VA_ARGS__ means copy all remaining arguments to this point in the substitution.
\par 
\par \f0 There are a few macros that are always defined.  These macros can be used to detect standards conformance, or to get information about the entity being compiled.  They are as follows:
\par 
\par \pard\tx2800\tx2820 __FILE__\tab The name of the file being compiled, as a string.
\par __LINE__\tab The line number of the current line, as a number.
\par __DATE__\tab The date the file is being processed, as a string.
\par __TIME__\tab The time the file is being processed, as a string.
\par 
\par __ORANGEC__\tab The LADSoft compiler is being used. No value.
\par __386__\tab This is an x86 compiler. No value.
\par __i386__\tab This is an x86 compiler.  No value.
\par __STDC__\tab The value 1, this compiler is ANSI compliant.
\par __STDC_VERSION__\tab The numeric value 199901L, only defined when compiling in C99 mode.
\par 
\par This compiler also defines _WIN32 and __WIN32__ if appropriate.
\par 
\par \cf1\b\f1 
\par }
100
Scribble100
Conditional Compilation
conditional; preprocessor; preprocessor conditional; #if; #ifdef; #ifndef; #else; #elif; #endif; defined



Writing



FALSE
40
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Conditional Compilation\f1\fs20 
\par 
\par \cf0\b0\f0 Conditional compilation is used to control what sections of source code actually get compiled.  It is possible to use conditional compilation directives to include some sections of the file, and exclude others.  Often this is done based on some configuration, e.g. including alternate functionality based on a compiler or operating system.  Many included files also have include guards, which are implemented with conditional compilation in case the included file accidentally gets parsed twice.
\par 
\par Configuration can be done through macros.  When they are expanded they guide the conditional compilation directives
\par 
\par #if\tab some constant \cf2\strike expression\cf3\strike0\{linkID=80\}\cf0 
\par \tab evaluates the expression, and compiles the following sequence of statements if it is true.
\par #ifdef\tab MACRONAME
\par \tab if MACRONAME has been defined with a #define statement, the following sequence of statements is compiled
\par #ifndef MACRONAME
\par \tab if MACRONAME has not been defined with a #define statement, the following sequence of statements is compiled
\par #else
\par \tab switch the compilation mode: if the previous sequence was being compiled, the next sequence will not be
\par \tab if the previous sequence was not being compiled, the next sequence will be.
\par #elif\tab some expression
\par \tab combines #else and #if
\par #endif
\par \tab ends a conditional statement
\par 
\par As part of the expression for a #if or #elif, the keyword \f2 defined\f0  may be used.  For example:
\par 
\par \f2 #if defined(RED) && defined(BLUE)
\par 
\par \f0 compiles the following code sequence if the macro RED has been defined and the macro BLUE has been defined.
\par 
\par Note: while the #else normally swaps the compilation mode, this will not happen if the #else is nested in a higher-level directive pair that is not itself being compiled.
\par 
\par As an example:
\par 
\par \f2 #ifdef RED
\par \tab int color = RED;
\par #else
\par \tab int color = GREEN;
\par #endif
\par 
\par \f1 creates a variable \f2 color\f1 .  If RED \f0 has been previously\f1  defined\f0  with the \f2 #define \f0 directive\f1 , color is \f0 declared and \f1 set to RED, else color is \f0 declared and \f1 set to GREEN.
\par }
110
Scribble110
Pragma Statements
pragma; preprocessor pragma statements; #pragma



Writing



FALSE
30
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\fs32 Pragma Statements\f1\fs20 
\par 
\par \cf0\b0\f0 Pragma statements are directives to the compiler.  They can mean almost anything, and except for a standard pragma space defined by the 1999 standard, the definition is at the whim of the compiler designer.  Therefore this documentation will not go into detail about the things that may be done with pragma statements.
\par 
\par The basic definition of a pragma statement is:
\par 
\par \f2\tab #pragma any text here
\par 
\par \f0 If the compiler recognizes the text, it will parse it and do something.  What it will do is dependent on the text and the compiler.  However, the 1999 standard specifies that if the first word is STDC, the directive is now reserved as part of the standard.  Currently, there are three such pragma statements:
\par 
\par #pragma STDC FENV_ACCESS OFF
\par #pragma STDC CX_LIMITED_RANGE OFF
\par #pragma STDC FP_CONTRACT ON
\par 
\par The final value of each can be either on or off; defaults are given above.
\par 
\par The FENV_ACCESS pragma, when on, indicates that the user may have gone around the compiler run-time system and changed settings in the floating point controller.  This is used in case the compiler needs to generate additional code to compensate.
\par 
\par The CX_LIMITED_CONTRACT pragma, when on, allows the compiler to assume that certain intrinsic functions such as multiply, divide, and absolute values of complex numbers can be done with a limited range.  This allows the compiler to choose a faster algorithm, on the assumption that intermediate results will not go out of range for the complex type being used.
\par 
\par The FP_CONTRACT pragma, when on, allows the compiler to calculate the result of constant expressions involving floating point numbers at compile time.  Turning it off can be useful to control the precision of the result.
\par 
\par Each of these pragma values, when used inside a function, is restored at the end of the current block.
\par 
\par In the 1999 standard, pragma values may also be invoked with a statement within a function body:
\par 
\par \tab _Pragma("STDC CX_LIMITED_RANGE_ON");\f2 
\par }
120
Scribble120
Preprocessor Diagnostic Directives
diagnostic; preprocessor diagnostic; #line; #error



Writing



FALSE
14
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Preprocessor Diagnostic Directives\f1\fs20 
\par 
\par \cf0\b0\f0 Diagnostic directives are used to change the characteristics reported for the file.\cf1\b\f1 
\par \cf0\b0\f0 
\par \f2\tab #line\tab 10, "filename.c"
\par 
\par \f0 causes the compiler to think it is at line 10 in filename.c, and increment lines from there until either the file is exhausted or another #line directive is encountered.  This effects diagnostics issued by the compiler, and also modifies the results of the __LINE__ and __FILE__ \cf2\strike macros\cf3\strike0\{linkID=90\}\cf0 .
\par 
\par \f2\tab #error some user error
\par 
\par \f0 causes the compiler to issue a severe diagnostic with the text 'some user error'.  This error will cause the compile to fail. It is usually used inside a \cf2\strike conditional compilation\cf3\strike0\{linkID=100\}\cf0  directive, to signify that some type of configuration needed by the program is not correct.  For example if several macro definitions within a configuration file are in an inconsistent state \f2 #error\f0  could be used to notify the user.\cf1\b\f1 
\par }
130
Scribble130
Include Files
include files; preprocessor include; #include



Writing



FALSE
16
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Include Files\cf0\b0\f1\fs20 
\par 
\par \f0 The include file directive is used to include the contents of one file into the current file being translated.  The file may be some system file that holds definitions of RTL or WIN32 functions, or it may be a project file that holds \cf2\strike macros\cf3\strike0\{linkID=90\}\cf0  and \cf2\strike declarations\cf3\strike0\{linkID=30\}\cf0  used in several different places.  When a file is included, all macros and declarations in the included file become visible within the file that performed the conclusion.
\par 
\par The general form is either:
\par 
\par \f2 #include <filename.h>
\par 
\par \f0 or
\par \f2 
\par #include "filename.h"
\par 
\par \f0 The difference between the two forms isn't well defined, and in fact in this compiler they are very similar.  However, the basic idea is that the <> form is to be used for system files, for example the files that came with the compiler.  And the "" form is to be used for project-specific files.  In general it is a good idea to stick to this usage as well as possible, to maintain portability to other compilers.\f1 
\par }
140
Scribble140
Comments
comments



Writing



FALSE
22
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Comments\cf0\b0\f1\fs20 
\par 
\par \f0 A comment is a section of the file meant to augment source code with human-readable text.  Any text inside a comment is simply ignored by the compiler.  However there are some types of documentation tools which will parse specific text sequences within comments to make documentation, and other types of tools such as linting tools which look for keywords in comments to guide the tool's progress.
\par \f1 
\par \f0 In the C language, comments were originally denoted like this:
\par 
\par \cf2\f2 /* This is a comment block */
\par 
\par /* This comment block
\par  * spans multiple lines
\par  */
\par 
\par \cf0\f0 where a comment extends from the /* to the */.   It is undefined as to whether comments can be nested.  Some compilers allow it and some don't.  It isn't generally a good idea to nest comment blocks.
\par 
\par The 1999 standard added the form:
\par 
\par \cf2\f2 // This is a comment line
\par 
\par \cf0\f0 Which extends a comment from the // to the end of the current line.  Many compilers were already supporting this prior to adoption of the 1999 standard.\f1 
\par }
150
Scribble150
Line Concatenation
line concatenation



Writing



FALSE
16
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\fs32 Line Concatenation\cf0\b0\f1\fs20 
\par 
\par \f0 Line concatenation may be used to concatenate several lines into one long string.  It may be used for example to make a long macro definition as follows:
\par 
\par \f2 #define MYFUNC(mybool) \{ if (mybool) \{ \\
\par \tab\tab\tab\tab\tab printf("true"); \\
\par \tab\tab\tab\tab  \} \\
\par \tab\tab\tab\tab  else \{ \\
\par \tab\tab\tab\tab\tab printf("false"); \\
\par \tab\tab\tab\tab  \} \\
\par \tab\tab\tab      \}
\par 
\par \f1 Where the backslash character is used to concatenate lines.  This is necessary with macro definitions since a preprocessor directive ends at the end of the current line\f0 .  However line concatenation may be used anywhere in a source file.\f1 
\par }
155
Scribble155
Trigraphs
trigraphs; ??



Writing



FALSE
19
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Trigraphs\cf0\b0\f1\fs20 
\par 
\par \f0 Trigraphs are a long-hand representation of certain characters not always found on non-english keyboards.  They can be used to enter these characters, when no key sequence exists for them.  Trigraphs always start with two question marks.  The trigraphs are as follows:
\par 
\par Trigraph\tab\tab ASCII Character
\par ??(\tab\tab [
\par ??)\tab\tab ]
\par ??<\tab\tab\{
\par ??>\tab\tab\}
\par ??/\tab\tab\\
\par ??=\tab\tab #
\par ??!\tab\tab |
\par ??'\tab\tab ^
\par ??-\tab\tab ~
\par 
\par Trigraphs will be substituted anywhere in the source file, including within \cf2\strike string constants\cf3\strike0\{linkID=195\}\cf0 .
\par }
160
Scribble160
Operator Precedence
operator precedence; precedence



Writing



FALSE
49
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\fs32 Operator Precedence\cf0\b0\f1\fs20 
\par 
\par \f0 The operator precedence determine what orders the operators are evaluated in when multiple operators appear in constant expressions.  The following table shows the precedence of each operator from highest to lowest.  Operators with the same precedence are listed in the same row.  Each operator is evaluated in the context of the associativity specified in the table.
\par 
\par 
\par \pard\tx6460 Operator\tab Associativity
\par --------------------------------------------------------------------------------------------------------------------------------
\par \f1 (expr)    [index]    ->\f0\tab\f1 Left ==> Right
\par !    ~    ++    --    (type)    sizeof\f0    \f1 Unary operator:    +    -    *\f0  &\tab\f1 Right <== Left
\par *    /    %\f0\tab L\f1 eft ==> Right
\par +    -\f0\tab\f1 Left ==> Right
\par <<    >>\f0\tab\f1 Left ==> right
\par <    <=    >    >=\f0\tab\f1 Left ==> Right
\par ==    !=\f0\tab\f1 Left ==> Right
\par Binary operator:    &\f0\tab\f1 Left ==> Right
\par Binary operator:    ^\f0\tab\f1 Left ==> Right
\par Binary operator:    |\f0\tab\f1 Left ==> Right
\par &&\f0\tab\f1 Left ==> Right
\par ||\f0\tab\f1 Left ==> Right
\par expr ? true_expr :  false_expr\f0\tab\f1 Right <== Left
\par +=    -=    *=    /=    <<=\f0  \f1 &=   ^=    |=   %=   >>=    =\f0\tab\f1 Right <== Left
\par \pard ,
\par 
\par \f0 According to these rules, the expression
\par 
\par a = b + c * d;
\par 
\par is evaluated as if it were written:
\par 
\par a = ( b + (c * d));\f1 
\par 
\par \f0 As an example of associativity:
\par 
\par b * c * d;
\par 
\par is evaluated from left to right as if it were written:
\par 
\par (b * c) * d;
\par 
\par whereas
\par 
\par b = c= d ;
\par 
\par is evaluated from right to left, as if it were written:
\par 
\par b = (c = d);\f1 
\par }
165
Scribble165
sizeof operator
sizeof



Writing



FALSE
34
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\fs32 sizeof operator\cf0\b0\f1\fs20 
\par 
\par \f0 The sizeof operator may be used in a number of ways to get the size of an entity.  The two most used methods get the size of a type, or the size of a variable.  However, within a function body sizeof will get the size of any expression.  Care needs to be taken with this as sizeof is evaluated at compile time; if there are side effects in an expression within a sizeof operator, they will never be executed.
\par 
\par Everything sizeof returns is in units of sizeof(char).  sizeof(char) is defined to be one.
\par 
\par For example:
\par 
\par \f2 int size = sizeof(int);
\par 
\par \f0 gets the size in characters of the int type.
\par 
\par \f2 int size = sizeof(struct fruit);
\par 
\par \f0 gets the size of the structured type tagged as fruit.
\par 
\par \f2 int size = sizeof(TYPEDEFFED_TYPE);
\par \f0 
\par gets the size of a type that was declared with typedef.
\par 
\par \f2 int size = sizeof(void *) ;
\par 
\par \f0 gets the size of an untyped pointer;
\par 
\par \f2 int size = sizeof(myVariable);
\par 
\par \f0 gets the size of a variable.
\par 
\par Sometimes sizeof does not need parenthesis. In general, if it is a variable or basic type the parenthesis aren't necessary..
\par 
\par \f2 int size = sizeof int;\f0 
\par }
170
Scribble170
Type Aggregation
aggregate types; struct; union; enum; initialization



Writing



FALSE
82
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Type Aggregation\cf0\b0\f1\fs20 
\par 
\par \f0 Type aggregation is used to combine a set of types into a larger type.  The three types of aggregation in the C language are \f2 struct\f0 , \f2 union\f0 , and \f2 enum\f0 .  An aggregate type definition consists of the associated aggregate keyword, an optional tag, and the declaration of the type.  Once the type is declared, it may optionally be used to declare variables immediately.  Such variables can optionally have initializers.
\par 
\par The tag is used to access the aggregate type.  It is in a different space than normal names, so for example the following declarations will not collide:
\par 
\par \f2 int _colors;
\par enum _colors;
\par 
\par enum\f0  gathers related integer constants together, giving them a name and value.  For example:
\par 
\par \f2 enum _colors \{ RED, GREEN, BLUE\};
\par 
\par \f0 declares an enumerated aggregate with a tag _colors, which may have one of the values RED, GREEN, or BLUE.  These are mapped to integers starting with 0, so RED = 0, GREEN = 1, BLUE = 2.  The enumeration constants may be given explicit values:
\par 
\par \f2 enum _colors \{ RED, GREEN=10, BLUE\};\f0 
\par 
\par When a mapping to an integer is specified, subsequent values will be indexed starting with the value.  For example in the above, RED=0, GREEN=10, BLUE=11.
\par 
\par A variable may be declared immediately during the declaration, or by using the tag:
\par 
\par enum _colors \{ RED, GREEN, BLUE\} rgbVar;
\par enum _colors rgbVar;
\par 
\par \f2 struct\f0  gathers a list of variables into a larger type.  Within the larger type, space will be reserved for each member variable in the order the variables are listed.  For example:
\par 
\par \f2 struct fruit \{
\par \tab enum _fruittype \{apple, orange\} type;
\par \tab enum _colors color;
\par \tab int size;
\par \};
\par \f0 
\par has three variables - the first is the type of fruit, the second is its color, and the third is its size.
\par 
\par Structures can hold any other type, including other structures, and arrays.  In the 1999 version of the standard, the last element of a structure may be a dimensionless array:
\par 
\par \f2 struct myStruct \{
\par \tab int size;
\par \tab char data[];
\par \};
\par 
\par \f0 The size of such as structure as reported by \cf2\strike\f2 sizeof()\cf3\strike0\{linkID=165\}\cf0\f0  does not include any size for the data member.  However, if the structure is dynamically allocated the array may be any required length.
\par \f1 
\par \f0 Unions are declared like structures, however, all values in the union share the same space, so only one value may be stored at a time.  Generally another mechanism is used to determine which member is active, such as wrapping the union in a structure.  For example:
\par 
\par \f2 union value \{
\par \tab int i;
\par \tab float f;
\par \tab float complex c
\par \tab float imaginary i;
\par \};
\par 
\par \f0 declares a union which may be one of several types of numbers.
\par 
\par structures and unions may be declared without a body:
\par 
\par \f2 struct fruit;
\par 
\par \f0 This allows them to be referenced before they are declared, or in situations where a full declaration is not necessary.  This also allows circular references between two or more structures:
\par 
\par \f2 struct _auto \{
\par \tab struct _tires *tires;
\par \};
\par 
\par struct _tires \{
\par \tab struct _auto *owner;
\par \};
\par 
\par \f0 H\f1 owever\f0  \f1 the structure declaration can only be used to declare pointers\f0  unless it has been given a body.
\par 
\par An added feature of structures and unions is that bit fields may be declared within them.  For example the following defines a structure with three bit fields.  The first holds a 3 bit value, the second holds a one-bit value, and the third holds a 4-bit value.
\par 
\par \f2 struct _myBitFields \{
\par \tab int field1 : 3;
\par \tab int field2 : 1;
\par \tab int field3 : 4;
\par \};
\par 
\par \f0 F\f1 rom a standards viewpoint the only basic type a bit field may have is \f2 int.  \f0 However, compilers that take advantage of processor-based instructions to actually pack these fields within a single machine word sometimes allow the use of other integer types to specify a machine-specific data sizes.  However, such packing is beyond the scope of the standard and inherently non portable.\f2 
\par }
180
Scribble180
Typedef Keyword
typedef



Writing



FALSE
32
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\fs32 Typedef Keyword\cf0\b0\f1\fs20 
\par 
\par \f0 The typedef keyword is used to create a type with a new name.  In some cases it is used to simply give a generic name to a built-in type; in other cases it is used as a shorthand to a more complex type.  The typedef has two parts; a type, and a name.  Neither part is optional.
\par 
\par In general, the type declaration is the same as a variable declaration, except that instead of allocating storage the name given may be used as an alias for the type, when creating other types or declaring variables.
\par 
\par For example:
\par 
\par \f2 typedef unsigned U32;
\par 
\par \f0 declares the type name U32 to be equivalent to unsigned.
\par 
\par \f2 typedef unsigned *U32_ptr;
\par \f0 
\par declares the type name U32_ptr to be equivalent to unsigned *.
\par 
\par \f2 typedef void (*MYFUNC_ptr)();
\par 
\par \f0 declares a pointer to a function that has no parameters and returns no value.
\par 
\par \f2 typedef void MYFUNC(int a);
\par 
\par \f0 declares a function prototype and so on.
\par 
\par In each case a variable of the type may be defined simply by using the name in the typedef as the type.  For example:
\par 
\par \f2 MYFUNC_ptr funcptr;
\par 
\par \f0 declares a pointer to a function using the previous type definition.\f1 
\par }
190
Scribble190
Constants
constants



Writing



FALSE
43
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\fs32 Constants\cf0\b0\f1\fs20 
\par 
\par \f0 There are several types of constants in the C language.  For example:
\par 
\par 40, -40 are decimal integer constants.
\par 
\par 0x65 is a hexadecimal (base 16) unsigned constant because it has a leading '0x'.
\par 
\par 047 is an octal (base 8) unsigned constant because it has a leading zero.
\par 
\par '&' is a character constant for the ampersand character.
\par L'&' is the wide character version of the above.
\par 
\par "hello world" is a string constant.  It implicitly has extra storage for a trailing null character.
\par L"hello world" is a wide character version of the above.
\par 
\par 4.0, -0.0437, 10.77e-22 are floating point constants.
\par In the 1999 version of the standard, 0x1.5p23 is also a floating point constant, specified in hexadecimal.
\par 
\par 'I' as defined in complex.h, is the constant imaginary 1.  It may be combined with real values to create complex and imaginary values.
\par 
\par Numeric constants can have various suffixes to specify the size of the constant:  These can be specified in either lower or upper case.  For example:
\par 
\par 7UL
\par 
\par is the unsigned long constant 7.  
\par 
\par By default integer constants are of int type.  The suffixes for integers are as follows:
\par 
\par U\tab\tab - unsigned
\par L\tab\tab - long
\par UL\tab\tab - unsigned long
\par LL\tab\tab - long long
\par ULL\tab\tab - unsigned long long
\par 
\par By default floating point constants are doubles.  The suffixes for floating point values are as follows:
\par F\tab\tab - float
\par L\tab\tab - long double.
\par \f1 
\par \f0 Functions and variables declared at file scope have a fixed location, and this location is a constant.  Additionally, the location of a statically-linked variable within a function body is a constant.  The location of auto variables and parameters are never constant.\f1 
\par }
195
Scribble195
String Constants
string constants



Writing



FALSE
32
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\fs32 String Constants\cf0\b0\f1\fs20 
\par 
\par \f0 String constants are made up of a combination of ASCII characters and control characters.  Each control character is signified by the character '\\' followed by a control character specifier.  The sequence "\\\\" is used as a representation of the plain ASCII character '\\'.  Each string constant has enough space reserved for an additional null terminator.
\par 
\par Control characters may also be used in character constants.
\par 
\par Control characters are as follows:
\par 
\par \\a\tab beep
\par \\b\tab backspace
\par \\f\tab form feed
\par \\n\tab line feed
\par \\r\tab carriage return
\par \\v\tab vertical tab
\par \\t\tab horizontal tab
\par \\'\tab the ' character.  Used when ' is to be a character constant: '\\''
\par \\"\tab the " character.  Used when " is to be embedded in a string constant
\par \\?\tab the ? character. Used to prevent trigraphs from being contracted
\par \\x\tab an embedded hexadecimal value.  For example \\x24 is the '$' symbol.
\par \\0-7\tab an embedded octal value with three digits.  (the 1999 standard allows for less than three).   For example \\044 is the '$' symbol.
\par 
\par an example is:
\par 
\par \f2 printf("\\a\\044Error\\x24\\nHi");
\par 
\par \f0 which beeps and prints the following message:
\par 
\par \f2 $Error$
\par Hi\f1 
\par }
200
Scribble200
main
main; argc; argv



Writing



FALSE
12
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\fs32 main\cf0\b0\f1\fs20 
\par 
\par \f0 The function main() is the first function called in a standard C program.  It is given a list of command line arguments, and returns a value back to the OS for external processing.  Every program must have a main.  (Note, windows Gui programs and dlls somewhat break this standard). The prototype for main is:
\par 
\par \f2 int main(int argc, char *argv[]);
\par 
\par \f0 main returns a value, which is often handed off to some operating system command processor for external use.  The constants EXIT_SUCCESS and EXIT_FAILURE in stdlib.h give basic return codes, however, other codes can be used.
\par 
\par The argument argc gives the number of command line arguments, and argv is a null-terminated array with pointers to the arguments.  Note that the first argument is the name of the program which is running.   So argc will always be one or greater and the first actual command line argument is stored in argv[1].\f1 
\par }
210
Scribble210
Control Statements
control statements



Writing



FALSE
12
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Control Statements\cf0\b0\f1\fs20 
\par 
\par \f0 There are several types of control statements.  Many types of control statements take a selecting \cf2\strike expression\cf3\strike0\{linkID=80\}\cf0 , which governs the use of the statement e.g. by choosing between selections or setting exit conditions for a loop.  Conditional statements may be used to choose an action, or between two or more sets of actions based on a selection.  Looping statements may be used to repeat an action until some condition is met.  Transfer statements simply transfer control to another point in the program.
\par 
\par There are two types of conditional statements.  These are the \cf2\strike\f2 if\cf3\strike0\{linkID=230\}\cf0\f0  statement and the \cf2\strike\f2 switch\cf3\strike0\{linkID=240\}\cf0\f0  statement.  The \f2 if\f0  statement is used to conditionally select an action or choose between two alternative actions, whereas the \f2 switch\f0  statement is used to choose between an arbitrary number of actions.
\par 
\par There are three types of loop statements.  The \cf2\strike\f2 for\cf3\strike0\{linkID=250\}\cf0\f0  statement is usually used to loop through a known quantity, such as an array or linked list.  The \cf2\strike\f2 while\cf3\strike0\{linkID=270\}\cf0\f0  statement and \cf2\strike\f2 do-while\cf3\strike0\{linkID=260\}\cf0\f0  statement are used for more generic loops which have a boolean exit condition.  The difference between the \f2 while\f0  statement and the \f2 do-while\f0  statement is that in the \f2 while\f0  statement, the exit condition is evaluated at the beginning of each loop, whereas in the \f2 do-while\f0  statement the exit condition is evaluated once at the end of each loop.  This means a \f2 while\f0  statement can be shorted circuited by setting the exit-condition false before entering the loop, whereas a \f2 do-while\f0  statement is guaranteed to execute at least once.
\par 
\par There are a variety of transfer statements.  The \f2 goto\f0  statement transfers control to an arbitrary point in the function, by accessing a label placed in the function.  The \cf2\strike\f2 break\cf3\strike0\{linkID=290\}\cf0\f0  and \cf2\strike\f2 continue\cf3\strike0\{linkID=300\}\cf0\f0  statements are used in loops; \f2 break\f0  means to exit the loop whereas \f2 continue\f0  means go to the beginning or end of the loop and re-evaluate the selecting expression.  The\f2  break\f0  statement is also used in conjunction with \f2 switch\f0  statements, to delimit one set of actions from the next.  The \f2 case\f0  statement is used in conjunction with \f2 switch\f0  statements, to specify constant selection values associated with actions.  Finally, the \cf2\strike\f2 return\cf3\strike0\{linkID=310\}\cf0\f0  statement is used to exit the current function, and optionally specifies a value to be returned to the caller.
\par }
230
Scribble230
If Statement
if



Writing



FALSE
28
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 If Statement\cf0\b0\f1\fs20 
\par 
\par \f0 The if statement takes an  boolean, integer, or pointer as a selecting \cf2\strike expression\cf3\strike0\{linkID=80\}\cf0 .  It controls access to one statement.  An optional else clause is executed if the first statement is not executed.  
\par 
\par If the value of the selecting \cf2\strike expression\cf3\strike0\{linkID=80\}\cf0  is true, nonzero, or non null, the statement or block is executed.  Otherwise, the statement or block is not executed, and if an else statement immediately follows, that is executed instead.
\par 
\par For example:
\par 
\par \f2 if (stoplightIsRed)
\par \{
\par \tab StopCar();
\par \}
\par else
\par \{
\par \tab StartCar();
\par \}
\par 
\par \f1 This example \f0 may\f1  also be written with\f0 out\f1  the block scoping:
\par 
\par \f2 if (stoplightIsRed)
\par \tab StopCar();
\par else
\par \tab StartCar();
\par 
\par \f1 
\par }
240
Scribble240
Switch Statement
switch



Writing



FALSE
26
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Switch Statement\cf0\b0\f1\fs20 
\par 
\par \f0 A switch statement uses a selecting \cf2\strike expression\cf3\strike0\{linkID=80\}\cf0  to choose between an arbitrary list of actions.  In a switch statement, the selection must be an integer or enum type.  Case statements nested within the switch statement serve to choose between the selector values, and an optional default statement is used to catch any values that have not been explicitly listed in case statements.  For example:
\par 
\par \f2 enum _colors \{ RED, YELLOW, GREEN \} currentLight;
\par \tab ...
\par switch (currentLight)
\par \{
\par \tab case RED: /* if currentLight == RED */
\par \tab\tab StopCar();
\par \tab\tab break;
\par \tab case YELLOW: /* if currentLight == YELLOW */
\par \tab\tab SlowDown();
\par \tab\tab break;
\par \tab case GREEN: /* if currentLight == GREEN */
\par \tab\tab KeepGoing();
\par \tab\tab break;
\par \tab default: /* if currentLight == any other value */
\par \tab\tab printf("currentLight has an unknown value");
\par \tab\tab break;
\par \}
\par 
\par \f0 N\f1 ote that the select\f0 ion\f1  values in case statements must be \f0 integer or enumerated \f1 constants.
\par }
250
Scribble250
For Statement
for



Writing



FALSE
35
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 For Statement\cf0\b0\f1\fs20 
\par 
\par \f0 The for statement is a looping construct often used for iterating through arrays or lists.  It may be used for any iterative looping that has a known exit condition.  An example follows which fills an array with sequentially increasing numbers:
\par 
\par \f2 int i;
\par int myArray[10];
\par for (i=0; i < sizeof(myArray)/sizeof(myArray[0]); i++)
\par \{
\par \tab myArray[i] = i;
\par \}
\par 
\par \f0 In the for statement three \cf2\strike expressions\cf3\strike0\{linkID=80\}\cf0  are listed.  The first expression is a comma separated list of values to initialize.  In this case we are initializing the single variable i to 0, which is the first index in an array.
\par 
\par The second expression is the selecting expression.  As long as it evaluates to true, nonzero or non null the loop will keep executing.
\par 
\par The third expression is a comma separated list of expressions to evaluate at the end of the loop but before the selecting expression is evaluated.  In this case, we are simply incrementing the index into the array.
\par 
\par In the 1999 standard, the first expression may also serve as a declaration.  For example the following \f1 increments i and decrements j until j is less than i, and fills each array element with a value.
\par \f0 
\par \f2 int myArray[10];
\par for (int i=0, j= 10; i <= j; i++, j--)
\par \{
\par \tab myArray[i] = (i + j)/2;
\par \tab myArray[j] = myArray[i];
\par \}
\par 
\par \f0 Any of the three expressions in the for loop may be omitted.  If the second expression is omitted, the loop has no explicit way to exit.  Sometimes infinite or never ending loops are written as:
\par 
\par \f2 for ( ; ; )
\par \{
\par \tab do something here
\par \}\f1 
\par }
260
Scribble260
Do-While Statement
do-while



Writing



FALSE
14
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Do-While Statement\cf0\b0\f1\fs20 
\par 
\par \f0 The do while statement repeatedly executes a loop.  After each execution of the loop, the selecting \cf2\strike expression\cf3\strike0\{linkID=80\}\cf0  is checked.  As long as it evaluates to true, nonzero or non null the loop will keep executing.
\par 
\par \f2 do
\par \{
\par \tab BlinkChristmasLights();
\par \} while (!IsSunrise());
\par 
\par \f0 which means, start by blinking the Christmas lights, and continue until it is sunrise.
\par \f1 
\par }
270
Scribble270
While Statement
while



Writing



FALSE
13
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 While Statement\cf0\b0\f1\fs20 
\par 
\par \f0 The while statement checks the selecting \cf2\strike expression\cf3\strike0\{linkID=80\}\cf0 .  If it is false, nonzero, or non null, the loop isn't executed.  Otherwise the loop is executed, and at the beginning of each execution of the loop the exit condition is checked again.  For example:
\par 
\par \f2 while (IsDark())
\par \{
\par \tab BlinkChristmasLights();
\par \}
\par 
\par \f0 This checks to see if it is dark, and if so the Christmas lights blink until it is not dark any more.\f1 
\par }
280
Scribble280
Goto Statement
goto



Writing



FALSE
35
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\fs32 Goto Statement\cf0\b0\f1\fs20 
\par 
\par \f0 The goto statement transfers control to a specific point in the function.  For example:
\par 
\par 
\par \f2 while (IsDark())
\par \{
\par \tab if (PowerFail())
\par \tab\tab goto exit;
\par \tab BlinkChristmasLights();
\par \}
\par exit:
\par 
\par \f0 where \f2 exit\f0  is a label control is to be transferred to.  When a power fail occurs, the loop is exited.  
\par 
\par Goto statements don't have to be used with loops, they can be placed anywhere inside a function.
\par 
\par In many cases the goto statement can be avoided with judicious structuring of other control statements.  In this particular case there are two alternate ways of writing the statement:
\par 
\par \f2 while (IsDark() && !PowerFail())
\par \{
\par \tab BlinkChristmasLights();
\par \}
\par 
\par \f0 or 
\par 
\par \f2 while (IsDark())
\par \{
\par \tab if (PowerFail())
\par \tab\tab break;
\par \tab BlinkChristmasLights();
\par \}\f1 
\par }
290
Scribble290
Break Statement
break



Writing



FALSE
16
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\fs32 Break Statement\cf0\b0\f1\fs20 
\par 
\par \f0 The break statement is used to transfer control out of a loop.  For example:
\par \f1 
\par \f2 while (IsDark())
\par \{
\par \tab if (PowerFail())
\par \tab\tab break;
\par \tab BlinkChristmasLights();
\par \}
\par \f0 
\par When a power fail occurs, the loop exits and there are no more attempts to blink the lights.\f1 
\par 
\par }
300
Scribble300
Continue Statement
continue



Writing



FALSE
25
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Continue Statement\cf0\b0\f1\fs20 
\par 
\par \f0 The continue statement is used to transfer control back to the selecting \cf2\strike expression\cf3\strike0\{linkID=80\}\cf0  of a loop.  It may be used to ignore the following statements in a loop and restart execution of the loop.  In the case of for loops, the continue statement also executes the third expression before checking the selecting expression.
\par \f1 
\par \f0 The following stays in the loop if there is a power failure, but doesn't try to blink the lights during a power fail.  However it resumes blinking the lights when the power comes back - as long as the loop hasn't exited from a lack of darkness.
\par 
\par \f1 
\par \f2 while (IsDark())
\par \{
\par \tab if (PowerFail())
\par \tab\tab continue;
\par \tab BlinkChristmasLights();
\par \}
\par \f0 
\par The following prints an index value if a condition is true:
\par 
\par \f2 for (i=0; i < 10; i++)
\par \{
\par \tab if (!Exists(i))
\par \tab\tab continue;
\par \tab printf("%d\\n",i);
\par \}\f0 
\par }
310
Scribble310
Return Statement
return



Writing



FALSE
20
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Return Statement\cf0\b0\f1\fs20 
\par 
\par \f0 The return statement is used to exit a function.  If the function was declared to return a value, the return statement also specifies the value to return.  The returned value may be any \cf2\strike expression\cf3\strike0\{linkID=80\}\cf0  with a \cf2\strike type\cf3\strike0\{linkID=50\}\cf0  that is compatible with the return type found in the function prototype.  If the function is prototyped with a return value of \f2 void\f0 , the return statement may not return a value.
\par 
\par For example, the following returns the number of flowers from 0 to 20, or -1 if there are more than 20 flowers.\f1 
\par \f0 
\par \f2 int GetNumberOfFlowers(struct _flower * flowers)
\par \{
\par \tab int count = 0;
\par \tab for ( ; flowers; flowers = flowers->next)
\par \tab\{
\par \tab\tab if (count > 20)
\par \tab\tab\tab return -1;
\par \tab\tab count++;
\par \tab\}
\par \tab return count;
\par \}\f0 
\par }
320
Scribble320
Operators
operators



Writing



FALSE
22
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Operators\cf0\b0\f1\fs20 
\par 
\par \f0 Operators are built-in functions that take one or two arguments, perform an operation on the arguments, and return a result.  \cf2\strike Arithmetic operators\cf3\strike0\{linkID=340\}\cf0  perform basic math functions and work with integer and floating point values.   \cf2\strike Bit wise operators\cf3\strike0\{linkID=350\}\cf0  work with Bit wise logical data.  \cf2\strike Logical operators\cf3\strike0\{linkID=360\}\cf0  work with boolean values.  \cf2\strike Shift operators\cf3\strike0\{linkID=370\}\cf0  work with powers of two. \cf2\strike Assignment operators\cf3\strike0\{linkID=380\}\cf0  are used for assigning results to variables.  \cf2\strike Equality operators\cf3\strike0\{linkID=390\}\cf0  test whether the values of two variables are equal.  \cf2\strike Relational operators\cf3\strike0\{linkID=400\}\cf0  are used to compare the relative values of two quantities.  \cf2\strike Pointer operators\cf3\strike0\{linkID=410\}\cf0  are used for assigning and accessing the values that pointers reference.  \cf2\strike Member operators\cf3\strike0\{linkID=420\}\cf0  allow access to the members of structures and unions.  \cf2\strike Auto increment operators\cf3\strike0\{linkID=430\}\cf0  is used to add or subtract one from a value.  The \cf2\strike hook operator\cf3\strike0\{linkID=440\}\cf0  is used to embed the equivalent of a simple \cf2\strike if statement\cf3\strike0\{linkID=230\}\cf0  inside an expression.  The \cf2\strike comma operator \cf3\strike0\{linkID=450\}\cf0  is used to evaluate several expressions sequentially.  The \cf2\strike sizeof operator\cf3\strike0\{linkID=165\}\cf0  is used for determining the size of an entity.  Finally, \cf2\strike\f1 function calls\cf3\strike0\{linkID=460\}\cf0\f0  are used to evaluate more complex quantities.
\par 
\par a simple expression which assigns a value to a variable is:
\par 
\par \f2 a = b + c;
\par 
\par \f0 Operators may be aggregated into larger expressions, for example:
\par 
\par \f2 a = b + c * d;
\par 
\par \f0 The order in which operators are evaluated is determined by the \cf2\strike operator precedence\cf3\strike0\{linkID=430\}\cf0 ; in the above example the multiplication occurs before the addition.  The precedence may be modified or clarified with parenthesis.  The following example causes the addition to occur before the multiplication:
\par 
\par \f2 a = (b + c) * d;
\par 
\par \f0 The operands to any given operator may themselves be larger expressions:
\par \f2 
\par a= (b + c + d) * (e + f + g);\f0 
\par }
330
Scribble330
Type Casting
casting; type casting



Writing



FALSE
75
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Type Casting\cf0\b0\f1\fs20 
\par 
\par \f0 Type casting is used to convert an \cf2\strike expression\cf3\strike0\{linkID=80\}\cf0  of one \cf2\strike type\cf3\strike0\{linkID=50\}\cf0  into an expression of another type.  In the 1999 standard, it may also be used as the basis for initialization of structures and arrays.
\par 
\par There are two types of type casting: implicit casts and explicit casts.  Implicit casts are casts done automatically by the compiler.  For example in the following assignment there is an implicit cast from the integer to the floating point variable:
\par 
\par 
\par \f2 float a;
\par int b;
\par a = b;
\par \f0 
\par In the following case if a out of the range that a char can hold, it will automatically be truncated before being stored in b:
\par \f2 char b;
\par int a;
\par 
\par b = a;
\par \f0 
\par Other implicit casts may occur with various operators.  In the following a and b are both cast to int before doing the addition, then cast back to short.  This does make a difference because if the sum of a and b is too large to store in a or b,  we still get an accurate result.  If the casting were not done prior to the operation, the result would be a truncated value.  In general, if an operator takes two operands both are cast either to the size of the larger operand, or to int, whichever is larger.
\par 
\par \f2 unsigned char a,b;
\par short c;
\par 
\par c = a + b;
\par 
\par \f0 The other type of casting is explicit casts.  In this case the program specifically states what type to use.  An explicit type cast is any type specifier, surround by parenthesis.
\par 
\par For example in:
\par 
\par \f2 long double a, b, c;
\par 
\par a = b + c;
\par 
\par \f0 will perform both operations in the long double type.  But in the following the addition will be done in long double, then explicitly converted to float, then implicitly converted back to long double.  Assuming the result wasn't too big to fit in a float, this has the effect of reducing the precision of the result.
\par \f2 
\par a = (float)(b + c);
\par 
\par \f0 Types may also be cast up.  In the following b is explicitly cast to long double, forcing c to be implicitly cast to long double.  The subtraction is then done in the long double type, and the result is implicitly converted back to float before storing it in a;
\par 
\par \f2 float a,b,c;
\par 
\par a = (long double)b - c;
\par 
\par \f0 When the type specifier would normally require an identifier to name a variable, the identifier can be left out:
\par \f2 
\par void (*myFuncPtr)() = (void (*)())NULL;
\par 
\par \f0 Type casts may also be used to convert a pointer of one type to a pointer of another:
\par 
\par \f2 char *ch_ptr;
\par struct _fruit *fruit_ptr;
\par 
\par ch_ptr = (char *)fruit_ptr;
\par \f0 
\par Historically such explicit casting of pointer types hasn't been imperative, although it clarifies the code and removes warnings from the output of some compilers.  It may become more of an issue in a future standard.
\par \f1 
\par \f0 In the 1999 standard, type casting may be used to \cf2\strike initialize\cf3\strike0\{linkID=45\}\cf0  automatic variables that are structures or arrays.  For example:
\par 
\par \f2 struct _car \{
\par \tab int tires;
\par \tab int cylinders;
\par \} ;
\par 
\par struct _car myCar = (struct _car)\{ 4, 4\};
\par 
\par \f0 This follows the same general semantics as other initializers in the 1999 standard.  However, for automatic variables generic expressions may be used for each initialized value.  As a simple example using variables:
\par 
\par \f2 int tires = 4;
\par int cylinders = 6;
\par 
\par struct _car myCar = (struct _car)\{tires, cylinders);
\par 
\par \f0 Array initialization proceed along similar lines.\f1 
\par }
340
Scribble340
Arithmetic Operators
arithmetic operators



Writing



FALSE
36
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\fs32 Arithmetic Operators\cf0\b0\f1\fs20 
\par 
\par \f0 Arithmetic operators form the building blocks for equations by allowing basic mathematical operations.  With the exception of %, which is only defined for integers, they may be used in conjunction with any combination of numeric quantities.  The arithmetic operators are as follows:
\par \f1 
\par \f0 Unary operators\f1 
\par \f0 +\tab takes one operand, does nothing with it other than implicitly converting it to int if required.
\par -\tab takes one operand, returns the result of negating it
\par \f1 
\par \f0 Binary operators\f1 
\par \f0 +\tab takes two operands, returns the result of adding them
\par -\tab takes two operands, returns the result of subtracting the right from the left
\par *\tab takes two operands, returns the result of multiplying them
\par /\tab takes two operands, returns the result of dividing the left by the right
\par 
\par An additional mathematical operator defined for integers is as follows:
\par 
\par %\tab takes two operands, returns the remainder when dividing the left by the right
\par 
\par Before one of these operator is applied, one or both operands may be implicitly cast to the result type.  The result type for these operations is defined as an int, if both operands are less than the size of an int.  Otherwise it is the size of the larger quantity.
\par 
\par These operators would be used as follows:
\par 
\par a * b\tab multiplies two variables and returns the result.
\par 
\par Note that the first two binary operators may also be used with pointers.  Care should be taken here because there is an implicit assumption that such operations will be done in units of the size of the entity the pointer is pointing to.  For example:
\par 
\par int *p = &myVar;;
\par p += 4;
\par *p = 3;
\par 
\par accesses the fifth integer in a linear array of integers that p is pointing to.
\par 
\par Two pointers may be subtracted, for example (p + 4) - (p + 2)  results in the value 2.  This is true no matter what the size of p is, as the result is the number of objects between the two pointers.
\par }
350
Scribble350
Bit wise Operators
bitwise operators



Writing



FALSE
22
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\fs32 Bit wise Operators\cf0\b0\f1\fs20 
\par 
\par \f0 Bit wise operators are used for Bit wise logic functions.  These operators only work with integer quantities.  They are as follows:
\par 
\par Unary operators
\par ~\tab takes one operand, returns Bit wise complement
\par \f1 
\par \f0 Binary operators
\par &\tab takes two operands, returns Bit wise and
\par |\tab takes two operands, returns Bit wise or
\par ^\tab takes two operands, returns Bit wise exclusive or
\par 
\par Before one of these operator is applied, one or both operands may be implicitly cast to the result type.  The result type for these operations is defined as an int, if both operands are less than the size of an int.  Otherwise it is the size of the larger quantity.
\par 
\par for example:
\par 
\par a ^ b
\par 
\par returns the exclusive or of a and b.
\par }
360
Scribble360
Logical Operators
logical operators



Writing



FALSE
26
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\fs32 Logical Operators\cf0\b0\f1\fs20 
\par 
\par \f0 Logical operators have one or two arguments, which may be numeric values, pointers, or booleans:  The logical operation is applied, and the result is either 0 or 1 depending on the operation.
\par 
\par 
\par Unary operators
\par !\tab takes one argument, returns 1 if the value is zero, null, or false, and 0 otherwise
\par 
\par Binary operators
\par &&\tab if one argument is either nonzero, non null, or true and the other argument is either nonzero, non null or true, return true, else return false;
\par ||\tab if one argument is either nonzero, non null, or true or the other argument is either nonzero, non null or true, return true, else return false;
\par \f1 
\par \f0 For example:
\par 
\par \f2 a = bb && cc;
\par 
\par if (quittingTime || lunchTime || tired)
\par \tab Stop();
\par 
\par \f0 Unlike with other operators, the program may not evaluate the right-hand side of binary logical operators.  For example in the following, when the operand \f2 (light == RED)\f0  evaluates as true, the program knows the result of the logical operator || is going to be true and doesn't evaluate \f2 (laps++ > 20)\f0 .  In this case, that will also mean that \f2 laps\f0  does not get incremented.\f1 
\par \f0 
\par \f2 if ((light == RED) || (laps++ > 20))
\par \tab stop();\f0 
\par }
370
Scribble370
Shift Operators
shift operators



Writing



FALSE
20
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\fs32 Shift Operators\cf0\b0\f1\fs20 
\par 
\par \f0 Shift operators multiply or divide an integer by a power of two.  They are:
\par \f1 
\par \f0 Binary operators\f1 
\par \f0 >>\tab takes two arguments, returns the result of dividing the left by the power of two on the right
\par <<\tab takes two arguments, returns the result of multiplying the left by the power of two on the right
\par 
\par The left operand to the shift operator is first converted to an integer, if its size happens to be less than that.  No conversion is performed on the right operand.
\par 
\par They would be used as follows:
\par 
\par \f2 a >> 4; // divide by 16
\par 
\par \f0 Note that some computers differentiate dividing signed quantities by powers of two and dividing unsigned quantities by powers of two.  Other computers do not make such a distinction.  Therefore, using shift operators on extremely large or negative numbers may cause portability issues.
\par 
\par Shift operators are useful for clarifying certain types of algorithms, in hardware register interactions, or in getting data to and from network packets.  However, most modern compilers will take a multiply or divide statement, and turn it into a shift statement when such a statement is warranted and available.  For example,  an x86 compiler could reasonably be expected to take the expression \f2 a * 16 \f0 and turn it into the faster expression \f2 a << 4\f0  as an optimization.\f1 
\par }
380
Scribble380
Assignment Operators
assignment operators



Writing



FALSE
35
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\fs32 Assignment Operators\cf0\b0\f1\fs20 
\par 
\par \f0 A variety of assignment expressions are available.  The simplest simply assigns the value of an expression to a variable.  The others take the left-hand operand, perform an operation on it based on the value of the right-hand operand, and stores the result back in the left hand side. 
\par 
\par =\tab simple assignment.  the left hand side is a quantity to assign to, and the right hand side is some other expression.
\par +=\tab add the right hand side to the left and assign the left the new result
\par -=\tab subtract the right hand side from the left and assign the left the new result
\par *=\tab multiply the left hand side by the right and assign the left the new result
\par /=\tab divide the left hand side by the right and assign the left the new result
\par %=\tab divide the left hand side by the right and assign the left the remainder
\par &=\tab the left hand side gets the result of the Bit wise and of the two operands\f1 
\par \f0 |=\tab the left hand side gets the result of the Bit wise or of the two operands
\par ^=\tab the left hand side gets the result of the Bit wise exclusive or of the two operands
\par >>=\tab the left hand side gets the result of shifting right by the right hand side
\par <<=\tab the left hand side gets the result of shifting left by the right hand side
\par \f1 
\par \f0 An assignment operand performs the same implicit casts as the associated operator: e.g. one or both arguments may be implicitly cast to a higher type before performing the operation.  The result is then cast to the type of the left-hand side and stored.
\par \f1 
\par \f0 Assignments operators return a value.  Then can be cascaded, or used within the context of other expressions.  For example the following stores the result of b*c in both b and a.
\par a = b *= c;
\par 
\par Some care needs to be taken because of the implicit casts that go on, for example the above assignment is really:
\par 
\par a = (b *= c);
\par 
\par which means a would be truncated to the size of b and could lose precision when b cannot hold the entire result, even if the type of a is larger than the type of b.  This is true because an implicit cast is done to the size of type of b.
\par 
\par Note that if two structured variables are of the same type, a simple assignment will copy the members of one to the members of the other:
\par 
\par \f2 struct _fruit apple, orange;
\par 
\par apple = orange;\f0 
\par }
390
Scribble390
Equality Operators
equality operators



Writing



FALSE
20
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\fs32 Equality Operators\cf0\b0\f1\fs20 
\par 
\par \f0 Equality operators test the values of two variables or constants, and return an integer value which is zero or one.
\par 
\par Binary Operators
\par ==\tab tests if two booleans, pointers, or numbers are equal, returns 1 if they are equal
\par !=\tab tests if two booleans, pointers, or numbers are not equal, returns 1 if they are not equal.
\par 
\par When comparing numbers, both sides are first cast to the size of the larger type, or to \f2 int\f0  if both types are smaller than that.
\par 
\par For example:
\par 
\par \f2 if (myCounter == 5)
\par \tab stop();
\par 
\par bKeepGoing = stopLight != RED;
\par \f1 
\par }
400
Scribble400
Relational Operators
relational operators



Writing



FALSE
21
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\fs32 Relational Operators\cf0\b0\f1\fs20 
\par 
\par \f0 Relational operators are used to compare the relative values of two numbers or pointers.  They return an integer value of 1 if the relationship is true, or 0 if not.
\par 
\par Binary operators\f1 
\par \f0 >\tab tests if the left is greater than the right
\par <\tab tests if the left is less than the right
\par >=\tab tests if the left is greater than or equal to the right
\par <=\tab tests if the left is less than or equal to the right
\par 
\par When comparing numbers, both sides are first cast to the size of the larger type, or to \f2 int\f0  if both types are smaller than that.
\par 
\par Comparing pointers is only defined if both pointers point to within the same array object.  Pointers to unlike quantities, to non-array objects, or to different arrays, should not be compared.
\par 
\par For example:
\par 
\par \f2 if (butterflyCounter > NUMBER_OF_BUTTERFLIES_IN_MASSACHUSETTS)
\par \tab printf("we have too many butterflies");
\par }
410
Scribble410
Pointer Operators
pointer operators



Writing



FALSE
38
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\fs32 Pointer Operators\cf0\b0\f1\fs20 
\par 
\par \f0 Pointer operations convert normal variables into pointers, or dereference pointers to allow access to the value being pointed to.
\par \f1 
\par \f0 Unary operations\f1 
\par \f0 &\tab turns a variable into an entity that may be assigned to a pointer
\par *\tab dereferences a pointer, allows access to the value the pointer references
\par []\tab reference an array element.
\par 
\par For example:
\par 
\par \f2 int b;
\par int *b_ptr = &b;
\par 
\par *b_ptr = 4;
\par 
\par \f0 results in the value of 'b' being 4. As another example:
\par 
\par \f2 int array[100];
\par 
\par int *arr_ptr = &array[0];
\par arr_ptr += 17;
\par *arr_ptr = 4;
\par 
\par \f0 has the same result as:
\par 
\par \f2 int array[100];
\par array[17] = 4;
\par 
\par \f1 which also has the same result as:
\par \f2 
\par int array[100];
\par int *arr_ptr = &array[0];
\par arr_ptr[17] = 4;
\par \f1 
\par }
420
Scribble420
Member Operators
member operators



Writing



FALSE
35
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\fs32 Member Operators\cf0\b0\f1\fs20 
\par 
\par The member operators\f0  are used to access a member of a structure or union;\f1 
\par 
\par .\tab\f0 access a member of a structured variable\f1 
\par ->\tab\f0 access a member of a structure which is pointed to\f2 
\par 
\par \f1 For example:
\par 
\par \f2 struct _fruit
\par \{
\par \tab enum _type \{ apple, orange\} type;
\par \} *myFruit ;
\par 
\par myFruit = malloc(sizeof(struct _fruit));
\par 
\par myFruit->type = apple;
\par 
\par \f1 sets the type of myFruit to be \f2 apple\f1 .\f0 
\par \f1 
\par \f0 on the other hand:
\par 
\par \f2 struct _fruit myOrange;
\par myOrange.type = orange;
\par 
\par \f0 sets the type of myOrange to be an orange.
\par 
\par When structures are nested, such operators may be cascaded and combined with other operators.  For example:
\par 
\par \f2 car->tires[1].hubcap->color
\par 
\par \f0 is a valid expression which goes through three structures and an array to get to the final data field.\f1 
\par }
430
Scribble430
Autoincrement Operators
autoincrement; autodecrement



Writing



FALSE
31
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;}
\viewkind4\uc1\pard\cf1\b\fs32 Auto increment Operators\cf0\b0\f1\fs20 
\par 
\par \f0 The Auto increment functions take a single argument, which may be a number or pointer..  They are as follows:
\par 
\par Unary operators:
\par ++\tab add one
\par --\tab subtract one
\par 
\par These operators simply adds or subtracts one from the argument, and stores the result back into the argument.  In the case of pointers, the addition or subtraction is based on the size of an object.  The following accesses the second integer in an array of integers p is pointing to.
\par 
\par \f2 int *p= &b, value;
\par p++;
\par value = *p;
\par 
\par \f0 Auto increment functions return a value.  Depending on whether the operator is placed before or after the variable, the value returned can be the number prior to the operation or after it.
\par 
\par For example:
\par 
\par \f2 int a, b , c = 7;
\par a = c++;\tab\tab //a = 7, c = 8;
\par c = 7;
\par b = ++c;\tab\tab //b = 8, c = 8;
\par 
\par \f1 Note that \f0 the point at which the ++ or -- is evaluated is somewhat ambiguous.  It is all right for a compiler to perform the operation at almost any point during the evaluation of an expression.  For example:
\par 
\par \f1 a = i++ + i;
\par 
\par \f0 has an undefined result because there the point at which the ++ is evaluated is undefined.\f1 
\par }
440
Scribble440
Hook Operator
hook operator



Writing



FALSE
19
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Hook Operator\cf0\b0\f1\fs20 
\par 
\par \f0 The hook operation is a simple way to perform any \cf2\strike if statement\cf3\strike0\{linkID=230\}\cf0  that looks similar to this:
\par 
\par \f2 if (snowIsMuddy)
\par \tab myVariable = a + b;
\par else
\par \tab myVariable = a - b;
\par 
\par \f0 This may be written as an expression using a hook operator:
\par 
\par \f2 myVariable = snowIsMuddy ? a + b : a - b;
\par 
\par \f0 where the ? : characters separate the condition from the if and else clauses.  Note that unlike a normal if statement, the else close is mandatory.
\par 
\par \f1 
\par }
450
Scribble450
Comma Operator
comma operator



Writing



FALSE
21
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Comma Operator\cf0\b0\f1\fs20 
\par 
\par \f0 The comma operator may be used to cascade multiple \cf2\strike expressions\cf3\strike0\{linkID=230\}\cf0  for sequential evaluation.  The comma operator isn't available in function arguments or declarations, but may otherwise be used in expressions.  For example:
\par 
\par \f2 if (toIncrement)
\par \tab counter++,pointer++;
\par 
\par \f0 T\f1 he comma operator is often used in the select\f0 ing expressions for\f1  complex for loops, for example:
\par 
\par \f2 for (i=0,j=20; i < 20; i++, j--)
\par \{
\par \}
\par 
\par \f1 The comma operator does return a value.  Its value is the value of the left hand most expression.  For example:
\par 
\par \f2 myInteger = a++, b++;
\par 
\par \f1 assigns the value of a to myInteger, prior to the increment.
\par }
460
Scribble460
Function Calls
function calls



Writing



FALSE
31
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fnil\fcharset0 Arial;}{\f1\fnil Arial;}{\f2\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green0\blue255;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\cf1\b\fs32 Function Calls\cf0\b0\f1\fs20 
\par 
\par \f0 Function calls are used to invoke a previously defined \cf2\strike function\cf3\strike0\{linkID=45\}\cf0 .  A function is given arguments, the operations in the function are performed, and a result may be returned.  As an example with the function:
\par 
\par \f2 long double addFloatToInt(int a, long double b)
\par \{
\par \tab return a + b;
\par \}
\par 
\par \f0 we could write:
\par 
\par \f2 short shortValue;
\par float floatingValue;
\par long double result = addFloatToInt(shortValue, floatingValue);
\par 
\par \f0 In this case we are also taking advantage of implicit type conversions that occur when evaluating the arguments.  We could also write:
\par 
\par \f2 int a,b;
\par 
\par result = addFloatToInt(a+b, floatingValue);
\par 
\par \f0 e.g. any argument can be a full-fledged expression, as long as that expression does not have a comma operator in it.
\par 
\par If we want to call a function but it doesn't return a value, or we simply want to ignore the value it returned, we could write:
\par 
\par \f2 addFloatToInt(a+b, floatingValue);
\par 
\par \f0 This particular function doesn't do anything useful when we do that, however.\f1 
\par }
0
0
0
57
1 Introduction=Scribble1
1 Preprocessor
2 Preprocessor=Scribble20
2 Comments=Scribble140
2 Trigraphs=Scribble155
2 Line Concatenation=Scribble150
2 Macro Expansion=Scribble90
2 Conditional compilation=Scribble100
2 Include Files=Scribble130
2 Preprocessor Diagnostic Directives=Scribble120
2 Pragma Statements=Scribble110
1 Compiler
2 Declarations=Scribble30
2 Variables=Scribble40
2 Initial Value=Scribble45
2 Constants
3 Constants=Scribble190
3 String Constants=Scribble195
2 Types
3 Types=Scribble50
3 Type Aggregation=Scribble170
3 Typedef Keyword=Scribble180
2 Functions
3 Functions=Scribble60
3 main=Scribble200
3 Statements
4 Statements=Scribble70
4 Control Statements=Scribble210
4 If Statement=Scribble230
4 Switch Statement=Scribble240
4 For Statement=Scribble250
4 Do-While Statement=Scribble260
4 While Statement=Scribble270
4 Goto Statement=Scribble280
4 Break Statement=Scribble290
4 Continue Statement=Scribble300
4 Return Statement=Scribble310
3 Expressions
4 Expressions=Scribble80
4 Type Casting=Scribble330
4 Operators
5 Operators=Scribble320
5 sizeof operator=Scribble165
5 Operator Precedence=Scribble160
5 Arithmetic Operators=Scribble340
5 Bitwise Operators=Scribble350
5 Logical Operators=Scribble360
5 Shift Operators=Scribble370
5 Assignment Operators=Scribble380
5 Equality Operators=Scribble390
5 Relational Operators=Scribble400
5 Pointer Operators=Scribble410
5 Member Operators=Scribble420
5 Autoincrement Operators=Scribble430
5 Hook Operator=Scribble440
5 Comma Operator=Scribble450
5 Function Calls=Scribble460
7
*InternetLink
16711680
Courier New
0
10
1
....
0
0
0
0
0
0
*ParagraphTitle
-16777208
Arial
0
11
1
B...
0
0
0
0
0
0
*PopupLink
-16777208
Arial
0
8
1
....
0
0
0
0
0
0
*PopupTopicTitle
16711680
Arial
0
10
1
B...
0
0
0
0
0
0
*TopicText
-16777208
Arial
0
10
1
....
0
0
0
0
0
0
*TopicTitle
16711680
Arial
0
16
1
B...
0
0
0
0
0
0
example
-16777208
Courier New
0
10
0
....
0
0
0
0
0
0
