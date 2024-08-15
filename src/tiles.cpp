// Necklace of the Eye v6.2
// roguelike frontend
// Copyright (C) 2010-2011 Zeno Rogue, see 'noteye.h' for details

namespace noteye {

std::set<TileImage*> all_images;

#define HASHMAX 65535

#define COLMOD 65519

void TileImage::debug() {
  printf("%p: image from %p [%s]\n", this, i.base, i->title.c_str());
  }

int tohash(Object *t) {
  return (int)(long long)t;
  }
 
template<class T> int tohash(const smartptr<T>& t) {
  return (int)(long long)t.base;
  }
 
int TileMerge::hash() const {
  return (tohash(t1) ^ (tohash(t2) * 13157)) & HASHMAX;
  }

void TileMerge::debug() {
  printf("%p: merge %p + %p\n", this, t1.base, t2.base);
  t1->debug(); t2->debug();
  }

int TileFill::hash() const {
  return (alpha + (color % COLMOD)) & HASHMAX;
  }

void TileFill::debug() {
  printf("%p: color %8x alpha %8x\n", this, color, alpha);
  }

int TileRecolor::hash() const {
  return ((color % COLMOD) ^ (mode + tohash(t1) * 13157)) & HASHMAX;
  }

int TileImage::hash() const {
  return (tohash(i) + ox * 513 + oy * 1351) & HASHMAX;
  }

void TileRecolor::debug() {
  printf("%p: recolor %p, color %8x mode %d\n", this, t1.base, color, mode);
  t1->debug();
  }

int TileSpatial::hash() const {
  return (tohash(t1)+sf) & HASHMAX;
  }

void TileSpatial::debug() {
  printf("%p: %p, spatial %8x\n", this, t1.base, sf);
  t1->debug();
  }

int TileLayer::hash() const {
  return (tohash(t1)+layerid) & HASHMAX;
  }

void TileLayer::debug() {
  printf("%p: %p, layer %d\n", this, t1.base, layerid);
  t1->debug();
  }

int TileTransform::hash() const {
  return (tohash(t1)+1973) & HASHMAX;
  }

void TileTransform::debug() { 
  printf("%p: transform %p\n", this, t1.base);
  t1->debug();
  }

int TileFreeform::hash() const {
  return (tohash(t1)+tohash(par)) & HASHMAX;
  }

void TileFreeform::debug() { 
  printf("%p: freeform %p\n", this, t1.base);
  t1->debug();
  }

#define SEPARATE_HASH 1 // needs C++14

#if SEPARATE_HASH
template<class T> T* hashtab_of[HASHMAX+1];
#else
Tile* hashtab[HASHMAX+1];
#endif

//--

bool eq(const TileImage& a, const TileImage& b) {
  return a.i == b.i && a.ox == b.ox && a.oy == b.oy && a.sx == b.sx && a.sy == b.sy &&
    a.trans == b.trans;
  }

bool eq(const TileMerge& a, const TileMerge& b) {
  return a.t1 == b.t1 && a.t2 == b.t2 && a.over == b.over;
  }

bool eq(const TileFill& a, const TileFill& b) {
  return a.color == b.color && a.alpha == b.alpha;
  }

bool eq(const TileRecolor& a, const TileRecolor& b) {
  return a.color == b.color && a.t1 == b.t1 && a.mode == b.mode;
  }

bool eq(const TileSpatial& a, const TileSpatial& b) {
  return a.t1 == b.t1 && a.sf == b.sf;
  }

bool eq(const TileLayer& a, const TileLayer& b) {
  return a.t1 == b.t1 && a.layerid == b.layerid;
  }

bool eq(const TileTransform& a, const TileTransform& b) {
  return a.t1 == b.t1 && a.dx == b.dx && a.dy == b.dy && a.sx == b.sx && a.sy == b.sy &&
    a.dz == b.dz && a.rot == b.rot;
  }

bool eq(const TileFreeform& a, const TileFreeform& b) {
  return a.t1 == b.t1 && a.par == b.par;
  }

int hashok = 0, hashcol = 0;

Tile::~Tile() { 
  if(nextinhash) nextinhash->previnhash = previnhash;
  if(previnhash) *previnhash = nextinhash; 

  for(auto& mapping: in_maps)
    mapping->uncache(this);
  }

template<class T> T* registerTile(const T& x) {

  #if SEPARATE_HASH
  using Tred = T;
  auto& hashtab = hashtab_of<T>;
  #else
  using Tred = Tile;
  #endif

  int hsh = x.hash();
  
  auto hso (&(hashtab[hsh]));
  auto hs = hso;
  while(*hs) {
    if((*hs)->previnhash != (Tile**) hs) printf("hashtable error!\n");
    #if SEPARATE_HASH
    auto y = *hs;
    #else
    T* y = dynamic_cast<T*> (*hs);
    #endif
    if(y && eq(x,*y)) { 
      hashok++; 
      if(hs != hso) {
        // move to the front
        Tile *nih = (*hs)->nextinhash;
        if(nih) nih->previnhash = (Tile**) hs; // (*hs)->previnhash;
        *hs = (Tred*) nih;

        (*hso)->previnhash = &(y->nextinhash);
        y->nextinhash = (Tile*) *hso;
        y->previnhash = (Tile**) hso;
        *hso = y;
        }
      return y; 
      }
    else {
      hashcol++;
      hs = (Tred**) &(*hs)->nextinhash;
      }
    }
    
  T *xc = new T;
  *xc = x;
  if(xc->refcount || xc->next_to_delete) printf("bad copy\n");
  xc->nextinhash = *hso;
  if(*hso) (*hso)->previnhash = &(xc->nextinhash);
  xc->previnhash = (Tile**) hso;
  *hso = xc;
  registerObject(xc);
  xc->preprocess();
  return xc;
  }

extern "C" {

TileImage* addTile(Image *i, int ox, int oy, int sx, int sy, int trans) {
  if(sx == 0 || sy == 0) {
    if(logfile)
      fprintf(logfile, "WARNING: attept to create tile of size %dx%d", sx, sy);
    return 0;
    }
  if(i == NULL) return NULL;
  TileImage T(sx, sy);
  T.i = i;
  T.ox = ox;
  T.oy = oy;
  T.trans = trans;
  return registerTile(T);
  }

Tile* addMerge(Tile *t1, Tile *t2, bool over) {
  // an optimization for merging zeros
  if(t1 == NULL) return t2;
  if(t2 == NULL) return t1;

  TileMerge T;
  T.t1 = t1;
  T.t2 = t2;
  T.over = over;
  
  return registerTile(T);
  }

TileLayer* addLayer(Tile *t1, int layerid) {
  if(t1 == NULL) return NULL;
  TileLayer TL;
  TL.t1 = t1;
  TL.layerid = layerid;
  return registerTile(TL);
  }

TileSpatial* addSpatial(Tile *t1, int sf) {
  if(t1 == NULL) return NULL;
  TileSpatial TSp;
  TSp.t1 = t1;
  TSp.sf = sf;
  return registerTile(TSp);
  }

TileTransform* addTransform(Tile *t1, double dx, double dy, double sx, double sy, double dz, double rot) {
  TileTransform TT;
  TT.t1 = t1;
  TT.dx = dx; TT.dy = dy;
  TT.sx = sx; TT.sy = sy;
  TT.dz = dz; TT.rot = rot;
  return registerTile(TT);
  }

TileFreeform* addFreeform(Tile *t1, FreeFormParam *p) {
  if(t1 == NULL) return NULL;
  TileFreeform TFF;
  TFF.t1 = t1;
  TFF.par = p;
  return registerTile(TFF);
  }

TileTransform* cloneTransform(Tile *t1, TileTransform *example) {
  if(t1 == NULL) return NULL;
  TileTransform TT;
  TT.t1 = t1;
  TT.dx = example->dx;
  TT.dy = example->dy;
  TT.sx = example->sx;
  TT.sy = example->sy;
  TT.dz = example->dz;
  TT.rot = example->rot;
  TT.nextinhash = NULL;
  TT.previnhash = NULL;
  return registerTile(TT);
  }
}

//--

#define rectrans 0xDEBEEF

void TileRecolor::recache() {
  if(!cache) return;
  StaticGet(TileImage, TIC, t1);
  int sx = TIC->sx, sy = TIC->sy;
  Image *i = cache->i.base;
  for(int y=0; y<sy; y++) for(int x=0; x<sx; x++) {
    noteyecolor pix = qpixel(TIC->i->s, TIC->ox+x, TIC->oy+y);
    if(istrans(pix, TIC->trans)) continue;
    noteye::recolor(pix, color, mode);
    qpixel(i->s, x, y) = pix;
    }
  }

TileRecolor::~TileRecolor() {
  if(cache) {
    StaticGet(TileImage, TIC, t1);
    totalimagecache -= TIC->sx * TIC->sy;
    IMGOBSERVE ( printf("... DEL tilerecolor cache %p %s\n", TIC->i.base, TIC->i->title.c_str()); )
    }
  }
  
void TileRecolor::preprocess() {
  auto TIC = t1->asImage();
  if(TIC) {
    int sx = TIC->sx, sy = TIC->sy;
    Image *i = new Image(sx, sy, TIC->trans == transAlpha ? 0 : rectrans);
    totalimagecache += sx * sy;
    char buf[256];
    sprintf(buf, "[%08x %d] ", color, mode);
    i->title = buf + TIC->i->title;
    cache = addTile(i, 0, 0, sx, sy, TIC->trans == transAlpha ? transAlpha : rectrans);
    cachechg = TIC->i->changes;
    recache();
    IMGOBSERVE ( printf("... %p tilerecolor cache %s\n", i, i->title.c_str()); )
    }
  else
    cache = 0;
  }

#include <typeinfo>

extern "C" {

TileFill *addFill(noteyecolor color, noteyecolor alpha) {
  TileFill TF;
  TF.color = color;
  TF.alpha = alpha;
  TF.cache = NULL;
  return registerTile(TF);
  }

Tile *addRecolor(Tile *t1, noteyecolor color, int mode) {

  // optimizations first
  if(color == noteyecolor(-1)) return t1;
  if(t1 == NULL) return NULL;

  return t1->recolor(color, mode);
  }

Tile *Tile::recolor(noteyecolor color, int mode) {
  if(color == noteyecolor(-1)) return this;
  TileRecolor T;
  T.t1 = this;
  T.color = color;
  T.mode = mode;
  T.cache = 0;
  
  return registerTile(T);
  }

noteyecolor getCol(Tile *x) {
  if(!x) return noteyecolor(-1);
  return x->getCol();
  }

Image *getImage(Tile *x) {
  if(!x) return NULL;
  return x->getImage();
  }

int getChar(Tile *x) {
  if(!x) return -1;
  return x->getChar();
  }

noteyecolor getBak(Tile *x) {
  //Get(TileRecolor, TR, x);
  //if(TR) return TR->color;
  if(!x) return -1;
  return x->getBak();
  }

void tileSetChid(Tile *x, int chid) {
  Get(TileImage, TI, x);
  if(TI) TI->chid = chid;
  }

Tile *tileSetFont(Tile *x, Font *f) {
  if(!x) return x;
  return x->setFont(f);
  }

Tile *TileImage::setFont(Font *f) {
  if(chid >= 0 && chid < 256) return f->gettile(chid);
  return this;
  }

Tile *distillLayer(Tile *x, int layerid) {
  if(!x) return x;
  return x->distillLayer(layerid);
  }

Tile *distill(Tile *x, int sp) {

  Get(TileImage, TI, x);
  if(TI) return x;
  
  Get(TileRecolor, TR, x);
  if(TR && TR->cache) {
    Get(TileImage, TIR, TR->t1);
    if(TIR->i->changes != TR->cachechg) {
      /*
      // invalidate cache!
      Get(TileImage, TIRC, TR->cache);
      delete TIRC->i;
      deleteobj(TR->cache);
      TR->cache = 0;
      TR->preprocess();
      */
      TR->cachechg = TIR->i->changes;
      TR->recache();
      }
    }
  if(TR && TR->cache) return TR->cache;
  if(TR) {
    Tile *ds = distill(TR->t1, sp);
    Get(TileMerge, TM, ds);
    if(TM) {
      return addMerge( distill(addRecolor(TM->t1, TR->color, TR->mode), sp), distill(addRecolor(TM->t2, TR->color, TR->mode), sp), TM->over);
      }
    
    Get(TileTransform, TT, ds);
    if(TT) {
      Tile *t1 = distill(addRecolor(TT->t1, TR->color, TR->mode), sp);
      return cloneTransform(t1, TT);
      }

    Get(TileFreeform, TFF, ds);
    if(TFF) {
      Tile *t1 = distill(addRecolor(TFF->t1, TR->color, TR->mode), sp);
      return addFreeform(t1, TFF->par);
      }

    Get(TileRecolor, TRe, ds);
    if(TRe) {
      return addRecolor(TRe->cache, TR->color, TR->mode);
      }

    Tile *i = addRecolor(ds, TR->color, TR->mode);
    return distill(i, sp);
    }
  
  Get(TileSpatial, TS, x);
  if(TS && (TS->sf & sp)) return distill(TS->t1, sp);
  
  Get(TileLayer, TL, x);
  if(TL) return distill(TL->t1, sp);
  
  Get(TileMerge, TM, x);
  if(TM) return addMerge( distill(TM->t1, sp), distill(TM->t2, sp), TM->over);
  
  Get(TileFill, TF, x);
  if(TF) return x;
  
  Get(TileTransform, TT, x);
  if(TT) {
    return cloneTransform(distill(TT->t1, sp), TT);
    }
  
  Get(TileFreeform, TFF, x);
  if(TFF) {
    Tile *di = distill(TFF->t1, sp);
    return addFreeform(di, TFF->par);
    }
  
  return 0;
  }
}

char tab[4] = {0,0,0,0};

extern "C" {

TileMapping *get_tmlayer(int layer) { return tmLayer[layer]; }

Tile *addMerge0(Tile *t1, Tile *t2) { return addMerge(t1, t2, false); }
Tile *addMerge1(Tile *t1, Tile *t2) { return addMerge(t1, t2, true); }

const char* getChar2(Tile *t) { int i = getChar(t); if(i==-1) return tab; else return utf8_encode(i); }

Tile *gp2(Tile *x) {
  Get(TileMerge, T, x);
  if(!T) return NULL;
  return T->t2;
  }

struct avcoba { int chr, col, bak; };

avcoba gavcoba(Tile *x) {
  avcoba res;
  res.chr = getChar(x);
  res.col = getCol(x);
  res.bak = getBak(x);
  return res;
  }

Tile *tileavcobaf(int kv, noteyecolor color, noteyecolor back, Font *F) {
  if(!F) return NULL;
  return addMerge(addFill(back, 0xFFFFFF), addRecolor(F->gettile(kv), color, recDefault), false);
  }

FreeFormParam *freeformparam(double x00, double x01, double x02, double x03, double x10, double x11, double x12, double x13, double x20, double x21, double x22, double x23, double x30, double x31, double x32, double x33) {
  FreeFormParam *P = new FreeFormParam;
  P->d[0][0] = x00;
  P->d[0][1] = x01;
  P->d[0][2] = x02;
  P->d[0][3] = x03;
  P->d[1][0] = x10;
  P->d[1][1] = x11;
  P->d[1][2] = x12;
  P->d[1][3] = x13;
  P->d[2][0] = x20;
  P->d[2][1] = x21;
  P->d[2][2] = x22;
  P->d[2][3] = x23;
  P->d[3][0] = x30;
  P->d[3][1] = x31;
  P->d[3][2] = x32;
  P->d[3][3] = x33;
  P->side = 4;
  P->shiftdown = false;
  return registerObject(P);
  }
  
void freeformparamflags(FreeFormParam* P, int side, bool shiftdown) {
  P->side = side;
  P->shiftdown = shiftdown;
  }
  
void tiledebug(Tile *T) {
  if(!T) { fprintf(stderr, "null\n"); }
  else T->debug();
  }

}

TileImage::TileImage(int _sx, int _sy) {
  sx = _sx; sy = _sy; chid = '?';
  ox = 0; oy = 0; 
  gltexture = NULL;
  sdltexture = NULL;
  bcurrent = -1;
  all_images.insert(this);
  }

TileImage::TileImage() {
  sx = 0; sy = 0; chid = '?';
  ox = 0; oy = 0; 
  gltexture = NULL;
  sdltexture = NULL;
  bcurrent = -1;
  all_images.insert(this);
  }

void provideBoundingBox(TileImage *T) {
  if(T->bcurrent == T->i->changes) return;
  T->bcurrent = T->i->changes;

  T->bx = 0, T->by = 0;
  T->tx = T->sx, T->ty = T->sy;

  for(int ay=0; ay<T->sy; ay++) for(int ax=0; ax<T->sx; ax++)
    if(!istransA(qpixel(T->i->s, T->ox+ax, T->oy+ay), T->trans)) {
      if(ax < T->tx) T->tx = ax;
      if(ay < T->ty) T->ty = ay;
      if(ax >= T->bx) T->bx = ax+1;
      if(ay >= T->by) T->by = ay+1;
      }
  
  /* printf("bouding-box: %s(%d,%d) = (%d,%d) to (%d,%d)\n",
    T->i->title.c_str(), T->ox, T->oy, T->bx, T->by, T->tx, T->ty
    ); */
  }

int getFppDown(TileImage *T) {
  provideBoundingBox(T);
  return T->sy - T->by;
  }

#define Info(x, y, z) if(id++ == i) return z;

extern "C" {

int getobjectinfo(Object *o, int i) {
  int id = 0;
  Get(TileImage, TI, o);
  if(TI) {
    Info(L, "type", 0x11);
    Info(L, "ox", TI->ox);
    Info(L, "oy", TI->oy);
    Info(L, "sx", TI->sx);
    Info(L, "sy", TI->sy);
    Info(L, "ch", TI->chid);
    Info(L, "trans", TI->trans);
    Info(L, "i", noteye_assign_handle(TI->i));
    Info(L, "bottom", getFppDown(TI));
    return id;
    }

  Get(TileRecolor, TR, o);
  if(TR) {
    Info(L, "type", 0x21);
    Info(L, "t1", noteye_assign_handle(TR->t1));
    Info(L, "mode", TR->mode);
    Info(L, "color", TR->color);
    return id;
    }
  
  Get(TileMerge, TM, o);
  if(TM) {
    Info(L, "type", TM->over ? 0x18 : 0x12);
    Info(L, "t1", noteye_assign_handle(TM->t1));
    Info(L, "t2", noteye_assign_handle(TM->t2));
    return id;
    }
  
  Get(TileSpatial, TSp, o);
  if(TSp) {
    Info(L, "type", 0x14);
    Info(L, "t1", noteye_assign_handle(TSp->t1));
    Info(L, "sf", TSp->sf);
    return id;
    }
  
  Get(TileLayer, TL, o);
  if(TL) {
    Info(L, "type", 0x19);
    Info(L, "t1", noteye_assign_handle(TL->t1));
    Info(L, "sf", TL->layerid);
    return id;
    }
  
  Get(TileFill, TF, o);
  if(TF) {
    Info(L, "type", 0x20);
    Info(L, "color", TF->color);
    Info(L, "alpha", TF->alpha);
    return id;
    }
  
  Get(Tile, TX, o);
  if(TX) {
    Info(L, "type", 0x10);
    return id;
    }

  Info(L, "type", 0);
  return id;
  }
}

#undef Info

}
