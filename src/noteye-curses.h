// Necklace of the Eye v7.6
// roguelike frontend
// Copyright (C) 2010-2014 Zeno Rogue, see 'noteye.h' for details

// include this file instead of Curses if you want to integrate it with NotEye

#define KEY_F0 256
#define NOTEYEERR (-256) // returned if there was an error with NotEye

#define DBASE (512*6)

#define D_RIGHT (DBASE+0)
#define D_UP    (DBASE+2)
#define D_LEFT  (DBASE+4)
#define D_DOWN  (DBASE+6)
#define D_PGUP  (DBASE+1)
#define D_PGDN  (DBASE+7)
#define D_HOME  (DBASE+3)
#define D_END   (DBASE+5)
#define D_CTR   (DBASE+8)

#define KEY_MOUSE       0631            /* Mouse event has occurred */

// note: Ctrl/Shift modifiers 

#ifdef CURSES_CONSTONLY
#define _NOTEYE_CURSES_H
#endif

#ifndef _NOTEYE_CURSES_H
#define _NOTEYE_CURSES_H

#ifdef NOTEYE_TRANSLATE
#define addstr    noteye_addstr
#define addch     noteye_addch
#define refresh   noteye_refresh
#define clrtoeol  noteye_clrtoeol
#define move      noteye_move
#define erase     noteye_erase
#define endwin    noteye_endwin
#define halfdelay noteye_halfdelay
#define cbreak    noteye_cbreak
#endif

union SDL_Event;

#ifdef __cplusplus
extern "C" { namespace noteye {
#endif

  // these simply replace Curses functions
  void noteye_addch(char ch);
  void noteye_addchx(int ch); // if more than 256 chars
  void noteye_addstr(const char *buf);
  int noteye_inch();
  int noteye_mvinch(int y, int x);
  void noteye_endwin();
  void noteye_cbreak();
  void noteye_halfdelay(int i);
  void noteye_halfdelayms(int i);
  void noteye_erase();
  void noteye_move(int y, int x);
  void noteye_clrtoeol();
  void noteye_refresh();
  void noteye_curs_set(int i);  // 0-hide, 1-normal, 2-big cursor (like in Curses)
  void noteye_curs_setx(int i); // cursor size 0 to 100 (like in Windows)
  void noteye_getclick(int &x, int &y, int &button);  // get last click position and button

  // use these instead of Curses functions
  void initScreen();
  
  // set foreground color to 'fore' and background color to 'back',
  // where colors are numbered 0-15, just like in DOS
  void setTextAttr(int fore, int back);

  // in this variant, colors are given like 0xFRRGGBB, where
  // RRGGBB is TrueColor value of the color, and F is the DOS
  // equivalent used on console
  void setTextAttr32(int fore, int back);

  int noteye_getch();
  // getchev will also report non-printable keypresses and key releases,
  // as 0
  int noteye_getchev();
  // report the full information about the last character seen by
  // getch or getchev, as SDL_Event
  union SDL_Event *noteye_getlastkeyevent();

  int noteye_lastkeyevent_type();
  int noteye_lastkeyevent_symbol();
  int noteye_lastkeyevent_mods();
  int noteye_lastkeyevent_chr();

  void noteye_mvaddch(int y, int x, char ch);
  void noteye_mvaddchx(int y, int x, int ch);
  void noteye_mvaddstr(int y, int x, const char *buf);

  // translate the DOS color index to FRRGGBB
  int getVGAcolor(int c);

#ifdef __cplusplus
  }}

#include <stdio.h>

namespace noteyetranslate {
  inline void move(int y, int x) { noteye::noteye_move(y,x); }
  inline void erase() { noteye::noteye_erase(); }
  inline void addch(char ch) { noteye::noteye_addch(ch); }
  inline void addchx(int ch) { noteye::noteye_addchx(ch); }
  inline void addstr(const char *buf) { noteye::noteye_addstr(buf); }
  inline void refresh() { noteye::noteye_refresh(); }
  inline void clrtoeol() { noteye::noteye_clrtoeol(); }
  inline void endwin() { noteye::noteye_endwin(); }
  inline void cbreak() { noteye::noteye_cbreak(); }
  inline void halfdelay(int i) { noteye::noteye_halfdelay(i); }
  }

#endif

#define NOCURSES
#endif
