import com.roguetemple.noteye.Noteye;

public class Sample
{
  Noteye n;
  char gamemap[][];
  boolean alive;
  
  int pcx, pcy;
  int woundcount;
  
  class Monster {
    int x, y, vx;
    char ch;
    int col32;
    };
  
  Monster mon[];
  
  public void getMapInfo() {
    n.table_new();
    for(int y=0; y<22; y++) {
      n.table_opensubAtInt(y);
      if(y+2 == pcy) {
        n.table_opensubAtInt(pcx);
        n.table_setInt("id", 0);
        n.table_closesub();
        }

      if(mon != null)
      for(int i=0; i<mon.length; i++) if(mon[i] != null && mon[i].y+2 == y) {
        n.table_opensubAtInt(mon[i].x);
        n.table_setInt("id", i+1);
        n.table_closesub();
        }
        
      n.table_closesub();
      }
    }
  
  public void initNoteye() {
    n.init();
    n.globalstr("noteyedir", "../../");

    
    final Sample _s = this;    
    n.callback = new Runnable() {
      Sample s = _s;
      public void run() { 
        // s.getMonsterId(n.argint(2), n.argint(3)); 
        s.getMapInfo();
        }
      };

    n.errorhandler = new Runnable() {
      Sample s = _s;
      public void run() { 
        // s.getMonsterId(n.argint(2), n.argint(3)); 
        System.out.println("b1="+s.n.error_b1+" b2="+s.n.error_b2);
        }
      };

    n.run("../../games/sample.noe", false);
    }

  public void initialize() {
    n = new Noteye();
    
    initNoteye();
    gamemap = new char[20][80];

    for(int y=0; y<20; y++) for(int x=0; x<80; x++)
      gamemap[y][x] = '#';

    for(int y=1; y<19; y++) for(int x=1; x<79; x++)
      gamemap[y][x] = '.';
    
    alive = true;
    pcx = 1; pcy = 10; woundcount = 0;
    
    mon = new Monster[200];
    int mids = 0;
    for(int y=1; y<19; y++) for(int x=70; x<79; x++) {
      Monster m = new Monster();
      m.x = x; m.y = y; m.ch = (char)('A'+y); m.col32 = 0xFF0000 + (0x1600-0x160000) * (x-70);
      m.vx = -1;
      mon[mids++] = m;
      }
    }

  public void movemonsters() {
    for(int i=0; i<mon.length; i++) if(mon[i] != null) {
      Monster m = mon[i];
      if(m.x == 78) m.vx = -1;
      if(m.x == 1) m.vx = +1;
      int nx = m.x + m.vx;
      int ny = m.y - 1 + (int) (Math.random() * 3);
      if(gamemap[ny][nx] == '#') continue;
      boolean blocked = false;
      if(pcx == nx && pcy >= m.y-1 && pcy <= m.y+1) {
        woundcount++;
        blocked = true;
        }
      for(int j=0; j<mon.length; j++) 
        if(mon[j] != null && mon[j].x == nx && mon[j].y == ny)
          blocked = true;
      if(!blocked) {
        m.x = nx;
        m.y = ny;
        }
      }
    }
  
  public void gameloop() {
  
    n.setTextAttr32(0xEFFC000, 0);
    
    n.mvaddstr(0, 30, "Sample Java Roguelike");
    n.mvaddstr(23, 30, "Number of wounds: "+Integer.toString(woundcount));
    
    for(int y=0; y<20; y++)
    for(int x=0; x<80; x++) {
      n.move(y+2, x);
      char c = gamemap[y][x];
      if(c == '.') n.setTextAttr(10, 0);
      if(c == '#') n.setTextAttr(6, 0);
      n.addch(c);
      }
    
    for(int i=0; i<mon.length; i++) if(mon[i] != null) {
      n.move(mon[i].y+2, mon[i].x); n.setTextAttr32(mon[i].col32, 0); n.addch(mon[i].ch);
      }

    n.move(pcy+2, pcx); n.setTextAttr(14, 0); n.addch('@');
    n.move(pcy+2, pcx);

    int ch = n.getch();
    if(ch == 'q') alive = false;

    // System.out.println(new Integer(ch).toString());
    // System.out.println(new Integer(n.DBASE).toString());
    
    if(ch >= n.DBASE && ch < n.DBASE+8) {
      int nx = pcx + n.dx[ch-n.DBASE];
      int ny = pcy + n.dy[ch-n.DBASE];
      
      if(gamemap[ny][nx] == '#') 
        return;
      
      for(int i=0; i<mon.length; i++) 
        if(mon[i] != null && mon[i].x == nx && mon[i].y == ny) {
          mon[i] = null;
          movemonsters();
          return;
          }

      pcx = nx;
      pcy = ny;
      movemonsters();
      }
    
    else if(ch == n.DWAIT || ch == ' ' || ch == '.')
      movemonsters();
    
    else if(ch == n.NOTEYEERR) {
      System.out.println("Reinitializing NotEye.\n");
      n.halt();
      initNoteye();
      }
    }
  
  public void finalize() {
    n.finishinternal(1);
    n.uifinish();
    n.halt();
    }

  public static void main(String[] args)
  {
    Sample s = new Sample();
    
    s.initialize();
    
    while(s.alive) s.gameloop();
    
    s.finalize();
  }
  
  }

