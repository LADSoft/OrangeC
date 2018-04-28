FILES =\
    bitstream.o \
    common.o \
    dct64_i386.o \
    decode_i386.o \
    encoder.o \
    fft.o \
    get_audio.o \
    id3tag.o \
    interface.o \
    lame.o \
    lame1.o \
    lametime.o \
    layer1.o \
    layer2.o \
    layer3.o \
    mpglib_interface.o \
    newmdct.o \
    parse.o \
    pcm.o \
    portableio.o \
    psymodel.o \
    quantize.o \
    quantize_pvt.o \
    reservoir.o \
    set_get.o \
    tabinit.o \
    tables.o \
    takehiro.o \
    timestatus.o \
    tools.o \
    util.o \
    vbrquantize.o \
    VbrTag.o \
    version.o \
    vorbis_interface.o

%.o: %.c
	occ /c -o$@ $^

test: lame.exe
	lame -h air_raid.wav q.mp3
	fc /b q.cmp q.mp3

lame.exe: $(FILES)
	olink /c /T:CON32 /o$@ @&&|
c0xpe.o $(FILES) clwin.l climp.l
|