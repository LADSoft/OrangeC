##OLink Target Configurations
 

  OLink has several default target configurations, that associate the various data needed for post-processing the linker output together.  Each target configuration includes a linker [specification file](OLink%20Specification%20Files.html), default definitions for items used but not declared in the specification file, and a reference to a post-processing tool that will take an image linked against the specification file and generate some final binary image, such as a ROM image or an Operating System executable.  
 
 Each target configuration is accessible via the **/T** linker switch.  For example:
 
> OLink /T:BIN test.o
 
 invokes the target configuration associated with the name BIN.  In the case of BIN the file is linked into three partitions; code, data and stack using the specification file **hex.spc**; and the results are dumped to a binary file using **DLHex**.
 
 The remainder of this section will discuss the default target configurations.


###Rom-based images

 There are several output file formats for generating a rom-based image.  However, they all use a common specification file and post-processing tool.  This section will briefly touch on the available output formats then touch on the specification file in more detail.
 
 The available output formats in this mode are:
 
* /T:M1  Motorola srecord file format, 2 byte addresses
* /T:M2  Motorola srecord file format, 3 byte addresses
* /T:M3  Motorola srecord file format, 4 byte addresses
* /T:I1    Intel hex file format, 16 bits
* /T:I2    Intel hex file format, segmented
* /T:I4    Intel hex file format, 32 bits
* /T:BIN Binary file format
  
 The default specification file for these output formats is **hex.spc**, and the default post-processing tool is **dlhex.exe**.  **Hex.spc** has 4 independent overlays for code and data.  Table 1 lists the overlays, the section names that are recognized in each overlay.  It also lists an identifier that can be used with the /D command line switch to the linker to adjust base addresses, and the default base address for each overlay.
 
    

|colheader |colheader |colheader |colheader |
|--- |--- |--- |--- |
|Overlay|Sections|Base Address Identifier|Base Address|
|RESET|reset|RESETBASE|0x00000|
|ROM|code, const, string|CODEBASE|0x00008|
|RAM|data, bss|RAMBASE|0x10000|
|STACK|stack|STACKBASE (size = STACKSIZE)|0x20000 ( 0x400)|   

  
  Table 1 - Hex.spc details


##WIN32 (PE) Images

 Several types of WIN32 images may be generated.  These include:
 
* /T:CON32 - console application
* /T:GUI32   - windowing application
* /T:DLL32   - dll application
  The default specification file for these output formats is **pe.spc**, and the default post-processing tool is dlpe.exe.  **PE.spc** has two independent overlays for code and data.  Table 2 lists the overlays, and the section names that are recognized in each overlay.  Table 3 lists the various values supported by **pe.spc** and **dlpe.exe** that may be adjusted on the linker command line.
 
 
    

|colheader |colheader |
|--- |--- |
|Overlay|Sections|
|.text|code, const|
|.data|data, string, bss|   

  
  Table 2 - PE.SPC overlays
 
    

|colheader |colheader |colheader |
|--- |--- |--- |
|Definition|Meaning|Default|
|FILEALIGN|Object Alignment within an executable file|0x200|
|HEAPCOMMIT|Amount of local heap to commit at program start|0|
|HEAPSIZE|Size of local heap|0x100000|
|IMAGEBASE|Base address for the image (used to resolve DLL Address collisions)|0x400000|
|OBJECTALIGN|Object alignment in memory|0x1000|
|STACKCOMMIT|Amount of stack to commit at program start|0x2000|
|STACKSIZE|Size of stack for default thread|0x100000|   

  Table 3 - PE.SPC adjustable parameters
 
 
 
     