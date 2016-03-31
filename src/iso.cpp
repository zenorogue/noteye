// Necklace of the Eye v6.2
// roguelike frontend
// Copyright (C) 2010-2011 Zeno Rogue, see 'noteye.h' for details

namespace noteye {

struct IsoParam : Object {
  int floor, wall, icon, iconh;
  int sx, sy, flev;
  FreeFormParam *Floor, *WalL, *WalR, *Item, *Ceil;
  void build();
  ~IsoParam();
  };

#ifdef USELUA
int lh_isoparam(lua_State *L) {
  checkArg(L, 4, "isoparam");
  
  IsoParam *P = new IsoParam;
  P->floor = luaInt(1);
  P->wall  = luaInt(2);
  P->icon  = luaInt(3);
  P->iconh = luaInt(4);
  
  P->build();
  
  return noteye_retObject(L, P);
  }

FreeFormParam *ffClear() {
  FreeFormParam *P = new FreeFormParam;
  for(int i=0; i<16; i++) P->d[i/4][i%4] = 0;
  P->d[0][0] = 1; 
  return P;
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

IsoParam::~IsoParam() {
  delete Floor;
  delete WalL;
  delete WalR;
  delete Item;
  delete Ceil;
  }

int lh_isosizes(lua_State *L) {
  checkArg(L, 1, "isosizes");
  IsoParam *P = luaO(1, IsoParam);
  lua_newtable(L);
  noteye_table_setInt(L, "floor", P->floor);
  noteye_table_setInt(L, "wall", P->wall);
  noteye_table_setInt(L, "icon", P->icon);
  noteye_table_setInt(L, "iconh", P->iconh);
  noteye_table_setInt(L, "x", P->sx);
  noteye_table_setInt(L, "y", P->sy);
  return 1;
  }

int getIso(lua_State *L, int ti, IsoParam *P) {
  int t0;
  t0 = addFreeform(tmIFloor->apply(ti), P->Floor);
  t0 = addMerge(t0, addFreeform(addRecolor(tmIWallL->apply(ti), 0xFF808080, recMult), P->WalL), false);
  t0 = addMerge(t0, addFreeform(addRecolor(tmIWallR->apply(ti), 0xFFC0C0C0, recMult), P->WalR), false);
  t0 = addMerge(t0, addFreeform(distill(ti, spIWallL), P->WalL), false);
  t0 = addMerge(t0, addFreeform(distill(ti, spIWallR), P->WalR), false);
  t0 = addMerge(t0, addFreeform(tmICeil->apply(ti), P->Ceil), false);
  t0 = addMerge(t0, addFreeform(tmIItem->apply(ti), P->Item), false);
  
  return t0;
  // return noteye_retInt(L, renderAsIso(luaInt(1), spIFloor | spIItem | spIWallL | spIWallR | spICeil, luaO(2, IsoParam)));
  }

int lh_iso(lua_State *L) {
  checkArg(L, 2, "isoproject");
  IsoParam *P = luaO(2, IsoParam);
  int ti = luaInt(1);
  return getIso(L, ti, P);
  }

/*
  local dminx = math.floor(rect.top.x + rect.top.y) - 2
  local dminy = math.floor(rect.top.y - rect.bot.x) - 2
  local dmaxx = math.ceil(rect.bot.x + rect.bot.y) + 2
  local dmaxy = math.ceil(rect.bot.y - rect.top.x) + 2
  
  print("draw "..getticks())
  for dx=dminx,dmaxx do
  for dy=dminy,dmaxy do
    local dr = V(tcs.x+isi.floor*(dx-dy), tcs.y+isi.floor*(dx+dy)/2)
    if dr > D.maparea.top - isi and dr < D.maparea.bot then
      local sg = scrget(Tiles, mcf.x+dx, mcf.y+dy)
      local ip = isoproject(sg, IsoParam)
      V.drawtile(D.output, ip, rectTS(dr, isi))
      end
    end end

  print("done "..getticks())
*/

int lh_drawScreenIso(lua_State *L) {
  checkArg(L, 15, "drawscreenIso");
  Screen *scr = luaO(2, Screen);
  Image *dest = luaO(1, Image);
  IsoParam *P = luaO(3, IsoParam);
  double x0 = luaNum(4);
  double y0 = luaNum(5);
  double x1 = luaNum(6);
  double y1 = luaNum(7);
  int xc = luaInt(8);
  int yc = luaInt(9);
  int xm = luaInt(10);
  int ym = luaInt(11);
  int rx0 = luaInt(12);
  int ry0 = luaInt(13);
  int rx1 = luaInt(14);
  int ry1 = luaInt(15);

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
      int sg = scr->get(xm+dx, ym+dy);
      int ip = getIso(L, sg, P);
      drawTile(dest, M, ip);
      }
    }
  
  return 1;
  }

#endif

}
