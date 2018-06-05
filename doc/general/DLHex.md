# DLHex

 
**DLHex** is a postprocessor for obtaining hex and binary files of the type used in generatring rom-based images for embedded
systems.  It can be used indirectly as part of the link process, if the default linker settings are sufficient.  In many cases
though configuring the output for an embedded system will require a customized linker specification file.  The linker documentation 
discusses this in more detail.  If a customized specification file is used, it may be necessary to call **DLHex** directly to 
obtain an output file.