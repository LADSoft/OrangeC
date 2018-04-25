## OGrep Command Line Options

>     
 The general format of an **OGrep** command line is as follows:
 
 OGrep \[options\] match-string list-of-files
 
 Where \[options\] are command line options, and the _match-string_ is searched for within the _list-of-files_.  Matches are listed with file and optionally line number.  The files in the _list-of-files_ may contain wildcards, for example:
 
>     OGrep "while" *.c
 
 Looks through all C language source files in the current directory for the word **while**.
 
 By default the match-string is assumed to hold a [regular expression](OGrep%20Regular%20Expressions.md).
 
 This page will discuss the various command-line options in detail.


### Getting Help

 By default, if an incomplete command line is specified (for example the match string and files are not listed) **OGrep** will display a one-line version of usage text to prompt you.  However, **OGrep** also has a more detailed help-screen.  To access it, you use the **-?**  command line switch.


### Case Sensitivity

 By default, OGrep does case sensitive text matching.  However, with the command line option **-i**, it will switch to doing case-insensitive comparisons.


### Disabling regular expressions

 To match a string without interpreting the _match-string_ as a [regular-expression](OGrep%20Regular%20Expressions.md), use the **-r-**  switch.


### Recursing through subdirectories

 In some cases it can be useful to search through some subset of files within a directory tree.  The **-d** switch can be used to specify that **OGrep** should search through the current directory and all subdirectories when trying to find files to match against.  For example:
 
>     OGrep -d "while" *.c
 
 searches for the word **while** in all files ending in **.c**, in the current directory as well as all its subdirectories.


### Matching complete words

 **OGrep**'s [regular expression](OGrep%20Regular%20Expressions.md) matching can be used to match complete words.  For example by default the regular expression 'abc'  would match within both 'abc' and 'xabcy'.  There are regular expression modifiers that can be used to make it match only 'abc' since in the other case abc occurs within another word.  With the **-w** command line switch, **OGrep** automatically takes the match string and makes it into this type of regular expression.  E.g, when the **-w** switch is used **OGrep** will only match complete words that don't occur within other words.  This facility may be used even when regular expressions are turned off with the **-r-** switch.


### Controlling the output

 By default OGrep will list each filename when it finds the first match within a file, then list each matching line underneath it.  At the end it will show a count of the number of matches.  But there are various command line options which can modify the format of the output.
 
* **-c**  list the file names matched, along with a count of matches
   
* **-l ** list only the file names for files that have matches
* **-n** list the line number of matching lines next to the matching line
* **-o** (Unix format) list the file name and line number to the left of each matching line, instead of showing the file names separately.
* **-v** lines nonmatching lines instead of matching lines
* **-z** list the file names matched, line numbers, and matched lines
   
  
 
