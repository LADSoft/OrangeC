CFLAGS=		/DEBUG/NOLIST
hfiles=		mconf.h-

ofiles=		ellf.obj-
		ellik.obj-
		ellpj.obj-
		ellpk.obj-
		mtherr.obj-
		cmplx.obj-
		const.obj-
		polevl.obj-

ellf.exe : $(ofiles)
	LINK  ellf/option
ellf.obj : ellf.c,$(HFILES)
    CC $(CFLAGS) ellf
ellik.obj : ellik.c,$(HFILES)
    CC $(CFLAGS) ellik
ellpj.obj : ellpj.c,$(HFILES)
    CC $(CFLAGS) ellpj
ellpk.obj : ellpk.c,$(HFILES)
    CC $(CFLAGS) ellpk
mtherr.obj : mtherr.c,$(HFILES)
    CC $(CFLAGS) mtherr
cmplx.obj : cmplx.c,$(HFILES)
    CC $(CFLAGS) cmplx
const.obj : const.c,$(HFILES)
    CC $(CFLAGS) const
polevl.obj : polevl.c,$(HFILES)
    CC $(CFLAGS) polevl
