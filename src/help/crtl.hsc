HelpScribble project file.
16
YNQ`bsg-113363
0
2



GNU C Help
TRUE

bitmaps
1
BrowseButtons()
0
FALSE

FALSE
TRUE
16777215
0
32768
8388736
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

243
10
PAGE_286
The C Library




Imported



FALSE
427
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fswiss\fcharset0 Arial;}{\f2\fmodern\fcharset0 Courier New;}{\f3\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 The C Library\{keepn\} 
\par \pard\b\fs34 The C Library\b0\fs18 
\par 
\par Next: \cf1\strike Introduction\strike0\{linkID=1210\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Main Menu\b0 
\par 
\par This \f1 help file is based on \f0 Edition 0.10, last updated 2001-07-06, of\i  The GNU C Library Reference Manual\i0 , for Version 2.2.x of the GNU C Library.
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Introduction\strike0\{linkID=1210\}\cf0 : Purpose of the C Library.
\par \f2\'b7\f0  \cf1\strike Error Reporting\strike0\{linkID=690\}\cf0 : How library functions report errors.
\par \f2\'b7\f0  \cf1\strike Memory\strike0\{linkID=1440\}\cf0 : Allocating virtual memory and controlling paging.
\par \f2\'b7\f0  \cf1\strike Character Handling\strike0\{linkID=270\}\cf0 : Character testing and conversion functions.
\par \f2\'b7\f0  \cf1\strike String and Array Utilities\strike0\{linkID=2160\}\cf0 : Utilities for copying and comparing strings and arrays.
\par \f2\'b7\f0  \cf1\strike Character Set Handling\strike0\{linkID=290\}\cf0 : Support for extended character sets.
\par \f2\'b7\f0  \cf1\strike Locales\strike0\{linkID=1340\}\cf0 : The country and language can affect the behavior of library functions.
\par \f2\'b7\f0  \cf1\strike Searching and Sorting\strike0\{linkID=1930\}\cf0 : General searching and sorting functions.
\par \f2\'b7\f0  \cf1\strike I/O Overview\strike0\{linkID=1270\}\cf0 : Introduction to the I/O facilities.
\par \f2\'b7\f0  \cf1\strike I/O on Streams\strike0\{linkID=1260\}\cf0 : High-level, portable I/O facilities.
\par \f2\'b7\f0  \cf1\strike Low-Level I/O\strike0\{linkID=1360\}\cf0 : Low-level, less portable I/O.
\par \f2\'b7\f0  \cf1\strike File System Interface\strike0\{linkID=820\}\cf0 : Functions for manipulating files.
\par \f2\'b7\f0  \cf1\strike Mathematics\strike0\{linkID=1410\}\cf0 : Math functions, useful constants, random numbers.
\par \f2\'b7\f0  \cf1\strike Arithmetic\strike0\{linkID=100\}\cf0 : Low level arithmetic functions.
\par \f2\'b7\f0  \cf1\strike Date and Time\strike0\{linkID=520\}\cf0 : Functions for getting the date and time and formatting them nicely.
\par \f2\'b7\f0  \cf1\strike Non-Local Exits\strike0\{linkID=1510\}\cf0 : Jumping out of nested function calls.
\par \f2\'b7\f0  \cf1\strike Signal Handling\strike0\{linkID=2010\}\cf0 : How to send, block, and handle signals.
\par \f2\'b7\f0  \cf1\strike Program Basics\strike0\{linkID=1750\}\cf0 : Writing the beginning and end of your program.
\par 
\par \pard Appendices
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Language Features\strike0\{linkID=1310\}\cf0 : C language features provided by the library.
\par \f2\'b7\f0  \cf1\strike Free Manuals\strike0\{linkID=1020\}\cf0 : Free Software Needs Free Documentation.
\par \f2\'b7\f0  \cf1\strike Copying\strike0\{linkID=470\}\cf0 : The GNU Lesser General Public License says how you can copy and share the C Library.
\par \f2\'b7\f0  \cf1\strike Documentation License\strike0\{linkID=590\}\cf0 : This manual is under the GNU Free Documentation License.
\par 
\par \pard Indices
\par 
\par 
\par --- The Detailed Node Listing ---
\par 
\par Introduction
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Getting Started\strike0\{linkID=1060\}\cf0 : What this manual is for and how to use it.
\par \f2\'b7\f0  \cf1\strike Roadmap to the Manual\strike0\{linkID=1880\}\cf0 : Overview of the remaining chapters in this manual.
\par 
\par \pard Standards and Portability
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike ISO C\strike0\{linkID=1230\}\cf0 : The international standard for the C programming language.
\par 
\par \pard Using the Library
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Header Files\strike0\{linkID=1090\}\cf0 : How to include the header files in your programs.
\par \f2\'b7\f0  \cf1\strike Macro Definitions\strike0\{linkID=1370\}\cf0 : Some functions in the library may really be implemented as macros.
\par \f2\'b7\f0  \cf1\strike Reserved Names\strike0\{linkID=1860\}\cf0 : The C standard reserves some names for the library, and some for users.
\par 
\par \pard Error Reporting
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Checking for Errors\strike0\{linkID=310\}\cf0 : How errors are reported by library functions.
\par \f2\'b7\f0  \cf1\strike Error Codes\strike0\{linkID=660\}\cf0 : Error code macros; all of these expand into integer constant values.
\par \f2\'b7\f0  \cf1\strike Error Messages\strike0\{linkID=670\}\cf0 : Mapping error codes onto error messages.
\par 
\par \pard Memory
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Memory Allocation\strike0\{linkID=1430\}\cf0 : Allocating storage for your program data
\par 
\par \pard Memory Allocation
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Memory Allocation and C\strike0\{linkID=1420\}\cf0 : How to get different kinds of allocation in C.
\par \f2\'b7\f0  \cf1\strike Unconstrained Allocation\strike0\{linkID=2340\}\cf0 : The \f3 malloc\f0  facility allows fully general dynamic allocation.
\par \f2\'b7\f0  \cf1\strike Variable Size Automatic\strike0\{linkID=2400\}\cf0 : Allocation of variable-sized blocks of automatic storage that are freed when the calling function returns.
\par 
\par \pard Unconstrained Allocation
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Basic Allocation\strike0\{linkID=170\}\cf0 : Simple use of \f3 malloc\f0 .
\par \f2\'b7\f0  \cf1\strike Malloc Examples\strike0\{linkID=1380\}\cf0 : Examples of \f3 malloc\f0 . \f3 xmalloc\f0 .
\par \f2\'b7\f0  \cf1\strike Freeing after Malloc\strike0\{linkID=1030\}\cf0 : Use \f3 free\f0  to free a block you got with \f3 malloc\f0 .
\par \f2\'b7\f0  \cf1\strike Changing Block Size\strike0\{linkID=260\}\cf0 : Use \f3 realloc\f0  to make a block bigger or smaller.
\par \f2\'b7\f0  \cf1\strike Allocating Cleared Space\strike0\{linkID=70\}\cf0 : Use \f3 calloc\f0  to allocate a block and clear it.
\par \pard\f2 
\par \f0 Variable Size Automatic
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Alloca Example\strike0\{linkID=60\}\cf0 : Example of using \f3 alloca\f0 .
\par \f2\'b7\f0  \cf1\strike Advantages of Alloca\strike0\{linkID=50\}\cf0 : Reasons to use \f3 alloca\f0 .
\par \f2\'b7\f0  \cf1\strike Disadvantages of Alloca\strike0\{linkID=580\}\cf0 : Reasons to avoid \f3 alloca\f0 .
\par \f2\'b7\f0  \cf1\strike Variable-Size Arrays\strike0\{linkID=1070\}\cf0 : \f1 H\f0 ere is an alternative method of allocating dynamically and freeing automatically.
\par 
\par \pard Character Handling
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Classification of Characters\strike0\{linkID=330\}\cf0 : Testing whether characters are letters, digits, punctuation, etc.
\par \f2\'b7\f0  \cf1\strike Case Conversion\strike0\{linkID=250\}\cf0 : Case mapping, and the like.
\par \f2\'b7\f0  \cf1\strike Classification of Wide Characters\strike0\{linkID=340\}\cf0 : Character class determination for wide characters.
\par \f2\'b7\f0  \cf1\strike Using Wide Char Classes\strike0\{linkID=2370\}\cf0 : Notes on using the wide character classes.
\par \f2\'b7\f0  \cf1\strike Wide Character Case Conversion\strike0\{linkID=2450\}\cf0 : Mapping of wide characters.
\par 
\par \pard String and Array Utilities
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Representation of Strings\strike0\{linkID=1850\}\cf0 : Introduction to basic concepts.
\par \f2\'b7\f0  \cf1\strike String/Array Conventions\strike0\{linkID=2200\}\cf0 : Whether to use a string function or an arbitrary array function.
\par \f2\'b7\f0  \cf1\strike String Length\strike0\{linkID=2180\}\cf0 : Determining the length of a string.
\par \f2\'b7\f0  \cf1\strike Copying and Concatenation\strike0\{linkID=460\}\cf0 : Functions to copy the contents of strings and arrays.
\par \f2\'b7\f0  \cf1\strike String/Array Comparison\strike0\{linkID=2190\}\cf0 : Functions for byte-wise and character-wise comparison.
\par \f2\'b7\f0  \cf1\strike Collation Functions\strike0\{linkID=370\}\cf0 : Functions for collating strings.
\par \f2\'b7\f0  \cf1\strike Search Functions\strike0\{linkID=1920\}\cf0 : Searching for a specific element or substring.
\par \f2\'b7\f0  \cf1\strike Finding Tokens in a String\strike0\{linkID=830\}\cf0 : Splitting a string into tokens by looking for delimiters.
\par 
\par \pard Character Set Handling
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Extended Char Intro\strike0\{linkID=730\}\cf0 : Introduction to Extended Characters.
\par \f2\'b7\f0  \cf1\strike Charset Function Overview\strike0\{linkID=300\}\cf0 : Overview about Character Handling Functions.
\par \f2\'b7\f0  \cf1\strike Restartable multibyte conversion\strike0\{linkID=1870\}\cf0 : Restartable multibyte conversion Functions.
\par \f2\'b7\f0  \cf1\strike Non-reentrant Conversion\strike0\{linkID=1540\}\cf0 : Non-reentrant Conversion Function.
\par 
\par \pard Restartable multibyte conversion
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Selecting the Conversion\strike0\{linkID=1950\}\cf0 : Selecting the conversion and its properties.
\par \f2\'b7\f0  \cf1\strike Keeping the state\strike0\{linkID=1290\}\cf0 : Representing the state of the conversion.
\par \f2\'b7\f0  \cf1\strike Converting a Character\strike0\{linkID=440\}\cf0 : Converting Single Characters.
\par \f2\'b7\f0  \cf1\strike Converting Strings\strike0\{linkID=450\}\cf0 : Converting Multibyte and Wide Character Strings.
\par \f2\'b7\f0  \cf1\strike Multibyte Conversion Example\strike0\{linkID=1470\}\cf0 : A Complete Multibyte Conversion Example.
\par 
\par \pard Non-reentrant Conversion
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Non-reentrant Character Conversion\strike0\{linkID=1530\}\cf0 : Non-reentrant Conversion of Single Characters.
\par \f2\'b7\f0  \cf1\strike Shift State\strike0\{linkID=1970\}\cf0 : States in Non-reentrant Functions.
\par 
\par \pard Locales
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Effects of Locale\strike0\{linkID=610\}\cf0 : Actions affected by the choice of locale.
\par \f2\'b7\f0  \cf1\strike Choosing Locale\strike0\{linkID=320\}\cf0 : How the user specifies a locale.
\par \f2\'b7\f0  \cf1\strike Locale Categories\strike0\{linkID=1320\}\cf0 : Different purposes for which you can select a locale.
\par \f2\'b7\f0  \cf1\strike Setting the Locale\strike0\{linkID=1960\}\cf0 : How a program specifies the locale with library functions.
\par \f2\'b7\f0  \cf1\strike Standard Locales\strike0\{linkID=2080\}\cf0 : Locale names available on all systems.
\par \f2\'b7\f0  \cf1\strike Locale Information\strike0\{linkID=1330\}\cf0 : How to access the information for the locale.
\par \f2\'b7\f0  \cf1\strike Yes-or-No Questions\strike0\{linkID=2480\}\cf0 : Check a Response against the locale.
\par 
\par \pard Locale Information
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike The Lame Way to Locale Data\strike0\{linkID=2280\}\cf0 : ISO C's \f3 localeconv\f0 .
\par 
\par \pard The Lame Way to Locale Data
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike General Numeric\strike0\{linkID=1040\}\cf0 : Parameters for formatting numbers and currency amounts.
\par \f2\'b7\f0  \cf1\strike Currency Symbol\strike0\{linkID=500\}\cf0 : How to print the symbol that identifies an amount of money (e.g. `\f3 $\f0 ').
\par \f2\'b7\f0  \cf1\strike Sign of Money Amount\strike0\{linkID=1980\}\cf0 : How to print the (positive or negative) sign for a monetary amount, if one exists.
\par 
\par \pard Searching and Sorting
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Comparison Functions\strike0\{linkID=380\}\cf0 : Defining how to compare two objects. Since the sort and search facilities are general, you have to specify the ordering.
\par \f2\'b7\f0  \cf1\strike Array Search Function\strike0\{linkID=110\}\cf0 : The \f3 bsearch\f0  function.
\par \f2\'b7\f0  \cf1\strike Array Sort Function\strike0\{linkID=120\}\cf0 : The \f3 qsort\f0  function.
\par \f2\'b7\f0  \cf1\strike Search/Sort Example\strike0\{linkID=1940\}\cf0 : An example program.
\par 
\par \pard I/O Overview
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike I/O Concepts\strike0\{linkID=1250\}\cf0 : Some basic information and terminology.
\par \f2\'b7\f0  \cf1\strike File Names\strike0\{linkID=760\}\cf0 : How to refer to a file.
\par 
\par \pard I/O Concepts
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Streams and File Descriptors\strike0\{linkID=2130\}\cf0 : The GNU Library provides two ways to access the contents of files.
\par \f2\'b7\f0  \cf1\strike File Position\strike0\{linkID=780\}\cf0 : The number of bytes from the beginning of the file.
\par 
\par \pard File Names
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Directories\strike0\{linkID=570\}\cf0 : Directories contain entries for files.
\par \f2\'b7\f0  \cf1\strike File Name Resolution\strike0\{linkID=750\}\cf0 : A file name specifies how to look up a file.
\par \f2\'b7\f0  \cf1\strike File Name Errors\strike0\{linkID=740\}\cf0 : Error conditions relating to file names.
\par 
\par \pard I/O on Streams
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Streams\strike0\{linkID=2150\}\cf0 : About the data type representing a stream.
\par \f2\'b7\f0  \cf1\strike Standard Streams\strike0\{linkID=2100\}\cf0 : Streams to the standard input and output devices are created for you.
\par \f2\'b7\f0  \cf1\strike Opening Streams\strike0\{linkID=1610\}\cf0 : How to create a stream to talk to a file.
\par \f2\'b7\f0  \cf1\strike Closing Streams\strike0\{linkID=360\}\cf0 : Close a stream when you are finished with it.
\par \f2\'b7\f0  \cf1\strike Streams and I18N\strike0\{linkID=2140\}\cf0 : Streams in internationalized applications.
\par \f2\'b7\f0  \cf1\strike Simple Output\strike0\{linkID=2050\}\cf0 : Unformatted output by characters and lines.
\par \f2\'b7\f0  \cf1\strike Character Input\strike0\{linkID=280\}\cf0 : Unformatted input by characters and words.
\par \f2\'b7\f0  \cf1\strike Line Input\strike0\{link=PAGE_329\}\cf0 : Reading a line or a record from a stream.
\par \f2\'b7\f0  \cf1\strike Unreading\strike0\{linkID=2360\}\cf0 : Peeking ahead/pushing back input just read.
\par \f2\'b7\f0  \cf1\strike Block Input/Output\strike0\{linkID=200\}\cf0 : Input and output operations on blocks of data.
\par \f2\'b7\f0  \cf1\strike Formatted Output\strike0\{linkID=970\}\cf0 : \f3 printf\f0  and related functions.
\par \f2\'b7\f0  \cf1\strike Formatted Input\strike0\{linkID=940\}\cf0 : \f3 scanf\f0  and related functions.
\par \f2\'b7\f0  \cf1\strike EOF and Errors\strike0\{linkID=650\}\cf0 : How you can tell if an I/O error happens.
\par \f2\'b7\f0  \cf1\strike Error Recovery\strike0\{linkID=680\}\cf0 : What you can do about errors.
\par \f2\'b7\f0  \cf1\strike Binary Streams\strike0\{linkID=190\}\cf0 : Some systems distinguish between text files and binary files.
\par \f2\'b7\f0  \cf1\strike File Positioning\strike0\{linkID=790\}\cf0 : About random-access streams.
\par \f2\'b7\f0  \cf1\strike Portable Positioning\strike0\{linkID=1730\}\cf0 : Random access on peculiar ISO C systems.
\par \f2\'b7\f0  \cf1\strike Stream Buffering\strike0\{linkID=2120\}\cf0 : How to control buffering of streams.
\par 
\par \pard Unreading
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Unreading Idea\strike0\{linkID=2350\}\cf0 : An explanation of unreading with pictures.
\par \f2\'b7\f0  \cf1\strike How Unread\strike0\{linkID=1110\}\cf0 : How to call \f3 ungetc\f0  to do unreading.
\par 
\par \pard Formatted Output
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Formatted Output Basics\strike0\{linkID=950\}\cf0 : Some examples to get you started.
\par \f2\'b7\f0  \cf1\strike Output Conversion Syntax\strike0\{linkID=1680\}\cf0 : General syntax of conversion specifications.
\par \f2\'b7\f0  \cf1\strike Table of Output Conversions\strike0\{linkID=2240\}\cf0 : Summary of output conversions and what they do.
\par \f2\'b7\f0  \cf1\strike Integer Conversions\strike0\{linkID=1180\}\cf0 : Details about formatting of integers.
\par \f2\'b7\f0  \cf1\strike Floating-Point Conversions\strike0\{linkID=900\}\cf0 : Details about formatting of floating-point numbers.
\par \f2\'b7\f0  \cf1\strike Other Output Conversions\strike0\{linkID=1670\}\cf0 : Details about formatting of strings, characters, pointers, and the like.
\par \f2\'b7\f0  \cf1\strike Formatted Output Functions\strike0\{linkID=960\}\cf0 : Descriptions of the actual functions.
\par \f2\'b7\f0  \cf1\strike Variable Arguments Output\strike0\{linkID=2390\}\cf0 : \f3 vprintf\f0  and friends.
\par 
\par \pard Formatted Input
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Formatted Input Basics\strike0\{linkID=920\}\cf0 : Some basics to get you started.
\par \f2\'b7\f0  \cf1\strike Input Conversion Syntax\strike0\{linkID=1170\}\cf0 : Syntax of conversion specifications.
\par \f2\'b7\f0  \cf1\strike Table of Input Conversions\strike0\{linkID=2230\}\cf0 : Summary of input conversions and what they do.
\par \f2\'b7\f0  \cf1\strike Numeric Input Conversions\strike0\{linkID=1590\}\cf0 : Details of conversions for reading numbers.
\par \f2\'b7\f0  \cf1\strike String Input Conversions\strike0\{linkID=2170\}\cf0 : Details of conversions for reading strings.
\par \f2\'b7\f0  \cf1\strike Other Input Conversions\strike0\{linkID=1660\}\cf0 : Details of miscellaneous other conversions.
\par \f2\'b7\f0  \cf1\strike Formatted Input Functions\strike0\{linkID=930\}\cf0 : Descriptions of the actual functions.
\par \f2\'b7\f0  \cf1\strike Variable Arguments Input\strike0\{linkID=2380\}\cf0 : \f3 vscanf\f0  and friends.
\par 
\par \pard Stream Buffering
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Buffering Concepts\strike0\{linkID=220\}\cf0 : Terminology is defined here.
\par \f2\'b7\f0  \cf1\strike Flushing Buffers\strike0\{linkID=910\}\cf0 : How to ensure that output buffers are flushed.
\par \f2\'b7\f0  \cf1\strike Controlling Buffering\strike0\{linkID=430\}\cf0 : How to specify what kind of buffering to use.
\par 
\par \pard Low-Level I/O
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Opening and Closing Files\strike0\{linkID=1600\}\cf0 : How to open and close file descriptors.
\par \f2\'b7\f0  \cf1\strike I/O Primitives\strike0\{linkID=1280\}\cf0 : Reading and writing data.
\par \f2\'b7\f0  \cf1\strike File Position Primitive\strike0\{linkID=770\}\cf0 : Setting a descriptor's file position.
\par \f2\'b7\f0  \cf1\strike Descriptors and Streams\strike0\{linkID=560\}\cf0 : Converting descriptor to stream or vice-versa.
\par \f2\'b7\f0  \cf1\strike Duplicating Descriptors\strike0\{linkID=600\}\cf0 : Fcntl commands for duplicating file descriptors.
\par \f2\'b7\f0  \cf1\strike File Status Flags\strike0\{linkID=810\}\cf0 : Fcntl commands for manipulating flags associated with open files.
\par 
\par \pard File Status Flags
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Access Modes\strike0\{linkID=40\}\cf0 : Whether the descriptor can read or write.
\par \f2\'b7\f0  \cf1\strike Open-time Flags\strike0\{linkID=1620\}\cf0 : Details of \f3 open\f0 .
\par \f2\'b7\f0  \cf1\strike Operating Modes\strike0\{linkID=1630\}\cf0 : Special modes to control I/O operations.
\par 
\par \pard File System Interface
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Working Directory\strike0\{linkID=2470\}\cf0 : This is used to resolve relative file names.
\par \f2\'b7\f0  \cf1\strike Deleting Files\strike0\{linkID=540\}\cf0 : How to delete a file, and what that means.
\par \f2\'b7\f0  \cf1\strike Renaming Files\strike0\{linkID=1840\}\cf0 : Changing a file's name.
\par \f2\'b7\f0  \cf1\strike Creating Directories\strike0\{linkID=490\}\cf0 : A system call just for creating a directory.
\par \f2\'b7\f0  \cf1\strike Temporary Files\strike0\{linkID=2250\}\cf0 : Naming and creating temporary files.
\par 
\par \pard File Attributes
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Attribute Meanings\strike0\{linkID=160\}\cf0 : The names of the file attributes, and what their values mean.
\par \f2\'b7\f0  \cf1\strike Reading Attributes\strike0\{linkID=1800\}\cf0 : How to read the attributes of a file.
\par \f2\'b7\f0  \cf1\strike File Size\strike0\{linkID=800\}\cf0 : Manually changing the size of a file.
\par 
\par \pard Mathematics
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Mathematical Constants\strike0\{linkID=1400\}\cf0 : Precise numeric values for often-used constants.
\par \f2\'b7\f0  \cf1\strike Trig Functions\strike0\{linkID=2320\}\cf0 : Sine, cosine, tangent, and friends.
\par \f2\'b7\f0  \cf1\strike Inverse Trig Functions\strike0\{linkID=1220\}\cf0 : Arcsine, arccosine, etc.
\par \f2\'b7\f0  \cf1\strike Exponents and Logarithms\strike0\{linkID=720\}\cf0 : Also pow and sqrt.
\par \f2\'b7\f0  \cf1\strike Hyperbolic Functions\strike0\{linkID=1130\}\cf0 : sinh, cosh, tanh, etc.
\par \f2\'b7\f0  \cf1\strike Special Functions\strike0\{linkID=2070\}\cf0 : Bessel, gamma, erf.
\par \f2\'b7\f0  \cf1\strike Pseudo-Random Numbers\strike0\{linkID=1780\}\cf0 : Functions for generating pseudo-random numbers.
\par 
\par \pard Pseudo-Random Numbers
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike ISO Random\strike0\{linkID=1240\}\cf0 : \f3 rand\f0  and friends.
\par 
\par \pard Arithmetic
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Integers\strike0\{linkID=1200\}\cf0 : Basic integer types and concepts
\par \f2\'b7\f0  \cf1\strike Integer Division\strike0\{linkID=1190\}\cf0 : Integer division with guaranteed rounding.
\par \f2\'b7\f0  \cf1\strike Floating Point Numbers\strike0\{linkID=870\}\cf0 : Basic concepts. IEEE 754.
\par \f2\'b7\f0  \cf1\strike Floating Point Classes\strike0\{linkID=840\}\cf0 : The five kinds of floating-point number.
\par \f2\'b7\f0  \cf1\strike Floating Point Errors\strike0\{linkID=860\}\cf0 : When something goes wrong in a calculation.
\par \f2\'b7\f0  \cf1\strike Rounding\strike0\{linkID=1900\}\cf0 : Controlling how results are rounded.
\par \f2\'b7\f0  \cf1\strike Control Functions\strike0\{linkID=420\}\cf0 : Saving and restoring the FPU's state.
\par \f2\'b7\f0  \cf1\strike Arithmetic Functions\strike0\{linkID=90\}\cf0 : Fundamental operations provided by the library.
\par \f2\'b7\f0  \cf1\strike Complex Numbers\strike0\{linkID=390\}\cf0 : The types. Writing complex constants.
\par \f2\'b7\f0  \cf1\strike Operations on Complex\strike0\{linkID=1650\}\cf0 : Projection, conjugation, decomposition.
\par \f2\'b7\f0  \cf1\strike Parsing of Numbers\strike0\{linkID=1710\}\cf0 : Converting strings to numbers.
\par 
\par \pard Floating Point Errors
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike FP Exceptions\strike0\{linkID=1010\}\cf0 : IEEE 754 math exceptions and how to detect them.
\par \f2\'b7\f0  \cf1\strike Infinity and NaN\strike0\{linkID=1160\}\cf0 : Special values returned by calculations.
\par \f2\'b7\f0  \cf1\strike Status bit operations\strike0\{linkID=2110\}\cf0 : Checking for exceptions after the fact.
\par \f2\'b7\f0  \cf1\strike Math Error Reporting\strike0\{linkID=1390\}\cf0 : How the math functions report errors.
\par 
\par \pard Arithmetic Functions
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Absolute Value\strike0\{linkID=30\}\cf0 : Absolute values of integers and floats.
\par \f2\'b7\f0  \cf1\strike Normalization Functions\strike0\{linkID=1570\}\cf0 : Extracting exponents and putting them back.
\par \f2\'b7\f0  \cf1\strike Rounding Functions\strike0\{linkID=1890\}\cf0 : Rounding floats to integers.
\par \f2\'b7\f0  \cf1\strike Remainder Functions\strike0\{linkID=1820\}\cf0 : Remainders on division, precisely defined.
\par \f2\'b7\f0  \cf1\strike FP Bit Twiddling\strike0\{linkID=990\}\cf0 : Sign bit adjustment. Adding epsilon.
\par \f2\'b7\f0  \cf1\strike FP Comparison Functions\strike0\{linkID=1000\}\cf0 : Comparisons without risk of exceptions.
\par \f2\'b7\f0  \cf1\strike Misc FP Arithmetic\strike0\{linkID=1450\}\cf0 : Max, min, positive difference, multiply-add.
\par 
\par \pard Parsing of Numbers
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Parsing of Integers\strike0\{linkID=1700\}\cf0 : Functions for conversion of integer values.
\par \f2\'b7\f0  \cf1\strike Parsing of Floats\strike0\{linkID=1690\}\cf0 : Functions for conversion of floating-point values.
\par 
\par \pard Date and Time
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Time Basics\strike0\{linkID=2290\}\cf0 : Concepts and definitions.
\par \f2\'b7\f0  \cf1\strike Elapsed Time\strike0\{linkID=620\}\cf0 : Data types to represent elapsed times
\par \f2\'b7\f0  \cf1\strike CPU Time\strike0\{linkID=480\}\cf0 : The \f3 clock\f0  function.
\par \f2\'b7\f0  \cf1\strike Calendar Time\strike0\{linkID=230\}\cf0 : Manipulation of ``real'' dates and times.
\par \f2\'b7\f0  \cf1\strike Sleeping\strike0\{linkID=2060\}\cf0 : Waiting for a period of time.
\par \pard 
\par Calendar Time
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Simple Calendar Time\strike0\{linkID=2040\}\cf0 : Facilities for manipulating calendar time.
\par \f2\'b7\f0  \cf1\strike Broken-down Time\strike0\{linkID=210\}\cf0 : Facilities for manipulating local time.
\par \f2\'b7\f0  \cf1\strike Formatting Calendar Time\strike0\{linkID=980\}\cf0 : Converting times to strings.
\par \f2\'b7\f0  \cf1\strike TZ Variable\strike0\{linkID=2330\}\cf0 : How users specify the time zone.
\par \f2\'b7\f0  \cf1\strike Time Zone Functions\strike0\{linkID=2310\}\cf0 : Functions to examine or specify the time zone.
\par \f2\'b7\f0  \cf1\strike Time Functions Example\strike0\{linkID=2300\}\cf0 : An example program showing use of some of the time functions.
\par 
\par \pard Non-Local Exits
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Intro\strike0\{linkID=1520\}\cf0 : When and how to use these facilities.
\par \f2\'b7\f0  \cf1\strike Details\strike0\{linkID=1500\}\cf0 : Functions for non-local exits.
\par 
\par \pard Signal Handling
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Concepts of Signals\strike0\{linkID=400\}\cf0 : Introduction to the signal facilities.
\par \f2\'b7\f0  \cf1\strike Standard Signals\strike0\{linkID=2090\}\cf0 : Particular kinds of signals with standard names and meanings.
\par \f2\'b7\f0  \cf1\strike Signal Actions\strike0\{linkID=1990\}\cf0 : Specifying what happens when a particular signal is delivered.
\par \f2\'b7\f0  \cf1\strike Defining Handlers\strike0\{linkID=530\}\cf0 : How to write a signal handler function.
\par \f2\'b7\f0  \cf1\strike Generating Signals\strike0\{linkID=1050\}\cf0 : How to send a signal to a process.
\par 
\par \pard Concepts of Signals
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Kinds of Signals\strike0\{linkID=1300\}\cf0 : Some examples of what can cause a signal.
\par \f2\'b7\f0  \cf1\strike Signal Generation\strike0\{linkID=2000\}\cf0 : Concepts of why and how signals occur.
\par 
\par \pard Standard Signals
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Program Error Signals\strike0\{linkID=1760\}\cf0 : Used to report serious program errors.
\par \pard\f2 
\par \f0 Signal Actions
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Basic Signal Handling\strike0\{linkID=180\}\cf0 : The simple \f3 signal\f0  function.
\par 
\par \pard Defining Handlers
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Handler Returns\strike0\{linkID=1080\}\cf0 : Handlers that return normally, and what this means.
\par \f2\'b7\f0  \cf1\strike Termination in Handler\strike0\{linkID=2260\}\cf0 : How handler functions terminate a program.
\par \f2\'b7\f0  \cf1\strike Longjmp in Handler\strike0\{linkID=1350\}\cf0 : Nonlocal transfer of control out of a signal handler.
\par \f2\'b7\f0  \cf1\strike Signals in Handler\strike0\{linkID=2030\}\cf0 : What happens when signals arrive while the handler is already occupied.
\par \f2\'b7\f0  \cf1\strike Nonreentrancy\strike0\{linkID=1480\}\cf0 : Do not call any functions unless you know they are reentrant with respect to signals.
\par \f2\'b7\f0  \cf1\strike Atomic Data Access\strike0\{linkID=130\}\cf0 : A single handler can run in the middle of reading or writing a single object.
\par 
\par \pard Atomic Data Access
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Non-atomic Example\strike0\{linkID=1490\}\cf0 : A program illustrating interrupted access.
\par \f2\'b7\f0  \cf1\strike Types\strike0\{linkID=140\}\cf0 : Data types that guarantee no interruption.
\par \f2\'b7\f0  \cf1\strike Usage\strike0\{linkID=150\}\cf0 : Proving that interruption is harmless.
\par 
\par \pard Generating Signals
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Signaling Yourself\strike0\{linkID=2020\}\cf0 : A process can send a signal to itself.
\par 
\par \pard Program Basics
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Program Arguments\strike0\{linkID=1740\}\cf0 : Parsing your program's command-line arguments.
\par \f2\'b7\f0  \cf1\strike Environment Variables\strike0\{linkID=640\}\cf0 : Less direct parameters affecting your program
\par \f2\'b7\f0  \cf1\strike Program Termination\strike0\{linkID=1770\}\cf0 : Telling the system you're done; return status
\par 
\par \pard Environment Variables
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Environment Access\strike0\{linkID=630\}\cf0 : How to get and set the values of environment variables.
\par 
\par \pard Program Termination
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Normal Termination\strike0\{linkID=1560\}\cf0 : If a program calls \f3 exit\f0 , a process terminates normally.
\par \f2\'b7\f0  \cf1\strike Exit Status\strike0\{linkID=710\}\cf0 : The \f3 exit status\f0  provides information about why the process terminated.
\par \f2\'b7\f0  \cf1\strike Cleanups on Exit\strike0\{linkID=350\}\cf0 : A process can run its own cleanup functions upon normal termination.
\par \f2\'b7\f0  \cf1\strike Aborting a Program\strike0\{linkID=20\}\cf0 : The \f3 abort\f0  function causes abnormal program termination.
\par 
\par \pard Processes
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Running a Command\strike0\{linkID=1910\}\cf0 : The easy way to run another program.
\par \f2\'b7\f0  \cf1\strike Executing a File\strike0\{linkID=700\}\cf0 : How to make a process execute another program.
\par 
\par \pard Language Features
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Consistency Checking\strike0\{linkID=410\}\cf0 : Using \f3 assert\f0  to abort if something ``impossible'' happens.
\par \f2\'b7\f0  \cf1\strike Variadic Functions\strike0\{linkID=2420\}\cf0 : Defining functions with varying numbers of args.
\par \f2\'b7\f0  \cf1\strike Null Pointer Constant\strike0\{linkID=1580\}\cf0 : The macro \f3 NULL\f0 .
\par \f2\'b7\f0  \cf1\strike Important Data Types\strike0\{linkID=1150\}\cf0 : Data types for object sizes.
\par \f2\'b7\f0  \cf1\strike Data Type Measurements\strike0\{linkID=510\}\cf0 : Parameters of data type representations.
\par 
\par \pard Variadic Functions
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Why Variadic\strike0\{linkID=2440\}\cf0 : Reasons for making functions take variable arguments.
\par \f2\'b7\f0  \cf1\strike How Variadic\strike0\{linkID=1120\}\cf0 : How to define and call variadic functions.
\par \f2\'b7\f0  \cf1\strike Variadic Example\strike0\{linkID=2410\}\cf0 : A complete example.
\par 
\par \pard How Variadic
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Variadic Prototypes\strike0\{linkID=2430\}\cf0 : How to make a prototype for a function with variable arguments.
\par \f2\'b7\f0  \cf1\strike Receiving Arguments\strike0\{linkID=1810\}\cf0 : Steps you must follow to access the optional argument values.
\par \f2\'b7\f0  \cf1\strike How Many Arguments\strike0\{linkID=1100\}\cf0 : How to decide whether there are more arguments.
\par \f2\'b7\f0  \cf1\strike Calling Variadics\strike0\{linkID=240\}\cf0 : Things you need to know about calling variable arguments functions.
\par \f2\'b7\f0  \cf1\strike Argument Macros\strike0\{linkID=80\}\cf0 : Detailed specification of the macros for accessing variable arguments.
\par 
\par \pard Data Type Measurements
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Width of Type\strike0\{linkID=2460\}\cf0 : How many bits does an integer type hold?
\par \f2\'b7\f0  \cf1\strike Range of Type\strike0\{linkID=1790\}\cf0 : What are the largest and smallest values that an integer type can hold?
\par \f2\'b7\f0  \cf1\strike Floating Type Macros\strike0\{linkID=890\}\cf0 : Parameters that measure the floating point types.
\par \f2\'b7\f0  \cf1\strike Structure Measurement\strike0\{linkID=2210\}\cf0 : Getting measurements on structure types.
\par 
\par \pard Floating Type Macros
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Floating Point Concepts\strike0\{linkID=850\}\cf0 : Definitions of terminology.
\par \f2\'b7\f0  \cf1\strike Floating Point Parameters\strike0\{linkID=880\}\cf0 : Details of specific macros.
\par \f2\'b7\f0  \cf1\strike IEEE Floating Point\strike0\{linkID=1140\}\cf0 : The measurements for one common representation.
\par 
\par 
\par }
20
PAGE_0
Aborting a Program
abortingaprogram;stdlib.h;abort



Imported



FALSE
21
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Aborting a Program \{keepn\}
\par \pard\fs18 Previous: \cf1\strike Cleanups on Exit\strike0\{linkID=350\}\cf0 , Up: \cf1\strike Program Termination\strike0\{linkID=1770\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Aborting a Program\b0 
\par 
\par You can abort your program using the \f1 abort\f0  function. The prototype for this function is in \f1 stdlib.h\f0 .
\par 
\par -- Function: void \b abort\b0  (\i void\i0 )
\par 
\par \pard\li500 The \f1 abort\f0  function causes abnormal program termination. This does not execute cleanup functions registered with \f1 atexit\f0 . 
\par 
\par This function actually terminates the process by raising a\f1  SIGABRT\f0  signal, and your program can include a handler to intercept this signal; see \cf1\strike Signal Handling\strike0\{linkID=2010\}\cf0 .
\par 
\par \trowd\cellx9000\pard\intbl\b Future Change Warning:\b0  Proposed Federal censorship regulations may prohibit us from giving you information about the possibility of calling this function. We would be required to say that this is not an acceptable way of terminating a program.\cell\row
\pard 
\par }
30
PAGE_2
Absolute Value
absolutevaluefunctions;hidave;math.h;stdlib.h;abs;labs;llabs;imaxabs;fabs;fabsf;fabsl;cabs;cabsf;cabsl



Imported



FALSE
51
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Absolute Value \{keepn\}
\par \pard\fs18 Next: \cf1\strike Normalization Functions\strike0\{linkID=1570\}\cf0 , Up: \cf1\strike Arithmetic Functions\strike0\{linkID=90\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Absolute Value\b0 
\par 
\par These functions are provided for obtaining the \i absolute value\i0  (or\i  magnitude\i0 ) of a number. The absolute value of a real number\i  x\i0  is \i x\i0  if \i x\i0  is positive, -\i x\i0  if \i x\i0  is negative. For a complex number \i z\i0 , whose real part is \i x\i0  and whose imaginary part is \i y\i0 , the absolute value is \f1 sqrt (\i\f0 x\i0\f1 *\i\f0 x\i0\f1  + \i\f0 y\i0\f1 *\i\f0 y\i0\f1 )\f0 . 
\par 
\par Prototypes for \f1 abs\f0 , \f1 labs\f0  and \f1 llabs\f0  are in \f1 stdlib.h\f0 ;\f1  imaxabs\f0  is declared in \f1 inttypes.h\f0 ;\f1  fabs\f0 , \f1 fabsf\f0  and \f1 fabsl\f0  are declared in \f1 math.h\f0 .\f1  cabs\f0 , \f1 cabsf\f0  and \f1 cabsl\f0  are declared in \f1 complex.h\f0 .
\par 
\par -- Function: int \b abs\b0  (\i int number\i0 )
\par 
\par \pard\li500 -- Function: long int \b labs\b0  (\i long int number\i0 )
\par 
\par \pard\li1000 -- Function: long long int \b llabs\b0  (\i long long int number\i0 )
\par 
\par \pard\li1500 -- Function: intmax_t \b imaxabs\b0  (\i intmax_t number\i0 )
\par 
\par \pard\li2000 These functions return the absolute value of \i number\i0 . 
\par 
\par Most computers use a two's complement integer representation, in which the absolute value of \f1 INT_MIN\f0  (the smallest possible \f1 int\f0 ) cannot be represented; thus, \f1 abs (INT_MIN)\f0  is not defined. 
\par 
\par \f1 llabs\f0  and \f1 imaxdiv\f0  are new to ISO C99. 
\par 
\par See \cf1\strike Integers\strike0\{linkID=1200\}\cf0  for a description of the \f1 intmax_t\f0  type.
\par 
\par \pard -- Function: double \b fabs\b0  (\i double number\i0 )
\par 
\par \pard\li500 -- Function: float \b fabsf\b0  (\i float number\i0 )
\par 
\par \pard\li1000 -- Function: long double \b fabsl\b0  (\i long double number\i0 )
\par 
\par \pard\li1500 This function returns the absolute value of the floating-point number\i  number\i0 .
\par 
\par \pard -- Function: double \b cabs\b0  (\i complex double z\i0 )
\par 
\par \pard\li500 -- Function: float \b cabsf\b0  (\i complex float z\i0 )
\par 
\par \pard\li1000 -- Function: long double \b cabsl\b0  (\i complex long double z\i0 )
\par 
\par \pard\li1500 These functions return the absolute value of the complex number \i z\i0  (see \cf1\strike Complex Numbers\strike0\{linkID=390\}\cf0 ). The absolute value of a complex number is: 
\par 
\par \f1           sqrt (creal (\i z\i0 ) * creal (\i z\i0 ) + cimag (\i z\i0 ) * cimag (\i z\i0 ))
\par      
\par \f0 This function should always be used instead of the direct formula because it takes special care to avoid losing precision. It may also take advantage of hardware support for this operation. See \f1 hypot\f0  in \cf1\strike Exponents and Logarithms\strike0\{linkID=720\}\cf0 .
\par 
\par }
40
PAGE_5
Access Modes
O_RDONLY;O_WRONLY;O_RDWR;O_READ;O_WRITE;O_EXEC;O_ACCMODE



Imported



FALSE
29
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Access Modes \{keepn\}
\par \pard\fs18 Next: \cf1\strike Open-time Flags\strike0\{linkID=1620\}\cf0 , Up: \cf1\strike File Status Flags\strike0\{linkID=810\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b File Access Modes\b0 
\par 
\par The file access modes allow a file descriptor to be used for reading, writing, or both.  The access modes are chosen when the file is opened, and never change.
\par 
\par -- Macro: int \b O_RDONLY\b0 
\par 
\par \pard\li500 Open the file for read access.
\par 
\par \pard -- Macro: int \b O_WRONLY\b0 
\par 
\par \pard\li500 Open the file for write access.
\par 
\par \pard -- Macro: int \b O_RDWR\b0 
\par 
\par \pard\li500 Open the file for both reading and writing.
\par \pard 
\par -- Macro: int \b O_ACCMODE\b0 
\par 
\par \pard\li500 This macro stands for a mask that can be bitwise-ANDed with the file status flag value to produce a value representing the file access mode. The mode will be \f1 O_RDONLY\f0 , \f1 O_WRONLY\f0 , or \f1 O_RDWR\f0 .
\par 
\par }
50
PAGE_15
Advantages of Alloca
longjmp



Imported



FALSE
31
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Advantages of Alloca \{keepn\}
\par \pard\fs18 Next: \cf1\strike Disadvantages of Alloca\strike0\{linkID=580\}\cf0 , Previous: \cf1\strike Alloca Example\strike0\{linkID=60\}\cf0 , Up: \cf1\strike Variable Size Automatic\strike0\{linkID=2400\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Advantages of \f1 alloca\b0\f0 
\par 
\par Here are the reasons why \f1 alloca\f0  may be preferable to \f1 malloc\f0 :
\par 
\par \pard\li500\f2\'b7\f0  Using \f1 alloca\f0  wastes very little space and is very fast. (It is open-coded by the C compiler.) 
\par \f2\'b7\f0  Since \f1 alloca\f0  does not have separate pools for different sizes of block, space used for any size block can be reused for any other size.\f1  alloca\f0  does not cause memory fragmentation. 
\par \f2\'b7\f0  Nonlocal exits done with \f1 longjmp\f0  (see \cf1\strike Non-Local Exits\strike0\{linkID=1510\}\cf0 ) automatically free the space allocated with \f1 alloca\f0  when they exit through the function that called \f1 alloca\f0 . This is the most important reason to use \f1 alloca\f0 . 
\par 
\par To illustrate this, suppose you have a function\f1  open_or_report_error\f0  which returns a descriptor, like\f1  open\f0 , if it succeeds, but does not return to its caller if it fails. If the file cannot be opened, it prints an error message and jumps out to the command level of your program using \f1 longjmp\f0 . Let's change \f1 open2\f0  (see \cf1\strike Alloca Example\strike0\{linkID=60\}\cf0 ) to use this subroutine: 
\par 
\par \f1           int
\par           open2 (char *str1, char *str2, int flags, int mode)
\par           \{
\par             char *name = (char *) alloca (strlen (str1) + strlen (str2) + 1);
\par             stpcpy (stpcpy (name, str1), str2);
\par             return open_or_report_error (name, flags, mode);
\par           \}
\par      
\par \f0 Because of the way \f1 alloca\f0  works, the memory it allocates is freed even when an error occurs, with no special effort required. 
\par 
\par By contrast, the previous definition of \f1 open2\f0  (which uses\f1  malloc\f0  and \f1 free\f0 ) would develop a memory leak if it were changed in this way. Even if you are willing to make more changes to fix it, there is no easy way to do so.
\par 
\par }
60
PAGE_18
Alloca Example




Imported



FALSE
38
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Alloca Example \{keepn\}
\par \pard\fs18 Next: \cf1\strike Advantages of Alloca\strike0\{linkID=50\}\cf0 , Up: \cf1\strike Variable Size Automatic\strike0\{linkID=2400\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b\f1 alloca\f0  Example\b0 
\par 
\par As an example of the use of \f1 alloca\f0 , here is a function that opens a file name made from concatenating two argument strings, and returns a file descriptor or minus one signifying failure:
\par 
\par \f1      int
\par      open2 (char *str1, char *str2, int flags, int mode)
\par      \{
\par        char *name = (char *) alloca (strlen (str1) + strlen (str2) + 1);
\par        stpcpy (stpcpy (name, str1), str2);
\par        return open (name, flags, mode);
\par      \}
\par \f0 
\par Here is how you would get the same results with \f1 malloc\f0  and\f1  free\f0 :
\par 
\par \f1      int
\par      open2 (char *str1, char *str2, int flags, int mode)
\par      \{
\par        char *name = (char *) malloc (strlen (str1) + strlen (str2) + 1);
\par        int desc;
\par        if (name == 0)
\par          fatal ("virtual memory exceeded");
\par        stpcpy (stpcpy (name, str1), str2);
\par        desc = open (name, flags, mode);
\par        free (name);
\par        return desc;
\par      \}
\par \f0 
\par As you can see, it is simpler with \f1 alloca\f0 . But \f1 alloca\f0  has other, more important advantages, and some disadvantages.
\par 
\par }
70
PAGE_19
Allocating Cleared Space
stdlib.h;calloc



Imported



FALSE
30
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Allocating Cleared Space \{keepn\}
\par \pard\fs18 Previous: \cf1\strike Changing Block Size\strike0\{linkID=260\}\cf0 , Up: \cf1\strike Unconstrained Allocation\strike0\{linkID=2340\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Allocating Cleared Space\b0 
\par 
\par The function \f1 calloc\f0  allocates memory and clears it to zero. It is declared in \f1 stdlib.h\f0 .
\par -- Function: void * \b calloc\b0  (\i size_t count, size_t eltsize\i0 )
\par 
\par \pard\li500 This function allocates a block long enough to contain a vector of\i  count\i0  elements, each of size \i eltsize\i0 . Its contents are cleared to zero before \f1 calloc\f0  returns.
\par 
\par \pard You could define \f1 calloc\f0  as follows:
\par 
\par \f1      void *
\par      calloc (size_t count, size_t eltsize)
\par      \{
\par        size_t size = count * eltsize;
\par        void *value = malloc (size);
\par        if (value != 0)
\par          memset (value, 0, size);
\par        return value;
\par      \}
\par \f0 
\par But in general, it is not guaranteed that \f1 calloc\f0  calls\f1  malloc\f0  internally. Therefore, if an application provides its own\f1  malloc\f0 /\f1 realloc\f0 /\f1 free\f0  outside the C library, it should always define \f1 calloc\f0 , too.
\par 
\par }
80
PAGE_44
Argument Macros
stdarg.h;va_list;va_start;va_arg;va_end;gt__va_copy



Imported



FALSE
52
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Argument Macros \{keepn\}
\par \pard\fs18 Previous: \cf1\strike Calling Variadics\strike0\{linkID=240\}\cf0 , Up: \cf1\strike How Variadic\strike0\{linkID=1120\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Argument Access Macros\b0 
\par 
\par Here are descriptions of the macros used to retrieve variable arguments. These macros are defined in the header file \f1 stdarg.h\f0 .
\par 
\par -- Data Type: \b va_list\b0 
\par 
\par \pard\li500 The type \f1 va_list\f0  is used for argument pointer variables.
\par 
\par \pard -- Macro: void \b va_start\b0  (\i va_list ap, last-required\i0 )
\par 
\par \pard\li500 This macro initializes the argument pointer variable \i ap\i0  to point to the first of the optional arguments of the current function;\i  last-required\i0  must be the last required argument to the function. 
\par \pard 
\par -- Macro: \i type\i0  \b va_arg\b0  (\i va_list ap, type\i0 )
\par 
\par \pard\li500 The \f1 va_arg\f0  macro returns the value of the next optional argument, and modifies the value of \i ap\i0  to point to the subsequent argument. Thus, successive uses of \f1 va_arg\f0  return successive optional arguments. 
\par 
\par The type of the value returned by \f1 va_arg\f0  is \i type\i0  as specified in the call. \i type\i0  must be a self-promoting type (not\f1  char\f0  or \f1 short int\f0  or \f1 float\f0 ) that matches the type of the actual argument.
\par 
\par \pard -- Macro: void \b va_end\b0  (\i va_list ap\i0 )
\par 
\par \pard\li500 This ends the use of \i ap\i0 . After a \f1 va_end\f0  call, further\f1  va_arg\f0  calls with the same \i ap\i0  may not work. You should invoke\f1  va_end\f0  before returning from the function in which \f1 va_start\f0  was invoked with the same \i ap\i0  argument. 
\par 
\par In th\f2 is \f0 C library, \f1 va_end\f0  does nothing, and you need not ever use it except for reasons of portability.
\par 
\par \pard Sometimes it is necessary to parse the list of parameters more than once or one wants to remember a certain position in the parameter list. To do this, one will have to make a copy of the current value of the argument. But \f1 va_list\f0  is an opaque type and one cannot necessarily assign the value of one variable of type \f1 va_list\f0  to another variable of the same type.
\par 
\par -- Macro: void \b __va_copy\b0  (\i va_list dest, va_list src\i0 )
\par 
\par \pard\li500 The \f1 va_copy\f0  macro allows copying of objects of type\f1  va_list\f0  even if this is not an integral type. The argument pointer in \i dest\i0  is initialized to point to the same argument as the pointer in \i src\i0 . 
\par \pard 
\par If you want to use \f1 va_copy\f0  you should always be prepared for the possibility that this macro will not be available. On architectures where a simple assignment is invalid, hopefully \f1 va_copy\f0  \i will\i0  be available, so one should always write something like this:
\par 
\par \f1      \{
\par        va_list ap, save;
\par        ...
\par      #ifdef va_copy
\par        va_copy (save, ap);
\par      #else
\par        save = ap;
\par      #endif
\par        ...
\par      \}
\par \f0 
\par }
90
PAGE_48
Arithmetic Functions




Imported



FALSE
21
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Arithmetic Functions \{keepn\}
\par \pard\fs18 Next: \cf1\strike Complex Numbers\strike0\{linkID=390\}\cf0 , Previous: \cf1\strike Control Functions\strike0\{linkID=420\}\cf0 , Up: \cf1\strike Arithmetic\strike0\{linkID=100\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Arithmetic Functions\b0 
\par 
\par The C library provides functions to do basic operations on floating-point numbers. These include absolute value, maximum and minimum, normalization, bit twiddling, rounding, and a few others.
\par 
\par \pard\li500\f1\'b7\f0  \cf1\strike Absolute Value\strike0\{linkID=30\}\cf0 : Absolute values of integers and floats.
\par \f1\'b7\f0  \cf1\strike Normalization Functions\strike0\{linkID=1570\}\cf0 : Extracting exponents and putting them back.
\par \f1\'b7\f0  \cf1\strike Rounding Functions\strike0\{linkID=1890\}\cf0 : Rounding floats to integers.
\par \f1\'b7\f0  \cf1\strike Remainder Functions\strike0\{linkID=1820\}\cf0 : Remainders on division, precisely defined.
\par \f1\'b7\f0  \cf1\strike FP Bit Twiddling\strike0\{linkID=990\}\cf0 : Sign bit adjustment. Adding epsilon.
\par \f1\'b7\f0  \cf1\strike FP Comparison Functions\strike0\{linkID=1000\}\cf0 : Comparisons without risk of exceptions.
\par \f1\'b7\f0  \cf1\strike Misc FP Arithmetic\strike0\{linkID=1450\}\cf0 : Max, min, positive difference, multiply-add.
\par 
\par }
100
PAGE_49
Arithmetic




Imported



FALSE
25
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24  Arithmetic\{keepn\}
\par \pard\fs18 Next: \cf1\strike Date and Time\strike0\{linkID=520\}\cf0 , Previous: \cf1\strike Mathematics\strike0\{linkID=1410\}\cf0 , Up: \cf1\strike Top\strike0\{linkID=10\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Arithmetic\b0 
\par 
\par This chapter contains information about functions for doing basic arithmetic operations, such as splitting a float into its integer and fractional parts or retrieving the imaginary part of a complex value. These functions are declared in the header files \f1 math.h\f0  and\f1  complex.h\f0 .
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Integers\strike0\{linkID=1200\}\cf0 : Basic integer types and concepts
\par \f2\'b7\f0  \cf1\strike Integer Division\strike0\{linkID=1190\}\cf0 : Integer division with guaranteed rounding.
\par \f2\'b7\f0  \cf1\strike Floating Point Numbers\strike0\{linkID=870\}\cf0 : Basic concepts. IEEE 754.
\par \f2\'b7\f0  \cf1\strike Floating Point Classes\strike0\{linkID=840\}\cf0 : The five kinds of floating-point number.
\par \f2\'b7\f0  \cf1\strike Floating Point Errors\strike0\{linkID=860\}\cf0 : When something goes wrong in a calculation.
\par \f2\'b7\f0  \cf1\strike Rounding\strike0\{linkID=1900\}\cf0 : Controlling how results are rounded.
\par \f2\'b7\f0  \cf1\strike Control Functions\strike0\{linkID=420\}\cf0 : Saving and restoring the FPU's state.
\par \f2\'b7\f0  \cf1\strike Arithmetic Functions\strike0\{linkID=90\}\cf0 : Fundamental operations provided by the library.
\par \f2\'b7\f0  \cf1\strike Complex Numbers\strike0\{linkID=390\}\cf0 : The types. Writing complex constants.
\par \f2\'b7\f0  \cf1\strike Operations on Complex\strike0\{linkID=1650\}\cf0 : Projection, conjugation, decomposition.
\par \f2\'b7\f0  \cf1\strike Parsing of Numbers\strike0\{linkID=1710\}\cf0 : Converting strings to numbers.
\par \f2 
\par }
110
PAGE_50
Array Search Function
searchfunction(forarrays;binarysearchfunction(forarrays;arraysearchfunction;lfind;lsearch;stdlib.h;bsearch



Imported



FALSE
37
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Array Search Function \{keepn\}
\par \pard\fs18 Next: \cf1\strike Array Sort Function\strike0\{linkID=120\}\cf0 , Previous: \cf1\strike Comparison Functions\strike0\{linkID=380\}\cf0 , Up: \cf1\strike Searching and Sorting\strike0\{linkID=1930\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Array Search Function\b0 
\par 
\par Generally searching for a specific element in an array means that potentially all elements must be checked. The C library contains functions to perform linear search. The prototypes for the following two functions can be found in \f1 search.h\f0 .
\par -- Function: void * \b lfind\b0  (\i const void *key, void *base, size_t *nmemb, size_t size, comparison_fn_t compar\i0 )
\par 
\par \pard\li500 The \f1 lfind\f0  function searches in the array with \f1 *\i\f0 nmemb\i0  elements of \i size\i0  bytes pointed to by \i base\i0  for an element which matches the one pointed to by \i key\i0 . The function pointed to by \i compar\i0  is used decide whether two elements match. 
\par 
\par The return value is a pointer to the matching element in the array starting at \i base\i0  if it is found. If no matching element is available \f1 NULL\f0  is returned. 
\par 
\par The mean runtime of this function is \f1 *\i\f0 nmemb\i0 /2. This function should only be used elements often get added to or deleted from the array in which case it might not be useful to sort the array before searching.
\par 
\par \pard -- Function: void * \b lsearch\b0  (\i const void *key, void *base, size_t *nmemb, size_t size, comparison_fn_t compar\i0 )
\par 
\par \pard\li500 The \f1 lsearch\f0  function is similar to the \f1 lfind\f0  function. It searches the given array for an element and returns it if found. The difference is that if no matching element is found the \f1 lsearch\f0  function adds the object pointed to by \i key\i0  (with a size of\i  size\i0  bytes) at the end of the array and it increments the value of\f1  *\i\f0 nmemb\i0  to reflect this addition. 
\par 
\par This means for the caller that if it is not sure that the array contains the element one is searching for the memory allocated for the array starting at \i base\i0  must have room for at least \i size\i0  more bytes. If one is sure the element is in the array it is better to use\f1  lfind\f0  so having more room in the array is always necessary when calling \f1 lsearch\f0 .
\par 
\par \pard To search a sorted array for an element matching the key, use the\f1  bsearch\f0  function. The prototype for this function is in the header file \f1 stdlib.h\f0 .
\par -- Function: void * \b bsearch\b0  (\i const void *key, const void *array, size_t count, size_t size, comparison_fn_t compare\i0 )
\par 
\par \pard\li500 The \f1 bsearch\f0  function searches the sorted array \i array\i0  for an object that is equivalent to \i key\i0 . The array contains \i count\i0  elements, each of which is of size \i size\i0  bytes. 
\par 
\par The \i compare\i0  function is used to perform the comparison. This function is called with two pointer arguments and should return an integer less than, equal to, or greater than zero corresponding to whether its first argument is considered less than, equal to, or greater than its second argument. The elements of the \i array\i0  must already be sorted in ascending order according to this comparison function. 
\par 
\par The return value is a pointer to the matching array element, or a null pointer if no match is found. If the array contains more than one element that matches, the one that is returned is unspecified. 
\par 
\par This function derives its name from the fact that it is implemented using the binary search algorithm.
\par 
\par }
120
PAGE_51
Array Sort Function
sortfunction(forarrays;quicksortfunction(forarrays;arraysortfunction;stdlib.h;qsort;stablesorting



Imported



FALSE
35
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Array Sort Functionrray Sort Function \{keepn\}
\par \pard\fs18 Next: \cf1\strike Search/Sort Example\strike0\{linkID=1940\}\cf0 , Previous: \cf1\strike Array Search Function\strike0\{linkID=110\}\cf0 , Up: \cf1\strike Searching and Sorting\strike0\{linkID=1930\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Array Sort Function\b0 
\par 
\par To sort an array using an arbitrary comparison function, use the\f1  qsort\f0  function. The prototype for this function is in\f1  stdlib.h\f0 .
\par -- Function: void \b qsort\b0  (\i void *array, size_t count, size_t size, comparison_fn_t compare\i0 )
\par 
\par \pard\li500 The \i qsort\i0  function sorts the array \i array\i0 . The array contains\i  count\i0  elements, each of which is of size \i size\i0 . 
\par 
\par The \i compare\i0  function is used to perform the comparison on the array elements. This function is called with two pointer arguments and should return an integer less than, equal to, or greater than zero corresponding to whether its first argument is considered less than, equal to, or greater than its second argument. 
\par 
\par \b Warning:\b0  If two objects compare as equal, their order after sorting is unpredictable. That is to say, the sorting is not stable. This can make a difference when the comparison considers only part of the elements. Two elements with the same sort key may differ in other respects. 
\par 
\par If you want the effect of a stable sort, you can get this result by writing the comparison function so that, lacking other reason distinguish between two elements, it compares them by their addresses. Note that doing this may make the sorting algorithm less efficient, so do it only if necessary. 
\par 
\par Here is a simple example of sorting an array of doubles in numerical order, using the comparison function defined above (see \cf1\strike Comparison Functions\strike0\{linkID=380\}\cf0 ): 
\par 
\par \f1           \{
\par             double *array;
\par             int size;
\par             ...
\par             qsort (array, size, sizeof (double), compare_doubles);
\par           \}
\par      
\par \f0 The \f1 qsort\f0  function derives its name from the fact that it was originally implemented using the "quick sort" algorithm. 
\par 
\par The implementation of \f1 qsort\f0  in this library might not be an in-place sort and might thereby use an extra amount of memory to store the array.
\par 
\par }
130
PAGE_55
Atomic Data Access
atomicdataccess



Imported



FALSE
19
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fswiss\fcharset0 Arial;}{\f2\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Atomic Data Access \{keepn\}
\par \pard\fs18 Previous: \cf1\strike Nonreentrancy\strike0\{linkID=1480\}\cf0 , Up: \cf1\strike Defining Handlers\strike0\{linkID=530\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Atomic Data Access and Signal Handling\b0 
\par 
\par Whether the data in your application concerns atoms, or mere text, you have to be careful about the fact that access to a single datum is not necessarily \i atomic\i0 . This means that it can take more than one instruction to read or write a single object. In such cases, a signal handler might be invoked in the middle of reading or writing the object. 
\par 
\par There are t\f1 wo\f0  ways you can cope with this problem. You can use data types that are always accessed atomically; you can carefully arrange that nothing untoward happens if an access is interrupted\f1 .\f0 
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Non-atomic Example\strike0\{linkID=1490\}\cf0 : A program illustrating interrupted access.
\par \f2\'b7\f0  \cf1\strike Types\strike0\{linkID=140\}\cf0 : Data types that guarantee no interruption.
\par \f2\'b7\f0  \cf1\strike Usage\strike0\{linkID=150\}\cf0 : Proving that interruption is harmless.
\par 
\par }
140
PAGE_56
Atomic Types
sig_atomic_t



Imported



FALSE
21
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Atomic Types \{keepn\}
\par \pard\fs18 Next: \cf1\strike Atomic Usage\strike0\{linkID=150\}\cf0 , Previous: \cf1\strike Non-atomic Example\strike0\{linkID=1490\}\cf0 , Up: \cf1\strike Atomic Data Access\strike0\{linkID=130\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Atomic Types\b0 
\par 
\par To avoid uncertainty about interrupting access to a variable, you can use a particular data type for which access is always atomic:\f1  sig_atomic_t\f0 . Reading and writing this data type is guaranteed to happen in a single instruction, so there's no way for a handler to run "in the middle" of an access. 
\par 
\par The type \f1 sig_atomic_t\f0  is always an integer data type, but which one it is, and how many bits it contains, may vary from machine to machine.
\par 
\par -- Data Type: \b sig_atomic_t\b0 
\par 
\par \pard\li500 This is an integer data type. Objects of this type are always accessed atomically.
\par 
\par \pard In practice, you can assume that \f1 int\f0  and other integer types no longer than \f1 int\f0  are atomic. You can also assume that pointer types are atomic; that is very convenient. Both of these assumptions are true on all of the machines that the C library supports and on all POSIX systems we know of.
\par 
\par }
150
PAGE_57
Atomic Usage




Imported



FALSE
17
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Atomic Usage \{keepn\}
\par \pard\fs18 Previous: \cf1\strike Atomic Types\strike0\{linkID=140\}\cf0 , Up: \cf1\strike Atomic Data Access\strike0\{linkID=130\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Atomic Usage Patterns\b0 
\par 
\par Certain patterns of access avoid any problem even if an access is interrupted. For example, a flag which is set by the handler, and tested and cleared by the main program from time to time, is always safe even if access actually requires two instructions. To show that this is so, we must consider each access that could be interrupted, and show that there is no problem if it is interrupted. 
\par 
\par An interrupt in the middle of testing the flag is safe because either it's recognized to be nonzero, in which case the precise value doesn't matter, or it will be seen to be nonzero the next time it's tested. 
\par 
\par An interrupt in the middle of clearing the flag is no problem because either the value ends up zero, which is what happens if a signal comes in just before the flag is cleared, or the value ends up nonzero, and subsequent events occur as if the signal had come in just after the flag was cleared. As long as the code handles both of these cases properly, it can also handle a signal in the middle of clearing the flag. (This is an example of the sort of reasoning you need to do to figure out whether non-atomic usage is safe.) 
\par 
\par }
160
PAGE_58
Attribute Meanings
statusofafile;attributesofafile;fileattributes;sys/stat.h;structstat;structstat;mode_t;inodenumber;ino_t;ino64_t;dev_t;nlink_t;blkcnt_t;blkcnt64_t



Imported



FALSE
47
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Attribute Meanings \{keepn\}
\par \pard\ri100\fs18 Next: \cf1\strike Reading Attributes\strike0\{linkID=1800\}\cf0 , Up: \cf1\strike File System Interface\cf2\strike0\{linkID=820\}\cf0\fs2 
\par \pard\fs10 
\par \fs18 
\par \b The meaning of the File Attributes\b0 
\par 
\par When you read the attributes of a file, they come back in a structure called \f1 struct stat\f0 . This section describes the names of the attributes, their data types, and what they mean. For the functions to read the attributes of a file, see \cf1\strike Reading Attributes\strike0\{linkID=1800\}\cf0 . 
\par 
\par The header file \f1 sys/stat.h\f0  declares all the symbols defined in this section
\par .
\par -- Data Type: \b struct stat\b0 
\par 
\par \pard\li500 The \f1 stat\f0  structure type is used to return information about the attributes of a file. It contains at least the following members:
\par 
\par \f1 mode_t st_mode\f0 
\par \f2            \f0 Specifies the mode of the file. This includes file type information  and the file permission bits 
\par \f1 ino_t st_ino\f0 
\par \pard\li1000 The file serial number, which distinguishes this file from all other files on the same device. 
\par \pard\li500\f1 dev_t st_dev\f0 
\par \pard\li1000 Identifies the device containing the file. The \f1 st_ino\f0  and\f1  st_dev\f0 , taken together, uniquely identify the file. The\f1  st_dev\f0  value is not necessarily consistent across reboots or system crashes, however. 
\par \pard\li500\f1 off_t st_size\f0 
\par \pard\li1000 This specifies the size of a regular file in bytes. For files that are really devices this field isn't usually meaningful. For symbolic links this specifies the length of the file name the link refers to. 
\par \pard\li500\f1 time_t st_atime\f0 
\par \pard\li1000 This is the last access time for the file.
\par \pard\li500\f1 time_t st_mtime\f0 
\par \pard\li1000 This is the time of the last modification to the contents of the file.
\par \pard\li500\f1 time_t st_ctime\f0 
\par \pard\li1000 This is the time of the last modification to the attributes of the file.
\par 
\par \pard Some of the file attributes have special data type names which exist specifically for those attributes. (They are all aliases for well-known integer types that you know and love.) These typedef names are defined in the header file \f1 sys/types.h\f0  as well as in \f1 sys/stat.h\f0 . Here is a list of them.
\par 
\par -- Data Type: \b mode_t\b0 
\par 
\par \pard\li500 This is an integer data type used to represent file modes. In the GNU system, this is equivalent to \f1 unsigned int\f0 .
\par 
\par \pard -- Data Type: \b ino_t\b0 
\par 
\par \pard\li500 This is an arithmetic data type used to represent file serial numbers. (In Unix jargon, these are sometimes called \i inode numbers\i0 .) In the GNU system, this type is equivalent to \f1 unsigned long int\f0 . 
\par 
\par If the source is compiled with \f1 _FILE_OFFSET_BITS == 64\f0  this type is transparently replaced by \f1 ino64_t\f0 .
\par \pard -- Data Type: \b dev_t\b0 
\par 
\par \pard\li500 This is an arithmetic data type used to represent file device numbers. In the GNU system, this is equivalent to \f1 int\f0 .
\par 
\par }
170
PAGE_60
Basic Allocation
allocationofmemorywithmalloc;stdlib.h;malloc



Imported



FALSE
35
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Basic Allocation \{keepn\}
\par \pard\fs18 Next: \cf1\strike Malloc Examples\strike0\{linkID=1380\}\cf0 , Up: \cf1\strike Unconstrained Allocation\strike0\{linkID=2340\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Basic Memory Allocation\b0 
\par 
\par To allocate a block of memory, call \f1 malloc\f0 . The prototype for this function is in \f1 stdlib.h\f0 .
\par 
\par -- Function: void * \b malloc\b0  (\i size_t size\i0 )
\par 
\par \pard\li500 This function returns a pointer to a newly allocated block \i size\i0  bytes long, or a null pointer if the block could not be allocated.
\par 
\par \pard The contents of the block are undefined; you must initialize it yourself (or use \f1 calloc\f0  instead; see \cf1\strike Allocating Cleared Space\strike0\{linkID=70\}\cf0 ). Normally you would cast the value as a pointer to the kind of object that you want to store in the block. Here we show an example of doing so, and of initializing the space with zeros using the library function\f1  memset\f0  (see \cf1\strike Copying and Concatenation\strike0\{linkID=460\}\cf0 ):
\par 
\par \f1      struct foo *ptr;
\par      ...
\par      ptr = (struct foo *) malloc (sizeof (struct foo));
\par      if (ptr == 0) abort ();
\par      memset (ptr, 0, sizeof (struct foo));
\par \f0 
\par You can store the result of \f1 malloc\f0  into any pointer variable without a cast, because ISO C automatically converts the type\f1  void *\f0  to another type of pointer when necessary. But the cast is necessary in contexts other than assignment operators or if you might want your code to run in traditional C. 
\par 
\par Remember that when allocating space for a string, the argument to\f1  malloc\f0  must be one plus the length of the string. This is because a string is terminated with a null character that doesn't count in the "length" of the string but does need space. For example:
\par 
\par \f1      char *ptr;
\par      ...
\par      ptr = (char *) malloc (length + 1);
\par \f0 
\par See \cf1\strike Representation of Strings\strike0\{linkID=1850\}\cf0 , for more information about this.
\par 
\par }
180
PAGE_62
Basic Signal Handling
signal;signal.h;sighandler_t;signal;SIG_DFL;defaultactionforasignal;SIG_IGN;ignoreactionforasignal;sysv_signal;ssignal;SIG_ERR



Imported



FALSE
87
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Basic Signal Handling \{keepn\}
\par \pard\fs18 Up: \cf1\strike Signal Actions\strike0\{linkID=1990\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Basic Signal Handling\b0 
\par 
\par The \f1 signal\f0  function provides a simple interface for establishing an action for a particular signal. The function and associated macros are declared in the header file \f1 signal.h\f0 .
\par 
\par -- Data Type: \b sighandler_t\b0 
\par 
\par \pard\li500 This is the type of signal handler functions. Signal handlers take one integer argument specifying the signal number, and have return type\f1  void\f0 . So, you should define handler functions like this: 
\par 
\par \f1           void \i handler\i0  (int signum) \{ ... \}
\par      
\par \f0 The name \f1 sighandler_t\f0  for this data type is a GNU extension.
\par 
\par \pard -- Function: sighandler_t \b signal\b0  (\i int signum, sighandler_t action\i0 )
\par 
\par \pard\li500 The \f1 signal\f0  function establishes \i action\i0  as the action for the signal \i signum\i0 . 
\par 
\par The first argument, \i signum\i0 , identifies the signal whose behavior you want to control, and should be a signal number. The proper way to specify a signal number is with one of the symbolic signal names (see \cf1\strike Standard Signals\strike0\{linkID=2090\}\cf0 )--don't use an explicit number, because the numerical code for a given kind of signal may vary from operating system to operating system. 
\par 
\par The second argument, \i action\i0 , specifies the action to use for the signal \i signum\i0 . This can be one of the following:
\par 
\par \f1 SIG_DFL\f0 
\par \pard\li1000\f1 SIG_DFL\f0  specifies the default action for the particular signal. The default actions for various kinds of signals are stated in \cf1\strike Standard Signals\strike0\{linkID=2090\}\cf0 . 
\par \pard\li500\f1 SIG_IGN\f0 
\par \pard\li1000\f1 SIG_IGN\f0  specifies that the signal should be ignored. 
\par 
\par Your program generally should not ignore signals that represent serious events or that are normally used to request termination. You cannot ignore the \f1 SIGKILL\f0  or \f1 SIGSTOP\f0  signals at all. You can ignore program error signals like \f1 SIGSEGV\f0 , but ignoring the error won't enable the program to continue executing meaningfully. Ignoring user requests such as \f1 SIGINT\f0 , \f1 SIGQUIT\f0 , and \f1 SIGTSTP\f0  is unfriendly. 
\par 
\par When you do not wish signals to be delivered during a certain part of the program, the thing to do is to block them, not ignore them. See \cf1\strike Blocking Signals\strike0\{link=PAGE_68\}\cf0 . 
\par \pard\li500\i handler\i0 
\par \pard\li1000 Supply the address of a handler function in your program, to specify running this handler as the way to deliver the signal. 
\par 
\par For more information about defining signal handler functions, see \cf1\strike Defining Handlers\strike0\{linkID=530\}\cf0 .
\par 
\par \pard\li500 If you set the action for a signal to \f1 SIG_IGN\f0 , or if you set it to \f1 SIG_DFL\f0  and the default action is to ignore that signal, then any pending signals of that type are discarded (even if they are blocked). Discarding the pending signals means that they will never be delivered, not even if you subsequently specify another action and unblock this kind of signal. 
\par 
\par The \f1 signal\f0  function returns the action that was previously in effect for the specified \i signum\i0 . You can save this value and restore it later by calling \f1 signal\f0  again. 
\par 
\par If \f1 signal\f0  can't honor the request, it returns \f1 SIG_ERR\f0  instead. The following \f1 errno\f0  error conditions are defined for this function:
\par 
\par \f1 EINVAL\f0 
\par \pard\li1000 You specified an invalid \i signum\i0 ; or you tried to ignore or provide a handler for \f1 SIGKILL\f0  or \f1 SIGSTOP\f0 .
\par 
\par \pard\b Compatibility Note:\b0  A problem encountered when working with the\f1  signal\f0  function is that it has different semantics on BSD and SVID systems. The difference is that on SVID systems the signal handler is deinstalled after signal delivery. On BSD systems the handler must be explicitly deinstalled. In th\f2 is library \f0 we use the \f2 SVID\f0  version.
\par 
\par Here is a simple example of setting up a handler to delete temporary files when certain fatal signals happen:
\par 
\par \f1      #include <signal.h>
\par      
\par      void
\par      termination_handler (int signum)
\par      \{
\par        struct temp_file *p;
\par      
\par        for (p = temp_file_list; p; p = p->next)
\par          unlink (p->name);
\par      \}
\par      
\par      int
\par      main (void)
\par      \{
\par        ...
\par        if (signal (SIGINT, termination_handler) == SIG_IGN)
\par          signal (SIGINT, SIG_IGN);
\par        if (signal (SIGHUP, termination_handler) == SIG_IGN)
\par          signal (SIGHUP, SIG_IGN);
\par        if (signal (SIGTERM, termination_handler) == SIG_IGN)
\par          signal (SIGTERM, SIG_IGN);
\par        ...
\par      \}
\par \f0 
\par Note that if a given signal was previously set to be ignored, this code avoids altering that setting. This is because non-job-control shells often ignore certain signals when starting children, and it is important for the children to respect this. 
\par 
\par We do not handle \f1 SIGQUIT\f0  or the program error signals in this example because these are designed to provide information for debugging (a core dump), and the temporary files may give useful information.
\par 
\par -- Macro: sighandler_t \b SIG_ERR\b0 
\par 
\par \pard\li500 The value of this macro is used as the return value from \f1 signal\f0  to indicate an error.
\par 
\par }
190
PAGE_64
Binary Streams
textstream;binarystream;lines(inatextfile



Imported



FALSE
26
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fswiss\fcharset0 Arial;}{\f2\fmodern Courier New;}{\f3\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Binary Streams \{keepn\}
\par \pard\fs18 Next: \cf1\strike File Positioning\strike0\{linkID=790\}\cf0 , Previous: \cf1\strike Error Recovery\strike0\{linkID=680\}\cf0 , Up: \cf1\strike I/O on Streams\strike0\{linkID=1260\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Text and Binary Streams\b0 
\par 
\par POSIX-compatible operating systems organize all files as uniform sequences of characters. However, \f1 this\f0  system make\f1 s\f0  a \f1 subtle \f0 distinction between files containing text and files containing binary data, and the input and output facilities of ISO C provide for this distinction\f1 .\f0  
\par 
\par When you open a stream, you can specify either a \i text stream\i0  or a\i  binary stream\i0 . You indicate that you want a binary stream by specifying the `\f2 b\f0 ' modifier in the \i opentype\i0  argument to\f2  fopen\f0 ; see \cf1\strike Opening Streams\strike0\{linkID=1610\}\cf0 . Without this option, \f2 fopen\f0  opens the file as a text stream. 
\par 
\par Text and binary streams differ in several ways:
\par 
\par \pard\li500\f3\'b7\f0  The data read from a text stream is divided into \i lines\i0  which are terminated by newline (\f2 '\\n'\f0 ) characters, while a binary stream is simply a long series of characters. A text stream might on some systems fail to handle lines more than 254 characters long (including the terminating newline character).
\par \f3\'b7\f0  On some systems, text files can contain only printing characters, horizontal tab characters, and newlines, and so text streams may not support other characters. However, binary streams can handle any character value. 
\par \f3\'b7\f0  Space characters that are written immediately preceding a newline character in a text stream may disappear when the file is read in again. 
\par \f3\'b7\f0  More generally, there need not be a one-to-one mapping between characters that are read from or written to a text stream, and the characters in the actual file.
\par 
\par \pard Since a binary stream is always more capable and more predictable than a text stream, you might wonder what purpose text streams serve. Why not simply always use binary streams? The answer is that on these operating systems, text and binary streams use different file formats, and the only way to read or write "an ordinary file of text" that can work with other text-oriented programs is through a text stream. 
\par 
\par In the GNU library, and on all POSIX systems, there is no difference between text streams and binary streams. When you open a stream, you get the same kind of stream regardless of whether you ask for binary. This stream can handle any file content, and has none of the restrictions that text streams sometimes have.
\par 
\par }
200
PAGE_65
Block Input/Output
binaryI/Otoastream;blockI/Otoastream;readingfromastream,byblocks;writingtoastream,byblocks;stdio.h;fread;fread_unlocked;fwrite;fwrite_unlocked



Imported



FALSE
27
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fswiss\fcharset0 Arial;}{\f2\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Block Input/Output \{keepn\}
\par \pard\fs18 Next: \cf1\strike Formatted Output\strike0\{linkID=970\}\cf0 , Previous: \cf1\strike\f1 Line Input\cf2\strike0\{linkID=1316\}\cf0\f0 , Up: \cf1\strike I/O on Streams\strike0\{linkID=1260\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Block Input/Output\b0 
\par 
\par This section describes how to do input and output operations on blocks of data. You can use these functions to read and write binary data, as well as to read and write text in fixed-size blocks instead of by characters or lines. Binary files are typically used to read and write blocks of data in the same format as is used to represent the data in a running program. In other words, arbitrary blocks of memory--not just character or string objects--can be written to a binary file, and meaningfully read in again by the same program. 
\par 
\par Storing data in binary form is often considerably more efficient than using the formatted I/O functions. Also, for floating-point numbers, the binary form avoids possible loss of precision in the conversion process. On the other hand, binary files can't be examined or modified easily using many standard file utilities (such as text editors), and are not portable between different implementations of the language, or different kinds of computers. 
\par 
\par These functions are declared in \f2 stdio.h\f0 .
\par 
\par -- Function: size_t \b fread\b0  (\i void *data, size_t size, size_t count, FILE *stream\i0 )
\par 
\par \pard\li500 This function reads up to \i count\i0  objects of size \i size\i0  into the array \i data\i0 , from the stream \i stream\i0 . It returns the number of objects actually read, which might be less than \i count\i0  if a read error occurs or the end of the file is reached. This function returns a value of zero (and doesn't read anything) if either \i size\i0  or \i count\i0  is zero. 
\par 
\par If \f2 fread\f0  encounters end of file in the middle of an object, it returns the number of complete objects read, and discards the partial object. Therefore, the stream remains at the actual end of the file.
\par 
\par \pard -- Function: size_t \b fwrite\b0  (\i const void *data, size_t size, size_t count, FILE *stream\i0 )
\par 
\par \pard\li500 This function writes up to \i count\i0  objects of size \i size\i0  from the array \i data\i0 , to the stream \i stream\i0 . The return value is normally \i count\i0 , if the call succeeds. Any other value indicates some sort of error, such as running out of space.
\par 
\par }
210
PAGE_69
Broken-down Time
broken-downtime;calendartimeandbroken-downtime;structtm;leapsecond;DaylightSavingTime;summertime;localtime;localtime_r;gmtime;gmtime_r;mktime;timelocal;timegm



Imported



FALSE
63
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fswiss\fcharset0 Arial;}{\f2\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Broken-down Time \{keepn\}
\par \pard\fs18 Next:\f1  \f0  \cf1\strike Formatting Calendar Time\cf2\strike0\{linkID=980\cf0\f1 ,\f0  \f1 Previous: \cf1\strike\f0 Simple Calendar Time\cf2\strike0\{linkID=2040\}\cf0 ,Up: \cf1\strike Calendar Time\strike0\{linkID=230\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Broken-down Time\b0 
\par 
\par Calendar time is represented by the usual\f1  \f0 C library functions as an elapsed time since a fixed base calendar time. This is convenient for computation, but has no relation to the way people normally think of calendar time. By contrast, \i broken-down time\i0  is a binary representation of calendar time separated into year, month, day, and so on. Broken-down time values are not useful for calculations, but they are useful for printing human readable time information. 
\par 
\par A broken-down time value is always relative to a choice of time zone\f1 .\f0 
\par 
\par The symbols in this section are declared in the header file \f2 time.h\f0 .
\par -- Data Type: \b struct tm\b0 
\par 
\par \pard\li500 This is the data type used to represent a broken-down time. The structure contains at least the following members, which can appear in any order.
\par 
\par \f2 int tm_sec\f0 
\par \pard\li1000 This is the number of full seconds since the top of the minute (normally in the range \f2 0\f0  through \f2 59\f0 , but the actual upper limit is\f2  60\f0 , to allow for leap seconds if leap second support is available).
\par \pard\li500\f2 int tm_min\f0 
\par \pard\li1000 This is the number of full minutes since the top of the hour (in the range \f2 0\f0  through \f2 59\f0 ). 
\par \pard\li500\f2 int tm_hour\f0 
\par \pard\li1000 This is the number of full hours past midnight (in the range \f2 0\f0  through\f2  23\f0 ). 
\par \pard\li500\f2 int tm_mday\f0 
\par \pard\li1000 This is the ordinal day of the month (in the range \f2 1\f0  through \f2 31\f0 ). Watch out for this one! As the only ordinal number in the structure, it is inconsistent with the rest of the structure. 
\par \pard\li500\f2 int tm_mon\f0 
\par \pard\li1000 This is the number of full calendar months since the beginning of the year (in the range \f2 0\f0  through \f2 11\f0 ). Watch out for this one! People usually use ordinal numbers for month-of-year (where January = 1). 
\par \pard\li500\f2 int tm_year\f0 
\par \pard\li1000 This is the number of full calendar years since 1900. 
\par \pard\li500\f2 int tm_wday\f0 
\par \pard\li1000 This is the number of full days since Sunday (in the range \f2 0\f0  through\f2  6\f0 ). 
\par \pard\li500\f2 int tm_yday\f0 
\par \pard\li1000 This is the number of full days since the beginning of the year (in the range \f2 0\f0  through \f2 365\f0 ). 
\par \pard\li500\f2 int tm_isdst\f0 
\par \pard\li1000 This is a flag that indicates whether Daylight Saving Time is (or was, or will be) in effect at the time described. The value is positive if Daylight Saving Time is in effect, zero if it is not, and negative if the information is not available. 
\par 
\par \pard -- Function: struct tm * \b localtime\b0  (\i const time_t *time\i0 )
\par 
\par \pard\li500 The \f2 localtime\f0  function converts the simple time pointed to by\i  time\i0  to broken-down time representation, expressed relative to the user's specified time zone. 
\par 
\par The return value is a pointer to a static broken-down time structure, which might be overwritten by subsequent calls to \f2 ctime\f0 , \f2 gmtime\f0 , or \f2 localtime\f0 . (But no other library function overwrites the contents of this object.) 
\par 
\par The return value is the null pointer if \i time\i0  cannot be represented as a broken-down time; typically this is because the year cannot fit into an \f2 int\f0 . 
\par 
\par Calling \f2 localtime\f0  has one other effect: it sets the variable\f2  tzname\f0  with information about the current time zone. See \cf1\strike Time Zone Functions\strike0\{linkID=2310\}\cf0 .
\par 
\par \pard -- Function: struct tm * \b gmtime\b0  (\i const time_t *time\i0 )
\par 
\par \pard\li500 This function is similar to \f2 localtime\f0 , except that the broken-down time is expressed as Coordinated Universal Time (UTC) (formerly called Greenwich Mean Time (GMT)) rather than relative to a local time zone.
\par 
\par \pard -- Function: time_t \b mktime\b0  (\i struct tm *brokentime\i0 )
\par 
\par \pard\li500 The \f2 mktime\f0  function is used to convert a broken-down time structure to a simple time representation. It also "normalizes" the contents of the broken-down time structure, by filling in the day of week and day of year based on the other date and time components. 
\par 
\par The \f2 mktime\f0  function ignores the specified contents of the\f2  tm_wday\f0  and \f2 tm_yday\f0  members of the broken-down time structure. It uses the values of the other components to determine the calendar time; it's permissible for these components to have unnormalized values outside their normal ranges. The last thing that\f2  mktime\f0  does is adjust the components of the \i brokentime\i0  structure (including the \f2 tm_wday\f0  and \f2 tm_yday\f0 ). 
\par 
\par If the specified broken-down time cannot be represented as a simple time,\f2  mktime\f0  returns a value of \f2 (time_t)(-1)\f0  and does not modify the contents of \i brokentime\i0 . 
\par 
\par Calling \f2 mktime\f0  also sets the variable \f2 tzname\f0  with information about the current time zone. See \cf1\strike Time Zone Functions\strike0\{linkID=2310\}\cf0 .
\par 
\par }
220
PAGE_75
Buffering Concepts
unbufferedstream;linebufferedstream;fullybufferedstream



Imported



FALSE
21
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern\fcharset0 Courier New;}{\f2\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Buffering Concepts \{keepn\}
\par \pard\fs18 Next: \cf1\strike Flushing Buffers\strike0\{linkID=910\}\cf0 , Up: \cf1\strike Stream Buffering\strike0\{linkID=2120\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Buffering Concepts\b0 
\par 
\par There are three different kinds of buffering strategies:
\par 
\par \pard\li500\f1\'b7\f0  Characters written to or read from an \i unbuffered\i0  stream are transmitted individually to or from the file as soon as possible.
\par \f1\'b7\f0  Characters written to a \i line buffered\i0  stream are transmitted to the file in blocks when a newline character is encountered.
\par \f1\'b7\f0  Characters written to or read from a \i fully buffered\i0  stream are transmitted to or from the file in blocks of arbitrary size.
\par 
\par \pard Newly opened streams are normally fully buffered, with one exception: a stream connected to an interactive device such as a terminal is initially line buffered. See \cf1\strike Controlling Buffering\strike0\{linkID=430\}\cf0 , for information on how to select a different kind of buffering. Usually the automatic selection gives you the most convenient kind of buffering for the file or device you open. 
\par 
\par The use of line buffering for interactive devices implies that output messages ending in a newline will appear immediately--which is usually what you want. Output that doesn't end in a newline might or might not show up immediately, so if you want them to appear immediately, you should flush buffered output explicitly with \f2 fflush\f0 , as described in \cf1\strike Flushing Buffers\strike0\{linkID=910\}\cf0 .
\par 
\par }
230
PAGE_78
Calendar Time
simpletime;high-resolutiontime;localtime;broken-downtime;Gregoriancalendar;calendar,Gregorian



Imported



FALSE
25
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fswiss\fcharset0 Arial;}{\f2\fmodern\fcharset0 Courier New;}{\f3\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Calendar Time \{keepn\}
\par \pard\fs18 Next: \cf1\strike Simple Calendar Time\cf2\strike0\{linkID=2040\}\cf0 , Previous:\f1  \cf1\strike CPU Time\cf2\strike0\{linkID=480\}\cf0\f0 ,, Up: \cf1\strike Date and Time\strike0\{linkID=520\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Calendar Time\b0 
\par 
\par This section describes facilities for keeping track of calendar time. See \cf1\strike Time Basics\strike0\{linkID=2290\}\cf0 . 
\par 
\par The\f1  \f0 C library represents calendar time \f1 two\f0  ways:
\par 
\par \pard\li500\f2\'b7\i\f0  Simple time\i0  (the \f3 time_t\f0  data type) is a compact representation, typically giving the number of seconds of elapsed time since some implementation-specific base time.
\par \f2\'b7\i\f0  Local time\i0  or \i broken-down time\i0  (the \f3 struct tm\f0  data type) represents a calendar time as a set of components specifying the year, month, and so on in the Gregorian calendar, for a specific time zone. This calendar time representation is usually used only to communicate with people.
\par 
\par \f2\'b7\f0  \cf1\strike Simple Calendar Time\strike0\{linkID=2040\}\cf0 : Facilities for manipulating calendar time.
\par \f2\'b7\f0  \cf1\strike Broken-down Time\strike0\{linkID=210\}\cf0 : Facilities for manipulating local time.
\par \f2\'b7\f0  \cf1\strike Formatting Calendar Time\strike0\{linkID=980\}\cf0 : Converting times to strings.
\par \f2\'b7\f0  \cf1\strike TZ Variable\strike0\{linkID=2330\}\cf0 : How users specify the time zone.
\par \f2\'b7\f0  \cf1\strike Time Zone Functions\strike0\{linkID=2310\}\cf0 : Functions to examine or specify the time zone.
\par \f2\'b7\f0  \cf1\strike Time Functions Example\strike0\{linkID=2300\}\cf0 : An example program showing use of some of the time functions.
\par 
\par }
240
PAGE_80
Calling Variadics
variadicfunctions,calling;callingvariadicfunctions;declaringvariadicfunctions;defaultargumentpromotions;argumentpromotion



Imported



FALSE
21
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fswiss\fcharset0 Arial;}{\f2\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Calling Variadics \{keepn\}
\par \pard\fs18 Next: \cf1\strike Argument Macros\strike0\{linkID=80\}\cf0 , Previous: \cf1\strike How Many Arguments\strike0\{linkID=1100\}\cf0 , Up: \cf1\strike How Variadic\strike0\{linkID=1120\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Calling Variadic Functions\b0 
\par 
\par You don't have to do anything special to call a variadic function. Just put the arguments (required arguments, followed by optional ones) inside parentheses, separated by commas, as usual. But you must declare the function with a prototype and know how the argument values are converted. 
\par 
\par In principle, functions that are \i defined\i0  to be variadic must also be \i declared\i0  to be variadic using a function prototype whenever you call them. (See \cf1\strike Variadic Prototypes\strike0\{linkID=2430\}\cf0 , for how.) This is because some C compilers use a different calling convention to pass the same set of argument values to a function depending on whether that function takes variable arguments or fixed arguments. 
\par 
\par In practice, th\f1 is \f0 C compiler always passes a given set of argument types in the same way regardless of whether they are optional or required. So, as long as the argument types are self-promoting, you can safely omit declaring them. Usually it is a good idea to declare the argument types for variadic functions, and indeed for all functions. But there are a few functions which it is extremely convenient not to have to declare as variadic--for example, \f2 open\f0  and\f2  printf\f0 . 
\par 
\par Since the prototype doesn't specify types for optional arguments, in a call to a variadic function the \i default argument promotions\i0  are performed on the optional argument values. This means the objects of type \f2 char\f0  or \f2 short int\f0  (whether signed or not) are promoted to either \f2 int\f0  or \f2 unsigned int\f0 , as appropriate; and that objects of type \f2 float\f0  are promoted to type\f2  double\f0 . So, if the caller passes a \f2 char\f0  as an optional argument, it is promoted to an \f2 int\f0 , and the function can access it with \f2 va_arg (\i\f0 ap\i0\f2 , int)\f0 . 
\par 
\par Conversion of the required arguments is controlled by the function prototype in the usual way: the argument expression is converted to the declared argument type as if it were being assigned to a variable of that type.
\par 
\par }
250
PAGE_84
Case Conversion
charactercaseconversion;caseconversionofcharacters;convertingcaseofcharacters;ctype.h;tolower;toupper;toascii;gt_tolower;gt_toupper



Imported



FALSE
39
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Case Conversion \{keepn\}
\par \pard\fs18 Next: \cf1\strike Classification of Wide Characters\strike0\{linkID=340\}\cf0 , Previous: \cf1\strike Classification of Characters\strike0\{linkID=330\}\cf0 , Up: \cf1\strike Character Handling\strike0\{linkID=270\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Case Conversion\b0 
\par 
\par This section explains the library functions for performing conversions such as case mappings on characters. For example, \f1 toupper\f0  converts any character to upper case if possible. If the character can't be converted, \f1 toupper\f0  returns it unchanged. 
\par 
\par These functions take one argument of type \f1 int\f0 , which is the character to convert, and return the converted character as an\f1  int\f0 . If the conversion is not applicable to the argument given, the argument is returned unchanged. 
\par 
\par \b Compatibility Note:\b0  In pre-ISO C dialects, instead of returning the argument unchanged, these functions may fail when the argument is not suitable for the conversion. Thus for portability, you may need to write \f1 islower(c) ? toupper(c) : c\f0  rather than just\f1  toupper(c)\f0 . 
\par 
\par These functions are declared in the header file \f1 ctype.h\f0 .
\par 
\par -- Function: int \b tolower\b0  (\i int c\i0 )
\par 
\par \pard\li500 If \i c\i0  is an upper-case letter, \f1 tolower\f0  returns the corresponding lower-case letter. If \i c\i0  is not an upper-case letter,\i  c\i0  is returned unchanged.
\par 
\par \pard -- Function: int \b toupper\b0  (\i int c\i0 )
\par 
\par \pard\li500 If \i c\i0  is a lower-case letter, \f1 toupper\f0  returns the corresponding upper-case letter. Otherwise \i c\i0  is returned unchanged.
\par 
\par \pard -- Function: int \b toascii\b0  (\i int c\i0 )
\par 
\par \pard\li500 This function converts \i c\i0  to a 7-bit \f1 unsigned char\f0  value that fits into the US/UK ASCII character set, by clearing the high-order bits. This function is a BSD extension and is also an SVID extension.
\par 
\par \pard -- Function: int \b _tolower\b0  (\i int c\i0 )
\par 
\par \pard\li500 This is identical to \f1 tolower\f0 , and is provided for compatibility with the SVID. See \cf1\strike SVID\strike0\{link=PAGE_605\}\cf0 .
\par 
\par \pard -- Function: int \b _toupper\b0  (\i int c\i0 )
\par 
\par \pard\li500 This is identical to \f1 toupper\f0 , and is provided for compatibility with the SVID.
\par 
\par }
260
PAGE_85
Changing Block Size
changingthesizeofablock(malloc);stdlib.h;realloc



Imported



FALSE
40
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Changing Block Size \{keepn\}
\par \pard\fs18 Next: \cf1\strike Allocating Cleared Space\strike0\{linkID=70\}\cf0 , Previous: \cf1\strike Freeing after Malloc\strike0\{linkID=1030\}\cf0 , Up: \cf1\strike Unconstrained Allocation\strike0\{linkID=2340\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Changing the Size of a Block\b0 
\par 
\par Often you do not know for certain how big a block you will ultimately need at the time you must begin to use the block. For example, the block might be a buffer that you use to hold a line being read from a file; no matter how long you make the buffer initially, you may encounter a line that is longer. 
\par 
\par You can make the block longer by calling \f1 realloc\f0 . This function is declared in \f1 stdlib.h\f0 .
\par 
\par -- Function: void * \b realloc\b0  (\i void *ptr, size_t newsize\i0 )
\par 
\par \pard\li500 The \f1 realloc\f0  function changes the size of the block whose address is\i  ptr\i0  to be \i newsize\i0 . 
\par 
\par Since the space after the end of the block may be in use, \f1 realloc\f0  may find it necessary to copy the block to a new address where more free space is available. The value of \f1 realloc\f0  is the new address of the block. If the block needs to be moved, \f1 realloc\f0  copies the old contents. 
\par 
\par If you pass a null pointer for \i ptr\i0 , \f1 realloc\f0  behaves just like `\f1 malloc (\i newsize\i0 )\f0 '. This can be convenient, but beware that older implementations (before ISO C) may not support this behavior, and will probably crash when \f1 realloc\f0  is passed a null pointer.
\par 
\par \pard Like \f1 malloc\f0 , \f1 realloc\f0  may return a null pointer if no memory space is available to make the block bigger. When this happens, the original block is untouched; it has not been modified or relocated. 
\par 
\par In most cases it makes no difference what happens to the original block when \f1 realloc\f0  fails, because the application program cannot continue when it is out of memory, and the only thing to do is to give a fatal error message. Often it is convenient to write and use a subroutine, conventionally called \f1 xrealloc\f0 , that takes care of the error message as \f1 xmalloc\f0  does for \f1 malloc\f0 :
\par 
\par \f1      void *
\par      xrealloc (void *ptr, size_t size)
\par      \{
\par        register void *value = realloc (ptr, size);
\par        if (value == 0)
\par          fatal ("Virtual memory exhausted");
\par        return value;
\par      \}
\par \f0 
\par You can also use \f1 realloc\f0  to make a block smaller. The reason you would do this is to avoid tying up a lot of memory space when only a little is needed. In several allocation implementations, making a block smaller sometimes necessitates copying it, so it can fail if no other space is available. 
\par 
\par If the new size you specify is the same as the old size, \f1 realloc\f0  is guaranteed to change nothing and return the same address that you gave.
\par 
\par }
270
PAGE_86
Character Handling
ctype.h



Imported



FALSE
21
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Character Handling\{keepn\}
\par \pard\fs18 Next: \cf1\strike String and Array Utilities\strike0\{linkID=2160\}\cf0 , Previous: \cf1\strike Memory\strike0\{linkID=1440\}\cf0 , Up: \cf1\strike Top\strike0\{linkID=10\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Character Handling\b0 
\par 
\par Programs that work with characters and strings often need to classify a character--is it alphabetic, is it a digit, is it whitespace, and so on--and perform case conversion operations on characters. The functions in the header file \f1 ctype.h\f0  are provided for this purpose. Since the choice of locale and character set can alter the classifications of particular character codes, all of these functions are affected by the current locale. (More precisely, they are affected by the locale currently selected for character classification--the\f1  LC_CTYPE\f0  category; see \cf1\strike Locale Categories\strike0\{linkID=1320\}\cf0 .) 
\par 
\par The ISO C standard specifies two different sets of functions. The one set works on \f1 char\f0  type characters, the other one on\f1  wchar_t\f0  wide characters (see \cf1\strike Extended Char Intro\strike0\{linkID=730\}\cf0 ).
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Classification of Characters\strike0\{linkID=330\}\cf0 : Testing whether characters are letters, digits, punctuation, etc.
\par \f2\'b7\f0  \cf1\strike Case Conversion\strike0\{linkID=250\}\cf0 : Case mapping, and the like.
\par \f2\'b7\f0  \cf1\strike Classification of Wide Characters\strike0\{linkID=340\}\cf0 : Character class determination for wide characters.
\par \f2\'b7\f0  \cf1\strike Using Wide Char Classes\strike0\{linkID=2370\}\cf0 : Notes on using the wide character classes.
\par \f2\'b7\f0  \cf1\strike Wide Character Case Conversion\strike0\{linkID=2450\}\cf0 : Mapping of wide characters.
\par 
\par }
280
PAGE_87
Character Input
readingfromastream,bycharacters;stdio.h;wchar.h;fgetc;fgetwc;fgetc_unlocked;fgetwc_unlocked;getc;getwc;getc_unlocked;getwc_unlocked;getchar;getwchar;getchar_unlocked;getwchar_unlocked;getw



Imported



FALSE
65
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Character Input \{keepn\}
\par \pard\fs18 Next: \cf1\strike Line Input\cf2\strike0\{linkID=1316\}\cf0 , Previous: \cf1\strike Simple Output\strike0\{linkID=2050\}\cf0 , Up: \cf1\strike I/O on Streams\strike0\{linkID=1260\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Character Input\b0 
\par 
\par This section describes functions for performing character-oriented input. These narrow streams functions are declared in the header file\f1  stdio.h\f0  and the wide character functions are declared in\f1  wchar.h\f0 . These functions return an \f1 int\f0  or \f1 wint_t\f0  value (for narrow and wide stream functions respectively) that is either a character of input, or the special value \f1 EOF\f0 /\f1 WEOF\f0  (usually -1). For the narrow stream functions it is important to store the result of these functions in a variable of type \f1 int\f0  instead of \f1 char\f0 , even when you plan to use it only as a character. Storing \f1 EOF\f0  in a\f1  char\f0  variable truncates its value to the size of a character, so that it is no longer distinguishable from the valid character `\f1 (char) -1\f0 '. So always use an \f1 int\f0  for the result of\f1  getc\f0  and friends, and check for \f1 EOF\f0  after the call; once you've verified that the result is not \f1 EOF\f0 , you can be sure that it will fit in a `\f1 char\f0 ' variable without loss of information.
\par \f2 
\par \f0 -- Function: int \b fgetc\b0  (\i FILE *stream\i0 )
\par 
\par \pard\li500 This function reads the next character as an \f1 unsigned char\f0  from the stream \i stream\i0  and returns its value, converted to an\f1  int\f0 . If an end-of-file condition or read error occurs,\f1  EOF\f0  is returned instead.
\par 
\par \pard -- Function: wint_t \b fgetwc\b0  (\i FILE *stream\i0 )
\par 
\par \pard\li500 This function reads the next wide character from the stream \i stream\i0  and returns its value. If an end-of-file condition or read error occurs, \f1 WEOF\f0  is returned instead.
\par 
\par \pard -- Function: int \b getc\b0  (\i FILE *stream\i0 )
\par 
\par \pard\li500 This is just like \f1 fgetc\f0 , except that it is permissible (and typical) for it to be implemented as a macro that evaluates the\i  stream\i0  argument more than once. \f1 getc\f0  is often highly optimized, so it is usually the best function to use to read a single character.
\par 
\par \pard -- Function: wint_t \b getwc\b0  (\i FILE *stream\i0 )
\par 
\par \pard\li500 This is just like \f1 fgetwc\f0 , except that it is permissible for it to be implemented as a macro that evaluates the \i stream\i0  argument more than once. \f1 getwc\f0  can be highly optimized, so it is usually the best function to use to read a single wide character.
\par 
\par \pard -- Function: int \b getchar\b0  (\i void\i0 )
\par 
\par \pard\li500 The \f1 getchar\f0  function is equivalent to \f1 getc\f0  with \f1 stdin\f0  as the value of the \i stream\i0  argument.
\par 
\par \pard -- Function: wint_t \b getwchar\b0  (\i void\i0 )
\par 
\par \pard\li500 The \f1 getwchar\f0  function is equivalent to \f1 getwc\f0  with \f1 stdin\f0  as the value of the \i stream\i0  argument.
\par 
\par \pard Here is an example of a function that does input using \f1 fgetc\f0 . It would work just as well using \f1 getc\f0  instead, or using\f1  getchar ()\f0  instead of \f1 fgetc (stdin)\f0 . The code would also work the same for the wide character stream functions.
\par 
\par \f1      int
\par      y_or_n_p (const char *question)
\par      \{
\par        fputs (question, stdout);
\par        while (1)
\par          \{
\par            int c, answer;
\par            /* Write a space to separate answer from question. */
\par            fputc (' ', stdout);
\par            /* Read the first character of the line.
\par               This should be the answer character, but might not be. */
\par            c = tolower (fgetc (stdin));
\par            answer = c;
\par            /* Discard rest of input line. */
\par            while (c != '\\n' && c != EOF)
\par              c = fgetc (stdin);
\par            /* Obey the answer if it was valid. */
\par            if (answer == 'y')
\par              return 1;
\par            if (answer == 'n')
\par              return 0;
\par            /* Answer was invalid: ask for valid answer. */
\par            fputs ("Please answer y or n:", stdout);
\par          \}
\par      \}
\par \f0 
\par }
290
PAGE_88
Character Set Handling




Imported



FALSE
18
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Character Set Handling \{keepn\}
\par \pard\fs18 Next: \cf1\strike Locales\strike0\{linkID=1340\}\cf0 , Previous: \cf1\strike String and Array Utilities\strike0\{linkID=2160\}\cf0 , Up: \cf1\strike Top\strike0\{linkID=10\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Character Set Handling\b0 
\par 
\par Character sets used in the early days of computing had only six, seven, or eight bits for each character: there was never a case where more than eight bits (one byte) were used to represent a single character. The limitations of this approach became more apparent as more people grappled with non-Roman character sets, where not all the characters that make up a language's character set can be represented by 2^8 choices. This chapter shows the functionality that was added to the C library to support multiple character sets.
\par 
\par \pard\li500\f1\'b7\f0  \cf1\strike Extended Char Intro\strike0\{linkID=730\}\cf0 : Introduction to Extended Characters.
\par \f1\'b7\f0  \cf1\strike Charset Function Overview\strike0\{linkID=300\}\cf0 : Overview about Character Handling Functions.
\par \f1\'b7\f0  \cf1\strike Restartable multibyte conversion\strike0\{linkID=1870\}\cf0 : Restartable multibyte conversion Functions.
\par \f1\'b7\f0  \cf1\strike Non-reentrant Conversion\strike0\{linkID=1540\}\cf0 : Non-reentrant Conversion Function.
\par \f1 
\par }
300
PAGE_90
Charset Function Overview




Imported



FALSE
15
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Charset Function Overview \{keepn\}
\par \pard\fs18 Next: \cf1\strike Restartable multibyte conversion\strike0\{linkID=1870\}\cf0 , Previous: \cf1\strike Extended Char Intro\strike0\{linkID=730\}\cf0 , Up: \cf1\strike Character Set Handling\strike0\{linkID=290\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Overview about Character Handling Functions\b0 
\par 
\par A Unix C library contains three different sets of functions in two families to handle character set conversion. One of the function families (the most commonly used) is specified in the ISO C90 standard and, therefore, is portable even beyond the Unix world. Unfortunately this family is the least useful one. These functions should be avoided whenever possible, especially when developing libraries (as opposed to applications). 
\par 
\par The second family of functions got introduced in the early Unix standards (XPG2) and is still part of the latest and greatest Unix standard: Unix 98. It is also the most powerful and useful set of functions. But we will start with the functions defined in Amendment 1 to ISO C90.
\par 
\par }
310
PAGE_91
Checking for Errors
errno.h;errno;errno.h



Imported



FALSE
34
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Checking for Errors \{keepn\}
\par \pard\fs18 Next: \cf1\strike Error Codes\strike0\{linkID=660\}\cf0 , Up: \cf1\strike Error Reporting\strike0\{linkID=690\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Checking for Errors\b0 
\par 
\par Most library functions return a special value to indicate that they have failed. The special value is typically \f1 -1\f0 , a null pointer, or a constant such as \f1 EOF\f0  that is defined for that purpose. But this return value tells you only that an error has occurred. To find out what kind of error it was, you need to look at the error code stored in the variable \f1 errno\f0 . This variable is declared in the header file\f1  errno.h\f0 .
\par -- Variable: volatile int \b errno\b0 
\par 
\par \pard\li500 The variable \f1 errno\f0  contains the system error number. You can change the value of \f1 errno\f0 . 
\par 
\par Since \f1 errno\f0  is declared \f1 volatile\f0 , it might be changed asynchronously by a signal handler; see \cf1\strike Defining Handlers\strike0\{linkID=530\}\cf0 . However, a properly written signal handler saves and restores the value of \f1 errno\f0 , so you generally do not need to worry about this possibility except when writing signal handlers. 
\par 
\par The initial value of \f1 errno\f0  at program startup is zero. Many library functions are guaranteed to set it to certain nonzero values when they encounter certain kinds of errors. These error conditions are listed for each function. These functions do not change \f1 errno\f0  when they succeed; thus, the value of \f1 errno\f0  after a successful call is not necessarily zero, and you should not use \f1 errno\f0  to determine \i whether\i0  a call failed. The proper way to do that is documented for each function. \i If\i0  the call failed, you can examine \f1 errno\f0 . 
\par 
\par Many library functions can set \f1 errno\f0  to a nonzero value as a result of calling other library functions which might fail. You should assume that any library function might alter \f1 errno\f0  when the function returns an error. 
\par 
\par \b Portability Note:\b0  ISO C specifies \f1 errno\f0  as a "modifiable lvalue" rather than as a variable, permitting it to be implemented as a macro. For example, its expansion might involve a function call, like \f1 *_errno ()\f0 . In fact, that is what \f2 happens\f0  on th\f2 is\f0  system. 
\par 
\par There are a few library functions, like \f1 sqrt\f0  and \f1 atan\f0 , that return a perfectly legitimate value in case of an error, but also set \f1 errno\f0 . For these functions, if you want to check to see whether an error occurred, the recommended method is to set \f1 errno\f0  to zero before calling the function, and then check its value afterward.
\par 
\par \pard All the error codes have symbolic names; they are macros defined in\f1  errno.h\f0 . The names start with `\f1 E\f0 ' and an upper-case letter or digit; you should consider names of this form to be reserved names. See \cf1\strike Reserved Names\strike0\{linkID=1860\}\cf0 . 
\par 
\par The error code values are all positive integers and are all distinct, with one exception: \f1 EWOULDBLOCK\f0  and \f1 EAGAIN\f0  are the same. Since the values are distinct, you can use them as labels in a\f1  switch\f0  statement; just don't use both \f1 EWOULDBLOCK\f0  and\f1  EAGAIN\f0 . Your program should not make any other assumptions about the specific values of these symbolic constants. 
\par 
\par The value of \f1 errno\f0  doesn't necessarily have to correspond to any of these macros, since some library functions might return other error codes of their own for other situations. The only values that are guaranteed to be meaningful for a particular library function are the ones that this manual lists for that function. 
\par 
\par On non-GNU systems, almost any system call can return \f1 EFAULT\f0  if it is given an invalid pointer as an argument. Since this could only happen as a result of a bug in your program, and since it will not happen \f2 in this\f0  system, we have saved space by not mentioning\f1  EFAULT\f0  in the descriptions of individual functions. 
\par 
\par }
320
PAGE_93
Choosing Locale
combininglocales



Imported



FALSE
21
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Choosing Locale \{keepn\}
\par \pard\fs18 Next: \cf1\strike Locale Categories\strike0\{linkID=1320\}\cf0 , Previous: \cf1\strike Effects of Locale\strike0\{linkID=610\}\cf0 , Up: \cf1\strike Locales\strike0\{linkID=1340\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Choosing a Locale\b0 
\par 
\par The simplest way for the user to choose a locale is to set the environment variable \f1 LANG\f0 . This specifies a single locale to use for all purposes. For example, a user could specify a hypothetical locale named `\f1 espana-castellano\f0 ' to use the standard conventions of most of Spain. 
\par 
\par The set of locales supported depends on the operating system you are using, and so do their names. We can't make any promises about what locales will exist, except for one standard locale called `\f1 C\f0 ' or `\f1 POSIX\f0 '. Later we will describe how to construct locales. 
\par 
\par A user also has the option of specifying different locales for different purposes--in effect, choosing a mixture of multiple locales. 
\par 
\par For example, the user might specify the locale `\f1 espana-castellano\f0 ' for most purposes, but specify the locale `\f1 usa-english\f0 ' for currency formatting. This might make sense if the user is a Spanish-speaking American, working in Spanish, but representing monetary amounts in US dollars. 
\par 
\par Note that both locales `\f1 espana-castellano\f0 ' and `\f1 usa-english\f0 ', like all locales, would include conventions for all of the purposes to which locales apply. However, the user can choose to use each locale for a particular subset of those purposes.
\par 
\par }
330
PAGE_94
Classification of Characters
charactertesting;classificationofcharacters;predicatesoncharacters;characterpredicates;ctype.h;lower-casecharacter;islower;upper-casecharacter;isupper;alphabeticcharacter;isalpha;digitcharacter;decimaldigitcharacter;isdigit;alphanumericcharacter;isalnum;hexadecimaldigitcharacter;isxdigit;punctuationcharacter;ispunct;whitespacecharacter;isspace;blankcharacter;isblank;graphiccharacter;isgraph;printingcharacter;isprint;controlcharacter;iscntrl;ASCIIcharacter;isascii



Imported



FALSE
81
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Classification of Characters\{keepn\}
\par \pard\fs18 Next: \cf1\strike Case Conversion\strike0\{linkID=250\}\cf0 , Up: \cf1\strike Character Handling\strike0\{linkID=270\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Classification of Characters\b0 
\par 
\par This section explains the library functions for classifying characters. For example, \f1 isalpha\f0  is the function to test for an alphabetic character. It takes one argument, the character to test, and returns a nonzero integer if the character is alphabetic, and zero otherwise. You would use it like this:
\par 
\par \f1      if (isalpha (c))
\par        printf ("The character `%c' is alphabetic.\\n", c);
\par \f0 
\par Each of the functions in this section tests for membership in a particular class of characters; each has a name starting with `\f1 is\f0 '. Each of them takes one argument, which is a character to test, and returns an \f1 int\f0  which is treated as a boolean value. The character argument is passed as an \f1 int\f0 , and it may be the constant value \f1 EOF\f0  instead of a real character. 
\par 
\par The attributes of any given character can vary between locales. See \cf1\strike Locales\strike0\{linkID=1340\}\cf0 , for more information on locales. 
\par 
\par These functions are declared in the header file \f1 ctype.h\f0 .
\par 
\par -- Function: int \b islower\b0  (\i int c\i0 )
\par 
\par \pard\li500 Returns true if \i c\i0  is a lower-case letter. The letter need not be from the Latin alphabet, any alphabet representable is valid.
\par 
\par \pard -- Function: int \b isupper\b0  (\i int c\i0 )
\par 
\par \pard\li500 Returns true if \i c\i0  is an upper-case letter. The letter need not be from the Latin alphabet, any alphabet representable is valid.
\par 
\par \pard -- Function: int \b isalpha\b0  (\i int c\i0 )
\par 
\par \pard\li500 Returns true if \i c\i0  is an alphabetic character (a letter). If\f1  islower\f0  or \f1 isupper\f0  is true of a character, then\f1  isalpha\f0  is also true. 
\par 
\par In some locales, there may be additional characters for which\f1  isalpha\f0  is true--letters which are neither upper case nor lower case. But in the standard \f1 "C"\f0  locale, there are no such additional characters.
\par 
\par \pard -- Function: int \b isdigit\b0  (\i int c\i0 )
\par 
\par \pard\li500 Returns true if \i c\i0  is a decimal digit (`\f1 0\f0 ' through `\f1 9\f0 ').
\par 
\par \pard -- Function: int \b isalnum\b0  (\i int c\i0 )
\par 
\par \pard\li500 Returns true if \i c\i0  is an alphanumeric character (a letter or number); in other words, if either \f1 isalpha\f0  or \f1 isdigit\f0  is true of a character, then \f1 isalnum\f0  is also true.
\par 
\par \pard -- Function: int \b isxdigit\b0  (\i int c\i0 )
\par 
\par \pard\li500 Returns true if \i c\i0  is a hexadecimal digit. Hexadecimal digits include the normal decimal digits `\f1 0\f0 ' through `\f1 9\f0 ' and the letters `\f1 A\f0 ' through `\f1 F\f0 ' and `\f1 a\f0 ' through `\f1 f\f0 '.
\par 
\par \pard -- Function: int \b ispunct\b0  (\i int c\i0 )
\par 
\par \pard\li500 Returns true if \i c\i0  is a punctuation character. This means any printing character that is not alphanumeric or a space character.
\par 
\par \pard -- Function: int \b isspace\b0  (\i int c\i0 )
\par 
\par \pard\li500 Returns true if \i c\i0  is a \i whitespace\i0  character. In the standard\f1  "C"\f0  locale, \f1 isspace\f0  returns true for only the standard whitespace characters:
\par 
\par \f1 ' '\f0 
\par \pard\li1000 space 
\par \pard\li500\f1 '\\f'\f0 
\par \pard\li1000 formfeed 
\par \pard\li500\f1 '\\n'\f0 
\par \pard\li1000 newline 
\par \pard\li500\f1 '\\r'\f0 
\par \pard\li1000 carriage return 
\par \pard\li500\f1 '\\t'\f0 
\par \pard\li1000 horizontal tab 
\par \pard\li500\f1 '\{link='\}
\par \pard\li1000 vertical tab
\par \pard\li500 
\par \pard -- Function: int \b isgraph\b0  (\i int c\i0 )
\par 
\par \pard\li500 Returns true if \i c\i0  is a graphic character; that is, a character that has a glyph associated with it. The whitespace characters are not considered graphic.
\par 
\par \pard -- Function: int \b isprint\b0  (\i int c\i0 )
\par 
\par \pard\li500 Returns true if \i c\i0  is a printing character. Printing characters include all the graphic characters, plus the space (` ') character.
\par 
\par \pard -- Function: int \b iscntrl\b0  (\i int c\i0 )
\par 
\par \pard\li500 Returns true if \i c\i0  is a control character (that is, a character that is not a printing character).
\par \f0 
\par }
340
PAGE_95
Classification of Wide Characters
wctype_t;wctype.h;wctype;wctype.h;iswctype;wctype.h;alphanumericcharacter;iswalnum;wctype.h;alphabeticcharacter;iswalpha;wctype.h;controlcharacter;iswcntrl;wctype.h;digitcharacter;iswdigit;wctype.h;graphiccharacter;iswgraph;wctype.h;lower-casecharacter;iswlower;wctype.h;printingcharacter;iswprint;wctype.h;punctuationcharacter;iswpunct;wctype.h;whitespacecharacter;iswspace;wctype.h;upper-casecharacter;iswupper;wctype.h;hexadecimaldigitcharacter;iswxdigit;wctype.h;blankcharacter;iswblank



Imported



FALSE
179
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Classification of Wide Characters \{keepn\}
\par \pard\fs18 Next: \cf1\strike Using Wide Char Classes\strike0\{linkID=2370\}\cf0 , Previous: \cf1\strike Case Conversion\strike0\{linkID=250\}\cf0 , Up: \cf1\strike Character Handling\strike0\{linkID=270\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Character class determination for wide characters\b0 
\par 
\par Amendment 1 to ISO C90 defines functions to classify wide characters. Although the original ISO C90 standard already defined the type \f1 wchar_t\f0 , no functions operating on them were defined. 
\par 
\par The general design of the classification functions for wide characters is more general. It allows extensions to the set of available classifications, beyond those which are always available. \f2  These extensions are not used by this compiler
\par \f0 
\par The character class functions are normally implemented with bitsets, with a bitset per character. For a given character, the appropriate bitset is read from a table and a test is performed as to whether a certain bit is set. Which bit is tested for is determined by the class. 
\par 
\par For the wide character classification functions this is made visible. There is a type classification type defined, a function to retrieve this value for a given class, and a function to test whether a given character is in this class, using the classification value. On top of this the normal character classification functions as used for\f1  char\f0  objects can be defined.
\par 
\par -- Data type: \b wctype_t\b0 
\par 
\par \pard\li500 The \f1 wctype_t\f0  can hold a value which represents a character class. The only defined way to generate such a value is by using the\f1  wctype\f0  function. 
\par 
\par This type is defined in \f1 wctype.h\f0 .
\par 
\par \pard -- Function: wctype_t \b wctype\b0  (\i const char *property\i0 )
\par 
\par \pard\li500 The \f1 wctype\f0  returns a value representing a class of wide characters which is identified by the string \i property\i0 . Beside some standard properties each locale can define its own ones. In case no property with the given name is known for the current locale selected for the \f1 LC_CTYPE\f0  category, the function returns zero. 
\par 
\par The properties known in every locale are: 
\par 
\par \trowd\cellx500\cellx2925\cellx5350\cellx7775\cellx9000\pard\intbl\cell\f1 "alnum"\f0  \cell\f1 "alpha"\f0  \cell\f1 "cntrl"\f0  \cell\f1 "digit"\f0\cell\row
\intbl\cell\f1 "graph"\f0  \cell\f1 "lower"\f0  \cell\f1 "print"\f0  \cell\f1 "punct"\f0\cell\row
\trowd\cellx500\cellx2925\cellx5350\cellx7775\pard\intbl\cell\f1 "space"\f0  \cell\f1 "upper"\f0  \cell\f1 "xdigit"\f0\cell\row
\pard\li500 
\par This function is declared in \f1 wctype.h\f0 .
\par 
\par \pard To test the membership of a character to one of the non-standard classes the ISO C standard defines a completely new function.
\par 
\par -- Function: int \b iswctype\b0  (\i wint_t wc, wctype_t desc\i0 )
\par 
\par \pard\li500 This function returns a nonzero value if \i wc\i0  is in the character class specified by \i desc\i0 . \i desc\i0  must previously be returned by a successful call to \f1 wctype\f0 . 
\par 
\par This function is declared in \f1 wctype.h\f0 .
\par 
\par \pard To make it easier to use the commonly-used classification functions, they are defined in the C library. There is no need to use\f1  wctype\f0  if the property string is one of the known character classes. In some situations it is desirable to construct the property strings, and then it is important that \f1 wctype\f0  can also handle the standard classes.
\par 
\par -- Function: int \b iswalnum\b0  (\i wint_t wc\i0 )
\par 
\par \pard\li500 This function returns a nonzero value if \i wc\i0  is an alphanumeric character (a letter or number); in other words, if either \f1 iswalpha\f0  or \f1 iswdigit\f0  is true of a character, then \f1 iswalnum\f0  is also true. 
\par 
\par This function can be implemented using 
\par 
\par \f1           iswctype (wc, wctype ("alnum"))
\par      
\par \f0 It is declared in \f1 wctype.h\f0 .
\par 
\par \pard -- Function: int \b iswalpha\b0  (\i wint_t wc\i0 )
\par 
\par \pard\li500 Returns true if \i wc\i0  is an alphabetic character (a letter). If\f1  iswlower\f0  or \f1 iswupper\f0  is true of a character, then\f1  iswalpha\f0  is also true. 
\par 
\par In some locales, there may be additional characters for which\f1  iswalpha\f0  is true--letters which are neither upper case nor lower case. But in the standard \f1 "C"\f0  locale, there are no such additional characters. 
\par 
\par This function can be implemented using 
\par 
\par \f1           iswctype (wc, wctype ("alpha"))
\par      
\par \f0 It is declared in \f1 wctype.h\f0 .
\par 
\par \pard -- Function: int \b iswcntrl\b0  (\i wint_t wc\i0 )
\par 
\par \pard\li500 Returns true if \i wc\i0  is a control character (that is, a character that is not a printing character). 
\par 
\par This function can be implemented using 
\par 
\par \f1           iswctype (wc, wctype ("cntrl"))
\par      
\par \f0 It is declared in \f1 wctype.h\f0 .
\par 
\par \pard -- Function: int \b iswdigit\b0  (\i wint_t wc\i0 )
\par 
\par \pard\li500 Returns true if \i wc\i0  is a digit (e.g., `\f1 0\f0 ' through `\f1 9\f0 '). Please note that this function does not only return a nonzero value for\i  decimal\i0  digits, but for all kinds of digits. A consequence is that code like the following will \b not\b0  work unconditionally for wide characters: 
\par 
\par \f1           n = 0;
\par           while (iswdigit (*wc))
\par             \{
\par               n *= 10;
\par               n += *wc++ - L'0';
\par             \}
\par      
\par \f0 This function can be implemented using 
\par 
\par \f1           iswctype (wc, wctype ("digit"))
\par      
\par \f0 It is declared in \f1 wctype.h\f0 .
\par 
\par \pard -- Function: int \b iswgraph\b0  (\i wint_t wc\i0 )
\par 
\par \pard\li500 Returns true if \i wc\i0  is a graphic character; that is, a character that has a glyph associated with it. The whitespace characters are not considered graphic. 
\par 
\par This function can be implemented using 
\par 
\par \f1           iswctype (wc, wctype ("graph"))
\par      
\par \f0 It is declared in \f1 wctype.h\f0 .
\par 
\par \pard -- Function: int \b iswlower\b0  (\i wint_t wc\i0 )
\par 
\par \pard\li500 Returns true if \i wc\i0  is a lower-case letter. The letter need not be from the Latin alphabet, any alphabet representable is valid. 
\par 
\par This function can be implemented using 
\par 
\par \f1           iswctype (wc, wctype ("lower"))
\par      
\par \f0 It is declared in \f1 wctype.h\f0 .
\par 
\par \pard -- Function: int \b iswprint\b0  (\i wint_t wc\i0 )
\par 
\par \pard\li500 Returns true if \i wc\i0  is a printing character. Printing characters include all the graphic characters, plus the space (`\f1  \f0 ') character. 
\par 
\par This function can be implemented using 
\par 
\par \f1           iswctype (wc, wctype ("print"))
\par      
\par \f0 It is declared in \f1 wctype.h\f0 .
\par 
\par \pard -- Function: int \b iswpunct\b0  (\i wint_t wc\i0 )
\par 
\par \pard\li500 Returns true if \i wc\i0  is a punctuation character. This means any printing character that is not alphanumeric or a space character. 
\par 
\par This function can be implemented using 
\par 
\par \f1           iswctype (wc, wctype ("punct"))
\par      
\par \f0 It is declared in \f1 wctype.h\f0 .
\par 
\par \pard -- Function: int \b iswspace\b0  (\i wint_t wc\i0 )
\par 
\par \pard\li500 Returns true if \i wc\i0  is a \i whitespace\i0  character. In the standard\f1  "C"\f0  locale, \f1 iswspace\f0  returns true for only the standard whitespace characters:
\par 
\par \f1 L' '\f0 
\par \pard\li1000 space 
\par \pard\li500\f1 L'\\f'\f0 
\par \pard\li1000 formfeed 
\par \pard\li500\f1 L'\\n'\f0 
\par \pard\li1000 newline 
\par \pard\li500\f1 L'\\r'\f0 
\par \pard\li1000 carriage return 
\par \pard\li500\f1 L'\\t'\f0 
\par \pard\li1000 horizontal tab 
\par \pard\li500\f1 L'\{link='\}
\par \pard\li1000 vertical tab
\par 
\par \pard\li500 This function can be implemented using 
\par 
\par           iswctype (wc, wctype ("space"))
\par      
\par It is declared in wctype.h.
\par 
\par \pard -- Function: int \b iswupper\b0  (\i wint_t wc\i0 )
\par 
\par \pard\li500 Returns true if \i wc\i0  is an upper-case letter. The letter need not be from the Latin alphabet, any alphabet representable is valid. 
\par 
\par This function can be implemented using 
\par 
\par           iswctype (wc, wctype ("upper"))
\par      
\par It is declared in wctype.h.
\par 
\par \pard -- Function: int \b iswxdigit\b0  (\i wint_t wc\i0 )
\par 
\par \pard\li500 Returns true if \i wc\i0  is a hexadecimal digit. Hexadecimal digits include the normal decimal digits `0' through `9' and the letters `A' through `F' and `a' through `f'. 
\par 
\par This function can be implemented using 
\par 
\par           iswctype (wc, wctype ("xdigit"))
\par      
\par It is declared in wctype.h.
\par 
\par }
350
PAGE_97
Cleanups on Exit
atexit;on_exit



Imported



FALSE
39
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Cleanups on Exit \{keepn\}
\par \pard\fs18 Next: \cf1\strike Aborting a Program\strike0\{linkID=20\}\cf0 , Previous: \cf1\strike Exit Status\strike0\{linkID=710\}\cf0 , Up: \cf1\strike Program Termination\strike0\{linkID=1770\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Cleanups on Exit\b0 
\par 
\par Your program can arrange to run its own cleanup functions if normal termination happens. If you are writing a library for use in various application programs, then it is unreliable to insist that all applications call the library's cleanup functions explicitly before exiting. It is much more robust to make the cleanup invisible to the application, by setting up a cleanup function in the library itself using \f1 atexit\f0  .
\par 
\par -- Function: int \b atexit\b0  (\i void \i0 (\i *function\i0 ) (\i void\i0 ))
\par 
\par \pard\li500 The \f1 atexit\f0  function registers the function \i function\i0  to be called at normal program termination. The \i function\i0  is called with no arguments. 
\par 
\par The return value from \f1 atexit\f0  is zero on success and nonzero if the function cannot be registered.
\par 
\par \pard Here's a trivial program that illustrates the use of \f1 exit\f0  and\f1  atexit\f0 :
\par 
\par \f1      #include <stdio.h>
\par      #include <stdlib.h>
\par      
\par      void
\par      bye (void)
\par      \{
\par        puts ("Goodbye, cruel world....");
\par      \}
\par      
\par      int
\par      main (void)
\par      \{
\par        atexit (bye);
\par        exit (EXIT_SUCCESS);
\par      \}
\par \f0 
\par When this program is executed, it just prints the message and exits.
\par 
\par }
360
PAGE_100
Closing Streams
closingastream;fclose;fcloseall



Imported



FALSE
33
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Closing Streams \{keepn\}
\par \pard\fs18 Next: \cf1\strike Streams and Threads\strike0\{link=PAGE_585\}\cf0 , Previous: \cf1\strike Opening Streams\strike0\{linkID=1610\}\cf0 , Up: \cf1\strike I/O on Streams\strike0\{linkID=1260\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Closing Streams\b0 
\par 
\par When a stream is closed with \f1 fclose\f0 , the connection between the stream and the file is canceled. After you have closed a stream, you cannot perform any additional operations on it.\f2 
\par \f0 
\par -- Function: int \b fclose\b0  (\i FILE *stream\i0 )
\par 
\par \pard\li500 This function causes \i stream\i0  to be closed and the connection to the corresponding file to be broken. Any buffered output is written and any buffered input is discarded. The \f1 fclose\f0  function returns a value of \f1 0\f0  if the file was closed successfully, and \f1 EOF\f0  if an error was detected. 
\par 
\par It is important to check for errors when you call \f1 fclose\f0  to close an output stream, because real, everyday errors can be detected at this time. For example, when \f1 fclose\f0  writes the remaining buffered output, it might get an error because the disk is full. Even if you know the buffer is empty, errors can still occur when closing a file if you are using NFS. 
\par 
\par The function \f1 fclose\f0  is declared in \f1 stdio.h\f0 .
\par 
\par \pard To close all streams currently available the C Library provides another function.
\par 
\par -- Function: int \b fcloseall\b0  (\i void\i0 )
\par 
\par \pard\li500 This function causes all open streams of the process to be closed and the connection to corresponding files to be broken. All buffered data is written and any buffered input is discarded. The \f1 fcloseall\f0  function returns a value of \f1 0\f0  if all the files were closed successfully, and \f1 EOF\f0  if an error was detected. 
\par 
\par This function should be used only in special situations, e.g., when an error occurred and the program must be aborted. Normally each single stream should be closed separately so that problems with individual streams can be identified. It is also problematic since the standard streams (see \cf1\strike Standard Streams\strike0\{linkID=2100\}\cf0 ) will also be closed. 
\par 
\par The function \f1 fcloseall\f0  is declared in \f1 stdio.h\f0 .
\par 
\par \pard If the \f1 main\f0  function to your program returns, or if you call the\f1  exit\f0  function (see \cf1\strike Normal Termination\strike0\{linkID=1560\}\cf0 ), all open streams are automatically closed properly. If your program terminates in any other manner, such as by calling the \f1 abort\f0  function (see \cf1\strike Aborting a Program\strike0\{linkID=20\}\cf0 ) or from a fatal signal (see \cf1\strike Signal Handling\strike0\{linkID=2010\}\cf0 ), open streams might not be closed properly. Buffered output might not be flushed and files may be incomplete. For more information on buffering of streams, see \cf1\strike Stream Buffering\strike0\{linkID=2120\}\cf0 .
\par 
\par }
370
PAGE_101
Collation Functions
collatingstrings;stringcollationfunctions;string.h;wchar.h;strcoll;wcscoll;convertingstringtocollationorder;strxfrm;wcsxfrm



Imported



FALSE
172
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Collation Functions \{keepn\}
\par \pard\fs18 Next: \cf1\strike Search Functions\strike0\{linkID=1920\}\cf0 , Previous: \cf1\strike String/Array Comparison\strike0\{linkID=2190\}\cf0 , Up: \cf1\strike String and Array Utilities\strike0\{linkID=2160\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Collation Functions\b0 
\par 
\par In some locales, the conventions for lexicographic ordering differ from the strict numeric ordering of character codes. For example, in Spanish most glyphs with diacritical marks such as accents are not considered distinct letters for the purposes of collation. On the other hand, the two-character sequence `\f1 ll\f0 ' is treated as a single letter that is collated immediately after `\f1 l\f0 '. 
\par 
\par You can use the functions \f1 strcoll\f0  and \f1 strxfrm\f0  (declared in the headers file \f1 string.h\f0 ) and \f1 wcscoll\f0  and \f1 wcsxfrm\f0  (declared in the headers file \f1 wchar\f0 ) to compare strings using a collation ordering appropriate for the current locale. The locale used by these functions in particular can be specified by setting the locale for the \f1 LC_COLLATE\f0  category; see \cf1\strike Locales\strike0\{linkID=1340\}\cf0 . In the standard C locale, the collation sequence for \f1 strcoll\f0  is the same as that for \f1 strcmp\f0 . Similarly, \f1 wcscoll\f0  and\f1  wcscmp\f0  are the same in this situation. 
\par 
\par Effectively, the way these functions work is by applying a mapping to transform the characters in a string to a byte sequence that represents the string's position in the collating sequence of the current locale. Comparing two such byte sequences in a simple fashion is equivalent to comparing the strings with the locale's collating sequence. 
\par 
\par The functions \f1 strcoll\f0  and \f1 wcscoll\f0  perform this translation implicitly, in order to do one comparison. By contrast, \f1 strxfrm\f0  and \f1 wcsxfrm\f0  perform the mapping explicitly. If you are making multiple comparisons using the same string or set of strings, it is likely to be more efficient to use \f1 strxfrm\f0  or \f1 wcsxfrm\f0  to transform all the strings just once, and subsequently compare the transformed strings with \f1 strcmp\f0  or \f1 wcscmp\f0 .
\par 
\par -- Function: int \b strcoll\b0  (\i const char *s1, const char *s2\i0 )
\par 
\par \pard\li500 The \f1 strcoll\f0  function is similar to \f1 strcmp\f0  but uses the collating sequence of the current locale for collation (the\f1  LC_COLLATE\f0  locale).
\par 
\par \pard -- Function: int \b wcscoll\b0  (\i const wchar_t *ws1, const wchar_t *ws2\i0 )
\par 
\par \pard\li500 The \f1 wcscoll\f0  function is similar to \f1 wcscmp\f0  but uses the collating sequence of the current locale for collation (the\f1  LC_COLLATE\f0  locale).
\par 
\par \pard Here is an example of sorting an array of strings, using \f1 strcoll\f0  to compare them. The actual sort algorithm is not written here; it comes from \f1 qsort\f0  (see \cf1\strike Array Sort Function\strike0\{linkID=120\}\cf0 ). The job of the code shown here is to say how to compare the strings while sorting them. (Later on in this section, we will show a way to do this more efficiently using \f1 strxfrm\f0 .)
\par 
\par \f1      /* This is the comparison function used with qsort. */
\par      
\par      int
\par      compare_elements (char **p1, char **p2)
\par      \{
\par        return strcoll (*p1, *p2);
\par      \}
\par      
\par      /* This is the entry point---the function to sort
\par         strings using the locale's collating sequence. */
\par      
\par      void
\par      sort_strings (char **array, int nstrings)
\par      \{
\par        /* Sort temp_array by comparing the strings. */
\par        qsort (array, nstrings,
\par               sizeof (char *), compare_elements);
\par      \}
\par \f0 
\par -- Function: size_t \b strxfrm\b0  (\i char *restrict to, const char *restrict from, size_t size\i0 )
\par 
\par \pard\li500 The function \f1 strxfrm\f0  transforms the string \i from\i0  using the collation transformation determined by the locale currently selected for collation, and stores the transformed string in the array \i to\i0 . Up to \i size\i0  characters (including a terminating null character) are stored. 
\par 
\par The behavior is undefined if the strings \i to\i0  and \i from\i0  overlap; see \cf1\strike Copying and Concatenation\strike0\{linkID=460\}\cf0 . 
\par 
\par The return value is the length of the entire transformed string. This value is not affected by the value of \i size\i0 , but if it is greater or equal than \i size\i0 , it means that the transformed string did not entirely fit in the array \i to\i0 . In this case, only as much of the string as actually fits was stored. To get the whole transformed string, call \f1 strxfrm\f0  again with a bigger output array. 
\par 
\par The transformed string may be longer than the original string, and it may also be shorter. 
\par 
\par If \i size\i0  is zero, no characters are stored in \i to\i0 . In this case, \f1 strxfrm\f0  simply returns the number of characters that would be the length of the transformed string. This is useful for determining what size the allocated array should be. It does not matter what\i  to\i0  is if \i size\i0  is zero; \i to\i0  may even be a null pointer.
\par 
\par \pard -- Function: size_t \b wcsxfrm\b0  (\i wchar_t *restrict wto, const wchar_t *wfrom, size_t size\i0 )
\par 
\par \pard\li500 The function \f1 wcsxfrm\f0  transforms wide character string \i wfrom\i0  using the collation transformation determined by the locale currently selected for collation, and stores the transformed string in the array\i  wto\i0 . Up to \i size\i0  wide characters (including a terminating null character) are stored. 
\par 
\par The behavior is undefined if the strings \i wto\i0  and \i wfrom\i0  overlap; see \cf1\strike Copying and Concatenation\strike0\{linkID=460\}\cf0 . 
\par 
\par The return value is the length of the entire transformed wide character string. This value is not affected by the value of \i size\i0 , but if it is greater or equal than \i size\i0 , it means that the transformed wide character string did not entirely fit in the array \i wto\i0 . In this case, only as much of the wide character string as actually fits was stored. To get the whole transformed wide character string, call\f1  wcsxfrm\f0  again with a bigger output array. 
\par 
\par The transformed wide character string may be longer than the original wide character string, and it may also be shorter. 
\par 
\par If \i size\i0  is zero, no characters are stored in \i to\i0 . In this case, \f1 wcsxfrm\f0  simply returns the number of wide characters that would be the length of the transformed wide character string. This is useful for determining what size the allocated array should be (remember to multiply with \f1 sizeof (wchar_t)\f0 ). It does not matter what\i  wto\i0  is if \i size\i0  is zero; \i wto\i0  may even be a null pointer.
\par 
\par \pard Here is an example of how you can use \f1 strxfrm\f0  when you plan to do many comparisons. It does the same thing as the previous example, but much faster, because it has to transform each string only once, no matter how many times it is compared with other strings. Even the time needed to allocate and free storage is much less than the time we save, when there are many strings.
\par 
\par \f1      struct sorter \{ char *input; char *transformed; \};
\par      
\par      /* This is the comparison function used with qsort
\par         to sort an array of struct sorter. */
\par      
\par      int
\par      compare_elements (struct sorter *p1, struct sorter *p2)
\par      \{
\par        return strcmp (p1->transformed, p2->transformed);
\par      \}
\par      
\par      /* This is the entry point---the function to sort
\par         strings using the locale's collating sequence. */
\par      
\par      void
\par      sort_strings_fast (char **array, int nstrings)
\par      \{
\par        struct sorter temp_array[nstrings];
\par        int i;
\par      
\par        /* Set up temp_array.  Each element contains
\par           one input string and its transformed string. */
\par        for (i = 0; i < nstrings; i++)
\par          \{
\par            size_t length = strlen (array[i]) * 2;
\par            char *transformed;
\par            size_t transformed_length;
\par      
\par            temp_array[i].input = array[i];
\par      
\par            /* First try a buffer perhaps big enough.  */
\par            transformed = (char *) xmalloc (length);
\par      
\par            /* Transform array[i].  */
\par            transformed_length = strxfrm (transformed, array[i], length);
\par      
\par            /* If the buffer was not large enough, resize it
\par               and try again.  */
\par            if (transformed_length >= length)
\par              \{
\par                /* Allocate the needed space. +1 for terminating
\par                   NUL character.  */
\par                transformed = (char *) xrealloc (transformed,
\par                                                 transformed_length + 1);
\par      
\par                /* The return value is not interesting because we know
\par                   how long the transformed string is.  */
\par                (void) strxfrm (transformed, array[i],
\par                                transformed_length + 1);
\par              \}
\par      
\par            temp_array[i].transformed = transformed;
\par          \}
\par      
\par        /* Sort temp_array by comparing transformed strings. */
\par        qsort (temp_array, sizeof (struct sorter),
\par               nstrings, compare_elements);
\par      
\par        /* Put the elements back in the permanent array
\par           in their sorted order. */
\par        for (i = 0; i < nstrings; i++)
\par          array[i] = temp_array[i].input;
\par      
\par        /* Free the strings we allocated. */
\par        for (i = 0; i < nstrings; i++)
\par          free (temp_array[i].transformed);
\par      \}
\par \f0 
\par The interesting part of this code for the wide character version would look like this:
\par 
\par \f1      void
\par      sort_strings_fast (wchar_t **array, int nstrings)
\par      \{
\par        ...
\par            /* Transform array[i].  */
\par            transformed_length = wcsxfrm (transformed, array[i], length);
\par      
\par            /* If the buffer was not large enough, resize it
\par               and try again.  */
\par            if (transformed_length >= length)
\par              \{
\par                /* Allocate the needed space. +1 for terminating
\par                   NUL character.  */
\par                transformed = (wchar_t *) xrealloc (transformed,
\par                                                    (transformed_length + 1)
\par                                                    * sizeof (wchar_t));
\par      
\par                /* The return value is not interesting because we know
\par                   how long the transformed string is.  */
\par                (void) wcsxfrm (transformed, array[i],
\par                                transformed_length + 1);
\par              \}
\par        ...
\par \f0 
\par Note the additional multiplication with \f1 sizeof (wchar_t)\f0  in the\f1  realloc\f0  call. 
\par 
\par \b Compatibility Note:\b0  The string collation functions are a new feature of ISO C90. Older C dialects have no equivalent feature. The wide character versions were introduced in Amendment 1 to ISO C90.
\par 
\par }
380
PAGE_104
Comparison Functions
ComparisonFunction;comparison_fn_t



Imported



FALSE
27
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Comparison Functions \{keepn\}
\par \pard\fs18 Next: \cf1\strike Array Search Function\strike0\{linkID=110\}\cf0 , Up: \cf1\strike Searching and Sorting\strike0\{linkID=1930\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Defining the Comparison Function\b0 
\par 
\par In order to use the sorted array library functions, you have to describe how to compare the elements of the array. 
\par 
\par To do this, you supply a comparison function to compare two elements of the array. The library will call this function, passing as arguments pointers to two array elements to be compared. Your comparison function should return a value the way \f1 strcmp\f0  (see \cf1\strike String/Array Comparison\strike0\{linkID=2190\}\cf0 ) does: negative if the first argument is "less" than the second, zero if they are "equal", and positive if the first argument is "greater". 
\par 
\par Here is an example of a comparison function which works with an array of numbers of type \f1 double\f0 :
\par 
\par \f1      int
\par      compare_doubles (const void *a, const void *b)
\par      \{
\par        const double *da = (const double *) a;
\par        const double *db = (const double *) b;
\par      
\par        return (*da > *db) - (*da < *db);
\par      \}
\par \f0 
\par 
\par }
390
PAGE_105
Complex Numbers
complex.h;complexnumbers;gt_Complex_I;I



Imported



FALSE
34
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Complex Numbers \{keepn\}
\par \pard\fs18 Next: \cf1\strike Operations on Complex\strike0\{linkID=1650\}\cf0 , Previous: \cf1\strike Arithmetic Functions\strike0\{linkID=90\}\cf0 , Up: \cf1\strike Arithmetic\strike0\{linkID=100\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Complex Numbers\b0 
\par 
\par ISO C99 introduces support for complex numbers in C. This is done with a new type qualifier, \f1 complex\f0 . It is a keyword if and only if \f1 complex.h\f0  has been included. There are three complex types, corresponding to the three real types: \f1 float complex\f0 ,\f1  double complex\f0 , and \f1 long double complex\f0 . 
\par 
\par To construct complex numbers you need a way to indicate the imaginary part of a number. There is no standard notation for an imaginary floating point constant. Instead, \f1 complex.h\f0  defines two macros that can be used to create complex numbers.
\par 
\par -- Macro: const float complex \b _Complex_I\b0 
\par 
\par \pard\li500 This macro is a representation of the complex number "0+1i". Multiplying a real floating-point value by \f1 _Complex_I\f0  gives a complex number whose value is purely imaginary. You can use this to construct complex constants: 
\par 
\par \f1           3.0 + 4.0i = 3.0 + 4.0 * _Complex_I
\par      
\par \f0 Note that \f1 _Complex_I * _Complex_I\f0  has the value \f1 -1\f0 , but the type of that value is \f1 complex\f0 .
\par 
\par \pard\f1 _Complex_I\f0  is a bit of a mouthful. \f1 complex.h\f0  also defines a shorter name for the same constant.
\par 
\par -- Macro: const float complex \b I\b0 
\par 
\par \pard\li500 This macro has exactly the same value as \f1 _Complex_I\f0 . Most of the time it is preferable. However, it causes problems if you want to use the identifier \f1 I\f0  for something else. You can safely write 
\par 
\par \f1           #include <complex.h>
\par           #undef I
\par      
\par \f0 if you need \f1 I\f0  for your own purposes. (In that case we recommend you also define some other short name for \f1 _Complex_I\f0 , such as\f1  J\f0 .)
\par 
\par }
400
PAGE_108
Concepts of Signals




Imported



FALSE
16
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Concepts of Signals \{keepn\}
\par \pard\fs18 Next: \cf1\strike Standard Signals\strike0\{linkID=2090\}\cf0 , Up: \cf1\strike Signal Handling\strike0\{linkID=2010\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Basic Concepts of Signals\b0 
\par 
\par This section explains basic concepts of how signals are generated, what happens after a signal is delivered, and how programs can handle signals.
\par 
\par \pard\li500\f1\'b7\f0  \cf1\strike Kinds of Signals\strike0\{linkID=1300\}\cf0 : Some examples of what can cause a signal.
\par \f1\'b7\f0  \cf1\strike Signal Generation\strike0\{linkID=2000\}\cf0 : Concepts of why and how signals occur.
\par \f1 
\par }
410
PAGE_114
Consistency Checking
consistencychecking;impossibleevents;assertions;assert.h;NDEBUG;assert;assert_perror



Imported



FALSE
39
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fmodern\fcharset0 Courier New;}{\f3\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Consistency Checking \{keepn\}
\par \pard\fs18 Next: \cf1\strike Variadic Functions\strike0\{linkID=2420\}\cf0 , Up: \cf1\strike Language Features\strike0\{linkID=1310\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Explicitly Checking Internal Consistency\b0 
\par 
\par When you're writing a program, it's often a good idea to put in checks at strategic places for "impossible" errors or violations of basic assumptions. These kinds of checks are helpful in debugging problems with the interfaces between different parts of the program, for example. 
\par 
\par The \f1 assert\f0  macro, defined in the header file \f1 assert.h\f0 , provides a convenient way to abort the program while printing a message about where in the program the error was detected. 
\par 
\par Once you think your program is debugged, you can disable the error checks performed by the \f1 assert\f0  macro by recompiling with the macro \f1 NDEBUG\f0  defined. This means you don't actually have to change the program source code to disable these checks. 
\par 
\par But disabling these consistency checks is undesirable unless they make the program significantly slower. All else being equal, more error checking is good no matter who is running the program. A wise user would rather have a program crash, visibly, than have it return nonsense without indicating anything might be wrong.
\par 
\par -- Macro: void \b assert\b0  (\i int expression\i0 )
\par 
\par \pard\li500 Verify the programmer's belief that \i expression\i0  is nonzero at this point in the program. 
\par 
\par If \f1 NDEBUG\f0  is not defined, \f1 assert\f0  tests the value of\i  expression\i0 . If it is false (zero), \f1 assert\f0  aborts the program (see \cf1\strike Aborting a Program\strike0\{linkID=20\}\cf0 ) after printing a message of the form: 
\par 
\par \f1           Assertion failed\f2  file(function:linenum) : expression\f1 .
\par     
\par \f0 on the standard error stream \f1 stderr\f0  (see \cf1\strike Standard Streams\strike0\{linkID=2100\}\cf0 ). The filename and line number are taken from the C preprocessor macros\f1  __FILE__\f0  and \f1 __LINE__\f0  and specify where the call to\f1  assert\f0  was made. When using the\f3  \f0 C compiler, the name of the function which calls \f1 assert\f0  is taken from the built-in variable\f3  __func__.  When C99 mode is not enabled, the function name and following colon are not present.\f0 
\par 
\par If the preprocessor macro \f1 NDEBUG\f0  is defined before\f1  assert.h\f0  is included, the \f1 assert\f0  macro is defined to do absolutely nothing. 
\par 
\par \b Warning:\b0  Even the argument expression \i expression\i0  is not evaluated if \f1 NDEBUG\f0  is in effect. So never use \f1 assert\f0  with arguments that involve side effects. For example, \f1 assert (++i > 0);\f0  is a bad idea, because \f1 i\f0  will not be incremented if\f1  NDEBUG\f0  is defined.
\par 
\par \pard\b Usage note:\b0  The \f1 assert\f0  facility is designed for detecting \i internal inconsistency\i0 ; it is not suitable for reporting invalid input or improper usage by the \i user\i0  of the program. 
\par 
\par The information in the diagnostic messages printed by the \f1 assert\f0  macro is intended to help you, the programmer, track down the cause of a bug, but is not really useful for telling a user of your program why his or her input was invalid or why a command could not be carried out. What's more, your program should not abort when given invalid input, as \f1 assert\f0  would do--it should exit with nonzero status (see \cf1\strike Exit Status\strike0\{linkID=710\}\cf0 ) after printing its error messages, or perhaps read another command or move on to the next input file. 
\par 
\par See \cf1\strike Error Messages\strike0\{linkID=670\}\cf0 , for information on printing error messages for problems that \i do not\i0  represent bugs in the program.
\par 
\par }
420
PAGE_118
Control Functions
fegetenv;feholdexcept;FE_DFL_ENV;FE_NOMASK_ENV;fesetenv;feupdateenv;feenableexcept;fedisableexcept;fegetexcept



Imported



FALSE
75
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fswiss\fcharset0 Arial;}{\f2\fmodern Courier New;}{\f3\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Contro\f1 l\f0  Functionsl \{keepn\}
\par \pard\fs18 Next: \cf1\strike Arithmetic Functions\strike0\{linkID=90\}\cf0 , Previous: \cf1\strike Rounding\strike0\{linkID=1900\}\cf0 , Up: \cf1\strike Arithmetic\strike0\{linkID=100\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Floating-Point Control Functions\b0 
\par 
\par IEEE 754 floating-point implementations allow the programmer to decide whether traps will occur for each of the exceptions, by setting bits in the \i control word\i0 . In C, traps result in the program receiving the \f2 SIGFPE\f0  signal; see \cf1\strike Signal Handling\strike0\{linkID=2010\}\cf0 . 
\par 
\par \b Note:\b0  IEEE 754 says that trap handlers are given details of the exceptional situation, and can set the result value. C signals do not provide any mechanism to pass this information back and forth. Trapping exceptions in C is therefore not very useful. 
\par 
\par It is sometimes necessary to save the state of the floating-point unit while you perform some calculation. The library provides functions which save and restore the exception flags, the set of exceptions that generate traps, and the rounding mode. This information is known as the\i  floating-point environment\i0 . 
\par 
\par The functions to save and restore the floating-point environment all use a variable of type \f2 fenv_t\f0  to store information. This type is defined in \f2 fenv.h\f0 . Its size and contents are implementation-defined. You should not attempt to manipulate a variable of this type directly. 
\par 
\par To save the state of the FPU, use one of these functions:
\par 
\par -- Function: int \b fegetenv\b0  (\i fenv_t *envp\i0 )
\par 
\par \pard\li500 Store the floating-point environment in the variable pointed to by\i  envp\i0 . 
\par 
\par The function returns zero in case the operation was successful, a non-zero value otherwise.
\par 
\par \pard -- Function: int \b feholdexcept\b0  (\i fenv_t *envp\i0 )
\par 
\par \pard\li500 Store the current floating-point environment in the object pointed to by\i  envp\i0 . Then clear all exception flags, and set the FPU to trap no exceptions. Not all FPUs support trapping no exceptions; if\f2  feholdexcept\f0  cannot set this mode, it returns nonzero value. If it succeeds, it returns zero.
\par 
\par \pard The functions which restore the floating-point environment can take these kinds of arguments:
\par 
\par \pard\li500\f3\'b7\f0  Pointers to \f2 fenv_t\f0  objects, which were initialized previously by a call to \f2 fegetenv\f0  or \f2 feholdexcept\f0 .
\par \f3\'b7\f0  The special macro \f2 FE_DFL_ENV\f0  which represents the floating-point environment as it was available at program start.
\par \f3\'b7\f0  Implementation defined macros with names starting with \f2 FE_\f0  and having type \f2 fenv_t *\f0 . 
\par 
\par Some platforms might define other predefined environments.
\par 
\par \pard To set the floating-point environment, you can use either of these functions:
\par 
\par -- Function: int \b fesetenv\b0  (\i const fenv_t *envp\i0 )
\par 
\par \pard\li500 Set the floating-point environment to that described by \i envp\i0 . 
\par 
\par The function returns zero in case the operation was successful, a non-zero value otherwise.
\par 
\par \pard -- Function: int \b feupdateenv\b0  (\i const fenv_t *envp\i0 )
\par 
\par \pard\li500 Like \f2 fesetenv\f0 , this function sets the floating-point environment to that described by \i envp\i0 . However, if any exceptions were flagged in the status word before \f2 feupdateenv\f0  was called, they remain flagged after the call. In other words, after \f2 feupdateenv\f0  is called, the status word is the bitwise OR of the previous status word and the one saved in \i envp\i0 . 
\par 
\par The function returns zero in case the operation was successful, a non-zero value otherwise.
\par 
\par \pard -- Function: \f1 void\f0  \b fe\f1 clearexcept\b0\f0  (\i\f1 int exceptflags\i0\f0 )
\par 
\par \pard\li500\f1 This function clears specific  flags related to a floating point exception.
\par \f0 
\par \pard -- Function: \f1 void\f0  \b fe\f1 raiseexcept\b0\f0  (\i\f1 int exceptflags\i0\f0 )
\par 
\par \pard\li500\f1 This function causes the floating point environment to raise the exceptions corresponding to the flags.
\par \f0 
\par \pard -- Function: \f1 void\f0  \b fe\f1 testexcept\b0\f0  (\i\f1 int exceptflags\i0\f0 )
\par 
\par \pard\li500\f1 This function tests the floating point environment to see if the exceptions corresponding to the flags have been raised.
\par \f0 
\par \f1 The function returns a bit mask including flags for the exceptions that have been raised.
\par 
\par \pard\f0 -- Function: \f1 void\f0  \b fe\f1 getexceptflag\b0\f0  (\i\f1 fexcept_t *flagp\i0 , \i int exceptflags\i0\f0 )
\par 
\par \pard\li500\f1 This function gets the state of the exceptions indicated by \i exceptflags\i0 , and stores it in \i flagp\i0 .
\par \f0 
\par \pard -- Function: \f1 void\f0  \b fe\f1 setexceptflag\b0\f0  (\i\f1 fexcept_t *flagp\i0 , \i int exceptflags\i0\f0 )
\par 
\par \pard\li500\f1 This function sets the internal state of the exception flags in \i flagp\i0  as masked by \i exceptflags\i0 .  It does not change the actual state of the floating point unit.
\par \f0 
\par }
430
PAGE_121
Controlling Buffering
buffering,controlling;stdio.h;setvbuf;gt_IOFBF;gt_IOLBF;gt_IONBF;BUFSIZ;setbuf;setbuffer;setlinebuf;gt__flbf;gt__fbufsize;gt__fpending



Imported



FALSE
51
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Controlling Buffering \{keepn\}
\par \pard\fs18 Previous: \cf1\strike Flushing Buffers\strike0\{linkID=910\}\cf0 , Up: \cf1\strike Stream Buffering\strike0\{linkID=2120\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Controlling Which Kind of Buffering\b0 
\par 
\par After opening a stream (but before any other operations have been performed on it), you can explicitly specify what kind of buffering you want it to have using the \f1 setvbuf\f0  function. The facilities listed in this section are declared in the header file \f1 stdio.h\f0 .
\par 
\par -- Function: int \b setvbuf\b0  (\i FILE *stream, char *buf, int mode, size_t size\i0 )
\par 
\par \pard\li500 This function is used to specify that the stream \i stream\i0  should have the buffering mode \i mode\i0 , which can be either \f1 _IOFBF\f0  (for full buffering), \f1 _IOLBF\f0  (for line buffering), or\f1  _IONBF\f0  (for unbuffered input/output). 
\par 
\par If you specify a null pointer as the \i buf\i0  argument, then \f1 setvbuf\f0  allocates a buffer itself using \f1 malloc\f0 . This buffer will be freed when you close the stream. 
\par 
\par Otherwise, \i buf\i0  should be a character array that can hold at least\i  size\i0  characters. You should not free the space for this array as long as the stream remains open and this array remains its buffer. You should usually either allocate it statically, or \f1 malloc\f0  (see \cf1\strike Unconstrained Allocation\strike0\{linkID=2340\}\cf0 ) the buffer. Using an automatic array is not a good idea unless you close the file before exiting the block that declares the array. 
\par 
\par While the array remains a stream buffer, the stream I/O functions will use the buffer for their internal purposes. You shouldn't try to access the values in the array directly while the stream is using it for buffering. 
\par 
\par The \f1 setvbuf\f0  function returns zero on success, or a nonzero value if the value of \i mode\i0  is not valid or if the request could not be honored.
\par 
\par \pard -- Macro: int \b _IOFBF\b0 
\par 
\par \pard\li500 The value of this macro is an integer constant expression that can be used as the \i mode\i0  argument to the \f1 setvbuf\f0  function to specify that the stream should be fully buffered.
\par 
\par \pard -- Macro: int \b _IOLBF\b0 
\par 
\par \pard\li500 The value of this macro is an integer constant expression that can be used as the \i mode\i0  argument to the \f1 setvbuf\f0  function to specify that the stream should be line buffered.
\par 
\par \pard -- Macro: int \b _IONBF\b0 
\par 
\par \pard\li500 The value of this macro is an integer constant expression that can be used as the \i mode\i0  argument to the \f1 setvbuf\f0  function to specify that the stream should be unbuffered.
\par 
\par \pard -- Macro: int \b BUFSIZ\b0 
\par 
\par \pard\li500 The value of this macro is an integer constant expression that is good to use for the \i size\i0  argument to \f1 setvbuf\f0 . This value is guaranteed to be at least \f1 256\f0 . 
\par 
\par The value of \f1 BUFSIZ\f0  is chosen on each system so as to make stream I/O efficient. So it is a good idea to use \f1 BUFSIZ\f0  as the size for the buffer when you call \f1 setvbuf\f0 . 
\par 
\par Sometimes people also use \f1 BUFSIZ\f0  as the allocation size of buffers used for related purposes, such as strings used to receive a line of input with \f1 fgets\f0  (see \cf1\strike Character Input\strike0\{linkID=280\}\cf0 ). There is no particular reason to use \f1 BUFSIZ\f0  for this instead of any other integer, except that it might lead to doing I/O in chunks of an efficient size.
\par 
\par \pard -- Function: void \b setbuf\b0  (\i FILE *stream, char *buf\i0 )
\par 
\par \pard\li500 If \i buf\i0  is a null pointer, the effect of this function is equivalent to calling \f1 setvbuf\f0  with a \i mode\i0  argument of\f1  _IONBF\f0 . Otherwise, it is equivalent to calling \f1 setvbuf\f0  with \i buf\i0 , and a \i mode\i0  of \f1 _IOFBF\f0  and a \i size\i0  argument of \f1 BUFSIZ\f0 . 
\par 
\par The \f1 setbuf\f0  function is provided for compatibility with old code; use \f1 setvbuf\f0  in all new programs.
\par 
\par }
440
PAGE_124
Converting a Character
btowc;wchar.h;wctob;wchar.h;mbrtowc;stateful;wchar.h;mbrlen;wchar.h;wcrtomb;wchar.h



Imported



FALSE
213
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Converting a Character \{keepn\}
\par \pard\fs18 Next: \cf1\strike Converting Strings\strike0\{linkID=450\}\cf0 , Previous: \cf1\strike Keeping the state\strike0\{linkID=1290\}\cf0 , Up: \cf1\strike Restartable multibyte conversion\strike0\{linkID=1870\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Converting Single Characters\b0 
\par 
\par The most fundamental of the conversion functions are those dealing with single characters. Please note that this does not always mean single bytes. But since there is very often a subset of the multibyte character set that consists of single byte sequences, there are functions to help with converting bytes. Frequently, ASCII is a subpart of the multibyte character set. In such a scenario, each ASCII character stands for itself, and all other characters have at least a first byte that is beyond the range 0 to 127.
\par 
\par -- Function: wint_t \b btowc\b0  (\i int c\i0 )
\par 
\par \pard\li500 The \f1 btowc\f0  function ("byte to wide character") converts a valid single byte character \i c\i0  in the initial shift state into the wide character equivalent using the conversion rules from the currently selected locale of the \f1 LC_CTYPE\f0  category. 
\par 
\par If \f1 (unsigned char) \i\f0 c\i0  is no valid single byte multibyte character or if \i c\i0  is \f1 EOF\f0 , the function returns \f1 WEOF\f0 . 
\par 
\par Please note the restriction of \i c\i0  being tested for validity only in the initial shift state. No \f1 mbstate_t\f0  object is used from which the state information is taken, and the function also does not use any static state. 
\par 
\par The \f1 btowc\f0  function was introduced in Amendment 1 to ISO C90 and is declared in \f1 wchar.h\f0 .
\par 
\par \pard Despite the limitation that the single byte value always is interpreted in the initial state this function is actually useful most of the time. Most characters are either entirely single-byte character sets or they are extension to ASCII. But then it is possible to write code like this (not that this specific example is very useful):
\par 
\par \f1      wchar_t *
\par      itow (unsigned long int val)
\par      \{
\par        static wchar_t buf[30];
\par        wchar_t *wcp = &buf[29];
\par        *wcp = L'\\0';
\par        while (val != 0)
\par          \{
\par            *--wcp = btowc ('0' + val % 10);
\par            val /= 10;
\par          \}
\par        if (wcp == &buf[29])
\par          *--wcp = L'0';
\par        return wcp;
\par      \}
\par \f0 
\par Why is it necessary to use such a complicated implementation and not simply cast \f1 '0' + val % 10\f0  to a wide character? The answer is that there is no guarantee that one can perform this kind of arithmetic on the character of the character set used for \f1 wchar_t\f0  representation. In other situations the bytes are not constant at compile time and so the compiler cannot do the work. In situations like this it is necessary \f1 btowc\f0 .
\par 
\par There also is a function for the conversion in the other direction.
\par 
\par -- Function: int \b wctob\b0  (\i wint_t c\i0 )
\par 
\par \pard\li500 The \f1 wctob\f0  function ("wide character to byte") takes as the parameter a valid wide character. If the multibyte representation for this character in the initial state is exactly one byte long, the return value of this function is this character. Otherwise the return value is\f1  EOF\f0 . 
\par 
\par \f1 wctob\f0  was introduced in Amendment 1 to ISO C90 and is declared in \f1 wchar.h\f0 .
\par 
\par \pard There are more general functions to convert single character from multibyte representation to wide characters and vice versa. These functions pose no limit on the length of the multibyte representation and they also do not require it to be in the initial state.
\par 
\par -- Function: size_t \b mbrtowc\b0  (\i wchar_t *restrict pwc, const char *restrict s, size_t n, mbstate_t *restrict ps\i0 )
\par 
\par \pard\li500 The \f1 mbrtowc\f0  function ("multibyte restartable to wide character") converts the next multibyte character in the string pointed to by \i s\i0  into a wide character and stores it in the wide character string pointed to by \i pwc\i0 . The conversion is performed according to the locale currently selected for the \f1 LC_CTYPE\f0  category. If the conversion for the character set used in the locale requires a state, the multibyte string is interpreted in the state represented by the object pointed to by \i ps\i0 . If \i ps\i0  is a null pointer, a static, internal state variable used only by the \f1 mbrtowc\f0  function is used. 
\par 
\par If the next multibyte character corresponds to the NUL wide character, the return value of the function is 0 and the state object is afterwards in the initial state. If the next \i n\i0  or fewer bytes form a correct multibyte character, the return value is the number of bytes starting from \i s\i0  that form the multibyte character. The conversion state is updated according to the bytes consumed in the conversion. In both cases the wide character (either the \f1 L'\\0'\f0  or the one found in the conversion) is stored in the string pointed to by \i pwc\i0  if \i pwc\i0  is not null. 
\par 
\par If the first \i n\i0  bytes of the multibyte string possibly form a valid multibyte character but there are more than \i n\i0  bytes needed to complete it, the return value of the function is \f1 (size_t) -2\f0  and no value is stored. Please note that this can happen even if \i n\i0  has a value greater than or equal to \f1 MB_CUR_MAX\f0  since the input might contain redundant shift sequences. 
\par 
\par If the first \f1 n\f0  bytes of the multibyte string cannot possibly form a valid multibyte character, no value is stored, the global variable\f1  errno\f0  is set to the value \f1 EILSEQ\f0 , and the function returns\f1  (size_t) -1\f0 . The conversion state is afterwards undefined. 
\par 
\par \f1 mbrtowc\f0  was introduced in Amendment 1 to ISO C90 and is declared in \f1 wchar.h\f0 .
\par 
\par \pard Use of \f1 mbrtowc\f0  is straightforward. A function that copies a multibyte string into a wide character string while at the same time converting all lowercase characters into uppercase could look like this (this is not the final version, just an example; it has no error checking, and sometimes leaks memory):
\par 
\par \f1      wchar_t *
\par      mbstouwcs (const char *s)
\par      \{
\par        size_t len = strlen (s);
\par        wchar_t *result = malloc ((len + 1) * sizeof (wchar_t));
\par        wchar_t *wcp = result;
\par        wchar_t tmp[1];
\par        mbstate_t state;
\par        size_t nbytes;
\par      
\par        memset (&state, '\\0', sizeof (state));
\par        while ((nbytes = mbrtowc (tmp, s, len, &state)) > 0)
\par          \{
\par            if (nbytes >= (size_t) -2)
\par              /* Invalid input string.  */
\par              return NULL;
\par            *result++ = towupper (tmp[0]);
\par            len -= nbytes;
\par            s += nbytes;
\par          \}
\par        return result;
\par      \}
\par \f0 
\par The use of \f1 mbrtowc\f0  should be clear. A single wide character is stored in \i tmp\i0\f1 [0]\f0 , and the number of consumed bytes is stored in the variable \i nbytes\i0 . If the conversion is successful, the uppercase variant of the wide character is stored in the \i result\i0  array and the pointer to the input string and the number of available bytes is adjusted. 
\par 
\par The only non-obvious thing about \f1 mbrtowc\f0  might be the way memory is allocated for the result. The above code uses the fact that there can never be more wide characters in the converted results than there are bytes in the multibyte input string. This method yields a pessimistic guess about the size of the result, and if many wide character strings have to be constructed this way or if the strings are long, the extra memory required to be allocated because the input string contains multibyte characters might be significant. The allocated memory block can be resized to the correct size before returning it, but a better solution might be to allocate just the right amount of space for the result right away. Unfortunately there is no function to compute the length of the wide character string directly from the multibyte string. There is, however, a function that does part of the work.
\par -- Function: size_t \b mbrlen\b0  (\i const char *restrict s, size_t n, mbstate_t *ps\i0 )
\par 
\par \pard\li500 The \f1 mbrlen\f0  function ("multibyte restartable length") computes the number of at most \i n\i0  bytes starting at \i s\i0 , which form the next valid and complete multibyte character. 
\par 
\par If the next multibyte character corresponds to the NUL wide character, the return value is 0. If the next \i n\i0  bytes form a valid multibyte character, the number of bytes belonging to this multibyte character byte sequence is returned. 
\par 
\par If the the first \i n\i0  bytes possibly form a valid multibyte character but the character is incomplete, the return value is\f1  (size_t) -2\f0 . Otherwise the multibyte character sequence is invalid and the return value is \f1 (size_t) -1\f0 . 
\par 
\par The multibyte sequence is interpreted in the state represented by the object pointed to by \i ps\i0 . If \i ps\i0  is a null pointer, a state object local to \f1 mbrlen\f0  is used. 
\par 
\par \f1 mbrlen\f0  was introduced in Amendment 1 to ISO C90 and is declared in \f1 wchar.h\f0 .
\par 
\par \pard The attentive reader now will note that \f1 mbrlen\f0  can be implemented as
\par 
\par \f1      mbrtowc (NULL, s, n, ps != NULL ? ps : &internal)
\par \f0 
\par This is true and in fact is mentioned in the official specification. How can this function be used to determine the length of the wide character string created from a multibyte character string? It is not directly usable, but we can define a function \f1 mbslen\f0  using it:
\par 
\par \f1      size_t
\par      mbslen (const char *s)
\par      \{
\par        mbstate_t state;
\par        size_t result = 0;
\par        size_t nbytes;
\par        memset (&state, '\\0', sizeof (state));
\par        while ((nbytes = mbrlen (s, MB_LEN_MAX, &state)) > 0)
\par          \{
\par            if (nbytes >= (size_t) -2)
\par              /* Something is wrong.  */
\par              return (size_t) -1;
\par            s += nbytes;
\par            ++result;
\par          \}
\par        return result;
\par      \}
\par \f0 
\par This function simply calls \f1 mbrlen\f0  for each multibyte character in the string and counts the number of function calls. Please note that we here use \f1 MB_LEN_MAX\f0  as the size argument in the \f1 mbrlen\f0  call. This is acceptable since a) this value is larger then the length of the longest multibyte character sequence and b) we know that the string\i  s\i0  ends with a NUL byte, which cannot be part of any other multibyte character sequence but the one representing the NUL wide character. Therefore, the \f1 mbrlen\f0  function will never read invalid memory. 
\par 
\par Now that this function is available (just to make this clear, this function is \i not\i0  part of the C library) we can compute the number of wide character required to store the converted multibyte character string \i s\i0  using
\par 
\par \f1      wcs_bytes = (mbslen (s) + 1) * sizeof (wchar_t);
\par \f0 
\par Please note that the \f1 mbslen\f0  function is quite inefficient. The implementation of \f1 mbstouwcs\f0  with \f1 mbslen\f0  would have to perform the conversion of the multibyte character input string twice, and this conversion might be quite expensive. So it is necessary to think about the consequences of using the easier but imprecise method before doing the work twice.
\par 
\par -- Function: size_t \b wcrtomb\b0  (\i char *restrict s, wchar_t wc, mbstate_t *restrict ps\i0 )
\par 
\par \pard\li500 The \f1 wcrtomb\f0  function ("wide character restartable to multibyte") converts a single wide character into a multibyte string corresponding to that wide character. 
\par 
\par If \i s\i0  is a null pointer, the function resets the state stored in the objects pointed to by \i ps\i0  (or the internal \f1 mbstate_t\f0  object) to the initial state. This can also be achieved by a call like this: 
\par 
\par \f1           wcrtombs (temp_buf, L'\\0', ps)
\par      
\par \f0 since, if \i s\i0  is a null pointer, \f1 wcrtomb\f0  performs as if it writes into an internal buffer, which is guaranteed to be large enough. 
\par 
\par If \i wc\i0  is the NUL wide character, \f1 wcrtomb\f0  emits, if necessary, a shift sequence to get the state \i ps\i0  into the initial state followed by a single NUL byte, which is stored in the string\i  s\i0 . 
\par 
\par Otherwise a byte sequence (possibly including shift sequences) is written into the string \i s\i0 . This only happens if \i wc\i0  is a valid wide character (i.e., it has a multibyte representation in the character set selected by locale of the \f1 LC_CTYPE\f0  category). If \i wc\i0  is no valid wide character, nothing is stored in the strings \i s\i0 ,\f1  errno\f0  is set to \f1 EILSEQ\f0 , the conversion state in \i ps\i0  is undefined and the return value is \f1 (size_t) -1\f0 . 
\par 
\par If no error occurred the function returns the number of bytes stored in the string \i s\i0 . This includes all bytes representing shift sequences. 
\par 
\par One word about the interface of the function: there is no parameter specifying the length of the array \i s\i0 . Instead the function assumes that there are at least \f1 MB_CUR_MAX\f0  bytes available since this is the maximum length of any byte sequence representing a single character. So the caller has to make sure that there is enough space available, otherwise buffer overruns can occur. 
\par 
\par \f1 wcrtomb\f0  was introduced in Amendment 1 to ISO C90 and is declared in \f1 wchar.h\f0 .
\par 
\par \pard Using \f1 wcrtomb\f0  is as easy as using \f1 mbrtowc\f0 . The following example appends a wide character string to a multibyte character string. Again, the code is not really useful (or correct), it is simply here to demonstrate the use and some problems.
\par 
\par \f1      char *
\par      mbscatwcs (char *s, size_t len, const wchar_t *ws)
\par      \{
\par        mbstate_t state;
\par        /* Find the end of the existing string.  */
\par        char *wp = strchr (s, '\\0');
\par        len -= wp - s;
\par        memset (&state, '\\0', sizeof (state));
\par        do
\par          \{
\par            size_t nbytes;
\par            if (len < MB_CUR_LEN)
\par              \{
\par                /* We cannot guarantee that the next
\par                   character fits into the buffer, so
\par                   return an error.  */
\par                errno = E2BIG;
\par                return NULL;
\par              \}
\par            nbytes = wcrtomb (wp, *ws, &state);
\par            if (nbytes == (size_t) -1)
\par              /* Error in the conversion.  */
\par              return NULL;
\par            len -= nbytes;
\par            wp += nbytes;
\par          \}
\par        while (*ws++ != L'\\0');
\par        return s;
\par      \}
\par \f0 
\par First the function has to find the end of the string currently in the array \i s\i0 . The \f1 strchr\f0  call does this very efficiently since a requirement for multibyte character representations is that the NUL byte is never used except to represent itself (and in this context, the end of the string). 
\par 
\par After initializing the state object the loop is entered where the first task is to make sure there is enough room in the array \i s\i0 . We abort if there are not at least \f1 MB_CUR_LEN\f0  bytes available. This is not always optimal but we have no other choice. We might have less than \f1 MB_CUR_LEN\f0  bytes available but the next multibyte character might also be only one byte long. At the time the \f1 wcrtomb\f0  call returns it is too late to decide whether the buffer was large enough. If this solution is unsuitable, there is a very slow but more accurate solution.
\par 
\par \f1        ...
\par        if (len < MB_CUR_LEN)
\par          \{
\par            mbstate_t temp_state;
\par            memcpy (&temp_state, &state, sizeof (state));
\par            if (wcrtomb (NULL, *ws, &temp_state) > len)
\par              \{
\par                /* We cannot guarantee that the next
\par                   character fits into the buffer, so
\par                   return an error.  */
\par                errno = E2BIG;
\par                return NULL;
\par              \}
\par          \}
\par        ...
\par \f0 
\par Here we perform the conversion that might overflow the buffer so that we are afterwards in the position to make an exact decision about the buffer size. Please note the \f1 NULL\f0  argument for the destination buffer in the new \f1 wcrtomb\f0  call; since we are not interested in the converted text at this point, this is a nice way to express this. The most unusual thing about this piece of code certainly is the duplication of the conversion state object, but if a change of the state is necessary to emit the next multibyte character, we want to have the same shift state change performed in the real conversion. Therefore, we have to preserve the initial shift state information. 
\par 
\par There are certainly many more and even better solutions to this problem. This example is only provided for educational purposes.
\par 
\par }
450
PAGE_125
Converting Strings
mbsrtowcs;wchar.h;stateful;wcsrtombs;wchar.h;mbsnrtowcs;wcsnrtombs



Imported



FALSE
46
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Converting Strings \{keepn\}
\par \pard\fs18 Next: \cf1\strike Multibyte Conversion Example\strike0\{linkID=1470\}\cf0 , Previous: \cf1\strike Converting a Character\strike0\{linkID=440\}\cf0 , Up: \cf1\strike Restartable multibyte conversion\strike0\{linkID=1870\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Converting Multibyte and Wide Character Strings\b0 
\par 
\par The functions described in the previous section only convert a single character at a time. Most operations to be performed in real-world programs include strings and therefore the ISO C standard also defines conversions on entire strings. 
\par 
\par -- Function: size_t \b mbsrtowcs\b0  (\i wchar_t *restrict dst, const char **restrict src, size_t len, mbstate_t *restrict ps\i0 )
\par 
\par \pard\li500 The \f1 mbsrtowcs\f0  function ("multibyte string restartable to wide character string") converts an NUL-terminated multibyte character string at \f1 *\i\f0 src\i0  into an equivalent wide character string, including the NUL wide character at the end. The conversion is started using the state information from the object pointed to by \i ps\i0  or from an internal object of \f1 mbsrtowcs\f0  if \i ps\i0  is a null pointer. Before returning, the state object is updated to match the state after the last converted character. The state is the initial state if the terminating NUL byte is reached and converted. 
\par 
\par If \i dst\i0  is not a null pointer, the result is stored in the array pointed to by \i dst\i0 ; otherwise, the conversion result is not available since it is stored in an internal buffer. 
\par 
\par If \i len\i0  wide characters are stored in the array \i dst\i0  before reaching the end of the input string, the conversion stops and \i len\i0  is returned. If \i dst\i0  is a null pointer, \i len\i0  is never checked. 
\par 
\par Another reason for a premature return from the function call is if the input string contains an invalid multibyte sequence. In this case the global variable \f1 errno\f0  is set to \f1 EILSEQ\f0  and the function returns \f1 (size_t) -1\f0 .
\par 
\par In all other cases the function returns the number of wide characters converted during this call. If \i dst\i0  is not null, \f1 mbsrtowcs\f0  stores in the pointer pointed to by \i src\i0  either a null pointer (if the NUL byte in the input string was reached) or the address of the byte following the last converted multibyte character. 
\par 
\par \f1 mbsrtowcs\f0  was introduced in Amendment 1 to ISO C90 and is declared in \f1 wchar.h\f0 .
\par 
\par \pard The definition of the \f1 mbsrtowcs\f0  function has one important limitation. The requirement that \i dst\i0  has to be a NUL-terminated string provides problems if one wants to convert buffers with text. A buffer is normally no collection of NUL-terminated strings but instead a continuous collection of lines, separated by newline characters. Now assume that a function to convert one line from a buffer is needed. Since the line is not NUL-terminated, the source pointer cannot directly point into the unmodified text buffer. This means, either one inserts the NUL byte at the appropriate place for the time of the \f1 mbsrtowcs\f0  function call (which is not doable for a read-only buffer or in a multi-threaded application) or one copies the line in an extra buffer where it can be terminated by a NUL byte. Note that it is not in general possible to limit the number of characters to convert by setting the parameter \i len\i0  to any specific value. Since it is not known how many bytes each multibyte character sequence is in length, one can only guess. 
\par 
\par There is still a problem with the method of NUL-terminating a line right after the newline character, which could lead to very strange results. As said in the description of the \f1 mbsrtowcs\f0  function above the conversion state is guaranteed to be in the initial shift state after processing the NUL byte at the end of the input string. But this NUL byte is not really part of the text (i.e., the conversion state after the newline in the original text could be something different than the initial shift state and therefore the first character of the next line is encoded using this state). But the state in question is never accessible to the user since the conversion stops after the NUL byte (which resets the state). Most stateful character sets in use today require that the shift state after a newline be the initial state-but this is not a strict guarantee. Therefore, simply NUL-terminating a piece of a running text is not always an adequate solution and, therefore, should never be used in generally used code. 
\par 
\par -- Function: size_t \b wcsrtombs\b0  (\i char *restrict dst, const wchar_t **restrict src, size_t len, mbstate_t *restrict ps\i0 )
\par 
\par \pard\li500 The \f1 wcsrtombs\f0  function ("wide character string restartable to multibyte string") converts the NUL-terminated wide character string at\f1  *\i\f0 src\i0  into an equivalent multibyte character string and stores the result in the array pointed to by \i dst\i0 . The NUL wide character is also converted. The conversion starts in the state described in the object pointed to by \i ps\i0  or by a state object locally to \f1 wcsrtombs\f0  in case \i ps\i0  is a null pointer. If\i  dst\i0  is a null pointer, the conversion is performed as usual but the result is not available. If all characters of the input string were successfully converted and if \i dst\i0  is not a null pointer, the pointer pointed to by \i src\i0  gets assigned a null pointer. 
\par 
\par If one of the wide characters in the input string has no valid multibyte character equivalent, the conversion stops early, sets the global variable \f1 errno\f0  to \f1 EILSEQ\f0 , and returns \f1 (size_t) -1\f0 . 
\par 
\par Another reason for a premature stop is if \i dst\i0  is not a null pointer and the next converted character would require more than\i  len\i0  bytes in total to the array \i dst\i0 . In this case (and if\i  dest\i0  is not a null pointer) the pointer pointed to by \i src\i0  is assigned a value pointing to the wide character right after the last one successfully converted. 
\par 
\par Except in the case of an encoding error the return value of the\f1  wcsrtombs\f0  function is the number of bytes in all the multibyte character sequences stored in \i dst\i0 . Before returning the state in the object pointed to by \i ps\i0  (or the internal object in case\i  ps\i0  is a null pointer) is updated to reflect the state after the last conversion. The state is the initial shift state in case the terminating NUL wide character was converted. 
\par 
\par The \f1 wcsrtombs\f0  function was introduced in Amendment 1 to ISO C90 and is declared in \f1 wchar.h\f0 .
\par 
\par \pard The restriction mentioned above for the \f1 mbsrtowcs\f0  function applies here also. There is no possibility of directly controlling the number of input characters. One has to place the NUL wide character at the correct place or control the consumed input indirectly via the available output array size (the \i len\i0  parameter).
\par 
\par 
\par }
460
PAGE_126
Copying and Concatenation
string.h;wchar.h;copyingstringsandarrays;stringcopyfunctions;arraycopyfunctions;concatenatingstrings;stringconcatenationfunctions;memcpy;wmemcpy;mempcpy;wmempcpy;memmove;wmemmove;memccpy;memset;wmemset;strcpy;wcscpy;strncpy;wcsncpy;strdup;wcsdup;strndup;stpcpy;wcpcpy;stpncpy;wcpncpy;strdupa;strndupa;strcat;wcscat;gt__va_copy;va_copy;strncat;wcsncat;bcopy;bzero



Imported



FALSE
300
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Copying and Concatenation \{keepn\}
\par \pard\fs18 Next: \cf1\strike String/Array Comparison\strike0\{linkID=2190\}\cf0 , Previous: \cf1\strike String Length\strike0\{linkID=2180\}\cf0 , Up: \cf1\strike String and Array Utilities\strike0\{linkID=2160\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Copying and Concatenation\b0 
\par 
\par You can use the functions described in this section to copy the contents of strings and arrays, or to append the contents of one string to another. The `\f1 str\f0 ' and `\f1 mem\f0 ' functions are declared in the header file \f1 string.h\f0  while the `\f1 wstr\f0 ' and `\f1 wmem\f0 ' functions are declared in the file \f1 wchar.h\f0 . A helpful way to remember the ordering of the arguments to the functions in this section is that it corresponds to an assignment expression, with the destination array specified to the left of the source array. All of these functions return the address of the destination array. 
\par 
\par Most of these functions do not work properly if the source and destination arrays overlap. For example, if the beginning of the destination array overlaps the end of the source array, the original contents of that part of the source array may get overwritten before it is copied. Even worse, in the case of the string functions, the null character marking the end of the string may be lost, and the copy function might get stuck in a loop trashing all the memory allocated to your program. 
\par 
\par All functions that have problems copying between overlapping arrays are explicitly identified in this manual. In addition to functions in this section, there are a few others like \f1 sprintf\f0  (see \cf1\strike Formatted Output Functions\strike0\{linkID=960\}\cf0 ) and \f1 scanf\f0  (see \cf1\strike Formatted Input Functions\strike0\{linkID=930\}\cf0 ).
\par 
\par -- Function: void * \b memcpy\b0  (\i void *restrict to, const void *restrict from, size_t size\i0 )
\par 
\par \pard\li500 The \f1 memcpy\f0  function copies \i size\i0  bytes from the object beginning at \i from\i0  into the object beginning at \i to\i0 . The behavior of this function is undefined if the two arrays \i to\i0  and\i  from\i0  overlap; use \f1 memmove\f0  instead if overlapping is possible. 
\par 
\par The value returned by \f1 memcpy\f0  is the value of \i to\i0 . 
\par 
\par Here is an example of how you might use \f1 memcpy\f0  to copy the contents of an array: 
\par 
\par \f1           struct foo *oldarray, *newarray;
\par           int arraysize;
\par           ...
\par           memcpy (new, old, arraysize * sizeof (struct foo));
\par      
\par \pard\f0 -- Function: wchar_t * \b wmemcpy\b0  (\i wchar_t *restrict wto, const wchar_t *restruct wfrom, size_t size\i0 )
\par 
\par \pard\li500 The \f1 wmemcpy\f0  function copies \i size\i0  wide characters from the object beginning at \i wfrom\i0  into the object beginning at \i wto\i0 . The behavior of this function is undefined if the two arrays \i wto\i0  and\i  wfrom\i0  overlap; use \f1 wmemmove\f0  instead if overlapping is possible. 
\par 
\par The following is a possible implementation of \f1 wmemcpy\f0  but there are more optimizations possible. 
\par 
\par \f1           wchar_t *
\par           wmemcpy (wchar_t *restrict wto, const wchar_t *restrict wfrom,
\par                    size_t size)
\par           \{
\par             return (wchar_t *) memcpy (wto, wfrom, size * sizeof (wchar_t));
\par           \}
\par      
\par \f0 The value returned by \f1 wmemcpy\f0  is the value of \i wto\i0 . 
\par 
\par This function was introduced in Amendment 1 to ISO C90.
\par 
\par \pard -- Function: void * \b memmove\b0  (\i void *to, const void *from, size_t size\i0 )
\par 
\par \pard\li500\f1 memmove\f0  copies the \i size\i0  bytes at \i from\i0  into the\i  size\i0  bytes at \i to\i0 , even if those two blocks of space overlap. In the case of overlap, \f1 memmove\f0  is careful to copy the original values of the bytes in the block at \i from\i0 , including those bytes which also belong to the block at \i to\i0 . 
\par 
\par The value returned by \f1 memmove\f0  is the value of \i to\i0 .
\par 
\par \pard -- Function: wchar_t * \b wmemmove\b0  (\i wchar *wto, const wchar_t *wfrom, size_t size\i0 )
\par 
\par \pard\li500\f1 wmemmove\f0  copies the \i size\i0  wide characters at \i wfrom\i0  into the \i size\i0  wide characters at \i wto\i0 , even if those two blocks of space overlap. In the case of overlap, \f1 memmove\f0  is careful to copy the original values of the wide characters in the block at \i wfrom\i0 , including those wide characters which also belong to the block at \i wto\i0 . 
\par 
\par The following is a possible implementation of \f1 wmemcpy\f0  but there are more optimizations possible. 
\par 
\par \f1           wchar_t *
\par           wmempcpy (wchar_t *restrict wto, const wchar_t *restrict wfrom,
\par                     size_t size)
\par           \{
\par             return (wchar_t *) mempcpy (wto, wfrom, size * sizeof (wchar_t));
\par           \}
\par      
\par \f0 The value returned by \f1 wmemmove\f0  is the value of \i wto\i0 . 
\par 
\par This function is a GNU extension.
\par 
\par \pard -- Function: void * \b memset\b0  (\i void *block, int c, size_t size\i0 )
\par 
\par \pard\li500 This function copies the value of \i c\i0  (converted to an\f1  unsigned char\f0 ) into each of the first \i size\i0  bytes of the object beginning at \i block\i0 . It returns the value of \i block\i0 .
\par 
\par \pard -- Function: wchar_t * \b wmemset\b0  (\i wchar_t *block, wchar_t wc, size_t size\i0 )
\par 
\par \pard\li500 This function copies the value of \i wc\i0  into each of the first\i  size\i0  wide characters of the object beginning at \i block\i0 . It returns the value of \i block\i0 .
\par 
\par \pard -- Function: char * \b strcpy\b0  (\i char *restrict to, const char *restrict from\i0 )
\par 
\par \pard\li500 This copies characters from the string \i from\i0  (up to and including the terminating null character) into the string \i to\i0 . Like\f1  memcpy\f0 , this function has undefined results if the strings overlap. The return value is the value of \i to\i0 .
\par 
\par \pard -- Function: wchar_t * \b wcscpy\b0  (\i wchar_t *restrict wto, const wchar_t *restrict wfrom\i0 )
\par 
\par \pard\li500 This copies wide characters from the string \i wfrom\i0  (up to and including the terminating null wide character) into the string\i  wto\i0 . Like \f1 wmemcpy\f0 , this function has undefined results if the strings overlap. The return value is the value of \i wto\i0 .
\par 
\par \pard -- Function: char * \b strncpy\b0  (\i char *restrict to, const char *restrict from, size_t size\i0 )
\par 
\par \pard\li500 This function is similar to \f1 strcpy\f0  but always copies exactly\i  size\i0  characters into \i to\i0 . 
\par 
\par If the length of \i from\i0  is more than \i size\i0 , then \f1 strncpy\f0  copies just the first \i size\i0  characters. Note that in this case there is no null terminator written into \i to\i0 . 
\par 
\par If the length of \i from\i0  is less than \i size\i0 , then \f1 strncpy\f0  copies all of \i from\i0 , followed by enough null characters to add up to \i size\i0  characters in all. This behavior is rarely useful, but it is specified by the ISO C standard. 
\par 
\par The behavior of \f1 strncpy\f0  is undefined if the strings overlap. 
\par 
\par Using \f1 strncpy\f0  as opposed to \f1 strcpy\f0  is a way to avoid bugs relating to writing past the end of the allocated space for \i to\i0 . However, it can also make your program much slower in one common case: copying a string which is probably small into a potentially large buffer. In this case, \i size\i0  may be large, and when it is, \f1 strncpy\f0  will waste a considerable amount of time copying null characters.
\par 
\par \pard -- Function: wchar_t * \b wcsncpy\b0  (\i wchar_t *restrict wto, const wchar_t *restrict wfrom, size_t size\i0 )
\par 
\par \pard\li500 This function is similar to \f1 wcscpy\f0  but always copies exactly\i  size\i0  wide characters into \i wto\i0 . 
\par 
\par If the length of \i wfrom\i0  is more than \i size\i0 , then\f1  wcsncpy\f0  copies just the first \i size\i0  wide characters. Note that in this case there is no null terminator written into \i wto\i0 . 
\par 
\par If the length of \i wfrom\i0  is less than \i size\i0 , then\f1  wcsncpy\f0  copies all of \i wfrom\i0 , followed by enough null wide characters to add up to \i size\i0  wide characters in all. This behavior is rarely useful, but it is specified by the ISO C standard. 
\par 
\par The behavior of \f1 wcsncpy\f0  is undefined if the strings overlap. 
\par 
\par Using \f1 wcsncpy\f0  as opposed to \f1 wcscpy\f0  is a way to avoid bugs relating to writing past the end of the allocated space for \i wto\i0 . However, it can also make your program much slower in one common case: copying a string which is probably small into a potentially large buffer. In this case, \i size\i0  may be large, and when it is, \f1 wcsncpy\f0  will waste a considerable amount of time copying null wide characters.
\par 
\par \pard -- Function: char * \b strdup\b0  (\i const char *s\i0 )
\par 
\par \pard\li500 This function copies the null-terminated string \i s\i0  into a newly allocated string. The string is allocated using \f1 malloc\f0 ; see \cf1\strike Unconstrained Allocation\strike0\{linkID=2340\}\cf0 . If \f1 malloc\f0  cannot allocate space for the new string, \f1 strdup\f0  returns a null pointer. Otherwise it returns a pointer to the new string.
\par 
\par \pard -- Function: char * \b strcat\b0  (\i char *restrict to, const char *restrict from\i0 )
\par 
\par \pard\li500 The \f1 strcat\f0  function is similar to \f1 strcpy\f0 , except that the characters from \i from\i0  are concatenated or appended to the end of\i  to\i0 , instead of overwriting it. That is, the first character from\i  from\i0  overwrites the null character marking the end of \i to\i0 . 
\par 
\par An equivalent definition for \f1 strcat\f0  would be: 
\par 
\par \f1           char *
\par           strcat (char *restrict to, const char *restrict from)
\par           \{
\par             strcpy (to + strlen (to), from);
\par             return to;
\par           \}
\par      
\par \f0 This function has undefined results if the strings overlap.
\par 
\par \pard -- Function: wchar_t * \b wcscat\b0  (\i wchar_t *restrict wto, const wchar_t *restrict wfrom\i0 )
\par 
\par \pard\li500 The \f1 wcscat\f0  function is similar to \f1 wcscpy\f0 , except that the characters from \i wfrom\i0  are concatenated or appended to the end of\i  wto\i0 , instead of overwriting it. That is, the first character from\i  wfrom\i0  overwrites the null character marking the end of \i wto\i0 . 
\par 
\par An equivalent definition for \f1 wcscat\f0  would be: 
\par 
\par \f1           wchar_t *
\par           wcscat (wchar_t *wto, const wchar_t *wfrom)
\par           \{
\par             wcscpy (wto + wcslen (wto), wfrom);
\par             return wto;
\par           \}
\par      
\par \f0 This function has undefined results if the strings overlap.
\par 
\par \pard Programmers using the \f1 strcat\f0  or \f1 wcscat\f0  function (or the following \f1 strncat\f0  or \f1 wcsncar\f0  functions for that matter) can easily be recognized as lazy and reckless. In almost all situations the lengths of the participating strings are known (it better should be since how can one otherwise ensure the allocated size of the buffer is sufficient?) Or at least, one could know them if one keeps track of the results of the various function calls. But then it is very inefficient to use \f1 strcat\f0 /\f1 wcscat\f0 . A lot of time is wasted finding the end of the destination string so that the actual copying can start. This is a common example:
\par 
\par \f1      /* This function concatenates arbitrarily many strings.  The last
\par         parameter must be NULL.  */
\par      char *
\par      concat (const char *str, ...)
\par      \{
\par        va_list ap, ap2;
\par        size_t total = 1;
\par        const char *s;
\par        char *result;
\par      
\par        va_start (ap, str);
\par        /* Actually va_copy, but this is the name more gcc versions
\par           understand.  */
\par        __va_copy (ap2, ap);
\par      
\par        /* Determine how much space we need.  */
\par        for (s = str; s != NULL; s = va_arg (ap, const char *))
\par          total += strlen (s);
\par      
\par        va_end (ap);
\par      
\par        result = (char *) malloc (total);
\par        if (result != NULL)
\par          \{
\par            result[0] = '\\0';
\par      
\par            /* Copy the strings.  */
\par            for (s = str; s != NULL; s = va_arg (ap2, const char *))
\par              strcat (result, s);
\par          \}
\par      
\par        va_end (ap2);
\par      
\par        return result;
\par      \}
\par \f0 
\par This looks quite simple, especially the second loop where the strings are actually copied. But these innocent lines hide a major performance penalty. Just imagine that ten strings of 100 bytes each have to be concatenated. For the second string we search the already stored 100 bytes for the end of the string so that we can append the next string. For all strings in total the comparisons necessary to find the end of the intermediate results sums up to 5500! If we combine the copying with the search for the allocation we can write this function more efficient:
\par 
\par \f1      char *
\par      concat (const char *str, ...)
\par      \{
\par        va_list ap;
\par        size_t allocated = 100;
\par        char *result = (char *) malloc (allocated);
\par        char *wp;
\par      
\par        if (allocated != NULL)
\par          \{
\par            char *newp;
\par      
\par            va_start (ap, atr);
\par      
\par            wp = result;
\par            for (s = str; s != NULL; s = va_arg (ap, const char *))
\par              \{
\par                size_t len = strlen (s);
\par      
\par                /* Resize the allocated memory if necessary.  */
\par                if (wp + len + 1 > result + allocated)
\par                  \{
\par                    allocated = (allocated + len) * 2;
\par                    newp = (char *) realloc (result, allocated);
\par                    if (newp == NULL)
\par                      \{
\par                        free (result);
\par                        return NULL;
\par                      \}
\par                    wp = newp + (wp - result);
\par                    result = newp;
\par                  \}
\par      
\par                wp = mempcpy (wp, s, len);
\par              \}
\par      
\par            /* Terminate the result string.  */
\par            *wp++ = '\\0';
\par      
\par            /* Resize memory to the optimal size.  */
\par            newp = realloc (result, wp - result);
\par            if (newp != NULL)
\par              result = newp;
\par      
\par            va_end (ap);
\par          \}
\par      
\par        return result;
\par      \}
\par \f0 
\par With a bit more knowledge about the input strings one could fine-tune the memory allocation. The difference we are pointing to here is that we don't use \f1 strcat\f0  anymore. We always keep track of the length of the current intermediate result so we can safe us the search for the end of the string and use \f1 mempcpy\f0 . Please note that we also don't use \f1 stpcpy\f0  which might seem more natural since we handle with strings. But this is not necessary since we already know the length of the string and therefore can use the faster memory copying function. The example would work for wide characters the same way. 
\par 
\par Whenever a programmer feels the need to use \f1 strcat\f0  she or he should think twice and look through the program whether the code cannot be rewritten to take advantage of already calculated results. Again: it is almost always unnecessary to use \f1 strcat\f0 .
\par -- Function: char * \b strncat\b0  (\i char *restrict to, const char *restrict from, size_t size\i0 )
\par 
\par \pard\li500 This function is like \f1 strcat\f0  except that not more than \i size\i0  characters from \i from\i0  are appended to the end of \i to\i0 . A single null character is also always appended to \i to\i0 , so the total allocated size of \i to\i0  must be at least \i size\i0\f1  + 1\f0  bytes longer than its initial length. 
\par 
\par The \f1 strncat\f0  function could be implemented like this: 
\par 
\par \f1           char *
\par           strncat (char *to, const char *from, size_t size)
\par           \{
\par             to[strlen (to) + size] = '\\0';
\par             strncpy (to + strlen (to), from, size);
\par             return to;
\par           \}
\par      
\par \f0 The behavior of \f1 strncat\f0  is undefined if the strings overlap.
\par 
\par \pard -- Function: wchar_t * \b wcsncat\b0  (\i wchar_t *restrict wto, const wchar_t *restrict wfrom, size_t size\i0 )
\par 
\par \pard\li500 This function is like \f1 wcscat\f0  except that not more than \i size\i0  characters from \i from\i0  are appended to the end of \i to\i0 . A single null character is also always appended to \i to\i0 , so the total allocated size of \i to\i0  must be at least \i size\i0\f1  + 1\f0  bytes longer than its initial length. 
\par 
\par The \f1 wcsncat\f0  function could be implemented like this: 
\par 
\par \f1           wchar_t *
\par           wcsncat (wchar_t *restrict wto, const wchar_t *restrict wfrom,
\par                    size_t size)
\par           \{
\par             wto[wcslen (to) + size] = L'\\0';
\par             wcsncpy (wto + wcslen (wto), wfrom, size);
\par             return wto;
\par           \}
\par      
\par \f0 The behavior of \f1 wcsncat\f0  is undefined if the strings overlap.
\par 
\par \pard Here is an example showing the use of \f1 strncpy\f0  and \f1 strncat\f0  (the wide character version is equivalent). Notice how, in the call to\f1  strncat\f0 , the \i size\i0  parameter is computed to avoid overflowing the character array \f1 buffer\f0 .
\par 
\par \f1      #include <string.h>
\par      #include <stdio.h>
\par      
\par      #define SIZE 10
\par      
\par      static char buffer[SIZE];
\par      
\par      main ()
\par      \{
\par        strncpy (buffer, "hello", SIZE);
\par        puts (buffer);
\par        strncat (buffer, ", world", SIZE - strlen (buffer) - 1);
\par        puts (buffer);
\par      \}
\par \f0 
\par The output produced by this program looks like:
\par 
\par \f1      hello
\par      hello, wo
\par \f0 
\par -- Function: void \b bcopy\b0  (\i const void *from, void *to, size_t size\i0 )
\par 
\par \pard\li500 This is a partially obsolete alternative for \f1 memmove\f0 , derived from BSD. Note that it is not quite equivalent to \f1 memmove\f0 , because the arguments are not in the same order and there is no return value.
\par 
\par \pard -- Function: void \b bzero\b0  (\i void *block, size_t size\i0 )
\par 
\par \pard\li500 This is a partially obsolete alternative for \f1 memset\f0 , derived from BSD. Note that it is not as general as \f1 memset\f0 , because the only value it can store is zero.
\par 
\par }
470
PAGE_127
Copying
LGPL,LesserGeneralPublicLicense



Imported



FALSE
170
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern\fcharset0 Courier New;}{\f2\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Copying\{keepn\}
\par \pard\fs18 Next: \cf1\strike Documentation License\strike0\{linkID=590\}\cf0 , Previous: \cf1\strike Free Manuals\strike0\{linkID=1020\}\cf0 , Up: \cf1\strike Top\strike0\{linkID=10\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Appendix G GNU Lesser General Public License\b0 
\par 
\par \pard\qc Version 2.1, February 1999
\par \pard 
\par \f1      Copyright \'a9 1991, 1999 Free Software Foundation, Inc.\f2 
\par      59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
\par      
\par      Everyone is permitted to copy and distribute verbatim copies
\par      of this license document, but changing it is not allowed.
\par      
\par      [This is the first released version of the Lesser GPL.  It also counts
\par      as the successor of the GNU Library Public License, version 2, hence the
\par      version number 2.1.]
\par \f0 
\par \b G.0.1 Preamble\b0 
\par 
\par The licenses for most software are designed to take away your freedom to share and change it. By contrast, the GNU General Public Licenses are intended to guarantee your freedom to share and change free software--to make sure the software is free for all its users. 
\par 
\par This license, the Lesser General Public License, applies to some specially designated software--typically libraries--of the Free Software Foundation and other authors who decide to use it. You can use it too, but we suggest you first think carefully about whether this license or the ordinary General Public License is the better strategy to use in any particular case, based on the explanations below. 
\par 
\par When we speak of free software, we are referring to freedom of use, not price. Our General Public Licenses are designed to make sure that you have the freedom to distribute copies of free software (and charge for this service if you wish); that you receive source code or can get it if you want it; that you can change the software and use pieces of it in new free programs; and that you are informed that you can do these things. 
\par 
\par To protect your rights, we need to make restrictions that forbid distributors to deny you these rights or to ask you to surrender these rights. These restrictions translate to certain responsibilities for you if you distribute copies of the library or if you modify it. 
\par 
\par For example, if you distribute copies of the library, whether gratis or for a fee, you must give the recipients all the rights that we gave you. You must make sure that they, too, receive or can get the source code. If you link other code with the library, you must provide complete object files to the recipients, so that they can relink them with the library after making changes to the library and recompiling it. And you must show them these terms so they know their rights. 
\par 
\par We protect your rights with a two-step method: (1) we copyright the library, and (2) we offer you this license, which gives you legal permission to copy, distribute and/or modify the library. 
\par 
\par To protect each distributor, we want to make it very clear that there is no warranty for the free library. Also, if the library is modified by someone else and passed on, the recipients should know that what they have is not the original version, so that the original author's reputation will not be affected by problems that might be introduced by others. 
\par 
\par Finally, software patents pose a constant threat to the existence of any free program. We wish to make sure that a company cannot effectively restrict the users of a free program by obtaining a restrictive license from a patent holder. Therefore, we insist that any patent license obtained for a version of the library must be consistent with the full freedom of use specified in this license. 
\par 
\par Most GNU software, including some libraries, is covered by the ordinary GNU General Public License. This license, the GNU Lesser General Public License, applies to certain designated libraries, and is quite different from the ordinary General Public License. We use this license for certain libraries in order to permit linking those libraries into non-free programs. 
\par 
\par When a program is linked with a library, whether statically or using a shared library, the combination of the two is legally speaking a combined work, a derivative of the original library. The ordinary General Public License therefore permits such linking only if the entire combination fits its criteria of freedom. The Lesser General Public License permits more lax criteria for linking other code with the library. 
\par 
\par We call this license the \i Lesser\i0  General Public License because it does \i Less\i0  to protect the user's freedom than the ordinary General Public License. It also provides other free software developers Less of an advantage over competing non-free programs. These disadvantages are the reason we use the ordinary General Public License for many libraries. However, the Lesser license provides advantages in certain special circumstances. 
\par 
\par For example, on rare occasions, there may be a special need to encourage the widest possible use of a certain library, so that it becomes a de-facto standard. To achieve this, non-free programs must be allowed to use the library. A more frequent case is that a free library does the same job as widely used non-free libraries. In this case, there is little to gain by limiting the free library to free software only, so we use the Lesser General Public License. 
\par 
\par In other cases, permission to use a particular library in non-free programs enables a greater number of people to use a large body of free software. For example, permission to use the GNU C Library in non-free programs enables many more people to use the whole GNU operating system, as well as its variant, the GNU/Linux operating system. 
\par 
\par Although the Lesser General Public License is Less protective of the users' freedom, it does ensure that the user of a program that is linked with the Library has the freedom and the wherewithal to run that program using a modified version of the Library. 
\par 
\par The precise terms and conditions for copying, distribution and modification follow. Pay close attention to the difference between a "work based on the library" and a "work that uses the library". The former contains code derived from the library, whereas the latter must be combined with the library in order to run.
\par 
\par \pard\li500 1. This License Agreement applies to any software library or other program which contains a notice placed by the copyright holder or other authorized party saying it may be distributed under the terms of this Lesser General Public License (also called "this License"). Each licensee is addressed as "you". 
\par 
\par A "library" means a collection of software functions and/or data prepared so as to be conveniently linked with application programs (which use some of those functions and data) to form executables. 
\par 
\par The "Library", below, refers to any such software library or work which has been distributed under these terms. A "work based on the Library" means either the Library or any derivative work under copyright law: that is to say, a work containing the Library or a portion of it, either verbatim or with modifications and/or translated straightforwardly into another language. (Hereinafter, translation is included without limitation in the term "modification".) 
\par 
\par "Source code" for a work means the preferred form of the work for making modifications to it. For a library, complete source code means all the source code for all modules it contains, plus any associated interface definition files, plus the scripts used to control compilation and installation of the library. 
\par 
\par Activities other than copying, distribution and modification are not covered by this License; they are outside its scope. The act of running a program using the Library is not restricted, and output from such a program is covered only if its contents constitute a work based on the Library (independent of the use of the Library in a tool for writing it). Whether that is true depends on what the Library does and what the program that uses the Library does. 
\par 2. You may copy and distribute verbatim copies of the Library's complete source code as you receive it, in any medium, provided that you conspicuously and appropriately publish on each copy an appropriate copyright notice and disclaimer of warranty; keep intact all the notices that refer to this License and to the absence of any warranty; and distribute a copy of this License along with the Library. 
\par 
\par You may charge a fee for the physical act of transferring a copy, and you may at your option offer warranty protection in exchange for a fee. 
\par 3. You may modify your copy or copies of the Library or any portion of it, thus forming a work based on the Library, and copy and distribute such modifications or work under the terms of Section 1 above, provided that you also meet all of these conditions:
\par 
\par \pard\li1000 1. The modified work must itself be a software library. 
\par 2. You must cause the files modified to carry prominent notices stating that you changed the files and the date of any change. 
\par 3. You must cause the whole of the work to be licensed at no charge to all third parties under the terms of this License. 
\par 4. If a facility in the modified Library refers to a function or a table of data to be supplied by an application program that uses the facility, other than as an argument passed when the facility is invoked, then you must make a good faith effort to ensure that, in the event an application does not supply such function or table, the facility still operates, and performs whatever part of its purpose remains meaningful. 
\par 
\par (For example, a function in a library to compute square roots has a purpose that is entirely well-defined independent of the application. Therefore, Subsection 2d requires that any application-supplied function or table used by this function must be optional: if the application does not supply it, the square root function must still compute square roots.)
\par 
\par \pard\li500 These requirements apply to the modified work as a whole. If identifiable sections of that work are not derived from the Library, and can be reasonably considered independent and separate works in themselves, then this License, and its terms, do not apply to those sections when you distribute them as separate works. But when you distribute the same sections as part of a whole which is a work based on the Library, the distribution of the whole must be on the terms of this License, whose permissions for other licensees extend to the entire whole, and thus to each and every part regardless of who wrote it. 
\par 
\par Thus, it is not the intent of this section to claim rights or contest your rights to work written entirely by you; rather, the intent is to exercise the right to control the distribution of derivative or collective works based on the Library. 
\par 
\par In addition, mere aggregation of another work not based on the Library with the Library (or with a work based on the Library) on a volume of a storage or distribution medium does not bring the other work under the scope of this License. 
\par 4. You may opt to apply the terms of the ordinary GNU General Public License instead of this License to a given copy of the Library. To do this, you must alter all the notices that refer to this License, so that they refer to the ordinary GNU General Public License, version 2, instead of to this License. (If a newer version than version 2 of the ordinary GNU General Public License has appeared, then you can specify that version instead if you wish.) Do not make any other change in these notices. 
\par 
\par Once this change is made in a given copy, it is irreversible for that copy, so the ordinary GNU General Public License applies to all subsequent copies and derivative works made from that copy. 
\par 
\par This option is useful when you wish to copy part of the code of the Library into a program that is not a library. 
\par 5. You may copy and distribute the Library (or a portion or derivative of it, under Section 2) in object code or executable form under the terms of Sections 1 and 2 above provided that you accompany it with the complete corresponding machine-readable source code, which must be distributed under the terms of Sections 1 and 2 above on a medium customarily used for software interchange. 
\par 
\par If distribution of object code is made by offering access to copy from a designated place, then offering equivalent access to copy the source code from the same place satisfies the requirement to distribute the source code, even though third parties are not compelled to copy the source along with the object code. 
\par 6. A program that contains no derivative of any portion of the Library, but is designed to work with the Library by being compiled or linked with it, is called a "work that uses the Library". Such a work, in isolation, is not a derivative work of the Library, and therefore falls outside the scope of this License. 
\par 
\par However, linking a "work that uses the Library" with the Library creates an executable that is a derivative of the Library (because it contains portions of the Library), rather than a "work that uses the library". The executable is therefore covered by this License. Section 6 states terms for distribution of such executables. 
\par 
\par When a "work that uses the Library" uses material from a header file that is part of the Library, the object code for the work may be a derivative work of the Library even though the source code is not. Whether this is true is especially significant if the work can be linked without the Library, or if the work is itself a library. The threshold for this to be true is not precisely defined by law. 
\par 
\par If such an object file uses only numerical parameters, data structure layouts and accessors, and small macros and small inline functions (ten lines or less in length), then the use of the object file is unrestricted, regardless of whether it is legally a derivative work. (Executables containing this object code plus portions of the Library will still fall under Section 6.) 
\par 
\par Otherwise, if the work is a derivative of the Library, you may distribute the object code for the work under the terms of Section 6. Any executables containing that work also fall under Section 6, whether or not they are linked directly with the Library itself. 
\par 7. As an exception to the Sections above, you may also combine or link a "work that uses the Library" with the Library to produce a work containing portions of the Library, and distribute that work under terms of your choice, provided that the terms permit modification of the work for the customer's own use and reverse engineering for debugging such modifications. 
\par 
\par You must give prominent notice with each copy of the work that the Library is used in it and that the Library and its use are covered by this License. You must supply a copy of this License. If the work during execution displays copyright notices, you must include the copyright notice for the Library among them, as well as a reference directing the user to the copy of this License. Also, you must do one of these things:
\par 
\par \pard\li1000 1. Accompany the work with the complete corresponding machine-readable source code for the Library including whatever changes were used in the work (which must be distributed under Sections 1 and 2 above); and, if the work is an executable linked with the Library, with the complete machine-readable "work that uses the Library", as object code and/or source code, so that the user can modify the Library and then relink to produce a modified executable containing the modified Library. (It is understood that the user who changes the contents of definitions files in the Library will not necessarily be able to recompile the application to use the modified definitions.) 
\par 2. Use a suitable shared library mechanism for linking with the Library. A suitable mechanism is one that (1) uses at run time a copy of the library already present on the user's computer system, rather than copying library functions into the executable, and (2) will operate properly with a modified version of the library, if the user installs one, as long as the modified version is interface-compatible with the version that the work was made with. 
\par 3. Accompany the work with a written offer, valid for at least three years, to give the same user the materials specified in Subsection 6a, above, for a charge no more than the cost of performing this distribution. 
\par 4. If distribution of the work is made by offering access to copy from a designated place, offer equivalent access to copy the above specified materials from the same place. 
\par 5. Verify that the user has already received a copy of these materials or that you have already sent this user a copy.
\par 
\par \pard\li500 For an executable, the required form of the "work that uses the Library" must include any data and utility programs needed for reproducing the executable from it. However, as a special exception, the materials to be distributed need not include anything that is normally distributed (in either source or binary form) with the major components (compiler, kernel, and so on) of the operating system on which the executable runs, unless that component itself accompanies the executable. 
\par 
\par It may happen that this requirement contradicts the license restrictions of other proprietary libraries that do not normally accompany the operating system. Such a contradiction means you cannot use both them and the Library together in an executable that you distribute. 
\par 8. You may place library facilities that are a work based on the Library side-by-side in a single library together with other library facilities not covered by this License, and distribute such a combined library, provided that the separate distribution of the work based on the Library and of the other library facilities is otherwise permitted, and provided that you do these two things:
\par 
\par \pard\li1000 1. Accompany the combined library with a copy of the same work based on the Library, uncombined with any other library facilities. This must be distributed under the terms of the Sections above. 
\par 2. Give prominent notice with the combined library of the fact that part of it is a work based on the Library, and explaining where to find the accompanying uncombined form of the same work.
\par 
\par \pard\li500 9. You may not copy, modify, sublicense, link with, or distribute the Library except as expressly provided under this License. Any attempt otherwise to copy, modify, sublicense, link with, or distribute the Library is void, and will automatically terminate your rights under this License. However, parties who have received copies, or rights, from you under this License will not have their licenses terminated so long as such parties remain in full compliance. 
\par 10. You are not required to accept this License, since you have not signed it. However, nothing else grants you permission to modify or distribute the Library or its derivative works. These actions are prohibited by law if you do not accept this License. Therefore, by modifying or distributing the Library (or any work based on the Library), you indicate your acceptance of this License to do so, and all its terms and conditions for copying, distributing or modifying the Library or works based on it. 
\par 11. Each time you redistribute the Library (or any work based on the Library), the recipient automatically receives a license from the original licensor to copy, distribute, link with or modify the Library subject to these terms and conditions. You may not impose any further restrictions on the recipients' exercise of the rights granted herein. You are not responsible for enforcing compliance by third parties with this License. 
\par 12. If, as a consequence of a court judgment or allegation of patent infringement or for any other reason (not limited to patent issues), conditions are imposed on you (whether by court order, agreement or otherwise) that contradict the conditions of this License, they do not excuse you from the conditions of this License. If you cannot distribute so as to satisfy simultaneously your obligations under this License and any other pertinent obligations, then as a consequence you may not distribute the Library at all. For example, if a patent license would not permit royalty-free redistribution of the Library by all those who receive copies directly or indirectly through you, then the only way you could satisfy both it and this License would be to refrain entirely from distribution of the Library. 
\par 
\par If any portion of this section is held invalid or unenforceable under any particular circumstance, the balance of the section is intended to apply, and the section as a whole is intended to apply in other circumstances. 
\par 
\par It is not the purpose of this section to induce you to infringe any patents or other property right claims or to contest validity of any such claims; this section has the sole purpose of protecting the integrity of the free software distribution system which is implemented by public license practices. Many people have made generous contributions to the wide range of software distributed through that system in reliance on consistent application of that system; it is up to the author/donor to decide if he or she is willing to distribute software through any other system and a licensee cannot impose that choice. 
\par 
\par This section is intended to make thoroughly clear what is believed to be a consequence of the rest of this License. 
\par 13. If the distribution and/or use of the Library is restricted in certain countries either by patents or by copyrighted interfaces, the original copyright holder who places the Library under this License may add an explicit geographical distribution limitation excluding those countries, so that distribution is permitted only in or among countries not thus excluded. In such case, this License incorporates the limitation as if written in the body of this License. 
\par 14. The Free Software Foundation may publish revised and/or new versions of the Lesser General Public License from time to time. Such new versions will be similar in spirit to the present version, but may differ in detail to address new problems or concerns. 
\par 
\par Each version is given a distinguishing version number. If the Library specifies a version number of this License which applies to it and "any later version", you have the option of following the terms and conditions either of that version or of any later version published by the Free Software Foundation. If the Library does not specify a license version number, you may choose any version ever published by the Free Software Foundation. 
\par 15. If you wish to incorporate parts of the Library into other free programs whose distribution conditions are incompatible with these, write to the author to ask for permission. For software which is copyrighted by the Free Software Foundation, write to the Free Software Foundation; we sometimes make exceptions for this. Our decision will be guided by the two goals of preserving the free status of all derivatives of our free software and of promoting the sharing and reuse of software generally. 
\par 16. BECAUSE THE LIBRARY IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY FOR THE LIBRARY, TO THE EXTENT PERMITTED BY APPLICABLE LAW. EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES PROVIDE THE LIBRARY "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE LIBRARY IS WITH YOU. SHOULD THE LIBRARY PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION. 
\par 17. IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY MODIFY AND/OR REDISTRIBUTE THE LIBRARY AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES, INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OR INABILITY TO USE THE LIBRARY (INCLUDING BUT NOT LIMITED TO LOSS OF DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY YOU OR THIRD PARTIES OR A FAILURE OF THE LIBRARY TO OPERATE WITH ANY OTHER SOFTWARE), EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
\par 
\par \pard\b How to Apply These Terms to Your New Libraries\b0 
\par 
\par If you develop a new library, and you want it to be of the greatest possible use to the public, we recommend making it free software that everyone can redistribute and change. You can do so by permitting redistribution under these terms (or, alternatively, under the terms of the ordinary General Public License). 
\par 
\par To apply these terms, attach the following notices to the library. It is safest to attach them to the start of each source file to most effectively convey the exclusion of warranty; and each file should have at least the "copyright" line and a pointer to where the full notice is found.
\par 
\par \f2      \i one line to give the library's name and an idea of what it does.
\par \i0      Copyright (C) \i year\i0   \i name of author
\par \i0      
\par      This library is free software; you can redistribute it and/or modify it
\par      under the terms of the GNU Lesser General Public License as published by
\par      the Free Software Foundation; either version 2.1 of the License, or (at
\par      your option) any later version.
\par      
\par      This library is distributed in the hope that it will be useful, but
\par      WITHOUT ANY WARRANTY; without even the implied warranty of
\par      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
\par      Lesser General Public License for more details.
\par      
\par      You should have received a copy of the GNU Lesser General Public
\par      License along with this library; if not, write to the Free Software
\par      Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,
\par      USA.
\par \f0 
\par Also add information on how to contact you by electronic and paper mail. 
\par 
\par You should also get your employer (if you work as a programmer) or your school, if any, to sign a "copyright disclaimer" for the library, if necessary. Here is a sample; alter the names:
\par 
\par \f2      Yoyodyne, Inc., hereby disclaims all copyright interest in the library
\par      `Frob' (a library for tweaking knobs) written by James Random Hacker.
\par      
\par      \i signature of Ty Coon\i0 , 1 April 1990
\par      Ty Coon, President of Vice
\par \f0 
\par That's all there is to it!
\par 
\par }
480
PAGE_128
CPU Time
time.h;CLOCKS_PER_SEC;CLK_TCK;clock_t;clock



Imported



FALSE
43
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fswiss\fcharset0 Arial;}{\f2\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 CPU Time \{keepn\}
\par \pard\fs18 Next: \cf1\strike Calendar Time\cf2\strike0\{linkID=230\}\cf0 , \f1 Previous: \cf1\strike Elapsed Time\cf2\strike0\{linkID=620\}\cf0 ,\f0 Up:\f1  \cf1\strike\f0 Date and Time\strike0\{linkID=520\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b CPU Time Inquiry\b0 
\par 
\par To get a process' CPU time, you can use the \f2 clock\f0  function. This facility is declared in the header file \f2 time.h\f0 . In typical usage, you call the \f2 clock\f0  function at the beginning and end of the interval you want to time, subtract the values, and then divide by \f2 CLOCKS_PER_SEC\f0  (the number of clock ticks per second) to get processor time, like this:
\par 
\par \f2      #include <time.h>
\par      
\par      clock_t start, end;
\par      double cpu_time_used;
\par      
\par      start = clock();
\par      ... /* Do the work. */
\par      end = clock();
\par      cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
\par \f0 
\par Do not use a single CPU time as an amount of time; it doesn't work that way. Either do a subtraction as shown above or query processor time directly. See \cf1\strike Processor Time\strike0\{link=PAGE_478\}\cf0 . 
\par 
\par Different computers and operating systems vary wildly in how they keep track of CPU time. It's common for the internal processor clock to have a resolution somewhere between a hundredth and millionth of a second.
\par 
\par -- Macro: int \b CLOCKS_PER_SEC\b0 
\par 
\par \pard\li500 The value of this macro is the number of clock ticks per second measured by the \f2 clock\f0  function. POSIX requires that this value be one million independent of the actual resolution.
\par 
\par \pard -- Macro: int \b CLK_TCK\b0 
\par 
\par \pard\li500 This is an obsolete name for \f2 CLOCKS_PER_SEC\f0 .
\par 
\par \pard -- Data Type: \b clock_t\b0 
\par 
\par \pard\li500 This is the type of the value returned by the \f2 clock\f0  function. Values of type \f2 clock_t\f0  are numbers of clock ticks.
\par 
\par \pard -- Function: clock_t \b clock\b0  (\i void\i0 )
\par 
\par \pard\li500 This function returns the calling process' current CPU time. If the CPU time is not available or cannot be represented, \f2 clock\f0  returns the value \f2 (clock_t)(-1)\f0 .
\par 
\par }
490
PAGE_132
Creating Directories
creatingadirectory;directories,creating;mkdir;mkdir;sys/stat.h



Imported



FALSE
29
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 + Creating Directories \{keepn\}
\par \pard\fs18 Next: \cf1\strike Attribute Meanings\cf2\strike0\{linkID=160\}\cf0 , Previous: \cf1\strike Renaming Files\strike0\{linkID=1840\}\cf0 , Up: \cf1\strike File System Interface\strike0\{linkID=820\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Creating Directories\b0 
\par 
\par Directories are created with the \f1 mkdir\f0  function. (There is also a shell command \f1 mkdir\f0  which does the same thing.)
\par -- Function: int \b mkdir\b0  (\i const char *filename, mode_t mode\i0 )
\par 
\par \pard\li500 The \f1 mkdir\f0  function creates a new, empty directory with name\i  filename\i0 . 
\par 
\par The argument \i mode\i0  specifies the file permissions for the new directory file. 
\par 
\par A return value of \f1 0\f0  indicates successful completion, and\f1  -1\f0  indicates failure. In addition to the usual file name syntax errors (see \cf1\strike File Name Errors\strike0\{linkID=740\}\cf0 ), the following \f1 errno\f0  error conditions are defined for this function:
\par 
\par \f1 EACCES\f0 
\par \pard\li1000 Write permission is denied for the parent directory in which the new directory is to be added. 
\par \pard\li500\f1 EEXIST\f0 
\par \pard\li1000 A file named \i filename\i0  already exists. 
\par \pard\li500\f1 ENOSPC\f0 
\par \pard\li1000 The file system doesn't have enough room to create the new directory. 
\par \pard\li500\f1 
\par \f0 To use this function, your program should include the header file\f1  \f2 dir\f1 .h\f0 .
\par 
\par }
500
PAGE_136
Currency Symbol
currencysymbols



Imported



FALSE
46
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Currency Symbol \{keepn\}
\par \pard\fs18 Next: \cf1\strike Sign of Money Amount\strike0\{linkID=1980\}\cf0 , Previous: \cf1\strike General Numeric\strike0\{linkID=1040\}\cf0 , Up: \cf1\strike The Lame Way to Locale Data\strike0\{linkID=2280\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Printing the Currency Symbol\b0 
\par 
\par These members of the \f1 struct lconv\f0  structure specify how to print the symbol to identify a monetary value--the international analog of `\f1 $\f0 ' for US dollars. 
\par 
\par Each country has two standard currency symbols. The \i local currency symbol\i0  is used commonly within the country, while the\i  international currency symbol\i0  is used internationally to refer to that country's currency when it is necessary to indicate the country unambiguously. 
\par 
\par For example, many countries use the dollar as their monetary unit, and when dealing with international currencies it's important to specify that one is dealing with (say) Canadian dollars instead of U.S. dollars or Australian dollars. But when the context is known to be Canada, there is no need to make this explicit--dollar amounts are implicitly assumed to be in Canadian dollars.
\par 
\par \f1 char *currency_symbol\f0 
\par \pard\li500 The local currency symbol for the selected locale. 
\par 
\par In the standard `\f1 C\f0 ' locale, this member has a value of \f1 ""\f0  (the empty string), meaning "unspecified". The ISO standard doesn't say what to do when you find this value; we recommend you simply print the empty string as you would print any other string pointed to by this variable. 
\par \pard\f1 char *int_curr_symbol\f0 
\par \pard\li500 The international currency symbol for the selected locale. 
\par 
\par The value of \f1 int_curr_symbol\f0  should normally consist of a three-letter abbreviation determined by the international standard\i  ISO 4217 Codes for the Representation of Currency and Funds\i0 , followed by a one-character separator (often a space). 
\par 
\par In the standard `\f1 C\f0 ' locale, this member has a value of \f1 ""\f0  (the empty string), meaning "unspecified". We recommend you simply print the empty string as you would print any other string pointed to by this variable. 
\par \pard\f1 char p_cs_precedes\f0 
\par \f1 char n_cs_precedes\f0 
\par \f1 char int_p_cs_precedes\f0 
\par \f1 char int_n_cs_precedes\f0 
\par \pard\li500 These members are \f1 1\f0  if the \f1 currency_symbol\f0  or\f1  int_curr_symbol\f0  strings should precede the value of a monetary amount, or \f1 0\f0  if the strings should follow the value. The\f1  p_cs_precedes\f0  and \f1 int_p_cs_precedes\f0  members apply to positive amounts (or zero), and the \f1 n_cs_precedes\f0  and\f1  int_n_cs_precedes\f0  members apply to negative amounts. 
\par 
\par In the standard `\f1 C\f0 ' locale, all of these members have a value of\f1  CHAR_MAX\f0 , meaning "unspecified". The ISO standard doesn't say what to do when you find this value. We recommend printing the currency symbol before the amount, which is right for most countries. In other words, treat all nonzero values alike in these members. 
\par 
\par The members with the \f1 int_\f0  prefix apply to the\f1  int_curr_symbol\f0  while the other two apply to\f1  currency_symbol\f0 . 
\par \pard\f1 char p_sep_by_space\f0 
\par \f1 char n_sep_by_space\f0 
\par \f1 char int_p_sep_by_space\f0 
\par \f1 char int_n_sep_by_space\f0 
\par \pard\li500 These members are \f1 1\f0  if a space should appear between the\f1  currency_symbol\f0  or \f1 int_curr_symbol\f0  strings and the amount, or \f1 0\f0  if no space should appear. The\f1  p_sep_by_space\f0  and \f1 int_p_sep_by_space\f0  members apply to positive amounts (or zero), and the \f1 n_sep_by_space\f0  and\f1  int_n_sep_by_space\f0  members apply to negative amounts. 
\par 
\par In the standard `\f1 C\f0 ' locale, all of these members have a value of\f1  CHAR_MAX\f0 , meaning "unspecified". The ISO standard doesn't say what you should do when you find this value; we suggest you treat it as 1 (print a space). In other words, treat all nonzero values alike in these members. 
\par 
\par The members with the \f1 int_\f0  prefix apply to the\f1  int_curr_symbol\f0  while the other two apply to\f1  currency_symbol\f0 . There is one specialty with the\f1  int_curr_symbol\f0 , though. Since all legal values contain a space at the end the string one either printf this space (if the currency symbol must appear in front and must be separated) or one has to avoid printing this character at all (especially when at the end of the string).
\par 
\par }
510
PAGE_140
Data Type Measurements




Imported



FALSE
18
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 + Data Type Measurements \{keepn\}
\par \pard\fs18 Previous: \cf1\strike Important Data Types\strike0\{linkID=1150\}\cf0 , Up: \cf1\strike Language Features\strike0\{linkID=1310\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Data Type Measurements\b0 
\par 
\par Most of the time, if you choose the proper C data type for each object in your program, you need not be concerned with just how it is represented or how many bits it uses. When you do need such information, the C language itself does not provide a way to get it. The header files \f1 limits.h\f0  and \f1 float.h\f0  contain macros which give you this information in full detail.
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Width of Type\strike0\{linkID=2460\}\cf0 : How many bits does an integer type hold?
\par \f2\'b7\f0  \cf1\strike Range of Type\strike0\{linkID=1790\}\cf0 : What are the largest and smallest values that an integer type can hold?
\par \f2\'b7\f0  \cf1\strike Floating Type Macros\strike0\{linkID=890\}\cf0 : Parameters that measure the floating point types.
\par \f2\'b7\f0  \cf1\strike Structure Measurement\strike0\{linkID=2210\}\cf0 : Getting measurements on structure types.
\par 
\par }
520
PAGE_144
Date and Time




Imported



FALSE
19
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern\fcharset0 Courier New;}{\f2\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Date and Time \{keepn\}
\par \pard\fs18 Next: \cf1\strike Non-Local Exits\cf2\strike0\{linkID=1510\}\cf0 , Previous: \cf1\strike Arithmetic\strike0\{linkID=100\}\cf0 , Up: \cf1\strike Top\strike0\{linkID=10\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Date and Time\b0 
\par 
\par This chapter describes functions for manipulating dates and times, including functions for determining what time it is and conversion between different time representations.
\par 
\par \pard\li500\f1\'b7\f0  \cf1\strike Time Basics\strike0\{linkID=2290\}\cf0 : Concepts and definitions.
\par \f1\'b7\f0  \cf1\strike Elapsed Time\strike0\{linkID=620\}\cf0 : Data types to represent elapsed times
\par \f1\'b7\f0  \cf1\strike CPU Time\strike0\{link\f2 ID=480\f0\}\cf0 : Time a program has spent executing.
\par \f1\'b7\f0  \cf1\strike Calendar Time\strike0\{linkID=230\}\cf0 : Manipulation of ``real'' dates and times.
\par \f1\'b7\f0  \cf1\strike Sleeping\strike0\{linkID=2060\}\cf0 : Waiting for a period of time.
\par 
\par }
530
PAGE_146
Defining Handlers
signalhandlerfunction



Imported



FALSE
29
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Defining Handlers \{keepn\}
\par \pard\fs18 Next: \cf1\strike Handler Returns\cf2\strike0\{linkID=1080\}\cf0 , Previous: \cf1\strike Signal Actions\strike0\{linkID=1990\}\cf0 , Up: \cf1\strike Signal Handling\strike0\{linkID=2010\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Defining Signal Handlers\b0 
\par 
\par This section describes how to write a signal handler function that can be established with the \f1 signal\f0  or \f1 sigaction\f0  functions. 
\par 
\par A signal handler is just a function that you compile together with the rest of the program. Instead of directly invoking the function, you use\f1  signal\f0  or \f1 sigaction\f0  to tell the operating system to call it when a signal arrives. This is known as \i establishing\i0  the handler. See \cf1\strike Signal Actions\strike0\{linkID=1990\}\cf0 . 
\par 
\par There are two basic strategies you can use in signal handler functions:
\par 
\par \pard\li500\f2\'b7\f0  You can have the handler function note that the signal arrived by tweaking some global data structures, and then return normally. 
\par \f2\'b7\f0  You can have the handler function terminate the program or transfer control to a point where it can recover from the situation that caused the signal.
\par 
\par \pard You need to take special care in writing handler functions because they can be called asynchronously. That is, a handler might be called at any point in the program, unpredictably. If two signals arrive during a very short interval, one handler can run within another. This section describes what your handler should do, and what you should avoid.
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Handler Returns\strike0\{linkID=1080\}\cf0 : Handlers that return normally, and what this means.
\par \f2\'b7\f0  \cf1\strike Termination in Handler\strike0\{linkID=2260\}\cf0 : How handler functions terminate a program.
\par \f2\'b7\f0  \cf1\strike Longjmp in Handler\strike0\{linkID=1350\}\cf0 : Nonlocal transfer of control out of a signal handler.
\par \f2\'b7\f0  \cf1\strike Signals in Handler\strike0\{linkID=2030\}\cf0 : What happens when signals arrive while the handler is already occupied.
\par \f2\'b7\f0  \cf1\strike Nonreentrancy\strike0\{linkID=1480\}\cf0 : Do not call any functions unless you know they are reentrant with respect to signals.
\par \f2\'b7\f0  \cf1\strike Atomic Data Access\strike0\{linkID=130\}\cf0 : A single handler can run in the middle of reading or writing a single object.
\par 
\par }
540
PAGE_148
Deleting Files
deletingafile;removingafile;unlinkingafile;unlink;unistd.h;rmdir;directories,deleting;deletingadirectory;unistd.h;remove;stdio.h



Imported



FALSE
52
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fswiss\fcharset0 Arial;}{\f2\fmodern Courier New;}{\f3\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Deleting Files \{keepn\}
\par \pard\fs18 Next: \cf1\strike Renaming Files\strike0\{linkID=1840\}\cf0 , Previous:\f1  \cf1\strike Working Directory\cf2\strike0\{linkID=2470\}\cf0\f0 , Up: \cf1\strike File System Interface\strike0\{linkID=820\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Deleting Files\b0 
\par 
\par You can delete a file with \f2 unlink\f0  or \f2 remove\f0 . 
\par 
\par Deletion actually deletes a file name. If this is the file's only name, then the file is deleted as well. 
\par 
\par -- Function: int \b unlink\b0  (\i const char *filename\i0 )
\par 
\par \pard\li500 The \f2 unlink\f0  function deletes the file name \i filename\i0 . If this is a file's sole name, the file itself is also deleted. (Actually, if any process has the file open when this happens, deletion is postponed until all processes have closed the file.) 
\par 
\par The function \f2 unlink\f0  is declared in the header file \f3 dir\f2 .h\f0 . 
\par 
\par This function returns \f2 0\f0  on successful completion, and \f2 -1\f0  on error. In addition to the usual file name errors (see \cf1\strike File Name Errors\strike0\{linkID=740\}\cf0 ), the following \f2 errno\f0  error conditions are defined for this function:
\par 
\par \f2 EACCES\f0 
\par \pard\li1000 Write permission is denied for the directory from which the file is to be removed, or the directory has the sticky bit set and you do not own the file. 
\par \pard\li500\f2 EBUSY\f0 
\par \pard\li1000 This error indicates that the file is being used by the system in such a way that it can't be unlinked. For example, you might see this error if the file name specifies the root directory or a mount point for a file system. 
\par \pard\li500\f2 ENOENT\f0 
\par \pard\li1000 The file name to be deleted doesn't exist. 
\par \pard\li500\f2 EPERM\f0 
\par \pard\li1000 On some systems \f2 unlink\f0  cannot be used to delete the name of a directory, or at least can only be used this way by a privileged user. To avoid such problems, use \f2 rmdir\f0  to delete directories. (In the GNU system \f2 unlink\f0  can never delete the name of a directory.) 
\par \pard\li500\f2 EROFS\f0 
\par \pard\li1000 The directory containing the file name to be deleted is on a read-only file system and can't be modified.
\par 
\par \pard -- Function: int \b rmdir\b0  (\i const char *filename\i0 )
\par 
\par \pard\li500 The \f2 rmdir\f0  function deletes a directory. The directory must be empty before it can be removed; in other words, it can only contain entries for \f2 .\f0  and \f2 ..\f0 . 
\par 
\par In most other respects, \f2 rmdir\f0  behaves like \f2 unlink\f0 . There are two additional \f2 errno\f0  error conditions defined for\f2  rmdir\f0 :
\par 
\par \f2 ENOTEMPTY\f0 
\par \f2 EEXIST\f0 
\par \pard\li1000 The directory to be deleted is not empty.
\par 
\par \pard\li500 These two error codes are synonymous; some systems use one, and some use the other. The GNU system always uses \f2 ENOTEMPTY\f0 . 
\par 
\par The prototype for this function is declared in the header file\f2  unistd.h\f0 .
\par 
\par \pard -- Function: int \b remove\b0  (\i const char *filename\i0 )
\par 
\par \pard\li500 This is the ISO C function to remove a file. It works like\f2  unlink\f0  for files and like \f2 rmdir\f0  for directories.\f2  remove\f0  is declared in \f2 stdio.h\f0 .
\par 
\par }
560
PAGE_152
Descriptors and Streams
streams,andfiledescriptors;convertingfiledescriptortostream;extractingfiledescriptorfromstream;stdio.h;fdopen;fileno;fileno_unlocked;standardfiledescriptors;filedescriptors,standard;unistd.h;STDIN_FILENO;standardinputfiledescriptor;STDOUT_FILENO;standardoutputfiledescriptor;STDERR_FILENO;standarderrorfiledescriptor



Imported



FALSE
23
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Descriptors and Streams \{keepn\}
\par \pard\fs18 Next: \cf1\strike Duplicating Descriptors\cf2\strike0\{linkID=600\}\cf0 , Previous: \cf1\strike File Position Primitive\strike0\{linkID=770\}\cf0 , Up: \cf1\strike Low-Level I/O\strike0\{linkID=1360\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Descriptors and Streams\b0 
\par 
\par Given an open file descriptor, you can create a stream for it with the\f1  fdopen\f0  function. \f2  \f0 Th\f2 is\f0  functions \f2 is\f0  declared in the header file \f1 stdio.h\f0 .
\par 
\par -- Function: FILE * \b fdopen\b0  (\i int filedes, const char *opentype\i0 )
\par 
\par \pard\li500 The \f1 fdopen\f0  function returns a new stream for the file descriptor\i  filedes\i0 . 
\par 
\par The \i opentype\i0  argument is interpreted in the same way as for the\f1  fopen\f0  function (see \cf1\strike Opening Streams\strike0\{linkID=1610\}\cf0 ), except that the `\f1 b\f0 ' option is not permitted; this is because GNU makes no distinction between text and binary files. Also, \f1 "w"\f0  and\f1  "w+"\f0  do not cause truncation of the file; these have an effect only when opening a file, and in this case the file has already been opened. You must make sure that the \i opentype\i0  argument matches the actual mode of the open file descriptor. 
\par 
\par The return value is the new stream. If the stream cannot be created (for example, if the modes for the file indicated by the file descriptor do not permit the access specified by the \i opentype\i0  argument), a null pointer is returned instead. 
\par 
\par In some other systems, \f1 fdopen\f0  may fail to detect that the modes for file descriptor do not permit the access specified by\f1  opentype\f0 . The C library always checks for this.
\par 
\par }
570
PAGE_153
Directories
directory;link;directoryentry;filenamecomponent



Imported



FALSE
21
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern\fcharset0 Courier New;}{\f2\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Directories\{keepn\}
\par \pard\fs18 Next: \cf1\strike File Name Resolution\strike0\{linkID=750\}\cf0 , Up: \cf1\strike File Names\strike0\{linkID=760\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Directories\b0 
\par 
\par In order to understand the syntax of file names, you need to understand how the file system is organized into a hierarchy of directories. 
\par 
\par A \i directory\i0  is a file that contains information to associate other files with names; these associations are called \i links\i0  or\i  directory entries\i0 . Sometimes, people speak of "files in a directory", but in reality, a directory only contains pointers to files, not the files themselves. 
\par 
\par The name of a file contained in a directory entry is called a \i file name component\i0 . In general, a file name consists of a sequence of one or more such components, separated by the slash character (`\f1\\\f0 '). A file name which is just one component names a file with respect to its directory. A file name with multiple components names a directory, and then a file in that directory, and so on. 
\par 
\par Some other documents, such as the POSIX standard, use the term\i  pathname\i0  for what we call a file name, and either \i filename\i0  or \i pathname component\i0  for what this manual calls a file name component. We don't use this terminology because a "path" is something completely different (a list of directories to search), and we think that "pathname" used for something else will confuse users. We always use "file name" and "file name component" (or sometimes just "component", where the context is obvious) in GNU documentation. \f2  
\par \f0 
\par You can find more detailed information about operations on directories in \cf1\strike File System Interface\strike0\{linkID=820\}\cf0 .
\par 
\par }
580
PAGE_155
Disadvantages of Alloca
alloca;disadvantagesofalloca



Imported



FALSE
16
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fmodern\fcharset0 Courier New;}{\f3\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Disadvantages of Alloca \{keepn\}
\par \pard\fs18 Next: \cf1\strike Variable-Size Arrays\strike0\{linkID=1070\}\cf0 , Previous: \cf1\strike Advantages of Alloca\strike0\{linkID=50\}\cf0 , Up: \cf1\strike Variable Size Automatic\strike0\{linkID=2400\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Disadvantages of \f1 alloca\b0\f0 
\par 
\par These are the disadvantages of \f1 alloca\f0  in comparison with\f1  malloc\f0 :
\par 
\par \pard\li500\f2\'b7\f0  If you try to allocate more memory than the machine can provide, you don't get a clean error message. Instead you get a fatal signal like the one you would get from an infinite recursion; probably a segmentation violation (see \cf1\strike Program Error Signals\strike0\{linkID=1760\}\cf0 ). 
\par \f2\'b7\f0  Some \f3 other\f0  systems fail to support \f1 alloca\f0 , so it is less portable. However, a slower emulation of \f1 alloca\f0  written in C is available for use on systems with this deficiency.
\par 
\par }
590
PAGE_156
Documentation License
FDL,GNUFreeDocumentationLicense



Imported



FALSE
129
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern\fcharset0 Courier New;}{\f2\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Documentation License \{keepn\}
\par \pard\fs18 Next: \cf1\strike Concept Index\strike0\{link=PAGE_106\}\cf0 , Previous: \cf1\strike Copying\strike0\{linkID=470\}\cf0 , Up: \cf1\strike Top\strike0\{linkID=10\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b GNU Free Documentation License\b0 
\par 
\par \pard\qc Version 1.1, March 2000
\par \pard 
\par \f1      Copyright \'a9 2000 Free Software Foundation, Inc.\f2 
\par      59 Temple Place, Suite 330, Boston, MA  02111-1307, USA
\par      
\par      Everyone is permitted to copy and distribute verbatim copies
\par      of this license document, but changing it is not allowed.
\par \f0 
\par \pard\li500 1. PREAMBLE 
\par 
\par The purpose of this License is to make a manual, textbook, or other written document \i free\i0  in the sense of freedom: to assure everyone the effective freedom to copy and redistribute it, with or without modifying it, either commercially or noncommercially. Secondarily, this License preserves for the author and publisher a way to get credit for their work, while not being considered responsible for modifications made by others. 
\par 
\par This License is a kind of "copyleft", which means that derivative works of the document must themselves be free in the same sense. It complements the GNU General Public License, which is a copyleft license designed for free software. 
\par 
\par We have designed this License in order to use it for manuals for free software, because free software needs free documentation: a free program should come with manuals providing the same freedoms that the software does. But this License is not limited to software manuals; it can be used for any textual work, regardless of subject matter or whether it is published as a printed book. We recommend this License principally for works whose purpose is instruction or reference. 
\par 2. APPLICABILITY AND DEFINITIONS 
\par 
\par This License applies to any manual or other work that contains a notice placed by the copyright holder saying it can be distributed under the terms of this License. The "Document", below, refers to any such manual or work. Any member of the public is a licensee, and is addressed as "you". 
\par 
\par A "Modified Version" of the Document means any work containing the Document or a portion of it, either copied verbatim, or with modifications and/or translated into another language. 
\par 
\par A "Secondary Section" is a named appendix or a front-matter section of the Document that deals exclusively with the relationship of the publishers or authors of the Document to the Document's overall subject (or to related matters) and contains nothing that could fall directly within that overall subject. (For example, if the Document is in part a textbook of mathematics, a Secondary Section may not explain any mathematics.) The relationship could be a matter of historical connection with the subject or with related matters, or of legal, commercial, philosophical, ethical or political position regarding them. 
\par 
\par The "Invariant Sections" are certain Secondary Sections whose titles are designated, as being those of Invariant Sections, in the notice that says that the Document is released under this License. 
\par 
\par The "Cover Texts" are certain short passages of text that are listed, as Front-Cover Texts or Back-Cover Texts, in the notice that says that the Document is released under this License. 
\par 
\par A "Transparent" copy of the Document means a machine-readable copy, represented in a format whose specification is available to the general public, whose contents can be viewed and edited directly and straightforwardly with generic text editors or (for images composed of pixels) generic paint programs or (for drawings) some widely available drawing editor, and that is suitable for input to text formatters or for automatic translation to a variety of formats suitable for input to text formatters. A copy made in an otherwise Transparent file format whose markup has been designed to thwart or discourage subsequent modification by readers is not Transparent. A copy that is not "Transparent" is called "Opaque". 
\par 
\par Examples of suitable formats for Transparent copies include plain ascii without markup, Texinfo input format, LaTeX input format, SGML or XML using a publicly available DTD, and standard-conforming simple HTML designed for human modification. Opaque formats include PostScript, PDF, proprietary formats that can be read and edited only by proprietary word processors, SGML or XML for which the DTD and/or processing tools are not generally available, and the machine-generated HTML produced by some word processors for output purposes only. 
\par 
\par The "Title Page" means, for a printed book, the title page itself, plus such following pages as are needed to hold, legibly, the material this License requires to appear in the title page. For works in formats which do not have any title page as such, "Title Page" means the text near the most prominent appearance of the work's title, preceding the beginning of the body of the text. 
\par 3. VERBATIM COPYING 
\par 
\par You may copy and distribute the Document in any medium, either commercially or noncommercially, provided that this License, the copyright notices, and the license notice saying this License applies to the Document are reproduced in all copies, and that you add no other conditions whatsoever to those of this License. You may not use technical measures to obstruct or control the reading or further copying of the copies you make or distribute. However, you may accept compensation in exchange for copies. If you distribute a large enough number of copies you must also follow the conditions in section 3. 
\par 
\par You may also lend copies, under the same conditions stated above, and you may publicly display copies. 
\par 4. COPYING IN QUANTITY 
\par 
\par If you publish printed copies of the Document numbering more than 100, and the Document's license notice requires Cover Texts, you must enclose the copies in covers that carry, clearly and legibly, all these Cover Texts: Front-Cover Texts on the front cover, and Back-Cover Texts on the back cover. Both covers must also clearly and legibly identify you as the publisher of these copies. The front cover must present the full title with all words of the title equally prominent and visible. You may add other material on the covers in addition. Copying with changes limited to the covers, as long as they preserve the title of the Document and satisfy these conditions, can be treated as verbatim copying in other respects. 
\par 
\par If the required texts for either cover are too voluminous to fit legibly, you should put the first ones listed (as many as fit reasonably) on the actual cover, and continue the rest onto adjacent pages. 
\par 
\par If you publish or distribute Opaque copies of the Document numbering more than 100, you must either include a machine-readable Transparent copy along with each Opaque copy, or state in or with each Opaque copy a publicly-accessible computer-network location containing a complete Transparent copy of the Document, free of added material, which the general network-using public has access to download anonymously at no charge using public-standard network protocols. If you use the latter option, you must take reasonably prudent steps, when you begin distribution of Opaque copies in quantity, to ensure that this Transparent copy will remain thus accessible at the stated location until at least one year after the last time you distribute an Opaque copy (directly or through your agents or retailers) of that edition to the public. 
\par 
\par It is requested, but not required, that you contact the authors of the Document well before redistributing any large number of copies, to give them a chance to provide you with an updated version of the Document. 
\par 5. MODIFICATIONS 
\par 
\par You may copy and distribute a Modified Version of the Document under the conditions of sections 2 and 3 above, provided that you release the Modified Version under precisely this License, with the Modified Version filling the role of the Document, thus licensing distribution and modification of the Modified Version to whoever possesses a copy of it. In addition, you must do these things in the Modified Version:
\par 
\par \pard\li1000 1. Use in the Title Page (and on the covers, if any) a title distinct from that of the Document, and from those of previous versions (which should, if there were any, be listed in the History section of the Document). You may use the same title as a previous version if the original publisher of that version gives permission. 
\par 2. List on the Title Page, as authors, one or more persons or entities responsible for authorship of the modifications in the Modified Version, together with at least five of the principal authors of the Document (all of its principal authors, if it has less than five). 
\par 3. State on the Title page the name of the publisher of the Modified Version, as the publisher. 
\par 4. Preserve all the copyright notices of the Document. 
\par 5. Add an appropriate copyright notice for your modifications adjacent to the other copyright notices. 
\par 6. Include, immediately after the copyright notices, a license notice giving the public permission to use the Modified Version under the terms of this License, in the form shown in the Addendum below. 
\par 7. Preserve in that license notice the full lists of Invariant Sections and required Cover Texts given in the Document's license notice. 
\par 8. Include an unaltered copy of this License. 
\par 9. Preserve the section entitled "History", and its title, and add to it an item stating at least the title, year, new authors, and publisher of the Modified Version as given on the Title Page. If there is no section entitled "History" in the Document, create one stating the title, year, authors, and publisher of the Document as given on its Title Page, then add an item describing the Modified Version as stated in the previous sentence. 
\par 10. Preserve the network location, if any, given in the Document for public access to a Transparent copy of the Document, and likewise the network locations given in the Document for previous versions it was based on. These may be placed in the "History" section. You may omit a network location for a work that was published at least four years before the Document itself, or if the original publisher of the version it refers to gives permission. 
\par 11. In any section entitled "Acknowledgments" or "Dedications", preserve the section's title, and preserve in the section all the substance and tone of each of the contributor acknowledgments and/or dedications given therein. 
\par 12. Preserve all the Invariant Sections of the Document, unaltered in their text and in their titles. Section numbers or the equivalent are not considered part of the section titles. 
\par 13. Delete any section entitled "Endorsements". Such a section may not be included in the Modified Version. 
\par 14. Do not retitle any existing section as "Endorsements" or to conflict in title with any Invariant Section.
\par 
\par \pard\li500 If the Modified Version includes new front-matter sections or appendices that qualify as Secondary Sections and contain no material copied from the Document, you may at your option designate some or all of these sections as invariant. To do this, add their titles to the list of Invariant Sections in the Modified Version's license notice. These titles must be distinct from any other section titles. 
\par 
\par You may add a section entitled "Endorsements", provided it contains nothing but endorsements of your Modified Version by various parties--for example, statements of peer review or that the text has been approved by an organization as the authoritative definition of a standard. 
\par 
\par You may add a passage of up to five words as a Front-Cover Text, and a passage of up to 25 words as a Back-Cover Text, to the end of the list of Cover Texts in the Modified Version. Only one passage of Front-Cover Text and one of Back-Cover Text may be added by (or through arrangements made by) any one entity. If the Document already includes a cover text for the same cover, previously added by you or by arrangement made by the same entity you are acting on behalf of, you may not add another; but you may replace the old one, on explicit permission from the previous publisher that added the old one. 
\par 
\par The author(s) and publisher(s) of the Document do not by this License give permission to use their names for publicity for or to assert or imply endorsement of any Modified Version. 
\par 6. COMBINING DOCUMENTS 
\par 
\par You may combine the Document with other documents released under this License, under the terms defined in section 4 above for modified versions, provided that you include in the combination all of the Invariant Sections of all of the original documents, unmodified, and list them all as Invariant Sections of your combined work in its license notice. 
\par 
\par The combined work need only contain one copy of this License, and multiple identical Invariant Sections may be replaced with a single copy. If there are multiple Invariant Sections with the same name but different contents, make the title of each such section unique by adding at the end of it, in parentheses, the name of the original author or publisher of that section if known, or else a unique number. Make the same adjustment to the section titles in the list of Invariant Sections in the license notice of the combined work. 
\par 
\par In the combination, you must combine any sections entitled "History" in the various original documents, forming one section entitled "History"; likewise combine any sections entitled "Acknowledgments", and any sections entitled "Dedications". You must delete all sections entitled "Endorsements." 
\par 7. COLLECTIONS OF DOCUMENTS 
\par 
\par You may make a collection consisting of the Document and other documents released under this License, and replace the individual copies of this License in the various documents with a single copy that is included in the collection, provided that you follow the rules of this License for verbatim copying of each of the documents in all other respects. 
\par 
\par You may extract a single document from such a collection, and distribute it individually under this License, provided you insert a copy of this License into the extracted document, and follow this License in all other respects regarding verbatim copying of that document. 
\par 8. AGGREGATION WITH INDEPENDENT WORKS 
\par 
\par A compilation of the Document or its derivatives with other separate and independent documents or works, in or on a volume of a storage or distribution medium, does not as a whole count as a Modified Version of the Document, provided no compilation copyright is claimed for the compilation. Such a compilation is called an "aggregate", and this License does not apply to the other self-contained works thus compiled with the Document, on account of their being thus compiled, if they are not themselves derivative works of the Document. 
\par 
\par If the Cover Text requirement of section 3 is applicable to these copies of the Document, then if the Document is less than one quarter of the entire aggregate, the Document's Cover Texts may be placed on covers that surround only the Document within the aggregate. Otherwise they must appear on covers around the whole aggregate. 
\par 9. TRANSLATION 
\par 
\par Translation is considered a kind of modification, so you may distribute translations of the Document under the terms of section 4. Replacing Invariant Sections with translations requires special permission from their copyright holders, but you may include translations of some or all Invariant Sections in addition to the original versions of these Invariant Sections. You may include a translation of this License provided that you also include the original English version of this License. In case of a disagreement between the translation and the original English version of this License, the original English version will prevail. 
\par 10. TERMINATION 
\par 
\par You may not copy, modify, sublicense, or distribute the Document except as expressly provided for under this License. Any other attempt to copy, modify, sublicense or distribute the Document is void, and will automatically terminate your rights under this License. However, parties who have received copies, or rights, from you under this License will not have their licenses terminated so long as such parties remain in full compliance. 
\par 11. FUTURE REVISIONS OF THIS LICENSE 
\par 
\par The Free Software Foundation may publish new, revised versions of the GNU Free Documentation License from time to time. Such new versions will be similar in spirit to the present version, but may differ in detail to address new problems or concerns. See \cf2\strike http://www.gnu.org/copyleft/\strike0\{link=*!ExecFile("http://www.gnu.org/copyleft/")\}\cf0 . 
\par 
\par Each version of the License is given a distinguishing version number. If the Document specifies that a particular numbered version of this License "or any later version" applies to it, you have the option of following the terms and conditions either of that specified version or of any later version that has been published (not as a draft) by the Free Software Foundation. If the Document does not specify a version number of this License, you may choose any version ever published (not as a draft) by the Free Software Foundation.
\par 
\par \pard\b ADDENDUM: How to use this License for your documents\b0 
\par 
\par To use this License in a document you have written, include a copy of the License in the document and put the following copyright and license notices just after the title page:
\par 
\par \f2        Copyright (C)  \i year\i0   \i your name\i0 .
\par        Permission is granted to copy, distribute and/or modify this document
\par        under the terms of the GNU Free Documentation License, Version 1.1
\par        or any later version published by the Free Software Foundation;
\par        with the Invariant Sections being \i list their titles\i0 , with the
\par        Front-Cover Texts being \i list\i0 , and with the Back-Cover Texts being \i list\i0 .
\par        A copy of the license is included in the section entitled ``GNU
\par        Free Documentation License''.
\par \f0 
\par If you have no Invariant Sections, write "with no Invariant Sections" instead of saying which ones are invariant. If you have no Front-Cover Texts, write "no Front-Cover Texts" instead of "Front-Cover Texts being \i list\i0 "; likewise for Back-Cover Texts. 
\par 
\par If your document contains nontrivial examples of program code, we recommend releasing these examples in parallel under your choice of free software license, such as the GNU General Public License, to permit their use in free software.
\par 
\par }
600
PAGE_157
Duplicating Descriptors
duplicatingfiledescriptors;redirectinginputandoutput;unistd.h;fcntl.h;dup;dup;F_DUPFD



Imported



FALSE
42
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fswiss\fcharset0 Arial;}{\f2\fmodern Courier New;}{\f3\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Duplicating Descriptors \{keepn\}
\par \pard\fs18 Next: \cf1\strike File Status Flags\strike0\{linkID=810\}\cf0 , Previous:  Previous: \cf1\strike Duplicating Descriptors\strike0\{linkID=600\}\cf0 , Up: \cf1\strike Low-Level I/O\strike0\{linkID=1360\}\cf0 
\par \fs10 
\par 
\par \pard\ri100\fs2 
\par \pard\b\fs18 Duplicating Descriptors\b0 
\par 
\par You can \i duplicate\i0  a file descriptor, or allocate another file descriptor that refers to the same open file as the original. Duplicate descriptors share one file position and one set of file status flags (see \cf1\strike File Status Flags\strike0\{linkID=810\}\cf0 ), but each has its own set of file descriptor flags\f1 .\f0 
\par 
\par The major use of duplicating a file descriptor is to implement\i  redirection\i0  of input or output: that is, to change the file or pipe that a particular file descriptor corresponds to. 
\par 
\par \f1 T\f0 here are also convenient functions\f2  dup\f0  and \f2 dup2\f0  for duplicating descriptors. 
\par 
\par \f1 P\f0 rototypes for \f2 dup\f0  and \f2 dup2\f0  are in the header file\f2  \f3 io\f2 .h\f0 .
\par \f1 
\par \f0 -- Function: int \b dup\b0  (\i int old\i0 )
\par 
\par \pard\li500 This function copies descriptor \i old\i0  to the first available descriptor number (the first number not currently open).
\par 
\par \pard -- Function: int \b dup2\b0  (\i int old, int new\i0 )
\par 
\par \pard\li500 This function copies the descriptor \i old\i0  to descriptor number\i  new\i0 . 
\par 
\par If \i old\i0  is an invalid descriptor, then \f2 dup2\f0  does nothing; it does not close \i new\i0 . Otherwise, the new duplicate of \i old\i0  replaces any previous meaning of descriptor \i new\i0 , as if \i new\i0  were closed first. 
\par 
\par \pard Here is an example showing how to use \f2 dup2\f0  to do redirection. Typically, redirection of the standard streams (like \f2 stdin\f0 ) is done by a shell or shell-like program before calling one of the\f2  exec\f0  functions (see \cf1\strike Executing a File\strike0\{linkID=700\}\cf0 ) to execute a new program in a child process. When the new program is executed, it creates and initializes the standard streams to point to the corresponding file descriptors, before its \f2 main\f0  function is invoked. 
\par 
\par So, to redirect standard input to a file, the shell could do something like:
\par 
\par \f2          char *filename;
\par          char *program;
\par          int file;
\par          ...
\par          file = TEMP_FAILURE_RETRY (open (filename, O_RDONLY));
\par          dup2 (file, STDIN_FILENO);
\par          TEMP_FAILURE_RETRY (close (file));
\par          execv (program, NULL);
\par 
\par \f0 
\par }
610
PAGE_161
Effects of Locale




Imported



FALSE
26
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern\fcharset0 Courier New;}{\f2\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Effects of Locale \{keepn\}
\par \pard\fs18 Next: \cf1\strike Choosing Locale\strike0\{linkID=320\}\cf0 , Up: \cf1\strike Locales\strike0\{linkID=1340\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b What Effects a Locale Has\b0 
\par 
\par Each locale specifies conventions for several purposes, including the following:
\par 
\par \pard\li500\f1\'b7\f0  What multibyte character sequences are valid, and how they are interpreted (see \cf1\strike Character Set Handling\strike0\{linkID=290\}\cf0 ). 
\par \f1\'b7\f0  Classification of which characters in the local character set are considered alphabetic, and upper- and lower-case conversion conventions (see \cf1\strike Character Handling\strike0\{linkID=270\}\cf0 ). 
\par \f1\'b7\f0  The collating sequence for the local language and character set (see \cf1\strike Collation Functions\strike0\{linkID=370\}\cf0 ). 
\par \f1\'b7\f0  Formatting of numbers and currency amounts (see \cf1\strike General Numeric\strike0\{linkID=1040\}\cf0 ). 
\par \f1\'b7\f0  Formatting of dates and times (see \cf1\strike Formatting Calendar Time\strike0\{linkID=980\}\cf0 ). 
\par \f1\'b7\f0  What language to use for user answers to yes-or-no questions (see \cf1\strike Yes-or-No Questions\strike0\{linkID=2480\}\cf0 ). 
\par 
\par \pard Some aspects of adapting to the specified locale are handled automatically by the library subroutines. For example, all your program needs to do in order to use the collating sequence of the chosen locale is to use \f2 strcoll\f0  or \f2 strxfrm\f0  to compare strings. 
\par 
\par Other aspects of locales are beyond the comprehension of the library. For example, the library can't automatically translate your program's output messages into other languages. The only way you can support output in the user's favorite language is to program this more or less by hand. The C library provides functions to handle translations for multiple languages easily. 
\par 
\par This chapter discusses the mechanism by which you can modify the current locale. The effects of the current locale on specific library functions are discussed in more detail in the descriptions of those functions.
\par 
\par }
620
PAGE_163
Elapsed Time
elapsedtime;difftime;structtimeval;timeval;structtimespec;timespec



Imported



FALSE
20
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fswiss\fcharset0 Arial;}{\f2\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Elapsed Time \{keepn\}
\par \pard\fs18 Next:\f1  \cf1\strike\f0 CPU Time\strike0\{lin\f1 kID=480\f0\}\cf0 , Previous: \cf1\strike Time Basics\strike0\{linkID=2290\}\cf0 , Up: \cf1\strike Date and Time\strike0\{linkID=520\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Elapsed Time\b0 
\par 
\par One way to represent an elapsed time is with a simple arithmetic data type, as with the following function to compute the elapsed time between two calendar times. This function is declared in \f2 time.h\f0 .
\par 
\par -- Function: double \b difftime\b0  (\i time_t time1, time_t time0\i0 )
\par 
\par \pard\li500 The \f2 difftime\f0  function returns the number of seconds of elapsed time between calendar time \i time1\i0  and calendar time \i time0\i0 , as a value of type \f2 double\f0 . The difference ignores leap seconds unless leap second support is enabled. 
\par 
\par In th\f1 is \f0 system, you can simply subtract \f2 time_t\f0  values. But on other systems, the \f2 time_t\f0  data type might use some other encoding where subtraction doesn't work directly.
\par 
\par \f2 
\par }
630
PAGE_166
Environment Access
environmentaccess;environmentrepresentation;stdlib.h;getenv;putenv;setenv;unsetenv;clearenv;environ



Imported



FALSE
40
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fswiss\fcharset0 Arial;}{\f2\fmodern Courier New;}{\f3\fmodern\fcharset0 Courier New;}{\f4\fnil\fcharset0 Courier New;}{\f5\fnil\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Environment Access \{keepn\}
\par \pard\f1\fs18 Previous: \cf1\strike Environment Variables\cf2\strike0\{linkID=640\}\cf0 , \f0 Up: \cf1\strike Environment Variables\strike0\{linkID=640\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Environment Access\b0 
\par 
\par The value of an environment variable can be accessed with the\f2  getenv\f0  function. This is declared in the header file\f2  stdlib.h\f0 . All of the following functions can be safely used in multi-threaded programs. It is made sure that concurrent modifications to the environment do not lead to errors.
\par 
\par -- Function: char * \b getenv\b0  (\i const char *name\i0 )
\par 
\par \pard\li500 This function returns a string that is the value of the environment variable \i name\i0 . You must not modify this string. In some systems it might be overwritten by subsequent calls to \f2 getenv\f0  (but not by any other library function). If the environment variable \i name\i0  is not defined, the value is a null pointer.
\par 
\par \pard -- Function: int \b\f1 pu\f0 tenv\b0  (\i const char *name\i0 )
\par 
\par \pard\li500 The \f3 pu\f2 tenv\f0  function can be used to add a new definition to the environment\f1 , or to modify an existing definition.  As an example:
\par 
\par \tab\tab\f4 putenv("myvar=hello");
\par 
\par \f5 sets the environment variable 'myvar' to the value 'hello'.\f0 
\par 
\par Please note that you cannot remove an entry completely using this function. 
\par 
\par 
\par \pard You can deal directly with the underlying representation of environment objects to add more variables to the environment (for example, to communicate with another program you are about to execute; see \cf1\strike Executing a File\strike0\{linkID=700\}\cf0 ).
\par 
\par -- Variable: char ** \f1 _\b\f0 environ\b0 
\par 
\par \pard\li500 The environment is represented as an array of strings. Each string is of the format `\i\f2 name\i0 =\i value\i0\f0 '. The order in which strings appear in the environment is not significant, but the same\i  name\i0  must not appear more than once. The last element of the array is a null pointer. 
\par 
\par This variable is declared in the header file \f3 stdlib\f2 .h\f0 . 
\par 
\par If you just want to get the value of an environment variable, use\f2  getenv\f0 .
\par 
\par \pard\f1 This system \f0 pass\f1 es\f0  the initial value of\f2  \f3 _\f2 environ\f0  as the third argument to \f2 main\f0 . See \cf1\strike Program Arguments\strike0\{linkID=1740\}\cf0 .
\par 
\par }
640
PAGE_167
Environment Variables
environmentvariable;environment



Imported



FALSE
25
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Environment Variables \{keepn\}
\par \pard\fs18 Previous: \cf1\strike Program Arguments\strike0\{linkID=1740\}\cf0 , Up: \cf1\strike Program Basics\strike0\{linkID=1750\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Environment Variables\b0 
\par 
\par When a program is executed, it receives information about the context in which it was invoked in two ways. The first mechanism uses the\i  argv\i0  and \i argc\i0  arguments to its \f1 main\f0  function, and is discussed in \cf1\strike Program Arguments\strike0\{linkID=1740\}\cf0 . The second mechanism uses\i  environment variables\i0  and is discussed in this section. 
\par 
\par The \i argv\i0  mechanism is typically used to pass command-line arguments specific to the particular program being invoked. The environment, on the other hand, keeps track of information that is shared by many programs, changes infrequently, and that is less frequently used. 
\par 
\par The environment variables discussed in this section are the same environment variables that you set using assignments and the\f1  export\f0  command in the shell. Programs executed from the shell inherit all of the environment variables from the shell. 
\par 
\par Standard environment variables are used for information about the user's home directory, terminal type, current locale, and so on; you can define additional variables for other purposes. The set of all environment variables that have values is collectively known as the\i  environment\i0 . 
\par 
\par Names of environment variables are case-sensitive and must not contain the character `\f1 =\f0 '. System-defined environment variables are invariably uppercase. 
\par 
\par The values of environment variables can be anything that can be represented as a string. A value must not contain an embedded null character, since this is assumed to terminate the string.
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Environment Access\strike0\{linkID=630\}\cf0 : How to get and set the values of environment variables.
\par 
\par }
650
PAGE_169
EOF and Errors
endoffile,onastream;EOF;WEOF;feof;feof_unlocked;ferror;ferror_unlocked



Imported



FALSE
38
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 EOF and Errors \{keepn\}
\par \pard\fs18 Next: \cf1\strike Error Recovery\strike0\{linkID=680\}\cf0 , Previous: \cf1\strike Formatted Input\strike0\{linkID=940\}\cf0 , Up: \cf1\strike I/O on Streams\strike0\{linkID=1260\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b End-Of-File and Errors\b0 
\par 
\par Many of the functions described in this chapter return the value of the macro \f1 EOF\f0  to indicate unsuccessful completion of the operation. Since \f1 EOF\f0  is used to report both end of file and random errors, it's often better to use the \f1 feof\f0  function to check explicitly for end of file and \f1 ferror\f0  to check for errors. These functions check indicators that are part of the internal state of the stream object, indicators set if the appropriate condition was detected by a previous I/O operation on that stream.
\par -- Macro: int \b EOF\b0 
\par 
\par \pard\li500 This macro is an integer value that is returned by a number of narrow stream functions to indicate an end-of-file condition, or some other error situation. With the GNU library, \f1 EOF\f0  is \f1 -1\f0 . In other libraries, its value may be some other negative number. 
\par 
\par This symbol is declared in \f1 stdio.h\f0 .
\par 
\par \pard -- Macro: int \b WEOF\b0 
\par 
\par \pard\li500 This macro is an integer value that is returned by a number of wide stream functions to indicate an end-of-file condition, or some other error situation. With the GNU library, \f1 WEOF\f0  is \f1 -1\f0 . In other libraries, its value may be some other negative number. 
\par 
\par This symbol is declared in \f1 wchar.h\f0 .
\par 
\par \pard -- Function: int \b feof\b0  (\i FILE *stream\i0 )
\par 
\par \pard\li500 The \f1 feof\f0  function returns nonzero if and only if the end-of-file indicator for the stream \i stream\i0  is set. 
\par 
\par This symbol is declared in \f1 stdio.h\f0 .
\par 
\par \pard -- Function: int \b ferror\b0  (\i FILE *stream\i0 )
\par 
\par \pard\li500 The \f1 ferror\f0  function returns nonzero if and only if the error indicator for the stream \i stream\i0  is set, indicating that an error has occurred on a previous operation on the stream. 
\par 
\par This symbol is declared in \f1 stdio.h\f0 .
\par 
\par \pard In addition to setting the error indicator associated with the stream, the functions that operate on streams also set \f1 errno\f0  in the same way as the corresponding low-level functions that operate on file descriptors. For example,  all of the \f1 errno\f0  error conditions defined for \f1 write\f0  are meaningful for functions\f2  \f0 such as \f1 fputc\f0 , \f1 printf\f0 , and \f1 fflush\f0 . For more information about the descriptor-level I/O functions, see \cf1\strike Low-Level I/O\strike0\{linkID=1360\}\cf0 .
\par 
\par }
660
PAGE_170
Error Codes
errno.h;EPERM;ENOENT;ESRCH;EINTR;EIO;ENXIO;E2BIG;ENOEXEC;EBADF;ECHILD;EDEADLK;ENOMEM;EACCES;EFAULT;ENOTBLK;EBUSY;EEXIST;EXDEV;ENODEV;ENOTDIR;EISDIR;EINVAL;EMFILE;ENFILE;ENOTTY;ETXTBSY;EFBIG;ENOSPC;ESPIPE;EROFS;EMLINK;EPIPE;EDOM;ERANGE;EAGAIN;EWOULDBLOCK;EINPROGRESS;EALREADY;ENOTSOCK;EMSGSIZE;EPROTOTYPE;ENOPROTOOPT;EPROTONOSUPPORT;ESOCKTNOSUPPORT;EOPNOTSUPP;EPFNOSUPPORT;EAFNOSUPPORT;EADDRINUSE;EADDRNOTAVAIL;ENETDOWN;ENETUNREACH;ENETRESET;ECONNABORTED;ECONNRESET;ENOBUFS;EISCONN;ENOTCONN;EDESTADDRREQ;ESHUTDOWN;ETOOMANYREFS;ETIMEDOUT;ECONNREFUSED;ELOOP;ENAMETOOLONG;EHOSTDOWN;EHOSTUNREACH;ENOTEMPTY;EPROCLIM;EUSERS;EDQUOT;ESTALE;EREMOTE;EBADRPC;ERPCMISMATCH;EPROGUNAVAIL;EPROGMISMATCH;EPROCUNAVAIL;ENOLCK;EFTYPE;EAUTH;ENEEDAUTH;ENOSYS;ENOTSUP;EILSEQ;EBACKGROUND;EDIED;ED;EGREGIOUS;EIEIO;EGRATUITOUS;EBADMSG;EIDRM;EMULTIHOP;ENODATA;ENOLINK;ENOMSG;ENOSR;ENOSTR;EOVERFLOW;EPROTO;ETIME;ERESTART;ECHRNG;EL2NSYNC;EL3HLT;EL3RST;ELNRNG;EUNATCH;ENOCSI;EL2HLT;EBADE;EBADR;EXFULL;ENOANO;EBADRQC;EBADSLT;EDEADLOCK;EBFONT;ENONET;ENOPKG;EADV;ESRMNT;ECOMM;EDOTDOT;ENOTUNIQ;EBADFD;EREMCHG;ELIBACC;ELIBBAD;ELIBSCN;ELIBMAX;ELIBEXEC;ESTRPIPE;EUCLEAN;ENOTNAM;ENAVAIL;EISNAM;EREMOTEIO;ENOMEDIUM;EMEDIUMTYPE



Imported



FALSE
490
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Error Codes \{keepn\}
\par \pard\fs18 Next: \cf1\strike Error Messages\strike0\{linkID=670\}\cf0 , Previous: \cf1\strike Checking for Errors\strike0\{linkID=310\}\cf0 , Up: \cf1\strike Error Reporting\strike0\{linkID=690\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Error Codes\b0 
\par 
\par The error code macros are defined in the header file \f1 errno.h\f0 . All of them expand into integer constant values. Some of these error codes can't occur on the GNU system, but they can occur using the GNU library on other systems.
\par -- Macro: int \b EPERM\b0 
\par 
\par \pard\li500 Operation not permitted; only the owner of the file (or other resource) or processes with special privileges can perform the operation.
\par 
\par \pard -- Macro: int \b ENOENT\b0 
\par 
\par \pard\li500 No such file or directory. This is a "file doesn't exist" error for ordinary files that are referenced in contexts where they are expected to already exist.
\par 
\par \pard -- Macro: int \b ESRCH\b0 
\par 
\par \pard\li500 No process matches the specified process ID.
\par 
\par \pard -- Macro: int \b EINTR\b0 
\par 
\par \pard\li500 Interrupted function call; an asynchronous signal occurred and prevented completion of the call. When this happens, you should try the call again. 
\par 
\par You can choose to have functions resume after a signal that is handled, rather than failing with \f1 EINTR\f0 ; see \cf1\strike Interrupted Primitives\strike0\{link=PAGE_304\}\cf0 .
\par 
\par \pard -- Macro: int \b EIO\b0 
\par 
\par \pard\li500 Input/output error; usually used for physical read or write errors.
\par 
\par \pard -- Macro: int \b ENXIO\b0 
\par 
\par \pard\li500 No such device or address. The system tried to use the device represented by a file you specified, and it couldn't find the device. This can mean that the device file was installed incorrectly, or that the physical device is missing or not correctly attached to the computer.
\par 
\par \pard -- Macro: int \b E2BIG\b0 
\par 
\par \pard\li500 Argument list too long; used when the arguments passed to a new program being executed with one of the \f1 exec\f0  functions (see \cf1\strike Executing a File\strike0\{linkID=700\}\cf0 ) occupy too much memory space. This condition never arises in the GNU system.
\par 
\par \pard -- Macro: int \b ENOEXEC\b0 
\par 
\par \pard\li500 Invalid executable file format. This condition is detected by the\f1  exec\f0  functions; see \cf1\strike Executing a File\strike0\{linkID=700\}\cf0 .
\par 
\par \pard -- Macro: int \b EBADF\b0 
\par 
\par \pard\li500 Bad file descriptor; for example, I/O on a descriptor that has been closed or reading from a descriptor open only for writing (or vice versa).
\par 
\par \pard -- Macro: int \b ECHILD\b0 
\par 
\par \pard\li500 There are no child processes. This error happens on operations that are supposed to manipulate child processes, when there aren't any processes to manipulate.
\par 
\par \pard -- Macro: int \b EDEADLK\b0 
\par 
\par \pard\li500 Deadlock avoided; allocating a system resource would have resulted in a deadlock situation. The system does not guarantee that it will notice all such situations. This error means you got lucky and the system noticed; it might just hang. See \cf1\strike File Locks\strike0\{link=PAGE_191\}\cf0 , for an example.
\par 
\par \pard -- Macro: int \b ENOMEM\b0 
\par 
\par \pard\li500 No memory available. The system cannot allocate more virtual memory because its capacity is full.
\par 
\par \pard -- Macro: int \b EACCES\b0 
\par 
\par \pard\li500 Permission denied; the file permissions do not allow the attempted operation.
\par 
\par \pard -- Macro: int \b EFAULT\b0 
\par 
\par \pard\li500 Bad address; an invalid pointer was detected. In the GNU system, this error never happens; you get a signal instead.
\par 
\par \pard -- Macro: int \b ENOTBLK\b0 
\par 
\par \pard\li500 A file that isn't a block special file was given in a situation that requires one. For example, trying to mount an ordinary file as a file system in Unix gives this error.
\par 
\par \pard -- Macro: int \b EBUSY\b0 
\par 
\par \pard\li500 Resource busy; a system resource that can't be shared is already in use. For example, if you try to delete a file that is the root of a currently mounted filesystem, you get this error.
\par 
\par \pard -- Macro: int \b EEXIST\b0 
\par 
\par \pard\li500 File exists; an existing file was specified in a context where it only makes sense to specify a new file.
\par 
\par \pard -- Macro: int \b EXDEV\b0 
\par 
\par \pard\li500 An attempt to make an improper link across file systems was detected. This happens not only when you use \f1 link\f0  (see \cf1\strike Hard Links\strike0\{link=PAGE_260\}\cf0 ) but also when you rename a file with \f1 rename\f0  (see \cf1\strike Renaming Files\strike0\{linkID=1840\}\cf0 ).
\par 
\par \pard -- Macro: int \b ENODEV\b0 
\par 
\par \pard\li500 The wrong type of device was given to a function that expects a particular sort of device.
\par 
\par \pard -- Macro: int \b ENOTDIR\b0 
\par 
\par \pard\li500 A file that isn't a directory was specified when a directory is required.
\par 
\par \pard -- Macro: int \b EISDIR\b0 
\par 
\par \pard\li500 File is a directory; you cannot open a directory for writing, or create or remove hard links to it.
\par 
\par \pard -- Macro: int \b EINVAL\b0 
\par 
\par \pard\li500 Invalid argument. This is used to indicate various kinds of problems with passing the wrong argument to a library function.
\par 
\par \pard -- Macro: int \b EMFILE\b0 
\par 
\par \pard\li500 The current process has too many files open and can't open any more. Duplicate descriptors do count toward this limit. 
\par 
\par In BSD and GNU, the number of open files is controlled by a resource limit that can usually be increased. If you get this error, you might want to increase the \f1 RLIMIT_NOFILE\f0  limit or make it unlimited; see \cf1\strike Limits on Resources\strike0\{link=PAGE_327\}\cf0 .
\par 
\par \pard -- Macro: int \b ENFILE\b0 
\par 
\par \pard\li500 There are too many distinct file openings in the entire system. Note that any number of linked channels count as just one file opening; see \cf1\strike Linked Channels\strike0\{link=PAGE_331\}\cf0 . This error never occurs in the GNU system.
\par 
\par \pard -- Macro: int \b ENOTTY\b0 
\par 
\par \pard\li500 Inappropriate I/O control operation, such as trying to set terminal modes on an ordinary file.
\par 
\par \pard -- Macro: int \b ETXTBSY\b0 
\par 
\par \pard\li500 An attempt to execute a file that is currently open for writing, or write to a file that is currently being executed. Often using a debugger to run a program is considered having it open for writing and will cause this error. (The name stands for "text file busy".) This is not an error in the GNU system; the text is copied as necessary.
\par 
\par \pard -- Macro: int \b EFBIG\b0 
\par 
\par \pard\li500 File too big; the size of a file would be larger than allowed by the system.
\par 
\par \pard -- Macro: int \b ENOSPC\b0 
\par 
\par \pard\li500 No space left on device; write operation on a file failed because the disk is full.
\par 
\par \pard -- Macro: int \b ESPIPE\b0 
\par 
\par \pard\li500 Invalid seek operation (such as on a pipe).
\par 
\par \pard -- Macro: int \b EROFS\b0 
\par 
\par \pard\li500 An attempt was made to modify something on a read-only file system.
\par 
\par \pard -- Macro: int \b EMLINK\b0 
\par 
\par \pard\li500 Too many links; the link count of a single file would become too large.\f1  rename\f0  can cause this error if the file being renamed already has as many links as it can take (see \cf1\strike Renaming Files\strike0\{linkID=1840\}\cf0 ).
\par 
\par \pard -- Macro: int \b EPIPE\b0 
\par 
\par \pard\li500 Broken pipe; there is no process reading from the other end of a pipe. Every library function that returns this error code also generates a\f1  SIGPIPE\f0  signal; this signal terminates the program if not handled or blocked. Thus, your program will never actually see \f1 EPIPE\f0  unless it has handled or blocked \f1 SIGPIPE\f0 .
\par 
\par \pard -- Macro: int \b EDOM\b0 
\par 
\par \pard\li500 Domain error; used by mathematical functions when an argument value does not fall into the domain over which the function is defined.
\par 
\par \pard -- Macro: int \b ERANGE\b0 
\par 
\par \pard\li500 Range error; used by mathematical functions when the result value is not representable because of overflow or underflow.
\par 
\par \pard -- Macro: int \b EAGAIN\b0 
\par 
\par \pard\li500 Resource temporarily unavailable; the call might work if you try again later. The macro \f1 EWOULDBLOCK\f0  is another name for \f1 EAGAIN\f0 ; they are always the same in the C library. 
\par 
\par This error can happen in a few different situations:
\par 
\par \pard\li1000\f2\'b7\f0  An operation that would block was attempted on an object that has non-blocking mode selected. Trying the same operation again will block until some external condition makes it possible to read, write, or connect (whatever the operation). You can use \f1 select\f0  to find out when the operation will be possible; see \cf1\strike Waiting for I/O\strike0\{link=PAGE_682\}\cf0 . 
\par 
\par \b Portability Note:\b0  In many older Unix systems, this condition was indicated by \f1 EWOULDBLOCK\f0 , which was a distinct error code different from \f1 EAGAIN\f0 . To make your program portable, you should check for both codes and treat them the same. 
\par \f2\'b7\f0  A temporary resource shortage made an operation impossible. \f1 fork\f0  can return this error. It indicates that the shortage is expected to pass, so your program can try the call again later and it may succeed. It is probably a good idea to delay for a few seconds before trying it again, to allow time for other processes to release scarce resources. Such shortages are usually fairly serious and affect the whole system, so usually an interactive program should report the error to the user and return to its command loop.
\par 
\par \pard -- Macro: int \b EWOULDBLOCK\b0 
\par 
\par \pard\li500 In the C library, this is another name for \f1 EAGAIN\f0  (above). The values are always the same, on every operating system. 
\par 
\par C libraries in many older Unix systems have \f1 EWOULDBLOCK\f0  as a separate error code.
\par 
\par \pard -- Macro: int \b EINPROGRESS\b0 
\par 
\par \pard\li500 An operation that cannot complete immediately was initiated on an object that has non-blocking mode selected. Some functions that must always block (such as \f1 connect\f0 ; see \cf1\strike Connecting\strike0\{link=PAGE_112\}\cf0 ) never return\f1  EAGAIN\f0 . Instead, they return \f1 EINPROGRESS\f0  to indicate that the operation has begun and will take some time. Attempts to manipulate the object before the call completes return \f1 EALREADY\f0 . You can use the \f1 select\f0  function to find out when the pending operation has completed; see \cf1\strike Waiting for I/O\strike0\{link=PAGE_682\}\cf0 .
\par 
\par \pard -- Macro: int \b EALREADY\b0 
\par 
\par \pard\li500 An operation is already in progress on an object that has non-blocking mode selected.
\par 
\par \pard -- Macro: int \b ENOTSOCK\b0 
\par 
\par \pard\li500 A file that isn't a socket was specified when a socket is required.
\par 
\par \pard -- Macro: int \b EMSGSIZE\b0 
\par 
\par \pard\li500 The size of a message sent on a socket was larger than the supported maximum size.
\par 
\par \pard -- Macro: int \b EPROTOTYPE\b0 
\par 
\par \pard\li500 The socket type does not support the requested communications protocol.
\par 
\par \pard -- Macro: int \b ENOPROTOOPT\b0 
\par 
\par \pard\li500 You specified a socket option that doesn't make sense for the particular protocol being used by the socket. See \cf1\strike Socket Options\strike0\{link=PAGE_563\}\cf0 .
\par 
\par \pard -- Macro: int \b EPROTONOSUPPORT\b0 
\par 
\par \pard\li500 The socket domain does not support the requested communications protocol (perhaps because the requested protocol is completely invalid). See \cf1\strike Creating a Socket\strike0\{link=PAGE_131\}\cf0 .
\par 
\par \pard -- Macro: int \b ESOCKTNOSUPPORT\b0 
\par 
\par \pard\li500 The socket type is not supported.
\par 
\par \pard -- Macro: int \b EOPNOTSUPP\b0 
\par 
\par \pard\li500 The operation you requested is not supported. Some socket functions don't make sense for all types of sockets, and others may not be implemented for all communications protocols. In the GNU system, this error can happen for many calls when the object does not support the particular operation; it is a generic indication that the server knows nothing to do for that call.
\par 
\par \pard -- Macro: int \b EPFNOSUPPORT\b0 
\par 
\par \pard\li500 The socket communications protocol family you requested is not supported.
\par 
\par \pard -- Macro: int \b EAFNOSUPPORT\b0 
\par 
\par \pard\li500 The address family specified for a socket is not supported; it is inconsistent with the protocol being used on the socket. See \cf1\strike Sockets\strike0\{link=PAGE_565\}\cf0 .
\par 
\par \pard -- Macro: int \b EADDRINUSE\b0 
\par 
\par \pard\li500 The requested socket address is already in use. See \cf1\strike Socket Addresses\strike0\{link=PAGE_559\}\cf0 .
\par 
\par \pard -- Macro: int \b EADDRNOTAVAIL\b0 
\par 
\par \pard\li500 The requested socket address is not available; for example, you tried to give a socket a name that doesn't match the local host name. See \cf1\strike Socket Addresses\strike0\{link=PAGE_559\}\cf0 .
\par 
\par \pard -- Macro: int \b ENETDOWN\b0 
\par 
\par \pard\li500 A socket operation failed because the network was down.
\par 
\par \pard -- Macro: int \b ENETUNREACH\b0 
\par 
\par \pard\li500 A socket operation failed because the subnet containing the remote host was unreachable.
\par 
\par \pard -- Macro: int \b ENETRESET\b0 
\par 
\par \pard\li500 A network connection was reset because the remote host crashed.
\par 
\par \pard -- Macro: int \b ECONNABORTED\b0 
\par 
\par \pard\li500 A network connection was aborted locally.
\par 
\par \pard -- Macro: int \b ECONNRESET\b0 
\par 
\par \pard\li500 A network connection was closed for reasons outside the control of the local host, such as by the remote machine rebooting or an unrecoverable protocol violation.
\par 
\par \pard -- Macro: int \b ENOBUFS\b0 
\par 
\par \pard\li500 The kernel's buffers for I/O operations are all in use. In GNU, this error is always synonymous with \f1 ENOMEM\f0 ; you may get one or the other from network operations.
\par 
\par \pard -- Macro: int \b EISCONN\b0 
\par 
\par \pard\li500 You tried to connect a socket that is already connected. See \cf1\strike Connecting\strike0\{link=PAGE_112\}\cf0 .
\par 
\par \pard -- Macro: int \b ENOTCONN\b0 
\par 
\par \pard\li500 The socket is not connected to anything. You get this error when you try to transmit data over a socket, without first specifying a destination for the data. For a connectionless socket (for datagram protocols, such as UDP), you get \f1 EDESTADDRREQ\f0  instead.
\par 
\par \pard -- Macro: int \b EDESTADDRREQ\b0 
\par 
\par \pard\li500 No default destination address was set for the socket. You get this error when you try to transmit data over a connectionless socket, without first specifying a destination for the data with \f1 connect\f0 .
\par 
\par \pard -- Macro: int \b ESHUTDOWN\b0 
\par 
\par \pard\li500 The socket has already been shut down.
\par 
\par \pard -- Macro: int \b ETOOMANYREFS\b0 
\par 
\par \pard\li500 ???
\par 
\par \pard -- Macro: int \b ETIMEDOUT\b0 
\par 
\par \pard\li500 A socket operation with a specified timeout received no response during the timeout period.
\par 
\par \pard -- Macro: int \b ECONNREFUSED\b0 
\par 
\par \pard\li500 A remote host refused to allow the network connection (typically because it is not running the requested service).
\par 
\par \pard -- Macro: int \b ELOOP\b0 
\par 
\par \pard\li500 Too many levels of symbolic links were encountered in looking up a file name. This often indicates a cycle of symbolic links.
\par 
\par \pard -- Macro: int \b ENAMETOOLONG\b0 
\par 
\par \pard\li500 Filename too long (longer than \f1 PATH_MAX\f0 ; see \cf1\strike Limits for Files\strike0\{link=PAGE_326\}\cf0 ) or host name too long (in \f1 gethostname\f0  or\f1  sethostname\f0 ; see \cf1\strike Host Identification\strike0\{link=PAGE_273\}\cf0 ).
\par 
\par \pard -- Macro: int \b EHOSTDOWN\b0 
\par 
\par \pard\li500 The remote host for a requested network connection is down.
\par 
\par \pard -- Macro: int \b EHOSTUNREACH\b0 
\par 
\par \pard\li500 The remote host for a requested network connection is not reachable.
\par 
\par \pard -- Macro: int \b ENOTEMPTY\b0 
\par 
\par \pard\li500 Directory not empty, where an empty directory was expected. Typically, this error occurs when you are trying to delete a directory.
\par 
\par \pard -- Macro: int \b EPROCLIM\b0 
\par 
\par \pard\li500 This means that the per-user limit on new process would be exceeded by an attempted \f1 fork\f0 . See \cf1\strike Limits on Resources\strike0\{link=PAGE_327\}\cf0 , for details on the \f1 RLIMIT_NPROC\f0  limit.
\par 
\par \pard -- Macro: int \b EUSERS\b0 
\par 
\par \pard\li500 The file quota system is confused because there are too many users.
\par 
\par \pard -- Macro: int \b EDQUOT\b0 
\par 
\par \pard\li500 The user's disk quota was exceeded.
\par 
\par \pard -- Macro: int \b ESTALE\b0 
\par 
\par \pard\li500 Stale NFS file handle. This indicates an internal confusion in the NFS system which is due to file system rearrangements on the server host. Repairing this condition usually requires unmounting and remounting the NFS file system on the local host.
\par 
\par \pard -- Macro: int \b EREMOTE\b0 
\par 
\par \pard\li500 An attempt was made to NFS-mount a remote file system with a file name that already specifies an NFS-mounted file. (This is an error on some operating systems, but we expect it to work properly on the GNU system, making this error code impossible.)
\par 
\par \pard -- Macro: int \b EBADRPC\b0 
\par 
\par \pard\li500 ???
\par 
\par \pard -- Macro: int \b ERPCMISMATCH\b0 
\par 
\par \pard\li500 ???
\par 
\par \pard -- Macro: int \b EPROGUNAVAIL\b0 
\par 
\par \pard\li500 ???
\par 
\par \pard -- Macro: int \b EPROGMISMATCH\b0 
\par 
\par \pard\li500 ???
\par 
\par \pard -- Macro: int \b EPROCUNAVAIL\b0 
\par 
\par \pard\li500 ???
\par 
\par \pard -- Macro: int \b ENOLCK\b0 
\par 
\par \pard\li500 No locks available. This is used by the file locking facilities; see \cf1\strike File Locks\strike0\{link=PAGE_191\}\cf0 . This error is never generated by the GNU system, but it can result from an operation to an NFS server running another operating system.
\par 
\par \pard -- Macro: int \b EFTYPE\b0 
\par 
\par \pard\li500 Inappropriate file type or format. The file was the wrong type for the operation, or a data file had the wrong format. 
\par 
\par On some systems \f1 chmod\f0  returns this error if you try to set the sticky bit on a non-directory file; see \cf1\strike Setting Permissions\strike0\{link=PAGE_535\}\cf0 .
\par 
\par \pard -- Macro: int \b EAUTH\b0 
\par 
\par \pard\li500 ???
\par 
\par \pard -- Macro: int \b ENEEDAUTH\b0 
\par 
\par \pard\li500 ???
\par 
\par \pard -- Macro: int \b ENOSYS\b0 
\par 
\par \pard\li500 Function not implemented. This indicates that the function called is not implemented at all, either in the C library itself or in the operating system. When you get this error, you can be sure that this particular function will always fail with \f1 ENOSYS\f0  unless you install a new version of the C library or the operating system.
\par 
\par \pard -- Macro: int \b ENOTSUP\b0 
\par 
\par \pard\li500 Not supported. A function returns this error when certain parameter values are valid, but the functionality they request is not available. This can mean that the function does not implement a particular command or option value or flag bit at all. For functions that operate on some object given in a parameter, such as a file descriptor or a port, it might instead mean that only \i that specific object\i0  (file descriptor, port, etc.) is unable to support the other parameters given; different file descriptors might support different ranges of parameter values. 
\par 
\par If the entire function is not available at all in the implementation, it returns \f1 ENOSYS\f0  instead.
\par 
\par \pard -- Macro: int \b EILSEQ\b0 
\par 
\par \pard\li500 While decoding a multibyte character the function came along an invalid or an incomplete sequence of bytes or the given wide character is invalid.
\par 
\par \pard -- Macro: int \b EBACKGROUND\b0 
\par 
\par \pard\li500 In the GNU system, servers supporting the \f1 term\f0  protocol return this error for certain operations when the caller is not in the foreground process group of the terminal. Users do not usually see this error because functions such as \f1 read\f0  and \f1 write\f0  translate it into a \f1 SIGTTIN\f0  or \f1 SIGTTOU\f0  signal. See \cf1\strike Job Control\strike0\{link=PAGE_318\}\cf0 , for information on process groups and these signals.
\par 
\par \pard -- Macro: int \b EDIED\b0 
\par 
\par \pard\li500 In the GNU system, opening a file returns this error when the file is translated by a program and the translator program dies while starting up, before it has connected to the file.
\par 
\par \pard -- Macro: int \b ED\b0 
\par 
\par \pard\li500 The experienced user will know what is wrong.
\par 
\par \pard -- Macro: int \b EGREGIOUS\b0 
\par 
\par \pard\li500 You did \b what\b0 ?
\par 
\par \pard -- Macro: int \b EIEIO\b0 
\par 
\par \pard\li500 Go home and have a glass of warm, dairy-fresh milk.
\par 
\par \pard -- Macro: int \b EGRATUITOUS\b0 
\par 
\par \pard\li500 This error code has no purpose.
\par 
\par \pard -- Macro: int \b EBADMSG\b0 
\par 
\par -- Macro: int \b EIDRM\b0 
\par 
\par -- Macro: int \b EMULTIHOP\b0 
\par 
\par -- Macro: int \b ENODATA\b0 
\par 
\par -- Macro: int \b ENOLINK\b0 
\par 
\par -- Macro: int \b ENOMSG\b0 
\par 
\par -- Macro: int \b ENOSR\b0 
\par 
\par -- Macro: int \b ENOSTR\b0 
\par 
\par -- Macro: int \b EOVERFLOW\b0 
\par 
\par -- Macro: int \b EPROTO\b0 
\par 
\par -- Macro: int \b ETIME\b0 
\par 
\par \i The following error codes are defined by the Linux/i386 kernel. They are not yet documented.\i0 
\par -- Macro: int \b ERESTART\b0 
\par 
\par -- Macro: int \b ECHRNG\b0 
\par 
\par -- Macro: int \b EL2NSYNC\b0 
\par 
\par -- Macro: int \b EL3HLT\b0 
\par 
\par -- Macro: int \b EL3RST\b0 
\par 
\par -- Macro: int \b ELNRNG\b0 
\par 
\par -- Macro: int \b EUNATCH\b0 
\par 
\par -- Macro: int \b ENOCSI\b0 
\par 
\par -- Macro: int \b EL2HLT\b0 
\par 
\par -- Macro: int \b EBADE\b0 
\par 
\par -- Macro: int \b EBADR\b0 
\par 
\par -- Macro: int \b EXFULL\b0 
\par 
\par -- Macro: int \b ENOANO\b0 
\par 
\par -- Macro: int \b EBADRQC\b0 
\par 
\par -- Macro: int \b EBADSLT\b0 
\par 
\par -- Macro: int \b EDEADLOCK\b0 
\par 
\par -- Macro: int \b EBFONT\b0 
\par 
\par -- Macro: int \b ENONET\b0 
\par 
\par -- Macro: int \b ENOPKG\b0 
\par 
\par -- Macro: int \b EADV\b0 
\par 
\par -- Macro: int \b ESRMNT\b0 
\par 
\par -- Macro: int \b ECOMM\b0 
\par 
\par -- Macro: int \b EDOTDOT\b0 
\par 
\par -- Macro: int \b ENOTUNIQ\b0 
\par 
\par -- Macro: int \b EBADFD\b0 
\par 
\par -- Macro: int \b EREMCHG\b0 
\par 
\par -- Macro: int \b ELIBACC\b0 
\par 
\par -- Macro: int \b ELIBBAD\b0 
\par 
\par -- Macro: int \b ELIBSCN\b0 
\par 
\par -- Macro: int \b ELIBMAX\b0 
\par 
\par -- Macro: int \b ELIBEXEC\b0 
\par 
\par -- Macro: int \b ESTRPIPE\b0 
\par 
\par -- Macro: int \b EUCLEAN\b0 
\par 
\par -- Macro: int \b ENOTNAM\b0 
\par 
\par -- Macro: int \b ENAVAIL\b0 
\par 
\par -- Macro: int \b EISNAM\b0 
\par 
\par -- Macro: int \b EREMOTEIO\b0 
\par 
\par -- Macro: int \b ENOMEDIUM\b0 
\par 
\par -- Macro: int \b EMEDIUMTYPE\b0 
\par 
\par }
670
PAGE_171
Error Messages
strerror;strerror_r;perror;programname;nameofrunningprogram;program_invocation_name;program_invocation_short_name;error;error_at_line;error_print_progname;error_message_count;error_one_per_line;warn;vwarn;warnx;vwarnx;err;verr;errx;verrx



Imported



FALSE
62
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 + Error Messages \{keepn\}
\par \pard\fs18 Previous: \cf1\strike Error Codes\strike0\{linkID=660\}\cf0 , Up: \cf1\strike Error Reporting\strike0\{linkID=690\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Error Messages\b0 
\par 
\par The library has functions and variables designed to make it easy for your program to report informative error messages in the customary format about the failure of a library call. The functions\f1  strerror\f0  and \f1 perror\f0  give you the standard error message for a given error code; the variable\f1  program_invocation_short_name\f0  gives you convenient access to the name of the program that encountered the error.
\par 
\par -- Function: char * \b strerror\b0  (\i int errnum\i0 )
\par 
\par \pard\li500 The \f1 strerror\f0  function maps the error code (see \cf1\strike Checking for Errors\strike0\{linkID=310\}\cf0 ) specified by the \i errnum\i0  argument to a descriptive error message string. The return value is a pointer to this string. 
\par 
\par The value \i errnum\i0  normally comes from the variable \f1 errno\f0 . 
\par 
\par You should not modify the string returned by \f1 strerror\f0 . Also, if you make subsequent calls to \f1 strerror\f0 , the string might be overwritten. (But it's guaranteed that no library function ever calls\f1  strerror\f0  behind your back.) 
\par 
\par The function \f1 strerror\f0  is declared in \f1 string.h\f0 .
\par 
\par 
\par \pard -- Function: void \b perror\b0  (\i const char *message\i0 )
\par 
\par \pard\li500 This function prints an error message to the stream \f1 stderr\f0 ; see \cf1\strike Standard Streams\strike0\{linkID=2100\}\cf0 . The orientation of \f1 stderr\f0  is not changed. 
\par 
\par If you call \f1 perror\f0  with a \i message\i0  that is either a null pointer or an empty string, \f1 perror\f0  just prints the error message corresponding to \f1 errno\f0 , adding a trailing newline. 
\par 
\par If you supply a non-null \i message\i0  argument, then \f1 perror\f0  prefixes its output with this string. It adds a colon and a space character to separate the \i message\i0  from the error string corresponding to \f1 errno\f0 . 
\par 
\par The function \f1 perror\f0  is declared in \f1 stdio.h\f0 .
\par 
\par \pard\f1 strerror\f0  and \f1 perror\f0  produce the exact same message for any given error code; the precise text varies from system to system. \f2 In this\f0  system, the messages are fairly short; there are no multi-line messages or embedded newlines. Each error message begins with a capital letter and does not include any terminating punctuation. 
\par 
\par 
\par Here is an example showing how to handle failure to open a file correctly. The function \f1 open_sesame\f0  tries to open the named file for reading and returns a stream if successful. The \f1 fopen\f0  library function returns a null pointer if it couldn't open the file for some reason. In that situation, \f1 open_sesame\f0  constructs an appropriate error message using the \f1 strerror\f0  function, and terminates the program. If we were going to make some other library calls before passing the error code to \f1 strerror\f0 , we'd have to save it in a local variable instead, because those other library functions might overwrite \f1 errno\f0  in the meantime.
\par 
\par \f1      #include <errno.h>
\par      #include <stdio.h>
\par      #include <stdlib.h>
\par      #include <string.h>
\par      
\par      FILE *
\par      open_sesame (char *name)
\par      \{
\par        FILE *stream;
\par      
\par        errno = 0;
\par        stream = fopen (name, "r");
\par        if (stream == NULL)
\par          \{
\par            fprintf (stderr, "Couldn't open file %s; %s\\n",
\par                     name, strerror (errno));
\par            exit (EXIT_FAILURE);
\par          \}
\par        else
\par          return stream;
\par      \}
\par \f0 
\par 
\par }
680
PAGE_172
Error Recovery
clearerr;clearerr_unlocked



Imported



FALSE
26
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Error Recovery \{keepn\}
\par \pard\fs18 Next: \cf1\strike Binary Streams\strike0\{linkID=190\}\cf0 , Previous: \cf1\strike EOF and Errors\strike0\{linkID=650\}\cf0 , Up: \cf1\strike I/O on Streams\strike0\{linkID=1260\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Recovering from errors\b0 
\par 
\par You may explicitly clear the error and EOF flags with the \f1 clearerr\f0  function.
\par 
\par -- Function: void \b clearerr\b0  (\i FILE *stream\i0 )
\par 
\par \pard\li500 This function clears the end-of-file and error indicators for the stream \i stream\i0 . 
\par 
\par The file positioning functions (see \cf1\strike File Positioning\strike0\{linkID=790\}\cf0 ) also clear the end-of-file indicator for the stream.
\par 
\par \pard Note that it is \i not\i0  correct to just clear the error flag and retry a failed stream operation. After a failed write, any number of characters since the last buffer flush may have been committed to the file, while some buffered data may have been discarded. Merely retrying can thus cause lost or repeated data. 
\par 
\par A failed read may leave the file pointer in an inappropriate position for a second try. In both cases, you should seek to a known position before retrying. 
\par 
\par Most errors that can happen are not recoverable -- a second try will always fail again in the same way. So usually it is best to give up and report the error to the user, rather than install complicated recovery logic. 
\par 
\par 
\par }
690
PAGE_173
Error Reporting
errorreporting;reportingerrors;errorcodes;statuscodes;errno.h



Imported



FALSE
19
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Error Reporting \{keepn\}
\par \pard\fs18 Next: \cf1\strike Memory\strike0\{linkID=1440\}\cf0 , Previous: \cf1\strike Introduction\strike0\{linkID=1210\}\cf0 , Up: \cf1\strike Top\strike0\{linkID=10\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Error Reporting\b0 
\par 
\par Many functions in the C library detect and report error conditions, and sometimes your programs need to check for these error conditions. For example, when you open an input file, you should verify that the file was actually opened correctly, and print an error message or take other appropriate action if the call to the library function failed. 
\par 
\par This chapter describes how the error reporting facility works. Your program should include the header file \f1 errno.h\f0  to use this facility.
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Checking for Errors\strike0\{linkID=310\}\cf0 : How errors are reported by library functions.
\par \f2\'b7\f0  \cf1\strike Error Codes\strike0\{linkID=660\}\cf0 : Error code macros; all of these expand into integer constant values.
\par \f2\'b7\f0  \cf1\strike Error Messages\strike0\{linkID=670\}\cf0 : Mapping error codes onto error messages.
\par 
\par }
700
PAGE_180
Executing a File
executingafile;exec;unistd.h;execv;execl;execve;execle;execvp;execlp



Imported



FALSE
62
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fswiss\fcharset0 Arial;}{\f2\fmodern Courier New;}{\f3\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Executing a File \{keepn\}
\par \pard\f1\fs18 Next: \cf1\strike Consistency Checking\cf2\strike0\{linkID=410\}\cf0 , \f0 Previous: \cf1\strike Running a Command\cf2\strike0\{linkID=1910\}\cf0 , Up: \cf1\strike Top\strike0\{linkID=10\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Executing a File\b0 
\par 
\par This section describes the \f2 exec\f0  family of functions, for executing a file as a process image. You can use these functions to make a child process execute a new program after it has been forked. 
\par 
\par To see the effects of \f2 exec\f0  from the point of view of the called program, See \cf1\strike Program Basics\strike0\{linkID=1750\}\cf0 . 
\par 
\par The functions in this family differ in how you specify the arguments, but otherwise they all do the same thing. They are declared in the header file \f3 process\f2 .h
\par \f0 .
\par -- Function: int \b execv\b0  (\i const char *filename, char *const argv\i0\f2 []\f0 )
\par 
\par \pard\li500 The \f2 execv\f0  function executes the file named by \i filename\i0  as a new process image. 
\par 
\par The \i argv\i0  argument is an array of null-terminated strings that is used to provide a value for the \f2 argv\f0  argument to the \f2 main\f0  function of the program to be executed. The last element of this array must be a null pointer. By convention, the first element of this array is the file name of the program sans directory names. See \cf1\strike Program Arguments\strike0\{linkID=1740\}\cf0 , for full details on how programs can access these arguments. 
\par 
\par The environment for the new process image is taken from the\f2  environ\f0  variable of the current process image; see \cf1\strike Environment Variables\strike0\{linkID=640\}\cf0 , for information about environments.
\par 
\par \pard -- Function: int \b execl\b0  (\i const char *filename, const char *arg0, ...\i0 )
\par 
\par \pard\li500 This is similar to \f2 execv\f0 , but the \i argv\i0  strings are specified individually instead of as an array. A null pointer must be passed as the last such argument.
\par 
\par \pard -- Function: int \b execve\b0  (\i const char *filename, char *const argv\i0\f2 []\i\f0 , char *const env\i0\f2 []\f0 )
\par 
\par \pard\li500 This is similar to \f2 execv\f0 , but permits you to specify the environment for the new program explicitly as the \i env\i0  argument. This should be an array of strings in the same format as for the \f2 environ\f0  variable; see \cf1\strike Environment Access\strike0\{linkID=630\}\cf0 .
\par 
\par \pard -- Function: int \b execle\b0  (\i const char *filename, const char *arg0, char *const env\i0\f2 []\i\f0 , ...\i0 )
\par 
\par \pard\li500 This is similar to \f2 execl\f0 , but permits you to specify the environment for the new program explicitly. The environment argument is passed following the null pointer that marks the last \i argv\i0  argument, and should be an array of strings in the same format as for the \f2 environ\f0  variable.
\par 
\par \pard -- Function: int \b execvp\b0  (\i const char *filename, char *const argv\i0\f2 []\f0 )
\par 
\par \pard\li500 The \f2 execvp\f0  function is similar to \f2 execv\f0 , except that it searches the directories listed in the \f2 PATH\f0  environment variable (see \cf1\strike Standard Environment\strike0\{link=PAGE_570\}\cf0 ) to find the full file name of a file from \i filename\i0  if \i filename\i0  does not contain a slash. 
\par 
\par This function is useful for executing system utility programs, because it looks for them in the places that the user has chosen. Shells use it to run the commands that users type.
\par 
\par \pard -- Function: int \b execlp\b0  (\i const char *filename, const char *arg0, ...\i0 )
\par 
\par \pard\li500 This function is like \f2 execl\f0 , except that it performs the same file name searching as the \f2 execvp\f0  function.
\par 
\par \pard These functions normally don't return, since execution of a new program causes the currently executing program to go away completely. A value of \f2 -1\f0  is returned in the event of a failure. In addition to the usual file name errors (see \cf1\strike File Name Errors\strike0\{linkID=740\}\cf0 ), the following\f2  errno\f0  error conditions are defined for these functions:
\par 
\par \f2 E2BIG\f0 
\par \pard\li500 The combined size of the new program's argument list and environment list is larger than \f2 ARG_MAX\f0  bytes. The GNU system has no specific limit on the argument list size, so this error code cannot result, but you may get \f2 ENOMEM\f0  instead if the arguments are too big for available memory. 
\par \pard\f2 ENOEXEC\f0 
\par \pard\li500 The specified file can't be executed because it isn't in the right format. 
\par \pard\f2 ENOMEM\f0 
\par \pard\li500 Executing the specified file requires more storage than is available.
\par 
\par \pard If execution of the new file succeeds, it updates the access time field of the file as if the file had been read. See \cf1\strike File Times\strike0\{link=PAGE_204\}\cf0 , for more details about access times of files. 
\par 
\par The point at which the file is closed again is not specified, but is at some point before the process exits or before another process image is executed. 
\par 
\par Executing a new process image completely changes the contents of memory, copying only the argument and environment strings to new locations. \f1  In this system file descriptors and streams are closed when the current process ends.\f0 
\par 
\par }
710
PAGE_181
Exit Status
exitstatus;stdlib.h;EXIT_SUCCESS;EXIT_FAILURE



Imported



FALSE
39
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Exit Status \{keepn\}
\par \pard\fs18 Next: \cf1\strike Cleanups on Exit\strike0\{linkID=350\}\cf0 , Previous: \cf1\strike Normal Termination\strike0\{linkID=1560\}\cf0 , Up: \cf1\strike Program Termination\strike0\{linkID=1770\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Exit Status\b0 
\par 
\par When a program exits, it can return to the parent process a small amount of information about the cause of termination, using the\i  exit status\i0 . This is a value between 0 and 255 that the exiting process passes as an argument to \f1 exit\f0 . 
\par 
\par Normally you should use the exit status to report very broad information about success or failure. You can't provide a lot of detail about the reasons for the failure, and most parent processes would not want much detail anyway. 
\par 
\par There are conventions for what sorts of status values certain programs should return. The most common convention is simply 0 for success and 1 for failure. Programs that perform comparison use a different convention: they use status 1 to indicate a mismatch, and status 2 to indicate an inability to compare. Your program should follow an existing convention if an existing convention makes sense for it. 
\par 
\par A general convention reserves status values 128 and up for special purposes. In particular, the value 128 is used to indicate failure to execute another program in a subprocess. This convention is not universally obeyed, but it is a good idea to follow it in your programs. 
\par 
\par \b Warning:\b0  Don't try to use the number of errors as the exit status. This is actually not very useful; a parent process would generally not care how many errors occurred. Worse than that, it does not work, because the status value is truncated to eight bits. Thus, if the program tried to report 256 errors, the parent would receive a report of 0 errors--that is, success. 
\par 
\par For the same reason, it does not work to use the value of \f1 errno\f0  as the exit status--these can exceed 255. 
\par 
\par \b Portability note:\b0  Some non-POSIX systems use different conventions for exit status values. For greater portability, you can use the macros \f1 EXIT_SUCCESS\f0  and \f1 EXIT_FAILURE\f0  for the conventional status value for success and failure, respectively. They are declared in the file \f1 stdlib.h\f0 .
\par 
\par -- Macro: int \b EXIT_SUCCESS\b0 
\par 
\par \pard\li500 This macro can be used with the \f1 exit\f0  function to indicate successful program completion. 
\par 
\par On POSIX systems, the value of this macro is \f1 0\f0 . On other systems, the value might be some other (possibly non-constant) integer expression.
\par 
\par \pard -- Macro: int \b EXIT_FAILURE\b0 
\par 
\par \pard\li500 This macro can be used with the \f1 exit\f0  function to indicate unsuccessful program completion in a general sense. 
\par 
\par On POSIX systems, the value of this macro is \f1 1\f0 . On other systems, the value might be some other (possibly non-constant) integer expression. Other nonzero status values also indicate failures. Certain programs use different nonzero status values to indicate particular kinds of "non-success". For example, \f1 diff\f0  uses status value\f1  1\f0  to mean that the files are different, and \f1 2\f0  or more to mean that there was difficulty in opening the files.
\par 
\par \pard Don't confuse a program's exit status with a process' termination status. There are lots of ways a process can terminate besides having it's program finish. In the event that the process termination \i is\i0  caused by program termination (i.e. \f1 exit\f0 ), though, the program's exit status becomes part of the process' termination status.
\par 
\par }
720
PAGE_183
Exponents and Logarithms
exponentiationfunctions;powerfunctions;logarithmfunctions;exp;expf;expl;exp;exp2f;exp2l;exp;exp10f;exp10l;pow;pow10f;pow10l;log;logf;logl;log;log10f;log10l;log;log2f;log2l;logb;logbf;logbl;ilogb;ilogbf;ilogbl;FP_ILOGB;FP_ILOGBNAN;pow;powf;powl;squarerootfunction;sqrt;sqrtf;sqrtl;cuberootfunction;cbrt;cbrtf;cbrtl;hypot;hypotf;hypotl;expm;expm1f;expm1l;log1p;log1pf;log1pl;complexexponentiationfunctions;complexlogarithmfunctions;cexp;cexpf;cexpl;clog;clogf;clogl;clog;clog10f;clog10l;csqrt;csqrtf;csqrtl;cpow;cpowf;cpowl



Imported



FALSE
225
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Exponents and Logarithms \{keepn\}
\par \pard\fs18 Next: \cf1\strike Hyperbolic Functions\strike0\{linkID=1130\}\cf0 , Previous: \cf1\strike Inverse Trig Functions\strike0\{linkID=1220\}\cf0 , Up: \cf1\strike Mathematics\strike0\{linkID=1410\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Exponentiation and Logarithms\b0 
\par 
\par -- Function: double \b exp\b0  (\i double x\i0 )
\par 
\par \pard\li500 -- Function: float \b expf\b0  (\i float x\i0 )
\par 
\par \pard\li1000 -- Function: long double \b expl\b0  (\i long double x\i0 )
\par 
\par \pard\li1500 These functions compute \f1 e\f0  (the base of natural logarithms) raised to the power \i x\i0 . 
\par 
\par If the magnitude of the result is too large to be representable,\f1  exp\f0  signals overflow.
\par 
\par \pard -- Function: double \b exp2\b0  (\i double x\i0 )
\par 
\par \pard\li500 -- Function: float \b exp2f\b0  (\i float x\i0 )
\par 
\par \pard\li1000 -- Function: long double \b exp2l\b0  (\i long double x\i0 )
\par 
\par \pard\li1500 These functions compute \f1 2\f0  raised to the power \i x\i0 . Mathematically, \f1 exp2 (x)\f0  is the same as \f1 exp (x * log (2))\f0 .
\par 
\par \pard -- Function: double \b exp10\b0  (\i double x\i0 )
\par 
\par \pard\li500 -- Function: float \b exp10f\b0  (\i float x\i0 )
\par 
\par \pard\li1000 -- Function: long double \b exp10l\b0  (\i long double x\i0 )
\par 
\par \pard\li1500 -- Function: double \b pow10\b0  (\i double x\i0 )
\par 
\par \pard\li2000 -- Function: float \b pow10f\b0  (\i float x\i0 )
\par 
\par \pard\li2500 -- Function: long double \b pow10l\b0  (\i long double x\i0 )
\par 
\par \pard\li3000 These functions compute \f1 10\f0  raised to the power \i x\i0 . Mathematically, \f1 exp10 (x)\f0  is the same as \f1 exp (x * log (10))\f0 . 
\par 
\par These functions are GNU extensions. The name \f1 exp10\f0  is preferred, since it is analogous to \f1 exp\f0  and \f1 exp2\f0 .
\par 
\par \pard -- Function: double \b log\b0  (\i double x\i0 )
\par 
\par \pard\li500 -- Function: float \b logf\b0  (\i float x\i0 )
\par 
\par \pard\li1000 -- Function: long double \b logl\b0  (\i long double x\i0 )
\par 
\par \pard\li1500 These functions compute the natural logarithm of \i x\i0 . \f1 exp (log (\i\f0 x\i0\f1 ))\f0  equals \i x\i0 , exactly in mathematics and approximately in C. 
\par 
\par If \i x\i0  is negative, \f1 log\f0  signals a domain error. If \i x\i0  is zero, it returns negative infinity; if \i x\i0  is too close to zero, it may signal overflow.
\par 
\par \pard -- Function: double \b log10\b0  (\i double x\i0 )
\par 
\par \pard\li500 -- Function: float \b log10f\b0  (\i float x\i0 )
\par 
\par \pard\li1000 -- Function: long double \b log10l\b0  (\i long double x\i0 )
\par 
\par \pard\li1500 These functions return the base-10 logarithm of \i x\i0 .\f1  log10 (\i\f0 x\i0\f1 )\f0  equals \f1 log (\i\f0 x\i0\f1 ) / log (10)\f0 .
\par 
\par \pard -- Function: double \b log2\b0  (\i double x\i0 )
\par 
\par \pard\li500 -- Function: float \b log2f\b0  (\i float x\i0 )
\par 
\par \pard\li1000 -- Function: long double \b log2l\b0  (\i long double x\i0 )
\par 
\par \pard\li1500 These functions return the base-2 logarithm of \i x\i0 .\f1  log2 (\i\f0 x\i0\f1 )\f0  equals \f1 log (\i\f0 x\i0\f1 ) / log (2)\f0 .
\par 
\par \pard -- Function: double \b logb\b0  (\i double x\i0 )
\par 
\par \pard\li500 -- Function: float \b logbf\b0  (\i float x\i0 )
\par 
\par \pard\li1000 -- Function: long double \b logbl\b0  (\i long double x\i0 )
\par 
\par \pard\li1500 These functions extract the exponent of \i x\i0  and return it as a floating-point value. If \f1 FLT_RADIX\f0  is two, \f1 logb\f0  is equal to \f1 floor (log2 (x))\f0 , except it's probably faster. 
\par 
\par If \i x\i0  is de-normalized, \f1 logb\f0  returns the exponent \i x\i0  would have if it were normalized. If \i x\i0  is infinity (positive or negative), \f1 logb\f0  returns &infin;. If \i x\i0  is zero,\f1  logb\f0  returns &infin;. It does not signal.
\par 
\par \pard -- Function: int \b ilogb\b0  (\i double x\i0 )
\par 
\par \pard\li500 -- Function: int \b ilogbf\b0  (\i float x\i0 )
\par 
\par \pard\li1000 -- Function: int \b ilogbl\b0  (\i long double x\i0 )
\par 
\par \pard\li1500 These functions are equivalent to the corresponding \f1 logb\f0  functions except that they return signed integer values.
\par 
\par \pard Since integers cannot represent infinity and NaN, \f1 ilogb\f0  instead returns an integer that can't be the exponent of a normal floating-point number. \f1 math.h\f0  defines constants so you can check for this.
\par -- Macro: int \b FP_ILOGB0\b0 
\par 
\par \pard\li500\f1 ilogb\f0  returns this value if its argument is \f1 0\f0 . The numeric value is either \f1 INT_MIN\f0  or \f1 -INT_MAX\f0 . 
\par 
\par This macro is defined in ISO C99.
\par 
\par \pard -- Macro: int \b FP_ILOGBNAN\b0 
\par 
\par \pard\li500\f1 ilogb\f0  returns this value if its argument is \f1 NaN\f0 . The numeric value is either \f1 INT_MIN\f0  or \f1 INT_MAX\f0 . 
\par 
\par This macro is defined in ISO C99.
\par 
\par \pard These values are system specific. They might even be the same. The proper way to test the result of \f1 ilogb\f0  is as follows:
\par 
\par \f1      i = ilogb (f);
\par      if (i == FP_ILOGB0 || i == FP_ILOGBNAN)
\par        \{
\par          if (isnan (f))
\par            \{
\par              /* Handle NaN.  */
\par            \}
\par          else if (f  == 0.0)
\par            \{
\par              /* Handle 0.0.  */
\par            \}
\par          else
\par            \{
\par              /* Some other value with large exponent,
\par                 perhaps +Inf.  */
\par            \}
\par        \}
\par \f0 
\par -- Function: double \b pow\b0  (\i double base, double power\i0 )
\par 
\par \pard\li500 -- Function: float \b powf\b0  (\i float base, float power\i0 )
\par 
\par \pard\li1000 -- Function: long double \b powl\b0  (\i long double base, long double power\i0 )
\par 
\par \pard\li1500 These are general exponentiation functions, returning \i base\i0  raised to \i power\i0 . 
\par 
\par Mathematically, \f1 pow\f0  would return a complex number when \i base\i0  is negative and \i power\i0  is not an integral value. \f1 pow\f0  can't do that, so instead it signals a domain error. \f1 pow\f0  may also underflow or overflow the destination type.
\par 
\par \pard -- Function: double \b sqrt\b0  (\i double x\i0 )
\par 
\par \pard\li500 -- Function: float \b sqrtf\b0  (\i float x\i0 )
\par 
\par \pard\li1000 -- Function: long double \b sqrtl\b0  (\i long double x\i0 )
\par 
\par \pard\li1500 These functions return the nonnegative square root of \i x\i0 . 
\par 
\par If \i x\i0  is negative, \f1 sqrt\f0  signals a domain error. Mathematically, it should return a complex number.
\par 
\par \pard -- Function: double \b cbrt\b0  (\i double x\i0 )
\par 
\par \pard\li500 -- Function: float \b cbrtf\b0  (\i float x\i0 )
\par 
\par \pard\li1000 -- Function: long double \b cbrtl\b0  (\i long double x\i0 )
\par 
\par \pard\li1500 These functions return the cube root of \i x\i0 . They cannot fail; every representable real value has a representable real cube root.
\par 
\par \pard -- Function: double \b hypot\b0  (\i double x, double y\i0 )
\par 
\par \pard\li500 -- Function: float \b hypotf\b0  (\i float x, float y\i0 )
\par 
\par \pard\li1000 -- Function: long double \b hypotl\b0  (\i long double x, long double y\i0 )
\par 
\par \pard\li1500 These functions return \f1 sqrt (\i\f0 x\i0\f1 *\i\f0 x\i0\f1  +\i\f0  y\i0\f1 *\i\f0 y\i0\f1 )\f0 . This is the length of the hypotenuse of a right triangle with sides of length \i x\i0  and \i y\i0 , or the distance of the point (\i x\i0 , \i y\i0 ) from the origin. Using this function instead of the direct formula is wise, since the error is much smaller. See also the function \f1 cabs\f0  in \cf1\strike Absolute Value\strike0\{linkID=30\}\cf0 .
\par 
\par \pard -- Function: double \b expm1\b0  (\i double x\i0 )
\par 
\par \pard\li500 -- Function: float \b expm1f\b0  (\i float x\i0 )
\par 
\par \pard\li1000 -- Function: long double \b expm1l\b0  (\i long double x\i0 )
\par 
\par \pard\li1500 These functions return a value equivalent to \f1 exp (\i\f0 x\i0\f1 ) - 1\f0 . They are computed in a way that is accurate even if \i x\i0  is near zero--a case where \f1 exp (\i\f0 x\i0\f1 ) - 1\f0  would be inaccurate owing to subtraction of two numbers that are nearly equal.
\par 
\par \pard -- Function: double \b log1p\b0  (\i double x\i0 )
\par 
\par \pard\li500 -- Function: float \b log1pf\b0  (\i float x\i0 )
\par 
\par \pard\li1000 -- Function: long double \b log1pl\b0  (\i long double x\i0 )
\par 
\par \pard\li1500 These functions returns a value equivalent to \f1 log (1 + \i\f0 x\i0\f1 )\f0 . They are computed in a way that is accurate even if \i x\i0  is near zero.
\par 
\par \pard ISO C99 defines complex variants of some of the exponentiation and logarithm functions.
\par -- Function: complex double \b cexp\b0  (\i complex double z\i0 )
\par 
\par \pard\li500 -- Function: complex float \b cexpf\b0  (\i complex float z\i0 )
\par 
\par \pard\li1000 -- Function: complex long double \b cexpl\b0  (\i complex long double z\i0 )
\par 
\par \pard\li1500 These functions return \f1 e\f0  (the base of natural logarithms) raised to the power of \i z\i0 . Mathematically, this corresponds to the value 
\par 
\par exp (z) = exp (creal (z)) * (cos (cimag (z)) + I * sin (cimag (z)))
\par 
\par \pard -- Function: complex double \b clog\b0  (\i complex double z\i0 )
\par 
\par \pard\li500 -- Function: complex float \b clogf\b0  (\i complex float z\i0 )
\par 
\par \pard\li1000 -- Function: complex long double \b clogl\b0  (\i complex long double z\i0 )
\par 
\par \pard\li1500 These functions return the natural logarithm of \i z\i0 . Mathematically, this corresponds to the value 
\par 
\par log (z) = log (cabs (z)) + I * carg (z) 
\par 
\par \f1 clog\f0  has a pole at 0, and will signal overflow if \i z\i0  equals or is very close to 0. It is well-defined for all other values of\i  z\i0 .
\par 
\par \pard -- Function: complex double \b clog10\b0  (\i complex double z\i0 )
\par 
\par \pard\li500 -- Function: complex float \b clog10f\b0  (\i complex float z\i0 )
\par 
\par \pard\li1000 -- Function: complex long double \b clog10l\b0  (\i complex long double z\i0 )
\par 
\par \pard\li1500 These functions return the base 10 logarithm of the complex value\i  z\i0 . Mathematically, this corresponds to the value 
\par 
\par log (z) = log10 (cabs (z)) + I * carg (z) 
\par 
\par These functions are GNU extensions.
\par 
\par \pard -- Function: complex double \b csqrt\b0  (\i complex double z\i0 )
\par 
\par \pard\li500 -- Function: complex float \b csqrtf\b0  (\i complex float z\i0 )
\par 
\par \pard\li1000 -- Function: complex long double \b csqrtl\b0  (\i complex long double z\i0 )
\par 
\par \pard\li1500 These functions return the complex square root of the argument \i z\i0 . Unlike the real-valued functions, they are defined for all values of \i z\i0 .
\par 
\par \pard -- Function: complex double \b cpow\b0  (\i complex double base, complex double power\i0 )
\par 
\par \pard\li500 -- Function: complex float \b cpowf\b0  (\i complex float base, complex float power\i0 )
\par 
\par \pard\li1000 -- Function: complex long double \b cpowl\b0  (\i complex long double base, complex long double power\i0 )
\par 
\par \pard\li1500 These functions return \i base\i0  raised to the power of\i  power\i0 . This is equivalent to \f1 cexp (y * clog (x))\f0 
\par 
\par }
730
PAGE_184
Extended Char Intro
internalrepresentation;widecharacter;Unicode;ISO;UCS-;UCS-;UTF-;UTF-;wchar_t;wint_t;wchar.h;WCHAR_MIN;WCHAR_MAX;WEOF;wchar.h;multibytecharacter;EBCDIC;ISO;EUC;Shift_JIS;SJIS;ISO;UTF-;UTF-



Imported



FALSE
103
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fswiss\fcharset0 Arial;}{\f3\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Extended Char Intro \{keepn\}
\par \pard\fs18 Next: \cf1\strike Charset Function Overview\strike0\{linkID=300\}\cf0 , Up: \cf1\strike Character Set Handling\strike0\{linkID=290\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Introduction to Extended Characters\b0 
\par 
\par A variety of solutions is available to overcome the differences between character sets with a 1:1 relation between bytes and characters and character sets with ratios of 2:1 or 4:1. The remainder of this section gives a few examples to help understand the design decisions made while developing the functionality of the C library. 
\par 
\par A distinction we have to make right away is between internal and external representation. \i Internal representation\i0  means the representation used by a program while keeping the text in memory. External representations are used when text is stored or transmitted through some communication channel. Examples of external representations include files waiting in a directory to be read and parsed. 
\par 
\par Traditionally there has been no difference between the two representations. It was equally comfortable and useful to use the same single-byte representation internally and externally. This comfort level decreases with more and larger character sets. 
\par 
\par One of the problems to overcome with the internal representation is handling text that is externally encoded using different character sets. Assume a program that reads two texts and compares them using some metric. The comparison can be usefully done only if the texts are internally kept in a common format. 
\par 
\par For such a common format (= character set) eight bits are certainly no longer enough. So the smallest entity will have to grow: \i wide characters\i0  will now be used. Instead of one byte per character, two or four will be used instead. (Three are not good to address in memory and more than four bytes seem not to be necessary). 
\par 
\par As shown in some other part of this manual, a completely new family has been created of functions that can handle wide character texts in memory. The most commonly used character sets for such internal wide character representations are Unicode and ISO 10646 (also known as UCS for Universal Character Set). Unicode was originally planned as a 16-bit character set; whereas, ISO 10646 was designed to be a 31-bit large code space. The two standards are practically identical. They have the same character repertoire and code table, but Unicode specifies added semantics. At the moment, only characters in the first \f1 0x10000\f0  code positions (the so-called Basic Multilingual Plane, BMP) have been assigned, but the assignment of more specialized characters outside this 16-bit space is already in progress. A number of encodings have been defined for Unicode and ISO 10646 characters: UCS-2 is a 16-bit word that can only represent characters from the BMP, UCS-4 is a 32-bit word than can represent any Unicode and ISO 10646 character, UTF-8 is an ASCII compatible encoding where ASCII characters are represented by ASCII bytes and non-ASCII characters by sequences of 2-6 non-ASCII bytes, and finally UTF-16 is an extension of UCS-2 in which pairs of certain UCS-2 words can be used to encode non-BMP characters up to \f1 0x10ffff\f0 . 
\par 
\par To represent wide characters the \f1 char\f0  type is not suitable. For this reason the ISO C standard introduces a new type that is designed to keep one character of a wide character string. To maintain the similarity there is also a type corresponding to \f1 int\f0  for those functions that take a single wide character.
\par 
\par -- Data type: \b wchar_t\b0 
\par 
\par \pard\li500 This data type is used as the base type for wide character strings. In other words, arrays of objects of this type are the equivalent of\f1  char[]\f0  for multibyte character strings. The type is defined in\f1  stddef.h\f0 . 
\par 
\par The ISO C90 standard, where \f1 wchar_t\f0  was introduced, does not say anything specific about the representation. It only requires that this type is capable of storing all elements of the basic character set. Therefore it would be legitimate to define \f1 wchar_t\f0  as \f1 char\f0 , which might make sense for embedded systems. 
\par 
\par \pard\f2 In this system \f1 wchar_t\f0  is always \f2 16\f0  bits wide and\f2  UTF-16 encoding with surrogate characters is used.\f0 
\par 
\par -- Data type: \b wint_t\b0 
\par 
\par \pard\li500\f1 wint_t\f0  is a data type used for parameters and variables that contain a single wide character. As the name suggests this type is the equivalent of \f1 int\f0  when using the normal \f1 char\f0  strings. The types \f1 wchar_t\f0  and \f1 wint_t\f0  often have the same representation if their size is 32 bits wide but i\f2 n this system\f0  \f1 wchar_t\f0  is defined as \f3 short,\f0  \f2  and t\f0 he type \f1 wint_t\f0  \f2 is\f0  be defined as\f1  int\f0  due to the parameter promotion. 
\par 
\par This type is defined in \f1 wchar.h\f0  and was introduced in Amendment 1 to ISO C90.
\par 
\par \pard As there are for the \f1 char\f0  data type macros are available for specifying the minimum and maximum value representable in an object of type \f1 wchar_t\f0 .
\par 
\par -- Macro: wint_t \b WCHAR_MIN\b0 
\par 
\par \pard\li500 The macro \f1 WCHAR_MIN\f0  evaluates to the minimum value representable by an object of type \f1 wint_t\f0 . 
\par 
\par This macro was introduced in Amendment 1 to ISO C90.
\par 
\par \pard -- Macro: wint_t \b WCHAR_MAX\b0 
\par 
\par \pard\li500 The macro \f1 WCHAR_MAX\f0  evaluates to the maximum value representable by an object of type \f1 wint_t\f0 . 
\par 
\par This macro was introduced in Amendment 1 to ISO C90.
\par 
\par \pard Another special wide character value is the equivalent to \f1 EOF\f0 .
\par 
\par -- Macro: wint_t \b WEOF\b0 
\par 
\par \pard\li500 The macro \f1 WEOF\f0  evaluates to a constant expression of type\f1  wint_t\f0  whose value is different from any member of the extended character set. 
\par 
\par \f1 WEOF\f0  need not be the same value as \f1 EOF\f0  and unlike\f1  EOF\f0  it also need \i not\i0  be negative. In other words, sloppy code like 
\par 
\par \f1           \{
\par             int c;
\par             ...
\par             while ((c = getc (fp)) < 0)
\par               ...
\par           \}
\par      
\par \f0 has to be rewritten to use \f1 WEOF\f0  explicitly when wide characters are used: 
\par 
\par \f1           \{
\par             wint_t c;
\par             ...
\par             while ((c = wgetc (fp)) != WEOF)
\par               ...
\par           \}
\par      
\par \f0 This macro was introduced in Amendment 1 to ISO C90 and is defined in \f1 wchar.h\f0 .
\par 
\par \pard These internal representations present problems when it comes to storing and transmittal. Because each single wide character consists of more than one byte, they are effected by byte-ordering. Thus, machines with different endianesses would see different values when accessing the same data. This byte ordering concern also applies for communication protocols that are all byte-based and, thereforet require that the sender has to decide about splitting the wide character in bytes. A last (but not least important) point is that wide characters often require more storage space than a customized byte-oriented character set. 
\par 
\par For all the above reasons, an external encoding that is different from the internal encoding is often used if the latter is UCS-2 or UCS-4. The external encoding is byte-based and can be chosen appropriately for the environment and for the texts to be handled. A variety of different character sets can be used for this external encoding (information that will not be exhaustively presented here-instead, a description of the major groups will suffice). All of the ASCII-based character sets fulfill one requirement: they are "filesystem safe." This means that the character \f1 '/'\f0  is used in the encoding \i only\i0  to represent itself. Things are a bit different for character sets like EBCDIC (Extended Binary Coded Decimal Interchange Code, a character set family used by IBM), but if the operation system does not understand EBCDIC directly the parameters-to-system calls have to be converted first anyhow.
\par 
\par \pard\li500\f3\'b7\f0  The simplest character sets are single-byte character sets. There can be only up to 256 characters (for 8 bit character sets), which is not sufficient to cover all languages but might be sufficient to handle a specific text. Handling of a 8 bit character sets is simple. This is not true for other kinds presented later, and therefore, the application one uses might require the use of 8 bit character sets. 
\par 
\par \f3\'b7\f0  The ISO 2022 standard defines a mechanism for extended character sets where one character \i can\i0  be represented by more than one byte. This is achieved by associating a state with the text. Characters that can be used to change the state can be embedded in the text. Each byte in the text might have a different interpretation in each state. The state might even influence whether a given byte stands for a character on its own or whether it has to be combined with some more bytes. 
\par 
\par In most uses of ISO 2022 the defined character sets do not allow state changes that cover more than the next character. This has the big advantage that whenever one can identify the beginning of the byte sequence of a character one can interpret a text correctly. Examples of character sets using this policy are the various EUC character sets (used by Sun's operations systems, EUC-JP, EUC-KR, EUC-TW, and EUC-CN) or Shift_JIS (SJIS, a Japanese encoding). 
\par 
\par But there are also character sets using a state that is valid for more than one character and has to be changed by another byte sequence. Examples for this are ISO-2022-JP, ISO-2022-KR, and ISO-2022-CN. 
\par \f3\'b7\f0  Early attempts to fix 8 bit character sets for other languages using the Roman alphabet lead to character sets like ISO 6937. Here bytes representing characters like the acute accent do not produce output themselves: one has to combine them with other characters to get the desired result. For example, the byte sequence \f1 0xc2 0x61\f0  (non-spacing acute accent, followed by lower-case `a') to get the "small a with acute" character. To get the acute accent character on its own, one has to write \f1 0xc2 0x20\f0  (the non-spacing acute followed by a space). 
\par 
\par Character sets like ISO 6937 are used in some embedded systems such as teletex. 
\par \f3\'b7\f0  Instead of converting the Unicode or ISO 10646 text used internally, it is often also sufficient to simply use an encoding different than UCS-2/UCS-4. The Unicode and ISO 10646 standards even specify such an encoding: UTF-8. This encoding is able to represent all of ISO 10646 31 bits in a byte string of length one to six. 
\par 
\par There were a few other attempts to encode ISO 10646 such as UTF-7, but UTF-8 is today the only encoding that should be used. In fact, with any luck UTF-8 will soon be the only external encoding that has to be supported. It proves to be universally usable and its only disadvantage is that it favors Roman languages by making the byte string representation of other scripts (Cyrillic, Greek, Asian scripts) longer than necessary if using a specific character set for these scripts. Methods like the Unicode compression scheme can alleviate these problems.
\par 
\par \pard The question remaining is: how to select the character set or encoding to use. The answer: you cannot decide about it yourself, it is decided by the developers of the system or the majority of the users. Since the goal is interoperability one has to use whatever the other people one works with use. If there are no constraints, the selection is based on the requirements the expected circle of users will have. In other words, if a project is expected to be used in only, say, Russia it is fine to use KOI8-R or a similar character set. But if at the same time people from, say, Greece are participating one should use a character set that allows all people to collaborate. 
\par 
\par The most widely useful solution seems to be: go with the most general character set, namely ISO 10646. Use UTF-8 as the external encoding and problems about users not being able to use their own language adequately are a thing of the past. 
\par 
\par One final comment about the choice of the wide character representation is necessary at this point. We have said above that the natural choice is using Unicode or ISO 10646. This is not required, but at least encouraged, by the ISO C standard. The standard defines at least a macro \f1 __STDC_ISO_10646__\f0  that is only defined on systems where the \f1 wchar_t\f0  type encodes ISO 10646 characters. If this symbol is not defined one should avoid making assumptions about the wide character representation. If the programmer uses only the functions provided by the C library to handle wide character strings there should be no compatibility problems with other systems.
\par 
\par }
740
PAGE_193
File Name Errors
filenameerrors;usualfilenameerrors



Imported



FALSE
25
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 File Name Errors \{keepn\}
\par \pard\fs18 Previous: \cf1\strike File Name Resolution\strike0\{linkID=750\}\cf0 , Up: \cf1\strike File Names\strike0\{linkID=760\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b File Name Errors\b0 
\par 
\par Functions that accept file name arguments usually detect these\f1  errno\f0  error conditions relating to the file name syntax or trouble finding the named file. These errors are referred to throughout this manual as the \i usual file name errors\i0 .
\par 
\par \f1 EACCES\f0 
\par \pard\li500 The process does not have search permission for a directory component of the file name. 
\par \pard\f1 ENAMETOOLONG\f0 
\par \pard\li500 This error is used when either the total length of a file name is greater than \f1 PATH_MAX\f0 , or when an individual file name component has a length greater than \f1 NAME_MAX\f0 .
\par 
\par In the GNU system, there is no imposed limit on overall file name length, but some file systems may place limits on the length of a component. 
\par \pard\f1 ENOENT\f0 
\par \pard\li480 This error is reported when a file referenced as a directory component in the file name doesn't exist\f2 .\f0 
\par \pard 
\par \f1 ENOTDIR\f0 
\par \pard\li500 A file that is referenced as a directory component in the file name exists, but it isn't a directory. 
\par \f1 
\par }
750
PAGE_195
File Name Resolution
filenameresolution;rootdirectory;absolutefilename;relativefilename;parentdirectory



Imported



FALSE
50
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern\fcharset0 Courier New;}{\f2\fswiss\fcharset0 Arial;}{\f3\fmodern Courier New;}{\f4\fnil\fcharset0 Arial;}{\f5\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 File Name Resolution \{keepn\}
\par \pard\fs18 Next: \cf1\strike File Name Errors\strike0\{linkID=740\}\cf0 , Previous: \cf1\strike Directories\strike0\{linkID=570\}\cf0 , Up: \cf1\strike File Names\strike0\{linkID=760\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b File Name Resolution\b0 
\par 
\par A file name consists of file name components separated by slash (`\f1\\\f0 ') characters.
\par 
\par The process of determining what file a file name refers to is called\i  file name resolution\i0 . This is performed by examining the components that make up a file name in left-to-right order, and locating each successive component in the directory named by the previous component. Of course, each of the files that are referenced as directories must actually exist, be directories instead of regular files, and have the appropriate permissions to be accessible by the process; otherwise the file name resolution fails. 
\par 
\par If a file name begins with a \f2 letter followed by a colon, the first component is located on the specified drive.  
\par 
\par If the file name begins with a slash '\\' the first component is located in the root of the current drive.  \f0 Such a file name is called an \i absolute file name\i0 . 
\par 
\par Otherwise, the first component in the file name is located in the current working directory (see \cf1\strike Working Directory\strike0\{linkID=2470\}\cf0 ). This kind of file name is called a \i relative file name\i0 . 
\par 
\par The file name components \f3 .\f0  ("dot") and \f3 ..\f0  ("dot-dot") have special meanings. Every directory has entries for these file name components. The file name component \f3 .\f0  refers to the directory itself, while the file name component \f3 ..\f0  refers to its\i  parent directory\i0  (the directory that contains the link for the directory in question). As a special case, \f3 ..\f0  in the root directory refers to the root directory itself, since it has no parent; thus \f3 /..\f0  is the same as \f3 /\f0 . 
\par 
\par Here are some examples of file names:
\par 
\par \f1\\\f3 a\f0 
\par \pard\li500 The file named \f3 a\f0 , in the root directory\f2  of the current drive\f0 . 
\par \pard\f1 c:\\a
\par \pard\fi480\f4 The file named \f1 a\f4 , int the root directory of the drive C\f3 
\par \pard\f1\\\f3 a\f1\\\f3 b\f0 
\par \pard\li500 The file named \f3 b\f0 , in the directory named \f3 a\f0  in the root directory\f2  of the current drive\f0 . 
\par \pard\f1 c:\\\f3 a\f1\\\f3 b\f0 
\par \pard\li500 The file named \f3 b\f0 , in the directory named \f3 a\f0  in the root directory\f2  of drive C\f0 . 
\par \pard\f3 a\f0 
\par \pard\li500 The file named \f3 a\f0 , in the current working directory. 
\par \pard\f1\\\f3 a\f1\\\f3 .\f1\\\f3 b\f0 
\par \pard\li500 This is the same as \f3 /a/b\f0 . 
\par \pard\f3 .\f1\\\f3 a\f0 
\par \pard\li500 The file named \f3 a\f0 , in the current working directory. 
\par \pard\f3 ..\f1\\\f3 a\f0 
\par \pard\li500 The file named \f3 a\f0 , in the parent directory of the current working directory.
\par 
\par \pard A file name that names a directory may optionally end in a `\f1\\\f0 '. You can specify a file name of \f1\\\f0  to refer to the root directory, but the empty string is not a meaningful file name. If you want to refer to the current working directory, use a file name of \f3 .\f0  or\f3  .\f1\\\f0 . 
\par 
\par \f2 File names may also include support for file types, specified by extensions to file names.  File types occur at the end of a file name, and are preceded by a dot.  For example:
\par 
\par \f5 C:\\a.c\f2 
\par 
\par Is the C language source file \f5 a.c\f4 , located in the root directory of drive C.\f0 
\par 
\par }
760
PAGE_196
File Names
filename



Imported



FALSE
18
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 File Names \{keepn\}
\par \pard\fs18 Previous: \cf1\strike I/O Concepts\strike0\{linkID=1250\}\cf0 , Up: \cf1\strike I/O Overview\strike0\{linkID=1270\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b File Names\b0 
\par 
\par In order to open a connection to a file, or to perform other operations such as deleting a file, you need some way to refer to the file. Nearly all files have names that are strings--even files which are actually devices such as tape drives or terminals. These strings are called\i  file names\i0 . You specify the file name to say which file you want to open or operate on. 
\par 
\par This section describes the conventions for file names and how the operating system works with them.
\par 
\par \pard\li500\f1\'b7\f0  \cf1\strike Directories\strike0\{linkID=570\}\cf0 : Directories contain entries for files.
\par \f1\'b7\f0  \cf1\strike File Name Resolution\strike0\{linkID=750\}\cf0 : A file name specifies how to look up a file.
\par \f1\'b7\f0  \cf1\strike File Name Errors\strike0\{linkID=740\}\cf0 : Error conditions relating to file names.
\par }
770
PAGE_198
File Position Primitive
filepositioningonafiledescriptor;positioningafiledescriptor;seekingonafiledescriptor;lseek;sparsefiles;holesinfiles;lseek;off_t;off64_t



Imported



FALSE
68
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 File Position Primitive \{keepn\}
\par \pard\fs18 Next: \cf1\strike Descriptors and Streams\strike0\{linkID=560\}\cf0 , Previous: \cf1\strike I/O Primitives\strike0\{linkID=1280\}\cf0 , Up: \cf1\strike Low-Level I/O\strike0\{linkID=1360\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Setting the File Position of a Descriptor\b0 
\par 
\par Just as you can set the file position of a stream with \f1 fseek\f0 , you can set the file position of a descriptor with \f1 lseek\f0 . This specifies the position in the file for the next \f1 read\f0  or\f1  write\f0  operation. See \cf1\strike File Positioning\strike0\{linkID=790\}\cf0 , for more information on the file position and what it means. 
\par 
\par To read the current file position value from a descriptor, use\f1  lseek (\i\f0 desc\i0\f1 , 0, SEEK_CUR)\f0 .
\par 
\par -- Function: off_t \b lseek\b0  (\i int filedes, off_t offset, int whence\i0 )
\par 
\par \pard\li500 The \f1 lseek\f0  function is used to change the file position of the file with descriptor \i filedes\i0 . 
\par 
\par The \i whence\i0  argument specifies how the \i offset\i0  should be interpreted, in the same way as for the \f1 fseek\f0  function, and it must be one of the symbolic constants \f1 SEEK_SET\f0 , \f1 SEEK_CUR\f0 , or\f1  SEEK_END\f0 .
\par 
\par \f1 SEEK_SET\f0 
\par \pard\li1000 Specifies that \i whence\i0  is a count of characters from the beginning of the file. 
\par \pard\li500\f1 SEEK_CUR\f0 
\par \pard\li1000 Specifies that \i whence\i0  is a count of characters from the current file position. This count may be positive or negative. 
\par \pard\li500\f1 SEEK_END\f0 
\par \pard\li1000 Specifies that \i whence\i0  is a count of characters from the end of the file. A negative count specifies a position within the current extent of the file; a positive count specifies a position past the current end. If you set the position past the current end, and actually write data, you will extend the file with zeros up to that position.
\par 
\par \pard\li500 The return value from \f1 lseek\f0  is normally the resulting file position, measured in bytes from the beginning of the file. You can use this feature together with \f1 SEEK_CUR\f0  to read the current file position. 
\par 
\par If you want to append to the file, setting the file position to the current end of file with \f1 SEEK_END\f0  is not sufficient. Another process may write more data after you seek but before you write, extending the file so the position you write onto clobbers their data. Instead, use the \f1 O_APPEND\f0  operating mode; see \cf1\strike Operating Modes\strike0\{linkID=1630\}\cf0 . 
\par 
\par You can set the file position past the current end of the file. This does not by itself make the file longer; \f1 lseek\f0  never changes the file. But subsequent output at that position will extend the file. Characters between the previous end of file and the new position are filled with zeros. Extending the file in this way can create a "hole": the blocks of zeros are not actually allocated on disk, so the file takes up less space than it appears to; it is then called a "sparse file". If the file position cannot be changed, or the operation is in some way invalid, \f1 lseek\f0  returns a value of -1. The following\f1  errno\f0  error conditions are defined for this function:
\par 
\par \f1 EBADF\f0 
\par \pard\li1000 The \i filedes\i0  is not a valid file descriptor. 
\par \pard\li500\f1 EINVAL\f0 
\par \pard\li1000 The \i whence\i0  argument value is not valid, or the resulting file offset is not valid. A file offset is invalid. 
\par \pard\li500\f1 ESPIPE\f0 
\par \pard\li1000 The \i filedes\i0  corresponds to an object that cannot be positioned, such as a pipe, FIFO or terminal device. (POSIX.1 specifies this error only for pipes and FIFOs, but in the GNU system, you always get\f1  ESPIPE\f0  if the object is not seekable.)
\par \pard\li500 
\par \pard You can have multiple descriptors for the same file if you open the file more than once, or if you duplicate a descriptor with \f1 dup\f0 . Descriptors that come from separate calls to \f1 open\f0  have independent file positions; using \f1 lseek\f0  on one descriptor has no effect on the other. For example,
\par 
\par \f1      \{
\par        int d1, d2;
\par        char buf[4];
\par        d1 = open ("foo", O_RDONLY);
\par        d2 = open ("foo", O_RDONLY);
\par        lseek (d1, 1024, SEEK_SET);
\par        read (d2, buf, 4);
\par      \}
\par \f0 
\par will read the first four characters of the file \f1 foo\f0 . (The error-checking code necessary for a real program has been omitted here for brevity.) 
\par 
\par By contrast, descriptors made by duplication share a common file position with the original descriptor that was duplicated. Anything which alters the file position of one of the duplicates, including reading or writing data, affects all of them alike. Thus, for example,
\par 
\par \f1      \{
\par        int d1, d2, d3;
\par        char buf1[4], buf2[4];
\par        d1 = open ("foo", O_RDONLY);
\par        d2 = dup (d1);
\par        d3 = dup (d2);
\par        lseek (d3, 1024, SEEK_SET);
\par        read (d1, buf1, 4);
\par        read (d2, buf2, 4);
\par      \}
\par \f0 
\par will read four characters starting with the 1024'th character of\f1  foo\f0 , and then four more characters starting with the 1028'th character.
\par }
780
PAGE_199
File Position
fileposition;sequential-accessfiles;random-accessfiles;append-accessfiles



Imported



FALSE
19
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fswiss\fcharset0 Arial;}{\f2\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 File Position \{keepn\}
\par \pard\fs18 Previous: \cf1\strike Streams and File Descriptors\strike0\{linkID=2130\}\cf0 , Up: \cf1\strike I/O Concepts\strike0\{linkID=1250\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b File Position\b0 
\par 
\par One of the attributes of an open file is its \i file position\i0  that keeps track of where in the file the next character is to be read or written. In th\f1 is \f0 system, and all POSIX.1 systems, the file position is simply an integer representing the number of bytes from the beginning of the file. 
\par 
\par The file position is normally set to the beginning of the file when it is opened, and each time a character is read or written, the file position is incremented. In other words, access to the file is normally\i  sequential\i0 . Ordinary files permit read or write operations at any position within the file. Some other kinds of files may also permit this. Files which do permit this are sometimes referred to as \i random-access\i0  files. You can change the file position using the \f2 fseek\f0  function on a stream (see \cf1\strike File Positioning\strike0\{linkID=790\}\cf0 ) or the \f2 lseek\f0  function on a file descriptor (see \cf1\strike I/O Primitives\strike0\{linkID=1280\}\cf0 ). If you try to change the file position on a file that doesn't support random access, you get the\f2  ESPIPE\f0  error. Streams and descriptors that are opened for \i append access\i0  are treated specially for output: output to such files is \i always\i0  appended sequentially to the \i end\i0  of the file, regardless of the file position. However, the file position is still used to control where in the file reading is done. If you think about it, you'll realize that several programs can read a given file at the same time. In order for each program to be able to read the file at its own pace, each program must have its own file pointer, which is not affected by anything the other programs do. 
\par 
\par In fact, each opening of a file creates a separate file position. Thus, if you open a file twice even in the same program, you get two streams or descriptors with independent file positions. 
\par 
\par By contrast, if you open a descriptor and then duplicate it to get another descriptor, these two descriptors share the same file position: changing the file position of one descriptor will affect the other.
\par 
\par }
790
PAGE_200
File Positioning
filepositioningonastream;positioningastream;seekingonastream;stdio.h;ftell;ftello;ftello;fseek;fseeko;fseeko;SEEK_SET;SEEK_CUR;SEEK_END;rewind;L_SET;L_INCR;L_XTND



Imported



FALSE
52
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fswiss\fcharset0 Arial;}{\f2\fmodern Courier New;}{\f3\fnil\fcharset0 Arial;}{\f4\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 File Positioning \{keepn\}
\par \pard\fs18 Next: \cf1\strike Portable Positioning\strike0\{linkID=1730\}\cf0 , Previous: \cf1\strike Binary Streams\strike0\{linkID=190\}\cf0 , Up: \cf1\strike I/O on Streams\strike0\{linkID=1260\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b File Positioning\b0 
\par 
\par The \i file position\i0  of a stream describes where in the file the stream is currently reading or writing. I/O on the stream advances the file position through the file. In th\f1 is \f0 system, the file position is represented as an integer, which counts the number of bytes from the beginning of the file. See \cf1\strike File Position\strike0\{linkID=780\}\cf0 . 
\par 
\par During I/O to an ordinary disk file, you can change the file position whenever you wish, so as to read or write any portion of the file. Some other kinds of files may also permit this. Files which support changing the file position are sometimes referred to as \i random-access\i0  files. 
\par 
\par You can use the functions in this section to examine or modify the file position indicator associated with a stream. The symbols listed below are declared in the header file \f2 stdio.h\f0 .
\par 
\par -- Function: long int \b ftell\b0  (\i FILE *stream\i0 )
\par 
\par \pard\li500 This function returns the current file position of the stream\i  stream\i0 . 
\par 
\par This function can fail if the stream doesn't support file positioning, or if the file position can't be represented in a \f2 long int\f0 , and possibly for other reasons as well. If a failure occurs, a value of\f2  -1\f0  is returned.
\par 
\par \pard -- Function: int \b fseek\b0  (\i FILE *stream, long int offset, int whence\i0 )
\par 
\par \pard\li500 The \f2 fseek\f0  function is used to change the file position of the stream \i stream\i0 . The value of \i whence\i0  must be one of the constants \f2 SEEK_SET\f0 , \f2 SEEK_CUR\f0 , or \f2 SEEK_END\f0 , to indicate whether the \i offset\i0  is relative to the beginning of the file, the current file position, or the end of the file, respectively. 
\par 
\par This function returns a value of zero if the operation was successful, and a nonzero value to indicate failure. A successful call also clears the end-of-file indicator of \i stream\i0  and discards any characters that were "pushed back" by the use of \f2 ungetc\f0 . 
\par 
\par \f2 fseek\f0  either flushes any buffered output before setting the file position or else remembers it so it will be written later in its proper place in the file.
\par 
\par 
\par \pard\b Portability Note:\b0  In \f1 this \f0 system, \f2 ftell\f0 ,\f2  \f3 and\f0  \f2 fseek\f0  work reliably only on binary streams. See \cf1\strike Binary Streams\strike0\{linkID=190\}\cf0 . 
\par 
\par The following symbolic constants are defined for use as the \i whence\i0  argument to \f2 fseek\f0 . They are also used with the \f2 lseek\f0  function (see \cf1\strike I/O Primitives\strike0\{linkID=1280\}\cf0 )\f1 .\f0 
\par 
\par -- Macro: int \b SEEK_SET\b0 
\par 
\par \pard\li500 This is an integer constant which, when used as the \i whence\i0  argument to the \f2 fseek\f0  or \f2 fseeko\f0  function, specifies that the offset provided is relative to the beginning of the file.
\par 
\par \pard -- Macro: int \b SEEK_CUR\b0 
\par 
\par \pard\li500 This is an integer constant which, when used as the \i whence\i0  argument to the \f2 fseek\f0  or \f2 fseeko\f0  function, specifies that the offset provided is relative to the current file position.
\par 
\par \pard -- Macro: int \b SEEK_END\b0 
\par 
\par \pard\li500 This is an integer constant which, when used as the \i whence\i0  argument to the \f2 fseek\f0  or \f2 fseeko\f0  function, specifies that the offset provided is relative to the end of the file.
\par 
\par \pard -- Function: void \b rewind\b0  (\i FILE *stream\i0 )
\par 
\par \pard\li500 The \f2 rewind\f0  function positions the stream \i stream\i0  at the beginning of the file. It is equivalent to calling \f2 fseek\f0  or\f2  fseek\f4  \f0 on the \i stream\i0  with an \i offset\i0  argument of\f2  0L\f0  and a \i whence\i0  argument of \f2 SEEK_SET\f0 , except that the return value is discarded and the error indicator for the stream is reset.
\par 
\par }
800
PAGE_201
File Size
truncate;truncate;ftruncate;ftruncate



Imported



FALSE
109
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 File Size \{keepn\}
\par \pard\fs18 Previous: \cf1\strike Temporary Files\cf2\strike0\{linkID=2250\}\cf0 , Up: \cf1\strike File System Interface\cf2\strike0\{linkID=820\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b File Size\b0 
\par 
\par Normally file sizes are maintained automatically. A file begins with a size of 0 and is automatically extended when data is written past its end. It is also possible to empty a file completely by an\f1  open\f0  or \f1 fopen\f0  call. 
\par 
\par However, sometimes it is necessary to \i reduce\i0  the size of a file. This can be done with the \f1 truncate\f0  and \f1 ftruncate\f0  functions. They were introduced in BSD Unix. \f1 ftruncate\f0  was later added to POSIX.1. 
\par 
\par Some systems allow you to extend a file (creating holes) with these functions. 
\par 
\par Using these functions on anything other than a regular file gives\i  undefined\i0  results. On many systems, such a call will appear to succeed, without actually accomplishing anything.
\par -- Function: int \b truncate\b0  (\i const char *filename, off_t length\i0 )
\par 
\par \pard\li500 The \f1 truncate\f0  function changes the size of \i filename\i0  to\i  length\i0 . If \i length\i0  is shorter than the previous length, data at the end will be lost. The file must be writable by the user to perform this operation. 
\par 
\par If \i length\i0  is longer, holes will be added to the end. However, some systems do not support this feature and will leave the file unchanged. 
\par 
\par When the source file is compiled with \f1 _FILE_OFFSET_BITS == 64\f0  the\f1  truncate\f0  function is in fact \f1 truncate64\f0  and the type\f1  off_t\f0  has 64 bits which makes it possible to handle files up to 2^63 bytes in length. 
\par 
\par The return value is 0 for success, or -1 for an error. In addition to the usual file name errors, the following errors may occur:
\par 
\par \f1 EACCES\f0 
\par \pard\li1000 The file is a directory or not writable. 
\par \pard\li500\f1 EINVAL\f0 
\par \pard\li1000\i length\i0  is negative. 
\par \pard\li500\f1 EFBIG\f0 
\par \pard\li1000 The operation would extend the file beyond the limits of the operating system. 
\par \pard\li500\f1 EIO\f0 
\par \pard\li1000 A hardware I/O error occurred. 
\par \pard\li500\f1 EPERM\f0 
\par \pard\li1000 The file is "append-only" or "immutable". 
\par \pard\li500\f1 EINTR\f0 
\par \pard\li1000 The operation was interrupted by a signal.
\par 
\par \pard -- Function: int \b truncate64\b0  (\i const char *name, off64_t length\i0 )
\par 
\par \pard\li500 This function is similar to the \f1 truncate\f0  function. The difference is that the \i length\i0  argument is 64 bits wide even on 32 bits machines, which allows the handling of files with sizes up to 2^63 bytes. 
\par 
\par When the source file is compiled with \f1 _FILE_OFFSET_BITS == 64\f0  on a 32 bits machine this function is actually available under the name\f1  truncate\f0  and so transparently replaces the 32 bits interface.
\par 
\par \pard -- Function: int \b ftruncate\b0  (\i int fd, off_t length\i0 )
\par 
\par \pard\li500 This is like \f1 truncate\f0 , but it works on a file descriptor \i fd\i0  for an opened file instead of a file name to identify the object. The file must be opened for writing to successfully carry out the operation. 
\par 
\par The POSIX standard leaves it implementation defined what happens if the specified new \i length\i0  of the file is bigger than the original size. The \f1 ftruncate\f0  function might simply leave the file alone and do nothing or it can increase the size to the desired size. In this later case the extended area should be zero-filled. So using \f1 ftruncate\f0  is no reliable way to increase the file size but if it is possible it is probably the fastest way. The function also operates on POSIX shared memory segments if these are implemented by the system. 
\par 
\par \f1 ftruncate\f0  is especially useful in combination with \f1 mmap\f0 . Since the mapped region must have a fixed size one cannot enlarge the file by writing something beyond the last mapped page. Instead one has to enlarge the file itself and then remap the file with the new size. The example below shows how this works. 
\par 
\par When the source file is compiled with \f1 _FILE_OFFSET_BITS == 64\f0  the\f1  ftruncate\f0  function is in fact \f1 ftruncate64\f0  and the type\f1  off_t\f0  has 64 bits which makes it possible to handle files up to 2^63 bytes in length. 
\par 
\par The return value is 0 for success, or -1 for an error. The following errors may occur:
\par 
\par \f1 EBADF\f0 
\par \pard\li1000\i fd\i0  does not correspond to an open file. 
\par \pard\li500\f1 EACCES\f0 
\par \pard\li1000\i fd\i0  is a directory or not open for writing. 
\par \pard\li500\f1 EINVAL\f0 
\par \pard\li1000\i length\i0  is negative. 
\par \pard\li500\f1 EFBIG\f0 
\par \pard\li1000 The operation would extend the file beyond the limits of the operating system. 
\par \pard\li500\f1 EIO\f0 
\par \pard\li1000 A hardware I/O error occurred. 
\par \pard\li500\f1 EPERM\f0 
\par \pard\li1000 The file is "append-only" or "immutable". 
\par \pard\li500\f1 EINTR\f0 
\par \pard\li1000 The operation was interrupted by a signal.
\par 
\par \pard -- Function: int \b ftruncate64\b0  (\i int id, off64_t length\i0 )
\par 
\par \pard\li500 This function is similar to the \f1 ftruncate\f0  function. The difference is that the \i length\i0  argument is 64 bits wide even on 32 bits machines which allows the handling of files with sizes up to 2^63 bytes. 
\par 
\par When the source file is compiled with \f1 _FILE_OFFSET_BITS == 64\f0  on a 32 bits machine this function is actually available under the name\f1  ftruncate\f0  and so transparently replaces the 32 bits interface.
\par 
\par \pard As announced here is a little example of how to use \f1 ftruncate\f0  in combination with \f1 mmap\f0 :
\par 
\par \f1      int fd;
\par      void *start;
\par      size_t len;
\par      
\par      int
\par      add (off_t at, void *block, size_t size)
\par      \{
\par        if (at + size > len)
\par          \{
\par            /* Resize the file and remap.  */
\par            size_t ps = sysconf (_SC_PAGESIZE);
\par            size_t ns = (at + size + ps - 1) & ~(ps - 1);
\par            void *np;
\par            if (ftruncate (fd, ns) < 0)
\par              return -1;
\par            np = mmap (NULL, ns, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
\par            if (np == MAP_FAILED)
\par              return -1;
\par            start = np;
\par            len = ns;
\par          \}
\par        memcpy ((char *) start + at, block, size);
\par        return 0;
\par      \}
\par \f0 
\par The function \f1 add\f0  writes a block of memory at an arbitrary position in the file. If the current size of the file is too small it is extended. Note the it is extended by a round number of pages. This is a requirement of \f1 mmap\f0 . The program has to keep track of the real size, and when it has finished a final \f1 ftruncate\f0  call should set the real size of the file.
\par 
\par }
810
PAGE_202
File Status Flags
filestatusflags;fcntl.h



Imported



FALSE
21
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fswiss\fcharset0 Arial;}{\f2\fmodern Courier New;}{\f3\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 File Status Flags \{keepn\}
\par \pard\fs18 Next: \cf1\strike Access Modes\cf2\strike0\{linkID=40\}\cf0 ,  Previous: \cf1\strike Duplicating Descriptors\strike0\{linkID=600\}\cf0 , Up: \cf1\strike Low-Level I/O\strike0\{linkID=1360\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b File Status Flags\b0 
\par 
\par \i File status flags\i0  are used to specify attributes of the opening of a file. \f1 T\f0 he file status flags are shared by duplicated file descriptors resulting from a single opening of the file. The file status flags are specified with the \i flags\i0  argument to \f2 open\f0 ; see \cf1\strike Opening and Closing Files\strike0\{linkID=1600\}\cf0 . 
\par 
\par File status flags fall into three categories, which are described in the following sections.
\par 
\par \pard\li500\f3\'b7\f0  \cf1\strike Access Modes\strike0\{linkID=40\}\cf0 , specify what type of access is allowed to the file: reading, writing, or both. They are set by \f2 open\f0 , but cannot be changed. 
\par \f3\'b7\f0  \cf1\strike Open-time Flags\strike0\{linkID=1620\}\cf0 , control details of what \f2 open\f0  will do. These flags are not preserved after the \f2 open\f0  call. 
\par \f3\'b7\f0  \cf1\strike Operating Modes\strike0\{linkID=1630\}\cf0 , affect how operations such as \f2 read\f0  and\f2  write\f0  are done. They are set by \f2 open\f0 .
\par 
\par \pard The symbols in this section are defined in the header file\f2  fcntl.h\f0 .
\par \f3 
\par }
820
PAGE_203
File System Interface




Imported



FALSE
21
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern\fcharset0 Courier New;}{\f2\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 File System Interface \{keepn\}
\par \pard\fs18 Next: \cf1\strike Working Directory\cf2\strike0\{linkID=2470\}\cf0 , Previous: \cf1\strike Low-Level I/O\strike0\{linkID=1360\}\cf0 , Up: \cf1\strike Top\strike0\{linkID=10\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b File System Interface\b0 
\par 
\par This chapter describes the C library's functions for manipulating files. Unlike the input and output functions (see \cf1\strike I/O on Streams\strike0\{linkID=1260\}\cf0 ; see \cf1\strike Low-Level I/O\strike0\{linkID=1360\}\cf0 ), these functions are concerned with operating on the files themselves rather than on their contents. 
\par 
\par Among the facilities described in this chapter are functions for examining or modifying directories, functions for renaming and deleting files, and functions for examining and setting file attributes such as access permissions and modification times.
\par 
\par \pard\li500\f1\'b7\f0  \cf1\strike Working Directory\strike0\{linkID=2470\}\cf0 : This is used to resolve relative file names.
\par \f1\'b7\f0  \cf1\strike Deleting Files\strike0\{linkID=540\}\cf0 : How to delete a file, and what that means.
\par \f1\'b7\f0  \cf1\strike Renaming Files\strike0\{linkID=1840\}\cf0 : Changing a file's name.
\par \f1\'b7\f0  \cf1\strike Creating Directories\strike0\{linkID=490\}\cf0 : A system call just for creating a directory.
\par \f1\'b7\f0  \cf1\strike Temporary Files\strike0\{linkID=2250\}\cf0 : Naming and creating temporary files.
\par \f1\'b7\f0  \cf1\strike\f2 File Size\strike0\f0\{linkID=\f2 800\f0\}\cf0 : \f2 Size of files.\f1 
\par }
830
PAGE_206
Finding Tokens in a String
tokenizingstrings;breakingastringintotokens;parsingtokensfromastring;string.h;strtok;wcstok;strtok_r;strsep;basename;basename;dirname



Imported



FALSE
73
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Finding Tokens in a String \{keepn\}
\par \pard\fs18 Previous: \cf1\strike Search Functions\strike0\{linkID=1920\}\cf0 , Up: \cf1\strike String and Array Utilities\strike0\{linkID=2160\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Finding Tokens in a String\b0 
\par 
\par It's fairly common for programs to have a need to do some simple kinds of lexical analysis and parsing, such as splitting a command string up into tokens. You can do this with the \f1 strtok\f0  function, declared in the header file \f1 string.h\f0 .
\par 
\par -- Function: char * \b strtok\b0  (\i char *restrict newstring, const char *restrict delimiters\i0 )
\par 
\par \pard\li500 A string can be split into tokens by making a series of calls to the function \f1 strtok\f0 . 
\par 
\par The string to be split up is passed as the \i newstring\i0  argument on the first call only. The \f1 strtok\f0  function uses this to set up some internal state information. Subsequent calls to get additional tokens from the same string are indicated by passing a null pointer as the \i newstring\i0  argument. Calling \f1 strtok\f0  with another non-null \i newstring\i0  argument reinitializes the state information. It is guaranteed that no other library function ever calls \f1 strtok\f0  behind your back (which would mess up this internal state information). 
\par 
\par The \i delimiters\i0  argument is a string that specifies a set of delimiters that may surround the token being extracted. All the initial characters that are members of this set are discarded. The first character that is\i  not\i0  a member of this set of delimiters marks the beginning of the next token. The end of the token is found by looking for the next character that is a member of the delimiter set. This character in the original string \i newstring\i0  is overwritten by a null character, and the pointer to the beginning of the token in \i newstring\i0  is returned. 
\par 
\par On the next call to \f1 strtok\f0 , the searching begins at the next character beyond the one that marked the end of the previous token. Note that the set of delimiters \i delimiters\i0  do not have to be the same on every call in a series of calls to \f1 strtok\f0 . 
\par 
\par If the end of the string \i newstring\i0  is reached, or if the remainder of string consists only of delimiter characters, \f1 strtok\f0  returns a null pointer. 
\par 
\par Note that "character" is here used in the sense of byte. In a string using a multibyte character encoding (abstract) character consisting of more than one byte are not treated as an entity. Each byte is treated separately. The function is not locale-dependent. 
\par 
\par Note that "character" is here used in the sense of byte. In a string using a multibyte character encoding (abstract) character consisting of more than one byte are not treated as an entity. Each byte is treated separately. The function is not locale-dependent.
\par 
\par \pard -- Function: wchar_t * \b wcstok\b0  (\i wchar_t *newstring, const char *delimiters\i0 )
\par 
\par \pard\li500 A string can be split into tokens by making a series of calls to the function \f1 wcstok\f0 . 
\par 
\par The string to be split up is passed as the \i newstring\i0  argument on the first call only. The \f1 wcstok\f0  function uses this to set up some internal state information. Subsequent calls to get additional tokens from the same wide character string are indicated by passing a null pointer as the \i newstring\i0  argument. Calling \f1 wcstok\f0  with another non-null \i newstring\i0  argument reinitializes the state information. It is guaranteed that no other library function ever calls\f1  wcstok\f0  behind your back (which would mess up this internal state information). 
\par 
\par The \i delimiters\i0  argument is a wide character string that specifies a set of delimiters that may surround the token being extracted. All the initial wide characters that are members of this set are discarded. The first wide character that is \i not\i0  a member of this set of delimiters marks the beginning of the next token. The end of the token is found by looking for the next wide character that is a member of the delimiter set. This wide character in the original wide character string \i newstring\i0  is overwritten by a null wide character, and the pointer to the beginning of the token in \i newstring\i0  is returned. 
\par 
\par On the next call to \f1 wcstok\f0 , the searching begins at the next wide character beyond the one that marked the end of the previous token. Note that the set of delimiters \i delimiters\i0  do not have to be the same on every call in a series of calls to \f1 wcstok\f0 . 
\par 
\par If the end of the wide character string \i newstring\i0  is reached, or if the remainder of string consists only of delimiter wide characters,\f1  wcstok\f0  returns a null pointer. 
\par 
\par Note that "character" is here used in the sense of byte. In a string using a multibyte character encoding (abstract) character consisting of more than one byte are not treated as an entity. Each byte is treated separately. The function is not locale-dependent.
\par 
\par \pard\b Warning:\b0  Since \f1 strtok\f0  and \f1 wcstok\f0  alter the string they is parsing, you should always copy the string to a temporary buffer before parsing it with \f1 strtok\f0 /\f1 wcstok\f0  (see \cf1\strike Copying and Concatenation\strike0\{linkID=460\}\cf0 ). If you allow \f1 strtok\f0  or \f1 wcstok\f0  to modify a string that came from another part of your program, you are asking for trouble; that string might be used for other purposes after\f1  strtok\f0  or \f1 wcstok\f0  has modified it, and it would not have the expected value. 
\par 
\par The string that you are operating on might even be a constant. Then when \f1 strtok\f0  or \f1 wcstok\f0  tries to modify it, your program will get a fatal signal for writing in read-only memory. See \cf1\strike Program Error Signals\strike0\{linkID=1760\}\cf0 . Even if the operation of \f1 strtok\f0  or \f1 wcstok\f0  would not require a modification of the string (e.g., if there is exactly one token) the string can (and in the GNU libc case will) be modified. 
\par 
\par This is a special case of a general principle: if a part of a program does not have as its purpose the modification of a certain data structure, then it is error-prone to modify the data structure temporarily. 
\par 
\par The functions \f1 strtok\f0  and \f1 wcstok\f0  are not reentrant. See \cf1\strike Nonreentrancy\strike0\{linkID=1480\}\cf0 , for a discussion of where and why reentrancy is important. 
\par 
\par Here is a simple example showing the use of \f1 strtok\f0 .
\par 
\par \f1      #include <string.h>
\par      #include <stddef.h>
\par      
\par      ...
\par      
\par      const char string[] = "words separated by spaces -- and, punctuation!";
\par      const char delimiters[] = " .,;:!-";
\par      char *token, *cp;
\par      
\par      ...
\par      
\par      cp = strdup (string);\f2  \f1                 /* Make writable copy.  */
\par      token = strtok (cp, delimiters);      /* token => "words" */
\par      token = strtok (NULL, delimiters);    /* token => "separated" */
\par      token = strtok (NULL, delimiters);    /* token => "by" */
\par      token = strtok (NULL, delimiters);    /* token => "spaces" */
\par      token = strtok (NULL, delimiters);    /* token => "and" */
\par      token = strtok (NULL, delimiters);    /* token => "punctuation" */
\par      token = strtok (NULL, delimiters);    /* token => NULL */
\par \f2      free(cp);                             /* free the copy */\f0 
\par }
840
PAGE_211
Floating Point Classes
floating-pointclasses;classes,floating-point;math.h;fpclassify;FP_NAN;FP_INFINITE;FP_ZERO;FP_SUBNORMAL;FP_NORMAL;isfinite;isnormal;isnan;isinf;isinff;isinfl;isnan;isnanf;isnanl;finite;finitef;finitel



Imported



FALSE
51
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Floating Point Classes \{keepn\}
\par \pard\fs18 Next: \cf1\strike Floating Point Errors\strike0\{linkID=860\}\cf0 , Previous: \cf1\strike Floating Point Numbers\strike0\{linkID=870\}\cf0 , Up: \cf1\strike Arithmetic\strike0\{linkID=100\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Floating-Point Number Classification Functions\b0 
\par 
\par ISO C99 defines macros that let you determine what sort of floating-point number a variable holds.
\par 
\par -- Macro: int \b fpclassify\b0  (\i float-type x\i0 )
\par 
\par \pard\li500 This is a generic macro which works on all floating-point types and which returns a value of type \f1 int\f0 . The possible values are:
\par 
\par \f1 FP_NAN\f0 
\par \pard\li1000 The floating-point number \i x\i0  is "Not a Number" (see \cf1\strike Infinity and NaN\strike0\{linkID=1160\}\cf0 )
\par \pard\li500\f1 FP_INFINITE\f0 
\par \pard\li1000 The value of \i x\i0  is either plus or minus infinity (see \cf1\strike Infinity and NaN\strike0\{linkID=1160\}\cf0 )
\par \pard\li500\f1 FP_ZERO\f0 
\par \pard\li1000 The value of \i x\i0  is zero. In floating-point formats like IEEE 754, where zero can be signed, this value is also returned if\i  x\i0  is negative zero.
\par \pard\li500\f1 FP_SUBNORMAL\f0 
\par \pard\li1000 Numbers whose absolute value is too small to be represented in the normal format are represented in an alternate, \i denormalized\i0  format (see \cf1\strike Floating Point Concepts\strike0\{linkID=850\}\cf0 ). This format is less precise but can represent values closer to zero. \f1 fpclassify\f0  returns this value for values of \i x\i0  in this alternate format.
\par \pard\li500\f1 FP_NORMAL\f0 
\par \pard\li1000 This value is returned for all other values of \i x\i0 . It indicates that there is nothing special about the number.
\par 
\par \pard\f1 fpclassify\f0  is most useful if more than one property of a number must be tested. There are more specific macros which only test one property at a time. Generally these macros execute faster than\f1  fpclassify\f0 , since there is special hardware support for them. You should therefore use the specific macros whenever possible.
\par 
\par -- Macro: int \b isfinite\b0  (\i float-type x\i0 )
\par 
\par \pard\li500 This macro returns a nonzero value if \i x\i0  is finite: not plus or minus infinity, and not NaN. It is equivalent to 
\par 
\par \f1           (fpclassify (x) != FP_NAN && fpclassify (x) != FP_INFINITE)
\par      
\par isfinite\f0  is implemented as a macro which accepts any floating-point type.
\par 
\par \pard -- Macro: int \b isnormal\b0  (\i float-type x\i0 )
\par 
\par \pard\li500 This macro returns a nonzero value if \i x\i0  is finite and normalized. It is equivalent to 
\par 
\par \f1           (fpclassify (x) == FP_NORMAL)
\par      
\par \pard\f0 -- Macro: int \b isnan\b0  (\i float-type x\i0 )
\par 
\par \pard\li500 This macro returns a nonzero value if \i x\i0  is NaN. It is equivalent to 
\par 
\par \f1           (fpclassify (x) == FP_NAN)
\par      
\par \f0 
\par }
850
PAGE_212
Floating Point Concepts
sign(offloatingpointnumber;base(offloatingpointnumber;radix(offloatingpointnumber;exponent(offloatingpointnumber;bias(offloatingpointnumberexponent;mantissa(offloatingpointnumber;significand(offloatingpointnumber;precision(offloatingpointnumber;hiddenbit(offloatingpointnumbermantissa;normalizedfloatingpointnumber



Imported



FALSE
37
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fmodern\fcharset0 Courier New;}{\f3\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Floating Point Concepts \{keepn\}
\par \pard\fs18 Next: \cf1\strike Floating Point Parameters\strike0\{linkID=880\}\cf0 , Up: \cf1\strike Floating Type Macros\strike0\{linkID=890\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Floating Point Representation Concepts\b0 
\par 
\par This section introduces the terminology for describing floating point representations. 
\par 
\par You are probably already familiar with most of these concepts in terms of scientific or exponential notation for floating point numbers. For example, the number \f1 123456.0\f0  could be expressed in exponential notation as \f1 1.23456e+05\f0 , a shorthand notation indicating that the mantissa \f1 1.23456\f0  is multiplied by the base \f1 10\f0  raised to power \f1 5\f0 . 
\par 
\par More formally, the internal representation of a floating point number can be characterized in terms of the following parameters:
\par 
\par \pard\li500\f2\'b7\f0  The \i sign\i0  is either \f1 -1\f0  or \f1 1\f0 . 
\par \f2\'b7\f0  The \i base\i0  or \i radix\i0  for exponentiation, an integer greater than \f1 1\f0 . This is a constant for a particular representation. 
\par \f2\'b7\f0  The \i exponent\i0  to which the base is raised. The upper and lower bounds of the exponent value are constants for a particular representation. 
\par 
\par Sometimes, in the actual bits representing the floating point number, the exponent is \i biased\i0  by adding a constant to it, to make it always be represented as an unsigned quantity. This is only important if you have some reason to pick apart the bit fields making up the floating point number by hand, which is something for which the \f3 C \f0 library provides no support. So this is ignored in the discussion that follows. 
\par \f2\'b7\f0  The \i mantissa\i0  or \i significand\i0  is an unsigned integer which is a part of each floating point number. 
\par \f2\'b7\f0  The \i precision\i0  of the mantissa. If the base of the representation is \i b\i0 , then the precision is the number of base-\i b\i0  digits in the mantissa. This is a constant for a particular representation. 
\par 
\par Many floating point representations have an implicit \i hidden bit\i0  in the mantissa. This is a bit which is present virtually in the mantissa, but not stored in memory because its value is always 1 in a normalized number. The precision figure (see above) includes any hidden bits. 
\par 
\par Again, the\f3  C\f0  library provides no facilities for dealing with such low-level aspects of the representation.
\par 
\par \pard The mantissa of a floating point number represents an implicit fraction whose denominator is the base raised to the power of the precision. Since the largest representable mantissa is one less than this denominator, the value of the fraction is always strictly less than \f1 1\f0 . The mathematical value of a floating point number is then the product of this fraction, the sign, and the base raised to the exponent. 
\par 
\par We say that the floating point number is \i normalized\i0  if the fraction is at least \f1 1/\i\f0 b\i0 , where \i b\i0  is the base. In other words, the mantissa would be too large to fit if it were multiplied by the base. Non-normalized numbers are sometimes called\i  denormal\i0 ; they contain less precision than the representation normally can hold. 
\par 
\par If the number is not normalized, then you can subtract \f1 1\f0  from the exponent while multiplying the mantissa by the base, and get another floating point number with the same value. \i Normalization\i0  consists of doing this repeatedly until the number is normalized. Two distinct normalized floating point numbers cannot be equal in value. 
\par 
\par (There is an exception to this rule: if the mantissa is zero, it is considered normalized. Another exception happens on certain machines where the exponent is as small as the representation can hold. Then it is impossible to subtract \f1 1\f0  from the exponent, so a number may be normalized even if its fraction is less than \f1 1/\i\f0 b\i0 .)
\par 
\par }
860
PAGE_213
Floating Point Errors




Imported



FALSE
16
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24  Floating Point Errors \{keepn\}
\par \pard\fs18 Next: \cf1\strike Rounding\strike0\{linkID=1900\}\cf0 , Previous: \cf1\strike Floating Point Classes\strike0\{linkID=840\}\cf0 , Up: \cf1\strike Arithmetic\strike0\{linkID=100\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Errors in Floating-Point Calculations\b0 
\par 
\par \pard\li500\f1\'b7\f0  \cf1\strike FP Exceptions\strike0\{linkID=1010\}\cf0 : IEEE 754 math exceptions and how to detect them.
\par \f1\'b7\f0  \cf1\strike Infinity and NaN\strike0\{linkID=1160\}\cf0 : Special values returned by calculations.
\par \f1\'b7\f0  \cf1\strike Status bit operations\strike0\{linkID=2110\}\cf0 : Checking for exceptions after the fact.
\par \f1\'b7\f0  \cf1\strike Math Error Reporting\strike0\{linkID=1390\}\cf0 : How the math functions report errors.
\par 
\par }
870
PAGE_214
Floating Point Numbers
floatingpoint;IEEE;IEEEfloatingpoint



Imported



FALSE
17
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Floating Point Numbers \{keepn\}
\par \pard\fs18 Next: \cf1\strike Floating Point Classes\strike0\{linkID=840\}\cf0 , Previous: \cf1\strike Integer Division\strike0\{linkID=1190\}\cf0 , Up: \cf1\strike Arithmetic\strike0\{linkID=100\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Floating Point Numbers\b0 
\par 
\par Most computer hardware has support for two different kinds of numbers: integers (...-3, -2, -1, 0, 1, 2, 3...) and floating-point numbers. Floating-point numbers have three parts: the\i  mantissa\i0 , the \i exponent\i0 , and the \i sign bit\i0 . The real number represented by a floating-point value is given by (s ? -1 : 1)\f1  *\f0  2^e\f1  *\f0  M where s is the sign bit, e the exponent, and M the mantissa. See \cf1\strike Floating Point Concepts\strike0\{linkID=850\}\cf0 , for details. (It is possible to have a different \i base\i0  for the exponent, but all modern hardware uses 2.) 
\par 
\par Floating-point numbers can represent a finite subset of the real numbers. While this subset is large enough for most purposes, it is important to remember that the only reals that can be represented exactly are rational numbers that have a terminating binary expansion shorter than the width of the mantissa. Even simple fractions such as 1/5 can only be approximated by floating point. 
\par 
\par Mathematical operations and functions frequently need to produce values that are not representable. Often these values can be approximated closely enough for practical purposes, but sometimes they can't. Historically there was no way to tell when the results of a calculation were inaccurate. Modern computers implement the IEEE 754 standard for numerical computations, which defines a framework for indicating to the program when the results of calculation are not trustworthy. This framework consists of a set of \i exceptions\i0  that indicate why a result could not be represented, and the special values \i infinity\i0  and \i not a number\i0  (NaN).
\par 
\par }
880
PAGE_215
Floating Point Parameters
float.h;FLT_ROUNDS;FLT_RADIX;FLT_MANT_DIG;DBL_MANT_DIG;LDBL_MANT_DIG;FLT_DIG;DBL_DIG;LDBL_DIG;FLT_MIN_EXP;DBL_MIN_EXP;LDBL_MIN_EXP;FLT_MIN_1fEXP;DBL_MIN_1fEXP;LDBL_MIN_1fEXP;FLT_MAX_EXP;DBL_MAX_EXP;LDBL_MAX_EXP;FLT_MAX_1fEXP;DBL_MAX_1fEXP;LDBL_MAX_1fEXP;FLT_MAX;DBL_MAX;LDBL_MAX;FLT_MIN;DBL_MIN;LDBL_MIN;FLT_EPSILON;DBL_EPSILON;LDBL_EPSILON



Imported



FALSE
101
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Floating Point Parameters \{keepn\}
\par \pard\fs18 Next: \cf1\strike IEEE Floating Point\strike0\{linkID=1140\}\cf0 , Previous: \cf1\strike Floating Point Concepts\strike0\{linkID=850\}\cf0 , Up: \cf1\strike Floating Type Macros\strike0\{linkID=890\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Floating Point Parameters\b0 
\par 
\par These macro definitions can be accessed by including the header file\f1  float.h\f0  in your program. 
\par 
\par Macro names starting with `\f1 FLT_\f0 ' refer to the \f1 float\f0  type, while names beginning with `\f1 DBL_\f0 ' refer to the \f1 double\f0  type and names beginning with `\f1 LDBL_\f0 ' refer to the \f1 long double\f0  type. 
\par 
\par Of these macros, only \f1 FLT_RADIX\f0  is guaranteed to be a constant expression. The other macros listed here cannot be reliably used in places that require constant expressions, such as `\f1 #if\f0 ' preprocessing directives or in the dimensions of static arrays. 
\par 
\par \f1 FLT_ROUNDS\f0 
\par \pard\li500 This value characterizes the rounding mode for floating point addition. The following values indicate standard rounding modes:
\par 
\par \f1 -1\f0 
\par \pard\li1000 The mode is indeterminable.
\par \pard\li500\f1 0\f0 
\par \pard\li1000 Rounding is towards zero.
\par \pard\li500\f1 1\f0 
\par \pard\li1000 Rounding is to the nearest number.
\par \pard\li500\f1 2\f0 
\par \pard\li1000 Rounding is towards positive infinity.
\par \pard\li500\f1 3\f0 
\par \pard\li1000 Rounding is towards negative infinity.
\par 
\par \pard\li500 Any other value represents a machine-dependent nonstandard rounding mode. 
\par 
\par On most machines, the value is \f1 1\f0 , in accordance with the IEEE standard for floating point. 
\par 
\par Here is a table showing how certain values round for each possible value of \f1 FLT_ROUNDS\f0 , if the other aspects of the representation match the IEEE single-precision standard. 
\par 
\par \f1                           0      1             2             3
\par            1.00000003    1.0    1.0           1.00000012    1.0
\par            1.00000007    1.0    1.00000012    1.00000012    1.0
\par           -1.00000003   -1.0   -1.0          -1.0          -1.00000012
\par           -1.00000007   -1.0   -1.00000012   -1.0          -1.00000012
\par      
\par \pard FLT_RADIX\f0 
\par \pard\li500 This is the value of the base, or radix, of the exponent representation. This is guaranteed to be a constant expression, unlike the other macros described in this section. The value is 2\f2 .\f1 
\par \pard FLT_MANT_DIG\f0 
\par \pard\li500 This is the number of base-\f1 FLT_RADIX\f0  digits in the floating point mantissa for the \f1 float\f0  data type. The following expression yields \f1 1.0\f0  (even though mathematically it should not) due to the limited number of mantissa digits: 
\par 
\par \f1           float radix = FLT_RADIX;
\par           
\par           1.0f + 1.0f / radix / radix / ... / radix
\par      
\par \f0 where \f1 radix\f0  appears \f1 FLT_MANT_DIG\f0  times.
\par \pard\f1 DBL_MANT_DIG\f0 
\par \f1 LDBL_MANT_DIG\f0 
\par \pard\li500 This is the number of base-\f1 FLT_RADIX\f0  digits in the floating point mantissa for the data types \f1 double\f0  and \f1 long double\f0 , respectively.
\par \pard\f1 FLT_DIG\f0 
\par \pard\li500 This is the number of decimal digits of precision for the \f1 float\f0  data type. Technically, if \i p\i0  and \i b\i0  are the precision and base (respectively) for the representation, then the decimal precision\i  q\i0  is the maximum number of decimal digits such that any floating point number with \i q\i0  base 10 digits can be rounded to a floating point number with \i p\i0  base \i b\i0  digits and back again, without change to the \i q\i0  decimal digits. 
\par 
\par The value of this macro is supposed to be at least \f1 6\f0 , to satisfy ISO C.
\par \pard\f1 DBL_DIG\f0 
\par \f1 LDBL_DIG\f0 
\par \pard\li500 These are similar to \f1 FLT_DIG\f0 , but for the data types\f1  double\f0  and \f1 long double\f0 , respectively. The values of these macros are supposed to be at least \f1 10\f0 .
\par \pard\f1 FLT_MIN_EXP\f0 
\par \pard\li500 This is the smallest possible exponent value for type \f1 float\f0 . More precisely, is the minimum negative integer such that the value\f1  FLT_RADIX\f0  raised to this power minus 1 can be represented as a normalized floating point number of type \f1 float\f0 .
\par \pard\f1 DBL_MIN_EXP\f0 
\par \f1 LDBL_MIN_EXP\f0 
\par \pard\li500 These are similar to \f1 FLT_MIN_EXP\f0 , but for the data types\f1  double\f0  and \f1 long double\f0 , respectively.
\par \pard\f1 FLT_MIN_10_EXP\f0 
\par \pard\li500 This is the minimum negative integer such that \f1 10\f0  raised to this power minus 1 can be represented as a normalized floating point number of type \f1 float\f0 . This is supposed to be \f1 -37\f0  or even less.
\par \pard\f1 DBL_MIN_10_EXP\f0 
\par \f1 LDBL_MIN_10_EXP\f0 
\par \pard\li500 These are similar to \f1 FLT_MIN_10_EXP\f0 , but for the data types\f1  double\f0  and \f1 long double\f0 , respectively.
\par \pard\f1 FLT_MAX_EXP\f0 
\par \pard\li500 This is the largest possible exponent value for type \f1 float\f0 . More precisely, this is the maximum positive integer such that value\f1  FLT_RADIX\f0  raised to this power minus 1 can be represented as a floating point number of type \f1 float\f0 .
\par \pard\f1 DBL_MAX_EXP\f0 
\par \f1 LDBL_MAX_EXP\f0 
\par \pard\li500 These are similar to \f1 FLT_MAX_EXP\f0 , but for the data types\f1  double\f0  and \f1 long double\f0 , respectively.
\par \pard\f1 FLT_MAX_10_EXP\f0 
\par \pard\li500 This is the maximum positive integer such that \f1 10\f0  raised to this power minus 1 can be represented as a normalized floating point number of type \f1 float\f0 . This is supposed to be at least \f1 37\f0 .
\par \pard\f1 DBL_MAX_10_EXP\f0 
\par \f1 LDBL_MAX_10_EXP\f0 
\par \pard\li500 These are similar to \f1 FLT_MAX_10_EXP\f0 , but for the data types\f1  double\f0  and \f1 long double\f0 , respectively.
\par \pard\f1 FLT_MAX\f0 
\par \pard\li500 The value of this macro is the maximum number representable in type\f1  float\f0 . It is supposed to be at least \f1 1E+37\f0 . The value has type \f1 float\f0 . 
\par 
\par The smallest representable number is \f1 - FLT_MAX\f0 .
\par \pard\f1 DBL_MAX\f0 
\par \f1 LDBL_MAX\f0 
\par \pard\li500 These are similar to \f1 FLT_MAX\f0 , but for the data types\f1  double\f0  and \f1 long double\f0 , respectively. The type of the macro's value is the same as the type it describes.
\par \pard\f1 FLT_MIN\f0 
\par \pard\li500 The value of this macro is the minimum normalized positive floating point number that is representable in type \f1 float\f0 . It is supposed to be no more than \f1 1E-37\f0 .
\par \pard\f1 DBL_MIN\f0 
\par \f1 LDBL_MIN\f0 
\par \pard\li500 These are similar to \f1 FLT_MIN\f0 , but for the data types\f1  double\f0  and \f1 long double\f0 , respectively. The type of the macro's value is the same as the type it describes.
\par \pard\f1 FLT_EPSILON\f0 
\par \pard\li500 This is the maximum positive floating point number of type \f1 float\f0  such that \f1 1.0 + FLT_EPSILON != 1.0\f0  is true. It's supposed to be no greater than \f1 1E-5\f0 .
\par \pard\f1 DBL_EPSILON\f0 
\par \f1 LDBL_EPSILON\f0 
\par \pard\li500 These are similar to \f1 FLT_EPSILON\f0 , but for the data types\f1  double\f0  and \f1 long double\f0 , respectively. The type of the macro's value is the same as the type it describes. The values are not supposed to be greater than \f1 1E-9\f0 .
\par 
\par }
890
PAGE_216
Floating Type Macros
floatingtypemeasurements;measurementsoffloatingtypes;typemeasurements,floating;limits,floatingtypes



Imported



FALSE
21
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Floating Type Macros \{keepn\}
\par \pard\fs18 Next: \cf1\strike Structure Measurement\strike0\{linkID=2210\}\cf0 , Previous: \cf1\strike Range of Type\strike0\{linkID=1790\}\cf0 , Up: \cf1\strike Data Type Measurements\strike0\{linkID=510\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Floating Type Macros\b0 
\par 
\par The specific representation of floating point numbers varies from machine to machine. Because floating point numbers are represented internally as approximate quantities, algorithms for manipulating floating point data often need to take account of the precise details of the machine's floating point representation. 
\par 
\par Some of the functions in the C library itself need this information; for example, the algorithms for printing and reading floating point numbers (see \cf1\strike I/O on Streams\strike0\{linkID=1260\}\cf0 ) and for calculating trigonometric and irrational functions (see \cf1\strike Mathematics\strike0\{linkID=1410\}\cf0 ) use it to avoid round-off error and loss of accuracy. User programs that implement numerical analysis techniques also often need this information in order to minimize or compute error bounds. 
\par 
\par The header file \f1 float.h\f0  describes the format used by your machine.
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Floating Point Concepts\strike0\{linkID=850\}\cf0 : Definitions of terminology.
\par \f2\'b7\f0  \cf1\strike Floating Point Parameters\strike0\{linkID=880\}\cf0 : Details of specific macros.
\par \f2\'b7\f0  \cf1\strike IEEE Floating Point\strike0\{linkID=1140\}\cf0 : The measurements for one common representation.
\par 
\par }
900
PAGE_217
Floating-Point Conversions




Imported



FALSE
64
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 + Floating-Point Conversions \{keepn\}
\par \pard\fs18 Next: \cf1\strike Other Output Conversions\strike0\{linkID=1670\}\cf0 , Previous: \cf1\strike Integer Conversions\strike0\{linkID=1180\}\cf0 , Up: \cf1\strike Formatted Output\strike0\{linkID=970\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Floating-Point Conversions\b0 
\par 
\par This section discusses the conversion specifications for floating-point numbers: the `\f1 %f\f0 ', `\f1 %e\f0 ', `\f1 %E\f0 ', `\f1 %g\f0 ', and `\f1 %G\f0 ' conversions. 
\par 
\par The `\f1 %f\f0 ' conversion prints its argument in fixed-point notation, producing output of the form [\f1 -\f0 ]\i ddd\i0\f1 .\i\f0 ddd\i0 , where the number of digits following the decimal point is controlled by the precision you specify. 
\par 
\par The `\f1 %e\f0 ' conversion prints its argument in exponential notation, producing output of the form [\f1 -\f0 ]\i d\i0\f1 .\i\f0 ddd\i0\f1 e\f0 [\f1 +\f0 |\f1 -\f0 ]\i dd\i0 . Again, the number of digits following the decimal point is controlled by the precision. The exponent always contains at least two digits. The `\f1 %E\f0 ' conversion is similar but the exponent is marked with the letter `\f1 E\f0 ' instead of `\f1 e\f0 '. 
\par 
\par The `\f1 %g\f0 ' and `\f1 %G\f0 ' conversions print the argument in the style of `\f1 %e\f0 ' or `\f1 %E\f0 ' (respectively) if the exponent would be less than -4 or greater than or equal to the precision; otherwise they use the `\f1 %f\f0 ' style. A precision of \f1 0\f0 , is taken as 1. is Trailing zeros are removed from the fractional portion of the result and a decimal-point character appears only if it is followed by a digit. 
\par 
\par The `\f1 %a\f0 ' and `\f1 %A\f0 ' conversions are meant for representing floating-point numbers exactly in textual form so that they can be exchanged as texts between different programs and/or machines. The numbers are represented is the form [\f1 -\f0 ]\f1 0x\i\f0 h\i0\f1 .\i\f0 hhh\i0\f1 p\f0 [\f1 +\f0 |\f1 -\f0 ]\i dd\i0 . At the left of the decimal-point character exactly one digit is print. This character is only \f1 0\f0  if the number is denormalized. Otherwise the value is unspecified; it is implementation dependent how many bits are used. The number of hexadecimal digits on the right side of the decimal-point character is equal to the precision. If the precision is zero it is determined to be large enough to provide an exact representation of the number (or it is large enough to distinguish two adjacent values if the \f1 FLT_RADIX\f0  is not a power of 2, see \cf1\strike Floating Point Parameters\strike0\{linkID=880\}\cf0 ). For the `\f1 %a\f0 ' conversion lower-case characters are used to represent the hexadecimal number and the prefix and exponent sign are printed as \f1 0x\f0  and \f1 p\f0  respectively. Otherwise upper-case characters are used and \f1 0X\f0  and \f1 P\f0  are used for the representation of prefix and exponent string. The exponent to the base of two is printed as a decimal number using at least one digit but at most as many digits as necessary to represent the value exactly. 
\par 
\par If the value to be printed represents infinity or a NaN, the output is [\f1 -\f0 ]\f1 inf\f0  or \f1 nan\f0  respectively if the conversion specifier is `\f1 %a\f0 ', `\f1 %e\f0 ', `\f1 %f\f0 ', or `\f1 %g\f0 ' and it is [\f1 -\f0 ]\f1 INF\f0  or \f1 NAN\f0  respectively if the conversion is `\f1 %A\f0 ', `\f1 %E\f0 ', or `\f1 %G\f0 '. 
\par 
\par The following flags can be used to modify the behavior:
\par 
\par `\f1 -\f0 '
\par \pard\li500 Left-justify the result in the field. Normally the result is right-justified. 
\par \pard `\f1 +\f0 '
\par \pard\li500 Always include a plus or minus sign in the result. 
\par \pard `\f1  \f0 '
\par \pard\li500 If the result doesn't start with a plus or minus sign, prefix it with a space instead. Since the `\f1 +\f0 ' flag ensures that the result includes a sign, this flag is ignored if you supply both of them. 
\par \pard `\f1 #\f0 '
\par \pard\li500 Specifies that the result should always include a decimal point, even if no digits follow it. For the `\f1 %g\f0 ' and `\f1 %G\f0 ' conversions, this also forces trailing zeros after the decimal point to be left in place where they would otherwise be removed. 
\par \pard `\f1 '\f0 '
\par \pard\li500 Separate the digits of the integer part of the result into groups as specified by the locale specified for the \f1 LC_NUMERIC\f0  category; see \cf1\strike General Numeric\strike0\{linkID=1040\}\cf0 . This flag is a GNU extension. 
\par \pard `\f1 0\f0 '
\par \pard\li500 Pad the field with zeros instead of spaces; the zeros are placed after any sign. This flag is ignored if the `\f1 -\f0 ' flag is also specified.
\par 
\par \pard The precision specifies how many digits follow the decimal-point character for the `\f1 %f\f0 ', `\f1 %e\f0 ', and `\f1 %E\f0 ' conversions. For these conversions, the default precision is \f1 6\f0 . If the precision is explicitly \f1 0\f0 , this suppresses the decimal point character entirely. For the `\f1 %g\f0 ' and `\f1 %G\f0 ' conversions, the precision specifies how many significant digits to print. Significant digits are the first digit before the decimal point, and all the digits after it. If the precision is \f1 0\f0  or not specified for `\f1 %g\f0 ' or `\f1 %G\f0 ', it is treated like a value of \f1 1\f0 . If the value being printed cannot be expressed accurately in the specified number of digits, the value is rounded to the nearest number that fits. 
\par 
\par Without a type modifier, the floating-point conversions use an argument of type \f1 double\f0 . (By the default argument promotions, any\f1  float\f0  arguments are automatically converted to \f1 double\f0 .) The following type modifier is supported:
\par 
\par `\f1 L\f0 '
\par \pard\li500 An uppercase `\f1 L\f0 ' specifies that the argument is a \f1 long double\f0 .
\par 
\par \pard Here are some examples showing how numbers print using the various floating-point conversions. All of the numbers were printed using this template string:
\par 
\par \f1      "|%13.4a|%13.4f|%13.4e|%13.4g|\\n"
\par \f0 
\par Here is the output:
\par 
\par \f1      |  0x0.0000p+0|       0.0000|   0.0000e+00|            0|
\par      |  0x1.0000p-1|       0.5000|   5.0000e-01|          0.5|
\par      |  0x1.0000p+0|       1.0000|   1.0000e+00|            1|
\par      | -0x1.0000p+0|      -1.0000|  -1.0000e+00|           -1|
\par      |  0x1.9000p+6|     100.0000|   1.0000e+02|          100|
\par      |  0x1.f400p+9|    1000.0000|   1.0000e+03|         1000|
\par      | 0x1.3880p+13|   10000.0000|   1.0000e+04|        1e+04|
\par      | 0x1.81c8p+13|   12345.0000|   1.2345e+04|    1.234e+04|
\par      | 0x1.86a0p+16|  100000.0000|   1.0000e+05|        1e+05|
\par      | 0x1.e240p+16|  123456.0000|   1.2346e+05|    1.235e+05|
\par \f0 
\par Notice how the `\f1 %g\f0 ' conversion drops trailing zeros.
\par 
\par }
910
PAGE_218
Flushing Buffers
flushingastream;stdio.h;fflush;fflush_unlocked;gt_flushlbf;gt__fpurge



Imported



FALSE
28
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern\fcharset0 Courier New;}{\f2\fmodern Courier New;}{\f3\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Flushing Buffers \{keepn\}
\par \pard\fs18 Next: \cf1\strike Controlling Buffering\strike0\{linkID=430\}\cf0 , Previous: \cf1\strike Buffering Concepts\strike0\{linkID=220\}\cf0 , Up: \cf1\strike Stream Buffering\strike0\{linkID=2120\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Flushing Buffers\b0 
\par 
\par \i Flushing\i0  output on a buffered stream means transmitting all accumulated characters to the file. There are many circumstances when buffered output on a stream is flushed automatically:
\par 
\par \pard\li500\f1\'b7\f0  When you try to do output and the output buffer is full. 
\par \f1\'b7\f0  When the stream is closed. See \cf1\strike Closing Streams\strike0\{linkID=360\}\cf0 . 
\par \f1\'b7\f0  When the program terminates by calling \f2 exit\f0 . See \cf1\strike Normal Termination\strike0\{linkID=1560\}\cf0 . 
\par \f1\'b7\f0  When a newline is written, if the stream is line buffered. 
\par \f1\'b7\f0  Whenever an input operation on \i\f3 a console\i0\f0  stream actually reads data from its file.
\par 
\par \pard If you want to flush the buffered output at another time, call\f2  fflush\f0 , which is declared in the header file \f2 stdio.h\f0 .
\par 
\par -- Function: int \b fflush\b0  (\i FILE *stream\i0 )
\par 
\par \pard\li500 This function causes any buffered output on \i stream\i0  to be delivered to the file. If \i stream\i0  is a null pointer, then\f2  fflush\f0  causes buffered output on \i all\i0  open output streams to be flushed. 
\par 
\par This function returns \f2 EOF\f0  if a write error occurs, or zero otherwise.
\par \pard 
\par 
\par }
920
PAGE_221
Formatted Input Basics
conversionspecifications(scanf);matchingfailure,inscanf



Imported



FALSE
26
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Formatted Input Basics \{keepn\}
\par \pard\fs18 Next: \cf1\strike Input Conversion Syntax\strike0\{linkID=1170\}\cf0 , Up: \cf1\strike Formatted Input\strike0\{linkID=940\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Formatted Input Basics\b0 
\par 
\par Calls to \f1 scanf\f0  are superficially similar to calls to\f1  printf\f0  in that arbitrary arguments are read under the control of a template string. While the syntax of the conversion specifications in the template is very similar to that for \f1 printf\f0 , the interpretation of the template is oriented more towards free-format input and simple pattern matching, rather than fixed-field formatting. For example, most \f1 scanf\f0  conversions skip over any amount of "white space" (including spaces, tabs, and newlines) in the input file, and there is no concept of precision for the numeric input conversions as there is for the corresponding output conversions. Ordinarily, non-whitespace characters in the template are expected to match characters in the input stream exactly, but a matching failure is distinct from an input error on the stream. Another area of difference between \f1 scanf\f0  and \f1 printf\f0  is that you must remember to supply pointers rather than immediate values as the optional arguments to \f1 scanf\f0 ; the values that are read are stored in the objects that the pointers point to. Even experienced programmers tend to forget this occasionally, so if your program is getting strange errors that seem to be related to \f1 scanf\f0 , you might want to double-check this. 
\par 
\par When a \i matching failure\i0  occurs, \f1 scanf\f0  returns immediately, leaving the first non-matching character as the next character to be read from the stream. The normal return value from \f1 scanf\f0  is the number of values that were assigned, so you can use this to determine if a matching error happened before all the expected values were read. The \f1 scanf\f0  function is typically used for things like reading in the contents of tables. For example, here is a function that uses\f1  scanf\f0  to initialize an array of \f1 double\f0 :
\par 
\par \f1      void
\par      readarray (double *array, int n)
\par      \{
\par        int i;
\par        for (i=0; i<n; i++)
\par          if (scanf (" %lf", &(array[i])) != 1)
\par            invalid_input_error ();
\par      \}
\par \f0 
\par The formatted input functions are not used as frequently as the formatted output functions. Partly, this is because it takes some care to use them properly. Another reason is that it is difficult to recover from a matching error. 
\par 
\par }
930
PAGE_222
Formatted Input Functions
stdio.h;scanf;wscanf;fscanf;fwscanf;sscanf;swscanf



Imported



FALSE
45
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 + Formatted Input Functions \{keepn\}
\par \pard\fs18 Next: \cf1\strike Variable Arguments Input\strike0\{linkID=2380\}\cf0 , Previous: \cf1\strike Other Input Conversions\strike0\{linkID=1660\}\cf0 , Up: \cf1\strike Formatted Input\strike0\{linkID=940\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Formatted Input Functions\b0 
\par 
\par Here are the descriptions of the functions for performing formatted input. Prototypes for these functions are in the header file \f1 stdio.h\f0 .\f2 
\par \f0 
\par -- Function: int \b scanf\b0  (\i const char *template, ...\i0 )
\par 
\par \pard\li500 The \f1 scanf\f0  function reads formatted input from the stream\f1  stdin\f0  under the control of the template string \i template\i0 . The optional arguments are pointers to the places which receive the resulting values. 
\par 
\par The return value is normally the number of successful assignments. If an end-of-file condition is detected before any matches are performed, including matches against whitespace and literal characters in the template, then \f1 EOF\f0  is returned.
\par 
\par \pard -- Function: int \b wscanf\b0  (\i const wchar_t *template, ...\i0 )
\par 
\par \pard\li500 The \f1 wscanf\f0  function reads formatted input from the stream\f1  stdin\f0  under the control of the template string \i template\i0 . The optional arguments are pointers to the places which receive the resulting values. 
\par 
\par The return value is normally the number of successful assignments. If an end-of-file condition is detected before any matches are performed, including matches against whitespace and literal characters in the template, then \f1 WEOF\f0  is returned.
\par 
\par \pard -- Function: int \b fscanf\b0  (\i FILE *stream, const char *template, ...\i0 )
\par 
\par \pard\li500 This function is just like \f1 scanf\f0 , except that the input is read from the stream \i stream\i0  instead of \f1 stdin\f0 .
\par 
\par \pard -- Function: int \b fwscanf\b0  (\i FILE *stream, const wchar_t *template, ...\i0 )
\par 
\par \pard\li500 This function is just like \f1 wscanf\f0 , except that the input is read from the stream \i stream\i0  instead of \f1 stdin\f0 .
\par 
\par \pard -- Function: int \b sscanf\b0  (\i const char *s, const char *template, ...\i0 )
\par 
\par \pard\li500 This is like \f1 scanf\f0 , except that the characters are taken from the null-terminated string \i s\i0  instead of from a stream. Reaching the end of the string is treated as an end-of-file condition. 
\par 
\par The behavior of this function is undefined if copying takes place between objects that overlap--for example, if \i s\i0  is also given as an argument to receive a string read under control of the `\f1 %s\f0 ', `\f1 %S\f0 ', or `\f1 %[\f0 ' conversion.
\par 
\par \pard -- Function: int \b swscanf\b0  (\i const wchar_t *ws, const char *template, ...\i0 )
\par 
\par \pard\li500 This is like \f1 wscanf\f0 , except that the characters are taken from the null-terminated string \i ws\i0  instead of from a stream. Reaching the end of the string is treated as an end-of-file condition. 
\par 
\par The behavior of this function is undefined if copying takes place between objects that overlap--for example, if \i ws\i0  is also given as an argument to receive a string read under control of the `\f1 %s\f0 ', `\f1 %S\f0 ', or `\f1 %[\f0 ' conversion.
\par 
\par }
940
PAGE_223
Formatted Input
formattedinputfromastream;readingfromastream,formatted;formatstring,forscanf;template,forscanf



Imported



FALSE
22
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Formatted Input \{keepn\}
\par \pard\fs18 Next: \cf1\strike EOF and Errors\strike0\{linkID=650\}\cf0 , Previous: \cf1\strike Variable Arguments Input\cf2\strike0\{linkID=2380\}\cf0 , Up: \cf1\strike I/O on Streams\strike0\{linkID=1260\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Formatted Input\b0 
\par 
\par The functions described in this section (\f1 scanf\f0  and related functions) provide facilities for formatted input analogous to the formatted output facilities. These functions provide a mechanism for reading arbitrary values under the control of a \i format string\i0  or\i  template string\i0 .
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Formatted Input Basics\strike0\{linkID=920\}\cf0 : Some basics to get you started.
\par \f2\'b7\f0  \cf1\strike Input Conversion Syntax\strike0\{linkID=1170\}\cf0 : Syntax of conversion specifications.
\par \f2\'b7\f0  \cf1\strike Table of Input Conversions\strike0\{linkID=2230\}\cf0 : Summary of input conversions and what they do.
\par \f2\'b7\f0  \cf1\strike Numeric Input Conversions\strike0\{linkID=1590\}\cf0 : Details of conversions for reading numbers.
\par \f2\'b7\f0  \cf1\strike String Input Conversions\strike0\{linkID=2170\}\cf0 : Details of conversions for reading strings.
\par \f2\'b7\f0  \cf1\strike Other Input Conversions\strike0\{linkID=1660\}\cf0 : Details of miscellaneous other conversions.
\par \f2\'b7\f0  \cf1\strike Formatted Input Functions\strike0\{linkID=930\}\cf0 : Descriptions of the actual functions.
\par \f2\'b7\f0  \cf1\strike Variable Arguments Input\strike0\{linkID=2380\}\cf0 : \f1 vscanf\f0  and friends.
\par 
\par }
950
PAGE_225
Formatted Output Basics
conversionspecifications(printf)



Imported



FALSE
35
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Formatted Output Basics \{keepn\}
\par \pard\fs18 Next: \cf1\strike Output Conversion Syntax\strike0\{linkID=1680\}\cf0 , Up: \cf1\strike Formatted Output\strike0\{linkID=970\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Formatted Output Basics\b0 
\par 
\par The \f1 printf\f0  function can be used to print any number of arguments. The template string argument you supply in a call provides information not only about the number of additional arguments, but also about their types and what style should be used for printing them. 
\par 
\par Ordinary characters in the template string are simply written to the output stream as-is, while \i conversion specifications\i0  introduced by a `\f1 %\f0 ' character in the template cause subsequent arguments to be formatted and written to the output stream. For example,
\par 
\par \f1      int pct = 37;
\par      char filename[] = "foo.txt";
\par      printf ("Processing of `%s' is %d%% finished.\\nPlease be patient.\\n",
\par              filename, pct);
\par \f0 
\par produces output like
\par 
\par \f1      Processing of `foo.txt' is 37% finished.
\par      Please be patient.
\par \f0 
\par This example shows the use of the `\f1 %d\f0 ' conversion to specify that an \f1 int\f0  argument should be printed in decimal notation, the `\f1 %s\f0 ' conversion to specify printing of a string argument, and the `\f1 %%\f0 ' conversion to print a literal `\f1 %\f0 ' character. 
\par 
\par There are also conversions for printing an integer argument as an unsigned value in octal, decimal, or hexadecimal radix (`\f1 %o\f0 ', `\f1 %u\f0 ', or `\f1 %x\f0 ', respectively); or as a character value (`\f1 %c\f0 '). 
\par 
\par Floating-point numbers can be printed in normal, fixed-point notation using the `\f1 %f\f0 ' conversion or in exponential notation using the `\f1 %e\f0 ' conversion. The `\f1 %g\f0 ' conversion uses either `\f1 %e\f0 ' or `\f1 %f\f0 ' format, depending on what is more appropriate for the magnitude of the particular number. 
\par 
\par You can control formatting more precisely by writing \i modifiers\i0  between the `\f1 %\f0 ' and the character that indicates which conversion to apply. These slightly alter the ordinary behavior of the conversion. For example, most conversion specifications permit you to specify a minimum field width and a flag indicating whether you want the result left- or right-justified within the field. 
\par 
\par The specific flags and modifiers that are permitted and their interpretation vary depending on the particular conversion. They're all described in more detail in the following sections. Don't worry if this all seems excessively complicated at first; you can almost always get reasonable free-format output without using any of the modifiers at all. The modifiers are mostly used to make the output look "prettier" in tables.
\par 
\par }
960
PAGE_226
Formatted Output Functions
stdio.h;printf;wprintf;fprintf;fwprintf;sprintf;swprintf;snprintf



Imported



FALSE
85
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Formatted Output Functions \{keepn\}
\par \pard\fs18 Next: \cf1\strike Dynamic Output\strike0\{link=PAGE_158\}\cf0 , Previous: \cf1\strike Other Output Conversions\strike0\{linkID=1670\}\cf0 , Up: \cf1\strike Formatted Output\strike0\{linkID=970\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Formatted Output Functions\b0 
\par 
\par This section describes how to call \f1 printf\f0  and related functions. Prototypes for these functions are in the header file \f1 stdio.h\f0 . Because these functions take a variable number of arguments, you\i  must\i0  declare prototypes for them before using them. Of course, the easiest way to make sure you have all the right prototypes is to just include \f1 stdio.h\f0 .
\par 
\par -- Function: int \b printf\b0  (\i const char *template, ...\i0 )
\par 
\par \pard\li500 The \f1 printf\f0  function prints the optional arguments under the control of the template string \i template\i0  to the stream\f1  stdout\f0 . It returns the number of characters printed, or a negative value if there was an output error.
\par 
\par \pard -- Function: int \b wprintf\b0  (\i const wchar_t *template, ...\i0 )
\par 
\par \pard\li500 The \f1 wprintf\f0  function prints the optional arguments under the control of the wide template string \i template\i0  to the stream\f1  stdout\f0 . It returns the number of wide characters printed, or a negative value if there was an output error.
\par 
\par \pard -- Function: int \b fprintf\b0  (\i FILE *stream, const char *template, ...\i0 )
\par 
\par \pard\li500 This function is just like \f1 printf\f0 , except that the output is written to the stream \i stream\i0  instead of \f1 stdout\f0 .
\par 
\par \pard -- Function: int \b fwprintf\b0  (\i FILE *stream, const wchar_t *template, ...\i0 )
\par 
\par \pard\li500 This function is just like \f1 wprintf\f0 , except that the output is written to the stream \i stream\i0  instead of \f1 stdout\f0 .
\par 
\par \pard -- Function: int \b sprintf\b0  (\i char *s, const char *template, ...\i0 )
\par 
\par \pard\li500 This is like \f1 printf\f0 , except that the output is stored in the character array \i s\i0  instead of written to a stream. A null character is written to mark the end of the string. 
\par 
\par The \f1 sprintf\f0  function returns the number of characters stored in the array \i s\i0 , not including the terminating null character. 
\par 
\par The behavior of this function is undefined if copying takes place between objects that overlap--for example, if \i s\i0  is also given as an argument to be printed under control of the `\f1 %s\f0 ' conversion. See \cf1\strike Copying and Concatenation\strike0\{linkID=460\}\cf0 . 
\par 
\par \b Warning:\b0  The \f1 sprintf\f0  function can be \b dangerous\b0  because it can potentially output more characters than can fit in the allocation size of the string \i s\i0 . Remember that the field width given in a conversion specification is only a \i minimum\i0  value. 
\par 
\par To avoid this problem, you can use \f1 snprintf\f0  or \f1 asprintf\f0 , described below.
\par 
\par \pard -- Function: int \b swprintf\b0  (\i wchar_t *s, size_t size, const wchar_t *template, ...\i0 )
\par 
\par \pard\li500 This is like \f1 wprintf\f0 , except that the output is stored in the wide character array \i ws\i0  instead of written to a stream. A null wide character is written to mark the end of the string. The \i size\i0  argument specifies the maximum number of characters to produce. The trailing null character is counted towards this limit, so you should allocate at least \i size\i0  wide characters for the string \i ws\i0 . 
\par 
\par The return value is the number of characters generated for the given input, excluding the trailing null. If not all output fits into the provided buffer a negative value is returned. You should try again with a bigger output string. \i Note:\i0  this is different from how\f1  snprintf\f0  handles this situation. 
\par 
\par Note that the corresponding narrow stream function takes fewer parameters. \f1 swprintf\f0  in fact corresponds to the \f1 snprintf\f0  function. Since the \f1 sprintf\f0  function can be dangerous and should be avoided the ISO C committee refused to make the same mistake again and decided to not define an function exactly corresponding to\f1  sprintf\f0 .
\par 
\par \pard -- Function: int \b snprintf\b0  (\i char *s, size_t size, const char *template, ...\i0 )
\par 
\par \pard\li500 The \f1 snprintf\f0  function is similar to \f1 sprintf\f0 , except that the \i size\i0  argument specifies the maximum number of characters to produce. The trailing null character is counted towards this limit, so you should allocate at least \i size\i0  characters for the string \i s\i0 . 
\par 
\par The return value is the number of characters which would be generated for the given input, excluding the trailing null. If this value is greater or equal to \i size\i0 , not all characters from the result have been stored in \i s\i0 . You should try again with a bigger output string. Here is an example of doing this: 
\par 
\par \f1           /* Construct a message describing the value of a variable
\par              whose name is \i name\i0  and whose value is \i value\i0 . */
\par           char *
\par           make_message (char *name, char *value)
\par           \{
\par             /* Guess we need no more than 100 chars of space. */
\par             int size = 100;
\par             char *buffer = (char *) xmalloc (size);
\par             int nchars;
\par             if (buffer == NULL)
\par               return NULL;
\par           
\par            /* Try to print in the allocated space. */
\par             nchars = snprintf (buffer, size, "value of %s is %s",
\par                                name, value);
\par             if (nchars >= size)
\par               \{
\par                 /* Reallocate buffer now that we know
\par                    how much space is needed. */
\par                 buffer = (char *) xrealloc (buffer, nchars + 1);
\par           
\par                 if (buffer != NULL)
\par                   /* Try again. */
\par                   snprintf (buffer, size, "value of %s is %s",
\par                             name, value);
\par               \}
\par             /* The last call worked, return the string. */
\par             return buffer;
\par           \}
\par      \f0 
\par }
970
PAGE_227
Formatted Output
formatstring,forprintf;template,forprintf;formattedoutputtoastream;writingtoastream,formatted



Imported



FALSE
23
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Formatted Output \{keepn\}
\par \pard\fs18 Next: \cf1\strike Customizing Printf\strike0\{link=PAGE_138\}\cf0 , Previous: \cf1\strike Block Input/Output\strike0\{linkID=200\}\cf0 , Up: \cf1\strike I/O on Streams\strike0\{linkID=1260\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Formatted Output\b0 
\par 
\par The functions described in this section (\f1 printf\f0  and related functions) provide a convenient way to perform formatted output. You call \f1 printf\f0  with a \i format string\i0  or \i template string\i0  that specifies how to format the values of the remaining arguments. 
\par 
\par Unless your program is a filter that specifically performs line- or character-oriented processing, using \f1 printf\f0  or one of the other related functions described in this section is usually the easiest and most concise way to perform output. These functions are especially useful for printing error messages, tables of data, and the like.
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Formatted Output Basics\strike0\{linkID=950\}\cf0 : Some examples to get you started.
\par \f2\'b7\f0  \cf1\strike Output Conversion Syntax\strike0\{linkID=1680\}\cf0 : General syntax of conversion specifications.
\par \f2\'b7\f0  \cf1\strike Table of Output Conversions\strike0\{linkID=2240\}\cf0 : Summary of output conversions and what they do.
\par \f2\'b7\f0  \cf1\strike Integer Conversions\strike0\{linkID=1180\}\cf0 : Details about formatting of integers.
\par \f2\'b7\f0  \cf1\strike Floating-Point Conversions\strike0\{linkID=900\}\cf0 : Details about formatting of floating-point numbers.
\par \f2\'b7\f0  \cf1\strike Other Output Conversions\strike0\{linkID=1670\}\cf0 : Details about formatting of strings, characters, pointers, and the like.
\par \f2\'b7\f0  \cf1\strike Formatted Output Functions\strike0\{linkID=960\}\cf0 : Descriptions of the actual functions.
\par \f2\'b7\f0  \cf1\strike Variable Arguments Output\strike0\{linkID=2390\}\cf0 : \f1 vprintf\f0  and friends.\cf1\strike 
\par }
980
PAGE_228
Formatting Calendar Time
time.h;asctime;asctime_r;ctime;ctime_r;strftime;wcsftime



Imported



FALSE
214
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Formatting Calendar Time \{keepn\}
\par \pard\fs18 Next: \cf1\strike TZ Variable\cf2\strike0\{linkID=2330\}\cf0 , Previous: \cf1\strike Broken-down Time\cf2\strike0\{linkID=210\}\cf0 , Up: \cf1\strike Calendar Time\strike0\{linkID=230\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Formatting Calendar Time\b0 
\par 
\par The functions described in this section format calendar time values as strings. These functions are declared in the header file \f1 time.h\f0 .
\par 
\par -- Function: char * \b asctime\b0  (\i const struct tm *brokentime\i0 )
\par 
\par \pard\li500 The \f1 asctime\f0  function converts the broken-down time value that\i  brokentime\i0  points to into a string in a standard format: 
\par 
\par \f1           "Tue May 21 13:46:22 1991\\n"
\par      
\par \f0 The abbreviations for the days of week are: `\f1 Sun\f0 ', `\f1 Mon\f0 ', `\f1 Tue\f0 ', `\f1 Wed\f0 ', `\f1 Thu\f0 ', `\f1 Fri\f0 ', and `\f1 Sat\f0 '. 
\par 
\par The abbreviations for the months are: `\f1 Jan\f0 ', `\f1 Feb\f0 ', `\f1 Mar\f0 ', `\f1 Apr\f0 ', `\f1 May\f0 ', `\f1 Jun\f0 ', `\f1 Jul\f0 ', `\f1 Aug\f0 ', `\f1 Sep\f0 ', `\f1 Oct\f0 ', `\f1 Nov\f0 ', and `\f1 Dec\f0 '. 
\par 
\par The return value points to a statically allocated string, which might be overwritten by subsequent calls to \f1 asctime\f0  or \f1 ctime\f0 . (But no other library function overwrites the contents of this string.)
\par 
\par \pard -- Function: char * \b ctime\b0  (\i const time_t *time\i0 )
\par 
\par \pard\li500 The \f1 ctime\f0  function is similar to \f1 asctime\f0 , except that you specify the calendar time argument as a \f1 time_t\f0  simple time value rather than in broken-down local time format. It is equivalent to 
\par 
\par \f1           asctime (localtime (\i time\i0 ))
\par      
\par ctime\f0  sets the variable \f1 tzname\f0 , because \f1 localtime\f0  does so. See \cf1\strike Time Zone Functions\strike0\{linkID=2310\}\cf0 .
\par 
\par \pard -- Function: size_t \b strftime\b0  (\i char *s, size_t size, const char *template, const struct tm *brokentime\i0 )
\par 
\par \pard\li500 This function is similar to the \f1 sprintf\f0  function (see \cf1\strike Formatted Input\strike0\{linkID=940\}\cf0 ), but the conversion specifications that can appear in the format template \i template\i0  are specialized for printing components of the date and time \i brokentime\i0  according to the locale currently specified for time conversion (see \cf1\strike Locales\strike0\{linkID=1340\}\cf0 ). 
\par 
\par Ordinary characters appearing in the \i template\i0  are copied to the output string \i s\i0 ; this can include multibyte character sequences. Conversion specifiers are introduced by a `\f1 %\f0 ' character, followed by an optional flag which can be one of the following. These flags are all GNU extensions. The first three affect only the output of numbers:
\par 
\par \f1 _\f0 
\par \pard\li1000 The number is padded with spaces. 
\par \pard\li500\f1 -\f0 
\par \pard\li1000 The number is not padded at all. 
\par \pard\li500\f1 0\f0 
\par \pard\li1000 The number is padded with zeros even if the format specifies padding with spaces. 
\par \pard\li500\f1 ^\f0 
\par \pard\li1000 The output uses uppercase characters, but only if this is possible (see \cf1\strike Case Conversion\strike0\{linkID=250\}\cf0 ).
\par 
\par \pard\li500 The default action is to pad the number with zeros to keep it a constant width. Numbers that do not have a range indicated below are never padded, since there is no natural width for them. 
\par 
\par Following the flag an optional specification of the width is possible. This is specified in decimal notation. If the natural size of the output is of the field has less than the specified number of characters, the result is written right adjusted and space padded to the given size. 
\par 
\par An optional modifier can follow the optional flag and width specification. The modifiers, which are POSIX.2 extensions, are:
\par 
\par \f1 E\f0 
\par \pard\li1000 Use the locale's alternate representation for date and time. This modifier applies to the \f1 %c\f0 , \f1 %C\f0 , \f1 %x\f0 , \f1 %X\f0 ,\f1  %y\f0  and \f1 %Y\f0  format specifiers. In a Japanese locale, for example, \f1 %Ex\f0  might yield a date format based on the Japanese Emperors' reigns. 
\par \pard\li500\f1 O\f0 
\par \pard\li1000 Use the locale's alternate numeric symbols for numbers. This modifier applies only to numeric format specifiers.
\par 
\par \pard\li500 If the format supports the modifier but no alternate representation is available, it is ignored. 
\par 
\par The conversion specifier ends with a format specifier taken from the following list. The whole `\f1 %\f0 ' sequence is replaced in the output string as follows:
\par 
\par \f1 %a\f0 
\par \pard\li1000 The abbreviated weekday name according to the current locale. 
\par \pard\li500\f1 %A\f0 
\par \pard\li1000 The full weekday name according to the current locale. 
\par \pard\li500\f1 %b\f0 
\par \pard\li1000 The abbreviated month name according to the current locale. 
\par \pard\li500\f1 %B\f0 
\par \pard\li1000 The full month name according to the current locale. 
\par \pard\li500\f1 %c\f0 
\par \pard\li1000 The preferred calendar time representation for the current locale. 
\par \pard\li500\f1 %C\f0 
\par \pard\li1000 The century of the year. This is equivalent to the greatest integer not greater than the year divided by 100. 
\par 
\par This format is a POSIX.2 extension and also appears in ISO C99. 
\par \pard\li500\f1 %d\f0 
\par \pard\li1000 The day of the month as a decimal number (range \f1 01\f0  through \f1 31\f0 ). 
\par \pard\li500\f1 %D\f0 
\par \pard\li1000 The date using the format \f1 %m/%d/%y\f0 . 
\par 
\par This format is a POSIX.2 extension and also appears in ISO C99. 
\par \pard\li500\f1 %e\f0 
\par \pard\li1000 The day of the month like with \f1 %d\f0 , but padded with blank (range\f1  1\f0  through \f1 31\f0 ). 
\par 
\par This format is a POSIX.2 extension and also appears in ISO C99. 
\par \pard\li500\f1 %F\f0 
\par \pard\li1000 The date using the format \f1 %Y-%m-%d\f0 . This is the form specified in the ISO 8601 standard and is the preferred form for all uses. 
\par 
\par This format is a ISO C99 extension. 
\par \pard\li500\f1 %g\f0 
\par \pard\li1000 The year corresponding to the ISO week number, but without the century (range \f1 00\f0  through \f1 99\f0 ). This has the same format and value as \f1 %y\f0 , except that if the ISO week number (see \f1 %V\f0 ) belongs to the previous or next year, that year is used instead. 
\par 
\par This format was introduced in ISO C99. 
\par \pard\li500\f1 %G\f0 
\par \pard\li1000 The year corresponding to the ISO week number. This has the same format and value as \f1 %Y\f0 , except that if the ISO week number (see\f1  %V\f0 ) belongs to the previous or next year, that year is used instead. 
\par 
\par This format was introduced in ISO C99 but was previously available as a GNU extension. 
\par \pard\li500\f1 %h\f0 
\par \pard\li1000 The abbreviated month name according to the current locale. The action is the same as for \f1 %b\f0 . 
\par 
\par This format is a POSIX.2 extension and also appears in ISO C99. 
\par \pard\li500\f1 %H\f0 
\par \pard\li1000 The hour as a decimal number, using a 24-hour clock (range \f1 00\f0  through\f1  23\f0 ). 
\par \pard\li500\f1 %I\f0 
\par \pard\li1000 The hour as a decimal number, using a 12-hour clock (range \f1 01\f0  through\f1  12\f0 ). 
\par \pard\li500\f1 %j\f0 
\par \pard\li1000 The day of the year as a decimal number (range \f1 001\f0  through \f1 366\f0 ). 
\par \pard\li500\f1 %k\f0 
\par \pard\li1000 The hour as a decimal number, using a 24-hour clock like \f1 %H\f0 , but padded with blank (range \f1 0\f0  through \f1 23\f0 ). 
\par 
\par This format is a GNU extension. 
\par \pard\li500\f1 %l\f0 
\par \pard\li1000 The hour as a decimal number, using a 12-hour clock like \f1 %I\f0 , but padded with blank (range \f1 1\f0  through \f1 12\f0 ). 
\par 
\par This format is a GNU extension. 
\par \pard\li500\f1 %m\f0 
\par \pard\li1000 The month as a decimal number (range \f1 01\f0  through \f1 12\f0 ). 
\par \pard\li500\f1 %M\f0 
\par \pard\li1000 The minute as a decimal number (range \f1 00\f0  through \f1 59\f0 ). 
\par \pard\li500\f1 %n\f0 
\par \pard\li1000 A single `\f1\\n\f0 ' (newline) character. 
\par 
\par This format is a POSIX.2 extension and also appears in ISO C99. 
\par \pard\li500\f1 %p\f0 
\par \pard\li1000 Either `\f1 AM\f0 ' or `\f1 PM\f0 ', according to the given time value; or the corresponding strings for the current locale. Noon is treated as `\f1 PM\f0 ' and midnight as `\f1 AM\f0 '. 
\par \pard\li500\f1 %P\f0 
\par \pard\li1000 Either `\f1 am\f0 ' or `\f1 pm\f0 ', according to the given time value; or the corresponding strings for the current locale, printed in lowercase characters. Noon is treated as `\f1 pm\f0 ' and midnight as `\f1 am\f0 '. 
\par 
\par This format was introduced in ISO C99 but was previously available as a GNU extension. 
\par \pard\li500\f1 %r\f0 
\par \pard\li1000 The complete calendar time using the AM/PM format of the current locale. 
\par 
\par This format is a POSIX.2 extension and also appears in ISO C99. 
\par \pard\li500\f1 %R\f0 
\par \pard\li1000 The hour and minute in decimal numbers using the format \f1 %H:%M\f0 . 
\par 
\par This format was introduced in ISO C99 but was previously available as a GNU extension. 
\par \pard\li500\f1 %s\f0 
\par \pard\li1000 The number of seconds since the epoch, i.e., since 1970-01-01 00:00:00 UTC. Leap seconds are not counted unless leap second support is available. 
\par 
\par This format is a GNU extension. 
\par \pard\li500\f1 %S\f0 
\par \pard\li1000 The seconds as a decimal number (range \f1 00\f0  through \f1 60\f0 ). 
\par \pard\li500\f1 %t\f0 
\par \pard\li1000 A single `\f1\\t\f0 ' (tabulator) character. 
\par 
\par This format is a POSIX.2 extension and also appears in ISO C99. 
\par \pard\li500\f1 %T\f0 
\par \pard\li1000 The time of day using decimal numbers using the format \f1 %H:%M:%S\f0 . 
\par 
\par This format is a POSIX.2 extension. 
\par \pard\li500\f1 %u\f0 
\par \pard\li1000 The day of the week as a decimal number (range \f1 1\f0  through\f1  7\f0 ), Monday being \f1 1\f0 . 
\par 
\par This format is a POSIX.2 extension and also appears in ISO C99. 
\par \pard\li500\f1 %U\f0 
\par \pard\li1000 The week number of the current year as a decimal number (range \f1 00\f0  through \f1 53\f0 ), starting with the first Sunday as the first day of the first week. Days preceding the first Sunday in the year are considered to be in week \f1 00\f0 . 
\par \pard\li500\f1 %V\f0 
\par \pard\li1000 The ISO 8601:1988 week number as a decimal number (range \f1 01\f0  through \f1 53\f0 ). ISO weeks start with Monday and end with Sunday. Week \f1 01\f0  of a year is the first week which has the majority of its days in that year; this is equivalent to the week containing the year's first Thursday, and it is also equivalent to the week containing January 4. Week \f1 01\f0  of a year can contain days from the previous year. The week before week \f1 01\f0  of a year is the last week (\f1 52\f0  or\f1  53\f0 ) of the previous year even if it contains days from the new year. 
\par 
\par This format is a POSIX.2 extension and also appears in ISO C99. 
\par \pard\li500\f1 %w\f0 
\par \pard\li1000 The day of the week as a decimal number (range \f1 0\f0  through\f1  6\f0 ), Sunday being \f1 0\f0 . 
\par \pard\li500\f1 %W\f0 
\par \pard\li1000 The week number of the current year as a decimal number (range \f1 00\f0  through \f1 53\f0 ), starting with the first Monday as the first day of the first week. All days preceding the first Monday in the year are considered to be in week \f1 00\f0 . 
\par \pard\li500\f1 %x\f0 
\par \pard\li1000 The preferred date representation for the current locale. 
\par \pard\li500\f1 %X\f0 
\par \pard\li1000 The preferred time of day representation for the current locale. 
\par \pard\li500\f1 %y\f0 
\par \pard\li1000 The year without a century as a decimal number (range \f1 00\f0  through\f1  99\f0 ). This is equivalent to the year modulo 100. 
\par \pard\li500\f1 %Y\f0 
\par \pard\li1000 The year as a decimal number, using the Gregorian calendar. Years before the year \f1 1\f0  are numbered \f1 0\f0 , \f1 -1\f0 , and so on. 
\par \pard\li500\f1 %z\f0 
\par \pard\li1000 RFC 822/ISO 8601:1988 style numeric time zone (e.g.,\f1  -0600\f0  or \f1 +0100\f0 ), or nothing if no time zone is determinable. 
\par 
\par This format was introduced in ISO C99 but was previously available as a GNU extension. 
\par 
\par A full RFC 822 timestamp is generated by the format `\f1 "%a, %d %b %Y %H:%M:%S %z"\f0 ' (or the equivalent `\f1 "%a, %d %b %Y %T %z"\f0 '). 
\par \pard\li500\f1 %Z\f0 
\par \pard\li1000 The time zone abbreviation (empty if the time zone can't be determined). 
\par \pard\li500\f1 %%\f0 
\par \pard\li1000 A literal `\f1 %\f0 ' character.
\par 
\par \pard\li500 The \i size\i0  parameter can be used to specify the maximum number of characters to be stored in the array \i s\i0 , including the terminating null character. If the formatted time requires more than \i size\i0  characters, \f1 strftime\f0  returns zero and the contents of the array\i  s\i0  are undefined. Otherwise the return value indicates the number of characters placed in the array \i s\i0 , not including the terminating null character. 
\par 
\par \i Warning:\i0  This convention for the return value which is prescribed in ISO C can lead to problems in some situations. For certain format strings and certain locales the output really can be the empty string and this cannot be discovered by testing the return value only. E.g., in most locales the AM/PM time format is not supported (most of the world uses the 24 hour time representation). In such locales\f1  "%p"\f0  will return the empty string, i.e., the return value is zero. To detect situations like this something similar to the following code should be used: 
\par 
\par \f1           buf[0] = '\\1';
\par           len = strftime (buf, bufsize, format, tp);
\par           if (len == 0 && buf[0] != '\\0')
\par             \{
\par               /* Something went wrong in the strftime call.  */
\par               ...
\par             \}
\par      
\par \f0 If \i s\i0  is a null pointer, \f1 strftime\f0  does not actually write anything, but instead returns the number of characters it would have written. 
\par 
\par According to POSIX.1 every call to \f1 strftime\f0  implies a call to\f1  tzset\f0 . So the contents of the environment variable \f1 TZ\f0  is examined before any output is produced. 
\par 
\par For an example of \f1 strftime\f0 , see \cf1\strike Time Functions Example\strike0\{linkID=2300\}\cf0 .
\par 
\par \pard -- Function: size_t \b wcsftime\b0  (\i wchar_t *s, size_t size, const wchar_t *template, const struct tm *brokentime\i0 )
\par 
\par \pard\li500 The \f1 wcsftime\f0  function is equivalent to the \f1 strftime\f0  function with the difference that it operates on wide character strings. The buffer where the result is stored, pointed to by \i s\i0 , must be an array of wide characters. The parameter \i size\i0  which specifies the size of the output buffer gives the number of wide character, not the number of bytes. 
\par 
\par Also the format string \i template\i0  is a wide character string. Since all characters needed to specify the format string are in the basic character set it is portably possible to write format strings in the C source code using the \f1 L"..."\f0  notation. The parameter\i  brokentime\i0  has the same meaning as in the \f1 strftime\f0  call. 
\par 
\par The \f1 wcsftime\f0  function supports the same flags, modifiers, and format specifiers as the \f1 strftime\f0  function. 
\par 
\par The return value of \f1 wcsftime\f0  is the number of wide characters stored in \f1 s\f0 . When more characters would have to be written than can be placed in the buffer \i s\i0  the return value is zero, with the same problems indicated in the \f1 strftime\f0  documentation.
\par 
\par }
990
PAGE_230
FP Bit Twiddling
FParithmetic;copysign;copysignf;copysignl;signbit;nextafter;nextafterf;nextafterl;nexttoward;nexttowardf;nexttowardl;NaN;nan;nanf;nanl



Imported



FALSE
59
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fswiss\fcharset0 Arial;}{\f2\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 FP Bit Twiddling \{keepn\f1\}\f0 
\par \pard\fs18 Next: \cf1\strike FP Comparison Functions\strike0\{linkID=1000\}\cf0 , Previous: \cf1\strike Remainder Functions\strike0\{linkID=1820\}\cf0 , Up: \cf1\strike Arithmetic Functions\strike0\{linkID=90\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Setting and modifying single bits of FP values\b0 
\par 
\par There are some operations that are too complicated or expensive to perform by hand on floating-point numbers. ISO C99 defines functions to do these operations, which mostly involve changing single bits.
\par 
\par -- Function: double \b copysign\b0  (\i double x, double y\i0 )
\par 
\par \pard\li500 -- Function: float \b copysignf\b0  (\i float x, float y\i0 )
\par 
\par \pard\li1000 -- Function: long double \b copysignl\b0  (\i long double x, long double y\i0 )
\par 
\par \pard\li1500 These functions return \i x\i0  but with the sign of \i y\i0 . They work even if \i x\i0  or \i y\i0  are NaN or zero. Both of these can carry a sign (although not all implementations support it) and this is one of the few operations that can tell the difference. 
\par 
\par \f2 copysign\f0  never raises an exception. 
\par 
\par This function is defined in IEC 559 (and the appendix with recommended functions in IEEE 754/IEEE 854).
\par 
\par \pard -- Function: int \b signbit\b0  (\i float-type x\i0 )
\par 
\par \pard\li500\f2 signbit\f0  is a generic macro which can work on all floating-point types. It returns a nonzero value if the value of \i x\i0  has its sign bit set. 
\par 
\par This is not the same as \f2 x < 0.0\f0 , because IEEE 754 floating point allows zero to be signed. The comparison \f2 -0.0 < 0.0\f0  is false, but \f2 signbit (-0.0)\f0  will return a nonzero value.
\par 
\par \pard -- Function: double \b nextafter\b0  (\i double x, double y\i0 )
\par 
\par \pard\li500 -- Function: float \b nextafterf\b0  (\i float x, float y\i0 )
\par 
\par \pard\li1000 -- Function: long double \b nextafterl\b0  (\i long double x, long double y\i0 )
\par 
\par \pard\li1500 The \f2 nextafter\f0  function returns the next representable neighbor of\i  x\i0  in the direction towards \i y\i0 . The size of the step between\i  x\i0  and the result depends on the type of the result. If\i  x\i0  = \i y\i0  the function simply returns \i y\i0 . If either value is \f2 NaN\f0 , \f2 NaN\f0  is returned. Otherwise a value corresponding to the value of the least significant bit in the mantissa is added or subtracted, depending on the direction.\f2  nextafter\f0  will signal overflow or underflow if the result goes outside of the range of normalized numbers. 
\par 
\par This function is defined in IEC 559 (and the appendix with recommended functions in IEEE 754/IEEE 854).
\par 
\par \pard -- Function: double \b nexttoward\b0  (\i double x, long double y\i0 )
\par 
\par \pard\li500 -- Function: float \b nexttowardf\b0  (\i float x, long double y\i0 )
\par 
\par \pard\li1000 -- Function: long double \b nexttowardl\b0  (\i long double x, long double y\i0 )
\par 
\par \pard\li1500 These functions are identical to the corresponding versions of\f2  nextafter\f0  except that their second argument is a \f2 long double\f0 .
\par 
\par \pard -- Function: double \b nan\b0  (\i const char *tagp\i0 )
\par 
\par \pard\li500 -- Function: float \b nanf\b0  (\i const char *tagp\i0 )
\par 
\par \pard\li1000 -- Function: long double \b nanl\b0  (\i const char *tagp\i0 )
\par 
\par \pard\li1500 The \f2 nan\f0  function returns a representation of NaN, provided that NaN is supported by the target platform.\f2  nan ("\i\f0 n-char-sequence\i0\f2 ")\f0  is equivalent to\f2  strtod ("NAN(\i\f0 n-char-sequence\i0\f2 )")\f0 . 
\par 
\par The argument \i tagp\i0  is used in an unspecified manner. On IEEE 754 systems, there are many representations of NaN, and \i tagp\i0  selects one. On other systems it may do nothing.
\par 
\par }
1000
PAGE_231
FP Comparison Functions
unorderedcomparison;isgreater;isgreaterequal;isless;islessequal;islessgreater;isunordered



Imported



FALSE
47
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 FP Comparison Functions \{keepn\}
\par \pard\fs18 Next: \cf1\strike Misc FP Arithmetic\strike0\{linkID=1450\}\cf0 , Previous: \cf1\strike FP Bit Twiddling\strike0\{linkID=990\}\cf0 , Up: \cf1\strike Arithmetic Functions\strike0\{linkID=90\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Floating-Point Comparison Functions\b0 
\par 
\par The standard C comparison operators provoke exceptions when one or other of the operands is NaN. For example,
\par 
\par \f1      int v = a < 1.0;
\par \f0 
\par will raise an exception if \i a\i0  is NaN. (This does \i not\i0  happen with \f1 ==\f0  and \f1 !=\f0 ; those merely return false and true, respectively, when NaN is examined.) Frequently this exception is undesirable. ISO C99 therefore defines comparison functions that do not raise exceptions when NaN is examined. All of the functions are implemented as macros which allow their arguments to be of any floating-point type. The macros are guaranteed to evaluate their arguments only once.
\par 
\par -- Macro: int \b isgreater\b0  (\i real-floating x, real-floating y\i0 )
\par 
\par \pard\li500 This macro determines whether the argument \i x\i0  is greater than\i  y\i0 . It is equivalent to \f1 (\i\f0 x\i0\f1 ) > (\i\f0 y\i0\f1 )\f0 , but no exception is raised if \i x\i0  or \i y\i0  are NaN.
\par 
\par \pard -- Macro: int \b isgreaterequal\b0  (\i real-floating x, real-floating y\i0 )
\par 
\par \pard\li500 This macro determines whether the argument \i x\i0  is greater than or equal to \i y\i0 . It is equivalent to \f1 (\i\f0 x\i0\f1 ) >= (\i\f0 y\i0\f1 )\f0 , but no exception is raised if \i x\i0  or \i y\i0  are NaN.
\par 
\par \pard -- Macro: int \b isless\b0  (\i real-floating x, real-floating y\i0 )
\par 
\par \pard\li500 This macro determines whether the argument \i x\i0  is less than \i y\i0 . It is equivalent to \f1 (\i\f0 x\i0\f1 ) < (\i\f0 y\i0\f1 )\f0 , but no exception is raised if \i x\i0  or \i y\i0  are NaN.
\par 
\par \pard -- Macro: int \b islessequal\b0  (\i real-floating x, real-floating y\i0 )
\par 
\par \pard\li500 This macro determines whether the argument \i x\i0  is less than or equal to \i y\i0 . It is equivalent to \f1 (\i\f0 x\i0\f1 ) <= (\i\f0 y\i0\f1 )\f0 , but no exception is raised if \i x\i0  or \i y\i0  are NaN.
\par 
\par \pard -- Macro: int \b islessgreater\b0  (\i real-floating x, real-floating y\i0 )
\par 
\par \pard\li500 This macro determines whether the argument \i x\i0  is less or greater than \i y\i0 . It is equivalent to \f1 (\i\f0 x\i0\f1 ) < (\i\f0 y\i0\f1 ) || (\i\f0 x\i0\f1 ) > (\i\f0 y\i0\f1 )\f0  (although it only evaluates \i x\i0  and \i y\i0  once), but no exception is raised if \i x\i0  or \i y\i0  are NaN. 
\par 
\par This macro is not equivalent to \i x\i0\f1  != \i\f0 y\i0 , because that expression is true if \i x\i0  or \i y\i0  are NaN.
\par 
\par \pard -- Macro: int \b isunordered\b0  (\i real-floating x, real-floating y\i0 )
\par 
\par \pard\li500 This macro determines whether its arguments are unordered. In other words, it is true if \i x\i0  or \i y\i0  are NaN, and false otherwise.
\par 
\par \pard Not all machines provide hardware support for these operations. On machines that don't, the macros can be very slow. Therefore, you should not use these functions when NaN is not a concern. 
\par 
\par \b Note:\b0  There are no macros \f1 isequal\f0  or \f1 isunequal\f0 . They are unnecessary, because the \f1 ==\f0  and \f1 !=\f0  operators do\i  not\i0  throw an exception if one or both of the operands are NaN.
\par 
\par }
1010
PAGE_232
FP Exceptions
exception;signal;zerodivide;divisionbyzero;inexactexception;invalidexception;overflowexception;underflowexception;matherr



Imported



FALSE
52
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 FP Exceptions \{keepn\}
\par \pard\fs18 Next: \cf1\strike Infinity and NaN\strike0\{linkID=1160\}\cf0 , Up: \cf1\strike Floating Point Errors\strike0\{linkID=860\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b FP Exceptions\b0 
\par 
\par The IEEE 754 standard defines five \i exceptions\i0  that can occur during a calculation. Each corresponds to a particular sort of error, such as overflow. 
\par 
\par When exceptions occur (when exceptions are \i raised\i0 , in the language of the standard), one of two things can happen. By default the exception is simply noted in the floating-point \i status word\i0 , and the program continues as if nothing had happened. The operation produces a default value, which depends on the exception (see the table below). Your program can check the status word to find out which exceptions happened. 
\par 
\par Alternatively, you can enable \i traps\i0  for exceptions. In that case, when an exception is raised, your program will receive the \f1 SIGFPE\f0  signal. The default action for this signal is to terminate the program. See \cf1\strike Signal Handling\strike0\{linkID=2010\}\cf0 , for how you can change the effect of the signal. 
\par 
\par In the System V math library, the user-defined function \f1 matherr\f0  is called when certain exceptions occur inside math library functions. However, the Unix98 standard deprecates this interface. We support it for historical compatibility, but recommend that you do not use it in new programs.
\par 
\par The exceptions defined in IEEE 754 are:
\par 
\par `\f1 Invalid Operation\f0 '
\par \pard\li500 This exception is raised if the given operands are invalid for the operation to be performed. Examples are (see IEEE 754, section 7):
\par 
\par \pard\li1000 1. Addition or subtraction: &infin; - &infin;. (But &infin; + &infin; = &infin;).
\par 2. Multiplication: 0 &middot; &infin;.
\par 3. Division: 0/0 or &infin;/&infin;.
\par 4. Remainder: x REM y, where y is zero or x is infinite.
\par 5. Square root if the operand is less then zero. More generally, any mathematical function evaluated outside its domain produces this exception.
\par 6. Conversion of a floating-point number to an integer or decimal string, when the number cannot be represented in the target format (due to overflow, infinity, or NaN).
\par 7. Conversion of an unrecognizable input string.
\par 8. Comparison via predicates involving < or >, when one or other of the operands is NaN. You can prevent this exception by using the unordered comparison functions instead; see \cf1\strike FP Comparison Functions\strike0\{linkID=1000\}\cf0 .
\par 
\par \pard\li500 If the exception does not trap, the result of the operation is NaN. 
\par \pard `\f1 Division by Zero\f0 '
\par \pard\li500 This exception is raised when a finite nonzero number is divided by zero. If no trap occurs the result is either +&infin; or -&infin;, depending on the signs of the operands. 
\par \pard `\f1 Overflow\f0 '
\par \pard\li500 This exception is raised whenever the result cannot be represented as a finite value in the precision format of the destination. If no trap occurs the result depends on the sign of the intermediate result and the current rounding mode (IEEE 754, section 7.3):
\par 
\par \pard\li1000 1. Round to nearest carries all overflows to &infin; with the sign of the intermediate result.
\par 2. Round toward 0 carries all overflows to the largest representable finite number with the sign of the intermediate result.
\par 3. Round toward -&infin; carries positive overflows to the largest representable finite number and negative overflows to -&infin;. 
\par 4. Round toward &infin; carries negative overflows to the most negative representable finite number and positive overflows to &infin;.
\par 
\par \pard\li500 Whenever the overflow exception is raised, the inexact exception is also raised. 
\par \pard `\f1 Underflow\f0 '
\par \pard\li500 The underflow exception is raised when an intermediate result is too small to be calculated accurately, or if the operation's result rounded to the destination precision is too small to be normalized. 
\par 
\par When no trap is installed for the underflow exception, underflow is signaled (via the underflow flag) only when both tininess and loss of accuracy have been detected. If no trap handler is installed the operation continues with an imprecise small value, or zero if the destination precision cannot hold the small exact result. 
\par \pard `\f1 Inexact\f0 '
\par \pard\li500 This exception is signalled if a rounded result is not exact (such as when calculating the square root of two) or a result overflows without an overflow trap.
\par 
\par }
1020
PAGE_234
Free Manuals
freedocumentation



Imported



FALSE
35
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Free Manuals \{keepn\}
\par \pard\fs18 Next: \cf1\strike Copying\strike0\{linkID=470\}\cf0 , Previous: \cf1\strike Language Features\cf2\strike0\{linkID=1310\}\cf0 , Up: \cf1\strike Top\strike0\{linkID=10\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Appendix F Free Software Needs Free Documentation\b0 
\par 
\par The biggest deficiency in the free software community today is not in the software--it is the lack of good free documentation that we can include with the free software. Many of our most important programs do not come with free reference manuals and free introductory texts. Documentation is an essential part of any software package; when an important free software package does not come with a free manual and a free tutorial, that is a major gap. We have many such gaps today. 
\par 
\par Consider Perl, for instance. The tutorial manuals that people normally use are non-free. How did this come about? Because the authors of those manuals published them with restrictive terms--no copying, no modification, source files not available--which exclude them from the free software world. 
\par 
\par That wasn't the first time this sort of thing happened, and it was far from the last. Many times we have heard a GNU user eagerly describe a manual that he is writing, his intended contribution to the community, only to learn that he had ruined everything by signing a publication contract to make it non-free. 
\par 
\par Free documentation, like free software, is a matter of freedom, not price. The problem with the non-free manual is not that publishers charge a price for printed copies--that in itself is fine. (The Free Software Foundation sells printed copies of manuals, too.) The problem is the restrictions on the use of the manual. Free manuals are available in source code form, and give you permission to copy and modify. Non-free manuals do not allow this. 
\par 
\par The criteria of freedom for a free manual are roughly the same as for free software. Redistribution (including the normal kinds of commercial redistribution) must be permitted, so that the manual can accompany every copy of the program, both on-line and on paper. 
\par 
\par Permission for modification of the technical content is crucial too. When people modify the software, adding or changing features, if they are conscientious they will change the manual too--so they can provide accurate and clear documentation for the modified program. A manual that leaves you no choice but to write a new manual to document a changed version of the program is not really available to our community. 
\par 
\par Some kinds of limits on the way modification is handled are acceptable. For example, requirements to preserve the original author's copyright notice, the distribution terms, or the list of authors, are ok. It is also no problem to require modified versions to include notice that they were modified. Even entire sections that may not be deleted or changed are acceptable, as long as they deal with nontechnical topics (like this one). These kinds of restrictions are acceptable because they don't obstruct the community's normal use of the manual. 
\par 
\par However, it must be possible to modify all the \i technical\i0  content of the manual, and then distribute the result in all the usual media, through all the usual channels. Otherwise, the restrictions obstruct the use of the manual, it is not free, and we need another manual to replace it. 
\par 
\par Please spread the word about this issue. Our community continues to lose manuals to proprietary publishing. If we spread the word that free software needs free reference manuals and free tutorials, perhaps the next person who wants to contribute by writing documentation will realize, before it is too late, that only free manuals contribute to the free software community. 
\par 
\par If you are writing documentation, please insist on publishing it under the GNU Free Documentation License or another free documentation license. Remember that this decision requires your approval--you don't have to let the publisher decide. Some commercial publishers will use a free license if you insist, but they will not propose the option; it is up to you to raise the issue and say firmly that this is what you want. If the publisher you are dealing with refuses, please try other publishers. If you're not sure whether a proposed license is free, write to \cf2\strike licensing@gnu.org\strike0\{link=*!ExecFile("mailto:licensing@gnu.org")\}\cf0 . 
\par 
\par You can encourage commercial publishers to sell more free, copylefted manuals and tutorials by buying them, and particularly by buying copies from the publishers that paid for their writing or for major improvements. Meanwhile, try to avoid buying non-free documentation at all. Check the distribution terms of a manual before you buy it, and insist that whoever seeks your business must respect your freedom. Check the history of the book, and try reward the publishers that have paid or pay the authors to work on it. 
\par 
\par The Free Software Foundation maintains a list of free documentation published by other publishers, at \cf2\strike http://www.fsf.org/doc/other-free-books.html\strike0\{link=*!ExecFile("http://www.fsf.org/doc/other-free-books.html")\}\cf0 .
\par 
\par }
1030
PAGE_235
Freeing after Malloc
freeingmemoryallocatedwithmalloc;heap,freeingmemoryfrom;stdlib.h;free;cfree



Imported



FALSE
41
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Freeing after Malloc \{keepn\}
\par \pard\fs18 Next: \cf1\strike Changing Block Size\strike0\{linkID=260\}\cf0 , Previous: \cf1\strike Malloc Examples\strike0\{linkID=1380\}\cf0 , Up: \cf1\strike Unconstrained Allocation\strike0\{linkID=2340\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Freeing Memory Allocated with \f1 malloc\b0\f0 
\par 
\par When you no longer need a block that you got with \f1 malloc\f0 , use the function \f1 free\f0  to make the block available to be allocated again. The prototype for this function is in \f1 stdlib.h\f0 .
\par 
\par -- Function: void \b free\b0  (\i void *ptr\i0 )
\par 
\par \pard\li500 The \f1 free\f0  function deallocates the block of memory pointed at by \i ptr\i0 .
\par \pard 
\par Freeing a block alters the contents of the block. \b Do not expect to find any data (such as a pointer to the next block in a chain of blocks) in the block after freeing it.\b0  Copy whatever you need out of the block before freeing it! Here is an example of the proper way to free all the blocks in a chain, and the strings that they point to:
\par 
\par \f1      struct chain
\par        \{
\par          struct chain *next;
\par          char *name;
\par        \}
\par      
\par      void
\par      free_chain (struct chain *chain)
\par      \{
\par        while (chain != 0)
\par          \{
\par            struct chain *next = chain->next;
\par            free (chain->name);
\par            free (chain);
\par            chain = next;
\par          \}
\par      \}
\par \f0 
\par Occasionally, \f1 free\f0  can actually return memory to the operating system and make the process smaller. Usually, all it can do is allow a later call to \f1 malloc\f0  to reuse the space. In the meantime, the space remains in your program as part of a free-list used internally by\f1  malloc\f0 . 
\par 
\par There is no point in freeing blocks at the end of a program, because all of the program's space is given back to the system when the process terminates.
\par 
\par }
1040
PAGE_241
General Numeric
decimal-pointseparator;groupingofdigits



Imported



FALSE
34
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 General Numeric \{keepn\}
\par \pard\fs18 Next: \cf1\strike Currency Symbol\strike0\{linkID=500\}\cf0 , Up: \cf1\strike The Lame Way to Locale Data\strike0\{linkID=2280\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Generic Numeric Formatting Parameters\b0 
\par 
\par These are the standard members of \f1 struct lconv\f0 ; there may be others.
\par 
\par \f1 char *decimal_point\f0 
\par \f1 char *mon_decimal_point\f0 
\par \pard\li500 These are the decimal-point separators used in formatting non-monetary and monetary quantities, respectively. In the `\f1 C\f0 ' locale, the value of \f1 decimal_point\f0  is \f1 "."\f0 , and the value of\f1  mon_decimal_point\f0  is \f1 ""\f0 .
\par \pard\f1 char *thousands_sep\f0 
\par \f1 char *mon_thousands_sep\f0 
\par \pard\li500 These are the separators used to delimit groups of digits to the left of the decimal point in formatting non-monetary and monetary quantities, respectively. In the `\f1 C\f0 ' locale, both members have a value of\f1  ""\f0  (the empty string). 
\par \pard\f1 char *grouping\f0 
\par \f1 char *mon_grouping\f0 
\par \pard\li500 These are strings that specify how to group the digits to the left of the decimal point. \f1 grouping\f0  applies to non-monetary quantities and \f1 mon_grouping\f0  applies to monetary quantities. Use either\f1  thousands_sep\f0  or \f1 mon_thousands_sep\f0  to separate the digit groups. Each member of these strings is to be interpreted as an integer value of type \f1 char\f0 . Successive numbers (from left to right) give the sizes of successive groups (from right to left, starting at the decimal point.) The last member is either \f1 0\f0 , in which case the previous member is used over and over again for all the remaining groups, or\f1  CHAR_MAX\f0 , in which case there is no more grouping--or, put another way, any remaining digits form one large group without separators. 
\par 
\par For example, if \f1 grouping\f0  is \f1 "\\04\\03\\02"\f0 , the correct grouping for the number \f1 123456787654321\f0  is `\f1 12\f0 ', `\f1 34\f0 ', `\f1 56\f0 ', `\f1 78\f0 ', `\f1 765\f0 ', `\f1 4321\f0 '. This uses a group of 4 digits at the end, preceded by a group of 3 digits, preceded by groups of 2 digits (as many as needed). With a separator of `\f1 ,\f0 ', the number would be printed as `\f1 12,34,56,78,765,4321\f0 '. 
\par 
\par A value of \f1 "\\03"\f0  indicates repeated groups of three digits, as normally used in the U.S. 
\par 
\par In the standard `\f1 C\f0 ' locale, both \f1 grouping\f0  and\f1  mon_grouping\f0  have a value of \f1 ""\f0 . This value specifies no grouping at all. 
\par \pard\f1 char int_frac_digits\f0 
\par \f1 char frac_digits\f0 
\par \pard\li500 These are small integers indicating how many fractional digits (to the right of the decimal point) should be displayed in a monetary value in international and local formats, respectively. (Most often, both members have the same value.) 
\par 
\par In the standard `\f1 C\f0 ' locale, both of these members have the value\f1  CHAR_MAX\f0 , meaning "unspecified". The ISO standard doesn't say what to do when you find this value; we recommend printing no fractional digits. (This locale also specifies the empty string for\f1  mon_decimal_point\f0 , so printing any fractional digits would be confusing!)
\par 
\par }
1050
PAGE_243
Generating Signals
sendingsignals;raisingsignals;signals,generating



Imported



FALSE
15
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Generating Signals \{keepn\}
\par \pard\fs18 Previous: \cf1\strike Signal Actions\cf2\strike0\{linkID=1990\}\cf0 , Up: \cf1\strike Signal Handling\strike0\{linkID=2010\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Generating Signals\b0 
\par 
\par Besides signals that are generated as a result of a hardware trap or interrupt, your program can explicitly send signals to itself or to another process.
\par 
\par \pard\li500\f1\'b7\f0  \cf1\strike Signaling Yourself\strike0\{linkID=2020\}\cf0 : A process can send a signal to itself.
\par \f1 
\par }
1060
PAGE_251
Getting Started




Imported



FALSE
17
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Getting Started \{keepn\}
\par \pard\fs18 Next: \cf1\strike Roadmap to the Manual\cf2\strike0\{linkID=1880\}\cf0 , Up: \cf1\strike Introduction\strike0\{linkID=1210\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Getting Started\b0 
\par 
\par This manual is written with the assumption that you are at least somewhat familiar with the C programming language and basic programming concepts. Specifically, familiarity with ISO standard C (see \cf1\strike ISO C\strike0\{linkID=1230\}\cf0 ), rather than "traditional" pre-ISO C dialects, is assumed. 
\par 
\par The C library includes several \i header files\i0 , each of which provides definitions and declarations for a group of related facilities; this information is used by the C compiler when processing your program. For example, the header file \f1 stdio.h\f0  declares facilities for performing input and output, and the header file \f1 string.h\f0  declares string processing utilities. The organization of this manual generally follows the same division as the header files. 
\par 
\par If you are reading this manual for the first time, you should read all of the introductory material and skim the remaining chapters. There are a \i lot\i0  of functions in the\f2  \f0 C library and it's not realistic to expect that you will be able to remember exactly \i how\i0  to use each and every one of them. It's more important to become generally familiar with the kinds of facilities that the library provides, so that when you are writing your programs you can recognize \i when\i0  to make use of library functions, and \i where\i0  in this manual you can find more specific information about them.
\par 
\par }
1070
PAGE_254
C99 Variable-Size Arrays
variable-sizedarrays



Imported



FALSE
27
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss\fcharset0 Arial;}{\f1\fswiss Arial;}{\f2\fmodern Courier New;}{\f3\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 C99 Variable-Size Arrays \{keepn\}\f1 
\par \pard\fs18 Previous: \cf1\strike Disadvantages of Alloca\strike0\{linkID=580\}\cf0 , Up: \cf1\strike Variable Size Automatic\strike0\{linkID=2400\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b\f0 C99 \f1 Variable-Size Arrays\b0 
\par 
\par In C\f0 99\f1 , you can replace most uses of \f2 alloca\f1  with an array of variable size. Here is how \f2 open2\f1  would look then:
\par 
\par \f2      int open2 (char *str1, char *str2, int flags, int mode)
\par      \{
\par        char name[strlen (str1) + strlen (str2) + 1];
\par        stpcpy (stpcpy (name, str1), str2);
\par        return open (name, flags, mode);
\par      \}
\par \f1 
\par But \f2 alloca\f1  is not always equivalent to a variable-sized array, for several reasons:
\par 
\par \pard\li500\f3\'b7\f1  A variable size array's space is freed at the end of the scope of the name of the array. The space allocated with \f2 alloca\f1  remains until the end of the function. 
\par \f3\'b7\f1  It is possible to use \f2 alloca\f1  within a loop, allocating an additional block on each iteration. This is impossible with variable-sized arrays.
\par 
\par \pard\b Note:\b0  If you mix use of \f2 alloca\f1  and variable-sized arrays within one function, exiting a scope in which a variable-sized array was declared frees all blocks allocated with \f2 alloca\f1  during the execution of that scope.
\par 
\par }
1080
PAGE_259
Handler Returns




Imported



FALSE
56
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Handler Returns \{keepn\}
\par \pard\fs18 Next: \cf1\strike Termination in Handler\strike0\{linkID=2260\}\cf0 , Up: \cf1\strike Defining Handlers\strike0\{linkID=530\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Signal Handlers that Return\b0 
\par 
\par Handlers which return normally are usually used for signals such as\f1  SIGALRM\f0  and the I/O and interprocess communication signals. But a handler for \f1 SIGINT\f0  might also return normally after setting a flag that tells the program to exit at a convenient time. 
\par 
\par It is not safe to return normally from the handler for a program error signal, because the behavior of the program when the handler function returns is not defined after a program error. See \cf1\strike Program Error Signals\strike0\{linkID=1760\}\cf0 . 
\par 
\par Handlers that return normally must modify some global variable in order to have any effect. Typically, the variable is one that is examined periodically by the program during normal operation. Its data type should be \f1 sig_atomic_t\f0  for reasons described in \cf1\strike Atomic Data Access\strike0\{linkID=130\}\cf0 . 
\par 
\par Here is a simple example of such a program. It executes the body of the loop until it has noticed that a \f1 SIGALRM\f0  signal has arrived. This technique is useful because it allows the iteration in progress when the signal arrives to complete before the loop exits.
\par 
\par \f1      #include <signal.h>
\par      #include <stdio.h>
\par      #include <stdlib.h>
\par      
\par      /* This flag controls termination of the main loop. */
\par      volatile sig_atomic_t keep_going = 1;
\par      
\par      /* The signal handler just clears the flag and re-enables itself. */
\par      void
\par      catch_alarm (int sig)
\par      \{
\par        keep_going = 0;
\par        signal (sig, catch_alarm);
\par      \}
\par      
\par      void
\par      do_stuff (void)
\par      \{
\par        puts ("Doing stuff while waiting for alarm....");
\par      \}
\par      
\par      int
\par      main (void)
\par      \{
\par        /* Establish a handler for SIGALRM signals. */
\par        signal (SIGALRM, catch_alarm);
\par      
\par        /* Set an alarm to go off in a little while. */
\par        alarm (2);
\par      
\par        /* Check the flag once in a while to see when to quit. */
\par        while (keep_going)
\par          do_stuff ();
\par      
\par        return EXIT_SUCCESS;
\par      \}
\par \f0 
\par }
1090
PAGE_262
Header Files
headerfiles;definition(comparedtodeclaration;declaration(comparedtodefinition



Imported



FALSE
35
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Header Files \{keepn\}
\par \pard\fs18 Next: \cf1\strike Macro Definitions\strike0\{linkID=1370\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Header Files\b0 
\par 
\par Libraries for use by C programs really consist of two parts: \i header files\i0  that define types and macros and declare variables and functions; and the actual library or \i archive\i0  that contains the definitions of the variables and functions. 
\par 
\par (Recall that in C, a \i declaration\i0  merely provides information that a function or variable exists and gives its type. For a function declaration, information about the types of its arguments might be provided as well. The purpose of declarations is to allow the compiler to correctly process references to the declared variables and functions. A \i definition\i0 , on the other hand, actually allocates storage for a variable or says what a function does.) In order to use the facilities in the C library, you should be sure that your program source files include the appropriate header files. This is so that the compiler has declarations of these facilities available and can correctly process references to them. Once your program has been compiled, the linker resolves these references to the actual definitions provided in the archive file. 
\par 
\par Header files are included into a program source file by the `\f1 #include\f0 ' preprocessor directive. The C language supports two forms of this directive; the first,
\par 
\par \f1      #include "\i header\i0 "
\par \f0 
\par is typically used to include a header file \i header\i0  that you write yourself; this would contain definitions and declarations describing the interfaces between the different parts of your particular application. By contrast,
\par 
\par \f1      #include <file.h>
\par \f0 
\par is typically used to include a header file \f1 file.h\f0  that contains definitions and declarations for a standard library. This file would normally be installed in a standard place by your system administrator. You should use this second form for the C library header files. 
\par 
\par Typically, `\f1 #include\f0 ' directives are placed at the top of the C source file, before any other code. If you begin your source files with some comments explaining what the code in the file does (a good idea), put the `\f1 #include\f0 ' directives immediately afterwards\f2 .
\par \f0 
\par The C library provides several header files, each of which contains the type and macro definitions and variable and function declarations for a group of related facilities. This means that your programs may need to include several header files, depending on exactly which facilities you are using. 
\par 
\par Some library header files include other library header files automatically. However, as a matter of programming style, you should not rely on this; it is better to explicitly include all the header files required for the library facilities you are using. The C library header files have been written in such a way that it doesn't matter if a header file is accidentally included more than once; including a header file a second time has no effect. Likewise, if your program needs to include multiple header files, the order in which they are included doesn't matter. 
\par 
\par \b Compatibility Note:\b0  Inclusion of standard header files in any order and any number of times works in any ISO C implementation. However, this has traditionally not been the case in many older C implementations. 
\par 
\par Strictly speaking, you don't \i have to\i0  include a header file to use a function it declares; you could declare the function explicitly yourself, according to the specifications in this manual. But it is usually better to include the header file because it may define types and macros that are not otherwise available and because it may define more efficient macro replacements for some functions. It is also a sure way to have the correct declaration.
\par 
\par }
1100
PAGE_276
How Many Arguments
numberofargumentspassed;howmanyarguments;arguments,howmany



Imported



FALSE
19
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss\fcharset0 Arial;}{\f1\fswiss Arial;}{\f2\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 How Many Arguments \{\f1 keepn\}
\par \pard\fs18 Next: \cf1\strike Calling Variadics\cf2\strike0\{linkID=240\}\cf0 , \f0 Previous:\cf1\strike Receiving Arguments\cf2\strike0\{linkID=1810\}\cf0  ,\f1 Up: \cf1\strike How Variadic\cf2\strike0\{linkID=1120\}\cf0 
\par 
\par 
\par \b\f0 How Many Arguments\b0\f1\fs24 
\par 
\par \fs18 There is no general way for a function to determine the number and type of the optional arguments it was called with. So whoever designs the function typically designs a convention for the caller to specify the number and type of arguments. It is up to you to define an appropriate calling convention for each variadic function, and write all calls accordingly. 
\par 
\par One kind of calling convention is to pass the number of optional arguments as one of the fixed arguments. This convention works provided all of the optional arguments are of the same type. 
\par 
\par A similar alternative is to have one of the required arguments be a bit mask, with a bit for each possible purpose for which an optional argument might be supplied. You would test the bits in a predefined sequence; if the bit is set, fetch the value of the next argument, otherwise use a default value. 
\par 
\par A required argument can be used as a pattern to specify both the number and types of the optional arguments. The format string argument to\f2  printf\f1  is one example of this (see \cf1\strike Formatted Output Functions\strike0\{linkID=960\}\cf0 ). 
\par 
\par Another possibility is to pass an "end marker" value as the last optional argument. For example, for a function that manipulates an arbitrary number of pointer arguments, a null pointer might indicate the end of the argument list. (This assumes that a null pointer isn't otherwise meaningful to the function.) The \f2 execl\f1  function works in just this way; see \cf1\strike Executing a File\strike0\{linkID=700\}\cf0 .
\par 
\par }
1110
PAGE_277
How Unread
ungetc;ungetwc



Imported



FALSE
48
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 How Unread \{keepn\}
\par \pard\fs18 Previous: \cf1\strike Unreading Idea\strike0\{linkID=2350\}\cf0 , Up: \cf1\strike Unreading\strike0\{linkID=2360\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Using \f1 ungetc\f0  To Do Unreading\b0 
\par 
\par The function to unread a character is called \f1 ungetc\f0 , because it reverses the action of \f1 getc\f0 .
\par 
\par -- Function: int \b ungetc\b0  (\i int c, FILE *stream\i0 )
\par 
\par \pard\li500 The \f1 ungetc\f0  function pushes back the character \i c\i0  onto the input stream \i stream\i0 . So the next input from \i stream\i0  will read \i c\i0  before anything else. 
\par 
\par If \i c\i0  is \f1 EOF\f0 , \f1 ungetc\f0  does nothing and just returns\f1  EOF\f0 . This lets you call \f1 ungetc\f0  with the return value of\f1  getc\f0  without needing to check for an error from \f1 getc\f0 . 
\par 
\par The character that you push back doesn't have to be the same as the last character that was actually read from the stream. In fact, it isn't necessary to actually read any characters from the stream before unreading them with \f1 ungetc\f0 ! But that is a strange way to write a program; usually \f1 ungetc\f0  is used only to unread a character that was just read from the same stream. 
\par 
\par The C library supports push\f2 ing\f0  back multiple characters; then reading from the stream retrieves the characters in the reverse order that they were pushed. 
\par 
\par Pushing back characters doesn't alter the file; only the internal buffering for the stream is affected. If a file positioning function (such as \f1 fseek\f0 , \f1 fseeko\f0  or \f1 rewind\f0 ; see \cf1\strike File Positioning\strike0\{linkID=790\}\cf0 ) is called, any pending pushed-back characters are discarded. 
\par 
\par Unreading a character on a stream that is at end of file clears the end-of-file indicator for the stream, because it makes the character of input available. After you read that character, trying to read again will encounter end of file.
\par 
\par \pard -- Function: wint_t \b ungetwc\b0  (\i wint_t wc, FILE *stream\i0 )
\par 
\par \pard\li500 The \f1 ungetwc\f0  function behaves just like \f1 ungetc\f0  just that it pushes back a wide character.
\par 
\par \pard Here is an example showing the use of \f1 getc\f0  and \f1 ungetc\f0  to skip over whitespace characters. When this function reaches a non-whitespace character, it unreads that character to be seen again on the next read operation on the stream.
\par 
\par \f1      #include <stdio.h>
\par      #include <ctype.h>
\par      
\par      void
\par      skip_whitespace (FILE *stream)
\par      \{
\par        int c;
\par        do
\par          /* No need to check for EOF because it is not
\par             isspace, and ungetc ignores EOF.  */
\par          c = getc (stream);
\par        while (isspace (c));
\par        ungetc (c, stream);
\par      \}
\par \f0 
\par }
1120
PAGE_278
How Variadic




Imported



FALSE
23
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern\fcharset0 Courier New;}{\f2\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 How Variadic \{keepn\}
\par \pard\fs18 Next: \cf1\strike Variadic Example\strike0\{linkID=2410\}\cf0 , Previous: \cf1\strike Why Variadic\strike0\{linkID=2440\}\cf0 , Up: \cf1\strike Variadic Functions\strike0\{linkID=2420\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b How Variadic Functions are Defined and Used\b0 
\par 
\par Defining and using a variadic function involves three steps:
\par 
\par \pard\li500\f1\'b7\i\f0  Define\i0  the function as variadic, using an ellipsis (`\f2 ...\f0 ') in the argument list, and using special macros to access the variable arguments. See \cf1\strike Receiving Arguments\strike0\{linkID=1810\}\cf0 . 
\par \f1\'b7\i\f0  Declare\i0  the function as variadic, using a prototype with an ellipsis (`\f2 ...\f0 '), in all the files which call it. See \cf1\strike Variadic Prototypes\strike0\{linkID=2430\}\cf0 . 
\par \f1\'b7\i\f0  Call\i0  the function by writing the fixed arguments followed by the additional variable arguments. See \cf1\strike Calling Variadics\strike0\{linkID=240\}\cf0 .
\par 
\par \f1\'b7\f0  \cf1\strike Variadic Prototypes\strike0\{linkID=2430\}\cf0 : How to make a prototype for a function with variable arguments.
\par \f1\'b7\f0  \cf1\strike Receiving Arguments\strike0\{linkID=1810\}\cf0 : Steps you must follow to access the optional argument values.
\par \f1\'b7\f0  \cf1\strike How Many Arguments\strike0\{linkID=1100\}\cf0 : How to decide whether there are more arguments.
\par \f1\'b7\f0  \cf1\strike Calling Variadics\strike0\{linkID=240\}\cf0 : Things you need to know about calling variable arguments functions.
\par \f1\'b7\f0  \cf1\strike Argument Macros\strike0\{linkID=80\}\cf0 : Detailed specification of the macros for accessing variable arguments.
\par \f1\'b7\f0  \cf1\strike 
\par }
1130
PAGE_279
Hyperbolic Functions
hyperbolicfunctions;sinh;sinhf;sinhl;cosh;coshf;coshl;tanh;tanhf;tanhl;hyperbolicfunctions;csinh;csinhf;csinhl;ccosh;ccoshf;ccoshl;ctanh;ctanhf;ctanhl;inversehyperbolicfunctions;asinh;asinhf;asinhl;acosh;acoshf;acoshl;atanh;atanhf;atanhl;inversecomplexhyperbolicfunctions;casinh;casinhf;casinhl;cacosh;cacoshf;cacoshl;catanh;catanhf;catanhl



Imported



FALSE
111
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Hyperbolic Functions \{keepn\}
\par \pard\fs18 Next: \cf1\strike Special Functions\strike0\{linkID=2070\}\cf0 , Previous: \cf1\strike Exponents and Logarithms\strike0\{linkID=720\}\cf0 , Up: \cf1\strike Mathematics\strike0\{linkID=1410\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Hyperbolic Functions\b0 
\par 
\par The functions in this section are related to the exponential functions; see \cf1\strike Exponents and Logarithms\strike0\{linkID=720\}\cf0 .
\par 
\par -- Function: double \b sinh\b0  (\i double x\i0 )
\par 
\par \pard\li500 -- Function: float \b sinhf\b0  (\i float x\i0 )
\par 
\par \pard\li1000 -- Function: long double \b sinhl\b0  (\i long double x\i0 )
\par 
\par \pard\li1500 These functions return the hyperbolic sine of \i x\i0 , defined mathematically as \f1 (exp (\i\f0 x\i0\f1 ) - exp (-\i\f0 x\i0\f1 )) / 2\f0 . They may signal overflow if \i x\i0  is too large.
\par 
\par \pard -- Function: double \b cosh\b0  (\i double x\i0 )
\par 
\par \pard\li500 -- Function: float \b coshf\b0  (\i float x\i0 )
\par 
\par \pard\li1000 -- Function: long double \b coshl\b0  (\i long double x\i0 )
\par 
\par \pard\li1500 These function return the hyperbolic cosine of \i x\i0 , defined mathematically as \f1 (exp (\i\f0 x\i0\f1 ) + exp (-\i\f0 x\i0\f1 )) / 2\f0 . They may signal overflow if \i x\i0  is too large.
\par 
\par \pard -- Function: double \b tanh\b0  (\i double x\i0 )
\par 
\par \pard\li500 -- Function: float \b tanhf\b0  (\i float x\i0 )
\par 
\par \pard\li1000 -- Function: long double \b tanhl\b0  (\i long double x\i0 )
\par 
\par \pard\li1500 These functions return the hyperbolic tangent of \i x\i0 , defined mathematically as \f1 sinh (\i\f0 x\i0\f1 ) / cosh (\i\f0 x\i0\f1 )\f0 . They may signal overflow if \i x\i0  is too large.
\par 
\par \pard There are counterparts for the hyperbolic functions which take complex arguments.
\par 
\par -- Function: complex double \b csinh\b0  (\i complex double z\i0 )
\par 
\par \pard\li500 -- Function: complex float \b csinhf\b0  (\i complex float z\i0 )
\par 
\par \pard\li1000 -- Function: complex long double \b csinhl\b0  (\i complex long double z\i0 )
\par 
\par \pard\li1500 These functions return the complex hyperbolic sine of \i z\i0 , defined mathematically as \f1 (exp (\i\f0 z\i0\f1 ) - exp (-\i\f0 z\i0\f1 )) / 2\f0 .
\par 
\par \pard -- Function: complex double \b ccosh\b0  (\i complex double z\i0 )
\par 
\par \pard\li500 -- Function: complex float \b ccoshf\b0  (\i complex float z\i0 )
\par 
\par \pard\li1000 -- Function: complex long double \b ccoshl\b0  (\i complex long double z\i0 )
\par 
\par \pard\li1500 These functions return the complex hyperbolic cosine of \i z\i0 , defined mathematically as \f1 (exp (\i\f0 z\i0\f1 ) + exp (-\i\f0 z\i0\f1 )) / 2\f0 .
\par 
\par \pard -- Function: complex double \b ctanh\b0  (\i complex double z\i0 )
\par 
\par \pard\li500 -- Function: complex float \b ctanhf\b0  (\i complex float z\i0 )
\par 
\par \pard\li1000 -- Function: complex long double \b ctanhl\b0  (\i complex long double z\i0 )
\par 
\par \pard\li1500 These functions return the complex hyperbolic tangent of \i z\i0 , defined mathematically as \f1 csinh (\i\f0 z\i0\f1 ) / ccosh (\i\f0 z\i0\f1 )\f0 .
\par 
\par \pard -- Function: double \b asinh\b0  (\i double x\i0 )
\par 
\par \pard\li500 -- Function: float \b asinhf\b0  (\i float x\i0 )
\par 
\par \pard\li1000 -- Function: long double \b asinhl\b0  (\i long double x\i0 )
\par 
\par \pard\li1500 These functions return the inverse hyperbolic sine of \i x\i0 --the value whose hyperbolic sine is \i x\i0 .
\par 
\par \pard -- Function: double \b acosh\b0  (\i double x\i0 )
\par 
\par \pard\li500 -- Function: float \b acoshf\b0  (\i float x\i0 )
\par 
\par \pard\li1000 -- Function: long double \b acoshl\b0  (\i long double x\i0 )
\par 
\par \pard\li1500 These functions return the inverse hyperbolic cosine of \i x\i0 --the value whose hyperbolic cosine is \i x\i0 . If \i x\i0  is less than\f1  1\f0 , \f1 acosh\f0  signals a domain error.
\par 
\par \pard -- Function: double \b atanh\b0  (\i double x\i0 )
\par 
\par \pard\li500 -- Function: float \b atanhf\b0  (\i float x\i0 )
\par 
\par \pard\li1000 -- Function: long double \b atanhl\b0  (\i long double x\i0 )
\par 
\par \pard\li1500 These functions return the inverse hyperbolic tangent of \i x\i0 --the value whose hyperbolic tangent is \i x\i0 . If the absolute value of\i  x\i0  is greater than \f1 1\f0 , \f1 atanh\f0  signals a domain error; if it is equal to 1, \f1 atanh\f0  returns infinity.
\par 
\par \pard -- Function: complex double \b casinh\b0  (\i complex double z\i0 )
\par 
\par \pard\li500 -- Function: complex float \b casinhf\b0  (\i complex float z\i0 )
\par 
\par \pard\li1000 -- Function: complex long double \b casinhl\b0  (\i complex long double z\i0 )
\par 
\par \pard\li1500 These functions return the inverse complex hyperbolic sine of\i  z\i0 --the value whose complex hyperbolic sine is \i z\i0 .
\par 
\par \pard -- Function: complex double \b cacosh\b0  (\i complex double z\i0 )
\par 
\par \pard\li500 -- Function: complex float \b cacoshf\b0  (\i complex float z\i0 )
\par 
\par \pard\li1000 -- Function: complex long double \b cacoshl\b0  (\i complex long double z\i0 )
\par 
\par \pard\li1500 These functions return the inverse complex hyperbolic cosine of\i  z\i0 --the value whose complex hyperbolic cosine is \i z\i0 . Unlike the real-valued functions, there are no restrictions on the value of \i z\i0 .
\par 
\par \pard -- Function: complex double \b catanh\b0  (\i complex double z\i0 )
\par 
\par \pard\li500 -- Function: complex float \b catanhf\b0  (\i complex float z\i0 )
\par 
\par \pard\li1000 -- Function: complex long double \b catanhl\b0  (\i complex long double z\i0 )
\par 
\par \pard\li1500 These functions return the inverse complex hyperbolic tangent of\i  z\i0 --the value whose complex hyperbolic tangent is \i z\i0 . Unlike the real-valued functions, there are no restrictions on the value of\i  z\i0 .
\par 
\par }
1140
PAGE_282
IEEE Floating Point
IEEEfloatingpointrepresentation;floatingpoint,IEEE



Imported



FALSE
40
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 IEEE Floating Point \{keepn\}
\par \pard\fs18 Previous: \cf1\strike Floating Point Parameters\strike0\{linkID=880\}\cf0 , Up: \cf1\strike Floating Type Macros\strike0\{linkID=890\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b IEEE Floating Point\b0 
\par 
\par Here is an example showing how the floating type measurements come out for the most common floating point representation, specified by the\i  IEEE Standard for Binary Floating Point Arithmetic (ANSI/IEEE Std 754-1985)\i0 . Nearly all computers designed since the 1980s use this format. 
\par 
\par The IEEE single-precision float representation uses a base of 2. There is a sign bit, a mantissa with 23 bits plus one hidden bit (so the total precision is 24 base-2 digits), and an 8-bit exponent that can represent values in the range -125 to 128, inclusive. 
\par 
\par So, for an implementation that uses this representation for the\f1  float\f0  data type, appropriate values for the corresponding parameters are:
\par 
\par \f1      FLT_RADIX                             2
\par      FLT_MANT_DIG                         24
\par      FLT_DIG                               6
\par      FLT_MIN_EXP                        -125
\par      FLT_MIN_10_EXP                      -37
\par      FLT_MAX_EXP                         128
\par      FLT_MAX_10_EXP                      +38
\par      FLT_MIN                 1.17549435E-38F
\par      FLT_MAX                 3.40282347E+38F
\par      FLT_EPSILON             1.19209290E-07F
\par \f0 
\par Here are the values for the \f1 double\f0  data type:
\par 
\par \f1      DBL_MANT_DIG                         53
\par      DBL_DIG                              15
\par      DBL_MIN_EXP                       -1021
\par      DBL_MIN_10_EXP                     -307
\par      DBL_MAX_EXP                        1024
\par      DBL_MAX_10_EXP                      308
\par      DBL_MAX         1.7976931348623157E+308
\par      DBL_MIN         2.2250738585072014E-308
\par      DBL_EPSILON     2.2204460492503131E-016
\par \f0 
\par }
1150
PAGE_284
Important Data Types
stddef.h;ptrdiff_t;size_t



Imported



FALSE
27
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Important Data Types \{keepn\}
\par \pard\fs18 Next: \cf1\strike Data Type Measurements\strike0\{linkID=510\}\cf0 , Previous: \cf1\strike Null Pointer Constant\strike0\{linkID=1580\}\cf0 , Up: \cf1\strike Language Features\strike0\{linkID=1310\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Important Data Types\b0 
\par 
\par The result of subtracting two pointers in C is always an integer, but the precise data type varies from C compiler to C compiler. Likewise, the data type of the result of \f1 sizeof\f0  also varies between compilers. ISO defines standard aliases for these two types, so you can refer to them in a portable fashion. They are defined in the header file\f1  stddef.h\f0 .
\par 
\par -- Data Type: \b ptrdiff_t\b0 
\par 
\par \pard\li500 This is the signed integer type of the result of subtracting two pointers. For example, with the declaration \f1 char *p1, *p2;\f0 , the expression \f1 p2 - p1\f0  is of type \f1 ptrdiff_t\f0 . This will probably be one of the standard signed integer types (\f1 short int\f0 , \f1 int\f0  or \f1 long int\f0 ), but might be a nonstandard type that exists only for this purpose.
\par 
\par \pard -- Data Type: \b size_t\b0 
\par 
\par \pard\li500 This is an unsigned integer type used to represent the sizes of objects. The result of the \f1 sizeof\f0  operator is of this type, and functions such as \f1 malloc\f0  (see \cf1\strike Unconstrained Allocation\strike0\{linkID=2340\}\cf0 ) and\f1  memcpy\f0  (see \cf1\strike Copying and Concatenation\strike0\{linkID=460\}\cf0 ) accept arguments of this type to specify object sizes. 
\par 
\par \b Usage Note:\b0  \f1 size_t\f0  is the preferred way to declare any arguments or variables that hold the size of an object.
\par 
\par \pard In th\f2 is\f0  system \f1 size_t\f0  is equivalent to \f1 unsigned int\f0 .
\par 
\par \b Compatibility Note:\b0  Implementations of C before the advent of ISO C generally used \f1 unsigned int\f0  for representing object sizes and \f1 int\f0  for pointer subtraction results. They did not necessarily define either \f1 size_t\f0  or \f1 ptrdiff_t\f0 . Unix systems did define \f1 size_t\f0 , in \f1 sys/types.h\f0 , but the definition was usually a signed type.
\par 
\par }
1160
PAGE_290
Infinity and NaN
infinity;notanumber;NaN;INFINITY;NAN



Imported



FALSE
41
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fnil\fcharset2 Symbol;}{\f3\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Infinity and NaN \{keepn\}
\par \pard\fs18 Next: \cf1\strike Status bit operations\strike0\{linkID=2110\}\cf0 , Previous: \cf1\strike FP Exceptions\strike0\{linkID=1010\}\cf0 , Up: \cf1\strike Floating Point Errors\strike0\{linkID=860\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Infinity and NaN\b0 
\par 
\par IEEE 754 floating point numbers can represent positive or negative infinity, and \i NaN\i0  (not a number). These three values arise from calculations whose result is undefined or cannot be represented accurately. You can also deliberately set a floating-point variable to any of them, which is sometimes useful. Some examples of calculations that produce infinity or NaN:
\par 
\par \pard\qj\f1      1/0 = \f2\'b5\f1 ;
\par \pard      log (0) = -\f2\'b5\f1 ;
\par      sqrt (-1) = NaN
\par \f0 
\par When a calculation produces any of these values, an exception also occurs; see \cf1\strike FP Exceptions\strike0\{linkID=1010\}\cf0 . 
\par 
\par The basic operations and math functions all accept infinity and NaN and produce sensible output. Infinities propagate through calculations as one would expect: for example, 2 + \f2\'b5\f0 ; = \f2\'b5\f0 ;, 4/\f2\'b5\f0 ; = 0, atan \f3 (\f2\'b5\f0 ) = \f2 p\f0 /2. NaN, on the other hand, infects any calculation that involves it. Unless the calculation would produce the same result no matter what real value replaced NaN, the result is NaN. 
\par 
\par In comparison operations, positive infinity is larger than all values except itself and NaN, and negative infinity is smaller than all values except itself and NaN. NaN is \i unordered\i0 : it is not equal to, greater than, or less than anything, \i including itself\i0 . \f1 x == x\f0  is false if the value of \f1 x\f0  is NaN. You can use this to test whether a value is NaN or not, but the recommended way to test for NaN is with the \f1 isnan\f0  function (see \cf1\strike Floating Point Classes\strike0\{linkID=840\}\cf0 ). In addition, \f1 <\f0 , \f1 >\f0 , \f1 <=\f0 , and \f1 >=\f0  will raise an exception when applied to NaNs. 
\par 
\par \f1 math.h\f0  defines macros that allow you to explicitly set a variable to infinity or NaN.
\par 
\par -- Macro: float \b INFINITY\b0 
\par 
\par \pard\li500 An expression representing positive infinity. It is equal to the value produced by mathematical operations like \f1 1.0 / 0.0\f0 .\f1  -INFINITY\f0  represents negative infinity. 
\par 
\par You can test whether a floating-point value is infinite by comparing it to this macro. However, this is not recommended; you should use the\f1  isfinite\f0  macro instead. See \cf1\strike Floating Point Classes\strike0\{linkID=840\}\cf0 . 
\par 
\par This macro was introduced in the ISO C99 standard.
\par 
\par \pard -- Macro: float \b NAN\b0 
\par 
\par \pard\li500 An expression representing a value which is "not a number". This macro is available only on machines that support the "not a number" value--that is to say, on all machines that support IEEE floating point. 
\par 
\par You can use `\f1 #ifdef NAN\f0 ' to test whether the machine supports NaN. 
\par 
\par \pard IEEE 754 also allows for another unusual value: negative zero. This value is produced when you divide a positive number by negative infinity, or when a negative result is smaller than the limits of representation. Negative zero behaves identically to zero in all calculations, unless you explicitly test the sign bit with\f1  signbit\f0  or \f1 copysign\f0 .
\par 
\par }
1170
PAGE_293
Input Conversion Syntax
flagcharacter(scanf);maximumfieldwidth(scanf);typemodifiercharacter(scanf)



Imported



FALSE
30
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Input Conversion Syntax \{keepn\}
\par \pard\fs18 Next: \cf1\strike Table of Input Conversions\strike0\{linkID=2230\}\cf0 , Previous: \cf1\strike Formatted Input Basics\strike0\{linkID=920\}\cf0 , Up: \cf1\strike Formatted Input\strike0\{linkID=940\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Input Conversion Syntax\b0 
\par 
\par A \f1 scanf\f0  template string is a string that contains ordinary multibyte characters interspersed with conversion specifications that start with `\f1 %\f0 '. 
\par 
\par Any whitespace character (as defined by the \f1 isspace\f0  function; see \cf1\strike Classification of Characters\strike0\{linkID=330\}\cf0 ) in the template causes any number of whitespace characters in the input stream to be read and discarded. The whitespace characters that are matched need not be exactly the same whitespace characters that appear in the template string. For example, write `\f1  , \f0 ' in the template to recognize a comma with optional whitespace before and after. 
\par 
\par Other characters in the template string that are not part of conversion specifications must match characters in the input stream exactly; if this is not the case, a matching failure occurs. 
\par 
\par The conversion specifications in a \f1 scanf\f0  template string have the general form:
\par 
\par \f1      % \i flags\i0  \i width\i0  \i type\i0  \i conversion
\par \i0\f0 
\par In more detail, an input conversion specification consists of an initial `\f1 %\f0 ' character followed in sequence by:
\par 
\par \pard\li500\f2\'b7\f0  An optional \i flag character\i0  `\f1 *\f0 ', which says to ignore the text read for this specification. When \f1 scanf\f0  finds a conversion specification that uses this flag, it reads input as directed by the rest of the conversion specification, but it discards this input, does not use a pointer argument, and does not increment the count of successful assignments.
\par \f2\'b7\f0  An optional decimal integer that specifies the \i maximum field width\i0 . Reading of characters from the input stream stops either when this maximum is reached or when a non-matching character is found, whichever happens first. Most conversions discard initial whitespace characters (those that don't are explicitly documented), and these discarded characters don't count towards the maximum field width. String input conversions store a null character to mark the end of the input; the maximum field width does not include this terminator.
\par \f2\'b7\f0  An optional \i type modifier character\i0 . For example, you can specify a type modifier of `\f1 l\f0 ' with integer conversions such as `\f1 %d\f0 ' to specify that the argument is a pointer to a \f1 long int\f0  rather than a pointer to an \f1 int\f0 .
\par \f2\'b7\f0  A character that specifies the conversion to be applied.
\par 
\par \pard The exact options that are permitted and how they are interpreted vary between the different conversion specifiers. See the descriptions of the individual conversions for information about the particular options that they allow. 
\par 
\par }
1180
PAGE_296
Integer Conversions




Imported



FALSE
79
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Integer Conversions \{keepn\}
\par \pard\fs18 Next: \cf1\strike Floating-Point Conversions\strike0\{linkID=900\}\cf0 , Previous: \cf1\strike Table of Output Conversions\strike0\{linkID=2240\}\cf0 , Up: \cf1\strike Formatted Output\strike0\{linkID=970\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Integer Conversions\b0 
\par 
\par This section describes the options for the `\f1 %d\f0 ', `\f1 %i\f0 ', `\f1 %o\f0 ', `\f1 %u\f0 ', `\f1 %x\f0 ', and `\f1 %X\f0 ' conversion specifications. These conversions print integers in various formats. 
\par 
\par The `\f1 %d\f0 ' and `\f1 %i\f0 ' conversion specifications both print an\f1  int\f0  argument as a signed decimal number; while `\f1 %o\f0 ', `\f1 %u\f0 ', and `\f1 %x\f0 ' print the argument as an unsigned octal, decimal, or hexadecimal number (respectively). The `\f1 %X\f0 ' conversion specification is just like `\f1 %x\f0 ' except that it uses the characters `\f1 ABCDEF\f0 ' as digits instead of `\f1 abcdef\f0 '. 
\par 
\par The following flags are meaningful:
\par 
\par `\f1 -\f0 '
\par \pard\li500 Left-justify the result in the field (instead of the normal right-justification). 
\par \pard `\f1 +\f0 '
\par \pard\li500 For the signed `\f1 %d\f0 ' and `\f1 %i\f0 ' conversions, print a plus sign if the value is positive. 
\par \pard `\f1  \f0 '
\par \pard\li500 For the signed `\f1 %d\f0 ' and `\f1 %i\f0 ' conversions, if the result doesn't start with a plus or minus sign, prefix it with a space character instead. Since the `\f1 +\f0 ' flag ensures that the result includes a sign, this flag is ignored if you supply both of them. 
\par \pard `\f1 #\f0 '
\par \pard\li500 For the `\f1 %o\f0 ' conversion, this forces the leading digit to be `\f1 0\f0 ', as if by increasing the precision. For `\f1 %x\f0 ' or `\f1 %X\f0 ', this prefixes a leading `\f1 0x\f0 ' or `\f1 0X\f0 ' (respectively) to the result. This doesn't do anything useful for the `\f1 %d\f0 ', `\f1 %i\f0 ', or `\f1 %u\f0 ' conversions. Using this flag produces output which can be parsed by the \f1 strtoul\f0  function (see \cf1\strike Parsing of Integers\strike0\{linkID=1700\}\cf0 ) and \f1 scanf\f0  with the `\f1 %i\f0 ' conversion (see \cf1\strike Numeric Input Conversions\strike0\{linkID=1590\}\cf0 ). 
\par \pard `\f1 0\f0 '
\par \pard\li500 Pad the field with zeros instead of spaces. The zeros are placed after any indication of sign or base. This flag is ignored if the `\f1 -\f0 ' flag is also specified, or if a precision is specified.
\par 
\par \pard If a precision is supplied, it specifies the minimum number of digits to appear; leading zeros are produced if necessary. If you don't specify a precision, the number is printed with as many digits as it needs. If you convert a value of zero with an explicit precision of zero, then no characters at all are produced. 
\par 
\par Without a type modifier, the corresponding argument is treated as an\f1  int\f0  (for the signed conversions `\f1 %i\f0 ' and `\f1 %d\f0 ') or\f1  unsigned int\f0  (for the unsigned conversions `\f1 %o\f0 ', `\f1 %u\f0 ', `\f1 %x\f0 ', and `\f1 %X\f0 '). Recall that since \f1 printf\f0  and friends are variadic, any \f1 char\f0  and \f1 short\f0  arguments are automatically converted to \f1 int\f0  by the default argument promotions. For arguments of other integer types, you can use these modifiers:
\par 
\par `\f1 hh\f0 '
\par \pard\li500 Specifies that the argument is a \f1 signed char\f0  or \f1 unsigned char\f0 , as appropriate. A \f1 char\f0  argument is converted to an\f1  int\f0  or \f1 unsigned int\f0  by the default argument promotions anyway, but the `\f1 h\f0 ' modifier says to convert it back to a\f1  char\f0  again. 
\par 
\par This modifier was introduced in ISO C99. 
\par \pard `\f1 h\f0 '
\par \pard\li500 Specifies that the argument is a \f1 short int\f0  or \f1 unsigned short int\f0 , as appropriate. A \f1 short\f0  argument is converted to an\f1  int\f0  or \f1 unsigned int\f0  by the default argument promotions anyway, but the `\f1 h\f0 ' modifier says to convert it back to a\f1  short\f0  again. 
\par \pard `\f1 j\f0 '
\par \pard\li500 Specifies that the argument is a \f1 intmax_t\f0  or \f1 uintmax_t\f0 , as appropriate. 
\par 
\par This modifier was introduced in ISO C99. 
\par \pard `\f1 l\f0 '
\par \pard\li500 Specifies that the argument is a \f1 long int\f0  or \f1 unsigned long int\f0 , as appropriate. Two `\f1 l\f0 ' characters is like the `\f1 L\f0 ' modifier, below. 
\par 
\par If used with `\f1 %c\f0 ' or `\f1 %s\f0 ' the corresponding parameter is considered as a wide character or wide character string respectively. This use of `\f1 l\f0 ' was introduced in Amendment 1 to ISO C90. 
\par \pard `\f1 ll\f0 '
\par \pard\li500 Specifies that the argument is a \f1 long long int\f0 .
\par \pard `\f1 t\f0 '
\par \pard\li500 Specifies that the argument is a \f1 ptrdiff_t\f0 . 
\par 
\par This modifier was introduced in ISO C99. 
\par \pard `\f1 z\f0 '
\par `\f1 Z\f0 '
\par \pard\li500 Specifies that the argument is a \f1 size_t\f0 . 
\par 
\par `\f1 z\f0 ' was introduced in ISO C99. `\f1 Z\f0 ' is a GNU extension predating this addition and should not be used in new code.
\par 
\par \pard Here is an example. Using the template string:
\par 
\par \f1      "|%5d|%-5d|%+5d|%+-5d|% 5d|%05d|%5.0d|%5.2d|%d|\\n"
\par \f0 
\par to print numbers using the different options for the `\f1 %d\f0 ' conversion gives results like:
\par 
\par \f1      |    0|0    |   +0|+0   |    0|00000|     |   00|0|
\par      |    1|1    |   +1|+1   |    1|00001|    1|   01|1|
\par      |   -1|-1   |   -1|-1   |   -1|-0001|   -1|  -01|-1|
\par      |100000|100000|+100000|+100000| 100000|100000|100000|100000|100000|
\par \f0 
\par In particular, notice what happens in the last case where the number is too large to fit in the minimum field width specified. 
\par 
\par Here are some more examples showing how unsigned integers print under various format options, using the template string:
\par 
\par \f1      "|%5u|%5o|%5x|%5X|%#5o|%#5x|%#5X|%#10.8x|\\n"
\par \f0 
\par \f1      |    0|    0|    0|    0|    0|    0|    0|  00000000|
\par      |    1|    1|    1|    1|   01|  0x1|  0X1|0x00000001|
\par      |100000|303240|186a0|186A0|0303240|0x186a0|0X186A0|0x000186a0|
\par \f0 
\par }
1190
PAGE_297
Integer Division
integerdivisionfunctions;stdlib.h;div_t;div;ldiv_t;ldiv;lldiv_t;lldiv;imaxdiv_t;imaxdiv



Imported



FALSE
91
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fswiss\fcharset0 Arial;}{\f2\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Integer Division \{keepn\}
\par \pard\fs18 Next: \cf1\strike Floating Point Numbers\strike0\{linkID=870\}\cf0 , Previous: \cf1\strike Integers\strike0\{linkID=1200\}\cf0 , Up: \cf1\strike Arithmetic\strike0\{linkID=100\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Integer Division\b0 
\par 
\par This section describes functions for performing integer division. These functions are redundant when \f1 C\f0 C\f1 386\f0  is used, because in\f1  this compiler\f0  the `\f2 /\f0 ' operator always rounds towards zero. But in other C implementations, `\f2 /\f0 ' may round differently with negative arguments.\f2  div\f0  and \f2 ldiv\f0  are useful because they specify how to round the quotient: towards zero. The remainder has the same sign as the numerator. 
\par 
\par These functions are specified to return a result \i r\i0  such that the value\i  r\i0\f2 .quot*\i\f0 denominator\i0\f2  + \i\f0 r\i0\f2 .rem\f0  equals\i  numerator\i0 . 
\par 
\par To use these facilities, you should include the header file\f2  stdlib.h\f0  in your program.
\par -- Data Type: \b div_t\b0 
\par 
\par \pard\li500 This is a structure type used to hold the result returned by the \f2 div\f0  function. It has the following members:
\par 
\par \f2 int quot\f0 
\par \pard\li1000 The quotient from the division. 
\par \pard\li500\f2 int rem\f0 
\par \pard\li1000 The remainder from the division.
\par 
\par \pard -- Function: div_t \b div\b0  (\i int numerator, int denominator\i0 )
\par 
\par \pard\li500 This function \f2 div\f0  computes the quotient and remainder from the division of \i numerator\i0  by \i denominator\i0 , returning the result in a structure of type \f2 div_t\f0 . 
\par 
\par If the result cannot be represented (as in a division by zero), the behavior is undefined. 
\par 
\par Here is an example, albeit not a very useful one. 
\par 
\par \f2           div_t result;
\par           result = div (20, -6);
\par      
\par \f0 Now \f2 result.quot\f0  is \f2 -3\f0  and \f2 result.rem\f0  is \f2 2\f0 .
\par 
\par \pard -- Data Type: \b ldiv_t\b0 
\par 
\par \pard\li500 This is a structure type used to hold the result returned by the \f2 ldiv\f0  function. It has the following members:
\par 
\par \f2 long int quot\f0 
\par \pard\li1000 The quotient from the division. 
\par \pard\li500\f2 long int rem\f0 
\par \pard\li1000 The remainder from the division.
\par 
\par \pard\li500 (This is identical to \f2 div_t\f0  except that the components are of type \f2 long int\f0  rather than \f2 int\f0 .)
\par 
\par \pard -- Function: ldiv_t \b ldiv\b0  (\i long int numerator, long int denominator\i0 )
\par 
\par \pard\li500 The \f2 ldiv\f0  function is similar to \f2 div\f0 , except that the arguments are of type \f2 long int\f0  and the result is returned as a structure of type \f2 ldiv_t\f0 .
\par 
\par \pard -- Data Type: \b lldiv_t\b0 
\par 
\par \pard\li500 This is a structure type used to hold the result returned by the \f2 lldiv\f0  function. It has the following members:
\par 
\par \f2 long long int quot\f0 
\par \pard\li1000 The quotient from the division. 
\par \pard\li500\f2 long long int rem\f0 
\par \pard\li1000 The remainder from the division.
\par 
\par \pard\li500 (This is identical to \f2 div_t\f0  except that the components are of type \f2 long long int\f0  rather than \f2 int\f0 .)
\par 
\par \pard -- Function: lldiv_t \b lldiv\b0  (\i long long int numerator, long long int denominator\i0 )
\par 
\par \pard\li500 The \f2 lldiv\f0  function is like the \f2 div\f0  function, but the arguments are of type \f2 long long int\f0  and the result is returned as a structure of type \f2 lldiv_t\f0 . 
\par 
\par The \f2 lldiv\f0  function was added in ISO C99.
\par 
\par \pard -- Data Type: \b imaxdiv_t\b0 
\par 
\par \pard\li500 This is a structure type used to hold the result returned by the \f2 imaxdiv\f0  function. It has the following members:
\par 
\par \f2 intmax_t quot\f0 
\par \pard\li1000 The quotient from the division. 
\par \pard\li500\f2 intmax_t rem\f0 
\par \pard\li1000 The remainder from the division.
\par 
\par \pard\li500 (This is identical to \f2 div_t\f0  except that the components are of type \f2 intmax_t\f0  rather than \f2 int\f0 .) 
\par 
\par See \cf1\strike Integers\strike0\{linkID=1200\}\cf0  for a description of the \f2 intmax_t\f0  type.
\par 
\par \pard -- Function: imaxdiv_t \b imaxdiv\b0  (\i intmax_t numerator, intmax_t denominator\i0 )
\par 
\par \pard\li500 The \f2 imaxdiv\f0  function is like the \f2 div\f0  function, but the arguments are of type \f2 intmax_t\f0  and the result is returned as a structure of type \f2 imaxdiv_t\f0 . 
\par 
\par See \cf1\strike Integers\strike0\{linkID=1200\}\cf0  for a description of the \f2 intmax_t\f0  type. 
\par 
\par The \f2 imaxdiv\f0  function was added in ISO C99.
\par 
\par }
1200
PAGE_298
Integers
integer;signedness;stdint.h;maximumpossibleinteger;minimumpossibleinteger



Imported



FALSE
59
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fswiss\fcharset0 Arial;}{\f2\fmodern Courier New;}{\f3\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Integers\{keepn\}
\par \pard\fs18 Next: \cf1\strike Integer Division\strike0\{linkID=1190\}\cf0 , Up: \cf1\strike Arithmetic\strike0\{linkID=100\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Integers\b0 
\par 
\par The C language defines several integer data types: integer, short integer, long integer, \f1 long long  integer, \f0 and character, all in both signed and unsigned varieties. The C integer types were intended to allow code to be portable among machines with different inherent data sizes (word sizes), so each type may have different ranges on different machines. The problem with this is that a program often needs to be written for a particular range of integers, and sometimes must be written for a particular size of storage, regardless of what machine the program runs on. 
\par 
\par To address this problem, the C library contains C type definitions you can use to declare integers that meet your exact needs. Because the C library header files are customized to a specific machine, your program source code doesn't have to be. 
\par 
\par These \f2 typedef\f0 s are in \f2 stdint.h\f0 . If you require that an integer be represented in exactly N bits, use one of the following types, with the obvious mapping to bit size and signedness:
\par 
\par \pard\li500\f3\'b7\f0  int8_t
\par \f3\'b7\f0  int16_t
\par \f3\'b7\f0  int32_t
\par \f3\'b7\f0  int64_t
\par \f3\'b7\f0  uint8_t
\par \f3\'b7\f0  uint16_t
\par \f3\'b7\f0  uint32_t
\par \f3\'b7\f0  uint64_t
\par 
\par \pard If your C compiler and target machine do not allow integers of a certain size, the corresponding above type does not exist. 
\par 
\par If you don't need a specific storage size, but want the smallest data structure with \i at least\i0  N bits, use one of these:
\par 
\par \pard\li500\f3\'b7\f0  int_least8_t
\par \f3\'b7\f0  int_least16_t
\par \f3\'b7\f0  int_least32_t
\par \f3\'b7\f0  int_least64_t
\par \f3\'b7\f0  uint_least8_t
\par \f3\'b7\f0  uint_least16_t
\par \f3\'b7\f0  uint_least32_t
\par \f3\'b7\f0  uint_least64_t
\par 
\par \pard If you don't need a specific storage size, but want the data structure that allows the fastest access while having at least N bits (and among data structures with the same access speed, the smallest one), use one of these:
\par 
\par \pard\li500\f3\'b7\f0  int_fast8_t
\par \f3\'b7\f0  int_fast16_t
\par \f3\'b7\f0  int_fast32_t
\par \f3\'b7\f0  int_fast64_t
\par \f3\'b7\f0  uint_fast8_t
\par \f3\'b7\f0  uint_fast16_t
\par \f3\'b7\f0  uint_fast32_t
\par \f3\'b7\f0  uint_fast64_t
\par 
\par \pard If you want an integer with the widest range possible on the platform on which it is being used, use one of the following. If you use these, you should write code that takes into account the variable size and range of the integer.
\par 
\par \pard\li500\f3\'b7\f0  intmax_t
\par \f3\'b7\f0  uintmax_t
\par 
\par \pard The C library also provides macros that tell you the maximum and minimum possible values for each integer data type. The macro names follow these examples: \f2 INT32_MAX\f0 , \f2 UINT8_MAX\f0 ,\f2  INT_FAST32_MIN\f0 , \f2 INT_LEAST64_MIN\f0 , \f2 UINTMAX_MAX\f0 ,\f2  INTMAX_MAX\f0 , \f2 INTMAX_MIN\f0 . Note that there are no macros for unsigned integer minima. These are always zero. There are similar macros for use with C's built in integer types which should come with your C compiler. These are described in \cf1\strike Data Type Measurements\strike0\{linkID=510\}\cf0 . 
\par 
\par Don't forget you can use the C \f2 sizeof\f0  function with any of these data types to get the number of bytes of storage each uses.
\par 
\par }
1210
PAGE_305
Introduction
library



Imported



FALSE
18
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fswiss\fcharset0 Arial;}{\f2\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Introduction\{keepn\}
\par \pard\fs18 Next: \cf1\strike Error Reporting\strike0\{linkID=690\}\cf0 , Previous: \cf1\strike Top\strike0\{linkID=10\}\cf0 , Up: \cf1\strike Top\strike0\{linkID=10\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Introduction\b0 
\par 
\par The C language provides no built-in facilities for performing such common operations as input/output, memory management, string manipulation, and the like. Instead, these facilities are defined in a standard \i library\i0 , which you compile and link with your programs. The C library, described in this document, defines all of the library functions that are specified by the ISO C standard\f1 .
\par 
\par \f0 The purpose of this manual is to tell you how to use the facilities of the \f1 C\f0  library. We have mentioned which features belong to which standards to help you identify things that are potentially non-portable to other systems. But the emphasis in this manual is not on strict portability.
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Getting Started\strike0\{linkID=1060\}\cf0 : What this manual is for and how to use it.
\par \f2\'b7\f0  \cf1\strike Roadmap to the Manual\strike0\{linkID=1880\}\cf0 : Overview of the remaining chapters in this manual.
\par 
\par }
1220
PAGE_306
Inverse Trig Functions
inversetrigonometricfunctions;asin;asinf;asinl;acos;acosf;acosl;atan;atanf;atanl;atan;atan2f;atan2l;inversecomplextrigonometricfunctions;casin;casinf;casinl;cacos;cacosf;cacosl;catan;catanf;catanl



Imported



FALSE
83
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Inverse Trig Functions \{keepn\}
\par \pard\fs18 Next: \cf1\strike Exponents and Logarithms\strike0\{linkID=720\}\cf0 , Previous: \cf1\strike Trig Functions\strike0\{linkID=2320\}\cf0 , Up: \cf1\strike Mathematics\strike0\{linkID=1410\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Inverse Trigonometric Functions\b0 
\par 
\par These are the usual arc sine, arc cosine and arc tangent functions, which are the inverses of the sine, cosine and tangent functions respectively.
\par 
\par -- Function: double \b asin\b0  (\i double x\i0 )
\par 
\par \pard\li500 -- Function: float \b asinf\b0  (\i float x\i0 )
\par 
\par \pard\li1000 -- Function: long double \b asinl\b0  (\i long double x\i0 )
\par 
\par \pard\li1500 These functions compute the arc sine of \i x\i0 --that is, the value whose sine is \i x\i0 . The value is in units of radians. Mathematically, there are infinitely many such values; the one actually returned is the one between \f1 -pi/2\f0  and \f1 pi/2\f0  (inclusive). 
\par 
\par The arc sine function is defined mathematically only over the domain \f1 -1\f0  to \f1 1\f0 . If \i x\i0  is outside the domain, \f1 asin\f0  signals a domain error.
\par 
\par \pard -- Function: double \b acos\b0  (\i double x\i0 )
\par 
\par \pard\li500 -- Function: float \b acosf\b0  (\i float x\i0 )
\par 
\par \pard\li1000 -- Function: long double \b acosl\b0  (\i long double x\i0 )
\par 
\par \pard\li1500 These functions compute the arc cosine of \i x\i0 --that is, the value whose cosine is \i x\i0 . The value is in units of radians. Mathematically, there are infinitely many such values; the one actually returned is the one between \f1 0\f0  and \f1 pi\f0  (inclusive). 
\par 
\par The arc cosine function is defined mathematically only over the domain \f1 -1\f0  to \f1 1\f0 . If \i x\i0  is outside the domain, \f1 acos\f0  signals a domain error.
\par 
\par \pard -- Function: double \b atan\b0  (\i double x\i0 )
\par 
\par \pard\li500 -- Function: float \b atanf\b0  (\i float x\i0 )
\par 
\par \pard\li1000 -- Function: long double \b atanl\b0  (\i long double x\i0 )
\par 
\par \pard\li1500 These functions compute the arc tangent of \i x\i0 --that is, the value whose tangent is \i x\i0 . The value is in units of radians. Mathematically, there are infinitely many such values; the one actually returned is the one between \f1 -pi/2\f0  and \f1 pi/2\f0  (inclusive).
\par 
\par \pard -- Function: double \b atan2\b0  (\i double y, double x\i0 )
\par 
\par \pard\li500 -- Function: float \b atan2f\b0  (\i float y, float x\i0 )
\par 
\par \pard\li1000 -- Function: long double \b atan2l\b0  (\i long double y, long double x\i0 )
\par 
\par \pard\li1500 This function computes the arc tangent of \i y\i0 /\i x\i0 , but the signs of both arguments are used to determine the quadrant of the result, and\i  x\i0  is permitted to be zero. The return value is given in radians and is in the range \f1 -pi\f0  to \f1 pi\f0 , inclusive. 
\par 
\par If \i x\i0  and \i y\i0  are coordinates of a point in the plane,\f1  atan2\f0  returns the signed angle between the line from the origin to that point and the x-axis. Thus, \f1 atan2\f0  is useful for converting Cartesian coordinates to polar coordinates. (To compute the radial coordinate, use \f1 hypot\f0 ; see \cf1\strike Exponents and Logarithms\strike0\{linkID=720\}\cf0 .)
\par 
\par If both \i x\i0  and \i y\i0  are zero, \f1 atan2\f0  returns zero.
\par 
\par \pard ISO C99 defines complex versions of the inverse trig functions.
\par 
\par -- Function: complex double \b casin\b0  (\i complex double z\i0 )
\par 
\par \pard\li500 -- Function: complex float \b casinf\b0  (\i complex float z\i0 )
\par 
\par \pard\li1000 -- Function: complex long double \b casinl\b0  (\i complex long double z\i0 )
\par 
\par \pard\li1500 These functions compute the complex arc sine of \i z\i0 --that is, the value whose sine is \i z\i0 . The value returned is in radians. 
\par 
\par Unlike the real-valued functions, \f1 casin\f0  is defined for all values of \i z\i0 .
\par 
\par \pard -- Function: complex double \b cacos\b0  (\i complex double z\i0 )
\par 
\par \pard\li500 -- Function: complex float \b cacosf\b0  (\i complex float z\i0 )
\par 
\par \pard\li1000 -- Function: complex long double \b cacosl\b0  (\i complex long double z\i0 )
\par 
\par \pard\li1500 These functions compute the complex arc cosine of \i z\i0 --that is, the value whose cosine is \i z\i0 . The value returned is in radians. 
\par 
\par Unlike the real-valued functions, \f1 cacos\f0  is defined for all values of \i z\i0 .
\par 
\par \pard -- Function: complex double \b catan\b0  (\i complex double z\i0 )
\par 
\par \pard\li500 -- Function: complex float \b catanf\b0  (\i complex float z\i0 )
\par 
\par \pard\li1000 -- Function: complex long double \b catanl\b0  (\i complex long double z\i0 )
\par 
\par \pard\li1500 These functions compute the complex arc tangent of \i z\i0 --that is, the value whose tangent is \i z\i0 . The value is in units of radians.
\par 
\par }
1230
PAGE_309
ISO C
ISOC;gcc



Imported



FALSE
18
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern\fcharset0 Courier New;}{\f2\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 ISO C \{keepn\}
\par \pard\fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b ISO C\b0 
\par 
\par The C library is compatible with the C standard adopted by the American National Standards Institute (ANSI):\i  American National Standard X3.159-1989--"ANSI C"\i0  and later by the International Standardization Organization (ISO):\i  ISO/IEC 9899:1990, "Programming languages--C"\i0 . We here refer to the standard as ISO C since this is the more general standard in respect of ratification. The header files and library facilities that make up the library are a superset of those specified by the ISO C standard. 
\par 
\par If you are concerned about strict adherence to the ISO C standard, you should use the `\f1 +A\f0 ' option when you compile your programs with the\f2  \f0 C compiler. This tells the compiler to define \i only\i0  ISO standard features from the library header files, unless you explicitly ask for additional features. 
\par 
\par Being able to restrict the library to include only ISO C features is important because ISO C puts limitations on what names can be defined by the library implementation, and \f2 some\f0  extensions don't fit these limitations. See \cf1\strike Reserved Names\strike0\{linkID=1860\}\cf0 , for more information about these restrictions. 
\par 
\par This manual does not attempt to give you complete details on the differences between ISO C and older dialects. It gives advice on how to write programs to work portably under multiple C dialects, but does not aim for completeness.
\par 
\par }
1240
PAGE_310
ISO Random
stdlib.h;RAND_MAX;rand;srand;rand_r



Imported



FALSE
30
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 ISO Random \{keepn\}
\par \pard\fs18 Up: \cf1\strike Pseudo-Random Numbers\strike0\{linkID=1780\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b ISO C Random Number Functions\b0 
\par 
\par This section describes the random number functions that are part of the ISO C standard. 
\par 
\par To use these facilities, you should include the header file\f1  stdlib.h\f0  in your program.
\par 
\par -- Macro: int \b RAND_MAX\b0 
\par 
\par \pard\li500 The value of this macro is an integer constant representing the largest value the \f1 rand\f0  function can return. In t\f2 his \f0 librar\f2 y\f0 , it\f2  is 32767.  In other libraries it may be higher.\f0 
\par 
\par \pard -- Function: int \b rand\b0  (\i void\i0 )
\par 
\par \pard\li500 The \f1 rand\f0  function returns the next pseudo-random number in the series. The value ranges from \f1 0\f0  to \f1 RAND_MAX\f0 .
\par 
\par \pard -- Function: void \b srand\b0  (\i unsigned int seed\i0 )
\par 
\par \pard\li500 This function establishes \i seed\i0  as the seed for a new series of pseudo-random numbers. If you call \f1 rand\f0  before a seed has been established with \f1 srand\f0 , it uses the value \f1 1\f0  as a default seed. 
\par 
\par To produce a different pseudo-random series each time your program is run, do \f1 srand (time (0))\f0 .
\par 
\par 
\par }
1250
PAGE_311
I/O Concepts
openingafile



Imported



FALSE
18
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 I/O Concepts \{keepn\}
\par \pard\fs18 Next: \cf1\strike File Names\strike0\{linkID=760\}\cf0 , Up: \cf1\strike I/O Overview\strike0\{linkID=1270\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Input/Output Concepts\b0 
\par 
\par Before you can read or write the contents of a file, you must establish a connection or communications channel to the file. This process is called \i opening\i0  the file. You can open a file for reading, writing, or both. The connection to an open file is represented either as a stream or as a file descriptor. You pass this as an argument to the functions that do the actual read or write operations, to tell them which file to operate on. Certain functions expect streams, and others are designed to operate on file descriptors. 
\par 
\par When you have finished reading to or writing from the file, you can terminate the connection by \i closing\i0  the file. Once you have closed a stream or file descriptor, you cannot do any more input or output operations on it.
\par 
\par \pard\li500\f1\'b7\f0  \cf1\strike Streams and File Descriptors\strike0\{linkID=2130\}\cf0 : The GNU Library provides two ways to access the contents of files.
\par \f1\'b7\f0  \cf1\strike File Position\strike0\{linkID=780\}\cf0 : The number of bytes from the beginning of the file.
\par 
\par }
1260
PAGE_312
I/O on Streams




Imported



FALSE
31
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern\fcharset0 Courier New;}{\f2\fswiss\fcharset0 Arial;}{\f3\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 I/O on Streams \{keepn\}
\par \pard\fs18 Next: \cf1\strike Low-Level I/O\strike0\{linkID=1360\}\cf0 , Previous: \cf1\strike I/O Overview\strike0\{linkID=1270\}\cf0 , Up: \cf1\strike Top\strike0\{linkID=10\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Input/Output on Streams\b0 
\par 
\par This chapter describes the functions for creating streams and performing input and output operations on them. As discussed in \cf1\strike I/O Overview\strike0\{linkID=1270\}\cf0 , a stream is a fairly abstract, high-level concept representing a communications channel to a file, device, or process.
\par 
\par \pard\li500\f1\'b7\f0  \cf1\strike Streams\strike0\{linkID=2150\}\cf0 : About the data type representing a stream.
\par \f1\'b7\f0  \cf1\strike Standard Streams\strike0\{linkID=2100\}\cf0 : Streams to the standard input and output devices are created for you.
\par \f1\'b7\f0  \cf1\strike Opening Streams\strike0\{linkID=1610\}\cf0 : How to create a stream to talk to a file.
\par \f1\'b7\f0  \cf1\strike Closing Streams\strike0\{linkID=360\}\cf0 : Close a stream when you are finished with it.
\par \f1\'b7\f0  \cf1\strike Streams and I18N\strike0\{linkID=2140\}\cf0 : Streams in internationalized applications.
\par \f1\'b7\f0  \cf1\strike Simple Output\strike0\{linkID=2050\}\cf0 : Unformatted output by characters and lines.
\par \f1\'b7\f0  \cf1\strike Character Input\strike0\{linkID=280\}\cf0 : Unformatted input by characters and words.
\par \f1\'b7\f0  \cf1\strike Unreading\strike0\{linkID=2360\}\cf0 : Peeking ahead/pushing back input just read\f2 .\f0 
\par \f1\'b7\f0  \cf1\strike Line Input\cf2\strike0\{linkID=1316\}\cf0 : \f2 Reading lines of data.\f0 
\par \f1\'b7\f0  \cf1\strike Block Input/Output\strike0\{linkID=200\}\cf0 : Input and output operations on blocks of data.
\par \f1\'b7\f0  \cf1\strike Formatted Output\strike0\{linkID=970\}\cf0 : \f3 printf\f0  and related functions.
\par \f1\'b7\f0  \cf1\strike Formatted Input\strike0\{linkID=940\}\cf0 : \f3 scanf\f0  and related functions.
\par \f1\'b7\f0  \cf1\strike EOF and Errors\strike0\{linkID=650\}\cf0 : How you can tell if an I/O error happens.
\par \f1\'b7\f0  \cf1\strike Error Recovery\strike0\{linkID=680\}\cf0 : What you can do about errors.
\par \f1\'b7\f0  \cf1\strike Binary Streams\strike0\{linkID=190\}\cf0 : Some systems distinguish between text files and binary files.
\par \f1\'b7\f0  \cf1\strike File Positioning\strike0\{linkID=790\}\cf0 : About random-access streams.
\par \f1\'b7\f0  \cf1\strike Portable Positioning\strike0\{linkID=1730\}\cf0 : Random access on peculiar ISO C systems.
\par \f1\'b7\f0  \cf1\strike Stream Buffering\strike0\{linkID=2120\}\cf0 : How to control buffering of streams.\cf1\strike 
\par }
1270
PAGE_313
I/O Overview




Imported



FALSE
21
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 I/O Overview \{keepn\}
\par \pard\fs18 Next: \cf1\strike I/O on Streams\strike0\{linkID=1260\}\cf0 , Up: \cf1\strike Top\strike0\{linkID=10\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Input/Output Overview\b0 
\par 
\par Most programs need to do either input (reading data) or output (writing data), or most frequently both, in order to do anything useful. The C library provides such a large selection of input and output functions that the hardest part is often deciding which function is most appropriate! 
\par 
\par This chapter introduces concepts and terminology relating to input and output. Other chapters relating to the I/O facilities are:
\par 
\par \pard\li500\f1\'b7\f0  \cf1\strike I/O on Streams\strike0\{linkID=1260\}\cf0 , which covers the high-level functions that operate on streams, including formatted input and output. 
\par \f1\'b7\f0  \cf1\strike Low-Level I/O\strike0\{linkID=1360\}\cf0 , which covers the basic I/O and control functions on file descriptors. 
\par \f1\'b7\f0  \cf1\strike File System Interface\strike0\{linkID=820\}\cf0 , which covers functions for operating on directories and for manipulating file attributes such as access modes and ownership. 
\par \f1\'b7\f0  \cf1\strike I/O Concepts\strike0\{linkID=1250\}\cf0 : Some basic information and terminology.
\par \f1\'b7\f0  \cf1\strike File Names\strike0\{linkID=760\}\cf0 : How to refer to a file.
\par 
\par }
1280
PAGE_314
I/O Primitives
unistd.h;ssize_t;readingfromafiledescriptor;read;end-of-file,onafiledescriptor;pread;pread;writingtoafiledescriptor;write;pwrite;pwrite



Imported



FALSE
70
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 I/O Primitives \{keepn\}
\par \pard\fs18 Next: \cf1\strike File Position Primitive\strike0\{linkID=770\}\cf0 , Previous: \cf1\strike Opening and Closing Files\strike0\{linkID=1600\}\cf0 , Up: \cf1\strike Low-Level I/O\strike0\{linkID=1360\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Input and Output Primitives\b0 
\par 
\par This section describes the functions for performing primitive input and output operations on file descriptors: \f1 read\f0 , \f1 write\f0 , and\f1  lseek\f0 . These functions are declared in the header file\f1  unistd.h\f0 .
\par 
\par -- Function: \f2 int\f0  \b read\b0  (\i int filedes, void *buffer, size_t size\i0 )
\par 
\par \pard\li500 The \f1 read\f0  function reads up to \i size\i0  bytes from the file with descriptor \i filedes\i0 , storing the results in the \i buffer\i0 . (This is not necessarily a character string, and no terminating null character is added.) 
\par 
\par The return value is the number of bytes actually read. This might be less than \i size\i0 ; for example, if there aren't that many bytes left in the file or if there aren't that many bytes immediately available. The exact behavior depends on what kind of file it is. Note that reading less than \i size\i0  bytes is not an error. 
\par 
\par A value of zero indicates end-of-file (except if the value of the\i  size\i0  argument is also zero). This is not considered an error. If you keep calling \f1 read\f0  while at end-of-file, it will keep returning zero and doing nothing else. 
\par 
\par If \f1 read\f0  returns at least one character, there is no way you can tell whether end-of-file was reached. But if you did reach the end, the next read will return zero. 
\par 
\par In case of an error, \f1 read\f0  returns -1. The following\f1  errno\f0  error conditions are defined for this function:
\par 
\par \f1 EAGAIN\f0 
\par \pard\li1000 Normally, when no input is immediately available, \f1 read\f0  waits for some input. But if the \f1 O_NONBLOCK\f0  flag is set for the file (see \cf1\strike File Status Flags\strike0\{linkID=810\}\cf0 ), \f1 read\f0  returns immediately without reading any data, and reports this error. 
\par 
\par \b Compatibility Note:\b0  Most versions of BSD Unix use a different error code for this: \f1 EWOULDBLOCK\f0 . In the GNU library,\f1  EWOULDBLOCK\f0  is an alias for \f1 EAGAIN\f0 , so it doesn't matter which name you use. 
\par 
\par On some systems, reading a large amount of data from a character special file can also fail with \f1 EAGAIN\f0  if the kernel cannot find enough physical memory to lock down the user's pages. This is limited to devices that transfer with direct memory access into the user's memory, which means it does not include terminals, since they always use separate buffers inside the kernel. This problem never happens in the GNU system. 
\par 
\par Any condition that could result in \f1 EAGAIN\f0  can instead result in a successful \f1 read\f0  which returns fewer bytes than requested. Calling \f1 read\f0  again immediately would result in \f1 EAGAIN\f0 . 
\par \pard\li500\f1 EBADF\f0 
\par \pard\li1000 The \i filedes\i0  argument is not a valid file descriptor, or is not open for reading. 
\par \pard\li500\f1 EINTR\f0 
\par \pard\li1000\f1 read\f0  was interrupted by a signal while it was waiting for input. See \cf1\strike Interrupted Primitives\strike0\{link=PAGE_304\}\cf0 . A signal will not necessary cause\f1  read\f0  to return \f1 EINTR\f0 ; it may instead result in a successful \f1 read\f0  which returns fewer bytes than requested. 
\par \pard\li500\f1 EIO\f0 
\par \pard\li1000 For many devices, and for disk files, this error code indicates a hardware error. 
\par 
\par \f1 EIO\f0  also occurs when a background process tries to read from the controlling terminal, and the normal action of stopping the process by sending it a \f1 SIGTTIN\f0  signal isn't working. This might happen if the signal is being blocked or ignored, or because the process group is orphaned. See \cf1\strike Job Control\strike0\{link=PAGE_318\}\cf0 , for more information about job control, and \cf1\strike Signal Handling\strike0\{linkID=2010\}\cf0 , for information about signals.
\par 
\par \pard -- Function: int \b write\b0  (\i int filedes, const void *buffer, size_t size\i0 )
\par 
\par \pard\li500 The \f1 write\f0  function writes up to \i size\i0  bytes from\i  buffer\i0  to the file with descriptor \i filedes\i0 . The data in\i  buffer\i0  is not necessarily a character string and a null character is output like any other character. 
\par 
\par The return value is the number of bytes actually written. This may be\i  size\i0 , but can always be smaller. Your program should always call\f1  write\f0  in a loop, iterating until all the data is written. 
\par 
\par Once \f1 write\f0  returns, the data is enqueued to be written and can be read back right away, but it is not necessarily written out to permanent storage immediately. You can use \f1 fsync\f0  when you need to be sure your data has been permanently stored before continuing. (It is more efficient for the system to batch up consecutive writes and do them all at once when convenient. Normally they will always be written to disk within a minute or less.) Modern systems provide another function\f1  fdatasync\f0  which guarantees integrity only for the file data and is therefore faster. You can use the \f1 O_FSYNC\f0  open mode to make \f1 write\f0  always store the data to disk before returning; see \cf1\strike Operating Modes\strike0\{linkID=1630\}\cf0 . 
\par 
\par In the case of an error, \f1 write\f0  returns -1. The following\f1  errno\f0  error conditions are defined for this function:
\par 
\par \f1 EAGAIN\f0 
\par \pard\li1000 Normally, \f1 write\f0  blocks until the write operation is complete. But if the \f1 O_NONBLOCK\f0  flag is set for the file (see \cf1\strike Control Operations\strike0\{link=PAGE_120\}\cf0 ), it returns immediately without writing any data and reports this error. An example of a situation that might cause the process to block on output is writing to a terminal device that supports flow control, where output has been suspended by receipt of a STOP character. 
\par 
\par \b Compatibility Note:\b0  Most versions of BSD Unix use a different error code for this: \f1 EWOULDBLOCK\f0 . In the GNU library,\f1  EWOULDBLOCK\f0  is an alias for \f1 EAGAIN\f0 , so it doesn't matter which name you use. 
\par 
\par On some systems, writing a large amount of data from a character special file can also fail with \f1 EAGAIN\f0  if the kernel cannot find enough physical memory to lock down the user's pages. This is limited to devices that transfer with direct memory access into the user's memory, which means it does not include terminals, since they always use separate buffers inside the kernel. This problem does not arise in the GNU system. 
\par \pard\li500\f1 EBADF\f0 
\par \pard\li1000 The \i filedes\i0  argument is not a valid file descriptor, or is not open for writing. 
\par \pard\li500\f1 EFBIG\f0 
\par \pard\li1000 The size of the file would become larger than the implementation can support. 
\par \pard\li500\f1 EINTR\f0 
\par \pard\li1000 The \f1 write\f0  operation was interrupted by a signal while it was blocked waiting for completion. A signal will not necessarily cause\f1  write\f0  to return \f1 EINTR\f0 ; it may instead result in a successful \f1 write\f0  which writes fewer bytes than requested. See \cf1\strike Interrupted Primitives\strike0\{link=PAGE_304\}\cf0 . 
\par \pard\li500\f1 EIO\f0 
\par \pard\li1000 For many devices, and for disk files, this error code indicates a hardware error. 
\par \pard\li500\f1 ENOSPC\f0 
\par \pard\li1000 The device containing the file is full. 
\par \pard\li500\f1 EPIPE\f0 
\par \pard\li1000 This error is returned when you try to write to a pipe or FIFO that isn't open for reading by any process. When this happens, a \f1 SIGPIPE\f0  signal is also sent to the process; see \cf1\strike Signal Handling\strike0\{linkID=2010\}\cf0 .
\par }
1290
PAGE_319
Keeping the state
stateful;mbstate_t;shiftstate;wchar.h;mbsinit;wchar.h



Imported



FALSE
59
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Keeping the state \{keepn\}
\par \pard\fs18 Next: \cf1\strike Converting a Character\strike0\{linkID=440\}\cf0 , Previous: \cf1\strike Selecting the Conversion\strike0\{linkID=1950\}\cf0 , Up: \cf1\strike Restartable multibyte conversion\strike0\{linkID=1870\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Representing the state of the conversion\b0 
\par 
\par In the introduction of this chapter it was said that certain character sets use a \i stateful\i0  encoding. That is, the encoded values depend in some way on the previous bytes in the text. 
\par 
\par Since the conversion functions allow converting a text in more than one step we must have a way to pass this information from one call of the functions to another.
\par 
\par -- Data type: \b mbstate_t\b0 
\par 
\par \pard\li500 A variable of type \f1 mbstate_t\f0  can contain all the information about the \i shift state\i0  needed from one call to a conversion function to another. 
\par 
\par \f1 mbstate_t\f0  is defined in \f1 wchar.h\f0 . It was introduced in Amendment 1 to ISO C90.
\par 
\par \pard To use objects of type \f1 mbstate_t\f0  the programmer has to define such objects (normally as local variables on the stack) and pass a pointer to the object to the conversion functions. This way the conversion function can update the object if the current multibyte character set is stateful. 
\par 
\par There is no specific function or initializer to put the state object in any specific state. The rules are that the object should always represent the initial state before the first use, and this is achieved by clearing the whole variable with code such as follows:
\par 
\par \f1      \{
\par        mbstate_t state;
\par        memset (&state, '\\0', sizeof (state));
\par        /* from now on \i state\i0  can be used.  */
\par        ...
\par      \}
\par \f0 
\par When using the conversion functions to generate output it is often necessary to test whether the current state corresponds to the initial state. This is necessary, for example, to decide whether to emit escape sequences to set the state to the initial state at certain sequence points. Communication protocols often require this.
\par 
\par -- Function: int \b mbsinit\b0  (\i const mbstate_t *ps\i0 )
\par 
\par \pard\li500 The \f1 mbsinit\f0  function determines whether the state object pointed to by \i ps\i0  is in the initial state. If \i ps\i0  is a null pointer or the object is in the initial state the return value is nonzero. Otherwise it is zero. 
\par 
\par \f1 mbsinit\f0  was introduced in Amendment 1 to ISO C90 and is declared in \f1 wchar.h\f0 .
\par 
\par \pard Code using \f1 mbsinit\f0  often looks similar to this:
\par 
\par \f1      \{
\par        mbstate_t state;
\par        memset (&state, '\\0', sizeof (state));
\par        /* Use \i state\i0 .  */
\par        ...
\par        if (! mbsinit (&state))
\par          \{
\par            /* Emit code to return to initial state.  */
\par            const wchar_t empty[] = L"";
\par            const wchar_t *srcp = empty;
\par            wcsrtombs (outbuf, &srcp, outbuflen, &state);
\par          \}
\par        ...
\par      \}
\par \f0 
\par The code to emit the escape sequence to get back to the initial state is interesting. The \f1 wcsrtombs\f0  function can be used to determine the necessary output code (see \cf1\strike Converting Strings\strike0\{linkID=450\}\cf0 ). Please note that on GNU systems it is not necessary to perform this extra action for the conversion from multibyte text to wide character text since the wide character encoding is not stateful. But there is nothing mentioned in any standard that prohibits making \f1 wchar_t\f0  using a stateful encoding.
\par 
\par }
1300
PAGE_321
Kinds of Signals




Imported



FALSE
22
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern\fcharset0 Courier New;}{\f2\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Kinds of Signals \{keepn\}
\par \pard\fs18 Next: \cf1\strike Signal Generation\strike0\{linkID=2000\}\cf0 , Up: \cf1\strike Concepts of Signals\strike0\{linkID=400\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Some Kinds of Signals\b0 
\par 
\par A signal reports the occurrence of an exceptional event. These are some of the events that can cause (or \i generate\i0 , or \i raise\i0 ) a signal:
\par 
\par \pard\li500\f1\'b7\f0  A program error such as dividing by zero or issuing an address outside the valid range. 
\par \f1\'b7\f0  A user request to interrupt or terminate the program. Most environments are set up to let a user suspend the program by typing \f2 C-z\f0 , or terminate it with \f2 C-c\f0 . Whatever key sequence is used, the operating system sends the proper signal to interrupt the process. 
\par \f1\'b7\f0  The termination of a child process. 
\par \f1\'b7\f0  Expiration of a timer or alarm. 
\par \f1\'b7\f0  A call to \f2 kill\f0  or \f2 raise\f0  by the same process. 
\par \f1\'b7\f0  A call to \f2 kill\f0  from another process. Signals are a limited but useful form of interprocess communication. 
\par 
\par \pard Each of these kinds of events (excepting explicit calls to \f2 kill\f0  and \f2 raise\f0 ) generates its own particular kind of signal. The various kinds of signals are listed and described in detail in \cf1\strike Standard Signals\strike0\{linkID=2090\}\cf0 .
\par 
\par }
1310
PAGE_322
Language Features




Imported



FALSE
19
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern\fcharset0 Courier New;}{\f2\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Language Features \{keepn\}
\par \pard\fs18 Next: \cf1\strike Free Manuals\cf2\strike0\{linkID=1020\}\cf0 , Up: \cf1\strike Top\strike0\{linkID=10\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Appendix A C Language Facilities in the Library\b0 
\par 
\par Some of the facilities implemented by the C library really should be thought of as parts of the C language itself. These facilities ought to be documented in the C Language Manual, not in the library manual; but since we don't have the language manual yet, and documentation for these features has been written, we are publishing it here.
\par 
\par \pard\li500\f1\'b7\f0  \cf1\strike Consistency Checking\strike0\{linkID=410\}\cf0 : Using \f2 assert\f0  to abort if something ``impossible'' happens.
\par \f1\'b7\f0  \cf1\strike Variadic Functions\strike0\{linkID=2420\}\cf0 : Defining functions with varying numbers of args.
\par \f1\'b7\f0  \cf1\strike Null Pointer Constant\strike0\{linkID=1580\}\cf0 : The macro \f2 NULL\f0 .
\par \f1\'b7\f0  \cf1\strike Important Data Types\strike0\{linkID=1150\}\cf0 : Data types for object sizes.
\par \f1\'b7\f0  \cf1\strike Data Type Measurements\strike0\{linkID=510\}\cf0 : Parameters of data type representations.
\par 
\par }
1316
Scribble1316
Line Input
getline;getdelim;fgets;fgetws;fgets_unlocked;fgetws_unlocked;gets



Writing



FALSE
37
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fswiss\fcharset0 Arial;}{\f3\fnil\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;\red0\green0\blue255;}
\viewkind4\uc1\pard\qc\f0\fs24 Line Input \{keepn\}
\par \pard\fs18 Next: \cf1\strike Unreading\strike0\{linkID=2370\}\cf0 , Previous: \cf1\strike Character Input\strike0\{linkID=280\}\cf0 , Up: \cf1\strike I/O on Streams\strike0\{linkID=1260\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Line-Oriented Input\b0 
\par 
\par Since many programs interpret input on the basis of lines, it is convenient to have functions to read a line of text from a stream. 
\par 
\par \pard\li500\f1      
\par \pard\f0 -- Function: char * \b fgets\b0  (\i char *s, int count, FILE *stream\i0 )
\par 
\par \pard\li500 The \f1 fgets\f0  function reads characters from the stream \i stream\i0  up to and including a newline character and stores them in the string\i  s\i0 , adding a null character to mark the end of the string. You must supply \i count\i0  characters worth of space in \i s\i0 , but the number of characters read is at most \i count\i0  - 1. The extra character space is used to hold the null character at the end of the string. 
\par 
\par If the system is already at end of file when you call \f1 fgets\f0 , then the contents of the array \i s\i0  are unchanged and a null pointer is returned. A null pointer is also returned if a read error occurs. Otherwise, the return value is the pointer \i s\i0 . 
\par 
\par \b Warning:\b0  If the input data has a null character, you can't tell. So don't use \f1 fgets\f0  unless you know the data cannot contain a null. Don't use it to read files edited by the user because, if the user inserts a null character, you should either handle it properly or print a clear error message. We recommend using \f1 getline\f0  instead of \f1 fgets\f0 .
\par 
\par \pard -- Function: wchar_t * \b fgetws\b0  (\i wchar_t *ws, int count, FILE *stream\i0 )
\par 
\par \pard\li500 The \f1 fgetws\f0  function reads wide characters from the stream\i  stream\i0  up to and including a newline character and stores them in the string \i ws\i0 , adding a null wide character to mark the end of the string. You must supply \i count\i0  wide characters worth of space in\i  ws\i0 , but the number of characters read is at most \i count\i0  - 1. The extra character space is used to hold the null wide character at the end of the string. 
\par 
\par If the system is already at end of file when you call \f1 fgetws\f0 , then the contents of the array \i ws\i0  are unchanged and a null pointer is returned. A null pointer is also returned if a read error occurs. Otherwise, the return value is the pointer \i ws\i0 . 
\par 
\par \b Warning:\b0  If the input data has a null wide character (which are null bytes in the input stream), you can't tell. So don't use\f1  fgetws\f0  unless you know the data cannot contain a null. Don't use it to read files edited by the user because, if the user inserts a null character, you should either handle it properly or print a clear error message.
\par 
\par \pard -- Deprecated function: char * \b gets\b0  (\i char *s\i0 )
\par 
\par \pard\li500 The function \f1 gets\f0  reads characters from the stream \f1 stdin\f0  up to the next newline character, and stores them in the string \i s\i0 . The newline character is discarded (note that this differs from the behavior of \f1 fgets\f0 , which copies the newline character into the string). If \f1 gets\f0  encounters a read error or end-of-file, it returns a null pointer; otherwise it returns \i s\i0 . 
\par 
\par \b Warning:\b0  The \f1 gets\f0  function is \b very dangerous\b0  because it provides no protection against overflowing the string\i  s\i0 . The \f2 C \f0 library includes it for compatibility only.
\par 
\par \pard\cf2\b\f3\fs32 
\par }
1320
PAGE_339
Locale Categories
categoriesforlocales;localecategories;LC_COLLATE;LC_CTYPE;LC_MONETARY;LC_NUMERIC;LC_TIME;LC_MESSAGES;LC_ALL;LANG;LANGUAGE



Imported



FALSE
30
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Locale Categories \{keepn\}
\par \pard\fs18 Next: \cf1\strike Setting the Locale\strike0\{linkID=1960\}\cf0 , Previous: \cf1\strike Choosing Locale\strike0\{linkID=320\}\cf0 , Up: \cf1\strike Locales\strike0\{linkID=1340\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Categories of Activities that Locales Affect\b0 
\par 
\par The purposes that locales serve are grouped into \i categories\i0 , so that a user or a program can choose the locale for each category independently. Here is a table of categories; each name is both an environment variable that a user can set, and a macro name that you can use as an argument to \f1 setlocale\f0 .
\par 
\par \f1 LC_COLLATE\f0 
\par \pard\li500 This category applies to collation of strings (functions \f1 strcoll\f0  and \f1 strxfrm\f0 ); see \cf1\strike Collation Functions\strike0\{linkID=370\}\cf0 .
\par \pard\f1 LC_CTYPE\f0 
\par \pard\li500 This category applies to classification and conversion of characters, and to multibyte and wide characters; see \cf1\strike Character Handling\strike0\{linkID=270\}\cf0 , and \cf1\strike Character Set Handling\strike0\{linkID=290\}\cf0 .
\par \pard\f1 LC_MONETARY\f0 
\par \pard\li500 This category applies to formatting monetary values; see \cf1\strike General Numeric\strike0\{linkID=1040\}\cf0 .
\par \pard\f1 LC_NUMERIC\f0 
\par \pard\li500 This category applies to formatting numeric values that are not monetary; see \cf1\strike General Numeric\strike0\{linkID=1040\}\cf0 .
\par \pard\f1 LC_TIME\f0 
\par \pard\li500 This category applies to formatting date and time values; see \cf1\strike Formatting Calendar Time\strike0\{linkID=980\}\cf0 .
\par \pard\f1 LC_MESSAGES\f0 
\par \pard\li500 This category applies to selecting the language used in the user interface for message translation and contains regular expressions for affirmative and negative responses.
\par \pard\f1 LC_ALL\f0 
\par \pard\li500 This is not an environment variable; it is only a macro that you can use with \f1 setlocale\f0  to set a single locale for all purposes. Setting this environment variable overwrites all selections by the other\f1  LC_*\f0  variables or \f1 LANG\f0 .
\par \pard\f1 LANG\f0 
\par \pard\li500 If this environment variable is defined, its value specifies the locale to use for all purposes except as overridden by the variables above.
\par 
\par }
1330
PAGE_340
Locale Information




Imported



FALSE
19
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Locale Information \{keepn\}
\par \pard\fs18 Next: \cf1\strike Yes-or-No Questions\cf2\strike0\{linkID=2480\}\cf0 , Previous: \cf1\strike Standard Locales\strike0\{linkID=2080\}\cf0 , Up: \cf1\strike Locales\strike0\{linkID=1340\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Accessing Locale Information\b0 
\par 
\par There are several ways to access locale information. The simplest way is to let the C library itself do the work. Several of the functions in this library implicitly access the locale data, and use what information is provided by the currently selected locale. This is how the locale model is meant to work normally. 
\par 
\par As an example take the \f1 strftime\f0  function, which is meant to nicely format date and time information (see \cf1\strike Formatting Calendar Time\strike0\{linkID=980\}\cf0 ). Part of the standard information contained in the \f1 LC_TIME\f0  category is the names of the months. Instead of requiring the programmer to take care of providing the translations the\f1  strftime\f0  function does this all by itself. \f1 %A\f0  in the format string is replaced by the appropriate weekday name of the locale currently selected by \f1 LC_TIME\f0 . This is an easy example, and wherever possible functions do things automatically in this way. 
\par 
\par But there are quite often situations when there is simply no function to perform the task, or it is simply not possible to do the work automatically. For these cases it is necessary to access the information in the locale directly. To do this the C library provides two functions: \f1 localeconv\f0  and \f1 nl_langinfo\f0 . The former is part of ISO C and therefore portable, but has a brain-damaged interface. The second is part of the Unix interface and is portable in as far as the system follows the Unix standards.
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike The Lame Way to Locale Data\strike0\{linkID=2280\}\cf0 : ISO C's \f1 localeconv\f0 .
\par 
\par }
1340
PAGE_341
Locales
internationalization;locales



Imported



FALSE
25
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fnil\fcharset0 Arial;}{\f2\fnil\fcharset0 David;}{\f3\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24  Locales\{keepn\}
\par \pard\fs18 Next: \cf1\strike Searching and Sorting\cf2\strike0\{linkID=1930\}\cf0 , Previous: \cf1\strike Character Set Handling\strike0\{linkID=290\}\cf0 , Up: \cf1\strike Top\strike0\{linkID=10\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Locales and Internationalization\b0 
\par 
\par \f1 Different countries and cultures have varying conventions for how to communicate. These conventions range from very simple ones, such as the format for representing dates and times, to very complex ones, such as the language spoken. 
\par \f2 
\par \i\f0 Internationalization\i0  of software means programming it to be able to adapt to the user's favorite conventions. In ISO C, internationalization works by means of \i locales\i0 . Each locale specifies a collection of conventions, one convention for each purpose. The user chooses a set of conventions by specifying a locale (via environment variables). 
\par 
\par All programs inherit the chosen locale as part of their environment. Provided the programs are written to obey the choice of locale, they will follow the conventions preferred by the user.
\par 
\par \pard\li500\f3\'b7\f0  \cf1\strike Effects of Locale\strike0\{linkID=610\}\cf0 : Actions affected by the choice of locale.
\par \f3\'b7\f0  \cf1\strike Choosing Locale\strike0\{linkID=320\}\cf0 : How the user specifies a locale.
\par \f3\'b7\f0  \cf1\strike Locale Categories\strike0\{linkID=1320\}\cf0 : Different purposes for which you can select a locale.
\par \f3\'b7\f0  \cf1\strike Setting the Locale\strike0\{linkID=1960\}\cf0 : How a program specifies the locale with library functions.
\par \f3\'b7\f0  \cf1\strike Standard Locales\strike0\{linkID=2080\}\cf0 : Locale names available on all systems.
\par \f3\'b7\f0  \cf1\strike Locale Information\strike0\{linkID=1330\}\cf0 : How to access the information for the locale.
\par \f3\'b7\f0  \cf1\strike Yes-or-No Questions\strike0\{linkID=2480\}\cf0 : Check a Response against the locale.
\par 
\par }
1350
PAGE_346
Longjmp in Handler
non-localexit,fromsignalhandler



Imported



FALSE
63
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Longjmp in Handler \{keepn\}
\par \pard\fs18 Next: \cf1\strike Signals in Handler\strike0\{linkID=2030\}\cf0 , Previous: \cf1\strike Termination in Handler\strike0\{linkID=2260\}\cf0 , Up: \cf1\strike Defining Handlers\strike0\{linkID=530\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Nonlocal Control Transfer in Handlers\b0 
\par 
\par You can do a nonlocal transfer of control out of a signal handler using the \f1 setjmp\f0  and \f1 longjmp\f0  facilities (see \cf1\strike Non-Local Exits\strike0\{linkID=1510\}\cf0 ). 
\par 
\par When the handler does a nonlocal control transfer, the part of the program that was running will not continue. If this part of the program was in the middle of updating an important data structure, the data structure will remain inconsistent. Since the program does not terminate, the inconsistency is likely to be noticed later on. 
\par 
\par There are two ways to avoid this problem. One is to block the signal for the parts of the program that update important data structures. Blocking the signal delays its delivery until it is unblocked, once the critical updating is finished. See \cf1\strike Blocking Signals\strike0\{link=PAGE_68\}\cf0 . 
\par 
\par The other way to re-initialize the crucial data structures in the signal handler, or make their values consistent. 
\par 
\par Here is a rather schematic example showing the reinitialization of one global variable.
\par 
\par \f1      #include <signal.h>
\par      #include <setjmp.h>
\par      
\par      jmp_buf return_to_top_level;
\par      
\par      volatile sig_atomic_t waiting_for_input;
\par      
\par      void
\par      handle_sigint (int signum)
\par      \{
\par        /* We may have been waiting for input when the signal arrived,
\par           but we are no longer waiting once we transfer control. */
\par        waiting_for_input = 0;
\par        longjmp (return_to_top_level, 1);
\par      \}
\par      
\par      int
\par      main (void)
\par      \{
\par        ...
\par        signal (SIGINT, sigint_handler);
\par        ...
\par        while (1) \{
\par          prepare_for_command ();
\par          if (setjmp (return_to_top_level) == 0)
\par            read_and_execute_command ();
\par        \}
\par      \}
\par      
\par      /* Imagine this is a subroutine used by various commands. */
\par      char *
\par      read_data ()
\par      \{
\par        if (input_from_terminal) \{
\par          waiting_for_input = 1;
\par          ...
\par          waiting_for_input = 0;
\par        \} else \{
\par          ...
\par        \}
\par      \}
\par \f0 
\par }
1360
PAGE_350
Low-Level I/O




Imported



FALSE
26
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern\fcharset0 Courier New;}{\f2\fmodern Courier New;}{\f3\fnil\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Low-Level I/O \{keepn\}
\par \pard\fs18 Next: \cf1\strike File System Interface\strike0\{linkID=820\}\cf0 , Previous: \cf1\strike I/O on Streams\strike0\{linkID=1260\}\cf0 , Up: \cf1\strike Top\strike0\{linkID=10\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Low-Level Input/Output\b0 
\par 
\par This chapter describes functions for performing low-level input/output operations on file descriptors. These functions include the primitives for the higher-level I/O functions described in \cf1\strike I/O on Streams\strike0\{linkID=1260\}\cf0 , as well as functions for performing low-level control operations for which there are no equivalents on streams. 
\par 
\par Stream-level I/O is more flexible and usually more convenient; therefore, programmers generally use the descriptor-level functions only when necessary. These are some of the usual reasons:
\par 
\par \pard\li500\f1\'b7\f0  For reading binary files in large chunks. 
\par \f1\'b7\f0  For reading an entire file into core before parsing it. 
\par \f1\'b7\f0  To perform operations other than data transfer, which can only be done with a descriptor. (You can use \f2 fileno\f0  to get the descriptor corresponding to a stream.) 
\par \f1\'b7\f0  To pass descriptors to a child process. (The child can create its own stream to use a descriptor that it inherits, but cannot inherit a stream directly.)
\par 
\par \f1\'b7\f0  \cf1\strike Opening and Closing Files\strike0\{linkID=1600\}\cf0 : How to open and close file descriptors.
\par \f1\'b7\f0  \cf1\strike I/O Primitives\strike0\{linkID=1280\}\cf0 : Reading and writing data.
\par \f1\'b7\f0  \cf1\strike File Position Primitive\strike0\{linkID=770\}\cf0 : Setting a descriptor's file position.
\par \f1\'b7\f0  \cf1\strike Descriptors and Streams\strike0\{linkID=560\}\cf0 : Converting descriptor to stream or vice-versa.
\par \f1\'b7\f3  \cf1\strike\f0 Duplicating Descriptors\strike0\{linkID=600\}\cf0 : Fcntl commands for duplicating file descriptors.
\par \f1\'b7\f0  \cf1\strike File Status Flags\strike0\{linkID=810\}\cf0 : Fcntl commands for manipulating flags associated with open files.
\par }
1370
PAGE_353
Macro Definitions
shadowingfunctionswithmacros;removingmacrosthatshadowfunctions;undefiningmacrosthatshadowfunctions



Imported



FALSE
39
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern\fcharset0 Courier New;}{\f2\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Macro Definitions \{keepn\}
\par \pard\fs18 Next: \cf1\strike Reserved Names\strike0\{linkID=1860\}\cf0 , Previous: \cf1\strike Header Files\strike0\{linkID=1090\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Macro Definitions of Functions\b0 
\par 
\par If we describe something as a function in this manual, it may have a macro definition as well. This normally has no effect on how your program runs--the macro definition does the same thing as the function would. In particular, macro equivalents for library functions evaluate arguments exactly once, in the same way that a function call would. The main reason for these macro definitions is that sometimes they can produce an inline expansion that is considerably faster than an actual function call. 
\par 
\par Taking the address of a library function works even if it is also defined as a macro. This is because, in this context, the name of the function isn't followed by the left parenthesis that is syntactically necessary to recognize a macro call. 
\par 
\par You might occasionally want to avoid using the macro definition of a function--perhaps to make your program easier to debug. There are two ways you can do this:
\par 
\par \pard\li500\f1\'b7\f0  You can avoid a macro definition in a specific use by enclosing the name of the function in parentheses. This works because the name of the function doesn't appear in a syntactic context where it is recognizable as a macro call. 
\par \f1\'b7\f0  You can suppress any macro definition for a whole source file by using the `\f2 #undef\f0 ' preprocessor directive, unless otherwise stated explicitly in the description of that facility.
\par 
\par \pard For example, suppose the header file \f2 stdlib.h\f0  declares a function named \f2 abs\f0  with
\par 
\par \f2      extern int abs (int);
\par \f0 
\par and also provides a macro definition for \f2 abs\f0 . Then, in:
\par 
\par \f2      #include <stdlib.h>
\par      int f (int *i) \{ return abs (++*i); \}
\par \f0 
\par the reference to \f2 abs\f0  might refer to either a macro or a function. On the other hand, in each of the following examples the reference is to a function and not a macro.
\par 
\par \f2      #include <stdlib.h>
\par      int g (int *i) \{ return (abs) (++*i); \}
\par      
\par      #undef abs
\par      int h (int *i) \{ return abs (++*i); \}
\par \f0 
\par Since macro definitions that double for a function behave in exactly the same way as the actual function version, there is usually no need for any of these methods. In fact, removing macro definitions usually just makes your program slower.
\par 
\par }
1380
PAGE_356
Malloc Examples




Imported



FALSE
36
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Malloc Examples \{keepn\}
\par \pard\fs18 Next: \cf1\strike Freeing after Malloc\strike0\{linkID=1030\}\cf0 , Previous: \cf1\strike Basic Allocation\strike0\{linkID=170\}\cf0 , Up: \cf1\strike Unconstrained Allocation\strike0\{linkID=2340\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Examples of \f1 malloc\b0\f0 
\par 
\par If no more space is available, \f1 malloc\f0  returns a null pointer. You should check the value of \i every\i0  call to \f1 malloc\f0 . It is useful to write a subroutine that calls \f1 malloc\f0  and reports an error if the value is a null pointer, returning only if the value is nonzero. This function is conventionally called \f1 xmalloc\f0 . Here it is:
\par 
\par \f1      void *
\par      xmalloc (size_t size)
\par      \{
\par        register void *value = malloc (size);
\par        if (value == 0)
\par          fatal ("virtual memory exhausted");
\par        return value;
\par      \}
\par \f0 
\par Here is a real example of using \f1 malloc\f0  (by way of \f1 xmalloc\f0 ). The function \f1 savestring\f0  will copy a sequence of characters into a newly allocated null-terminated string:
\par 
\par \f1      char *
\par      savestring (const char *ptr, size_t len)
\par      \{
\par        register char *value = (char *) xmalloc (len + 1);
\par        value[len] = '\\0';
\par        return (char *) memcpy (value, ptr, len);
\par      \}
\par \f0 
\par The block that \f1 malloc\f0  gives you is guaranteed to be aligned so that it can hold any type of data. In th\f2 is \f0 system, the address is always a multiple of \f2 four\f0  on most systems\f2 .
\par 
\par \f0 Note that the memory located after the end of the block is likely to be in use for something else; perhaps a block already allocated by another call to \f1 malloc\f0 . If you attempt to treat the block as longer than you asked for it to be, you are liable to destroy the data that\f1  malloc\f0  uses to keep track of its blocks, or you may destroy the contents of another block. If you have already allocated a block and discover you want it to be bigger, use \f1 realloc\f0  (see \cf1\strike Changing Block Size\strike0\{linkID=260\}\cf0 ).
\par 
\par }
1390
PAGE_360
Math Error Reporting
errors,mathematical;domainerror;rangeerror;HUGE_VAL;HUGE_VALF;HUGE_VALL



Imported



FALSE
28
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Math Error Reporting \{keepn\}
\par \pard\fs18 Previous: \cf1\strike Status bit operations\strike0\{linkID=2110\}\cf0 , Up: \cf1\strike Floating Point Errors\strike0\{linkID=860\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Error Reporting by Mathematical Functions\b0 
\par 
\par Many of the math functions are defined only over a subset of the real or complex numbers. Even if they are mathematically defined, their result may be larger or smaller than the range representable by their return type. These are known as \i domain errors\i0 , \i overflows\i0 , and\i  underflows\i0 , respectively. Math functions do several things when one of these errors occurs. In this manual we will refer to the complete response as \i signalling\i0  a domain error, overflow, or underflow. 
\par 
\par When a math function suffers a domain error, it raises the invalid exception and returns NaN. It also sets \i errno\i0  to \f1 EDOM\f0 ; this is for compatibility with old systems that do not support IEEE 754 exception handling. Likewise, when overflow occurs, math functions raise the overflow exception and return &infin; or -&infin; as appropriate. They also set \i errno\i0  to\f1  ERANGE\f0 . When underflow occurs, the underflow exception is raised, and zero (appropriately signed) is returned. \i errno\i0  may be set to \f1 ERANGE\f0 , but this is not guaranteed. 
\par 
\par Some of the math functions are defined mathematically to result in a complex value over parts of their domains. The most familiar example of this is taking the square root of a negative number. The complex math functions, such as \f1 csqrt\f0 , will return the appropriate complex value in this case. The real-valued functions, such as \f1 sqrt\f0 , will signal a domain error. 
\par 
\par Some older hardware does not support infinities. On that hardware, overflows instead return a particular very large number (usually the largest representable number). \f1 math.h\f0  defines macros you can use to test for overflow on both old and new hardware.
\par -- Macro: double \b HUGE_VAL\b0 
\par 
\par \pard\li500 -- Macro: float \b HUGE_VALF\b0 
\par 
\par \pard\li1000 -- Macro: long double \b HUGE_VALL\b0 
\par 
\par \pard\li1500 An expression representing a particular very large number. On machines that use IEEE 754 floating point format, \f1 HUGE_VAL\f0  is infinity. On other machines, it's typically the largest positive number that can be represented. 
\par 
\par Mathematical functions return the appropriately typed version of\f1  HUGE_VAL\f0  or \f1 -HUGE_VAL\f0  when the result is too large to be represented.
\par 
\par }
1400
PAGE_361
Mathematical Constants
constants;mathematicalconstants;M_E;M_LOG2E;M_LOG10E;M_LN;M_LN;M_PI;M_PI_;M_PI_;M_1_PI;M_2_PI;M_2_SQRTPI;M_SQRT;M_SQRT1_;PI



Imported



FALSE
40
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Mathematical Constants \{keepn\}
\par \pard\fs18 Next: \cf1\strike Trig Functions\strike0\{linkID=2320\}\cf0 , Up: \cf1\strike Mathematics\strike0\{linkID=1410\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Predefined Mathematical Constants\b0 
\par 
\par The header \f1 math.h\f0  defines several useful mathematical constants. All values are defined as preprocessor macros starting with \f1 M_\f0 . The values provided are:
\par 
\par \f1 M_E\f0 
\par \pard\li500 The base of natural logarithms.
\par \pard\f1 M_LOG2E\f0 
\par \pard\li500 The logarithm to base \f1 2\f0  of \f1 M_E\f0 .
\par \pard\f1 M_LOG10E\f0 
\par \pard\li500 The logarithm to base \f1 10\f0  of \f1 M_E\f0 .
\par \pard\f1 M_LN2\f0 
\par \pard\li500 The natural logarithm of \f1 2\f0 .
\par \pard\f1 M_LN10\f0 
\par \pard\li500 The natural logarithm of \f1 10\f0 .
\par \pard\f1 M_PI\f0 
\par \pard\li500 Pi, the ratio of a circle's circumference to its diameter.
\par \pard\f1 M_PI_2\f0 
\par \pard\li500 Pi divided by two.
\par \pard\f1 M_PI_4\f0 
\par \pard\li500 Pi divided by four.
\par \pard\f1 M_1_PI\f0 
\par \pard\li500 The reciprocal of pi (1/pi)
\par \pard\f1 M_2_PI\f0 
\par \pard\li500 Two times the reciprocal of pi.
\par \pard\f1 M_2_SQRTPI\f0 
\par \pard\li500 Two times the reciprocal of the square root of pi.
\par \pard\f1 M_SQRT2\f0 
\par \pard\li500 The square root of two.
\par \pard\f1 M_SQRT1_2\f0 
\par \pard\li500 The reciprocal of the square root of two (also the square root of 1/2).
\par \pard 
\par }
1410
PAGE_362
Mathematics
math.h;complex.h



Imported



FALSE
22
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Mathematics\{keepn\}
\par \pard\fs18 Next: \cf1\strike Arithmetic\strike0\{linkID=100\}\cf0 , Previous: \cf1\strike File System Interface\cf2\strike0\{linkID=820\}\cf0 , Up: \cf1\strike Top\strike0\{linkID=10\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Mathematics\b0 
\par 
\par This chapter contains information about functions for performing mathematical computations, such as trigonometric functions. Most of these functions have prototypes declared in the header file\f1  math.h\f0 . The complex-valued functions are defined in\f1  complex.h\f0 . All mathematical functions which take a floating-point argument have three variants, one each for \f1 double\f0 , \f1 float\f0 , and\f1  long double\f0  arguments. The \f1 double\f0  versions are mostly defined in ISO C89. The \f1 float\f0  and \f1 long double\f0  versions are from the numeric extensions to C included in ISO C99. 
\par 
\par Which of the three versions of a function should be used depends on the situation. For most calculations, the \f1 float\f0  functions are the fastest. On the other hand, the \f1 long double\f0  functions have the highest precision. \f1 double\f0  is somewhere in between. It is usually wise to pick the narrowest type that can accommodate your data. Not all machines have a distinct \f1 long double\f0  type; it may be the same as \f1 double\f0 .
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Mathematical Constants\strike0\{linkID=1400\}\cf0 : Precise numeric values for often-used constants.
\par \f2\'b7\f0  \cf1\strike Trig Functions\strike0\{linkID=2320\}\cf0 : Sine, cosine, tangent, and friends.
\par \f2\'b7\f0  \cf1\strike Inverse Trig Functions\strike0\{linkID=1220\}\cf0 : Arcsine, arccosine, etc.
\par \f2\'b7\f0  \cf1\strike Exponents and Logarithms\strike0\{linkID=720\}\cf0 : Also pow and sqrt.
\par \f2\'b7\f0  \cf1\strike Hyperbolic Functions\strike0\{linkID=1130\}\cf0 : sinh, cosh, tanh, etc.
\par \f2\'b7\f0  \cf1\strike Special Functions\strike0\{linkID=2070\}\cf0 : Bessel, gamma, erf.
\par \f2\'b7\f0  \cf1\strike Pseudo-Random Numbers\strike0\{linkID=1780\}\cf0 : Functions for generating pseudo-random numbers.
\par }
1420
PAGE_363
Memory Allocation and C
staticmemoryallocation;staticstorageclass;automaticmemoryallocation;automaticstorageclass;dynamicmemoryallocation;dynamicmemoryallocation



Imported



FALSE
40
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern\fcharset0 Courier New;}{\f2\fswiss\fcharset0 Arial;}{\f3\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Memory Allocation and C \{keepn\}
\par \pard\fs18 Next: \cf1\strike Unconstrained Allocation\strike0\{linkID=2340\}\cf0 , Up: \cf1\strike Memory Allocation\strike0\{linkID=1430\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Memory Allocation in C Programs\b0 
\par 
\par The C language supports two kinds of memory allocation through the variables in C programs:
\par 
\par \pard\li500\f1\'b7\i\f0  Static allocation\i0  is what happens when you declare a static or global variable. Each static or global variable defines one block of space, of a fixed size. The space is allocated once, when your program is started (part of the exec operation), and is never freed.
\par \f1\'b7\i\f0  Automatic allocation\i0  happens when you declare an automatic variable, such as a function argument or a local variable. The space for an automatic variable is allocated when the compound statement containing the declaration is entered, and is freed when that compound statement is exited. In C\f2 99 \f0 he size of the automatic storage can be an expression that varies. In \f2 earlier C impl\f0 ementations, it must be a constant.
\par 
\par \pard A third important kind of memory allocation, \i dynamic allocation\i0 , is not supported by C variables but is available via C library functions.
\par 
\par \b Dynamic Memory Allocation\b0 
\par 
\par \i Dynamic memory allocation\i0  is a technique in which programs determine as they are running where to store some information. You need dynamic allocation when the amount of memory you need, or how long you continue to need it, depends on factors that are not known before the program runs. 
\par 
\par For example, you may need a block to store a line read from an input file; since there is no limit to how long a line can be, you must allocate the memory dynamically and make it dynamically larger as you read more of the line. 
\par 
\par Or, you may need a block for each record or each definition in the input data; since you can't know in advance how many there will be, you must allocate a new block for each record or definition as you read it. 
\par 
\par When you use dynamic allocation, the allocation of a block of memory is an action that the program requests explicitly. You call a function or macro when you want to allocate space, and specify the size with an argument. If you want to free the space, you do so by calling another function or macro. You can do these things whenever you want, as often as you want. 
\par 
\par Dynamic allocation is not supported by C variables; there is no storage class "dynamic", and there can never be a C variable whose value is stored in dynamically allocated space. The only way to get dynamically allocated memory is via a system call (which is generally via a C library function call), and the only way to refer to dynamically allocated space is through a pointer. Because it is less convenient, and because the actual process of dynamic allocation requires more computation time, programmers generally use dynamic allocation only when neither static nor automatic allocation will serve. 
\par 
\par For example, if you want to allocate dynamically some space to hold a\f3  struct foobar\f0 , you cannot declare a variable of type \f3 struct foobar\f0  whose contents are the dynamically allocated space. But you can declare a variable of pointer type \f3 struct foobar *\f0  and assign it the address of the space. Then you can use the operators `\f3 *\f0 ' and `\f3 ->\f0 ' on this pointer variable to refer to the contents of the space:
\par 
\par \f3      \{
\par        struct foobar *ptr
\par           = (struct foobar *) malloc (sizeof (struct foobar));
\par        ptr->name = x;
\par        ptr->next = current_foobar;
\par        current_foobar = ptr;
\par      \}
\par \f0 
\par }
1430
PAGE_364
Memory Allocation




Imported



FALSE
17
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fswiss\fcharset0 Arial;}{\f3\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Memory Allocation \{keepn\}
\par \pard\fs18 Up: \cf1\strike Memory\strike0\{linkID=1440\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Allocating Storage For Program Data\b0 
\par 
\par This section covers how ordinary programs manage storage for their data, including the famous \f1 malloc\f0  function and some fancier facilities special \f2 to \f0 the C library and \f2 CC386\f0  Compiler.
\par 
\par \pard\li500\f3\'b7\f0  \cf1\strike Memory Allocation and C\strike0\{linkID=1420\}\cf0 : How to get different kinds of allocation in C.
\par \f3\'b7\f0  \cf1\strike Unconstrained Allocation\strike0\{linkID=2340\}\cf0 : The \f1 malloc\f0  facility allows fully general dynamic allocation.
\par \f3\'b7\f0  \cf1\strike Variable Size Automatic\strike0\{linkID=2400\}\cf0 : Allocation of variable-sized blocks of automatic storage that are freed when the calling function returns.
\par 
\par }
1440
PAGE_368
Memory
memoryallocation;storageallocation



Imported



FALSE
19
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern\fcharset0 Courier New;}{\f2\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Memory\{keepn\}
\par \pard\fs18 Next: \cf1\strike Character Handling\strike0\{linkID=270\}\cf0 , Previous: \cf1\strike Error Reporting\strike0\{linkID=690\}\cf0 , Up: \cf1\strike Top\strike0\{linkID=10\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Virtual Memory Allocation And Paging\b0 
\par 
\par This chapter describes how processes manage and use memory in a system that uses the C library. 
\par 
\par The C Library has several functions for dynamically allocating virtual memory in various ways.
\par 
\par \pard\li500\f1\'b7\f0  \cf1\strike Memory Allocation\strike0\{linkID=1430\}\cf0 : Allocating storage for your program data
\par 
\par \pard\f1\'b7\f0  Memory mapped I/O is not discussed in this\f2  document.\f0 
\par 
\par }
1450
PAGE_375
Misc FP Arithmetic
minimum;maximum;positivedifference;multiply-add;fmin;fminf;fminl;fmax;fmaxf;fmaxl;fdim;fdimf;fdiml;fma;fmaf;fmal;butterfly;FP_FAST_FMA



Imported



FALSE
59
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Misc FP Arithmetic \{keepn\}
\par \pard\fs18 Previous: \cf1\strike FP Comparison Functions\strike0\{linkID=1000\}\cf0 , Up: \cf1\strike Arithmetic Functions\strike0\{linkID=90\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Miscellaneous FP arithmetic functions\b0 
\par 
\par The functions in this section perform miscellaneous but common operations that are awkward to express with C operators. On some processors these functions can use special machine instructions to perform these operations faster than the equivalent C code.
\par 
\par -- Function: double \b fmin\b0  (\i double x, double y\i0 )
\par 
\par \pard\li500 -- Function: float \b fminf\b0  (\i float x, float y\i0 )
\par 
\par \pard\li1000 -- Function: long double \b fminl\b0  (\i long double x, long double y\i0 )
\par 
\par \pard\li1500 The \f1 fmin\f0  function returns the lesser of the two values \i x\i0  and \i y\i0 . It is similar to the expression 
\par 
\par \f1           ((x) < (y) ? (x) : (y))
\par      
\par \f0 except that \i x\i0  and \i y\i0  are only evaluated once. 
\par 
\par If an argument is NaN, the other argument is returned. If both arguments are NaN, NaN is returned.
\par 
\par \pard -- Function: double \b fmax\b0  (\i double x, double y\i0 )
\par 
\par \pard\li500 -- Function: float \b fmaxf\b0  (\i float x, float y\i0 )
\par 
\par \pard\li1000 -- Function: long double \b fmaxl\b0  (\i long double x, long double y\i0 )
\par 
\par \pard\li1500 The \f1 fmax\f0  function returns the greater of the two values \i x\i0  and \i y\i0 . 
\par 
\par If an argument is NaN, the other argument is returned. If both arguments are NaN, NaN is returned.
\par 
\par \pard -- Function: double \b fdim\b0  (\i double x, double y\i0 )
\par 
\par \pard\li500 -- Function: float \b fdimf\b0  (\i float x, float y\i0 )
\par 
\par \pard\li1000 -- Function: long double \b fdiml\b0  (\i long double x, long double y\i0 )
\par 
\par \pard\li1500 The \f1 fdim\f0  function returns the positive difference between\i  x\i0  and \i y\i0 . The positive difference is \i x\i0  -\i  y\i0  if \i x\i0  is greater than \i y\i0 , and 0 otherwise. 
\par 
\par If \i x\i0 , \i y\i0 , or both are NaN, NaN is returned.
\par 
\par \pard -- Function: double \b fma\b0  (\i double x, double y, double z\i0 )
\par 
\par \pard\li500 -- Function: float \b fmaf\b0  (\i float x, float y, float z\i0 )
\par 
\par \pard\li1000 -- Function: long double \b fmal\b0  (\i long double x, long double y, long double z\i0 )
\par 
\par \pard\li1500 The \f1 fma\f0  function performs floating-point multiply-add. This is the operation (\i x\i0  \f2 *\f0  \i y\i0 ) + \i z\i0 , but the intermediate result is not rounded to the destination type. This can sometimes improve the precision of a calculation. 
\par 
\par This function was introduced because some processors have a special instruction to perform multiply-add. The C compiler cannot use it directly, because the expression `\f1 x*y + z\f0 ' is defined to round the intermediate result. \f1 fma\f0  lets you choose when you want to round only once. 
\par 
\par On processors which do not implement multiply-add in hardware,\f1  fma\f0  can be very slow since it must avoid intermediate rounding.\f1  math.h\f0  defines the symbols \f1 FP_FAST_FMA\f0 ,\f1  FP_FAST_FMAF\f0 , and \f1 FP_FAST_FMAL\f0  when the corresponding version of \f1 fma\f0  is no slower than the expression `\f1 x*y + z\f0 '.
\par 
\par }
1460
PAGE_377
Miscellaneous Signals
SIGUSR;SIGUSR;usersignals;SIGWINCH;SIGINFO



Imported



FALSE
22
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Miscellaneous Signals \{keepn\}
\par \pard\fs18 Previous: \cf1\strike Program Error Signals\cf2\strike0\{linkID=1760\}\cf0 , Up: \cf1\strike Standard Signals\strike0\{linkID=2090\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Miscellaneous Signals\b0 
\par 
\par These signals are used for various other purposes. In general, they will not affect your program unless it explicitly uses them for something.
\par 
\par -- Macro: int \b SIGUSR1\b0 
\par 
\par \pard\li500 -- Macro: int \b SIGUSR2\b0 
\par 
\par \pard\li1000 The \f1 SIGUSR1\f0  and \f1 SIGUSR2\f0  signals are set aside for you to use any way you want. They're useful for simple interprocess communication, if you write a signal handler for them in the program that receives the signal. 
\par 
\par The default action is to terminate the process.
\par 
\par \pard\li500 
\par }
1470
PAGE_385
Multibyte Conversion Example




Imported



FALSE
92
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Multibyte Conversion Example \{keepn\}
\par \pard\fs18 Previous: \cf1\strike Converting Strings\strike0\{linkID=450\}\cf0 , Up: \cf1\strike Restartable multibyte conversion\strike0\{linkID=1870\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b A Complete Multibyte Conversion Example\b0 
\par 
\par The example programs given in the last sections are only brief and do not contain all the error checking, etc. Presented here is a complete and documented example. It features the \f1 mbrtowc\f0  function but it should be easy to derive versions using the other functions.
\par 
\par \f1      int
\par      file_mbsrtowcs (int input, int output)
\par      \{
\par        /* Note the use of MB_LEN_MAX.
\par           MB_CUR_MAX cannot portably be used here.  */
\par        char buffer[BUFSIZ + MB_LEN_MAX];
\par        mbstate_t state;
\par        int filled = 0;
\par        int eof = 0;
\par      
\par        /* Initialize the state.  */
\par        memset (&state, '\\0', sizeof (state));
\par      
\par        while (!eof)
\par          \{
\par            ssize_t nread;
\par            ssize_t nwrite;
\par            char *inp = buffer;
\par            wchar_t outbuf[BUFSIZ];
\par            wchar_t *outp = outbuf;
\par      
\par            /* Fill up the buffer from the input file.  */
\par            nread = read (input, buffer + filled, BUFSIZ);
\par            if (nread < 0)
\par              \{
\par                perror ("read");
\par                return 0;
\par              \}
\par            /* If we reach end of file, make a note to read no more. */
\par            if (nread == 0)
\par              eof = 1;
\par      
\par            /* filled is now the number of bytes in buffer. */
\par            filled += nread;
\par      
\par            /* Convert those bytes to wide characters--as many as we can. */
\par            while (1)
\par              \{
\par                size_t thislen = mbrtowc (outp, inp, filled, &state);
\par                /* Stop converting at invalid character;
\par                   this can mean we have read just the first part
\par                   of a valid character.  */
\par                if (thislen == (size_t) -1)
\par                  break;
\par                /* We want to handle embedded NUL bytes
\par                   but the return value is 0.  Correct this.  */
\par                if (thislen == 0)
\par                  thislen = 1;
\par                /* Advance past this character. */
\par                inp += thislen;
\par                filled -= thislen;
\par                ++outp;
\par              \}
\par      
\par            /* Write the wide characters we just made.  */
\par            nwrite = write (output, outbuf,
\par                            (outp - outbuf) * sizeof (wchar_t));
\par            if (nwrite < 0)
\par              \{
\par                perror ("write");
\par                return 0;
\par              \}
\par      
\par            /* See if we have a \i real\i0  invalid character. */
\par            if ((eof && filled > 0) || filled >= MB_CUR_MAX)
\par              \{
\par                error (0, 0, "invalid multibyte character");
\par                return 0;
\par              \}
\par      
\par            /* If any characters must be carried forward,
\par               put them at the beginning of buffer. */
\par            if (filled > 0)
\par              memmove (inp, buffer, filled);
\par          \}
\par      
\par        return 1;
\par      \}
\par \f0 
\par }
1480
PAGE_393
Nonreentrancy
restrictionsonsignalhandlerfunctions;volatile;reentrantfunctions



Imported



FALSE
43
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Nonreentrancy\{keepn\}
\par \pard\fs18 Next: \cf1\strike Atomic Data Access\strike0\{linkID=130\}\cf0 , Previous: \cf1\strike Signals in Handler\cf2\strike0\{linkID=2030\}\cf0 , Up: \cf1\strike Defining Handlers\strike0\{linkID=530\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Signal Handling and Nonreentrant Functions\b0 
\par 
\par Handler functions usually don't do very much. The best practice is to write a handler that does nothing but set an external variable that the program checks regularly, and leave all serious work to the program. This is best because the handler can be called asynchronously, at unpredictable times--perhaps in the middle of a primitive function, or even between the beginning and the end of a C operator that requires multiple instructions. The data structures being manipulated might therefore be in an inconsistent state when the handler function is invoked. Even copying one \f1 int\f0  variable into another can take two instructions on most machines. 
\par 
\par This means you have to be very careful about what you do in a signal handler.
\par 
\par \pard\li500\f2\'b7\f0  If your handler needs to access any global variables from your program, declare those variables \f1 volatile\f0 . This tells the compiler that the value of the variable might change asynchronously, and inhibits certain optimizations that would be invalidated by such modifications. 
\par \f2\'b7\f0  If you call a function in the handler, make sure it is \i reentrant\i0  with respect to signals, or else make sure that the signal cannot interrupt a call to a related function.
\par 
\par \pard A function can be non-reentrant if it uses memory that is not on the stack.
\par 
\par \pard\li500\f2\'b7\f0  If a function uses a static variable or a global variable, or a dynamically-allocated object that it finds for itself, then it is non-reentrant and any two calls to the function can interfere. 
\par 
\par For example, suppose that the signal handler uses \f1 gethostbyname\f0 . This function returns its value in a static object, reusing the same object each time. If the signal happens to arrive during a call to\f1  gethostbyname\f0 , or even after one (while the program is still using the value), it will clobber the value that the program asked for. 
\par 
\par However, if the program does not use \f1 gethostbyname\f0  or any other function that returns information in the same object, or if it always blocks signals around each use, then you are safe. 
\par 
\par There are a large number of library functions that return values in a fixed object, always reusing the same object in this fashion, and all of them cause the same problem. Function descriptions in this manual always mention this behavior. 
\par \f2\'b7\f0  If a function uses and modifies an object that you supply, then it is potentially non-reentrant; two calls can interfere if they use the same object. 
\par 
\par This case arises when you do I/O using streams. Suppose that the signal handler prints a message with \f1 fprintf\f0 . Suppose that the program was in the middle of an \f1 fprintf\f0  call using the same stream when the signal was delivered. Both the signal handler's message and the program's data could be corrupted, because both calls operate on the same data structure--the stream itself. 
\par 
\par However, if you know that the stream that the handler uses cannot possibly be used by the program at a time when signals can arrive, then you are safe. It is no problem if the program uses some other stream. 
\par \f2\'b7\f0  On most systems, \f1 malloc\f0  and \f1 free\f0  are not reentrant, because they use a static data structure which records what memory blocks are free. As a result, no library functions that allocate or free memory are reentrant. This includes functions that allocate space to store a result. 
\par 
\par The best way to avoid the need to allocate memory in a handler is to allocate in advance space for signal handlers to use. 
\par 
\par The best way to avoid freeing memory in a handler is to flag or record the objects to be freed, and have the program check from time to time whether anything is waiting to be freed. But this must be done with care, because placing an object on a chain is not atomic, and if it is interrupted by another signal handler that does the same thing, you could "lose" one of the objects. 
\par \f2\'b7\f0  Any function that modifies \f1 errno\f0  is non-reentrant, but you can correct for this: in the handler, save the original value of\f1  errno\f0  and restore it before returning normally. This prevents errors that occur within the signal handler from being confused with errors from system calls at the point the program is interrupted to run the handler. 
\par 
\par This technique is generally applicable; if you want to call in a handler a function that modifies a particular object in memory, you can make this safe by saving and restoring that object. 
\par \f2\'b7\f0  Merely reading from a memory object is safe provided that you can deal with any of the values that might appear in the object at a time when the signal can be delivered. Keep in mind that assignment to some data types requires more than one instruction, which means that the handler could run "in the middle of" an assignment to the variable if its type is not atomic. See \cf1\strike Atomic Data Access\strike0\{linkID=130\}\cf0 . 
\par \f2\'b7\f0  Merely writing into a memory object is safe as long as a sudden change in the value, at any time when the handler might run, will not disturb anything.
\par 
\par }
1490
PAGE_394
Non-atomic Example




Imported



FALSE
45
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Non-atomic Example \{keepn\}
\par \pard\fs18 Next: \cf1\strike Atomic Types\strike0\{linkID=140\}\cf0 , Up: \cf1\strike Atomic Data Access\strike0\{linkID=130\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Problems with Non-Atomic Access\b0 
\par 
\par Here is an example which shows what can happen if a signal handler runs in the middle of modifying a variable. (Interrupting the reading of a variable can also lead to paradoxical results, but here we only show writing.)
\par 
\par \f1      #include <signal.h>
\par      #include <stdio.h>
\par      
\par      struct two_words \{ int a, b; \} memory;
\par      
\par      void
\par      handler(int signum)
\par      \{
\par         printf ("%d,%d\\n", memory.a, memory.b);
\par         alarm (1);
\par      \}
\par      
\par      int
\par      main (void)
\par      \{
\par         static struct two_words zeros = \{ 0, 0 \}, ones = \{ 1, 1 \};
\par         signal (SIGALRM, handler);
\par         memory = zeros;
\par         alarm (1);
\par         while (1)
\par           \{
\par             memory = zeros;
\par             memory = ones;
\par           \}
\par      \}
\par \f0 
\par This program fills \f1 memory\f0  with zeros, ones, zeros, ones, alternating forever; meanwhile, once per second, the alarm signal handler prints the current contents. (Calling \f1 printf\f0  in the handler is safe in this program because it is certainly not being called outside the handler when the signal happens.) 
\par 
\par Clearly, this program can print a pair of zeros or a pair of ones. But that's not all it can do! On most machines, it takes several instructions to store a new value in \f1 memory\f0 , and the value is stored one word at a time. If the signal is delivered in between these instructions, the handler might find that \f1 memory.a\f0  is zero and\f1  memory.b\f0  is one (or vice versa). 
\par 
\par On some machines it may be possible to store a new value in\f1  memory\f0  with just one instruction that cannot be interrupted. On these machines, the handler will always print two zeros or two ones.
\par 
\par }
1500
PAGE_395
Non-Local Details
setjmp.h;jmp_buf;setjmp;longjmp



Imported



FALSE
40
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Non-Local Details \{keepn\}
\par \pard\fs18 Previous: \cf1\strike Non-Local Intro\strike0\{linkID=1520\}\cf0 , Up: \cf1\strike Non-Local Exits\strike0\{linkID=1510\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Details of Non-Local Exits\b0 
\par 
\par Here are the details on the functions and data structures used for performing non-local exits. These facilities are declared in\f1  setjmp.h\f0 .
\par 
\par -- Data Type: \b jmp_buf\b0 
\par 
\par \pard\li500 Objects of type \f1 jmp_buf\f0  hold the state information to be restored by a non-local exit. The contents of a \f1 jmp_buf\f0  identify a specific place to return to.
\par 
\par \pard -- Macro: int \b setjmp\b0  (\i jmp_buf state\i0 )
\par 
\par \pard\li500 When called normally, \f1 setjmp\f0  stores information about the execution state of the program in \i state\i0  and returns zero. If\f1  longjmp\f0  is later used to perform a non-local exit to this\i  state\i0 , \f1 setjmp\f0  returns a nonzero value.
\par 
\par \pard -- Function: void \b longjmp\b0  (\i jmp_buf state, int value\i0 )
\par 
\par \pard\li500 This function restores current execution to the state saved in\i  state\i0 , and continues execution from the call to \f1 setjmp\f0  that established that return point. Returning from \f1 setjmp\f0  by means of\f1  longjmp\f0  returns the \i value\i0  argument that was passed to\f1  longjmp\f0 , rather than \f1 0\f0 . (But if \i value\i0  is given as\f1  0\f0 , \f1 setjmp\f0  returns \f1 1\f0 ).
\par 
\par \pard There are a lot of obscure but important restrictions on the use of\f1  setjmp\f0  and \f1 longjmp\f0 . Most of these restrictions are present because non-local exits require a fair amount of magic on the part of the C compiler and can interact with other parts of the language in strange ways. 
\par 
\par The \f1 setjmp\f0  function is actually a macro without an actual function definition, so you shouldn't try to `\f1 #undef\f0 ' it or take its address. In addition, calls to \f1 setjmp\f0  are safe in only the following contexts:
\par 
\par \pard\li500\f2\'b7\f0  As the test expression of a selection or iteration statement (such as `\f1 if\f0 ', `\f1 switch\f0 ', or `\f1 while\f0 '). 
\par \f2\'b7\f0  As one operand of a equality or comparison operator that appears as the test expression of a selection or iteration statement. The other operand must be an integer constant expression. 
\par \f2\'b7\f0  As the operand of a unary `\f1 !\f0 ' operator, that appears as the test expression of a selection or iteration statement. 
\par \f2\'b7\f0  By itself as an expression statement.
\par 
\par \pard Return points are valid only during the dynamic extent of the function that called \f1 setjmp\f0  to establish them. If you \f1 longjmp\f0  to a return point that was established in a function that has already returned, unpredictable and disastrous things are likely to happen. 
\par 
\par You should use a nonzero \i value\i0  argument to \f1 longjmp\f0 . While\f1  longjmp\f0  refuses to pass back a zero argument as the return value from \f1 setjmp\f0 , this is intended as a safety net against accidental misuse and is not really good programming style. 
\par 
\par When you perform a non-local exit, accessible objects generally retain whatever values they had at the time \f1 longjmp\f0  was called. The exception is that the values of automatic variables local to the function containing the \f1 setjmp\f0  call that have been changed since the call to \f1 setjmp\f0  are indeterminate, unless you have declared them \f1 volatile\f0 .
\par 
\par }
1510
PAGE_397
Non-Local Exits
non-localexits;longjumps



Imported



FALSE
15
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fswiss\fcharset0 Arial;}{\f2\fmodern Courier New;}{\f3\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Non-Local Exits \{keepn\}
\par \pard\fs18 Next: \cf1\strike Signal Handling\strike0\{linkID=2010\}\cf0 , Previous: \cf1\strike\f1 Date and Time\cf2\strike0\{linkID=520\}\cf0\f0 , Up: \cf1\strike Top\strike0\{linkID=10\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Non-Local Exits\b0 
\par 
\par Sometimes when your program detects an unusual situation inside a deeply nested set of function calls, you would like to be able to immediately return to an outer level of control. This section describes how you can do such \i non-local exits\i0  using the \f2 setjmp\f0  and \f2 longjmp\f0  functions.
\par 
\par \pard\li500\f3\'b7\f0  \cf1\strike Intro\strike0\{linkID=1520\}\cf0 : When and how to use these facilities.
\par \f3\'b7\f0  \cf1\strike Details\strike0\{linkID=1500\}\cf0 : Functions for non-local exits.\cf1\strike 
\par }
1520
PAGE_398
Non-Local Intro




Imported



FALSE
67
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Non-Local Intro \{keepn\}
\par \pard\fs18 Next: \cf1\strike Non-Local Details\strike0\{linkID=1500\}\cf0 , Up: \cf1\strike Non-Local Exits\strike0\{linkID=1510\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Introduction to Non-Local Exits\b0 
\par 
\par As an example of a situation where a non-local exit can be useful, suppose you have an interactive program that has a "main loop" that prompts for and executes commands. Suppose the "read" command reads input from a file, doing some lexical analysis and parsing of the input while processing it. If a low-level input error is detected, it would be useful to be able to return immediately to the "main loop" instead of having to make each of the lexical analysis, parsing, and processing phases all have to explicitly deal with error situations initially detected by nested calls. 
\par 
\par (On the other hand, if each of these phases has to do a substantial amount of cleanup when it exits--such as closing files, deallocating buffers or other data structures, and the like--then it can be more appropriate to do a normal return and have each phase do its own cleanup, because a non-local exit would bypass the intervening phases and their associated cleanup code entirely. Alternatively, you could use a non-local exit but do the cleanup explicitly either before or after returning to the "main loop".) 
\par 
\par In some ways, a non-local exit is similar to using the `\f1 return\f0 ' statement to return from a function. But while `\f1 return\f0 ' abandons only a single function call, transferring control back to the point at which it was called, a non-local exit can potentially abandon many levels of nested function calls. 
\par 
\par You identify return points for non-local exits by calling the function\f1  setjmp\f0 . This function saves information about the execution environment in which the call to \f1 setjmp\f0  appears in an object of type \f1 jmp_buf\f0 . Execution of the program continues normally after the call to \f1 setjmp\f0 , but if an exit is later made to this return point by calling \f1 longjmp\f0  with the corresponding \f1 jmp_buf\f0  object, control is transferred back to the point where \f1 setjmp\f0  was called. The return value from \f1 setjmp\f0  is used to distinguish between an ordinary return and a return made by a call to\f1  longjmp\f0 , so calls to \f1 setjmp\f0  usually appear in an `\f1 if\f0 ' statement. 
\par 
\par Here is how the example program described above might be set up:
\par 
\par \f1      #include <setjmp.h>
\par      #include <stdlib.h>
\par      #include <stdio.h>
\par      
\par      jmp_buf main_loop;
\par      
\par      void
\par      abort_to_main_loop (int status)
\par      \{
\par        longjmp (main_loop, status);
\par      \}
\par      
\par      int
\par      main (void)
\par      \{
\par        while (1)
\par          if (setjmp (main_loop))
\par            puts ("Back at main loop....");
\par          else
\par            do_command ();
\par      \}
\par      
\par      
\par      void
\par      do_command (void)
\par      \{
\par        char buffer[128];
\par        if (fgets (buffer, 128, stdin) == NULL)
\par          abort_to_main_loop (-1);
\par        else
\par          exit (EXIT_SUCCESS);
\par      \}
\par \f0 
\par The function \f1 abort_to_main_loop\f0  causes an immediate transfer of control back to the main loop of the program, no matter where it is called from. 
\par 
\par The flow of control inside the \f1 main\f0  function may appear a little mysterious at first, but it is actually a common idiom with\f1  setjmp\f0 . A normal call to \f1 setjmp\f0  returns zero, so the "else" clause of the conditional is executed. If\f1  abort_to_main_loop\f0  is called somewhere within the execution of\f1  do_command\f0 , then it actually appears as if the \i same\i0  call to \f1 setjmp\f0  in \f1 main\f0  were returning a second time with a value of \f1 -1\f0 . 
\par 
\par So, the general pattern for using \f1 setjmp\f0  looks something like:
\par 
\par \f1      if (setjmp (\i buffer\i0 ))
\par        /* Code to clean up after premature return. */
\par        ...
\par      else
\par        /* Code to be executed normally after setting up the return point. */
\par        ...
\par \f0 
\par }
1530
PAGE_399
Non-reentrant Character Conversion
mbtowc;wctomb;mblen;stdlib.h



Imported



FALSE
52
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Non-reentrant Character Conversion \{keepn\}
\par \pard\fs18 Next: \cf1\strike Non-reentrant String Conversion\strike0\{linkID=1550\}\cf0 , Up: \cf1\strike Non-reentrant Conversion\strike0\{linkID=1540\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Non-reentrant Conversion of Single Characters\b0 
\par 
\par -- Function: int \b mbtowc\b0  (\i wchar_t *restrict result, const char *restrict string, size_t size\i0 )
\par 
\par \pard\li500 The \f1 mbtowc\f0  ("multibyte to wide character") function when called with non-null \i string\i0  converts the first multibyte character beginning at \i string\i0  to its corresponding wide character code. It stores the result in \f1 *\i\f0 result\i0 . 
\par 
\par \f1 mbtowc\f0  never examines more than \i size\i0  bytes. (The idea is to supply for \i size\i0  the number of bytes of data you have in hand.) 
\par 
\par \f1 mbtowc\f0  with non-null \i string\i0  distinguishes three possibilities: the first \i size\i0  bytes at \i string\i0  start with valid multibyte characters, they start with an invalid byte sequence or just part of a character, or \i string\i0  points to an empty string (a null character). 
\par 
\par For a valid multibyte character, \f1 mbtowc\f0  converts it to a wide character and stores that in \f1 *\i\f0 result\i0 , and returns the number of bytes in that character (always at least 1 and never more than \i size\i0 ). 
\par 
\par For an invalid byte sequence, \f1 mbtowc\f0  returns -1. For an empty string, it returns 0, also storing \f1 '\\0'\f0  in\f1  *\i\f0 result\i0 . 
\par 
\par If the multibyte character code uses shift characters, then\f1  mbtowc\f0  maintains and updates a shift state as it scans. If you call \f1 mbtowc\f0  with a null pointer for \i string\i0 , that initializes the shift state to its standard initial value. It also returns nonzero if the multibyte character code in use actually has a shift state. See \cf1\strike Shift State\strike0\{linkID=1970\}\cf0 .
\par 
\par \pard -- Function: int \b wctomb\b0  (\i char *string, wchar_t wchar\i0 )
\par 
\par \pard\li500 The \f1 wctomb\f0  ("wide character to multibyte") function converts the wide character code \i wchar\i0  to its corresponding multibyte character sequence, and stores the result in bytes starting at\i  string\i0 . At most \f1 MB_CUR_MAX\f0  characters are stored. 
\par 
\par \f1 wctomb\f0  with non-null \i string\i0  distinguishes three possibilities for \i wchar\i0 : a valid wide character code (one that can be translated to a multibyte character), an invalid code, and\f1  L'\\0'\f0 . 
\par 
\par Given a valid code, \f1 wctomb\f0  converts it to a multibyte character, storing the bytes starting at \i string\i0 . Then it returns the number of bytes in that character (always at least 1 and never more than \f1 MB_CUR_MAX\f0 ). 
\par 
\par If \i wchar\i0  is an invalid wide character code, \f1 wctomb\f0  returns -1. If \i wchar\i0  is \f1 L'\\0'\f0 , it returns \f1 0\f0 , also storing \f1 '\\0'\f0  in \f1 *\i\f0 string\i0 . 
\par 
\par If the multibyte character code uses shift characters, then\f1  wctomb\f0  maintains and updates a shift state as it scans. If you call \f1 wctomb\f0  with a null pointer for \i string\i0 , that initializes the shift state to its standard initial value. It also returns nonzero if the multibyte character code in use actually has a shift state. See \cf1\strike Shift State\strike0\{linkID=1970\}\cf0 . 
\par 
\par Calling this function with a \i wchar\i0  argument of zero when\i  string\i0  is not null has the side-effect of reinitializing the stored shift state \i as well as\i0  storing the multibyte character\f1  '\\0'\f0  and returning 0.
\par 
\par \pard Similar to \f1 mbrlen\f0  there is also a non-reentrant function that computes the length of a multibyte character. It can be defined in terms of \f1 mbtowc\f0 .
\par -- Function: int \b mblen\b0  (\i const char *string, size_t size\i0 )
\par 
\par \pard\li500 The \f1 mblen\f0  function with a non-null \i string\i0  argument returns the number of bytes that make up the multibyte character beginning at\i  string\i0 , never examining more than \i size\i0  bytes. (The idea is to supply for \i size\i0  the number of bytes of data you have in hand.) 
\par 
\par The return value of \f1 mblen\f0  distinguishes three possibilities: the first \i size\i0  bytes at \i string\i0  start with valid multibyte characters, they start with an invalid byte sequence or just part of a character, or \i string\i0  points to an empty string (a null character). 
\par 
\par For a valid multibyte character, \f1 mblen\f0  returns the number of bytes in that character (always at least \f1 1\f0  and never more than\i  size\i0 ). For an invalid byte sequence, \f1 mblen\f0  returns -1. For an empty string, it returns 0. 
\par 
\par If the multibyte character code uses shift characters, then \f1 mblen\f0  maintains and updates a shift state as it scans. If you call\f1  mblen\f0  with a null pointer for \i string\i0 , that initializes the shift state to its standard initial value. It also returns a nonzero value if the multibyte character code in use actually has a shift state. See \cf1\strike Shift State\strike0\{linkID=1970\}\cf0 . 
\par 
\par The function \f1 mblen\f0  is declared in \f1 stdlib.h\f0 .
\par 
\par }
1540
PAGE_400
Non-reentrant Conversion




Imported



FALSE
21
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Non-reentrant Conversion \{keepn\}
\par \pard\fs18 Previous: \cf1\strike Restartable multibyte conversion\strike0\{linkID=1870\}\cf0 , Up: \cf1\strike Character Set Handling\strike0\{linkID=290\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Non-reentrant Conversion Function\b0 
\par 
\par The functions described in the previous chapter are defined in Amendment 1 to ISO C90, but the original ISO C90 standard also contained functions for character set conversion. The reason that these original functions are not described first is that they are almost entirely useless. 
\par 
\par The problem is that all the conversion functions described in the original ISO C90 use a local state. Using a local state implies that multiple conversions at the same time (not only when using threads) cannot be done, and that you cannot first convert single characters and then strings since you cannot tell the conversion functions which state to use. 
\par 
\par These original functions are therefore usable only in a very limited set of situations. One must complete converting the entire string before starting a new one, and each string/text must be converted with the same function (there is no problem with the library itself; it is guaranteed that no library function changes the state of any of these functions).\b  For the above reasons it is highly requested that the functions described in the previous section be used in place of non-reentrant conversion functions.\b0 
\par 
\par \pard\li500\f1\'b7\f0  \cf1\strike Non-reentrant Character Conversion\strike0\{linkID=1530\}\cf0 : Non-reentrant Conversion of Single Characters.
\par \f1\'b7\f0  \cf1\strike Non-reentrant String Conversion\strike0\{linkID=1550\}\cf0 : Non-reentrant Conversion of Strings.
\par \f1\'b7\f0  \cf1\strike Shift State\strike0\{linkID=1970\}\cf0 : States in Non-reentrant Functions.
\par 
\par }
1550
PAGE_401
Non-reentrant String Conversion
mbstowcs;wcstombs



Imported



FALSE
44
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Non-reentrant String Conversion \{keepn\}
\par \pard\fs18 Next: \cf1\strike Shift State\strike0\{linkID=1970\}\cf0 , Previous: \cf1\strike Non-reentrant Character Conversion\strike0\{linkID=1530\}\cf0 , Up: \cf1\strike Non-reentrant Conversion\strike0\{linkID=1540\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Non-reentrant Conversion of Strings\b0 
\par 
\par For convenience the ISO C90 standard also defines functions to convert entire strings instead of single characters. These functions suffer from the same problems as their reentrant counterparts from Amendment 1 to ISO C90; see \cf1\strike Converting Strings\strike0\{linkID=450\}\cf0 .
\par 
\par -- Function: size_t \b mbstowcs\b0  (\i wchar_t *wstring, const char *string, size_t size\i0 )
\par 
\par \pard\li500 The \f1 mbstowcs\f0  ("multibyte string to wide character string") function converts the null-terminated string of multibyte characters\i  string\i0  to an array of wide character codes, storing not more than\i  size\i0  wide characters into the array beginning at \i wstring\i0 . The terminating null character counts towards the size, so if \i size\i0  is less than the actual number of wide characters resulting from\i  string\i0 , no terminating null character is stored. 
\par 
\par The conversion of characters from \i string\i0  begins in the initial shift state. 
\par 
\par If an invalid multibyte character sequence is found, the \f1 mbstowcs\f0  function returns a value of -1. Otherwise, it returns the number of wide characters stored in the array \i wstring\i0 . This number does not include the terminating null character, which is present if the number is less than \i size\i0 . 
\par 
\par Here is an example showing how to convert a string of multibyte characters, allocating enough space for the result. 
\par 
\par \f1           wchar_t *
\par           mbstowcs_alloc (const char *string)
\par           \{
\par             size_t size = strlen (string) + 1;
\par             wchar_t *buf = xmalloc (size * sizeof (wchar_t));
\par           
\par             size = mbstowcs (buf, string, size);
\par             if (size == (size_t) -1)
\par               return NULL;
\par             buf = xrealloc (buf, (size + 1) * sizeof (wchar_t));
\par             return buf;
\par           \}
\par      
\par \pard\f0 -- Function: size_t \b wcstombs\b0  (\i char *string, const wchar_t *wstring, size_t size\i0 )
\par 
\par \pard\li500 The \f1 wcstombs\f0  ("wide character string to multibyte string") function converts the null-terminated wide character array \i wstring\i0  into a string containing multibyte characters, storing not more than\i  size\i0  bytes starting at \i string\i0 , followed by a terminating null character if there is room. The conversion of characters begins in the initial shift state. 
\par 
\par The terminating null character counts towards the size, so if \i size\i0  is less than or equal to the number of bytes needed in \i wstring\i0 , no terminating null character is stored. 
\par 
\par If a code that does not correspond to a valid multibyte character is found, the \f1 wcstombs\f0  function returns a value of -1. Otherwise, the return value is the number of bytes stored in the array\i  string\i0 . This number does not include the terminating null character, which is present if the number is less than \i size\i0 .
\par 
\par }
1560
PAGE_402
Normal Termination
exit



Imported



FALSE
25
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Normal Termination \{keepn\}
\par \pard\fs18 Next: \cf1\strike Exit Status\strike0\{linkID=710\}\cf0 , Up: \cf1\strike Program Termination\strike0\{linkID=1770\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Normal Termination\b0 
\par 
\par A process terminates normally when its program signals it is done by calling \f1 exit\f0 . Returning from \f1 main\f0  is equivalent to calling \f1 exit\f0 , and the value that \f1 main\f0  returns is used as the argument to \f1 exit\f0 .
\par 
\par -- Function: void \b exit\b0  (\i int status\i0 )
\par 
\par \pard\li500 The \f1 exit\f0  function tells the system that the program is done, which causes it to terminate the process. 
\par 
\par \i status\i0  is the program's exit status, which becomes part of the process' termination status. This function does not return.
\par 
\par \pard Normal termination causes the following actions:
\par 
\par \pard\li500 1. Functions that were registered with the \f1 atexit\f0  function are called in the reverse order of their registration. This mechanism allows your application to specify its own "cleanup" actions to be performed at program termination. Typically, this is used to do things like saving program state information in a file, or unlocking locks in shared data bases. 
\par 2. All open streams are closed, writing out any buffered output data. See \cf1\strike Closing Streams\strike0\{linkID=360\}\cf0 . In addition, temporary files opened with the \f1 tmpfile\f0  function are removed; see \cf1\strike Temporary Files\strike0\{linkID=2250\}\cf0 . 
\par 3.\f1  _exit\f0  is called, terminating the program.
\par 
\par }
1570
PAGE_403
Normalization Functions
normalizationfunctions(floating-point;math.h;frexp;frexpf;frexpl;ldexp;ldexpf;ldexpl;scalb;scalbf;scalbl;scalbn;scalbnf;scalbnl;scalbln;scalblnf;scalblnl;significand;significandf;significandl



Imported



FALSE
74
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Normalization Functions \{keepn\}
\par \pard\fs18 Next: \cf1\strike Rounding Functions\strike0\{linkID=1890\}\cf0 , Previous: \cf1\strike Absolute Value\strike0\{linkID=30\}\cf0 , Up: \cf1\strike Arithmetic Functions\strike0\{linkID=90\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Normalization Functions\b0 
\par 
\par The functions described in this section are primarily provided as a way to efficiently perform certain low-level manipulations on floating point numbers that are represented internally using a binary radix; see \cf1\strike Floating Point Concepts\strike0\{linkID=850\}\cf0 . These functions are required to have equivalent behavior even if the representation does not use a radix of 2, but of course they are unlikely to be particularly efficient in those cases. 
\par 
\par 
\par All these functions are declared in \f1 math.h\f0 .
\par -- Function: double \b frexp\b0  (\i double value, int *exponent\i0 )
\par 
\par \pard\li500 -- Function: float \b frexpf\b0  (\i float value, int *exponent\i0 )
\par 
\par \pard\li1000 -- Function: long double \b frexpl\b0  (\i long double value, int *exponent\i0 )
\par 
\par \pard\li1500 These functions are used to split the number \i value\i0  into a normalized fraction and an exponent. 
\par 
\par If the argument \i value\i0  is not zero, the return value is \i value\i0  times a power of two, and is always in the range 1/2 (inclusive) to 1 (exclusive). The corresponding exponent is stored in\f1  *\i\f0 exponent\i0 ; the return value multiplied by 2 raised to this exponent equals the original number \i value\i0 . 
\par 
\par For example, \f1 frexp (12.8, &exponent)\f0  returns \f1 0.8\f0  and stores \f1 4\f0  in \f1 exponent\f0 . 
\par 
\par If \i value\i0  is zero, then the return value is zero and zero is stored in \f1 *\i\f0 exponent\i0 .
\par 
\par \pard -- Function: double \b ldexp\b0  (\i double value, int exponent\i0 )
\par 
\par \pard\li500 -- Function: float \b ldexpf\b0  (\i float value, int exponent\i0 )
\par 
\par \pard\li1000 -- Function: long double \b ldexpl\b0  (\i long double value, int exponent\i0 )
\par 
\par \pard\li1500 These functions return the result of multiplying the floating-point number \i value\i0  by 2 raised to the power \i exponent\i0 . (It can be used to reassemble floating-point numbers that were taken apart by \f1 frexp\f0 .) 
\par 
\par For example, \f1 ldexp (0.8, 4)\f0  returns \f1 12.8\f0 .
\par 
\par \pard The following functions, which come from BSD, provide facilities equivalent to those of \f1 ldexp\f0  and \f1 frexp\f0 . See also the ISO C function \f1 logb\f0  which originally also appeared in BSD.
\par -- Function: double \b scalb\b0  (\i double value, int exponent\i0 )
\par 
\par \pard\li500 -- Function: float \b scalbf\b0  (\i float value, int exponent\i0 )
\par 
\par \pard\li1000 -- Function: long double \b scalbl\b0  (\i long double value, int exponent\i0 )
\par 
\par \pard\li1500 The \f1 scalb\f0  function is the BSD name for \f1 ldexp\f0 .
\par 
\par \pard -- Function: long long int \b scalbn\b0  (\i double x, int n\i0 )
\par 
\par \pard\li500 -- Function: long long int \b scalbnf\b0  (\i float x, int n\i0 )
\par 
\par \pard\li1000 -- Function: long long int \b scalbnl\b0  (\i long double x, int n\i0 )
\par 
\par \pard\li1500\f1 scalbn\f0  is identical to \f1 scalb\f0 , except that the exponent\i  n\i0  is an \f1 int\f0  instead of a floating-point number.
\par 
\par \pard -- Function: long long int \b scalbln\b0  (\i double x, long int n\i0 )
\par 
\par \pard\li500 -- Function: long long int \b scalblnf\b0  (\i float x, long int n\i0 )
\par 
\par \pard\li1000 -- Function: long long int \b scalblnl\b0  (\i long double x, long int n\i0 )
\par 
\par \pard\li1500\f1 scalbln\f0  is identical to \f1 scalb\f0 , except that the exponent\i  n\i0  is a \f1 long int\f0  instead of a floating-point number.
\par 
\par \pard -- Function: long long int \b significand\b0  (\i double x\i0 )
\par 
\par \pard\li500 -- Function: long long int \b significandf\b0  (\i float x\i0 )
\par 
\par \pard\li1000 -- Function: long long int \b significandl\b0  (\i long double x\i0 )
\par 
\par \pard\li1500\f1 significand\f0  returns the mantissa of \i x\i0  scaled to the range [1, 2). It is equivalent to \f1 scalb (\i\f0 x\i0\f1 , (double) -ilogb (\i\f0 x\i0\f1 ))\f0 . 
\par 
\par This function exists mainly for use in certain standardized tests of IEEE 754 conformance.
\par 
\par }
1580
PAGE_411
Null Pointer Constant
nullpointerconstant;NULL



Imported



FALSE
21
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Null Pointer Constant \{keepn\}
\par \pard\fs18 Next: \cf1\strike Important Data Types\strike0\{linkID=1150\}\cf0 , Previous: \cf1\strike Variadic Functions\strike0\{linkID=2420\}\cf0 , Up: \cf1\strike Language Features\strike0\{linkID=1310\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Null Pointer Constant\b0 
\par 
\par The null pointer constant is guaranteed not to point to any real object. You can assign it to any pointer variable since it has type \f1 void *\f0 . The preferred way to write a null pointer constant is with\f1  NULL\f0 .
\par 
\par -- Macro: void * \b NULL\b0 
\par 
\par \pard\li500 This is a null pointer constant.
\par 
\par \pard You can also use \f1 0\f0  or \f1 (void *)0\f0  as a null pointer constant, but using \f1 NULL\f0  is cleaner because it makes the purpose of the constant more evident. 
\par 
\par If you use the null pointer constant as a function argument, then for complete portability you should make sure that the function has a prototype declaration. Otherwise, if the target machine has two different pointer representations, the compiler won't know which representation to use for that argument. You can avoid the problem by explicitly casting the constant to the proper pointer type, but we recommend instead adding a prototype for the function you are calling.
\par 
\par }
1590
PAGE_412
Numeric Input Conversions




Imported



FALSE
63
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Numeric Input Conversions \{keepn\}
\par \pard\fs18 Next: \cf1\strike String Input Conversions\strike0\{linkID=2170\}\cf0 , Previous: \cf1\strike Table of Input Conversions\strike0\{linkID=2230\}\cf0 , Up: \cf1\strike Formatted Input\strike0\{linkID=940\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Numeric Input Conversions\b0 
\par 
\par This section describes the \f1 scanf\f0  conversions for reading numeric values. 
\par 
\par The `\f1 %d\f0 ' conversion matches an optionally signed integer in decimal radix. The syntax that is recognized is the same as that for the\f1  strtol\f0  function (see \cf1\strike Parsing of Integers\strike0\{linkID=1700\}\cf0 ) with the value\f1  10\f0  for the \i base\i0  argument. 
\par 
\par The `\f1 %i\f0 ' conversion matches an optionally signed integer in any of the formats that the C language defines for specifying an integer constant. The syntax that is recognized is the same as that for the\f1  strtol\f0  function (see \cf1\strike Parsing of Integers\strike0\{linkID=1700\}\cf0 ) with the value\f1  0\f0  for the \i base\i0  argument. (You can print integers in this syntax with \f1 printf\f0  by using the `\f1 #\f0 ' flag character with the `\f1 %x\f0 ', `\f1 %o\f0 ', or `\f1 %d\f0 ' conversion. See \cf1\strike Integer Conversions\strike0\{linkID=1180\}\cf0 .) 
\par 
\par For example, any of the strings `\f1 10\f0 ', `\f1 0xa\f0 ', or `\f1 012\f0 ' could be read in as integers under the `\f1 %i\f0 ' conversion. Each of these specifies a number with decimal value \f1 10\f0 . 
\par 
\par The `\f1 %o\f0 ', `\f1 %u\f0 ', and `\f1 %x\f0 ' conversions match unsigned integers in octal, decimal, and hexadecimal radices, respectively. The syntax that is recognized is the same as that for the \f1 strtoul\f0  function (see \cf1\strike Parsing of Integers\strike0\{linkID=1700\}\cf0 ) with the appropriate value (\f1 8\f0 , \f1 10\f0 , or \f1 16\f0 ) for the \i base\i0  argument. 
\par 
\par The `\f1 %X\f0 ' conversion is identical to the `\f1 %x\f0 ' conversion. They both permit either uppercase or lowercase letters to be used as digits. 
\par 
\par The default type of the corresponding argument for the \f1 %d\f0  and\f1  %i\f0  conversions is \f1 int *\f0 , and \f1 unsigned int *\f0  for the other integer conversions. You can use the following type modifiers to specify other sizes of integer:
\par 
\par `\f1 hh\f0 '
\par \pard\li500 Specifies that the argument is a \f1 signed char *\f0  or \f1 unsigned char *\f0 . 
\par 
\par This modifier was introduced in ISO C99. 
\par \pard `\f1 h\f0 '
\par \pard\li500 Specifies that the argument is a \f1 short int *\f0  or \f1 unsigned short int *\f0 . 
\par \pard `\f1 j\f0 '
\par \pard\li500 Specifies that the argument is a \f1 intmax_t *\f0  or \f1 uintmax_t *\f0 . 
\par 
\par This modifier was introduced in ISO C99. 
\par \pard `\f1 l\f0 '
\par \pard\li500 Specifies that the argument is a \f1 long int *\f0  or \f1 unsigned long int *\f0 . Two `\f1 l\f0 ' characters is like the `\f1 L\f0 ' modifier, below. 
\par 
\par If used with `\f1 %c\f0 ' or `\f1 %s\f0 ' the corresponding parameter is considered as a pointer to a wide character or wide character string respectively. This use of `\f1 l\f0 ' was introduced in Amendment 1 to ISO C90. 
\par \pard `\f1 ll\f0 '
\par \pard\li500 Specifies that the argument is a \f1 long long int *\f0  or \f1 unsigned long long int *\f0 . 
\par \pard `\f1 t\f0 '
\par \pard\li500 Specifies that the argument is a \f1 ptrdiff_t *\f0 . 
\par 
\par This modifier was introduced in ISO C99. 
\par \pard `\f1 z\f0 '
\par \pard\li500 Specifies that the argument is a \f1 size_t *\f0 . 
\par 
\par This modifier was introduced in ISO C99.
\par 
\par \pard All of the `\f1 %e\f0 ', `\f1 %f\f0 ', `\f1 %g\f0 ', `\f1 %E\f0 ', and `\f1 %G\f0 ' input conversions are interchangeable. They all match an optionally signed floating point number, in the same syntax as for the\f1  strtod\f0  function (see \cf1\strike Parsing of Floats\strike0\{linkID=1690\}\cf0 ). 
\par 
\par For the floating-point input conversions, the default argument type is\f1  float *\f0 . (This is different from the corresponding output conversions, where the default type is \f1 double\f0 ; remember that\f1  float\f0  arguments to \f1 printf\f0  are converted to \f1 double\f0  by the default argument promotions, but \f1 float *\f0  arguments are not promoted to \f1 double *\f0 .) You can specify other sizes of float using these type modifiers:
\par 
\par `\f1 l\f0 '
\par \pard\li500 Specifies that the argument is of type \f1 double *\f0 . 
\par \pard `\f1 L\f0 '
\par \pard\li500 Specifies that the argument is of type \f1 long double *\f0 .
\par 
\par \pard For all the above number parsing formats there is an additional optional flag `\f1 '\f0 '. When this flag is given the \f1 scanf\f0  function expects the number represented in the input string to be formatted according to the grouping rules of the currently selected locale (see \cf1\strike General Numeric\strike0\{linkID=1040\}\cf0 ). 
\par 
\par If the \f1 "C"\f0  or \f1 "POSIX"\f0  locale is selected there is no difference. But for a locale which specifies values for the appropriate fields in the locale the input must have the correct form in the input. Otherwise the longest prefix with a correct form is processed.
\par 
\par }
1600
PAGE_420
Opening and Closing Files
openingafiledescriptor;closingafiledescriptor;unistd.h;fcntl.h;open;open;creat;creat;close



Imported



FALSE
69
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fswiss\fcharset0 Arial;}{\f3\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Opening and Closing Files \{keepn\}
\par \pard\fs18 Next: \cf1\strike I/O Primitives\strike0\{linkID=1280\}\cf0 , Up: \cf1\strike Low-Level I/O\strike0\{linkID=1360\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Opening and Closing Files\b0 
\par 
\par This section describes the primitives for opening and closing files using file descriptors. The \f1 open\f0  and \f1 creat\f0  functions are declared in the header file \f1 fcntl.h\f0 , while \f1 close\f0  is declared in \f1 unistd.h\f0 .\f2 
\par \f0 
\par -- Function: int \b open\b0  (\i const char *filename, int flags\i0 [\i , mode_t mode\i0 ])
\par 
\par \pard\li500 The \f1 open\f0  function creates and returns a new file descriptor for the file named by \i filename\i0 . Initially, the file position indicator for the file is at the beginning of the file. The argument\i  mode\i0  is used only when a file is created, but it doesn't hurt to supply the argument in any case. 
\par 
\par The \i flags\i0  argument controls how the file is to be opened. This is a bit mask; you create the value by the bitwise OR of the appropriate parameters (using the `\f1 |\f0 ' operator in C). See \cf1\strike File Status Flags\strike0\{linkID=810\}\cf0 , for the parameters available. 
\par 
\par The normal return value from \f1 open\f0  is a non-negative integer file descriptor. In the case of an error, a value of -1 is returned instead. In addition to the usual file name errors (see \cf1\strike File Name Errors\strike0\{linkID=740\}\cf0 ), the following \f1 errno\f0  error conditions are defined for this function:
\par 
\par \f1 EACCES\f0 
\par \pard\li1000 The file exists but is not readable/writable as requested by the \i flags\i0  argument, the file does not exist and the directory is unwritable so it cannot be created. 
\par \pard\li500\f1 EEXIST\f0 
\par \pard\li1000 Both \f1 O_CREAT\f0  and \f1 O_EXCL\f0  are set, and the named file already exists. 
\par \pard\li500\f1 EMFILE\f0 
\par \f2            \f0 The process has too many files open. 
\par \f1 ENFILE\f0 
\par \pard\li1000 The entire system, or perhaps the file system which contains the directory, cannot support any additional open files at the moment. (This problem cannot happen on the GNU system.) 
\par \pard\li500\f1 ENOENT\f0 
\par \pard\li1000 The named file does not exist, and \f1 O_CREAT\f0  is not specified. 
\par \pard\li500\f1 ENOSPC\f0 
\par \pard\li1000 The directory or file system that would contain the new file cannot be extended, because there is no disk space left. 
\par \pard\li500\f1 ENXIO\f0 
\par \pard\li1000\f1 O_NONBLOCK\f0  and \f1 O_WRONLY\f0  are both set in the \i flags\i0  argument, the file named by \i filename\i0  is a FIFO (see \cf1\strike Pipes and FIFOs\strike0\{link=PAGE_454\}\cf0 ), and no process has the file open for reading. 
\par \pard\li500\f1 EROFS\f0 
\par \pard\li1000 The file resides on a read-only file system and any of \f1 O_WRONLY\f0 ,\f1  O_RDWR\f0 , and \f1 O_TRUNC\f0  are set in the \i flags\i0  argument, or \f1 O_CREAT\f0  is set and the file does not already exist.
\par \pard\li500 
\par \pard -- Obsolete function: int \b creat\b0  (\i const char *filename, mode_t mode\i0 )
\par 
\par \pard\li500 This function is obsolete. The call: 
\par 
\par \f1           creat (\i filename\i0 , \i mode\i0 )
\par      
\par \f0 is equivalent to: 
\par 
\par \f1           open (\i filename\i0 , O_WRONLY | O_CREAT | O_TRUNC, \i mode\i0 )
\par      \f0 
\par \pard -- Function: int \b close\b0  (\i int filedes\i0 )
\par 
\par \pard\li500 The function \f1 close\f0  closes the file descriptor \i filedes\i0 . Closing a file has the following consequences:
\par 
\par \pard\li1000\f3\'b7\f0  The file descriptor is deallocated. 
\par \f3\'b7\f0  Any record locks owned by the process on the file are unlocked. 
\par \f3\'b7\f0  When all file descriptors associated with a pipe or FIFO have been closed, any unread data is discarded.
\par 
\par \pard\li500 This function is a cancellation point in multi-threaded programs. This is a problem if the thread allocates some resources (like memory, file descriptors, semaphores or whatever) at the time \f1 close\f0  is called. If the thread gets canceled these resources stay allocated until the program ends. To avoid this, calls to \f1 close\f0  should be protected using cancellation handlers. 
\par 
\par The normal return value from \f1 close\f0  is 0; a value of -1 is returned in case of failure. The following \f1 errno\f0  error conditions are defined for this function:
\par 
\par \f1 EBADF\f0 
\par \pard\li1000 The \i filedes\i0  argument is not a valid file descriptor. 
\par \pard\li500\f1 ENOSPC\f0 
\par \f1 EIO\f0 
\par \f1 EDQUOT\f0 
\par \pard\li1000 When the file is accessed by NFS, these errors from \f1 write\f0  can sometimes not be detected until \f1 close\f0 . See \cf1\strike I/O Primitives\strike0\{linkID=1280\}\cf0 , for details on their meaning.
\par 
\par \pard To close a stream, call \f1 fclose\f0  (see \cf1\strike Closing Streams\strike0\{linkID=360\}\cf0 ) instead of trying to close its underlying file descriptor with \f1 close\f0 . This flushes any buffered output and updates the stream object to indicate that it is closed.
\par 
\par }
1610
PAGE_421
Opening Streams
openingastream;stdio.h;fopen;streamorientation;orientation,stream;fopen;FOPEN_MAX;freopen;freopen;gt__freadable;gt__fwritable;gt__freading;gt__fwriting



Imported



FALSE
60
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Opening Streams \{keepn\}
\par \pard\fs18 Next: \cf1\strike Closing Streams\strike0\{linkID=360\}\cf0 , Previous: \cf1\strike Standard Streams\strike0\{linkID=2100\}\cf0 , Up: \cf1\strike I/O on Streams\strike0\{linkID=1260\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Opening Streams\b0 
\par 
\par Opening a file with the \f1 fopen\f0  function creates a new stream and establishes a connection between the stream and a file. This may involve creating a new file. 
\par 
\par Everything described in this section is declared in the header file\f1  stdio.h\f0 .
\par 
\par -- Function: FILE * \b fopen\b0  (\i const char *filename, const char *opentype\i0 )
\par 
\par \pard\li500 The \f1 fopen\f0  function opens a stream for I/O to the file\i  filename\i0 , and returns a pointer to the stream. 
\par 
\par The \i opentype\i0  argument is a string that controls how the file is opened and specifies attributes of the resulting stream. It must begin with one of the following sequences of characters:
\par 
\par `\f1 r\f0 '
\par \pard\li1000 Open an existing file for reading only. 
\par \pard\li500 `\f1 w\f0 '
\par \pard\li1000 Open the file for writing only. If the file already exists, it is truncated to zero length. Otherwise a new file is created. 
\par \pard\li500 `\f1 a\f0 '
\par \pard\li1000 Open a file for append access; that is, writing at the end of file only. If the file already exists, its initial contents are unchanged and output to the stream is appended to the end of the file. Otherwise, a new, empty file is created. 
\par \pard\li500 `\f1 r+\f0 '
\par \pard\li1000 Open an existing file for both reading and writing. The initial contents of the file are unchanged and the initial file position is at the beginning of the file. 
\par \pard\li500 `\f1 w+\f0 '
\par \pard\li1000 Open a file for both reading and writing. If the file already exists, it is truncated to zero length. Otherwise, a new file is created. 
\par \pard\li500 `\f1 a+\f0 '
\par \pard\li1000 Open or create file for both reading and appending. If the file exists, its initial contents are unchanged. Otherwise, a new file is created. The initial file position for reading is at the beginning of the file, but output is always appended to the end of the file.
\par 
\par \pard\li500 As you can see, `\f1 +\f0 ' requests a stream that can do both input and output. The ISO standard says that when using such a stream, you must call \f1 fflush\f0  (see \cf1\strike Stream Buffering\strike0\{linkID=2120\}\cf0 ) or a file positioning function such as \f1 fseek\f0  (see \cf1\strike File Positioning\strike0\{linkID=790\}\cf0 ) when switching from reading to writing or vice versa. Otherwise, internal buffers might not be emptied properly. 
\par 
\par Additional characters may appear after these to specify flags for the call. Always put the mode (`\f1 r\f0 ', `\f1 w+\f0 ', etc.) first; that is the only part you are guaranteed will be understood by all systems. 
\par 
\par The character `\f1 b\f0 ' in \i opentype\i0  has a standard meaning; it requests a binary stream rather than a text stream. But this makes no difference in POSIX systems (including the GNU system). If both `\f1 +\f0 ' and `\f1 b\f0 ' are specified, they can appear in either order. See \cf1\strike Binary Streams\strike0\{linkID=190\}\cf0 . 
\par 
\par \f2 The\f0  stream is opened initially unoriented and the orientation is decided with the first file operation. If the first operation is a wide character operation, the stream is not only marked as wide-oriented, also the conversion functions to convert to the coded character set used for the current locale are loaded. This will not change anymore from this point on even if the locale selected for the \f1 LC_CTYPE\f0  category is changed. 
\par 
\par Any other characters in \i opentype\i0  are simply ignored. They may be meaningful in other systems. 
\par 
\par If the open fails, \f1 fopen\f0  returns a null pointer. 
\par 
\par You can have multiple streams (or file descriptors) pointing to the same file open at the same time. If you do only input, this works straightforwardly, but you must be careful if any output streams are included. See \cf1\strike Stream/Descriptor Precautions\strike0\{link=PAGE_587\}\cf0 . This is equally true whether the streams are in one program (not usual) or in several programs (which can easily happen). 
\par 
\par \pard -- Macro: int \b FOPEN_MAX\b0 
\par 
\par \pard\li500 The value of this macro is an integer constant expression that represents the minimum number of streams that the implementation guarantees can be open simultaneously. You might be able to open more than this many streams, but that is not guaranteed. The value of this constant is at least eight, which includes the three standard streams\f1  stdin\f0 , \f1 stdout\f0 , and \f1 stderr\f0 . In POSIX.1 systems this value is determined by the \f1 OPEN_MAX\f0  parameter; see \cf1\strike General Limits\strike0\{link=PAGE_240\}\cf0 . In BSD and GNU, it is controlled by the \f1 RLIMIT_NOFILE\f0  resource limit; see \cf1\strike Limits on Resources\strike0\{link=PAGE_327\}\cf0 .
\par 
\par \pard -- Function: FILE * \b freopen\b0  (\i const char *filename, const char *opentype, FILE *stream\i0 )
\par 
\par \pard\li500 This function is like a combination of \f1 fclose\f0  and \f1 fopen\f0 . It first closes the stream referred to by \i stream\i0 , ignoring any errors that are detected in the process. (Because errors are ignored, you should not use \f1 freopen\f0  on an output stream if you have actually done any output using the stream.) Then the file named by\i  filename\i0  is opened with mode \i opentype\i0  as for \f1 fopen\f0 , and associated with the same stream object \i stream\i0 . 
\par 
\par If the operation fails, a null pointer is returned; otherwise,\f1  freopen\f0  returns \i stream\i0 . 
\par 
\par \f1 freopen\f0  has traditionally been used to connect a standard stream such as \f1 stdin\f0  with a file of your own choice. This is useful in programs in which use of a standard stream for certain purposes is hard-coded. In th\f2 is\f0  C library, you can simply close the standard streams and open new ones with \f1 fopen\f0 . But other systems lack this ability, so using \f1 freopen\f0  is more portable. 
\par 
\par }
1620
PAGE_423
Open-time Flags
filenametranslationflags;flags,filenametranslation;open-timeactionflags;flags,open-timeaction;O_CREAT;createonopen(filestatusflag;O_EXCL;O_NONBLOCK;non-blockingopen;O_NOCTTY;controllingterminal,setting;O_IGNORE_CTTY;O_NOLINK;symboliclink,opening;O_NOTRANS;O_TRUNC;O_SHLOCK;O_EXLOCK



Imported



FALSE
36
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Open-time Flags \{keepn\}
\par \pard\fs18 Next: \cf1\strike Operating Modes\strike0\{linkID=1630\}\cf0 , Previous: \cf1\strike Access Modes\strike0\{linkID=40\}\cf0 , Up: \cf1\strike File Status Flags\strike0\{linkID=810\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Open-time Flags\b0 
\par 
\par The open-time flags specify options affecting how \f1 open\f0  will behave. These options are not preserved once the file is open. See \cf1\strike Opening and Closing Files\strike0\{linkID=1600\}\cf0 , for how to call\f1  open\f0 . 
\par 
\par There are two sorts of options specified by open-time flags.
\par 
\par \pard\li500\f2\'b7\i\f0  File name translation flags\i0  affect how \f1 open\f0  looks up the file name to locate the file, and whether the file can be created.
\par \f2\'b7\i\f0  Open-time action flags\i0  specify extra operations that \f1 open\f0  will perform on the file once it is open.
\par 
\par \pard Here are the file name translation flags.
\par 
\par -- Macro: int \b O_CREAT\b0 
\par 
\par \pard\li500 If set, the file will be created if it doesn't already exist.
\par 
\par \pard -- Macro: int \b O_EXCL\b0 
\par 
\par \pard\li500 If both \f1 O_CREAT\f0  and \f1 O_EXCL\f0  are set, then \f1 open\f0  fails if the specified file already exists. This is guaranteed to never clobber an existing file.
\par 
\par \pard The open-time action flags tell \f1 open\f0  to do additional operations which are not really related to opening the file. The reason to do them as part of \f1 open\f0  instead of in separate calls is that \f1 open\f0  can do them \i atomically\i0 .
\par 
\par -- Macro: int \b O_TRUNC\b0 
\par 
\par \pard\li500 Truncate the file to zero length. This option is only useful for regular files, not special files such as directories or FIFOs. POSIX.1 requires that you open the file for writing to use \f1 O_TRUNC\f0 . In BSD and GNU you must have permission to write the file to truncate it, but you need not open for write access. 
\par 
\par This is the only open-time action flag specified by POSIX.1. There is no good reason for truncation to be done by \f1 open\f0 , instead of by calling \f1 ftruncate\f0  afterwards. The \f1 O_TRUNC\f0  flag existed in Unix before \f1 ftruncate\f0  was invented, and is retained for backward compatibility.
\par 
\par }
1630
PAGE_425
Operating Modes
O_APPEND;O_NONBLOCK;O_NDELAY;O_ASYNC;O_FSYNC;O_SYNC;O_NOATIME



Imported



FALSE
16
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Operating Modes \{keepn\}
\par \pard\fs18 Previous: \cf1\strike Open-time Flags\strike0\{linkID=1620\}\cf0 , Up: \cf1\strike File Status Flags\strike0\{linkID=810\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b I/O Operating Modes\b0 
\par 
\par The operating modes affect how input and output operations using a file descriptor work. These flags are set by \f1 open\f0  and can be fetched and changed with \f1 fcntl\f0 .
\par -- Macro: int \b O_APPEND\b0 
\par 
\par \pard\li500 The bit that enables append mode for the file. If set, then all\f1  write\f0  operations write the data at the end of the file, extending it, regardless of the current file position. This is the only reliable way to append to a file. In append mode, you are guaranteed that the data you write will always go to the current end of the file, regardless of other processes writing to the file. Conversely, if you simply set the file position to the end of file and write, then another process can extend the file after you set the file position but before you write, resulting in your data appearing someplace before the real end of file.
\par 
\par }
1650
PAGE_427
Operations on Complex
projectcomplexnumbers;conjugatecomplexnumbers;decomposecomplexnumbers;complex.h;creal;crealf;creall;cimag;cimagf;cimagl;conj;conjf;conjl;carg;cargf;cargl;cproj;cprojf;cprojl



Imported



FALSE
57
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Operations on Complex \{keepn\}
\par \pard\fs18 Next: \cf1\strike Parsing of Numbers\strike0\{linkID=1710\}\cf0 , Previous: \cf1\strike Complex Numbers\strike0\{linkID=390\}\cf0 , Up: \cf1\strike Arithmetic\strike0\{linkID=100\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Projections, Conjugates, and Decomposing of Complex Numbers\b0 
\par 
\par ISO C99 also defines functions that perform basic operations on complex numbers, such as decomposition and conjugation. The prototypes for all these functions are in \f1 complex.h\f0 . All functions are available in three variants, one for each of the three complex types.
\par 
\par -- Function: double \b creal\b0  (\i complex double z\i0 )
\par 
\par \pard\li500 -- Function: float \b crealf\b0  (\i complex float z\i0 )
\par 
\par \pard\li1000 -- Function: long double \b creall\b0  (\i complex long double z\i0 )
\par 
\par \pard\li1500 These functions return the real part of the complex number \i z\i0 .
\par 
\par \pard -- Function: double \b cimag\b0  (\i complex double z\i0 )
\par 
\par \pard\li500 -- Function: float \b cimagf\b0  (\i complex float z\i0 )
\par 
\par \pard\li1000 -- Function: long double \b cimagl\b0  (\i complex long double z\i0 )
\par 
\par \pard\li1500 These functions return the imaginary part of the complex number \i z\i0 .
\par 
\par \pard -- Function: complex double \b conj\b0  (\i complex double z\i0 )
\par 
\par \pard\li500 -- Function: complex float \b conjf\b0  (\i complex float z\i0 )
\par 
\par \pard\li1000 -- Function: complex long double \b conjl\b0  (\i complex long double z\i0 )
\par 
\par \pard\li1500 These functions return the conjugate value of the complex number\i  z\i0 . The conjugate of a complex number has the same real part and a negated imaginary part. In other words, `\f1 conj(a + bi) = a + -bi\f0 '.
\par 
\par \pard -- Function: double \b carg\b0  (\i complex double z\i0 )
\par 
\par \pard\li500 -- Function: float \b cargf\b0  (\i complex float z\i0 )
\par 
\par \pard\li1000 -- Function: long double \b cargl\b0  (\i complex long double z\i0 )
\par 
\par \pard\li1500 These functions return the argument of the complex number \i z\i0 . The argument of a complex number is the angle in the complex plane between the positive real axis and a line passing through zero and the number. This angle is measured in the usual fashion and ranges from 0 to 2&pi;. 
\par 
\par \f1 carg\f0  has a branch cut along the positive real axis.
\par 
\par \pard -- Function: complex double \b cproj\b0  (\i complex double z\i0 )
\par 
\par \pard\li500 -- Function: complex float \b cprojf\b0  (\i complex float z\i0 )
\par 
\par \pard\li1000 -- Function: complex long double \b cprojl\b0  (\i complex long double z\i0 )
\par 
\par \pard\li1500 These functions return the projection of the complex value \i z\i0  onto the Riemann sphere. Values with a infinite imaginary part are projected to positive infinity on the real axis, even if the real part is NaN. If the real part is infinite, the result is equivalent to 
\par 
\par \f1           INFINITY + I * copysign (0.0, cimag (z))
\par      
\par }
1660
PAGE_431
Other Input Conversions




Imported



FALSE
23
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Other Input Conversions \{keepn\}
\par \pard\fs18 Next: \cf1\strike Formatted Input Functions\strike0\{linkID=930\}\cf0 , Previous: \cf1\strike String Input Conversions\cf2\strike0\{linkID=2170\}\cf0 , Up: \cf1\strike Formatted Input\strike0\{linkID=940\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Other Input Conversions\b0 
\par 
\par This section describes the miscellaneous input conversions. 
\par 
\par The `\f1 %p\f0 ' conversion is used to read a pointer value. It recognizes the same syntax used by the `\f1 %p\f0 ' output conversion for\f1  printf\f0  (see \cf1\strike Other Output Conversions\strike0\{linkID=1670\}\cf0 ); that is, a hexadecimal number just as the `\f1 %x\f0 ' conversion accepts. The corresponding argument should be of type \f1 void **\f0 ; that is, the address of a place to store a pointer. 
\par 
\par The resulting pointer value is not guaranteed to be valid if it was not originally written during the same program execution that reads it in. 
\par 
\par The `\f1 %n\f0 ' conversion produces the number of characters read so far by this call. The corresponding argument should be of type \f1 int *\f0 . This conversion works in the same way as the `\f1 %n\f0 ' conversion for\f1  printf\f0 ; see \cf1\strike Other Output Conversions\strike0\{linkID=1670\}\cf0 , for an example. 
\par 
\par The `\f1 %n\f0 ' conversion is the only mechanism for determining the success of literal matches or conversions with suppressed assignments. If the `\f1 %n\f0 ' follows the locus of a matching failure, then no value is stored for it since \f1 scanf\f0  returns before processing the `\f1 %n\f0 '. If you store \f1 -1\f0  in that argument slot before calling\f1  scanf\f0 , the presence of \f1 -1\f0  after \f1 scanf\f0  indicates an error occurred before the `\f1 %n\f0 ' was reached. 
\par 
\par Finally, the `\f1 %%\f0 ' conversion matches a literal `\f1 %\f0 ' character in the input stream, without using an argument. This conversion does not permit any flags, field width, or type modifier to be specified.
\par 
\par }
1670
PAGE_434
Other Output Conversions




Imported



FALSE
58
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 + Other Output Conversions \{keepn\}
\par \pard\fs18 Next: \cf1\strike Formatted Output Functions\strike0\{linkID=960\}\cf0 , Previous: \cf1\strike Floating-Point Conversions\strike0\{linkID=900\}\cf0 , Up: \cf1\strike Formatted Output\strike0\{linkID=970\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Other Output Conversions\b0 
\par 
\par This section describes miscellaneous conversions for \f1 printf\f0 . 
\par 
\par The `\f1 %c\f0 ' conversion prints a single character. In case there is no `\f1 l\f0 ' modifier the \f1 int\f0  argument is first converted to an\f1  unsigned char\f0 . Then, if used in a wide stream function, the character is converted into the corresponding wide character. The `\f1 -\f0 ' flag can be used to specify left-justification in the field, but no other flags are defined, and no precision or type modifier can be given. For example:
\par 
\par \f1      printf ("%c%c%c%c%c", 'h', 'e', 'l', 'l', 'o');
\par \f0 
\par prints `\f1 hello\f0 '. 
\par 
\par If there is a `\f1 l\f0 ' modifier present the argument is expected to be of type \f1 wint_t\f0 . If used in a multibyte function the wide character is converted into a multibyte character before being added to the output. In this case more than one output byte can be produced. 
\par 
\par The `\f1 %s\f0 ' conversion prints a string. If no `\f1 l\f0 ' modifier is present the corresponding argument must be of type \f1 char *\f0  (or\f1  const char *\f0 ). If used in a wide stream function the string is first converted in a wide character string. A precision can be specified to indicate the maximum number of characters to write; otherwise characters in the string up to but not including the terminating null character are written to the output stream. The `\f1 -\f0 ' flag can be used to specify left-justification in the field, but no other flags or type modifiers are defined for this conversion. For example:
\par 
\par \f1      printf ("%3s%-6s", "no", "where");
\par \f0 
\par prints `\f1  nowhere \f0 '. 
\par 
\par If there is a `\f1 l\f0 ' modifier present the argument is expected to be of type \f1 wchar_t\f0  (or \f1 const wchar_t *\f0 ). 
\par 
\par If you accidentally pass a null pointer as the argument for a `\f1 %s\f0 ' conversion, the GNU library prints it as `\f1 (null)\f0 '. We think this is more useful than crashing. But it's not good practice to pass a null argument intentionally. 
\par 
\par The `\f1 %p\f0 ' conversion prints a pointer value. The corresponding argument must be of type \f1 void *\f0 . In practice, you can use any type of pointer. 
\par 
\par In th\f2 is system, \f0 pointers are printed as unsigned integers, as if a `\f1 %#x\f0 ' conversion were used.
\par 
\par For example:
\par 
\par \f1      printf ("%p", "testing");
\par \f0 
\par prints `\f1 0x\f0 ' followed by a hexadecimal number--the address of the string constant \f1 "testing"\f0 . It does not print the word `\f1 testing\f0 '. 
\par 
\par You can supply the `\f1 -\f0 ' flag with the `\f1 %p\f0 ' conversion to specify left-justification, but no other flags, precision, or type modifiers are defined. 
\par 
\par The `\f1 %n\f0 ' conversion is unlike any of the other output conversions. It uses an argument which must be a pointer to an \f1 int\f0 , but instead of printing anything it stores the number of characters printed so far by this call at that location. The `\f1 h\f0 ' and `\f1 l\f0 ' type modifiers are permitted to specify that the argument is of type\f1  short int *\f0  or \f1 long int *\f0  instead of \f1 int *\f0 , but no flags, field width, or precision are permitted. 
\par 
\par For example,
\par 
\par \f1      int nchar;
\par      printf ("%d %s%n\\n", 3, "bears", &nchar);
\par \f0 
\par prints:
\par 
\par \f1      3 bears
\par \f0 
\par and sets \f1 nchar\f0  to \f1 7\f0 , because `\f1 3 bears\f0 ' is seven characters. 
\par 
\par The `\f1 %%\f0 ' conversion prints a literal `\f1 %\f0 ' character. This conversion doesn't use an argument, and no flags, field width, precision, or type modifiers are permitted.
\par 
\par }
1680
PAGE_436
Output Conversion Syntax
NL_ARGMAX;flagcharacter(printf);minimumfieldwidth(printf);precision(printf);typemodifiercharacter(printf)



Imported



FALSE
31
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Output Conversion Syntax \{keepn\}
\par \pard\fs18 Next: \cf1\strike Table of Output Conversions\strike0\{linkID=2240\}\cf0 , Previous: \cf1\strike Formatted Output Basics\strike0\{linkID=950\}\cf0 , Up: \cf1\strike Formatted Output\strike0\{linkID=970\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Output Conversion Syntax\b0 
\par 
\par This section provides details about the precise syntax of conversion specifications that can appear in a \f1 printf\f0  template string. 
\par 
\par Characters in the template string that are not part of a conversion specification are printed as-is to the output stream. Multibyte character sequences (see \cf1\strike Character Set Handling\strike0\{linkID=290\}\cf0 ) are permitted in a template string. 
\par 
\par The conversion specifications in a \f1 printf\f0  template string have the general form:
\par 
\par \f1      % [ \i param-no\i0  $] \i flags\i0  \i width\i0  [ . \i precision\i0  ] \i type\i0  \i conversion
\par \i0\f0 
\par For example, in the conversion specifier `\f1 %-10.8ld\f0 ', the `\f1 -\f0 ' is a flag, `\f1 10\f0 ' specifies the field width, the precision is `\f1 8\f0 ', the letter `\f1 l\f0 ' is a type modifier, and `\f1 d\f0 ' specifies the conversion style. (This particular type specifier says to print a \f1 long int\f0  argument in decimal notation, with a minimum of 8 digits left-justified in a field at least 10 characters wide.) 
\par 
\par In more detail, output conversion specifications consist of an initial `\f1 %\f0 ' character followed in sequence by:
\par 
\par \pard\li500\f2\'b7\f0  Zero or more \i flag characters\i0  that modify the normal behavior of the conversion specification.
\par \f2\'b7\f0  An optional decimal integer specifying the \i minimum field width\i0 . If the normal conversion produces fewer characters than this, the field is padded with spaces to the specified width. This is a \i minimum\i0  value; if the normal conversion produces more characters than this, the field is \i not\i0  truncated. Normally, the output is right-justified within the field. You can also specify a field width of `\f1 *\f0 '. This means that the next argument in the argument list (before the actual value to be printed) is used as the field width. The value must be an \f1 int\f0 . If the value is negative, this means to set the `\f1 -\f0 ' flag (see below) and to use the absolute value as the field width. 
\par \f2\'b7\f0  An optional \i precision\i0  to specify the number of digits to be written for the numeric conversions. If the precision is specified, it consists of a period (`\f1 .\f0 ') followed optionally by a decimal integer (which defaults to zero if omitted). You can also specify a precision of `\f1 *\f0 '. This means that the next argument in the argument list (before the actual value to be printed) is used as the precision. The value must be an \f1 int\f0 , and is ignored if it is negative. If you specify `\f1 *\f0 ' for both the field width and precision, the field width argument precedes the precision argument. Other C library versions may not recognize this syntax. 
\par \f2\'b7\f0  An optional \i type modifier character\i0 , which is used to specify the data type of the corresponding argument if it differs from the default type. (For example, the integer conversions assume a type of \f1 int\f0 , but you can specify `\f1 h\f0 ', `\f1 l\f0 ', or `\f1 L\f0 ' for other integer types.)
\par \f2\'b7\f0  A character that specifies the conversion to be applied.
\par 
\par \pard The exact options that are permitted and how they are interpreted vary between the different conversion specifiers. See the descriptions of the individual conversions for information about the particular options that they use. 
\par 
\par }
1690
PAGE_443
Parsing of Floats
stdlib.h;strtod;strtof;strtold;wcstod;wcstof;wcstold;atof



Imported



FALSE
67
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Parsing of Floats \{keepn\}
\par \pard\fs18 Previous: \cf1\strike Parsing of Integers\strike0\{linkID=1700\}\cf0 , Up: \cf1\strike Parsing of Numbers\strike0\{linkID=1710\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Parsing of Floats\b0 
\par 
\par The `\f1 str\f0 ' functions are declared in \f1 stdlib.h\f0  and those beginning with `\f1 wcs\f0 ' are declared in \f1 wchar.h\f0 . One might wonder about the use of \f1 restrict\f0  in the prototypes of the functions in this section. It is seemingly useless but the ISO C standard uses it (for the functions defined there) so we have to do it as well.
\par 
\par -- Function: double \b strtod\b0  (\i const char *restrict string, char **restrict tailptr\i0 )
\par 
\par \pard\li500 The \f1 strtod\f0  ("string-to-double") function converts the initial part of \i string\i0  to a floating-point number, which is returned as a value of type \f1 double\f0 . 
\par 
\par This function attempts to decompose \i string\i0  as follows:
\par 
\par \pard\li1000\f2\'b7\f0  A (possibly empty) sequence of whitespace characters. Which characters are whitespace is determined by the \f1 isspace\f0  function (see \cf1\strike Classification of Characters\strike0\{linkID=330\}\cf0 ). These are discarded. 
\par \f2\'b7\f0  An optional plus or minus sign (`\f1 +\f0 ' or `\f1 -\f0 '). 
\par \f2\'b7\f0  A floating point number in decimal or hexadecimal format. The decimal format is:
\par 
\par \pard\li1500\f2\'b7\f0  A nonempty sequence of digits optionally containing a decimal-point character--normally `\f1 .\f0 ', but it depends on the locale (see \cf1\strike General Numeric\strike0\{linkID=1040\}\cf0 ). 
\par \f2\'b7\f0  An optional exponent part, consisting of a character `\f1 e\f0 ' or `\f1 E\f0 ', an optional sign, and a sequence of digits.
\par 
\par \pard\li1000 The hexadecimal format is as follows:
\par 
\par \pard\li1500\f2\'b7\f0  A 0x or 0X followed by a nonempty sequence of hexadecimal digits optionally containing a decimal-point character--normally `\f1 .\f0 ', but it depends on the locale (see \cf1\strike General Numeric\strike0\{linkID=1040\}\cf0 ). 
\par \f2\'b7\f0  An optional binary-exponent part, consisting of a character `\f1 p\f0 ' or `\f1 P\f0 ', an optional sign, and a sequence of digits.
\par 
\par \pard\li1000\f2\'b7\f0  Any remaining characters in the string. If \i tailptr\i0  is not a null pointer, a pointer to this tail of the string is stored in\f1  *\i\f0 tailptr\i0 .
\par 
\par \pard\li500 If the string is empty, contains only whitespace, or does not contain an initial substring that has the expected syntax for a floating-point number, no conversion is performed. In this case, \f1 strtod\f0  returns a value of zero and the value returned in \f1 *\i\f0 tailptr\i0  is the value of \i string\i0 . 
\par 
\par In a locale other than the standard \f1 "C"\f0  or \f1 "POSIX"\f0  locales, this function may recognize additional locale-dependent syntax. 
\par 
\par If the string has valid syntax for a floating-point number but the value is outside the range of a \f1 double\f0 , \f1 strtod\f0  will signal overflow or underflow as described in \cf1\strike Math Error Reporting\strike0\{linkID=1390\}\cf0 . 
\par 
\par \f1 strtod\f0  recognizes four special input strings. The strings\f1  "inf"\f0  and \f1 "infinity"\f0  are converted to &infin;, or to the largest representable value if the floating-point format doesn't support infinities. You can prepend a \f1 "+"\f0  or \f1 "-"\f0  to specify the sign. Case is ignored when scanning these strings. 
\par 
\par The strings \f1 "nan"\f0  and \f1 "nan(\i\f0 chars...\i0\f1 )"\f0  are converted to NaN. Again, case is ignored. If \i chars...\i0  are provided, they are used in some unspecified fashion to select a particular representation of NaN (there can be several). 
\par 
\par Since zero is a valid result as well as the value returned on error, you should check for errors in the same way as for \f1 strtol\f0 , by examining \i errno\i0  and \i tailptr\i0 .
\par 
\par \pard -- Function: float \b strtof\b0  (\i const char *string, char **tailptr\i0 )
\par 
\par \pard\li500 -- Function: long double \b strtold\b0  (\i const char *string, char **tailptr\i0 )
\par 
\par \pard\li1000 These functions are analogous to \f1 strtod\f0 , but return \f1 float\f0  and \f1 long double\f0  values respectively. They report errors in the same way as \f1 strtod\f0 . \f1 strtof\f0  can be substantially faster than \f1 strtod\f0 , but has less precision; conversely, \f1 strtold\f0  can be much slower but has more precision (on systems where \f1 long double\f0  is a separate type). 
\par 
\par These functions have been GNU extensions and are new to ISO C99.
\par 
\par \pard -- Function: double \b wcstod\b0  (\i const wchar_t *restrict string, wchar_t **restrict tailptr\i0 )
\par 
\par \pard\li500 -- Function: float \b wcstof\b0  (\i const wchar_t *string, wchar_t **tailptr\i0 )
\par 
\par \pard\li1000 -- Function: long double \b wcstold\b0  (\i const wchar_t *string, wchar_t **tailptr\i0 )
\par 
\par \pard\li1500 The \f1 wcstod\f0 , \f1 wcstof\f0 , and \f1 wcstol\f0  functions are equivalent in nearly all aspect to the \f1 strtod\f0 , \f1 strtof\f0 , and\f1  strtold\f0  functions but it handles wide character string. 
\par 
\par The \f1 wcstod\f0  function was introduced in Amendment 1 of ISO C90. The \f1 wcstof\f0  and \f1 wcstold\f0  functions were introduced in ISO C99.
\par 
\par \pard -- Function: double \b atof\b0  (\i const char *string\i0 )
\par 
\par \pard\li500 This function is similar to the \f1 strtod\f0  function, except that it need not detect overflow and underflow errors. The \f1 atof\f0  function is provided mostly for compatibility with existing code; using\f1  strtod\f0  is more robust.
\par 
\par }
1700
PAGE_444
Parsing of Integers
stdlib.h;wchar.h;strtol;wcstol;strtoul;wcstoul;strtoll;wcstoll;strtoq;wcstoq;strtoull;wcstoull;strtouq;wcstouq;strtoimax;wcstoimax;strtoumax;wcstoumax;atol;atoi;atoll



Imported



FALSE
179
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fmodern\fcharset0 Courier New;}{\f3\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Parsing of Integers \{keepn\}
\par \pard\fs18 Next: \cf1\strike Parsing of Floats\strike0\{linkID=1690\}\cf0 , Up: \cf1\strike Parsing of Numbers\strike0\{linkID=1710\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Parsing of Integers\b0 
\par 
\par The `\f1 str\f0 ' functions are declared in \f1 stdlib.h\f0  and those beginning with `\f1 wcs\f0 ' are declared in \f1 wchar.h\f0 . One might wonder about the use of \f1 restrict\f0  in the prototypes of the functions in this section. It is seemingly useless but the ISO C standard uses it (for the functions defined there) so we have to do it as well.
\par 
\par -- Function: long int \b strtol\b0  (\i const char *restrict string, char **restrict tailptr, int base\i0 )
\par 
\par \pard\li500 The \f1 strtol\f0  ("string-to-long") function converts the initial part of \i string\i0  to a signed integer, which is returned as a value of type \f1 long int\f0 . 
\par 
\par This function attempts to decompose \i string\i0  as follows:
\par 
\par \pard\li1000\f2\'b7\f0  A (possibly empty) sequence of whitespace characters. Which characters are whitespace is determined by the \f1 isspace\f0  function (see \cf1\strike Classification of Characters\strike0\{linkID=330\}\cf0 ). These are discarded. 
\par \f2\'b7\f0  An optional plus or minus sign (`\f1 +\f0 ' or `\f1 -\f0 '). 
\par \f2\'b7\f0  A nonempty sequence of digits in the radix specified by \i base\i0 . 
\par 
\par If \i base\i0  is zero, decimal radix is assumed unless the series of digits begins with `\f1 0\f0 ' (specifying octal radix), or `\f1 0x\f0 ' or `\f1 0X\f0 ' (specifying hexadecimal radix); in other words, the same syntax used for integer constants in C. 
\par 
\par Otherwise \i base\i0  must have a value between \f1 2\f0  and \f1 36\f0 . If \i base\i0  is \f1 16\f0 , the digits may optionally be preceded by `\f1 0x\f0 ' or `\f1 0X\f0 '. If base has no legal value the value returned is \f1 0l\f0  and the global variable \f1 errno\f0  is set to \f1 EINVAL\f0 . 
\par \f2\'b7\f0  Any remaining characters in the string. If \i tailptr\i0  is not a null pointer, \f1 strtol\f0  stores a pointer to this tail in\f1  *\i\f0 tailptr\i0 .
\par 
\par \pard\li500 If the string is empty, contains only whitespace, or does not contain an initial substring that has the expected syntax for an integer in the specified \i base\i0 , no conversion is performed. In this case,\f1  strtol\f0  returns a value of zero and the value stored in\f1  *\i\f0 tailptr\i0  is the value of \i string\i0 . 
\par 
\par In a locale other than the standard \f1 "C"\f0  locale, this function may recognize additional implementation-dependent syntax. 
\par 
\par If the string has valid syntax for an integer but the value is not representable because of overflow, \f1 strtol\f0  returns either\f1  LONG_MAX\f0  or \f1 LONG_MIN\f0  (see \cf1\strike Range of Type\strike0\{linkID=1790\}\cf0 ), as appropriate for the sign of the value. It also sets \f1 errno\f0  to \f1 ERANGE\f0  to indicate there was overflow. 
\par 
\par You should not check for errors by examining the return value of\f1  strtol\f0 , because the string might be a valid representation of\f1  0l\f0 , \f1 LONG_MAX\f0 , or \f1 LONG_MIN\f0 . Instead, check whether\i  tailptr\i0  points to what you expect after the number (e.g. \f1 '\\0'\f0  if the string should end after the number). You also need to clear \i errno\i0  before the call and check it afterward, in case there was overflow. 
\par 
\par There is an example at the end of this section.
\par 
\par \pard -- Function: long int \b wcstol\b0  (\i const wchar_t *restrict string, wchar_t **restrict tailptr, int base\i0 )
\par 
\par \pard\li500 The \f1 wcstol\f0  function is equivalent to the \f1 strtol\f0  function in nearly all aspects but handles wide character strings. 
\par 
\par The \f1 wcstol\f0  function was introduced in Amendment 1 of ISO C90.
\par 
\par \pard -- Function: unsigned long int \b strtoul\b0  (\i const char *retrict string, char **restrict tailptr, int base\i0 )
\par 
\par \pard\li500 The \f1 strtoul\f0  ("string-to-unsigned-long") function is like\f1  strtol\f0  except it converts to an \f1 unsigned long int\f0  value. The syntax is the same as described above for \f1 strtol\f0 . The value returned on overflow is \f1 ULONG_MAX\f0  (see \cf1\strike Range of Type\strike0\{linkID=1790\}\cf0 ). 
\par 
\par If \i string\i0  depicts a negative number, \f1 strtoul\f0  acts the same as \i strtol\i0  but casts the result to an unsigned integer. That means for example that \f1 strtoul\f0  on \f1 "-1"\f0  returns \f1 ULONG_MAX\f0  and an input more negative than \f1 LONG_MIN\f0  returns (\f1 ULONG_MAX\f0  + 1) / 2. 
\par 
\par \f1 strtoul\f0  sets \i errno\i0  to \f1 EINVAL\f0  if \i base\i0  is out of range, or \f1 ERANGE\f0  on overflow.
\par 
\par \pard -- Function: unsigned long int \b wcstoul\b0  (\i const wchar_t *restrict string, wchar_t **restrict tailptr, int base\i0 )
\par 
\par \pard\li500 The \f1 wcstoul\f0  function is equivalent to the \f1 strtoul\f0  function in nearly all aspects but handles wide character strings. 
\par 
\par The \f1 wcstoul\f0  function was introduced in Amendment 1 of ISO C90.
\par 
\par \pard -- Function: long long int \b strtoll\b0  (\i const char *restrict string, char **restrict tailptr, int base\i0 )
\par 
\par \pard\li500 The \f1 strtoll\f0  function is like \f1 strtol\f0  except that it returns a \f1 long long int\f0  value, and accepts numbers with a correspondingly larger range. 
\par 
\par If the string has valid syntax for an integer but the value is not representable because of overflow, \f1 strtoll\f0  returns either\f1  LONG_LONG_MAX\f0  or \f1 LONG_LONG_MIN\f0  (see \cf1\strike Range of Type\strike0\{linkID=1790\}\cf0 ), as appropriate for the sign of the value. It also sets \f1 errno\f0  to\f1  ERANGE\f0  to indicate there was overflow. 
\par 
\par The \f1 strtoll\f0  function was introduced in ISO C99.
\par 
\par \pard -- Function: long long int \b wcstoll\b0  (\i const wchar_t *restrict string, wchar_t **restrict tailptr, int base\i0 )
\par 
\par \pard\li500 The \f1 wcstoll\f0  function is equivalent to the \f1 strtoll\f0  function in nearly all aspects but handles wide character strings. 
\par 
\par The \f1 wcstoll\f0  function was introduced in Amendment 1 of ISO C90.
\par 
\par \pard -- Function: long long int \b strtoq\b0  (\i const char *restrict string, char **restrict tailptr, int base\i0 )
\par 
\par \pard\li500\f1 strtoq\f0  ("string-to-quad-word") is the BSD name for \f1 strtoll\f0 .
\par 
\par \pard -- Function: long long int \b wcstoq\b0  (\i const wchar_t *restrict string, wchar_t **restrict tailptr, int base\i0 )
\par 
\par \pard\li500 The \f1 wcstoq\f0  function is equivalent to the \f1 strtoq\f0  function in nearly all aspects but handles wide character strings. 
\par 
\par The \f1 wcstoq\f0  function is a GNU extension.
\par 
\par \pard -- Function: unsigned long long int \b strtoull\b0  (\i const char *restrict string, char **restrict tailptr, int base\i0 )
\par 
\par \pard\li500 The \f1 strtoull\f0  function is related to \f1 strtoll\f0  the same way\f1  strtoul\f0  is related to \f1 strtol\f0 . 
\par 
\par The \f1 strtoull\f0  function was introduced in ISO C99.
\par 
\par \pard -- Function: unsigned long long int \b wcstoull\b0  (\i const wchar_t *restrict string, wchar_t **restrict tailptr, int base\i0 )
\par 
\par \pard\li500 The \f1 wcstoull\f0  function is equivalent to the \f1 strtoull\f0  function in nearly all aspects but handles wide character strings. 
\par 
\par The \f1 wcstoull\f0  function was introduced in Amendment 1 of ISO C90.
\par 
\par \pard -- Function: unsigned long long int \b strtouq\b0  (\i const char *restrict string, char **restrict tailptr, int base\i0 )
\par 
\par \pard\li500\f1 strtouq\f0  is the BSD name for \f1 strtoull\f0 .
\par 
\par \pard -- Function: unsigned long long int \b wcstouq\b0  (\i const wchar_t *restrict string, wchar_t **restrict tailptr, int base\i0 )
\par 
\par \pard\li500 The \f1 wcstouq\f0  function is equivalent to the \f1 strtouq\f0  function in nearly all aspects but handles wide character strings. 
\par 
\par The \f1 wcstoq\f0  function is a GNU extension.
\par 
\par \pard -- Function: intmax_t \b strtoimax\b0  (\i const char *restrict string, char **restrict tailptr, int base\i0 )
\par 
\par \pard\li500 The \f1 strtoimax\f0  function is like \f1 strtol\f0  except that it returns a \f1 intmax_t\f0  value, and accepts numbers of a corresponding range. 
\par 
\par If the string has valid syntax for an integer but the value is not representable because of overflow, \f1 strtoimax\f0  returns either\f1  INTMAX_MAX\f0  or \f1 INTMAX_MIN\f0  (see \cf1\strike Integers\strike0\{linkID=1200\}\cf0 ), as appropriate for the sign of the value. It also sets \f1 errno\f0  to\f1  ERANGE\f0  to indicate there was overflow. 
\par 
\par See \cf1\strike Integers\strike0\{linkID=1200\}\cf0  for a description of the \f1 intmax_t\f0  type. The\f1  strtoimax\f0  function was introduced in ISO C99.
\par 
\par \pard -- Function: intmax_t \b wcstoimax\b0  (\i const wchar_t *restrict string, wchar_t **restrict tailptr, int base\i0 )
\par 
\par \pard\li500 The \f1 wcstoimax\f0  function is equivalent to the \f1 strtoimax\f0  function in nearly all aspects but handles wide character strings. 
\par 
\par The \f1 wcstoimax\f0  function was introduced in ISO C99.
\par 
\par \pard -- Function: uintmax_t \b strtoumax\b0  (\i const char *restrict string, char **restrict tailptr, int base\i0 )
\par 
\par \pard\li500 The \f1 strtoumax\f0  function is related to \f1 strtoimax\f0  the same way that \f1 strtoul\f0  is related to \f1 strtol\f0 . 
\par 
\par See \cf1\strike Integers\strike0\{linkID=1200\}\cf0  for a description of the \f1 intmax_t\f0  type. The\f1  strtoumax\f0  function was introduced in ISO C99.
\par 
\par \pard -- Function: uintmax_t \b wcstoumax\b0  (\i const wchar_t *restrict string, wchar_t **restrict tailptr, int base\i0 )
\par 
\par \pard\li500 The \f1 wcstoumax\f0  function is equivalent to the \f1 strtoumax\f0  function in nearly all aspects but handles wide character strings. 
\par 
\par The \f1 wcstoumax\f0  function was introduced in ISO C99.
\par 
\par \pard -- Function: long int \b atol\b0  (\i const char *string\i0 )
\par 
\par \pard\li500 This function is similar to the \f1 strtol\f0  function with a \i base\i0  argument of \f1 10\f0 , except that it need not detect overflow errors. The \f1 atol\f0  function is provided mostly for compatibility with existing code; using \f1 strtol\f0  is more robust.
\par 
\par \pard -- Function: int \b atoi\b0  (\i const char *string\i0 )
\par 
\par \pard\li500 This function is like \f1 atol\f0 , except that it returns an \f1 int\f0 . The \f1 atoi\f0  function is also considered obsolete; use \f1 strtol\f0  instead.
\par 
\par \pard -- Function: long long int \b atoll\b0  (\i const char *string\i0 )
\par 
\par \pard\li500 This function is similar to \f1 atol\f0 , except it returns a \f1 long long int\f0 . 
\par 
\par The \f1 atoll\f0  function was introduced in ISO C99. It too is obsolete (despite having just been added); use \f1 strtoll\f0  instead.
\par 
\par \pard All the functions mentioned in this section so far do not handle alternative representations of characters as described in the locale data. Some locales specify thousands separator and the way they have to be used which can help to make large numbers more readable. To read such numbers one has to \f3 parse them manually. 
\par 
\par \f0 Here is a function which parses a string as a sequence of integers and returns the sum of them:
\par 
\par \f1      int
\par      sum_ints_from_string (char *string)
\par      \{
\par        int sum = 0;
\par      
\par        while (1) \{
\par          char *tail;
\par          int next;
\par      
\par          /* Skip whitespace by hand, to detect the end.  */
\par          while (isspace (*string)) string++;
\par          if (*string == 0)
\par            break;
\par      
\par          /* There is more nonwhitespace,  */
\par          /* so it ought to be another number.  */
\par          errno = 0;
\par          /* Parse it.  */
\par          next = strtol (string, &tail, 0);
\par          /* Add it in, if not overflow.  */
\par          if (errno)
\par            printf ("Overflow\\n");
\par          else
\par            sum += next;
\par          /* Advance past it.  */
\par          string = tail;
\par        \}
\par      
\par        return sum;
\par      \}
\par \f0 
\par }
1710
PAGE_445
Parsing of Numbers
parsingnumbers(informattedinput;convertingstringstonumbers;numbersyntax,parsing;syntax,forreadingnumbers



Imported



FALSE
16
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 + Parsing of Numbers \{keepn\}
\par \pard\fs18 Previous: \cf1\strike Operations on Complex\strike0\{linkID=1650\}\cf0 , Up: \cf1\strike Arithmetic\strike0\{linkID=100\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Parsing of Numbers\b0 
\par 
\par This section describes functions for "reading" integer and floating-point numbers from a string. It may be more convenient in some cases to use \f1 sscanf\f0  or one of the related functions; see \cf1\strike Formatted Input\strike0\{linkID=940\}\cf0 . But often you can make a program more robust by finding the tokens in the string by hand, then converting the numbers one by one.
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Parsing of Integers\strike0\{linkID=1700\}\cf0 : Functions for conversion of integer values.
\par \f2\'b7\f0  \cf1\strike Parsing of Floats\strike0\{linkID=1690\}\cf0 : Functions for conversion of floating-point values.
\par 
\par }
1730
PAGE_456
Portable Positioning
stdio.h;fpos_t;fpos64_t;fgetpos;fgetpos;fsetpos;fsetpos



Imported



FALSE
36
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fswiss\fcharset0 Arial;}{\f2\fmodern Courier New;}{\f3\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Portable Positioning \{keepn\}
\par \pard\fs18 Next: \cf1\strike Stream Buffering\strike0\{linkID=2120\}\cf0 , Previous: \cf1\strike File Positioning\strike0\{linkID=790\}\cf0 , Up: \cf1\strike I/O on Streams\strike0\{linkID=1260\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Portable File-Position Functions\b0 
\par \f1 
\par In this\f0  system, the file position is truly a character count. You can specify any character count value as an argument to \f2 fseek\f0  and get reliable results for any random access file. However, \f1 other ISO C systems may not represent file positions in this manner.\f0 
\par 
\par As a consequence, you must observe certain rules\f1  when writing code which is portable to these systems\f0 :
\par 
\par \pard\li500\f3\'b7\f0  The value returned from \f2 ftell\f0  on a text stream has no predictable relationship to the number of characters you have read so far. The only thing you can rely on is that you can use it subsequently as the\i  offset\i0  argument to \f2 fseek\f0  or \f2 fseeko\f0  to move back to the same file position. 
\par \f3\'b7\f0  In a call to \f2 fseek\f0  on a text stream, either the\i  offset\i0  must be zero, or \i whence\i0  must be \f2 SEEK_SET\f0  and and the \i offset\i0  must be the result of an earlier call to \f2 ftell\f0  on the same stream. 
\par \f3\'b7\f0  The value of the file position indicator of a text stream is undefined while there are characters that have been pushed back with \f2 ungetc\f0  that haven't been read or discarded. See \cf1\strike Unreading\strike0\{linkID=2360\}\cf0 .
\par 
\par \pard\f1 I\f0 f you do want to support systems with peculiar encodings for the file positions, it is better to use the functions \f2 fgetpos\f0  and\f2  fsetpos\f0  instead. These functions represent the file position using the data type \f2 fpos_t\f0 , whose internal representation varies from system to system. 
\par 
\par These symbols are declared in the header file \f2 stdio.h\f0 .
\par 
\par -- Data Type: \b fpos_t\b0 
\par 
\par \pard\li500 This is the type of an object that can encode information about the file position of a stream, for use by the functions \f2 fgetpos\f0  and\f2  fsetpos\f0 . 
\par 
\par In the GNU system, \f2 fpos_t\f0  is an opaque data structure that contains internal data to represent file offset and conversion state information. In other systems, it might have a different internal representation. 
\par 
\par \pard -- Function: int \b fgetpos\b0  (\i FILE *stream, fpos_t *position\i0 )
\par 
\par \pard\li500 This function stores the value of the file position indicator for the stream \i stream\i0  in the \f2 fpos_t\f0  object pointed to by\i  position\i0 . If successful, \f2 fgetpos\f0  returns zero; otherwise it returns a nonzero value and stores an implementation-defined positive value in \f2 errno\f0 . 
\par 
\par \pard -- Function: int \b fsetpos\b0  (\i FILE *stream, const fpos_t *position\i0 )
\par 
\par \pard\li500 This function sets the file position indicator for the stream \i stream\i0  to the position \i position\i0 , which must have been set by a previous call to \f2 fgetpos\f0  on the same stream. If successful, \f2 fsetpos\f0  clears the end-of-file indicator on the stream, discards any characters that were "pushed back" by the use of \f2 ungetc\f0 , and returns a value of zero. Otherwise, \f2 fsetpos\f0  returns a nonzero value and stores an implementation-defined positive value in \f2 errno\f0 . 
\par }
1740
PAGE_479
Program Arguments
programarguments;commandlinearguments;arguments,toprogram;programstartup;startupofprogram;invocationofprogram;main;main;argc(programargumentcount);argv(programargumentvector)



Imported



FALSE
30
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fswiss\fcharset0 Arial;}{\f3\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Program Arguments \{keepn\}
\par \pard\fs18 Next: \cf1\strike Environment Variables\strike0\{linkID=640\}\cf0 , Up: \cf1\strike Program Basics\strike0\{linkID=1750\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Program Arguments\b0 
\par 
\par The system starts a C program by calling the function \f1 main\f0 . It is up to you to write a function named \f1 main\f0 --otherwise, you won't even be able to link your program without errors. 
\par 
\par In ISO C you can define \f1 main\f0  either to take no arguments, or to take two arguments that represent the command line arguments to the program, like this:
\par 
\par \f1      int main (int \i argc\i0 , char *\i argv\i0 [])
\par \f0 
\par The command line arguments are the whitespace-separated tokens given in the shell command used to invoke the program; thus, in `\f1 cat foo bar\f0 ', the arguments are `\f1 foo\f0 ' and `\f1 bar\f0 '. \f2  A program may also look at its arguments by examining the \i __argv\i0  and \i __argc\i0  variables\f0 
\par 
\par 
\par The value of the \i argc\i0  argument is the number of command line arguments. The \i argv\i0  argument is a vector of C strings; its elements are the individual command line argument strings. The file name of the program being run is also included in the vector as the first element; the value of \i argc\i0  counts this element. A null pointer always follows the last element: \i argv\i0\f1 [\i\f0 argc\i0\f1 ]\f0  is this null pointer. 
\par 
\par For the command `\f1 cat foo bar\f0 ', \i argc\i0  is 3 and \i argv\i0  has three elements, \f1 "cat"\f0 , \f1 "foo"\f0  and \f1 "bar"\f0 . 
\par 
\par In \f2 this \f0 systems you can define \f1 main\f0  a third way, using three arguments:
\par 
\par \f1      int main (int \i argc\i0 , char *\i argv\i0 [], char *\i envp\i0 [])
\par \f0 
\par The first two arguments are just the same. The third argument\i  envp\i0  gives the program's environment; it is the same as the value of \f2 _\f1 environ\f0 . See \cf1\strike Environment Variables\strike0\{linkID=640\}\cf0 . POSIX.1 does not allow this three-argument form, so to be portable it is best to write\f1  main\f0  to take two arguments, and use the value of \f2 _\f1 environ\f0 .
\par \f3 
\par }
1750
PAGE_480
Program Basics
process;program;addressspace;threadofcontrol



Imported



FALSE
23
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Program Basics \{keepn\}
\par \pard\fs18 Next: \cf1\strike Language Features\cf2\strike0\{linkID=1310\}\cf0 , Previous: \cf1\strike Signal Handling\strike0\{linkID=2010\}\cf0 , Up: \cf1\strike Top\strike0\{linkID=10\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b The Basic Program/System Interface\b0 
\par 
\par \i Processes\i0  are the primitive units for allocation of system resources. Each process has its own address space and (usually) one thread of control. A process executes a program; you can have multiple processes executing the same program, but each process has its own copy of the program within its own address space and executes it independently of the other copies. Though it may have multiple threads of control within the same program and a program may be composed of multiple logically separate modules, a process always executes exactly one program. 
\par 
\par Note that we are using a specific definition of "program" for the purposes of this manual, which corresponds to a common definition In popular usage, "program" enjoys a much broader definition; it can refer for example to a system's kernel, an editor macro, a complex package of software, or a discrete section of code executing within a process. 
\par 
\par Writing the program is what this manual is all about. This chapter explains the most basic interface between your program and the system that runs, or calls, it. This includes passing of parameters (arguments and environment) from the system, requesting basic services from the system, and telling the system the program is done. 
\par 
\par A program starts another program with the \f1 exec\f0  family of system calls. This chapter looks at program startup from the execee's point of view. To see the event from the execor's point of view, See \cf1\strike Executing a File\strike0\{linkID=700\}\cf0 .
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Program Arguments\strike0\{linkID=1740\}\cf0 : Parsing your program's command-line arguments.
\par \f2\'b7\f0  \cf1\strike Environment Variables\strike0\{linkID=640\}\cf0 : Less direct parameters affecting your program
\par \f2\'b7\f0  \cf1\strike Program Termination\strike0\{linkID=1770\}\cf0 : Telling the system you're done; return status
\par 
\par }
1760
PAGE_481
Program Error Signals
programerrorsignals;COREFILE;SIGFPE;exception;floating-pointexception;FPE_INTOVF_TRAP;FPE_INTDIV_TRAP;FPE_SUBRNG_TRAP;FPE_FLTOVF_TRAP;FPE_FLTDIV_TRAP;FPE_FLTUND_TRAP;FPE_DECOVF_TRAP;SIGILL;illegalinstruction;SIGSEGV;segmentationviolation;SIGBUS;buserror;SIGABRT;abortsignal;SIGIOT;SIGTRAP;SIGEMT;SIGSYS



Imported



FALSE
48
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Program Error Signals \{keepn\}
\par \pard\fs18 Next: \cf1\strike Miscellaneous Signals\cf2\strike0\{linkID=1460\}\cf0 , Up: \cf1\strike Standard Signals\strike0\{linkID=2090\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Program Error Signals\b0 
\par 
\par The following signals are generated when a serious program error is detected by the operating system or the computer itself. In general, all of these signals are indications that your program is seriously broken in some way, and there's usually no way to continue the computation which encountered the error. 
\par 
\par Some programs handle program error signals in order to tidy up before terminating; for example, programs that turn off echoing of terminal input should handle program error signals in order to turn echoing back on. The handler should end by specifying the default action for the signal that happened and then reraising it; this will cause the program to terminate with that signal, as if it had not had a handler. (See \cf1\strike Termination in Handler\strike0\{linkID=2260\}\cf0 .) 
\par 
\par Termination is the sensible ultimate outcome from a program error in most programs. However, programming systems such as Lisp that can load compiled user programs might need to keep executing even if a user program incurs an error. These programs have handlers which use\f1  longjmp\f0  to return control to the command level. 
\par 
\par The default action for all of these signals is to cause the process to terminate. If you block or ignore these signals or establish handlers for them that return normally, your program will probably break horribly when such signals happen, unless they are generated by \f1 raise\f0  or\f1  kill\f0  instead of a real error. 
\par 
\par -- Macro: int \b SIGFPE\b0 
\par 
\par \pard\li500 The \f1 SIGFPE\f0  signal reports a fatal arithmetic error. Although the name is derived from "floating-point exception", this signal actually covers all arithmetic errors, including division by zero and overflow. If a program stores integer data in a location which is then used in a floating-point operation, this often causes an "invalid operation" exception, because the processor cannot recognize the data as a floating-point number. Actual floating-point exceptions are a complicated subject because there are many types of exceptions with subtly different meanings, and the\f1  SIGFPE\f0  signal doesn't distinguish between them. The \i IEEE Standard for Binary Floating-Point Arithmetic (ANSI/IEEE Std 754-1985 and ANSI/IEEE Std 854-1987)\i0  defines various floating-point exceptions and requires conforming computer systems to report their occurrences. However, this standard does not specify how the exceptions are reported, or what kinds of handling and control the operating system can offer to the programmer.
\par 
\par \pard -- Macro: int \b SIGILL\b0 
\par 
\par \pard\li500 The name of this signal is derived from "illegal instruction"; it usually means your program is trying to execute garbage or a privileged instruction. Since the C compiler generates only valid instructions,\f1  SIGILL\f0  typically indicates that the executable file is corrupted, or that you are trying to execute data. Some common ways of getting into the latter situation are by passing an invalid object where a pointer to a function was expected, or by writing past the end of an automatic array (or similar problems with pointers to automatic variables) and corrupting other data on the stack such as the return address of a stack frame. 
\par 
\par \f1 SIGILL\f0  can also be generated when the stack overflows, or when the system has trouble running the handler for a signal.
\par 
\par \pard -- Macro: int \b SIGSEGV\b0 
\par 
\par \pard\li500 This signal is generated when a program tries to read or write outside the memory that is allocated for it, or to write memory that can only be read. (Actually, the signals only occur when the program goes far enough outside to be detected by the system's memory protection mechanism.) The name is an abbreviation for "segmentation violation". 
\par 
\par Common ways of getting a \f1 SIGSEGV\f0  condition include dereferencing a null or uninitialized pointer, or when you use a pointer to step through an array, but fail to check for the end of the array. It varies among systems whether dereferencing a null pointer generates\f1  SIGSEGV\f0  or \f1 SIGBUS\f0 .
\par 
\par 
\par \pard -- Macro: int \b SIGABRT\b0 
\par 
\par \pard\li500 This signal indicates an error detected by the program itself and reported by calling \f1 abort\f0 . See \cf1\strike Aborting a Program\strike0\{linkID=20\}\cf0 .
\par 
\par \pard -- Macro: int \b SIGTRAP\b0 
\par 
\par \pard\li500 Generated by the machine's breakpoint instruction, and possibly other trap instructions. This signal is used by debuggers. Your program will probably only see \f1 SIGTRAP\f0  if it is somehow executing bad instructions.
\par 
\par \pard -- Macro: int \b SIGEMT\b0 
\par 
\par \pard\li500 Emulator trap; this results from certain unimplemented instructions which might be emulated in software, or the operating system's failure to properly emulate them.
\par 
\par }
1770
PAGE_482
Program Termination
programtermination;processtermination;exitstatusvalue



Imported



FALSE
22
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fswiss\fcharset0 Arial;}{\f2\fmodern Courier New;}{\f3\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Program Termination \{keepn\}
\par \pard\fs18 Previous:\f1  \cf1\strike Environment Variables\cf2\strike0\{linkID=640\}\cf0\f0 , Up: \cf1\strike Program Basics\strike0\{linkID=1750\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Program Termination\b0 
\par 
\par The usual way for a program to terminate is simply for its \f2 main\f0  function to return. The \i exit status value\i0  returned from the\f2  main\f0  function is used to report information back to the process's parent process or shell. 
\par 
\par A program can also terminate normally by calling the \f2 exit\f0  function. 
\par 
\par In addition, programs can be terminated by signals; this is discussed in more detail in \cf1\strike Signal Handling\strike0\{linkID=2010\}\cf0 . The \f2 abort\f0  function causes a signal that kills the program.
\par 
\par \pard\li500\f3\'b7\f0  \cf1\strike Normal Termination\strike0\{linkID=1560\}\cf0 : If a program calls \f2 exit\f0 , a process terminates normally.
\par \f3\'b7\f0  \cf1\strike Exit Status\strike0\{linkID=710\}\cf0 : The \f2 exit status\f0  provides information about why the process terminated.
\par \f3\'b7\f0  \cf1\strike Cleanups on Exit\strike0\{linkID=350\}\cf0 : A process can run its own cleanup functions upon normal termination.
\par \f3\'b7\f0  \cf1\strike Aborting a Program\strike0\{linkID=20\}\cf0 : The \f2 abort\f0  function causes abnormal program termination.
\par \f3 
\par }
1780
PAGE_484
Pseudo-Random Numbers
randomnumbers;pseudo-randomnumbers;seed(forrandomnumbers



Imported



FALSE
20
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fswiss\fcharset0 Arial;}{\f2\fmodern\fcharset0 Courier New;}{\f3\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Pseudo-Random Numbers \{keepn\}
\par \pard\fs18 Previous: \cf1\strike Special Functions\cf2\strike0\{linkID=2070\}\cf0 , Up: \cf1\strike Mathematics\strike0\{linkID=1410\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Pseudo-Random Numbers\b0 
\par 
\par This section describes the GNU facilities for generating a series of pseudo-random numbers. The numbers generated are not truly random; typically, they form a sequence that repeats periodically, with a period so large that you can ignore it for ordinary purposes. The random number generator works by remembering a \i seed\i0  value which it uses to compute the next random number and also to compute a new seed. 
\par 
\par Although the generated numbers look unpredictable within one run of a program, the sequence of numbers is \i exactly the same\i0  from one run to the next. This is because the initial seed is always the same. This is convenient when you are debugging a program, but it is unhelpful if you want the program to behave unpredictably. If you want a different pseudo-random series each time your program runs, you must specify a different seed each time. For ordinary purposes, basing the seed on the current time works well. 
\par 
\par You can obtain repeatable sequences of numbers on a particular machine type by specifying the same initial seed value for the random number generator. There is no standard meaning for a particular seed value; the same seed, used in different C libraries or on different CPU types, will give you different random numbers. 
\par 
\par Th\f1 is\f0  library supports the standard ISO C random number functions \f1 
\par \f0 
\par \pard\li500\f2\'b7\f0  \cf1\strike ISO Random\strike0\{linkID=1240\}\cf0 : \f3 rand\f0  and friends.\cf1\strike 
\par }
1790
PAGE_489
Range of Type
integertyperange;rangeofintegertype;limits,integertypes;SCHAR_MIN;SCHAR_MAX;UCHAR_MAX;CHAR_MIN;CHAR_MAX;SHRT_MIN;SHRT_MAX;USHRT_MAX;INT_MIN;INT_MAX;UINT_MAX;LONG_MIN;LONG_MAX;ULONG_MAX;LONG_LONG_MIN;LONG_LONG_MAX;ULONG_LONG_MAX;WCHAR_MAX



Imported



FALSE
49
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 + Range of Type \{keepn\}
\par \pard\fs18 Next: \cf1\strike Floating Type Macros\strike0\{linkID=890\}\cf0 , Previous: \cf1\strike Width of Type\strike0\{linkID=2460\}\cf0 , Up: \cf1\strike Data Type Measurements\strike0\{linkID=510\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Range of an Integer Type\b0 
\par 
\par Suppose you need to store an integer value which can range from zero to one million. Which is the smallest type you can use? There is no general rule; it depends on the C compiler and target machine. You can use the `\f1 MIN\f0 ' and `\f1 MAX\f0 ' macros in \f1 limits.h\f0  to determine which type will work. 
\par 
\par Each signed integer type has a pair of macros which give the smallest and largest values that it can hold. Each unsigned integer type has one such macro, for the maximum value; the minimum value is, of course, zero. 
\par 
\par The values of these macros are all integer constant expressions. The `\f1 MAX\f0 ' and `\f1 MIN\f0 ' macros for \f1 char\f0  and \f1 short int\f0  types have values of type \f1 int\f0 . The `\f1 MAX\f0 ' and `\f1 MIN\f0 ' macros for the other types have values of the same type described by the macro--thus, \f1 ULONG_MAX\f0  has type\f1  unsigned long int\f0 .
\par 
\par \f1 SCHAR_MIN\f0 
\par \pard\li500 This is the minimum value that can be represented by a \f1 signed char\f0 .
\par \pard\f1 SCHAR_MAX\f0 
\par \f1 UCHAR_MAX\f0 
\par \pard\li500 These are the maximum values that can be represented by a\f1  signed char\f0  and \f1 unsigned char\f0 , respectively.
\par \pard\f1 CHAR_MIN\f0 
\par \pard\li500 This is the minimum value that can be represented by a \f1 char\f0 . It's equal to \f1 SCHAR_MIN\f0  if \f1 char\f0  is signed, or zero otherwise.
\par \pard\f1 CHAR_MAX\f0 
\par \pard\li500 This is the maximum value that can be represented by a \f1 char\f0 . It's equal to \f1 SCHAR_MAX\f0  if \f1 char\f0  is signed, or\f1  UCHAR_MAX\f0  otherwise.
\par \pard\f1 SHRT_MIN\f0 
\par \pard\li500 This is the minimum value that can be represented by a \f1 signed short int\f0 . \f2  In this system\f0 ,\f1  short\f0  integers are 16-bit quantities.
\par \pard\f1 SHRT_MAX\f0 
\par \f1 USHRT_MAX\f0 
\par \pard\li500 These are the maximum values that can be represented by a\f1  signed short int\f0  and \f1 unsigned short int\f0 , respectively.
\par \pard\f1 INT_MIN\f0 
\par \pard\li500 This is the minimum value that can be represented by a \f1 signed int\f0 .\f2   In this system\f0 , an \f1 int\f0  is a 32-bit quantity.
\par \pard\f1 INT_MAX\f0 
\par \f1 UINT_MAX\f0 
\par \pard\li500 These are the maximum values that can be represented by, respectively, the type \f1 signed int\f0  and the type \f1 unsigned int\f0 .
\par \pard\f1 LONG_MIN\f0 
\par \pard\li500 This is the minimum value that can be represented by a \f1 signed long int\f0 . \f2 I\f0 n \f2 this system, \f1 long\f0  integers are 32-bit quantities, the same size as \f1 int\f0 .
\par \pard\f1 LONG_MAX\f0 
\par \f1 ULONG_MAX\f0 
\par \pard\li500 These are the maximum values that can be represented by a\f1  signed long int\f0  and \f1 unsigned long int\f0 , respectively.
\par \pard\f1 LONG_LONG_MIN\f0 
\par \pard\li500 This is the minimum value that can be represented by a \f1 signed long long int\f0 . \f2 In this system\f0 ,\f1  long long\f0  integers are 64-bit quantities.
\par \pard\f1 LONG_LONG_MAX\f0 
\par \f1 ULONG_LONG_MAX\f0 
\par \pard\li500 These are the maximum values that can be represented by a \f1 signed long long int\f0  and \f1 unsigned long long int\f0 , respectively.
\par \pard\f1 WCHAR_MAX\f0 
\par \pard\li500 This is the maximum value that can be represented by a \f1 wchar_t\f0 . See \cf1\strike Extended Char Intro\strike0\{linkID=730\}\cf0 .
\par \pard 
\par }
1800
PAGE_491
Reading Attributes
stat;stat;fstat;fstat;lstat;lstat



Imported



FALSE
38
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fnil\fcharset0 Arial;}{\f3\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Reading Attributes \{keepn\}
\par \pard\fs18 Previous: \cf1\strike Attribute Meanings\strike0\{linkID=160\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Reading the Attributes of a File\b0 
\par 
\par To examine the attributes of files, use the functions \f1 stat\f0 ,\f1  \f2 and \f1 fstat\f0  and \f1 lstat\f0 . They return the attribute information in a \f1 struct stat\f0  object. \f3  Both \f0 functions are declared in the header file \f1 sys/stat.h\f0 .
\par 
\par -- Function: int \b stat\b0  (\i const char *filename, struct stat *buf\i0 )
\par 
\par \pard\li500 The \f1 stat\f0  function returns information about the attributes of the file named by \i filename\i0  in the structure pointed to by \i buf\i0 . 
\par 
\par If \i filename\i0  is the name of a symbolic link, the attributes you get describe the file that the link points to. If the link points to a nonexistent file name, then \f1 stat\f0  fails reporting a nonexistent file. 
\par 
\par The return value is \f1 0\f0  if the operation is successful, or\f1  -1\f0  on failure. In addition to the usual file name errors (see \cf1\strike File Name Errors\strike0\{linkID=740\}\cf0 , the following \f1 errno\f0  error conditions are defined for this function:
\par 
\par \f1 ENOENT\f0 
\par \pard\li1000 The file named by \i filename\i0  doesn't exist.
\par 
\par \pard\li500 When the sources are compiled with \f1 _FILE_OFFSET_BITS == 64\f0  this function is in fact \f1 stat64\f0  since the LFS interface transparently replaces the normal implementation.
\par 
\par \pard - Function: int \b fstat\b0  (\i int filedes, struct stat *buf\i0 )
\par 
\par \pard\li500 The \f1 fstat\f0  function is like \f1 stat\f0 , except that it takes an open file descriptor as an argument instead of a file name. See \cf1\strike Low-Level I/O\strike0\{linkID=1360\}\cf0 . 
\par 
\par Like \f1 stat\f0 , \f1 fstat\f0  returns \f1 0\f0  on success and \f1 -1\f0  on failure. The following \f1 errno\f0  error conditions are defined for\f1  fstat\f0 :
\par 
\par \f1 EBADF\f0 
\par \pard\li1000 The \i filedes\i0  argument is not a valid file descriptor.
\par 
\par \pard\li500 When the sources are compiled with \f1 _FILE_OFFSET_BITS == 64\f0  this function is in fact \f1 fstat64\f0  since the LFS interface transparently replaces the normal implementation.
\par 
\par 
\par }
1810
PAGE_495
Receiving Arguments
variadicfunctionargumentaccess;arguments(variadicfunctions;stdarg.h



Imported



FALSE
33
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Receiving Arguments \{keepn\}
\par \pard\fs18 Next: \cf1\strike How Many Arguments\strike0\{linkID=1100\}\cf0 , Previous: \cf1\strike Variadic Prototypes\strike0\{linkID=2430\}\cf0 , Up: \cf1\strike How Variadic\strike0\{linkID=1120\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Receiving the Argument Values\b0 
\par 
\par Ordinary fixed arguments have individual names, and you can use these names to access their values. But optional arguments have no names--nothing but `\f1 ...\f0 '. How can you access them? 
\par 
\par The only way to access them is sequentially, in the order they were written, and you must use special macros from \f1 stdarg.h\f0  in the following three step process:
\par 
\par \pard\li500 1. You initialize an argument pointer variable of type \f1 va_list\f0  using\f1  va_start\f0 . The argument pointer when initialized points to the first optional argument. 
\par 2. You access the optional arguments by successive calls to \f1 va_arg\f0 . The first call to \f1 va_arg\f0  gives you the first optional argument, the next call gives you the second, and so on. 
\par 
\par You can stop at any time if you wish to ignore any remaining optional arguments. It is perfectly all right for a function to access fewer arguments than were supplied in the call, but you will get garbage values if you try to access too many arguments. 
\par 3. You indicate that you are finished with the argument pointer variable by calling \f1 va_end\f0 . 
\par 
\par (In practice, with most C compilers, calling \f1 va_end\f0  does nothing. This is true in \f2 the CC386\f0  compiler. But you might as well call\f1  va_end\f0  just in case your program is someday compiled with a peculiar compiler.)
\par 
\par \pard See \cf1\strike Argument Macros\strike0\{linkID=80\}\cf0 , for the full definitions of \f1 va_start\f0 ,\f1  va_arg\f0  and \f1 va_end\f0 . 
\par 
\par Steps 1 and 3 must be performed in the function that accepts the optional arguments. However, you can pass the \f1 va_list\f0  variable as an argument to another function and perform all or part of step 2 there. 
\par 
\par You can perform the entire sequence of three steps multiple times within a single function invocation. If you want to ignore the optional arguments, you can do these steps zero times. 
\par 
\par You can have more than one argument pointer variable if you like. You can initialize each variable with \f1 va_start\f0  when you wish, and then you can fetch arguments with each argument pointer as you wish. Each argument pointer variable will sequence through the same set of argument values, but at its own pace. 
\par 
\par \b Portability note:\b0  With some compilers, once you pass an argument pointer value to a subroutine, you must not keep using the same argument pointer value after that subroutine returns. For full portability, you should just pass it to \f1 va_end\f0 . This is actually an ISO C requirement, but most ANSI C compilers work happily regardless.
\par 
\par }
1820
PAGE_502
Remainder Functions
fmod;fmodf;fmodl;drem;dremf;dreml;remainder;remainderf;remainderl



Imported



FALSE
45
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Remainder Functions \{keepn\}
\par \pard\fs18 Next: \cf1\strike FP Bit Twiddling\strike0\{linkID=990\}\cf0 , Previous: \cf1\strike Rounding Functions\strike0\{linkID=1890\}\cf0 , Up: \cf1\strike Arithmetic Functions\strike0\{linkID=90\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Remainder Functions\b0 
\par 
\par The functions in this section compute the remainder on division of two floating-point numbers. Each is a little different; pick the one that suits your problem.
\par 
\par -- Function: double \b fmod\b0  (\i double numerator, double denominator\i0 )
\par 
\par \pard\li500 -- Function: float \b fmodf\b0  (\i float numerator, float denominator\i0 )
\par 
\par \pard\li1000 -- Function: long double \b fmodl\b0  (\i long double numerator, long double denominator\i0 )
\par 
\par \pard\li1500 These functions compute the remainder from the division of\i  numerator\i0  by \i denominator\i0 . Specifically, the return value is\i  numerator\i0\f1  - \i\f0 n\i0\f1  * \i\f0 denominator\i0 , where \i n\i0  is the quotient of \i numerator\i0  divided by \i denominator\i0 , rounded towards zero to an integer. Thus, \f1 fmod (6.5, 2.3)\f0  returns\f1  1.9\f0 , which is \f1 6.5\f0  minus \f1 4.6\f0 . 
\par 
\par The result has the same sign as the \i numerator\i0  and has magnitude less than the magnitude of the \i denominator\i0 . 
\par 
\par If \i denominator\i0  is zero, \f1 fmod\f0  signals a domain error.
\par 
\par \pard -- Function: double \b drem\b0  (\i double numerator, double denominator\i0 )
\par 
\par \pard\li500 -- Function: float \b dremf\b0  (\i float numerator, float denominator\i0 )
\par 
\par \pard\li1000 -- Function: long double \b dreml\b0  (\i long double numerator, long double denominator\i0 )
\par 
\par \pard\li1500 These functions are like \f1 fmod\f0  except that they rounds the internal quotient \i n\i0  to the nearest integer instead of towards zero to an integer. For example, \f1 drem (6.5, 2.3)\f0  returns \f1 -0.4\f0 , which is \f1 6.5\f0  minus \f1 6.9\f0 . 
\par 
\par The absolute value of the result is less than or equal to half the absolute value of the \i denominator\i0 . The difference between\f1  fmod (\i\f0 numerator\i0\f1 , \i\f0 denominator\i0\f1 )\f0  and \f1 drem (\i\f0 numerator\i0\f1 , \i\f0 denominator\i0\f1 )\f0  is always either\i  denominator\i0 , minus \i denominator\i0 , or zero. 
\par 
\par If \i denominator\i0  is zero, \f1 drem\f0  signals a domain error.
\par 
\par \pard -- Function: double \b remainder\b0  (\i double numerator, double denominator\i0 )
\par 
\par \pard\li500 -- Function: float \b remainderf\b0  (\i float numerator, float denominator\i0 )
\par 
\par \pard\li1000 -- Function: long double \b remainderl\b0  (\i long double numerator, long double denominator\i0 )
\par 
\par \pard\li1500 This function is another name for \f1 drem\f0 .
\par 
\par }
1840
PAGE_504
Renaming Files
renamingafile;rename



Imported



FALSE
51
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Renaming Files \{keepn\}
\par \pard\fs18 Next: \cf1\strike Creating Directories\strike0\{linkID=490\}\cf0 , Previous: \cf1\strike Deleting Files\strike0\{linkID=540\}\cf0 , Up: \cf1\strike File System Interface\strike0\{linkID=820\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Renaming Files\b0 
\par 
\par The \f1 rename\f0  function is used to change a file's name.
\par 
\par -- Function: int \b rename\b0  (\i const char *oldname, const char *newname\i0 )
\par 
\par \pard\li500 The \f1 rename\f0  function renames the file \i oldname\i0  to\i  newname\i0 . The file formerly accessible under the name\i  oldname\i0  is afterwards accessible as \i newname\i0  instead. (If the file had any other names aside from \i oldname\i0 , it continues to have those names.) 
\par 
\par The directory containing the name \i newname\i0  must be on the same file system as the directory containing the name \i oldname\i0 . 
\par 
\par One special case for \f1 rename\f0  is when \i oldname\i0  and\i  newname\i0  are two names for the same file. The consistent way to handle this case is to delete \i oldname\i0 . However, in this case POSIX requires that \f1 rename\f0  do nothing and report success--which is inconsistent. We don't know what your operating system will do. 
\par 
\par If \i oldname\i0  is not a directory, then any existing file named\i  newname\i0  is removed during the renaming operation. However, if\i  newname\i0  is the name of a directory, \f1 rename\f0  fails in this case. 
\par 
\par If \i oldname\i0  is a directory, then either \i newname\i0  must not exist or it must name a directory that is empty. In the latter case, the existing directory named \i newname\i0  is deleted first. The name\i  newname\i0  must not specify a subdirectory of the directory\f1  oldname\f0  which is being renamed. 
\par 
\par One useful feature of \f1 rename\f0  is that the meaning of \i newname\i0  changes "atomically" from any previously existing file by that name to its new meaning (i.e. the file that was called \i oldname\i0 ). There is no instant at which \i newname\i0  is non-existent "in between" the old meaning and the new meaning. If there is a system crash during the operation, it is possible for both names to still exist; but\i  newname\i0  will always be intact if it exists at all. 
\par 
\par If \f1 rename\f0  fails, it returns \f1 -1\f0 . In addition to the usual file name errors (see \cf1\strike File Name Errors\strike0\{linkID=740\}\cf0 ), the following\f1  errno\f0  error conditions are defined for this function:
\par 
\par \f1 EACCES\f0 
\par \pard\li1000 One of the directories containing \i newname\i0  or \i oldname\i0  refuses write permission; or \i newname\i0  and \i oldname\i0  are directories and write permission is refused for one of them. 
\par \pard\li500\f1 EBUSY\f0 
\par \pard\li1000 A directory named by \i oldname\i0  or \i newname\i0  is being used by the system in a way that prevents the renaming from working. This includes directories that are mount points for filesystems, and directories that are the current working directories of processes. 
\par \pard\li500\f1 ENOTEMPTY\f0 
\par \f1 EEXIST\f0 
\par \pard\li1000 The directory \i newname\i0  isn't empty. The GNU system always returns\f1  ENOTEMPTY\f0  for this, but some other systems return \f1 EEXIST\f0 . 
\par \pard\li500\f1 EINVAL\f0 
\par \pard\li1000\i oldname\i0  is a directory that contains \i newname\i0 . 
\par \pard\li500\f1 EISDIR\f0 
\par \pard\li1000\i newname\i0  is a directory but the \i oldname\i0  isn't. 
\par \pard\li500\f1 EMLINK\f0 
\par \pard\li1000 The parent directory of \i newname\i0  would have too many links (entries). 
\par \pard\li500\f1 ENOENT\f0 
\par \pard\li1000 The file \i oldname\i0  doesn't exist. 
\par \pard\li500\f1 ENOSPC\f0 
\par \pard\li1000 The directory that would contain \i newname\i0  has no room for another entry, and there is no space left in the file system to expand it. 
\par \pard\li500\f1 EROFS\f0 
\par \pard\li1000 The operation would involve writing to a directory on a read-only file system. 
\par \pard\li500\f1 EXDEV\f0 
\par \pard\li1000 The two file names \i newname\i0  and \i oldname\i0  are on different file systems.
\par 
\par }
1850
PAGE_506
Representation of Strings
string,representationof;string;multibytecharacterstring;widecharacterstring;nullcharacter;nullwidecharacter;stringliteral;lengthofstring;allocationsizeofstring;sizeofstring;stringlength;stringallocation;single-bytestring;multibytestring;widecharacterstring



Imported



FALSE
35
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Representation of Strings \{keepn\}
\par \pard\fs18 Next: \cf1\strike String/Array Conventions\strike0\{linkID=2200\}\cf0 , Up: \cf1\strike String and Array Utilities\strike0\{linkID=2160\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Representation of Strings\b0 
\par 
\par This section is a quick summary of string concepts for beginning C programmers. It describes how character strings are represented in C and some common pitfalls. If you are already familiar with this material, you can skip this section. 
\par 
\par A \i string\i0  is an array of \f1 char\f0  objects. But string-valued variables are usually declared to be pointers of type \f1 char *\f0 . Such variables do not include space for the text of a string; that has to be stored somewhere else--in an array variable, a string constant, or dynamically allocated memory (see \cf1\strike Memory Allocation\strike0\{linkID=1430\}\cf0 ). It's up to you to store the address of the chosen memory space into the pointer variable. Alternatively you can store a \i null pointer\i0  in the pointer variable. The null pointer does not point anywhere, so attempting to reference the string it points to gets an error. 
\par 
\par "string" normally refers to multibyte character strings as opposed to wide character strings. Wide character strings are arrays of type\f1  wchar_t\f0  and as for multibyte character strings usually pointers of type \f1 wchar_t *\f0  are used. 
\par 
\par By convention, a \i null character\i0 , \f1 '\\0'\f0 , marks the end of a multibyte character string and the \i null wide character\i0 ,\f1  L'\\0'\f0 , marks the end of a wide character string. For example, in testing to see whether the \f1 char *\f0  variable \i p\i0  points to a null character marking the end of a string, you can write\f1  !*\i\f0 p\i0  or \f1 *\i\f0 p\i0\f1  == '\\0'\f0 . 
\par 
\par A null character is quite different conceptually from a null pointer, although both are represented by the integer \f1 0\f0 . 
\par 
\par \i String literals\i0  appear in C program source as strings of characters between double-quote characters (`\f1 "\f0 ') where the initial double-quote character is immediately preceded by a capital `\f1 L\f0 ' (ell) character (as in \f1 L"foo"\f0 ). In ISO C, string literals can also be formed by \i string concatenation\i0 : \f1 "a" "b"\f0  is the same as \f1 "ab"\f0 . For wide character strings one can either use\f1  L"a" L"b"\f0  or \f1 L"a" "b"\f0 . Modification of string literals is \f2 allowed by this \f0 C compiler, \f2 but for portability this is not a good idea \f0 because \f2 with some systems \f0 literals are placed in read-only storage. 
\par 
\par Character arrays that are declared \f1 const\f0  cannot be modified either. It's generally good style to declare non-modifiable string pointers to be of type \f1 const char *\f0 , since this often allows the C compiler to detect accidental modifications as well as providing some amount of documentation about what your program intends to do with the string. 
\par 
\par The amount of memory allocated for the character array may extend past the null character that normally marks the end of the string. In this document, the term \i allocated size\i0  is always used to refer to the total amount of memory allocated for the string, while the term\i  length\i0  refers to the number of characters up to (but not including) the terminating null character. A notorious source of program bugs is trying to put more characters in a string than fit in its allocated size. When writing code that extends strings or moves characters into a pre-allocated array, you should be very careful to keep track of the length of the text and make explicit checks for overflowing the array. Many of the library functions\i  do not\i0  do this for you! Remember also that you need to allocate an extra byte to hold the null character that marks the end of the string. 
\par 
\par Originally strings were sequences of bytes where each byte represents a single character. This is still true today if the strings are encoded using a single-byte character encoding. Things are different if the strings are encoded using a multibyte encoding (for more information on encodings see \cf1\strike Extended Char Intro\strike0\{linkID=730\}\cf0 ). There is no difference in the programming interface for these two kind of strings; the programmer has to be aware of this and interpret the byte sequences accordingly. 
\par 
\par But since there is no separate interface taking care of these differences the byte-based string functions are sometimes hard to use. Since the count parameters of these functions specify bytes a call to\f1  strncpy\f0  could cut a multibyte character in the middle and put an incomplete (and therefore unusable) byte sequence in the target buffer. 
\par 
\par To avoid these problems later versions of the ISO C standard introduce a second set of functions which are operating on \i wide characters\i0  (see \cf1\strike Extended Char Intro\strike0\{linkID=730\}\cf0 ). These functions don't have the problems the single-byte versions have since every wide character is a legal, interpretable value. This does not mean that cutting wide character strings at arbitrary points is without problems. It normally is for alphabet-based languages (except for non-normalized text) but languages based on syllables still have the problem that more than one wide character is necessary to complete a logical unit. This is a higher level problem which the C library functions are not designed to solve. But it is at least good that no invalid byte sequences can be created. Also, the higher level functions can also much easier operate on wide character than on multibyte characters so that a general advise is to use wide characters internally whenever text is more than simply copied. 
\par 
\par The remaining of this chapter will discuss the functions for handling wide character strings in parallel with the discussion of the multibyte character strings since there is almost always an exact equivalent available.
\par 
\par }
1860
PAGE_507
Reserved Names
reservednames;namespace;dirent.h;fcntl.h;grp.h;limits.h;pwd.h;signal.h;sys/stat.h;sys/times.h;termios.h



Imported



FALSE
37
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern\fcharset0 Courier New;}{\f2\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Reserved Names \{keepn\}
\par \pard\fs18 Previous: \cf1\strike Macro Definitions\strike0\{linkID=1370\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Reserved Names\b0 
\par 
\par The names of all library types, macros, variables and functions that come from the ISO C standard are reserved unconditionally; your program\b  may not\b0  redefine these names. All other library names are reserved if your program explicitly includes the header file that defines or declares them. There are several reasons for these restrictions:
\par 
\par \pard\li500\f1\'b7\f0  Other people reading your code could get very confused if you were using a function named \f2 exit\f0  to do something completely different from what the standard \f2 exit\f0  function does, for example. Preventing this situation helps to make your programs easier to understand and contributes to modularity and maintainability. 
\par \f1\'b7\f0  It avoids the possibility of a user accidentally redefining a library function that is called by other library functions. If redefinition were allowed, those other functions would not work properly. 
\par \f1\'b7\f0  It allows the compiler to do whatever special optimizations it pleases on calls to these functions, without the possibility that they may have been redefined by the user. Some library facilities, such as those for dealing with variadic arguments (see \cf1\strike Variadic Functions\strike0\{linkID=2420\}\cf0 ) and non-local exits (see \cf1\strike Non-Local Exits\strike0\{linkID=1510\}\cf0 ), actually require a considerable amount of cooperation on the part of the C compiler, and with respect to the implementation, it might be easier for the compiler to treat these as built-in parts of the language.
\par 
\par \pard In addition to the names documented in this manual, reserved names include all external identifiers (global functions and variables) that begin with an underscore (`\f2 _\f0 ') and all identifiers regardless of use that begin with either two underscores or an underscore followed by a capital letter are reserved names. This is so that the library and header files can define functions, variables, and macros for internal purposes without risk of conflict with names in user programs. 
\par 
\par Some additional classes of identifier names are reserved for future extensions to the C language or the POSIX.1 environment. While using these names for your own purposes right now might not cause a problem, they do raise the possibility of conflict with future versions of the C or POSIX standards, so you should avoid these names.
\par 
\par \pard\li500\f1\'b7\f0  Names beginning with a capital `\f2 E\f0 ' followed a digit or uppercase letter may be used for additional error code names. See \cf1\strike Error Reporting\strike0\{linkID=690\}\cf0 . 
\par \f1\'b7\f0  Names that begin with either `\f2 is\f0 ' or `\f2 to\f0 ' followed by a lowercase letter may be used for additional character testing and conversion functions. See \cf1\strike Character Handling\strike0\{linkID=270\}\cf0 . 
\par \f1\'b7\f0  Names that begin with `\f2 LC_\f0 ' followed by an uppercase letter may be used for additional macros specifying locale attributes. See \cf1\strike Locales\strike0\{linkID=1340\}\cf0 . 
\par \f1\'b7\f0  Names of all existing mathematics functions (see \cf1\strike Mathematics\strike0\{linkID=1410\}\cf0 ) suffixed with `\f2 f\f0 ' or `\f2 l\f0 ' are reserved for corresponding functions that operate on \f2 float\f0  and \f2 long double\f0  arguments, respectively. 
\par \f1\'b7\f0  Names that begin with `\f2 SIG\f0 ' followed by an uppercase letter are reserved for additional signal names. See \cf1\strike Standard Signals\strike0\{linkID=2090\}\cf0 . 
\par \f1\'b7\f0  Names that begin with `\f2 SIG_\f0 ' followed by an uppercase letter are reserved for additional signal actions. See \cf1\strike Basic Signal Handling\strike0\{linkID=180\}\cf0 . 
\par \f1\'b7\f0  Names beginning with `\f2 str\f0 ', `\f2 mem\f0 ', or `\f2 wcs\f0 ' followed by a lowercase letter are reserved for additional string and array functions. See \cf1\strike String and Array Utilities\strike0\{linkID=2160\}\cf0 . 
\par \f1\'b7\f0  Names that end with `\f2 _t\f0 ' are reserved for additional type names.
\par 
\par \pard In addition, some individual header files reserve names beyond those that they actually define. You only need to worry about these restrictions if your program includes that particular header file.
\par 
\par \pard\li500\f1\'b7\f0  The header file \f2 limits.h\f0  reserves names suffixed with `\f2 _MAX\f0 '.
\par \f1\'b7\f0  The header file \f2 signal.h\f0  reserves names prefixed with `\f2 sa_\f0 ' and `\f2 SA_\f0 '.
\par \f1\'b7\f0  The header file \f2 sys/stat.h\f0  reserves names prefixed with `\f2 st_\f0 ' and `\f2 S_\f0 '.
\par \f1\'b7\f0  The header file \f2 sys/times.h\f0  reserves names prefixed with `\f2 tms_\f0 '.
\par \f1 
\par }
1870
PAGE_511
Restartable multibyte conversion




Imported



FALSE
24
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern\fcharset0 Courier New;}{\f2\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Restartable multibyte conversion \{keepn\}
\par \pard\fs18 Next: \cf1\strike Non-reentrant Conversion\strike0\{linkID=1540\}\cf0 , Previous: \cf1\strike Charset Function Overview\strike0\{linkID=300\}\cf0 , Up: \cf1\strike Character Set Handling\strike0\{linkID=290\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Restartable Multibyte Conversion Functions\b0 
\par 
\par The ISO C standard defines functions to convert strings from a multibyte representation to wide character strings. There are a number of peculiarities:
\par 
\par \pard\li500\f1\'b7\f0  The character set assumed for the multibyte encoding is not specified as an argument to the functions. Instead the character set specified by the \f2 LC_CTYPE\f0  category of the current locale is used; see \cf1\strike Locale Categories\strike0\{linkID=1320\}\cf0 . 
\par \f1\'b7\f0  The functions handling more than one character at a time require NUL terminated strings as the argument (i.e., converting blocks of text does not work unless one can add a NUL byte at an appropriate place). 
\par 
\par \pard Despite these limitations the ISO C functions can be used in many contexts. In graphical user interfaces, for instance, it is not uncommon to have functions that require text to be displayed in a wide character string if the text is not simple ASCII. The text itself might come from a file with translations and the user should decide about the current locale, which determines the translation and therefore also the external encoding used. In such a situation (and many others) the functions described here are perfect.
\par 
\par \pard\li500\f1\'b7\f0  \cf1\strike Selecting the Conversion\strike0\{linkID=1950\}\cf0 : Selecting the conversion and its properties.
\par \f1\'b7\f0  \cf1\strike Keeping the state\strike0\{linkID=1290\}\cf0 : Representing the state of the conversion.
\par \f1\'b7\f0  \cf1\strike Converting a Character\strike0\{linkID=440\}\cf0 : Converting Single Characters.
\par \f1\'b7\f0  \cf1\strike Converting Strings\strike0\{linkID=450\}\cf0 : Converting Multibyte and Wide Character Strings.
\par \f1\'b7\f0  \cf1\strike Multibyte Conversion Example\strike0\{linkID=1470\}\cf0 : A Complete Multibyte Conversion Example.
\par 
\par }
1880
PAGE_512
Roadmap to the Manual




Imported



FALSE
32
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern\fcharset0 Courier New;}{\f2\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Roadmap to the Manual \{keepn\}
\par \pard\fs18 Previous: \cf1\strike Getting Started\cf2\strike0\{linkID=1060\}\cf0 , Up: \cf1\strike Introduction\strike0\{linkID=1210\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Roadmap to the Manual\b0 
\par 
\par Here is an overview of the contents of the remaining chapters of this manual.
\par 
\par \pard\li500\f1\'b7\f0  \cf1\strike Error Reporting\strike0\{linkID=690\}\cf0 , describes how errors detected by the library are reported. 
\par \f1\'b7\f0  \cf1\strike Language Features\strike0\{linkID=1310\}\cf0 , contains information about library support for standard parts of the C language, including things like the \f2 sizeof\f0  operator and the symbolic constant \f2 NULL\f0 , how to write functions accepting variable numbers of arguments, and constants describing the ranges and other properties of the numerical types. There is also a simple debugging mechanism which allows you to put assertions in your code, and have diagnostic messages printed if the tests fail. 
\par \f1\'b7\f0  \cf1\strike Memory\strike0\{linkID=1440\}\cf0 , describes the GNU library's facilities for managing and using virtual and real memory, including dynamic allocation of virtual memory. If you do not know in advance how much memory your program needs, you can allocate it dynamically instead, and manipulate it via pointers. 
\par \f1\'b7\f0  \cf1\strike Character Handling\strike0\{linkID=270\}\cf0 , contains information about character classification functions (such as \f2 isspace\f0 ) and functions for performing case conversion. 
\par \f1\'b7\f0  \cf1\strike String and Array Utilities\strike0\{linkID=2160\}\cf0 , has descriptions of functions for manipulating strings (null-terminated character arrays) and general byte arrays, including operations such as copying and comparison. 
\par \f1\'b7\f0  \cf1\strike I/O Overview\strike0\{linkID=1270\}\cf0 , gives an overall look at the input and output facilities in the library, and contains information about basic concepts such as file names. 
\par \f1\'b7\f0  \cf1\strike I/O on Streams\strike0\{linkID=1260\}\cf0 , describes I/O operations involving streams (or\f2  FILE *\f0  objects). These are the normal C library functions from \f2 stdio.h\f0 . 
\par \f1\'b7\f0  \cf1\strike Low-Level I/O\strike0\{linkID=1360\}\cf0 , contains information about I/O operations on file descriptors. File descriptors are a lower-level mechanism specific to the Unix family of operating systems. 
\par \f1\'b7\f0  \cf1\strike File System Interface\strike0\{linkID=820\}\cf0 , has descriptions of operations on entire files, such as functions for deleting and renaming them and for creating new directories. This chapter also contains information about how you can access the attributes of a file, such as its owner and file protection modes. 
\par \f1\'b7\f0  \cf1\strike Mathematics\strike0\{linkID=1410\}\cf0 , contains information about the math library functions. These include things like random-number generators and remainder functions on integers as well as the usual trigonometric and exponential functions on floating-point numbers. 
\par \f1\'b7\f0  \cf1\strike Low-Level Arithmetic Functions\strike0\{linkID=100\}\cf0 , describes functions for simple arithmetic, analysis of floating-point values, and reading numbers from strings. 
\par \f1\'b7\f0  \cf1\strike Searching and Sorting\strike0\{linkID=1930\}\cf0 , contains information about functions for searching and sorting arrays. You can use these functions on any kind of array by providing an appropriate comparison function. 
\par \f1\'b7\f0  \cf1\strike Date and Time\strike0\{linkID=520\}\cf0 , describes functions for measuring both calendar time and CPU time, as well as functions for setting alarms and timers. 
\par \f1\'b7\f0  \cf1\strike Character Set Handling\strike0\{linkID=290\}\cf0 , contains information about manipulating characters and strings using character sets larger than will fit in the usual \f2 char\f0  data type. 
\par \f1\'b7\f0  \cf1\strike Locales\strike0\{linkID=1340\}\cf0 , describes how selecting a particular country or language affects the behavior of the library. For example, the locale affects collation sequences for strings and how monetary values are formatted. 
\par \f1\'b7\f0  \cf1\strike Non-Local Exits\strike0\{linkID=1510\}\cf0 , contains descriptions of the \f2 setjmp\f0  and\f2  longjmp\f0  functions. These functions provide a facility for\f2  goto\f0 -like jumps which can jump from one function to another. 
\par \f1\'b7\f0  \cf1\strike Signal Handling\strike0\{linkID=2010\}\cf0 , tells you all about signals--what they are, how to establish a handler that is called when a particular kind of signal is delivered, and how to prevent signals from arriving during critical sections of your program. 
\par \f1\'b7\f0  \cf1\strike Program Basics\strike0\{linkID=1750\}\cf0 , tells how your programs can access their command-line arguments and environment variables. 
\par 
\par }
1890
PAGE_513
Rounding Functions
convertingfloatstointegers;math.h;ceil;ceilf;ceill;floor;floorf;floorl;trunc;truncf;truncl;rint;rintf;rintl;nearbyint;nearbyintf;nearbyintl;round;roundf;roundl;lrint;lrintf;lrintl;llrint;llrintf;llrintl;lround;lroundf;lroundl;llround;llroundf;llroundl;modf;modff;modfl



Imported



FALSE
107
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Rounding Functions \{keepn\}
\par \pard\fs18 Next: \cf1\strike Remainder Functions\strike0\{linkID=1820\}\cf0 , Previous: \cf1\strike Normalization Functions\strike0\{linkID=1570\}\cf0 , Up: \cf1\strike Arithmetic Functions\strike0\{linkID=90\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Rounding Functions\b0 
\par 
\par The functions listed here perform operations such as rounding and truncation of floating-point values. Some of these functions convert floating point numbers to integer values. They are all declared in\f1  math.h\f0 . 
\par 
\par You can also convert floating-point numbers to integers simply by casting them to \f1 int\f0 . This discards the fractional part, effectively rounding towards zero. However, this only works if the result can actually be represented as an \f1 int\f0 --for very large numbers, this is impossible. The functions listed here return the result as a \f1 double\f0  instead to get around this problem.\f2 
\par \f0 
\par -- Function: double \b ceil\b0  (\i double x\i0 )
\par 
\par \pard\li500 -- Function: float \b ceilf\b0  (\i float x\i0 )
\par 
\par \pard\li1000 -- Function: long double \b ceill\b0  (\i long double x\i0 )
\par 
\par \pard\li1500 These functions round \i x\i0  upwards to the nearest integer, returning that value as a \f1 double\f0 . Thus, \f1 ceil (1.5)\f0  is \f1 2.0\f0 .
\par 
\par \pard -- Function: double \b floor\b0  (\i double x\i0 )
\par 
\par \pard\li500 -- Function: float \b floorf\b0  (\i float x\i0 )
\par 
\par \pard\li1000 -- Function: long double \b floorl\b0  (\i long double x\i0 )
\par 
\par \pard\li1500 These functions round \i x\i0  downwards to the nearest integer, returning that value as a \f1 double\f0 . Thus, \f1 floor (1.5)\f0  is \f1 1.0\f0  and \f1 floor (-1.5)\f0  is \f1 -2.0\f0 .
\par 
\par \pard -- Function: double \b trunc\b0  (\i double x\i0 )
\par 
\par \pard\li500 -- Function: float \b truncf\b0  (\i float x\i0 )
\par 
\par \pard\li1000 -- Function: long double \b truncl\b0  (\i long double x\i0 )
\par 
\par \pard\li1500 The \f1 trunc\f0  functions round \i x\i0  towards zero to the nearest integer (returned in floating-point format). Thus, \f1 trunc (1.5)\f0  is \f1 1.0\f0  and \f1 trunc (-1.5)\f0  is \f1 -1.0\f0 .
\par 
\par \pard -- Function: double \b rint\b0  (\i double x\i0 )
\par 
\par \pard\li500 -- Function: float \b rintf\b0  (\i float x\i0 )
\par 
\par \pard\li1000 -- Function: long double \b rintl\b0  (\i long double x\i0 )
\par 
\par \pard\li1500 These functions round \i x\i0  to an integer value according to the current rounding mode. See \cf1\strike Floating Point Parameters\strike0\{linkID=880\}\cf0 , for information about the various rounding modes. The default rounding mode is to round to the nearest integer; some machines support other modes, but round-to-nearest is always used unless you explicitly select another. 
\par 
\par If \i x\i0  was not initially an integer, these functions raise the inexact exception.
\par 
\par \pard -- Function: double \b nearbyint\b0  (\i double x\i0 )
\par 
\par \pard\li500 -- Function: float \b nearbyintf\b0  (\i float x\i0 )
\par 
\par \pard\li1000 -- Function: long double \b nearbyintl\b0  (\i long double x\i0 )
\par 
\par \pard\li1500 These functions return the same value as the \f1 rint\f0  functions, but do not raise the inexact exception if \i x\i0  is not an integer.
\par 
\par \pard -- Function: double \b round\b0  (\i double x\i0 )
\par 
\par \pard\li500 -- Function: float \b roundf\b0  (\i float x\i0 )
\par 
\par \pard\li1000 -- Function: long double \b roundl\b0  (\i long double x\i0 )
\par 
\par \pard\li1500 These functions are similar to \f1 rint\f0 , but they round halfway cases away from zero instead of to the nearest even integer.
\par 
\par \pard -- Function: long int \b lrint\b0  (\i double x\i0 )
\par 
\par \pard\li500 -- Function: long int \b lrintf\b0  (\i float x\i0 )
\par 
\par \pard\li1000 -- Function: long int \b lrintl\b0  (\i long double x\i0 )
\par 
\par \pard\li1500 These functions are just like \f1 rint\f0 , but they return a\f1  long int\f0  instead of a floating-point number.
\par 
\par \pard -- Function: long long int \b llrint\b0  (\i double x\i0 )
\par 
\par \pard\li500 -- Function: long long int \b llrintf\b0  (\i float x\i0 )
\par 
\par \pard\li1000 -- Function: long long int \b llrintl\b0  (\i long double x\i0 )
\par 
\par \pard\li1500 These functions are just like \f1 rint\f0 , but they return a\f1  long long int\f0  instead of a floating-point number.
\par 
\par \pard -- Function: long int \b lround\b0  (\i double x\i0 )
\par 
\par \pard\li500 -- Function: long int \b lroundf\b0  (\i float x\i0 )
\par 
\par \pard\li1000 -- Function: long int \b lroundl\b0  (\i long double x\i0 )
\par 
\par \pard\li1500 These functions are just like \f1 round\f0 , but they return a\f1  long int\f0  instead of a floating-point number.
\par 
\par \pard -- Function: long long int \b llround\b0  (\i double x\i0 )
\par 
\par \pard\li500 -- Function: long long int \b llroundf\b0  (\i float x\i0 )
\par 
\par \pard\li1000 -- Function: long long int \b llroundl\b0  (\i long double x\i0 )
\par 
\par \pard\li1500 These functions are just like \f1 round\f0 , but they return a\f1  long long int\f0  instead of a floating-point number.
\par 
\par \pard -- Function: double \b modf\b0  (\i double value, double *integer-part\i0 )
\par 
\par \pard\li500 -- Function: float \b modff\b0  (\i float value, float *integer-part\i0 )
\par 
\par \pard\li1000 -- Function: long double \b modfl\b0  (\i long double value, long double *integer-part\i0 )
\par 
\par \pard\li1500 These functions break the argument \i value\i0  into an integer part and a fractional part (between \f1 -1\f0  and \f1 1\f0 , exclusive). Their sum equals \i value\i0 . Each of the parts has the same sign as \i value\i0 , and the integer part is always rounded toward zero. 
\par 
\par \f1 modf\f0  stores the integer part in \f1 *\i\f0 integer-part\i0 , and returns the fractional part. For example, \f1 modf (2.5, &intpart)\f0  returns \f1 0.5\f0  and stores \f1 2.0\f0  into \f1 intpart\f0 .
\par 
\par }
1900
PAGE_514
Rounding
FE_TONEAREST;FE_UPWARD;FE_DOWNWARD;FE_TOWARDZERO;fegetround;fesetround



Imported



FALSE
54
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Rounding\{keepn\}
\par \pard\fs18 Next: \cf1\strike Control Functions\strike0\{linkID=420\}\cf0 , Previous: \cf1\strike Floating Point Errors\strike0\{linkID=860\}\cf0 , Up: \cf1\strike Arithmetic\strike0\{linkID=100\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Rounding Modes\b0 
\par 
\par Floating-point calculations are carried out internally with extra precision, and then rounded to fit into the destination type. This ensures that results are as precise as the input data. IEEE 754 defines four possible rounding modes:
\par 
\par Round to nearest.
\par \pard\li500 This is the default mode. It should be used unless there is a specific need for one of the others. In this mode results are rounded to the nearest representable value. If the result is midway between two representable values, the even representable is chosen. \i Even\i0  here means the lowest-order bit is zero. This rounding mode prevents statistical bias and guarantees numeric stability: round-off errors in a lengthy calculation will remain smaller than half of \f1 FLT_EPSILON\f0 .
\par \pard Round toward plus Infinity.
\par \pard\li500 All results are rounded to the smallest representable value which is greater than the result.
\par \pard Round toward minus Infinity.
\par \pard\li500 All results are rounded to the largest representable value which is less than the result. 
\par \pard Round toward zero.
\par \pard\li500 All results are rounded to the largest representable value whose magnitude is less than that of the result. In other words, if the result is negative it is rounded up; if it is positive, it is rounded down.
\par 
\par \pard\f1 fenv.h\f0  defines constants which you can use to refer to the various rounding modes. Each one will be defined if and only if the FPU supports the corresponding rounding mode.
\par 
\par \f1 FE_TONEAREST\f0 
\par \pard\li500 Round to nearest.
\par 
\par \pard\f1 FE_UPWARD\f0 
\par \pard\li500 Round toward +&infin;.
\par 
\par \pard\f1 FE_DOWNWARD\f0 
\par \pard\li500 Round toward -&infin;.
\par 
\par \pard\f1 FE_TOWARDZERO\f0 
\par \pard\li500 Round toward zero.
\par 
\par \pard Underflow is an unusual case. Normally, IEEE 754 floating point numbers are always normalized (see \cf1\strike Floating Point Concepts\strike0\{linkID=850\}\cf0 ). Numbers smaller than 2^r (where r is the minimum exponent,\f1  FLT_MIN_RADIX-1\f0  for \i float\i0 ) cannot be represented as normalized numbers. Rounding all such numbers to zero or 2^r would cause some algorithms to fail at 0. Therefore, they are left in denormalized form. That produces loss of precision, since some bits of the mantissa are stolen to indicate the decimal point. 
\par 
\par If a result is too small to be represented as a denormalized number, it is rounded to zero. However, the sign of the result is preserved; if the calculation was negative, the result is \i negative zero\i0 . Negative zero can also result from some operations on infinity, such as 4/-&infin;. Negative zero behaves identically to zero except when the \f1 copysign\f0  or \f1 signbit\f0  functions are used to check the sign bit directly. 
\par 
\par At any time one of the above four rounding modes is selected. You can find out which one with this function:
\par 
\par -- Function: int \b fegetround\b0  (\i void\i0 )
\par 
\par \pard\li500 Returns the currently selected rounding mode, represented by one of the values of the defined rounding mode macros.
\par 
\par \pard To change the rounding mode, use this function:
\par 
\par -- Function: int \b fesetround\b0  (\i int round\i0 )
\par 
\par \pard\li500 Changes the currently selected rounding mode to \i round\i0 . If\i  round\i0  does not correspond to one of the supported rounding modes nothing is changed. \f1 fesetround\f0  returns zero if it changed the rounding mode, a nonzero value if the mode is not supported.
\par 
\par \pard You should avoid changing the rounding mode if possible. It can be an expensive operation; also, some hardware requires you to compile your program differently for it to work. The resulting code may run slower. See your compiler documentation for details.
\par 
\par }
1910
PAGE_515
Running a Command
runningacommand;system;sh;stdlib.h



Imported



FALSE
23
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fswiss\fcharset0 Arial;}{\f2\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Running a Command \{keepn\}
\par \pard\fs18 Next: \cf1\strike Executing a File\cf2\strike0\{linkID=700\}\cf0 , \f1 Previous: \cf1\strike Aborting a Program\cf2\strike0\{linkID=20\}\cf0 , \f0 Up: \cf1\strike\f1 Top\cf2\strike0\{linkID=10\}\cf0\f0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Running a Command\b0 
\par 
\par The easy way to run another program is to use the \f2 system\f0  function. This function does all the work of running a subprogram, but it doesn't give you much control over the details: you have to wait until the subprogram terminates before you can do anything else.
\par -- Function: int \b system\b0  (\i const char *command\i0 )
\par 
\par \pard\li500 This function executes \i command\i0  as a shell command. In the  C library, it \f1 uses the file configured by the COMMAND environment variable\f0  to run the command\f1 , or 'cmd.exe' if there is none\f0 . In particular, it searches the directories in \f2 PATH\f0  to find programs to execute. The return value is \f2 -1\f0  if it wasn't possible to create the shell process, and otherwise is the status of the shell process.
\par 
\par If the \i command\i0  argument is a null pointer, a return value of zero indicates that no command processor is available. 
\par 
\par The \f2 system\f0  function is declared in the header file\f2  stdlib.h\f0 .
\par 
\par \pard\b Portability Note:\b0  Some C implementations may not have any notion of a command processor that can execute other programs. You can determine whether a command processor exists by executing\f2  system (NULL)\f0 ; if the return value is nonzero, a command processor is available. 
\par 
\par 
\par }
1920
PAGE_521
Search Functions
string.h;searchfunctions(forstrings;stringsearchfunctions;memchr;wmemchr;rawmemchr;memrchr;strchr;wcschr;strchrnul;wcschrnul;strrchr;wcsrchr;strstr;wcsstr;wcswcs;strcasestr;memmem;strspn;wcsspn;strcspn;wcscspn;strpbrk;wcspbrk;index;rindex



Imported



FALSE
128
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fswiss\fcharset0 Arial;}{\f3\fnil\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Search Functions \{keepn\}
\par \pard\fs18 Next: \cf1\strike Finding Tokens in a String\strike0\{linkID=830\}\cf0 , Previous: \cf1\strike Collation Functions\strike0\{linkID=370\}\cf0 , Up: \cf1\strike String and Array Utilities\strike0\{linkID=2160\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Search Functions\b0 
\par 
\par This section describes library functions which perform various kinds of searching operations on strings and arrays. These functions are declared in the header file \f1 string.h\f0 .
\par 
\par -- Function: void * \b memchr\b0  (\i const void *block, int c, size_t size\i0 )
\par 
\par \pard\li500 This function finds the first occurrence of the byte \i c\i0  (converted to an \f1 unsigned char\f0 ) in the initial \i size\i0  bytes of the object beginning at \i block\i0 . The return value is a pointer to the located byte, or a null pointer if no match was found.
\par 
\par \pard -- Function: wchar_t * \b wmemchr\b0  (\i const wchar_t *block, wchar_t wc, size_t size\i0 )
\par 
\par \pard\li500 This function finds the first occurrence of the wide character \i wc\i0  in the initial \i size\i0  wide characters of the object beginning at\i  block\i0 . The return value is a pointer to the located wide character, or a null pointer if no match was found.
\par 
\par 
\par \pard -- Function: void * \b memrchr\b0  (\i const void *block, int c, size_t size\i0 )
\par 
\par \pard\li500 The function \f1 memrchr\f0  is like \f1 memchr\f0 , except that it searches backwards from the end of the block defined by \i block\i0  and \i size\i0  (instead of forwards from the front).
\par 
\par \pard -- Function: char * \b strchr\b0  (\i const char *string, int c\i0 )
\par 
\par \pard\li500 The \f1 strchr\f0  function finds the first occurrence of the character\i  c\i0  (converted to a \f1 char\f0 ) in the null-terminated string beginning at \i string\i0 . The return value is a pointer to the located character, or a null pointer if no match was found. 
\par 
\par For example, 
\par 
\par \f1           strchr ("hello, world", 'l')
\par               => "llo, world"
\par           strchr ("hello, world", '?')
\par               => NULL
\par      
\par \f0 The terminating null character is considered to be part of the string, so you can use this function get a pointer to the end of a string by specifying a null character as the value of the \i c\i0  argument. It would be better (but less portable) to use \f1 strchrnul\f0  in this case, though.
\par 
\par \pard -- Function: wchar_t * \b wcschr\b0  (\i const wchar_t *wstring, int wc\i0 )
\par 
\par \pard\li500 The \f1 wcschr\f0  function finds the first occurrence of the wide character \i wc\i0  in the null-terminated wide character string beginning at \i wstring\i0 . The return value is a pointer to the located wide character, or a null pointer if no match was found. 
\par 
\par The terminating null character is considered to be part of the wide character string, so you can use this function get a pointer to the end of a wide character string by specifying a null wude character as the value of the \i wc\i0  argument. It would be better (but less portable) to use \f1 wcschrnul\f0  in this case, though.
\par 
\par \pard One useful, but unusual, use of the \f1 strchr\f0  function is when one wants to have a pointer pointing to the NUL byte terminating a string. This is often written in this way:
\par 
\par \f1        s += strlen (s);
\par \f0 
\par This is almost optimal but the addition operation duplicated a bit of the work already done in the \f1 strlen\f0  function. A better solution is this:
\par 
\par \f1        s = strchr (s, '\\0');
\par \f0 
\par There is no restriction on the second parameter of \f1 strchr\f0  so it could very well also be the NUL character. \f2 But this is slightly more expensive than the \f3 strlen\f2  function since \f3 strchr\f2  has two exit criteria.
\par \f0 
\par -- Function: char * \b strrchr\b0  (\i const char *string, int c\i0 )
\par 
\par \pard\li500 The function \f1 strrchr\f0  is like \f1 strchr\f0 , except that it searches backwards from the end of the string \i string\i0  (instead of forwards from the front). 
\par 
\par For example, 
\par 
\par \f1           strrchr ("hello, world", 'l')
\par               => "ld"
\par      
\par \pard\f0 -- Function: wchar_t * \b wcsrchr\b0  (\i const wchar_t *wstring, wchar_t c\i0 )
\par 
\par \pard\li500 The function \f1 wcsrchr\f0  is like \f1 wcschr\f0 , except that it searches backwards from the end of the string \i wstring\i0  (instead of forwards from the front).
\par 
\par \pard -- Function: char * \b strstr\b0  (\i const char *haystack, const char *needle\i0 )
\par 
\par \pard\li500 This is like \f1 strchr\f0 , except that it searches \i haystack\i0  for a substring \i needle\i0  rather than just a single character. It returns a pointer into the string \i haystack\i0  that is the first character of the substring, or a null pointer if no match was found. If\i  needle\i0  is an empty string, the function returns \i haystack\i0 . 
\par 
\par For example, 
\par 
\par \f1           strstr ("hello, world", "l")
\par               => "llo, world"
\par           strstr ("hello, world", "wo")
\par               => "world"
\par      
\par \pard\f0 -- Function: wchar_t * \b wcsstr\b0  (\i const wchar_t *haystack, const wchar_t *needle\i0 )
\par 
\par \pard\li500 This is like \f1 wcschr\f0 , except that it searches \i haystack\i0  for a substring \i needle\i0  rather than just a single wide character. It returns a pointer into the string \i haystack\i0  that is the first wide character of the substring, or a null pointer if no match was found. If\i  needle\i0  is an empty string, the function returns \i haystack\i0 .
\par 
\par \pard -- Function: size_t \b strspn\b0  (\i const char *string, const char *skipset\i0 )
\par 
\par \pard\li500 The \f1 strspn\f0  ("string span") function returns the length of the initial substring of \i string\i0  that consists entirely of characters that are members of the set specified by the string \i skipset\i0 . The order of the characters in \i skipset\i0  is not important. 
\par 
\par For example, 
\par 
\par \f1           strspn ("hello, world", "abcdefghijklmnopqrstuvwxyz")
\par               => 5
\par      
\par \f0 Note that "character" is here used in the sense of byte. In a string using a multibyte character encoding (abstract) character consisting of more than one byte are not treated as an entity. Each byte is treated separately. The function is not locale-dependent.
\par 
\par \pard -- Function: size_t \b wcsspn\b0  (\i const wchar_t *wstring, const wchar_t *skipset\i0 )
\par 
\par \pard\li500 The \f1 wcsspn\f0  ("wide character string span") function returns the length of the initial substring of \i wstring\i0  that consists entirely of wide characters that are members of the set specified by the string\i  skipset\i0 . The order of the wide characters in \i skipset\i0  is not important.
\par 
\par \pard -- Function: size_t \b strcspn\b0  (\i const char *string, const char *stopset\i0 )
\par 
\par \pard\li500 The \f1 strcspn\f0  ("string complement span") function returns the length of the initial substring of \i string\i0  that consists entirely of characters that are \i not\i0  members of the set specified by the string \i stopset\i0 . (In other words, it returns the offset of the first character in \i string\i0  that is a member of the set \i stopset\i0 .) 
\par 
\par For example, 
\par 
\par \f1           strcspn ("hello, world", " \\t\\n,.;!?")
\par               => 5
\par      
\par \f0 Note that "character" is here used in the sense of byte. In a string using a multibyte character encoding (abstract) character consisting of more than one byte are not treated as an entity. Each byte is treated separately. The function is not locale-dependent.
\par 
\par \pard -- Function: size_t \b wcscspn\b0  (\i const wchar_t *wstring, const wchar_t *stopset\i0 )
\par 
\par \pard\li500 The \f1 wcscspn\f0  ("wide character string complement span") function returns the length of the initial substring of \i wstring\i0  that consists entirely of wide characters that are \i not\i0  members of the set specified by the string \i stopset\i0 . (In other words, it returns the offset of the first character in \i string\i0  that is a member of the set \i stopset\i0 .)
\par 
\par \pard -- Function: char * \b strpbrk\b0  (\i const char *string, const char *stopset\i0 )
\par 
\par \pard\li500 The \f1 strpbrk\f0  ("string pointer break") function is related to\f1  strcspn\f0 , except that it returns a pointer to the first character in \i string\i0  that is a member of the set \i stopset\i0  instead of the length of the initial substring. It returns a null pointer if no such character from \i stopset\i0  is found.
\par 
\par For example, 
\par 
\par \f1           strpbrk ("hello, world", " \\t\\n,.;!?")
\par               => ", world"
\par      
\par \f0 Note that "character" is here used in the sense of byte. In a string using a multibyte character encoding (abstract) character consisting of more than one byte are not treated as an entity. Each byte is treated separately. The function is not locale-dependent.
\par 
\par \pard -- Function: wchar_t * \b wcspbrk\b0  (\i const wchar_t *wstring, const wchar_t *stopset\i0 )
\par 
\par \pard\li500 The \f1 wcspbrk\f0  ("wide character string pointer break") function is related to \f1 wcscspn\f0 , except that it returns a pointer to the first wide character in \i wstring\i0  that is a member of the set\i  stopset\i0  instead of the length of the initial substring. It returns a null pointer if no such character from \i stopset\i0  is found.
\par \pard\b 
\par }
1930
PAGE_522
Searching and Sorting




Imported



FALSE
17
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern\fcharset0 Courier New;}{\f2\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Searching and Sorting \{keepn\}
\par \pard\fs18 Next: \cf1\strike I/O Overview\cf2\strike0\{linkID=1270\}\cf0 , Previous: \cf1\strike Locales\cf2\strike0\{linkID=1340\}\cf0 , Up: \cf1\strike Top\strike0\{linkID=10\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Searching and Sorting\b0 
\par 
\par This chapter describes functions for searching and sorting arrays of arbitrary objects. You pass the appropriate comparison function to be applied as an argument, along with the size of the objects in the array and the total number of elements.
\par 
\par \pard\li500\f1\'b7\f0  \cf1\strike Comparison Functions\strike0\{linkID=380\}\cf0 : Defining how to compare two objects. Since the sort and search facilities are general, you have to specify the ordering.
\par \f1\'b7\f0  \cf1\strike Array Search Function\strike0\{linkID=110\}\cf0 : The \f2 bsearch\f0  function.
\par \f1\'b7\f0  \cf1\strike Array Sort Function\strike0\{linkID=120\}\cf0 : The \f2 qsort\f0  function.
\par \f1\'b7\f0  \cf1\strike Search/Sort Example\strike0\{linkID=1940\}\cf0 : An example program.\cf1\strike 
\par }
1940
PAGE_523
Search/Sort Example
Kermitthefrog



Imported



FALSE
143
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Search/Sort Example \{keepn\}
\par \pard\fs18 Previous: \cf1\strike Array Sort Function\strike0\{linkID=120\}\cf0 , Up: \cf1\strike Searching and Sorting\strike0\{linkID=1930\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Searching and Sorting Example\b0 
\par 
\par Here is an example showing the use of \f1 qsort\f0  and \f1 bsearch\f0  with an array of structures. The objects in the array are sorted by comparing their \f1 name\f0  fields with the \f1 strcmp\f0  function. Then, we can look up individual objects based on their names.
\par 
\par \f1      #include <stdlib.h>
\par      #include <stdio.h>
\par      #include <string.h>
\par      
\par      /* Define an array of critters to sort. */
\par      
\par      struct critter
\par        \{
\par          const char *name;
\par          const char *species;
\par        \};
\par      
\par      struct critter muppets[] =
\par        \{
\par          \{"Kermit", "frog"\},
\par          \{"Piggy", "pig"\},
\par          \{"Gonzo", "whatever"\},
\par          \{"Fozzie", "bear"\},
\par          \{"Sam", "eagle"\},
\par          \{"Robin", "frog"\},
\par          \{"Animal", "animal"\},
\par          \{"Camilla", "chicken"\},
\par          \{"Sweetums", "monster"\},
\par          \{"Dr. Strangepork", "pig"\},
\par          \{"Link Hogthrob", "pig"\},
\par          \{"Zoot", "human"\},
\par          \{"Dr. Bunsen Honeydew", "human"\},
\par          \{"Beaker", "human"\},
\par          \{"Swedish Chef", "human"\}
\par        \};
\par      
\par      int count = sizeof (muppets) / sizeof (struct critter);
\par      
\par      
\par      
\par      /* This is the comparison function used for sorting and searching. */
\par      
\par      int
\par      critter_cmp (const struct critter *c1, const struct critter *c2)
\par      \{
\par        return strcmp (c1->name, c2->name);
\par      \}
\par      
\par      
\par      /* Print information about a critter. */
\par      
\par      void
\par      print_critter (const struct critter *c)
\par      \{
\par        printf ("%s, the %s\\n", c->name, c->species);
\par      \}
\par      
\par      
\par      /* Do the lookup into the sorted array. */
\par      
\par      void
\par      find_critter (const char *name)
\par      \{
\par        struct critter target, *result;
\par        target.name = name;
\par        result = bsearch (&target, muppets, count, sizeof (struct critter),
\par                          critter_cmp);
\par        if (result)
\par          print_critter (result);
\par        else
\par          printf ("Couldn't find %s.\\n", name);
\par      \}
\par      
\par      /* Main program. */
\par      
\par      int
\par      main (void)
\par      \{
\par        int i;
\par      
\par        for (i = 0; i < count; i++)
\par          print_critter (&muppets[i]);
\par        printf ("\\n");
\par      
\par        qsort (muppets, count, sizeof (struct critter), critter_cmp);
\par      
\par        for (i = 0; i < count; i++)
\par          print_critter (&muppets[i]);
\par        printf ("\\n");
\par      
\par        find_critter ("Kermit");
\par        find_critter ("Gonzo");
\par        find_critter ("Janice");
\par      
\par        return 0;
\par      \}
\par \f0 
\par The output from this program looks like:
\par 
\par \f1      Kermit, the frog
\par      Piggy, the pig
\par      Gonzo, the whatever
\par      Fozzie, the bear
\par      Sam, the eagle
\par      Robin, the frog
\par      Animal, the animal
\par      Camilla, the chicken
\par      Sweetums, the monster
\par      Dr. Strangepork, the pig
\par      Link Hogthrob, the pig
\par      Zoot, the human
\par      Dr. Bunsen Honeydew, the human
\par      Beaker, the human
\par      Swedish Chef, the human
\par      
\par      Animal, the animal
\par      Beaker, the human
\par      Camilla, the chicken
\par      Dr. Bunsen Honeydew, the human
\par      Dr. Strangepork, the pig
\par      Fozzie, the bear
\par      Gonzo, the whatever
\par      Kermit, the frog
\par      Link Hogthrob, the pig
\par      Piggy, the pig
\par      Robin, the frog
\par      Sam, the eagle
\par      Swedish Chef, the human
\par      Sweetums, the monster
\par      Zoot, the human
\par      
\par      Kermit, the frog
\par      Gonzo, the whatever
\par      Couldn't find Janice.
\par \f0 
\par }
1950
PAGE_524
Selecting the Conversion
MB_LEN_MAX;limits.h;MB_CUR_MAX;stdlib.h



Imported



FALSE
41
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Selecting the Conversion \{keepn\}
\par \pard\fs18 Next: \cf1\strike Keeping the state\strike0\{linkID=1290\}\cf0 , Up: \cf1\strike Restartable multibyte conversion\strike0\{linkID=1870\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Selecting the conversion and its properties\b0 
\par 
\par We already said above that the currently selected locale for the\f1  LC_CTYPE\f0  category decides about the conversion that is performed by the functions we are about to describe. Each locale uses its own character set (given as an argument to \f1 localedef\f0 ) and this is the one assumed as the external multibyte encoding. The wide character character set always is UCS-\f2 2.\f0 
\par 
\par A characteristic of each multibyte character set is the maximum number of bytes that can be necessary to represent one character. This information is quite important when writing code that uses the conversion functions (as shown in the examples below). The ISO C standard defines two macros that provide this information.
\par 
\par -- Macro: int \b MB_LEN_MAX\b0 
\par 
\par \pard\li500\f1 MB_LEN_MAX\f0  specifies the maximum number of bytes in the multibyte sequence for a single character in any of the supported locales. It is a compile-time constant and is defined in \f1 limits.h\f0 .
\par 
\par \pard -- Macro: int \b MB_CUR_MAX\b0 
\par 
\par \pard\li500\f1 MB_CUR_MAX\f0  expands into a positive integer expression that is the maximum number of bytes in a multibyte character in the current locale. The value is never greater than \f1 MB_LEN_MAX\f0 . Unlike\f1  MB_LEN_MAX\f0  this macro need not be a compile-time constant, and in th\f2 is\f0  C library it is not. 
\par 
\par \f1 MB_CUR_MAX\f0  is defined in \f1 stdlib.h\f0 .
\par 
\par \pard Two different macros are necessary since strictly ISO C90 compilers do not allow variable length array definitions, but still it is desirable to avoid dynamic allocation. This incomplete piece of code shows the problem:
\par 
\par \f1      \{
\par        char buf[MB_LEN_MAX];
\par        ssize_t len = 0;
\par      
\par        while (! feof (fp))
\par          \{
\par            fread (&buf[len], 1, MB_CUR_MAX - len, fp);
\par            /* ... process buf */
\par            len -= used;
\par          \}
\par      \}
\par \f0 
\par The code in the inner loop is expected to have always enough bytes in the array \i buf\i0  to convert one multibyte character. The array\i  buf\i0  has to be sized statically since many compilers do not allow a variable size. The \f1 fread\f0  call makes sure that \f1 MB_CUR_MAX\f0  bytes are always available in \i buf\i0 . Note that it isn't a problem if \f1 MB_CUR_MAX\f0  is not a compile-time constant.
\par 
\par }
1960
PAGE_536
Setting the Locale
changingthelocale;locale,changing;locale.h;setlocale;locale



Imported



FALSE
79
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Setting the Locale \{keepn\}
\par \pard\fs18 Next: \cf1\strike Standard Locales\strike0\{linkID=2080\}\cf0 , Previous: \cf1\strike Locale Categories\strike0\{linkID=1320\}\cf0 , Up: \cf1\strike Locales\strike0\{linkID=1340\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b How Programs Set the Locale\b0 
\par 
\par A C program inherits its locale environment variables when it starts up. This happens automatically. However, these variables do not automatically control the locale used by the library functions, because ISO C says that all programs start by default in the standard `\f1 C\f0 ' locale. To use the locales specified by the environment, you must call\f1  setlocale\f0 . Call it as follows:
\par 
\par \f1      setlocale (LC_ALL, "");
\par \f0 
\par to select a locale based on the user choice of the appropriate environment variables. 
\par 
\par You can also use \f1 setlocale\f0  to specify a particular locale, for general use or for a specific category. 
\par 
\par The symbols in this section are defined in the header file \f1 locale.h\f0 .
\par -- Function: char * \b setlocale\b0  (\i int category, const char *locale\i0 )
\par 
\par \pard\li500 The function \f1 setlocale\f0  sets the current locale for category\i  category\i0  to \i locale\i0 . A list of all the locales the system provides can be created by running
\par 
\par \f1             locale -a
\par      
\par \f0 If \i category\i0  is \f1 LC_ALL\f0 , this specifies the locale for all purposes. The other possible values of \i category\i0  specify an single purpose (see \cf1\strike Locale Categories\strike0\{linkID=1320\}\cf0 ). 
\par 
\par You can also use this function to find out the current locale by passing a null pointer as the \i locale\i0  argument. In this case,\f1  setlocale\f0  returns a string that is the name of the locale currently selected for category \i category\i0 . 
\par 
\par The string returned by \f1 setlocale\f0  can be overwritten by subsequent calls, so you should make a copy of the string (see \cf1\strike Copying and Concatenation\strike0\{linkID=460\}\cf0 ) if you want to save it past any further calls to\f1  setlocale\f0 . (The standard library is guaranteed never to call\f1  setlocale\f0  itself.) 
\par 
\par You should not modify the string returned by \f1 setlocale\f0 . It might be the same string that was passed as an argument in a previous call to\f1  setlocale\f0 . One requirement is that the \i category\i0  must be the same in the call the string was returned and the one when the string is passed in as \i locale\i0  parameter. 
\par 
\par When you read the current locale for category \f1 LC_ALL\f0 , the value encodes the entire combination of selected locales for all categories. In this case, the value is not just a single locale name. In fact, we don't make any promises about what it looks like. But if you specify the same "locale name" with \f1 LC_ALL\f0  in a subsequent call to\f1  setlocale\f0 , it restores the same combination of locale selections. 
\par 
\par To be sure you can use the returned string encoding the currently selected locale at a later time, you must make a copy of the string. It is not guaranteed that the returned pointer remains valid over time. 
\par 
\par When the \i locale\i0  argument is not a null pointer, the string returned by \f1 setlocale\f0  reflects the newly-modified locale. 
\par 
\par If you specify an empty string for \i locale\i0 , this means to read the appropriate environment variable and use its value to select the locale for \i category\i0 . 
\par 
\par If a nonempty string is given for \i locale\i0 , then the locale of that name is used if possible. 
\par 
\par If you specify an invalid locale name, \f1 setlocale\f0  returns a null pointer and leaves the current locale unchanged.
\par 
\par \pard Here is an example showing how you might use \f1 setlocale\f0  to temporarily switch to a new locale.
\par 
\par \f1      #include <stddef.h>
\par      #include <locale.h>
\par      #include <stdlib.h>
\par      #include <string.h>
\par      
\par      void
\par      with_other_locale (char *new_locale,
\par                         void (*subroutine) (int),
\par                         int argument)
\par      \{
\par        char *old_locale, *saved_locale;
\par      
\par        /* Get the name of the current locale.  */
\par        old_locale = setlocale (LC_ALL, NULL);
\par      
\par        /* Copy the name so it won't be clobbered by setlocale. */
\par        saved_locale = strdup (old_locale);
\par        if (saved_locale == NULL)
\par          fatal ("Out of memory");
\par      
\par        /* Now change the locale and do some stuff with it. */
\par        setlocale (LC_ALL, new_locale);
\par        (*subroutine) (argument);
\par      
\par        /* Restore the original locale. */
\par        setlocale (LC_ALL, saved_locale);
\par        free (saved_locale);
\par      \}
\par \f0 
\par \b Portability Note:\b0  Some ISO C systems may define additional locale categories, and future versions of the library will do so. For portability, assume that any symbol beginning with `\f1 LC_\f0 ' might be defined in \f1 locale.h\f0 .
\par 
\par }
1970
PAGE_539
Shift State




Imported



FALSE
49
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Shift State \{keepn\}
\par \pard\fs18 Previous: \cf1\strike Non-reentrant String Conversion\strike0\{linkID=1550\}\cf0 , Up: \cf1\strike Non-reentrant Conversion\strike0\{linkID=1540\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b States in Non-reentrant Functions\b0 
\par 
\par In some multibyte character codes, the \i meaning\i0  of any particular byte sequence is not fixed; it depends on what other sequences have come earlier in the same string. Typically there are just a few sequences that can change the meaning of other sequences; these few are called\i  shift sequences\i0  and we say that they set the \i shift state\i0  for other sequences that follow. 
\par 
\par To illustrate shift state and shift sequences, suppose we decide that the sequence \f1 0200\f0  (just one byte) enters Japanese mode, in which pairs of bytes in the range from \f1 0240\f0  to \f1 0377\f0  are single characters, while \f1 0201\f0  enters Latin-1 mode, in which single bytes in the range from \f1 0240\f0  to \f1 0377\f0  are characters, and interpreted according to the ISO Latin-1 character set. This is a multibyte code that has two alternative shift states ("Japanese mode" and "Latin-1 mode"), and two shift sequences that specify particular shift states. 
\par 
\par When the multibyte character code in use has shift states, then\f1  mblen\f0 , \f1 mbtowc\f0 , and \f1 wctomb\f0  must maintain and update the current shift state as they scan the string. To make this work properly, you must follow these rules:
\par 
\par \pard\li500\f2\'b7\f0  Before starting to scan a string, call the function with a null pointer for the multibyte character address--for example, \f1 mblen (NULL, 0)\f0 . This initializes the shift state to its standard initial value. 
\par \f2\'b7\f0  Scan the string one character at a time, in order. Do not "back up" and rescan characters already scanned, and do not intersperse the processing of different strings.
\par 
\par \pard Here is an example of using \f1 mblen\f0  following these rules:
\par 
\par \f1      void
\par      scan_string (char *s)
\par      \{
\par        int length = strlen (s);
\par      
\par        /* Initialize shift state.  */
\par        mblen (NULL, 0);
\par      
\par        while (1)
\par          \{
\par            int thischar = mblen (s, length);
\par            /* Deal with end of string and invalid characters.  */
\par            if (thischar == 0)
\par              break;
\par            if (thischar == -1)
\par              \{
\par                error ("invalid multibyte character");
\par                break;
\par              \}
\par            /* Advance past this character.  */
\par            s += thischar;
\par            length -= thischar;
\par          \}
\par      \}
\par \f0 
\par The functions \f1 mblen\f0 , \f1 mbtowc\f0  and \f1 wctomb\f0  are not reentrant when using a multibyte code that uses a shift state. However, no other library functions call these functions, so you don't have to worry that the shift state will be changed mysteriously.
\par 
\par }
1980
PAGE_541
Sign of Money Amount




Imported



FALSE
43
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Sign of Money Amount \{keepn\}
\par \pard\fs18 Previous: \cf1\strike Currency Symbol\strike0\{linkID=500\}\cf0 , Up: \cf1\strike The Lame Way to Locale Data\strike0\{linkID=2280\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Printing the Sign of a Monetary Amount\b0 
\par 
\par These members of the \f1 struct lconv\f0  structure specify how to print the sign (if any) of a monetary value.
\par 
\par \f1 char *positive_sign\f0 
\par \f1 char *negative_sign\f0 
\par \pard\li500 These are strings used to indicate positive (or zero) and negative monetary quantities, respectively. 
\par 
\par In the standard `\f1 C\f0 ' locale, both of these members have a value of\f1  ""\f0  (the empty string), meaning "unspecified". 
\par 
\par The ISO standard doesn't say what to do when you find this value; we recommend printing \f1 positive_sign\f0  as you find it, even if it is empty. For a negative value, print \f1 negative_sign\f0  as you find it unless both it and \f1 positive_sign\f0  are empty, in which case print `\f1 -\f0 ' instead. (Failing to indicate the sign at all seems rather unreasonable.) 
\par \pard\f1 char p_sign_posn\f0 
\par \f1 char n_sign_posn\f0 
\par \f1 char int_p_sign_posn\f0 
\par \f1 char int_n_sign_posn\f0 
\par \pard\li500 These members are small integers that indicate how to position the sign for nonnegative and negative monetary quantities, respectively. (The string used by the sign is what was specified with\f1  positive_sign\f0  or \f1 negative_sign\f0 .) The possible values are as follows:
\par 
\par \f1 0\f0 
\par \pard\li1000 The currency symbol and quantity should be surrounded by parentheses. 
\par \pard\li500\f1 1\f0 
\par \pard\li1000 Print the sign string before the quantity and currency symbol. 
\par \pard\li500\f1 2\f0 
\par \pard\li1000 Print the sign string after the quantity and currency symbol. 
\par \pard\li500\f1 3\f0 
\par \pard\li1000 Print the sign string right before the currency symbol. 
\par \pard\li500\f1 4\f0 
\par \pard\li1000 Print the sign string right after the currency symbol. 
\par \pard\li500\f1 CHAR_MAX\f0 
\par \pard\li1000 "Unspecified". Both members have this value in the standard `\f1 C\f0 ' locale.
\par 
\par \pard\li500 The ISO standard doesn't say what you should do when the value is\f1  CHAR_MAX\f0 . We recommend you print the sign after the currency symbol. 
\par 
\par The members with the \f1 int_\f0  prefix apply to the\f1  int_curr_symbol\f0  while the other two apply to\f1  currency_symbol\f0 .
\par 
\par }
1990
PAGE_542
Signal Actions
signalactions;establishingahandler



Imported



FALSE
15
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Signal Actions \{keepn\}
\par \pard\fs18 Next: \cf1\strike Defining Handlers\strike0\{linkID=530\}\cf0 , Previous: \cf1\strike Standard Signals\strike0\{linkID=2090\}\cf0 , Up: \cf1\strike Signal Handling\strike0\{linkID=2010\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Specifying Signal Actions\b0 
\par 
\par The simplest way to change the action for a signal is to use the\f1  signal\f0  function. You can specify a built-in action (such as to ignore the signal), or you can \i establish a handler\i0 . 
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Basic Signal Handling\strike0\{linkID=180\}\cf0 : The simple \f1 signal\f0  function.
\par \cf1\strike 
\par }
2000
PAGE_545
Signal Generation
generationofsignals



Imported



FALSE
21
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Signal Generation \{keepn\}
\par \pard\fs18 Previous: \cf1\strike Kinds of Signals\strike0\{linkID=1300\}\cf0 , Up: \cf1\strike Concepts of Signals\strike0\{linkID=400\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Concepts of Signal Generation\b0 
\par 
\par In general, the events that generate signals fall into three major categories: errors, external events, and explicit requests. 
\par 
\par An error means that a program has done something invalid and cannot continue execution. But not all kinds of errors generate signals--in fact, most do not. For example, opening a nonexistent file is an error, but it does not raise a signal; instead, \f1 open\f0  returns \f1 -1\f0 . In general, errors that are necessarily associated with certain library functions are reported by returning a value that indicates an error. The errors which raise signals are those which can happen anywhere in the program, not just in library calls. These include division by zero and invalid memory addresses. 
\par 
\par Signals may be generated \i synchronously\i0  or \i asynchronously\i0 . A synchronous signal pertains to a specific action in the program, and is delivered (unless blocked) during that action. Most errors generate signals synchronously, and so do explicit requests by a process to generate a signal for that same process. On some machines, certain kinds of hardware errors (usually floating-point exceptions) are not reported completely synchronously, but may arrive a few instructions later. 
\par 
\par Asynchronous signals are generated by events outside the control of the process that receives them. These signals arrive at unpredictable times during execution. External events generate signals asynchronously, and so do explicit requests that apply to some other process. 
\par 
\par A given type of signal is either typically synchronous or typically asynchronous. For example, signals for errors are typically synchronous because errors generate signals synchronously. But any type of signal can be generated synchronously or asynchronously with an explicit request.
\par 
\par }
2010
PAGE_546
Signal Handling
signal



Imported



FALSE
25
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Signal Handling \{keepn\}
\par \pard\fs18 Next: \cf1\strike Program Basics\strike0\{linkID=1750\}\cf0 , Previous: \cf1\strike Non-Local Exits\strike0\{linkID=1510\}\cf0 , Up: \cf1\strike Top\strike0\{linkID=10\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Signal Handling\b0 
\par 
\par A \i signal\i0  is a software interrupt delivered to a process. The operating system uses signals to report exceptional situations to an executing program. Some signals report errors such as references to invalid memory addresses; others report asynchronous events, such as disconnection of a phone line. 
\par 
\par The C library defines a variety of signal types, each for a particular kind of event. Some kinds of events make it inadvisable or impossible for the program to proceed as usual, and the corresponding signals normally abort the program. Other kinds of signals that report harmless events are ignored by default. 
\par 
\par If you anticipate an event that causes signals, you can define a handler function and tell the operating system to run it when that particular type of signal arrives. 
\par 
\par Finally, one process can send a signal to another process; this allows a parent process to abort a child, or two related processes to communicate and synchronize.
\par 
\par \pard\li500\f1\'b7\f0  \cf1\strike Concepts of Signals\strike0\{linkID=400\}\cf0 : Introduction to the signal facilities.
\par \f1\'b7\f0  \cf1\strike Standard Signals\strike0\{linkID=2090\}\cf0 : Particular kinds of signals with standard names and meanings.
\par \f1\'b7\f0  \cf1\strike Signal Actions\strike0\{linkID=1990\}\cf0 : Specifying what happens when a particular signal is delivered.
\par \f1\'b7\f0  \cf1\strike Defining Handlers\strike0\{linkID=530\}\cf0 : How to write a signal handler function.
\par \f1\'b7\f0  \cf1\strike Generating Signals\strike0\{linkID=1050\}\cf0 : How to send a signal to a process.
\par \f1 
\par }
2020
PAGE_551
Signaling Yourself
signal.h;raise;gsignal



Imported



FALSE
17
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Signaling Yourself \{keepn\}
\par \pard\fs18 Up: \cf1\strike Generating Signals\strike0\{linkID=1050\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Signaling Yourself\b0 
\par 
\par A process can send itself a signal with the \f1 raise\f0  function. This function is declared in \f1 signal.h\f0 .
\par -- Function: int \b raise\b0  (\i int signum\i0 )
\par 
\par \pard\li500 The \f1 raise\f0  function sends the signal \i signum\i0  to the calling process. It returns zero if successful and a nonzero value if it fails. About the only reason for failure would be if the value of \i signum\i0  is invalid.
\par 
\par \pard\b Portability note:\b0  \f1 raise\f0  was invented by the ISO C committee. Older systems may not support it\f2 .\f0 
\par }
2030
PAGE_552
Signals in Handler
raceconditions,relatingtosignals



Imported



FALSE
15
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Signals in Handler \{keepn\}
\par \pard\fs18 Next: \cf1\strike Nonreentrancy\cf2\strike0\{linkID=1480\}\cf0 , Previous: \cf1\strike Longjmp in Handler\strike0\{linkID=1350\}\cf0 , Up: \cf1\strike Defining Handlers\strike0\{linkID=530\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Signals Arriving While a Handler Runs\b0 
\par 
\par What happens if another signal arrives while your signal handler function is running? 
\par 
\par When the handler for a particular signal is invoked, that signal is automatically blocked until the handler returns. That means that if two signals of the same kind arrive close together, the second one will be held until the first has been handled. 
\par 
\par }
2040
PAGE_554
Simple Calendar Time
time.h;epoch;time_t;time;stime



Imported



FALSE
31
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fswiss\fcharset0 Arial;}{\f3\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Simple Calendar Time \{keepn\}
\par \pard\fs18 Next: \cf1\strike Broken-down Time\cf2\strike0\{linkID=210\}\cf0 , Up: \cf1\strike Calendar Time\strike0\{linkID=230\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Simple Calendar Time\b0 
\par 
\par This section describes the \f1 time_t\f0  data type for representing calendar time as simple time, and the functions which operate on simple time objects. These facilities are declared in the header file \f1 time.h\f0 .
\par -- Data Type: \b time_t\b0 
\par 
\par \pard\li500 This is the data type used to represent simple time. Sometimes, it also represents an elapsed time. When interpreted as a calendar time value, it represents the number of seconds elapsed since 00:00:00 on January 1, 1970, Coordinated Universal Time. (This calendar time is sometimes referred to as the \i epoch\i0 .) POSIX requires that this count not include leap seconds, but on some systems this count includes leap seconds if you set \f1 TZ\f0  to certain values (see \cf1\strike TZ Variable\strike0\{linkID=2330\}\cf0 ). 
\par 
\par Note that a simple time has no concept of local time zone. Calendar Time \i T\i0  is the same instant in time regardless of where on the globe the computer is. 
\par 
\par In th\f2 is\f0  C library, \f1 time_t\f0  is equivalent to \f1 long\f3  \f1 int\f0 . In other systems, \f1 time_t\f0  might be either an integer or floating-point type.
\par 
\par \pard The function \f1 difftime\f0  tells you the elapsed time between two simple calendar times, which is not always as easy to compute as just subtracting. See \cf1\strike Elapsed Time\strike0\{linkID=620\}\cf0 .
\par 
\par -- Function: time_t \b time\b0  (\i time_t *result\i0 )
\par 
\par \pard\li500 The \f1 time\f0  function returns the current calendar time as a value of type \f1 time_t\f0 . If the argument \i result\i0  is not a null pointer, the calendar time value is also stored in \f1 *\i\f0 result\i0 . If the current calendar time is not available, the value\f1  (time_t)(-1)\f0  is returned.
\par 
\par \pard -- Function: int \b stime\b0  (\i time_t *newtime\i0 )
\par 
\par \pard\li500\f1 stime\f0  sets the system clock, i.e. it tells the system that the current calendar time is \i newtime\i0 , where \f1 newtime\f0  is interpreted as described in the above definition of \f1 time_t\f0 . 
\par 
\par If the function succeeds, the return value is zero.
\par }
2050
PAGE_557
Simple Output
writingtoastream,bycharacters;stdio.h;wchar.h;fputc;fputwc;fputc_unlocked;fputwc_unlocked;putc;putwc;putc_unlocked;putwc_unlocked;putchar;putwchar;putchar_unlocked;putwchar_unlocked;fputs;fputws;fputs_unlocked;fputws_unlocked;puts;putw



Imported



FALSE
68
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Simple Output \{keepn\}
\par \pard\fs18 Next: \cf1\strike Character Input\strike0\{linkID=280\}\cf0 , Previous: \cf1\strike Streams and I18N\strike0\{linkID=2140\}\cf0 , Up: \cf1\strike I/O on Streams\strike0\{linkID=1260\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Simple Output by Characters or Lines\b0 
\par 
\par This section describes functions for performing character- and line-oriented output. 
\par 
\par These narrow streams functions are declared in the header file\f1  stdio.h\f0  and the wide stream functions in \f1 wchar.h\f0 .
\par 
\par -- Function: int \b fputc\b0  (\i int c, FILE *stream\i0 )
\par 
\par \pard\li500 The \f1 fputc\f0  function converts the character \i c\i0  to type\f1  unsigned char\f0 , and writes it to the stream \i stream\i0 .\f1  EOF\f0  is returned if a write error occurs; otherwise the character \i c\i0  is returned.
\par 
\par \pard -- Function: wint_t \b fputwc\b0  (\i wchar_t wc, FILE *stream\i0 )
\par 
\par \pard\li500 The \f1 fputwc\f0  function writes the wide character \i wc\i0  to the stream \i stream\i0 . \f1 WEOF\f0  is returned if a write error occurs; otherwise the character \i wc\i0  is returned.
\par \pard 
\par -- Function: int \b putc\b0  (\i int c, FILE *stream\i0 )
\par 
\par \pard\li500 This is just like \f1 fputc\f0 , except that most systems implement it as a macro, making it faster. One consequence is that it may evaluate the\i  stream\i0  argument more than once, which is an exception to the general rule for macros. \f1 putc\f0  is usually the best function to use for writing a single character.
\par 
\par \pard -- Function: wint_t \b putwc\b0  (\i wchar_t wc, FILE *stream\i0 )
\par 
\par \pard\li500 This is just like \f1 fputwc\f0 , except that it can be implement as a macro, making it faster. One consequence is that it may evaluate the\i  stream\i0  argument more than once, which is an exception to the general rule for macros. \f1 putwc\f0  is usually the best function to use for writing a single wide character.
\par 
\par \pard -- Function: int \b putchar\b0  (\i int c\i0 )
\par 
\par \pard\li500 The \f1 putchar\f0  function is equivalent to \f1 putc\f0  with\f1  stdout\f0  as the value of the \i stream\i0  argument.
\par 
\par \pard -- Function: wint_t \b putwchar\b0  (\i wchar_t wc\i0 )
\par 
\par \pard\li500 The \f1 putwchar\f0  function is equivalent to \f1 putwc\f0  with\f1  stdout\f0  as the value of the \i stream\i0  argument.
\par 
\par \pard -- Function: int \b fputs\b0  (\i const char *s, FILE *stream\i0 )
\par 
\par \pard\li500 The function \f1 fputs\f0  writes the string \i s\i0  to the stream\i  stream\i0 . The terminating null character is not written. This function does \i not\i0  add a newline character, either. It outputs only the characters in the string. 
\par 
\par This function returns \f1 EOF\f0  if a write error occurs, and otherwise a non-negative value. 
\par 
\par For example: 
\par 
\par \f1           fputs ("Are ", stdout);
\par           fputs ("you ", stdout);
\par           fputs ("hungry?\\n", stdout);
\par      
\par \f0 outputs the text `\f1 Are you hungry?\f0 ' followed by a newline.
\par 
\par \pard -- Function: int \b fputws\b0  (\i const wchar_t *ws, FILE *stream\i0 )
\par 
\par \pard\li500 The function \f1 fputws\f0  writes the wide character string \i ws\i0  to the stream \i stream\i0 . The terminating null character is not written. This function does \i not\i0  add a newline character, either. It outputs only the characters in the string. 
\par 
\par This function returns \f1 WEOF\f0  if a write error occurs, and otherwise a non-negative value.
\par 
\par \pard -- Function: int \b puts\b0  (\i const char *s\i0 )
\par 
\par \pard\li500 The \f1 puts\f0  function writes the string \i s\i0  to the stream\f1  stdout\f0  followed by a newline. The terminating null character of the string is not written. (Note that \f1 fputs\f0  does \i not\i0  write a newline as this function does.) 
\par 
\par \f1 puts\f0  is the most convenient function for printing simple messages. For example: 
\par 
\par \f1           puts ("This is a message.");
\par      
\par \f0 outputs the text `\f1 This is a message.\f0 ' followed by a newline.
\par }
2060
PAGE_558
Sleeping
sleep;nanosleep



Imported



FALSE
24
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Sleeping\{keepn\}
\par \pard\fs18 Previous: \cf1\strike Calendar Time\cf2\strike0\{linkID=230\}\cf0 , Up: \cf1\strike Date and Time\strike0\{linkID=520\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Sleeping\b0 
\par 
\par The function \f1 sleep\f0  gives a simple way to make the program wait for a short interval. If your program doesn't use signals (except to terminate), then you can expect \f1 sleep\f0  to wait reliably throughout the specified interval. Otherwise, \f1 sleep\f0  can return sooner if a signal arrives\f2 .
\par 
\par \f0 -- Function: unsigned int \b sleep\b0  (\i unsigned int seconds\i0 )
\par 
\par \pard\li500 The \f1 sleep\f0  function waits for \i seconds\i0  or until a signal is delivered, whichever happens first. 
\par 
\par If \f1 sleep\f0  function returns because the requested interval is over, it returns a value of zero. If it returns because of delivery of a signal, its return value is the remaining time in the sleep interval. 
\par 
\par The \f1 sleep\f0  function is declared in \f1 unistd.h\f0 .
\par 
\par \pard Resist the temptation to implement a sleep for a fixed amount of time by using the return value of \f1 sleep\f0 , when nonzero, to call\f1  sleep\f0  again. This will work with a certain amount of accuracy as long as signals arrive infrequently. But each signal can cause the eventual wakeup time to be off by an additional second or so. Suppose a few signals happen to arrive in rapid succession by bad luck--there is no limit on how much this could shorten or lengthen the wait. 
\par 
\par Instead, compute the calendar time at which the program should stop waiting, and keep trying to wait until that calendar time. This won't be off by more than a second. With just a little more work, you can use\f1  select\f0  and make the waiting period quite accurate. (Of course, heavy system load can cause additional unavoidable delays--unless the machine is dedicated to one application, there is no way you can avoid this.) 
\par }
2070
PAGE_569
Special Functions
specialfunctions;Besselfunctions;gammafunction;erf;erff;erfl;erfc;erfcf;erfcl;lgamma;lgammaf;lgammal;signgam;lgamma_r;lgammaf_r;lgammal_r;gamma;gammaf;gammal;tgamma;tgammaf;tgammal;j;j0f;j0l;j;j1f;j1l;jn;jnf;jnl;y;y0f;y0l;y;y1f;y1l;yn;ynf;ynl



Imported



FALSE
63
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Special Functions \{keepn\}
\par \pard\fs18 Next: \cf1\strike Pseudo-Random Numbers\cf2\strike0\{linkID=1780\}\cf0 , Previous: \cf1\strike Hyperbolic Functions\strike0\{linkID=1130\}\cf0 , Up: \cf1\strike Mathematics\strike0\{linkID=1410\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Special Functions\b0 
\par 
\par These are some more exotic mathematical functions which are sometimes useful. Currently they only have real-valued versions.
\par 
\par -- Function: double \b erf\b0  (\i double x\i0 )
\par 
\par \pard\li500 -- Function: float \b erff\b0  (\i float x\i0 )
\par 
\par \pard\li1000 -- Function: long double \b erfl\b0  (\i long double x\i0 )
\par 
\par \pard\li1500\f1 erf\f0  returns the error function of \i x\i0 . The error function is defined as 
\par 
\par \f1           erf (x) = 2/sqrt(pi) * integral from 0 to x of exp(-t^2) dt
\par      
\par \pard\f0 -- Function: double \b erfc\b0  (\i double x\i0 )
\par 
\par \pard\li500 -- Function: float \b erfcf\b0  (\i float x\i0 )
\par 
\par \pard\li1000 -- Function: long double \b erfcl\b0  (\i long double x\i0 )
\par 
\par \pard\li1500\f1 erfc\f0  returns \f1 1.0 - erf(\i\f0 x\i0\f1 )\f0 , but computed in a fashion that avoids round-off error when \i x\i0  is large.
\par 
\par \pard -- Function: double \b lgamma\b0  (\i double x\i0 )
\par 
\par \pard\li500 -- Function: float \b lgammaf\b0  (\i float x\i0 )
\par 
\par \pard\li1000 -- Function: long double \b lgammal\b0  (\i long double x\i0 )
\par 
\par \pard\li1500\f1 lgamma\f0  returns the natural logarithm of the absolute value of the gamma function of \i x\i0 . The gamma function is defined as 
\par 
\par \f1           gamma (x) = integral from 0 to &infin; of t^(x-1) e^-t dt
\par      
\par \f0 The sign of the gamma function is stored in the global variable\i  signgam\i0 , which is declared in \f1 math.h\f0 . It is \f1 1\f0  if the intermediate result was positive or zero, or \f1 -1\f0  if it was negative. 
\par 
\par To compute the real gamma function you can use the \f1 tgamma\f0  function or you can compute the values as follows: 
\par 
\par \f1           lgam = lgamma(x);
\par           gam  = signgam*exp(lgam);
\par      
\par \f0 The gamma function has singularities at the non-positive integers.\f1  lgamma\f0  will raise the zero divide exception if evaluated at a singularity.
\par 
\par \pard -- Function: double \b tgamma\b0  (\i double x\i0 )
\par 
\par \pard\li500 -- Function: float \b tgammaf\b0  (\i float x\i0 )
\par 
\par \pard\li1000 -- Function: long double \b tgammal\b0  (\i long double x\i0 )
\par 
\par \pard\li1500\f1 tgamma\f0  applies the gamma function to \i x\i0 . The gamma function is defined as 
\par 
\par \f1           gamma (x) = integral from 0 to &infin; of t^(x-1) e^-t dt
\par      
\par \f0 This function was introduced in ISO C99.
\par 
\par 
\par }
2080
PAGE_571
Standard Locales




Imported



FALSE
24
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Standard Locales \{keepn\}
\par \pard\fs18 Next: \cf1\strike Locale Information\strike0\{linkID=1330\}\cf0 , Previous: \cf1\strike Setting the Locale\strike0\{linkID=1960\}\cf0 , Up: \cf1\strike Locales\strike0\{linkID=1340\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Standard Locales\b0 
\par 
\par The only locale names you can count on finding on all operating systems are these three standard ones:
\par 
\par \f1 "C"\f0 
\par \pard\li500 This is the standard C locale. The attributes and behavior it provides are specified in the ISO C standard. When your program starts up, it initially uses this locale by default. 
\par \pard\f1 "POSIX"\f0 
\par \pard\li500 This is the standard POSIX locale. Currently, it is an alias for the standard C locale. 
\par \pard\f1 ""\f0 
\par \pard\li500 The empty name says to select a locale based on environment variables. See \cf1\strike Locale Categories\strike0\{linkID=1320\}\cf0 .
\par 
\par \pard Defining and installing named locales is normally a responsibility of the system administrator at your site (or the person who installed the C library). It is also possible for the user to create private locales.
\par 
\par If your program needs to use something other than the `\f1 C\f0 ' locale, it will be more portable if you use whatever locale the user specifies with the environment, rather than trying to specify some non-standard locale explicitly by name. Remember, different machines might have different sets of locales installed.
\par 
\par }
2090
PAGE_572
Standard Signals
signalnames;namesofsignals;signal.h;signalnumber;NSIG



Imported



FALSE
22
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Standard Signals \{keepn\}
\par \pard\fs18 Next: \cf1\strike Signal Actions\strike0\{linkID=1990\}\cf0 , Previous: \cf1\strike Concepts of Signals\strike0\{linkID=400\}\cf0 , Up: \cf1\strike Signal Handling\strike0\{linkID=2010\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Standard Signals\b0 
\par 
\par This section lists the names for various standard kinds of signals and describes what kind of event they mean. Each signal name is a macro which stands for a positive integer--the \i signal number\i0  for that kind of signal. Your programs should never make assumptions about the numeric code for a particular kind of signal, but rather refer to them always by the names defined here. This is because the number for a given kind of signal can vary from system to system, but the meanings of the names are standardized and fairly uniform. 
\par 
\par The signal names are defined in the header file \f1 signal.h\f0 .
\par 
\par -- Macro: int \b NSIG\b0 
\par 
\par \pard\li500 The value of this symbolic constant is the total number of signals defined. Since the signal numbers are allocated consecutively,\f1  NSIG\f0  is also one greater than the largest defined signal number.
\par 
\par \f2\'b7\f0  \cf1\strike Program Error Signals\strike0\{linkID=1760\}\cf0 : Used to report serious program errors.
\par \f2\'b7\f0  \cf1\strike Miscellaneous Signals\strike0\{linkID=1460\}\cf0 : Miscellaneous Signals.
\par \f2 
\par }
2100
PAGE_573
Standard Streams
standardstreams;streams,standard;stdio.h;stdin;standardinputstream;stdout;standardoutputstream;stderr;standarderrorstream



Imported



FALSE
38
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Standard Streams \{keepn\}
\par \pard\fs18 Next: \cf1\strike Opening Streams\strike0\{linkID=1610\}\cf0 , Previous: \cf1\strike Streams\strike0\{linkID=2150\}\cf0 , Up: \cf1\strike I/O on Streams\strike0\{linkID=1260\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Standard Streams\b0 
\par 
\par When the \f1 main\f0  function of your program is invoked, it already has three predefined streams open and available for use. These represent the "standard" input and output channels that have been established for the process. 
\par 
\par These streams are declared in the header file \f1 stdio.h\f0 .
\par 
\par -- Variable: FILE * \b stdin\b0 
\par 
\par \pard\li500 The \i standard input\i0  stream, which is the normal source of input for the program.
\par 
\par \pard -- Variable: FILE * \b stdout\b0 
\par 
\par \pard\li500 The \i standard output\i0  stream, which is used for normal output from the program.
\par 
\par \pard -- Variable: FILE * \b stderr\b0 
\par 
\par \pard\li500 The \i standard error\i0  stream, which is used for error messages and diagnostics issued by the program.
\par 
\par \pard In th\f2 is\f0  system, you can specify what files or processes correspond to these streams using the pipe and redirection facilities provided by the shell. (The primitives shells use to implement these facilities are described in \cf1\strike File System Interface\strike0\{linkID=820\}\cf0 .) Most other operating systems provide similar mechanisms, but the details of how to use them can vary. 
\par 
\par In the\f2  \f0 C library, \f1 stdin\f0 , \f1 stdout\f0 , and \f1 stderr\f0  are normal variables which you can set just like any others. For example, to redirect the standard output to a file, you could do:
\par 
\par \f1      fclose (stdout);
\par      stdout = fopen ("standard-output-file", "w");
\par \f0 
\par Note however, that in other systems \f1 stdin\f0 , \f1 stdout\f0 , and\f1  stderr\f0  are macros that you cannot assign to in the normal way. But you can use \f1 freopen\f0  to get the effect of closing one and reopening it. See \cf1\strike Opening Streams\strike0\{linkID=1610\}\cf0 . 
\par 
\par The three streams \f1 stdin\f0 , \f1 stdout\f0 , and \f1 stderr\f0  are not unoriented at program start (see \cf1\strike Streams and I18N\strike0\{linkID=2140\}\cf0 ).
\par 
\par }
2110
PAGE_577
Status bit operations
FE_INEXACT;FE_DIVBYZERO;FE_UNDERFLOW;FE_OVERFLOW;FE_INVALID;feclearexcept;feraiseexcept;fetestexcept;fegetexceptflag;fesetexceptflag



Imported



FALSE
77
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Status bit operations \{keepn\}
\par \pard\fs18 Next: \cf1\strike Math Error Reporting\strike0\{linkID=1390\}\cf0 , Previous: \cf1\strike Infinity and NaN\strike0\{linkID=1160\}\cf0 , Up: \cf1\strike Floating Point Errors\strike0\{linkID=860\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Examining the FPU status word\b0 
\par 
\par ISO C99 defines functions to query and manipulate the floating-point status word. You can use these functions to check for untrapped exceptions when it's convenient, rather than worrying about them in the middle of a calculation. 
\par 
\par These constants represent the various IEEE 754 exceptions. Not all FPUs report all the different exceptions. Each constant is defined if and only if the FPU you are compiling for supports that exception, so you can test for FPU support with `\f1 #ifdef\f0 '. They are defined in\f1  fenv.h\f0 .
\par 
\par \f1 FE_INEXACT\f0 
\par \pard\li500 The inexact exception.
\par \pard\f1 FE_DIVBYZERO\f0 
\par \pard\li500 The divide by zero exception.
\par \pard\f1 FE_UNDERFLOW\f0 
\par \pard\li500 The underflow exception.
\par \pard\f1 FE_OVERFLOW\f0 
\par \pard\li500 The overflow exception.
\par \pard\f1 FE_INVALID\f0 
\par \pard\li500 The invalid exception.
\par 
\par \pard The macro \f1 FE_ALL_EXCEPT\f0  is the bitwise OR of all exception macros which are supported by the FP implementation. 
\par 
\par These functions allow you to clear exception flags, test for exceptions, and save and restore the set of exceptions flagged.
\par -- Function: int \b feclearexcept\b0  (\i int excepts\i0 )
\par 
\par 
\par \pard\li500 This function clears all of the supported exception flags indicated by\i  excepts\i0 . 
\par 
\par The function returns zero in case the operation was successful, a non-zero value otherwise.
\par 
\par \pard -- Function: int \b feraiseexcept\b0  (\i int excepts\i0 )
\par 
\par \pard\li500 This function raises the supported exceptions indicated by\i  excepts\i0 . If more than one exception bit in \i excepts\i0  is set the order in which the exceptions are raised is undefined except that overflow (\f1 FE_OVERFLOW\f0 ) or underflow (\f1 FE_UNDERFLOW\f0 ) are raised before inexact (\f1 FE_INEXACT\f0 ). Whether for overflow or underflow the inexact exception is also raised is also implementation dependent. 
\par 
\par The function returns zero in case the operation was successful, a non-zero value otherwise.
\par 
\par \pard -- Function: int \b fetestexcept\b0  (\i int excepts\i0 )
\par 
\par \pard\li500 Test whether the exception flags indicated by the parameter \i except\i0  are currently set. If any of them are, a nonzero value is returned which specifies which exceptions are set. Otherwise the result is zero.
\par 
\par \pard To understand these functions, imagine that the status word is an integer variable named \i status\i0 . \f1 feclearexcept\f0  is then equivalent to `\f1 status &= ~excepts\f0 ' and \f1 fetestexcept\f0  is equivalent to `\f1 (status & excepts)\f0 '. The actual implementation may be very different, of course. 
\par 
\par Exception flags are only cleared when the program explicitly requests it, by calling \f1 feclearexcept\f0 . If you want to check for exceptions from a set of calculations, you should clear all the flags first. Here is a simple example of the way to use \f1 fetestexcept\f0 :
\par 
\par \f1      \{
\par        double f;
\par        int raised;
\par        feclearexcept (FE_ALL_EXCEPT);
\par        f = compute ();
\par        raised = fetestexcept (FE_OVERFLOW | FE_INVALID);
\par        if (raised & FE_OVERFLOW) \{ /* ... */ \}
\par        if (raised & FE_INVALID) \{ /* ... */ \}
\par        /* ... */
\par      \}
\par \f0 
\par You cannot explicitly set bits in the status word. You can, however, save the entire status word and restore it later. This is done with the following functions:
\par 
\par -- Function: int \b fegetexceptflag\b0  (\i fexcept_t *flagp, int excepts\i0 )
\par 
\par \pard\li500 This function stores in the variable pointed to by \i flagp\i0  an implementation-defined value representing the current setting of the exception flags indicated by \i excepts\i0 . 
\par 
\par The function returns zero in case the operation was successful, a non-zero value otherwise.
\par 
\par \pard -- Function: int \b fesetexceptflag\b0  (\i const fexcept_t *flagp, int\i0 
\par 
\par \pard\li500\i excepts\i0 ) This function restores the flags for the exceptions indicated by\i  excepts\i0  to the values stored in the variable pointed to by\i  flagp\i0 . 
\par 
\par The function returns zero in case the operation was successful, a non-zero value otherwise.
\par 
\par \pard Note that the value stored in \f1 fexcept_t\f0  bears no resemblance to the bit mask returned by \f1 fetestexcept\f0 . The type may not even be an integer. Do not attempt to modify an \f1 fexcept_t\f0  variable.
\par 
\par }
2120
PAGE_581
Stream Buffering
bufferingofstreams



Imported



FALSE
23
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Stream Buffering \{keepn\}
\par \pard\fs18 Previous: \cf1\strike Portable Positioning\strike0\{linkID=1730\}\cf0 , Up: \cf1\strike I/O on Streams\strike0\{linkID=1260\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Stream Buffering\b0 
\par 
\par Characters that are written to a stream are normally accumulated and transmitted asynchronously to the file in a block, instead of appearing as soon as they are output by the application program. Similarly, streams often retrieve input from the host environment in blocks rather than on a character-by-character basis. This is called \i buffering\i0 . 
\par 
\par If you are writing programs that do interactive input and output using streams, you need to understand how buffering works when you design the user interface to your program. Otherwise, you might find that output (such as progress or prompt messages) doesn't appear when you intended it to, or displays some other unexpected behavior. 
\par 
\par This section deals only with controlling when characters are transmitted between the stream and the file or device, and \i not\i0  with how things like echoing, flow control, and the like are handled on specific classes of devices. 
\par 
\par You can bypass the stream buffering facilities altogether by using the low-level input and output functions that operate on file descriptors instead. See \cf1\strike Low-Level I/O\strike0\{linkID=1360\}\cf0 .
\par 
\par \pard\li500\f1\'b7\f0  \cf1\strike Buffering Concepts\strike0\{linkID=220\}\cf0 : Terminology is defined here.
\par \f1\'b7\f0  \cf1\strike Flushing Buffers\strike0\{linkID=910\}\cf0 : How to ensure that output buffers are flushed.
\par \f1\'b7\f0  \cf1\strike Controlling Buffering\strike0\{linkID=430\}\cf0 : How to specify what kind of buffering to use.
\par 
\par }
2130
PAGE_583
Streams and File Descriptors




Imported



FALSE
23
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Streams and File Descriptors \{keepn\}
\par \pard\fs18 Next: \cf1\strike File Position\strike0\{linkID=780\}\cf0 , Up: \cf1\strike I/O Concepts\strike0\{linkID=1250\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Streams and File Descriptors\b0 
\par 
\par When you want to do input or output to a file, you have a choice of two basic mechanisms for representing the connection between your program and the file: file descriptors and streams. File descriptors are represented as objects of type \f1 int\f0 , while streams are represented as \f1 FILE *\f0  objects. 
\par 
\par File descriptors provide a primitive, low-level interface to input and output operations. Both file descriptors and streams can represent a connection to a device (such as a terminal), as well as a normal file. But, if you want to do control operations that are specific to a particular kind of device, you must use a file descriptor; there are no facilities to use streams in this way. You must also use file descriptors if your program needs to do input or output in special modes, such as nonblocking (or polled) input (see \cf1\strike File Status Flags\strike0\{linkID=810\}\cf0 ). 
\par 
\par Streams provide a higher-level interface\f2 .  \f0 he stream interface treats all kinds of files pretty much alike--the sole exception being the three styles of buffering that you can choose (see \cf1\strike Stream Buffering\strike0\{linkID=2120\}\cf0 ). 
\par 
\par The main advantage of using the stream interface is that the set of functions for performing actual input and output operations (as opposed to control operations) on streams is much richer and more powerful than the corresponding facilities for file descriptors. The file descriptor interface provides only simple functions for transferring blocks of characters, but the stream interface also provides powerful formatted input and output functions (\f1 printf\f0  and \f1 scanf\f0 ) as well as functions for character- and line-oriented input and output. 
\par 
\par In general, you should stick with using streams rather than file descriptors, unless there is some specific operation you want to do that can only be done on a file descriptor. If you are a beginning programmer and aren't sure what functions to use, we suggest that you concentrate on the formatted input functions (see \cf1\strike Formatted Input\strike0\{linkID=940\}\cf0 ) and formatted output functions (see \cf1\strike Formatted Output\strike0\{linkID=970\}\cf0 ). 
\par 
\par If you are concerned about portability of your programs to systems other than \f2 this\f0 , you should also be aware that file descriptors are not as portable as streams. You can expect any system running ISO C to support streams, but \f2 other\f0  systems may not support file descriptors at all, or may only implement a subset of the functions that operate on file descriptors. Most of the file descriptor functions in th\f2 is \f0 library are included in the POSIX.1 standard, however.
\par 
\par }
2140
PAGE_584
Streams and I18N
C++streams;streams,C++;orientation,stream;streamorientation;fwide



Imported



FALSE
57
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Streams and I18N \{keepn\}
\par \pard\fs18 Next: \cf1\strike Simple Output\strike0\{linkID=2050\}\cf0 , Previous: \cf1\strike Streams and Threads\strike0\{link=PAGE_585\}\cf0 , Up: \cf1\strike I/O on Streams\strike0\{linkID=1260\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Streams in Internationalized Applications\b0 
\par 
\par ISO C90 introduced the new type \f1 wchar_t\f0  to allow handling larger character sets. What was missing was a possibility to output strings of \f1 wchar_t\f0  directly. One had to convert them into multibyte strings using \f1 mbstowcs\f0  (there was no \f1 mbsrtowcs\f0  yet) and then use the normal stream functions. While this is doable it is very cumbersome since performing the conversions is not trivial and greatly increases program complexity and size. 
\par 
\par The Unix standard early on (I think in XPG4.2) introduced two additional format specifiers for the \f1 printf\f0  and \f1 scanf\f0  families of functions. Printing and reading of single wide characters was made possible using the \f1 %C\f0  specifier and wide character strings can be handled with \f1 %S\f0 . These modifiers behave just like \f1 %c\f0  and\f1  %s\f0  only that they expect the corresponding argument to have the wide character type and that the wide character and string are transformed into/from multibyte strings before being used. 
\par 
\par This was a beginning but it is still not good enough. Not always is it desirable to use \f1 printf\f0  and \f1 scanf\f0 . The other, smaller and faster functions cannot handle wide characters. Second, it is not possible to have a format string for \f1 printf\f0  and \f1 scanf\f0  consisting of wide characters. The result is that format strings would have to be generated if they have to contain non-basic characters. 
\par 
\par In the Amendment 1 to ISO C90 a whole new set of functions was added to solve the problem. Most of the stream functions got a counterpart which take a wide character or wide character string instead of a character or string respectively. The new functions operate on the same streams (like \f1 stdout\f0 ). This is different from the model of the C++ runtime library where separate streams for wide and normal I/O are used. 
\par 
\par Being able to use the same stream for wide and normal operations comes with a restriction: a stream can be used either for wide operations or for normal operations. Once it is decided there is no way back. Only a call to \f1 freopen\f0  or \f1 freopen64\f0  can reset the\i  orientation\i0 . The orientation can be decided in three ways:
\par 
\par \pard\li500\f2\'b7\f0  If any of the normal character functions is used (this includes the\f1  fread\f0  and \f1 fwrite\f0  functions) the stream is marked as not wide oriented. 
\par \f2\'b7\f0  If any of the wide character functions is used the stream is marked as wide oriented. 
\par \f2\'b7\f0  The \f1 fwide\f0  function can be used to set the orientation either way.
\par 
\par \pard It is important to never mix the use of wide and not wide operations on a stream. There are no diagnostics issued. The application behavior will simply be strange or the application will simply crash. The\f1  fwide\f0  function can help avoiding this.
\par 
\par -- Function: int \b fwide\b0  (\i FILE *stream, int mode\i0 )
\par 
\par \pard\li500 The \f1 fwide\f0  function can be used to set and query the state of the orientation of the stream \i stream\i0 . If the \i mode\i0  parameter has a positive value the streams get wide oriented, for negative values narrow oriented. It is not possible to overwrite previous orientations with \f1 fwide\f0 . I.e., if the stream \i stream\i0  was already oriented before the call nothing is done. 
\par 
\par If \i mode\i0  is zero the current orientation state is queried and nothing is changed. 
\par 
\par The \f1 fwide\f0  function returns a negative value, zero, or a positive value if the stream is narrow, not at all, or wide oriented respectively. 
\par 
\par This function was introduced in Amendment 1 to ISO C90 and is declared in \f1 wchar.h\f0 .
\par 
\par \pard It is generally a good idea to orient a stream as early as possible. This can prevent surprise especially for the standard streams\f1  stdin\f0 , \f1 stdout\f0 , and \f1 stderr\f0 . If some library function in some situations uses one of these streams and this use orients the stream in a different way the rest of the application expects it one might end up with hard to reproduce errors. Remember that no errors are signal if the streams are used incorrectly. Leaving a stream unoriented after creation is normally only necessary for library functions which create streams which can be used in different contexts. 
\par 
\par When writing code which uses streams and which can be used in different contexts it is important to query the orientation of the stream before using it (unless the rules of the library interface demand a specific orientation). The following little, silly function illustrates this.
\par 
\par \f1      void
\par      print_f (FILE *fp)
\par      \{
\par        if (fwide (fp, 0) > 0)
\par          /* Positive return value means wide orientation.  */
\par          fputwc (L'f', fp);
\par        else
\par          fputc ('f', fp);
\par      \}
\par \f0 
\par Note that in this case the function \f1 print_f\f0  decides about the orientation of the stream if it was unoriented before (will not happen if the advise above is followed). 
\par 
\par The encoding used for the \f1 wchar_t\f0  values is unspecified and the user must not make any assumptions about it. For I/O of \f1 wchar_t\f0  values this means that it is impossible to write these values directly to the stream. This is not what follows from the ISO C locale model either. What happens instead is that the bytes read from or written to the underlying media are first converted into the internal encoding chosen by the implementation for \f1 wchar_t\f0 . The external encoding is determined by the \f1 LC_CTYPE\f0  category of the current locale or by the `\f1 ccs\f0 ' part of the mode specification given to \f1 fopen\f0 ,\f1  fopen64\f0 , \f1 freopen\f0 , or \f1 freopen64\f0 . How and when the conversion happens is unspecified and it happens invisible to the user. 
\par 
\par Since a stream is created in the unoriented state it has at that point no conversion associated with it. The conversion which will be used is determined by the \f1 LC_CTYPE\f0  category selected at the time the stream is oriented. If the locales are changed at the runtime this might produce surprising results unless one pays attention. This is just another good reason to orient the stream explicitly as soon as possible, perhaps with a call to \f1 fwide\f0 .
\par 
\par }
2150
PAGE_586
Streams
filepointer;stdio.h;FILE



Imported



FALSE
19
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Streams\{keepn\}
\par \pard\fs18 Next: \cf1\strike Standard Streams\strike0\{linkID=2100\}\cf0 , Up: \cf1\strike I/O on Streams\strike0\{linkID=1260\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Streams\b0 
\par 
\par For historical reasons, the type of the C data structure that represents a stream is called \f1 FILE\f0  rather than "stream". Since most of the library functions deal with objects of type \f1 FILE *\f0 , sometimes the term \i file pointer\i0  is also used to mean "stream". This leads to unfortunate confusion over terminology in many books on C. This manual, however, is careful to use the terms "file" and "stream" only in the technical sense. The \f1 FILE\f0  type is declared in the header file \f1 stdio.h\f0 .
\par 
\par -- Data Type: \b FILE\b0 
\par 
\par \pard\li500 This is the data type used to represent stream objects. A \f1 FILE\f0  object holds all of the internal state information about the connection to the associated file, including such things as the file position indicator and buffering information. Each stream also has error and end-of-file status indicators that can be tested with the \f1 ferror\f0  and \f1 feof\f0  functions; see \cf1\strike EOF and Errors\strike0\{linkID=650\}\cf0 .
\par 
\par \pard\f1 FILE\f0  objects are allocated and managed internally by the input/output library functions. Don't try to create your own objects of type \f1 FILE\f0 ; let the library do it. Your programs should deal only with pointers to these objects (that is, \f1 FILE *\f0  values) rather than the objects themselves.
\par 
\par }
2160
PAGE_589
String and Array Utilities




Imported



FALSE
25
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 String and Array Utilities \{keepn\}
\par \pard\fs18 Next: \cf1\strike Character Set Handling\strike0\{linkID=290\}\cf0 , Previous: \cf1\strike Character Handling\strike0\{linkID=270\}\cf0 , Up: \cf1\strike Top\strike0\{linkID=10\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b String and Array Utilities\b0 
\par 
\par Operations on strings (or arrays of characters) are an important part of many programs. The C library provides an extensive set of string utility functions, including functions for copying, concatenating, comparing, and searching strings. Many of these functions can also operate on arbitrary regions of storage; for example, the \f1 memcpy\f0  function can be used to copy the contents of any kind of array. 
\par 
\par It's fairly common for beginning C programmers to "reinvent the wheel" by duplicating this functionality in their own code, but it pays to become familiar with the library functions and to make use of them, since this offers benefits in maintenance, efficiency, and portability. 
\par 
\par For instance, you could easily compare one string to another in two lines of C code, but if you use the built-in \f1 strcmp\f0  function, you're less likely to make a mistake. And, since these library functions are typically highly optimized, your program may run faster too.
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Representation of Strings\strike0\{linkID=1850\}\cf0 : Introduction to basic concepts.
\par \f2\'b7\f0  \cf1\strike String/Array Conventions\strike0\{linkID=2200\}\cf0 : Whether to use a string function or an arbitrary array function.
\par \f2\'b7\f0  \cf1\strike String Length\strike0\{linkID=2180\}\cf0 : Determining the length of a string.
\par \f2\'b7\f0  \cf1\strike Copying and Concatenation\strike0\{linkID=460\}\cf0 : Functions to copy the contents of strings and arrays.
\par \f2\'b7\f0  \cf1\strike String/Array Comparison\strike0\{linkID=2190\}\cf0 : Functions for byte-wise and character-wise comparison.
\par \f2\'b7\f0  \cf1\strike Collation Functions\strike0\{linkID=370\}\cf0 : Functions for collating strings.
\par \f2\'b7\f0  \cf1\strike Search Functions\strike0\{linkID=1920\}\cf0 : Searching for a specific element or substring.
\par \f2\'b7\f0  \cf1\strike Finding Tokens in a String\strike0\{linkID=830\}\cf0 : Splitting a string into tokens by looking for delimiters.\cf1\strike 
\par }
2170
PAGE_590
String Input Conversions




Imported



FALSE
60
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 + String Input Conversions \{keepn\}
\par \pard\fs18 Next: \cf1\strike Dynamic String Input\strike0\{link=PAGE_159\}\cf0 , Previous: \cf1\strike Numeric Input Conversions\strike0\{linkID=1590\}\cf0 , Up: \cf1\strike Formatted Input\strike0\{linkID=940\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b String Input Conversions\b0 
\par 
\par This section describes the \f1 scanf\f0  input conversions for reading string and character values: `\f1 %s\f0 ', `\f1 %S\f0 ', `\f1 %[\f0 ', `\f1 %c\f0 ', and `\f1 %C\f0 '. 
\par 
\par You have two options for how to receive the input from these conversions:
\par 
\par \pard\li500\f2\'b7\f0  Provide a buffer to store it in. This is the default. You should provide an argument of type \f1 char *\f0  or \f1 wchar_t *\f0  (the latter of the `\f1 l\f0 ' modifier is present). 
\par 
\par \b Warning:\b0  To make a robust program, you must make sure that the input (plus its terminating null) cannot possibly exceed the size of the buffer you provide. In general, the only way to do this is to specify a maximum field width one less than the buffer size. \b If you provide the buffer, always specify a maximum field width to prevent overflow.\b0  
\par \f2\'b7\f0  Ask \f1 scanf\f0  to allocate a big enough buffer, by specifying the `\f1 a\f0 ' flag character. This is a GNU extension. You should provide an argument of type \f1 char **\f0  for the buffer address to be stored in. See \cf1\strike Dynamic String Input\strike0\{link=PAGE_159\}\cf0 .
\par 
\par \pard The `\f1 %c\f0 ' conversion is the simplest: it matches a fixed number of characters, always. The maximum field width says how many characters to read; if you don't specify the maximum, the default is 1. This conversion doesn't append a null character to the end of the text it reads. It also does not skip over initial whitespace characters. It reads precisely the next \i n\i0  characters, and fails if it cannot get that many. Since there is always a maximum field width with `\f1 %c\f0 ' (whether specified, or 1 by default), you can always prevent overflow by making the buffer long enough. 
\par 
\par If the format is `\f1 %lc\f0 ' or `\f1 %C\f0 ' the function stores wide characters which are converted using the conversion determined at the time the stream was opened from the external byte stream. The number of bytes read from the medium is limited by \f1 MB_CUR_LEN * \i\f0 n\i0  but at most \i n\i0  wide character get stored in the output string. 
\par 
\par The `\f1 %s\f0 ' conversion matches a string of non-whitespace characters. It skips and discards initial whitespace, but stops when it encounters more whitespace after having read something. It stores a null character at the end of the text that it reads. 
\par 
\par For example, reading the input:
\par 
\par \f1       hello, world
\par \f0 
\par with the conversion `\f1 %10c\f0 ' produces \f1 " hello, wo"\f0 , but reading the same input with the conversion `\f1 %10s\f0 ' produces\f1  "hello,"\f0 . 
\par 
\par \b Warning:\b0  If you do not specify a field width for `\f1 %s\f0 ', then the number of characters read is limited only by where the next whitespace character appears. This almost certainly means that invalid input can make your program crash--which is a bug. 
\par 
\par The `\f1 %ls\f0 ' and `\f1 %S\f0 ' format are handled just like `\f1 %s\f0 ' except that the external byte sequence is converted using the conversion associated with the stream to wide characters with their own encoding. A width or precision specified with the format do not directly determine how many bytes are read from the stream since they measure wide characters. But an upper limit can be computed by multiplying the value of the width or precision by \f1 MB_CUR_MAX\f0 . 
\par 
\par To read in characters that belong to an arbitrary set of your choice, use the `\f1 %[\f0 ' conversion. You specify the set between the `\f1 [\f0 ' character and a following `\f1 ]\f0 ' character, using the same syntax used in regular expressions. As special cases:
\par 
\par \pard\li500\f2\'b7\f0  A literal `\f1 ]\f0 ' character can be specified as the first character of the set. 
\par \f2\'b7\f0  An embedded `\f1 -\f0 ' character (that is, one that is not the first or last character of the set) is used to specify a range of characters. 
\par \f2\'b7\f0  If a caret character `\f1 ^\f0 ' immediately follows the initial `\f1 [\f0 ', then the set of allowed input characters is the everything \i except\i0  the characters listed.
\par 
\par \pard The `\f1 %[\f0 ' conversion does not skip over initial whitespace characters. 
\par 
\par Here are some examples of `\f1 %[\f0 ' conversions and what they mean:
\par 
\par `\f1 %25[1234567890]\f0 '
\par \pard\li500 Matches a string of up to 25 digits. 
\par \pard `\f1 %25[][]\f0 '
\par \pard\li500 Matches a string of up to 25 square brackets. 
\par \pard `\f1 %25[^ \\f\\n\\r\\t\{link=]\}'
\par \pard\li500 Matches a string up to 25 characters long that doesn't contain any of the standard whitespace characters. This is slightly different from `%s', because if the input begins with a whitespace character, `%[' reports a matching failure while `%s' simply discards the initial whitespace. 
\par \pard `%25[a-z]'
\par \pard\li500 Matches up to 25 lowercase characters.
\par 
\par \pard As for `%c' and `%s' the `%[' format is also modified to produce wide characters if the `l' modifier is present. All what is said about `%ls' above is true for `%l['. 
\par 
\par One more reminder: the `%s' and `%[' conversions are\b  dangerous\b0  if you don't specify a maximum width or use the `a' flag, because input too long would overflow whatever buffer you have provided for it. No matter how long your buffer is, a user could supply input that is longer. A well-written program reports invalid input with a comprehensible error message, not with a crash.
\par 
\par \f0 
\par }
2180
PAGE_591
String Length
string.h;strlen;wcslen;strnlen;wcsnlen



Imported



FALSE
65
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 String Length \{keepn\}
\par \pard\fs18 Next: \cf1\strike Copying and Concatenation\strike0\{linkID=460\}\cf0 , Previous: \cf1\strike String/Array Conventions\strike0\{linkID=2200\}\cf0 , Up: \cf1\strike String and Array Utilities\strike0\{linkID=2160\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b String Length\b0 
\par 
\par You can get the length of a string using the \f1 strlen\f0  function. This function is declared in the header file \f1 string.h\f0 .
\par 
\par -- Function: size_t \b strlen\b0  (\i const char *s\i0 )
\par 
\par \pard\li500 The \f1 strlen\f0  function returns the length of the null-terminated string \i s\i0  in bytes. (In other words, it returns the offset of the terminating null character within the array.) 
\par 
\par For example, 
\par 
\par \f1           strlen ("hello, world")
\par               => 12
\par      
\par \f0 When applied to a character array, the \f1 strlen\f0  function returns the length of the string stored there, not its allocated size. You can get the allocated size of the character array that holds a string using the \f1 sizeof\f0  operator: 
\par 
\par \f1           char string[32] = "hello, world";
\par           sizeof (string)
\par               => 32
\par           strlen (string)
\par               => 12
\par      
\par \f0 But beware, this will not work unless \i string\i0  is the character array itself, not a pointer to it. For example: 
\par 
\par \f1           char string[32] = "hello, world";
\par           char *ptr = string;
\par           sizeof (string)
\par               => 32
\par           sizeof (ptr)
\par               => 4  /* (on a machine with 4 byte pointers) */
\par      
\par \f0 This is an easy mistake to make when you are working with functions that take string arguments; those arguments are always pointers, not arrays. 
\par 
\par It must also be noted that for multibyte encoded strings the return value does not have to correspond to the number of characters in the string. To get this value the string can be converted to wide characters and \f1 wcslen\f0  can be used or something like the following code can be used: 
\par 
\par \f1           /* The input is in string.
\par              The length is expected in n.  */
\par           \{
\par             mbstate_t t;
\par             char *scopy = string;
\par             /* In initial state.  */
\par             memset (&t, '\\0', sizeof (t));
\par             /* Determine number of characters.  */
\par             n = mbsrtowcs (NULL, &scopy, strlen (scopy), &t);
\par           \}
\par      
\par \f0 This is cumbersome to do so if the number of characters (as opposed to bytes) is needed often it is better to work with wide characters.
\par 
\par \pard The wide character equivalent is declared in \f1 wchar.h\f0 .
\par 
\par -- Function: size_t \b wcslen\b0  (\i const wchar_t *ws\i0 )
\par 
\par \pard\li500 The \f1 wcslen\f0  function is the wide character equivalent to\f1  strlen\f0 . The return value is the number of wide characters in the wide character string pointed to by \i ws\i0  (this is also the offset of the terminating null wide character of \i ws\i0 ). 
\par 
\par Since there are no multi wide character sequences making up one character the return value is not only the offset in the array, it is also the number of wide characters. 
\par 
\par This function was introduced in Amendment 1 to ISO C90.
\par }
2190
PAGE_594
String/Array Comparison
comparingstringsandarrays;stringcomparisonfunctions;arraycomparisonfunctions;predicatesonstrings;predicatesonarrays;string.h;memcmp;wmemcmp;strcmp;wcscmp;strcasecmp;wcscasecmp;strncmp;wcsncmp;strncasecmp;wcsncasecmp;strverscmp;bcmp



Imported



FALSE
119
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 String/Array Comparison \{keepn\}
\par \pard\fs18 Next: \cf1\strike Collation Functions\strike0\{linkID=370\}\cf0 , Previous: \cf1\strike Copying and Concatenation\strike0\{linkID=460\}\cf0 , Up: \cf1\strike String and Array Utilities\strike0\{linkID=2160\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b String/Array Comparison\b0 
\par 
\par You can use the functions in this section to perform comparisons on the contents of strings and arrays. As well as checking for equality, these functions can also be used as the ordering functions for sorting operations. See \cf1\strike Searching and Sorting\strike0\{linkID=1930\}\cf0 , for an example of this. 
\par 
\par Unlike most comparison operations in C, the string comparison functions return a nonzero value if the strings are \i not\i0  equivalent rather than if they are. The sign of the value indicates the relative ordering of the first characters in the strings that are not equivalent: a negative value indicates that the first string is "less" than the second, while a positive value indicates that the first string is "greater". 
\par 
\par The most common use of these functions is to check only for equality. This is canonically done with an expression like `\f1 ! strcmp (s1, s2)\f0 '. 
\par 
\par All of these functions are declared in the header file \f1 string.h\f0 .
\par 
\par -- Function: int \b memcmp\b0  (\i const void *a1, const void *a2, size_t size\i0 )
\par 
\par \pard\li500 The function \f1 memcmp\f0  compares the \i size\i0  bytes of memory beginning at \i a1\i0  against the \i size\i0  bytes of memory beginning at \i a2\i0 . The value returned has the same sign as the difference between the first differing pair of bytes (interpreted as \f1 unsigned char\f0  objects, then promoted to \f1 int\f0 ). 
\par 
\par If the contents of the two blocks are equal, \f1 memcmp\f0  returns\f1  0\f0 .
\par 
\par \pard -- Function: int \b wmemcmp\b0  (\i const wchar_t *a1, const wchar_t *a2, size_t size\i0 )
\par 
\par \pard\li500 The function \f1 wmemcmp\f0  compares the \i size\i0  wide characters beginning at \i a1\i0  against the \i size\i0  wide characters beginning at \i a2\i0 . The value returned is smaller than or larger than zero depending on whether the first differing wide character is \i a1\i0  is smaller or larger than the corresponding character in \i a2\i0 . 
\par 
\par If the contents of the two blocks are equal, \f1 wmemcmp\f0  returns\f1  0\f0 .
\par 
\par \pard On arbitrary arrays, the \f1 memcmp\f0  function is mostly useful for testing equality. It usually isn't meaningful to do byte-wise ordering comparisons on arrays of things other than bytes. For example, a byte-wise comparison on the bytes that make up floating-point numbers isn't likely to tell you anything about the relationship between the values of the floating-point numbers. 
\par 
\par \f1 wmemcmp\f0  is really only useful to compare arrays of type\f1  wchar_t\f0  since the function looks at \f1 sizeof (wchar_t)\f0  bytes at a time and this number of bytes is system dependent. 
\par 
\par You should also be careful about using \f1 memcmp\f0  to compare objects that can contain "holes", such as the padding inserted into structure objects to enforce alignment requirements, extra space at the end of unions, and extra characters at the ends of strings whose length is less than their allocated size. The contents of these "holes" are indeterminate and may cause strange behavior when performing byte-wise comparisons. For more predictable results, perform an explicit component-wise comparison. 
\par 
\par For example, given a structure type definition like:
\par 
\par \f1      struct foo
\par        \{
\par          unsigned char tag;
\par          union
\par            \{
\par              double f;
\par              long i;
\par              char *p;
\par            \} value;
\par        \};
\par \f0 
\par you are better off writing a specialized comparison function to compare\f1  struct foo\f0  objects instead of comparing them with \f1 memcmp\f0 .
\par 
\par -- Function: int \b strcmp\b0  (\i const char *s1, const char *s2\i0 )
\par 
\par \pard\li500 The \f1 strcmp\f0  function compares the string \i s1\i0  against\i  s2\i0 , returning a value that has the same sign as the difference between the first differing pair of characters (interpreted as\f1  unsigned char\f0  objects, then promoted to \f1 int\f0 ). 
\par 
\par If the two strings are equal, \f1 strcmp\f0  returns \f1 0\f0 . 
\par 
\par A consequence of the ordering used by \f1 strcmp\f0  is that if \i s1\i0  is an initial substring of \i s2\i0 , then \i s1\i0  is considered to be "less than" \i s2\i0 . 
\par 
\par \f1 strcmp\f0  does not take sorting conventions of the language the strings are written in into account. To get that one has to use\f1  strcoll\f0 .
\par 
\par \pard -- Function: int \b wcscmp\b0  (\i const wchar_t *ws1, const wchar_t *ws2\i0 )
\par 
\par \pard\li500 The \f1 wcscmp\f0  function compares the wide character string \i ws1\i0  against \i ws2\i0 . The value returned is smaller than or larger than zero depending on whether the first differing wide character is \i ws1\i0  is smaller or larger than the corresponding character in \i ws2\i0 . 
\par 
\par If the two strings are equal, \f1 wcscmp\f0  returns \f1 0\f0 . 
\par 
\par A consequence of the ordering used by \f1 wcscmp\f0  is that if \i ws1\i0  is an initial substring of \i ws2\i0 , then \i ws1\i0  is considered to be "less than" \i ws2\i0 . 
\par 
\par \f1 wcscmp\f0  does not take sorting conventions of the language the strings are written in into account. To get that one has to use\f1  wcscoll\f0 .
\par 
\par \pard -- Function: int \b strcasecmp\b0  (\i const char *s1, const char *s2\i0 )
\par 
\par \pard\li500 This function is like \f1 strcmp\f0 , except that differences in case are ignored. How uppercase and lowercase characters are related is determined by the currently selected locale. In the standard \f1 "C"\f2  locale the characters \'c4 and \'e4 do not match but in a locale which regards these characters as parts of the alphabet they do match. \f0 
\par 
\par \f1 strcasecmp\f0  is derived from BSD.
\par 
\par \pard -- Function: int \b wcscasecmp\b0  (\i const wchar_t *ws1, const wchar_T *ws2\i0 )
\par 
\par \pard\li500 This function is like \f1 wcscmp\f0 , except that differences in case are ignored. How uppercase and lowercase characters are related is determined by the currently selected locale. In the standard \f1 "C"\f2  locale the characters \'c4 and \'e4 do not match but in a locale which regards these characters as parts of the alphabet they do match. \f0 
\par 
\par \f1 wcscasecmp\f0  is a GNU extension.
\par 
\par \pard -- Function: int \b strncmp\b0  (\i const char *s1, const char *s2, size_t size\i0 )
\par 
\par \pard\li500 This function is the similar to \f1 strcmp\f0 , except that no more than\i  size\i0  wide characters are compared. In other words, if the two strings are the same in their first \i size\i0  wide characters, the return value is zero.
\par 
\par \pard -- Function: int \b wcsncmp\b0  (\i const wchar_t *ws1, const wchar_t *ws2, size_t size\i0 )
\par 
\par \pard\li500 This function is the similar to \f1 wcscmp\f0 , except that no more than\i  size\i0  wide characters are compared. In other words, if the two strings are the same in their first \i size\i0  wide characters, the return value is zero.
\par 
\par \pard -- Function: int \b strn\f2 i\f0 cmp\b0  (\i const char *s1, const char *s2, size_t n\i0 )
\par 
\par \pard\li500 This function is like \f1 strncmp\f0 , except that differences in case are ignored. Like \f1 strcasecmp\f0 , it is locale dependent how uppercase and lowercase characters are related. 
\par 
\par \f1 strncasecmp\f0  is a\f2 n\f0  extension.
\par 
\par \pard -- Function: int \b wcsn\f2 i\f0 cmp\b0  (\i const wchar_t *ws1, const wchar_t *s2, size_t n\i0 )
\par 
\par \pard\li500 This function is like \f1 wcsncmp\f0 , except that differences in case are ignored. Like \f1 wcscasecmp\f0 , it is locale dependent how uppercase and lowercase characters are related. 
\par 
\par \f1 wcsncasecmp\f0  is a\f2 n\f0  extension.
\par 
\par \pard Here are some examples showing the use of \f1 strcmp\f0  and\f1  strncmp\f0  (equivalent examples can be constructed for the wide character functions). These examples assume the use of the ASCII character set. (If some other character set--say, EBCDIC--is used instead, then the glyphs are associated with different numeric codes, and the return values and ordering may differ.)
\par 
\par \f1      strcmp ("hello", "hello")
\par          => 0    /* These two strings are the same. */
\par      strcmp ("hello", "Hello")
\par          => 32   /* Comparisons are case-sensitive. */
\par      strcmp ("hello", "world")
\par          => -15  /* The character 'h' comes before 'w'. */
\par      strcmp ("hello", "hello, world")
\par          => -44  /* Comparing a null character against a comma. */
\par      strncmp ("hello", "hello, world", 5)
\par          => 0    /* The initial 5 characters are the same. */
\par      strncmp ("hello, world", "hello, stupid world!!!", 5)
\par          => 0    /* The initial 5 characters are the same. */
\par \f0 
\par }
2200
PAGE_595
String/Array Conventions
wint_t;parameterpromotion



Imported



FALSE
21
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 String/Array Conventions \{keepn\}
\par \pard\fs18 Next: \cf1\strike String Length\strike0\{linkID=2180\}\cf0 , Previous: \cf1\strike Representation of Strings\strike0\{linkID=1850\}\cf0 , Up: \cf1\strike String and Array Utilities\strike0\{linkID=2160\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b String and Array Conventions\b0 
\par 
\par This chapter describes both functions that work on arbitrary arrays or blocks of memory, and functions that are specific to null-terminated arrays of characters and wide characters. 
\par 
\par Functions that operate on arbitrary blocks of memory have names beginning with `\f1 mem\f0 ' and `\f1 wmem\f0 ' (such as \f1 memcpy\f0  and\f1  wmemcpy\f0 ) and invariably take an argument which specifies the size (in bytes and wide characters respectively) of the block of memory to operate on. The array arguments and return values for these functions have type \f1 void *\f0  or \f1 wchar_t\f0 . As a matter of style, the elements of the arrays used with the `\f1 mem\f0 ' functions are referred to as "bytes". You can pass any kind of pointer to these functions, and the \f1 sizeof\f0  operator is useful in computing the value for the size argument. Parameters to the `\f1 wmem\f0 ' functions must be of type\f1  wchar_t *\f0 . These functions are not really usable with anything but arrays of this type. 
\par 
\par In contrast, functions that operate specifically on strings and wide character strings have names beginning with `\f1 str\f0 ' and `\f1 wcs\f0 ' respectively (such as \f1 strcpy\f0  and \f1 wcscpy\f0 ) and look for a null character to terminate the string instead of requiring an explicit size argument to be passed. (Some of these functions accept a specified maximum length, but they also check for premature termination with a null character.) The array arguments and return values for these functions have type \f1 char *\f0  and \f1 wchar_t *\f0  respectively, and the array elements are referred to as "characters" and "wide characters". 
\par 
\par In many cases, there are both `\f1 mem\f0 ' and `\f1 str\f0 '/`\f1 wcs\f0 ' versions of a function. The one that is more appropriate to use depends on the exact situation. When your program is manipulating arbitrary arrays or blocks of storage, then you should always use the `\f1 mem\f0 ' functions. On the other hand, when you are manipulating null-terminated strings it is usually more convenient to use the `\f1 str\f0 '/`\f1 wcs\f0 ' functions, unless you already know the length of the string in advance. The `\f1 wmem\f0 ' functions should be used for wide character arrays with known size. 
\par 
\par Some of the memory and string functions take single characters as arguments. Since a value of type \f1 char\f0  is automatically promoted into an value of type \f1 int\f0  when used as a parameter, the functions are declared with \f1 int\f0  as the type of the parameter in question. In case of the wide character function the situation is similarly: the parameter type for a single wide character is \f1 wint_t\f0  and not\f1  wchar_t\f0 . This would for many implementations not be necessary since the \f1 wchar_t\f0  is large enough to not be automatically promoted, but since the ISO C standard does not require such a choice of types the \f1 wint_t\f0  type is used.
\par 
\par }
2210
PAGE_596
Structure Measurement
offsetof



Imported



FALSE
19
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Structure Measurement \{keepn\}
\par \pard\fs18 Previous: \cf1\strike Floating Type Macros\strike0\{linkID=890\}\cf0 , Up: \cf1\strike Data Type Measurements\strike0\{linkID=510\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Structure Field Offset Measurement\b0 
\par 
\par You can use \f1 offsetof\f0  to measure the location within a structure type of a particular structure member.
\par 
\par -- Macro: size_t \b offsetof\b0  (\i type, member\i0 )
\par 
\par \pard\li500 This expands to a integer constant expression that is the offset of the structure member named \i member\i0  in a the structure type \i type\i0 . For example, \f1 offsetof (struct s, elem)\f0  is the offset, in bytes, of the member \f1 elem\f0  in a \f1 struct s\f0 . 
\par 
\par This macro won't work if \i member\i0  is a bit field; you get an error from the C compiler in that case.
\par 
\par }
2230
PAGE_621
Table of Input Conversions
inputconversions,forscanf



Imported



FALSE
46
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Table of Input Conversions \{keepn\}
\par \pard\fs18 Next: \cf1\strike Numeric Input Conversions\strike0\{linkID=1590\}\cf0 , Previous: \cf1\strike Input Conversion Syntax\strike0\{linkID=1170\}\cf0 , Up: \cf1\strike Formatted Input\strike0\{linkID=940\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Table of Input Conversions\b0 
\par 
\par Here is a table that summarizes the various conversion specifications:
\par 
\par `\f1 %d\f0 '
\par \pard\li500 Matches an optionally signed integer written in decimal. See \cf1\strike Numeric Input Conversions\strike0\{linkID=1590\}\cf0 . 
\par \pard `\f1 %i\f0 '
\par \pard\li500 Matches an optionally signed integer in any of the formats that the C language defines for specifying an integer constant. See \cf1\strike Numeric Input Conversions\strike0\{linkID=1590\}\cf0 . 
\par \pard `\f1 %o\f0 '
\par \pard\li500 Matches an unsigned integer written in octal radix. See \cf1\strike Numeric Input Conversions\strike0\{linkID=1590\}\cf0 . 
\par \pard `\f1 %u\f0 '
\par \pard\li500 Matches an unsigned integer written in decimal radix. See \cf1\strike Numeric Input Conversions\strike0\{linkID=1590\}\cf0 . 
\par \pard `\f1 %x\f0 ', `\f1 %X\f0 '
\par \pard\li500 Matches an unsigned integer written in hexadecimal radix. See \cf1\strike Numeric Input Conversions\strike0\{linkID=1590\}\cf0 . 
\par \pard `\f1 %e\f0 ', `\f1 %f\f0 ', `\f1 %g\f0 ', `\f1 %E\f0 ', `\f1 %G\f0 '
\par \pard\li500 Matches an optionally signed floating-point number. See \cf1\strike Numeric Input Conversions\strike0\{linkID=1590\}\cf0 . 
\par \pard `\f1 %s\f0 '
\par \pard\li500 Matches a string containing only non-whitespace characters. See \cf1\strike String Input Conversions\strike0\{linkID=2170\}\cf0 . The presence of the `\f1 l\f0 ' modifier determines whether the output is stored as a wide character string or a multibyte string. If `\f1 %s\f0 ' is used in a wide character function the string is converted as with multiple calls to \f1 wcrtomb\f0  into a multibyte string. This means that the buffer must provide room for\f1  MB_CUR_MAX\f0  bytes for each wide character read. In case `\f1 %ls\f0 ' is used in a multibyte function the result is converted into wide characters as with multiple calls of \f1 mbrtowc\f0  before being stored in the user provided buffer. 
\par \pard `\f1 %S\f0 '
\par \pard\li500 This is an alias for `\f1 %ls\f0 ' which is supported for compatibility with the Unix standard. 
\par \pard `\f1 %[\f0 '
\par \pard\li500 Matches a string of characters that belong to a specified set. See \cf1\strike String Input Conversions\strike0\{linkID=2170\}\cf0 . The presence of the `\f1 l\f0 ' modifier determines whether the output is stored as a wide character string or a multibyte string. If `\f1 %[\f0 ' is used in a wide character function the string is converted as with multiple calls to \f1 wcrtomb\f0  into a multibyte string. This means that the buffer must provide room for\f1  MB_CUR_MAX\f0  bytes for each wide character read. In case `\f1 %l[\f0 ' is used in a multibyte function the result is converted into wide characters as with multiple calls of \f1 mbrtowc\f0  before being stored in the user provided buffer. 
\par \pard `\f1 %c\f0 '
\par \pard\li500 Matches a string of one or more characters; the number of characters read is controlled by the maximum field width given for the conversion. See \cf1\strike String Input Conversions\strike0\{linkID=2170\}\cf0 . 
\par 
\par If the `\f1 %c\f0 ' is used in a wide stream function the read value is converted from a wide character to the corresponding multibyte character before storing it. Note that this conversion can produce more than one byte of output and therefore the provided buffer be large enough for up to \f1 MB_CUR_MAX\f0  bytes for each character. If `\f1 %lc\f0 ' is used in a multibyte function the input is treated as a multibyte sequence (and not bytes) and the result is converted as with calls to \f1 mbrtowc\f0 . 
\par \pard `\f1 %C\f0 '
\par \pard\li500 This is an alias for `\f1 %lc\f0 ' which is supported for compatibility with the Unix standard. 
\par \pard `\f1 %p\f0 '
\par \pard\li500 Matches a pointer value in the same implementation-defined format used by the `\f1 %p\f0 ' output conversion for \f1 printf\f0 . See \cf1\strike Other Input Conversions\strike0\{linkID=1660\}\cf0 . 
\par \pard `\f1 %n\f0 '
\par \pard\li500 This conversion doesn't read any characters; it records the number of characters read so far by this call. See \cf1\strike Other Input Conversions\strike0\{linkID=1660\}\cf0 . 
\par \pard `\f1 %%\f0 '
\par \pard\li500 This matches a literal `\f1 %\f0 ' character in the input stream. No corresponding argument is used. See \cf1\strike Other Input Conversions\strike0\{linkID=1660\}\cf0 .
\par 
\par \pard If the syntax of a conversion specification is invalid, the behavior is undefined. If there aren't enough function arguments provided to supply addresses for all the conversion specifications in the template strings that perform assignments, or if the arguments are not of the correct types, the behavior is also undefined. On the other hand, extra arguments are simply ignored.
\par 
\par }
2240
PAGE_622
Table of Output Conversions
outputconversions,forprintf



Imported



FALSE
48
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Table of Output Conversions \{keepn\}
\par \pard\fs18 Next: \cf1\strike Integer Conversions\strike0\{linkID=1180\}\cf0 , Previous: \cf1\strike Output Conversion Syntax\strike0\{linkID=1680\}\cf0 , Up: \cf1\strike Formatted Output\strike0\{linkID=970\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Table of Output Conversions\b0 
\par 
\par Here is a table summarizing what all the different conversions do:
\par 
\par `\f1 %d\f0 ', `\f1 %i\f0 '
\par \pard\li500 Print an integer as a signed decimal number. See \cf1\strike Integer Conversions\strike0\{linkID=1180\}\cf0 , for details. `\f1 %d\f0 ' and `\f1 %i\f0 ' are synonymous for output, but are different when used with \f1 scanf\f0  for input (see \cf1\strike Table of Input Conversions\strike0\{linkID=2230\}\cf0 ). 
\par \pard `\f1 %o\f0 '
\par \pard\li500 Print an integer as an unsigned octal number. See \cf1\strike Integer Conversions\strike0\{linkID=1180\}\cf0 , for details. 
\par \pard `\f1 %u\f0 '
\par \pard\li500 Print an integer as an unsigned decimal number. See \cf1\strike Integer Conversions\strike0\{linkID=1180\}\cf0 , for details. 
\par \pard `\f1 %x\f0 ', `\f1 %X\f0 '
\par \pard\li500 Print an integer as an unsigned hexadecimal number. `\f1 %x\f0 ' uses lower-case letters and `\f1 %X\f0 ' uses upper-case. See \cf1\strike Integer Conversions\strike0\{linkID=1180\}\cf0 , for details. 
\par \pard `\f1 %f\f0 '
\par \pard\li500 Print a floating-point number in normal (fixed-point) notation. See \cf1\strike Floating-Point Conversions\strike0\{linkID=900\}\cf0 , for details. 
\par \pard `\f1 %e\f0 ', `\f1 %E\f0 '
\par \pard\li500 Print a floating-point number in exponential notation. `\f1 %e\f0 ' uses lower-case letters and `\f1 %E\f0 ' uses upper-case. See \cf1\strike Floating-Point Conversions\strike0\{linkID=900\}\cf0 , for details. 
\par \pard `\f1 %g\f0 ', `\f1 %G\f0 '
\par \pard\li500 Print a floating-point number in either normal or exponential notation, whichever is more appropriate for its magnitude. `\f1 %g\f0 ' uses lower-case letters and `\f1 %G\f0 ' uses upper-case. See \cf1\strike Floating-Point Conversions\strike0\{linkID=900\}\cf0 , for details. 
\par \pard `\f1 %a\f0 ', `\f1 %A\f0 '
\par \pard\li500 Print a floating-point number in a hexadecimal fractional notation which the exponent to base 2 represented in decimal digits. `\f1 %a\f0 ' uses lower-case letters and `\f1 %A\f0 ' uses upper-case. See \cf1\strike Floating-Point Conversions\strike0\{linkID=900\}\cf0 , for details. 
\par \pard `\f1 %c\f0 '
\par \pard\li500 Print a single character. See \cf1\strike Other Output Conversions\strike0\{linkID=1670\}\cf0 . 
\par \pard `\f1 %C\f0 '
\par \pard\li500 This is an alias for `\f1 %lc\f0 ' which is supported for compatibility with the Unix standard. 
\par \pard `\f1 %s\f0 '
\par \pard\li500 Print a string. See \cf1\strike Other Output Conversions\strike0\{linkID=1670\}\cf0 . 
\par \pard `\f1 %S\f0 '
\par \pard\li500 This is an alias for `\f1 %ls\f0 ' which is supported for compatibility with the Unix standard. 
\par \pard `\f1 %p\f0 '
\par \pard\li500 Print the value of a pointer. See \cf1\strike Other Output Conversions\strike0\{linkID=1670\}\cf0 . 
\par \pard `\f1 %n\f0 '
\par \pard\li500 Get the number of characters printed so far. See \cf1\strike Other Output Conversions\strike0\{linkID=1670\}\cf0 . Note that this conversion specification never produces any output. 
\par \pard `\f1 %m\f0 '
\par \pard\li500 Print the string corresponding to the value of \f1 errno\f0 . (This is a GNU extension.) See \cf1\strike Other Output Conversions\strike0\{linkID=1670\}\cf0 . 
\par \pard `\f1 %%\f0 '
\par \pard\li500 Print a literal `\f1 %\f0 ' character. See \cf1\strike Other Output Conversions\strike0\{linkID=1670\}\cf0 .
\par 
\par \pard If the syntax of a conversion specification is invalid, unpredictable things will happen, so don't do this. If there aren't enough function arguments provided to supply values for all the conversion specifications in the template string, or if the arguments are not of the correct types, the results are unpredictable. If you supply more arguments than conversion specifications, the extra argument values are simply ignored; this is sometimes useful.
\par 
\par }
2250
PAGE_623
Temporary Files
stdio.h;tmpfile;tmpfile;tmpnam;tmpnam_r;L_tmpnam;TMP_MAX;tempnam;TMPDIRenvironmentvariable;P_tmpdir;mktemp;mkstemp;mkdtemp



Imported



FALSE
42
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fswiss\fcharset0 Arial;}{\f2\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Temporary Files \{keepn\}
\par \pard\f1\fs18 Next: \cf1\strike File Size\cf2\strike0\{linkID=800\}\cf0 , \f0 Previous: \cf1\strike Creating Directories\cf2\strike0\{linkID=490\}\cf0 , Up: \cf1\strike File System Interface\strike0\{linkID=820\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Temporary Files\b0 
\par 
\par If you need to use a temporary file in your program, you can use the\f2  tmpfile\f0  function to open it. Or you can use the \f2 tmpnam\f0  (better: \f2 tmpnam_r\f0 ) function to provide a name for a temporary file and then you can open it in the usual way with \f2 fopen\f0 . 
\par 
\par The \f2 tempnam\f0  function is like \f2 tmpnam\f0  but lets you choose what directory temporary files will go in, and something about what their file names will look like. Important for multi-threaded programs is that \f2 tempnam\f0  is reentrant, while \f2 tmpnam\f0  is not since it returns a pointer to a static buffer. 
\par 
\par These facilities are declared in the header file \f2 stdio.h\f0 .
\par 
\par -- Function: FILE * \b tmpfile\b0  (\i void\i0 )
\par 
\par \pard\li500 This function creates a temporary binary file for update mode, as if by calling \f2 fopen\f0  with mode \f2 "wb+"\f0 . The file is deleted automatically when it is closed or when the program terminates. (On some other ISO C systems the file may fail to be deleted if the program terminates abnormally). 
\par 
\par This function is reentrant. 
\par 
\par \pard -- Function: char * \b tmpnam\b0  (\i char *result\i0 )
\par 
\par \pard\li500 This function constructs and returns a valid file name that does not refer to any existing file. If the \i result\i0  argument is a null pointer, the return value is a pointer to an internal static string, which might be modified by subsequent calls and therefore makes this function non-reentrant. Otherwise, the \i result\i0  argument should be a pointer to an array of at least \f2 L_tmpnam\f0  characters, and the result is written into that array. 
\par 
\par It is possible for \f2 tmpnam\f0  to fail if you call it too many times without removing previously-created files. This is because the limited length of the temporary file names gives room for only a finite number of different names. If \f2 tmpnam\f0  fails it returns a null pointer. 
\par 
\par \b Warning:\b0  Between the time the pathname is constructed and the file is created another process might have created a file with the same name using \f2 tmpnam\f0 , leading to a possible security hole. The implementation generates names which can hardly be predicted, but when opening the file you should use the \f2 O_EXCL\f0  flag. Using\f2  tmpfile\f0  or \f2 mkstemp\f0  is a safe way to avoid this problem.
\par 
\par \pard -- Macro: int \b L_tmpnam\b0 
\par 
\par \pard\li500 The value of this macro is an integer constant expression that represents the minimum size of a string large enough to hold a file name generated by the \f2 tmpnam\f0  function.
\par 
\par \pard -- Macro: int \b TMP_MAX\b0 
\par 
\par \pard\li500 The macro \f2 TMP_MAX\f0  is a lower bound for how many temporary names you can create with \f2 tmpnam\f0 . You can rely on being able to call\f2  tmpnam\f0  at least this many times before it might fail saying you have made too many temporary file names. 
\par 
\par With the GNU library, you can create a very large number of temporary file names. If you actually created the files, you would probably run out of disk space before you ran out of names. Some other systems have a fixed, small limit on the number of temporary files. The limit is never less than \f2 25\f0 .
\par 
\par 
\par }
2260
PAGE_626
Termination in Handler




Imported



FALSE
42
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Termination in Handler \{keepn\}
\par \pard\fs18 Next: \cf1\strike Longjmp in Handler\strike0\{linkID=1350\}\cf0 , Previous: \cf1\strike Handler Returns\strike0\{linkID=1080\}\cf0 , Up: \cf1\strike Defining Handlers\strike0\{linkID=530\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Handlers That Terminate the Process\b0 
\par 
\par Handler functions that terminate the program are typically used to cause orderly cleanup or recovery from program error signals and interactive interrupts. 
\par 
\par The cleanest way for a handler to terminate the process is to raise the same signal that ran the handler in the first place. Here is how to do this:
\par 
\par \f1      volatile sig_atomic_t fatal_error_in_progress = 0;
\par      
\par      void
\par      fatal_error_signal (int sig)
\par      \{
\par        /* Since this handler is established for more than one kind of signal, 
\par           it might still get invoked recursively by delivery of some other kind
\par           of signal.  Use a static variable to keep track of that. */
\par        if (fatal_error_in_progress)
\par          raise (sig);
\par        fatal_error_in_progress = 1;
\par      
\par        /* Now do the clean up actions:
\par           - reset terminal modes
\par           - kill child processes
\par           - remove lock files */
\par        ...
\par      
\par        /* Now reraise the signal.  We reactivate the signal's
\par           default handling, which is to terminate the process.
\par           We could just call exit or abort,
\par           but reraising the signal sets the return status
\par           from the process correctly. */
\par        signal (sig, SIG_DFL);
\par        raise (sig);
\par      \}
\par \f0 
\par }
2280
PAGE_635
The Lame Way to Locale Data
locale.h;monetaryvalueformatting;numericvalueformatting;localeconv;structlconv



Imported



FALSE
28
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 The Lame Way to Locale Data \{keepn\}
\par \pard\fs18 Up: \cf1\strike Locale Information\strike0\{linkID=1330\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b\f1 localeconv\f0 : It is portable but ...\b0 
\par 
\par Together with the \f1 setlocale\f0  function the ISO C people invented the \f1 localeconv\f0  function. It is a masterpiece of poor design. It is expensive to use, not extendable, and not generally usable as it provides access to only \f1 LC_MONETARY\f0  and\f1  LC_NUMERIC\f0  related information. Nevertheless, if it is applicable to a given situation it should be used since it is very portable. The function \f1 strfmon\f0  formats monetary amounts according to the selected locale using this information.
\par -- Function: struct lconv * \b localeconv\b0  (\i void\i0 )
\par 
\par \pard\li500 The \f1 localeconv\f0  function returns a pointer to a structure whose components contain information about how numeric and monetary values should be formatted in the current locale. 
\par 
\par You should not modify the structure or its contents. The structure might be overwritten by subsequent calls to \f1 localeconv\f0 , or by calls to\f1  setlocale\f0 , but no other function in the library overwrites this value.
\par 
\par \pard -- Data Type: \b struct lconv\b0 
\par 
\par \pard\li500\f1 localeconv\f0 's return value is of this data type. Its elements are described in the following subsections.
\par 
\par \pard If a member of the structure \f1 struct lconv\f0  has type \f1 char\f0 , and the value is \f1 CHAR_MAX\f0 , it means that the current locale has no value for that parameter.
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike General Numeric\strike0\{linkID=1040\}\cf0 : Parameters for formatting numbers and currency amounts.
\par \f2\'b7\f0  \cf1\strike Currency Symbol\strike0\{linkID=500\}\cf0 : How to print the symbol that identifies an amount of money (e.g. `\f1 $\f0 ').
\par \f2\'b7\f0  \cf1\strike Sign of Money Amount\strike0\{linkID=1980\}\cf0 : How to print the (positive or negative) sign for a monetary amount, if one exists.
\par 
\par }
2290
PAGE_639
Time Basics
time;calendartime;date;interval;elapsedtime;time,elapsed;periodoftime;CPUtime;processortime



Imported



FALSE
17
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Time Basics \{keepn\}
\par \pard\fs18 Next: \cf1\strike Elapsed Time\strike0\{linkID=620\}\cf0 , Up: \cf1\strike Date and Time\strike0\{linkID=520\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Time Basics\b0 
\par 
\par Discussing time in a technical manual can be difficult because the word "time" in English refers to lots of different things. In this manual, we use a rigorous terminology to avoid confusion, and the only thing we use the simple word "time" for is to talk about the abstract concept. 
\par 
\par A \i calendar time\i0  is a point in the time continuum, for example November 4, 1990 at 18:02.5 UTC. Sometimes this is called "absolute time". We don't speak of a "date", because that is inherent in a calendar time. An \i interval\i0  is a contiguous part of the time continuum between two calendar times, for example the hour between 9:00 and 10:00 on July 4, 1980. An \i elapsed time\i0  is the length of an interval, for example, 35 minutes. People sometimes sloppily use the word "interval" to refer to the elapsed time of some interval. An \i amount of time\i0  is a sum of elapsed times, which need not be of any specific intervals. For example, the amount of time it takes to read a book might be 9 hours, independently of when and in how many sittings it is read. 
\par 
\par A \i period\i0  is the elapsed time of an interval between two events, especially when they are part of a sequence of regularly repeating events.\i  CPU time\i0  is like calendar time, except that it is based on the subset of the time continuum when a particular process is actively using a CPU. CPU time is, therefore, relative to a process.\i  Processor time\i0  is an amount of time that a CPU is in use. In fact, it's a basic system resource, since there's a limit to how much can exist in any given interval (that limit is the elapsed time of the interval times the number of CPUs in the processor). People often call this CPU time, but we reserve the latter term in this manual for the definition above.
\par 
\par }
2300
PAGE_640
Time Functions Example




Imported



FALSE
49
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Time Functions Example \{keepn\}
\par \pard\fs18 Previous: \cf1\strike Time Zone Functions\strike0\{linkID=2310\}\cf0 , Up: \cf1\strike Calendar Time\strike0\{linkID=230\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Time Functions Example\b0 
\par 
\par Here is an example program showing the use of some of the calendar time functions.
\par 
\par \f1      #include <time.h>
\par      #include <stdio.h>
\par      
\par      #define SIZE 256
\par      
\par      int
\par      main (void)
\par      \{
\par        char buffer[SIZE];
\par        time_t curtime;
\par        struct tm *loctime;
\par      
\par        /* Get the current time. */
\par        curtime = time (NULL);
\par      
\par        /* Convert it to local time representation. */
\par        loctime = localtime (&curtime);
\par      
\par        /* Print out the date and time in the standard format. */
\par        fputs (asctime (loctime), stdout);
\par      
\par        /* Print it out in a nice format. */
\par        strftime (buffer, SIZE, "Today is %A, %B %d.\\n", loctime);
\par        fputs (buffer, stdout);
\par        strftime (buffer, SIZE, "The time is %I:%M %p.\\n", loctime);
\par        fputs (buffer, stdout);
\par      
\par        return 0;
\par      \}
\par \f0 
\par It produces output like this:
\par 
\par \f1      Wed Jul 31 13:02:36 1991
\par      Today is Wednesday, July 31.
\par      The time is 01:02 PM.
\par \f0 
\par }
2310
PAGE_641
Time Zone Functions
tzname;tzset;timezone;daylight



Imported



FALSE
33
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss\fcharset0 Arial;}{\f1\fswiss Arial;}{\f2\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 T\f1 ime Zone Functions \{keepn\}
\par \pard\fs18 Next: \cf1\strike Time Functions Example\strike0\{linkID=2300\}\cf0 , Previous: \cf1\strike TZ Variable\strike0\{linkID=2330\}\cf0 , Up: \cf1\strike Calendar Time\strike0\{linkID=230\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Functions and Variables for Time Zones\b0 
\par 
\par -- Variable: char * \b tzname\b0  [\i 2\i0 ]
\par 
\par \pard\li500 The array \f2 tzname\f1  contains two strings, which are the standard names of the pair of time zones (standard and Daylight Saving) that the user has selected. \f2 tzname[0]\f1  is the name of the standard time zone (for example, \f2 "EST"\f1 ), and \f2 tzname[1]\f1  is the name for the time zone when Daylight Saving Time is in use (for example, \f2 "EDT"\f1 ). These correspond to the \i std\i0  and \i dst\i0  strings (respectively) from the \f2 TZ\f1  environment variable. If Daylight Saving Time is never used, \f2 tzname[1]\f1  is the empty string. 
\par 
\par The \f2 tzname\f1  array is initialized from the \f2 TZ\f1  environment variable whenever \f2 tzset\f1 , \f2 ctime\f1 , \f2 strftime\f1 ,\f2  mktime\f1 , or \f2 localtime\f1  is called. If multiple abbreviations have been used (e.g. \f2 "EWT"\f1  and \f2 "EDT"\f1  for U.S. Eastern War Time and Eastern Daylight Time), the array contains the most recent abbreviation. 
\par 
\par The \f2 tzname\f1  array is required for POSIX.1 compatibility, but in GNU programs it is better to use the \f2 tm_zone\f1  member of the broken-down time structure, since \f2 tm_zone\f1  reports the correct abbreviation even when it is not the latest one. 
\par 
\par Though the strings are declared as \f2 char *\f1  the user must refrain from modifying these strings. Modifying the strings will almost certainly lead to trouble.
\par 
\par \pard -- Function: void \b tzset\b0  (\i void\i0 )
\par 
\par \pard\li500 The \f2 tzset\f1  function initializes the \f2 tzname\f1  variable from the value of the \f2 TZ\f1  environment variable. It is not usually necessary for your program to call this function, because it is called automatically when you use the other time conversion functions that depend on the time zone.
\par 
\par \pard -- Variable: long int \b timezone\b0 
\par 
\par \pard\li500 This contains the difference between UTC and the latest local standard time, in seconds west of UTC. For example, in the U.S. Eastern time zone, the value is \f2 5*60*60\f1 . 
\par 
\par \pard -- Variable: int \b daylight\b0 
\par 
\par \pard\li500 This variable has a nonzero value if Daylight Saving Time rules apply. A nonzero value does not necessarily mean that Daylight Saving Time is now in effect; it means only that Daylight Saving Time is sometimes in effect.
\par 
\par }
2320
PAGE_652
Trig Functions
trigonometricfunctions;pi(trigonometricconstant;sin;sinf;sinl;cos;cosf;cosl;tan;tanf;tanl;sincos;sincosf;sincosl;complextrigonometricfunctions;csin;csinf;csinl;ccos;ccosf;ccosl;ctan;ctanf;ctanl



Imported



FALSE
91
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Trig Functions \{keepn\}
\par \pard\fs18 Next: \cf1\strike Inverse Trig Functions\strike0\{linkID=1220\}\cf0 , Previous: \cf1\strike Mathematical Constants\strike0\{linkID=1400\}\cf0 , Up: \cf1\strike Mathematics\strike0\{linkID=1410\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Trigonometric Functions\b0 
\par 
\par These are the familiar \f1 sin\f0 , \f1 cos\f0 , and \f1 tan\f0  functions. The arguments to all of these functions are in units of radians; recall that pi radians equals 180 degrees. 
\par 
\par The math library normally defines \f1 M_PI\f0  to a \f1 double\f0  approximation of pi. If strict ISO and/or POSIX compliance are requested this constant is not defined, but you can easily define it yourself:
\par 
\par \f1      #define M_PI 3.14159265358979323846264338327
\par \f0 
\par You can also compute the value of pi with the expression \f1 acos (-1.0)\f0 .
\par 
\par -- Function: double \b sin\b0  (\i double x\i0 )
\par 
\par \pard\li500 -- Function: float \b sinf\b0  (\i float x\i0 )
\par 
\par \pard\li1000 -- Function: long double \b sinl\b0  (\i long double x\i0 )
\par 
\par \pard\li1500 These functions return the sine of \i x\i0 , where \i x\i0  is given in radians. The return value is in the range \f1 -1\f0  to \f1 1\f0 .
\par 
\par \pard -- Function: double \b cos\b0  (\i double x\i0 )
\par 
\par \pard\li500 -- Function: float \b cosf\b0  (\i float x\i0 )
\par 
\par \pard\li1000 -- Function: long double \b cosl\b0  (\i long double x\i0 )
\par 
\par \pard\li1500 These functions return the cosine of \i x\i0 , where \i x\i0  is given in radians. The return value is in the range \f1 -1\f0  to \f1 1\f0 .
\par 
\par \pard -- Function: double \b tan\b0  (\i double x\i0 )
\par 
\par \pard\li500 -- Function: float \b tanf\b0  (\i float x\i0 )
\par 
\par \pard\li1000 -- Function: long double \b tanl\b0  (\i long double x\i0 )
\par 
\par \pard\li1500 These functions return the tangent of \i x\i0 , where \i x\i0  is given in radians. 
\par 
\par Mathematically, the tangent function has singularities at odd multiples of pi/2. If the argument \i x\i0  is too close to one of these singularities, \f1 tan\f0  will signal overflow.
\par 
\par \pard In many applications where \f1 sin\f0  and \f1 cos\f0  are used, the sine and cosine of the same angle are needed at the same time. It is more efficient to compute them simultaneously, so the library provides a function to do that.
\par 
\par -- Function: void \b sincos\b0  (\i double x, double *sinx, double *cosx\i0 )
\par 
\par \pard\li500 -- Function: void \b sincosf\b0  (\i float x, float *sinx, float *cosx\i0 )
\par 
\par \pard\li1000 -- Function: void \b sincosl\b0  (\i long double x, long double *sinx, long double *cosx\i0 )
\par 
\par \pard\li1500 These functions return the sine of \i x\i0  in \f1 *\i\f0 sinx\i0  and the cosine of \i x\i0  in \f1 *\i\f0 cos\i0 , where \i x\i0  is given in radians. Both values, \f1 *\i\f0 sinx\i0  and \f1 *\i\f0 cosx\i0 , are in the range of \f1 -1\f0  to \f1 1\f0 . 
\par 
\par This function is a GNU extension. Portable programs should be prepared to cope with its absence.
\par 
\par \pard ISO C99 defines variants of the trig functions which work on complex numbers. The C library provides these functions, but they are only useful if your compiler supports the new complex types defined by the standard.
\par 
\par -- Function: complex double \b csin\b0  (\i complex double z\i0 )
\par 
\par \pard\li500 -- Function: complex float \b csinf\b0  (\i complex float z\i0 )
\par 
\par \pard\li1000 -- Function: complex long double \b csinl\b0  (\i complex long double z\i0 )
\par 
\par \pard\li1500 These functions return the complex sine of \i z\i0 . The mathematical definition of the complex sine is 
\par 
\par sin (z) = 1/(2*i) * (exp (z*i) - exp (-z*i)).
\par 
\par \pard -- Function: complex double \b ccos\b0  (\i complex double z\i0 )
\par 
\par \pard\li500 -- Function: complex float \b ccosf\b0  (\i complex float z\i0 )
\par 
\par \pard\li1000 -- Function: complex long double \b ccosl\b0  (\i complex long double z\i0 )
\par 
\par \pard\li1500 These functions return the complex cosine of \i z\i0 . The mathematical definition of the complex cosine is 
\par 
\par cos (z) = 1/2 * (exp (z*i) + exp (-z*i))
\par 
\par \pard -- Function: complex double \b ctan\b0  (\i complex double z\i0 )
\par 
\par \pard\li500 -- Function: complex float \b ctanf\b0  (\i complex float z\i0 )
\par 
\par \pard\li1000 -- Function: complex long double \b ctanl\b0  (\i complex long double z\i0 )
\par 
\par \pard\li1500 These functions return the complex tangent of \i z\i0 . The mathematical definition of the complex tangent is 
\par 
\par tan (z) = -i * (exp (z*i) - exp (-z*i)) / (exp (z*i) + exp (-z*i)) 
\par 
\par The complex tangent has poles at pi/2 + 2n, where n is an integer. \f1 ctan\f0  may signal overflow if \i z\i0  is too close to a pole.
\par 
\par }
2330
PAGE_655
TZ Variable
time.h;timezone;gt/etc/localtime;localtime;timezonedatabase;gt/share/lib/zoneinfo;zoneinfo



Imported



FALSE
49
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fswiss\fcharset0 Arial;}{\f2\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 TZ Variable \{keepn\}
\par \pard\fs18 Next: \cf1\strike Time Zone Functions\strike0\{linkID=2310\}\cf0 , Previous: \cf1\strike\f1 Formatting Calendar Time\cf2\strike0\{linkID=980\}\cf0\f0 , Up: \cf1\strike Calendar Time\strike0\{linkID=230\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Specifying the Time Zone with \f2 TZ\b0\f0 
\par 
\par In POSIX systems, a user can specify the time zone by means of the\f2  TZ\f0  environment variable. For information about how to set environment variables, see \cf1\strike Environment Variables\strike0\{linkID=640\}\cf0 . The functions for accessing the time zone are declared in \f2 time.h\f0 . You should not normally need to set \f2 TZ\f0 . If the system is configured properly, the default time zone will be correct. You might set \f2 TZ\f0  if you are using a computer over a network from a different time zone, and would like times reported to you in the time zone local to you, rather than what is local to the computer. 
\par 
\par In POSIX.1 systems the value of the \f2 TZ\f0  variable can be in one of three formats. With the C library, the most common format is the last one, which can specify a selection from a large database of time zone information for many regions of the world. The first two formats are used to describe the time zone information directly, which is both more cumbersome and less precise. But the POSIX.1 standard only specifies the details of the first two formats, so it is good to be familiar with them in case you come across a POSIX.1 system that doesn't support a time zone information database. 
\par 
\par The first format is used when there is no Daylight Saving Time (or summer time) in the local time zone:
\par 
\par \f2      \i std\i0  \i offset
\par \i0\f0 
\par The \i std\i0  string specifies the name of the time zone. It must be three or more characters long and must not contain a leading colon, embedded digits, commas, nor plus and minus signs. There is no space character separating the time zone name from the \i offset\i0 , so these restrictions are necessary to parse the specification correctly. 
\par 
\par The \i offset\i0  specifies the time value you must add to the local time to get a Coordinated Universal Time value. It has syntax like [\f2 +\f0 |\f2 -\f0 ]\i hh\i0 [\f2 :\i\f0 mm\i0 [\f2 :\i\f0 ss\i0 ]]. This is positive if the local time zone is west of the Prime Meridian and negative if it is east. The hour must be between \f2 0\f0  and\f2  23\f0 , and the minute and seconds between \f2 0\f0  and \f2 59\f0 . 
\par 
\par For example, here is how we would specify Eastern Standard Time, but without any Daylight Saving Time alternative:
\par 
\par \f2      EST+5
\par \f0 
\par The second format is used when there is Daylight Saving Time:
\par 
\par \f2      \i std\i0  \i offset\i0  \i dst\i0  [\i offset\i0 ],\i start\i0 [/\i time\i0 ],\i end\i0 [/\i time\i0 ]
\par \f0 
\par The initial \i std\i0  and \i offset\i0  specify the standard time zone, as described above. The \i dst\i0  string and \i offset\i0  specify the name and offset for the corresponding Daylight Saving Time zone; if the\i  offset\i0  is omitted, it defaults to one hour ahead of standard time. 
\par 
\par The remainder of the specification describes when Daylight Saving Time is in effect. The \i start\i0  field is when Daylight Saving Time goes into effect and the \i end\i0  field is when the change is made back to standard time. The following formats are recognized for these fields:
\par 
\par \f2 J\i\f0 n\i0 
\par \pard\li500 This specifies the Julian day, with \i n\i0  between \f2 1\f0  and \f2 365\f0 . February 29 is never counted, even in leap years. 
\par \pard\i n\i0 
\par \pard\li500 This specifies the Julian day, with \i n\i0  between \f2 0\f0  and \f2 365\f0 . February 29 is counted in leap years. 
\par \pard\f2 M\i\f0 m\i0\f2 .\i\f0 w\i0\f2 .\i\f0 d\i0 
\par \pard\li500 This specifies day \i d\i0  of week \i w\i0  of month \i m\i0 . The day\i  d\i0  must be between \f2 0\f0  (Sunday) and \f2 6\f0 . The week\i  w\i0  must be between \f2 1\f0  and \f2 5\f0 ; week \f2 1\f0  is the first week in which day \i d\i0  occurs, and week \f2 5\f0  specifies the\i  last\i0  \i d\i0  day in the month. The month \i m\i0  should be between \f2 1\f0  and \f2 12\f0 .
\par 
\par \pard The \i time\i0  fields specify when, in the local time currently in effect, the change to the other time occurs. If omitted, the default is\f2  02:00:00\f0 . 
\par 
\par For example, here is how you would specify the Eastern time zone in the United States, including the appropriate Daylight Saving Time and its dates of applicability. The normal offset from UTC is 5 hours; since this is west of the prime meridian, the sign is positive. Summer time begins on the first Sunday in April at 2:00am, and ends on the last Sunday in October at 2:00am.
\par 
\par \f2      EST+5EDT,M4.1.0/2,M10.5.0/2
\par \f0 
\par The schedule of Daylight Saving Time in any particular jurisdiction has changed over the years. To be strictly correct, the conversion of dates and times in the past should be based on the schedule that was in effect then. However, this format has no facilities to let you specify how the schedule has changed from year to year. The most you can do is specify one particular schedule--usually the present day schedule--and this is used to convert any date, no matter when. 
\par }
2340
PAGE_656
Unconstrained Allocation
unconstrainedmemoryallocation;malloc;heap,dynamicallocationfrom



Imported



FALSE
18
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fswiss\fcharset0 Arial;}{\f2\fmodern Courier New;}{\f3\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Unconstrained Allocation \{keepn\}
\par \pard\f1\fs18 Next: \cf1\strike Variable Size Automatic\cf2\strike0\{linkID=2400\}\cf0 , \f0 Previous: \cf1\strike Memory Allocation and C\strike0\{linkID=1420\}\cf0 , Up: \cf1\strike Memory Allocation\strike0\{linkID=1430\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Unconstrained Allocation\b0 
\par 
\par The most general dynamic allocation facility is \f2 malloc\f0 . It allows you to allocate blocks of memory of any size at any time, make them bigger or smaller at any time, and free the blocks individually at any time (or never).
\par 
\par \pard\li500\f3\'b7\f0  \cf1\strike Basic Allocation\strike0\{linkID=170\}\cf0 : Simple use of \f2 malloc\f0 .
\par \f3\'b7\f0  \cf1\strike Malloc Examples\strike0\{linkID=1380\}\cf0 : Examples of \f2 malloc\f0 . \f2 xmalloc\f0 .
\par \f3\'b7\f0  \cf1\strike Freeing after Malloc\strike0\{linkID=1030\}\cf0 : Use \f2 free\f0  to free a block you got with \f2 malloc\f0 .
\par \f3\'b7\f0  \cf1\strike Changing Block Size\strike0\{linkID=260\}\cf0 : Use \f2 realloc\f0  to make a block bigger or smaller.
\par \f3\'b7\f0  \cf1\strike Allocating Cleared Space\strike0\{linkID=70\}\cf0 : Use \f2 calloc\f0  to allocate a block and clear it.\cf1\strike 
\par }
2350
PAGE_657
Unreading Idea




Imported



FALSE
36
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Unreading Idea \{keepn\}
\par \pard\fs18 Next: \cf1\strike How Unread\strike0\{linkID=1110\}\cf0 , Up: \cf1\strike Unreading\strike0\{linkID=2360\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b What Unreading Means\b0 
\par 
\par Here is a pictorial explanation of unreading. Suppose you have a stream reading a file that contains just six characters, the letters `\f1 foobar\f0 '. Suppose you have read three characters so far. The situation looks like this:
\par 
\par \f1      f  o  o  b  a  r
\par               ^
\par \f0 
\par so the next input character will be `\f1 b\f0 '.
\par 
\par If instead of reading `\f1 b\f0 ' you unread the letter `\f1 o\f0 ', you get a situation like this:
\par 
\par \f1      f  o  o  b  a  r
\par               |
\par            o--
\par            ^
\par \f0 
\par so that the next input characters will be `\f1 o\f0 ' and `\f1 b\f0 '.
\par 
\par If you unread `\f1 9\f0 ' instead of `\f1 o\f0 ', you get this situation:
\par 
\par \f1      f  o  o  b  a  r
\par               |
\par            9--
\par            ^
\par \f0 
\par so that the next input characters will be `\f1 9\f0 ' and `\f1 b\f0 '.
\par 
\par }
2360
PAGE_658
Unreading
peekingatinput;unreadingcharacters;pushinginputback



Imported



FALSE
18
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fswiss\fcharset0 Arial;}{\f2\fmodern Courier New;}{\f3\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Unreading\{keepn\}
\par \pard\fs18 Next: \cf1\strike Line Input\cf2\strike0\{linkID=1316\}\cf0 , Previous: \cf1\strike\f1 Character Input\cf2\strike0\f0\{linkID=280\}\cf0 , Up: \cf1\strike I/O on Streams\strike0\{linkID=1260\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Unreading\b0 
\par 
\par In parser programs it is often useful to examine the next character in the input stream without removing it from the stream. This is called "peeking ahead" at the input because your program gets a glimpse of the input it will read next. 
\par 
\par Using stream I/O, you can peek ahead at input by first reading it and then \i unreading\i0  it (also called \i pushing it back\i0  on the stream). Unreading a character makes it available to be input again from the stream, by the next call to \f2 fgetc\f0  or other input function on that stream.
\par 
\par \pard\li500\f3\'b7\f0  \cf1\strike Unreading Idea\strike0\{linkID=2350\}\cf0 : An explanation of unreading with pictures.
\par \f3\'b7\f0  \cf1\strike How Unread\strike0\{linkID=1110\}\cf0 : How to call \f2 ungetc\f0  to do unreading.
\par 
\par }
2370
PAGE_669
Using Wide Char Classes




Imported



FALSE
48
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Using Wide Char Classes \{keepn\}
\par \pard\fs18 Next: \cf1\strike Wide Character Case Conversion\strike0\{linkID=2450\}\cf0 , Previous: \cf1\strike Classification of Wide Characters\strike0\{linkID=340\}\cf0 , Up: \cf1\strike Character Handling\strike0\{linkID=270\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Notes on using the wide character classes\b0 
\par 
\par The first note is probably not astonishing but still occasionally a cause of problems. The \f1 isw\i\f0 XXX\i0  functions can be implemented using macros and in fact, the \f2 C\f0  C library does this. They are still available as real functions but when the \f1 wctype.h\f0  header is included the macros will be used. This is the same as the\f1  char\f0  type versions of these functions. 
\par 
\par The second note covers something new. It can be best illustrated by a (real-world) example. The first piece of code is an excerpt from the original code. It is truncated a bit but the intention should be clear.
\par 
\par \f1      int
\par      is_in_class (int c, const char *class)
\par      \{
\par        if (strcmp (class, "alnum") == 0)
\par          return isalnum (c);
\par        if (strcmp (class, "alpha") == 0)
\par          return isalpha (c);
\par        if (strcmp (class, "cntrl") == 0)
\par          return iscntrl (c);
\par        ...
\par        return 0;
\par      \}
\par \f0 
\par Now, with the \f1 wctype\f0  and \f1 iswctype\f0  you can avoid the\f1  if\f0  cascades, but rewriting the code as follows is wrong:
\par 
\par \f1      int
\par      is_in_class (int c, const char *class)
\par      \{
\par        wctype_t desc = wctype (class);
\par        return desc ? iswctype ((wint_t) c, desc) : 0;
\par      \}
\par \f0 
\par The problem is that it is not guaranteed that the wide character representation of a single-byte character can be found using casting. In fact, usually this fails miserably. The correct solution to this problem is to write the code as follows:
\par 
\par \f1      int
\par      is_in_class (int c, const char *class)
\par      \{
\par        wctype_t desc = wctype (class);
\par        return desc ? iswctype (btowc (c), desc) : 0;
\par      \}
\par \f0 
\par See \cf1\strike Converting a Character\strike0\{linkID=440\}\cf0 , for more information on \f1 btowc\f0 . Note that this change probably does not improve the performance of the program a lot since the \f1 wctype\f0  function still has to make the string comparisons. It gets really interesting if the\f1  is_in_class\f0  function is called more than once for the same class name. In this case the variable \i desc\i0  could be computed once and reused for all the calls. Therefore the above form of the function is probably not the final one.
\par 
\par }
2380
PAGE_672
Variable Arguments Input
vscanf;vwscanf;vfscanf;vfwscanf;vsscanf;vswscanf



Imported



FALSE
39
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Variable Arguments Input \{keepn\}
\par \pard\fs18 Previous: \cf1\strike Formatted Input Functions\strike0\{linkID=930\}\cf0 , Up: \cf1\strike Formatted Input\strike0\{linkID=940\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Variable Arguments Input Functions\b0 
\par 
\par The functions \f1 vscanf\f0  and friends are provided so that you can define your own variadic \f1 scanf\f0 -like functions that make use of the same internals as the built-in formatted output functions. These functions are analogous to the \f1 vprintf\f0  series of output functions. See \cf1\strike Variable Arguments Output\strike0\{linkID=2390\}\cf0 , for important information on how to use them. 
\par 
\par \b Portability Note:\b0  The functions listed in this section were introduced in ISO C99.
\par 
\par -- Function: int \b vscanf\b0  (\i const char *template, va_list ap\i0 )
\par 
\par \pard\li500 This function is similar to \f1 scanf\f0 , but instead of taking a variable number of arguments directly, it takes an argument list pointer \i ap\i0  of type \f1 va_list\f0  (see \cf1\strike Variadic Functions\strike0\{linkID=2420\}\cf0 ).
\par 
\par \pard -- Function: int \b vwscanf\b0  (\i const wchar_t *template, va_list ap\i0 )
\par 
\par \pard\li500 This function is similar to \f1 wscanf\f0 , but instead of taking a variable number of arguments directly, it takes an argument list pointer \i ap\i0  of type \f1 va_list\f0  (see \cf1\strike Variadic Functions\strike0\{linkID=2420\}\cf0 ).
\par 
\par \pard -- Function: int \b vfscanf\b0  (\i FILE *stream, const char *template, va_list ap\i0 )
\par 
\par \pard\li500 This is the equivalent of \f1 fscanf\f0  with the variable argument list specified directly as for \f1 vscanf\f0 .
\par 
\par \pard -- Function: int \b vfwscanf\b0  (\i FILE *stream, const wchar_t *template, va_list ap\i0 )
\par 
\par \pard\li500 This is the equivalent of \f1 fwscanf\f0  with the variable argument list specified directly as for \f1 vwscanf\f0 .
\par 
\par \pard -- Function: int \b vsscanf\b0  (\i const char *s, const char *template, va_list ap\i0 )
\par 
\par \pard\li500 This is the equivalent of \f1 sscanf\f0  with the variable argument list specified directly as for \f1 vscanf\f0 .
\par 
\par \pard -- Function: int \b vswscanf\b0  (\i const wchar_t *s, const wchar_t *template, va_list ap\i0 )
\par 
\par \pard\li500 This is the equivalent of \f1 swscanf\f0  with the variable argument list specified directly as for \f1 vwscanf\f0 .
\par \pard 
\par }
2390
PAGE_673
Variable Arguments Output
stdio.h;vprintf;vwprintf;vfprintf;vfwprintf;vsprintf;vswprintf;vsnprintf;vasprintf;obstack_vprintf



Imported



FALSE
91
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Variable Arguments Output \{keepn\}
\par \pard\fs18 Previous: \cf1\strike Formatted Output Functions\cf2\strike0\{linkID=960\}\cf0 , Up: \cf1\strike Formatted Output\strike0\{linkID=970\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Variable Arguments Output Functions\b0 
\par 
\par The functions \f1 vprintf\f0  and friends are provided so that you can define your own variadic \f1 printf\f0 -like functions that make use of the same internals as the built-in formatted output functions. 
\par 
\par The most natural way to define such functions would be to use a language construct to say, "Call \f1 printf\f0  and pass this template plus all of my arguments after the first five." But there is no way to do this in C, and it would be hard to provide a way, since at the C language level there is no way to tell how many arguments your function received. 
\par 
\par Since that method is impossible, we provide alternative functions, the\f1  vprintf\f0  series, which lets you pass a \f1 va_list\f0  to describe "all of my arguments after the first five." 
\par 
\par When it is sufficient to define a macro rather than a real function, \f2 C99\f0  provides a way to do this much more easily with macros. For example:
\par 
\par \f1      #define myprintf(a, b, c, d, e, rest...) \\
\par                  printf (mytemplate , ## rest...)
\par \f0 
\par But this is limited to macros, and does not apply to real functions at all. 
\par 
\par Before calling \f1 vprintf\f0  or the other functions listed in this section, you \i must\i0  call \f1 va_start\f0  (see \cf1\strike Variadic Functions\strike0\{linkID=2420\}\cf0 ) to initialize a pointer to the variable arguments. Then you can call \f1 va_arg\f0  to fetch the arguments that you want to handle yourself. This advances the pointer past those arguments. 
\par 
\par Once your \f1 va_list\f0  pointer is pointing at the argument of your choice, you are ready to call \f1 vprintf\f0 . That argument and all subsequent arguments that were passed to your function are used by\f1  vprintf\f0  along with the template that you specified separately. 
\par 
\par In some other systems, the \f1 va_list\f0  pointer may become invalid after the call to \f1 vprintf\f0 , so you must not use \f1 va_arg\f0  after you call \f1 vprintf\f0 . Instead, you should call \f1 va_end\f0  to retire the pointer from service. However, you can safely call\f1  va_start\f0  on another pointer variable and begin fetching the arguments again through that pointer. Calling \f1 vprintf\f0  does not destroy the argument list of your function, merely the particular pointer that you passed to it. 
\par 
\par \f2 CC386\f0  does not have such restrictions. You can safely continue to fetch arguments from a \f1 va_list\f0  pointer after passing it to\f1  vprintf\f0 , and \f1 va_end\f0  is a no-op. (Note, however, that subsequent \f1 va_arg\f0  calls will fetch the same arguments which\f1  vprintf\f0  previously used.) 
\par 
\par Prototypes for these functions are declared in \f1 stdio.h\f0 .
\par 
\par -- Function: int \b vprintf\b0  (\i const char *template, va_list ap\i0 )
\par 
\par \pard\li500 This function is similar to \f1 printf\f0  except that, instead of taking a variable number of arguments directly, it takes an argument list pointer \i ap\i0 .
\par 
\par \pard -- Function: int \b vwprintf\b0  (\i const wchar_t *template, va_list ap\i0 )
\par 
\par \pard\li500 This function is similar to \f1 wprintf\f0  except that, instead of taking a variable number of arguments directly, it takes an argument list pointer \i ap\i0 .
\par 
\par \pard -- Function: int \b vfprintf\b0  (\i FILE *stream, const char *template, va_list ap\i0 )
\par 
\par \pard\li500 This is the equivalent of \f1 fprintf\f0  with the variable argument list specified directly as for \f1 vprintf\f0 .
\par 
\par \pard -- Function: int \b vfwprintf\b0  (\i FILE *stream, const wchar_t *template, va_list ap\i0 )
\par 
\par \pard\li500 This is the equivalent of \f1 fwprintf\f0  with the variable argument list specified directly as for \f1 vwprintf\f0 .
\par 
\par \pard -- Function: int \b vsprintf\b0  (\i char *s, const char *template, va_list ap\i0 )
\par 
\par \pard\li500 This is the equivalent of \f1 sprintf\f0  with the variable argument list specified directly as for \f1 vprintf\f0 .
\par 
\par \pard -- Function: int \b vswprintf\b0  (\i wchar_t *s, size_t size, const wchar_t *template, va_list ap\i0 )
\par 
\par \pard\li500 This is the equivalent of \f1 swprintf\f0  with the variable argument list specified directly as for \f1 vwprintf\f0 .
\par 
\par \pard -- Function: int \b vsnprintf\b0  (\i char *s, size_t size, const char *template, va_list ap\i0 )
\par 
\par \pard\li500 This is the equivalent of \f1 snprintf\f0  with the variable argument list specified directly as for \f1 vprintf\f0 .
\par 
\par \pard -- Function: int \b vasprintf\b0  (\i char **ptr, const char *template, va_list ap\i0 )
\par 
\par \pard\li500 The \f1 vasprintf\f0  function is the equivalent of \f1 asprintf\f0  with the variable argument list specified directly as for \f1 vprintf\f0 .
\par 
\par \pard -- Function: int \b obstack_vprintf\b0  (\i struct obstack *obstack, const char *template, va_list ap\i0 )
\par 
\par \pard\li500 The \f1 obstack_vprintf\f0  function is the equivalent of\f1  obstack_printf\f0  with the variable argument list specified directly as for \f1 vprintf\f0 .
\par 
\par \pard Here's an example showing how you might use \f1 vfprintf\f0 . This is a function that prints error messages to the stream \f1 stderr\f0 , along with a prefix indicating the name of the program (see \cf1\strike Error Messages\strike0\{linkID=670\}\cf0 , for a description of\f1  program_invocation_short_name\f0 ).
\par 
\par \f1      #include <stdio.h>
\par      #include <stdarg.h>
\par      
\par      void
\par      eprintf (const char *template, ...)
\par      \{
\par        va_list ap;
\par        extern char *program_invocation_short_name;
\par      
\par        fprintf (stderr, "%s: ", program_invocation_short_name);
\par        va_start (ap, template);
\par        vfprintf (stderr, template, ap);
\par        va_end (ap);
\par      \}
\par \f0 
\par You could call \f1 eprintf\f0  like this:
\par 
\par \f1      eprintf ("file `%s' does not exist\\n", filename);
\par \f0 
\par }
2400
PAGE_675
Variable Size Automatic
automaticfreeing;alloca;automaticstoragewithvariablesize;stdlib.h;alloca



Imported



FALSE
28
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fswiss\fcharset0 Arial;}{\f2\fmodern Courier New;}{\f3\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Variable Size Automatic \{keepn\}
\par \pard\fs18 Previous: \cf1\strike\f1 Unconstrained Allocation\cf2\strike0\{linkID=2340\}\cf0\f0 , Up: \cf1\strike Memory Allocation\strike0\{linkID=1430\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Automatic Storage with Variable Size\b0 
\par 
\par The function \f2 alloca\f0  supports a kind of half-dynamic allocation in which blocks are allocated dynamically but freed automatically. 
\par 
\par Allocating a block with \f2 alloca\f0  is an explicit action; you can allocate as many blocks as you wish, and compute the size at run time. But all the blocks are freed when you exit the function that \f2 alloca\f0  was called from, just as if they were automatic variables declared in that function. There is no way to free the space explicitly. 
\par 
\par The prototype for \f2 alloca\f0  is in \f2 stdlib.h\f0 . This function is a BSD extension.
\par 
\par -- Function: void * \b alloca\b0  (\i size_t size\i0 )\i ;\i0 
\par 
\par \pard\li500 The return value of \f2 alloca\f0  is the address of a block of \i size\i0  bytes of memory, allocated in the stack frame of the calling function.
\par 
\par \pard Do not use \f2 alloca\f0  inside the arguments of a function call--you will get unpredictable results, because the stack space for the\f2  alloca\f0  would appear on the stack in the middle of the space for the function arguments. An example of what to avoid is \f2 foo (x, alloca (4), y)\f0 .
\par 
\par \pard\li500\f3\'b7\f0  \cf1\strike Alloca Example\strike0\{linkID=60\}\cf0 : Example of using \f2 alloca\f0 .
\par \f3\'b7\f0  \cf1\strike Advantages of Alloca\strike0\{linkID=50\}\cf0 : Reasons to use \f2 alloca\f0 .
\par \f3\'b7\f0  \cf1\strike Disadvantages of Alloca\strike0\{linkID=580\}\cf0 : Reasons to avoid \f2 alloca\f0 .
\par \f3\'b7\f0  \cf1\strike C\f1 99\f0  Variable-Size Arrays\strike0\{linkID=1070\}\cf0 : Only in \f1 C99,\f0  here is an alternative method of allocating dynamically and freeing automatically.
\par 
\par }
2410
PAGE_677
Variadic Example




Imported



FALSE
44
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Variadic Example \{keepn\}
\par \pard\fs18 Previous: \cf1\strike How Variadic\strike0\{linkID=1120\}\cf0 , Up: \cf1\strike Variadic Functions\strike0\{linkID=2420\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Example of a Variadic Function\b0 
\par 
\par Here is a complete sample function that accepts a variable number of arguments. The first argument to the function is the count of remaining arguments, which are added up and the result returned. While trivial, this function is sufficient to illustrate how to use the variable arguments facility.
\par 
\par \f1      #include <stdarg.h>
\par      #include <stdio.h>
\par      
\par      int
\par      add_em_up (int count,...)
\par      \{
\par        va_list ap;
\par        int i, sum;
\par      
\par        va_start (ap, count);         /* Initialize the argument list. */
\par      
\par        sum = 0;
\par        for (i = 0; i < count; i++)
\par          sum += va_arg (ap, int);    /* Get the next argument value. */
\par      
\par        va_end (ap);                  /* Clean up. */
\par        return sum;
\par      \}
\par      
\par      int
\par      main (void)
\par      \{
\par        /* This call prints 16. */
\par        printf ("%d\\n", add_em_up (3, 5, 5, 6));
\par      
\par        /* This call prints 55. */
\par        printf ("%d\\n", add_em_up (10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10));
\par      
\par        return 0;
\par      \}
\par \f0 
\par }
2420
PAGE_678
Variadic Functions
variablenumberofarguments;variadicfunctions;optionalarguments



Imported



FALSE
21
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fmodern\fcharset0 Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Variadic Functions \{keepn\}
\par \pard\fs18 Next: \cf1\strike Null Pointer Constant\strike0\{linkID=1580\}\cf0 , Previous: \cf1\strike Consistency Checking\strike0\{linkID=410\}\cf0 , Up: \cf1\strike Language Features\strike0\{linkID=1310\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Variadic Functions\b0 
\par 
\par ISO C defines a syntax for declaring a function to take a variable number or type of arguments. (Such functions are referred to as\i  varargs functions\i0  or \i variadic functions\i0 .) However, the language itself provides no mechanism for such functions to access their non-required arguments; instead, you use the variable arguments macros defined in \f1 stdarg.h\f0 . 
\par 
\par This section describes how to declare variadic functions, how to write them, and how to call them properly. 
\par 
\par \b Compatibility Note:\b0  Many older C dialects provide a similar, but incompatible, mechanism for defining functions with variable numbers of arguments, using \f1 varargs.h\f0 .
\par 
\par \pard\li500\f2\'b7\f0  \cf1\strike Why Variadic\strike0\{linkID=2440\}\cf0 : Reasons for making functions take variable arguments.
\par \f2\'b7\f0  \cf1\strike How Variadic\strike0\{linkID=1120\}\cf0 : How to define and call variadic functions.
\par \f2\'b7\f0  \cf1\strike Variadic Example\strike0\{linkID=2410\}\cf0 : A complete example.
\par 
\par }
2430
PAGE_679
Variadic Prototypes
functionprototypes(variadic;prototypesforvariadicfunctions;variadicfunctionprototypes



Imported



FALSE
23
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Variadic Prototypes \{keepn\}
\par \pard\fs18 Next: \cf1\strike Receiving Arguments\strike0\{linkID=1810\}\cf0 , Up: \cf1\strike How Variadic\strike0\{linkID=1120\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Syntax for Variable Arguments\b0 
\par 
\par A function that accepts a variable number of arguments must be declared with a prototype that says so. You write the fixed arguments as usual, and then tack on `\f1 ...\f0 ' to indicate the possibility of additional arguments. The syntax of ISO C requires at least one fixed argument before the `\f1 ...\f0 '. For example,
\par 
\par \f1      int
\par      func (const char *a, int b, ...)
\par      \{
\par        ...
\par      \}
\par \f0 
\par defines a function \f1 func\f0  which returns an \f1 int\f0  and takes two required arguments, a \f1 const char *\f0  and an \f1 int\f0 . These are followed by any number of anonymous arguments. 
\par 
\par \b Portability note:\b0  For some C compilers, the last required argument must not be declared \f1 register\f0  in the function definition. Furthermore, this argument's type must be\i  self-promoting\i0 : that is, the default promotions must not change its type. This rules out array and function types, as well as\f1  float\f0 , \f1 char\f0  (whether signed or not) and \f1 short int\f0  (whether signed or not). This is actually an ISO C requirement.
\par 
\par }
2440
PAGE_688
Why Variadic




Imported



FALSE
23
{\rtf1\ansi\ansicpg1252\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}{\f2\fswiss\fcharset0 Arial;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 + Why Variadic \{keepn\}
\par \pard\fs18 Next: \cf1\strike How Variadic\strike0\{linkID=1120\}\cf0 , Up: \cf1\strike Variadic Functions\strike0\{linkID=2420\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Why Variadic Functions are Used\b0 
\par 
\par Ordinary C functions take a fixed number of arguments. When you define a function, you specify the data type for each argument. Every call to the function should supply the expected number of arguments, with types that can be converted to the specified ones. Thus, if the function `\f1 foo\f0 ' is declared with \f1 int foo (int, char *);\f0  then you must call it with two arguments, a number (any kind will do) and a string pointer. 
\par 
\par But some functions perform operations that can meaningfully accept an unlimited number of arguments. 
\par 
\par In some cases a function can handle any number of values by operating on all of them as a block. For example, consider a function that allocates a one-dimensional array with \f1 malloc\f0  to hold a specified set of values. This operation makes sense for any number of values, as long as the length of the array corresponds to that number. Without facilities for variable arguments, you would have to define a separate function for each possible array size. 
\par 
\par The library function \f1 printf\f0  (see \cf1\strike Formatted Output\strike0\{linkID=970\}\cf0 ) is an example of another class of function where variable arguments are useful. This function prints its arguments (which can vary in type as well as number) under the control of a format template string. 
\par 
\par These are good reasons to define a \i variadic\i0  function which can handle as many arguments as the caller chooses to pass. 
\par 
\par Some functions such as \f1 open\f0  take a fixed set of arguments, but occasionally ignore the last few. Strict adherence to ISO C requires these functions to be defined as variadic; in practice, however, th\f2 is\f0  \f2 C\f0  compiler and most other C compilers let you define such a function to take a fixed set of arguments--the most it can ever use--and then only\i  declare\i0  the function as variadic (or not declare its arguments at all!).
\par 
\par }
2450
PAGE_689
Wide Character Case Conversion
wctrans_t;wctype.h;wctrans;wctype.h;towctrans;wctype.h;towlower;wctype.h;towupper;wctype.h



Imported



FALSE
59
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Wide Character Case Conversion \{keepn\}
\par \pard\fs18 Previous: \cf1\strike Using Wide Char Classes\strike0\{linkID=2370\}\cf0 , Up: \cf1\strike Character Handling\strike0\{linkID=270\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Mapping of wide characters.\b0 
\par 
\par The classification functions are also generalized by the ISO C standard. Instead of just allowing the two standard mappings, a locale can contain others. Again, the \f1 localedef\f0  program already supports generating such locale data files.
\par 
\par -- Data Type: \b wctrans_t\b0 
\par 
\par \pard\li500 This data type is defined as a scalar type which can hold a value representing the locale-dependent character mapping. There is no way to construct such a value apart from using the return value of the\f1  wctrans\f0  function. 
\par 
\par This type is defined in \f1 wctype.h\f0 .
\par 
\par \pard -- Function: wctrans_t \b wctrans\b0  (\i const char *property\i0 )
\par 
\par \pard\li500 The \f1 wctrans\f0  function has to be used to find out whether a named mapping is defined in the current locale selected for the\f1  LC_CTYPE\f0  category. If the returned value is non-zero, you can use it afterwards in calls to \f1 towctrans\f0 . If the return value is zero no such mapping is known in the current locale. 
\par 
\par Beside locale-specific mappings there are two mappings which are guaranteed to be available in every locale: 
\par 
\par \trowd\cellx500\cellx5050\cellx9000\pard\intbl\cell\f1 "tolower"\f0  \cell\f1 "toupper"\f0\cell\row
\pard\li500 
\par These functions are declared in \f1 wctype.h\f0 .
\par 
\par \pard -- Function: wint_t \b towctrans\b0  (\i wint_t wc, wctrans_t desc\i0 )
\par 
\par \pard\li500\f1 towctrans\f0  maps the input character \i wc\i0  according to the rules of the mapping for which \i desc\i0  is a descriptor, and returns the value it finds. \i desc\i0  must be obtained by a successful call to \f1 wctrans\f0 . 
\par 
\par This function is declared in \f1 wctype.h\f0 .
\par 
\par \pard For the generally available mappings, the ISO C standard defines convenient shortcuts so that it is not necessary to call \f1 wctrans\f0  for them.
\par 
\par -- Function: wint_t \b towlower\b0  (\i wint_t wc\i0 )
\par 
\par \pard\li500 If \i wc\i0  is an upper-case letter, \f1 towlower\f0  returns the corresponding lower-case letter. If \i wc\i0  is not an upper-case letter,\i  wc\i0  is returned unchanged. 
\par 
\par \f1 towlower\f0  can be implemented using 
\par 
\par \f1           towctrans (wc, wctrans ("tolower"))
\par      
\par \f0 This function is declared in \f1 wctype.h\f0 .
\par 
\par \pard -- Function: wint_t \b towupper\b0  (\i wint_t wc\i0 )
\par 
\par \pard\li500 If \i wc\i0  is a lower-case letter, \f1 towupper\f0  returns the corresponding upper-case letter. Otherwise \i wc\i0  is returned unchanged. 
\par 
\par \f1 towupper\f0  can be implemented using 
\par 
\par \f1           towctrans (wc, wctrans ("toupper"))
\par      
\par \f0 This function is declared in \f1 wctype.h\f0 .
\par 
\par \pard The same warnings given in the last section for the use of the wide character classification functions apply here. It is not possible to simply cast a \f1 char\f0  type value to a \f1 wint_t\f0  and use it as an argument to \f1 towctrans\f0  calls.
\par 
\par }
2460
PAGE_690
Width of Type
integertypewidth;widthofintegertype;typemeasurements,integer;limits.h



Imported



FALSE
26
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fswiss\fcharset0 Arial;}{\f2\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Width of\f1  \f0 Type \{keepn\}
\par \pard\fs18 Next: \cf1\strike Range of Type\strike0\{linkID=1790\}\cf0 , Up: \cf1\strike Data Type Measurements\strike0\{linkID=510\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Computing the Width of an Integer Data Type\b0 
\par 
\par The most common reason that a program needs to know how many bits are in an integer type is for using an array of \f2 long int\f0  as a bit vector. You can access the bit at index \i n\i0  with
\par 
\par \f2      vector[\i n\i0  / LONGBITS] & (1 << (\i n\i0  % LONGBITS))
\par \f0 
\par provided you define \f2 LONGBITS\f0  as the number of bits in a\f2  long int\f0 . 
\par 
\par There is no operator in the C language that can give you the number of bits in an integer data type. But you can compute it from the macro\f2  CHAR_BIT\f0 , defined in the header file \f2 limits.h\f0 .
\par 
\par \f2 CHAR_BIT\f0 
\par \pard\li500 This is the number of bits in a \f2 char\f0 --eight, on most systems. The value has type \f2 int\f0 . 
\par 
\par You can compute the number of bits in any data type \i type\i0  like this: 
\par 
\par \f2           sizeof (\i type\i0 ) * CHAR_BIT
\par      
\par }
2470
PAGE_694
Working Directory
currentworkingdirectory;workingdirectory;changeworkingdirectory;cd;unistd.h;getcwd;getwd;get_current_dir_name;PWD;chdir;fchdir



Imported



FALSE
56
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Working Directory \{keepn\}
\par \pard\fs18 Next: \cf1\strike Deleting Files\cf2\strike0\{linkID=540\}\cf0 , Up: \cf1\strike File System Interface\strike0\{linkID=820\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Working Directory\b0 
\par 
\par Each process has associated with it a directory, called its \i current working directory\i0  or simply \i working directory\i0 , that is used in the resolution of relative file names (see \cf1\strike File Name Resolution\strike0\{linkID=750\}\cf0 ). 
\par 
\par When you log in and begin a new session, your working directory is initially set to the home directory associated with your login account in the system user database. You can find any user's home directory using the \f1 getpwuid\f0  or \f1 getpwnam\f0  functions; see \cf1\strike User Database\strike0\{link=PAGE_662\}\cf0 . 
\par 
\par Users can change the working directory using shell commands like\f1  cd\f0 . The functions described in this section are the primitives used by those commands and by other programs for examining and changing the working directory. Prototypes for these functions are declared in the header file\f1  unistd.h\f0 .
\par 
\par -- Function: char * \b getcwd\b0  (\i char *buffer, size_t size\i0 )
\par 
\par \pard\li500 The \f1 getcwd\f0  function returns an absolute file name representing the current working directory, storing it in the character array\i  buffer\i0  that you provide. The \i size\i0  argument is how you tell the system the allocation size of \i buffer\i0 . 
\par 
\par The return value is \i buffer\i0  on success and a null pointer on failure. The following \f1 errno\f0  error conditions are defined for this function:
\par 
\par \f1 EINVAL\f0 
\par \pard\li1000 The \i size\i0  argument is zero and \i buffer\i0  is not a null pointer. 
\par \pard\li500\f1 ERANGE\f0 
\par \pard\li1000 The \i size\i0  argument is less than the length of the working directory name. You need to allocate a bigger array and try again. 
\par \pard\li500\f1 EACCES\f0 
\par \pard\li1000 Permission to read or search a component of the file name was denied.
\par 
\par \pard You could implement the behavior of GNU's \f1 getcwd (NULL, 0)\f0  using only the standard behavior of \f1 getcwd\f0 :
\par 
\par \f1      char *
\par      gnu_getcwd ()
\par      \{
\par        size_t size = 100;
\par      
\par        while (1)
\par          \{
\par            char *buffer = (char *) xmalloc (size);
\par            if (getcwd (buffer, size) == buffer)
\par              return buffer;
\par            free (buffer);
\par            if (errno != ERANGE)
\par              return 0;
\par            size *= 2;
\par          \}
\par      \}
\par \f0 
\par See \cf1\strike Malloc Examples\strike0\{linkID=1380\}\cf0 , for information about \f1 xmalloc\f0 , which is not a library function but is a customary name used in most GNU software.
\par 
\par -- Function: int \b chdir\b0  (\i const char *filename\i0 )
\par 
\par \pard\li500 This function is used to set the process's working directory to\i  filename\i0 . 
\par 
\par The normal, successful return value from \f1 chdir\f0  is \f1 0\f0 . A value of \f1 -1\f0  is returned to indicate an error. The \f1 errno\f0  error conditions defined for this function are the usual file name syntax errors (see \cf1\strike File Name Errors\strike0\{linkID=740\}\cf0 ), plus \f1 ENOTDIR\f0  if the file \i filename\i0  is not a directory.
\par }
2480
PAGE_699
Yes-or-No Questions
rpmatch



Imported



FALSE
55
{\rtf1\ansi\deff0{\fonttbl{\f0\fswiss Arial;}{\f1\fmodern Courier New;}}
{\colortbl ;\red0\green128\blue0;\red128\green0\blue0;}
\viewkind4\uc1\pard\qc\lang1033\f0\fs24 Yes-or-No Questions \{keepn\}
\par \pard\fs18 Previous: \cf1\strike Locale Information\cf2\strike0\{linkID=1330\}\cf0 , Up: \cf1\strike Locales\strike0\{linkID=1340\}\cf0 
\par \fs10 
\par \pard\ri100\fs2 
\par \pard\fs10 
\par \fs18 
\par \b Yes-or-No Questions\b0 
\par 
\par Some non GUI programs ask a yes-or-no question. If the messages (especially the questions) are translated into foreign languages, be sure that you localize the answers too. It would be very bad habit to ask a question in one language and request the answer in another, often English. 
\par 
\par The C library contains \f1 rpmatch\f0  to give applications easy access to the corresponding locale definitions.
\par -- Function: int \b rpmatch\b0  (\i const char *response\i0 )
\par 
\par \pard\li500 The function \f1 rpmatch\f0  checks the string in \i response\i0  whether or not it is a correct yes-or-no answer and if yes, which one. The check uses the \f1 YESEXPR\f0  and \f1 NOEXPR\f0  data in the\f1  LC_MESSAGES\f0  category of the currently selected locale. The return value is as follows:
\par 
\par \f1 1\f0 
\par \pard\li1000 The user entered an affirmative answer. 
\par \pard\li500\f1 0\f0 
\par \pard\li1000 The user entered a negative answer. 
\par \pard\li500\f1 -1\f0 
\par \pard\li1000 The answer matched neither the \f1 YESEXPR\f0  nor the \f1 NOEXPR\f0  regular expression.
\par 
\par \pard\li500 This function is not standardized but available beside in GNU libc at least also in the IBM AIX library.
\par 
\par \pard This function would normally be used like this:
\par 
\par \f1        ...
\par        /* Use a safe default.  */
\par        _Bool doit = false;
\par      
\par        fputs (gettext ("Do you really want to do this? "), stdout);
\par        fflush (stdout);
\par        /* Prepare the getline call.  */
\par        line = NULL;
\par        len = 0;
\par        while (getline (&line, &len, stdout) >= 0)
\par          \{
\par            /* Check the response.  */
\par            int res = rpmatch (line);
\par            if (res >= 0)
\par              \{
\par                /* We got a definitive answer.  */
\par                if (res > 0)
\par                  doit = true;
\par                break;
\par              \}
\par          \}
\par        /* Free what getline allocated.  */
\par        free (line);
\par \f0 
\par Note that the loop continues until an read error is detected or until a definitive (positive or negative) answer is read.
\par 
\par }
1
main="C RTL Help", (, , , ), 52236, (255, 255, 255),(255, 255, 255), f4
0
0
302
1 The C Run-Time Library=PAGE_286
1 Welcome
2 Introduction=PAGE_305
2 Getting Started=PAGE_251
2 Standards
3 ISO C=PAGE_309
2 General Considerations
3 Header Files=PAGE_262
3 Macro Definitions=PAGE_353
3 Reserved Names=PAGE_507
2 Roadmap to the Manual=PAGE_512
1 Errors
2 Error Reporting=PAGE_173
2 Checking for Errors=PAGE_91
2 Error Codes=PAGE_170
2 Error Messages=PAGE_171
1 Memory
2 Memory=PAGE_368
2 Memory Allocation
3 Memory Allocation=PAGE_364
3 Memory Allocation and C=PAGE_363
3 C Language Allocation
4 Unconstrained Allocation=PAGE_656
4 Basic Allocation=PAGE_60
4 Malloc Examples=PAGE_356
4 Freeing after Malloc=PAGE_235
4 Changing Block Size=PAGE_85
4 Allocating Cleared Space=PAGE_19
4 Summary of Malloc=PAGE_601
3 Automatic Allocation
4 Variable Size Automatic=PAGE_675
4 Alloca Example=PAGE_18
4 Advantages of Alloca=PAGE_15
4 Disadvantages of Alloca=PAGE_155
4 Variable-Size Arrays=PAGE_254
1 Character Handling
2 Character Handling=PAGE_86
2 Classification of Characters=PAGE_94
2 Case Conversion=PAGE_84
2 Classification of Wide Characters=PAGE_95
2 Using Wide Char Classes=PAGE_669
2 Wide Character Case Conversion=PAGE_689
1 Strings
2 String and Array Utilities=PAGE_589
2 Representation of Strings=PAGE_506
2 String/Array Conventions=PAGE_595
2 String Length=PAGE_591
2 Copying and Concatenation=PAGE_126
2 String/Array Comparison=PAGE_594
2 Collation Functions=PAGE_101
2 Search Functions=PAGE_521
2 Finding Tokens in a String=PAGE_206
1 Character Sets
2 Character Set Handling=PAGE_88
2 Extended Char Intro=PAGE_184
2 Charset Function Overview=PAGE_90
2 Multibyte Conversion
3 Restartable multibyte conversion=PAGE_511
3 Selecting the Conversion=PAGE_524
3 Keeping the state=PAGE_319
3 Converting a Character=PAGE_124
3 Converting Strings=PAGE_125
3 Multibyte Conversion Example=PAGE_385
2 Non-reentrant Conversion
3 Non-reentrant Conversion=PAGE_400
3 Non-reentrant Character Conversion=PAGE_399
3 Non-reentrant String Conversion=PAGE_401
3 Shift State=PAGE_539
1 Locales
2 Locales=PAGE_341
2 Effects of Locale=PAGE_161
2 Choosing Locale=PAGE_93
2 Locale Categories=PAGE_339
2 Setting the Locale=PAGE_536
2 Standard Locales=PAGE_571
2 Basic Locale Information
3 Locale Information=PAGE_340
3 Specific Locale Information
4 The Lame Way to Locale Data=PAGE_635
4 General Numeric=PAGE_241
4 Currency Symbol=PAGE_136
4 Sign of Money Amount=PAGE_541
2 Yes-or-No Questions=PAGE_699
1 Searching And Sorting
2 Searching and Sorting=PAGE_522
2 Comparison Functions=PAGE_104
2 Array Search Function=PAGE_50
2 Array Sort Function=PAGE_51
2 Search/Sort Example=PAGE_523
1 I/0
2 I/O Overview=PAGE_313
2 I/O Concepts
3 I/O Concepts=PAGE_311
3 Streams and File Descriptors=PAGE_583
3 File Position=PAGE_199
2 Files
3 File Names=PAGE_196
3 Directories=PAGE_153
3 File Name Resolution=PAGE_195
3 File Name Errors=PAGE_193
1 Streamed I/O
2 I/O on Streams=PAGE_312
2 Streams=PAGE_586
2 Standard Streams=PAGE_573
2 Opening Streams=PAGE_421
2 Closing Streams=PAGE_100
2 Streams and I18N=PAGE_584
2 Simple Output=PAGE_557
2 Character Input=PAGE_87
2 Line Input=PAGE_329
2 Unreading
3 Unreading=PAGE_658
3 Unreading Idea=PAGE_657
3 How Unread=PAGE_277
2 Block Input/Output=PAGE_65
2 Formatted Output
3 Formatted Output=PAGE_227
3 Formatted Output Basics=PAGE_225
3 Output Conversion Syntax=PAGE_436
3 Table of Output Conversions=PAGE_622
3 Integer Conversions=PAGE_296
3 Floating-Point Conversions=PAGE_217
3 Other Output Conversions=PAGE_434
3 Formatted Output Functions=PAGE_226
3 Variable Arguments Output=PAGE_673
2 Formatted Input
3 Formatted Input=PAGE_223
3 Formatted Input Basics=PAGE_221
3 Input Conversion Syntax=PAGE_293
3 Table of Input Conversions=PAGE_621
3 Numeric Input Conversions=PAGE_412
3 String Input Conversions=PAGE_590
3 Other Input Conversions=PAGE_431
3 Formatted Input Functions=PAGE_222
3 Variable Arguments Input=PAGE_672
2 EOF and Errors=PAGE_169
2 Error Recovery=PAGE_172
2 Binary Streams=PAGE_64
2 File Positioning=PAGE_200
2 Portable Positioning=PAGE_456
2 Buffering
3 Stream Buffering=PAGE_581
3 Buffering Concepts=PAGE_75
3 Flushing Buffers=PAGE_218
3 Controlling Buffering=PAGE_121
1 Descriptor I/O
2 Low-Level I/O=PAGE_350
2 Opening and Closing Files=PAGE_420
2 I/O Primitives=PAGE_314
2 File Position Primitive=PAGE_198
2 Descriptors and Streams=PAGE_152
2 Duplicating Descriptors=PAGE_157
2 Descriptor Flags=PAGE_151
2 File Status
3 File Status Flags=PAGE_202
3 Access Modes=PAGE_5
3 Open-time Flags=PAGE_423
3 Operating Modes=PAGE_425
1 File System
2 File System Interface=PAGE_203
2 Working Directory=PAGE_694
2 Deleting Files=PAGE_148
2 Renaming Files=PAGE_504
2 Creating Directories=PAGE_132
2 File System Attributes
3 Attribute Meanings=PAGE_58
3 Reading Attributes=PAGE_491
3 Testing File Type=PAGE_630
3 Permission Bits=PAGE_450
3 File Size=PAGE_201
2 Temporary Files=PAGE_623
1 Mathematics
2 Mathematics=PAGE_362
2 Mathematical Constants=PAGE_361
2 Trig Functions=PAGE_652
2 Inverse Trig Functions=PAGE_306
2 Exponents and Logarithms=PAGE_183
2 Hyperbolic Functions=PAGE_279
2 Special Functions=PAGE_569
2 Random Numbers
3 Pseudo-Random Numbers=PAGE_484
3 ISO Random=PAGE_310
1 Arithmetic
2 Arithmetic=PAGE_49
2 Integers=PAGE_298
2 Integer Division=PAGE_297
2 Floating Point Numbers=PAGE_214
2 Floating Point Classes=PAGE_211
2 Floating Point
3 Floating Point Errors=PAGE_213
3 FP Exceptions=PAGE_232
3 Infinity and NaN=PAGE_290
3 Status bit operations=PAGE_577
3 Math Error Reporting=PAGE_360
2 Rounding=PAGE_514
2 Control Functions=PAGE_118
2 Arithmetic Functions
3 Arithmetic Functions=PAGE_48
3 Absolute Value=PAGE_2
3 Normalization Functions=PAGE_403
3 Rounding Functions=PAGE_513
3 Remainder Functions=PAGE_502
3 FP Bit Twiddling=PAGE_230
3 FP Comparison Functions=PAGE_231
3 Misc FP Arithmetic=PAGE_375
2 Complex Numbers=PAGE_105
2 Operations on Complex=PAGE_427
2 Parsing Of Numbers
3 Parsing of Numbers=PAGE_445
3 Parsing of Integers=PAGE_444
3 Parsing of Floats=PAGE_443
1 Date And Time
2 Date and Time=PAGE_144
2 Time Basics=PAGE_639
2 Elapsed Time=PAGE_163
2 CPU Time=PAGE_128
3 Calendar Time=PAGE_78
3 Simple Calendar Time=PAGE_554
3 Broken-down Time=PAGE_69
3 Formatting Calendar Time=PAGE_228
3 TZ Variable=PAGE_655
3 Time Zone Functions=PAGE_641
3 Time Functions Example=PAGE_640
2 Sleeping=PAGE_558
1 Non-Local Exits
2 Non-Local Exits=PAGE_397
2 Non-Local Intro=PAGE_398
2 Non-Local Details=PAGE_395
1 Signals
2 Signal Handling=PAGE_546
2 Concepts
3 Concepts of Signals=PAGE_108
3 Kinds of Signals=PAGE_321
3 Signal Generation=PAGE_545
2 Signal Types
3 Standard Signals=PAGE_572
3 Program Error Signals=PAGE_481
3 Operation Error Signals=PAGE_426
3 Miscellaneous Signals=PAGE_377
2 Signal Actions
3 Signal Actions=PAGE_542
3 Basic Signal Handling=PAGE_62
2 Signal Handlers
3 Defining Handlers=PAGE_146
3 Handler Returns=PAGE_259
3 Termination in Handler=PAGE_626
3 Longjmp in Handler=PAGE_346
3 Signals in Handler=PAGE_552
3 Nonreentrancy=PAGE_393
3 Atomic Data
4 Atomic Data Access=PAGE_55
4 Non-atomic Example=PAGE_394
4 Atomic Types=PAGE_56
4 Atomic Usage=PAGE_57
2 Generating Signals
3 Generating Signals=PAGE_243
3 Signaling Yourself=PAGE_551
3 Remembering a Signal=PAGE_503
1 Basics
2 Program Basics=PAGE_480
2 EMPTY
3 Program Arguments=PAGE_479
2 Environment Variables
3 Environment Variables=PAGE_167
3 Environment Access=PAGE_166
2 Program Termination
3 Program Termination=PAGE_482
3 Normal Termination=PAGE_402
3 Exit Status=PAGE_181
3 Cleanups on Exit=PAGE_97
3 Aborting a Program=PAGE_0
1 Executing Programs
2 Running a Command=PAGE_515
2 Executing a File=PAGE_180
1 Language Features
2 Language Features=PAGE_322
2 Consistency Checking=PAGE_114
2 Variadic Functions
3 Variadic Functions=PAGE_678
3 Why Variadic=PAGE_688
3 How Variadic=PAGE_278
3 Variadic Prototypes=PAGE_679
3 Receiving Arguments=PAGE_495
3 How Many Arguments=PAGE_276
3 Calling Variadics=PAGE_80
3 Argument Macros=PAGE_44
3 Variadic Example=PAGE_677
2 Null Pointer Constant=PAGE_411
2 Important Data Types=PAGE_284
2 Data Types
3 Data Type Measurements=PAGE_140
3 Width of Type=PAGE_690
3 Range of Type=PAGE_489
3 Floating Point
4 Floating Type Macros=PAGE_216
4 Floating Point Concepts=PAGE_212
4 Floating Point Parameters=PAGE_215
4 IEEE Floating Point=PAGE_282
3 Structure Measurement=PAGE_596
1 Free Manuals=PAGE_234
1 Copying=PAGE_127
1 Documentation License=PAGE_156
9

-16777208
Arial
0
9
1
B...
3
0
0
0
0
0

-16777208
Arial
0
12
1
....
2
0
0
0
0
0
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
<new macro>
-16777208
Arial
0
9
1
B...
3
0
0
0
0
0
