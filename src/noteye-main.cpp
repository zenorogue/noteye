// Necklace of the Eye v6.2
// roguelike frontend
// Copyright (C) 2010-2011 Zeno Rogue, see 'noteye.h' for details

#include "noteye.h"

using namespace noteye;

// #define HYDRA // undefine if you do not want Hydra Slayer included internally

static int gargc;
static char **gargv;

#ifdef HYDRA
#include "../hydra/hydra-noteye.cpp"
#endif

#undef main

#ifdef MAC
int SDL_main(int argc, char **argv) {
#else
int main(int argc, char **argv) {
#endif

  noteye_args(gargc=argc, gargv=argv);
  
  noteye_init();

#ifdef HYDRA
  addHydraToLua();
#endif

#ifdef HYDRAONLY  
  noteye_run("common/hydraslayer.noe", true);
#else
  noteye_run("common/noteye.noe", true);
#endif
  noteye_halt();
  return 0;
  }

