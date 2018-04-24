## OCC Preprocessor File Control

 +i    create a file with preprocessed text.  
 
     For each file processed, OCC will create a file with the same name as the original source with the extension.  '.i'.  The contents of this file will be the source code, with each identifier which corresponded to a macro expanded out to its full value.  For example:
 
     OCC +i myfile.c
 
     results in myfile.i
 
  
  