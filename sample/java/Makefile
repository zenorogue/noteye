all: allexec

allexec: com/roguetemple/noteye/Noteye.class Sample.class com_roguetemple_noteye_Noteye.h

#libNoteye.so: Noteye.cpp com_roguetemple_noteye_Noteye.h
#	g++ -I /usr/include/classpath -shared -fPIC -o libNoteye.so Noteye.cpp

com_roguetemple_noteye_Noteye.h: com/roguetemple/noteye/Noteye.class
	javah -jni com.roguetemple.noteye.Noteye
	cp *Noteye.h ../../src/noteye-jni.h

com/roguetemple/noteye/Noteye.class: com/roguetemple/noteye/Noteye.java
	javac com/roguetemple/noteye/Noteye.java

Sample.class: Sample.java
	javac Sample.java

run: allexec
	LD_LIBRARY_PATH=`pwd`/../.. java Sample
