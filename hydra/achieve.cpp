// Hydra Slayer: math puzzle roguelike
// Copyright (C) 2010-2012 Zeno Rogue, see 'hydra.cpp' for details

// achievements (achieve.cpp)
//============================================

#define NUMACH 57

struct achievementdata {
  const char *textcode;
  const char *hname;
  const char *desc;
  int score;
  };

struct achievementdata ach[NUMACH] = {
  {"BEGINNER", "Basics of Hydra Slaying", "Kill all Hydras on Level 1. ", 3}, 
  {"ADEPTSLAYER", "Adept Hydra Slayer", "Kill all Hydras on Level 5", 5}, 
  {"BEGINNERPERFECT", "Perfect Start", "Kill all Hydras on Level 1 without taking any wounds.", 5}, 
  {"DEEPEXPLORER", "Explorer", "Conquer 100 levels. ", 10}, 
  {"VDEEPEXPLORER", "Explorer II", "Conquer 150 levels. ", 10}, 
  {"BRONZENAGA", "Echidna Bronze Medal", "Defeat the first boss with an Echidna.", 10}, 
  {"BRONZEHUMAN", "Human Bronze Medal", "Defeat the first boss with a Human.", 10}, 
  {"BRONZECEN", "Centaur Bronze Medal", "Defeat the first boss with a Centaur.", 10}, 
  {"BRONZETROLL", "Titan Bronze Medal", "Defeat the first boss with a Titan.", 10}, 
  {"BRONZETWIN", "Twin Bronze Medal", "Defeat the first boss with the Twins.", 10}, 
  {"SILVERHUMAN", "Human Silver Medal", "Defeat the second boss with a Human.", 10}, 
  {"SILVERNAGA", "Echidna Silver Medal", "Defeat the second boss with an Echidna.", 10}, 
  {"SILVERCEN", "Centaur Silver Medal", "Defeat the second boss with a Centaur.", 10}, 
  {"SILVERTROLL", "Titanic Silver Medal", "Defeat the second boss with a Titan.", 10}, 
  {"SILVERTWIN", "Twin Silver Medal", "Defeat the second boss with the Twins.", 10}, 
  {"GOLDHUMAN", "Human Gold Medal", "Defeat the second boss with a Human, using at most one Potion of Life.", 10}, 
  {"GOLDCEN", "Centaur Gold Medal", "Defeat the second boss as a Centaur, using at most one Potion of Life.", 10}, 
  {"GOLDNAGA", "Echidna Gold Medal", "Defeat the second boss as an Echidna, using at most one Potion of Life.", 10}, 
  {"GOLDTROLL", "Titanic Gold Medal", "Defeat the second boss as a Titan, using at most one Potion of Life.", 10}, 
  {"GOLDTWIN", "Twin Gold Medal", "Defeat the second boss as the Twins, using at most one Potion of Life.", 10}, 
  {"PLATINUM", "Platinum Medal", "Defeat the second boss without using any Potions of Life, as any race.", 10}, 
  {"MUSHROOMSTAFF", "Mushroom Lord", "Used the Mushroom Staff.", 5}, 
  {"HYDRAQUAKES", "Hydra Quakes", "Used the titanic artifact.", 5}, 
  {"ZERO", "Fingernail Attack", "Got a blade of size 0 and attacked with it.", 5}, 
  {"NULLSECTOR", "Sanity", "Found the crazy weapon and tried to attack with it. ", 5}, 
  {"SHIELDABUSER", "Shield Abuser", "Tried to abuse the shields.", 5}, 
  {"BEAMISHBOY", "Beamish Boy", "Slain a Hydra with at least 100 heads with the Vorpal Blade.", 5}, 
  {"DECOMPOSED", "Decomposer", "Decomposed the Decomposer as a Divisor of size at least 17. ", 5}, 
  {"GIANTWEAPON", "Giant Slayer", "Killed a Hydra with a giant blade", 5}, 
  {"SILVERCUTTER", "Silver Cutter", "Killed a hydra >= 100 with the Silver Cutter", 5}, 
  {"WELLPREPARED", "Well Prepared", "Killed a Two-Headed Giant with a machete", 5}, 
  {"MAKEUSEFUL", "Practical Worker", "Made the Null Sector useful", 5}, 
  {"TELEFRAG", "Telefragger", "Killed a monster using the wand of phasing", 5}, 
  {"GROWTHMASTER", "Master of Growth", "Gave 10000+ heads at once using the Powder of Growth", 5}, 
  {"NECROPOWER", "Necro Power", "Created a zombie with 200000+ heads", 5}, 
  {"LIFESAVER", "Lifesaver", "Saved the Twin with a Potion of Life", 5}, 
  {"SEEDMASTER", "Master of Seeds", "Had 50+ active mushroom seeds at once", 5}, 
  {"SPEEDOFLIGHT", "Speed of Light", "Had 11+ active Potions of Speed at once", 5}, 
  {"RUNEMASTER", "Powdermaster", "Had 5+ active powders at once", 5}, 
  {"ALLAROUND", "All Around", "Killed 8 hydras with an all-around attack", 10}, 
  {"VAMPIRESLAYER", "Vulture Slayer", "Killed the first Vulture Hydra honorably", 5}, 
  {"TOUGHFIGHTER", "Barbarian Fighter", "Killed the first boss with weapons only", 10}, 
  {"MIRRORIMAGES", "Mirror Images", "Had one of the Twins return as a mirror image", 5}, 
  {"COLLAPSE", "Alternative Fighter", "Collapsed a Hydra under its own weight", 10}, 
  {"AMBIKILLER", "Master of Ambidexterity", "Killed a 2000+ Hydra with an ambidextrous attack", 5}, 
  {"SUICIDE", "Suicide", "Committed a suicide", 12}, 
  {"BOOMERANG", "Boomerang", "Had a missile dropped under their feet", 5}, 
  {"LONGTHROW", "Long Throw", "Killed 11+ hydras with a single missile throw", 10}, 
  {"RESURRECTIONX", "Resurrection", "Was killed and came back to life (not as Twin)", 10},
  {"NODIVISOR", "Don't Divide but Conquer", "Reached and killed the first boss without attacking with divisor blades", 10},
  {"NOJUICE", "Power is for the Weak", "Reached and killed the first boss without drinking the first Power Juice you pick up", 10},
  {"TRAPPER", "Trapper", "Killed a hydra with a trap", 5},
  {"NOJUICE3", "Innate Power", "Reached and killed the first boss without drinking the first three Power Juices you find", 10},
  {"BRONZEATLAS", "Atlantean Bronze Medal", "Defeat the first boss with an Atlantean.", 10}, 
  {"SILVERATLAS", "Atlantean Silver Medal", "Defeat the second boss with an Atlantean.", 10}, 
  {"GOLDATLAS", "Atlantean Gold Medal", "Defeat the second boss with an Atlantean, using at most one Potion of Life.", 10},
  {"PERFECTATLAS", "Scythe Mastery", "Clear the first level using no items or weapons except the ash scythe.", 10}, 
  };

void setmax(int& a, int b) { if(b>a) a=b; }

void viewNumAchievements(const vector<playerinfo>& pi, bool global, void_continuation vcon) {

  bool gamewon = false;
  bool gamewon2= false;
  int minwound = 1000000;
  int minwound2= 1000000;
  int minvalue = 1000000;
  int minvalue2= 1000000;
  int minarm   = 1000000;
  int minarm2  = 1000000;
  int minbi    = 1000000;
  int maxjuice = -1;
  int maxhydra = 0;
  int maxambi  = 0;
  int maxmsl   = 0;
  int maxchrg  = 0;
  int maxswipe = 0;
  int maxcoll  = 0;
  
  int mots[MOT];
  for(int a=0; a<MOT; a++) mots[a] = 1000000;

  for(int i=0; i<isize(pi); i++) {
    const playerinfo& Pi(pi[i]);
    if(global ? false : Pi.username != pinfo.username)
      continue;
    
    if(Pi.stats.endtype >= 3) {
      gamewon = true;
      minwound = min(minwound, Pi.stats.woundwin);
      minvalue = min(minvalue, Pi.stats.treasure);
      minarm   = min(minarm,   Pi.stats.armscore);
      minbi    = min(minbi,    Pi.stats.bossinv);
      if(Pi.stats.usedup[IT_PLIFE] == 0)
        setmax(maxjuice, Pi.player.inv[IT_PARMS]);
      for(int a=0; a<MOT; a++)
        mots[a] = min(mots[a], Pi.stats.motwin[a]);
      }
    
    if(Pi.stats.endtype >= 6) {
      gamewon2 = true;
      minwound2 = min(minwound2, Pi.stats.woundwin2);
      minvalue2 = min(minvalue2, Pi.stats.treasure2);
      minarm2   = min(minarm2,   Pi.stats.armscore2);
      }
    
    setmax(maxhydra, Pi.stats.hydrakill);
    setmax(maxambi,  Pi.stats.maxambi);
    setmax(maxmsl,   Pi.stats.maxmsl);
    setmax(maxchrg,  Pi.stats.maxchrg);
    setmax(maxswipe, Pi.stats.maxkill);
    setmax(maxcoll,  Pi.stats.owncrush);
    }
  
  erase(); col(7);
  
  addstr("Your achievements:\n\n");
  
  if(!gamewon) 
    addstr("You have not won the game yet.\n");
  else {
    addstri("Minimum wounds in a winning game: "+its(minwound)+"\n");
    addstri("Minimum value of used items: "+its(minvalue)+"\n");
    addstri("Best mutation score: "+its(minarm)+"\n");
    addstri("Least inventory items used against the boss: "+its(minbi)+"\n");
    if(maxjuice < 0)
      addstri("You still have to win without using the Potion of Life!\n");
    else {
      addstri("You have won without using the Potion of Life!\n");
      if(maxjuice > 0)
        addstri("... and "+its(maxjuice)+" Potions of Power Juice!\n");
      else
        addstri("... but you still have used all the Potions of Power Juice.\n");
      }
    
    addstri("You have won using blades (-) of size at most "+its(max(mots[MOT_OBSID], mots[MOT_BLADE]))+".\n");
    int nouse = 0;
    if(mots[MOT_BLUNT] == 0 && mots[MOT_SHLD] == 0)
      addstri("You have won without using blunt weapons and without shields!\n");
    else if(mots[MOT_SHLD] == 0)
      addstri("You have won using blunt weapons of size "+its(mots[MOT_BLUNT])+" and no shields.\n");
    else
      addstri("You have won using shields of size at most "+its(mots[MOT_SHLD])+".\n");
    if(mots[MOT_OBSID] == 0)
      addstri("You have won without using obsidian blades.\n"), nouse++;
    if(mots[MOT_MSL] == 0)
      addstri("You have won without using missiles.\n"), nouse++;
    if(mots[MOT_DIV] == 0)
      addstri("You have won without using divisors at all!!\n"), nouse++;
    else if(mots[MOT_DIV] <= 2)
      addstri("You have won without using divisors except the Bisector!\n"), nouse++;
    if(nouse == 0)
      addstri("You have always been using obsidian blades, missiles, and large divisors.\n");
    if(!gamewon2)
      addstri("But you still have not won the complete game!");
    }
  addstr("\n");

  if(gamewon2) {
    addstri("Minimum wounds in a complete winning game: "+its(minwound2)+"\n");
    addstri("Minimum value of items used in a complete game: "+its(minvalue2)+"\n");
    addstri("Best mutation score: "+its(minarm2)+"\n");
    }
    
  addstr("\n");

  addstri("Your best score was killing "+its(maxhydra)+" Hydras.\n");
  if(maxambi > 0)
    addstri("The biggest hydra one-shotted by you had "+its(maxambi)+" heads.\n");
  if(maxcoll > 1)
    addstri("You managed to collapse "+its(maxcoll)+" hydras in a single game!\n");
  else if(maxcoll)
    addstri("You managed to collapse a Hydra under its own weight!\n");
  if(maxmsl > 1)
    addstri("You managed to kill "+its(maxmsl)+" Hydras with one missile!\n");
  if(maxswipe > 1)
    addstri("You managed to kill "+its(maxswipe)+" Hydras with one power swipe!\n");
  if(maxchrg > 1)
    addstri("You managed to kill "+its(maxchrg)+" Hydras with one power charge!\n");
  
  KH(c, IC_VIEWDESC) { ignore_value(c); vcon(); };
  }

long long getAchievementId(const char* s) {
  long long res = 0;
  for(int i=0; i<NUMACH; i++)
    if(strcmp(s, ach[i].textcode) == 0) 
      res |= (1ll << i);
  return res;
  }

void achievement(const char* s) {
#ifdef STEAM
  extern void achievement_gain(const char *name);
  achievement_gain(s);  
#endif
  for(int i=0; i<NUMACH; i++)
    if(strcmp(s, ach[i].textcode) == 0) {
      stats.achiev |= (1ll << i);
      return;
      }
  addMessage("Warning: unrecognized achievement!");
  }

void addAchievementsToLog() {
  int myscore = 0;

  for(int i=0; i<NUMACH; i++) {
    bool inmy = (stats.achiev >> i) & 1;
    int sco = ach[i].score;
    if(inmy) myscore += sco;
    }
  
  if(!myscore) return;
  
  glog.push_back("\n");
  glog.push_back("Achievements earned in this game: (" + its(myscore)+" points)\n");

  for(int i=0; i<NUMACH; i++) {
    bool inmy = (stats.achiev >> i) & 1;
    if(inmy)
      glog.push_back("  " + s0 + ach[i].hname + " (" + its(ach[i].score) + ")\n");
    }
  }

void viewAchievements(const vector<playerinfo>& pi, bool global, void_continuation vcon) {

  static int basey = 0;
  
  uint64_t total = 0;

  for(int i=0; i<isize(pi); i++) {
    const playerinfo& Pi(pi[i]);
    if(global ? false : Pi.username != pinfo.username)
      continue;
    
    if(!Pi.curgame) {
      long long ach = Pi.stats.achiev;
      if(P.oldversion <= 1704)
        ach &=~ getAchievementId("NOJUICE2");
      total |= ach;
      }
    }

  static bool descr = false;
  
  int newscore = 0;
  int myscore = 0;
  int totscore = 0;
  
  for(int i=0; i<NUMACH; i++) {
    bool inmy = (stats.achiev >> i) & 1;
    bool intot = (total >> i) & 1;
    int sco = ach[i].score;
    if(intot || inmy) totscore += sco;
    if(inmy) myscore += sco;
    if(inmy && !intot) newscore += sco;
    }
  
  NOEMS(while(true)) {
    erase();
    move(0, 0); col(11); addstr("Your achievements:  ");
    col(10); 
    addstr(global ? "F-global" : "F-player");
    addstr(" ");
    addstr(descr ? "D-descr" : "D-names");
    addstr(" ");
    addstr("N-numerical");
    
    col(14); move(23, 30);
    addstri("total score: " + its(totscore)+" this game: "+ its(myscore)+" new points: "+its(newscore));
    
    if(basey < 0) basey = 0;
    if(basey + 20 > NUMACH) basey = NUMACH - 20;
    
    for(int i=0; i<20; i++) {
      int y = i + basey;
      move(i+2, 0);
      if((stats.achiev >> y) & 1) {
        if((total >> y) & 1) {
          col(11); addstr("this game");
          }
        else {
          col(14); addstr("NEW!");
          }
        }
      else if((total >> y) & 1) {
        col(15); addstr("got it");
        }
      else {
        col(8); addstr("missing");
        }
      move(i+2, 10); addstri(its(ach[y].score));
      
      if(descr) {
        move(i+2, 13); col(15); addstri(string(ach[y].desc).substr(0, 65));
        }
      else {
        move(i+2, 13); col(15);
        addstr(ach[y].hname);
        move(i+2, 37); col(7);
        addstri(string(ach[y].desc).substr(0, 43));
        }
      }

    bool gl = global;
    KH(c, IC_VIEWACH) { bool global = gl;
    switch(c) {
      case 'g': case 'f':
        global = !global; break; // goto replay;
      
      case 'n':
        viewNumAchievements(pi, global, [=] { viewAchievements(pi, global, vcon); });
        return;
      
      case 'd': case D_RIGHT: case D_LEFT: case 'h': case 'l':
        descr = !descr;
        break;
      
      case D_HOME:
        basey = 0;
        break;
        
      case D_END:
        basey = 0;
        break;
        
      case D_DOWN: case 'j': 
        basey ++;
        break;

      case D_PGDN:
        basey += 10;
        break;

      case D_UP: case 'k':
        basey --;
        break;
      
      case D_PGUP:
        basey -= 10;
        break;
      
      default:
        vcon();
        return;
      }
    ONEMS( viewAchievements(pi, global, vcon); )
    };
    }
  
  }

