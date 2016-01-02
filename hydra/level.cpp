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

vec2 randVec() { return vec2(rand() % SX, rand() % SY); }

vector<sclass*> toput[LEVELS];

int levseed[LEVELS];

vec2 findEmpty() { while(true) { vec2 v = randVec(); if(M[v].isEmpty()) return v; } }

int uftab[MSX*MSY];

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

      if(who && !c.explored && targetval < 2) {
        targetval = 2;
        targetdir = ngv;
        }

      if(who && !c.explored) continue;

      if(who && c.it && P.race != R_TROLL && c.it->asItem() && 
        c.it->asItem()->type != IT_HINT && targetval < 3) {
        targetval = 3;
        targetdir = ngv;
        }
      
      if(who && c.type == CT_STAIRDOWN && targetval < 1 && nmush) {
        targetval = 1;
        targetdir = ngv;
        }

      if(((who && c.h) || c.isPassable(nmush)) && !c.dist) {
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
    int norm = rand() % 26;
    vec2 v = randVec();
    for(int dx=-10; dx<=10; dx++) for(int dy=-10; dy<=10; dy++) if(dx*dx+dy*dy <= norm) {
      vec2 v2 = v + vec2(dx, dy);
      if(!inlevel(wrap(v2))) continue;
      M[v2].type = t&1 ? CT_WALL : CT_EMPTY;
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
    rx = rand() % 2000000;
    ry = rand() % 2000000;
    }
  while(!incarpet(rx+1 + rand() % (SX-2), ry+1+rand() % (SY-2)));
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
  for(int y=0; y<SY*SX; y++) uftab[y] = y;

  av.clear();
  
  for(int y=1; y<SY-1; y++) for(int x=1; x<SX-1; x++) {
    vec2 v(x,y);
    if(emptyMode(v, mode)) {
      av.push_back(v);
      for(int d=0; d<DIRS; d++) if(emptyMode(v+dirs[d], mode))
        funion(vindex(v), vindex(wrap(v+dirs[d])));
      }
    }

  for(int i=1; i<size(av); i++) 
    if(ufind(vindex(av[i])) != ufind(vindex(av[0])))
      return false;
    
  return true;
  }

void ensureConnected() {

  verifyConnected();
  
  for(int i=0; i<size(av); i++) swap(av[i], av[rand() % (i+1)]);
  
  for(int i=1; i<size(av); i++) 
    if(ufind(vindex(av[i])) != ufind(vindex(av[0]))) {
      int r;
      do { r = rand() % size(av); } while(vindex(av[r]) != vindex(av[0]));
      int l = len(av[i] - av[r]) * 2;
      
      vec2 dif = pickMinus(av[i], av[r]);
      
      if(l==0) continue;

      int dmode;
      if(DIRS == 4 || (DIRS == 8 && rand() % 10 >= 5))
        dmode = 1 + (rand() % 2);
      else
        dmode = 0;

      if(dmode) l*=2;

      // playerpos = av[i]; drawMap(); ghch();
      for(int u=0; u<=l; u++) {
      
        vec2 vhb = dif * u / l;
        vec2 v0(0,0);

        if(dmode==1) vhb = (u&1) ? cxy(vhb, v0) : cxy(dif, vhb);
        if(dmode==2) vhb = (u&1) ? cxy(v0, vhb) : cxy(vhb, dif);
        
        vec2 vh = av[r] + vhb;        

        M[vh].type = CT_EMPTY;
        funion(vindex(wrap(vh)), vindex(av[0]));

        if(P.geometry == 16) {
          for(int k=0; k<8; k++) if(inlevel(vh+dirs8[k]))
            M[vh+dirs8[k]].type = CT_EMPTY;
          }
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
      if(M[vh].isEmpty()) M[vh].mushrooms = 1 + rand() % msize;
      }
    }
  }

void mushroomRandom(int pct, int msize) {
  for(int y=1; y<SY-1; y++) for(int x=1; x<SX-1; x++) if(rand() % 100 < pct)
    if(M.m[y][x].type == CT_EMPTY)
      M.m[y][x].mushrooms = 1 + rand() % msize;
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
  int rx = rand() % 10;
  int ry = rand() % 3;
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
    M.m[y][x].mushrooms = 1 + rand() % msize;
    }
  
  if(lcount < 160) lifeMap(msize);
  }

void brownianMap() {
  int quota = (SX-2) * (SY-2) / 2;
  if(DIRS == 6) quota /= 2;
  vec2 at = center;
  for(int x=0; x<SX; x++) for(int y=0; y<SY; y++) 
    M.m[y][x].type = CT_WALL;
  while(quota) {
    if(M[at].type == CT_WALL) M[at].type = CT_EMPTY, quota--;
    at += dirs[rand() % DIRS];
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
  int d = rand() % (P.curlevel+1);
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

int randHCol() { return rand() % HCOLORS; }
int randSCol() { return HCOLORS + rand() % SCOLORS; }

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
  else if(P.curlevel >= 20 && rand() % 7 == 0)
    // huge hydras, more healing, less chance of a dragon
    hydrasize *= 10, healing *= 3, dragonmin += 5;
  else if(rand() % 6 == 0)
    // small hydras but less healing, also less chance of a dragon
    hydrasize /= 4, healing /= 2, dragonmin += 5;

  int dragoncolor = -1;
  if(rand() % P.curlevel >= dragonmin)
    dragoncolor = rand() % HCOLORS;
  
  int dirtycolor = -1;
  if(rand() % 5 == 0)
    dirtycolor = rand() % HCOLORS;
    
  int dirtycolor2 = -1;
  if(dirtycolor != -1 && rand() % 2 == 0)
    dirtycolor2 = rand() % HCOLORS;
  
  // Hydras
  if(P.curlevel != 49) for(int c=0; c<HCOLORS; c++) {
    hydra *H = new hydra(c, rand() % hydrasize + 1, growpow, healing / 10);
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
    int hc = 900000 + rand() % 50000;
    while(primediv(hc) != -1) hc++;
    hydra *adragon = new hydra(HC_ANCIENT | HC_DRAGON, hc, growpow, 0);
    adragon->dirty = IS_DIRTY-1; // do not know susceptibilities
    adragon->put();
    }
  else {
    int special = rand() % 5;
    
    // Wizards are picked only after Level 30
    if(P.curlevel < 30)
      special = rand() % 4;
  
    // Growing hydras only after Level 20 (otherwise it's free)
    if(P.curlevel < 20 && special == 3)
      return;
      
    if(rand() % 6 == 0) // monkeys each 7 levels
      special = 5;
    
    if(rand() % 7 == 0) // shadows each 7 levels
      special = 6;
    
    if(P.curlevel == levseed[5] % 23 + 25) 
      special = 7;
    
    if(P.curlevel >= 75 && rand() % 20 == 0)
      special = 7;

    
    hydra *h = NULL;
      
    switch(special) {
      case 0: case 7:
        h = new hydra(HC_ETTIN, 1, 1, 0);
        h->heads = 2;
        if(special == 7) h->heads = 50;
        switch(rand() % 3) {
          case 0:
            h->ewpn = new weapon(randHCol(), 5 * (P.curlevel/5), WT_BLADE);
            break;
          case 1:
            h->ewpn = new weapon(randSCol(), P.curlevel+4, WT_BLUNT);
            break;
          case 2:
            h->ewpn = new weapon(randHCol(), P.curlevel+2-rand() % 5, WT_AXE);
            break;
          }
        break;
      
      case 1:
        h = new hydra(HC_VAMPIRE, hydrasize - rand() % (hydrasize/3), growpow, healing/5);
        break;
  
      case 2:
        h = new hydra(HC_ALIEN, hydrasize * 2 + rand() % 60, growpow, healing/7);
        for(int i=0; i<COLORS; i++) h->res[i] = i == HC_OBSID ? -2 : 0;
        break;
      
      case 3:
        h = new hydra(HC_GROW, hydrasize + rand() % 100, 1, healing/4);
        break;
  
      case 4:
        h = new hydra(HC_WIZARD, P.curlevel + rand() % P.curlevel, growpow+2, healing/4);
        break;
      
      case 5:
        h = new hydra(HC_MONKEY, 1, 1, 5);
        h->heads = 3;
        break;
      
      case 6:
        h = new hydra(HC_SHADOW, hydrasize + rand() % 100, 1, healing/4);
        break;
      }
  
    if(h) h->put();
    }
  }

void make(int a, int b, int c) { 
  (new weapon(a,b,c))->put();
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
    make(rand() % 9 ? randSCol() : HC_OBSID, 1, WT_PICK);

  if(onlev(25, 100))
    make(rand() % 15, P.curlevel, WT_PHASE);

  if(onlev(12, 70))
    make(randSCol(), 2, WT_SPEED);  
  
  if(onlev(20, 70))
    make(HC_OBSID, 1, WT_FUNG);

  // weaponry
  if(onlev(38, 64)) {
    switch(rand() % 6) {
      case 0:
        make(randHCol(), 1000 + rand() % 500, WT_BLADE);
        break;
      
      case 1:
        make(randHCol(), 1000 + rand() % 500, WT_MSL);
        break;
      
      case 2:
        make(randHCol(), 1000 + rand() % 500, WT_AXE);
        break;
      
      case 3:
        make(randHCol(), 100 + rand() % 50, WT_DIV);
        break;

      case 4:
        make(randSCol(), 1000 + rand() % 500, WT_BLUNT);
        break;

      case 5:
        make(randHCol(), 1000 + rand() % 500, WT_SHLD);
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
  if(onlev(15, 17))   make(randHCol(), 0, WT_DIV);
  if(onlev(15, 100))  make(1, 1, WT_RAND);
  }

void generateExtraItems() {
  // extra blades
  for(int u=0; u<2; u++) if(rand() % 4 == 0)
    make(randHCol(), 1 + rand() % (10 + P.curlevel / 5), WT_BLADE);
  
  // extra blunts
  for(int u=0; u<2; u++) if(rand() % 4 == 0)
    make(randSCol(), 1 + rand() % (13 + (P.curlevel / 5)), WT_BLUNT);
  
  // extra shield
  if(rand() % 4 == 0)
    make(randHCol(), 2 + rand() % (3 + P.curlevel / 10), WT_SHLD);
  
  // extra axe
  if(rand() % 4 == 0) make(randHCol(), 1 + rand() % (10 + P.curlevel / 5), WT_AXE);

  // extra missile
  if(rand() % 4 == 0) make(randHCol(), 1 + rand() % (P.curlevel / 3), WT_MSL);

  // extra divisor
  if(rand() % 4 == 0) make(randHCol(), 2 + rand() % (1 + P.curlevel / 10), WT_DIV);

  // extra stone
  if(rand() % 4 == 0) make(randSCol(), 1 + rand() % (1 + P.curlevel / 3), WT_STONE);
  
  // extra spear
  if(rand() % 4 == 0) make(randSCol(), 1 + rand() % (1 + P.curlevel / 3), WT_SPEAR);

  if(rand() % 2 == 0) (new item(IT_RGROW))->put();
  if(rand() % 5 == 0) (new item(IT_RCANC))->put();
  if(rand() % 5 == 0) (new item(IT_RDEAD))->put();
  if(rand() % 5 == 0) (new item(IT_RSTUN))->put();
  if(rand() % 5 == 0) (new item(IT_RCONF))->put();
  if(rand() % 6 == 0) (new item(IT_RFUNG))->put();
  if(rand() % 6 == 0) (new item(IT_RNECR))->put();
  
  for(int u=0; u<2; u++) if(rand() % 2 == 0) (new item(IT_SGROW))->put(); // big weapons are fun!
  if(rand() % 2 == 0) (new item(IT_SXMUT))->put();

  // Reforging and Ambidexterity tend to be together, to help the Trolls who want to use the combo
  if(rand() % 6 == 0) (new item(IT_SPART))->put(), (new item(IT_PAMBI))->put();
  if(rand() % 30 == 0) (new item(IT_SPART))->put();
  if(rand() % 30 == 0) (new item(IT_PAMBI))->put();

  if(rand() % 9 == 0) (new item(IT_PSWIP))->put();
  if(rand() % 3 == 0) (new item(IT_PFAST))->put();
  if(rand() % 3 == 0) (new item(IT_PSEED))->put();
  if(rand() % 5 == 0) (new item(IT_PCHRG))->put();
  if(rand() % 9 == 0) (new item(IT_PKNOW))->put(); // too much of them anyway
  }

void generateDeepItems() {
  
  // extra Eradicators
  if(rand() % 100 == 0 && P.curlevel >= 30)
    make(randHCol(), 2, WT_ROOT);
  if(rand() % 100 == 0 && P.curlevel >= 30)
    make(HC_OBSID, 2, WT_ROOT);
  if(rand() % 100 == 0 && P.curlevel >= 30)
    make(randHCol(), 3, WT_ROOT);
  
  // extra special shields
  if(rand() % 30 == 0 && P.curlevel >= 30)
    make(randSCol(), 2 + rand() % (2 + P.curlevel / 30), WT_SHLD);
  
  // extra obsidian blades
  if(rand() % 100 == 0 && P.curlevel >= 30)
    make(HC_OBSID, 1 + rand() % (2 + P.curlevel / 10), WT_BLADE);
  if(rand() % 100 == 0 && P.curlevel >= 30)
    make(HC_OBSID, 1 + rand() % (2 + P.curlevel / 10), WT_MSL);
  if(rand() % 100 == 0 && P.curlevel >= 30)
    make(HC_OBSID, 1 + rand() % (2 + P.curlevel / 10), WT_DIV);
  
  // extra pick axes
  if(rand() % 40 == 0 && P.curlevel >= 40)
    make(randSCol(), 1 + rand() % 3, WT_PICK);
  if(rand() % 100 == 0 && P.curlevel >= 50)
    make(HC_OBSID, 1 + rand() % 3, WT_PICK);
  
  // extra mushroom staves
  if(rand() % 200 == 0 && P.curlevel >= 55)
    make(HC_OBSID, 1 + rand() % 3, WT_FUNG);
  
  // extra decomposers
  if(rand() % 70 == 0 && P.curlevel >= 50)
    make(randHCol(), P.curlevel, WT_DECO);
  if(rand() % 70 == 0 && P.curlevel >= 50)
    make(HC_OBSID, P.curlevel/2, WT_DECO);
  
  // extra dancers
  if(rand() % 50 == 0 && P.curlevel >= 40)
    make(randHCol(), P.curlevel-10, WT_DANCE);
  if(rand() % 100 == 0 && P.curlevel >= 50)
    make(HC_OBSID, P.curlevel/3, WT_DANCE);
  
  // extra vorpals
  if(rand() % 100 == 0 && P.curlevel >= 60)
    make(randHCol(), 1, WT_VORP);
  if(rand() % 200 == 0 && P.curlevel >= 70)
    make(HC_OBSID, 1, WT_VORP);
  
  // extra precs
  if(rand() % 50 == 0 && P.curlevel >= 50)
    make(randHCol(), 1 + rand() % P.curlevel, WT_PREC);
  if(rand() % 100 == 0 && P.curlevel >= 50)
    make(HC_OBSID, 1 + rand() % (P.curlevel/5), WT_PREC);

  // extra primeslayers
  if(rand() % 150 == 0 && P.curlevel >= 50)
    make(randHCol(), 1 + rand() % P.curlevel, WT_PSLAY);
  if(rand() % 150 == 0 && P.curlevel >= 50)
    make(HC_OBSID, 0, WT_PSLAY);
  
  // extra loggers
  if(rand() % 100 == 0 && P.curlevel >= 50)
    make(randHCol(), 2 + rand() % 2, WT_LOG);
  if(rand() % 100 == 0 && P.curlevel >= 50)
    make(HC_OBSID, 2, WT_LOG);
    
  // extra chakrams
  if(rand() % 50 == 0 && P.curlevel >= 50)
    make(randHCol(), 1 + rand() % (P.curlevel/3), WT_DISK);
  
  // extra wands
  if(rand() % 150 == 0 && P.curlevel >= 50)
    make(rand() % 15, P.curlevel, WT_PHASE);

  if(rand() % 100 == 0 && P.curlevel >= 50)
    make(randSCol(), 2 + (rand() % 2 ? 0:1) + (rand() % 4 ? 0:1) + (rand() % 8 ? 0:1), WT_SPEED);

  // extra subdivisors
  if(rand() % 100 == 0 && P.curlevel >= 50)
    make(randHCol(), 2 + rand() % 2, WT_SUBD);
  if(rand() % 100 == 0 && P.curlevel >= 50)
    make(randHCol(), 3 + (rand() % 2) * 2, WT_QUI);
  
  // extra golden sectors
  if(rand() % 100 == 0 && P.curlevel >= 50)
    make(9, 1, WT_GOLD);
  
  // extra timedaggers
  if(rand() % 40 == 0 && P.curlevel >= LEVELS)
    make(randHCol(), 1, WT_TIME);

  // extra rainbow blades
  if(rand() % 40 == 0 && P.curlevel >= LEVELS)
    make(randHCol(), P.curlevel, WT_RAIN);

  // extra twisters
  if(rand() % 100 == 0 && P.curlevel >= 100)
    make(1, 2 + (P.curlevel-100)/10, WT_RAND);
  }


void generateExtra() {

  lseed = levseed[0];
  
  generateUniqueItems();
  if(P.curlevel == 998) return;

  generateExtraMonsters();
  generateExtraItems();
  generateDeepItems();
  }

void generateLevel() {

  hydras.clear(); 

  P.flags &= ~dfShadowAware; setDirs();

  vorpalRegenerate();
  
  if(P.curlevel < LEVELS) { 
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
  
  srand(P.curlevel < LEVELS ? levseed[P.curlevel] : levseed[0] + P.curlevel);
  rand(); rand(); rand();
  
  if(P.flags & dfTutorial) {
    P.curlevel = -1;
    generateTutorialLevel();
    return;
    }
    
  do {
    int msize = P.curlevel < LEVELS ? linf[P.curlevel].msize : 10;
    
    int qml = 8;
        
    int levtype = rand() % 1150;
    
    topx = TBAR, topy = TBAR;
    clearLevel();
    
    qml = 8;
    
    if(0)
      setAnyTopology(), brownianMap(); // debug
    
    else if(P.curlevel == LEVELS-1)
      mandelbrotMap(msize), qml = 0;

    else if(P.curlevel < LEVELS || levtype < 150)
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

    else
      setWrapTopology(), lifeMap(msize), qml = 0;

    // addMessage("levtype = "+its(levtype)+" time= "+its(time(NULL))+" tx="+its(topx)+" ty="+its(topy));
    
    
    // mushroom lines are shorter for weird topologies, so generate more of them
    if(topx != TBAR) qml *= 3, qml /= 2;
    if(topy != TBAR) qml *= 3, qml /= 2;
    
    if(DIRS == 6)
      for(int y=0; y<SY; y++) for(int x=0; x<SX; x++) if((y^x)&1)
        M.m[y][x].type = CT_HEXOUT, M.m[y][x].mushrooms = 0;
    
    ensureConnected();
    
    buggylevel = !verifyConnected();
    
    mushroomLines(qml, msize);
  } while(countEmpty() < 100);
  
  // addMessage("done, time= "+its(time(NULL)));
    
  if(P.curlevel < LEVELS) {
  
    for(int i=0; i<size(toput[P.curlevel]); i++) toput[P.curlevel][i]->put();
    toput[P.curlevel].clear();
    }
  
  else generateExtra();
  
  // for(int y=1; y<SY-1; y++) for(int x=1; x<SX-1; x++) M.m[y][x].type = CT_EMPTY, M.m[y][x].mushrooms = 0;

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
  }

int dcolors[HCOLORS];

void generateGame() {

  /* if(gameExists) {
    addMessage("ERROR: generateGame called twice!");
    return;
    } */
  gameExists = true;
  srand(P.gameseed);
  
  int dirtylev = 5 + rand() % 5;

  for(int i=0; i<LEVELS; i++) {
    levseed[i] = rand() * 1000 + rand();
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
    
    int count = 6 + rand() % 5;
    for(int c=0; c<count; c++)
      toput[i].push_back(new item(its[rand() % (sizeof(its) / sizeof(int))]));

    if(i == LEVELS-1) continue; // only items are put on this level
    
    int r = rand() % 6;
    
    // 2/3 chance of a good blade (or a good axe)
    if(r <= 4) {
      if(i > 0 && rand() % 10 <= 2)
        toput[i].push_back(new weapon(randHCol(), linf[i].weapon, WT_AXE));
      else 
        toput[i].push_back(new weapon(randHCol(), linf[i].weapon, WT_BLADE));
      }
    
    // 2/3 chance of a Big Stick; either a good blade or a Bigstick is guaranteed
    if(r >= 2)
      toput[i].push_back(new item(IT_SGROW));
    
    // one hydra of each color
    for(int c=0; c<HCOLORS; c++)
      toput[i].push_back(new hydra(c, rand() % linf[i].maxheads + 1, linf[i].growlimit, linf[i].heal));
    
    // make some hydras dirty
    if(i == 10 || i == dirtylev) {
      hydra *H = (hydra*) toput[i][size(toput[i])-1-rand() % 10];
      H->dirty = 2*IS_DIRTY-1;
      H->heal += 5;
      }
    
    // 3 * 1/3 chance of another blade
    for(int u=0; u<3; u++) if(rand() % 3 == 0)
      toput[i].push_back(new weapon(randHCol(), 1 + rand() % linf[i].weapon, WT_BLADE));

    // 2 * 1/2 chance of a bludgeon
    for(int u=0; u<2; u++) if(rand() % 2 == 0)
      toput[i].push_back(new weapon(randSCol(), 1 + rand() % linf[i].bweapon, WT_BLUNT));    
    }
  
  for(int i=0; i<HCOLORS; i++) dcolors[i] = i;
  for(int i=0; i<HCOLORS; i++) swap(dcolors[i], dcolors[rand() % (i+1)]);
  sclass *extras[LEVELS];
  
  // original weapons and extras...
  
  if(!stats.savecount) {
    wpn[0] = new weapon(dcolors[0], 1, WT_BLADE);
    wpn[1] = new weapon(dcolors[1], 2, WT_BLADE);
    wpn[0]->level = wpn[1]->level = 0;
    }
    
  int artdie = rand() % 6;

  extras[0] = new item(IT_PARMS);
  extras[1] = new item(IT_PARMS);
  extras[2] = new weapon(dcolors[2], 4, WT_MSL);
  extras[3] = new weapon(dcolors[3], 2, WT_SHLD);
  extras[4] = new item(IT_PARMS);
  extras[5] = new weapon(dcolors[4], 3 + rand() % 3, WT_SHLD);
  
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
  swap(extras[2], extras[1 + rand() % 2]);
  swap(extras[3], extras[1 + rand() % 3]);
  
  // randomize 4-6
  swap(extras[5], extras[4 + rand() % 2]);
  swap(extras[6], extras[4 + rand() % 3]);

  // randomize 7-9
  swap(extras[8], extras[7 + rand() % 2]);
  swap(extras[9], extras[7 + rand() % 3]);
  
  for(int u=0; u<LEVELS; u++) toput[u].push_back(extras[u]);
  
  toput[8 + rand() % 2].push_back(new weapon(dcolors[7], 10, WT_DIV));
  toput[7 + rand() % 3].push_back(new weapon(HC_ANCIENT, 1, WT_SHLD));
  
  // extra small shield
  if(rand() % 2 == 0) toput[4 + rand() % 6].push_back(new weapon(dcolors[8], 2, WT_SHLD));

  // extra larger shields
  if(rand() % 3 == 0) toput[7 + rand() % 4].push_back(new weapon(randHCol(), 3, WT_SHLD));
  if(rand() % 4 == 0) toput[8 + rand() % 3].push_back(new weapon(randHCol(), 4, WT_SHLD));
  
  // extra missiles
  if(rand() % 2 == 0) toput[4 + rand() % 7].push_back(new weapon(randHCol(), 1, WT_MSL));
  if(rand() % 2 == 0) toput[4 + rand() % 7].push_back(new weapon(randHCol(), 2, WT_MSL));
  if(rand() % 2 == 0) toput[4 + rand() % 7].push_back(new weapon(dcolors[9], 4, WT_MSL));
  int lev = 3 + rand() % 7;
  toput[lev].push_back(new weapon(randSCol(), lev/2, WT_STONE));
  
  // runes of fungification
  for(int i=0; i<10; i++) if(rand() % 4 == 0)
    toput[rand() % 11].push_back(new item(IT_RFUNG));

  // runes of fungal necromancy
  for(int i=0; i<10; i++) if(rand() % 4 == 0)
    toput[3 + rand() % 8].push_back(new item(IT_RNECR));

  // scrolls of reduction
  for(int i=0; i<10; i++) if(rand() % 3 == 0)
    toput[3 + rand() % 8].push_back(new item(IT_SPART));

  // potions of ambidexterity
  for(int i=0; i<8; i++) if(rand() % 2 == 0)
    toput[rand() % 11].push_back(new item(IT_PAMBI));

  // potions of weapon charge
  for(int i=0; i<10; i++) if(rand() % 3 == 0)
    toput[rand() % 11].push_back(new item(IT_PCHRG));

  // two extra Big Sticks
  for(int i=0; i<2; i++) toput[rand() % 11].push_back(new item(IT_SGROW));

  // 4 special monsters on levels 4 to 10
  int tab[7];
  for(int t=0; t<7; t++) tab[t] = 4+t;
  for(int t=0; t<7; t++) swap(tab[t], tab[rand() % (t+1)]);
  
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
      toput[i].push_back(new hydra(HC_VAMPIRE, linf[i].maxheads - rand() % 3, linf[i].growlimit, linf[i].heal + 5));
    }

  // boss monster
  toput[LEVELS-1].push_back(new hydra(HC_ANCIENT, 99000 + rand() % 1000, linf[10].growlimit, 0));
  
  toput[LEVELS-1].push_back(new weapon(dcolors[9], 2, WT_ROOT));
  
  // store level info for weapons
  for(int u=0; u<LEVELS; u++) for(int v=0; v<size(toput[u]); v++) {
    weapon* w = toput[u][v]->asWpn();
    if(w) w->level = u+1;
    }
  }

void fixTheSavefile() {
  if(P.curlevel >= 0 && !verifyConnected(1)) {
    ensureConnected();
    addMessage("A sudden earthquake shakes the reality!");
    }
  }
