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

int lh_iso(lua_State *L) {
  checkArg(L, 2, "isoproject");
  IsoParam *P = luaO(2, IsoParam);
  
  int t0;
  int ti = luaInt(1);
  t0 = addFreeform(tmIFloor->apply(ti), P->Floor);
  t0 = addMerge(t0, addFreeform(addRecolor(tmIWallL->apply(ti), 0xFF808080, recMult), P->WalL), false);
  t0 = addMerge(t0, addFreeform(addRecolor(tmIWallR->apply(ti), 0xFFC0C0C0, recMult), P->WalR), false);
//t0 = addMerge(t0, addFreeform(distill(ti, spIWallL), P->WalL), false);
//t0 = addMerge(t0, addFreeform(distill(ti, spIWallR), P->WalR), false);
  t0 = addMerge(t0, addFreeform(tmICeil->apply(ti), P->Ceil), false);
  t0 = addMerge(t0, addFreeform(tmIItem->apply(ti), P->Item), false);
  
  return noteye_retInt(L, t0);
  // return noteye_retInt(L, renderAsIso(luaInt(1), spIFloor | spIItem | spIWallL | spIWallR | spICeil, luaO(2, IsoParam)));
  }
#endif

}
