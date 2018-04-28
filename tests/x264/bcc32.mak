FILES=\
    y4m.c \
    analyse.c \
    bitstream.c \
    cabac.c \
    cache.c \
    cavlc.c \
    common.c \
    cpu.c \
    crop.c \
    dct.c \
    deblock.c \
    depth.c \
    ecabac.c \
    emacroblock.c \
    encoder.c \
    eset.c \
    filters.c \
    fix_vfr_pts.c \
    flv.c \
    flv_bytestream.c \
    frame.c \
    getopt.c \
    input.c \
    internal.c \
    lookahead.c \
    macroblock.c \
    matroska.c \
    matroska_ebml.c \
    mc.c \
    me.c \
    mvpred.c \
    oraw.c \
    osdep.c \
    pixel.c \
    predict.c \
    quant.c \
    ratecontrol.c \
    raw.c \
    rectangle.c \
    resize.c \
    select_every.c \
    set.c \
    source.c \
    thread.c \
    threadpool.c \
    timecode.c \
    video.c \
    vlc.c \
    win32thread.c \
    x264.c

y4m.exe: $(FILES)
	mingw32-gcc -std=c99 @&&|
$(FILES)
|