// Necklace of the Eye v6.2
// roguelike frontend
// Copyright (C) 2010-2012 Zeno Rogue, see 'noteye.h' for details

#define USELUA

#include "noteye.h"

#ifdef USELUA
extern "C" {
  #include <lua.h>
  #include <lualib.h>
  #include <lauxlib.h>
  }
#endif

using namespace std;

#include "util.cpp"
#include "opengl.cpp"
#include "sdltex.cpp"
#include "gfx.cpp"
#include "image.cpp"
#include "tiles.cpp"
#include "drawtile.cpp"
#include "screen.cpp"
#include "fpp.cpp"
#include "iso.cpp"
#include "utf8.cpp"
#include "font.cpp"
#include "process.cpp"
#include "stream.cpp"
#ifndef NOCONSOUT
#include "consout.cpp"
#endif
#include "sound.cpp"
#include "textinput.cpp"
#include "lua.cpp"

using namespace noteye;

extern "C" {

void noteye_init() {
  errfile = stdout;
  initMode();  
  initMappings();
  initLua();
  }

void noteye_run(const char *noemain, bool applyenv) {
  
  string runfile = noemain;
  
  if(applyenv && getenv("NOTEYEDIR"))
    runfile = getenv("NOTEYEDIR") + ("/" + runfile);
  
  if(luaL_dofile(LS, runfile.c_str()))
    noteyeError(21, "dofile", lua_tostring(LS, -1));
  }

void noteye_handleerror(noteyehandler h) {
  noteyeErrorHandler = h;
  }

void noteye_halt() {
  closeLua();
  eventobjs.clear();
  object_to_handle.clear();
  handle_to_object.clear();  
  printf("after closing:\n");
  noteye_gc();
  if(logfile) {
    fprintf(logfile, "%s", noteyeStats());
    fclose(logfile);
    logfile = NULL;
    }
#ifndef NOAUDIO
  extern void closeAudio();
  closeAudio();
#endif
  initJoysticks(false);
  SDL_Quit();
  SDL_FreeSurface(exsurface); exsurface = NULL;
  P = NULL;
  }

}
