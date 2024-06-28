// Necklace of the Eye v5.1
// roguelike frontend
// Copyright (C) 2010-2012 Zeno Rogue, see 'noteye.h' for details

#define USELUA

#include "noteye.h"
using namespace std;
using namespace noteye;

#include "util.cpp"
#include "sdltex.cpp"
#include "gfx.cpp"
#include "image.cpp"
#include "tiles.cpp"
#include "utf8.cpp"
#include "screen.cpp"
#include "fpp.cpp"
#include "iso.cpp"
#include "font.cpp"
#include "process.cpp"
#include "stream.cpp"
#include "consout.cpp"
#include "sound.cpp"
#include "lua.cpp"

int noteyeStats() {
  fprintf(logfile, "NOTEYE STATS\n");
  fprintf(logfile, "Objects allocated: %d\n", isize(objs));
  fprintf(logfile, "Hashtable collisions: %d/%d\n", hashcol, hashok);
  fprintf(logfile, "Compression stats for streams:\n");
  fprintf(logfile, "  write %9d B -> %9d B\n", writeUnc, writeCmp);
  fprintf(logfile, "  read  %9d B -> %9d B\n", readUnc, readCmp);
  
  fprintf(logfile, "Total size of images: %lld pixels\n", totalimagesize);
  fprintf(logfile, "Total size of isocache: %lld pixels\n", totalimagesize);

  return 0;
  }
  

int main(int argc, char **argv) {

  gargc = argc; gargv = argv;

  errfile = stdout;

  fprintf(errfile, "Necklace of the Eye v"VER" (C) 2010-2012 Zeno Rogue\n");
  
  objs.push_back(NULL);
  objs.push_back(&gfx); gfx.id = 1;

  initMode();
  
  initLua();
  runLua();
  closeLua();
  
  logfile && noteyeStats();

  endwin();
  halt();
  }

