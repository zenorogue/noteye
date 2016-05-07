all: ../noteye

#for development:
#CFLAGS=-Wall -Werror -g
#for release:
CFLAGS=-O9 -Wall

TGT=
#TGT=-m32

# note: for Raspberry Pi (and possibly others) add -DRASPBERRY_FIX
DEF=-DHYDRA -DLINUX -DOPENGL -O9

#noteye: noteye.cpp noteye.h util.cpp screen.cpp tiles.cpp gfx.cpp linux.cpp image.cpp fpp.cpp font.cpp internal.cpp stream.cpp
#	g++ noteye.cpp -o noteye -lSDL -lSDL_image -llua5.1 -lutil -DLINUX -lSDL_net -lz

libnoteye.o: libnoteye.cpp noteye.h util.cpp screen.cpp tiles.cpp gfx.cpp linux.cpp image.cpp fpp.cpp font.cpp internal.cpp stream.cpp consout.cpp sound.cpp lua.cpp iso.cpp noteye-curses.h opengl.cpp drawtile.cpp sdltex.cpp
	g++ -fpic -I/usr/include/lua5.1 libnoteye.cpp -c -o libnoteye.o $(CFLAGS) $(DEF) $(TGT)

noteye-jni.o: noteye-jni.cpp noteye.h
	g++ -fpic -I. noteye-jni.cpp -c -o noteye-jni.o $(CFLAGS) $(DEF)

../libnoteye.so: libnoteye.o noteye-jni.o
	g++ -shared -Wl,-soname,libnoteye.so -o ../libnoteye.so libnoteye.o noteye-jni.o -lSDL2 -lSDL2_image -llua5.1 -lutil -lSDL2_mixer -lSDL2_net -lGL -lGLU $(TGT) -lz -lcurses -lSDL2_ttf

hydra.o: noteye-curses.h ../hydra/classes.cpp ../hydra/hydra.cpp ../hydra/mainmenu.cpp ../hydra/ui.cpp ../hydra/tutorial.cpp ../hydra/weapons.cpp ../hydra/data.cpp ../hydra/save.cpp ../hydra/achieve.cpp ../hydra/utils.cpp ../hydra/level.cpp ../hydra/monster.cpp ../hydra/drawhydras.cpp
	g++ ../hydra/hydra.cpp -c -o hydra.o -DNOTEYE $(CFLAGS) $(TGT) $(DEF)

../noteye: ../libnoteye.so hydra.o noteye-main.cpp ../hydra/hydra-noteye.cpp
	g++ $(DEF) -Wl,-rpath,. -Wl,-rpath,/usr/share/noteye hydra.o noteye-main.cpp -o ../noteye ../libnoteye.so -lGL -lGLU $(TGT)

#../noteye: ../libnoteye.so noteye-main.cpp noteye.h
#	g++ -Wl,-rpath,. noteye-main.cpp -o ../noteye ../libnoteye.so

DESTDIR = /usr/share/noteye

install: ../noteye
	strip ../noteye
	install -T -D -m 755 ../noteye "$(DESTDIR)/noteye"
	install -T -D -m 755 ../libnoteye.so "$(DESTDIR)/libnoteye.so"
	install -D -m 755 calleri.sh "$(DESTDIR)/caller.sh"
	install -D -m 755 rogue.sh "$(DESTDIR)/rogue"
	mkdir -p $(DESTDIR)/gfx
	mkdir -p $(DESTDIR)/gfx/dawnlike
	mkdir -p $(DESTDIR)/gfx/dawnlike/Items
	mkdir -p $(DESTDIR)/gfx/dawnlike/Objects
	mkdir -p $(DESTDIR)/gfx/dawnlike/Characters
	mkdir -p $(DESTDIR)/gfx/dawnlike/GUI
	mkdir -p $(DESTDIR)/sound
	mkdir -p $(DESTDIR)/sound/hydra-old
	mkdir -p $(DESTDIR)/sound/hydra-new
	mkdir -p $(DESTDIR)/sound/hydra-new/other
	mkdir -p $(DESTDIR)/sound/hydra-new/elements
	mkdir -p $(DESTDIR)/sound/hydra-new/hydras
	mkdir -p $(DESTDIR)/sound/hydra-new/weapons
	mkdir -p $(DESTDIR)/common
	mkdir -p $(DESTDIR)/games
	ln -s /usr/share/noteye/libnoteye.so /usr/lib/libnoteye.so
	bash -c "cd ../gfx; install -D -m 744 -t \"$(DESTDIR)/gfx/\" *.png *.jpeg *.ttf *.otf"
	bash -c "cd ../gfx/dawnlike; install -D -m 744 -t \"$(DESTDIR)/gfx/dawnlike/\" README.txt"
	bash -c "cd ../gfx/dawnlike/Items/; install -D -m 744 -t \"$(DESTDIR)/gfx/dawnlike/Items/\" *.png"
	bash -c "cd ../gfx/dawnlike/Objects/; install -D -m 744 -t \"$(DESTDIR)/gfx/dawnlike/Objects/\" *.png"
	bash -c "cd ../gfx/dawnlike/Characters/; install -D -m 744 -t \"$(DESTDIR)/gfx/dawnlike/Characters/\" *.png"
	bash -c "cd ../gfx/dawnlike/GUI/; install -D -m 744 -t \"$(DESTDIR)/gfx/dawnlike/GUI/\" *.png"
	#bash -c "cd ../sound; install -D -m 744 -t \"$(DESTDIR)/sound/\" *.ogg"
	bash -c "cd ../sound/hydra-old; install -D -m 744 -t \"$(DESTDIR)/sound/hydra-old/\" *.ogg"
	bash -c "cd ../sound/hydra-new; install -D -m 744 -t \"$(DESTDIR)/sound/hydra-new/\" *.ogg"
	bash -c "cd ../sound/hydra-new/other; install -D -m 744 -t \"$(DESTDIR)/sound/hydra-new/other/\" *.ogg"
	bash -c "cd ../sound/hydra-new/elements; install -D -m 744 -t \"$(DESTDIR)/sound/hydra-new/elements/\" *.ogg"
	bash -c "cd ../sound/hydra-new/hydras; install -D -m 744 -t \"$(DESTDIR)/sound/hydra-new/hydras/\" *.ogg"
	bash -c "cd ../sound/hydra-new/weapons; install -D -m 744 -t \"$(DESTDIR)/sound/hydra-new/weapons/\" *.ogg"
	bash -c "cd ..; install -D -m 744 -t \"$(DESTDIR)/common\" common/*.noe"
	bash -c "cd ..; install -D -m 744 -t \"$(DESTDIR)/games\" games/*.noe"
	touch "$(DESTDIR)/hydrascores.sav"
	chmod 666 "$(DESTDIR)/hydrascores.sav"
	install -T -D -m 755 hydraslayer.sh "/usr/bin/hydraslayer"
	install -T -D -m 755 noteye.sh "/usr/bin/noteye"

uninstall:
	rm -rf /usr/share/noteye /usr/bin/noteye /usr/bin/hydraslayer

clean:
	rm *.o

# note about the install script:
# unfortunately I cannot set the group ID to games since it would cause a security hole (NotEye is scriptable)
# I don't know how to solve this (other than creating a separate hydra slayer executable), thus I simply make 
# the scores writeable by anyone

# uninstall will remove the global highscores!

