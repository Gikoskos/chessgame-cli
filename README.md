# Sample chess library
## Overview

The game itself is self-explanatory, but basically it's a very simple chess game for two players.

No AI implemented. You type the uppercase letter of the piece you want to move as you see it on the board and the coordinates of the square you want to move to (for example if you want to move Pawn to square A3 you type Pa3), and the game will find the piece you want to move and move it by itself.

A log file that records all the moves that took place, is created during the game.


## Prerequisites

Needs libncurses to compile.

Also colors only work on linux.

Would recommend to compile through the Makefile if you're on Linux, only after you've installed gcc and libncurses though.

## TODO

fix stupid bugs like this (FIXED)

![Alt text](http://i.imgur.com/u7DMUjg.png)

implement that dreaded piecesOverlap function

add the functionality for the game to end when a king is unable to make a move without being threatened by another piece

### License stuff

![WTFPL logo](http://www.wtfpl.net/wp-content/uploads/2012/12/logo-220x1601.png)

Copyright © 2015 G.K. <Gikoskos@users.noreply.github.com>

This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net for more details.
