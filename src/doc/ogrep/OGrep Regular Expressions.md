****


##OGrep Regular Expressions

 Regular expressions can be used as expressions within match-strings.  They allow a more general mechanism for doing pattern-matches than having to specify each character specifically.  For example, the '.' matches any character, so using the sequence 'a.c' would match any three character sequence starting with 'a' and ending with 'c'.  This page discusses the various pattern matching mechanisms available when regular expression matching is enabled.


###Quoting

 
 Because the pattern matching function of regular expressions uses characters to specify patterns, those characters can not be matched directly.  For example '.' matches any character, but there may be instances when you want it to match only a period.  To resolve this, the pattern matching symbols may be quoted by preceding them with the '\\' character.  This means that the pattern 'a\\.b' matches only the sequence a.b.
 
 Since the quote character generally means quote the next character, trying to match a quote character means the quote character itself has to be quoted.  For example to match the text 'a\\b' one one have to write the pattern 'a\\\\b'.
 
 The quote character is sometimes used to extend the working of the pattern matcher, for example the sequence \\b does not mean a 'b' character is being quoted, it means match the beginning or ending of a word.
 
 **
 **


###Basic Pattern matching

 The following symbols match various types of patterns:
 
* '.'  match any character
* '\*' match zero or more occurrances of the preceding character
* '+' match one or more occurrances of the preceding character
* '?' match zero or one occurrances of the preceding character
* '^' match the start of a line
* '$' match the end of a line
* '\\b' match the beginning or ending of a word
* '\\B' match within a word
* '\\w' match the beginning of a word
* '\\W' match the ending of a word
* '\\<' match the beginning, ending, or inside a word
* '\\>' match anything other than the beginning, ending, or inside a word


###Range Matching

 Some of the basic pattern matching such as '+' can match multiple occurances of a character.  Range Matching is a more general statement on the number of occurances of a character, for example you can match a bounded range, say from two to four 'a' characters by doing the following:
 
> OGrep "a\\{2,4\\}"  \*.c
 
>


###Matching set members

 Brackets \[\] can be used to delimit a set of characters.  Then the bracketed sequence will match any character in the set.  For example
 
> OGrep "\[abc\]" \*.c
 
 matches any of the characters a,b,c.  A range of characters can be specified:
 
> OGrep "\[a-m\]"  \*.c
 
 matches any characters in the range a-m.
 
 Set negation is possible:
 
> OGrep "\[^a-z\]" \*.c
 
 matches anything but a lowercase letter.
 
 Sets can be more complex:
 
> OGrep "\[A-Za-z0-9\]" \*.c
 
 matches any alphanumeric value.
 
 Sets can be combined with more basic pattern matching:
 
> OGrep "\[A-Z\]?" \*.c
 
  matches zero or one upper case characters.
 
> Ogrep "\[0-9\]\\{2,4\\}"
 
 matches from two to four digits.


###Region matching
 

 Sometimes, it is desirable to match the same sub-pattern multiple times before having grep declare the pattern as a match for the text.  In a simple case:
 
> OGrep "\\(myword\\)|\\0" \*.c
 
 matches the string:
 
> myword|myword
 
 In this case the quoted parenthesis surround the region to match, and the \\0 says match that region again.  This is not a very interesting case, but when combined with other pattern matching it becomes more powerful, because the \\0 doesn't reapply the pattern but instead matches exactly the same pattern as before.  For example to combine it with a set:
 
 OGrep "\\(\[a-z\]\\)||\\0" \*.c
 
 matches any lower-case word twice, as long as it is separated from itself by two | characters.  This pattern would match 'ab||ab' but not 'ab||xy'.
 
 Up to ten such regions may be specified in the pattern; to access them use \\0 \\1 \\2 ... \\9  where the digit gives the order the region is encountered within the pattern.
 
  