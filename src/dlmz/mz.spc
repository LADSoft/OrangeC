partition {
  overlay {
	CODEBASE = $;
    region {} code* [ align = 2];
	region {} const* [ align = 2];
	region {} * & !(code* | far* | const* | data* | bss* | string* | cstartup | crundown | stack*) [align = 2 ];
	CODESIZE = CODEBASE - $;
  } .text;
} pt1;

region {} farcode* [ align = 2 ];
region {} far* & !farcode* [ align = 2 ];
partition {
  overlay {
	RAMBASE = $;
    region {} data* [ align = 2];
	INITSTART = $;
	region {} cstartup;
	INITEND = $;
	EXITSTART = $;
	region {} crundown;
	EXITEND = $;
    region {} string* [ align = 2];
	BSSSTART = $;
	region {} bss* [ align = 2];
	BSSEND = $;
	BSSSIZE = BSSEND - BSSSTART;
	RAMSIZE = $ - RAMBASE;
  } .data ;
} pt2;

partition {
  overlay {
	region {} stack [ size = STACKSIZE, align = 16];
	STACKTOP = $;
  } .stack ;
} pt3;
