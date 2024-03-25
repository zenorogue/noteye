// Hydra Slayer: math puzzle roguelike
// Copyright (C) 2010-2011 Zeno Rogue, see 'hydra.cpp' for details

int weapon::cutoff(int heads, bool mode) {
  if(cuts() && heads >= size) return size;

  if(type == WT_DIV || type == WT_SUBD) {
    if(size <= 1) return -1;
    if(type == WT_SUBD && heads) heads--;
    if(!(heads % size)) return heads - (heads/size);
    if(mode) return heads - (heads/size) - 1;
    }
  
  if(type == WT_QUI) {
    if(size <= 1) return -1;
    if(heads == 1) return 1;
    if(heads % size != 1) return (heads+(size-1))/size;
    if(mode) return (heads/size);
    }
  
  if(type == WT_GOLD) {
    if(heads == 0) return 0; // silver should not return -1 for stunned
    int fib[60];
    fib[0] = 0; fib[1] = 1; int fc = 2;
    for(; fib[fc-1] <= heads; fc++)
      fib[fc] = fib[fc-2] + fib[fc-1];
    fc--;
    bool silv = color != 9;
    int ret = 0, sh = silv?1:2;
    
    while(fc > 2) { 
      if(heads >= fib[fc]) {
        heads -= fib[fc], ret += fib[fc-sh];
        }
      fc--;
      }
    if((silv ? !heads : heads) && !mode) return -1;
    if(silv) ret++;
    return ret;
    }

  if(type == WT_PSLAY) {
    if(mode || (primediv(heads) == -1 && heads>1)) {
      int idx = getPrimeIndex(heads);
      if(idx == -1) return -1;
      return heads-idx;
      }
    }
  if(type == WT_ROOT) {
    if(size < 1) return -1;
    if(size == 1) return 0;
    for(int c=0; c<=heads; c++) {
      int d = 1;
      for(int s=0; s<size; s++) { d *= c; if(d > heads) d = heads+1; }
      if(d > heads && !mode) return -1;
      if(d >= heads) return heads-c;
      }
    }                         
  if(type == WT_LOG) {
    if(size < 2) return -1;
    if(heads == 0) return 0; // no stunned heads remain no stunned heads
    int c = 1, pow = 0;
    while(c < heads) c *= size, pow++;
    if(c > heads && !mode) return -1;
    return heads-pow;
    }
  return -1;
  }

void vorpalClear() {
  vorplast = NULL;
  vorpalhsf = 0;
  }

int vorpalHP(int x) {
  return int(log(1+x) * log(1+x) * 3);
  }
  
void vorpalCredit() {
  int vc0 = vorpalHP(P.vorpalc);
  P.vorpalc += vorpalhsf;
  stats.vorpalhead += vorpalhsf;
  int vc1 = vorpalHP(P.vorpalc);
  if(vc1 > vc0) {
    int hp = vc1 - vc0;
    string msg;
    if(hp < 10)
      msg = "And through and through!";
    else if(hp < 100)
      msg = "The vorpal blade went snicker-snack!";
    else {
      msg = "O frabjous day! You chortle in your joy.";
      achievement("BEAMISHBOY");
      }
    if(hp >= 10) shareS("cut", " "+its(hp)+" heads with the vorpal blade");
    addMessage(msg + " ("+its(vorpalhsf)+" heads = "+its(hp)+" HP)");
    removeWounds(hp);
    stats.vorpalhp += hp;
    }
  vorpalClear();
  }

void vorpalRegenerate() {
  int l = P.curlevel+1; if(l>99) l = 99;
  P.vorpalc = (P.vorpalc * l) / 100;
  
  int l2 = P.curlevel+1; if(l>399) l = 399;
  P.timemushlimit = (P.timemushlimit * l2) / 400;
  }

string addAdj(string name, int id) {
  string bigadj[5] = { " of power", " of might", " of force", " of death", " of energy"};
  if(id >= 100) { name = "giant " + name; if(id%5 == 4) return name; }
  id %= 5;
  if(id == 4) return "epic " + name;
  else return name + bigadj[id];
  }

int mushroomlevel(int k) {
  if(k <= 0) return 0;
  if(k <= 1) return 1;
  if(k <= 3) return 2;
  if(k <= 6) return 3;
  if(k <= 10) return 4;
  if(k <= 15) return 5;
  if(k <= 21) return 6;
  return 7;
  }

string mushroomname(int k) {
  string tab[8] = {"Serf", "Lord", "Baron", "Duke", "Prince", "King", "Emperor", "God"};
  return tab[mushroomlevel(k)];
  }

int getItemForOrb(int c) {
  int c2 = cinf[c].color;
  for(int i=0; i<ITEMS; i++) if(i != IT_SXMUT && iinf[i].color == c2) return i;
  return 0;
  }

int getOrbForItem(int i) {
  for(int c=0; c<COLORS; c++) if(cinf[c].color == iinf[i].color) return c;
  return 0;
  }

bool activateOrb(weapon *w) {
  printf("%d -> %d -> %d\n", IT_PAMBI, getOrbForItem(IT_PAMBI), getItemForOrb(getOrbForItem(IT_PAMBI)));
  bool b = useup(getItemForOrb(w->color), w);
  if(b) { w->size--; w->sc.sc[WS_USE]++; }
  return b;
  }

string weapon::fullname() {
  string s = name();
  s += " (";
  s += type;
  s += its(size);
  s += ")";
  return s;  
  }

string weapon::name() {
  string edges[14] = {
    "fingernail", "dagger", "machete", "sabre", "shortsword", "longsword", "broadsword", "scythe",
    "double sword", "bastard sword", "two-handed sword", "katana", "triple sword", "epic sword"
    };
  
  string blunts[14] = {
    "fist", "staff", "club", "hammer", "quarterstaff", "mace", "warhammer", "morningstar", "flail", "maul",
    "great mace", "great flail", "triple staff", "epic staff"
    };
  
  string shields[10] = {
    "ring", "knuckle", "buckler", "spiked buckler", "small shield", "spiked shield", "medium shield",
    "large shield", "tower shield", "epic shield"
    };
  
  string missiles[10] = {
    "broken knife", "throwing knife", "double knife", "triple knife", "shuriken", "pentagram", "star",
    "heptagram", "double shuriken", "epic star"
    };
  
  string precedence[14] = {
    "bracketnail", "quick dagger", "machete of precedence", "parenthesabre",
    "shortsword of binding", "longsword of binding", "bracketsword", "scythe of the vinculum",
    "double sword of binding", "big bracketsword", "great sword of precedence", "daikakko katana",
    "triple sword of binding", "katana of Karibareisu"
    };
  
  string spears[12] = {
    "broken spear", "spear", "forked spear", "trident", "pitchfork",
    "large pitchfork", "double trident", "menorah fork", "double pitchfork",
    "tritrident", "decadent", "blivet"
    };
    
  string axes[14] = {
    "broad knife", "hatchet", "hand axe", "battle axe", "bardiche",
    "broad axe", "double axe", "cleaver", "large axe", "triple axe", 
    "halberd", "great axe", "giant axe", "executioner's axe"
    };
  
  string sizes[13] = {
    "tiny ", "small ", "", "large ", "big ", "great ",
    "huge ", "humongous ", "gigantic ", "colossal ",
    "", "large ", "massive "};
  
  string precadj[5] = {" of binding", " of strong binding", " of bracketing", " of precedence", " of order"};
  
  string bigblunt[6] = { "club", "staff", "hammer", "mace", "maul", "flail"};
  string bigsword[4] = { "sword", "katana", "blade", "edge"};
  
  string cname = info().wname;
  string name;
  
  if(wpnflags & wfTrap) {
    if(type == WT_BLADE) name = "blade trap";
    else if(type == WT_BLUNT) name = "smashing trap";
    else if(type == WT_DIV) name = "dicing trap";
    }
  
  else if(type == WT_PHASE)
    name = "wand of phase wall";

  else if(type == WT_ORB) {
    int i = getItemForOrb(color);
    name = "Orb" + iinf[i].name.substr(6);
    cname = "";
    }

  else if(type == WT_AXE) {
    if(size < 14) name = axes[size];
    else name = addAdj("axe", size);
    }

  else if(type == WT_SPEAR) {
    if(size < 12) name = spears[size];
    else name = addAdj("blivet", size);
    }

  else if(type == WT_SPEED) {
    if(size == 2) name = "witch's broom";
    else name = addAdj("broom", size);
    }

  else if(type == WT_STONE) {
    if(size < 13) {
      name = sizes[size] + butlast(cname);
      if(size >= 10) name += " boulder";
      }
    else name = addAdj(butlast(cname), size);
    return name;
    }

  else if(type == WT_DISK) {
    if(size < 10) {
      return sizes[size] + cname + "chakram";
      }
    else name = addAdj("chakram", size);
    }

  else if(type == WT_DIV || type == WT_SUBD || type == WT_QUI) {
    if(size == 0) name = "Null Sector";
    else if(size == 1) name = "Unisector";
    else if(size == 2) name = "Bisector";
    else if(size == 3) name = "Trisector";
    else if(size == 4) name = "Double Bisector";
    else if(size == 5) name = "Pentasector";
    else if(size == 6) name = "Bitrisector";
    else if(size == 7) name = "Heptasector";
    else if(size == 8) name = "Triple Bisector";
    else if(size == 9) name = "Double Trisector";
    else if(size == 10) name = "Decimator";
    else if(size == 11) name = "UnDecimator";
    else if(size == 12) name = "DoDecimator";
    else if(size == 16) name = "Quad Bisector";
    else name = addAdj("Divisor", size);
    if(type == WT_QUI)  {
      if(size == 3) name = "Sesquisector";
      else if(size == 5) name = "Quasquisector";
      else name = "Co " + name;
      }
    if(type == WT_SUBD) name = "Sub " + name;
    cname[0] = toupper(cname[0]);
    }
  else if(type == WT_GOLD) {
    name = "Sector";
    if(size > 1) name += " x" + its(size);
    cname[0] = toupper(cname[0]);
    }
  else if(type == WT_ROOT) {
    name = "Eradicator";
    if(size > 2) name = "Mighty Eradicator";
    if(size < 2) name = "Weak Eradicator";
    cname[0] = toupper(cname[0]);
    }
  else if(type == WT_LOG) {
    name = "Blade of Logaems";
    if(size > 2) name = "Blade of Mullog";
    cname[0] = toupper(cname[0]);
    }
  else if(type == WT_BOW) {
    if(size == 1) name = "short bow";
    else if(size == 2) name = "medium bow";
    else if(size == 3) name = "long bow";
    else if(size == 4) name = "composite bow";
    else name = addAdj("bow", size);
    }
  else if(type == WT_FUNG) {
    return "Staff of the Mushroom "+mushroomname(size);
    }
  else if(type == WT_DECO) {
    name = "Decomposer";
    if(size > 10) name = "Prime Decomposer";
    cname[0] = toupper(cname[0]);
    }
  else if(type == WT_PSLAY) {
    name = "Primeslayer";
    if(size) name = "S" + its(size)+" "+name;
    cname[0] = toupper(cname[0]);
    }
  else if(type == WT_RAND) {
    name = "Mersenne Twister";
    if(size>1) name = "+" + its(size)+" "+name;
    return name;
    }
  else if(type == WT_COLL) {
    name = "Syracuse Blade";
    if(size>0) name = "+" + its(size)+" "+name;
    return name;
    }
  else if(type == WT_QUAKE) {
    name = "titanic club";
    }
  else if(type == WT_DANCE) {
    if(size < 14) name = edges[size];
    else name = bigsword[size%4];
    name += " of dancing";
    }
  else if(type == WT_TIME) {
    if(size < 14) name = edges[size];
    else name = bigsword[size%4];
    name = "time-" + name;
    }
  else if(type == WT_RAIN) {
    if(size < 14) name = edges[size];
    else name = bigsword[size%4];
    name += " of Iris";
    }
  else if(type == WT_VORP) {
    if(size < 14) name = edges[size];
    else name = bigsword[size%4];
    name = "vorpal " + name;
    }
  else if(type == WT_PICK) {
    name = "pick axe";
    if(size == 2) name = "pick axe of Persephone";
    if(size == 3) name = "pick axe of speed";
    if(size == 4) name = "pick axe of greed";
    if(size == 5) name = "pick axe of power";
    if(size > 5) name = addAdj("pick axe", size);
    }
  else if(type == WT_PREC) {
    if(size >= 14) name = "epic "+bigsword[size%4]+precadj[size%5];
    else name = precedence[size];
    }
  else {
    int i = size;
    if(i > 13) i = 13;
    if((type == WT_SHLD || type == WT_MSL) && i > 9) i = 9;
    
    bool sq = false, tri = false, cub = false;
    for(int a=0; a<20; a++) { 
      if(a*a == size) sq = true;
      if(a*(a+1) == 2*size) tri = true; 
      if(a*a*a == size) cub = true;
      }
    
    if(type == WT_BLADE && size == 13) name = "claymore";
    else if(type == WT_BLADE && size == 14) name = "double scythe";
    else if(type == WT_BLADE && size > 10 && !(size%5)) 
      name = namenum(size/5)+"-handed sword";
    else if(type == WT_BLADE && size == 16) name = "quad sword";
    else if(type == WT_BLADE && size > 16) name = addAdj(bigsword[(size-(size/5))%4], size-(size/5));

    else if(type == WT_BLUNT && size == 13) name = "staff of misfortune";
    else if(type == WT_BLUNT && size == 14) name = "2-staff of fortune";
    else if(type == WT_BLUNT && size == 28) name = "staff of perfection";
    else if(type == WT_BLUNT && size > 14) name = addAdj(bigblunt[(size-15) % 6], size);

    else if(type == WT_SHLD  && size >= 9 && sq)  name = "square shield";
    else if(type == WT_SHLD  && size == 28)  name = "perfect shield";
    else if(type == WT_SHLD  && size >= 9 && tri)  name = "triangle shield";
    else if(type == WT_SHLD  && size >= 9 && cub)  name = "cubic shield";
    else if(type == WT_SHLD  && (size == 20 || size == 35 || size == 56 || size == 84))  name = "pyramid shield";
    else if(type == WT_SHLD  && size > 10)  name = addAdj("shield", size);

    else if(type == WT_MSL   && size == 10)  name = "double pentagram";
    else if(type == WT_MSL   && size == 12)  name = "triple shuriken";
    else if(type == WT_MSL   && size == 15)  name = "triple pentagram";
    else if(type == WT_MSL   && size == 16)  name = "death shuriken";
    else if(type == WT_MSL   && size > 8)  name = addAdj("star", size);
    
    else name = (type == WT_BLADE ? edges : type == WT_SHLD ? shields : type == WT_MSL ? missiles : blunts)[i];
    }
  return cname + name;
  }

// a struct which takes care of decomposition
struct decomposer {
  weapon *w;
  int odeco;
  
  // record stats!
  void sw() {
    swap(stats.ws[MOT_DECO], stats.ws[MOT_DIV]);
    swap(stats.maxoftype[MOT_DECO], stats.maxoftype[MOT_DIV]);
    }
  
  // initialize for given weapon...
  decomposer(weapon *W, int size) : w(W) {
    odeco = 0;
    if(w && w->type == WT_DECO) {
      int p = primediv(size);
      if(p == -1) return;
      odeco = w->size;
      w->type = WT_DIV; w->size = p;
      sw();
      }
    }
  
  // decomposer has been used...
  void reduce() {
    if(odeco) {
      odeco--;
      if(odeco == 0) {
        addMessage("Your weapon finally decomposes to "+w->name()+"!");
        if(w->size >= 17) achievement("DECOMPOSED");
        sw();
        }
      }
    }
  
  // object destroyed -- the divisor becomes a decomposer again
  ~decomposer() {
    if(odeco) {
      w->type = WT_DECO, w->size = odeco;
      sw();
      }
    }
  };

int ambiAttack(cell *c, int virt);

int sizeToVolume(int x) {
  //                  0   1   2   3   4   5   6   7   8   9  10  11  12
  int volmin[20] = { 10, 20, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80 };
  
  if(x > 12) x = 12;
  x = volmin[x];
  x = x + hrand(21);
  return x;
  }

int headsToVolume(int x) {
  int p = powerf(x);
  if(p > 10) p = 10;
  p = 10 + p * 6 + hrand(31);
  return p;
  }

int elementDelay() {
  return 10 + hrand(50);
  }

int hydraDelay() {
  return 50 + hrand(100);
  }

void playAttackSound(weapon *w, hydra *who, bool tiny = false, int delay = 0) {
  if(who && !who->lowhead() && who->color != HC_TWIN) {
    int vol = headsToVolume(who->heads - who->sheads);
    int hd = delay + hydraDelay();
    if(who->color == HC_SHADOW)
      // make it loud!
      playSound("hydras/hydra-shadow", 255, hd);
    else if(who->color == HC_VAMPIRE)
      playSound("hydras/hydra-vulture", vol, hd);
    else if(who->color == HC_ALIEN)
      playSound("hydras/hydra-alien", vol, hd);
    else if(who->color == HC_GROW)
      playSound("hydras/hydra-ivy", vol, hd);
    else if(who->color == HC_ANCIENT)
      playSound("hydras/hydra-ancient", vol, hd);
    else if(who->color == HC_WIZARD)
      playSound("hydras/hydra-arch", vol, hd);
    else if(who->color & HC_DRAGON)
      playSound("hydras/hydra-dragon", vol, hd);
    
    if(who->color < 10)
      playSound(cinf[who->color].soundfile, vol, hd + elementDelay());
    
    return;
    }
  if(w == NULL) return;
  
  int vol = sizeToVolume(w->size);
  
  if(tiny) vol /= 2;

  if(w->type == WT_BOW) playSound("weapons/bowAttack-hit", vol, delay);
  else if(w->type == WT_SHLD) playSound("weapons/shield", vol, delay);
  else if(w->type == WT_MSL) playSound("weapons/shurikenAttack", vol, delay);
  else if(w->type == WT_SPEAR) playSound("weapons/spearAttack-small", vol, delay);
  else if(w->type == WT_STONE) playSound("weapons/stone", vol, delay);
  else if(w->type == WT_RAND) playSound("weapons/mersenne-twister", vol, delay);
  else if(w->stuns() || w->type == WT_RAND) {
    if(w->size < 2 + hrand(4)) 
      playSound("weapons/staffAttack-small", vol, delay);
    else if(w->size < 6 + hrand(6)) 
      playSound("weapons/staffAttack-medium", vol, delay);
    else
      playSound("weapons/staffAttack-heavy", vol, delay);
    }
  else if(w->axe()) {
    if(w->size < 2 + hrand(4)) 
      playSound("weapons/axeAttack-small", vol, delay);
    else if(w->size < 6 + hrand(6)) 
      playSound("weapons/axeAttack-medium", vol, delay);
    else
      playSound("weapons/axeAttack-heavy", vol, delay);
    }
  else if(w->cuts() || w->xcuts() || w->axe() || w->type == WT_COLL) {
    if(w->size < 2 + hrand(4)) 
      playSound("weapons/swordAttack-small", vol, delay);
    else if(w->size < 6 + hrand(6)) 
      playSound("weapons/swordAttack-medium", vol, delay);
    else
      playSound("weapons/swordAttack-heavy", vol, delay);
    }

  /* if(tiny) vol /= 2;
  playSound(cinf[w->color].soundfile, vol, delay + elementDelay()); */
  }

void playSwitchSound(weapon *w) {
  if(w == NULL) return;
  int vol = sizeToVolume(w->size);
  
  if(w->type == WT_MSL) playSound("weapons/switchToShuriken", vol);
  else if(w->type == WT_SHLD) playSound("weapons/switchToShield", vol);
  else if(w->type == WT_SPEAR) playSound("weapons/switchToSpear", vol);
  else if(w->stuns()) playSound("weapons/switchToStaff", vol);
  else if(w->type == WT_BOW) playSound("weapons/switchToBow", vol);
  else if(w->cuts() || w->xcuts()) playSound("weapons/switchToSword", vol);
  else if(w->type == WT_STONE) playSound("weapons/switchToStone", vol);

  // playSound(cinf[w->color].soundfile, sizeToVolume(w->size) / 2);
  }

void addAnimation(cell *c, int headid, int cutcount, int color);

void collapse(hydra *h, hydra *whoh) {
  stats.owncrush++;
  addMessage("The "+h->name()+" collapses under the weight of its own heads!");
  achievement("COLLAPSE");
  M[h->pos].hydraDead(whoh);
  }

void cell::attack(weapon *w, int power, sclass *who) {

  hydra *whoh = who ? who->asHydra() : NULL;
  
  if(h == twin && whoh) {
    twinswap();
    hydraAttackPlayer(whoh, true);
    twinswap();
    if(h->heads <= 0) hydraDead(NULL);
    return;
    }
  
  string origname = mushrooms ? "mushroom" : h->name();
  bool isStunned = mushrooms ? false : h->sheads == h->heads;

  if(mushrooms && w->type == WT_TIME) { 
    mushrooms += power;
    w->addStat(WS_MHEAD, power, 0);
    playAttackSound(w, whoh, true);
    addMessage("You attack the mushroom!");
    return;
    }

  if(mushrooms && (w->cuts() || w->doubles())) { 
    mushrooms -= power;
    w->addStat(WS_MHEAD, power, 0);
    if(mushrooms) addMessage("You attack the mushroom!");
    else addMessage("You cut through the mushroom!"), w->addStat(WS_MKILL, 1, 0);
    playAttackSound(w, whoh, true);
    return;
    }
  if(mushrooms && w->xcuts()) {
    int cutcount = w->cutoff(mushrooms, false);
    w->addStat(WS_MHEAD, cutcount, 0);
    mushrooms -= cutcount;
    
    if(w->type == WT_GOLD) for(int u=1; u<w->size; u++) {
      if(w->cutoff(mushrooms, false) >= 0) {
        int cutcount = w->cutoff(mushrooms, false);
        w->addStat(WS_MHEAD, cutcount, 0);
        mushrooms -= cutcount;
        }
      }

    addMessage("You attack the poor mushroom with your mighty weapon!");
    playAttackSound(w, whoh, true);
    return;
    }
  
  int spec = 0;
  if(whoh && whoh->lowhead()) spec = MOT_ETTIN;
  else if(whoh && whoh->zombie) spec = MOT_ZOMBIE;
  else if(whoh && whoh != twin) spec = MOT_HYDRA;
  else if(who == w) spec = MOT_TRAP;
  
  if(h->color == HC_TWIN) stats.twinwounds += h->heads;
  
  string xs = its(h->heads);
  int cutcount = 0;
  
  if((w->type == WT_VORP) != (vorplast == h))
    vorpalClear();

  if(w->type == WT_VORP) {
    vorplast = h;
    if(!h->zombie)
      vorpalhsf += min(power, h->heads - h->sheads);
    if(P.active[IT_PSWIP] || P.active[IT_PCHRG])
      vorpalClear();
    }

  if(w->type == WT_TIME) {
    w->addStat(WS_GROW, power, spec);
    h->heads += power;
    xs += " + " + its(power);
    cutcount = power;
    }
  
  else if(w->cuts()) {
    cutcount = power;
    h->heads -= cutcount; 
    if(whoh && whoh->color == HC_VAMPIRE)
      whoh->heads += power, stats.vampire += power;
    if(h->sheads > h->heads) {
      w->addStat(WS_HSTUN, h->sheads-h->heads, spec);
      h->cutStunHeads();
      }
    xs += " - " + its(power);
    w->addStat(WS_HHEAD, cutcount, spec);
    }

  if(w->xcuts()) {
  
    // int si = w->size;
    
    // replay:
    
    cutcount = w->cutoff(h->heads, false);
    int scutcount = w->cutoff(h->sheads, true);

    h->heads  -= cutcount;
    h->sheads -= scutcount;
    h->stunforce -= w->cutoff(h->stunforce, true);

    // enchanted Golden Sector...
    if(w->type == WT_GOLD) for(int u=1; u<w->size; u++) {
      if(w->cutoff(h->heads, false) >= 0) {
        int xcutcount = w->cutoff(h->heads, false);
        int xscutcount = w->cutoff(h->sheads, true);

        h->heads  -= xcutcount;
        h->sheads -= xscutcount;
        h->stunforce -= w->cutoff(h->stunforce, true);
        
        cutcount += xcutcount;
        scutcount += xscutcount;
        }
      }

    w->addStat(WS_HHEAD, cutcount, spec);
    w->addStat(WS_HSTUN, scutcount, spec);

    if(w->type == WT_DIV) xs += " / " + its(power);
    else if(w->type == WT_SUBD) xs += " (-1) /"+its(power);
    else if(w->type == WT_QUI) xs += " /("+its(power)+"/"+its(power-1)+")";
    else if(w->type == WT_GOLD) xs += " /phi";
    else if(w->type == WT_PSLAY) xs = "pi("+xs+")";
    else xs = (w->type) + its(power)+"("+xs+")";
  
    if(w->type == WT_PSLAY && h->heads >= h->sheads + w->size) {
      h->sheads += w->size;
      h->stunforce += w->size * w->info().stunturns;
      }
    }

  if(w->doubles()) {
    w->addStat(WS_HHEAD, power, spec);
    if(h->noregrow()) {
      h->heads -= power;
      xs = "- " + its(power);
      }
    else {
      int sf = 2*power*w->info().stunturns;
      h->heads += power;
      h->sheads += power*2;
      h->stunforce += sf;
      if(h->sheads > h->heads) {
        w->addStat(WS_HSTUN, h->sheads-h->heads, spec);
        h->cutStunHeads();
        }
      xs += " + " + its(power);
      }
    }
  
  if(w->stuns()) { 
    w->addStat(WS_HHEAD, power, spec);
    if(h->color == HC_TWIN) {
      h->heads -= power;
      xs = "(twin) - "+its(power);
      }
    else {
      h->sheads += power;
      h->stunforce += w->info().stunturns * power;
      xs = "[stun] " + its(power);
      }

    if(h->color != HC_SHADOW) {
      stunnedHydra = h; stunnedColor = w->gcolor();
      }
    }
  
  int grow = h->res[w->color];

  if(h->color == HC_EVOLVE) {
    if(w->type == WT_TIME)
      h->res[w->color]--, grow--;
    else
      h->res[w->color]++;
    }

  if(!w->stuns() && h->heads && grow) {
    if(grow < 0) {
      grow = -grow*cutcount;
      }
    
    if(w->type == WT_TIME) {
      w->addStat(WS_HHEAD, grow, spec);
      h->heads -= grow;
      xs += " - " + its(grow);
      }
    else {
      h->heads += grow;
      w->addStat(WS_GROW, grow, spec);
      xs += " + " + its(grow);
      }
    xs += " = " + its(h->heads);
    }

  if(w->axe() && grow && h->heads) {
    h->sheads += grow;
    h->stunforce += (grow + power) * w->info().stunturns;
    stunnedHydra = h; stunnedColor = w->gcolor();
    }
  
  if(h->invisible()) xs = "";
  
  if(w->color < HCOLORS) {
    h->dirty &= ~(1<<w->color);
    if(h->color == w->color) hydraKnowDirty(h);
    }

  int splashcol = (h->dirty & IS_DIRTY) ? HC_ANCIENT : h->color & HC_DRMASK;

  if(w->type == WT_TIME && h->zombie) {
    M[h->pos].mushrooms = h->heads;
    h->heads = 0;
    addMessage(form(whoh, "turn") + " the "+origname+" back into mushrooms! " + xs);
    playAttackSound(w, whoh);
    }
  else if(whoh && !(h->aware() || whoh->aware()))
    ;
  else if(h->heads <= 0) {
    addMessage(form(who, "kill") + " the "+origname+"! " + xs);
    if(power > stats.maxambi) stats.maxambi = power;
    playAttackSound(w, whoh);
    addAnimation(this, splashcol, cutcount, 0);
    }
  else if(h->heads == h->sheads && !isStunned) {
    addMessage(form(who, "completely stun") + " the "+origname+"! "+xs);
    playAttackSound(w, whoh);
    }
  else {
    if(who && inWaitMode) ;
    else {
      addMessage(form(who, w->info().hverb) + " the "+origname+"! "+xs);
      playAttackSound(w, whoh);
      }
    addAnimation(this, splashcol, cutcount, 0);
    }
  
  if(h->color == HC_TWIN) stats.twinwounds -= h->heads;  

  if(!h->heads) {
    w->addStat(WS_HKILL, 1, spec);
    if(w->type == WT_BLADE && w->size >= 1000) achievement("GIANTWEAPON");
    if(w->type == WT_GOLD && cutcount >= 100) achievement("SILVERCUTTER");
    if(h->color == HC_ETTIN && w->cuts() && w->size == 2) achievement("WELLPREPARED");
    if(w->type == WT_VORP)
      vorpalCredit();
    hydraDead(whoh);
    }

  else if(h->heads >= COLLAPSE)  collapse(h, whoh);

  }

bool checkShadow(cell& c);

int ambiAttack(cell *c, int virt) {
  // virt=2 : perform the attack without outputing messages or killing the hydra
  // virt=1 : just calculate and return the number of heads regrown
  // virt=0 : cut off heads, return 1 if successful
  
  if(P.race == R_ELF) {
    if(virt) return 0;
    checkShadow(*c);
    addMessage("Elves are unable to perform ambidextrous attacks.");
    return false;
    }

  hydra *target = c->h;
  
  if(virt == 1 && target && target->invisible()) {
    addstr("???");
    return true;
    }

  int have[16], hvdiv[16], hvera[16], haveaxe[16];
  for(int b=0; b<16; b++)
    have[b] = 0, hvdiv[b] = 1, hvera[b] = 0, haveaxe[b] = 0;
  
  int hcut = 0, hgrow = 0, hsforce = 0, hdiv = 1, hroot = 1, hocut = 0,
    hprecut = 0, hsgrow = 0, hacut = 0, hstun = 0, hdub = 0;
    
  bool mystery = false;
  
  for(int b=0; b<P.arms; b++) if(havebit(P.ambiArm, b)) {
    weapon *w = wpn[b];
    if(!w) {
      if(virt) return 0;
      checkShadow(*c);
      addMessage("You have selected an empty slot for ambidextrous attack.");
      return false;
      }
    if(!w->ambivalid()) {
      if(virt) return 0;
      if(w->type == WT_DECO) {
        checkShadow(*c);
        addMessage("Decomposer can be used only as the star weapon.");
        return false;
        }
      checkShadow(*c);
      addMessage("You have selected a weapon which cannot be used for ambidextrous attack.");
      return false;
      }
    
    if(virt == 1 && w->color < COLORS && (c->h->dirty & (1<<w->color)))
      mystery = true;

    if(w->axe())
      hacut += w->size,
      haveaxe[w->color] += w->size;

    else if(w->cuts())
      (w->type == WT_PREC ? hprecut : hcut) += w->size, 
      have[w->color] += w->size;
    
    if(w->cuts() && w->color == HC_OBSID) hocut += w->size;
    
    if(w->doubles()) {
      if(target && !target->noregrow()) {
        hdub += w->size;
        hsforce += w->size * 2 * w->info().stunturns;
        }
      else
        hcut += w->size;
      }

    if(w->stuns())
      hstun += w->size, 
      hsforce += w->size * w->info().stunturns;

    if(w->type == WT_DIV) {
      hdiv *= w->size, hvdiv[w->color] *= w->size;
      if(hdiv >= 1000000) hdiv = 1000001;
      if(hvdiv[w->color] > 1000000) hvdiv[w->color] = 1000001;
      }

    if(w->type == WT_ROOT)
      hroot *= w->size, hvera[w->color] ++;
    // avoid weird overflows
    if(hroot >= 1000000) hroot = 1000001;
    }
  
  int nheads, nsheads, nsforce;
  
  if(target) {
    for(int b=0; b<COLORS; b++) {
      int gr = 0;
      if(have[b] || haveaxe[b] || hvdiv[b]>1 || hvera[b]) {
        if(target->res[b] > 0) gr = target->res[b];
        else gr = -(have[b]+haveaxe[b]) * target->res[b];
        }
      hgrow += gr;
      if(haveaxe[b]) {
        if(target->res[b] > 0) gr = target->res[b];
        else gr = -haveaxe[b] * target->res[b];
        hsgrow += gr;
        hgrow -= gr;
        hsforce += (gr + haveaxe[b]) * cinf[b].stunturns;
        }
      }
  
    nheads = target->heads, nsheads = target->sheads, nsforce = target->stunforce;
    }
  else
    nheads = c->mushrooms, nsheads = nheads, nsforce = 1;

  int origheads = nheads;

  int hcera = 0, hcdiv = 0;
  
  string targetname = target ? target->name() : "mushroom";
  
  if(hprecut) {
    nheads -= hprecut;
    if(nheads < 0) {
      if(virt) return 0;
      if(!checkShadow(*c))
        addMessage("Not enough heads for your ambidextrous attack!");
      return false;
      }
    if(nsheads > nheads && nheads) {
      nsheads = nheads;
      nsforce = ((nsforce+nsheads-1) / nsheads) * nheads;
      }
    }
  
  if(hroot > 1) {
    weapon fake(0, hroot, WT_ROOT);
    hcera = fake.cutoff(nheads, false);
    if(hcera < 0) {
      if(virt) return 0;
      if(!checkShadow(*c))
        addMessage("Your ambidextrous eradicator attack does not work on the "+targetname+".");
      return false;
      }
    nheads -= hcera;
    nsheads -= fake.cutoff(nsheads, true);
    nsforce -= fake.cutoff(nsforce, true);
    for(int b=0; b<COLORS; b++)
      if(hvera[b] && target && target->res[b] < 0)
        hgrow -= target->res[b] * hcera;
    }
  
  if(hdiv > 1) {
    weapon fake(0, hdiv, WT_DIV);
    hcdiv = fake.cutoff(nheads, false);
    if(hcdiv < 0) {
      if(virt) return 0;
      if(!checkShadow(*c))
        addMessage("Your ambidextrous divisor attack does not work on the "+targetname+".");
      return false;
      }
    nheads -= hcdiv;
    nsheads -= fake.cutoff(nsheads, true);
    nsforce -= fake.cutoff(nsforce, true);
    for(int b=0; b<COLORS; b++) 
      if(hvdiv[b] && target && target->res[b] < 0) 
        hgrow -= target->res[b] * nheads * (hvdiv[b]-1);
    }
  
  if(hacut+hstun+hdub > 0 && hacut+hstun+hdub > nheads - nsheads) {
    if(virt) return false;
    if(!checkShadow(*c))
      addMessage("Not enough active heads for your ambidextrous attack!");
    return false;
    }
  
  if(hacut + hcut + hdub + hstun > nheads) {
    if(virt) return false;
    if(!checkShadow(*c))
      addMessage("Not enough heads for your ambidextrous attack!");
    return false;
    }  

  if(virt==1) {
    if(hacut + hcut == nheads)
      addstr("KILL");
    else if(mystery)
      addstr("???");
    else {
      int q = nheads-hcut-hacut+hgrow+hsgrow+hdub;
      addstri("="+its(q));
      int q2 = nheads-nsheads-hcut-hacut-hstun;
      if(q2<0) q2 = 0;
      q2 += hgrow;
      if(drainpower(c->h)) displayDrain(c->h, q2);
      }
    //addMessage("nh "+its(nheads)+" c "+its(hcut)+" ac "+its(hacut)+" g "+its(hgrow)+" sg "+its(hsgrow)+ " d"+its(hdub));
    return 1;
    }
  
  if(!virt) if(target) for(int b=0; b<COLORS; b++)
    if(have[b] || haveaxe[b] || hvdiv[b]>1 || hvera[b])
      target->dirty &= ~(1<<b);

  if(!virt) vorpalClear();
  
  if(P.ambifresh == P.active[IT_PAMBI] && !virt)
    P.ambifresh--;
  
  int st = target ? WS_HHEAD : WS_MHEAD;
  if(!virt) {
    stats.ws[MOT_ROOT ].sc[st] += hcera;
    stats.ws[MOT_DIV  ].sc[st] += hcdiv;
    stats.ws[MOT_OBSID].sc[st] += hocut;
    stats.ws[MOT_BLADE].sc[st] += hcut-hocut;
    stats.ws[MOT_AMBI ].sc[st] += hcera+hcdiv+hcut+hacut;
    }

  if(!virt) for(int b=0; b<P.arms; b++) if(havebit(P.ambiArm, b)) {
    wpn[b]->addStat(WS_USE, 1, 0), stats.ambiwpn++;
    attackEffect(wpn[b], target && target->res[wpn[b]->color] < 0);
    int ambidelay = hrand(50);
    if(wpn[b]->type == WT_PREC) ;
    else if(wpn[b]->type == WT_ROOT) ambidelay += 50;
    else if(wpn[b]->xcuts()) ambidelay += 100;
    else ambidelay += 150;
    if(!hdiv && ambidelay >= 150) ambidelay -= 50;
    if(!hroot && ambidelay >= 100) ambidelay -= 50;
    if(!hprecut) ambidelay -= 50;

    playAttackSound(wpn[b], NULL, true, ambidelay);
    }
  
  nheads -= hcut+hacut; nsforce += hsforce;
  
  if(!virt) stats.ws[MOT_AMBI].sc[WS_USE] ++;
  
  if(target && target->color == HC_EVOLVE && !virt) for(int b=0; b<COLORS; b++)
    if(have[b] || haveaxe[b] || hvdiv[b]>1 || hvera[b]) 
      target->res[b]++;

  if(nheads == 0 && hdub == 0) {
    if(virt == 2) {
      target->heads = 0;
      return true;
      }
    if(target) addMessage("You slay the "+target->name()+" with your ambidextrous attack!");
    else addMessage("You destroy this miserable mushroom!!!");
    stats.ws[MOT_AMBI].sc[target ? WS_HKILL : WS_MKILL]++;
    if(origheads > stats.maxambi) stats.maxambi = origheads;
    if(origheads >= 2000) achievement("AMBIKILLER");
    hgrow = 0; hsgrow = 0;
    }
  else if(!virt)
    addMessage(target ? "You attack the "+target->name()+"!" : "You attack the poor mushroom!!");
  
  if(!virt) stats.ws[MOT_AMBI].sc[WS_GROW] += hgrow+hsgrow;
  
  if(!target && !virt) {
    c->mushrooms = nheads;
    return true;
    }
  
  target->heads = nheads, target->sheads = nsheads, target->stunforce = nsforce;

  target->heads += hsgrow+hdub;
  target->sheads += hsgrow+hstun+2*hdub;

  if(target->sheads > target->heads) {
    if(!virt)
      stats.ws[MOT_AMBI].sc[WS_HSTUN] += nsheads - nheads;
    target->cutStunHeads();
    }
  
  target->heads += hgrow; // these never stunned
  
  if(target->heads == 0) {
    M[target->pos].hydraDead(NULL);
    }

  else if(target->heads >= COLLAPSE) collapse(target, NULL);

  return true;
  }

weapon *weapon::reduce() {
  if(type == WT_BLADE && size > 4 && (size % 4 == 0))
    return new weapon(color, 4 * (size / 8), WT_BLADE);
  else if(type == WT_BLADE && size > 12 && (size % 3 == 0))
    return new weapon(color, size/3 - 3, WT_DIV);
  else if(type == WT_BLADE && (size % 3 == 1) && size >= 2 && color < HCOLORS)
    return new weapon(color, size/3, WT_SHLD);
  else if(type == WT_BLADE && size >= 3)
    return new weapon(color, size/3, WT_MSL);
  else if(type == WT_BLADE && size == 2)
    return new weapon(color, size/2, WT_BLADE);
  else if(type == WT_MSL && (size % 2 == 1) && size >= 3)
    return new weapon(color, size/2, WT_MSL);
  else if(type == WT_MSL && (size % 2 == 0) && size >= 2)
    return new weapon(color, size/2, WT_BLADE);
  else if(type == WT_SHLD && color > HCOLORS && size >= 2)
    return new weapon(color, size/2, WT_SHLD);
  else if(type == WT_SHLD && (size % 2 == 0) && size >= 2)
    return new weapon(color, size/2, WT_BLADE);
  else if(type == WT_SHLD && (size % 2 == 1) && size >= 2)
    return new weapon(color, size/2, WT_SHLD);
  else if(type == WT_DIV && size >= 2)
    return new weapon(color, size/2, WT_BLADE);
  else if(type == WT_SUBD && size > 2)
    return new weapon(color, size/2, WT_BLADE);
  else if(type == WT_QUI  && size > 9 && !(size % 3))
    return new weapon(color, size/3, WT_SUBD);
  else if(type == WT_QUI  && size > 4)
    return new weapon(color, size/2, WT_DIV);
  else if(type == WT_GOLD && size >= 16)
    return new weapon(13 - color, 1, WT_GOLD);
  else if(type == WT_ROOT && size >= 2)
    return new weapon(color, size/2, WT_DIV);
  else if(type == WT_DECO && size >= 2 && primediv(size) > 0)
    return new weapon(color, primediv(size), WT_DIV);
  else if(type == WT_LOG && size >= 2)
    return new weapon(color, size/2, WT_ROOT);
  else if(type == WT_BOW && size >= 2)
    return new weapon(color, size/2, WT_BOW);
  else if(type == WT_BLUNT && size >= 9 && squareroot(size) >= 0)
    return new weapon(color, squareroot(size), WT_STONE);
  else if(type == WT_BLUNT && !(size %3))
    return new weapon(color, size/3, WT_SPEAR);
  else if(type == WT_BLUNT && size >= 2)
    return new weapon(color, size/2, WT_BLUNT);
  else if(type == WT_AXE   && size >= 2)
    return new weapon(color, size/2, WT_AXE);
  else if(type == WT_SPEAR && size >= 2)
    return new weapon(color, size/2, WT_BLUNT);
  else if(type == WT_DANCE && size >= 2)
    return new weapon(color, size/2, WT_DANCE);
  else if(type == WT_VORP && size >= 2)
    return new weapon(color, size/2, WT_VORP);
  else if(type == WT_FUNG && size >= 2)
    return new weapon(color, size/2, WT_BLUNT);
  else if(type == WT_PICK && size >= 2)
    return new weapon(color, size/2, WT_BLUNT);
  else if(type == WT_PREC && !(size%3))
    return new weapon(color, size/3, WT_PREC);
  else if(type == WT_PREC && size > 3 && size < 10)
    return new weapon(color, 3, WT_DIV);
  else if(type == WT_PREC && size >= 10)
    return new weapon(color, (size / 9) * 3 + (size % 3), WT_DIV);
  else if(type == WT_PSLAY && size)
    return new weapon(color, size, WT_SHLD);
  else if(type == WT_STONE && size >= 2)
    return new weapon(color, size/2, WT_BLUNT);
  else if(type == WT_DISK && size >= 2)
    return new weapon(color, size/2, WT_DISK);
  else if(type == WT_SPEED && size >= 4)
    return new weapon(color, size/2, WT_SPEED);
  else if(type == WT_TIME && size >= 1)
    return new weapon(color, size/2, WT_TIME);
  else if(type == WT_RAIN && size >= 3)
    return new weapon(color, size/3, WT_RAIN);
  else if(type == WT_RAND && size >= 2) {
    int pos[5] = { WT_BLADE, WT_MSL, WT_SHLD, WT_DIV, WT_AXE };
    return new weapon(randHCol(), 1 + hrand(size-1), pos[hrand(5)]);
    }
  else if(type == WT_ORB && size >= 3) {
    int it = getItemForOrb(color);
    if(it == IT_RFUNG) return new weapon(getOrbForItem(IT_RSTUN), size/3, WT_ORB);
    if(it == IT_RCONF) return new weapon(getOrbForItem(IT_RDEAD), size/3, WT_ORB);
    if(it == IT_RNECR) return new weapon(getOrbForItem(IT_RSTUN), size/3, WT_ORB);

    if(it == IT_RDEAD) return new weapon(getOrbForItem(IT_RSTUN), size/2, WT_ORB);
    if(it == IT_RSTUN) return new weapon(getOrbForItem(IT_RDEAD), size/2, WT_ORB);

    if(it == IT_PAMBI) return new weapon(getOrbForItem(IT_RCANC), size/3, WT_ORB);
    if(it == IT_PSWIP) return new weapon(getOrbForItem(IT_PCHRG), size/3, WT_ORB);

    if(it == IT_RCANC) return new weapon(getOrbForItem(IT_PFAST), size/3, WT_ORB);

    if(it == IT_PFAST) return new weapon(getOrbForItem(IT_RGROW), size/3, WT_ORB);
    if(it == IT_PCHRG) return new weapon(getOrbForItem(IT_RGROW), size/3, WT_ORB);

    if(it == IT_RGROW) return new weapon(getOrbForItem(IT_RGROW), size/3, WT_ORB);
    return NULL;
    }
  else return NULL;
  }

void weapon::grow() {
  string lname = name();
  size++;
  switch(type) {
    case WT_BLADE: case WT_DANCE: case WT_VORP: case WT_RAIN: case WT_TIME:
      switch(size) {
        case 7: case 14:
          addMessage("The "+lname+" turns into a 7-like shape!");
          break;
        case 8:
          addMessage("The "+lname+" splits into two!");
          break;
        case 9:
          addMessage("The "+lname+" merges into one big sword!");
          break;
        case 12:
          addMessage("The "+lname+" splits into three!");
          break;
        default:
          addMessage(
            size > 12 ? 
              "The "+lname+" now looks even more impressive and sharp!" :
              "The "+lname+" becomes bigger and sharper!"
              );
        }
      break;
    
    case WT_ORB: {
      int i = getItemForOrb(color);
      if(i == IT_RGROW) size += 2;
      if(i == IT_PFAST || i == IT_PCHRG) size++;
      int chcount = 0;
      if(i == IT_RSTUN || i == IT_RDEAD) chcount = 3;
      if(i == IT_RFUNG || i == IT_RNECR || i == IT_RCONF) chcount = 5;
      if(i == IT_PAMBI || i == IT_PSWIP) chcount = 2;
      if(chcount) {
        P.orbcharge++;
        if(P.orbcharge == chcount-1) {
          addMessage("The "+lname+" requires one more charge to recharge!");
          size--;
          break;
          }
        else if(P.orbcharge < chcount) {
          addMessage("The "+lname+" requires "+its(chcount-P.orbcharge)+" more charges to recharge!");
          size--;
          break;
          }
        else P.orbcharge = 0;
        }
      addMessage("The "+lname+" glows with new power!");
      break;
      }
    
    case WT_DECO:
      size += 4; // 5 in total
      addMessage("The "+lname+" seems to slow down its decomposition!");
      break;
    
    case WT_PHASE:
      size += 4; // 5 in total
      addMessage("The "+lname+" glows with new power!");
      break;
    
    case WT_QUAKE:
      if(size == 1)
        addMessage("The "+lname+" has its power restored!");
      else
        addMessage("The "+lname+" glows with new power!");
      break;
    
    case WT_PICK:
      if(size == 2)
        addMessage("The "+lname+" now can dig through mushrooms instantly!");
      else if(size == 3)
        addMessage("The "+lname+" now can dig through walls faster!");
      else if(size == 4)
        addMessage("The "+lname+" grows stronger! One more...");
      else if(size == 5)
        addMessage("The "+lname+" now can dig through walls instantly!");
      else
        addMessage("The "+lname+" grows stronger!");
      break;
      
    case WT_FUNG:
      addMessage("The "+lname+" releases a cloud of spores!");
      break;
      
    case WT_DIV: case WT_ROOT: case WT_LOG: case WT_SUBD: case WT_QUI:
      switch(size) {
        case 1:
          addMessage("The "+lname+" now looks more sane, but still not very useful.");
          break;
        case 2:
          addMessage("The "+lname+" looks useful now!");
          achievement("MAKEUSEFUL");
          break;
        default:        
          addMessage("The "+lname+" buzzes magically!");
        }
      break;
    
    case WT_PSLAY:
      if(size == 1)
        addMessage("Sparks fly around the "+lname+"!");
      else
        addMessage("Sparks multiply around the "+lname+"!");
      break;
    
    case WT_RAND:
      addMessage("The "+lname+" twists itself!");
      break;
    
    case WT_GOLD: case WT_COLL:
      addMessage("The "+lname+" looks even more impressive now!");
      break;
    
    case WT_SPEED:
      addMessage("The "+lname+" looks more aerodynamic now!");
      break;
    
    case WT_SHLD:
      switch(size) {
        case 2:
          addMessage("The "+lname+" grows to buckler size!");
          break;
        case 3: case 5:
          addMessage("The "+lname+" grows some spikes!");
          break;
        case 4: case 6:
          addMessage("The spikes on the "+lname+" spin and form a larger shield!");
          break;
        default:
          addMessage("The "+lname+" suddenly grows!");
        }
      break;
    
    case WT_MSL:
      addMessage("The "+lname+" grows another blade!");
      break;
    
    case WT_BOW:
      addMessage("The "+lname+" grows bigger!");
      break;
    
    case WT_BLUNT:
      switch(size) {
        case 2: case 5:
          addMessage("The "+lname+" becomes heavier at one end!");
          break;
        case 3: case 6: case 10:
          addMessage("The "+lname+" becomes even heavier at one end!");
          break;
        case 4:
          addMessage("The "+lname+" becomes heavy at both ends!");
          break;
        case 7:
          addMessage("The "+lname+" grows spikes!");
          break;
        case 8: case 11:
          addMessage("The "+lname+" grows a chain!");
        case 9:
          addMessage("The "+lname+" becomes heavier and loses its chain and spikes!");
        case 12:
          addMessage("The "+lname+" splits into three!");
        default:
          addMessage("The "+lname+" now looks even more impressive and heavy!");
        }
      break;
    
    case WT_PREC:
      addMessage("Small glowing brackets circle the "+lname+" for a moment!");
      break;
    }
  }

bool transmute(weapon *w) {
  if(!w) {
    addMessage("Magically painting your fingernails won't help you against Hydras!");
    return false;
    }
  string lname = w->name();

  if(P.race == R_ATLANTEAAN && w->type != WT_ORB && w->type != WT_GOLD) {
    w->color = atlantean_xmut_color;
    addMessage("You now got a "+w->name()+"!");
    return true;
    }

  wpnset++;
  if(w->type == WT_GOLD) {
    addMessage("The "+lname+" glows blindingly and changes both its color and shape!");
    playSound("../hydra-old/rune", sizeToVolume(w->size), 0);
    w->color = 13 - w->color;
    return true;
    }
  if(w->type == WT_ORB) {
    int i1 = getItemForOrb(w->color);
    int i2 = i1;
    if(i1 == IT_RSTUN) i2 = IT_RDEAD;
    if(i1 == IT_RDEAD) i2 = IT_RSTUN;
    if(i1 == IT_PCHRG) i2 = IT_PFAST;
    if(i1 == IT_PFAST) i2 = IT_PCHRG;
    if(i1 == IT_RCANC) i2 = IT_RGROW;
    if(i1 == IT_RGROW) i2 = IT_RCANC;
    if(i1 == IT_RCONF) i2 = IT_RNECR;
    if(i1 == IT_RNECR) i2 = IT_RFUNG;
    if(i1 == IT_RFUNG) i2 = IT_RCONF;
    if(i1 == IT_PSWIP) i2 = IT_PAMBI;
    if(i1 == IT_PAMBI) i2 = IT_PSWIP;
    if(i1 == i2) return false;
    addMessage("The "+lname+" glows!");
    playSound("../hydra-old/rune", 50, 0);
    w->color = getOrbForItem(i2);
    if(i2 == IT_RCANC) w->size = w->size * 3/10;
    if(i1 == IT_RCANC) w->size = w->size * 10/3;
    return true;
    }    
  /* if(w->type == WT_VORP) {
    addMessage("The transmutation causes the "+lname+" to lose its accumulated power!");
    vorpalClear();
    w->size = 1;
    } */
  if(w->color == HCOLORS - 1) w->color = 0;
  else if(w->color == HCOLORS + SCOLORS - 1) w->color = HCOLORS;
  else if(w->color == HCOLORS + SCOLORS) ; // no change for the obsidian dagger
  else w->color++;
  char buf[200];
  sprintf(buf, w->info().hmessage, lname.c_str());
  addMessage(buf);
  playSound(cinf[w->color].soundfile, 100);
  return true;
  }

int encode(int heads, int sheads, int& code) {
  if(heads < AMAX) {
    code = heads + sheads * AMAX + AMAXS;
    return 2;
    }
  else if(heads < AMAXS) {
    code = heads;
    return sheads == 0 ? 2 : 1;
    }
  code = 0;
  return 0;
  }

bool decode(int& heads, int& sheads, int code) {
  if(code < AMAXS) {
    heads = code; sheads = 0;
    return true;
    }
  else {
    heads = (code-AMAXS) % AMAX;
    sheads = (code-AMAXS) / AMAX;
    return true;
    }
  }

bool wpnfirst(int w1, int w2) {
  if(!(w2&AMBIWPN) && havebit(P.twinarms, w2))
    return true;
  return false;
  }

#define HINF 20000

// Calculate the effect of using weapon W on a variant of hydra h
// with hd heads, sh stunned heads.

bool calcWeaponEffect(weapon *W, hydra *h, int &hd, int &sh) {
  int ws = W->size;
  int hr = h->res[W->color];

  bool isBlade = W->cuts();
  if(W->type == WT_VORP) ws = 1;

  if(W->type != WT_BOW && !W->xcuts())
  if(W->activeonly() ? hd-sh < ws : hd<ws)
    return false;
    
  if(W->type == WT_TIME) {
    if(hr < 0) hr = -hr * ws;
    if(hd-sh > hr || (hd-sh == hr && sh))
      hd = hd - hr + ws;
    else return false;
    return true;
    }

  else if(W->stuns()) {
    sh += ws;
    return true;
    }

  else if(W->doubles()) {
    int siz = min(hd, ws);
    if(h->noregrow()) {
      hd -= ws;
      if(hd <= 0) { hd = 0; return true; }
      }
    else {
      hd += siz; sh += siz; sh += siz; if(sh>hd) sh=hd;
      }
    return true;
    }

  else if(W->xcuts()) {
    int cut = W->cutoff(hd, false);
    if(cut < 0) return false;
    hd -= cut;
    sh -= W->cutoff(sh, true);

    if(W->type == WT_GOLD) for(int u=1; u<W->size; u++) {
      if(W->cutoff(hd, false) >= 0) {
        int c = W->cutoff(hd, false);
        cut += c;
        hd -= c;
        sh -= W->cutoff(sh, false);
        }
      }

    if(hd == 0) {
      return true;
      }

    if(hr < 0) hd -= hr * cut;
    else hd += hr;

    if(W->type == WT_PSLAY && hd >= sh + ws) {
      sh += ws;
      }
    
    return true;
    }

  else if(isBlade && (hd == ws)) {
    hd = 0;
    return true;
    }

  else if(isBlade) {
    hd -= ws; if(sh > hd) sh = hd;
    int gr = hr < 0 ? -hr*ws : hr;
    hd += gr;
    if(W->axe()) sh += gr;
    return true;
    }

  else return true;
  }

struct hydraAnalyzer {
  hydra *h;
  
  vector<int> damh;
  
  void setDamh() {
    damh.resize(AMAXS);
    if(h->color == HC_SHADOW) {
      for(int i=0; i<AMAXS; i++) damh[i] = min(i, 3);
      }
    else if(h->color >= 0) {
      SI.prepare(AMAXS, h);
      for(int i=0; i<AMAXS; i++) damh[i] = SI.dampost(i);
      }
    else {
      // for mushrooms, just calculate time required
      for(int i=0; i<AMAXS; i++) damh[i] = 1;
      }  
    if(h->ewpn) {
      // for ettins, damage is constant
      for(int i=0; i<AMAXS; i++) damh[i] = SI.dampost(h->ewpn->size);
      }
    }
  
  struct cfrom {
    int from, wpn, next;
    int wounds;
    };
  
  struct woundrec {
    int code;
    int next;
    };

  // lcf[y]: i, index of the last comefrom for code y
  // cffrom[i]: previous code
  // cfwpn[i]:  weapon
  // ncf[i]:    previous cf
    
  int lcf[CODES];
  vector<cfrom> cf;
  
  void addEdge(int y, int hd, int sh, int w, int wnd) {
    int y2;
    if(encode(hd, sh, y2) > 0) {
      int i = isize(cf);
      cfrom c; c.from = y; c.wpn = w; c.next = lcf[y2]; c.wounds = wnd;
      cf.push_back(c);
      lcf[y2] = i;
      }
    }
  
  int wwnd[WMAX];
  int wweq[WMAX];
  vector<woundrec> wrec;

  void addWoundRec(int code, int neww, int gol, int time, int wpn) {

    int h1, s1, h2, s2;
    decode(h1, s1, code); decode(h2, s2, gol);
    
    if(wnd[code] > neww) {
      wnd[code] = neww;

      int i = isize(wrec);
      woundrec wr; wr.next = -1; wr.code = code;
      wrec.push_back(wr);
      
      if(wweq[neww] >= 0) wrec[wweq[neww]].next = i;
      else wwnd[neww] = i;
      
      wweq[neww] = i;
      }
    else {
      if(wnd[code] < neww) return;
      if(wtime[code] < time) return;
      if(wtime[code] == time && !wpnfirst(usew[code], wpn)) return;
      }

    wtime[code] = time;
    goal[code] = gol; 
    usew[code] = wpn;
    }
  
  void buildGraph() {
    for(int y=0; y<CODES; y++) for(int w=-2; w<P.arms; w++) if(w < 0 || wpn[w]) {
      int hd, sh;
      decode(hd, sh, y);
      
      // printf("y=%d w=%d [%d,%d]\n", y, w, hd, sh);
    
      if(sh > hd) continue;
    
      if(w >= 0) {

        weapon *W = wpn[w];
    
        if(W->msl()) continue;
        
        if(!calcWeaponEffect(W, h, hd, sh)) continue;

        if(hd == 0) {
          if(wnd[y] == 0 && wpnfirst(usew[y], w)) continue;
          addWoundRec(y, 0, 0, 1, w);
          }
        }

      if(w == -1 && !drainpower(h)) 
        continue;

      if(sh>hd) sh=hd;
      int hwound = hd-sh < AMAXS ? damh[hd-sh] : damh[AMAXS-1];

      if(w == -2) { if(!sh) continue; sh = 0; hwound = 0; }
      else hd += hwound * drainpower(h);

      // fprintf(, "%3d %3d (%d) %3d %3d\n", y%AMAX, y/AMAX, w, hd, sh);
      addEdge(y, hd, sh, w, hwound);
      }
    }
  
  void buildAmbiGraph() {

    int savehd = h->heads;
    int savest = h->sheads;
    int saveforce = h->stunforce;
    int saveset = P.ambiArm;
    cell& c(M[h->pos]);
  
    int max = 0;
    for(int i=0; i<P.arms; i++) 
      if(wpn[i] && wpn[i]->ambivalid())
        max |= 1<<i;
    
    for(int y=1; y<CODES; y++) 
    for(int w=max; w; w = (w-1) & max) 
    if(bitcount(w) > 1) {
      decode(h->heads, h->sheads, y);
      // int sah = h->heads, sash = h->sheads;
      if(h->sheads > h->heads) continue;
      P.ambiArm = w;
      if(ambiAttack(&c, 2)) {
        /* if(h->heads - h->sheads < 0) {
          printf("heads=%d sheads=%d\n", oheads, osheads);
          printf("y=%d heads=%d sheads=%d w=%d\n", y, h->heads, h->sheads, w);
          } */
        if(h->sheads > h->heads) h->sheads = h->heads;
        int hwound = h->heads-h->sheads < AMAXS ? damh[h->heads-h->sheads] : damh[AMAXS-1];

        if(h->heads) {
          h->heads += hwound * drainpower(h);
          }
        if(h->heads)
          addEdge(y, h->heads, h->sheads, w | AMBIWPN, hwound);
        else
          addWoundRec(y, 0, 0, 1, w | AMBIWPN);
        }
      }

    h->heads = savehd;
    h->sheads = savest;
    h->stunforce = saveforce;
    P.ambiArm = saveset;
    }
  
  void vampireUpdate() {
    for(int i=AMAXS-1; i>=0; i--) {
      int j = i + damh[i];
      if(j >= AMAXS) continue;
      damh[j] = damh[i];
      }
    }
  
  // this algorithm is extremely ugly
  void fromWhereTo(int pos) {
  
    int pos0 = pos;

    for(int p=0; p<HINF; p++) {
      
      int hd, sh; decode(hd, sh, pos = pos0);
      
      if(h->color == HC_GROW && hd > sh && hd <= 720) {
        if(p>=2) return;
        // p==0: used when it was better to grow
        // p==1: used when it was better to not grow
        int hdx = p?hd+1:hd-1;
        int pos2;
        // verify that it was indeed better to (not) grow
        if(encode(hdx, sh, pos2) && (p ? wnd[pos2] >= wnd[pos] : wnd[pos2] > wnd[pos]))
          continue;
        if(!p) hd=hdx, pos = pos2; // use hd/pos before growing
        if(p==1) p = HINF; // finish after p==1
        }
      else if(h->color == HC_WIZARD) {
        int pos2;
        if(p == 0) {
          // make sure that we could not gain by losing heads
          for(int hdx=sh+1; hdx<hd; hdx++) if(hdx >= hd*3/4) {
            if(encode(hdx, sh, pos2) && wnd[pos2] > wnd[pos])
              p = HINF;
            }
          }
        int hdx = hd+p;
        if(p == HINF || hd < hdx*3/4 || hdx >= AMAXS)
          break;
        hd = hdx;
        encode(hd, sh, pos2);
        if(p && wnd[pos2] >= wnd[pos]) {
          break;
          }
        pos = pos2;
        }
      else {
        p=HINF; // consider only one
        }

      // note: for HC_GROW pos is ungrown pos, even if the hydra will grow
      // this allows giveHint to check whether the hydra should grow or not
      
      int oldw = wnd[pos0];
      int newt = wtime[pos0]+1;
    
      int cfi = lcf[pos];
    
      while(cfi >= 0) {
        int wp = cf[cfi].wpn;
        int neww = oldw + cf[cfi].wounds;
        if(neww < WMAX) 
          addWoundRec(cf[cfi].from, neww, pos, newt, wp);
        cfi = cf[cfi].next;
        }
      }
    }
  
  void analyze() {
    setDamh();

    for(int i=0; i<CODES; i++) lcf[i] = -1;
    for(int i=0; i<WMAX; i++) wwnd[i] = -1, wweq[i] = -1;
    
    for(int y=0; y<CODES; y++) wnd[y] = WMAX, wtime[y] = TMAX;
  
    buildGraph();
    if(P.active[IT_PAMBI]) buildAmbiGraph();

    // update the damager array for vampires
    if(drainpower(h))  vampireUpdate();
    
    for(int wc=0; wc<WMAX; wc++) {
      int wap = wwnd[wc];     
      while(wap >= 0) {
        int pos = wrec[wap].code;
        wap = wrec[wap].next;

        if(wnd[pos] != wc) continue;
        fromWhereTo(pos);
        }
      }
    }
  
  };

void analyzeHydra(hydra *h) {
  hydraAnalyzer ha;
  ha.h = h;
  ha.analyze();
  }

void ambiAttackFull(cell *c, decomposer& D) {
  if(P.ambiArm & P.twinarms) {
    if(!twin) {
      addMessage("Your twin is not available.");
      return;
      }
    if(P.twinmode == 2) {
      addMessage("Your twin has already moved. Joint attack impossible.");
      return;
      }
    if(twindiff()) {
      addMessage("Cannot synchronize a joint attack!");
      return;
      }
    if(ambiAttack(c, 0)) {
      if(P.active[IT_PFAST]) {
        P.active[IT_PFAST]--;
        playSound("potion/potion-extreme-speed", 100, 0);

        P.twinspd--;
        }
      else moveHydras();
      D.reduce();
      stats.twinambij++;
      }
    }
  else if(ambiAttack(c, 0)) {
    cancelspeed();
    D.reduce();
    }
  }

int quakefun(int heads, int color) {
  int force = cinf[color].stunturns;

  return int((force*10 + 1500.0) * heads / log(heads+50) / 5.6);
  }

int bowpower(weapon *w) {
  int s = 1;
  int t = w->size * w->info().stunturns * w->info().stunturns;
  while(s*s < t) s++;
  return s;
  }

bool tryBow(int dir, bool justLine) {
  weapon* w = wpn[P.cArm];
  int thc = wpn[P.cArm]->size;
  int dist = 0;
  
  vec2 opos = wrap(playerpos);
  
  bool hadEffect = false;

  while(true) {
    dist++; vec2 apos = wrap(playerpos + dirs[dir] * dist);
    cell& xc(M[apos]);
    M[apos].ontarget = true;
    
    if(apos == opos) {
      if(justLine) return true;
      addMessage("You shoot yourself!");
      takeWounds(thc);
      w->addStat(WS_HKILL, thc, 0);
      return true;
      }
    if(apos == opos || xc.type == CT_WALL || !xc.seen) {
      if(!hadEffect && !justLine) addMessage("You don't see any target in this direction.");
      return hadEffect;
      }
    if(xc.mushrooms && !justLine) {
      if(!hadEffect) addMessage("You shoot an arrow at the mushroom!");
      playAttackSound(w, NULL);
      hadEffect = true;
      if(xc.mushrooms <= thc) {
        w->addStat(WS_MHEAD, xc.mushrooms, 0);
        xc.mushrooms = 0;
        w->addStat(WS_MKILL, 1, 0);
        }
      else {
        w->addStat(WS_MHEAD, thc, 0);
        xc.mushrooms -= thc;
        }
      }
    if(xc.h && !justLine) {
      if(xc.h->dodger()) {
        addMessage("The "+xc.h->name()+" is able to dodge arrows!");
        playSound("weapons/bowAttack-miss", sizeToVolume(thc), 0);
        }
      else {
        if(xc.h && xc.h->invisible()) continue;
        if(xc.h) xc.h->increaseAwareness();

        if(!hadEffect) addMessage("You shoot an arrow at the "+xc.h->name()+"!");
        if(xc.h == vorplast) vorpalClear();
        hadEffect = true;
        playAttackSound(w, NULL);
        
        int th = thc;
        if(th > xc.h->heads) th = xc.h->heads;

        if(xc.h->noregrow()) {
          if(xc.h->heads == th) {
            addMessage("The "+xc.h->name()+" is destroyed by your arrow!");
            w->addStat(WS_HKILL, th, 0);
            xc.hydraDead(NULL);
            continue;
            }
          else {
            xc.h->heads -= th;
            }
          }
        else {        
          xc.h->heads += th;
          xc.h->sheads += th * 2;
          xc.h->stunforce += bowpower(w);
          }
        if(xc.h->sheads > xc.h->heads) {
          xc.h->stunforce = (xc.h->stunforce * (long long) xc.h->heads) / xc.h->sheads;
          xc.h->sheads = xc.h->heads;
          }
        w->addStat(WS_HHEAD, th, 0);
        }
      }
    }
  }

bool tryLineAttack(int dir, bool doFire, bool justLine) {

  if(!wpn[P.cArm]) return false;
  weapon *wpnUsed = wpn[P.cArm];
  
  bool missile = wpnUsed->msl() && doFire;
  bool charge =  wpnUsed->cuts() &&
    P.active[IT_PCHRG] && !missile && !wpnUsed->axe();

  if(missile && wpnUsed->type == WT_BOW) {
    if(tryBow(dir, justLine)) {
      if(justLine) return true;
      wpnUsed->sc.sc[WS_USE]++;
      cancelspeed();
      }
    return true;
    }
  
  if(!missile && !charge) return false;
  
  int thc = wpnUsed->size;
  int dist = 0;
  
  if(wpnUsed->type == WT_STONE) thc *= thc;
  
  bool multi = charge || wpnUsed->type == WT_MSL;
  
  // we reuse dist to mark places already visited by the line attack
  for(int y=0; y<SY; y++) for(int x=0; x<SX; x++)
    M.m[y][x].dist = 0;
  M[playerpos].dist = 2;
  
  while(true) {
    dist++; vec2 apos = wrap(playerpos + dirs[dir] * dist);
    if(wpnUsed->type == WT_DISK) thc = wpnUsed->size * dist;
    cell& xc(M[apos]);
    M[apos].ontarget = true;
    if(dist == SX*SY*DIRS) return false; // looping charges are not allowed
    if(missile && xc.dist == 2) break; // OK, hit yourself
    if(xc.type == CT_WALL || (multi ? !xc.explored : !xc.seen)) {
      if(charge || justLine) return false;
      addMessage(thc > 0 ? 
        "You don't see enough heads in this direction to throw the "+wpnUsed->name()+"." :
        "Noooo, the "+wpnUsed->name()+" would fall on something else!"
        );
      return true;
      }
    if(xc.seen && xc.h && xc.h->dodger() && !charge && !justLine) {
      addMessage("The "+xc.h->name()+" is able to dodge missile weapons!");
      }
    else {
      if(xc.dist == 0) {
        int hd = xc.seen ? xc.headsAt(wpnUsed) : xc.mushrooms;
        if(xc.h && xc.h->invisible()) hd = 0; // invisible, so not counted
        thc -= hd;
        if(hd > 0 && thc > 0 && !multi) {
          if(!justLine)
            addMessage("Not enough heads there to throw the "+wpnUsed->name()+"!");
          return true;
          }
        xc.dist = 1;
        }
      }
    if(thc <= 0 && (charge || !xc.it)) break;
    }
  
  if(justLine) return true;

  if(charge)
    addMessage("You charge!");
  else
    addMessage("You throw the "+wpnUsed->name()+".");

  thc = wpnUsed->size;
  if(wpnUsed->type == WT_STONE) thc *= thc;
  
  int destroy = 0;
  int damage = 0;
  int musha = 0, beast = 0, mushb = 0;
  dist = 0;
  
  wpnUsed->addStat(WS_USE, 1, 0);

  while(true) {
    dist++; vec2 apos = wrap(playerpos + dirs[dir] * dist);
    if(wpnUsed->type == WT_DISK) thc = wpnUsed->size * dist;
    cell& xc(M[apos]);
    int hc;
    if(thc)
    if((hc = xc.headsAt(wpnUsed)))
    if(!xc.h || !xc.h->dodger() || charge) {
      if(xc.h) { beast++; if(mushb) musha++; } else if(beast) mushb++; else musha++;
      if(hc <= thc) {
        destroy++;
        if(xc.h && !xc.seen) stats.invisible++;
        xc.attack(wpnUsed, hc, NULL), thc -= hc;
        if(thc == 0 && charge) {
          playerpos = playerpos + dirs[dir] * dist;
          break;
          }
        }
      else {
        damage++;
        if(xc.h) xc.h->increaseAwareness();
        xc.attack(wpnUsed, thc, NULL), thc = 0;
        if(charge) {
          if(xc.h) {
            stats.stunchrg += xc.h->heads - xc.h->sheads;
            xc.h->sheads = xc.h->heads;
            xc.h->stunforce += xc.h->sheads * 3 + 50;
            addMessage("The "+xc.h->name()+ " is stunned by your charge!");
            stunnedHydra = xc.h; stunnedColor = iinf[IT_PCHRG].color;
            }
          playerpos = playerpos + dirs[dir] * (dist-1);
          break;
          }
        }
      }
    if(missile && xc.dist == 2) {
      takeWounds(thc);
      if(thc) {
        addMessage("The "+wpnUsed->name()+" returns and hits you!"),
        stats.mslselfdam += thc, stats.mslself++;
        if(P.curHP <= 0) shareFixed("commited suicide with the "+wpnUsed->name());
        if(P.curHP <= 0) achievement("SUICIDE");
        }
      else {
        addMessage("The "+wpnUsed->name()+" lands gently back in your hand."),
        stats.mslgent++;
        achievement("BOOMERANG");
        }
      break;
      }
    if(thc == 0 && !xc.it) {
      if(missile) {
        xc.it = wpnUsed, wpn[P.cArm] = NULL;
        }
      break;
      }
    }
  
  if(missile) {
    if(beast > 1) stats.mixdkill++;
    stats.mshits += beast;
    if(damage && destroy) stats.mixkill++;
    if(beast && musha) stats.mixfunga++;
    if(beast && mushb) stats.mixfungb++;
    if(beast > stats.maxmsl) stats.maxmsl = beast;
    if(beast >= 11) achievement("LONGTHROW");
    if(beast >= 3) shareFixed("killed "+its(beast)+" enemies at once with the "+wpnUsed->name());
    }
  else {
    P.active[IT_PCHRG]--;
    playSound("potion/potion-weaponcharge-attack", 100);
    if(beast > stats.maxchrg) stats.maxchrg = beast;
    if(beast >= 11) achievement("LONGCHARGE");
    if(beast >= 3) shareFixed("charged at "+its(beast)+" enemies at once with the "+wpnUsed->name());
    }
  cancelspeed();
  return true;
  }

bool tryWandUse(weapon *w, int dir, bool justLine) {
  if(w->size == 0) {
    addMessage("Your "+wpn[P.cArm]->name()+" has no more power!");
    return false;
    }
  int mana = 1;
  if(P.race == R_TROLL) mana += 2;
  if(w->size < mana+2) {
    addMessage("Safety checks on the "+wpn[P.cArm]->name()+" prevent you from using it!");
    return false;
    }
  if(!justLine) {
    addMessage("You phase through a wall!");
    playSound("other/teleport", 100, 0);
    }
  vec2 pos = playerpos;
  while(true) {
    vec2 pos2 = pos + dirs[dir];
    if(!inlevel(wrap(pos2))) {
      if(P.geometry == 16)
        dir = (4+dir) & 7;
      else if(P.geometry == 8) {
        // vertical reflection
        if(havebit(1+2+128, dir) && !inlevel(wrap(pos+dirs[0])))
          dir = (4-dir) & 7;
        else if(havebit(8+16+32, dir) && !inlevel(wrap(pos+dirs[4])))
          dir = (4-dir) & 7;
        // horizontal reflection
        if(havebit(2+4+8, dir) && !inlevel(wrap(pos+dirs[2])))
          dir = (8-dir) & 7;
        else if(havebit(32+64+128, dir) && !inlevel(wrap(pos+dirs[6])))
          dir = (8-dir) & 7;
        }
      else if(DIRS == 6) {
        if(havebit(1+2+32, dir) ? !inlevel(wrap(pos+dirs[0])) : !inlevel(wrap(pos+dirs[3])))
          dir = (9-dir) % 6;
        if(havebit(2+4, dir) && !inlevel(wrap(pos+dirs[1])) && !inlevel(wrap(pos+dirs[2])))
          dir = (6-dir) % 6;
        if(havebit(16+32, dir) && !inlevel(wrap(pos+dirs[4])) && !inlevel(wrap(pos+dirs[5])))
          dir = (6-dir) % 6;
        }
      else if(DIRS == 4) {
        dir = (dir+2)&3;
        }
      }
    pos = pos2;
    cell& c(M[pos]);
    if(justLine) {
      M[pos2].ontarget = true;
      if(!c.explored) continue;
      }
    c.explored = true;
    if(c.h && c.h->isAncient()) {
      addMessage("You feel reflected by an ancient force!");
      playSound("other/reflectedByAncientForce", 100, 0);
      dir = (dir+4) & 7;
      continue;
      }
    if(c.type != CT_WALL) break;
    if(!justLine) w->addStat(WS_GROW, 1, 0);
    }
  if(justLine) return true;
  w->size-=mana;
  w->addStat(WS_USE, 1, 0);
  cell& c(M[pos]);
  if(c.mushrooms) {
    w->size -= 2;
    w->addStat(WS_MHEAD, c.mushrooms, 0);
    w->addStat(WS_MKILL, 1, 0);
    addMessage("A mushroom explodes around you!");
    c.mushrooms = 0;
    }
  if(c.h) {
    w->size -= 2;
    addMessage("Your materialize inside "+c.h->name()+"!");
    w->addStat(WS_HHEAD, c.h->heads, 0);
    w->addStat(WS_HKILL, 1, 0);
    c.hydraDead(NULL);
    achievement("TELEFRAG");
    }
  if(pos == playerpos) {
    addMessage("You seem to have returned to the same position!");
    }
  playerpos = pos;
  cancelspeed();
  return true;
  }

bool tryPoleAttack(int dir) {

  int ws = P.active[IT_PAMBI] ? (P.ambiArm & ~P.twinarms) : (1<<P.cArm);
  int wsize = 0;
  for(int i=0; i<P.arms; i++) if(havebit(ws, i)) {
    if(!wpn[i]->polewpn()) {
      addMessage("Select only pole weapons.");
      return false;
      }
    wsize += wpn[i]->size;
    }
  
  weapon* w = wpn[P.cArm];
  
  cell& c1(M[playerpos + dirs[dir]]);
  if(c1.type == CT_WALL) {
    addMessage("There is a wall there!");
    return false;
    }
  cell& c2(M[playerpos + dirs[dir]*2]);
  if(c2.h && c2.h->dodger()) {
    addMessage("The "+c2.h->name()+" is able to dodge pole weapons!");
    return false;
    }
  if(c1.headsAt(w)+c2.headsAt(w) < wsize) {
    addMessage("Not enough heads in this direction!");
    return false;
    }
  
  int thc = wsize;
  int hc = c1.headsAt(w);

  if(hc) for(int i=0; i<P.arms; i++) if(havebit(ws, i)) {
    int co = hc; if(co > wpn[i]->size) co = wpn[i]->size;
    if(co) c1.attack(wpn[i], co, NULL), thc -= co, hc -= co;
    }

  if(thc == 0) thc = wsize;
  hc = c2.headsAt(w);
  if(hc >= thc) for(int i=0; i<P.arms; i++) if(havebit(ws, i)) {
    int co = hc; if(co > wpn[i]->size) co = wpn[i]->size;
    if(co) c2.attack(wpn[i], co, NULL), thc -= co, hc -= co;
    }
  
  bool isambi = bitcount(ws) > 1;
  if(P.ambifresh == P.active[IT_PAMBI] && isambi)
    P.ambifresh--;
  for(int i=0; i<P.arms; i++) if(havebit(ws, i))
    wpn[i]->addStat(WS_USE, 1, 0);
  cancelspeed();
  return true;
  }

string weapon::describe() {
  string s;
  string dx;
  
  bool showmat = true;

  if(wpnflags & wfTrap) dx += 
    "You are not sure whether these traps have been placed by giants to scare the "
    "slayers away, or by the slayers to help with fighting hydras. "
    "Anyway, when a hydra (or you) steps into a trap, it is attacked with a specific "
    "powerful attack (or missed, if the attack cannot hurt the given hydra). "
    "Then, the trap disappears, whether it hits or misses. "
    "Hydras tend to intuitively avoid traps, unless they can smell their prey, and "
    "they are pursuing it.\n\n";

  if(!stuns() && !doubles() && !wand() && !orb() && color != HC_OBSID) dx += 
    "Hydras usually regrow heads after your attack; the number of heads "
    "regrown depends on the sword's color, and can be seen next to the "
    "hydra on the right side of the screen (for currently selected sword) "
    "or on the 'f' screen (for all types of swords). ";
    
  if(msl()) dx += 
      "There are two ways to use a missile weapon: 1) just select it and "
      "press one of the "+its(DIRS)+" directions to fire, 2) select it and press 't'"
      "(and then a direction). You can change which one you prefer by "
      "pressing 'T'.\n";
  
  if(stuns() || doubles() || axe() || type == WT_QUAKE)
    dx += 
      "This weapons stuns some heads of its victims, making them unable to attack or follow you. "
      "Stunned heads regain consciousness after time given next to the hydra on the right side "
      "of the screen. If a hydra is attacked by several stunning attacks, then all the heads "
      "awaken together, after time which is the average of the times for all the affected heads.";
  
  if(type == WT_SPEAR)
    dx += 
      "You can press 't' while a spear is selected in order to attack "
      "monsters who are two squares from you.";

  if(type == WT_BLADE) {
    if(size == 0)
      s = 
        "Your fingernail is not long enough yet to cut a head. "
        "You will need to enchant it further. Unless you want to "
        "scratch hydras to cause them to regrow heads.";
    else if(size == 1) {
      s = 
        "A dagger is enough to remove a single mushroom, or cut a single "
        "hydra's head.";
      if(color != HC_OBSID)
        s += " But you will need larger weapons in the future, since "
        "the hydras will probably regrow heads faster than you can "
        "cut them down with your dagger.";
      }
    else 
      s = "A magical sword is a hydra slayer's main weapon. This one can "
        "cut "+namenum(size)+" heads in a single attack. It cannot cut fewer, "
        "so you will need to use something else against smaller hydras.";
    }
  
  if(type == WT_BLUNT) {
    s = 
      "A good bludgeon cannot cut down hydra heads, but it can stun them for "
      "a long time, which is sometimes very helpful in the art of hydra "
      "slaying. On a good side, it never causes a hydra to regrow heads.";
    if(size > 1)
      s += " This one can stun "+namenum(size)+" heads in one hit (and no fewer).";
    }
  
  if(type == WT_DIV) {
    if(size == 0)
      s = "A useless creation of a really crazy hydra slayer. You feel "
        "annoyed by just looking at it. You feel someone is making fun "
        "of you and your profession.";
    else if(size == 1)
      s = 
        "This weapon does not look very useful for anything else than "
        "just annoying hydras.";
    else s = 
      "When hydras grow too big to be cut by standard magical swords, "
      "divisor swords come into play. This one can reduce the number of "
      "heads on a hydra "+namenum(size)+" times... and it has to be exactly "+
      namenum(size)+" times.";
    }
  
  if(type == WT_MSL) {
    s = 
      "Missile weapons work like the basic swords, except that they "
      "can be thrown at a distance.";
    if(size > 1)
      s += " This missile can cut "+its(size)+" heads, and they do not "
        "need to belong to one Hydra, it is enough if they are all on "
        "the same line. Most hydra slayers like to boast about how they "
        "have killed several hydras with a single missile throw, but "
        "feel a bit ashamed if some hydras are killed and others are "
        "just wounded.";
    }
  
  if(type == WT_SHLD) {
    if(size == 1)
      s = "This knuckle looks like a very small shield. Too small enough "
        "to provide any physical protection, but it probably protects "
        "you from some kind of attack.";
    else
      s = "Many hydra slayers carry shields for protection; if you carry "
        "this one, it will make "+namenum(size)+" attacking heads count as "
        "one unprotected. It also provides some extra elemental protection "
        "from attacks of one or two specific types of hydras. Shields also "
        "can be used to bash hydras to stun them, but most shields are not "
        "designed for that, so the stunning won't last as long as on "
        "a proper bludgeon.";
    }
  
  if(type == WT_FUNG) {
    return 
      "This ancient artifact of Demeter has a power over mushrooms: they will move out "
      "of your way, and while you are carrying it, occassionally "
      "mushroom spores will appear in your inventory. "
      "As for fighting Hydras, well, it seems really hard, "
      "so it is probably good for stunning. And some mushrooms will appear too.";
    }
  
  if(type == WT_PICK) {
    s =
      "This tool can be used as a typical hydra stunner, but it can be also "
      "used to crush walls and quickly crush mushrooms.";
    }

  if(type == WT_DANCE) {
    s =
      "This weapon is very similar to a normal cutter, but much more elegant. "
      "You think it was designed by the Muses themselves. "
      "When you wield it, you get a special feeling, which is very similar "
      "to the one you get after drinking a potion of Power Swipe.";
    }

  if(type == WT_VORP) {
    s =
      "One, two! One, two, four, eight, sixteen!\n"
      "Not only this is one of the few blades which can slay a hydra of any "
      "size, but it also makes hydra bleed in a very special way, providing "
      "a significant health bonus for those who slay huge hydras solely with "
      "a vorpal blade. Quantity of such additional health is based on the "
      "total number of non-stunned heads cut (including those regrown "
      "by the hydra during the battle), without using any other weapons, "
      "Powders of Growth, Potions of Power Swipe/Weapon Charge, or attacking "
      "another monster with a vorpal blade in the meantime. This power is of "
      "limited use, though: slaying two 100-headed hydras on the same level "
      "gives the same amount of health as slaying a 200-headed hydra, but "
      "much less than two times the bonus for a single 100-headed hydra. "
      "The power is regenerated between levels, although very slowly.";
    }

  if(type == WT_PREC) {
    s =
      "This weapon somehow seems a bit faster than a normal sword, and even "
      "a bit faster than a divisor sword. It is also a bit curved, which "
      "somehow reminds you of parentheses which you have seen "
      "during your mathematical training.";
    }

  if(type == WT_BOW) {
    s =
      "Centaurs put a lot of time into their archery training, and are "
      "very good at it. In hydra slaying, this skill is not as useful, "
      "since an arrow cannot cut off a hydra's head; it is able to "
      "knock off a head, but the hydra regrows another stunned one. "
      "A bow affects all mushrooms and hydras on the line. Bigger bows "
      "are able to affect several heads for each Hydra on the way, but "
      "the stunning time is reduced a bit.";
    
    if(color == 10) showmat = false;
    }
  
  if(type == WT_ROOT) {
    s = "A sword of a very strange twisted shape.";
    }

  if(type == WT_LOG) {
    string prong = "a big prong";
    if(size>2) prong = namenum(size-1) + " big prongs";
    s = "A sword of a very strange feature. It is split into "+its(size)+
      " equal parts by "+prong+"; each of the "+namenum(size)+" parts is split "
      "again into two parts by smaller prongs, et caetera. Somehow you feel "
      "that this is an overpowered artifact which cuts down hydras very "
      "fast... but... well... you remember the first rule of hydra slaying "
      "weapons, which holds for most swords, from basic ones through "
      "divisors to the Eradicator and other artifacts? "
      "If 10 heads remain after an attack, then hydras of only 10 smaller "
      "different sizes could be attacked by the same weapon...";
    }

  if(type == WT_DECO) {
    s = "This artifact is similar to a divisor sword, but a bit different; "
      "you feel there is some magic which makes it work a bit like many "
      "normal divisor swords at once. However, you feel that this magic is "
      "limited...\n"
      "Decomposer finds the smallest number that the number of heads of the given "
      "hydra is disible by (except 1), and works as such a divisor. After its last "
      "charge is drained, it permanently becomes the divisor which it was in its "
      "last attack. Does not work on primes.";
    }

  if(type == WT_QUAKE) {
    if(P.race == R_TROLL)
      s = 
        "You are lucky to have found the ancient Club of Hydra Quakes! "
        "Created a long time ago by the titan smiths, "
        "it has a power of causing earthquakes which stun all enemies "
        "in vicinity. This power is limited, but can be recharged using the "
        "Scrolls of the Big Stick.";
    else
      return 
        "This seems to be some artifact of the Titans. You are too "
        "weak to even pick it up.";
    }
  
  if(type == WT_PSLAY) {
    s = 
      "Any experienced hydra slayer knows that there are hydras with "
      "a nasty number of heads which cannot be affected by any divisor, or "
      "even by most of the legendary artifacts. The legendary Primeslayer "
      "is an artifact which has been designed precisely to fight such "
      "hydras; it has no power otherwise.";

    if(size)
    s += "\nIt has been additionally enchanted to also provide some "
      "stunning.";
    }
  
  if(type == WT_AXE) {
    s =
      "Axes are similar to swords, but much heavier. In terms of hydra "
      "slaying, this means that heads regrown after axe attacks "
      "are stunned for a short time. This time depends on the axe material "
      "and the proportion of heads cut to heads regrown, and is usually "
      "shorter than for weapons which only stun. "
      "Unfortunately, an axe cannot be used for charging or for cutting "
      "stunned heads.";
    }
  
  if(type == WT_SPEAR) {
    s =
      "A spear is a strange weapon for hydra slaying, since it "
      "only makes hydras grow. A hydra head attacked with a spear "
      "will grow into two heads, "
      "but these two heads will be stunned. "
      "Tridents and larger spear-like weapons do this thing to "
      "several heads at once. Spears are very long and can be "
      "used to attack hydras from a distance.";
    
    if(P.race == R_CENTAUR)
      s += "\nBut still, compared to bows, spears are quite useless...";
//    "most centaurs laugh of spear "
//    "users, since their bows are much more powerful, and it is impossible "
//    "to do a power swipe with a spear.";
    }
  
  if(type == WT_STONE) {
    s =
      "That thing is really beautiful, you know many noble ladies "
      "who would love to have something like this for decoration. "
      "However, as a simple hydra slayer, you don't really care about "
      "that. You know another use for it: stunning hydras from afar!\n"
      "There is one special thing about ranged stunning weapons: "
      "The number of stunned heads is not proportional to its size, "
      "but to its square.";
    }
  
  if(type == WT_DISK) {
    s =
      "This blade is not designed for melee attacks, but for throwing. "
      "It has different properties than thrown knives and shurikens, "
      "though. It is impossible to attack several enemies in a row. "
      "However, it has a special magical enchantment which makes it "
      "cut many more heads, precisely, the product of the "
      "chakram's size and the distance it travelled before hitting "
      "the hydra.";
    }
  
  if(type == WT_PHASE) {
    s =
      "This wand can be used to go through walls, but not by destroying "
      "them: you just magically appear on the other side of the wall. "
      "This 'phasing' works even if the wall is very wide, although you cannot "
      "escape the Hydras Pit, because its outer walls just reflect the "
      "phasers. "
      "Each phasing uses up one charge from the wand"
      " (or three for very large users, like titans)"
      ", but if you "
      "end up in a mushroom or a creature, two extra charges are extended "
      "to protect you from being killed as a result. Of course, the obstacle "
      "does not receive such protection and is destroyed. Really powerful "
      "hydras are protected from such an attack though.";
    
    showmat = false;
    }

  if(type == WT_ORB) {
    s =
      "This Orb can be used to repeatedly invoke the power of a given powder or "
      "potion. The downsides are that it can be only used a limited number of "
      "times, and that it is quite big, and thus it takes up a weapon slot.\n"
      "Orbs are activated by hitting a wall or hydra, or by pressing 't' (throw)."
      "Just as items, they cannot be picked up by Titans.";
    
    showmat = false;
    }

  if(type == WT_TIME) {
    s = 
      "This was actually intended to be a healing tool, not a weapon. "
      "By making a weapon that reverses the time around itself whenever it attacked, "
      "the priests of Chronos expected to be able to heal wounds by attacking "
      "with time daggers.\n"
      "Unfortunately, for some reason, this did not turn out to work as well "
      "as expected. "
      "Even if the wounds were healed, there were substantial negative "
      "side effects. "
      "Usually, the healer started to hate the victim. You have heard so many "
      "stories about these negative effects that you would never try this "
      "healing method "
      "yourself. The project was abandoned...\n"
      "Well, at least it should work pretty reliably against Hydras.";
    }
  
  if(type == WT_RAIN) {
    s = 
      "This wonderful artifact blade transmutes itself after each attack.";
    }
  
  if(type == WT_RAND) {
    if(P.flags & dfBackups) 
      s = "A very weird weapon, this one, as its effects are believed "
      "to be unpredictable.\n"
      "You never believed that something can be unpredictable. Whenever "
      "something was expected to be unpredictable, it worked for you "
      "in the best possible way, unless you did not care. You are such "
      "a lucky person.\n"
      "But now, your luck ends. This weapon does not work for you. At all.";
    else s = 
      "A very weird weapon, this one, as its effects are unpredictable. "
      "The hydra will have a random number of heads after the attack. It "
      "is popularly called the Mersenne "
      "Twister because the first testers believed that "
      "it has something to do with Mersenne numbers, but in fact, there is "
      "no direct relation.\n"
      "Only non-stunned heads are affected. The expected number of non-stunned "
      "heads after the attack is equal to the number before; the smallest "
      "possible is 0, and the largest is double the original head count. "
      "The more Scrolls of Big Sticks you use on the Twister, the better "
      "the distribution will be. Does not work against very large hydras.";
    }
  
  if(type == WT_COLL) {
    s =
      "This weird "+info().wname+" blade is a "
      "yet another approach to a weapon which could kill any hydra effectively. "
      "The smiths who created it were not sure whether they have succeeded, though...\n"
      "Against even-headed hydras, it "
      "works as a standard Bisector, "
      "not causing any heads to regrow. It is also able to kill all 1-headed "
      "hydras. Against other odd-headed "
      "hydras (but not zombies and mushrooms), "
      "it works by multiplying their head count by three, and growing "
      "one extra head.\n"

      "This blade may be overenchanted. One attack with a +3 blade will work as "
      "two attacks with the standard version, +7 blade will work as "
      "three, and +27 blade will work as four.\n";
    }

  if(type == WT_SUBD) {
    s =
      "A strange divisor sword which ignores one head on its target for some "
      "weird reason. Rumored to have been created by a smith who wanted "
      "to create an ultimate hydra slaying weapon, but his magic did not work "
      "and he got this defective design instead. He wanted to fix this "
      "defect, but he was visited by a hydra slayer, who bought it for a large "
      "sum of money.";
    }

  if(type == WT_QUI) {
    s =
      "This twisted divisor sword was created by a smith who was unhappy with "
      "the disadvantages of common divisor swords, and wanted to create "
      "something different. It only kills those heads that a typical /" 
      + its(size) + " divisor would leave. On the good side, "
      "it usually works with hydras whose head count is not divisible: "
      "the number of heads cut is rounded up in this case. "
      "However, for some reason, it is unable to round up 1/"+its(size)+" "
      "(the First Rule of Hydra Slaying is rumored to have something to do "
      "with that). Surprisingly, single-headed monsters and mushrooms are still "
      "killable.";
    }

  if(type == WT_SPEED) {
    s = 
      "At the first glance you thought that somebody had understood his mission "
      "to clean the dungeon literally... but no, this broom is not a typical "
      "cleaning implement, but rather a flying device, which allows its user "
      "to move quickly. Since flying in a dungeon is a bit dangerous, you "
      "lose some HP whenever you do this (more if you are a Titan).";
    }
  
  if(type == WT_GOLD) {
    if(color == 9)
      s = 
        "The smith who has created this beautiful golden sword has clearly "
        "been an artist. The ratio of the length of the beautifully "
        "decorated hilt to the length of the blade of pure shining gold "
        "is the same as the ratio of the length of the blade to the length "
        "of the whole sword. What is important from the hydra slayer's point "
        "of view is that also the ratio of the number of heads after "
        "an attack to the number of heads before an attack is roughly the "
        "same. Thus, it makes 1 head out of 2, 2 heads out of 3, 3 heads out "
        "of 5, 5 heads out of 8, 8 heads out of 13, and so on. Other head "
        "counts are decomposed as sums of distinct numbers in the sequence, "
        "for example 81 = 55 + 21 + 5 will become 50 = 34 + 13 + 3. The "
        "enchantment given by the Scroll of Big Stick gives lots of extra "
        "power with almost no cost, since it gives an optional power to "
        "do such cutting several times with one attack -- if only one attack "
        "is possible, it will attack just once.";
    else 
      s = 
        "Your transmutation magic has not only changed the material, but "
        "also the shape: now the blade and the hilt are replaced. The blade "
        "of pure shining silver can cut all hydras which could not be cut "
        "with the Golden Cutter, and the Scroll of Big Stick still gives "
        "additional power without big costs.";
    }
  
  if(showmat) return s + "\n" + info().wdesc + "\n" + dx;
  
  return s + "\n" + dx;
  }

void cancelVorpalOn(weapon *w) {
  if(w && w->type == WT_VORP) w->size = 1;
  }

// vorpal and rainbow blades have special effects on attack
void attackEffect(weapon *w, bool havedouble) {
  if(w && w->type == WT_VORP && !havedouble)
    w->size <<= 2;
  if(w && w->type == WT_RAIN)
    transmute(w);
  }

void cancelVorpal() {
  for(int i=0; i<P.arms; i++) if(!havebit(P.twinarms, i))
  if(wpn[i] && wpn[i]->type == WT_VORP) {
    wpn[i]->size++;
    wpn[i]->size >>= 1;
    }
  }

bool chooseAutoAttack(hydra *h) {

  // runes should be simply activated
  if(P.active[IT_RGROW] || P.active[IT_RDEAD] || P.active[IT_RCANC] ||
    P.active[IT_RSTUN] || P.active[IT_RFUNG] || P.active[IT_RCONF]) 
    return true;
  
  if(h->invisible()) 
    // do not try auto-attack
    return true;

  stats.autoatt++;

  if(h->dirty) {
    addMessage("Do this bloody job yourself, would you?");
    return false;
    }
  
  analyzeHydra(h);

  int spos; encode(h->heads, h->sheads, spos);
  
  if(h->heads < AMAXS && wnd[spos] < WMAX) {
    int u = usew[spos];
    if(u < 0) {
      addMessage("Would not it be better to just wait?");
      return false;
      }
    else if(u & AMBIWPN) {
      for(int i=0; i<10; i++) if(havebit(u, i) && !havebit(P.twinarms, i)) {
        P.cArm = i;
        P.ambiArm = u;
        return true;
        }
      addMessage("I think that the twin should attack now...");
      return false;
      }
    else {
      P.cArm = u; P.ambiArm = 1 << u;
      return true;
      }
    }

  for(int i=0; i<P.arms; i++) if(!wpn[i]) {
    P.cArm = i;
    return true;
    }

  addMessage("Solve this yourself, would you?");
  return false;
  }

void collatz(weapon *w, hydra *h) {
  long long hc = h->heads;
  long long sh = h->heads - h->sheads;
  string s = its(hc);
  bool next = false;

  int qty =
    w->size < 3 ? 1 :
    w->size < 7 ? 2 :
    w->size < 27 ? 3 :
    4;

  while(qty--) {
    if(hc == 1) { hc = 0, s += next ? "; kill" : " kill"; break; }

    if(next) s += "; ";

    if(hc&1) {
      if(h->color == HC_MUSH) { s += " (mushroom ignores)"; break; }
      if(h->zombie) { s += " (zombie ignores)"; break; }
      else hc = 3 * hc + 1, s += "*3+1=" + its(hc);
      }
    else hc /= 2, s += "/2=" + its(hc);
    if(!sh) sh = 0; else if(sh&1) sh = 3 * sh + 1; else sh /= 2;
    next = true;
    }
  addMessage("You slice the "+h->name()+"! " + s);
  if(hc >= COLLAPSE) {
    w->addStat(WS_HKILL, 1, 0);
    if(h->color != HC_MUSH)
    collapse(h, NULL);
    }
  else if(hc == 0) {
    w->addStat(WS_HKILL, 1, 0);
    if(h->color != HC_MUSH)
      M[h->pos].hydraDead(NULL);
    else
      h->heads = 0;
    }
  else {
    if(sh > hc) sh = hc;
    if(hc > h->heads)
      w->addStat(WS_GROW, hc-h->heads, 0);
    else if(hc < h->heads)
      w->addStat(WS_HHEAD, h->heads-hc, 0);
    h->heads = hc;
    h->sheads = hc - sh;
    }

  w->addStat(WS_USE, 1, 0);
  cancelspeed();
  }

void mersenneTwist(weapon *w, hydra *h) {

  if(P.flags & dfBackups) {
    addMessage("Your weapon does not work!");
    addMessage("You hear a twisted voice: 'be responsible for your actions!'");
    return;
    }

  if(w->size > 1) 
    analyzeHydra(h);
  
  int limit = h->heads - h->sheads;
  if(h->heads + limit >= AMAXS) limit = AMAXS-h->heads-1;
  
  if(h->heads == h->sheads) {
    addMessage("The "+w->name()+" twists only non-stunned heads!");
    return;
    }
  else if(limit <= 0) {
    addMessage("The "+h->name()+" is too huge for the "+w->name()+"...");
    return;
    }
  
  int bsm = WMAX + 10, bbi = WMAX + 10, vsm = 0, vbi = 0, spos;

  int vul = drainpower(h);
  if((P.phase+1) & ((1<<P.active[IT_PFAST])-1))
    vul = 0;

  SI.prepare(h->heads*2+1, h);
  
  for(int i=0; i<w->size; i++) {
    int v = 1 + hrand(limit);
    int hv = h->heads - v;
    
    encode(hv + vul * SI.dampost(hv-h->sheads), h->sheads, spos);
    if(wnd[spos] < bsm) bsm = wnd[spos], vsm = v;

    hv = h->heads + v;
    encode(hv + vul * SI.dampost(hv-h->sheads), h->sheads, spos);
    if(wnd[spos] < bbi) bbi = wnd[spos], vbi = v;
    }
  
  addMessage("Your weapon twists the "+h->name()+"!");
  
  if(hrand(vsm + vbi) < vbi) {
    h->heads -= vsm;
    w->addStat(h->color == HC_MUSH ? WS_MHEAD : WS_HHEAD, vsm, 0);
    }
  else {
    h->heads += vbi;
    if(h->color != HC_MUSH) w->addStat(WS_GROW, vsm, 0);
    }
  
  if(h->heads == 0) {
    if(h->color == HC_MUSH)
      w->addStat(WS_MKILL, 1, 0);
    else {
      w->addStat(WS_HKILL, 1, 0);
      M[h->pos].hydraDead(NULL);
      }
    }
    
  w->addStat(WS_USE, 1, 0);
  cancelspeed();
  }

void weaponMessage(weapon *w) {
  if(w->type == WT_ROOT) {
    if(P.race == R_ELF)
      addMessage("The famous Eradicator. From Elvish 'radix', which means 'root'.");
    else
      addMessage("You think that 'Eradicator' has something to do with roots in Latin.");
    }
  if(w->type == WT_LOG)
    addMessage("Arrgh, another weird overpowered weapon that almost never can be used...");
  if(w->type == WT_FUNG)
    addMessage("You feel like the Mushroom "+mushroomname(w->size)+"!");
  if(w->type == WT_DECO)
    addMessage("This feels like a divisor weapon, but you are not sure of the number...");
  if(w->type == WT_PSLAY)
    addMessage("You are somehow sure that this won't work against really big hydras.");
  if(w->type == WT_DANCE && !P.active[IT_PSWIP]) {
    if(P.race == R_ELF)
      addMessage("Your Elven skill of attacking multiple targets feels more precise.");
    else if(P.race == R_CENTAUR)
      addMessage("With this weapon, even a horse could dance after drinking!");
    else
      addMessage("You feel as if you have drunk the Potion of Power Swipe!");
    }
  if(w->type == WT_VORP) {
    addMessage("One, two! One, two! And through, and through!");
    }
  if(w->type == WT_PREC) {
    if(P.race == R_ELF)
      addMessage("You feel the special powers of this weapon are not accessible for Elves.");
    else
      addMessage("A strange thought crosses your mind... 'subtract first, divide later'.");
    }
  char typ = w->type;
  if(typ == WT_FUNG || typ == WT_DANCE || typ == WT_VORP || typ == WT_LOG || typ == WT_DECO ||
    typ == WT_PREC || typ == WT_PSLAY || typ == WT_PHASE || typ == WT_GOLD || typ == WT_SUBD ||
    typ == WT_SPEED || typ == WT_QUI || typ == WT_ORB)
    shareS("find", " the "+w->name());
  }
