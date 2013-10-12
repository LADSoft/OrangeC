partition {
  overlay {
    _CODESTART = $;
	CODEBASE = $;
    region {} code [ align = 2];
    region {} vsc* [ align = 2];
	region {} const [ align = 8, roundsize = 8];
    __TLSINITSTART = $;
    region {} tls [align = 8];
    __TLSINITEND = $;
	IMPORTTHUNKS = $;
	region {} importThunks [align = 4, size = IMPORTCOUNT * 6 ];
	CODESIZE = $ - CODEBASE;
  } .text;
} pt1 [addr=IMAGEBASE + OBJECTALIGN];

partition {
  overlay {
	RAMBASE = $;
    region {} data [ align = 8, roundsize = 4];
    region {} vsd* [ align = 8, roundsize = 4];
	_INITSTART = $;
	region {} cstartup [ align = 2];
	_INITEND = $;
	_EXITSTART = $;
	region {} crundown [ align = 2];
	_EXITEND = $;
    region {} string [ align = 2];
	INITSIZE = $ - RAMBASE;
	_BSSSTART = $;
	region {} bss [ align = 8, roundsize = 4];
	_BSSEND = $;
	RAMSIZE = $ - RAMBASE;
  } .data ;
} pt2 [addr=((CODEBASE + CODESIZE + OBJECTALIGN - 1) / OBJECTALIGN) * OBJECTALIGN];
