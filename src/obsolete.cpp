/*
int lh_tileMerge(lua_State *L) {
  checkArg(L, 2, "tilemerge");
  return noteye_retInt(L, addMerge(luaInt(1), luaInt(2), false));
  }

int lh_tileMergeOver(lua_State *L) {
  checkArg(L, 2, "tilemergeover");
  return noteye_retInt(L, addMerge(luaInt(1), luaInt(2), true));
  }

int lh_addTile(lua_State *L) {
  checkArg(L, 6, "addtile");
  return noteye_retInt(L, addTile(luaO(1, Image), luaInt(2), luaInt(3), 
    luaInt(4), luaInt(5),
    luaInt(6)
    ));
  }

int lh_gchv(lua_State *L) {
  return noteye_retInt(L, getChar(luaInt(1)));
  }

int lh_gco(lua_State *L) {
  return noteye_retInt(L, getCol(luaInt(1)));
  }

int lh_gimg(lua_State *L) {
  return noteye_retInt(L, getImage(luaInt(1)));
  }

int lh_gba(lua_State *L) {
  return noteye_retInt(L, getBak(luaInt(1)));
  }
*/

/* int lh_gch(lua_State *L) {
  int i = getChar(luaInt(1));
  char c = i == -1 ? 0 : i;
  lua_pushlstring(L, &c, 1);
  return 1;
  } */

/*
int lh_setpixel(lua_State *L) {
  checkArg(L, 4, "setpixel");
  Image *srcI = luaO(1, Image);
  img_setpixel2(srcI, luaInt(2), luaInt(3), luaInt(4));
  return 0;
  }

int lh_getpixel(lua_State *L) {
  checkArg(L, 3, "getpixel");
  Image *srcI = luaO(1, Image);
  int res = img_getpixel2(srcI, luaInt(2), luaInt(3));
  return noteye_retInt(L, res);
  }
*/

inline void obsolete() {
//noteye_globalfun("tilemerge", lh_tileMerge);
//noteye_globalfun("tilemergeover", lh_tileMergeOver);
//noteye_globalfun("addtile", lh_addTile);
//noteye_globalfun("gchv", lh_gchv);
//noteye_globalfun("gimg", lh_gimg);
//noteye_globalfun("gba", lh_gba);
//noteye_globalfun("gco", lh_gco);
//noteye_globalfun("gch", lh_gch);
//noteye_globalfun("getpixel", lh_getpixel);
//noteye_globalfun("setpixel", lh_setpixel);
  }

/*
int lh_scrget(lua_State *L) {
  checkArg(L, 3, "scrget");
  return noteye_retInt(L, luaO(1, Screen)->get(luaInt(2), luaInt(3)));
  }

int lh_scrset(lua_State *L) {
  checkArg(L, 4, "scrset");
  int& C ( luaO(1, Screen)->get(luaInt(2), luaInt(3)) );
  C = luaInt(4);
  return 0;
  }

  noteye_globalfun("scrget", lh_scrget);
  noteye_globalfun("scrset", lh_scrset);

*/

/*
int lh_bAND(lua_State *L) {
  return noteye_retInt(L, luaInt(1) & luaInt(2));
  }

int lh_bOR(lua_State *L) {
  return noteye_retInt(L, luaInt(1) | luaInt(2));
  }

int lh_bXOR(lua_State *L) {
  return noteye_retInt(L, luaInt(1) ^ luaInt(2));
  }
*/

/*
  noteye_globalfun("bAND", lh_bAND);
  noteye_globalfun("bOR", lh_bOR);
  noteye_globalfun("bXOR", lh_bXOR);
*/

