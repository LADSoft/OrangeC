all: alltests

%.tst: %.nas
	oasm --gas /b -o$*.tst $^
	fc /b $*.tst $*.cmpx

alltests: gastest.tst gasdir.tst gassect.tst