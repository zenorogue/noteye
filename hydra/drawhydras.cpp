#include "../src/noteye.h"
#define main hydraMain // SDL redefines this

using namespace std;

int hydranoteyeflag;

int hydrapic_col;
int hydrapic_tail;
int hydrapic_id;

int hmap[32][32];

#define PRIME 1009
int rseq[PRIME];
int fseq[PRIME];

int frand(int fidx, int k) { return fseq[(fidx & 0xFFFFFF) % PRIME] % k; }
int nrand0(int ridx) { return rseq[(ridx & 0xFFFFFF) % PRIME]; }
int nrand(int ridx, int k) { return nrand0(ridx) % k; }

void drawline(int x0, int y0, int x1, int y1, int qx, int qy, int lev, int ridx) {
  if(abs(x1-x0) > 4 || abs(y1-y0) > 4) {
    int xm = (x1+x0+nrand(ridx+0, 2))/2+nrand(ridx+1, 2)-nrand(ridx+2, 2);
    int ym = (y1+y0+nrand(ridx+3, 2))/2+nrand(ridx+4, 2)-nrand(ridx+5, 2);
    if(abs(x1-x0) > 8 || abs(y1-y0) > 8)
      xm += nrand(ridx+6, 2) - nrand(ridx+7, 2),
      ym += nrand(ridx+8, 2) - nrand(ridx+9, 2);
    drawline(x0, y0, xm, ym, qx, qy, lev, ridx*11);
    drawline(xm, ym, x1, y1, qx, qy, lev, ridx*13);
    }  
  for(int k=0; k<=5; k++) 
  for(int ax=0; ax<qx; ax++) for(int ay=0; ay<qy; ay++) {
    int y = y0+((y1-y0)*k+2)/5+ay-qy/2;
    int x = x0+((x1-x0)*k+2)/5+ax-qx/2;
    if((x|31) == 31 && (y|31) == 31)
    hmap[y][x] = hydrapic_tail;
    }
  }

int nhead;

int headx[640], heady[640], headr[640];

SDL_Surface *hydrabox, *hydraheads, *hydraheads2;

int *hydraboxc; // 'changes' field which keeps track of updates

static int ZZ;
static int& qpixel(SDL_Surface *surf, int x, int y) {
  if(x<0 || y<0 || x >= surf->w || y >= surf->h) return ZZ;
  char *p = (char*) surf->pixels;
  p += y * surf->pitch;
  int *pi = (int*) (p);
  return pi[x];
  }

#define HEADX 12
#define HEADY 15

int headkey;

void prepareheads() {
  for(int id=0; id<ANIM_MAX; id++)
  for(int ky=0; ky<HEADY; ky++) for(int kx=0; kx<HEADX; kx++) {
    hydrapic_col = id < HYDRAS ? getVGAcolorX(hyinf[id].color) : 0xFF0000;
    hydrapic_col &= 0xF0F0F0;
    hydrapic_col >>= 4;
    
    int dx = id*HEADX+kx, dy = ky;

    int hdc = qpixel(hydraheads, dx, dy);
    int& hdc2 = qpixel(hydraheads2, dx, dy);
    headkey = qpixel(hydraheads, 0, 0);

    if(hdc != (int) 0xFF008080 && hdc != (int) 0xFF007F7F) {
      hdc -= 0xFF000000;
      if(hdc > 9 && hdc < 15) hdc = 9;
      hdc2 = (hdc && hdc <= 9 ? (hdc+6) * hydrapic_col : hdc);
      }
    else hdc2 = headkey;
    }
  }

void drawhead(int x, int y, int ridx, int hid) {
  x -= 5; y -= 5;
  int by = 0;
  int hm = 0;
  for(int u=0; u<16; u++) hm += nrand0(ridx+u);
  
  int lcc = hrandpos() & 0xFF030303;
  
  if(hydranoteyeflag & 1) {
    if(hid % 3 == 1) by = HEADY;
    }
  else
    if(hm % 4 == 0) by = HEADY;
  for(int ky=0; ky<HEADY; ky++) for(int kx=0; kx<HEADX; kx++) {
    int hdc = qpixel(hydraheads, hydrapic_id*HEADX+kx, by+ky);
    // for some reason, on Mac it is 0xFF007F7F, not 0xFF008080
    if(hdc != (int) 0xFF008080 && hdc != (int) 0xFF007F7F) {
      hdc -= 0xFF000000;
      int ax = x+kx, ay = y+ky;
      // another Mac weirdness
      if(hdc > 9 && hdc < 15) hdc = 9;
      if((ax|31) == 31 && (ay|31) == 31) 
        hmap[ay][ax] = (hdc && hdc <= 9 ? (hdc+6) * hydrapic_col : hdc) ^ lcc;
      }
    }
  }

int dx[4] = {1,0,-1,0};
int dy[4] = {0,1,0,-1};

void drawshadows() {
  for(int x=0; x<32; x++) for(int y=0; y<32; y++) for(int k=0; k<4; k++) {
    int ax = x + dx[k], ay = y + dy[k];
    if((ax|31) == 31 && (ay|31) == 31)
    if(hmap[y][x] == 0x8080 && hmap[ay][ax] != 0x8080 && hmap[ay][ax] != 0)
      hmap[y][x] = 0;
    }
  }

void createTree() {
  }

int tlast = 0;

#include <math.h>

int nrsin(int x0, int x1, int ridx) {
  int nrd = 0;
  for(int t=0; t<40; t++) nrd += nrand0(ridx+t*t);
  for(int t=0; t<30; t++) nrd -= nrand0(ridx+t*t+40);
  if(nrd<0) nrd=-nrd;
  float f = (nrd%25) / 25.0;
  f = f * 2;
  if(f>1) f = 2-f;
  int xr = x0 + int((x1-x0) * f);
  return xr;
  }

void splitoff(int sx, int sy, int dx0, int dy0, int dx1, int dy1, int q, int lev, int ridx, int fidx, int flag) {
  if(q == 1) {
    int spx = nrsin(dx0, dx1, ridx);
    int spy = nrsin(dy0, dy1, ridx+70);
    drawline(sx, sy, spx, spy, 2, 2, lev, ridx);
    headx[nhead] = spx; heady[nhead] = spy; headr[nhead] = ridx;
    nhead++;
    }
  else if(flag) {
    int sa = 1;
    for(int t=2; t<q; t++) if(frand(fidx+t, 2)) sa++;
    int spx = dx0; for(int t=dx0; t<dx1; t++) if(nrand(ridx+t, 2)) spx++;
    int spy = dy1; for(int t=dy0; t<dy1; t++) if(nrand(ridx+t+100, q) == 0) spy--;
    drawline(sx, sy, spx, spy, 2, 2, lev, ridx*17);
    splitoff(spx, spy, dx0, dy0, spx, spy, sa, lev, 6+ridx*23, 5+fidx*23, !flag);
    splitoff(spx, spy, spx, dy0, dx1, spy, q-sa, lev, ridx*29, fidx*29, !flag);
    }
  else {
    int sa = 1;
    for(int t=2; t<q; t++) if(frand(fidx+t, 2)) sa++;
    int spx = dx0; for(int t=dx0; t<dx1; t++) if(nrand(ridx+t, 2)) spx++;
    int spy = dy1; for(int t=dy0; t<dy1; t++) if(nrand(ridx+t, 4)/2) spy--;
    drawline(sx, sy, spx, spy, 3, 1, lev, ridx*17);
    splitoff(spx, spy, dx0, dy0, dx1, spy, sa, lev, 6+ridx*23, 5+fidx*23, !flag);
    splitoff(spx, spy, dx0, spy, dx1, dy1, q-sa, lev, ridx*29, fidx*29, !flag);
    }
  }

void splitdown(int dx0, int dy0, int dx1, int dyl, int dyr, int ex0, int ex1, int q, int lev, int ridx, int fidx, int splits) {
  int sa = q;
  
  if(splits > 0) {
    // for(int t=1; t<q; t++) printf("%d ", frand(fidx+t, 16)); printf("\n");
    for(int t=1; t<q; t++) if(frand(fidx+t, 2)) sa--;
    }
  // if(q==6) printf("%d -> %d\n", q, sa);
  
  if(dx1 == dx0+1) {
    int width = 1 + (28-dx1) / 6;
    if(width > 3) dyl--, dyr--;
    drawline(dx0, dyl, dx1, dyr, 1, width, lev, ridx);
    if(q) splitoff(dx0, dyl, ex0, dy0, ex1, dyr-2, q, lev, ridx, fidx, true);
    return;
    }

  int spx = dx0+1; for(int t=dx0+2; t<dx1; t++) if(frand(fidx+t, 2)) spx++;
  
  int spy = nrand(ridx, 2) ? dyl : dyr; 
  if(nrand(ridx+1, 2) && dx1-dx0 > 4 && dx1-dx0 < 8) spy --;
  
  int exs = ex1; if(splits > 0) for(int t=ex0; t<ex1; t++) if(nrand(ridx+t, 2)) exs--;
  
  splitdown(dx0, dy0, spx, dyl, spy, ex0, exs, sa, lev, ridx*3, fidx*3, splits-1);
  splitdown(spx, dy0, dx1, spy, dyr - (spx > 24 ? nrand(ridx+3, 2) : 0), exs, ex1, q-sa, lev, 4+ridx*7, 4+fidx*7, splits-1);
  }

void hydrapicDraw(hydra *H) {
  nhead = 0; 
  hydrapic_col = getVGAcolorX(H->gcolor());
  hydrapic_col &= 0xF0F0F0;
  hydrapic_col >>= 4;
  hydrapic_tail = hydrapic_col * 10;
  hydrapic_id = H->color & HC_DRMASK;
  
  // no special head
  if(H->dirty & IS_DIRTY) hydrapic_id = HC_ETTIN; 

  H->setgfxid();
  
  int id = H->gfxid;
  
  for(int y=0; y<32; y++) for(int x=0; x<32; x++)
    hmap[y][x] = 0x8080;

  int id2 = P.curlevel * 64 + id;
  int hds = H->heads;
  int splits = 1;
  if(hds >= 100000) splits++;
  if(hds >= 10000) splits++;
  if(hds >= 1000) splits++;
  if(hds >= 100) hds = 100, splits+=2;
  else if(hds > 33) splits++;
  
  if(H->color == HC_ALIEN)
    splitdown(4, 4, 28, 29, 29, 4, 28, hds, 1, id2, id2, splits);
  else if(H->color == 2)
    splitdown(4, 4, 28, 29, 29, 12, 28, hds, 1, id2, id2, splits);
  else
    splitdown(4, 4, 28, 29, 29, 4, 28, hds, 1, id2, id2, splits);

  drawshadows();
  for(int h=0; h<nhead; h++) 
    drawhead(headx[h], heady[h], headr[h], nhead-h);
  
  // fire hydras
  if(hydrapic_id == 2) {
    for(int x=0; x<32; x++) {
      int flame = 0;
      for(int y=31; y>=0; y--) {
        if(hmap[y][x] != 0x8080) flame = 7;
        else if(flame) {
          flame -= rand() % 4;
          int flames[8] = {0, 0x400000, 0x800000, 0xC00000, 0xFF0000, 0xFF8000, 0xFFFF00, 0xFFFF00};
          if(flame < 0) flame = 0;
          hmap[y][x] = flames[flame];
          }
        }
      }
    }

  // ice hydras
  if(hydrapic_id == 7) {
    for(int x=0; x<32; x++) {
      int flame = 0;
      for(int y=31; y>=0; y--) {
        if(hmap[y][x] != 0x8080) flame = 1;
        else if(flame) {
          hmap[y][x] = 0xFFFFFF;
          flame = 0;
          }
        }
      }
    }

  for(int y=0; y<32; y++)
  for(int x=0; x<32; x++)
    qpixel(hydrabox, id*32+x, y) = hmap[y][x];
  
  (*hydraboxc)++;
  }

bool hydrapicInit = false;

void hydrapicAnimate(int i) {
  if(!hydrapicInit) {
    for(int u=0; u<PRIME; u++) fseq[u] = rand();
    for(int u=0; u<PRIME; u++) rseq[u] = rand();
    hydrapicInit = true;
    }
  else while(i--) rseq[rand() % PRIME]++;
  }

struct animinfo {
  int t;
  double dx;
  double dy;
  int headid;
  int color;
  };

void addAnimation(cell *c, int headid, int cutcount, int color = 0) {
  int cclimit = 10;
  while(cutcount > cclimit) { cutcount /= 2; cclimit++; if(cutcount<cclimit) cutcount = cclimit; }
  while(cutcount--) {
    animinfo ai;
    ai.t = SDL_GetTicks();
    ai.dx = randf(-1, 1);
    ai.dy = randf(-1, 1);
    ai.headid = headid;
    ai.color = color;
    c->animations.push_back(ai);
    }
  }

Image *headimage;
Tile *headtile[ANIM_MAX];

double r64(double z) { return floor(z * 64 + .5) / 64; }

Tile *getAnimation(cell *c) {
  Tile *res = nullptr;
  int d = 0;
  int t = SDL_GetTicks();
  for(; d<size(c->animations); d++) {
    animinfo& ai = c->animations[d];
    if(ai.t < t - 250) {
      c->animations[d] = c->animations[size(c->animations)-1];
      c->animations.resize(size(c->animations)-1);
      d--; continue;
      }
    
    double tt = (t - ai.t) / 250.0;
    
    if(!headtile[ai.headid]) {
      headtile[ai.headid] = 
        addTransform(
         addTile(headimage, ai.headid * HEADX, 0, HEADX, HEADY, headkey),
         (32-HEADX)/64., (32-HEADY)/64., HEADX/32., HEADY/32., 0, 0);
      // printf("headtile %d = %d\n", ai.headid, headtile[ai.headid]);
      }
    
    auto h = headtile[ai.headid];
    h = addLayer(h, 1);
    if(ai.color) h = addRecolor(h, getVGAcolor(ai.color), recDefault);
    h = addTransform(h, r64(ai.dx*tt), r64(ai.dy*tt), 1, 1, 0, 0);
    res = addMerge(res, h, false);
    }
  return res;
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

void clearMapCache() {
  gmapcache.clear();
  // also clear the animations
  for(int y=0; y<MSY; y++) for(int x=0; x<MSX; x++)
    M.m[y][x].animations.clear();
  M.out.animations.clear();
  }

// return the player coordinates (internal and on-screen)
void getcoordsLua(lua_State *L) {
  noteye_table_new(L);
  noteye_table_setInt(L, "intx", playerpos.x);
  noteye_table_setInt(L, "inty", playerpos.y);
  noteye_table_setInt(L, "scrx", topx == TBAR ? playerpos.x : center.x);
  noteye_table_setInt(L, "scry", topy == TBAR ? playerpos.y : center.y);
  noteye_table_setInt(L, "topx", topx);
  noteye_table_setInt(L, "topy", topy);
  if(P.race == R_TWIN && P.twinmode && twin) {
    vec2 twinpos = playerpos + pickMinus(twin->pos, playerpos);
    noteye_table_setInt(L, "twinx", twinpos.x);
    noteye_table_setInt(L, "twiny", twinpos.y);
    }
  // more information
  noteye_table_setInt(L, "level", P.curlevel);
  if(canGoDown())
    noteye_table_setInt(L, "cangodown", 1);
  noteye_table_setInt(L, "hitpoints", P.curHP);
  extern int current_context;
  noteye_table_setInt(L, "context", current_context);
  noteye_table_setInt(L, "flags", P.flags);
  return;
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
  int uu = (((v.x+3*v.y) % 5)+5)%5;
  
  bool hvis =
    (c.h && (c.h->visible() ? c.seen : seeallmode()));
    
  Tile *anim = getAnimation(&c);

  if(!onplayer && !hvis && !anim && !(mode == 8 && (c.mushrooms || c.it || c.dead))) {
    long long cacheid = shadow&15;
    cacheid <<= 3; cacheid += uu;
    /* printf("cache x=%d y=%d (%d-%d) (%d-%d) uu=%d\n", v.x, v.y, 
      playerpos.x, center.x, playerpos.y, center.y, uu); */
    cacheid <<= 1; if(c.type == CT_HEXOUT) cacheid++;
    cacheid <<= 1; if(c.ontarget) cacheid++;
    if(c.explored) {
      cacheid <<= 6; if(c.mushrooms) cacheid |= 1+hydraiconid(c.mushrooms);
      cacheid <<= 7; if(c.it) cacheid |= c.it->iconExtended();
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
    int col = P.race == R_NAGA ? 14 : P.race == R_CENTAUR ? 12 : 
      P.race == R_TROLL ? 11 :
      P.race == R_ATLANTEAAN ? 10 :
      15;
    noteye_table_setInt(L, "hcolor", getVGAcolor(col));
    }
  
  noteye_table_setInt(L, "shadow", shadow);
  noteye_table_setInt(L, "uu", uu);

  if(c.type == CT_HEXOUT)
    noteye_table_setInt(L, "hex", 1);
    
  if(c.ontarget && &c != &M.out)
    noteye_table_setInt(L, "target", 1);
  
  if(anim)
    noteye_table_setInt(L, "animation", noteye_get_handle(anim));

  if(c.explored) {
  
    if(hvis) {
      noteye_table_setInt(L, "hcolor", getVGAcolorX(c.h->gcolor()));
      noteye_table_setInt(L, "hicon", c.h->icon());
      noteye_table_setInt(L, "hid", c.h->uid);
      if(c.h->color == HC_MONKEY)
        noteye_table_setInt(L, "monkey", c.h->ewpn ? 2 : 1);
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
      noteye_table_setInt(L, "iicon", c.it->iconExtended());
      if(c.it->asWpn()) noteye_table_setInt(L, "itype", c.it->asWpn()->type);
      }
    
    noteye_table_setInt(L, "floor", ".#<>" [floorid]);
    
    if(c.dead == HC_TWIN+1) noteye_table_setInt(L, "dead", -1);
    else if(c.dead) noteye_table_setInt(L, "dead", getVGAcolorX(hyinf[c.dead-1].color));
    
    if(c.seen) noteye_table_setInt(L, "seen", 1);
    }
  }

#ifdef NOTEYE
int current_context;

int ghch(int context) { current_context = context; return noteye_getch(); }

struct soundtoplay {
  int type;
  int vol, delay; const char *sfname;
  int hid; vec2 v;  
  };

vector<soundtoplay> soundqueue;

void playSound(const char *fname, int vol, int msToWait) {
  if(!fname) return;
  soundtoplay P;
  P.type = 0; P.vol = vol; P.delay = msToWait; P.sfname = fname;
  soundqueue.push_back(P);
  }

void sendSwapEvent() {
  soundtoplay P;
  P.type = 1;
  soundqueue.push_back(P);
  }

void sendAutoexploreEvent() {
  soundtoplay P;
  P.type = 3;
  soundqueue.push_back(P);
  }

void sendAttackEvent(int hid, vec2 from, vec2 to) {
  soundtoplay P;
  P.type = 2; P.v = pickMinus(to, playerpos) - pickMinus(from, playerpos); P.hid = hid;
  soundqueue.push_back(P);
  }

int lh_getSounds(lua_State *L) {
  noteye_table_new(L);
  for(int i=0; i<size(soundqueue); i++) {
    soundtoplay& P(soundqueue[i]);
    noteye::noteye_table_opensubAtInt(L, i);
    if(P.type == 0) {
      noteye_table_setStr(L, "sound", P.sfname);
      noteye_table_setInt(L, "vol", P.vol);
      noteye_table_setInt(L, "delay", P.delay);
      }
    else if(P.type == 1) {
      noteye_table_setInt(L, "swap", 1);
      }
    else if(P.type == 3) {
      noteye_table_setInt(L, "autoexplore", 1);
      }
    else if(P.type == 2) {
      if(P.hid == ATT_TWIN)
        noteye_table_setStr(L, "hid", "twin");
      else if(P.hid == ATT_PLAYER)
        noteye_table_setStr(L, "hid", "player");
      else noteye_table_setInt(L, "hid", P.hid);
      noteye_table_setInt(L, "dx", P.v.x);
      noteye_table_setInt(L, "dy", P.v.y);
      }
    noteye_table_closesub(L);
    }
  soundqueue.clear();
  return 1;
  }

void drawHydraBak() {
  Image *i = new Image(1920, 1440, 0);
  
  for(int u=0; u<2; u++) {

  for(int y=0; y<1440; y+=32) for(int x=0; x<1920; x+=32) {
    while(!hydras.empty()) 
      M[hydras[0]->pos].hydraDead(NULL);
    int hbcol[14] = { HC_ALIEN, HC_GROW, HC_ANCIENT, HC_WIZARD, HC_SHADOW, HC_VAMPIRE };
    int hdcnt[12] = {1,2,3,4,5,6,8,12,16,64,256,512};
    
    int col = ((x+3*y)>>5)%10;
    if(u ==1 && hrand(100) < 25) col = hbcol[hrand(6)];
    
    int siz = hdcnt[hrand(9)];
    
    if(col == HC_ANCIENT || col == HC_GROW || col == HC_ANCIENT || col == HC_VAMPIRE)
      siz = 64 * (1 +hrand(4));

    (new hydra(col, siz, 10, 20))->put();
    hydrapicInit = false;
    hydras[0]->setgfxid();
    hydrapicAnimate(0);
    hydrapicDraw(hydras[0]);
    int sh = 32*hydras[0]->gfxid;
    if(hydras[0]->color == HC_SHADOW) sh = 0;
    // printf("%d %d id=%d\n", y, x, hydras[0]->gfxid);
    for(int dy=0; dy<32; dy++) for(int dx=0; dx<32; dx++) {
      int p = qpixel(hydrabox, sh+dx, dy);
/*      if(p == 0x8080)
        p = (dx==0 || dy == 0 || dx == 31 || dy == 31) ? 0 :
            (dx==1 || dy == 1 || dx == 30 || dy == 30) ? 0x402010 : 0x804020; */
      qpixel(i->s, x+dx, y+dy) = p;
      }
    }
  
  SDL_SaveBMP(i->s, u ? "bighydras.bmp" : "smallhydras.bmp");
  }
  delete i;
  }

#endif

