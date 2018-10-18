// Necklace of the Eye v6.2
// roguelike frontend
// Copyright (C) 2010-2011 Zeno Rogue, see 'noteye.h' for details

namespace noteye {

struct IsoParam : Object {
  int floor, wall, icon, iconh;
  int sx, sy, flev;
  smartptr<FreeFormParam> Floor, WalL, WalR, Item, Ceil;
  void build();
  };

extern "C" {
IsoParam* isoparam(int floor, int wall, int icon, int iconh) { 
  
  IsoParam *P = new IsoParam;
  P->floor = floor;
  P->wall  = wall;
  P->icon  = icon;
  P->iconh = iconh;
  
  P->build();
  
  return registerObject(P);
  }

FreeFormParam *ffClear() {
  FreeFormParam *P = new FreeFormParam;
  for(int i=0; i<16; i++) P->d[i/4][i%4] = 0;
  P->d[0][0] = 1; 
  return registerObject(P);
  }
}

void IsoParam::build() {
  sx = max(icon, floor*2);
  flev = max(iconh, floor/2 + wall);
  sy = (floor/2) + flev;  

  Floor = ffClear();
  Floor->d[0][1] = .5; Floor->d[0][2] = (flev - floor/2.)/sy;
  Floor->d[1][1] = (floor*1./sx); Floor->d[1][2] = (floor/2.)/sy;
  Floor->d[2][1] = -(floor*1./sx); Floor->d[2][2] = (floor/2.)/sy;
  Floor->d[3][2] = -wall*1./sy;
  
  Item = ffClear();
  Item->d[0][1] = (sx/2-icon/2.)/sx;
  Item->d[0][2] = (flev - icon+.0)/sy;
  Item->d[1][1] = (icon*1./sx); 
  Item->d[2][2] = (iconh*1./sy);
  
  Ceil = ffClear();
  Ceil->d[0][1] = .5; Ceil->d[0][2] = (flev - floor/2. - wall)/sy;
  Ceil->d[1][1] = (floor*1./sx); Ceil->d[1][2] = (floor/2.)/sy;
  Ceil->d[2][1] = -(floor*1./sx); Ceil->d[2][2] = (floor/2.)/sy;
  Ceil->d[3][2] = wall*1./sy;
  
  WalL = ffClear();
  WalL->d[0][1] = (sx/2.-floor/1.)/sx;
  WalL->d[0][2] = (flev - wall +.0)/sy;
  WalL->d[1][1] = (floor/1.)/sx;
  WalL->d[1][2] = (floor/2.)/sy;
  WalL->d[2][2] = (wall+.0)/sy;
  WalL->d[3][1] = (floor/1.)/sx;
  WalL->d[3][2] = -(floor/2.)/sy;
  
  WalR = ffClear();
  WalR->d[0][1] = .5;
  WalR->d[0][2] = (flev - wall +.0+floor/2.+.2)/sy;
  WalR->d[1][1] = (floor/1.)/sx;
  WalR->d[1][2] = -(floor/2.)/sy;
  WalR->d[2][2] = (wall+.0)/sy;
  WalR->d[3][1] = -(floor/1.)/sx;
  WalR->d[3][2] = -(floor/2.)/sy;
  
  }

extern "C" {

Tile *getIso(Tile *ti, IsoParam *P) {
  Tile *t0;
  t0 = addFreeform(tmIFloor->apply(ti), P->Floor);
  t0 = addMerge(t0, addFreeform(addRecolor(tmIWallL->apply(ti), 0xFF808080, recMult), P->WalL), false);
  t0 = addMerge(t0, addFreeform(addRecolor(tmIWallR->apply(ti), 0xFFC0C0C0, recMult), P->WalR), false);
  t0 = addMerge(t0, addFreeform(distill(ti, spIWallL), P->WalL), false);
  t0 = addMerge(t0, addFreeform(distill(ti, spIWallR), P->WalR), false);
  t0 = addMerge(t0, addFreeform(tmICeil->apply(ti), P->Ceil), false);
  t0 = addMerge(t0, addFreeform(tmIItem->apply(ti), P->Item), false);
  
  return t0;
  }

void drawScreenIso(Image *dest, Screen *scr, IsoParam *P, double x0, double y0, double x1, double y1, int xc, int yc, int xm, int ym, int rx0, int ry0, int rx1, int ry1) {

  int dminx = (int) floor(x0 + y0) - 2;
  int dminy = (int) floor(y0 - x1) - 2;
  int dmaxx = (int) ceil(x1 + y1) + 2;
  int dmaxy = (int) ceil(y1 - x0) + 2;

  drawmatrix M;
  M.tx = P->sx; M.ty = P->sy;
  M.txy = M.tyx = M.tzx = M.tzy = 0;
  
  rx0 -= P->sx; ry0 -= P->sy;
  rx0 -= P->floor; ry0 -= P->floor;
  rx1 += P->floor; ry1 += P->floor;

  for(int dx=dminx; dx<=dmaxx; dx++)
  for(int dy=dminy; dy<=dmaxy; dy++) {
    M.x = xc + P->floor * (dx-dy);
    M.y = yc + P->floor * (dx+dy)/2;
  
    if(M.x >= rx0 && M.x <= rx1 &&
       M.y >= ry0 && M.y <= ry1) {
      Tile *sg = scr->get(xm+dx, ym+dy);
      Tile *ip = getIso(sg, P);
      drawTile(dest, M, ip);
      }
    }
  }
}

}
