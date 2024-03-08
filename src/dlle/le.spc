partition {
  overlay {
	_CODESTART = $;
	CODEBASE = $;
    region {} code [ align = 2];
    region {} text [ align = 2];
    region {} vsc* [ align = 2];
	region {} const [ align = 8, roundsize = 4];
    _TLSINITSTART = $;
    region {} tls [align = 4, roundsize = 4];
    _TLSINITEND = $;
	CODESIZE = $ - CODEBASE;
  } .text;
} pt1;

partition {
  overlay {
	RAMBASE = $;
    region {} data [ align = 8, roundsize = 4];
    region {} vsd* [ align = 8, roundsize = 4];
	_INITSTART = $;
	region {} cstartup [ align = 2];
        region {} vss* [ align = 2];
	_INITEND = $;
	_EXITSTART = $;
	region {} crundown [ align = 2];
        region {} vsr* [ align = 2];
	_EXITEND = $;
    region {} string [ align = 2];
	RAMSIZE = $ - RAMBASE;
  } .data ;
} pt2 [addr=((CODEBASE + CODESIZE + 4096 - 1) / 4096) * 4096];

partition {
  overlay {
	_BSSSTART = $;
	region {} bss [ align = 8, roundsize = 4];
	_BSSEND = $;
	_BSSSIZE = _BSSEND - _BSSSTART;
  } .bss ;
} pt3 [addr=((RAMBASE + RAMSIZE + 4096 - 1) / 4096) * 4096];

partition {
  overlay {
	region {} stack [ size = STACKSIZE, roundsize = 4096 ];
	STACKTOP = $;
  } .stack ;
} pt4 [addr=((_BSSSTART + _BSSSIZE + 4096 - 1) / 4096) * 4096];
