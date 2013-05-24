c:\cc386\bin\cl386 /W32CON -e=cl386 *.c > err
rem bcc -ml -H -O1 -ecl386 -2 *.c
rem wcl -ml -fh -osx -fe=cl386w -2 *.c
rem cl386 @cl386.rsp
rem cl386a @cl386.rsp %1 %2 %3 %4 %5
copy cl386.exe ..\test