// Hydra Slayer: math puzzle roguelike
// Copyright (C) 2010-2011 Zeno Rogue, see 'hydra.cpp' for details

// stats and the player struct (mainmenu.cpp)
//============================================

#include <stdint.h>
typedef bool bool_t;
//typedef time_t mytime_t; // won't run correctly after 2106, sorry
typedef uint32_t mytime_t; // won't run correctly after 2106, sorry

#define MAXMOT 32

#pragma pack(push,4)

struct wpnscore {
  int32_t sc[7];
  wpnscore() { for(int i=0; i<7; i++) sc[i] = 0; }
  };

struct challengedata {
  int32_t wounds[CLEVELS];      // wounds per level
  int32_t itcost[CLEVELS];      // item cost per level
  };

struct statstruct {
  mytime_t tstart;     // real time of game start
  int32_t wounds;      // total wounds
  int32_t turns;       // total turns

  int32_t maxambi;     // max hydra killed with ambi
  int32_t ambiwpn;     // ambidextrous single hits

  int32_t necro;       // total mushrooms turned to zombie heads

  int32_t maxkill;     // max hydras killed at once with a Power Swipe
  int32_t maxmsl;      // max hydras killed at once with a Missile
  int32_t maxchrg;     // max hydras killed at once with a Weapon Charge
  int32_t maxspeed;    // max speed at once
  int32_t maxseed;     // max seeds at once
  int32_t maxrune;     // max active runes at once

  int32_t mixfungb;    // how many mushroom before hydra
  int32_t mixfunga;    // how many mushroom after hydra
  int32_t mixkill;     // how many kill-wound mixes
  int32_t mixdkill;    // how many double kills
  int32_t mshits;      // target hits with missiles
  int32_t mslself;     // self hits
  int32_t mslselfdam;  // self hits (total damage)
  int32_t mslgent;     // self hits (0 damage)

  int32_t magegrow;    // heads grown magically
  int32_t mushgrow;    // mushrooms grown magically
  int32_t magestun;    // heads stunned magically
  int32_t awakenhd;    // heads awakened
  int32_t awakenhyd;   // hydras awakened
  int32_t stunchrg;    // heads stunned by weapon charge
  int32_t vampire;     // how many heads regrown by vampire hydras
  int32_t savecount;   // how many times was the game saved and loaded
  int32_t owncrush;    // how many hydras have been crushed by their own weight
  int32_t invisible;   // how many hydras have been killed blindly
  int32_t ettinkill;   // how many ettins killed
  int32_t vampikill;   // how many vampires killed
  int32_t ettinsave;   // how many ettins survived
  int32_t unhonor;     // unhonorable attacks against vampires

  int32_t ettinwnd;    // damage done by the ettins
  int32_t ancientwnd;  // damage done by the ancient hydra
  int32_t powerignore; // potions of power juice ignored
  int32_t forgegrow;   // how many point32_ts grown with ambi+reforge combination
  int32_t ambiscroll;  // how many times ambi+trans/grow
  int32_t ambiforge;   // how many times ambi+forge
  
  int32_t maxoftype[MAXMOT];// the biggest piece of equipment of each type used
  int32_t motwin   [MAXMOT];// the biggest piece of equipment of each type used (for winning)

  wpnscore ws  [MAXMOT];// wpnscores for each weapon type
  
  int32_t woundwin;    // wounds to win
  int32_t treasure;    // treasure used up to win
  int32_t armscore;    // penalty for extra arms
  int32_t bossinv;     // number of inventory items used during the boss fight
  int32_t hydrakill;   // the number of hydras killed
  mytime_t gamestart;  // time of game start
  mytime_t gameend;    // time of game end
  int32_t solved;      // levels solved
  int32_t endtype;     // 0 = dead, 1 = quit, 2 = escape
  int32_t emsave;      // emergency saves
  
  int32_t shieldprev;  // damage prevented by shields (size)
  int32_t shieldmatch; // damage prevented by shields (color)
  
  int32_t treasure2, woundwin2; // for true victory
  
  int32_t usedup[ITEMS], usedb[ITEMS]; // item stats
  
  int32_t whistSize;    // size of the weapon history
  int32_t itemNotFound; // items that have not been found

  int32_t armscore2;   // armscore for true victory

  int32_t turntwin;    // turns when both twins are controlled
  int32_t twinwounds;  // wounds between twins
  
  int32_t ancientwnd2; // damage done by the second ancient hydra
  
  int32_t twindef2;    // joint twin defense
  int32_t twindef1;    // single twin defense
  int32_t twinai;      // how many times did AI attack
  int32_t twinmy;      // how many times did attack himself
  int32_t twinambij;   // joint twin ambidextrous attacks
  
  int32_t shieldheal;  // amount of HP gained from shieldhealing
  
  int32_t monkeysave;
  int32_t monkeykill;
  int32_t wizardkill;
  int32_t ivykill;
  int32_t dragonkill; 
  
  int32_t ivygrow;
  int32_t wiztimes;
  int32_t wizport;
  int32_t wizout;
  
  int32_t shadowwounds;// wounds from shadow hydras
  int32_t shadowkill;
  
  int32_t broomhp;     // broom moves
  int32_t broomdist;   // broom extra distance
  
  int32_t vorpalhead;  // total heads from vorpal
  int32_t vorpalhp;    // HP gain from vorpal weaponry
  
  int64_t achiev;

  //int32_t res1;        // padding
  
  //int64_t achiev;
  //int64_t achiev;      // achievements
  //int64_t achiev2;     // achievements reserve (if >64 achievements)
  
  int32_t backupsaves;   // how many times saved the game
  int32_t backuploads;   // how many times loaded the game
  
  int32_t automush;      // mushroom heads removed by autoexplore
  int32_t automove;      // moves suggested
  int32_t autoatt;       // attacks suggested
  int32_t autokill;      // kills on auto-attack
  int32_t alienkill;     // alien hydra kills
  
  int32_t savecheck;     // saved consistency check value
  
  int32_t waitturns;     // turns of rest

  int32_t evolkill;      // kills of evolving Hydras
  int32_t reserve[26];   // space reserved for new stats in new versions of Hydra Slayer
  };

struct pstruct {
  int32_t saveformat;  
  int32_t version;
  
  bool_t vchanged; // version has been changed during game

  int32_t inv[ITEMS], active[ITEMS];
  
  int32_t phase;     // used when hasted
  int32_t curlevel;  // current level
  
  int32_t ambifresh; // is ambidexterity fresh?
  
  int32_t gameseed;   // seed for the game
  int32_t flags;      // binary flags, see df* constants below
  
  int32_t maxHP, curHP;

  int32_t arms, cArm;

  int32_t ambiArm; // mask of arms used for ambidextrous attack
  
  int32_t race;    // race
  
  bool_t manualfire;   // autofire mode
  bool_t quickmode;    // quick inventory mode
  bool_t simplehydras; // use simple hydra graphics
  bool_t altkeys;      // alternate keyboard layout
  
  int32_t stairqueue;  // queue on the stairs
  int32_t twinarms;    // arms of your brother
  int32_t twinmax;     // twin's max HP
  int32_t twincarm;    // twin's cArm
  int32_t twinmode;    // 0 (AI), 1 (#1), 2 (#2)
  int32_t twinseed, twinspd; // twin's potion state
  
  bool_t twinsNamed; // are twins named

  int32_t geometry;    // number of movement directions: 4, 6, or 8  
  int32_t vorpalc;     // heads cut by vorpal so far

  char orbcharge, res1, res2, res3; // how many times an Orb has been partially charged

  int32_t oldversion;  // version when the game was started
  int32_t timemushlimit; // limit on the usage of time-blade against mushrooms
  int32_t reserve[5];  // reserved for future
  };
#pragma pack(pop)

#define dfDebug            1 // debugging mode
#define dfSeeAll           2 // see all cheat
#define dfAutoAttack       4 // auto attack mode
#define dfBackups          8 // backups allowed
#define dfTutorial        16 // we are playing the tutorial
#define dfAutoON          32 // auto attack mode is actually on
#define dfShadowAware     64 // are we aware of the shadow hydra
#define dfCleanup        128 // not really killing hydras
#define dfChallenge      256 // challenge game
#define dfRaceSeeded     512 // race/geometry depending on the set
#define dfDaily         1024 // daily challenge
#define dfFree          2048 // a free version has been used
#define dfSteam         4096 // a Steam version has been used
#define dfSaveEdit      8192 // savefile tampering detected
#define dfMemEdit      16384 // memory tampering detected
#define dfConsist      32768 // consistency check
#define dfDailyAgain   65536 // daily again
#define dfScoreFromSteam 131072 // score received from Steam


// the game is not valid for (normal) scoring if one of these is activated
#define dfsInvalid     (1|16|128|256)

struct playerinfo {
  statstruct stats;
  pstruct player;
  string charname;
  string username;
  string twin[2];                  // twin names
  bool curgame;                    // this is the current game
  vector<struct weapon*> whist;    // weapon history
  vector<int32_t> whistAt;         // weapon history: where was it dropped
  vector<struct weapon*> trollwpn; // troll weaponry
  vector<int32_t> trollkey;        // troll weapon hotkeys
  bool reserve;                    // more reserve
  challengedata cdata;             // data for the challenge
  };

playerinfo pinfo;
bool fixedseed = false;

// abbreviations
statstruct& stats(pinfo.stats);
static pstruct& P(pinfo.player);

bool debugon()    { return P.flags & dfDebug; }
bool seeallmode() { return P.flags & dfSeeAll; }

// main (hydra.cpp)
//==================

struct sclass {
  virtual int sct() { return 0; }
  void put();
  void putOn(vec2 v);  
  virtual string name() { return "sclass"; }
  virtual string fullname() { return name(); }
  virtual void csave();
  virtual void cload() = 0;
  virtual struct item* asItem() { return NULL; }
  virtual struct weapon* asWpn() { return NULL; }
  virtual struct hydra* asHydra() { return NULL; }
  virtual string describe() { return "sclass"; }
  virtual int gcolor() { return 8; }
  virtual int icon() { return '?'; }
  virtual int iconExtended() { return icon(); }
  virtual void draw() { 
    col(gcolor()); 
    addch(icon());
    }
  virtual string getname() { return "the " + name(); }
  virtual ~sclass() { }
  };

#define SCT_ITEM 1
#define SCT_WPN  2
#define SCT_HYDRA 3

struct item : sclass {
  int32_t type;
  int sct() { return SCT_ITEM; }
  virtual struct item* asItem() { return this; }
  item(int _t) { type = _t; }
  int gcolor() { return iinf[type].color; }
  string name() { return iinf[type].name; }
  string getname();
  void draw() { col(gcolor()); addch(icon()); }
  int icon() { return iinf[type].icon; }
  void csave();
  void cload();
  string describe();
  };

void twinswap();

// weapons (weapons.cpp)
//=======================

#define WT_BLADE '-'
#define WT_BLUNT 'S'
#define WT_DIV   '/'
#define WT_MSL   'M'
#define WT_SHLD  'P'
#define WT_ROOT  'R'
#define WT_FUNG  'F'
#define WT_PICK  'W'
#define WT_DANCE 'D'
#define WT_VORP  'V'
#define WT_DECO  '\\'
#define WT_LOG   'L'
#define WT_QUAKE 'Q'
#define WT_BOW   'B'
#define WT_PREC  '('
#define WT_PSLAY 'p'
#define WT_AXE   'A'
#define WT_STONE 'K'
#define WT_DISK  'C'
#define WT_SPEAR 'I'
#define WT_PHASE 'T'
#define WT_GOLD  'G'
#define WT_SUBD  'O'
#define WT_SPEED '!'
#define WT_TIME  '+'
#define WT_RAIN  'X'
#define WT_RAND  '?'
#define WT_QUI   ':'
#define WT_ORB   'i'
#define WT_COLL  'c'

#define WS_USE   0
#define WS_HHEAD 1
#define WS_HKILL 2
#define WS_GROW  3
#define WS_MHEAD 4
#define WS_MKILL 5
#define WS_HSTUN 6

#define MOT 32
#define MOT_BLADE  0
#define MOT_OBSID  1
#define MOT_BLUNT  2
#define MOT_DIV    3
#define MOT_ROOT   4
#define MOT_MSL    5
#define MOT_SHLD   6
#define MOT_ZOMBIE 7
#define MOT_ETTIN  8
#define MOT_HYDRA  9
#define MOT_AMBI   10
#define MOT_BARE   11
#define MOT_DECAP  12
#define MOT_DECO   13
#define MOT_LOG    14
#define MOT_PICK   15
#define MOT_PSLAY  16
#define MOT_BOW    17
#define MOT_VORPAL 18
#define MOT_AXE    19
#define MOT_STONE  20
#define MOT_DISK   21
#define MOT_SPEAR  22
#define MOT_PHASE  23
#define MOT_GOLD   24
#define MOT_SILVER 25
#define MOT_SUBDIV 26
#define MOT_RANDOM 27
#define MOT_TIME   28
#define MOT_RAIN   29
#define MOT_TRAP   30
#define MOT_COLL   31

string typenames[MOT] = { 
  "normal blade", "meteorite blade", "blunt weapon", "divisor", "eradicator", "missile", "shield",
  "zombie", "giant", "non-zombie hydras", "ambidexterity", "bare hands", "decapitation",
  "decomposer", "logblade", "pickaxe", "primeslayer", "bow", "vorpal", 
  "axe", "stone", "blade disk", "spear", "phasewall",
  "golden sector", "silver sector", "sub-divisor",
  "Mersenne twister", "timeblade", "rainbow blade", "trap", "Sword of Collatz"
  };

#define wfTrap 1

struct weapon : sclass {
  int32_t color;
  int32_t size;
  char type;
  
  int32_t level; // where it was found
  int32_t ocolor, osize; // original data
  
  wpnscore sc;  // weaponscore
  int wpnflags; // weapon flags, such as 'trap'
  
  string name();
  string fullname();
  int sct() { return SCT_WPN; }
  virtual struct weapon* asWpn() { return this; }

  weapon(int _c, int _s, int _t) { 
    color = ocolor = _c; size = osize = _s; type = _t; level = P.curlevel;
    wpnflags = 0;
    }

  int getMot() {
    if((type == WT_BLADE || type == WT_DANCE) && color != HC_OBSID)
      return MOT_BLADE;
    if(type == WT_VORP) return MOT_VORPAL;
    else if(type == WT_BLADE) return MOT_OBSID;
    else if(type == WT_BLUNT || type == WT_FUNG) return MOT_BLUNT;
    else if(type == WT_MSL) return MOT_MSL;
    else if(type == WT_SHLD) return MOT_SHLD;
    else if(type == WT_DIV) return MOT_DIV;
    else if(type == WT_ROOT) return MOT_ROOT;
    else if(type == WT_DECO) return MOT_DECO;
    else if(type == WT_LOG) return MOT_LOG;
    else if(type == WT_PICK) return MOT_PICK;
    else if(type == WT_QUAKE) return MOT_PICK;
    else if(type == WT_BOW) return MOT_BOW;
    else if(type == WT_PSLAY) return MOT_PSLAY;
    else if(type == WT_STONE) return MOT_STONE;
    else if(type == WT_AXE) return MOT_AXE;
    else if(type == WT_DISK) return MOT_DISK;
    else if(type == WT_SPEAR) return MOT_SPEAR;
    else if(type == WT_PHASE) return MOT_PHASE;
    else if(type == WT_GOLD && color == 9) return MOT_GOLD;
    else if(type == WT_GOLD) return MOT_SILVER;
    else if(type == WT_COLL) return MOT_COLL;
    else if(type == WT_SUBD) return MOT_SUBDIV;
    else if(type == WT_QUI) return MOT_SUBDIV;
    else if(type == WT_RAND) return MOT_RANDOM;
    else if(type == WT_TIME) return MOT_TIME;
    else if(type == WT_RAIN) return MOT_RAIN;
    return MOT_BLUNT;
    }

  bool protectAgainst(hydra *h); // for shields

  void addStat(int type, int qty, int special) {
    int mot = special ? special : getMot();
    stats.maxoftype[mot] = max(stats.maxoftype[mot], size);
    sc.sc[type] += qty;
    stats.ws[mot].sc[type] += qty;
    }

  colorInfo& info() { return cinf[color]; }
  int gcolor() { return info().color; }
  string describe();

  int icon() {
    if(wpnflags & wfTrap) return ('^');
    if(type == WT_BLADE) return ('(');
    else if(type == WT_DANCE) return ('{');
    else if(type == WT_TIME) return ('{');
    else if(type == WT_RAIN) return ('{');
    else if(type == WT_VORP)  return ('{');
    else if(type == WT_BLUNT) return (')');
    else if(type == WT_FUNG) return ('|');
    else if(type == WT_PICK) return ('y');
    else if(type == WT_MSL) return ('*');
    else if(type == WT_SHLD) return (']');
    else if(type == WT_ROOT) return squareRootSign();
    else if(type == WT_DECO) return ('\\');
    else if(type == WT_SUBD) return ('\\');
    else if(type == WT_GOLD) return ('\\');
    else if(type == WT_COLL) return ('\\');
    else if(type == WT_PSLAY) return ('\\');
    else if(type == WT_QUAKE) return ('&');
    else if(type == WT_RAND) return ('&');
    else if(type == WT_LOG) return ('}');
    else if(type == WT_PREC) return ('[');
    else if(type == WT_BOW) return ('}');
    else if(type == WT_STONE || type == WT_DISK) return ('o');
    else if(type == WT_SPEAR) return ('+');
    else if(type == WT_AXE) return (';');
    else if(type == WT_QUI) return ('}');
    else if(type == WT_PHASE || type == WT_SPEED || type == WT_ORB) return ('-');
    else return ('/');
    }

  int iconExtended() {
    if(wpnflags & wfTrap) {
      if(type == WT_BLADE) return 'g';
      if(type == WT_DIV) return 'h';
      if(type == WT_BLUNT) return 'i';
      }
    if(type == WT_BLADE && size == 2) return 'a';
    if(type == WT_BLADE && size == 1) return 'b';
    if(type == WT_DIV   && size == 0) return 'c';
    if(type == WT_VORP  && size >= 0) return 'd';
    if(type == WT_BLADE && size >= 100) return 'e';
    if(type == WT_GOLD || type == WT_COLL) return 'f';
    return icon();
    }
  
  weapon *asTrap() { wpnflags |= wfTrap; return this; }
    
  void grow();
  
  bool axe() { 
    return 
      type == WT_AXE;
    }
  bool doubles() {
    return
      type == WT_BOW || type == WT_SPEAR || type == WT_PICK;
    }
  bool wand() {
    return
      type == WT_PHASE;
    }
  bool orb() {
    return
      type == WT_ORB;
    }
  bool cuts() { 
    return 
      type == WT_BLADE || type == WT_MSL || type == WT_DISK ||
      type == WT_DANCE || type == WT_PREC || type == WT_VORP ||
      type == WT_AXE || type == WT_RAIN;
    }
  bool xcuts() { 
    return type == WT_DIV || type == WT_ROOT || 
      type == WT_LOG || type == WT_DECO || type == WT_PSLAY ||
      type == WT_GOLD || type == WT_SUBD || type == WT_QUI;
    }
  bool stuns() {
    return type == WT_BLUNT || type == WT_SHLD || 
      type == WT_FUNG || type == WT_STONE || type == WT_SPEED;
    }
  bool activeonly() {
    return stuns() || type == WT_AXE;
    }
  bool msl() { return type == WT_MSL || type == WT_BOW || type == WT_DISK || type == WT_STONE; }
  bool csd() { return cuts() || stuns() || doubles(); }
  bool csdx() { return (cuts() || stuns() || doubles() || xcuts()); }
  bool swipable() { return type != WT_SPEAR; }
  bool polewpn() { return type == WT_SPEAR; }
  
  bool ambivalid() {
    if(type == WT_RAND || type == WT_TIME || type == WT_COLL) return false;
    if(msl() || type == WT_QUAKE || type == WT_DECO) return false;
    if(xcuts() && type != WT_DIV && type != WT_ROOT) return false;
    if(xcuts() && size == 0) return false;
    return true;
    }
  
  // mode==true: do not require divisibility/rootability, to cut stunned heads correctly
  int cutoff(int heads, bool mode);
  
  weapon *reduce();
  void csave();
  void cload();
  };

void playSwitchSound(weapon *w);

weapon* wpn[MAXARMS];

// for analyzehydra

/*#ifdef ANDROID
#define AMAX    64
#define AMAXS   3000
#define WMAX    50
#define TMAX    100
#else */
#define AMAX    128
#define AMAXS   20100
#define WMAX    1000
#define TMAX    2000
// #endif
#define AMBIWPN (1<<14)
#define CODES (AMAXS+AMAX*AMAX)

#define COLLAPSE 1000000

#define TIME_MUSH_LIMIT 10

int usew[CODES], goal[CODES], wnd[CODES], wtime[CODES];

int wpnset = 1; // last weaponset analyzed

int vorpalhsf;       // non-stunned heads cut from the current hydra
hydra *vorplast;     // last hydra attacked by a vorpal sword
void cancelVorpal();
void attackEffect(weapon *w, bool havedouble);
void vorpalRegenerate();

// monsters (from monster.cpp)
//============================

int randRegrow(int maxval, bool strong) {
  int glim = hrand(maxval);
  if(strong) glim = (glim + maxval) / 2;
  int maxlim = 13 + hrand(4);
  if(glim <= maxlim) return glim;
  return maxlim;
  }

string twinName(bool tw=true, int mode=0);

#define IS_DIRTY (1<<HCOLORS)

int32_t lastuid;

struct hydra : sclass {
  int32_t uid;
  int32_t color;
  int32_t heads, sheads, stunforce, heal;
  int32_t res[COLORS];
  bool_t conflict; // is conflicted
  bool_t zombie;   // is a friendly zombie
  bool_t ambi_OUT; // is it affected by ambidexterity [obsolete]
  weapon *ewpn;  // weapon for an Ettin
  vec2 pos;
  int lastwpnset;// last weapon set analyzed
  int *swnd;     // saved 'wnd' array
  int32_t dirty;     // set of which colors are unknown, plus IS_DIRTY if the main color is unknown
  int32_t awareness;
  int gfxid;     // graphical ID
  void increaseAwareness() { if(awareness < 1000) awareness++; }

  void setgfxid();  // set gfxid for this hydra
  void freegfxid(); // free gfxid for this hydra
  
  string name() { 
    if(color == HC_SHADOW)
      return "shadow hydra";
    if(color == HC_MUSH)
      return "mushroom";
    if(color == HC_ETTIN) 
      return 
        heads == 1 ? "giant" :
        heads == 50 ? "hecatoncheire" :
        namenum(heads)+"-headed giant";
    if(color == HC_MONKEY)
      return namenum(heads)+"-headed monkey";
    if(color == HC_TWIN) return twinName(true, 0);
    return its(heads)+"-headed " + 
      ((dirty & IS_DIRTY) ? "bloody " : info().hname)+(
        (color & HC_DRAGON) ? "dragon":
        zombie ? "zombie":
        "hydra"
        );
    }

  string shortname() {
    if(color == HC_TWIN)
      return twinName(true, 2)+" "+its(heads)+"/"+its(P.twinmax);
    if(color == HC_SHADOW) return info().hname;
    return its(heads)+(color & HC_DRAGON ? "!" : "-")+((dirty&IS_DIRTY) ? "bloody" : info().hname);
    }

  int sct() { return SCT_HYDRA; }

  virtual struct hydra* asHydra() { return this; }

  hydra(int _c, int _heads, int _glim, int _heal) { 
    uid = lastuid++;
    heal = _heal; awareness = 0; gfxid = 0;
    color = _c; heads = _heads; sheads = 0; stunforce = 0;
    zombie = false; conflict = false;  ambi_OUT = false; ewpn = NULL;
    for(int i=0; i<COLORS; i++)
      res[i] = 
        i == color ? -2 : 
        i < HCOLORS ? randRegrow(_glim, i==info().strength) : 
        0;
    if(info().suscept >= 0) 
      res[info().suscept] = 0;
    swnd = NULL; lastwpnset = -1;
    dirty = 0;
    }
  
  void clearswnd() { lastwpnset = -1; if(swnd) delete swnd; swnd = NULL; }

  hydraInfo& info() { 
    if(color == HC_TWIN) return hyinf[HC_TWIN_R];
    return hyinf[color & HC_DRMASK];
    }
  bool isAncient() { return (color & HC_DRMASK) == HC_ANCIENT; }
  bool lowhead() { return color == HC_ETTIN || color == HC_MONKEY; }
  bool noregrow() { return zombie || lowhead(); }
  bool dodger() { return lowhead() && heads > sheads; }
  bool visible() { return color != HC_SHADOW; }
  bool invisible() { return color == HC_SHADOW; }
  bool aware();

  bool isTwin() { return color == HC_TWIN; }
  int gcolor() { return (dirty & IS_DIRTY ? 4 : info().color); }

  int icon() {
    if(color == HC_TWIN) return '@';
    else if(color == HC_SHADOW) return ' ';
    else return hydraicon(heads);
    }

  int power() { return powerf(heads-sheads); }

  void cutStunHeads() {
    stunforce = (heads * (long long) stunforce) / sheads;
    sheads = heads;
    }    

  void csave();
  void cload();
  
  string describe();
  virtual ~hydra() { clearswnd(); }
  };

vector<hydra*>  hydras;

struct shieldinfo {
  vector<int> ehcnt;   // effective head count
  vector<int> shused;
  int maxss;           // max shield used
  int minss;           // min shield used
  int reduce;          // reduction due to shield color

  void prepare(int maxsh, hydra *h, bool shu = false);
  int ehcntx(int q);
  int dampre(int q);
  int dampost(int q);
  int usize(int size);
  };

shieldinfo SI;

bool nagavulnerable; // naga is after its first move
// bool shadowAware;    // are we aware of existence of a shadow hydra
int shadowwarning;   // switch colors on shadow

vector<hydra*> stairqueue;
hydra *twin;

void hydraAttackPlayer(hydra* H, bool brother = false);
void moveHydra(hydra* H);
void moveHydras();
void popStairQueue();
bool canGoDown();
bool isFleeing(hydra *H);
void tryStealing(hydra *H, bool postmove);

// level (level.cpp)
//===================

#define CT_EMPTY 0
#define CT_WALL 1
#define CT_STAIRUP 2
#define CT_STAIRDOWN 3
#define CT_HEXOUT 4

#ifdef NOTEYE
struct animinfo;
#endif

struct cell {
  int type;
  int mushrooms;
  int dead;
  sclass *it;
  hydra  *h;
  bool seen;
  bool explored;
  bool ontarget;
  // vec2 pos;
  int dist;   // distance from the player
  signed char govia; // direction the player should take to go there
  bool isEmpty() { return !type && !mushrooms && !it && !h; }
  bool isPassable(bool canCutMushrooms = false);
#ifdef NOTEYE
  vector<animinfo> animations;
#endif
  void clear() { 
    type = CT_EMPTY; mushrooms = 0; dead = 0; 
    if(it) {
      weapon* wp = it->asWpn();
      if(it->asItem()) {
        if(P.race == R_TROLL)
          P.inv[it->asItem()->type]++;
        else 
          stats.itemNotFound++;
        }
      if(wp && (wp->sc.sc[WS_USE] || wp->sc.sc[WS_MKILL])) {
        pinfo.whistAt.push_back(P.curlevel-1);
        pinfo.whist.push_back(wp);
        }
      else delete it;
      it = NULL;
      }
    if(h) delete h; h = NULL;
    explored = false; seen = false;
    }
  int headsAt(weapon *w) { 
    if(w->activeonly()) {
      if(h) return h->heads - h->sheads;
      return 0;
      }
    if(h) return h->heads;
    return mushrooms;
    }
  weapon *trapped() { if(!it) return NULL; weapon *w = it->asWpn(); if(w && (w->wpnflags & wfTrap)) return w; return NULL; }
  void attack(weapon* x, int power, sclass *who); // who = who attacks
  void hydraDead(hydra *killer);
  };

vec2 wrap(vec2 v); bool inbound(vec2 v);

struct levelmap {
  cell m[MSY][MSX];
  cell out;
  cell& operator [](const vec2& v) { vec2 w = wrap(v); if(!inbound(w)) return out; return m[w.y][w.x]; }
  };

levelmap M;

vec2 playerpos;
vec2 stairpos;

bool cell::isPassable(bool ccm) {
  return 
    (type != CT_WALL) && 
    (ccm | !mushrooms) && 
    !h && (this != &M[playerpos])
    ;
  }

int targetdir; // direction suggested by BFS

// who==0: hydras
// who==1: us
// who==2: twin

void bfs(int who, bool nmush = false);

void generateTutorialLevel();

// user interface (ui.cpp)
//========================

hydra *stunnedHydra; // stunned Hydra, to animate stun
int stunnedColor;    // color of stun-stars
int animframe;       // animation frame
bool exploreOn;      // is the auto-explore currently switched on
bool exploreWithDestination; // is the auto-explore trying to reach some destination
vec2 exploreDestination;     // .. and this destination



vector<string> msgs;
void addMessage(string s);
void viewMultiLine(string s, int& cy, int narrow = 0);
void viewHelp();
int ghch(int context);
bool yesno(int context);
void drawScreen();
void giveHint(hydra *h);

// saves
void emSaveGame();

#ifndef DBASE
#define DBASE (512*6)

#define D_RIGHT DBASE+0
#define D_UP    DBASE+2
#define D_LEFT  DBASE+4
#define D_DOWN  DBASE+6
#define D_PGUP  DBASE+1
#define D_PGDN  DBASE+7
#define D_HOME  DBASE+3
#define D_END   DBASE+5 
#define D_CTR   DBASE+8
#endif

#define ESC     27
#define PANIC   -3

#define INDB   (512*7) // for internal use: INDB+x is direction x


// interface contexts:

bool quitgame = false;  // should we quit

#define IC_GAME       0
#define IC_GAMETWIN   1 // just like GAME, but playing twins
#define IC_YESNO      2 // yes/no question
#define IC_ASKDIR     3 // asking for direction

#define IC_QUIT      16 // quitting menu
#define IC_RACE      17 // race selection
#define IC_INV       18 // inventory
#define IC_TROLL     19 // troll inventory
#define IC_EDIT      20 // editString
#define IC_HALL      21 // hall of fame
#define IC_VIEWDESC  22 // view description
#define IC_HELP      23 // help
#define IC_FULLINFO  24 // fullinfo
#define IC_CALL      25 // call debug
#define IC_MYESNO    26 // yes/no question in menu
#define IC_CHEATMENU 27 // cheat menu
#define IC_VIEWACH   28 // view achievements
#define IC_CHALLENGE 29 // challenge menu

bool gameExists;        // has the game been generated?
