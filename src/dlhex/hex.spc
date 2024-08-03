partition {
  overlay {
    region {} reset [ size = RESETSIZE];
  } RESET;
} pt0 [addr = RESETBASE];
partition {
  overlay {
    region {} code [ align = 2];
        region {} vsc* [ align = 8, roundsize = 4];
	region {} const [ align = 4];
    region {} string [ align = 2];
  } ROM;
} pt1 [addr=CODEBASE];

partition {
  overlay {
    RAMDATA = $;
    region {} data [ align = 4];
        region {} vsd* [ align = 8, roundsize = 4];
	region {} bss [ align = 4];
        region {} vsb* [ align = 8, roundsize = 4];
  } RAM ;
} pt2 [addr=RAMBASE];

partition {
  overlay {
    region {} stack[size = STACKSIZE];
    STACKPOINTER = $;
  } STACK;
} pt3 [addr = STACKBASE];
