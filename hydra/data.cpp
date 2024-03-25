// Hydra Slayer: math puzzle roguelike
// Copyright (C) 2010-2016 Zeno Rogue, see 'hydra.cpp' for details

#define LEVELS   12
#define LEVELS2  50

#define CLEVELS  10

struct levelinfo {
  int maxheads;
  int growlimit;
  int weapon;
  int bweapon;
  int heal;
  int msize;
  };

levelinfo linf[LEVELS] = {
  {  3,  3,  3, 3, 4, 1},
  {  6,  4,  4, 6, 6, 1},
  {  9,  5,  5, 7, 6, 2},
  { 12,  6,  6, 8, 7, 2},
  { 17,  7,  7, 9, 7, 3},
  { 23,  8,  8,10, 7, 4},
  { 30,  9,  9,11, 8, 5},
  { 40, 10, 10,12, 8, 6},
  { 50, 11, 11,13, 9, 7},
  { 60, 12, 12,14,10, 8},
  {256, 12, 12,15,25, 9},
  {  0, 12, 0,  0, 5,10}
  };

struct itemInfo {
  string name;
  char icon;
  int color;
  char hotkey;
  int value;
  const char *desc;
  };

#define ITEMS    18    // number of item types
#define IT_POTS  10    // first index of a potion
#define IT_SCRO  7     // first index of a scroll

#define IT_RSTUN 0
#define IT_RDEAD 1
#define IT_RCANC 2
#define IT_RGROW 3
#define IT_RCONF 4
#define IT_RFUNG 5
#define IT_RNECR 6
#define IT_SXMUT 7
#define IT_SGROW 8
#define IT_SPART 9
#define IT_PSWIP 10
#define IT_PCHRG 11
#define IT_PFAST 12
#define IT_PKNOW 13
#define IT_PARMS 14
#define IT_PSEED 15
#define IT_PAMBI 16
#define IT_PLIFE 17
#define IT_HINT  18


itemInfo iinf[ITEMS+1] = {
  { "Powder of Stunning", '$', 9, 's', 50,
    "This powder must have been made by Hypnos himself. "
    "Throw it at a hydra, and it will be completely stunned!\n"
    "Although this powder will affect every denizen of the Hydras Nest, "
    "larger ones are stunned for shorter time.\n"
    },
  { "Powder of Decapitation", '$', 7, 'd', 50,
    "Some people think that killing a stunned hydra is not honorable. What is your opinion?\n"
    "Also known as Powder of Death or the Charon's Obol, it causes all stunned heads of a hydra "
    "to be instantly moved to Hades.\n"
    "Rumored not to work on very powerful beings.\n"
    },
  { "Powder of Cancellation", '$', 15, 'c', 10,
    "These hydras, and their heads growing back... Is there anything which can cause the heads to never grow back?\n"
    "Yes, there is! The Powder of Cancellation removes all regenerative capabilities of a given hydra. "
    "Some very powerful hydras partially resist it, though.\n"
    },
  { "Powder of Growth", '$', 10, 'g', 3,
    "Made by Apollo himself, this powder gives the hydra additional heads. Who would use that? Well, master hydra slayers "
    "know that fewer heads is not always less trouble...\n"
    "The number of heads grown is chosen so that it will be possible to kill the hydra "
    "with current weaponry and the smallest amount of wounds possible.\n"
    "If there are several possibilities, one with the smallest number of new heads "
    "(greater than 0) is chosen. If it is impossible to slay hydra by growing heads, no heads are grown.\n"
    "For very large hydras (over 20000 heads), it tries to grow as many heads as needed to make you able to use one of your divisors or powerful weapons; "
    "if this would require more than 100 heads, only a part is grown.\n"
    "The Powder of Growth calculates the strategies in the same way as the Potion of Knowledge does.\n"
    },
  { "Powder of Conflict", '$', 12, 'o', 50,
    "Hydras are aggresive beasts, and Eris taught the master hydra slayers "
    "how to use this fact.\nThrow this powder at a target, and it "
    "will fight other hydras."
    },
  { "Powder of Fungification", '$', 3, 'f', 90,
    "Made by Demeter herself, this powder immediately turns a hydra into a mushroom. "
    "You have heard that this kind of unhonorable magic usually doesn't work on the most powerful "
    "of your enemies, and you suppose this is also the case for this powder."
    },
  { "Powder of Fungal Necromancy", '$', 8, 'n', 50,
    "Mixes a dead hydra and all mushrooms in sight into a friendly zombie hydra. "
    "This zombie will join your team and attack your enemies, but it will be unable to regrow heads naturally. "
    "Works only on the corpses of regular hydras."
    },
  { "Scroll of Transmutation", '?', 1, 't', 3,
    "Changes the material of the current weapon to the next one. The sequence of "
    "elemental materials is the same as is displayed by (f)ull info."
    },
  { "Scroll of the Big Stick", '?', 6, 'b', 10,
    "This prayer to Hephaestus increases the size of the current weapon or shield by 1."
    },
  { "Scroll of Reforging", '?', 10, 'r', 7,
    "Turns a part of your current weapon or shield into a new one. The new one might be of a different type.\n"
    "When combined with Ambidexterity, it is also possible to use this on several weapons of the same material, "
    "and have the biggest one of them steal exactly half of the size off each other one.\n"
    },
  { "Potion of Power Swipe", '!', 13, 'p', 25,
    "Allows one to attack several surrounding hydras with one swipe. For "
    "example, you can kill a 4-headed green hydra and cut "
    "off 3 heads of 6-headed red hydra with a single Scythe (-7) attack.\n"
    "Hydras are attacked in an anti-clockwise direction for the weapon in the "
    "right hand (1), and clockwise for the weapon in the left hand (2). "
    "On the last hydra, unstunned heads are affected first, even when using "
    "divisors and similar artifacts.\n"
    "Some brave or desperate heroes are rumored to end "
    "with cutting their own head, but you are not one of them.\n"
    "Centaurs are usually unable to use power swipes."
    },
  { "Potion of Weapon Charge", '!', 5, 'w', 3,
    "Allows one to attack several hydras in a straight line with a blade. For example, you can kill a 4-headed green hydra and cut "
    "off 3 heads of 6-headed red hydra with a single Scythe (-7) attack. If the last hydra in line is not completely killed, "
    "it is stunned for a short time."
    },
  { "Potion of Extreme Speed", '!', 4, 'e', 3,
    "Created by Hermes himself, this potion gives you double speed.\n"
    "Double speed lasts as long as you are just moving and doing things that take no time. "
    "When you attack or pick up an item, the effect is gone right away, although if you "
    "do this on your first move of two, you can still move a second time before the hydra "
    "attacks you."
    },
  { "Potion of Knowledge", '!', 14, 'k', 5,
    "Created by Athena herself, this potion gives hints about how to kill any visible hydra most efficiently with only your weapons.\n"
    "An efficient kill is one that gives you as few wounds as possible. In case of a tie, usually a faster method is chosen.\n"
    "For very large hydras (over roughly 20000 heads) it might not work at all, or (over roughly 100 heads) not recommend stunning while it should. "
    "It does not take your powders (including active ones) and missile weapons into account. Also assumes that "
    "stunning lasts for long enough, which is usually not true for shield bashing, and maybe even for emerald weapons.\n"
    "This potion works correctly for ambidextrous users, but might be really slow when you have a lot of choices.\n"
    },
  { "Potion of Power Juice", '!', 11, 'j', 1000,
    "Some people believe this potion will increase your chances against the hydras somehow.\n"
    "Other people believe this potion will decrease your chances of getting friends once you return to the society, "
    "because it makes you somehow more like a hydra.\n"
    "Also known as Potion of Strength."
    },
  { "Potion of Mushroom Spores", '!', 8, 'm', 2,
    "Originally created by Demeter to help with farming, "
    "but hydra slayers have found out that it is also useful in the dungeons.\n"
    "Those who drink this potion leave a trace of mushrooms behind them."
    },
  { "Potion of Ambidexterity", '!', 1, 'a', 30,
    "Allows you to attack with several weapons at once. A greater control of your arms comes as a cost of losing "
    "contol of your legs, so the effect wears off when you move (i.e. change position) after using the powers.\n"
    "If you attack with several weapons of the same color, the hydra regrows heads only once.\n"
    "Division takes precedence before subtraction. Missiles cannot be used.\n"
    "The Potion of Ambidexterity also allows using scrolls on several weapons at once. In this case, "
    "the potion can be used only for the scroll and nothing else.\n"
    "Ambidexterity is not compatible with Power Swipes and Weapon Charges.\n"
    "To help Titans, you keep the effects after moving if you have not used the powers yet.\n"
    },
  { "Potion of Life", '!', 2, 'l', 5000,
    "Who would be foolish enough to go to Olympus and steal some of "
    "their famous nectar? And then lose it in the dungeon?\n"
    "You have no idea, but you are happy that you have found it. "
    "Extremely valuable, the more of them you carry home with you, "
    "the richer you will be...\n"
    "This potions permanently doubles your health and heals you completely, and is believed to even "
    "be able to restore life.\n"
    },
  { "Scroll of Learning", '?', 14, 'l', 0,
    "No hint about this.\n"
    }
  };

// use up an item
bool useup(int ii, struct weapon *orb = NULL);

// letters available: ouvxyz

#define HCOLORS  10    // number of colors of hydras and blades
#define SCOLORS   4    // number of colors of bludgeons
#define COLORS (HCOLORS+SCOLORS+1)
// weapon colors
#define HC_OBSID 14

struct colorInfo {
  string wname;
  int color;
  int stunturns;
  const char *hverb;
  const char *soundfile;
  const char *hmessage;
  const char *wdesc;
  };

colorInfo cinf[COLORS] = {
  { "ash ", 6, 2, "hit", "elements/ash",
    "Your %s loses its shine and turns to wood!",
    "It is made of ash wood, which is believed to be effective against "
    "skeletons, vultures, and some hydras. Wooden weapons are lighter "
    "than other weapons, but that does not matter much to you."
    },
  { "chaos ", 13,  2, "hit", "elements/chaos", 
    "Your %s starts wildly mutating!",
    "One of the weapons created in the old times by the titans of Chaos to "
    "destroy the Golden Legion of Order; it emits a "
    "bright purple glow, and seems to constantly slowly change its shape."
    },
  { "flaming ", 12,  3, "burn", "elements/fire",
    "Your %s is suddenly consumed by flames!",
    "Even Hephaestus would be proud "
    "of this weapon. It is permanently covered with flames!\n"
    "You hope they burn the hydras' "
    "neck enough so that they don't grow new heads. Especially the icy "
    "ones.",
    },
  { "poison ",  10,  3, "cut", "elements/poison",
    "A green liquid extinguishes flames on your %s!",
    "Poison is dripping from it. Sometimes the poison is strong enough to "
    "prevent new heads from growing. For some reason, poison is most potent "
    "against chaos hydras."    
    },
  { "silver ",  15,  4, "cut", "elements/silver",
    "The poison on your %s dries off and you see bare metal!",
    "Although silver is a relatively soft metal, and usually not very good "
    "for weapons, it is great at slaying werewolves and some hydras."    
    },
  { "storm ",   11,  4, "shock", "elements/storm",
    "Sparks fly off your %s!",
    "Blessed by Zeus himself, it is crackling all the time; the famous "
    "hydra slayers who fought "
    "hydras in the swamps have said that this crackling power disrupts "
    "the hydras' regrowing ability, but it does not work so well in "
    "locations which are more dry."
    },
  { "acid ",    3,  5, "melt", "elements/acid",
    "You produce acid using your %s!",
    "It is covered by acid, which is especially good against the metallic "
    "white hydras."
    },
  { "frozen ",  9,  5, "freeze", "elements/frozen",
    "The acid on your %s freezes!",
    "It is extremely cold to the touch, it can freeze some hydra necks still "
    "before they regrow new heads."
    },

  { "bone ",    7,  6, "hit", "elements/bone",
    "The ice from your %s melts and bare bone is shown!",
    "According to your knowledge, weapons made of hydra bones are good against "
    "most hydras and beasts with storm powers."
    },

  { "golden ",  14,  6, "hit", "elements/gold",
    "Your %s is now made of pure gold!",
    "Gold is a heavy precious material with many special properties, including "
    "resistance to acid, and an ability to prevent head regrowth in some hydras. "
    },

  { "emerald ", 2, 15, "slam", NULL,
    "Your %s softens and turns green!",
    "Although heavy, emerald is a relatively soft gem. You wonder why a "
    "stunner has been made of it, there are so many harder gem types available."
    },
  { "amethyst ", 5, 30, "strike", NULL,
    "Your %s hardens and turns purple!",
    "Amethyst weapons are believed to be extremely good against alcohol-breathing "
    "monsters, but this won't help you since you have never heard about "
    "alcohol hydras. Still, amethyst is quite hard."
    },
  { "sapphire ", 1, 60, "smash", NULL,
    "Your %s hardens and turns blue!",
    "Sapphire is a very hard gem, great to stun your enemies with."
    },
  { "ruby ",    4,100, "crush", NULL,
    "Your %s hardens mightily and turns red!",
    "Ruby is not only one of the hardest gems by itself, but it also has magical "
    "properties which make it your best option if you want to stun someone for as "
    "long as possible."
    },  
  { "meteorite ", 8,300, "cut", "elements/meteorite",
    "Your mighty %s seems not affected!",
    "Iron weapons are mostly useless for hydra slaying, as all hydras "
    "always regrow two heads for each head cut with an iron weapon. "
    "But you somehow feel that this is not ordinary iron... you feel "
    "that it came from another world. Maybe this is the legendary "
    "material which can kill any hydra on Earth?"
    }
/*  { "obsidian ", 8,300, "cut",
    "Your mighty %s seems not affected!",
    "You have not heard about obsidian weapons during your hydra slayer "
    "training... but maybe this is the legendary material which can kill "
    "any hydra on Earth?"
    } */
  };

#define HYDRAS 21

#define HC_ALIEN   10
#define HC_VAMPIRE 11
#define HC_ETTIN   12
#define HC_ANCIENT 13
#define HC_GROW    14
#define HC_WIZARD  15
#define HC_MONKEY  16
#define HC_TWIN_R  17
#define HC_SHADOW  18
#define HC_MUSH    19 // mushrooms produce fake hydras when using RGROW
#define HC_EVOLVE  20
#define HC_TWIN    32
#define HC_50      33 // just for generation!
#define HC_DRAGON  64
#define HC_DRMASK  63

#define ANIM_PLUS 21
#define ANIM_HAMMER 22
#define ANIM_DUST 23
#define ANIM_ZIG 24
#define ANIM_CANCEL 25

#define ANIM_MAX 26

struct hydraInfo {
  string hname;
  int atttype;
  int color;
  int suscept;
  int weakness;
  int strength;
  const char *hverb;
  const char *dverb;
  const char *hdesc;
  };

hydraInfo hyinf[HYDRAS] = {
  { "were-", 0, 6, 4, 4, 8, "bites", "throws thorns at",
    "This hydra looks more like a mammal than a reptile. It is covered with "
    "thick brown fur, and its heads look similar to those of wolves, jackals, "
    "and giant rats. Some of them even look similar to human heads..."
    },

  { "chaos ", 1, 13, 3, 3, 9, "bites", "breathes Chaos at",
    "This hydra looks really strange. Each of its heads looks different, "
    "and some of them are changing while you watch them! That makes no "
    "sense. Anyway, it looks very dangerous."
    },
  { "fire ", 2, 12, 7, 7, -1, "burns", "breathes fire at",
    "This hydra is covered with hot red scales, and its jaws constantly "
    "release smoke and steam."
    },
  { "swamp ", 3, 10, 5, 5, 1, "bites", "spits poison at",
    "A nasty green liquid is dripping from the fangs of this hydra. It looks "
    "as if it came right from a swamp, since it is all wet and dirty."
    },
  { "white ",  4, 15, 6, 6, 0, "bites", "breathes silverdust at",
    "This hydra is covered by white scales. You wonder how it is "
    "cleaning its teeth, as its breath actually smells nice, and "
    "all of its 2N fangs look shining white, as if made of silver.",
    },
  { "storm ", 5, 11, 8, 8, 3, "bites", "zaps",
    "This hydra has a huge tail covered with a light cyan fur, "
    "which crackles and releases sparks as it is dragged on the ground. "
    "It reminds you of an urchin, since all of its 1N heads are supported "
    "on very long necks, which seem to be straightened by some unnatural force. "
    "Small lightnings appear whenever one of the heads gets close to a "
    "wall."
    },
  { "acid ", 6, 3, 9, 9, 4, "bites", "spits acid at",
    "A nasty liquid is dripping from the fangs of this hydra. It seems to "
    "burn the ground after it falls."
    },
  { "ice ", 7, 9, 2, 2, -1, "freezes", "spits ice at",
    "Although the dungeon is not so cold, this hydra is covered with ice. You also "
    "see very sharp icy crystals on its fangs."
    },

  { "skeletal ", 8, 7, 0, 0, 5, "bites", "breathes Death at",
    "You have seen hydra skeletons in the collections of your rich friends, "
    "but this one seems alive. Its main body seems to have lots of unnecessary "
    "bones... or maybe are they used to regrow heads?"
    },

  { "golden ", 9, 14, 1, 1, 6, "bites", "shines blindingly on",
    "This hydra glows like Sun, and you look forward to improving your armor "
    "by adding some of its golden scales."
    },

  { "alien ", 14, 2, 14, 10, -1, "cuts", "fires a laser at",
    "This hydra is not covered with scales, but with a deep green skin. "
    "Its only features are huge black eyes and razor sharp teeth. The "
    "legends say that such monsters have appeared after a strange "
    "magical star fell on the land, and they are somehow different than "
    "beings without extraterrestial heritage...\n"
    "Alien hydras have x2 against meteorite weapons and resist Cancellation."
    },
  { "vulture ", 14, 5, 0, 11, -1, "bites", "casts a spell at",
    "This hydra is a relative of the vulture who used to feed on the liver "
    "of Prometheus. After eating the liver which regenerated each day, "
    "it obtained its own special regenerative properties: it grows heads "
    "not only when attacked, but also when attacking.\n"
//  "The main body of this hydra is dark purple, but its heads are pale white, "
//  "with crimson red fangs. 
//  You feel that these fangs can suck out your "
//  "blood and use it for the hydra's own purposes.\n"
    "Vulture hydras gain a head for each enemy wound, and resist Conflict."
    },
  { "giant ", 14, 1, 12, 12, -1, "hits", "hits",
    "These two-headed giants are children of Geryon. "
    "They live in hills and caves close to the Hydras Pit, "
    "and sometimes a band of adventurous giants leaves their cave and "
    "causes destruction among the the peasants, who try to defend their "
    "possessions. Apparently such a band of giants has also entered the "
    "Hydras Pit, but contrary to the peasants, the hydras did not want "
    "to fight... probably two heads confused the hydras, and they "
    "thought of them as ones of their kind, and considered them to be friends. "
    "Your mission does not force you to kill giants, but still, they are friends "
    "with your enemies, and carry nice oversized weapons... that is, they would "
    "be nice if you wielded them. Giants are more nimble than hydras, and they "
    "are able to dodge ranged weapons. The powder of growth works on them, but "
    "it is not as powerful as on hydras. They do not attack as mindlessly as "
    "hydras; they attack only if they consider the target vulnerable."
    },
  { "ancient ", 14, 4, 14, 13, -1, "attacks", "reaches",
    "You did not know that such huge beasts exist! But they apparently do. It "
    "is much larger than any hydra you have seen or heard of, and its deep red "
    "scales and deep dark eyes look very old, but its teeth still look "
    "dangerous... You feel that this hydra is alone "
    "only because mundane hydras respect its age and size, and do not want to "
    "get into the way. You feel extremely unimportant compared to this huge beast."
    "\n"
    "Ancient hydra resists Fungification, Decapitation, partially Cancellation."
    },
  { "ivy-",     14,18, 14, 10, -1, "crushes", "breathes leaves",
    "A hydra with green plant-like skin. It seems to be able to regrow a head at "
    "will, and also to understand your weapons enough to know that growing an "
    "extra head is not always helpful."
    },
  { "arch-",     14,21, 14, 11, -1, "bites", "casts a spell",
    "All 1N heads of this hydra are surrounded by a glowing aura. You remember "
    "this glow... the nymphs whose teleportation tricks you have "
    "watched as a child glowed in the same way. But you think that this hydra "
    "won't use them to amuse you, but rather to make your job as hard as "
    "possible."
    },  
  { "monkey", 14, 1, 12, 12, -1, "punches", "throws feces at",
    "A pack of three-headed monkeys was rumored to once be brought from "
    "some remote island and live peacefully with humans for "
    "some time... until they noticed a hydra. From this point, "
    "they have decided that they are more alike hydras than humans. "
    "They have escaped and joined the hydras in the Hydras Pit... "
    "and they were lucky, Hydras accepted their friendship.\n"
    "This monkey looks as if it wants to sneak on you from behind, "
    "and seems to concentrate on your weapons. "
    "By looking at its muscles "
    "which are larger than those of giants, "
    "you think that it has already practiced, and that it could "
    "even carry a weapon of size ML.\n"
    "Just like giants, monkeys dodge ranged attacks, resist Growth, and "
    "attack only vulnerable targets.\n"
    },
  { "twin",      14, 13, -1, -1, -1,
    "<copied>"
    },
  { "shadow", 14, 8, -1, -1, -1, "touches", "wails at",
    "While shadow hydras have no natural instant regeneration ability, "
    "they have another power: invisibility. Even if you know where "
    "the shadow hydra is, you still don't know how many heads it has... "
    "even approximately, since they always attack with at most three heads to "
    "mask the total amount of heads.\n"
    "Shadow hydras fight only hydra slayers (hydras and other monsters "
    "will just ignore them, even if zombified or conflicted), and ignore bows.\n"
    "If your move is impossible due to presence of a shadow hydra with a "
    "head count not right for your selected weapon, you will 'miss' it "
    "and lose your turn.\n"
    "You can use a Potion of Knowledge on the shadow hydra. "
    "It won't tell you how many heads it has, or where exactly it is, "
    "but it will recommend the first three single attacks.\n"
    },
  { "mushroom", 14, 8, -1, -1, -1, "crushes", "throws spores at",
  "Mushrooms block your way, so "
  "you have to cut them before you proceed. They also have many uses, "
  "though. They block the way of hydras, and also provide additional "
  "\"heads\" which are useful with some of the special abilities, or "
  "for testing your weapons.\n" },

  { "evolving ", 14, 20, 10, 11, -1, "bites", "spits at",
    "This hydra rapidly evolves. "
    "Whenever you hit it with a weapon, it improves its resistance against it. "
    "Your further attacks with a weapon of the same material will cause it "
    "to regrow one head more. Even meteorite weapons are affected.\n\n"
    "Potion of Knowledge and Rune of Growth won't work reliably on evolving "
    "hydras, because they won't take the improving resistance into account."
    },
  };

#define MAXARMS    10

#define RACES   6
#define R_HUMAN 0
#define R_ELF   1
#define R_NAGA  2
#define R_TROLL 3
#define R_TWIN  4
#define R_CENTAUR  5

struct raceInfo {
  string rname;
  char rkey;
  char atsign;
  int color;
  const char *desc;
  };

raceInfo rinf[RACES] = {
  { "Human", 'h', '@', 7,
    "Humans are easy to learn, but hard to master.\n"
    "They are recommended for newcomers. "
    "Although they are probably not the easiest race to win, "
    "they are the easiest race to play.\n"
#ifdef ANDROID
    "(Especially on Android)\n"
#endif
    },
  { "Elf", 'E', '@', 9,
    "Elves have mastered fencing and archery. An Elf armed with a longsword (-5) can kill "
    "two 2-headed hydras and one 1-headed mushroom "
    "in one swipe. However, this power can only be used when the "
    "heads of multiple targets add up exactly to the size of the weapon, "
    "and only with normal blades (no divisors or axes). "
    "This special attack style "
    "makes them also unable to perform Ambidexterous attacks, even with a potion.\n"
    "Elves also start with an elven bow instead of a dagger, and more arrows than they will ever need. "
    "Shooting a hydra head turns it into two stunned heads. An attack hits all targets in the "
    "given direction that they can see... and they can see very well, looking far "
    "inside the underground mushroom forests.\n"
//  "Elves think that using magical Runes is not honorable. They convert them "
//  "to Scrolls of Transmutation instead of using them.\n"
    },
  { "Echidna", 'e', '@', 10,
    "Children of Echidna, with huge serpentine tails instead of legs. Although moving on such a "
    "tail is much slower than walking on legs, their monstrous nature allows them to attack with "
    "two weapons at once... or more. Echidna has been known to defeat her mighty enemies "
    "with just one or two such ambidextrous attacks.\nThis specialization on quick attacks comes "
    "at a cost, though: in battles which last longer than one turn, they are easier to hurt. "
    "Also, regenerative effect received from the blood of killed Hydras is not as strong "
    "as it is for other races, and using items is slower than for other races (especially "
    "the powders).\n"
    "(To select weapons 2, 3 and 5, press the following sequence of keys: 2235)\n"
#ifdef ANDROID
    "\nNote: Echidna might be risky to play on Android due to memory requirements"
#endif
    },
  { "Titan", 't', '@', 12,
    "Titans are too impatient to keep items other than weapons (such as powders, scrolls, "
    "and potions) for later, so they use them up immediately after pick up. Although this does "
    "not mean that they have to use the effect immediately (as they can walk a bit before the "
    "effect actually goes off), they are unable to keep any effects for another level (except "
    "Mushroom Spores).\n"
    "On the other hand, Titans value weapons very much, and are strong enough to carry lots of "
    "them in their inventory, instead of items. The most valuable titanic artifact weapon is "
    "rumored to be lost somewhere in the Hydra Pit. It is so heavy that only a Titan could "
    "pick it up, and is believed to allow them to overcome disadvantages caused by their "
    "impatience."
#ifdef ANDROID
    "\nNote: Titans are not yet adapted for Android!"
#endif
    },
  { "Twins", 'w', '@', 13,
    "You start with fewer HP and an ability to wield just one "
    "weapon. However, you have your twin with you, and you are both "
    "self-claimed experts at hydra slaying!\n"
    "You can press 's' to switch the twins at any time, and 'c' to switch "
    "between controlling one or both twins. When controlling only one, "
    "your twin will try to act reasonably, and should successfully guess "
    "your plans often.\n"
    "For convenience, the Twins share their items, so any of them can use an "
    "item, no matter which one has picked it up. They always drink potions "
    "of ambidexterity in pairs, because they have learned how to "
    "make joint ambidextrous attacks."
#ifdef ANDROID
    "\nNote: Twins might be a bit inconvenient to play on Android"
#endif
    },
  { "Centaur", 'c', '@', 9,
    "Centaurs have mastered archery, and thus they start with bows instead of daggers, "
    "and more arrows than they will ever need. "
    "Shooting a hydra head turns it into two stunned heads. An attack hits all targets in the "
    "given direction that the Centaur can see.\n"
    "Due to their equine bodies, it is hard for Centaurs to turn "
    "around, and thus they are not able to "
    "use the Potion of Power Swipe to attack several enemies around them "
    "at once.\n"
    }
  };

#define HELPLEN 9

const char* helpinfo[HELPLEN] = {

"\bjHydra Slayer v" VER " by Zeno Rogue <zeno@attnam.com>\n"
#ifndef STEAM
"released under GNU General Public License version 2 and thus "
"comes with absolutely no warranty; see COPYING for details\n"
#else
"(Steam)\n"
#endif

"\bgHydra Slayer is a Roguelike game focused on one thing: slaying Hydras. "
"It is inspired by Greek mythology, Dungeon Crawl, MathRL seven day "
"roguelike, and some mathematical puzzles about brave heroes slaying many "
"headed beasts.\n"

"\bkPress Enter to get help about keyboard. Then press Enter again for the "
"background history. Press Space to exit help. You can also use arrows.\n"

"\blIf you need more information about Hydra Slayer, see the official website:\n"
"\bohttp://roguetemple.com/z/hydra.php\n",

"\biThere are two keyboard layouts available (press '=' now to switch). "
"Use arrow keys, numpad, WASD, or extended VI keys to move and to attack Hydras "
"with your weapons and active powders. Use numbers (1, 2...) or [] keys to "
"switch your magical weapons of hydra slaying.\n",

"Hydra slaying has a long history.\n"
"First explorers who have met hydras were completely unable to do any harm. "
"As they cut hydras' heads, two new heads always grew in place of each one cut.\n"
"Some of them started using clubs instead of swords. However, this was only "
"a temporary solution. A club may stun a hydra, and make it temporarily less "
"dangerous, but not kill it. When time comes, it will wake up and attack "
"further. Hydras remained indestructible monsters.\n"
"Some wise men have been employed to research methods of slaying Hydras. "
"The first thing they have invented was the Powder of Decapitation, which could "
"kill a completely stunned hydra, or permanently destroy some heads of a "
"non-completely stunned one. Another thing were the Powder of Cancellation, which "
"could remove hydra's regenerative abilities. However, both of these Powders "
"were very expensive to create, and thus not practical.\n",

"Then, a report came, claiming that heads won't regrow if the sword was put "
"on fire. Apparently all Hydras in some region were killed using that method. "
"Their meat was found extremely tasty, their blood good for healing wounds "
"(due to hydras' natural regenerative abilities), and their scales good for "
"improving armors.\n"
"Wise men tried this method. Unfortunately, it did not work for them, though. "
"However, they have experimented with other kinds of magically imbued weapons. "
"It turned out that some enchantments will reduce the number of heads "
"regrown. Possibly even no heads will regrow. Some hydras could be slayed "
"after these findings, but not all. It is rumored that some mage long ago "
"created a weapon which could slay any Hydra without regrowing heads, but "
"he got overconfident and killed, and the secret was lost.\n",

"Wise men experimented with their magical weapons even further. And they had "
"a new result: weapons which could cut (or stun) several heads at once. If "
"they cut all heads, the hydra was killed. If not, the Hydra would usually "
"regrow the same number of heads as if only one head was cut. The mages were "
"happy with that major result.\n"
"Of course, some new hydra slaying weapons and magics were created later, "
"but they were nothing compared to the previous major achievement. The "
"numbers of Hydras kept decreasing, and finally, they disappeared, and "
"were almost forgotten... And fewer and fewer people became acquainted with "
"the hydra slaying techniques, as other people laughed at them, because "
"their profession was completely useless in these times.\n",

"Now you, a young student, have heard about a manifestation of hydras in so "
"called Hydras Nest. Apparently some brave warriors came to kill the Hydras... "
"Brave, but stupid. Even if they equipped themselves with mighty hydra slaying "
"weapons, they had no knowledge about how to use them correctly. What use is "
"a mighty sword which can cut ten heads in one swipe, against a Hydra which "
"has only 9 heads? All these knights have been killed.\n"

"You are more clever. Since your earliest "
"years you have had an unnatural ability to count people in an instant, and "
"your great grandfather has suggested you to pursue the Hydra Slayer career, "
"and learn the forgotten art of Hydra Slaying. Now, you will show people that "
"it is not just a useless funny thing. You take two small weapons and "
"some items, hoping to rob better ones off dead bodies of these stupid "
"warriors, and head to Hydras Nest. Of course, not that you are one of these "
"clever guys who are too scared and weak to be useful in a battle... you could "
"also wield a two handed weapon with one hand.\n",

"\bgHint: You can use command line options to set default values for the "
"options, such as default character name and default geometry, or to "
"use special features like starting the game in a debug mode or from a given "
"random seed.\n"
"\biIn the main menu, you have an option to choose your geometry. Your options "
"are: (4) allow only horizontal and vertical movement, (6) simulate a hex "
"board, (8) allow movement in all directions. You change the selection by "
"pressing D, Space, 4, 6, or 8. Press '3' to create a game with variable "
"geometry.\n"
"\biYou can move by pressing arrows (in 4 directions), "
"numpad (in all directions), YUBN (for diagonal movement, or vertical "
"movement in the 4-directional mode) and HJKL (for orthogonal movement).\n"
"\boYour selection affects "
"what directions are available to all actions, including your movement, "
"hydra movement, missile weapons, grids targetted by Power Swipes, and so "
"on. Additionally, (6) makes levels smaller (because only each second "
"character is used) and (4) makes Hydras better at smelling you (for "
"balance reasons).\n",

"\bgIf you are too bored to explore the Hydras Nest yourself, you can press "
"'o' for automatic exploration. It only works if there is no enemy in sight, "
"and will try to do the following, in order:\n"
"- pick up an inventory item you are standing on (unless a Titan),\r"
"- go to the nearest inventory item not behind mushrooms,\r"
"- go to the nearest non-explored space not behind mushrooms,\r"
"- go to the nearest inventory item through mushrooms,\r"
"- go to the nearest non-explored space through mushrooms,\r"
"- go to the stairway down.\n\n",

"Also, pressing O (shift+O) if there are no enemies on the level will collect "
"all items (or all weapons, for Titan). If there are enemies on level, it will "
"quickly pass turns until you are attacked, a hydra dies (useful with Powder of "
"Conflict or Fungal Necromancy), a hydra approaches you, or 2000 turns pass."

"\boArt, game design, texts, and programming by Zeno Rogue, 2010-2016.\n"
"\boSounds effects and music by Brett Cornwall <brett@brettcornwall.com>, under the Creative Commons BY-SA 4.0 license.\n"
//"\bjHydra Slayer uses the following music from Rogue Bard:\n"
//"* \"The naive Bard\" by Bushy (a cover by Mingos)\n"
//"* \"Azog's March\" by jice\n"
//"See more at http://roguebard.eptalys.net\n"
"\biThanks to Ancient, Xecutor, JLC, sbluen, cephalopid, Legend, "
"CommentLurker, tricosahedron, zulmetefza, Ryan Dorkoski, #16, "
"Bloodysetsail, vo3435, Patashu, Nick, and tehora "
"for their bug reports, suggestions, and other help!\n\n"
"\boAnd thanks to you for playing!"
};
