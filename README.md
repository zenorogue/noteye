#NotEye
Necklace of the Eye (NotEye in short) is a roguelike frontend. It can
modify the output of console and libtcod roguelikes, and it also works
as a library which allows to easily port old ASCII roguelikes like ADOM 
into 21th century. NotEye is a complicated piece of software, and
it is constantly in development, and not documented very well. This README
contains just the basic information; see http://roguetemple.com/z/noteye.php
for many more details.

#Hydra Slayer
Hydra Slayer is a Roguelike game focused on one thing: slaying Hydras. It
is inspired by Greek mythology, Dungeon Crawl, MathRL seven day roguelike,
and some mathematical puzzles about brave heroes slaying many headed beasts.
Hydra Slayer features an original combat system, based on mathematical
puzzles. Again, this README contains just the basic information; see
http://roguetemple.com/z/hydra.php for more information about Hydra Slayer.
You can also start the game and press '?' to get Help, or press 'T' to
start the Tutorial.

Since Hydra Slayer is a flagship roguelike for NotEye, the two are
distributed together.

#Compilation, installation, and usage
##For Windows users

If you just want to play Hydra Slayer, download the NotEye+Hydra Slayer package
from [the official site](http://www.roguetemple.com/z/noteye/download.php). 

If you want to play a game which supports NotEye very well
(other than Hydra Slayer), you can also download ADOM from [its official site](http://adom.de/).

If you want to try NotEye's capability of running with various games (such as DoomRL
or Brogue), and you are using Windows, the Bundle is what you want -- it can be also
found on the [official site](http://www.roguetemple.com/z/noteye/download.php).

If you want to compile NotEye and Hydra Slayer after downloading it (from GitHub or the
official site), execute 
    make -f Makefile.mgw
in the "src" subdirectory. You can also execute the same command in the "hydra"
subdirectory to compile the console only version of Hydra Slayer.

If you want to use NotEye for the game you are developing, please read the 
[dev guide](http://www.roguetemple.com/z/noteye/dev.php) and download one of the two
packages (depending on your OS and the number of working examples you want). Please
remember about the licensing secion in the dev guide!

##For Linux (Ubuntu)

Clone the GitHub project with:
    git clone https://github.com/zenorogue/noteye noteye

Install the dependencies:
    sudo apt-get install libsdl2-dev liblua5.1-0-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-net-dev libncurses5-dev

Change the directory:
    cd noteye
    cd src

Create the "noteye" executable (and the "hydraslayer" script):
    make

Install both NotEye and Hydra Slayer on your system:
    make install

If you want to use NotEye for the game you are developing, please read the 
[dev guide](http://www.roguetemple.com/z/noteye/dev.php). Also, please
remember about the licensing secion in the dev guide!


#Credits
NotEye is distributed under GNU General Public License version 3 or later.
It comes with absolutely no warranty; see the file COPYING for details.

See the files in the "licenses" subdirectory for the licenses of third
party libraries used by NotEye.

See gfx/license.txt and sound/license.txt for information about sources and
licensing of the graphical images and sounds and music, respectively.

