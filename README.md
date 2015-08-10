# Sample chess game
## Overview

The game is self-explanatory, but basically it's a very simple chess game for two players.

No AI implemented yet. You type the uppercase letter of the piece you want 
to move as you see it on the board and the coordinates of the square you want to move to 
(for example if you want to move Pawn to square A3 you type Pa3), and the game will find the piece you want to move
and move it by itself.

A log file that records all the moves that took place, is created during the game.

The thought process behind this project was for me to implement a chess game for the first time, without seeing
code from other projects.

There might be tons of other implementations that are probably way smarter than mine but even so, I just did this to have fun :D

## Prerequisites

Needs libncurses to compile. (on Linux)

If you're compiling on windows, try using Mingw-w64 to install gcc. It's the only compiler for Windows I've tested it on.

Would recommend to compile through the Makefile, if you're on a linux distro, only after you've installed
gcc, make and libncurses though.

## TODO

* fix stupid bugs like this (DONE)

 ![Alt text](http://i.imgur.com/u7DMUjg.png)

* implement that dreaded piecesOverlap function (DONE)

* fix bug on Windows that rejects input other than the first character after some time (DONE)

* add the functionality for the game to end when a king is unable to make a move without being threatened by another piece (WIP)

* fix this bug that only happens with black Pawns (DONE)

  <img src="http://i.imgur.com/cVGe6Sd.png" alt="step1" width = "257" height = "341"/> <img src="http://i.imgur.com/mkwlxOY.png" alt="step1" width = "257" height = "341"/>

* port the main engine to other languages

### License stuff

![GPLv3 logo](http://www.gnu.org/graphics/gplv3-127x51.png)

See COPYING for more information.

Copyright © 2015 G.K. <cyberchiller@gmail.com>
