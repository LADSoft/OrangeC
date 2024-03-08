FILES=\
    analyse.o \
    bitstream.o \
    cabac.o \
    cache.o \
    cavlc.o \
    common.o \
    cpu.o \
    crop.o \
    dct.o \
    deblock.o \
    depth.o \
    ecabac.o \
    emacroblock.o \
    encoder.o \
    eset.o \
    filters.o \
    fix_vfr_pts.o \
    flv.o \
    flv_bytestream.o \
    frame.o \
    getopt.o \
    input.o \
    internal.o \
    lookahead.o \
    macroblock.o \
    matroska.o \
    matroska_ebml.o \
    mc.o \
    me.o \
    mvpred.o \
    oraw.o \
    osdep.o \
    pixel.o \
    predict.o \
    quant.o \
    ratecontrol.o \
    raw.o \
    rectangle.o \
    resize.o \
    select_every.o \
    set.o \
    source.o \
    thread.o \
    threadpool.o \
    timecode.o \
    video.o \
    vlc.o \
    win32thread.o \
    x264.o \
    y4m.o \

%.o: %.c
	occ /c /9 /! -o$@ $^

test: x264.exe
	x264 claire_qcif.y4m -o q.mkv 2> test.out
#	fc /b test.cmpx q.mkv # seems like different cpus might have slight variations in how floating point is done?

x264.exe: $(FILES)
	olink /! /T:CON32 /c /mx /o$@ @&&|
c0xpe.o $(FILES) clwin.l climp.l
|

clean:
	$(CLEAN)

