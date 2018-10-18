// Necklace of the Eye v6.2
// roguelike frontend
// Copyright (C) 2010-2011 Zeno Rogue, see 'noteye.h' for details

#include <complex>
#include <math.h>

namespace noteye {

viewpar V;

SDL_Surface *viewimg;
Image *viewimage;
Window *viewglwindow;
Window *viewsdlwindow;

int xscale(double x, double y) {
  int res = int(V.xm + V.xs * x/y);
  return res;
  }

int yscale(double z, double y) {
  int res = int(V.ym + V.ys * z/y);
  return res;
  }

typedef complex<double> cd;

cd facevec;
cd facevecc;

cd rot(int x, int y) { return cd(x*V.xz,y*V.yz) * facevecc; }
cd rot(double x, double y) { return cd(x*V.xz,y*V.yz) * facevecc; }

cd p[257];  

double det(fpoint4 a, fpoint4 b, fpoint4 c) {
  return
    a.x * b.y * c.z - a.x * b.z * c.y
  + a.y * b.z * c.x - a.y * b.x * c.z
  + a.z * b.x * c.y - a.z * b.y * c.x;
  }

fpoint4 operator + (const fpoint4& a, const fpoint4& b) {
  return fpoint4(a.x+b.x, a.y+b.y, a.z+b.z);
}

fpoint4 operator - (const fpoint4& a, const fpoint4& b) {
  return fpoint4(a.x-b.x, a.y-b.y, a.z-b.z);
}

fpoint4 operator - (const fpoint4& a) {
  return fpoint4(-a.x, -a.y, -a.z);
  }

fpoint4 operator * (fpoint4 t, double a) {
  return fpoint4(t.x*a, t.y*a, t.z*a);
  }

fpoint4 rot4(int x, int y, int z) {
  cd xy = cd(x*V.xz,y*V.yz) * facevecc;
  return fpoint4(real(xy), imag(xy), z);
  }

fpoint4 rot4(double x, double y, double z) {
  cd xy = cd(x*V.xz,y*V.yz) * facevecc;
  return fpoint4(real(xy), imag(xy), z);
  }

fpoint4 rotgl(int x, int y, int z, int sx, int sy) {
  cd xy = cd(x*V.xz,y*V.yz) * facevecc;
  return fpoint4(real(xy)+sx, -z, sy-imag(xy));
  }

fpoint4 rotgl(double x, double y, int z, int sx, int sy) {
  cd xy = cd(x*V.xz,y*V.yz) * facevecc;
  return fpoint4(real(xy)+sx, -z, sy-imag(xy));
  }

int min3(int a, int b, int c) { return min(a, min(b,c)); }
int max3(int a, int b, int c) { return max(a, max(b,c)); }

int cross(int x1,int y1,int x2,int y2,int x3,int y3) {
  return (x2-x1)*(y3-y1)-(x3-x1)*(y2-y1);
  }

struct pt2 { int x, y; };

vector<pt2> fpporder;

int hexdist(pt2 p) {
  if(p.x<0) p.x=-p.x;
  if(p.y<0) p.y=-p.y;
  return max(p.y*2, p.x+p.y);
  }

int hexdist2(pt2 p) {
  return p.x*p.x+p.y*p.y*3;
  }

bool fppordering(const pt2& P1, const pt2& P2) {
  int d1 = hexdist(P1);
  int d2 = hexdist(P2);
  if(d1 != d2) return d1 > d2;
  int h1 = hexdist2(P1);
  int h2 = hexdist2(P2);
  if(h1 != h2) return h1 > h2;
  return false;
  }

void genfpporder() {
  if(fpporder.size()) return;
  for(int x=-40; x<=40; x++) for(int y=-40; y<=40; y++) {
    pt2 p; p.x=x; p.y=y; 
    fpporder.push_back(p);
    }
  sort(fpporder.begin(), fpporder.end(), fppordering);
  }

void renderAffineTriangle(int pix, noteyecolor trans, fpoint4 orig, fpoint4 ox, fpoint4 oy) {
  if(orig.y <= 0 && orig.y+ox.y <= 0 && orig.y+oy.y <= 0)
    return;
    
  int ax = xscale(orig.x, orig.y);
  int ay = yscale(orig.z, orig.y);
  
  fpoint4 orih = orig + ox;
  
  int bx = xscale(orih.x, orih.y);
  int by = yscale(orih.z, orih.y);

  orih = orig + oy;

  int cx = xscale(orih.x, orih.y);
  int cy = yscale(orih.z, orih.y);
  
  int xmi = min3(ax,bx,cx);
  int xma = max3(ax,bx,cx);
  int ymi = min3(ay,by,cy);
  int yma = max3(ay,by,cy);
  
  if(cross(ax,ay,bx,by,cx,cy) < 0) swap(bx,cx), swap(by,cy);
  
  if(xmi < -100 || xma > 1000 || ymi < -100 || yma > 1000) return;
  
  for(int x=max(xmi,V.x0); x<=xma && x <= V.x1; x++)
  for(int y=max(ymi,V.y0); y<=yma && y <= V.y1; y++)
    if(cross(ax,ay,bx,by,x,y) >= 0)
    if(cross(bx,by,cx,cy,x,y) >= 0)
    if(cross(cx,cy,ax,ay,x,y) >= 0) {
      if(trans == transAlpha) 
        alphablend(qpixel(viewimg, x, y), pix);
      else 
        qpixel(viewimg, x, y) = pix;
      }
  }

void renderAffinePixel(int pix, int trans, fpoint4 orig, fpoint4 ox, fpoint4 oy) {
  if(istrans(pix, trans)) return;
  renderAffineTriangle(pix,trans,orig,ox,oy);
  renderAffineTriangle(pix,trans,orig+ox+oy,-ox,-oy);
  }

int debugid = 0;

void renderAffineImage(TileImage *w, fpoint4 orig, fpoint4 ox, fpoint4 oy) {

  if(V.side) {
    double d = det(orig, ox, oy);
    if(viewglwindow) d = -d;
    if(V.side == 1 && d>=0) return;
    if(V.side == 2 && d<=0) return;
    }

#ifdef OPENGL  
  if(viewglwindow) {
    renderAffineImageGL(viewglwindow, w, orig, ox, oy);
    return;
    }
#endif
  
  if(V.shiftdown) orig = addShift(orig, oy, w);

  if(orig.y <= 0 && orig.y+ox.y <= 0 && orig.y+oy.y <= 0 && orig.y+ox.y+oy.y <= 0)
    return;

  viewimage->setLock(true);
  w->i->setLock(true);
  
  double dx = 1./w->sx;
  double dy = 1./w->sy;
  
  ox = ox * dx;
  oy = oy * dy;
  
  for(int ax=0; ax<w->sx; ax++) for(int ay=0; ay<w->sy; ay++)
    renderAffinePixel(
      qpixel(w->i->s, w->ox+ax, w->oy+ay),
      w->trans,
      orig+ox*ax+oy*ay,
      ox,oy
      );
  }

void renderAffine(Tile *ch, fpoint4 orig, fpoint4 dx, fpoint4 dy, fpoint4 dz) {
  if(ch == NULL) return;

  Get(TileImage, TI, ch);
  if(TI) renderAffineImage(TI, orig, dx, dy);
  
  Get(TileFill, TF, ch);
  if(TF) renderAffineImage(getFillCache(TF), orig, dx, dy);

  Get(TileMerge, TM, ch);
  if(TM)
    renderAffine(TM->t1, orig, dx, dy, dz),
    renderAffine(TM->t2, orig, dx, dy, dz);

  bool b = V.shiftdown;
  
  Get(TileFreeform, TFF, ch);
  if(TFF) {
    fpoint4 norig = 
      orig * TFF->par->d[0][0] + 
      dx   * TFF->par->d[0][1] + 
      dy   * TFF->par->d[0][2] + 
      dz   * TFF->par->d[0][3];
    fpoint4 ndx = 
      orig * TFF->par->d[1][0] + 
      dx   * TFF->par->d[1][1] + 
      dy   * TFF->par->d[1][2] + 
      dz   * TFF->par->d[1][3];
    fpoint4 ndy = 
      orig * TFF->par->d[2][0] + 
      dx   * TFF->par->d[2][1] + 
      dy   * TFF->par->d[2][2] + 
      dz   * TFF->par->d[2][3];
    fpoint4 ndz = 
      orig * TFF->par->d[3][0] + 
      dx   * TFF->par->d[3][1] + 
      dy   * TFF->par->d[3][2] + 
      dz   * TFF->par->d[3][3];
    
    int s = V.side;
    if(TFF->par->side < 3) V.side = TFF->par->side;
    else if(TFF->par->side == 3 && V.side) V.side = 3-V.side;
    
    V.shiftdown = TFF->par->shiftdown;
    renderAffine(TFF->t1, norig, ndx, ndy, ndz);
    V.shiftdown = b;
    V.side = s;
    }

  Get(TileTransform, TTF, ch);
  if(TTF) { 
    fpoint4 norig = orig + dx * (TTF->dx) + dy * (TTF->dy) + dz * (TTF->dz);
    double C = TTF->sx * +cos(TTF->rot*M_PI/180);
    double S = TTF->sx * -sin(TTF->rot*M_PI/180);
    fpoint4 ndx   = dx * C - dz * S;
    fpoint4 ndy   = dy * (TTF->sy);
    fpoint4 ndz   = dx * S + dz * C;
    V.shiftdown = false;
    renderAffine(TTF->t1, norig, ndx, ndy, ndz);
    V.shiftdown = b;
    }
  }

void imagepixel(noteyecolor &pix, TileImage *w, int x, int y) {
  int lpix = qpixel(w->i->s, w->ox + ((x * w->sx) >> 8), w->oy + ((y * w->sy) >> 8));
  if(w->trans == transAlpha) alphablend(pix, lpix);
  else {
    if(istrans(lpix, w->trans)) return;
    pix = lpix;
    }
  }

double eps = 1e-3;

void renderTileImage(TileImage *w, cd p0, cd p1, double z0, double z1) {
  
  if(imag(p0) <= eps && imag(p1) <= eps) return;
  
  if(V.side) {
    double d = det(
      fpoint4(real(p0), imag(p0), z0), 
      fpoint4(real(p1-p0), imag(p1-p0), 0), 
      fpoint4(0,0,z1-z0)
      );
    if(viewglwindow) d = -d;
    if(V.side == 1 && d>=0) return;
    if(V.side == 2 && d<=0) return;
    }

  /* fprintf(debug, "(%lf,%lf) - (%lf,%lf) [%d:%d]\n",
    real(p0), imag(p0), real(p1), imag(p1), z0, z1);
  
  fflush(debug); */
    
  for(int xt=0; xt<=w->sx; xt++) {
    p[xt] = p0 + (p1-p0) * double(xt / (w->sx + .0));
    }

  viewimage->setLock(true);
  w->i->setLock(true);
  
  int ay = V.shiftdown ? getFppDown(w) : 0;

  for(int xt=0; xt<w->sx; xt++) {
    if(imag(p[xt]) <= eps || imag(p[xt+1]) <= eps) continue;
                              
    int sx0 = xscale(real(p[xt]), imag(p[xt]));
    int sx1 = xscale(real(p[xt+1]), imag(p[xt+1]));
    
    int syt0 = yscale(z0, imag(p[xt]));
    int syb0 = yscale(z1, imag(p[xt]));
    int syt1 = yscale(z0, imag(p[xt+1]));
    int syb1 = yscale(z1, imag(p[xt+1]));
    
    /* fprintf(debug, "t%-2d (%3d,%3d) - (%3d,%3d) - (%3d,%3d) - (%3d,%3d)\n",
      xt,
      sx0, syt0, sx1, syt1, sx1, syb1, sx0, syb0
      ); */
    
    int dx0 = sx0;
    int dx1 = sx1;
    
    // bool b = dx0>dx1;
    
    if(dx0 > dx1) swap(dx0, dx1);
    if(dx0 < V.x0) dx0 = V.x0;
    if(dx1 > V.x1) dx1 = V.x1;
    
    for(int x=dx0; x<dx1; x++) {
      int dyt = syt0 + (syt1-syt0) * (x-sx0) / (sx1-sx0);
      int dyb = syb0 + (syb1-syb0) * (x-sx0) / (sx1-sx0);
      
      int y0 = dyt;
      int y1 = dyb;
      if(y0 > y1) swap(y0, y1);
      if(y0 < V.y0) y0 = V.y0;
      if(y1 > V.y1) y1 = V.y1;
      
      for(int y=y0; y<y1; y++) {
        int yt = ((y-dyt) * w->sy) / (dyb-dyt);
        if(yt<ay) continue;
        yt += w->sy - 1 - w->sy + 1;
        if(yt < 0) yt = 0;
        if(yt >= w->sy) yt = w->sy-1;
        int pix = qpixel(w->i->s, w->ox+xt, w->oy+yt-ay);
        if(w->trans == transAlpha) 
          alphablend(qpixel(viewimg, x, y), pix);
        else if(!istrans(pix, w->trans))
          qpixel(viewimg, x, y) = pix;
        }
      }
    }
  
  }

void renderChar0(Tile* ch, cd p0, cd p1, cd p2, double z0, double z1) {
  if(ch == NULL) return;

  Get(TileImage, TI, ch);
  if(TI) renderTileImage(TI, p0, p1, z0, z1);

  Get(TileFill, TF, ch);
  if(TF) renderTileImage(getFillCache(TF), p0, p1, z0, z1);

  Get(TileMerge, TM, ch);
  if(TM) 
    renderChar0(TM->t1, p0, p1, p2, z0, z1),
    renderChar0(TM->t2, p0, p1, p2, z0, z1);
  
  bool b = V.shiftdown;

  Get(TileTransform, TT, ch);
  if(TT) {
    cd shift(TT->dx, TT->dz);
    double scale = TT->sx;
    cd rot = cd(scale * cos(TT->rot*M_PI/180), scale * sin(TT->rot*M_PI/180));
    cd np0 = p0 + (p1-p0) * shift;
    cd np1 = np0 + (p1-p0) * rot;

    double nz0 = z0 + (z1-z0) * (TT->dy);
    double nz1 = z0 + (z1-z0) * (TT->dy) + (z1-z0) * (TT->sy);
    V.shiftdown = false;    
    renderChar0(TT->t1, np0, np1, p2, nz0, nz1);
    V.shiftdown = b;
    }

  Get(TileFreeform, TFF, ch);
  if(TFF) {
    // use Affine instead
    fpoint4 orig = fpoint4(real(p0), imag(p0), z0);
    fpoint4 ox = fpoint4(real(p1-p0), imag(p1-p0), 0);
    fpoint4 oy = fpoint4(0, 0, z1-z0);
    fpoint4 oz = fpoint4(real(p2), imag(p2), 0);
    V.shiftdown = false;
    renderAffine(ch, orig, ox, oy, oz);
    V.shiftdown = b;
    }
  }

void renderChar(Tile *ch, cd p0, cd p1, cd p2, double z0, double z1) {
  z0 -= V.delta.z;
  z1 -= V.delta.z;
  p0 = p0 + cd(-V.delta.x, -V.delta.y);
  p1 = p1 + cd(-V.delta.x, -V.delta.y);
  renderChar0(ch, p0, p1, p2, z0, z1);
  }

void cellpixel(noteyecolor &pix, Tile *ch, int x, int y) {
  if(ch == NULL) return;

  Get(TileImage, TI, ch);
  if(TI) { imagepixel(pix, TI, x, y); return; }

  Get(TileMerge, TM, ch);
  if(TM) {
    cellpixel(pix, TM->t1, x, y),
    cellpixel(pix, TM->t2, x, y);
    return;
    }

  Get(TileFill, TF, ch);
  if(TF) pix = TF->color;
  }

void drawFPP(double x, double y, Tile *c) {

  if(!c) return;
  
  fpoint4 orig = rot4(x*32-16, -y*32+16, +16.) - V.delta;
  fpoint4 ox = rot4(32, 0, 0);
  fpoint4 oy = rot4(0, -32, 0);
  fpoint4 oz = rot4(0, 0, -32);
  renderAffine(tmFree->apply(c), orig, ox, oy, oz);

  V.shiftdown = false;
  // these rear walls are actually drawn only if affected with freeform (assuming side==1)
  if(y<=0) renderChar(tmWallN->apply(c), rot(x*32+16, -y*32+16), rot(x*32-16, -y*32+16), rot(0,-32),  -16, +16);
  if(x<=0) renderChar(tmWallW->apply(c), rot(x*32-16, -y*32+16), rot(x*32-16, -y*32-16), rot(32,0),  -16, +16);
  if(y>=0) renderChar(tmWallS->apply(c), rot(x*32-16, -y*32-16), rot(x*32+16, -y*32-16), rot(0,32), -16, +16);
  if(x>=0) renderChar(tmWallE->apply(c), rot(x*32+16, -y*32-16), rot(x*32+16, -y*32+16), rot(-32,0), -16, +16);
  
  // 12 advance chars  
  V.shiftdown = true;
  cd pos = rot(x*32, -y*32);
  renderChar(tmCenter->apply(c), pos + cd(-V.ctrsize, 0), pos + cd(V.ctrsize, 0), cd(0, V.ctrsize), -16, +16);
  renderChar(tmMonst->apply(c), pos + cd(-V.monsize, -V.monpush), pos + cd(V.monsize,-V.monpush), cd(0, V.monsize), -16, +16);
  renderChar(tmItem->apply(c), pos + cd(-V.objsize, -V.objpush), pos + cd(+V.objsize,-V.objpush), cd(0, V.objsize), -2, +16);
  
  // front walls
  V.shiftdown = false;
  if(y>0) renderChar(tmWallN->apply(c), rot(x*32+16, -y*32+16), rot(x*32-16, -y*32+16), rot(0,-32),  -16, +16);
  if(x>0) renderChar(tmWallW->apply(c), rot(x*32-16, -y*32+16), rot(x*32-16, -y*32-16), rot(32,0),  -16, +16);
  if(y<0) renderChar(tmWallS->apply(c), rot(x*32-16, -y*32-16), rot(x*32+16, -y*32-16), rot(0,32), -16, +16);
  if(x<0) renderChar(tmWallE->apply(c), rot(x*32+16, -y*32-16), rot(x*32+16, -y*32+16), rot(-32,0), -16, +16);
  }

void drawFPPat(double wax, double way, double facedir, Screen *s) {
  
  static Screen sfloor, sceil;
  sfloor.setSize(s->sx, s->sy);
  sceil. setSize(s->sx, s->sy);

  for(int x=0; x<s->sx; x++)
  for(int y=0; y<s->sy; y++) {
    sfloor.get(x,y) = tmFloor->apply(s->get(x,y));
    sceil .get(x,y) = tmCeil->apply(s->get(x,y));
    }
  
  facevec = exp(cd(0, (facedir-90) * M_PI/180));
  facevec = cd(real(facevec) * V.yz, imag(facevec) * V.xz);
  facevec /= abs(facevec);  
  facevecc = conj(facevec);
  
  double sf0 = 8.0 * (16 - V.delta.z);
  double sf1 = -8.0 * (16 + V.delta.z);
  cd mxy = cd(128, 128) + facevecc * cd(V.delta.x, -V.delta.y) * cd(8, 0) + cd(wax*256, way*256);
  
  for(int y=V.y0; y<V.y1; y++)
  for(int x=V.x0; x<V.x1; x++) {

    noteyecolor& pix(qpixel(viewimg,x,y));
        
    int ux, uy;

    double sf;
    if(y > V.ym) {
      sf = sf0 / (y-V.ym);
      }
    else if(y < V.ym) {
      sf = sf1 / (y-V.ym);
      }
    else continue;

    cd u((V.xm-x) * sf, V.ys * sf);
    // u *= cd(0, 1);
    // cd u(-scal*sf, (x-V.xm) * sf);
      
    u *= -facevecc;
    ux = int(floor(real(mxy) + real(u) / V.xz));
    uy = int(floor(imag(mxy) + imag(u) / V.yz));

    int ux0 = ux>>8, uy0 = uy>>8;

#ifdef NDISTILL
    //int flag = y > V.ym ? spFloor : spCeil;    
    int C=(s->get(ux0,uy0));
#else
    //int flag=-1;
    Tile *C=(y > V.ym ? sfloor : sceil).get(ux0,uy0);
    #endif
      
    ux &= 255;
    uy &= 255;
    
    cellpixel(pix, C, ux, uy);
    }

  int iwax = int(wax + .5);
  int iway = int(way + .5);
  
  for(int i=0; i<(int) fpporder.size(); i++) {
    int X = fpporder[i].x;
    int Y = fpporder[i].y;
    Tile *g = s->get(iwax+X,iway+Y);
    if(g) drawFPP(X-(wax-iwax), Y-(way-iway), g);
    }
  }

// ph=0: walls and floors
// ph=1: other stuff
void drawFPP_GL(double xx, double yy, Tile *c, int ph) {
  if(!c) return;
  Tile *cc;
  
  fpoint4 orig = rotgl(32*xx-16, 16-yy*32, +16, 0, 0) + V.delta;
  fpoint4 ox = rotgl(32, 0, 0, 0, 0);
  fpoint4 oy = rotgl(0, -32, 0, 0, 0);
  fpoint4 oz = rotgl(0, 0, -32, 0, 0);

  if(ph == 0) {
    renderAffine(tmFloor->apply(c), orig, ox, oy, oz);

    orig = rotgl(32*xx-16, 16-yy*32, -16, 0, 0) + V.delta;
  
    if(V.side == 1 || V.side == 2) V.side = 3-V.side;
    renderAffine(tmCeil->apply(c), orig, ox, oy, oz);
    if(V.side == 1 || V.side == 2) V.side = 3-V.side;
    return;
    }

  renderAffine(tmFree->apply(c), orig, ox, oy, oz);

  V.shiftdown = false;
  
  Tile *cw = tmAllWall->apply(c);

  if(yy <= 0 && cw && (cc = tmWallN->apply(c))) {
    orig = rotgl(16+32*xx, +16-yy*32, -16, 0, 0) + V.delta;
    ox = rotgl(-32, 0, 0, 0, 0);
    oy = rotgl(0, 0, 32, 0, 0);
    oz = rotgl(0, -32, 0, 0, 0);
    renderAffine(cc, orig, ox, oy, oz);
    }

  if(xx <= 0 && cw && (cc = tmWallW->apply(c))) {
    orig = rotgl(-16+32*xx, +16-yy*32, -16, 0, 0) + V.delta;
    ox = rotgl(0, -32, 0, 0, 0);
    oy = rotgl(0, 0, 32, 0, 0);
    oz = rotgl(32, 0, 0, 0, 0);
    renderAffine(cc, orig, ox, oy, oz);
    }

  if(yy >= 0 && cw && (cc = tmWallS->apply(c))) {
    orig = rotgl(-16+32*xx, -16-yy*32, -16, 0, 0) + V.delta;
    ox = rotgl(32, 0, 0, 0, 0);
    oy = rotgl(0, 0, 32, 0, 0);
    oz = rotgl(0, 32, 0, 0, 0);
    renderAffine(cc, orig, ox, oy, oz);
    }

  if(xx >= 0 && cw && (cc = tmWallE->apply(c))) {
    orig = rotgl(16+32*xx, -16-yy*32, -16, 0, 0) + V.delta;
    ox = rotgl(0, 32, 0, 0, 0);
    oy = rotgl(0, 0, 32, 0, 0);
    oz = rotgl(-32, 0, 0, 0, 0);
    renderAffine(cc, orig, ox, oy, oz);
    }
  
  V.shiftdown = true;
  
  Tile *ci = tmCMI->apply(c);
  
  if(ci) {

    if((cc = tmCenter->apply(c))) {
      orig = rotgl(32*xx, -yy*32, -16, -16, 0) + V.delta;
      ox = rotgl(32, 0, 0, 16, 0);
      oy = rotgl(0, 0, 32, 0, 0);
      oz = rotgl(0, 0, 0, 0, 16);
      renderAffine(cc, orig, ox, oy, oz);
      }
  
    if((cc = tmMonst->apply(c))) {
      orig = rotgl(32*xx, -yy*32, -16, -16, V.monpush) + V.delta;
      ox = rotgl(0, 0, 0, 32, 0);
      oy = rotgl(0, 0, 32, 0, 0);
      oz = rotgl(0, 0, 0, 0, 32);
      renderAffine(cc, orig, ox, oy, oz);
      }
    
    if((cc = tmItem->apply(c))) {
      orig = rotgl(32*xx, -yy*32, 0, -8, V.objpush) + V.delta;
      ox = rotgl(0, 0, 0, 16, 0);
      oy = rotgl(0, 0, 16, 0, 0);
      oz = rotgl(0, 0, 0, 0, 16);
      renderAffine(cc, orig, ox, oy, oz);
      }
    }
  
  V.shiftdown = false;
  
  if(yy > 0 && cw && (cc = tmWallN->apply(c))) {
    orig = rotgl(16+32*xx, +16-32*yy, -16, 0, 0) + V.delta;
    ox = rotgl(-32, 0, 0, 0, 0);
    oy = rotgl(0, 0, 32, 0, 0);
    oz = rotgl(0, -32, 0, 0, 0);
    renderAffine(cc, orig, ox, oy, oz);
    }

  if(xx > 0 && cw && (cc = tmWallW->apply(c))) {
    orig = rotgl(-16+32*xx, +16-yy*32, -16, 0, 0) + V.delta;
    ox = rotgl(0, -32, 0, 0, 0);
    oy = rotgl(0, 0, 32, 0, 0);
    oz = rotgl(32, 0, 0, 0, 0);
    renderAffine(cc, orig, ox, oy, oz);
    }

  if(yy < 0 && cw && (cc = tmWallS->apply(c))) {
    orig = rotgl(-16+32*xx, -16-yy*32, -16, 0, 0) + V.delta;
    ox = rotgl(32, 0, 0, 0, 0);
    oy = rotgl(0, 0, 32, 0, 0);
    oz = rotgl(0, 32, 0, 0, 0);
    renderAffine(cc, orig, ox, oy, oz);
    }

  if(xx < 0 && cw && (cc = tmWallE->apply(c))) {
    orig = rotgl(16+32*xx, -16-yy*32, -16, 0, 0) + V.delta;
    ox = rotgl(0, 32, 0, 0, 0);
    oy = rotgl(0, 0, 32, 0, 0);
    oz = rotgl(-32, 0, 0, 0, 0);
    renderAffine(cc, orig, ox, oy, oz);
    }
  
  if(V.delta.y <= -16 && cw && (cc = tmWallTop->apply(c))) {
    fpoint4 orig = rotgl(32*xx-16, 16-yy*32, -16, 0, 0) + V.delta;
    fpoint4 ox = rotgl(32, 0, 0, 0, 0);
    fpoint4 oy = rotgl(0, -32, 0, 0, 0);
    fpoint4 oz = rotgl(0, 0, -32, 0, 0);

    renderAffine(cc, orig, ox, oy, oz);
    }

  if(V.delta.y >= 16 && cw && (cc = tmWallBot->apply(c))) {
    fpoint4 orig = rotgl(32*xx-16, 16-yy*32, +16, 0, 0) + V.delta;
    fpoint4 ox = rotgl(32, 0, 0, 0, 0);
    fpoint4 oy = rotgl(0, -32, 0, 0, 0);
    fpoint4 oz = rotgl(0, 0, -32, 0, 0);

    if(V.side == 1 || V.side == 2) V.side = 3-V.side;
    renderAffine(cc, orig, ox, oy, oz);
    if(V.side == 1 || V.side == 2) V.side = 3-V.side;
    }

  }

void drawFPP_GL(double wax, double way, double facedir, Screen *s) {

  facevec = exp(cd(0, (facedir-90) * M_PI/180));
  facevec = cd(real(facevec) * V.yz, imag(facevec) * V.xz);
  facevec /= abs(facevec);
  
  facevecc = conj(facevec);

  V.delta.x = -V.delta.x;
  swap(V.delta.y, V.delta.z);
  
  int iwax = int(wax + .5);
  int iway = int(way + .5);
  
  for(int ph=0; ph<2; ph++) for(int i=0; i<(int) fpporder.size(); i++) {
    int X = fpporder[i].x;
    int Y = fpporder[i].y;
    Tile *g = s->get(iwax+X,iway+Y);
    if(g) drawFPP_GL(X-(wax-iwax), Y-(way-iway), g, ph);
    }
  }


#ifdef USELUA
extern "C" {
void fpp(viewpar vp, double wax, double way, double facedir, Screen *s) {

  ASSERT_TYPE(s, Screen, );
  
  genfpporder();
  
  V = vp;

  viewimage = vp.vimg;
  
  viewglwindow = useGL(viewimage);
  viewsdlwindow = useSDL(viewimage);
    
#ifdef OPENGL  
  if(viewglwindow) {
    initFPPGL(viewglwindow);
    drawFPP_GL(wax, way, facedir, s);
    initOrthoGL(viewglwindow);
    //glDisable( GL_DEPTH_TEST );
    return;
    }
#endif

  else if(viewsdlwindow) {
    int x0, y0;
    initFPPSDL(viewsdlwindow, x0, y0);
    viewimg = viewimage->s;
    drawFPPat(wax, way, facedir, s);
    drawFPPSDL(viewsdlwindow, x0, y0);
    }
  
  else {
    viewimg = viewimage->s;
    viewimage->setLock(true);
    drawFPPat(wax, way, facedir, s);
    }
  
  return;
  }
#endif

#ifdef OPENGL
static fpoint4 addShift(fpoint4 o, fpoint4 y, TileImage *w) {
  return o + y * (getFppDown(w) * 1. / w->sy);
  }
#endif
}

}
