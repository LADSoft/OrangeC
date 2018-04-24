#OGrep

 
 **OGrep** is a utility that searches for text within files.  It is capable of matching text with a direct string comparison, with or without case sensitivity.  It also is capable of matching text within a file against [regular expressions](OGrep%20Regular%20Expressions.html).  Regular expressions allow a mechanism for specifying ways to match text, while specifying parts of the text which can vary and still match.  For example the '.' character matches any other character, so a regular expression such as 'a.c'  would match any three-character string starting with 'a' and ending with 'c'.  More powerful matching is also possible, such as matching sequences of the same character, matching against any character in a specified set, and so forth.


##Command Line Options

 The general format of an **OGrep** [command line](OGrep%20Command%20Line%20Options.html) is as follows:
 
> OGrep \[options\] match-string list-of-files
 
 **OGrep** will search in the _list-of-files_ for text that matches the _match-string_, and list file and optionally line number information for each match found.  In simple cases the match string does not need to be surrounded by quotes, but in more complex cases involving spacing characters and special symbols it may be necessary to quote the _match-string_.


##****Regular Expressions

 **OGrep** has a powerful [regular expression](OGrep%20Regular%20Expressions.html) matcher, which is turned on by default.  However there is a [command line](OGrep%20Command%20Line%20Options.html) option to disable it.  When it is turned on some characters will not be matched directly against the text, but will be interpreted in a way that allows the program to perform abstract types of matches.  There are several types of matching groups:
 
* Match a character or sequence of the same character
* Match the start or end of a line
* Match the start, end, or middle of a word
* Match one character out of a set of characters
* A match can be specified to be repeated at another point within the sequence
  Some of these matching algorithms can be combined, for example matching one character out of a set of characters can be combined with matching a sequence of characters to find words consisting of characters in a subset of the letters and numbers.
 
 