void killHydras() {
  while(!hydras.empty()) 
    M[hydras[0]->pos].hydraDead(NULL);
  for(int y=0; y<SY; y++) for(int x=0; x<SX; x++) if(M.m[y][x].type == CT_STAIRDOWN)
    playerpos = vec2(x,y);
  if(P.curHP < 1) P.curHP = 1;
  }

void corridor() {
  for(int y=0; y<SY; y++) for(int x=0; x<SX; x++) 
    if((y == 5 && x>5) || (x==5 && y >= 5))
      M.m[y][x].type = CT_EMPTY, M.m[y][x].mushrooms = 0;
    else if(x+y <= 20)
      M.m[y][x].type = CT_WALL, M.m[y][x].mushrooms = 0;
  playerpos = vec2(15, 5);
  }

void clearItems() {
  for(int y=0; y<SY; y++) for(int x=0; x<SX; x++) 
    M.m[y][x].it = NULL;
  for(int y=0; y<SY; y++) for(int x=0; x<SX; x++) 
    M.m[y][x].dead = 0;
  }
  
void central() {
  playerpos = vec2(SX/4 + hrand(SX/2), SY/4 + hrand(SY/2));
  }

int sceneid = 0;
string scenetext;

void place(hydra *h, vec2 v) {
  h->pos = v; M[v].h = h;
  hydras.push_back(h);
  }

int rmush() {
  if(hrand(5) == 0) return hrand(10);
  return 0;
  }

void placeAtDistance(hydra *h, int d) {
  vec2 dx(d, hrand(2*d+1) - d);
  if(hrand(2)) dx = vec2(-dx.x, dx.y);
  if(hrand(2)) dx = vec2(dx.y, dx.x);
  
  for(int u=0; u<=10; u++) {
    vec2 vh = playerpos + dx * u / 10;
    M[vh].type = CT_EMPTY;
    M[vh].mushrooms = 0;
    }
  
  place(h, playerpos + dx);
  }

void scene(string s0, string s1) { 
  msgs.clear();
  msgs.push_back(s0);  
  msgs.push_back(s1);
  }

void trailer() {

  hydra *h;
  
  sceneid++;
  
  P.race = R_HUMAN;
  
  switch(sceneid) {
    case 1: 
      killHydras();
      P.active[IT_PAMBI] = 0;
      P.curlevel = 10;
      P.arms = 2;
      P.race = R_CENTAUR;
      for(int i=0; i<10; i++) wpn[i] = NULL;
      generateLevel();
      killHydras();
      clearItems();
      central();
      for(int y=1; y<SY-1; y++)
      for(int x=1; x<SX-1; x++)
        M.m[y][x].type = CT_EMPTY,
        M.m[y][x].mushrooms = rmush();
      for(int y=playerpos.y-2; y<=playerpos.y+2; y++)
      for(int x=playerpos.x-5; x<=playerpos.x+5; x++)
        M.m[y][x].type = CT_EMPTY,
        M.m[y][x].mushrooms = 0;
      P.curHP = 4;
      P.maxHP = 7;
        
      place(new hydra(2, 5, 5, 20), playerpos + vec2(2,-1));
      place(new hydra(1, 7, 5, 20), playerpos + vec2(2,1));
      place(new hydra(7, 3, 10, 20), playerpos + vec2(3,0));
      place(new hydra(5, 24, 10, 20), playerpos + vec2(4,-1));
      place(new hydra(6, 12, 10, 20), playerpos + vec2(4,0));
      place(new hydra(HC_VAMPIRE, 36, 10, 20), playerpos + vec2(4,1));
      place(new hydra(HC_GROW, 99, 10, 20), playerpos + vec2(5,0));
      place(new hydra(HC_WIZARD, 27, 10, 20), playerpos + vec2(6,1));
      scene("", "Ancient monsters, the hydras, have returned.");
      break;
    
    case 2:
      P.race = R_HUMAN;
      killHydras();
      P.curlevel = 4;
      generateLevel();
      killHydras();
      clearItems();
      corridor();
      P.curHP = 40; P.maxHP = 40;
      wpn[0] = new weapon(dcolors[2], 10, WT_BLADE);
      place(new hydra(dcolors[2], 15, 1, 20), playerpos + vec2(3,0));
      scene("", "Many brave warriors tried to fight them.");
      break;
    
    case 3: 
      place(new hydra(dcolors[3], 1, 1, 20), vec2(5,10));
      scene("", "None of them returned.");
      break;
    
    case 4:
      killHydras();
      P.curlevel = 0; 
      wpn[0] = new weapon(dcolors[0], 1, WT_BLADE);
      wpn[1] = new weapon(dcolors[1], 2, WT_BLADE);
      generateLevel();
      killHydras();
      clearItems();
      P.curHP = 24; P.maxHP = 24;
      playerpos = vec2(SX/2, SY - 10);
      for(int y=0; y<SY; y++)
      for(int x=0; x<SX; x++)
        if(x == SX/2 && y == SY-3)
          M.m[y][x].type = CT_STAIRDOWN, M.m[y][x].mushrooms = 0;
        else if(x >= SX/2 - (SY-3-y) && x <= SX/2 + (SY-3-y)) {
          M.m[y][x].type = CT_EMPTY,
          M.m[y][x].mushrooms = rmush();
          }
        else
          M.m[y][x].type = CT_WALL, M.m[y][x].mushrooms = 0;
      scene("", "You are more clever.");
      break;
    
    case 5:
      killHydras();
      P.curlevel = 0; 
      generateLevel();
      killHydras();
      clearItems();
      P.curHP = 24; P.maxHP = 24;
      placeAtDistance(new hydra(dcolors[3], 1, 4, 20), 1);
      placeAtDistance(new hydra(dcolors[4], 2, 4, 20), 4);
      scene("", "You have learned the forgotten art of Hydra Slaying.");
      break;

    case 6:
      killHydras();
      P.curlevel = 3; P.curHP = 54; P.maxHP = 27;
      generateLevel();
      killHydras();
      clearItems();
      wpn[0] = new weapon(dcolors[0], 5, WT_BLADE);
      wpn[1] = new weapon(dcolors[1], 2, WT_BLADE);
      placeAtDistance(new hydra(dcolors[5], 5, 5, 20), 2);
      placeAtDistance(new hydra(dcolors[6], 2, 5, 20), 5);
      scene("A longsword cuts five heads in a single strike.", "No more, and no fewer.");
      break;
    
    case 7:
      killHydras();
      h = new hydra(dcolors[7], 11, 1, 20);
      h->res[dcolors[0]] = 2;
      placeAtDistance(h, 2);
      scene("", "If it does not kill a hydra, some heads will usually regrow.");
      break;
    
    case 8:
      P.inv[IT_RSTUN] = 2;
      P.inv[IT_RSTUN] = 1;
      P.inv[IT_RCANC] = 1;
      P.inv[IT_RGROW] = 5;
      P.inv[IT_RCONF] = 1;
      P.inv[IT_RFUNG] = 1;
      P.inv[IT_RNECR] = 2;
      P.inv[IT_SXMUT] = 4;
      P.inv[IT_SGROW] = 2;
      P.inv[IT_SPART] = 1;
      P.inv[IT_PSWIP] = 2;
      P.inv[IT_PCHRG] = 2;
      P.inv[IT_PFAST] = 1;
      P.inv[IT_PKNOW] = 3;
      P.inv[IT_PARMS] = 1;
      P.inv[IT_PSEED] = 4;
      P.inv[IT_PAMBI] = 1;
      P.inv[IT_PLIFE] = 1;
      P.inv[IT_RDEAD] = 1;
      placeAtDistance(new hydra(dcolors[7], 3, 6, 20), 2);
      scene("Use magical powders and potions.", "Sometimes making the hydra stronger is the only way to kill it.");
      break;

    case 9:
      P.curlevel = 11; 
      generateLevel();
      killHydras();
      clearItems(); P.curHP = 134; P.maxHP = 134;
      P.arms = 6;
      wpn[2] = new weapon(dcolors[4], 1, WT_TIME);
      wpn[3] = new weapon(dcolors[5], 10, WT_DIV);
      wpn[4] = new weapon(HC_OBSID, 1, WT_BLADE);
      wpn[5] = new weapon(13, 1, WT_SHLD);
      M[playerpos + vec2(1,0)].mushrooms = 9;
      M[playerpos + vec2(2,0)].mushrooms = 7;
      M[playerpos + vec2(3,0)].it = new weapon(dcolors[9], 2, WT_ROOT);
      scene("Discover many types of weapons, including artifacts", 
        "such as the Primeslayer or the Staff of the Mushroom Lord.");
      break;

    case 10:
      P.curlevel = 10; 
      generateLevel();
      P.curHP = 124; P.maxHP = 124;
      P.arms = 6;
      wpn[0] = new weapon(HC_OBSID, 1, WT_BLADE);
      wpn[1] = new weapon(9, 7, WT_SHLD);
      wpn[2] = new weapon(dcolors[8], 3, WT_DIV);
      wpn[3] = new weapon(dcolors[9], 12, WT_BLADE);
      wpn[4] = new weapon(12, 18, WT_BLUNT);
      wpn[5] = new weapon(dcolors[0], 7, WT_AXE);
      placeAtDistance(new hydra(dcolors[0], 127, 12, 20), 5);
      scene("", "Every hydra is a challenge.");
      break;

    case 11:
      P.race = R_NAGA;
      P.active[IT_PAMBI] = 1;
      P.arms = 6;
      placeAtDistance(new hydra(dcolors[4], 38, 12, 20), 2);
      scene("Play as a human, an impatient Titan,", "mighty Echidna, Centaur archer, or Twins.");
      break;

    case 12:
      scene("", "Welcome to Hydra Slayer.");
    
    default:
      sceneid = 0;
    }
      
  }
