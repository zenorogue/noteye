sample: noteye.o sample.o
	dmd -ofsample -L-rpath=../.. -L-L../.. -L-lnoteye noteye.o sample.o

noteye.o: noteye.d
	dmd -c -ofnoteye.o noteye.d

sample.o: sample.d
	dmd -c -ofsample.o sample.d
