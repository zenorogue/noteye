all: sample

sample: sample.c ../../libnoteye.so ../../src/noteye.h ../../src/noteye-curses.h
	gcc -Wl,-rpath,../.. sample.c -o ./sample ../../libnoteye.so

sample0: sample0.c ../../libnoteye.so ../../src/noteye.h ../../src/noteye-curses.h
	gcc -Wl,-rpath,../.. sample0.c -o ./sample0 ../../libnoteye.so
