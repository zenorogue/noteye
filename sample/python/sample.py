#!/usr/bin/python
import noteye
import ctypes
import random

random.seed()

gamemap = ["#" * 80] + ["#" + "." * 78 + "#"] * 18 + ["#" * 80]

class Player:
      x = 40
      y = 12
      id = 0

player = Player()

ids = 0

class Monster:
      def __init__(self, x, y, char):
          global ids
          ids = ids+1
          self.id = ids
          self.x = x
          self.y = y
          self.char = char

monsters = [ Monster(70,15,"X"), Monster(60,10,"Y") ]

alive = True

def newmonster():
    x = random.randrange(1, 79)
    y = random.randrange(1, 19)
    c = chr(random.randrange(65, 91))
    if gamemap[y][x] == "." and findmonster(x,y) == None:
        monsters.append(Monster(x,y,c))

def findmonster(x, y):
    if player.x == x and player.y == y:
      return player
    for m in monsters:
        if m.x == x and m.y == y:
           return m
    return None

def killmonster(x, y):
    for i in range(len(monsters)):
        if monsters[i].x == x and monsters[i].y == y:
           monsters.pop(i)
           gamemap[y] = gamemap[y][:x] + "#" + gamemap[y][x+1:]
           return

def movemonsters():
    for m in monsters:
        mx = m.x
        my = m.y
        if mx > player.x: 
            mx = mx - 1
        if mx < player.x: 
            mx = mx + 1
        if my > player.y: 
            my = my - 1
        if my < player.y: 
            my = my + 1
        if mx == player.x and my == player.y:
            global alive
            alive = False
        elif gamemap[my][mx] == "#":
            pass
        elif findmonster(mx, my) == None:
            m.x = mx
            m.y = my

def getmapinfo(L):
#    x = noteye.argint(L,1)
#    y = noteye.argint(L,2)
    noteye.table_new(L)
    for y in range(0,20):
      noteye.table_opensubAtInt(L, y+2)
      for x in range(0,80):
        m = findmonster(x,y)
        if m:
          noteye.table_opensubAtInt(L, x)
          noteye.table_setInt(L, "id", m.id)
          noteye.table_closesub(L)
      noteye.table_closesub(L)
    return 1

getmapinfo = noteye.LUAFUN(getmapinfo)

def errorhandler(i,b1,b2,param):
    print("b1="+b1+" b2="+b2+"\n")

errorhandler = noteye.ERRORHANDLER(errorhandler)

noteye.errorhandler(errorhandler)

def noteye_initall():
  noteye.init()
  noteye.globalstr("noteyedir", "../../")
  noteye.globalfun("getmapinfo", getmapinfo)
  noteye.run("../../games/sample.noe", True)

def mainloop():
  while alive:
  
    for x in range(0,80):
        for y in range(0,20):
            c = gamemap[y][x]
            if c == "#":
                noteye.settextattr(4, 0)
            else:
                noteye.settextattr(8, 0)
            noteye.move(2+y, x)
            noteye.addstr(gamemap[y][x])
    
    noteye.move(2+player.y,player.x)
    noteye.settextattr(14, 0)
    noteye.addstr("@")
    
    for m in monsters:
        noteye.move(2+m.y, m.x)
        noteye.settextattr(13, 1)
        noteye.addstr(m.char)
  
    noteye.move(0,20)
    noteye.settextattr32(0x9090FF, 0)
    noteye.addstr("Sample Roguelike in Python!")
    
    ch = noteye.getch()
    
    if noteye.isdir(ch):
      dx,dy = noteye.dxy(ch)
      pcx = player.x + dx
      pcy = player.y + dy
      if findmonster(pcx, pcy):
          killmonster(pcx, pcy)
      elif gamemap[pcy][pcx] == "#":
          pass
      else:
          player.x = pcx
          player.y = pcy
      
      movemonsters()
      if random.random() < 0.1:
          newmonster()
    elif ch == ord("."):
      movemonsters()
      newmonster()
    elif ch == ord("q"):
      print("Exiting the game.")
      break
    elif ch >= 0 and ch < 256:
      print("Key pressed: character "+chr(ch))
    elif ch == noteye.NOTEYEERR:
      print("Reinitializing NotEye due to error")
      noteye.halt()
      noteye_initall()
    else:
      print("Other key pressed: "+str(ch))

noteye_initall()
mainloop()
noteye.finishinternal(1)
noteye.uifinish()
noteye.halt()
