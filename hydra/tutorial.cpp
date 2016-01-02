// Hydra Slayer: math puzzle roguelike
// Copyright (C) 2010-2012 Zeno Rogue, see 'hydra.cpp' for details

char tutorialmap[MSY][MSX+1] = {

  "##################################################",
  "#             rrrr   ...........j........r  OOO  #",
  "#  rrrrr     r....r  .            r.[[[[.r  ..O  #",
  "#  r.9..M..i..!!!!....  rrrrr     r.[[[[..k...O  #",
  "#  r...r     .!!!!. M  r.....r    r......r  .    #",
  "#  rr.rr     rr...r 1 r.......r    rrrrrr   .    #",
  "#    M         rrr  4  r..(..r              .    #",
  "#    .     rrr     ...  r.f.r               .    #",
  "#    .    r...rr  r...r   M                 .    #",
  "#  O....O r.....r  NNNr   M    rrrrr        l    #",
  "#    ?    r......r      .M... r.....r       .    #",
  "#  O..   r....3....M.g.M4M...eM..2..N.d     .    #",
  "#    h  r........r      .M... r.....r .   r...r  #",
  "#  O..  r.......r       .M...  rrrrr  .   r...r  #",
  "#    m  r......r                    ...   r...r  #",
  "#  O...  r.....r      rrrr          .     r.>.r  #",
  "#    ...  rr.rr      r....r    rrrrMM     r...r  #",
  "#    . ..   .        ..NN..c.M...1...r     rMr   #",
  "#    .  ..  M    ..bM..NN..    rMMMMMr      M    #",
  "#    ........    a   r....r     r....r  rrr...   #",
  "#                <    rrrr       rrrr   G.....   #",
  "##################################################"
  };

const char* tutorialTexts[13] = {
#ifdef ANDROID
  "Welcome to Hydra Slayer for Android!\n"
  "Note that Hydra Slayer is primarily designed for computers. For this reason,"
  "the game will tell you what key to press. You don't need to use a keyboard "
  "on Android, though. You can get the menu with all common actions by pressing 'MENU' on the "
  "screen; some actions can be also done with hardware buttons "
  "(for example, CALL to get/use). "
  "Yes/no questions are answered 'yes' by touching the top of the "
  "screen, and 'no' by touching the bottom.\n"
  "You will learn some basics of Hydra Slaying in this tutorial.\n"
  "Note that Hydra Slayer is a puzzle roguelike. Maybe you prefer "
  "to figure out the basic rules yourself? If so, go back to the "
  "stairs and touch MENU then 'get/use' to exit the tutorial.\n",

#else
  "Welcome to Hydra Slayer!\n"
  "You will learn some basics of Hydra Slaying in this tutorial.\n"
  "Note that Hydra Slayer is a puzzle roguelike. Maybe you prefer "
  "to figure out the basic rules yourself? If so, go back to the "
  "stairs and press 'g' to exit the tutorial.\n"
  "Note: for convenience, this tutorial tells you how to do things by "
  "using keyboard. You can also do them by pressing mouse: just move "
  "the mouse to the bottom right corner, and you will see icons "
  "to do all the most important actions (other than movement and weapon "
  "switching, which are done by clicking the map and the weapon list, "
  "respectively).\n"
  "If you feel lost, you can press '?' or F1 to get some Help.\n"
#ifdef NOTEYE
  "Press Ctrl+M to change system options (audiovisual etc.).\n"
#endif
  "Press Enter to close this message, and proceed "
  "to the next scroll to read it.\n",
#endif
  
  "There are some mushrooms before you. Mushrooms block your way, so "
  "you have to cut them before you proceed. They also have many uses, "
  "though. They block the way of Hydras, and also provide additional "
  "\"heads\" which are useful with some of the special abilities, or "
  "for testing your weapons.\n"
  "However, the one before you simply blocks your way. You can cut it "
  "simply by moving into it. Make sure that you have selected your dagger "
  "as a weapon (press '1' or click it on the list"
#ifdef ANDROID
  ", you can also use volume up/down buttons if you have them"
#endif
  ").\n"
  "One thing about movement. You can move with "
#ifdef ANDROID
  "hard buttons, touching the screen, "
#endif
   "arrow keys, numpad, "
#ifdef NOTEYE
  "mouse, "
#endif  
  "or with so-called \"vi keys\" (around J on qwerty). In this tutorial, the map is composed "
  "out of squares, and you can move in four directions. But for a real "
  "game, you can also choose 8-directional movement (allow diagonal movements "
  "for both you and hydras), or to have a map made of hexagonal cells. "
  ,
  
  "There is a hydra before you! Not a big deal, since it only has one head. "
  "It will be as simple to kill it as to cut a mushroom. Again, make sure that "
  "the dagger is selected, and you will kill it simply by moving into it.\n",
  
  "The next hydra has two heads. Note that you see the list of all hydras you "
  "see on your screen. When you select the dagger, you will see \"x2\" next "
  "to the hydra; this means that it grows two heads for each head you cut. "
  "So your dagger will cut one head and the hydra will regrow two, which is "
  "not helpful.\n"
  "You can also switch to your machete by pressing '2' or by clicking it "
  "on the list. The hydra would regrow one head (+1), but it does not matter "
  "(if you cut all its heads, the hydra is dead and does not regrow). Thus, "
  "select the machete, and you will be able to kill it easily.",
  
  "There is a four-headed hydra before you! If you check both your weapons, "
  "you will see that there is no way to kill this one. You will need to "
  "find a better weapon. Thanks to the mushrooms which block the hydra, you "
  "can try to explore the room to the North.\n",  
  "There is a shortsword before you, and it is just what you need! It is able "
  "to cut four heads at once, so you will be able to slay that four-headed "
  "hydra easily.\n"
  "Select your machete, go to the shortsword, and press 'g' to get it. "
  "You are currently only able to carry two weapons, one in each hand, "
  "so you will have to drop your machete. But the shortsword is better.",
  
  "There is a three-headed hydra before you! And it will be a problem. "
  "Hydra slaying weapons have magical enchantments which allow them to cut "
  "several heads at once, but there is a cost: it is impossible to attack a "
  "hydra which has less heads. So your shortsword will be useless, for now. "
  "You can attack with your machete, but the hydra will now have more than "
  "4 heads after it. If you think about it, you will see that it is impossible "
  "to kill it with your current weapons: it will always have an odd number "
  "of heads, and more than 1.\n"
  
  "Luckily you started with some tools to help with difficult cases. Your "
  "Powder of Stunning is able to stun all the heads of a single hydra completely. "
  "A stunned hydra cannot move or attack, but it would still regrow heads "
  "after an attack. However, you also have a Powder of Decapitation. It makes "
  "all the stunned heads disappear (and kill the Hydra if all the heads were "
  "stunned). Use the two powders in a combination, and the path will be clear!\n\n"
  "Press 'i' to see your inventory. To use the powder against the hydra, "
  "move next to it, press 'i', and select the powder to use it.\n",
  
  "The hydra before you might look difficult with its 9 heads. It regrows "
  "1 head after each attack with your shortsword, so it will have 3 heads "
  "after two attacks. However, it is still possible to defeat it with your "
  "current weapons (and without using powders). "
  "Find how to do this yourself! This is one of the two "
  "main challenges of Hydra Slayer: finding the combination of attacks "
  "which kills the given Hydra most effectively.\n"

  "Alternatively, you can also drink the Potion of Knowledge while the hydra "
  "is in your sight. It will show you the combination of attacks that it "
  "thinks is the best, and it will usually work correctly.\n",
  
  "Hopefully you now know the basics of Hydra slaying.\nIn the next chamber, "
  "there is a display of some items that are commonly used by Hydra slayers. "
  "You can pick up them and read what they do from your inventory ('i'). "
  "You can also press 'f'/'l' to get a list of all items, weapons, and hydras "
  "in your sight, and to get detailed information about each of them. "
  "Note that hydras will show the number of heads regrown when attacked by "
  "each of the 10 colors of blades.\n"
  "A hint: potions of Power Juice are very important! Drink them as soon as "
  "you find them, unless you really want a challenge.",
  
  "And the next chamber shows you some weapons. You will find the two most "
  "common kinds of weapons: blades (you already know these, they cut some "
  "heads and the hydra usually regrows some), and blunt weapons (these "
  "stun the hydra heads so they are unable to attack, but you will need "
  "to cut them with a blade to actually defeat them; on the good side, "
  "they are usually larger than blades, and will never cause hydras to "
  "grow more heads.\n",
  
  "There are more types of weapons available. The most successful slayers "
  "will be even able to find rare artifact weapons, with very special "
  "hydra slaying powers.\n"
  "But there is a general rule that the more powerful the weapon is, "
  "the less hydras it is able to affect. A shortsword cuts 4 heads, but "
  "it works only on "
  "hydras with at least 4 heads, and a Bisector takes half of the heads, "
  "but only if the number of heads is even.\nThe challenge is to find the "
  "set of weapons that makes you able to slay hydras as effectively as "
  "possible.",
  
  "There are stairs before you. They lead to the next level of the dungeon. "
  "Since your task is to slay all hydras, you can only use stairs if "
  "you have killed all the hydras on the current level.\n"
  
  "The stairs before you will take you into the beginning of the real game. "
  "Note that everything that you did in this tutorial does not count, "
  "and you will start with the usual set of items and weapons.\n"
  
  "Good luck, Hydra Slayer!",
  
  "Take care of your HP! Although it was possible to slay all the hydras "
  "so far without taking any wounds, it is not possible in general. You "
  "will lose HP when hydras attack you; you lose the game if it drops "
  "down to 0. On the other hand, successfully slaying a hydra will heal "
  "some of your HP, and also improve your maximum HP by 1. You do not "
  "regenerate HP automatically, so you will need to fight hydras (there "
  "are several other ways of healing, but they are very limited).\n"
  };

void generateTutorialLevel() {

  P.geometry = 4; P.race = R_HUMAN; setDirs();
  vorpalRegenerate();
  clearLevel();
  topx = TBAR, topy = TBAR;
  
  for(int y=0; y<MSY; y++) for(int x=0; x<MSX; x++) {
    char c = tutorialmap[y][x];
    
    vec2 v(x, y);
    cell& C(M[v]);
    
    hydra *h;
    weapon *w;
    
    switch(c) {
      case ' ': case '#':
        C.type = CT_WALL;
        break;
      
      case '.':
        break;
      
      case '<':
        C.type = CT_STAIRUP; stairpos = playerpos = v;
        break;
      
      case '>':
        C.type = CT_STAIRDOWN;
        break;
      
      case 'r':
        C.type = rand() % 2 ? CT_EMPTY : CT_WALL;
        break;
      
      case '1':
        h = new hydra(dcolors[1], 1, 5, 30);
        h->res[dcolors[0]] = 2;
        h->putOn(v);
        break;
      
      case '2':
        h = new hydra(dcolors[0], 2, 5, 30);
        h->res[dcolors[1]] = 1;
        h->putOn(v);
        break;
      
      case '3':
        h = new hydra(dcolors[3], 3, 5, 30);
        h->res[dcolors[0]] = 3;
        h->res[dcolors[1]] = 4;
        h->res[dcolors[2]] = 2;
        h->putOn(v);
        break;
      
      case '4':
        h = new hydra(dcolors[5], 4, 8, 30);
        h->res[dcolors[0]] = 5;
        h->res[dcolors[1]] = 6;
        h->res[dcolors[2]] = 0;
        h->putOn(v);
        break;
      
      case '9':
        h = new hydra(dcolors[4], 9, 5, 30);
        h->res[dcolors[0]] = 2;
        h->res[dcolors[1]] = 2;
        h->res[dcolors[2]] = 1;
        h->putOn(v);
        break;
      
      case 'G':
        h = new hydra(dcolors[6], 19, 5, 30);
        h->res[dcolors[0]] = 6;
        h->res[dcolors[1]] = 3;
        h->res[dcolors[2]] = 0;
        h->putOn(v);
        break;
      
      case '(':
        w = new weapon(dcolors[2], 4, WT_BLADE);
        w->putOn(v);
        break;
      
      case '?':
        (new item(IT_PKNOW))->putOn(v);
        break;
      
      case '[': {
        if(rand() % 2)
          w = new weapon(rand() % HCOLORS, 1 + rand() % 3, WT_BLADE);
        else
          w = new weapon(HCOLORS + rand() % SCOLORS, 1 + rand() % 5, WT_BLUNT);
        w->putOn(v);
        break;
        }
      
      case '!': {
        int arr[8] = { IT_RCANC, IT_RGROW, IT_SXMUT, IT_SGROW,
          IT_PSWIP, IT_PFAST, IT_PARMS, IT_PSEED };
        
        (new item(arr[rand() % 8]))->putOn(v);
        break;
        }
      
      case 'M':
        C.mushrooms = 1;
        break;
        
      case 'N':
        C.mushrooms = 2;
        break;
        
      case 'O':
        C.mushrooms = 5 + rand() % 5;
        break;
        
      default:
        if(c >= 'a' && c <= 'm')
          (new item(IT_HINT))->putOn(v);
        break;
      }
    }
  }

string item::describe() {
  if(type == IT_HINT) {
    for(int y=0; y<MSY; y++)
    for(int x=0; x<MSX; x++)
      if(M.m[y][x].it == this)
        return tutorialTexts[tutorialmap[y][x] - 'a'];
    }
  return iinf[type].desc;
  }
