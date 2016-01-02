// Hydra Slayer: math puzzle roguelike              
// Copyright (C) 2010-2011 Zeno Rogue, see 'hydra.cpp' for details

vector<string> glog;
  
#define STATC(x, msg) glog.push_back("  " + its(stats.x)    + msg);
#define STAT(x, msg) if(stats.x) STATC(x, msg);
#define STATI(x, msg) if(stats.x) glog.push_back("    " + its(stats.x)    + msg);

string logScore(const wpnscore& ws, int type) {
  string s = its(ws.sc[WS_USE])+ " uses";
  if(ws.sc[WS_HHEAD]) s += ", " + its(ws.sc[WS_HHEAD]) + " enemy heads";
  if(ws.sc[WS_HKILL]) s += ", " + its(ws.sc[WS_HKILL]) +
    (type == MOT_PICK ? " monsters" : type == -1 ? " own wounds" : " kills");
  if(ws.sc[WS_MHEAD]) s += ", " + its(ws.sc[WS_MHEAD]) + " mushroom heads";
  if(ws.sc[WS_MKILL]) s += ", " + its(ws.sc[WS_MKILL]) + " destroyed";
  if(ws.sc[WS_GROW ]) s += ", " + its(ws.sc[WS_GROW ]) + (type == MOT_PICK || type == MOT_PHASE ? " walls" : " heads regrown");
  if(ws.sc[WS_HSTUN]) s += ", " + its(ws.sc[WS_HSTUN]) + " stunned heads cut";
  return s;
  }

void createLog(bool saved) {
  // current: include weapon and last message info
  glog.clear();
  glog.push_back("Hydra Slayer v" VER " character log for "+rinf[P.race].rname+" "+pinfo.charname+" ("+pinfo.username+")\n");
  if(P.vchanged)
    glog.push_back("Version has been changed while playing this savegame.\n");
  if(debugon())
    glog.push_back("This character was run in the debug mode.\n");
  if(P.flags & dfAutoAttack)
    glog.push_back("This character was run in the auto-attack mode.\n");
  if(P.flags & dfBackups)
    glog.push_back("This character was not responsible for their actions.\n");

  if(saved) glog.push_back("Game saved.\n");
  else switch(stats.endtype) {
    case 0:
      glog.push_back("You are dead...\n");
      break;
    
    case 1: 
      glog.push_back("You have quit prematurely.\n");
      break;
    
    case 2:
      glog.push_back("You have escaped in shame.\n");
      break;

    case 3:
      glog.push_back("You have been killed after defeating the Ancient Hydra...\n");
      break;
    
    case 4:
      glog.push_back("You have killed the Ancient Hydra, but quit prematurely.\n");
      break;
    
    case 5:
      glog.push_back("You have won and escaped after killing the Ancient Hydra!\n");
      break;    

    case 6:
      glog.push_back("You have been killed after defeating both Ancient Hydras...\n");
      break;
    
    case 7:
      glog.push_back("You have won the full game and quit prematurely.\n");
      break;
    
    case 8:
      glog.push_back("You have won the full game and escaped!!\n");
      break;    
    }

  glog.push_back("\n");
  glog.push_back("Items found and used:\n");
  for(int i=0; i<ITEMS; i++) if(P.inv[i] + stats.usedup[i])
    glog.push_back("  " + iinf[i].name +
       " (" + its(stats.usedup[i]) + "/" + its(P.inv[i]+stats.usedup[i]) + ")\n"
       );
  STAT(itemNotFound, " items have not been picked up");
  
  bool usedNone = true;
  for(int i=0; i<ITEMS; i++) if(stats.usedb[i]) {
    if(usedNone) {
      glog.push_back("\n");
      glog.push_back(
        P.curlevel >= LEVELS ? "Items used on the ancient hydra level:\n" :
        "Items used on the last level:\n"
        );
      usedNone = false;
      }
    glog.push_back("  " + iinf[i].name + " (" + its(stats.usedb[i]) + ")\n");
    }
  
  if(usedNone && P.curlevel >= LEVELS) 
    glog.push_back("\nNo items used on the ancient hydra level - congratulations!\n");
  
  if(stats.endtype >= 3) {
    glog.push_back("\n");
    glog.push_back("Costs of winning the small game:\n");
    STATC(woundwin, " wounds\n");
    STATC(treasure, "$ of inventory items used up\n");
    STATC(armscore, " mutation score\n    (when slaying each hydra, you get +1 for each extra arm)\n");
    STATC(bossinv,  " items used against the boss\n"
               "     (including permanent effects of BS/L/PJ obtained on previous levels)\n");
    STATC(ancientwnd," wounds from the ancient hydra attacks\n");
    }

  if(stats.endtype >= 6) {
    glog.push_back("\n");
    glog.push_back("Costs of winning the big game:\n");
    STATC(woundwin2, " wounds\n");
    STATC(treasure2, "$ of inventory items used up\n");
    STATC(armscore2, " mutation score\n");
    STATC(ancientwnd2," wounds from the ancient dragon attacks\n");
    }

  glog.push_back("\n");
  glog.push_back("Various stats:\n");
  glog.push_back("  "+its(stats.wounds)+ " total wounds ("+its(P.curHP)+" of "+its(P.maxHP)+" HP left)\n");

  glog.push_back("  "+its(stats.turns)+ " total turns (hydra movements)\n");
  glog.push_back("  "+its(stats.tstart) + " total seconds of real time\n");

  if(P.geometry == 3) {
    glog.push_back("  variable geometry\n");
    }
  if(P.geometry == 4) {
    glog.push_back("  four-directional movement\n");
    }
  if(P.geometry == 8) {
    glog.push_back("  eight-directional movement\n");
    }
  if(P.geometry == 16) {
    glog.push_back("  knight movement\n");
    }
  if(P.geometry == 6) {
    glog.push_back("  played on a hex board\n");
    }

  STAT(solved,    " levels solved\n");  
  STAT(savecount, " times saved a game\n");
  STAT(emsave,    " times used an auto-saved game\n");
  STAT(backupsaves," times backed up the game\n");
  STAT(backuploads," times loaded the backup\n");
  STAT(automove,  " moves suggested by auto-explore\n");
  STAT(autoatt,   " attacks suggested by auto-attack\n");
  STAT(autokill,  " kills on auto-attack\n");
  
  if(stats.powerignore < 100)
    STAT(powerignore, " Potions of Power Juice consistently ignored\n");
  STAT(owncrush,  " hydras collapsed under own weight\n");
  STAT(invisible, " hydras have been killed without seeing\n");
  STAT(vampikill, " vulture hydras have been killed\n");
  STATI(unhonor,  " unhonorable attacks against mighty vultures\n");
  STAT(ettinkill, " giants were killed\n");
  STAT(ettinsave, " giants were left alive\n");
  STAT(monkeykill," monkeys were killed\n");
  STAT(monkeysave, " monkeys were left alive\n");
  STATI(ettinwnd, " wounds from armed attacks\n");
  STAT(ivykill,   " ivy hydras were killed\n");
  STAT(wizardkill," arch-hydras were killed\n");
  STAT(wiztimes,   " times Arch-Hydras aided other hydras\n");
  STAT(alienkill, " alien hydras have been killed\n");
  STAT(dragonkill," dragons were killed\n");
  STAT(shadowkill, " shadow hydras killed\n");
  STATI(shadowwounds," wounds from shadow hydras\n");
  STAT(ambiforge, " ambidextrous uses of scrolls of reforging\n");
  STATI(forgegrow, " points of size stolen from other weapons\n");
  STAT(ambiscroll," ambidextrous uses of scrolls of big stick and transmutation\n");
  STAT(broomhp, " HP lost flying (distance "+its(stats.broomdist)+")\n");
  
  glog.push_back("\n");
  glog.push_back("Various head stats:\n");
  STAT(vampire,   " heads regrown by vulture hydras while attacking\n");
  STAT(awakenhd,  " total heads awakened after stunning\n");
  STAT(awakenhyd, " total beasts awakened after stunning (part or total)\n");
  STAT(magegrow,  " enemy heads grown by Powder of Growth\n");
  STAT(mushgrow,  " mushroom heads grown by Powder of Growth\n");
  STAT(magestun,  " heads stunned by Powder of Stunning\n");
  STAT(stunchrg,  " total heads stunned by Potion of Weapon Charge\n");
  STAT(ivygrow,   " heads grown by Ivy Hydras\n");
  STAT(wizport,   " heads teleported by Arch-Hydras\n");
  STAT(wizout,    " heads teleported away by Arch-Hydras\n");
  STAT(vorpalhead," heads of hydras slain only with a vorpal blade (+"+its(stats.vorpalhp)+" HP)\n");
  STAT(automush,  " mushroom heads auto-removed by auto-explore\n");

  if(P.race == R_TWIN) {
    glog.push_back("\n");
    glog.push_back("Twin stats:\n");
    if(P.twinsNamed)
      glog.push_back("  Names: "+twinName(0)+"/"+twinName(1)+"\n");
    hydra *h = twinAlive();
    int ta = bitcount(P.twinarms);
    glog.push_back("  HP "+its(h ? h->heads : 0)+"/"+its(P.twinmax)+" : "+its(P.curHP)+"/"+its(P.maxHP)+", arms "+
      its(ta)+":"+its(P.arms-ta)+"\n");
    STAT(turntwin,  " of "+its(stats.turns)+" turns of controlling both twins\n");
    STAT(twinwounds," wounds to one twin from the other twin's attacks\n");
    STAT(twindef2,  " times a hydra could attack both twins\n");
    STAT(twindef1,  " times a hydra could attack only one twin\n");
    STAT(twinai,    " attacks by the twin AI \n");
    STAT(twinmy,    " attacks by you (not counting ambidextrous)\n");
    STAT(twinambij, " ambidextrous attacks by both twins at once\n");
    }

  glog.push_back("\n");
  glog.push_back("Records:\n");
  STATC(maxambi,   " max hydra one-shotted\n");
  if(stats.maxseed > 7) STAT(maxseed,  " max power of Mushroom Spores at once\n");
  if(stats.maxspeed> 1) STAT(maxspeed, " max speed drank at once\n");
  if(stats.maxrune > 1) STAT(maxrune,  " max powders active at once\n");
    if(stats.maxchrg > 1) STAT(maxchrg,  " max beasts killed at once with a Weapon Charge\n");
  if(stats.maxkill > 1) STAT(maxkill,  " max beasts killed at once with a Power Swipe\n");
  if(stats.maxmsl  > 1) STAT(maxmsl,   " max hydras killed at once with a missile\n");

  glog.push_back("\n");
  glog.push_back("Attack types used:\n");
  for(int i=0; i<MOT; i++) if(i == 0 || stats.ws[i].sc[WS_USE] || stats.maxoftype[i]) {
    string s = "  "+typenames[i] + ": " ;
    if(stats.maxoftype[i])
      s += "max " + its(stats.maxoftype[i]) + ", ";
    if(stats.motwin[i])
      s += "to win " + its(stats.motwin[i]) + ", ";
    glog.push_back(s + logScore(stats.ws[i], i) + "\n");
    if(i == MOT_MSL) {
      STATI(mshits,    " total hydras hit by your missiles\n");
      STATI(mixdkill,  " missile attacks killed multiple hydras\n");
      STATI(mixfunga,  " missile attacks hit mushrooms before hydras\n");
      STATI(mixfungb,  " missile attacks hit mushrooms after hydras\n");
      STATI(mixkill,   " missile attacks mixed total and partial destruction\n");
      STATI(mslself,   " times hit themselves with a missile ("+its(stats.mslselfdam)+" damage)\n");
      STATI(mslgent,   " times the missile returned to their hand without damage\n");
      }
    if(i == MOT_AMBI) {
      STATI(ambiwpn,   " one-hand hits used in ambidextrous attacks\n");
      }
    if(i == MOT_SHLD) {
      STATI(shieldprev," total damage prevented by having a shield of big size\n");
      STATI(shieldmatch," total damage prevented by having a shield of matching color\n");
      STATI(shieldheal," HP of healing from your shields\n");
      }
    if(i == MOT_ZOMBIE)
      STATI(necro,     " total mushroom heads converted to zombie heads\n");
    }
  }

void weaponToLog(weapon* w, string header) {
  string s = "  " + header + w->name();
  s += " (";
  s += w->type;
  s += its(w->size);
  s += ", ";
  if(w->osize == -1) s += "obtained by reforging, ";
  else if(w->ocolor != w->color || w->osize != w->size) {
    swap(w->ocolor, w->color);
    swap(w->osize, w->size);
    s += w->name();
    swap(w->ocolor, w->color);
    swap(w->osize, w->size);
    s += " ";
    }
  if(w->level < -10) s += "from a Giant on Level "+its(-10-w->level);
  else if(w->level) s += "from Level "+its(w->level);
  else s += "brought from home";
  s += ")";
  glog.push_back(s + "\n");
  if(w->sc.sc[WS_USE] || w->sc.sc[WS_MKILL])
    glog.push_back("    " + logScore(w->sc, w->getMot()) + "\n");
  }


void addCurrentInfoToLog() {
  glog.push_back("\n");
  glog.push_back("Weapons at the end:\n");
  for(int i=0; i<P.arms; i++) if(wpn[i]) weaponToLog(wpn[i], "");

  if(size(pinfo.trollkey)) {
    glog.push_back("\n");
    glog.push_back("Titan weapon inventory:\n");
    for(int i=0; i<size(pinfo.trollkey); i++) {
      string s = "[";
      s += pinfo.trollkey[i];
      s += "] ";
      weaponToLog(pinfo.trollwpn[i], s);
      }
    }

  
  bool ground = true;
  for(int y=0; y<SY; y++) for(int x=0; x<SX; x++) {
    cell& m(M.m[y][x]); weapon* w;
    if(m.explored && m.it && m.it->asWpn()) {
      w = m.it->asWpn();
      if(ground) {
        glog.push_back("\n");
        glog.push_back("On the ground:\n");
        ground = false;
        }
      weaponToLog(w, "");
      }
    }

  if(stats.whistSize) {
    glog.push_back("\n");
    glog.push_back("Weapon history:\n");
    for(int i=0; i<stats.whistSize; i++)
      weaponToLog(pinfo.whist[i], "(L" + its(pinfo.whistAt[i]+1) +") ");
    }

  glog.push_back("\n");
  glog.push_back("Last messages:\n");
  int from = size(msgs)-loglines;
  if(from < 0) from = 0;
  for(int f=from; f<size(msgs); f++) glog.push_back("  "+msgs[f]+"\n");
  }

void editString(string& s, string title = "Enter the name: ") {
  while(true) {
    move(22, 0); clrtoeol();
    move(22, 0); addstri(title+s);    
    char c = ghch(IC_EDIT);
    if(c == '\r' || c == '\n' || c == PANIC) return;
    else if(c == 1 || c == 27 || c == '\t')
      s = "";
    else if(c == '\b' || c == 7 || c == 8) {
      if(s != "") s = s.substr(0, s.size()-1);
      }
    else s = s + c;
    }
  }

void recordToHall() {
  if(P.flags & dfsInvalid) return;
  P.version = VERSION;
  savefile = fopen(scorename, "ab");
  error = !savefile;
  if(error) return;
  save(P);
  save(stats);
  saveString(pinfo.charname);
  saveString(pinfo.username);
  fclose(savefile);
  }

char sorttype;

bool fame_fullwin = false;

int pistat(const playerinfo& p) {
  switch(sorttype) {
    case 'h': return -p.stats.hydrakill;
    case 'm': return -p.stats.maxkill - p.stats.maxmsl - p.stats.maxchrg;
    case 'e': return p.stats.gameend;
    }
  if(p.stats.endtype < 3) return 1000000;
  switch(sorttype) {
    case 'w': return fame_fullwin ? p.stats.woundwin2: p.stats.woundwin;
    case 'u': return fame_fullwin ? p.stats.armscore2: p.stats.armscore;
    case 'c': return fame_fullwin ? p.stats.treasure2: p.stats.treasure;
    case 'b': return p.stats.bossinv;
    }
  return 0;
  }

bool pisort(const playerinfo& p1, const playerinfo& p2) {
  return pistat(p1) < pistat(p2);
  }

void viewHall(bool current) {
  savefile = fopen(scorename, "rb");
  if(!savefile) return;
  vector<playerinfo> pi;
  error = false;
  
  while(not(feof(savefile))) {
    playerinfo Pi;
    load(Pi.player);
    if(error) break;
    loadStats(Pi.stats, Pi.player.saveformat);
    Pi.charname = loadString();
    Pi.username = loadString();
    Pi.curgame = false;
    pi.push_back(Pi);
    }
  fclose(savefile);
  if(current) {
    pinfo.curgame = true; pi.push_back(pinfo);
    }
  
  sorttype = 'h'; stable_sort(pi.begin(), pi.end(), pisort);  
  
  int startat = 0;
  bool global = true;
  
  int crace = -1;

  while(true) {
    
    erase();
    move(0, 35); col(11); addstri(" Hki Mk  Wnd Cost$ Bi mUt end of game time");
    
    int cury = 1, at = startat;
    
    while(cury < 19) {
      if(at >= size(pi)) break;
      playerinfo& Pi(pi[at]);
      if(!global && Pi.username != pinfo.username) { at++; continue; }
      if(crace >= 0 && Pi.player.race != crace) { at++; continue; }
      
      move(cury, 0); col(Pi.curgame ? 14 : 7); addstri(itsf(at+1, 3) + (Pi.curgame ? "*" : "."));
  
      move(cury, 5); col(Pi.curgame ? 14 : 8); for(int i=0; i<10; i++) addstr(" . ");
      
      int etcol[9] = { 4, 7, 6, 12, 15, 14, 13, 10, 11}; col(etcol[Pi.stats.endtype]);
      
      move(cury, 5); addstri(Pi.charname);
      move(cury, 15); addstri(Pi.username);
      
      move(cury, 33); col(rinf[Pi.player.race].color); addch(rinf[Pi.player.race].rkey);
  
      move(cury, 35);
      col(15);
      addstri(itsf(Pi.stats.hydrakill, 4));
      addstri(itsf(Pi.stats.maxmsl + Pi.stats.maxchrg + Pi.stats.maxkill, 3));

      if(fame_fullwin) {
        if(Pi.stats.endtype < 6) {
          col(8); 
          addstri(" full winners only");
          }
        else {
          col(11);
          if(Pi.stats.armscore2 == 0) Pi.stats.armscore2 = 9999;
          addstri(itsf(Pi.stats.woundwin2, 5));
          addstri(itsf(Pi.stats.treasure2, 6));
          addstri(itsf(Pi.stats.armscore2, 7));
          }
        }
      else {
        if(Pi.stats.endtype < 3) {
          col(8); 
          addstri("  for winners only");
          }
        else {
          col(14);
          if(Pi.stats.treasure == 0) Pi.stats.treasure = 99999;
          if(Pi.stats.woundwin == 0) Pi.stats.woundwin = 9999;
          addstri(itsf(Pi.stats.woundwin, 5));
          addstri(itsf(Pi.stats.treasure, 6));
          addstri(itsf(Pi.stats.bossinv,  3));
          addstri(itsf(Pi.stats.armscore, 4));
          }
        }
      
      addstri(" ");
      time_t gameend = Pi.stats.gameend;
      struct tm *tmp = localtime(&gameend);
      char buf[100]; col(7);
      strftime(buf, 100, "%y/%m/%d %H:%M:%S", tmp);
      addstri(buf);
      
      // addstri(Pi.stats.maxambi ? itsf(Pi.stats.maxambi, 5) : "     ");
      // addstri(Pi.stats.owncrush ? itsf(Pi.stats.owncrush, 2) : "  ");
      at++; cury++;
      }
  
    col(7);
    move(20, 0);
    addstr("Wnd-wounds to win, Cost-cost of inv used to win, Bi-inv items used on boss");
    move(21, 0);
    addstr("mUt-mutation score; Hki-total hydras killed, Mk-sum of multikill scores");
  
    move(23, 0); col(10);
    string filter = global ? "global" : "player";
    string isfull = fame_fullwin ? "full" : "part";
    string racename = crace == -1 ? "ALL" : rinf[crace].rname;
    addstri("WCBUHME-sort, Q-menu, F-"+filter+", A-achievements, P-"+isfull+" R-"+racename);
    
    int c = ghch(IC_HALL);
    switch(c) {
      case 'w': case 'c': case 'b': case 'u': case 'h': case 'm': case 'e':
        sorttype = c;
        stable_sort(pi.begin(), pi.end(), pisort);
        break;
      
      case 'f':
        global = !global;
        break;
        
      case 't':
        startat = 0;
        break;
      
      case 'n':
        startat += 10;
        break;
      
      case 'a':
        viewAchievements(pi, global);
        break;
      
      case 'o':
        startat ++;
        break;
      
      case 'p':
        fame_fullwin = !fame_fullwin;
        break;
      
      case 'r':
        crace++; if(crace == R_ELF) crace++;
        if(crace >= RACES) crace = -1;
        break;
      
      case 'q': case ' ': case '\r': case '\n': case ESC: case PANIC:
        return;
      
      default:
        if(c == D_RIGHT || c == D_DOWN) startat++;
        if(c == D_LEFT  || c == D_UP  ) startat--;
        if(c == D_PGUP) startat -= 10;
        if(c == D_PGDN) startat += 10;
        if(c == D_HOME) startat = 0;
        if(c == D_END ) startat = size(pi);
        if(startat > size(pi)-17) startat = size(pi) - 17;
        if(startat < 0) startat = 0;
        break;
      }
    }
  }

void clearGame() {
  if(!gameExists) return;
  for(int i=0; i<LEVELS; i++) {
    for(int j=0; j<size(toput[i]); j++) delete toput[i][j];
    toput[i].clear();
    }
  gameExists = false;
  
  while(size(hydras))
    M[hydras[0]->pos].hydraDead(NULL);

  clearLevel();
  
  int deb = P.flags;
  int race = P.race;
  
  int sdirs = P.geometry;
  
  bool sman = P.manualfire;
  bool salt = P.altkeys;
  bool squi = P.quickmode;
  bool simp = P.simplehydras;
  
  memset(&stats, 0, sizeof(stats));
  memset(&P, 0, sizeof(P));
  
  swap(P.manualfire, sman);
  swap(P.altkeys, salt);
  swap(P.quickmode, squi);
  swap(P.simplehydras, simp);
  
  P.race = race; P.flags = deb;
  
  pinfo.whist.clear();
  pinfo.whistAt.clear();
  pinfo.trollwpn.clear();
  pinfo.trollkey.clear();
  stairqueue.clear();
  for(int i=0; i<MAXARMS; i++) if(wpn[i]) { delete wpn[i]; wpn[i] = NULL; }
  
  P.geometry = sdirs;
  }

void cheatMenu() {
  erase();

  move(0, 0); col(12); addstri("Cheats available");
  
  int cy = 2; move(cy, 0); col(7);

  const char *cheats = 
    "(A) Auto-attack. This option will let you press 'a'/'y' to let the game "
    "select the weapons for your attacks automatically. Only recommended "
    "for those who understand the game very well and want to concentrate "
    "on the challenge of getting the best equipment, without having to "
    "compute the strategy for each individual hydra. Not recommended "
    "for beginners, since you need to understand your weapons, and there "
    "are some ways of Hydra slaying that it does not understand.\n"
    
    "(B) Backups. Normally you can reload a saved game only once, so you "
    "cannot cheat by reloading the game again after making a mistake. "
    "This option allows you to keep a backup of your game. "
    "Note that this does not "
    "help that much in Hydra Slayer as in other roguelikes: the combat system "
    "is deterministic in most cases, you will find the same equipment if "
    "you reload the game from the backup, and if your equipment strategy "
    "is bad, backups will not help you. Warning: you will get big penalties "
    "in all challenges based on randomness.\n"
    
    "Even though the game still remains a challenge with the cheats above, "
    "you are only allowed to get highscores and achievements if you do not "
    "use any of them. Press (C) to cancel this menu and return to game.\n"
    
    "When starting a new game, you can also press (D) instead of (N) to "
    "start the game in the debug mode.\n";
  
  viewMultiLine(cheats, cy); move(23, 79);
  
  int c = ghch(IC_CHEATMENU);
  
  if(c >= 'A' && c <= 'Z') c += 32;
  
  if(c == 'a' || c == 'b') {

    addstr("Are you sure you want to activate this cheat? (y/n)");
    if(!yesno(IC_MYESNO)) return;
    
    if(c == 'a') P.flags |= dfAutoAttack | dfAutoON;
    if(c == 'b') { P.flags |= dfBackups; stats.backupsaves++; saveGame(backname); }
    }
  }

void mainloop();
void initGame();
bool selectRace();

void mainmenu() {

  while(!quitgame) {
  
    bool saveerror = false;

    if(gameExists) {  
      stats.gameend = time(NULL);
      emSaveGame();
      saveerror = error;
    
      stats.solved = P.curlevel;
      if(canGoDown()) stats.solved++;
      if(P.curHP <= 0 || (P.race == R_TWIN && !twinAlive()))
        stats.endtype = 0;
      else if(M[playerpos].type == CT_STAIRUP)
        stats.endtype = 2;
      else
        stats.endtype = 1;
      if(stats.solved >= LEVELS) stats.endtype += 3;
      if(stats.solved >= 50) stats.endtype += 3;
      }
    else stats.endtype = 10;

    erase();
  
    move(0, 2); col(4); addstr("Hydra Slayer v" VER " by Zeno Rogue");
    
    move(2, 0); col(15);
    
    if(saveerror) {
      col(12);
      addstr("ERROR saving the game!");
      }
    
    else if(P.flags & dfTutorial) {
      if(stats.endtype == 0)
        addstr("You have been killed. Try to be more careful next time!");
      else
        addstr("Do you want to exit the Tutorial?");
      }
      
    else switch(stats.endtype) {
      case 10:
        addstr("Welcome to Hydra Slayer! Press (H) or F1 or ? for help.");
        break;
        
      case 0:
        addstr("You are dead.");
        break;
      
      case 2:
        addstr("Do you want to escape in shame?");
        break;
    
      case 3:
        addstr("You are dead... But at least you have won the battle!");
        break;
    
      case 5:
        addstr("You have won the battle, but not the war! Do you want to go home anyway?");
        break;

      case 6:
        addstr("You are dead... But at least you have won the war!");
        break;
    
      case 8:
        addstr("You have won the war! Do you want to go home?");
        break;
      }

    col(7);
    
    if(!gameExists) {
      move(4, 0); addstr("(T) start the Tutorial");
      move(5, 0); addstr("(N) start a new game");
      move(6, 0); addstr("(Q) quit the game");
      #ifdef NOTEYE
      move(7, 0); addstr("(F4) sound/gfx settings");
      #endif
      }
    else {
      move(4, 0); addstr("(Z) Return to game (also Space/Enter/Esc)");
      move(5, 0); addstr(stats.endtype%3 ? "(S) Save the game and quit" : "(S) Save the body and quit");
      move(6, 0); if(!debugon()) addstr("(Q) Quit this character");
      move(7, 0); addstr("(X) Quit without recording to Hall of Fame");
      move(8, 0); addstri("(D) Cheats");
      }

    move(10, 0); addstr("(A) View Achievements and the Hall of Fame");
    move(11, 0); addstr("(O) open another savefile");
    if(gameExists) addstr(" (R to rename)");

    if(P.flags & dfBackups) {
      move(12, 0);
      addstri("(B) Backup your game");

      move(12, 40);
      addstri("(Shift+B) Reload the game from the backup");
      }
    
    
    move(13, 0);
    addstri("(C) Character name: " + pinfo.charname);
    
    move(14, 0);
    addstri("(P) Player name: " + pinfo.username);
    
    
    if(P.race == R_TWIN) {
      col(13);
      move(15, 0);
      if(P.twinsNamed)
        addstr("(T) just call them 'you' and 'your twin'");
      else
        addstr("(T) name twins individually");
      move(16, 0);
      if(P.twinsNamed)
        addstri("(1) "+pinfo.twin[0]+" (2) "+pinfo.twin[1]);
      }
    
    move(0, 79);
    int c = ghch(IC_QUIT);
    
    switch(c) {
      case 't': case 'T':
        if(gameExists)
          P.twinsNamed = !P.twinsNamed;
        else {
          P.race = R_HUMAN;
          P.flags = dfTutorial;
          P.geometry = 4;
          initGame();
          mainloop();
          }
        break;
      
      case '1':
        if(P.twinsNamed) editString(pinfo.twin[0]);
        break;
        
      case '2':
        if(P.twinsNamed) editString(pinfo.twin[1]);
        break;
        
      case 'c':
        editString(pinfo.charname);
        break;
        
      case 'p':
        editString(pinfo.username);
        break;

      case ' ': case '\n': case '\r': case ESC: case 'z': case 'Z':
        if(gameExists) mainloop();
        break;
      
      case 'h': case 'H': case KEY_F0+1: case '?': case '/':
        viewHelp();
        break;
      
      case 'a': case 'A':
        viewHall(gameExists);
        break;
      
      case 'n': case 'N':
        if(gameExists) break;
        if(selectRace()) {
          initGame();
          mainloop();
          }
        break;
      
      case 'd': case 'D':
        if(gameExists) cheatMenu();
        else {
          move(17, 0); col(15); addstr("Start the game in Debug mode? (y/n)");
          if(!yesno(IC_MYESNO)) continue;
          if(selectRace()) {
            P.flags = dfDebug;
            initGame(); mainloop();
            }
          }
        break;

      case 's': case 'S': case 'q': case 'Q': case 'x': case 'X': {
        if(!gameExists) return;
        move(17, 0); col(15); addstr("Are you sure? (y/n)");
        if(gameExists)
          if((c == 's' || c == 'S') ? false : !yesno(IC_MYESNO)) continue;
        stats.tstart  = time(NULL) - stats.tstart;
        endwin();
        createLog(c == 's' || c == 'S');
        addCurrentInfoToLog();
        initScreen();
        FILE *f = fopen(logname, "wt");
        if(f) {
          for(int i=0; i<size(glog); i++) {
            fprintf(f, "%s", glog[i].c_str());
            #ifndef ANDROID
            printf("%s", glog[i].c_str());
            #endif
            }
          fclose(f);
          printf("Log saved to '%s'.\n", logname);
          }
        #ifndef ANDROID
        else printf("Error while saving log to '%s'.\n", logname);
        #endif
        if(c == 's' || c == 'S') {
          saveGame();
          #ifndef ANDROID
          if(error)
            printf("Error while saving to '%s'.\n", savename.c_str());
          else
            printf("Game saved to '%s'.\n", savename.c_str());
          #endif
          }
        else deleteGame();
        if(c == 'q' || c == 'Q') {
          recordToHall();
          #ifndef ANDROID
          if(error) 
            printf("Error while writing to '%s'.\n", scorename);
          else if(!debugon())
            printf("Added character to '%s'.\n", scorename);
          #endif
          }
        
        P.flags = 0;
        clearGame();
        if(c == 's' || c == 'S') return;
        break;
        }

      case 'o': case 'O': {
        string s = savename;
        saveGame(savename);
        editString(savename);
        if(savename == "") savename = s;
        clearGame();
        loadGame(savename);
        if(gameExists) initGame();
        if(P.flags & dfBackups) editString(backname);
        break;
        }
      
      case 'r': case 'R': {
        string s = savename;
        deleteGame(savename);
        editString(savename);
        if(savename == "") savename = s;
        break;
        }
      
      case 'b':
        if(P.flags & dfBackups) {
          stats.backupsaves++;
          // how many times saved the game
          saveGame(backname);
          addMessage("Backup saved.");
          return;
          }
        break;

      case 'B':
        if(P.flags & dfBackups) {
          clearGame();
          loadGame(backname);
          stats.backuploads++;
          saveGame(backname);
          addMessage("Backup loaded.");
          return;
          }
        break;
      }
    }
  }

void mainloop();

bool selectRace() {

  while(!quitgame) {
    erase();
  
    move(1, 2); col(15); addstr("Select your race: ");

    #ifdef ANDROID
    move(1, 40);
    #else
    move(1, 30);
    #endif

    for(int i=0; i<RACES; i++) if(i != R_ELF) {
      if(i) { col(8); addstr(" | "); }
      bool light = true;
      for(int l=0; l<size(rinf[i].rname); l++) {
        char c = rinf[i].rname[l];
        if(rinf[i].rkey == tolower(c) && light) col(14), light = false;
        else if(i == P.race) col(15);
        else col(7);
        addch(c);
        }
      }
    
    int cy = 3;
    col(7); viewMultiLine(rinf[P.race].desc, cy);

    move(21, 2); col(7); 
    addstr("Geometry: ");
    switch(P.geometry) {
      case 16: addstr("(D) knight movement (secret)"); break;
      case 8:  addstr("(D) 8 directions (traditional geometry)"); break;
      case 6:  addstr("(D) 6 directions (hex board)"); break;
      case 4:  addstr("(D) 4 directions (for laptop users)"); break;
      case 3:  addstr("(D) variable (4/6/8)"); break;
      }

    move(23, 2); col(15); addstri("Press Enter to start playing as "+pinfo.charname+" the "+rinf[P.race].rname+"!");

    move(0, 75); int c = ghch(IC_RACE); 
    
    switch(c) {

      case 'q': case 27:
        return false;
      
      case 'd': case ' ':
        if(P.geometry == 4) P.geometry = 6;
        else if(P.geometry == 6) P.geometry = 8;
        else P.geometry = 4;
        break;
      
      case '4': case '6': case '8': case '3':
        P.geometry = c - '0';
        break;
      
      case '^':
        P.geometry = 16;
        break;
      
      case 'r': case 10: case 13:
        return true;
      
      default:
        if(c >= DBASE && c <= DBASE+7) {
          if(c >= D_LEFT) { P.race--; if(P.race<0) P.race += RACES; }
          else { P.race++; P.race %= RACES; }
          if(P.race == R_ELF) {
            if(c >= D_LEFT) { P.race--; if(P.race<0) P.race += RACES; }
            else { P.race++; P.race %= RACES; }
            }
          }    
      }
    
    for(int i=0; i<RACES; i++) if(rinf[i].rkey == c) {
      P.race = i;
      }
    }
  return false;
  }
