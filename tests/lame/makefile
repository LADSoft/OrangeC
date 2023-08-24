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

CFLAGS= /! /Wcl
LDFLAGS = /Wcl /pl-mx

LDLIBS = delayimp.l
test: lame.exe
	lame -h air_raid.wav q.mp3
	fc /b test.cmp q.mp3

clean:
	$(CLEAN)

lame.exe: $(FILES)
