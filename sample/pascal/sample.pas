program NotEyeSample;
uses NotEye;

type Monster = record
  x, y: Integer;
  c: char;
  color: Integer;
  down: boolean;
  end;

var mon: Array[0..50] of Monster;

var gamemap: Array[2..21, 0..79] of Char;

function getmapinfo(L: PLuaState): CInt; cdecl;
var y, i: Integer;
begin
  noteye_table_new(L);
  for y:=2 to 21 do begin
    noteye_table_opensubAtInt(L, y);
    for i:=0 to 50 do if mon[i].y = y then begin
      noteye_table_opensubAtInt(L, mon[i].x); 
      noteye_table_setInt(L, 'id', i);
      noteye_table_closesub(L); 
      end;
    noteye_table_closesub(L)
    end;
  getmapinfo := 1
  end;

procedure SampleError(id: CInt; b1, b2: PChar; param: CInt); cdecl;
begin
  WriteLn('NotEye error number ', id);
  end;

procedure initializeNoteye;
begin
  noteye_init;
  noteye_globalstr('noteyedir', '../../');
  noteye_globalfun('getmapinfo', @getmapinfo);
  noteye_handleerror(@SampleError);
  noteye_run('../../games/sample.noe', true);
  end;
  
procedure CreateMap;
var x, y: Integer;
begin
  for y:=2 to 21 do for x:=0 to 79 do
    gamemap[y][x] := '#';
  for y:=3 to 20 do for x:=1 to 78 do
    gamemap[y][x] := '.';
  end;

function monsterat(x: Integer; y: Integer): Integer;
var i: Integer;
begin
  monsterat := -1;
  for i:=0 to 50 do if (mon[i].y = y) and (mon[i].x = x) then
    monsterat := i;
  end;

procedure MoveMonsters;
var i, m, dx, dy: Integer;
begin
  for i:=1 to 50 do if mon[i].x > 0 then begin
    if mon[i].y = 3 then mon[i].down := true;
    if mon[i].y = 20 then mon[i].down := false;

    dx := Random(3) - 1;
    if mon[i].down then dy:=1 else dy:=-1;
    
    if (mon[0].y = mon[i].y+dy) and (mon[0].x >= mon[i].x-1) and (mon[0].x <= mon[i].x+1) then
      mon[0].color := 12;
    
    m := monsterat(mon[i].x+dx, mon[i].y+dy);
    if gamemap[mon[i].y+dy][mon[i].x+dx] = '#' then continue;
    if m = -1 then begin
      mon[i].x := mon[i].x + dx;
      mon[i].y := mon[i].y + dy;
      end;
    end
  end;

var i, x, y, c, sizex, sizey: Integer;
var nx, ny, ni: Integer;
begin

  Randomize;
  
  initializeNoteye;
  
  sizex := noteye_getinternalx;
  sizey := noteye_getinternaly;
  
  writeln('screen size: ', sizex, 'x', sizey);
  
  CreateMap;
  
  mon[0].x := sizex div 2;
  mon[0].y := 12;
  mon[0].c := '@';
  mon[0].color := 14;  
  
  for i:=1 to 50 do begin
    mon[i].y := 3;
    mon[i].x := 15 + i;
    if i < 26 then
      mon[i].c := Chr(ord('A') + i-1)
    else
      mon[i].c := Chr(ord('a') + i-26);
    mon[i].color := 1 + random(15);
    mon[i].down := true;
    end;
  
  while true do begin

    setTextAttr(15, 0);
    noteye_move(0, 25);
    noteye_addstr('Welcome to Sample Roguelike (Pascal)!');
    noteye_move(23, 35);
    noteye_addstr('Press q to die.');

    for y:=2 to 21 do for x:=0 to sizex-1 do begin
      noteye_move(y, x);
      if gamemap[y][x] = '#' then
        setTextAttr(8, 0)
      else if (y+x) mod 2 = 0 then
        setTextAttr(2, 0)
      else
        setTextAttr(10, 0);
      noteye_addch(gamemap[y][x])
      end; 
    
    for i:=0 to 50 do if mon[i].x > 0 then begin
      noteye_move(mon[i].y, mon[i].x);
      setTextAttr(mon[i].color, 0);
      noteye_addch(mon[i].c);
      end;
    
    noteye_move(mon[0].y, mon[0].x); 
    
    c := noteye_getch;
    if c = ord('q') then break;
    if (c >= DBASE) and (c < DBASE+8) then begin
      nx := mon[0].x + dx[c];
      ny := mon[0].y + dy[c];      
      ni := monsterat(nx, ny);
      
      if ni >= 0 then begin
        mon[ni].x := 0;
        MoveMonsters
        end
      else if gamemap[ny][nx] = '.' then begin
        mon[0].x := nx;
        mon[0].y := ny;
        MoveMonsters
        end
      end
    else if (c = DBASE+8) or (c = ord(' ')) or (c = ord('.')) then
      MoveMonsters
    else if c = NOTEYEERR then begin
      WriteLn('NotEye error detected, restarting NotEye');
      noteye_halt();
      initializeNoteye;
      end
    end;
  
  noteye_finishinternal(1);
  noteye_uifinish;
  noteye_halt;
  end.
