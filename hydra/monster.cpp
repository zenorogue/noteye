// Hydra Slayer: math puzzle roguelike
// Copyright (C) 2010-2011 Zeno Rogue, see 'hydra.cpp' for details

int drainpower(hydra *h) {
  int b = 0;
  if(h->color == HC_VAMPIRE) b++;
  if(P.race == R_ATLANTEAAN && !h->lowhead()) b++;
  return b;
  }

bool hydra::aware() { return visible() ? M[pos].seen : (P.flags & dfShadowAware); }

void takeWounds(int thc) {
  P.curHP -= thc;
  stats.wounds += thc;
  if((P.flags & dfChallenge) && P.curlevel < CLEVELS)
    pinfo.cdata.wounds[P.curlevel] += thc;
  }

void removeWounds(int hp) {
  if(hp > P.maxHP - P.curHP) hp = P.maxHP - P.curHP;
  takeWounds(-hp);
  }

void useupItem(int ii) {
  if((P.flags & dfChallenge) && P.curlevel < CLEVELS)
    pinfo.cdata.itcost[P.curlevel] += iinf[ii].value;
  P.inv[ii]--; stats.usedup[ii]++;
  if(!stats.woundwin) stats.usedb[ii]++;
  }

bool nearPlayer(vec2 loc) {
  if(neighbor(loc, playerpos)) return true;
  if(!twin) return false;
  if(neighbor(loc, twin->pos)) return true;
  return false;
  }

bool nearPlayer(hydra *H) {
  return nearPlayer(H->pos);
  }

bool weapon::protectAgainst(hydra *h) {
  if(type != WT_SHLD) return false;
  if(color == h->info().suscept) return true;
  if(color == h->info().weakness) return true;
  if(h->ewpn && hyinf[h->ewpn->color].weakness == color)
    return true;
  return false;
  }

void shieldinfo::prepare(int maxsh, hydra *h, bool shu) {
  maxss = 0; minss = 999999999;
  for(int j=0; j<P.arms; j++) if(wpn[j] && wpn[j]->type ==WT_SHLD && !havebit(P.twinarms, j))
    maxss = max(maxss, wpn[j]->size),
    minss = min(minss, wpn[j]->size);
    
  if(maxsh > maxss*maxss) maxsh = maxss * maxss;
  if(maxsh < 10) maxsh = 10;
  
  ehcnt.resize(maxsh+1);
  if(shu) shused.resize(maxsh+1);
  
  for(int i=0; i<=maxsh; i++) ehcnt[i] = i;
  if(shu) for(int i=0; i<=maxsh; i++) shused[i] = 0; // i;
  
  for(int j=0; j<P.arms; j++) if(wpn[j] && wpn[j]->type ==WT_SHLD && !havebit(P.twinarms, j)) {
    // only shields that match weapon color or ettin color are effective against ettins
    if(h->ewpn && !wpn[j]->protectAgainst(h))
      continue;

    int siz = wpn[j]->size;
    
    stats.maxoftype[MOT_SHLD] = max(stats.maxoftype[MOT_SHLD], siz);
    
    for(int i=siz; i<=maxsh; i++)
      if(ehcnt[i-siz]+1 <= ehcnt[i]) {
        ehcnt[i] = ehcnt[i-siz]+1; 
        if(shu) shused[i] = siz;
        }
    }
  
  reduce = 0;
  if(P.race == R_NAGA && !h->ewpn) reduce--;

  if(!h->ewpn)
    for(int j=0; j<P.arms; j++) 
      if(wpn[j] && wpn[j]->protectAgainst(h) && !havebit(P.twinarms, j)) 
        reduce++;
  
  if(h->ewpn) reduce = -3; 
  }

int shieldinfo::ehcntx(int q) {
  if(maxss == 0) return q;
  if(q >= size(ehcnt)) {
    int b = (q / maxss) - (maxss-1);
    return ehcnt[q - b * maxss] + b;
    }
  return ehcnt[q];
  }

int shieldinfo::dampre(int q) { // without SI.reduce
  if(reduce == -3) return ehcntx(q);
  return powerf(ehcntx(q));
  }
  
int shieldinfo::dampost(int q) {
  if(reduce == -3) return ehcntx(q);
  int d = powerf(ehcntx(q));
  d -= reduce;
  if(d < 0) d = 0;
  return d;
  }

int shieldinfo::dampost_true(int q) {
  if(reduce == -3) return ehcntx(q);
  int d = powerf(ehcntx(q));
  d -= reduce;
  return d;
  }

int shieldinfo::usize(int siz) {
  if(siz < size(shused)) return shused[siz];
  return maxss;
  }

void cell::hydraDead(hydra *killer) {
  if(h == vorplast) vorpalClear();
  
  if(h->color != HC_TWIN) P.maxHP++;
  
  if(h->color == HC_SHADOW && (P.flags & dfBackups)) {
    addMessage("That kill was strangely unsatisfying!");
    h->heal = 0;
    }

  h->freegfxid();

  if(killer && killer != h)
    killer->heal += h->heal;
  else
    P.curHP += h->heal;
  
  // nagas heal less!
  if(P.race == R_NAGA && h->heal >= 2) P.curHP--;
  if(P.race == R_NAGA && h->heal >= 4) P.curHP--;
  if(P.race == R_NAGA && h->heal >= 6) P.curHP--;
  
  // Atlanteans heal a bit more!
  if(P.race == R_ATLANTEAAN) {
    if(P.curlevel <= 0)
      P.curHP += 5;
    else if(P.curlevel < 12)
      P.curHP += 2;
    else if(P.curlevel < 50)
      P.curHP ++;
    }

  // stop waiting if a hydra dies
  if(h->visible()) inWaitMode = false;

  if(P.curHP > P.maxHP) P.curHP = P.maxHP;

  if(h->visible())
    dead = (h->color & HC_DRMASK) + 1;
  else
    P.flags &= ~dfShadowAware;
  
  if(h->ewpn) {
    if(!it) it = h->ewpn;
    else for(int d=0; d<DIRS; d++) {
      vec2 pos2 = h->pos + dirs[d];
      cell& c(M[pos2]);
      if(c.it == NULL && c.type == CT_EMPTY) { c.it = h->ewpn; break; }
      }
    }
  
  if(h->color == HC_ETTIN) stats.ettinkill++;
  else if(h->color == HC_MONKEY) stats.monkeykill++;
  else if(h->color != HC_TWIN) {
    stats.hydrakill++;
    highscore("hydras killed", stats.hydrakill, 1);
    if(!stats.usedup[IT_PLIFE]) highscore("hydras killed/no life", stats.hydrakill, 1);
    if(P.flags & dfDaily) highscoreDaily();
    if(P.curlevel < GLEVELS) stats.armscore += P.arms - 2;
    if(P.curlevel < LEVELS2) stats.armscore2 += P.arms - 2;
    }
  if(h->color == HC_GROW) stats.ivykill++;
  if(h->color == HC_ALIEN) stats.alienkill++;
  if(h->color == HC_WIZARD) stats.wizardkill++;
  if(h->color == HC_EVOLVE) stats.evolkill++;
  if(h->color == HC_SHADOW) stats.shadowkill++;
  if(h->color == HC_VAMPIRE) stats.vampikill++;
  if(h->color & HC_DRAGON) stats.dragonkill++;
  if(P.flags & dfAutoON) stats.autokill++;

  if(h->color == HC_VAMPIRE && !stats.unhonor) achievement("VAMPIRESLAYER");
  
  for(int i=0; i<size(hydras); i++) if(hydras[i] == h) {
    for(int j=i+1; j<size(hydras); j++) hydras[j-1] = hydras[j];
    hydras.resize(size(hydras)-1);
    }

  bool cleanup = (pinfo.player.flags & dfCleanup);
  
  bool anc = h->isAncient();

  if(h->isAncient() && !cleanup) {
    if(P.flags & dfChallenge) {
      if(debugon())
        addMessage("OK, you win. Cheater.");
      else
        addMessage("The Challenge is completed!");
      stats.woundwin = stats.wounds;
      stats.treasure = 0;
      for(int i=0; i<ITEMS; i++)
        stats.treasure += stats.usedup[i] * iinf[i].value;
      highscoreDaily();
      }
    else if(P.curlevel < GLEVELS) {
      if(debugon())
        addMessage("You kill the Ancient Hydra!!! But it is possible without cheating!");
      else
        addMessage("You kill the Ancient Hydra!!! This will make you famous!");
      string yourself = twinAlive() ? "yourselves!" : "yourself!";
      addMessage("Go up to let your successors do the rest, or down to do it "+yourself);
      stats.woundwin = stats.wounds;
      stats.treasure = 0;
      for(int i=0; i<16; i++) stats.motwin[i] = stats.maxoftype[i];
      for(int i=0; i<ITEMS; i++)
        stats.bossinv += stats.usedb[i],
        stats.treasure += stats.usedup[i] * iinf[i].value;
      
      if(P.race == R_NAGA) achievement("BRONZENAGA");
      if(P.race == R_HUMAN) achievement("BRONZEHUMAN");
      // if(P.race == R_ELF) achievement("BRONZEELF");
      if(P.race == R_CENTAUR) achievement("BRONZECEN");
      if(P.race == R_TROLL) achievement("BRONZETROLL");
      if(P.race == R_TWIN) achievement("BRONZETWIN");
      
      if(stats.maxoftype[MOT_DIV] == 0) achievement("NODIVISOR");
      
      if(P.race != R_TROLL && stats.powerignore >= 1) achievement("NOJUICE");
      if(stats.powerignore >= 3) achievement("NOJUICE3");

      bool usedNone = true;
      for(int i=0; i<ITEMS; i++) if(stats.usedb[i]) usedNone = false;
      if(usedNone) achievement("TOUGHFIGHTER");

      highscore("wounds to win/partial", stats.woundwin, -1);
      highscore("value of items used/partial", stats.treasure, -1);
      highscore("mutation score/partial", stats.armscore, -1);
      string s = " the Ancient Hydra taking only "+its(stats.woundwin)+" wounds";
      if(usedNone) s += " without using any items";
      if(P.arms < 6) s += " with just "+its(P.arms)+" arms";
      shareS("kill", s);
      }
    else {
      string feeling = twinAlive() ? "true winners" : "a true winner";
      if(debugon())
        addMessage("You feel like a true debugger!");
      else if(P.inv[IT_PLIFE] || P.race == R_TROLL)
        addMessage("You kill the Ancient Dragon!!! You feel like "+feeling+"!");
      else
        addMessage("You would feel like "+feeling+" if you still had a Potion of Life!");
      addMessage("But you can still go down for more hydra slaying!");
      stats.woundwin2 = stats.wounds;
      stats.treasure2 = 0;
      for(int i=0; i<ITEMS; i++)
        stats.treasure2 += stats.usedup[i] * iinf[i].value;

      if(P.race == R_NAGA) achievement("SILVERNAGA");
      if(P.race == R_HUMAN) achievement("SILVERHUMAN");
      // if(P.race == R_ELF) achievement("SILVERELF");
      if(P.race == R_CENTAUR) achievement("SILVERCEN");
      if(P.race == R_TROLL) achievement("SILVERTROLL");
      if(P.race == R_TWIN) achievement("SILVERTWIN");
      
      if(stats.usedup[IT_PLIFE] < 2) {
        if(P.race == R_NAGA) achievement("GOLDNAGA");
        if(P.race == R_HUMAN) achievement("GOLDHUMAN");
        // if(P.race == R_ELF) achievement("GOLDELF");
        if(P.race == R_CENTAUR) achievement("GOLDCEN");
        if(P.race == R_TROLL) achievement("GOLDTROLL");
        if(P.race == R_TWIN) achievement("GOLDTWIN");
        }
      
      if(stats.usedup[IT_PLIFE] == 0) achievement("PLATINUM");

      highscore("wounds to win/full", stats.woundwin, -1);
      highscore("value of items used/full", stats.treasure2, -1);
      highscore("mutation score/full", stats.armscore2, -1);

      string s = " the Ancient Dragon taking only "+its(stats.woundwin2)+" wounds";
      if(P.inv[IT_PLIFE]) s += " without using "+its(P.inv[IT_PLIFE])+" Potions of Life";
      if(P.arms < 10) s += " with just "+its(P.arms)+" arms";
      shareS("kill", s);
      }
    }

  if(h == twin) {
    if(P.inv[IT_PLIFE])
      addMessage("It seems you'll have to use the Potion of Life!");
    else
      addMessage(twinName(true,1)+" is dead! How could you allow that?!");
    twin = NULL;
    }

  bool req = !h->lowhead() && !h->zombie;

  delete h;
  extern int sceneid;
  if(req && canGoDown() && !sceneid && !cleanup) {
    string msgs[11] = {
      "First level completed! Step on the stairs ('>') to go to the next level.",
      "No mighty hydra slaying weapons yet... well, no REAL hydras either.",
      "You carefully improve your armor using the scales of killed hydras.",
      "You stop for a while to calmly drink the delicious and healthy hydra blood.",
      "You feel a strange sense of satisfaction.",
      "You feel relatively safe now.",
      "Another pack of Hydras slain!",
      "Your mission on this level is accomplished. Wow, what a tasty hydra meat.",
      "Such powerful Hydras slain. You feel satisfied!",
      "You have a feeling that you will meet something big soon.",
      "No, that was not that big! To the next level!"
      };
    if(!anc) {
      if(P.curlevel < 0)
        addMessage("Congratulations! You have successfully completed the Tutorial.");
      else if(P.flags & dfChallenge)
        addMessage(msgs[2 + hrand(7)]);
      else if(P.curlevel < LEVELS-1)
        addMessage(msgs[P.curlevel]);
      else if(P.curlevel > LEVELS-1 && P.curlevel != 49)
        addMessage(msgs[2 + hrand(7)]);
      }
    playSound("other/tada", 100, 0);
    
    if(P.curlevel == 0) achievement("BEGINNER");
    if(P.curlevel == 0 && stats.wounds == 0) achievement("BEGINNERPERFECT");
    if(P.curlevel == 0 && P.race == R_ATLANTEAAN) {
      bool valid = true;
      for(int i=0; i<ITEMS; i++) if(stats.usedup[i]) valid = false;

      weapon *w =
        wpn[0]->size == 7 ? wpn[0] :
        wpn[1]->size == 7 ? wpn[1] :
        NULL;

      if(!w || stats.ws[MOT_BLADE].sc[WS_USE] != w->sc.sc[WS_USE])
        valid = false;

      for(int i=1; i<MOT; i++)
        if(i != MOT_BARE)
          if(stats.ws[i].sc[WS_USE]) valid = false;

      if(valid)
        achievement("PERFECTATLAS");
      }
    if(P.curlevel == 4) achievement("ADEPTSLAYER");
    if(P.curlevel == 99) achievement("DEEPEXPLORER");
    if(P.curlevel == 149) achievement("VDEEPEXPLORER");

    shareS("defeat", " all Hydras on Level "+its(P.curlevel+1));
    if(stats.wounds == 0) shareS("defeat", " all Hydras on Level "+its(P.curlevel+1)+" taking no wounds");
    }
  h = NULL;
  #ifdef ANDROID
  if(gameExists) emSaveGame();
  #endif
  }

void specialExtra(hydra *H);
bool specialMove(hydra *H);
bool breathAttack(hydra *H, int dir, bool doit);
void hydraAttack(hydra *H, vec2 pos, bool checkranged = true);

void hydraKnowDirty(hydra *H) {
  H->dirty &= ~IS_DIRTY;
  if(H->color < HCOLORS) {
    H->dirty &= ~(1<<H->color);
    H->dirty &= ~(1<<hyinf[H->color].weakness);
    }
  }

void hydraAttackPlayer(hydra* H, bool brother) {
  string amsg;
  
  inWaitMode = false;
  
  if(H->ewpn) {
    string verb = H->ewpn->info().hverb;
    if(verb[size(verb)-1] == 'h') verb += "e";
    amsg = "The "+H->name()+" "+verb+"s";
    }
  else if(H->color == (HC_ANCIENT | HC_DRAGON))
    amsg = "The "+H->name()+" "+hyinf[hrand(10)].dverb;
  else if(H->color & HC_DRAGON)
    amsg = "The "+H->name()+" "+H->info().dverb;
  else 
    amsg = "The "+H->name()+" "+H->info().hverb;
  
  amsg += " " + twinName(brother && !P.twinsNamed)+"!";
  
  if(H->invisible()) {
    int wnd = H->heads - H->sheads;
    if(wnd > 3) wnd = 3;
    amsg += " "+its(wnd)+" wnd";
    addMessage(amsg);
    
    takeWounds(wnd);
    stats.shadowwounds+= wnd;
    P.flags |= dfShadowAware;
    playAttackSound(NULL, H);
    if(shadowwarning) shadowwarning = 0;
    else shadowwarning = 10;

    int b = drainpower(H);
    H->heads += b*wnd, stats.vampire += b*wnd;
    }
  
  else if(true) {
    int hcount;
    
    if(H->ewpn) {
      hcount = H->ewpn->size;
      amsg += " " + H->ewpn->name() + " " + (H->ewpn->type + its(hcount));
      }
    else {
      hcount = H->heads - H->sheads;
      amsg += " " + its(hcount) + " hd";
      }
    
    if(hcount < 0) { addMessage(amsg); return; } // avoid crash bugs
    SI.prepare(hcount, H, true);
  
    if(SI.usize(hcount) && hcount >= SI.minss) {
      int val = hcount;
      amsg += " = ";
      int tucount = 0;
      while(SI.usize(val) && val >= SI.minss) { 
        int cusize = SI.usize(val);
        int ucount = 0;
        while(SI.usize(val) == cusize) val -= cusize, ucount++;
        amsg += its(ucount) + "*" + its(cusize);
        if(val) amsg += " + ";
        tucount += ucount;
        }
      if(val) amsg += its(val), tucount += val;
      amsg += " => ";
      amsg += its(tucount);
      }
  
    if(!H->ewpn) amsg += " => " + its(SI.dampre(hcount));
    stats.shieldprev += powerf(hcount) - SI.dampre(hcount);

    if(SI.reduce != -3) {
      if(SI.reduce>0) amsg += "-" + its(SI.reduce);
      if(SI.reduce<0) amsg += "+" + its(-SI.reduce);
      }
    
    int dmg = SI.dampre(hcount) - SI.reduce;
    
    if(dmg < 0) {
      if(stats.shieldheal >= P.curlevel) {
        SI.reduce = SI.dampre(hcount);
        }
    
      else {
        removeWounds(-dmg);
      
        stats.shieldheal -= dmg;
      
        if(stats.shieldheal >= P.curlevel) {
          addMessage("Hey, stop abusing this!");
          achievement("SHIELDABUSER");
          shareFixed("tried to abuse shields");
          }
        }
      }
    
    int nbase = P.race == R_NAGA ? 1 : 0;
    
    if(SI.reduce+nbase > 0) stats.shieldmatch += SI.reduce+nbase;
  
    amsg += " wnd";
  
    addMessage(amsg);
    hydraKnowDirty(H);
    playAttackSound(NULL, H);
    
    int dam = SI.dampost(hcount);
  
    takeWounds(dam);
    if(H->ewpn) stats.ettinwnd += dam;
    if(H->isAncient()) {
      if(P.curlevel < LEVELS)
        stats.ancientwnd += dam;
      else
        stats.ancientwnd2 += dam;
      }

    int b = drainpower(H);
    H->heads += b*dam, stats.vampire += b*dam;
      
    if(P.curHP <= 0) shareBe("killed by the "+H->name()+" after killing "+its(stats.hydrakill)+ " hydras");
    }
  }

void repelEttin(hydra *H, int by) {
  bool haveHydras = false;
  bool haveMushrooms = false;
  
  for(int dir=0; dir<DIRS; dir++) {
    vec2 pos2 = wrap(playerpos + dirs[dir]);
    cell& c(M[pos2]);
    if(c.h && !c.h->zombie && c.h->heads > c.h->sheads)
      haveHydras = true;
    if(c.mushrooms) haveMushrooms = true;
    }
  
  if(haveHydras) return; 

  if(P.race == R_HUMAN || P.race == R_CENTAUR) return;
  
  if(P.race == R_ATLANTEAAN && H->color == HC_MONKEY) return;

  if(P.race == R_ELF && !haveMushrooms) return;
  
  if(P.race == R_NAGA && nagavulnerable) return;
  
  if(P.race == R_TROLL) {
    bool vulnerable = true;
    for(int w=0; w<P.arms; w++) if(wpn[w]) {
      if(wpn[w]->type == WT_DANCE && wpn[w]->size >= H->heads)
        vulnerable = false;
      if((wpn[w]->cuts() || wpn[w]->stuns()) && wpn[w]->type != WT_MSL && wpn[w]->size == 2)
        vulnerable = false;
      }
    if(vulnerable) return;
    }
  
  for(int dir=0; dir<DIRS; dir++) {
    vec2 pos2 = wrap(playerpos + dirs[dir]);
    if(twin && !neighbor(pos2, twin->pos)) continue;
    cell& c(M[pos2]);
    c.dist += by;
    }
  }

bool canAttackBothTwins(hydra *H) {
  return H->color >= HCOLORS && !H->lowhead() && 
    P.curlevel >= (P.flags & dfChallenge ? GLEVELS/2 : LEVELS);
  }

void twinAttackBoth(hydra *H) {
  int hcount = H->ewpn ? H->ewpn->size : H->heads - H->sheads;

  shieldinfo twinSI;
  twinSI.prepare(hcount, H);
  
  twinswap();
  SI.prepare(hcount, H);

  int max = -1;
  for(int i=0; i<=hcount; i++) {
    int dmg = SI.dampost(i) + twinSI.dampost(hcount-i);
    if(dmg > max) max = dmg;
    }
  
  int count = 0;
  for(int i=0; i<=hcount; i++) {
    int dmg = SI.dampost(i) + twinSI.dampost(hcount-i);
    if(dmg == max) count++;
    }

  count = hrand(count);
  for(int i=0; i<=hcount; i++) {
    int dmg = SI.dampost(i) + twinSI.dampost(hcount-i);
    if(dmg == max) {
      if(count == 0) { max = i; break; }
      count--;
      }
    }

  if(max == 0) {
    twinswap();
    hydraAttack(H, playerpos);
    }
  else if(max == hcount) {
    twinswap();
    hydraAttack(H, twin->pos);
    }
  else {
    // save the number of heads
    // so that vampires work correctly
    int oheads = H->heads;
    H->sheads = H->heads - max;
    twinswap();
    hydraAttack(H, twin->pos);
    H->sheads = H->heads - (hcount-max);
    hydraAttack(H, playerpos);
    H->sheads = oheads - hcount;
    }
  }

void twinAttackPick(hydra *H) {
  int hcount = H->ewpn ? H->ewpn->size : H->heads - H->sheads;
  SI.prepare(hcount, H);
  int mine = SI.dampost(hcount);
  if(mine > P.curHP) mine = 2000;
  twinswap();
  SI.prepare(hcount, H);
  int twins = SI.dampost(hcount);
  if(twins > P.curHP) twins = 2000;
  
  if(twins == mine) {
    mine -= twin->heads;
    twins -= P.curHP;
    }

  if(twins == mine) {
    twins += hrand(2) ? 1 : -1;
    }
  
  if(twins > mine) {
    twinswap();
    hydraAttack(H, twin->pos);
    }
  else {
    twinswap();
    hydraAttack(H, playerpos);
    }
  }

void hydraAttackHydra(hydra *H, cell& c) {

  int pw = H->power();
  
  c.mushrooms -= pw;
  if(c.mushrooms < 0) c.mushrooms = 0;
  if(!c.h) return;
    
  weapon fake(H->info().atttype, pw, WT_BLADE);

  if(fake.size > c.h->heads) 
    fake.size = c.h->heads;
  if(c.h == twin) stats.twindef1++;
  c.attack(&fake, fake.size, H);
  hydraKnowDirty(H);
  }

bool isEnemy(hydra *H, hydra *H2 = NULL) {
  // if H2 is NULL, just check whether enemy to player
  if(!H2) {
    return H->conflict || !H->zombie;
    }
  else {
    if((H->invisible() || H2->invisible()) && !(H->isTwin() || H2->isTwin()))
      return false;
    return H->conflict || H2->conflict || (H->zombie ^ H2->zombie);
    }
  }

vector<cell*> traps; // traps activated in this turn

bool trapHits(cell *c) {
  weapon *t = c->trapped();
  if(!t) return false;
  if(!c->h) return false;
  if(t->type == WT_BLADE && c->h->heads < t->size)
    return false;
  if(t->type == WT_BLUNT && (c->h->heads-c->h->sheads) < t->size)
    return false;
  if(t->type == WT_DIV && c->h->heads % t->size)
    return false;
  return true;
  }

void trapHitPlayer() {
  weapon *t = M[playerpos].trapped();
  if(!t) return;
  addMessage("You are hit by the "+t->fullname()+"!");
  takeWounds(t->size);
  if(P.curHP <= 0) shareFixed("commited suicide with the "+t->fullname());
  if(P.curHP <= 0) achievement("SUICIDE");
  delete t; M[playerpos].it = NULL;
  }

void moveHydra(hydra* H) {
  vec2 playerwrap = wrap(playerpos);
  int morder[DIRS];
  for(int i=0; i<DIRS; i++) morder[i] = i;
  for(int i=0; i<DIRS; i++) swap(morder[i], morder[hrand(i+1)]);

  vec2 moveto = H->pos;
  int smell = H->power() + 1;
  int odist = DANGER * 2;
    
  if(H->color == HC_ETTIN) smell += 2;
  if(H->color == HC_MONKEY) smell += 30;
  if(H->color == HC_SHADOW) smell += 50;
  if(inWaitMode && waitsteps >= 1000) smell += 500;
  if(H->color == HC_TWIN) smell = 900;
  smell += H->awareness;

  if(DIRS == 4) {
    smell *= 2;
    smell += smell / 8;
    }
  if(hrand(3) == 0) {
    smell--;
    if(hrand(2)) {
      smell--;
      if(hrand(2)) smell--;
      // previously, a power N hydra would lose track of the PC in distance 2 with chance 1/3*2^(N-1);
      // currently, a big hydra never loses track
      }
    }
  
  // check if cornered
  bool cornered = H->color == HC_TWIN;
  if(cornered) {
    for(int i=0; i<DIRS; i++) {
      cell& c(M[H->pos + dirs[i]]);
      if(c.dist < DANGER && c.isPassable())
        cornered = false;
      }
    }
  
  if(H->color == HC_MONKEY) tryStealing(H, false);
    
  if(neighbor(H->pos, playerpos) && isEnemy(H) && !isFleeing(H)) {
    if(twin && neighbor(H->pos, twin->pos)) {
      stats.twindef2++;
      if(canAttackBothTwins(H))
        twinAttackBoth(H);
      else
        twinAttackPick(H);
      return;
      }
    else {
      if(twin) stats.twindef1++;
      sendAttackEvent(H->uid, H->pos, playerpos);
      hydraAttack(H, playerpos);
      }
    return;
    }
                                        
  for(int i=0; i<DIRS; i++) {
    vec2 mvec = dirs[morder[i]];
    vec2 vnw = H->pos + mvec;
    vec2 v = wrap(vnw);
  
    if(v == playerwrap) {
      if(H->color == HC_TWIN) {
        bool mirrored = false;
        for(int i=0; i<DIRS; i++) 
          if(wrap(vnw + dirs[i]) != wrap(playerpos + dirs[i]))
            mirrored = true;
        if(mirrored) achievement("MIRRORIMAGES");
        static bool hadmirrored = false;
        if(mirrored && !hadmirrored) hadmirrored = true, shareBe("now mirror images");
        }
      continue; // friendly, attacking was done previously
      }
  
    if(M[v].h && isEnemy(H, M[v].h)) {
      if(H->ewpn) {
        if(H->ewpn->size <= (H->ewpn->type == WT_BLADE ? M[v].h->heads : M[v].h->heads - M[v].h->sheads)) {
          if(M[v].h == twin) stats.twindef1++;
          M[v].attack(H->ewpn, H->ewpn->size, H);
          return;
          }
        }
      else if(H->color == HC_TWIN) {
        hydra *h2 = M[v].h;
        
        if(h2->invisible()) {
          // the twin cheats again: we assume they know
          // where the shadow is and they do not try to go in this direction
          continue;
          }
        
        if(h2->heads >= AMAXS) continue;

        // drunk Potion of Speed, do not attack
        if(P.twinspd) continue;

        twinswap();
        analyzeHydra(h2);
        twinswap();
        int spos;
        encode(h2->heads, h2->sheads, spos);
        
        // too stupid to attack that
        if(wnd[spos] == WMAX) continue;

        // waiting out is recommended, do that
        if(usew[spos] == -1) { return; }

        // do not get yourself wounded alone
        if(wnd[spos] && (!neighbor(v, playerpos) && !cornered))
          continue;

        // you are to attack now
        if(!havebit(P.twinarms, usew[spos])) {
          continue;
          }

        stats.twinai++;
        vec2 twinpos = H->pos;
        twinswap();
        bool hd = h2->res[wpn[usew[spos]]->color] < 0;
        sendAttackEvent(ATT_TWIN, twinpos, v);
        M[v].attack(wpn[usew[spos]], wpn[usew[spos]]->size, twin);
        attackEffect(wpn[usew[spos]], hd);
        cancelVorpal();
        twinswap();
        return;
        }
      else {
        if(!inWaitMode) sendAttackEvent(H->uid, H->pos, v);
        hydraAttack(H, v);
        return;
        }
      }
  
    if(M[v].isPassable()) {
      // if we can smell the PC, go towards them
      
      int dist = M[v].dist;
      
      if(M[v].trapped()) {
        if(!M[v].explored) continue;
        if(dist > smell || dist == 0) continue;
        if(H->color == HC_TWIN || H->color == HC_MONKEY || H->color == HC_ETTIN) 
          continue;
        if(H->color == HC_SHADOW && !(P.flags & dfShadowAware)) continue;
        }
      
      if(dist > smell) dist = DANGER-10;
      if(dist == 0) dist = DANGER-10;
      
      if(isFleeing(H)) dist = DANGER-1 - dist;
      
      if(dist > odist)
        continue;

      if(isFleeing(H) && nearPlayer(v))
        continue;
      
      if(M[v].dist >= DANGER && !cornered) continue;
    
      moveto = vnw; odist = dist;
      }
    }

  if(H->color == HC_TWIN && P.twinseed) {
    int q = (P.twinseed+6) / 7;
    M[H->pos].mushrooms += q;
    P.twinseed -= q;
    }

  if(specialMove(H))
    return;
  
  M[H->pos].h = NULL;
  H->pos = moveto;
  M[moveto].h = H;
  
  weapon *trap = M[moveto].trapped();
  if(trap) traps.push_back(&M[moveto]);
  
  if(H == twin) {
    twinswap();
    cancelVorpal();
    twinswap();
    }
  }

void popStairQueue() {
  if(stairqueue.empty()) return;
  if(wrap(stairpos) == wrap(playerpos)) return;
  cell& c(M[stairpos]);
  if(c.mushrooms) return;
  if(c.h) return;
  
  hydra *h = stairqueue[0];
  int n = size(stairqueue);
  for(int i=1; i<n; i++) stairqueue[i-1] = stairqueue[i];
  stairqueue.resize(n-1);

  addMessage("Your " + h->name() + " enters the level.");
  h->putOn(stairpos);
  
  if(h->color != HC_TWIN) {
    int hd = h->heads;
    int hh = headlossfun(hd, P.curlevel);
    if(hh < hd) {
      addMessage("It was too big to fit on the stairway!");
      h->heads = hh; h->sheads = 0; h->stunforce = 0;
      }
    }
  }

void moveHydras() {
  if(inWaitMode) 
    stats.waitturns++;
  else {
    stats.turns++;
    if(twin && P.twinmode) stats.turntwin++;
    }
  bfs(0); 
  
  traps.clear();
  
  for(int i=0; i<size(hydras); i++) {

    hydra* H(hydras[i]);
    if(H->power()) {
      if(H->lowhead()) repelEttin(H, +DANGER);
      if(H != twin) moveHydra(H);
      specialExtra(H);
      if(H->lowhead()) repelEttin(H, -DANGER);
      }
    H->stunforce -= H->sheads;
    if(H->stunforce < 0) {
      stats.awakenhd += H->sheads;
      bool next = false;
      if(M[H->pos].seen && H->sheads == H->heads && H->color != HC_SHADOW) {
        stats.awakenhyd++, next = true,
        addMessage("The "+H->name()+" wakes up.");
        playSound("other/wakesUp", 75, 0);
        }
      if(M[H->pos].seen && H->sheads < H->heads && H->color != HC_SHADOW) {
        stats.awakenhyd++,
        addMessage("The "+H->name()+" is fully awake again!");
        playSound("other/wakesUp", 100, 0);
        }
      H->stunforce = 0, H->sheads = 0;
      if(next) continue;
      }
    }
  
  for(int i=0; i<size(traps); i++) {
    weapon *t = traps[i]->trapped();
    if(!t) continue;
    if(trapHits(traps[i])) 
      traps[i]->attack(t, t->size, t);
    else {
      addMessage("The "+t->fullname()+" misses the "+traps[i]->h->name()+"!");
      playSound("weapons/miss", 100, 0);
      }
    if(!traps[i]->h) achievement("TRAPPER");
    delete traps[i]->it; traps[i]->it = NULL;
    }
  
  popStairQueue();  
  }

bool growHeads(hydra *h) {
  if(h->heads < AMAXS) {
    // try using the info from analyzeHydra
    int nh = 0; int nw = WMAX;
    for(int ah=h->heads+1; ah<AMAXS; ah++) {
      int acode;
      encode(ah, h->sheads, acode);
      int aw = wnd[acode];
      if(aw < nw) { nw = aw; nh = ah; }
      }
    if(h->lowhead() && nh > 2*h->heads) {
      addMessage("The "+h->name()+" partially resists your spell.");
      nh = 2*h->heads;
      }
    if(nh) { h->heads = nh; return true; }
    }
  
  // if too many heads or slaying was impossible, 
  // then just use the appropriate divisor
  // note that the closest match will be always used
  
  /*
  int div = 0;
  for(int w=0; w<P.arms; w++) 
    if(wpn[w] && wpn[w]->type == WT_DIV && wpn[w]->size > div && 
      h->res[wpn[w]->color] >= 0)
      div = wpn[w]->size;
  */
  
  /*
  int res = h->res[wpn[w]->color];
  
  int count = 0;
  while(h->heads >= 64) {
    h->heads /= div;
    h->heads += res;
    count++;
    }
  h->heads ++;
  while(count) {
    count--;
    h->heads -= res;
    h->heads *= div;
    }
  */
  
  int nhd;
  
  /* if(div) nhd = (h->heads/div) * div + div;
  else */
  {
    // try to cover Eradicator, Blade of Logaems, etc.

    for(int hb=1; hb<=100000; hb++) {
      for(int w=0; w<P.arms; w++) 
        if(wpn[w] && wpn[w]->xcuts() && h->res[wpn[w]->color] >= 0)
          if(wpn[w]->cutoff(h->heads + hb, false) > 0) {
            if(hb >= 100) hb = 100 + (hb-100) / 2;
            nhd = h->heads + hb;
            goto knowhd;
            }
      }

    return false;
    }
  
  knowhd:
  if(nhd > COLLAPSE) return false;
  
  h->heads = nhd;
  return true;
  }

int monkeymax() {
  if(P.flags & dfChallenge) return (2+P.curlevel) * 3;
  else return P.curlevel * 2;
  }

string hydra::describe() {
  if(color == HC_TWIN)
    return rinf[R_TWIN].desc;
    
  string stundesc = "";
  if(conflict)
    stundesc += " It looks confused.";
    
  if(sheads) {
    stundesc += "\nStunned heads (won't attack or pursue you): "+its(sheads)+ ", for ";
    if(stunforce >= sheads) stundesc += its(stunforce/sheads);
    if(stunforce%sheads) stundesc += " " + its(stunforce%sheads)+"/"+its(sheads);
    if(stunforce > sheads)
      stundesc += " turns";
    else stundesc += " turn";
    }
  
  
  if(zombie)
    return
      "This is a zombie, raised by your Powder of Fungal Necromancy. "
      "It is your slave, and does not question anything." + stundesc;
    
  if(color == HC_SHADOW && stats.backupsaves)
    return
      "Shadow hydras are invisible. Most slayers fighting them "
      "say that they had big problems, because they could not "
      "tell the number of heads.\n"
      "Of course, not you! You are quite sure that your intuition "
      "would allow you to guess the exact number of heads before "
      "the fight, if you wanted.\n";
  
  if(dirty & IS_DIRTY)
    return 
      "This hydra is covered with so much blood, fragments of mushrooms, dungeon dust, "
      "and other unidentifiable dirty things, that you cannot tell what is the type of "
      "this Hydra. You are quite sure that it is quite a normal Hydra, though. Seeing "
      "it attack will let you know the type, but still, you don't see the tiny marks "
      "which Hydra Slayers use to determine the number of heads regrown for each type of "
      "attack. To know that, you will need to either use this attack, or to "
      "drink a Potion of Knowledge (note that the potion used "
      "to identify bloody hydras does not do anything else)." + stundesc;

  if(heads == 1 && !lowhead() && color != HC_TWIN && color != HC_MUSH)
    return 
      "This 'hydra' has only one head. It should be enough to "
      "attack it with a dagger, and it will be dead. No problems "
      "with heads regrowing then." + stundesc;

  string s = info().hdesc;
  if(color == HC_ETTIN) {
    string t;
    if(P.race == R_ELF)
      t = "attack Elves only when they are not close to mushrooms";
    if(P.race == R_TROLL)
      t = "attack Titans when they wield no weapon which instantly kills or stuns them";
    if(P.race == R_NAGA)
      t = "attack Echidnae only after their *first* move";
    if(P.race == R_TWIN)
      t = "attack Twins one at a time";
    if(P.race == R_HUMAN)
      t = "are not afraid of Humans";
    if(P.race == R_CENTAUR)
      t = "are not afraid of Centaurs";
    if(P.race == R_ATLANTEAAN)
      t = "attack Atlanteans only when accompanied by a Hydra (but monkeys are not afraid)";
    s += "\nGiants "+ t + ".";
    }
  if(color & HC_DRAGON) {
    s += "\nSome hydras have learned how to attack their enemies from afar. "
      "These hydras are called dragons, even if they are not exactly "
      "the same as mighty one-headed dragons from other regions of the world.";
    }
  if(s.find("1N") != string::npos) {
    s.replace(s.find("1N"), 2, namenum(heads));
    }
  if(s.find("2N") != string::npos) {
    s.replace(s.find("2N"), 2, namenum(heads*2));
    }
  if(s.find("ML") != string::npos) {
    s.replace(s.find("ML"), 2, its(monkeymax()));
    }

  return s + stundesc;
  }

void prepareHah(hydra *H) {
  
  // don't take ambidexterity into account correctly
  // (calculations would take too long)

  int q = P.active[IT_PAMBI];
  if(P.curlevel < 75) // ... unless after level 75
    P.active[IT_PAMBI] = 0;
  
  if(H->lastwpnset != wpnset) {
    analyzeHydra(H);
    H->clearswnd(); H->lastwpnset = wpnset;
    H->swnd = new int[CODES];
    for(int i=0; i<CODES; i++) H->swnd[i] = wnd[i];
    }
  
  P.active[IT_PAMBI] = q;
  }

int headsafterhit(hydra *H, int hd) {
  if(H->swnd && hd < AMAXS) {
    int acode;
    encode(hd, H->sheads, acode);
    if(H->swnd[acode] < WMAX) return H->swnd[acode] * 100;
    }
  int best = hd;
  for(int i=0; i<P.arms; i++)
    if(wpn[i] && (wpn[i]->cuts() || wpn[i]->xcuts())) {
      int ah = hd;
      int co = wpn[i]->cutoff(ah, false);
      ah -= co;
      if(ah == 0) return 0;
      if(H->res[wpn[i]->color] < 0) continue;
      ah += H->res[wpn[i]->color];
      if(ah < best) best = ah;
      }
  return WMAX * 100 + best;
  }

bool nearPlayerOf(hydra *H, bool ntwin) {
  if(ntwin) return twin && neighbor(H->pos, twin->pos);
  else return neighbor(H->pos, playerpos);
  }

void specialExtra(hydra *H) {
  // not available if stunned
  if(H->sheads == H->heads) return;

  if(H->color == HC_GROW && M[H->pos].seen && H->heads <= 720) {
    prepareHah(H);
    if(headsafterhit(H, H->heads+1) > headsafterhit(H, H->heads)) {
      addMessage("The "+H->name()+" suddenly grows a head!");
      playSound("../hydra-old/rune", 25, 0);
      H->heads++;
      stats.ivygrow++;
      }
    }
  if(H->color == HC_WIZARD && nearPlayer(H) && H->heads < AMAXS) {
    if(H->heads == 1) return; // no mana left
    prepareHah(H);
    int nw = -1, nh = -1;
    for(int ah=H->sheads+1; ah<=H->heads; ah++) {
      if(ah < H->heads*3/4) continue; // looks too suspicious      
      int aw = headsafterhit(H, ah);
      if(aw >= nw) { nw = aw; nh = ah; }
      }
    if(nh < H->heads) {
      addMessage("The "+H->name()+" teleports "+its(H->heads-nh)+" of its heads away!");
      playSound("other/teleport", headsToVolume(H->heads-nh), 0);
      stats.wizout += (H->heads-nh);
      H->heads = nh;
      }
    }
  
  if(H->color == HC_MONKEY) tryStealing(H, true);
  }

void hydraAttack(hydra *H, vec2 pos, bool checkranged) {

  if(checkranged && (H->color & HC_DRAGON)) {
    breathAttack(H, neighborDir(H->pos,pos), true);
    return;
    }
  
  if(wrap(pos) == wrap(playerpos))
    hydraAttackPlayer(H, false);
  
  hydraAttackHydra(H, M[pos]);
  }

// doit==true: do breathe; false: just check
bool breathAttack(hydra *H, int dir, bool doit) {
  vec2 loc = H->pos;
  int hd = H->sheads;
  int hc = H->heads-hd;
  
  bool had = false;

  while(hc) {
    loc += dirs[dir];
    
    M[loc].ontarget = true;
    
    // breathe only on player, so if player does not see the location, then breathing
    // impossible
    if(!M[loc].seen && !doit) return had;

    if(M[loc].type == CT_WALL) return had;
    
    if(doit) {
      H->sheads = H->heads - hc;
      hydraAttack(H, loc, false);
      H->sheads = min(hd, H->heads); // in case if reached self ;)
      }
    else if((M[loc].h == twin && twin) || wrap(loc) == wrap(playerpos))
      had = true;
    
    hc >>= 1;
    }
  return had;
  }

bool specialMove(hydra *H) {
  // not available if stunned
  if(H->sheads == H->heads) return false;
  if(!M[H->pos].seen) return false;

  if(H->color == HC_WIZARD) {
  
    if(H->heads == 1) return false; // no mana left

    if(P.active[IT_PAMBI]) return false;
    vector<int> owncost;
    
    for(int hi=0; hi<size(hydras); hi++) if(hydras[hi] != H) {
      hydra *H2 = hydras[hi];
      if(!nearPlayer(H2)) continue;
      if(isEnemy(H, H2)) continue;
      if(H2->lowhead()) continue;
      if(!owncost.size()) {
        prepareHah(H);
        for(int i=0; i<(3+H->heads-H->sheads)/4; i++) {
          int aw = headsafterhit(H, H->heads-i);
          owncost.push_back(aw);
          }
        }
      int hcount = 0, bscore = 0, noop = 0;
      prepareHah(H2);
      for(int i=0; i<size(owncost); i++) {
        int aw = headsafterhit(H2, H2->heads+i);
        if(i==0) noop = aw;
        int cscore = (aw - noop) + (owncost[i] - owncost[0]);
        if(cscore > bscore) bscore = cscore, hcount = i;
        }
      if(hcount) {
        addMessage("The "+H->name()+" teleports "+its(hcount)+" heads to the "+H2->name()+"!");
        playSound("other/teleport", headsToVolume(hcount), 0);
        H->heads -= hcount;
        H2->heads += hcount;
        stats.wizport += hcount;
        stats.wiztimes++;
        return true;
        }
      }
    }
  
  if(H->color & HC_DRAGON) {
    // vec2 playerwrap = wrap(playerpos);
    int morder[DIRS];
    for(int i=0; i<DIRS; i++) morder[i] = i;
    for(int i=0; i<DIRS; i++) swap(morder[i], morder[hrand(i+1)]);
    
    // int shc = H->heads - H->sheads;
    
    for(int i=0; i<DIRS; i++) {
      int d = morder[i];
      if(breathAttack(H, d, false)) {
        breathAttack(H, d, true);
        return true;
        }
      }
    }

  return false;
  }

bool isFleeing(hydra *H) {
  // monkey with a weapon stolen flees unless cornered
  if(H->color == HC_MONKEY && H->ewpn && !canGoDown()) {
    if(!nearPlayer(H)) return true;
    for(int i=0; i<DIRS; i++) {
      vec2 v(H->pos + dirs[i]);
      if(!M[v].isPassable()) continue;
      if(!nearPlayer(v))
        return true;
      }
    return false;
    }
  return false;
  }

bool stealable(hydra *H, int id) {
  weapon *w = wpn[id];
  if(!w) return false;
  if(w->msl()) return false;
  if(w->size > monkeymax()) return false;
  bool nearit = nearPlayerOf(H, havebit(P.twinarms, id));
  if(!nearit) return false;
  return w->cuts() || w->stuns();
  }

void tryStealing(hydra *H, bool postmove) {
  if(H->color != HC_MONKEY || H->ewpn)
    return;
  if(nearPlayer(H)) {
    int best = -1, score = -1;

    tmprand t(P.gameseed * 513901301 + P.curlevel * 910510111);
    for(int i=0; i<60; i++) hrand(2);
    int likeblade  = hrand(10) * 100;
    int likestun   = hrand(10) * 100;
    int likeshield = hrand(10) * 100;

    if(likeshield < likestun) likeshield = likestun;
    
    for(int i=0; i<P.arms; i++) if(stealable(H, i)) {
      weapon *w = wpn[i];
      int nscore = w->size;
      if(w->cuts()) nscore += likeblade;
      if(w->type == WT_SHLD) nscore += likeshield;
      else if(w->stuns()) nscore += likestun;
      if(w->size == H->heads && postmove) 
        nscore += 50000;
      if(nscore > score) best = i, score = nscore;
      }
    
    if(best != -1) {
      swap(H->ewpn, wpn[best]);
      addMessage("The "+H->name()+" steals your "+H->ewpn->name()+"!");
      playSound("other/stealsYourWeapon", sizeToVolume(H->ewpn->size), 0);
      }
    }
  }

bool gfxused[64];

void hydra::setgfxid() {
  if(gfxid != 0) return;
  for(int u=1; u<64; u++) if(!gfxused[u]) {
    gfxid = u; 
    gfxused[u] = true;
    return;
    }
  }

void hydra::freegfxid() { gfxused[gfxid] = false; }
