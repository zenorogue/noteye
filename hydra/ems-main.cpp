void ems_main(void_continuation vcon) {
  hydraUserdir("idbfs");
  P.curlevel = 0; 
  P.gameseed = time(NULL);
  randgen.seed(P.gameseed);
  
  pinfo.twin[0] = "Castor";
  pinfo.twin[1] = "Pollux";

  P.geometry = 4;
  pinfo.username = "Heracles";
  pinfo.charname = "Heracles";

  initScreen();
  loadGame();
  if(gameExists) {
    initGame();
    mainloop([=] { mainmenu(vcon); });
    }
  else {
    clearGame(); 
    mainmenu(vcon);
    }
  }

void INIC(int x) {
  string keytable;
  if(x == IC_GAME) {
    keytable += "<table style=\"font-size:200%;\">";
    keytable += "<tr>";
    auto akey = [&] (string kname, string show) {
      keytable += "<td><a onclick=\"webkey('" + kname + "')\">" + show + "</a></td>";
      };
    auto nokey = [&] () { keytable += "<td></td>"; };
    auto longb = [&] () { keytable += "<td>&nbsp;&nbsp;&nbsp;</td>"; };
    int dir = DIRS;
    if(dir >= 6) akey("Home", "↖"); else nokey();
    if(dir != 6) akey("ArrowUp", "⬆"); else nokey();
    if(dir >= 6) akey("PageUp", "↗"); else nokey();
    longb();
    akey("g", "get/go (g)");
    longb();
    if(P.race == R_TROLL)
      akey("G", "troll (G)");
    else if(P.race == R_TWIN)
      akey("c", "twin control (c)");
    else
      akey("o", "explore (o)");
    keytable += "</tr>";

    keytable += "<tr>";
    akey("ArrowLeft", "⬅");
    akey(".", "⟲");
    akey("ArrowRight", "➡");
    longb();
    akey("i", "inventory (i)");
    longb();
    akey("v", "weapon (v)");
    
    keytable += "</tr>";

    keytable += "<tr>";
    if(dir >= 6) akey("End", "↙"); else nokey();
    if(dir != 6) akey("ArrowDown", "⬇"); else nokey();
    if(dir >= 6) akey("PageDown", "↘"); else nokey();
    longb();
    akey("f", "information (f)");
    longb();
    akey("h", "help (h)");
    keytable += "</tr>";
    keytable += "</table>";
    }
  else if(x == IC_MYESNO) {
    keytable = "<font style=\"font-size:300%\"> <a onclick=\"webkey('y')\">Yes</a> <a onclick=\"webkey('y')\">No</a></font>";
    }
  else {
    keytable = "<br/><br/><br/>";
    }
  set_value("keytable", keytable);
  }

