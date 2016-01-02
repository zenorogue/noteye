// Hydra Slayer: math puzzle roguelike
// Copyright (C) 2010-2011 Zeno Rogue, see 'hydra.cpp' for details

#ifdef MINGW
#undef getch
#include <conio.h>
#endif

void addMessage(string s) { msgs.push_back(s); }

bool yesno(int context) {
  while(true) {
    char ch = ghch(context); 
    if(ch == 'y' || ch == '\n' || ch == '\r') return true;
    if(ch == 'n' || ch == ' ' || ch == ESC || ch == PANIC) return false;
    }
  }

#ifndef NOTEYE
void playSound(const char *fname, int vol, int msToWait) {}
#endif

#ifdef NOTEYE
int ghch(int context) { return noteye_getch(); }

struct soundtoplay {
  int vol, delay;
  const char *sfname;
  };

vector<soundtoplay> soundqueue;

void playSound(const char *fname, int vol, int msToWait) {
  if(!fname) return;
  soundtoplay P;
  P.vol = vol; P.delay = msToWait; P.sfname = fname;
  soundqueue.push_back(P);
  }

#undef erase
#include "../src/noteye.h"
#define erase noteye_erase

int lh_getSounds(lua_State *L) {
  noteye_table_new(L);
  for(int i=0; i<size(soundqueue); i++) {
    soundtoplay& P(soundqueue[i]);
    noteye::noteye_table_opensubAtInt(L, i);
    noteye_table_setStr(L, "sound", P.sfname);
    noteye_table_setInt(L, "vol", P.vol);
    noteye_table_setInt(L, "delay", P.delay);
    noteye_table_closesub(L);
    }
  soundqueue.clear();
  return 1;
  }

#endif

#ifndef NOCURSES
int ghch(int context) {

  refresh();
  int ch=getch();
  
  // thanks to Xecutor on Roguetemple for this hack
  // it allows you to use Alt+keypad for movement on Macs

#ifdef MINGW
  if(ch == 224 || ch == 0) {
    ch = getch();
    if(ch == 'G') return D_HOME;
    if(ch == 'H') return D_UP;
    if(ch == 'I') return D_PGUP;
    if(ch == 'K') return D_LEFT;
    if(ch == 'M') return D_RIGHT;
    if(ch == 'O') return D_END;
    if(ch == 'P') return D_DOWN;
    if(ch == 'Q') return D_PGDN;

/*    return -1;
    }

  if(ch == 0) {
    ch=getch(); */
    if(ch == ';') return KEY_F0+1;
    return -1;
    }
  return ch;

#else
  if(ch==0x1b) {
    ch=getch();
    switch(ch) {
      case '1': return D_END;
      case '2': return D_DOWN;
      case '3': return D_PGDN;
      case '4': return D_LEFT;
      case '6': return D_RIGHT;
      case '7': return D_HOME;
      case '8': return D_UP;
      case '9': return D_PGUP;
      case '5': return D_CTR;

      // also Home and End don't work on some computers without
      // the following
      case 91: { 
        int c = getch();
        if(c == 49) ch = D_HOME;
        if(c == 52) ch = D_END;
        getch();
        return ch;
        }
      }
    return 0x1b;
    }  
  
  switch(ch) {
      case KEY_RIGHT:
#ifdef KEY_B3
      case KEY_B3:
#endif
        return D_RIGHT;
      
      case KEY_A3:
      case KEY_PPAGE:
        return  D_PGUP;
      
      case KEY_UP:
#ifdef KEY_A2
      case KEY_A2:
#endif
        return D_UP;
      
      case KEY_HOME:
      case KEY_A1:
        return D_HOME;
      
      case KEY_LEFT:
#ifdef KEY_B1
      case KEY_B1:
#endif           
        return D_LEFT;
      
      case KEY_END:
      case KEY_C1:
        return D_END;
      
      case KEY_DOWN:
#ifdef KEY_C2
      case KEY_C2:
#endif
        return D_DOWN;
        break;
      
      case KEY_NPAGE:
      case KEY_C3:
        return D_PGDN;
        break;
      
      case KEY_B2:
        return D_CTR;

      default:
        return ch;
      }
#endif
    
  }
#endif

const char *getLayoutName() {
  // using the same order as in the getDir function
  // would hide the meaning of WASD...
  return
    P.geometry == 16 ? "QWERASDF" :
    DIRS == 4 ? "WASD" :
    DIRS == 6 ? "WEADZX" : "QWEADZXC";
  }

int getDir(int ch) {

  if(ch == '.' || ch == D_CTR || ch == ' ') return -2;
  
  int dir = -1;
  
  if(ch >= INDB && ch <= INDB+15) return ch-INDB;
  
  if(ch >= DBASE && ch <= DBASE+7) dir = ch-DBASE;
  else {
    if(P.altkeys) {
      const char *str = 
        P.geometry == 16 ? "rewqasdf" :
        DIRS == 4 ? "dwas" :
        DIRS == 6 ? "dewazx" : "dewqazxc";
      for(int i=0; str[i]; i++) if(str[i] == ch) return i;
      if(ch == 's') return -2;
      }
    else {
      const char *str = "lukyhbjn";
      for(int i=0; i<8; i++) if(str[i] == ch) dir = i;
      if(DIRS == 6 && dir == 2) dir = 0;
      if(DIRS == 6 && dir == 6) dir = 0;
      if(DIRS == 4 && (dir&1) && dir>0) { dir = (dir/4)*4+2; }
      }
    }
  
  if(DIRS == 8) return dir;
  if(DIRS == 4) {
    if(dir < 0) return dir;
    if(dir&1) return -1;
    return dir/2;
    }
  if(DIRS == 6) {
    if(dir<0) return dir;
    int xlat[8] = {0, 1, -1, 2, 3, 4, -1, 5};
    return xlat[dir];
    }
  // impossible
  fprintf(stderr, "wrong directions\n");
  exit(1);
  }

void viewMultiLine(string s, int& cy) {
  s = s + " ";
  int cut = -1;

#ifdef ANDROID
#define MAXLINE 39
#else
#define MAXLINE 79
#endif

  int cx = 0;

  while(s != "") {
    move(cy, cx);
    if(s[0] == '\b') { col(s[1] - 'a' + 1); s = s.substr(2); }
    
    cut = size(s);

    for(int x=0; x<MAXLINE && x < size(s); x++) 
      if(s[x] == '\n' || s[x] == '\r') { cut = x; break; } 
      else if(s[x] == ' ' ) cut = x;
    
    if(cut == -1) return;
    addstri(s.substr(0, cut)); 
    
    #ifdef ANDROID
    if(s[cut] == '\n') cy++;
    else { cx ^= 40; if(!cx) cy++; }
    
    #else
    cy++; if(s[cut] == '\n') cy++;
    #endif
    
    if(s[cut] == 0) return;
    
    s = s.substr(cut+1);
    }      
  }

vec2 playerposScreen, stunnedHydraPosScreen;

void adddig(int n, int d) {
  while(d--) n /= 10; addch('0'+n%10);
  }

void drawMap() {
  vec2 playerWrap = wrap(playerpos);
  stunnedHydraPosScreen.x = -1;
  M.out.explored = true;
  for(int y=0; y<SY; y++) for(int x=0; x<SX; x++) {
    move(y,x);
    vec2 v = vec2(x,y);
    if(topx != TBAR) v.x += playerpos.x - center.x;
    if(topy != TBAR) v.y += playerpos.y - center.y;
    cell& c(M[v]);
    if(c.type == CT_HEXOUT) continue;
    
    if(wrap(v) == playerWrap) {
      col(15); addch('@'); playerposScreen = vec2(x,y);
      }
    else if(!c.explored) { 
      if(c.ontarget) { col(12); addch('.'); }
      else { col(7); addch(' '); }
      }
    else if(c.h && (c.h->visible() ? c.seen : seeallmode())) {
      c.h->draw(); 

      if(c.h == stunnedHydra && inlevel(vec2(x,y)))
        stunnedHydraPosScreen = vec2(x,y);
      if(DIRS == 6 && x>0 && y>0 && x < SX-1 && y < SY-1 && c.h->color != HC_TWIN) {
        int hd = c.h->heads; 
        if(hd < 10) ;
        else if(hd < 100) {
          move(y,x+1); adddig(hd,0);
          move(y,x); adddig(hd,1);
          }
        else if(hd < 1000) {
          move(y,x+1); adddig(hd,0);
          move(y,x  ); adddig(hd,1);
          move(y,x-1); adddig(hd,2);
          }
        else if(hd < 10000 && y>0) {
          move(y,x+1); adddig(hd,0);
          move(y,x  ); adddig(hd,1);
          move(y,x-1); adddig(hd,2);
          move(y-1,x); adddig(hd,3);
          }
        else if(hd < 100000 && y>0 && y<SY-1) {
          move(y+1,x); adddig(hd,0);
          move(y,x+1); adddig(hd,1);
          move(y,x  ); adddig(hd,2);
          move(y,x-1); adddig(hd,3);
          move(y-1,x); adddig(hd,4);
          }
        else if(x>1 && y>1 && x<SX-2 && y<SY-2) {
          move(y-1,x-2); adddig(hd,5);
          move(y-1,x); adddig(hd, 4);
          move(y-1,x+2); adddig(hd,3);
          move(y,x+1); addch('Z');
          move(y,x-1); addch('Z');
          move(y+1,x-2); adddig(hd,2);
          move(y+1,x); adddig(hd,1);
          move(y+1,x+2); adddig(hd,0);
          }
        }
      }
    else if(c.mushrooms) {
      col(c.it ? 2 : 8);
      addch(hydraicon(c.mushrooms));
      }
    else if(c.it && c.explored) c.it->draw();
    else {
      if(c.type == CT_STAIRDOWN) { col(c.seen ? 15 : 8); addch('>'); }
      else if(c.type == CT_STAIRUP) { col(c.seen ? 15 : 8); addch('<'); }
      else if(c.type == CT_WALL) { col(c.ontarget ? 12 : 8); addch('#'); }
      else if(c.dead == HC_TWIN+1) { col(12); addch('@'); }
      else if(c.dead) { col(c.seen ? hyinf[c.dead-1].color : 8); addch('%'); }
      else { 
        if(c.ontarget) { col(c.seen ? 12 : 4); addch(':'); }
        else { col(c.seen ? 7 : 8); addch('.'); }
        }
      }
    }
  }

#ifdef NOTEYE

#include "drawhydras.cpp"

char squareRootSign() { 
  InternalProcess* P = noteye_getinternal();
  Font* F = P ? P->f : NULL;
  int i = F ? F->ti[64] : 0;
  TileImage *ti = dynamic_cast<TileImage*> (noteye_getobj(i));
  if(ti && ti->i && ti->i->title.find("cp437") != string::npos) return 251;
  if(ti && ti->i && ti->i->title.find("fantasy") != string::npos) return 251;  
  return '^';
  }


bool doshadow(cell &c1, cell &c2) {
//if(c1.seen && !c2.seen) return true;
  if(c1.explored && !c2.explored) return true;
  if(c1.explored && c2.explored)
    if(c1.type != CT_WALL && c2.type == CT_WALL) return true;
  return false;
  }

map<long long, int> gmapcache;

long long lastid;

void cacheMap(int id, int val) {
  gmapcache[lastid] = val;
  }

// return a multi-layer tile as a Lua table
void drawMapLua(lua_State *L, int x, int y, int mode) {
  vec2 v = vec2(x,y);
  if(topx != TBAR) v.x += playerpos.x - center.x;
  if(topy != TBAR) v.y += playerpos.y - center.y;
  cell& c(M[v]);
  
  noteye_table_new(L);
  if(&c == &M.out) {
    noteye_table_setInt(L, "out", 1);
    return;
    }
  
  int shadow = 0;
  if(doshadow(M[v], M[v+vec2(+1, 0)])) shadow |= 1;
  if(doshadow(M[v], M[v+vec2(-1, 0)])) shadow |= 2;
  if(doshadow(M[v], M[v+vec2( 0,+1)])) shadow |= 4;
  if(doshadow(M[v], M[v+vec2( 0,-1)])) shadow |= 8;

  int floorid;    
  if(c.type == CT_STAIRDOWN) floorid = 3;
  else if(c.type == CT_STAIRUP) floorid = 2;
  else if(c.type == CT_WALL) floorid = 1;
  else floorid = 0;

  bool onplayer = wrap(v) == wrap(playerpos);
  if(!onplayer && !c.h && !(mode == 8 && (c.mushrooms || c.it || c.dead))) {
    long long cacheid = shadow&15;
    cacheid <<= 3; cacheid += (x+3*y) % 5;
    cacheid <<= 1; if(c.type == CT_HEXOUT) cacheid++;
    cacheid <<= 1; if(c.ontarget) cacheid++;
    if(c.explored) {
      cacheid <<= 6; if(c.mushrooms) cacheid |= 1+hydraiconid(c.mushrooms);
      cacheid <<= 7; if(c.it) cacheid |= c.it->icon();
      cacheid <<= 5; if(c.it) cacheid |= c.it->gcolor();
      cacheid <<= 2; cacheid |= floorid;
      cacheid <<= 1; cacheid |= c.seen;
      cacheid <<= 5; cacheid |= c.dead == HC_TWIN+1 ? 31 : c.dead;
      }
    else { cacheid <<= (6+7+5+2+1+5); cacheid |= 30; }
    if(gmapcache.count(cacheid)) {
      noteye_table_setInt(L, "cached", gmapcache[cacheid]);
      return;
      }
    noteye_table_setInt(L, "cacheid", cacheid);
    lastid = cacheid;
    }

  if(wrap(v) == wrap(playerpos)) {
    noteye_table_setInt(L, "hicon", '@');
    int col = P.race == R_NAGA ? 14 : P.race == R_CENTAUR ? 12 : 15;
    noteye_table_setInt(L, "hcolor", getVGAcolor(col));
    }
  
  noteye_table_setInt(L, "shadow", shadow);

  if(c.type == CT_HEXOUT)
    noteye_table_setInt(L, "hex", 1);
    
  if(c.ontarget && &c != &M.out)
    noteye_table_setInt(L, "target", 1);

  if(c.explored) {
  
    if(c.h && (c.h->visible() ? c.seen : seeallmode())) {
      noteye_table_setInt(L, "hcolor", getVGAcolorX(c.h->gcolor()));
      noteye_table_setInt(L, "hicon", c.h->icon());
      noteye_table_setInt(L, "hid", c.h->uid);
      if(c.h->sheads)
        noteye_table_setInt(L, "stun", 1 + 6 * c.h->sheads / c.h->heads);

      if(hydrabox && !P.simplehydras)
      if(c.h->color != HC_TWIN && c.h->color != HC_ETTIN && c.h->color != HC_MONKEY) {
        hydrapicDraw(c.h);
        noteye_table_setInt(L, "gfxid", c.h->gfxid);
        }
      }
  
    if(c.mushrooms) {
      noteye_table_setInt(L, "hicon", hydraicon(c.mushrooms));
      noteye_table_setInt(L, "hcolor", getVGAcolorX(8));
      }
    
    if(c.it) {
      noteye_table_setInt(L, "icolor", getVGAcolorX(c.it->gcolor()));
      noteye_table_setInt(L, "iicon", c.it->icon());
      if(c.it->asWpn()) noteye_table_setInt(L, "itype", c.it->asWpn()->type);
      }
    
    noteye_table_setInt(L, "floor", ".#<>" [floorid]);
    
    if(c.dead == HC_TWIN+1) noteye_table_setInt(L, "dead", -1);
    else if(c.dead) noteye_table_setInt(L, "dead", getVGAcolorX(hyinf[c.dead-1].color));
    
    if(c.seen) noteye_table_setInt(L, "seen", 1);
    }
  }

void viewDescription(sclass*);
void drawScreen();

void helpAbout(int x, int y) {
  vec2 v = vec2(x,y);
  if(topx != TBAR) v.x += playerpos.x - center.x;
  if(topy != TBAR) v.y += playerpos.y - center.y;
  cell& c(M[v]);
  
  if(!c.seen) {
    addMessage("You cannot see this place currently.");
    }
  else if(c.h && !c.h->invisible())
    viewDescription(c.h);
  else if(c.mushrooms) {
    hydra h(HC_MUSH, c.mushrooms, 1, 0);
    viewDescription(&h);
    }
  else if(c.it)
    viewDescription(c.it);
  else if(c.type == CT_WALL) {
    addMessage("Walls are basically walls. You cannot move through them.");
    }
  else if(c.type == CT_STAIRUP) {
    addMessage("Stairs upwards. You can step there to escape the Hydras Pit.");
    }
  else if(c.type == CT_STAIRDOWN) {
    addMessage("Stairs downwards. Proceed to the next level once all hydras are killed.");
    }
  else if(c.type == CT_HEXOUT) {
    addMessage("Click more centrally.");
    }
  else if(c.dead == 1 + HC_TWIN) {
    addMessage("Your twin is dead...");
    }
  else if(c.dead) {
    addMessage("A dead monster.");
    }
  else {
    addMessage("Nothing interesting there.");
    }
  drawScreen();
  }

#else

char squareRootSign() { return '^'; }
#endif

void cursorOnPlayer() {
  move(playerposScreen.y, playerposScreen.x);
  }

#define SDIV (MSX+1)

void drawScreen() {

  bool dead = P.curHP <= 0;
  
  bool showamb = P.active[IT_PAMBI];

  erase();
  drawMap();
  for(int i=0; i<P.arms; i++) {
  
    bool amb = havebit(P.ambiArm, i);
    bool bro = havebit(P.twinarms, i);
    col(dead ? 12 : showamb ? (amb ? (bro ? 13 : 14) : 8) : bro ? 13 : 15);
    move(i, SDIV+1-(i&1)); addch('0' + (i+1)%10);
    
    if(P.flags & dfAutoON) {
      col(12); move(i, SDIV+(i&1)), addch('*');
      }
      
    else if(i == P.cArm) {
      col(14); move(i, SDIV+(i&1)), addch('*');
      }

    else if(i == P.twincarm && P.race == R_TWIN) {
      col(8); move(i, SDIV+(i&1)), addch('*');
      }

    col(7);
    if(wpn[i]) {
      col(wpn[i]->gcolor());
      move(i, SDIV+3), addstri(wpn[i]->name().substr(0,23));
      int x = 77;
      if(wpn[i]->size >= 100) x--;
      if(wpn[i]->size >= 1000) x--;
      if(wpn[i]->size >= 10000) x--;
      if(wpn[i]->size >= 100000) x--;
      move(i, x), addstri(wpn[i]->type + its(wpn[i]->size));
      if(wpn[i]->size < 10) addch(' ');
      }
    }
  
  #ifdef ANDROID
  int ma = MAXARMS;
  #else
  int ma = MAXARMS-1;
  if(P.arms == MAXARMS) ma++;
  #endif
  
  move(ma, SDIV);
  move(ma, SDIV);
  
  int deadcol;
  if(P.curHP < -999) P.curHP = -999;
  if(P.curHP <= 0) deadcol = 4;
  else if(P.curHP <= 30 && P.curHP <= P.maxHP/5) deadcol = 12;
  else deadcol = 15;
  col(deadcol);
  addstri("LV "+its(P.curlevel+1) + " HP "+its(P.curHP) + "/" + its(P.maxHP));
  
  move(ma, SDIV+17);
  
  for(int i=0; i<ITEMS; i++) if(P.active[i]) {
    // do not draw for Nagas unless drank extra
    if(i == IT_PAMBI && P.race == R_NAGA && P.active[i] == 1) 
      continue;
    col(iinf[i].color);
    addch(iinf[i].icon);
    if(P.active[i] == 2) addch(iinf[i].icon);
    if(P.active[i] > 2) addstri(its(P.active[i]));
    addch(' ');
    }
  
  int cy = MAXARMS+1;
  
  for(int i=0; i<size(hydras); i++) if(hydras[i]->aware()) {
    hydra* h = hydras[i];
    col(h->gcolor());
    move(cy, SDIV);
    addstri(h->shortname());

    if(h->zombie) {
      move(cy, 65); col(8); addch('$');
      }
    
    if(h->conflict) {
      move(cy, 65); col(12); addch('$');
      }
    
    if(h->sheads && h->visible()) {
      int left = h->heads - h->sheads;
      if(left == 0) col(12);
      else col(14);
      move(cy, 66);
      addstri(its(left));
      }
    
    move(cy, 72);

    if(h->color == HC_TWIN) {
      col(13);
      if(P.twinmode == 0) addstr("AI");
      else if(P.twinmode == 1) addstr("#1");
      else addstr("#2");
      }
    
    else if(bitcount(P.ambiArm) > 1 && P.active[IT_PAMBI]) {
      col(1);
      ambiAttack(&M[h->pos], 1);
      }
    
    else if(wpn[P.cArm]) {
      weapon* w ( wpn[P.cArm] );

      col(w->gcolor());
      
      if(w->color < HCOLORS && havebit(h->dirty, w->color))
        addstri("???");
      else {
        if(w->doubles())
          addstri("x2");
        else if(!w->stuns() && !w->wand()) {
          int val = h->res[w->color];
          if(val < 0) addstri("x"+its(-val));
          else {
            addstri("+" + its(val));
            }
          }
      
        move(cy, 76);
        if(w->type == WT_BOW)
          addstri(its(bowpower(w) / min(w->size, h->heads) / 2));
        else if(w->stuns() || w->doubles() || (w->type == WT_PSLAY && w->size))
          addstri(its(w->info().stunturns));
  
        if(w->axe()) {
          int grow = h->res[w->color];
          if(grow < 0) grow = 2 * w->size;
          if(grow) addstri(its(w->info().stunturns * (grow + w->size) / grow));
          }
        }
      
      if(w->type == WT_QUAKE) {
        if(h->lowhead())
          addstri(its((w->info().stunturns + 5) / 10));
        else {
          addstri(its(quakefun(h->heads, w->color) / h->heads));
          }
        }
      }
    cy++;
    }

  if(canGoDown()) {
    col(15);
    move(cy, SDIV);
    addstr("level completed!");
    }
  
#ifdef ANDROID
  if(P.curlevel < 1 && cy < 20) {
    col(15); cy++;
    move(cy, SDIV); addstr("touch here for MENU"); cy++;

    cell& c(M[playerpos]); move(cy, SDIV);
    
    if(c.type == CT_STAIRUP)
      addstr("('get/use' stairs to exit)");
    else if(asItem(c.it) && asItem(c.it)->type == IT_HINT)
      addstr("('get/use' to read)");
    else if(c.type == CT_STAIRDOWN)
      addstr("('get/use' stairs to go)");
    else if(c.it)
      addstr("('get/use' to pick up)");
    }
#endif

  col(deadcol);
  move(SY, 0); addstri(msgs[size(msgs)-2]);
  move(SY+1, 0); addstri(msgs[size(msgs)-1]);
  
  cursorOnPlayer();
  }

void drawStar(vec2 pos, int color) {
  if(pos == playerposScreen) return;
  col(color); move(pos.y, pos.x); addch('*');
  }

void showResistances(hydra *h, int cy) {
  col(h->gcolor());
  
  move(cy, 2); h->draw();
  
  move(cy, 4); addstri(h->name());
  
  move(cy, 30); if(h->visible() || seeallmode())
    addstri(its(h->heads-h->sheads)+"/"+its(h->heads));

  if(h->ewpn) {
    move(cy, 40);         
    col(h->ewpn->gcolor());
    addstri(h->ewpn->name() + " " + h->ewpn->type + its(h->ewpn->size));
    }
  
  else for(int c=0; c<HCOLORS; c++) {
    move(cy, 40+c*4);
    col(cinf[c].color);
    int val = h->res[c];
    if(havebit(h->dirty, c))
      addstri("???");
    else if(val < 0) addstri("x"+its(-val));
    else addstri("+"+its(val));
    }
  }

void viewDescription(sclass *x) {

  if(useKnowledgeOn(x))
    return;
    
  int cy = 2;
  erase();
  col(x->gcolor());
  move(0, 0); addch(x->icon()); addch(' '); addstri("About: "+x->name()+"...");
  viewMultiLine(x->describe(), cy);
  if(x->sct() == SCT_HYDRA) {
    shareS("meet", " the "+x->name());
    hydra *h = (hydra*) x;
    cy++; move(cy,0); addstr("Resistances against weapon colors:");
    cy+=2; showResistances(h, cy);
    }
  
  ghch(IC_VIEWDESC);
  }

void showMenuOption(int cy, char letter, bool selected, int cx = 0) {
  col(selected ? 14 : 7);
  move(cy, cx); addch(letter); addch(' ');
  }

bool changeSelection(int dir, int& sel, int qty) {
  if(dir == D_PGUP) sel-=3;
  else if(dir == D_UP) sel--;
  else if(dir == D_HOME) sel=0;
  else if(dir == D_END) sel=-1;
  else if(dir == D_DOWN) sel++;
  else if(dir == D_PGDN) sel+=3;
  else return false;

  sel %= qty; if(sel<0) sel += qty;
  return true;
  }

#define MAXINF 20

void fullHydraInfo() {
  int selection = 0;
  
  while(true) {
  
  erase();
  
  move(0, 0); col(15); addstr("View full description for:");
  
  for(int c=0; c<HCOLORS; c++) {
    move(0, 40+c*4);
    col(cinf[c].color);
    string s = cinf[c].wname.substr(0,4);
    s[0] = toupper(s[0]);
    addstri(s);
    }

  int cy = 0;
  
  sclass * infos[MAXINF];

  for(int i=0; i<size(hydras); i++) if(hydras[i]->aware()) {
    if(cy >= MAXINF) break;
    showMenuOption(cy+2, 'a'+cy, cy==selection);
    infos[cy] = hydras[i];
    showResistances(hydras[i], cy+2);
    cy++;
    }

  for(int o=0; o<2; o++)

  for(int y=0; y<SY; y++) for(int x=0; x<SX; x++) {
    cell& c(M.m[y][x]);
    if(!c.it) continue;
    weapon* w = c.it->asWpn();
    if(c.seen && (o?!w:!!w)) {
      if(cy >= MAXINF) break;
      showMenuOption(cy+2, 'a'+cy, cy==selection);
      infos[cy++] = c.it;

      move(cy+1, 2); c.it->draw();
      move(cy+1, 4); 
      if(w)
        addstri(w->name() + " " + w->type + its(w->size));
      else
        addstri(c.it->name());
      }
    }
  
  if(cy == 0) {
    addMessage("Nothing interesting around.");
    return;
    }
  
  int ch = ghch(IC_FULLINFO);

  if(ch >= 'a' && ch < 'a'+cy) {
    selection = ch - 'a';
    viewDescription(infos[selection]);
    }
  else if(ch == 10 || ch == 13)
    viewDescription(infos[selection]);
  else if(!changeSelection(ch, selection, cy)) return;
  }
  }

void showReduceTable(int &cy) {
  weapon* w = wpn[P.cArm]; cy--;
  if(!w) {
    addMessage("Splitting nothing into two parts does not help with your mission.");
    return;
    }
  int t = 1;
  if(w->size == 0) t = 0;
  if(w->size > 10) t = w->size - 5;
  int osiz = w->size;
  while(cy < 23) {
    w->size = t++;
    weapon *wnew = w->reduce();
    if(w->size == osiz) {
      if(!wnew)
        addMessage("You cannot currently reforge your selected weapon...");
      else if(w->type == WT_RAND)
        addMessage("Do you want to create a random weapon?");
      else
        addMessage("Do you want to create a "+wnew->name()+" ("+wnew->type + its(wnew->size) + ")?");
      }

    if(wnew) {
      move(cy, 0);
      col(w->gcolor()); addstri(w->name() + " (" + w->type + its(w->size) + ")");
      col(10); addstr(" => ");
      col(wnew->gcolor()); addstri(wnew->name() + " (" + wnew->type + its(wnew->size) + ")");
      cy++;
      delete wnew;
      }
    if(t > osiz + 100) break;
    }
  w->size = osiz;
  }

bool onscreenconf = false;

bool viewHelpForItem(int ii) {
  erase();
  move(0,0); col(iinf[ii].color);
  addch(iinf[ii].icon); addch(' '); addstri("About the "+iinf[ii].name+"...");

  int cy = 2; viewMultiLine(iinf[ii].desc, cy);
  
  string adjs[6] = {
    "bit long", "long", "very long", "really long", "extremely long",
    "long long long"
    };

  move(0, 40); col(14); addstri("Value: $"+its(iinf[ii].value));
  
  bool multiok = P.race == R_NAGA ? P.active[IT_PAMBI] > 1 : P.ambifresh;
  
  if(ii == IT_SPART && !(P.active[IT_PAMBI] && bitcount(P.ambiArm) > 1 && multiok))
    showReduceTable(cy);
  else if((ii == IT_SGROW || ii == IT_SXMUT) && P.active[IT_PAMBI] && bitcount(P.ambiArm) > 1 && !multiok) {
    if(P.race == R_NAGA)
      addMessage("Without the Potion of Ambidexterity, only one item will be affected. (y/n)");
    else
      addMessage("Ambidexterity not fresh, will affect only one item. Are you sure? (y/n)");
    }
  else if((ii == IT_PKNOW || ii == IT_RGROW) && P.active[IT_PAMBI] && P.arms > 4)
    addMessage("Warning: might take a "+adjs[P.arms-5]+" time to compute! (y/n)");
  else 
    if(P.quickmode) return true;
  else 
    addMessage("Use the "+iinf[ii].name+"? (y/n)");
  
  col(15); move(23, 0); addstri(msgs[size(msgs)-1]);
  return yesno(IC_MYESNO);
  }

int viewInventory() {
  
  int mapp[24];
  int kinds = 2;
  for(int i=0; i<ITEMS; i++) if(P.inv[i]) 
    mapp[kinds++] = i;

  if(kinds == 2) {
    addMessage("Your inventory is empty!");
    return -1;
    }
  
  int selection = 2;
  
  while(true) {
    drawScreen();
    
    for(int x=SX; x<80; x++)
    for(int y=0; y<24; y++)
      move(y,x), col(7), addch(' ');
    
    int X = SX+1;
    // erase();
    move(0, X); col(14); 
    addstr("Inventory: (Enter to select)");

    move(2, X); col(selection == 0 ? 14 : 8); addstr("i - exit");
    move(3, X); col(selection == 1 ? 14 : 8); 
    addstr("h - quick mode "); addstr(P.quickmode ? "on" : "off");
  
    for(int i=2; i<kinds; i++) {
      int ii = mapp[i];
      showMenuOption(3+i, iinf[ii].hotkey, i == selection, X);
      col(iinf[ii].color); addch(iinf[ii].icon); addch(' ');
      addstri(iinf[ii].name.substr(10));
      if(P.inv[ii] > 1) addstri(" (x" + its(P.inv[ii]) + ")");
      }
    
    int ch = ghch(IC_INV);
  
    for(int i=2; i<kinds; i++)
      if(ch == iinf[mapp[i]].hotkey) { 
        selection = i;
        ch = 10;
        }

    if(ch == 10 || ch == 13) {
    
      if(selection == 0) return -1;
      if(selection == 1) {
        P.quickmode = !P.quickmode;
        continue;
        }
      
      int ii = mapp[selection];

      if(viewHelpForItem(ii))
        return ii;

      if(onscreenconf) {
        // remove targetting lines
        los();
        // add targetting circle for power swipe
        if(ii == IT_PSWIP) for(int dir=0; dir<DIRS; dir++)
          M[playerpos + dirs[dir]].ontarget = true;
        // 'active' makes weapon charge's targetting lines appear
        if(ii == IT_PCHRG) {
          P.active[ii]++; 
          for(int i=0; i<DIRS; i++)
            tryLineAttack(i, true, true);
          P.active[ii]--;
          }
        drawScreen();
        }
      
      addMessage("Cancelled.");
      }
    
//  else if(ch == '?' || ch == 9 || ch == 'h' || ch == KEY_F0+1 || ch == '/')
//    viewHelpForItem(selection, true);
    
    else if(ch == 'h')
      P.quickmode = !P.quickmode;
      
    else if(ch == D_PGUP || ch == D_HOME)
      selection = 0;

    else if(ch == D_PGDN || ch == D_END)
      selection = kinds-1;
    
    else if(ch == D_UP || ch == D_DOWN) {
      if(ch == D_UP) selection--; else selection++;
      selection %= kinds; if(selection<0) selection += kinds;
      }
    
    else return -1;
    }
  }

void viewHelp() {

  int page = 0;
  
  while(true) {
  
    page %= HELPLEN; page += HELPLEN; page %= HELPLEN;

    erase(); col(15);
    
    string pag = helpinfo[page];
    
    if(page == 1) {

      if(P.altkeys) pag += 
        "S,F2,F10,ESC = go to the main menu\r"
        "i = view or use the inventory (powders, scrolls and potions)\r"
        ". = wait one turn\r"
        "l = look at hydras and items (full information)\r"
        "g = pick up a weapon or item, or use stairs (G for titans)\r"
        "k = drop current weapon             v = view its description\r"
        "t = throw/shoot                     T = auto-fire mode on-off\r"
        "b = switch twin control ('both')    u = switch twin order ('you')\r"
        "o = auto-explore (see later)        m = see recent messages\r";

      else pag += 
        "q,S,F2,F10,ESC = go to the main menu\r"
        "i = view or use the inventory (powders, scrolls and potions)\r"
        ". = wait one turn\r"
        "f = full information about hydras and items in sight\r"
        "g = pick up a weapon or item, or use stairs\r"
        "d = drop current weapon             v = view its description\r"
        "t = throw/shoot                     T = auto-fire mode on/off\r"
        "c = switch twin control             s = switch twin order\r"
        "o = auto-explore (see later)        m = see recent messages\r";
      
      #ifdef NOTEYE
      pag += 
        "F4 = system config (gfx sound etc)  p = animate hydras on/off\n";
      #endif

      pag += 
       "In most menus you can use arrows to navigate, Enter to select, "
       "and Space to cancel. When entering names you can press TAB to "
       "clear the current name.";
      }
        
    int cy = 2; viewMultiLine(pag, cy);

    int ch = ghch(IC_HELP);
  
    if(ch >= 'a' && ch <= 'f') page = ch - 'a';
    
    if(ch == 10  || ch == 13) {
      page++; if(page == HELPLEN) break;
      }
    
    else if(ch == ' ' || ch == ESC || ch == PANIC) break;
    
    else if(ch == '=') P.altkeys = !P.altkeys;
    
    else if(ch<0) break;
    
    else if(ch == D_PGUP || ch == D_UP || ch == D_LEFT) page--;
    else if(ch == D_HOME) page = 0;
    else if(ch == D_END) page = -1;
    else if(ch == D_PGDN || ch == D_DOWN || ch == D_RIGHT) page++;
    }
  }

char wpnnotochar(int id) { return "1234567890"[id]; }

void giveHint(hydra *h) {
  erase();
  move(0,0); col(h->gcolor()); addstri(h->name());
  move(0, 66);
  col(10); addstri(" health +"+its(h->heal));
  if(h->invisible()) {
    int i = P.active[IT_PAMBI];
    P.active[IT_PAMBI] = 0;
    analyzeHydra(h);
    P.active[IT_PAMBI] = i;
    }
  else analyzeHydra(h);
  int cy = 2;
  
  int spos; encode(h->heads, h->sheads, spos);
  
  if(h->heads >= AMAXS) {
    move(2, 2); col(15); addstri("Your magic cannot analyze this gigantic "+h->name()+" currently."); ghch(IC_VIEWDESC); return;
    }
  if(wnd[spos] == WMAX) {
    move(2, 2); col(12); addstri("It seems you cannot kill "+h->name()+" with only your current weapons.");
    ghch(IC_VIEWDESC); return;
    }
  move(1, 0); col(13); addstr("HD  AC  WND weapon, growth & wounds");
  
  bool showdam = false;
  
  while(spos && cy < 24) {

    if(h->invisible() && cy >= 5) {
      col(8); move(cy+1, 0);
      addstr("Further attacks are hidden in the shadows...");
      break;
      }

    int hd, sh; decode(hd, sh, spos);

    if(showdam && !h->invisible()) { 
      col(12); addstri(" W" + its(SI.dampost(hd - sh)));

      if(h->color == HC_GROW && hd > sh && hd <= 720) {
        col(h->gcolor()); 
        int pos2;
        hd++; encode(hd, sh, pos2);
        if(wnd[pos2] > wnd[spos]) { spos = pos2; addstri(" +1"); }
        else { hd--; addstri(" +0"); }
        }

      if(h->color == HC_WIZARD && hd > sh) {
        col(h->gcolor()); 
              
        int bw = -1, bwa = -1, bhc = 0;

        for(int ah=hd; ah>=sh+1 && ah >= hd*3/4; ah--) {
          int pos2; encode(ah, sh, pos2);
          if(wnd[pos2] > bw) bw = wnd[pos2], bwa = pos2, bhc = ah;
          }
        addstri(" -"+its(hd-bhc)); spos = bwa; hd = bhc;
        }
      }
    showdam = true;

    col(h->gcolor());
    if(h->invisible()) {
      move(cy, 0); addstr("??? ??? ???");
      }
    else {
      move(cy, 0); addstri(its(hd)); 
      move(cy, 4); addstri(its(hd - sh));
      move(cy, 8); addstri(its(wnd[spos]));
      }
    move(cy, 12);
    
    int ospos = spos;
  
    if(usew[spos] == -1) {
      col(8); addstr("allow it to hit you...");
      spos = goal[spos];       
      }
    else if(usew[spos] == -2) {
      col(8); addstr("wait until awakened...");
      spos = goal[spos]; 
      showdam = false;
      }
    else if(usew[spos] & AMBIWPN) {
      col(7); addstr("weapon set: ");
      col(14);
      for(int i=0; i<10; i++) if(havebit(usew[spos], i)) addch(wpnnotochar(i));
      spos = goal[spos]; 
      }
    else {
      col(wpn[usew[spos]]->gcolor());
      addch(wpnnotochar(usew[spos])); addch(' '); addstri(wpn[usew[spos]]->name());
      addstri(" " + (wpn[usew[spos]]->type + its(wpn[usew[spos]]->size) + " "));
      col(h->gcolor());
      int hr = wpn[usew[spos]]->stuns() ? 0 : h->res[wpn[usew[spos]]->color];
      spos = goal[spos]; if(spos == 0) hr = 0;
      if(hr > 0) addstri(" +" + its(hr)); else if(hr < 0) addstri(" x" + its(-hr));
      }
    
    if(h->color == HC_VAMPIRE && showdam && spos) {
      showdam = false;
      int wn = wnd[ospos] - wnd[spos];
      col(12); addstri(" W" + its(wn)); col(h->gcolor()); addstri(" +"+its(wn));
      }
    
    cy++;
    }
  
  if(cy < 24) {
    move(23, 0); col(7); addstr("HD = heads, AC = active heads, WND = total wounds from this moment");
    }  
  
  ghch(IC_VIEWDESC);
  }

string sortorder = "tkbl";

bool trollcmp(int a, int b) {
  weapon *wa = pinfo.trollwpn[a];
  weapon *wb = pinfo.trollwpn[b];
  for(int i = 0; i < size(sortorder); i++) switch(sortorder[i]) {
    case 't':
      if(wa->type != wb->type) return wa->type > wb->type;
      break;
    case 'k':
      if(wa->color!= wb->color)return wa->color > wb->color;
      break;
    case 'b':
      if(wa->size != wb->size) return wa->size > wb->size;
      break;
    case 'l':
      return pinfo.trollkey[a] > pinfo.trollkey[b];
      break;
    case 'f':
      if(wa->level != wb->level) return wa->level > wb->level;
      break;
    }
  return false;
  }

bool quickGet(weapon*& w) {
  bool free[128];
  for(int c=33; c<128; c++) free[c] = true;
  
  int s = size(pinfo.trollwpn);
  for(int i=0; i<s; i++) {
    free[int(pinfo.trollkey[i])] = false;
    }

  int ch = ' ';
  for(ch='a'; ch<='z'; ch++) if(free[ch]) goto keyfound;
  for(ch='A'; ch<='Z'; ch++) if(free[ch]) goto keyfound;
  for(ch=33;  ch<=127; ch++) if(free[ch]) goto keyfound;
  
  addMessage("Your inventory is full!");
  return false;
  
  keyfound:  
  addMessage("Picked up the "+w->name()+" (key '"+char(ch)+"').");
  pinfo.trollwpn.push_back(w); w = NULL;
  pinfo.trollkey.push_back(ch);
  return true;
  }

bool viewTrollInventory() {

  int selection = -1;

  while(true) {

  erase();
  
  move(0,45); col(8); addstr("Ctrl+FLBKT = change sort order");
  
  move(0,0); col(14); 
  int s = size(pinfo.trollwpn);
  if(wpn[P.cArm])
    addstri("Press a key to store "+wpn[P.cArm]->name());
  else
    addstri("Take what?");
    
  bool free[128];
  for(int c=33; c<128; c++) free[c] = true;
  
  int wid = 80;
  if(s > 0) wid = wid / ((s+19)/20);
  
  for(int i=1; i<s; i++) if(i>0) {
    if(trollcmp(i-1, i)) 
      swap(pinfo.trollkey[i-1], pinfo.trollkey[i]),
      swap(pinfo.trollwpn[i-1], pinfo.trollwpn[i]), i-=2;
    }
  
  char cpick = ' ';

  for(int i=0; i<s; i++) {
    move(2 + i % 20, wid * (i / 20));
    if(i == selection) cpick = pinfo.trollkey[i];
    col(i==selection?14:8); addch(pinfo.trollkey[i]); addch(' ');
    free[int(pinfo.trollkey[i])] = false;
    weapon* w = pinfo.trollwpn[i];
    if(s>40) {
      col(w->gcolor());
      string s = w->type + its(w->size) + " " + w->name();
      s = s.substr(0, wid-2);
      addstri(s);
      }
    else {
      w->draw(); addch(' ');
      addstri(w->name() + " " + w->type + its(w->size));
      }
    }

  move(23, 0); col(7); if(s > 20) {
    for(int c=33; c<127; c++) if(free[c]) addch(c);
    }
  else {
    addstr("free keys: ");
    for(int c='a'; c<='z'; c++) if(free[c]) addch(c);
    for(int c='A'; c<='Z'; c++) if(free[c]) addch(c);
    addstr("...");
    }

  if(cpick == ' ') {
    for(int c=33; c<127; c++) if(free[c]) {
      cpick = c; break;
      }
    }
  
  int ch = ghch(IC_TROLL);
  int xch = ch+96;
  
  if(ch == PANIC) return false;
  
  if(xch == 'f' || xch == 'b' || xch == 't' || xch == 'k' || xch == 'l') {
    sortorder = char(xch) + sortorder;
    continue;
    }
  
  if(ch == 10 || ch == 13) ch = cpick;
  
  for(int i=0; i<s; i++) if(pinfo.trollkey[i] == ch) {
    cancelVorpalOn(wpn[P.cArm]);
    swap(pinfo.trollwpn[i], wpn[P.cArm]);
    playSwitchSound(wpn[P.cArm]);
    if(pinfo.trollwpn[i] == NULL) {
      while(i < s-1)
        pinfo.trollwpn[i] = pinfo.trollwpn[i+1], pinfo.trollkey[i] = pinfo.trollkey[i+1], i++;
      pinfo.trollwpn.resize(i);
      pinfo.trollkey.resize(i);
      }
    return true;
    }

  if(ch>=DBASE && ch <= DBASE+7) { 
    if(ch == D_RIGHT) selection += 20;
    if(ch == D_PGUP || ch == D_HOME) selection = 0;
    if(ch == D_UP) selection--;
    if(ch == D_LEFT) selection -= 20;
    if(ch == D_DOWN) selection++;
    if(ch == D_PGDN || ch == D_END) selection = -2;
    selection %= (s+1); selection += s+1; selection %= s+1;  
    continue;
    }

  if(ch <= 32 || ch >= 127) return false;
  
  if(wpn[P.cArm] == NULL) return true;
  cancelVorpalOn(wpn[P.cArm]);
  pinfo.trollwpn.push_back(wpn[P.cArm]);
  wpn[P.cArm] = NULL;
  pinfo.trollkey.push_back(ch);
  return viewTrollInventory() || true; // allow to take another one
  }
  }

void wpnswitch(char ch) {
  if(ch == ']') {
    P.cArm++;
    P.cArm %= P.arms;
    P.ambiArm <<= 1;
    if(P.ambiArm & (1<<P.arms)) P.ambiArm -= (1<<P.arms) - 1;
    }
  if(ch == '[') {
    P.cArm--;
    if(P.cArm < 0) P.cArm += P.arms;
    if(P.ambiArm & 1) P.ambiArm |= (1<<P.arms);
    P.ambiArm >>= 1;
    }
  }

void createTargetLines() {
  for(int i=0; i<DIRS; i++) {
    tryLineAttack(i, !P.manualfire, true);
    if(wpn[P.cArm] && wpn[P.cArm]->wand() && (M[playerpos+dirs[i]].type == CT_WALL))
      tryWandUse(wpn[P.cArm], i, true);
    if(wpn[P.cArm] && wpn[P.cArm]->type == WT_SPEED) {
      for(int k=1; k<=wpn[P.cArm]->size; k++) {
        cell& c(M[playerpos+dirs[i] * k]);
        if(!c.isPassable()) break;
        if(k != wpn[P.cArm]->size - 1)
          c.ontarget = true;
        }
      }
    }

  for(int i=0; i<size(hydras); i++) {
    hydra *H = hydras[i];
    if(M[H->pos].seen)
    if(H->color & HC_DRAGON) for(int d=0; d<DIRS; d++) breathAttack(H, d, false);
    }
  }

int headask() {
  static string last = "50";
  editString(last, "Size: ");
  int i = atoi(last.c_str());
  if(i<1) return 1;
  return i;
  }

hydra *enemyInSight() {
  
  for(int i=0; i<size(hydras); i++)
    if(M[hydras[i]->pos].seen && !hydras[i]->zombie)
      return hydras[i];

  return NULL;
  }


int autoexplore() {
  if(!exploreOn) return 0;

#ifdef CURSES
      nodelay(mainwin, true);

      int ch = ghch(IC_GAME);
#else
#ifdef NOTEYE
  halfdelay(0); refresh(IC_GAME);
  int ch = ghch(IC_GAME);
#else
#ifdef MINGW
  if(kbhit()) { exploreOn = false; return 0; }
  int ch = 0;
#else
  int ch = 0;
#endif
#endif
#endif

/*halfdelay(1); cbreak(); refresh(IC_GAME);

  int ch = ghch(P.race == R_TWIN ? IC_GAMETWIN : IC_GAME); */
  if(ch > 0) { exploreOn = false; return ch; }

  hydra *h = enemyInSight();
  
  if(h) {
    addMessage("You see the "+h->name()+".");
    drawScreen();
    exploreOn = false;
    return 0;
    }
  
  if(P.race != R_TROLL && M[playerpos].it && M[playerpos].it->asItem()) {
    exploreOn = false;
    return 'g';
    }
  
  bfs(1);
  
  if(targetdir < 0) {
    bfs(1, true);
    }
  
  if(targetdir < 0) {
    addMessage("Level completely explored.");
    drawScreen();
    exploreOn = false; 
    return 0;
    }
  
  cell& c2 = M[playerpos + dirs[targetdir]];
  
  if(c2.mushrooms && canGoDown()) {
    stats.automush += c2.mushrooms;
    // assume there are no mushrooms
    c2.mushrooms = 0;
    }
  
  if(!!c2.mushrooms)
    exploreOn = false;
  
  stats.automove++;
  return INDB + targetdir;
  }

// (0) only if not shown yet, (1) say "Note:", (2) say without "Note:"
void informAlt(int b) {
  if(P.altkeys) return;
  static bool toshow = true;
  if(toshow && stats.turns < 100 && b == 0) b = 1;
  if(b == 1)
    addMessage(s0 + "Note: press '=' to move with " + getLayoutName() + ".\n");
  if(b == 2)
    addMessage(s0 + "Press '=' to move with " + getLayoutName() + ".\n");
  toshow = false;
  }

bool wasDead, isDead;

#include "trailer.cpp"

void mainloop() {
  while(!quitgame) {
  
    if(P.race == R_NAGA) {
      P.ambifresh = 1;
      if(!P.active[IT_PAMBI]) 
        P.active[IT_PAMBI] = 1;
      }

    // addMessage("cur="+its(P.curHP)+" twin="+its(twin?1:0));
    if(P.curHP <= 0 && twin) {
      twinswap();
      M[twin->pos].hydraDead(NULL);
      }

    wasDead = isDead;
    isDead = P.curHP <= 0;
    if(isDead && !wasDead && !sceneid) return;
    if(wasDead && !isDead) achievement("RESURRECTION");
    
    los();
    createTargetLines();
    drawScreen();

    if(stunnedHydra) {
      drawStar(stunnedHydraPosScreen + dirs8[animframe & 7], stunnedColor);
      if(stunnedHydra->sheads == stunnedHydra->heads)
        drawStar(stunnedHydraPosScreen - dirs8[animframe & 7], stunnedColor);
      cursorOnPlayer();
      animframe++;
      }

    int ch = autoexplore();
    
    if(!ch)  {
      
      #ifdef NOCURSES
        if(stunnedHydra) halfdelay(1); else cbreak();
        ch = ghch(P.race == R_TWIN ? IC_GAMETWIN : IC_GAME);
        if(stunnedHydra) cbreak();

      #else
      #ifdef MINGW
        ch = -1;
        if(kbhit() || !stunnedHydra) 
          ch = ghch(IC_GAME);
        else
          refresh(IC_GAME);

      #else
        if(stunnedHydra) halfdelay(1); else cbreak();

      ch = ghch(IC_GAME);
      nocbreak(); cbreak();
      #endif
      #endif
      }
    
    if(true) {

      int dir = getDir(ch);
      
      if(dir != -1) {
        if(dir == -2) {
          if(twin) {
            singlestep();
            }
          else {
            P.phase = 0; cancelVorpal();
            moveHydras();
            }
          }
        else movedir(dir);
        continue;
        }
  
      if(P.altkeys) {
        // simply translate:
        if(ch == 'l') ch = 'f';
        else if(ch == 'u') ch = 's';
        else if(ch == 'h') ch = '?';
        else if(ch == 'b') ch = 'c';
        else if(ch == 'y') ch = 'a';
        else if(ch == 'k') ch = 'd';
        }
      }

    int ii;
    
    switch(ch) {
      case PANIC:
        saveGame();
        return;
        
      case 'q': case 'S': case KEY_F0+2: case KEY_F0+10: case 27:
        return;
      
      case '[': case ']':
        wpnswitch(ch);
        while(havebit(P.twinarms, P.cArm) && !P.active[IT_PAMBI])
          wpnswitch(ch);
        break;
      
      case '1': case '2': case '3': case '4': case '5': case '6':  case '7': case '8': case '9': 
      case '0': {
      
        P.flags &= ~dfAutoON;
      
        int anum = ch - '1';
        if(anum<0) anum = 9;
        
        if(havebit(P.twinarms, anum) && !P.active[IT_PAMBI]) {

          if(twin && !neighbor(playerpos, twin->pos)) {
            addMessage("Too far to switch weapons with "+twinName()+".");
            break;
            }
          
          else if(!twinAlive()) {
            addMessage("You are too horrified to take "+twinName()+"'s weapon!");
            break;
            }
          
          swap(wpn[P.cArm], wpn[anum]);
          addMessage("Swapped weapons with "+twinName()+".");
          cancelspeed();
          break;
          }
      
        if(havebit(P.ambiArm, anum)) P.ambiArm = 1 << anum;
        else P.ambiArm |= (1<<anum);

        if(anum >= P.arms) addMessage("No weapon number "+its(anum+1)+".");
        else {
          if(P.active[IT_PAMBI] && bitcount(P.ambiArm) > 1 && wpn[anum])
            addMessage("Added "+wpn[anum]->name()+" to your set.");
          addMessage("Switched to ("+its(anum+1)+") " + (wpn[anum] ? wpn[anum]->name() : "nothing") + ".");
          playSwitchSound(wpn[anum]);
          P.cArm = anum;
          }
        break;
        }
      
      case 'T':
        P.manualfire = !P.manualfire;
        if(P.manualfire) addMessage("Fire mode set to 'manual'.");
        else addMessage("Fire mode set to 'auto'.");
        break;
        
      case 't': {
        if(!wpn[P.cArm]) {
          addMessage("No weapon selected.");
          break;
          }
        else if(wpn[P.cArm]->polewpn())
          addMessage("Thrust "+wpn[P.cArm]->name()+" in which direction?");
        else if(!wpn[P.cArm]->msl()) {
          addMessage("Your current weapon cannot be used for ranged attacks.");
          break;
          }
        else if(!P.manualfire) {
          addMessage("Firing is automatic. Press 'T' to invoke manual fire.");
          break;
          }
        else if(wpn[P.cArm]->type == WT_BOW)
          addMessage("Shoot "+wpn[P.cArm]->name()+" which direction?");
        else 
          addMessage("Throw "+wpn[P.cArm]->name()+" which direction?");

        // draw targetting lines
        los();
        for(int i=0; i<DIRS; i++) tryLineAttack(i, true, true);
        drawScreen();
        
        ch = ghch(IC_ASKDIR);
        int dir = getDir(ch);
        if(dir >= 0) {
          if(wpn[P.cArm]->polewpn())
            tryPoleAttack(dir);
          else
            tryLineAttack(dir, true, false);
          }
        break;
        }
        
      case 'd':
        if(!wpn[P.cArm]) addMessage("No weapon to drop.");
        else if(M[playerpos].it) addMessage("There is already something here.");
        else if(M[playerpos].type != CT_EMPTY) addMessage("There is something here.");
        else {
          cancelVorpalOn(wpn[P.cArm]);
          M[playerpos].it = wpn[P.cArm];
          wpn[P.cArm] = NULL;
          addMessage("Dropped a weapon.");
          wpnset++;
          }
        informAlt(0);
        break;
    
      case 'm': {
        erase();
        col(15);
        for(int i=0; i<24; i++) {
          move(i, 0);
          int xpos = size(msgs) + i - 24;
          if(xpos >= 0) addstr(msgs[xpos].c_str());
          }
        ghch(IC_VIEWDESC);
        break;
        }
      
      case 'f':
        fullHydraInfo();
        break;
      
      case 'v':
        if(wpn[P.cArm]) {
          shareBe("wielding the " + wpn[P.cArm]->name());
          viewDescription(wpn[P.cArm]);
          }
        else
          addMessage("Your empty hand is useless against hydras.");
        break;
      
      case 'i': case 'I': {
        stunnedHydra = NULL;
        if((P.race == R_TROLL) ^ (ch == 'I' && debugon())) {
          if(viewTrollInventory()) {
            if(P.active[IT_PAMBI] && bitcount(P.ambiArm) > 1) {              
              while(true) {
                P.cArm++;
                P.cArm %= P.arms;
                if(havebit(P.ambiArm, P.cArm)) {
                  if(!viewTrollInventory()) break;
                  else if(P.ambifresh == P.active[IT_PAMBI]) P.ambifresh--;
                  }
                }
              }
            cancelspeed(); wpnset++;
            }
          }
        else {
          ii = viewInventory();
          if(ii < 0) break;
          else if(P.curHP <= 0) 
            addMessage("It seems you are not yet used to being dead...");
          else if(useup(ii)) {
            P.inv[ii]--, stats.usedup[ii]++;
            if(P.race == R_NAGA) singlestep();
            if(!stats.woundwin) stats.usedb[ii]++;
            }
          }
        break;
        }
        
      case 'g': case 'G':
        stunnedHydra = NULL;
        if(P.curHP <= 0) {
          if(debugon()) P.curHP = P.maxHP;
          addMessage("You are dead, got it? Press 'q' to quit.");
          }
        else if((M[playerpos].type == CT_STAIRUP) && !M[playerpos].it)
          return;
        else if((M[playerpos].type == CT_STAIRDOWN/* || M[playerpos].type == CT_STAIRUP*/) && !M[playerpos].it) {
          emSaveGame();
          
          if(P.race == R_TWIN && !twinAlive()) {
            addMessage("You cannot continue alone!");
            continue;
            }
            
          if(!canGoDown()) {
            addMessage("You have to slay all the Hydras here before going further!");
            continue;
            }
        
          for(int i=0; i<size(hydras); i++) {
            M[hydras[i]->pos].h = NULL;
            if(hydras[i]->color == HC_ETTIN) {
              stats.ettinsave++;
              // the contract does not force you to kill ettins
              delete hydras[0];
              }
            else if(hydras[i]->color == HC_MONKEY) {
              stats.monkeysave++;
              // the contract does not force you to kill ettins
              delete hydras[0];
              }
            else
              stairqueue.push_back(hydras[i]);
            }
          
          if(P.flags & dfTutorial) {
            clearGame();
            P.flags &= ~dfTutorial;
            initGame();
            break;
            }
          
          twin = NULL;
          hydras.resize(0);
          P.curlevel++;
          generateLevel();
          addMessage("You climb down the stairs.");
          
          // trolls lose their enchantments
          if(P.race == R_TROLL) {
            for(int i=0; i<ITEMS; i++) if(i != IT_PSEED) P.active[i] = 0;
            P.ambifresh = 0;
            }
          
          // mushroom staff: generate a potion
          for(int i=0; i<P.arms; i++) if(wpn[i] && wpn[i]->type == WT_FUNG) {
            if(P.race == R_TROLL)
              P.active[IT_PSEED] += 7 + 7 * wpn[i]->size;
            else
              P.inv[IT_PSEED]++;
            if(wpn[i]->level == P.curlevel-1)
              addMessage("You gather some spores from your "+wpn[i]->name()+".");
            }
          }
        else if(!M[playerpos].it) {
          addMessage("Nothing to get or use here.");
          }
        else if(M[playerpos].it && M[playerpos].it->sct() == SCT_WPN) {
          weapon*& w ((weapon*&) M[playerpos].it);
          if(w->type == WT_QUAKE) {
            if(P.race == R_TROLL)
              addMessage("You have found the ancient Club of Hydra Quakes!!");
            else {
              addMessage("This giant spiked club seems to be some titanic artifact.");
              addMessage("Even you are too weak to even pick it up!");
              }
            if(P.race != R_TROLL) break;
            }
          weapon *newwpn = w;
          if(ch == 'G' && P.race == R_TROLL) {
            if(!quickGet(w)) break;
            }
          else {
            swap(w, wpn[P.cArm]); wpnset++;
            cancelVorpalOn(w);
            if(M[playerpos].it)
              addMessage("Swapped the "+M[playerpos].it->name()+" for the "+wpn[P.cArm]->name()+".");
            else
              addMessage("Picked up the "+wpn[P.cArm]->name()+".");
            }
          weaponMessage(newwpn);
          playSwitchSound(newwpn);
          cancelspeed();
          #ifdef ANDROID
          emSaveGame();
          #endif
          }
        else {
          item* it = M[playerpos].it->asItem();
          if(!it) break;
          if(it->type == IT_HINT) {
            viewDescription(it);
            break;
            }
          else if(P.race == R_TROLL) {
            if(!viewHelpForItem(it->type)) break;
            if(!useup(it->type)) break;
            stats.usedup[it->type]++;
            if(P.curlevel < LEVELS) stats.usedb[it->type]++;
            // Trolls get speed twice, because they use up one
            if(it->type == IT_PFAST)
              P.active[IT_PFAST]++;
            }
          else {
            P.inv[it->type]++;
            addMessage("Picked up the "+it->name()+".");
            }
          delete it;
          if(M[playerpos].it == it)
            M[playerpos].it = NULL;
          cancelspeed();
          #ifdef ANDROID
          emSaveGame();
          #endif
          }
        break;
      
      case KEY_F0+1: case '?': case '/':
        viewHelp();
        break;
      
      case 'a': // auto-attack
        if(P.flags & dfAutoAttack) {
          P.flags |= dfAutoON;
          addstr("Auto-attack activated! Select the weapon yourself to deactivate it.");
          }
        else {
          informAlt(2);
          if(P.altkeys) addMessage("Auto-attack cheat has to be turned on (ESC-d)");
          }
        break;
        
      case 'O':
        if(debugon()) {
          totalKnowledge();
          break;
          }
      
      case 'P':
        if(debugon()) {
          FILE *f = fopen("quaketable.txt", "wt");
          for(int c=0; c<15; c++) {
            fprintf(f, "color %s:\n", cinf[c].wname.c_str());
            int lastt = -1;
            for(int q=1; q<COLLAPSE; q++) {
              int ct = (quakefun(q, c)+(q-1)) / q;
              if(ct != lastt) lastt = ct, fprintf(f, "%6d %4d\n", q, ct);
              }
            }
          addMessage("Quaketable written to a file.");
          }
        break;
      
      case 'K':
        if(debugon()) {
          addMessage("Kill all Hydras!");
          while(!hydras.empty()) 
            M[hydras[0]->pos].hydraDead(NULL);
          for(int y=0; y<SY; y++) for(int x=0; x<SX; x++) if(M.m[y][x].type == CT_STAIRDOWN)
            playerpos = vec2(x,y);
          if(P.curHP < 1) P.curHP = 1;
          }
        break;

      case 'Y': 
      
        if(debugon()) for(int i=0; i<100; i++) {

          while(!hydras.empty()) 
            M[hydras[0]->pos].hydraDead(NULL);

          twin = NULL;
          hydras.resize(0);
          P.curlevel++;
          generateLevel();
          
          if(buggylevel) break;
          }
        break;
      
      /* case 'Z':
        if(debugon()) trailer();
        break; */

      case 'F':
        if(debugon()) {
          P.flags ^= dfSeeAll;
          }
        break;
      
      case 'H': if(debugon()) {
        addMessage("Summon what kind of hydra?"); drawScreen();
        int c = ghch(IC_CALL);
        hydra *h = NULL;
        
        int arr[3] = { WT_BLUNT, WT_AXE, WT_BLADE };

        if(c >= '0' && c <= '9') {
          h = new hydra(c - '0', headask(), 10, 20);
          }
        else switch(c) {
          case 'v': h = new hydra(HC_VAMPIRE, headask(), 10, 20); break;
          case 'a': 
            h = new hydra(HC_ALIEN, headask(), 1, 20);
            for(int i=0; i<COLORS; i++) h->res[i] = i == HC_OBSID ? -2 : 0;
            break;
          case 'i':
            h = new hydra(HC_GROW, headask(), 1, 20);
            break;
          case 'w':
            h = new hydra(HC_WIZARD, headask(), 10, 20);
            break;
          case 'e':
            h = new hydra(HC_ETTIN, 2, 1, 0);
            h->ewpn = new weapon(randSCol(), headask(), arr[rand() % 3]);
            break;
          case 'm':
            h = new hydra(HC_MONKEY, headask(), 1, 0);
            break;
          case 's':
            h = new hydra(HC_SHADOW, headask(), 1, 20);
            break;
          case 'b':
            h = new hydra(rand() % HCOLORS, headask(), 1, 20);
            h->dirty = 2*IS_DIRTY-1;
            break;
          case 'x':
            h = new hydra(HC_ANCIENT, headask(), 1, 20);
            break;
          case 'd':
            h = new hydra(HC_DRAGON | (rand() % HCOLORS), headask(), 1, 20);
            break;
          }
        if(h) {h->put(); addMessage("Summoned "+h->name()+" to a random location.");}
        else addMessage("No such thing.");
        break;
        }
      
      case 'c': // twin control
        if(twinAlive()) {
          if(P.active[IT_PAMBI]) {
            int mywpn = bitcount(P.ambiArm & P.twinarms);
            if(mywpn * 2 == bitcount(P.ambiArm) && neighbor(playerpos, twin->pos)) {
              int set = P.ambiArm;
              while(set) {
                int i = 0, j = 0;
                while(!havebit(set & P.twinarms, i)) i++;
                while(!havebit(set & ~P.twinarms, j)) j++;
                swap(wpn[i], wpn[j]);
                set ^= (1<<i) | (1<<j);
                }
              addMessage("You exchange weapons with your twin.");
              moveHydras();
              }
            else {
              addMessage("You have to control both twins while ambidextrous.");
              if(neighbor(playerpos, twin->pos))
                addMessage("Select an equal number of weapons to exchange them.");
              else
                addMessage("Twins are too far away to trade weapons.");
              }
            continue;
            }
          else switch(P.twinmode) {
            case 0:
              P.twinmode = 1;
              addMessage("You now control both twins.");
              break;
            case 1:
              P.twinmode = 0;
              if(P.twinsNamed)
                addMessage("You now control only "+twinName(0)+".");
              else
                addMessage("You now control one twin.");
              break;
            case 2:
              P.twinmode = 0;
              if(P.twinsNamed)
                addMessage("You now control only "+twinName(0)+".");
              else
                addMessage("You now control one twin.");
              twinswap(); singlestep(); twinswap();
              break;
              }
            }
          else if(P.race == R_TWIN)
            addMessage("You no longer can do that...");
          else {
            addMessage("You suddenly feel an urge to control.");
            informAlt(1);
            }
          break;
      
      case 'p':
        P.simplehydras = !P.simplehydras;
#ifdef NOTEYE
        if(P.simplehydras)
          addMessage("Using pretty simplistic (but also pretty clear) hydra pictures.\n");
        else
          addMessage("Using procedurally-generated hydra animations.\n");
#else
        addMessage("This option works only in NotEye.\n");
#endif
        break;
        
      case 's': // move other twin
        
        if(P.twinmode == 2) 
          addMessage(twinName(1,1)+" has already moved.");
        else if(twin) {
          if(!phaseswappable()) {
            addMessage("Execute all moves first.");
            break;
            }
          twinswap_phase(); addMessage("Order switched.");
          }
        else if(twinAlive()) {
          addMessage(twinName(1,1)+" not here yet.");
          }
        else if(P.race == R_TWIN) {
          addMessage("You feel you have lost a part of your life...");
          }                                                      
        else {
          addMessage("You suddenly feel very lonely...");
          informAlt(1);
          }
        break;
      
      case '=':
        P.altkeys = !P.altkeys;
        if(P.altkeys) {
          addMessage(s0 + "Using keyboard layout #2 (" + getLayoutName() + ").");
          }
        else
          addMessage("Using keyboard layout #1 (VI keys: YUHJKLBN).");
        break;
      
      case 'w':
        informAlt(2);
        break;
      
      case 'n': case 'j':
        addMessage("Press '=' to move with VI keys.");
        break;
      
      case 'o':
        exploreOn = !exploreOn;
        break;
        
      default:
        if(ch >= 'a' && ch <= 'z')
          addMessage("Unknown key pressed. Press F1 or ? to get help.");
        break;
        
      }
    }
  }

