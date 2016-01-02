#include "../src/noteye.h"
#define main hydraMain // SDL redefines this

using namespace std;

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

SDL_Surface *hydrabox, *hydraheads;

int *hydraboxc; // 'changes' field which keeps track of updates

static int ZZ;
static int& qpixel(SDL_Surface *surf, int x, int y) {
  if(x<0 || y<0 || x >= surf->w || y >= surf->h) return ZZ;
  char *p = (char*) surf->pixels;
  p += y * surf->pitch;
  int *pi = (int*) (p);
  return pi[x];
  }

void drawhead(int x, int y, int ridx) {
  x -= 5; y -= 5;
  int by = 0;
  int hm = 0;
  for(int u=0; u<16; u++) hm += nrand0(ridx+u);
  if(hm % 4 == 0) by = 15;
  for(int ky=0; ky<15; ky++) for(int kx=0; kx<12; kx++) {
    int hdc = qpixel(hydraheads, hydrapic_id*12+kx, by+ky);
    // for some reason, on Mac it is 0xFF007F7F, not 0xFF008080
    if(hdc != (int) 0xFF008080 && hdc != (int) 0xFF007F7F) {
      hdc -= 0xFF000000;
      int ax = x+kx, ay = y+ky;
      // another Mac weirdness
      if(hdc > 9 && hdc < 15) hdc = 9;
      if((ax|31) == 31 && (ay|31) == 31) 
        hmap[ay][ax] = (hdc && hdc <= 9 ? (hdc+6) * hydrapic_col : hdc);
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
    for(int t=1; t<q; t++) if(frand(fidx+t, 2)) sa--;
    }
  
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
  splitdown(4, 4, 28, 29, 29, 4, 28, hds, 1, id2, id2, splits);
  drawshadows();
  for(int h=0; h<nhead; h++) 
    drawhead(headx[h], heady[h], headr[h]);
  
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
