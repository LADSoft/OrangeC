partition {
  overlay {
    _CODESTART = $;
	CODEBASE = $;
    region {} code [ align = 2];
    region {} text [ align = 2];
    region {} vsc* [ align = 2];
    __TLSINITSTART = $;
    region {} tls [align = 8, roundsize = 4];
    __TLSINITEND = $;
    ____imageBase = $;
    region {} IMGBASE [align = 4, size = 4];
    IMPORTTHUNKS = $;
    region {} importThunks [align = 4, size = IMPORTCOUNT * 6 ];
    region {} delayLoadThunks [align = 4, size = DELAYLOADTHUNKSIZE];
    _CODEEND = $;
	CODESIZE = $ - CODEBASE;
  } .text;
} pt1 [addr=IMAGEBASE + OBJECTALIGN, fill = 0x90];

partition {
  overlay {
    RAMBASE = $;
    region {} const [ align = 8, roundsize = 8];
    region {} data [ align = 8, roundsize = 4];
    region {} vsd* [ align = 8, roundsize = 4];
    _INITSTART = $;
    region {} cstartup [ align = 2];
    _INITEND = $;
    _EXITSTART = $;
    region {} crundown [ align = 2];
    _EXITEND = $;
    _TLSINITSTART = $;
    region {} tstartup [ align = 2];
    _TLSINITEND = $;
    _TLSEXITSTART = $;
    region {} trundown [ align = 2];
    _TLSEXITEND = $;
    region {} string [ align = 2];
    INITSIZE = $ - RAMBASE;
    _BSSSTART = $;
    region {} bss [ align = 8, roundsize = 4];
    region {} vsb* [ align = 8];
    region {} delayLoadHandles [align = 4, size = DELAYLOADHANDLESIZE];
    _BSSEND = $;
    RAMSIZE = $ - RAMBASE;
  } .data ;
} pt2 [addr=((CODEBASE + CODESIZE + OBJECTALIGN - 1) / OBJECTALIGN) * OBJECTALIGN];
