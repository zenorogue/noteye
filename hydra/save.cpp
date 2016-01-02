// Hydra Slayer: math puzzle roguelike
// Copyright (C) 2010-2011 Zeno Rogue, see 'hydra.cpp' for details

#define SAVEFORMAT 104

#ifdef MINGW
#include <io.h>
#else
#include <unistd.h>
#endif

FILE *savefile;

bool error;

template<class T> void save(const T& t) { fwrite(&t, 1, sizeof(t), savefile); }
template<class T> void load(T& t, int size = sizeof(T)) { if(error) return; if(fread(&t, size, 1, savefile) != 1) error = true; }

void saveString(const string& s) {
  int size = s.size();
  save(size);
  for(int i=0; i<size; i++) save(s[i]);
  }

string loadString() {
  int size; load(size);
  string ret(size, ' ');
  for(int i=0; i<size; i++) { load(ret[i]); }
  return ret;
  }

void deleteGame(string sav = savename) { unlink(sav.c_str()); }

void saveGame(string sav = savename) {

  if(!P.arms) {
    deleteGame();
    return;
    }

  savefile = fopen(sav.c_str(), "wb");
  
  error = !savefile;
  
  if(error) return;
  
  P.saveformat = SAVEFORMAT;
  P.version = VERSION;
  stats.whistSize = size(pinfo.whist);
  P.stairqueue = size(stairqueue);
  
  save(P);
  save(stats);
  save(playerpos); save(topx); save(topy);
  saveString(pinfo.charname);
  saveString(pinfo.username); 
  
  if(P.twinsNamed)
    saveString(pinfo.twin[0]),
    saveString(pinfo.twin[1]);

  int set = 0; for(int i=0; i<MAXARMS; i++) if(wpn[i]) set |= (1<<i);
  
  save(set); for(int i=0; i<MAXARMS; i++) if(wpn[i]) wpn[i]->csave();
  
  if(true) {
    int trollsize = size(pinfo.trollwpn);
    save(trollsize);
    for(int i=0; i<trollsize; i++) save(pinfo.trollkey[i]);
    for(int i=0; i<trollsize; i++) pinfo.trollwpn[i]->csave();
    }
  
  for(int i=0; i<P.stairqueue; i++) stairqueue[i]->csave();
  
  for(int i=0; i<stats.whistSize; i++) save(pinfo.whistAt[i]), pinfo.whist[i]->csave();
  
  for(int y=0; y<SY; y++) for(int x=0; x<SX; x++) {
    cell& c(M.m[y][x]);
    #define ITEMMASK (1<<24)
    if(c.it) c.mushrooms |= ITEMMASK;
    save(c.type), save(c.mushrooms), save(c.dead), save(c.explored);
    if(c.it) c.it->csave();
    c.mushrooms &= ~ITEMMASK;
    }  
  
  int32_t i = hydras.size();
  save(i); for(int i=0; i<size(hydras); i++) hydras[i]->csave();
  
  fclose(savefile);
  }

sclass *loadS() {
  sclass *ret;
  typeof(ret->sct()) i;
  load(i);
  
  if(i == 0) {
    load(i); // a bugfix for format incompatibility
    }
  
  if(i == SCT_ITEM) ret = new item(0);
  else if(i == SCT_WPN) ret = new weapon(0,0,0);
  else if(i == SCT_HYDRA) ret = new hydra(0,0,1,0);
  else {
    fprintf(stderr, "fatal: unknown class\n");
    exit(1);
    }
  ret->cload();
  return ret;
  }

void sclass::csave() { save(sct()); }

void item::csave() { sclass::csave(); save(type); }
void weapon::csave() { 
  sclass::csave(); save(type), save(size), save(color), save(level), save(ocolor), save(osize), save(sc);
  }
void hydra::csave() {
  sclass::csave();
  save(color), save(heads), save(sheads), save(stunforce), save(heal), save(res);
  save(conflict), save(zombie), save(ambi_OUT), save(pos);
  if(color == HC_ETTIN) ewpn->csave();
  if(color == HC_MONKEY) {
    bool b = ewpn;
    save(b);
    if(b) ewpn->csave();
    }
  save(dirty);
  save(awareness);
  }

void item::cload() { load(type); }
void weapon::cload() {
  load(type), load(size), load(color), load(level), load(ocolor), load(osize), load(sc);
  }
void hydra::cload() { 
  load(color), load(heads), load(sheads), load(stunforce), load(heal), load(res);
  load(conflict), load(zombie), load(ambi_OUT), load(pos);
  if(color == HC_ETTIN) ewpn = (weapon*) loadS();
  if(color == HC_MONKEY) {
    bool b;
    load(b);
    if(b) ewpn = (weapon*) loadS();
    else ewpn = NULL;
    }
  if(P.version >= 1300) load(dirty); else dirty = 0;
  if(P.version >= 1350) load(awareness); else awareness = 0;
  }

// shift for backward compatibility
template<class x, class y> int pdif(x& a, y& b) { return (char*)&b - (char*) &a; }
template<class x> void loadshift(x& a, int pos, int shift) {
  char *v = (char*) &a;
  memmove(v+pos+shift, v+pos, sizeof(a) - pos-shift);
  memset(v+pos, 0, shift);
  }

#define HYDRADATE 1278319078

#define ofs(x) (((char*)(&S.x)) - ((char*)(&S)))

void loadStats(statstruct& S, int saveformat) {
  if(saveformat == 103) {
    /*
    fprintf(stderr, "mot1 = %d\n", pdif(S, S.maxoftype));
    fprintf(stderr, "mot2 = %d\n", pdif(S, S.motwin));
    fprintf(stderr, "mot3 = %d\n", pdif(S, S.ws));
    fprintf(stderr, "mot4 = %d\n", pdif(S, S.woundwin));
    fprintf(stderr, "size = %d\n", int(sizeof(statstruct)));
    // 164 228 292 740  1208
    // 164 292 420 1316 1784
    //     +64 +64 +448
    
    // 740 + 448
    // 292 + 64
    // 228 + 64
    
    */
    load(S, 1208);
    loadshift(S, 740, 448);
    loadshift(S, 292, 64);
    loadshift(S, 228, 64);
    }
  else load(S);
  
  if(S.gamestart < HYDRADATE || S.gameend < HYDRADATE) {
    char *c = (char*) &S;
    memmove(c+4, c+8, sizeof(statstruct)-8);
    memmove(c+ofs(gameend), c+ofs(gameend)+4, sizeof(statstruct)-ofs(gameend)-8);
    memmove(c+ofs(solved), c+ofs(solved)+4, sizeof(statstruct)-ofs(solved)-12);
    if(saveformat != 103) load(c[sizeof(statstruct)-12], 12);
    }
  
  }


void loadGame(string sav = savename) {
  gameExists = false;
  savefile = fopen(sav.c_str(), "rb");

  if(!savefile) { error = true; return; }
  error = false;
  
  load(P);

  if(P.saveformat < 103) {
    printf("Savefile format incompatible\n");
    return;
    }
  
  loadStats(stats, P.saveformat);
  if(P.geometry == 0) P.geometry = 8; setDirs();
  load(playerpos); load(topx); load(topy);
  pinfo.charname = loadString();
  pinfo.username = loadString();  

  if(P.twinsNamed)
    pinfo.twin[0] = loadString(),
    pinfo.twin[1] = loadString();
  
  if(P.version != VERSION) P.vchanged = true;
  
  int set; load(set); for(int i=0; i<MAXARMS; i++) if((set>>i) & 1) wpn[i] = (weapon*) loadS();

  if(P.race == R_TROLL || (P.version >= 1030)) {
    int trollsize;
    load(trollsize);
    pinfo.trollwpn.clear(); pinfo.trollkey.resize(trollsize);
    for(int i=0; i<trollsize; i++) load(pinfo.trollkey[i]);
    for(int i=0; i<trollsize; i++) pinfo.trollwpn.push_back((weapon*) loadS());;
    }

  for(int i=0; i<P.stairqueue; i++) stairqueue.push_back((hydra*) loadS());
  
  pinfo.whistAt.resize(stats.whistSize);
  for(int i=0; i<stats.whistSize; i++) 
    load(pinfo.whistAt[i]), pinfo.whist.push_back((weapon*) loadS());
  
  for(int y=0; y<SY; y++) for(int x=0; x<SX; x++) {
    cell& c(M.m[y][x]);
    c.it = NULL;
    load(c.type), load(c.mushrooms), load(c.dead), load(c.explored);
    if(c.mushrooms & ITEMMASK) {
      c.mushrooms &= ~ITEMMASK;
      c.it = loadS();
      }
    if(c.type == CT_STAIRUP)
      stairpos = vec2(x,y);
    }
  M.out.clear(); M.out.type = CT_WALL;
  
  int32_t i; load(i); hydras.resize(i);
  for(int i=0; i<size(hydras); i++) {
    hydra *h = (hydra*) loadS();
    hydras[i] = h;
    M[h->pos].h = h;
    if(h->color == HC_TWIN) twin = h;
    }
  
  char x; if(error || fread(&x, 1, 1, savefile) != 0) error = true;
  
  fclose(savefile);
  stats.savecount++;
  
  if(error) { 
    printf("Error while loading savegame - file deleted\n"); 
    deleteGame();
    exit(0);
    }
  else gameExists = true;
  }

void emSaveGame() {
  int ts = stats.tstart;
  stats.tstart = time(NULL) - stats.tstart; stats.emsave++;
  saveGame();
  stats.tstart = ts; stats.emsave--;
  }
