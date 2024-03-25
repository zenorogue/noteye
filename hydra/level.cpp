// Hydra Slayer: math puzzle roguelike
// Copyright (C) 2010-2011 Zeno Rogue, see 'hydra.cpp' for details

// geometry

void setDirs() {
  DIRS = P.geometry;
  if(DIRS == 3) DIRS = 8 - 2 * (P.curlevel % 3);
  if(DIRS == 4) dirs = dirs4;
  if(DIRS == 6) dirs = dirs6;
  if(DIRS == 8) dirs = dirs8;
  if(DIRS == 16) dirs = dirs16, DIRS = 8;
  }

// *topology*

#define TBAR  0 // barrier
#define TLOOP 1 // orientable
#define TMOB  2 // non-orientable;

int topx, topy; // currenty topology type for X and Y

// *utilities*

bool inbound(vec2 v) { return v.x >= 0 && v.y >= 0 && v.x < SX && v.y < SY; }
bool inlevel(vec2 v) { 
  if(DIRS == 6 && topx == TMOB)
    return v.y > 0 && v.y < SY-1;
  if(DIRS == 6 && topy == TMOB && topx == TBAR)
    return v.x > 1 && v.x < SX-2;
  if(DIRS == 6 && topy == TMOB && topx == TLOOP)
    return v.x > 0 && v.x < SX-1;
  if(DIRS == 6 && topx == TLOOP)
    return v.y > 0 && v.y < SY-1;
  if(DIRS == 6)
    return v.x > 1 && v.y > 0 && v.x < SX-2 && v.y < SY-1;
  return v.x > 0 && v.y > 0 && v.x < SX-1 && v.y < SY-1;
  }

vec2 randVec() { return vec2(hrand(SX), hrand(SY)); }

#define GLEVELS ((P.flags & dfChallenge) ? CLEVELS : LEVELS)
#define MLEVELS (LEVELS>CLEVELS?LEVELS:CLEVELS)

vector<sclass*> toput[MLEVELS];

int levseed[MLEVELS];

vec2 findEmpty() { while(true) { vec2 v = randVec(); if(M[v].isEmpty()) return v; } }

#define QTAB (MSX*MSY)

int uftab[QTAB];

void fuInit() { for(int y=0; y<QTAB; y++) uftab[y] = y; }

int ufind(int a) {
  if(uftab[a] == a) return a; else return uftab[a] = ufind(uftab[a]);
  }

void funion(int a, int b) {
  uftab[ufind(a)] = ufind(b);
  }

int vindex(vec2 v) { return v.y * SX + v.x; }

vec2 wrap(vec2 v) {
  if(topx == TLOOP) {
    while(v.x >= SX-1) v.x -= SX-2;
    while(v.x <= 0)    v.x += SX-2;
    }
  if(topx == TMOB) {
    int wr = DIRS == 6 ? SX : SX-1;
    while(v.x >= wr) v.x -= wr-1, v.y = SY-1-v.y;
    while(v.x <= 0)  v.x += wr-1, v.y = SY-1-v.y;
    }
  if(topy == TLOOP) {
    while(v.y >= SY-1) v.y -= SY-2;
    while(v.y <= 0)    v.y += SY-2;
    }
  if(topy == TMOB) {
    int wr = DIRS == 6 ? SY : SY-1;
    while(v.y >= wr) v.y -= wr-1, v.x = SX-1-v.x;
    while(v.y <= 0)  v.y += wr-1, v.x = SX-1-v.x;
    }
  return v;
  }

int neighborDir(vec2 v, vec2 w) {
  w = wrap(w); v = wrap(v);
  for(int i=0; i<DIRS; i++) if(wrap(v+dirs[i]) == w) return i;
  return -1;
  }

bool neighbor(vec2 v, vec2 w) {
  return neighborDir(v,w) > -1;
  }

vec2 center(SX/2, SY/2);

vec2 pickMinus(vec2 v, vec2 w) {
  // pick the best line from v to w
  vec2 bestdif = v-w; int bestlen = len(v-w);
  v = wrap(v);
  for(int wx=w.x-SX; wx<=w.x+SX; wx++) for(int wy=w.y-SY; wy<=w.y+SY; wy++) {
    vec2 v0 = vec2(wx,wy);
    if(wrap(v0) == v && len(v0-w) < bestlen) bestdif = v0-w, bestlen = len(v0-w);
    }
  return bestdif;
  }

void sclass::putOn(vec2 pos) {
  switch(sct()) {
    case SCT_ITEM: case SCT_WPN:
      M[pos].it = this;
      // the mushroom staff is generated with a mushroom ring
      if(sct() == SCT_WPN && asWpn()->type == WT_FUNG) {
        for(int i=0; i<DIRS; i++) {
          cell& c(M[pos+dirs[i]]);
          if(c.type == CT_EMPTY && !c.h && !c.it)
            c.mushrooms = 60;
          }
        }
      // traps are generated in empty areas
      if(asWpn() && (asWpn()->wpnflags & wfTrap)) for(int i=0; i<8; i++) {
        if(!inlevel(pos+dirs[i])) continue;
        cell& c(M[pos+dirs[i]]);
        if(c.type == CT_WALL) {
          c.type = CT_EMPTY;
          c.mushrooms = 1 + hrand(3);
          }
        }
      break;
    
    case SCT_HYDRA:
      hydra* h = (hydra*) this;
      M[h-> pos = pos].h = h;      

      if(h->color == HC_TWIN) {
        twin = h;
        h->zombie = true;
        hydras.insert(hydras.begin(), twin);
        }
      else hydras.push_back(h);

      if(h->color == HC_ETTIN) {
        h->ewpn->level = -11-P.curlevel;
        }
      break;
    }
  }

void sclass::put() {
  putOn(findEmpty());
  }

// BFS and LOS

void seeVec(vec2 v) {
  int obs = 0;
  int maxobs = P.race == R_ELF ? 4 : 2;
  for(int u=0; u<=64; u++) {
    if(DIRS == 4 && u) {
      vec2 vo = v*(u-1) / 64;
      vec2 vn = v*u / 64;
      cell& c1(M[playerpos+vec2(vo.x,vn.y)]);
      cell& c2(M[playerpos+vec2(vn.x,vo.y)]);
      if(vo.x != vn.x && vo.y != vn.y && 
        c1.type == CT_WALL && c2.type == CT_WALL) {
        c1.seen = true; c2.seen = true;
        break;
        }
      }
    cell& c(M[playerpos + (v * u) / 64]);
    c.seen = true;
    if(!c.explored) {
      c.explored = true;
      if(c.it) {
        exploreOn = false;
        addMessage("You find "+c.it->getname()+".");
        }
      }
    if(c.mushrooms) obs++;
    if(c.type == CT_WALL) {
      if(P.geometry==16) obs++; else obs+=20;
      }
    if(obs >= maxobs) break;
    }
  }

void seeall() {
  if(DIRS == 6) {
    for(int y=0; y<=64; y++) {
      seeVec(vec2(+64+y,   +64-y));
      seeVec(vec2(+64-2*y, +64));
      seeVec(vec2(-64-y,   +64-y));
      seeVec(vec2(+64+y,   -64+y));
      seeVec(vec2(+64-2*y, -64));
      seeVec(vec2(-64-y,   -64+y));
      }
    }
  else for(int y=-64; y<=64; y++) {
    seeVec(vec2(y,-64));
    seeVec(vec2(y,+64));
    seeVec(vec2(-64,y));
    seeVec(vec2(+64,y));
    }
  }

void los() {
  if(seeallmode()) {
    for(int y=0; y<SY; y++) for(int x=0; x<SX; x++) 
      M.m[y][x].seen = M.m[y][x].explored = true;
    return;
    }
  for(int y=0; y<SY; y++) for(int x=0; x<SX; x++) 
    M.m[y][x].seen = false,
    M.m[y][x].ontarget = false;
  seeall();
  if(twin) {
    twinswap();
    seeall();
    twinswap();
    }
  }

#define DANGER 1000

void bfs(int who, bool nmush) {

  int targetval = 0;
  targetdir = -1;
  if(nmush && M[playerpos].type == CT_STAIRDOWN) targetval = 1;

  for(int y=0; y<SY; y++) for(int x=0; x<SX; x++) M.m[y][x].dist = 0;
  M[playerpos].dist = 1; M[playerpos].govia = -1;
  
  vector<vec2> d; d.reserve(SX*SY);
  d.push_back(playerpos); 

  if(twin && !who) {
    M[twin->pos].dist = 1;
    d.push_back(twin->pos);
    }
  
  int qs = 0;
  while(qs < size(d)) {
    vec2 pos = d[qs++];
    int di = M[pos].dist;
    int gv = M[pos].govia;
    for(int dir=0; dir<DIRS; dir++) {
      vec2 pos2 = pos + dirs[dir];
      cell& c(M[pos2]);
      int ngv = gv >= 0 ? gv : dir;

      if(who && !c.explored && targetval < 2 && !exploreWithDestination) {
        targetval = 2;
        targetdir = ngv;
        }

      if(who && exploreWithDestination && (&c == &M[exploreDestination]) && targetval < 4) {
        targetval = 4;
        targetdir = ngv;
        }

      if(who && !c.explored && nmush && !c.dist && &c != &M.out && exploreWithDestination) {
        c.dist = di+1;
        c.govia = ngv;
        d.push_back(pos2);
        }

      if(who && !c.explored) continue;
      
      if(who && c.it && P.race != R_TROLL && c.it->asItem() && 
        c.it->asItem()->type != IT_HINT && targetval < 3 && !exploreWithDestination) {
        targetval = 3;
        targetdir = ngv;
        }
      
      if(who && c.type == CT_STAIRDOWN && targetval < 1 && nmush && !exploreWithDestination) {
        targetval = 1;
        targetdir = ngv;
        }
      
      // don't autoexplore through zombies
      if((who == 1) && c.h && c.h->zombie && !nmush) continue;

      if(((who && c.h) || c.isPassable(nmush)) && !c.dist && !(who && c.trapped())) {
        c.dist = di+1;
        c.govia = ngv;
        d.push_back(pos2);
        }
      }
    }
  
  if(who == 2) {
    for(int i=0; i<size(hydras); i++)
      if(M[hydras[i]->pos].seen && 
        !hydras[i]->zombie && 
        hydras[i]->heads > hydras[i]->sheads &&
        hydras[i]->visible()
        )
    for(int dir=0; dir<DIRS; dir++) {
      cell& c(M[hydras[i]->pos + dirs[dir]]);
      c.dist += DANGER;
      }
    }  
  }

// dungeon generator utils

void wallsAround() {
  for(int x=0; x<SX; x++) M.m[0][x].type = CT_WALL;
  for(int x=0; x<SX; x++) M.m[SY-1][x].type = CT_WALL;
  for(int y=0; y<SY; y++) M.m[y][0].type = CT_WALL;
  for(int y=0; y<SY; y++) M.m[y][SX-1].type = CT_WALL;
  }

void clearLevel() {
  M.out.clear(); M.out.type = CT_WALL;
  for(int y=0; y<SY; y++) for(int x=0; x<SX; x++) {
    M.m[y][x].clear();
    if(!inlevel(vec2(x,y))) M.m[y][x].type = CT_WALL;
    }
  }

int countEmpty() {
  int qty = 0;
  for(int y=0; y<SY; y++) for(int x=0; x<SX; x++) 
    if(M.m[y][x].type == CT_EMPTY && !M.m[y][x].mushrooms) qty++;
  return qty;
  }

void bubbleMap(int qty = 2000) {
  
  /* for(int y=0; y<SY; y++) for(int x=0; x<SX; x++)
    M.m[y][x].type = CT_WALL;

  for(int t=0; t<20; t++) {
    vec2 v = randVec();
    if(inlevel(v))
    M[v].type = CT_EMPTY;
    }
  return; */
  
  for(int t=0; t<qty; t++) {
    int norm = hrand(26);
    vec2 v = randVec();
    for(int dx=-10; dx<=10; dx++) for(int dy=-10; dy<=10; dy++) if(dx*dx+dy*dy <= norm) {
      vec2 v2 = v + vec2(dx, dy);
      if(!inlevel(wrap(v2))) continue;
      M[v2].type = t&1 ? CT_WALL : CT_EMPTY;
      }
    }
  }

void swissMap() {
  
  int qty = 5 << hrand(4);
  
  for(int y=0; y<SY; y++) for(int x=0; x<SX; x++)
    M.m[y][x].type = CT_WALL;

  for(int t=0; t<qty; t++) {
    int norm = 8000;
    vec2 v = randVec();
    for(int dx=-30; dx<=30; dx++) for(int dy=-30; dy<=30; dy++) {
      int n0 = dx*dx+dy*dy;
      if(n0 >= norm) continue;
      vec2 v2 = v + vec2(dx, dy);
      if(inlevel(wrap(v2)) && M[v2].type == M[v].type) continue;
      norm = n0;
      }

    for(int dx=-30; dx<=30; dx++) for(int dy=-30; dy<=30; dy++) {
      int n0 = dx*dx+dy*dy;
      vec2 v2 = v + vec2(dx, dy);
      if(n0 < norm) M[v2].type ^= CT_WALL ^ CT_EMPTY;
      }
    }
  }

bool incarpet(int x, int y) {
  if(x%3 == 1 && y%3 == 1) return false;
  if(x<3 && y<3) return true;
  return incarpet(x/3,y/3);
  }

void sierpinskiCarpet() {
  int rx, ry;
  do {
    rx = hrand(2000000);
    ry = hrand(2000000);
    }
  while(!incarpet(rx+1 + hrand(SX-2), ry+1+hrand(SY-2)));
  for(int y=1; y<SY-1; y++) for(int x=1; x<SX-1; x++)
    if(inlevel(vec2(x,y)))
      M.m[y][x].type = incarpet(rx+x, ry+y) ? CT_EMPTY : CT_WALL;
  }

void inverseMap() {
  for(int y=1; y<SY-1; y++) for(int x=1; x<SX-1; x++)
    if(inlevel(vec2(x,y)))
      M.m[y][x].type ^= CT_EMPTY ^ CT_WALL;
  }

vec2 cxy(vec2 a, vec2 b) { return vec2(a.x,b.y); }

vector<vec2> av;

bool buggylevel;

bool emptyMode(vec2 v, int mode) {
  if(mode == 0)
    return M[v].isEmpty() || M[v].mushrooms;
  if(mode == 1)
    return M[v].type != CT_HEXOUT && M[v].type != CT_WALL;
  return false;
  }

bool verifyConnected(int mode = 0) {
  fuInit();

  av.clear();
  
  for(int y=1; y<SY-1; y++) for(int x=1; x<SX-1; x++) {
    vec2 v(x,y);
    if(emptyMode(v, mode)) {
      av.push_back(v);
      for(int d=0; d<DIRS; d++) if(emptyMode(v+dirs[d], mode)) {
        funion(vindex(v), vindex(wrap(v+dirs[d])));
        }
      }
    }

  for(int i=1; i<size(av); i++) 
    if(ufind(vindex(av[i])) != ufind(vindex(av[0])))
      return false;
    
  return true;
  }

void drawline(vec2 from, vec2 dif, int l, int idx, bool wide) {
  int dmode;
  if(DIRS == 4 || (DIRS == 8 && hrand(10) >= 5))
    dmode = 1 + (hrand(2));
  else
    dmode = 0;

  if(dmode) l*=2;
  if(l == 0) l++;

  // playerpos = av[i]; drawMap(); ghch();
  for(int u=0; u<=l; u++) {
  
    vec2 vhb = dif * u / l;
    vec2 v0(0,0);

    if(dmode==1) vhb = (u&1) ? cxy(vhb, v0) : cxy(dif, vhb);
    if(dmode==2) vhb = (u&1) ? cxy(v0, vhb) : cxy(vhb, dif);
    
    vec2 vh = from + vhb;

    M[vh].type = CT_EMPTY;
    if(idx >= 0) funion(vindex(wrap(vh)), idx);

    if(P.geometry == 16 || wide) if(DIRS != 6) {
      for(int k=0; k<8; k++) if(inlevel(vh+dirs8[k]))
        M[vh+dirs8[k]].type = CT_EMPTY;
      }
    }
  }

void ensureConnected() {

  verifyConnected();
  
  for(int i=0; i<size(av); i++) swap(av[i], av[hrand(i+1)]);
  
  for(int i=1; i<size(av); i++) 
    if(ufind(vindex(av[i])) != ufind(vindex(av[0]))) {
      int r;
      do { r = hrand(size(av)); } while(vindex(av[r]) != vindex(av[0]));
      int l = len(av[i] - av[r]) * 2;
      
      vec2 dif = pickMinus(av[i], av[r]);
      
      if(l==0) continue;
      
      drawline(av[r], dif, l, vindex(av[0]), false);
      }
  }

#define MAXLOOP 30000

#include <set>

bool operator < (vec2 v1, vec2 v2) {
  if(v1.y != v2.y) return v1.y < v2.y;
  return v1.x < v2.x;
  }

void ensureLooped() {
  int tries = 0;
  nexttry:
  tries++; if(tries > 10) return;
  vec2 v;
  if(topx == TBAR && topy == TBAR) return;
  for(int a=0;; a++) {
    v = vec2(hrand(SX), hrand(SY));
    if(inlevel(v) && M[v].type == CT_EMPTY) break;
    if(a == 1000) return;
    }
  
  vec2 lqueue[MAXLOOP];
  int lb=0, le=0;
  set<vec2> reachable;

  reachable.insert(v); lqueue[le++] = v;
  
  while(lb < le && le < MAXLOOP) {
    vec2 vc = lqueue[lb++];
    for(int i=0; i<DIRS; i++) {
      if(le == MAXLOOP) break;
      vec2 vc2 = vc + dirs[i];
      if(M[vc2].type == CT_EMPTY && !reachable.count(vc2))
        reachable.insert(vc2), lqueue[le++] = vc2;
      }
    }
  
  for(int r=10; r<400; r++) {
    vec2 vr = vec2(hrand(r/5) - hrand(r/5), hrand(r/5) - hrand(r/5));
    vec2 v2 = v + vr;
    if(M[v2].type == CT_EMPTY && !reachable.count(v2)) {
      drawline(v, vr, len(vr)*2, -1, true);
      goto nexttry;
      }
    }
  }

void mushroomLines(int qty, int msize) {
  for(int i=0; i<qty; i++) {
    if(countEmpty() < 100) break;
    vec2 v = findEmpty();
    vec2 w = findEmpty();
    int l = len(v-w) * 3;
    vec2 dif = pickMinus(v,w);
    if(l == 0) continue;
    for(int u=0; u<=l; u++) {
      vec2 vh = v + dif * u / l;
      if(M[vh].isEmpty()) M[vh].mushrooms = 1 + hrand(msize);
      }
    }
  }

void mushroomRandom(int pct, int msize) {
  for(int y=1; y<SY-1; y++) for(int x=1; x<SX-1; x++) if(hrand(100) < pct)
    if(M.m[y][x].type == CT_EMPTY)
      M.m[y][x].mushrooms = 1 + hrand(msize);
  }

void roundEdges() {
  if(topx == TBAR && topy == TBAR)
  for(int y=1; y<SY-1; y++) for(int x=1; x<SX-1; x++) if(inlevel(vec2(x,y))) {
    int dx = x<SX/2 ? x : SX-1-x;
    int dy = y<SY/2 ? y : SY-1-y;
    if(dx*dy < (SX+SY)/5) M.m[y][x].mushrooms = 0, M.m[y][x].type = CT_WALL;
    }
  }

int dragonfun(int x, int y) {
  if(x==0 && y==0) return 0;
  if((x^y)&1) x--;
  
  // (x+iy / (i-1)) = ((x+iy)(i+1) / (i-1)(i+1)) = ((x-y + i(x+y)) / -2)
  int nx = (y-x)/2;
  int ny = -(x+y)/2;
  
  return 1+dragonfun(nx, ny);
  }

void dragonCurveMap() {
  int rx = hrand(10);
  int ry = hrand(3);
  for(int y=1; y<SY-1; y++) for(int x=1; x<SX-1; x++) {
    if(inlevel(vec2(x,y)))
    M.m[y][x].type = dragonfun(x-SX/2+rx, y-SY/2+ry) < 10 ? CT_EMPTY : CT_WALL;
    }
  }

typedef complex<double> comp;

void mandelbrotMap(int msize) {
  for(int y=1; y<SY-1; y++) for(int x=1; x<SX-1; x++) if(inlevel(vec2(x,y))) {
    comp z0 = 0;
    comp b((x-15*SX/20)/16.0, (y-SY/2)/8.0);
    int iter;
    int wallto = 4;
    int msize = 10;
    for(iter=0; iter<wallto+msize; iter++) {
      if(abs(z0) > 100) break;
      z0 = z0*z0 + b;
      }
    if(iter < wallto) M.m[y][x].type = CT_WALL;
    else M.m[y][x].type = CT_EMPTY, M.m[y][x].mushrooms = msize+wallto-iter;
    }

  /*
  for(int y=1; y<SY-1; y++) {
    for(int x=1; x<SX-1; x++) 
      if(M.m[y][x].type == CT_WALL) printf("#"); else 
      if(M.m[y][x].mushrooms) printf("0");
      else printf(".");
    printf("\n");
    }
  exit(0);
  */
  }

bool inMandel(comp z0, comp b) {
  for(int i=0; i<100; i++) z0 = z0*z0+b;
  return abs(z0) < 100;
  }

void juliaMap(int msize) {

  comp z0 = 0;
  comp b;
  
  while(true) {
    z0 = 0;
    b = comp (randf(-2,2), randf(-2, 2));
    if(!inMandel(z0, b)) continue;
    if(!inMandel(z0, b+comp(+.01,0))) break;
    if(!inMandel(z0, b+comp(-.01,0))) break;
    if(!inMandel(z0, b+comp(0,+.01))) break;
    if(!inMandel(z0, b+comp(0,-.01))) break;
    }
  /*
  sprintf(buf, "julia %.6f,%.6f", float(b.real()), float(b.imag()));
  extern void addMessage(string s);
  addMessage(buf);
  */
  
  for(int y=1; y<SY-1; y++) for(int x=1; x<SX-1; x++) if(inlevel(vec2(x,y))) {
    comp z1((x-SX/2)/16.0, (y-SY/2)/8.0);
    int iter;
    int wallto = 4;
    int msize = 10;
    for(iter=0; iter<wallto+msize; iter++) {
      if(abs(z1) > 100) break;
      z1 = z1*z1 + b;
      }
    if(iter < wallto) M.m[y][x].type = CT_WALL;
    else M.m[y][x].type = CT_EMPTY, M.m[y][x].mushrooms = msize+wallto-iter;
    }

  }

void lifeMap(int msize) {
  clearLevel();
  mushroomRandom(20, 1);

  for(int iter=0; iter<50; iter++) {

    for(int x=0; x<SX; x++) for(int y=0; y<SY; y++) if(inlevel(vec2(x,y))) {
      vec2 v(x,y);
      if(M[v].mushrooms&1) for(int d=0; d<8; d++)
        M[v+dirs8[d]].mushrooms += 2;
      }

    for(int x=0; x<SX; x++) for(int y=0; y<SY; y++) if(inlevel(vec2(x,y))) {
      int& m(M.m[y][x].mushrooms);
      m = (m >= 5 && m <= 7) ? 1 : 0;
      }
    }
  
  int lcount = 0;

  for(int x=0; x<SX; x++) for(int y=0; y<SY; y++) if(inlevel(vec2(x,y))) if(M.m[y][x].mushrooms) {
    lcount++;
    M.m[y][x].mushrooms = 1 + hrand(msize);
    }
  
  if(lcount < 160) lifeMap(msize);
  }

void livingcaveMap(int msize) {
  clearLevel();
  for(int y=1; y<SY-1; y++) for(int x=1; x<SX-1; x++) if(hrand(100) < 50)
    if(M.m[y][x].type == CT_EMPTY)
      M.m[y][x].type = CT_WALL;
  mushroomRandom(25, 1);

  for(int iter=0; iter<50; iter++) {

    for(int x=0; x<SX; x++) for(int y=0; y<SY; y++) if(inlevel(vec2(x,y))) {
      vec2 v(x,y);
      int walls=0, nomush=0, mush=0;
      if(M[v].type == CT_WALL) walls++;
      else if(M[v].mushrooms&1) mush++;
      else nomush++;
      for(int d=0; d<8; d++) {
        vec2 w = v + dirs8[d];
        if(inlevel(w)) {
          if(M[w].type == CT_WALL) walls+=2;
          else if(M[w].mushrooms&1) mush+=2;
          else nomush+=2;
          }
        }
      if(walls > mush + nomush)
        M[v].mushrooms |= 2;
      else if(mush >= nomush)
        M[v].mushrooms |= 4;
      }

    for(int x=0; x<SX; x++) for(int y=0; y<SY; y++) if(inlevel(vec2(x,y))) {
      vec2 v(x,y);
      M[v].type = (M[v].mushrooms & 2) ? CT_WALL : CT_EMPTY;
      M[v].mushrooms = (M[v].mushrooms & 4) ? 1 : 0;
      }
    }
  
  for(int x=0; x<SX; x++) for(int y=0; y<SY; y++) if(inlevel(vec2(x,y))) if(M.m[y][x].mushrooms)
    M.m[y][x].mushrooms = 1 + hrand(msize);
  }

void brownianMap() {
  int quota = (SX-2) * (SY-2) / 2;
  if(DIRS == 6) quota /= 2;
  vec2 at = center;
  for(int x=0; x<SX; x++) for(int y=0; y<SY; y++) 
    M.m[y][x].type = CT_WALL;
  while(quota) {
    if(M[at].type == CT_WALL) M[at].type = CT_EMPTY, quota--;
    at += dirs[hrand(DIRS)];
    while(!inlevel(wrap(at))) {
      at = randVec();
      if(DIRS == 6 && (at.y^at.x)&1) at.x++;
      }
    }
  }

void starMap() {
  vec2 v = center;
  
  for(int x=1; x<SX-1; x++) for(int y=1; y<SY-1; y++) 
    M.m[y][x].type = CT_WALL;

  for(int it=0; it<16; it++) {
    vec2 w0 = randVec();
    if(M[w0].type != CT_WALL) continue;
    if(it == 0) w0 = v;
    for(int dx=-2; dx<3; dx++)
    for(int dy=-2; dy<3; dy++) if(dx*dx+dy*dy<8) {
      vec2 w = w0+vec2(dx,dy);
      if(!inlevel(w)) continue;
      int l = len(w-v);
      if(l == 0) continue;
      for(int u=0; u<=l; u++) {
        vec2 v3 = v+((w-v)*u)/l;
        if(inlevel(v3))
          M[v3].type = CT_EMPTY;
        }
      }
    }
  }
  
int topgen() {
  int d = (P.flags & dfChallenge) ? hrand(LEVELS*2) : hrand(P.curlevel+1);
  d %= LEVELS*2;
  if(d < LEVELS) return TBAR;
  if(d < LEVELS + 6) return TLOOP;
   return TMOB;
  }

void setAnyTopology() {
  while(true) {
    topx = topgen();
    topy = topgen();
    if(topx != TMOB || topy != TMOB)
      break;
    }
  clearLevel();
  }

void setWrapTopology() {
  while(true) {
    setAnyTopology();
    if(topx != TBAR && topy != TBAR)
      break;
    }
  }

// === game itself ===

int randHCol() { return hrand(HCOLORS); }
int randSCol() { return HCOLORS + hrand(SCOLORS); }

int lseed;

bool onlev(int min, int max) {
  int d = max-min+1;
  int res = (lseed % d);
  lseed = (lseed + res * (1ll<<31)) / d;
  res += min;
  if(P.curlevel == 998) {
    // addMessage("min="+its(min)+" max="+its(max)+" res="+its(res));
    if(res < 50) return true;
    }
  return res == P.curlevel;
  }

weapon *newTrap(int col, int size, char type) {
  weapon *w = new weapon(col, size, type);
  w->wpnflags |= wfTrap;
  return w;
  }

void generateExtraMonsters() {
  int growpow = 13 + (P.curlevel - 13) / 5;
  int hydrasize = 200 + 5 * P.curlevel;
  int healing = 200;
  
  if(P.curlevel >= 14)
    healing = 150;

  if(P.curlevel >= 15)
    healing = 100;
  
  if(P.curlevel > 24)
    healing -= 10;

  if(P.curlevel > 35)
    healing -= 10;

  if(P.curlevel > 40)
    healing -= 10;

  if(P.curlevel > 49)
    healing = 100 - P.curlevel;

  if(healing < 0) healing = 0;

  int dragonmin = 25;
  // when hydras are small or big, the chance will be changed
  
  if(P.curlevel == 48)
    // second-to-last level: very huge hydras, lots of healing,
    // low chance of a dragon
    hydrasize *= 50, healing *= 6, dragonmin = 40;
  else if(P.curlevel >= 20 && onein(7))
    // huge hydras, more healing, less chance of a dragon
    hydrasize *= 10, healing *= 3, dragonmin += 5;
  else if(onein(6))
    // small hydras but less healing, also less chance of a dragon
    hydrasize /= 4, healing /= 2, dragonmin += 5;

  int dragoncolor = -1;
  if(hrand(P.curlevel) >= dragonmin)
    dragoncolor = randHCol();
  
  int dirtycolor = -1;
  if(onein(5))
    dirtycolor = randHCol();
    
  int dirtycolor2 = -1;
  if(dirtycolor != -1 && onein(2))
    dirtycolor2 = randHCol();
  
  // Hydras
  if(P.curlevel != 49) for(int c=0; c<HCOLORS; c++) {
    hydra *H = new hydra(c, hrand(hydrasize) + 1, growpow, healing / 10);
    if(c == dragoncolor) H->color |= HC_DRAGON, H->heal = healing / 8;

    if(!(P.flags & dfBackups)) {    
      if(c == dirtycolor) H->dirty = 2*IS_DIRTY-1, H->heal += healing / 10;
      if(c == dirtycolor2 && c != dirtycolor) H->dirty = 2*IS_DIRTY-1, H->heal += healing / 8;
      if(P.curlevel >= 50 && P.curlevel % 5 == 0)
        H->dirty = 2*IS_DIRTY-1, H->heal += healing / 5;
      }

    H->put();
    }

  if(P.curlevel == 49) {
    int hc = 900000 + hrand(50000);
    while(primediv(hc) != -1) hc++;
    hydra *adragon = new hydra(HC_ANCIENT | HC_DRAGON, hc, growpow, 0);
    adragon->dirty = IS_DIRTY-1; // do not know susceptibilities
    adragon->put();
    }
  else {
    int special = hrand(5);
    
    // Wizards are picked only after Level 30
    if(P.curlevel < 30)
      special = hrand(4);
  
    // Growing hydras only after Level 20 (otherwise it's free)
    if(P.curlevel < 20 && special == 3)
      return;
      
    if(onein(6)) // monkeys each 7 levels
      special = 5;
    
    if(onein(7)) // shadows each 7 levels
      special = 6;
    
    if(P.curlevel == levseed[5] % 23 + 25) 
      special = 7;
    
    if(P.curlevel >= 75 && hrand(20) == 0)
      special = 7;

    
    hydra *h = NULL;
      
    switch(special) {
      case 0: case 7:
        h = new hydra(HC_ETTIN, 1, 1, 0);
        h->heads = 2;
        if(special == 7) h->heads = 50;
        switch(hrand(3)) {
          case 0:
            h->ewpn = new weapon(randHCol(), 5 * (P.curlevel/5), WT_BLADE);
            break;
          case 1:
            h->ewpn = new weapon(randSCol(), P.curlevel+4, WT_BLUNT);
            break;
          case 2:
            h->ewpn = new weapon(randHCol(), P.curlevel+2-hrand(5), WT_AXE);
            break;
          }
        break;
      
      case 1:
        h = new hydra(HC_VAMPIRE, hydrasize - hrand(hydrasize/3), growpow, healing/5);
        break;
  
      case 2:
        h = new hydra(HC_ALIEN, hydrasize * 2 + hrand(60), growpow, healing/7);
        for(int i=0; i<COLORS; i++) h->res[i] = i == HC_OBSID ? -2 : 0;
        break;
      
      case 3:
        h = new hydra(HC_GROW, hydrasize + hrand(100), 1, healing/4);
        break;
  
      case 4:
        h = new hydra(HC_WIZARD, P.curlevel + hrand(P.curlevel), growpow+2, healing/4);
        break;
      
      case 5:
        h = new hydra(HC_MONKEY, 1, 1, 5);
        h->heads = 3;
        break;
      
      case 6:
        h = new hydra(HC_SHADOW, hydrasize + hrand(100), 1, healing/4);
        break;
      }
  
    if(h) h->put();
    }

  // traps
  if(onein(8)) 
    newTrap(randHCol(), 1 + hrand(hydrasize), WT_BLADE)->put();

  if(onein(8)) 
    newTrap(randSCol(), 1 + hrand(hydrasize), WT_BLUNT)->put();

  if(onein(8)) 
    newTrap(randHCol(), 3 + hrand((int) sqrt(hydrasize)), WT_DIV)->put();
  }

void make(int a, int b, int c) { 
  (new weapon(a,b,c))->put();
  }

weapon *generateOrb(int rating) {
  switch(hrand(11)) {
    case 0:  return new weapon(getOrbForItem(IT_RGROW), rating, WT_ORB);
    case 1:  return new weapon(getOrbForItem(IT_RCANC), rating/3, WT_ORB);
    case 2:  return new weapon(getOrbForItem(IT_PCHRG), rating/2, WT_ORB);
    case 3:  return new weapon(getOrbForItem(IT_PFAST), rating/2, WT_ORB);
    case 4:  return new weapon(getOrbForItem(IT_PSWIP), rating/4, WT_ORB);
    case 5:  return new weapon(getOrbForItem(IT_PAMBI), rating/4, WT_ORB);
    case 6:  return new weapon(getOrbForItem(IT_RSTUN), rating/5, WT_ORB);
    case 7:  return new weapon(getOrbForItem(IT_RDEAD), rating/5, WT_ORB);
    case 8:  return new weapon(getOrbForItem(IT_RNECR), rating/10, WT_ORB);
    case 9:  return new weapon(getOrbForItem(IT_RFUNG), rating/10, WT_ORB);
    case 10: return new weapon(getOrbForItem(IT_RCONF), rating/10, WT_ORB);
  }
  return NULL;
  }

void generateUniqueItems() {

  // guaranteed stuff

  if(onlev(13, 14))
    make(HC_ALIEN, 2, WT_QUAKE);

  if(onlev(12, 14)) (new item(IT_PARMS))->put();
  if(onlev(18, 20)) (new item(IT_PARMS))->put();
  if(onlev(26, 29)) (new item(IT_PARMS))->put();
  if(onlev(32, 36)) (new item(IT_PARMS))->put();
  
  // tools
  if(onlev(16, 60))
    make(hrand(9) ? randSCol() : HC_OBSID, 1, WT_PICK);

  if(onlev(25, 100))
    make(hrand(15), P.curlevel, WT_PHASE);

  if(onlev(12, 70))
    make(randSCol(), 2, WT_SPEED);  
  
  if(onlev(20, 70))
    make(HC_OBSID, 1, WT_FUNG);

  // weaponry
  if(onlev(38, 64)) {
    switch(hrand(6)) {
      case 0:
        make(randHCol(), 1000 + hrand(500), WT_BLADE);
        break;
      
      case 1:
        make(randHCol(), 1000 + hrand(500), WT_MSL);
        break;
      
      case 2:
        make(randHCol(), 1000 + hrand(500), WT_AXE);
        break;
      
      case 3:
        make(randHCol(), 100 + hrand(50), WT_DIV);
        break;

      case 4:
        make(randSCol(), 1000 + hrand(500), WT_BLUNT);
        break;

      case 5:
        make(randHCol(), 1000 + hrand(500), WT_SHLD);
        break;
      }
    }
  
  if(onlev(30, 72))   make(randHCol(), 2, WT_LOG);
  if(onlev(42, 100))  make(HC_OBSID, 2, WT_LOG);
  if(onlev(20, 56))   make(randHCol(), P.curlevel - 7, WT_DANCE);
  if(onlev(30, 76))   make(HC_OBSID, 3, WT_DANCE);
  if(onlev(20, 80))   make(randHCol(), 1, WT_VORP);
  if(onlev(25, 150))  make(HC_OBSID, 1, WT_VORP);
  if(onlev(20, 75))   make(randHCol(), 1 + levseed[5] % 9, WT_PREC);
  if(onlev(30, 80))   make(HC_OBSID, 1, WT_PREC);
  if(onlev(20, 85))   make(randHCol(), 20 + P.curlevel, WT_DECO);
  if(onlev(40, 90))   make(HC_OBSID, P.curlevel/2, WT_DECO);

  if(onlev(32, 99))   make(randHCol(), 0, WT_PSLAY);
  if(onlev(12, 65))   make(randHCol(), 1, WT_DISK);
  if(onlev(12, 75))   make(randHCol(), P.curlevel/3 - 3, WT_DISK);
  if(onlev(14, 80))   make(randHCol(), 3, WT_SUBD);
  if(onlev(14, 80))   make(randHCol(), 5, WT_QUI);
  if(onlev(20, 85))   make(9, 1, WT_GOLD);
  if(onlev(20, 85))   make(12, 0, WT_COLL);
  if(onlev(15, 17))   make(randHCol(), 0, WT_DIV);
  if(onlev(15, 100))  make(1, 1, WT_RAND);
  if(onlev(12, 150))  generateOrb(50 + hrand(150))->put();
  }

void generateExtraItems() {
  // extra blades
  for(int u=0; u<2; u++) if(onein(4))
    make(randHCol(), 1 + hrand(10 + P.curlevel / 5), WT_BLADE);
  
  // extra blunts
  for(int u=0; u<2; u++) if(onein(4))
    make(randSCol(), 1 + hrand(13 + (P.curlevel / 5)), WT_BLUNT);
  
  // extra shield
  if(onein(4))
    make(randHCol(), 2 + hrand(3 + P.curlevel / 10), WT_SHLD);
  
  // extra axe
  if(onein(4)) make(randHCol(), 1 + hrand(10 + P.curlevel / 5), WT_AXE);

  // extra missile
  if(onein(4)) make(randHCol(), 1 + hrand(P.curlevel / 3), WT_MSL);

  // extra divisor
  if(onein(4)) make(randHCol(), 2 + hrand(1 + P.curlevel / 10), WT_DIV);

  // extra stone
  if(onein(4)) make(randSCol(), 1 + hrand(1 + P.curlevel / 3), WT_STONE);
  
  // extra spear
  if(onein(4)) make(randSCol(), 1 + hrand(1 + P.curlevel / 3), WT_SPEAR);

  if(onein(2)) (new item(IT_RGROW))->put();
  if(onein(5)) (new item(IT_RCANC))->put();
  if(onein(5)) (new item(IT_RDEAD))->put();
  if(onein(5)) (new item(IT_RSTUN))->put();
  if(onein(5)) (new item(IT_RCONF))->put();
  if(onein(6)) (new item(IT_RFUNG))->put();
  if(onein(6)) (new item(IT_RNECR))->put();
  
  for(int u=0; u<2; u++) if(onein(2)) (new item(IT_SGROW))->put(); // big weapons are fun!
  if(onein(2)) (new item(IT_SXMUT))->put();

  // Reforging and Ambidexterity tend to be together, to help the Trolls who want to use the combo
  if(onein(6)) (new item(IT_SPART))->put(), (new item(IT_PAMBI))->put();
  if(onein(30)) (new item(IT_SPART))->put();
  if(onein(30)) (new item(IT_PAMBI))->put();

  if(onein(9)) (new item(IT_PSWIP))->put();
  if(onein(3)) (new item(IT_PFAST))->put();
  if(onein(3)) (new item(IT_PSEED))->put();
  if(onein(5)) (new item(IT_PCHRG))->put();
  if(onein(9)) (new item(IT_PKNOW))->put(); // too much of them anyway
  
  }

void generateDeepItems() {
  
  // extra Eradicators
  if(onein(100) && P.curlevel >= 30)
    make(randHCol(), 2, WT_ROOT);
  if(onein(100) && P.curlevel >= 30)
    make(HC_OBSID, 2, WT_ROOT);
  if(onein(100) && P.curlevel >= 30)
    make(randHCol(), 3, WT_ROOT);
  
  // extra special shields
  if(onein(30) && P.curlevel >= 30)
    make(randSCol(), 2 + hrand(2 + P.curlevel / 30), WT_SHLD);
  
  // extra obsidian blades
  if(onein(100) && P.curlevel >= 30)
    make(HC_OBSID, 1 + hrand(2 + P.curlevel / 10), WT_BLADE);
  if(onein(100) && P.curlevel >= 30)
    make(HC_OBSID, 1 + hrand(2 + P.curlevel / 10), WT_MSL);
  if(onein(100) && P.curlevel >= 30)
    make(HC_OBSID, 1 + hrand(2 + P.curlevel / 10), WT_DIV);
  
  // extra pick axes
  if(onein(40) && P.curlevel >= 40)
    make(randSCol(), 1 + hrand(3), WT_PICK);
  if(onein(100) && P.curlevel >= 50)
    make(HC_OBSID, 1 + hrand(3), WT_PICK);
  
  // extra mushroom staves
  if(onein(200) && P.curlevel >= 55)
    make(HC_OBSID, 1 + hrand(3), WT_FUNG);
  
  // extra decomposers
  if(onein(70) && P.curlevel >= 50)
    make(randHCol(), P.curlevel, WT_DECO);
  if(onein(70) && P.curlevel >= 50)
    make(HC_OBSID, P.curlevel/2, WT_DECO);
  
  // extra dancers
  if(onein(50) && P.curlevel >= 40)
    make(randHCol(), P.curlevel-10, WT_DANCE);
  if(onein(100) && P.curlevel >= 50)
    make(HC_OBSID, P.curlevel/3, WT_DANCE);
  
  // extra vorpals
  if(onein(100) && P.curlevel >= 60)
    make(randHCol(), 1, WT_VORP);
  if(onein(200) && P.curlevel >= 70)
    make(HC_OBSID, 1, WT_VORP);
  
  // extra precs
  if(onein(50) && P.curlevel >= 50)
    make(randHCol(), 1 + hrand(P.curlevel), WT_PREC);
  if(onein(100) && P.curlevel >= 50)
    make(HC_OBSID, 1 + hrand(P.curlevel/5), WT_PREC);

  // extra primeslayers
  if(onein(150) && P.curlevel >= 50)
    make(randHCol(), 1 + hrand(P.curlevel), WT_PSLAY);
  if(onein(150) && P.curlevel >= 50)
    make(HC_OBSID, 0, WT_PSLAY);
  
  // extra loggers
  if(onein(100) && P.curlevel >= 50)
    make(randHCol(), 2 + hrand(2), WT_LOG);
  if(onein(100) && P.curlevel >= 50)
    make(HC_OBSID, 2, WT_LOG);
    
  // extra chakrams
  if(onein(50) && P.curlevel >= 50)
    make(randHCol(), 1 + hrand(P.curlevel/3), WT_DISK);
  
  // extra wands
  if(onein(150) && P.curlevel >= 50)
    make(hrand(15), P.curlevel, WT_PHASE);

  if(onein(100) && P.curlevel >= 50)
    make(randSCol(), 2 + (hrand(2) ? 0:1) + (hrand(4) ? 0:1) + (hrand(8) ? 0:1), WT_SPEED);

  // extra subdivisors
  if(onein(100) && P.curlevel >= 50)
    make(randHCol(), 2 + hrand(2), WT_SUBD);
  if(onein(100) && P.curlevel >= 50)
    make(randHCol(), hrand(2) ? 3 : 5, WT_QUI);
  
  // extra golden sectors
  if(onein(100) && P.curlevel >= 50)
    make(9, 1, WT_GOLD);

  // extra Syracuse blades
  if(onein(200) && P.curlevel >= 50)
    make(12, 0, WT_COLL);
  
  // extra timedaggers
  if(onein(40) && P.curlevel >= LEVELS)
    make(randHCol(), 1, WT_TIME);

  // extra rainbow blades
  if(onein(40) && P.curlevel >= LEVELS)
    make(randHCol(), P.curlevel, WT_RAIN);

  // extra twisters
  if(onein(100) && P.curlevel >= 100)
    make(1, 2 + (P.curlevel-100)/10, WT_RAND);
  
  if(onein(100) && P.curlevel >= 75)
    generateOrb(60 + hrand(140));
  }


void generateExtra() {

  lseed = levseed[0];
  
  generateUniqueItems();
  if(P.curlevel == 998) return;

  generateExtraMonsters();
  generateExtraItems();
  generateDeepItems();
  }

void generateAnyMap() {
  do {
    int msize = P.curlevel < LEVELS ? linf[P.curlevel].msize : 10;
    
    if(P.flags & dfChallenge)
      msize = min(5 + P.curlevel, 10);
    
    int qml = 8;
        
    int levtype = hrand(1400);
    
    topx = TBAR, topy = TBAR;
    clearLevel();
    
    qml = 8;
    
    if(0)
      topx = TLOOP, topy = TMOB, swissMap();
      // setAnyTopology(), swissMap();
    
    else if(P.curlevel == LEVELS-1 && !(P.flags & dfChallenge))
      mandelbrotMap(msize), qml = 0;

    else if((P.curlevel < LEVELS  && !(P.flags & dfChallenge)) || levtype < 150)
      setAnyTopology(), bubbleMap();

    else if(levtype < 400)
      setAnyTopology(), brownianMap();

    else if(levtype < 500)
      sierpinskiCarpet(); 

    else if(levtype < 550)
      sierpinskiCarpet(), inverseMap(); 
    
    else if(levtype < 650)
      setAnyTopology(), mushroomRandom(50, msize), qml = 0, roundEdges();
    
    else if(levtype < 750)
      setAnyTopology(), qml = 20, roundEdges();
      
    else if(levtype < 800)
      dragonCurveMap(), mushroomRandom(10, msize), qml = 0;
    
    else if(levtype < 950)
      juliaMap(msize), qml = 0;
    
    else if(levtype < 1050)
      starMap(), qml = 12;

    else if(levtype < 1150)
      setWrapTopology(), lifeMap(msize), qml = 0;

    else if(levtype < 1300)
      setAnyTopology(), swissMap(), qml = 0;

    else if(levtype < 1400)
      setAnyTopology(), livingcaveMap(msize);

    // addMessage("levtype = "+its(levtype)+" time= "+its(time(NULL))+" tx="+its(topx)+" ty="+its(topy));
    
    
    // mushroom lines are shorter for weird topologies, so generate more of them
    if(topx != TBAR) qml *= 3, qml /= 2;
    if(topy != TBAR) qml *= 3, qml /= 2;
    
    if(DIRS == 6)
      for(int y=0; y<SY; y++) for(int x=0; x<SX; x++) if((y^x)&1)
        M.m[y][x].type = CT_HEXOUT, M.m[y][x].mushrooms = 0;
    
    ensureConnected();
    ensureLooped();
    
    buggylevel = !verifyConnected();
    
    mushroomLines(qml, msize);
  } while(countEmpty() < 100);  
  }

int shuford(hydra *h) {
  if(h->color == HC_TWIN) return 0;
  if(h->color >= 10) return 3;
  if(h->dirty) return 2;
  return 1;
  }

void bloodshuffle() {
  int sh = size(hydras);
  int i = 0;
  while(i < sh) {
    if(i && shuford(hydras[i]) < shuford(hydras[i-1])) { 
      swap(hydras[i], hydras[i-1]);
      i--;
      }
    else i++;
    }
  int firstblood = -1;
  for(int i=0; i<sh; i++) if(shuford(hydras[i]) == 2) {
    if(firstblood == -1) firstblood = i;
    swap(hydras[i], hydras[firstblood + hrand(i-firstblood+1)]);
    }
  }

void generateLevel() {

  {
            time_t t = time(NULL);
            struct tm *tmp = localtime(&t);
            char buf[100]; 
            strftime(buf, 100, " %y/%m/%d %H:%M:%S", tmp);
  // printf("generating level, time = %s\n", buf);
  }
    
#ifdef NOTEYE
    clearMapCache();
#endif

  hydras.clear(); 

  P.flags &= ~dfShadowAware; setDirs();

  vorpalRegenerate();
  
  if(P.curlevel < GLEVELS) { 
    // NO, we have not used the potion of ambidexterity
    if(P.race == R_NAGA) P.active[IT_PAMBI] = 0;
    
    for(int i=0; i<ITEMS; i++) stats.usedb[i] = P.active[i];
    stats.usedb[IT_PSEED] += 6; stats.usedb[IT_PSEED] /= 7;
    // initialize items used before
    stats.bossinv = stats.usedup[IT_PLIFE] + stats.usedup[IT_PARMS];
    for(int i=0; i<P.arms; i++) if(wpn[i]) {
      if(wpn[i]->osize >= 0 && wpn[i]->osize <= wpn[i]->size)
        stats.bossinv += wpn[i]->size - wpn[i]->osize;
      }
    }
      
  int seed = P.curlevel < GLEVELS ? levseed[P.curlevel] : levseed[0] + P.curlevel;
  
  tmprand t(seed);

  if(P.flags & dfTutorial) {
    P.curlevel = -1;
    generateTutorialLevel();
    return;
    }
  
  generateAnyMap();
    
  // addMessage("done, time= "+its(time(NULL)));
    
  if(P.curlevel < GLEVELS) {
  
    for(int i=0; i<size(toput[P.curlevel]); i++) toput[P.curlevel][i]->put();
    toput[P.curlevel].clear();
    }
  
  else generateExtra();
  
  // for(int y=1; y<SY-1; y++) for(int x=1; x<SX-1; x++) M.m[y][x].type = CT_EMPTY, M.m[y][x].mushrooms = 0;

  if((P.flags & dfChallenge) && P.curlevel == CLEVELS-1)
    ;
  else 
    M[findEmpty()].type = CT_STAIRDOWN;  

  while(true) {
    vec2 pos = findEmpty();
    bool ok = false;
    for(int i=0; i<DIRS; i++) if(M[pos+dirs[i]].isEmpty()) ok = true;
    if(ok) {
      M[stairpos = playerpos = pos].type = CT_STAIRUP;
      break;
      }
    }
  
  bloodshuffle();
  }

int dcolors[HCOLORS];

void generateNormalGame() {

  /* if(gameExists) {
    addMessage("ERROR: generateGame called twice!");
    return;
    } */
  gameExists = true;
  tmprand t(P.gameseed);
  
  int dirtylev = 5 + hrand(5);

  for(int i=0; i<LEVELS; i++) {
    levseed[i] = hrand(1000000000);
    if(levseed[i] < 0) levseed[i] = -levseed[i];
    
    // other random items
    int its[] = { 
      IT_RGROW, IT_RGROW,
      IT_RCANC, IT_RCANC, 
      IT_RDEAD,
      IT_RSTUN,
      IT_RCONF, 
      IT_SXMUT, IT_SXMUT, 
      IT_SGROW, 
      IT_PSWIP, 
      IT_PFAST, 
      IT_PKNOW, IT_PKNOW,
      IT_PSEED
      };
    
    int count = 6 + hrand(5);
    for(int c=0; c<count; c++)
      toput[i].push_back(new item(its[hrand(sizeof(its) / sizeof(int))]));

    if(i == LEVELS-1) continue; // only items are put on this level
    
    int r = hrand(6);
    
    // 2/3 chance of a good blade (or a good axe)
    if(r <= 4) {
      if(i > 0 && hrand(10) <= 2)
        toput[i].push_back(new weapon(randHCol(), linf[i].weapon, WT_AXE));
      else 
        toput[i].push_back(new weapon(randHCol(), linf[i].weapon, WT_BLADE));
      }
    
    // 2/3 chance of a Big Stick; either a good blade or a Bigstick is guaranteed
    if(r >= 2)
      toput[i].push_back(new item(IT_SGROW));
    
    // one hydra of each color
    for(int c=0; c<HCOLORS; c++)
      toput[i].push_back(new hydra(c, hrand(linf[i].maxheads) + 1, linf[i].growlimit, linf[i].heal));
    
    // make some hydras dirty
    if(i == 10 || i == dirtylev) {
      hydra *H = (hydra*) toput[i][size(toput[i])-1-hrand(10)];
      H->dirty = 2*IS_DIRTY-1;
      H->heal += 5;
      }
    
    // 3 * 1/3 chance of another blade
    for(int u=0; u<3; u++) if(onein(3))
      toput[i].push_back(new weapon(randHCol(), 1 + hrand(linf[i].weapon), WT_BLADE));

    // 2 * 1/2 chance of a bludgeon
    for(int u=0; u<2; u++) if(onein(2))
      toput[i].push_back(new weapon(randSCol(), 1 + hrand(linf[i].bweapon), WT_BLUNT));    
    }
  
  for(int i=0; i<HCOLORS; i++) dcolors[i] = i;
  for(int i=0; i<HCOLORS; i++) swap(dcolors[i], dcolors[hrand(i+1)]);
  sclass *extras[LEVELS];
  
  // original weapons and extras...
  
  if(!stats.savecount) {
    wpn[0] = new weapon(dcolors[0], 1, WT_BLADE);
    wpn[1] = new weapon(dcolors[1], 2, WT_BLADE);
    wpn[0]->level = wpn[1]->level = 0;
    }
    
  int artdie = hrand(6);

  extras[0] = new item(IT_PARMS);
  extras[1] = new item(IT_PARMS);
  extras[2] = new weapon(dcolors[2], 4, WT_MSL);
  extras[3] = new weapon(dcolors[3], 2, WT_SHLD);
  extras[4] = new item(IT_PARMS);
  extras[5] = new weapon(dcolors[4], 3 + hrand(3), WT_SHLD);
  
  // extras[6] is randomized
  if(artdie == 5)
    extras[6] = new weapon(dcolors[5], 1,  WT_TIME);
  else if(artdie == 4)
    extras[6] = new weapon(dcolors[5], 6,  WT_RAIN);
  else
    extras[6] = new weapon(dcolors[5], 2,  WT_DIV);

  extras[7] = new item(IT_PARMS);
  extras[8] = new weapon(dcolors[6], 3,  WT_DIV);
  extras[9] = new weapon(HC_OBSID, 1, WT_BLADE);
  extras[10] = new item(IT_PLIFE);
  extras[11] = new item(IT_PLIFE);
  
  // randomize 1-3
  swap(extras[2], extras[1 + hrand(2)]);
  swap(extras[3], extras[1 + hrand(3)]);
  
  // randomize 4-6
  swap(extras[5], extras[4 + hrand(2)]);
  swap(extras[6], extras[4 + hrand(3)]);

  // randomize 7-9
  swap(extras[8], extras[7 + hrand(2)]);
  swap(extras[9], extras[7 + hrand(3)]);
  
  for(int u=0; u<LEVELS; u++) toput[u].push_back(extras[u]);
  
  toput[8 + hrand(2)].push_back(new weapon(dcolors[7], 10, WT_DIV));
  toput[7 + hrand(3)].push_back(new weapon(HC_ANCIENT, 1, WT_SHLD));
  
  // extra small shield
  if(onein(2)) toput[4 + hrand(6)].push_back(new weapon(dcolors[8], 2, WT_SHLD));

  // extra larger shields
  if(onein(3)) toput[7 + hrand(4)].push_back(new weapon(randHCol(), 3, WT_SHLD));
  if(onein(4)) toput[8 + hrand(3)].push_back(new weapon(randHCol(), 4, WT_SHLD));
  
  // extra missiles
  if(onein(2)) toput[4 + hrand(7)].push_back(new weapon(randHCol(), 1, WT_MSL));
  if(onein(2)) toput[4 + hrand(7)].push_back(new weapon(randHCol(), 2, WT_MSL));
  if(onein(2)) toput[4 + hrand(7)].push_back(new weapon(dcolors[9], 4, WT_MSL));
  int lev = 3 + hrand(7);
  toput[lev].push_back(new weapon(randSCol(), lev/2, WT_STONE));
  
  // runes of fungification
  for(int i=0; i<10; i++) if(onein(4))
    toput[hrand(11)].push_back(new item(IT_RFUNG));

  // runes of fungal necromancy
  for(int i=0; i<10; i++) if(onein(4))
    toput[3 + hrand(8)].push_back(new item(IT_RNECR));

  // scrolls of reduction
  for(int i=0; i<10; i++) if(onein(3))
    toput[3 + hrand(8)].push_back(new item(IT_SPART));

  // potions of ambidexterity
  for(int i=0; i<8; i++) if(onein(2))
    toput[hrand(11)].push_back(new item(IT_PAMBI));

  // potions of weapon charge
  for(int i=0; i<10; i++) if(onein(3))
    toput[hrand(11)].push_back(new item(IT_PCHRG));

  // two extra Big Sticks
  for(int i=0; i<2; i++) toput[hrand(11)].push_back(new item(IT_SGROW));

  // 4 special monsters on levels 4 to 10
  int tab[7];
  for(int t=0; t<7; t++) tab[t] = 4+t;
  for(int t=0; t<7; t++) swap(tab[t], tab[hrand(t+1)]);
  
  for(int j=0; j<4; j++) {
    int i = tab[j];
    if(j < 2) {
      hydra *h = new hydra(HC_ETTIN, 1, 1, 0);
      h->heads = 2;
      if(j == 0)
        h->ewpn = new weapon(randHCol(), linf[i].weapon, WT_BLADE);
      else
        h->ewpn = new weapon(randSCol(), linf[i].weapon, WT_BLUNT);
      toput[i].push_back(h);
      }
    else
      toput[i].push_back(new hydra(HC_VAMPIRE, linf[i].maxheads - hrand(3), linf[i].growlimit, linf[i].heal + 5));
    }

  // boss monster
  toput[LEVELS-1].push_back(new hydra(HC_ANCIENT, 99000 + hrand(1000), linf[10].growlimit, 0));
  
  toput[LEVELS-1].push_back(new weapon(dcolors[9], 2, WT_ROOT));
  
  // store level info for weapons
  for(int u=0; u<LEVELS; u++) for(int v=0; v<size(toput[u]); v++) {
    weapon* w = toput[u][v]->asWpn();
    if(w) w->level = u+1;
    }
  }

void fixTheSavefile() {
  if(stats.savecount && P.curlevel >= 0 && !verifyConnected(1)) {
    ensureConnected();
    addMessage("A sudden earthquake shakes the reality!");
    }
  }

void putat(int lev, sclass *o) {
  if(lev >= 0 && lev < CLEVELS && o)
    toput[lev].push_back(o);
  }

int cheadcount[CLEVELS];
int cgrowlimit[CLEVELS];

int atmost(int r) {
  int z = 1;
  for(; z<CLEVELS && cheadcount[z] < r; z++) ;  
  return hrand(z);
  }

weapon *finisher(int q) {
  switch(hrand(7)) {
    case 0: return new weapon(HC_OBSID, 1, hrand(100) < 10 ? WT_PREC : WT_BLADE);
    case 1: return new weapon(hrand(100) < 10 ? HC_OBSID : randHCol(), 6 + hrand(6), WT_MSL);
    case 2: 
      if(P.race == R_CENTAUR)
        for(int i=hrand(3); i; i--) toput[atmost(35)].push_back(new item(IT_PSWIP));
      return new weapon(hrand(100) < 10 ? HC_OBSID : randHCol(), 6 + hrand(6), WT_DANCE);    
    case 3:
      if(P.race == R_CENTAUR) return NULL;
      return new weapon(randSCol(), 1, WT_SPEAR);
    case 4:
      if(P.race == R_CENTAUR) return NULL;
      for(int i=1; i<CLEVELS; i++) putat(i, new item(IT_PSWIP));
      return NULL;
    case 5:
      if(P.race == R_NAGA) return NULL;
      for(int i=1; i<CLEVELS; i++) 
        putat(i, new item(IT_PCHRG));
      return NULL;
    case 6:
      for(int i=1; i<CLEVELS; i++) 
        putat(i, new item(IT_RGROW));
      return NULL;      
    }
  return NULL;
  }

weapon *finhelper() {
  switch(hrand(4)) {
    case 0: return new weapon(randSCol(), 1 + hrand(4), WT_PICK); break;
    case 1: return new weapon(randSCol(), 2 + hrand(4), WT_SHLD); break;
    case 2: return new weapon(randSCol(), 4 + hrand(4), WT_FUNG); break;
    case 3: return new weapon(randSCol(), 6 + hrand(6), WT_BLUNT); break;
    }
  return NULL;
  }

weapon *antibig(int q) {
  switch(hrand(P.race == R_NAGA ? 4 : 8)) {
    case 0: return new weapon(randHCol(), 3 + hrand(q?6:12), WT_DIV);
    case 1: return new weapon(HC_OBSID, 2 + hrand(q?3:6), WT_DIV);
    case 2: return new weapon(onein(5) ? HC_OBSID : randHCol(), onein(5) ? 3 : 2, WT_ROOT);
    case 3: return new weapon(onein(5) ? HC_OBSID : randHCol(), 10 + hrand(60), WT_DECO);
    case 4: return new weapon(onein(5) ? HC_OBSID : randHCol(), onein(5) ? hrand(6) : 0, WT_PSLAY);
    case 5: return new weapon(onein(5) ? HC_OBSID : randHCol(), 1, WT_VORP);
    case 6: return new weapon(9, hrand(6), WT_GOLD);
    case 7: return new weapon(randHCol(), 3 + hrand(3), WT_SUBD);
    case 8: 
      int hr = hrand(100);
      return new weapon(12, hr < 5 ? 27 : hr < 10 ? 7 :hr < 20 ? 3 : 0, WT_COLL);
    }
  return NULL;
  }

void genChallengeChar() {
  // initialize the character/inventory
  for(int ii=0; ii<ITEMS; ii++) P.inv[ii] = 0;
  P.version = VERSION;
  stats.powerignore = 100;
  stats.gamestart = time(NULL);
  
  if(P.race == R_TWIN) {
    P.maxHP = 30; P.curHP = 20;
    P.arms = 4;
    P.twinarms = 10; P.twinmax = 30; P.twincarm = 1;
    stairqueue.push_back(new hydra(HC_TWIN, 20, 1, 0));
    }
  else {
    P.maxHP = 60; P.curHP = 40;
    P.arms = 4; P.cArm = 0; P.ambiArm = 3;
    }
  
  if(P.race == R_CENTAUR)
    wpn[0] = new weapon(randSCol(), 1, WT_BOW);
  else
    wpn[0] = new weapon(randHCol(), 1, onein(3) ? WT_PREC : WT_BLADE);
  
  wpn[1] = new weapon(randHCol(), 2 + hrand(4), onein(10) ? WT_DANCE : WT_BLADE);
    
  if(onein(7))
    wpn[2] = new weapon(randHCol(), 1, WT_TIME);
  else
    wpn[2] = new weapon(randHCol(), 6 + hrand(4), onein(6) ? WT_RAIN : onein(3) ? WT_AXE : WT_BLADE);
  
  switch(hrand(4)) {
    case 0:
      wpn[3] = new weapon(randSCol(), 8 + hrand(6), WT_BLUNT);
      break;
    
    case 1:
      wpn[3] = new weapon(randHCol(), 6 + hrand(4), onein(6) ? WT_RAIN : onein(3) ? WT_AXE : WT_BLADE);
      break;
    
    case 2:
      wpn[3] = new weapon(randHCol(), 4 + hrand(4), WT_MSL);
      break;
    
    case 3:
      wpn[3] = new weapon(randHCol(), 3 + hrand(3), WT_SHLD);
      break;
    }

  for(int i=0; i<4; i++) wpn[i]->level = 0;

  // safety items
  if(P.race != R_TROLL) switch(hrand(3)) {
    case 0:
      P.inv[IT_RSTUN] += 2;
      P.inv[IT_RDEAD] += 2;
      break;
    
    case 1:
      P.inv[IT_RFUNG] += 2;
      break;
    
    case 2:
      P.inv[IT_RCANC] += 3;
      break;
    }
  
  // for a Titan, 3..6 extra weapons
  int d = 3 + hrand(4);
  if(P.race == R_TROLL) for(int i=0; i<d; i++) {
    weapon *wp = new weapon(randHCol(), 3 + hrand(7), onein(3) ? WT_AXE : WT_BLADE);
    wp->level = 0;
    pinfo.trollwpn.push_back(wp);
    pinfo.trollkey.push_back('a'+i);
    }
  }

int powerlaw(int minv, int maxv, double x, double shift) {
  double total = 0;
  for(int i=minv; i <= maxv; i++) total += pow(i+shift, x);
  double xx = randf(0, total);
  for(int i=minv; i <= maxv+1; i++) {
    xx -= pow(i+shift, x);
    if(xx<0) return i;
    }
  return maxv;
  }

#define REP(i,k) for(int i=k; i>0; i--)

// #define GENITEMS(z,r) {vector<int> u; for(int d=0; d<100; d++) u.push_back(z); sort(u.begin(), u.end()); for(int d=0; d<20; d++) printf("%4d", u[d*5+2]); printf(" %s\n",iinf[r].name.c_str());}
#define GENITEMS(z,r) REP(q,z) putat(hrand(CLEVELS), new item(r));

void generateItems() {
  GENITEMS(powerlaw(0, 100, -.8, 2), IT_SGROW);
  GENITEMS(powerlaw(0, 100, -1.1, 1), IT_RCANC);
  GENITEMS(powerlaw(0, 100, -1, 3), IT_SXMUT);
  GENITEMS(powerlaw(0, 20, -1.1, .4), IT_RNECR);
  GENITEMS(powerlaw(0, 20, -1.0, 2), IT_SPART);
  GENITEMS(powerlaw(0, 15, -1, .4), IT_RCONF);
  GENITEMS(powerlaw(0, 30,  -1, 2), IT_RSTUN);
  GENITEMS(powerlaw(0, 100, -1, 2), IT_RDEAD);
  GENITEMS(powerlaw(0, 25, -1.1, 1), IT_RFUNG);
  GENITEMS(powerlaw(0, 15, -1, 1), IT_PAMBI);
  GENITEMS(powerlaw(0, 60, -1, 1), IT_PSWIP);
  GENITEMS(powerlaw(0, 30, -1, 1), IT_PFAST);
  GENITEMS(powerlaw(0, 60, -1, 1), IT_PCHRG);
  GENITEMS(powerlaw(0, 60, -1, 1), IT_PKNOW);
  GENITEMS(powerlaw(0, 150, -.7, .1), IT_RGROW);
  GENITEMS(powerlaw(0, 50, -1, 1), IT_PSEED);
  }

int raceForSeed(int seed) {
  int r = seed % 5;
  if(r == R_ELF) r = R_CENTAUR;
  return r;
  }

int geometryForSeed(int seed) {
  // choose the geometry
  switch(seed & 3) {
    case 0: return 3;
    case 1: return 4;
    case 2: return 6;
    case 3: return 8;
    }
  return 16;
  }

string geometryName(int g) {
  switch(g) {
    case 16: return "knight movement (secret)"; 
    case 8:  return "diagonal movement allowed (8 directions)"; 
    case 6:  return "hex board (6 directions)"; 
    case 4:  return "cardinal movement only (4 directions)";
    case 3:  return "variable geometry (4/6/8)";
    }
  return "unknown geometry";
  }

void generateChallengeGame() {

  int conscheck = 0;

  gameExists = true;
  tmprand t(P.gameseed);
  
  for(int i=0; i<CLEVELS-1; i++)
    cheadcount[i] = (int) exp(randf(2, 9));
  
  sort(cheadcount, cheadcount+CLEVELS-1);

  for(int i=0; i<CLEVELS-1; i++)
    cgrowlimit[i] = 5 + hrand(10);
  sort(cgrowlimit, cgrowlimit+CLEVELS-1);

  // set levseeds
  for(int i=0; i<CLEVELS; i++)
    levseed[i] = hrand(1000000000);
    
  conscheck += hrand(1000000000);
  
  // create normal hydras
  hydra *hydratab[CLEVELS][HCOLORS];
  
  for(int i=0; i<CLEVELS-1; i++) for(int c=0; c<HCOLORS; c++)
    hydratab[i][c] = new hydra(c, hrand(cheadcount[i]) + 1, cgrowlimit[i], 20 + 5 * i);
    
  
  for(int i=0; i<CLEVELS-1; i++) for(int c=0; c<HCOLORS; c++)
    toput[i].push_back(hydratab[i][c]);
  
  // create finishers
  for(int q=0; q<2; q++) {
    if(q == 1 && P.race == R_CENTAUR) continue; // Bow is enough for you!
    weapon *f = finisher(q);
    if(f) {
      weapon *h = (f->type == WT_BLADE && !q) ? finhelper() : NULL;
      int lev = atmost(q ? 25 : 75);
      if(h) {
        int lev2 = atmost(q ? 25 : 75);
        if(lev>lev2) swap(lev, lev2);
        toput[lev2].push_back(h);
        }
      toput[lev].push_back(f);
      if(hrand(100) < 80 && !q) toput[lev].push_back(new item(IT_PARMS));
      }
    }
  
  // create weapons against big hydras
  for(int q=0; q<2; q++) {
    weapon *a = antibig(q);
    int lev = atmost(q?40:120);
    toput[lev].push_back(a);
    if(hrand(100) < 80 && !q) toput[lev].push_back(new item(IT_PARMS));
    }
  
  conscheck += hrand(1000000000);

  // 0 to 5 transmuters early
  { int qt = hrand(6);
  for(int q=0; q<qt; q++) toput[hrand(q+1)].push_back(new item(IT_SXMUT)); }

  REP(q,hrand(5))  toput[hrand(CLEVELS)].push_back(new item(IT_PARMS));
  
  generateItems();
  
  // make the hydras dirty
  for(int q=hrand(6); q; q--) {
    int l = max(hrand(CLEVELS-1), hrand(CLEVELS-1));
    int c = hrand(HCOLORS);
    hydratab[l][c]->dirty = 2*IS_DIRTY-1;
    }

  // change the hydras into dragons
  for(int q=hrand(6); q; q--) {
    int l = max(hrand(CLEVELS-1), hrand(CLEVELS-1));
    int c = hrand(HCOLORS);
    hydratab[l][c]->color |= HC_DRAGON;
    }
  
  conscheck += hrand(1000000000);

  // special hydras
  for(int i=0; i<CLEVELS-1; i++) {
    hydra *h = NULL;
    
    switch(hrand(7)) {
      case 0:
        h = new hydra(HC_ETTIN, 1, 1, 0);
        h->heads = 2;
        switch(hrand(3)) {
          case 0:
            h->ewpn = new weapon(randHCol(), 5 * (1+P.curlevel/3), WT_BLADE);
            break;
          case 1:
            h->ewpn = new weapon(randSCol(), P.curlevel*3+4, WT_BLUNT);
            break;
          case 2:
            h->ewpn = new weapon(randHCol(), P.curlevel*2+2-hrand(5), WT_AXE);
            break;
          }
        break;
      
      case 1:
        if(cheadcount[i] >= 20)
          h = new hydra(HC_VAMPIRE, cheadcount[i] - hrand(cheadcount[i]/3), cgrowlimit[i], 30 + 10 * i);
        break;
  
      case 2:
        h = new hydra(HC_ALIEN, cheadcount[i] + hrand(cheadcount[i]*2+5*i), cgrowlimit[i], 30 + 10 * i);
        { for(int i=0; i<COLORS; i++) h->res[i] = i == HC_OBSID ? -2 : 0; }
        break;
      
      case 3:
        if(cheadcount[i] >= 100)
          h = new hydra(HC_GROW, cheadcount[i] + hrand(5*i), 1, 30 + 10 * i);
        break;
  
      case 4:
        if(cheadcount[i] >= 100)
          h = new hydra(HC_WIZARD, i*3 + hrand(i), cgrowlimit[i]+2, 30 + 10 * i);
        break;
      
      case 5:
        h = new hydra(HC_MONKEY, 1, 1, 0); h->heads = 3;
        break;
      
      case 6:
        if(cheadcount[i] >= 100)
          h = new hydra(HC_SHADOW, cheadcount[i] + hrand(cheadcount[i]), 1, 50 + 15 * i);
        break;
      }
    
    putat(i, h);
    }

  conscheck += hrand(1000000000);

  // shields
  if(hrand(100) < 75)
    for(int z=0; z<(P.race == R_TWIN ? 2 : 1); z++)
      toput[hrand(CLEVELS)].push_back(new weapon(randSCol(), 1 + hrand(15), WT_SHLD));
  
  if(hrand(100) < 35)
    for(int j=0; j<CLEVELS; j++) {
      int l = hrand(CLEVELS);
      toput[l].push_back(new weapon(randHCol(), 1 + hrand(2*l+5), WT_BLADE));
      }
  
  if(hrand(100) < 35)
    for(int j=0; j<CLEVELS; j++) {
      int l = hrand(CLEVELS);
      toput[l].push_back(new weapon(randHCol(), 1 + hrand(2*l+5), WT_AXE));
      }
  
  if(hrand(100) < 35)
    for(int j=0; j<CLEVELS; j++) {
      int l = hrand(CLEVELS);
      toput[l].push_back(new weapon(randSCol(), 1 + hrand(4*l+5), WT_BLUNT));
      }
  
  if(hrand(100) < 45)
    for(int j=0; j<CLEVELS/3; j++) {
      int l = hrand(CLEVELS);
      toput[l].push_back(new weapon(randHCol(), 1 + hrand(2*l+1), WT_SHLD));
      }
    
  if(hrand(100) < 45)
    for(int j=0; j<CLEVELS/2; j++) {
      int l = hrand(CLEVELS);
      toput[l].push_back(new weapon(randHCol(), 2 + hrand(l+1), WT_DIV));
      }

  if(hrand(100) < 10)
    for(int j=0; j<2; j++) {
      int l = hrand(CLEVELS);
      toput[l].push_back(new weapon(HC_OBSID, 1 + hrand(2*l+5), WT_BLADE));
      }
  
  while(hrand(100) < 20)
    toput[hrand(CLEVELS)].push_back(generateOrb(10 + hrand(200)));
    
  conscheck += hrand(1000000000);

  // extra stuff
  for(int i=hrand(6); i; i--) {
    switch(hrand(13)) {

      case 0:
        // more reforges
        for(int j=0; j<5; j++) toput[hrand(CLEVELS)].push_back(new item(IT_SPART));
        for(int j=0; j<5; j++) toput[hrand(CLEVELS)].push_back(new item(IT_PAMBI));
        for(int j=0; j<5; j++) toput[hrand(CLEVELS)].push_back(new item(IT_SGROW));
        break;
      
      case 1:
        // precedence blades
        for(int j=0; j<2; j++) {
          int l = hrand(CLEVELS);
          toput[l].push_back(new weapon(randHCol(), 1 + hrand(2*l+5), WT_PREC));
          }
        for(int j=0; j<4; j++) toput[hrand(CLEVELS)].push_back(new item(IT_PAMBI));
        break;
      
      case 3:
        // dance blades
        for(int j=0; j<2; j++) {
          int l = hrand(CLEVELS);
          toput[l].push_back(new weapon(randHCol(), 1 + hrand(2*l+5), WT_DANCE));
          }
        break;
      
      case 4: { // mushroom staff
        int l = hrand(CLEVELS);
        toput[l].push_back(new weapon(HC_OBSID, 1 + hrand(3*l+5), WT_FUNG));
        break;
        }
      
      case 5: { // twister
        int l = hrand(CLEVELS);
        toput[l].push_back(new weapon(randHCol(), hrand(l+1), WT_RAND));
        break;
        }
      
      case 6: {
        // more subdivisors
        for(int j=0; j<3; j++) {
          int l = hrand(CLEVELS);
          toput[l].push_back(new weapon(randSCol(), 2 + hrand(l+1), WT_SUBD));
          }
        break;
        }
      
      case 7: { // broom of speed
        int l = hrand(CLEVELS);
        toput[l].push_back(new weapon(randSCol(), 2 + hrand(5), WT_SPEED));
        break;
        }

      case 8: { // wand of phasing
        int l = hrand(CLEVELS);
        toput[l].push_back(new weapon(hrand(15), 20 + hrand(30), WT_PHASE));
        break;
        }
      
      case 9: { // blade of Logaems
        int l = hrand(CLEVELS);
        toput[l].push_back(new weapon(randHCol(), 2, WT_LOG));
        break;
        }

      case 10: {
        // bladed disks
        for(int j=0; j<CLEVELS; j++) {
          int l = hrand(CLEVELS);
          toput[l].push_back(new weapon(randHCol(), 1 + hrand(l+2), WT_DISK));
          }
        break;
        }
      
      case 11: {
        // huge stones
        for(int j=0; j<CLEVELS; j++) {
          int l = hrand(CLEVELS);
          toput[l].push_back(new weapon(randSCol(), 2 + hrand(l+2), WT_STONE));
          }
        break;
        }
      
      case 12: { // giant weapon
        int l = 2+hrand(CLEVELS-2);
        int siz = 100 + hrand(1500);
        int col = (hrand(siz) < 100) ? HC_OBSID : randSCol();
        int type = (hrand(siz) < 100) ? (hrand(2) ? WT_MSL : WT_DANCE) : WT_BLADE;
        toput[l].push_back(new weapon(col, siz, type));
        break;
        }
      }
    }
  
  // extra finisher
  if(hrand(100) < 66) putat(hrand(CLEVELS), finisher(2));

  // extra helper
  if(hrand(100) < 66) putat(hrand(CLEVELS), finhelper());

  // extra antibig
  if(hrand(100) < 66) putat(hrand(CLEVELS), antibig(2));

  conscheck += hrand(1000000000);

  if(true) {
    int hc = 900000 + hrand(50000);
    while(primediv(hc) != -1) hc++;
    hydra *adragon = new hydra(HC_ANCIENT | HC_DRAGON, hc, 15, 0);
    if(hrand(100) < 50) adragon->dirty = IS_DIRTY-1; // do not know susceptibilities
    toput[CLEVELS-1].push_back(adragon);
    }

  for(int u=0; u<CLEVELS-1; u++)  switch(hrand(5)) {
    case 0:
      putat(u, newTrap(randHCol(), 1 + hrand(cheadcount[u]), WT_BLADE));
      break;
    
    case 1:
      putat(u, newTrap(randSCol(), 1 + hrand(cheadcount[u]), WT_BLUNT));
      break;
    
    case 2:
      putat(u, newTrap(randHCol(), 3 + hrand((int) sqrt(cheadcount[u]+.5)), WT_DIV));
      break;
    }

  conscheck += hrand(1000000000);

  // kill the Ancient Hydra with traps!
  if(hrand(100) < 20) {
    for(int i=0; i<COLORS; i++)
      if(i >= HCOLORS && i < HC_OBSID)
        putat(CLEVELS-1, newTrap(i, 1 + hrand((int) exp(randf(0, log(900000)))), WT_BLUNT));
      else if(hrand(2) == 1)
        putat(CLEVELS-1, newTrap(i, 1 + hrand((int) exp(randf(0, log(900000)))), WT_BLADE));
      else
        putat(CLEVELS-1, newTrap(i, 3 + hrand(21), WT_DIV));
    }
  
  conscheck += hrand(1000000000);

  // store level info for weapons
  for(int u=0; u<CLEVELS; u++) for(int v=0; v<size(toput[u]); v++) {
    weapon* w = toput[u][v]->asWpn();
    if(w) w->level = u+1;
    }

  conscheck += hrand(1000000000);

  extern void listChallenge();
  listChallenge();
  if(!stats.savecount) {
    stats.savecheck = conscheck;
    genChallengeChar();
    generateLevel();
    P.curlevel = 0;    
    }
  
  if(stats.savecheck != conscheck) P.flags |= dfConsist;
  }

void generateGame() {
  if(!fixedseed && !stats.savecount) P.gameseed = time(NULL);
  if(P.flags & dfRaceSeeded) {
    P.race = raceForSeed(P.gameseed);
    P.geometry = geometryForSeed(P.gameseed);
    }
  if(P.flags & dfChallenge)
    generateChallengeGame();
  else
    generateNormalGame();
  }

void listChallenge() {
  printf("#%d race=%s dirs=%d\n", P.gameseed, rinf[P.race].rname.c_str(), P.geometry);
/*  for(int i=0; i<ITEMS; i++) if(P.inv[i]) printf("%dx %s\n", P.inv[i], iinf[i].name.c_str());
  for(int i=0; i<4; i++) if(wpn[i]) {
    string s = wpn[i]->fullname();
    printf("%s\n", s.c_str());
    }
  printf("\n");

  for(int j=0; j<GLEVELS; j++) {
    for(int k=0; k<size(toput[j]); k++) {
      string s = fullname(toput[j][k]);
      printf("%3d %s\n", j, s.c_str());
      }
    printf("\n");
    }        
  printf("\n\n");
  fflush(stdout); */
  }

void listChallenges() {
  P.flags = dfChallenge;
  for(int i=0; i<20; i++) {
    P.gameseed = i;
    generateChallengeGame();
    extern void clearGame();
    clearGame();
    }
  exit(0);
  }
