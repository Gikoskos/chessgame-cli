# ChessLib
## Overview

ChessLib is a chess game implementation in C. It can be used as a library, to simulate a simple chess game with
almost all rules (en passant doesn't work at the moment). Only use it for testing purposes please, since it's at a very 
early stage of development. To test out the engine you can run chessgame which is a console-based game 
running on ChessLib. Note that for now it only supports human players and a random-move computer player.

The thought process behind this project was for me to implement a chess engine for the first time, without seeing
code from other projects or how implementations of chess engines are generally done.

If you find any bugs or encounter problematic behavior, either do a pull-request or file a bug report (issues on github).
If the above methods won't work for you try contacting me on my e-mail.

## How to play

You type the letter of the piece you want to move as you see it on the board and 
the coordinates of the square you want to move to (for example if you want to move Pawn to square 
A3 you type Pa3 or pa3 or PA3 or pA3), and the game will find the piece you want to move
and move it by itself.

The game ends if the king has no moves to make, but it won't prevent you from moving the King to
a square that will threaten him, so it can end after the other player directly captures the King.
At the current version you will see possible moves the King can make when he's threatened, or
when an enemy piece is threatening a square surrounding the King. This is mainly for debugging purposes
since the check and checkmate features are at a very alpha stage.

A log file that records all the moves that took place, is created during the game.

To play against the random move generator run:

    chessgame-cli -c

With the Linux version if you type 'pieces'/'PIECES' you will get the Unicode chess characters 
and if you type 'letters'/'LETTERS' it reverts back to the capital letter representation.

## Building

Needs libncurses to compile on Linux.

If you're compiling on Windows, try using Mingw-w64 to install gcc and dmake (or make). It will probably won't compile as well with MSVC.

To compile and run on Windows with dmake (or make):

    dmake dll & dmake exe & chessgame.exe
    
or if you're using Powershell:

    dmake dll;dmake exe;./chessgame.exe

To compile and run on Linux:

    make && make run
    
To compile with the random-move computer player on Linux:

    make AItests && ./tests/AItests

I will be releasing precompiled binaries made on Ubuntu 14.04 i386 
and Windows 7 32bit, when bugs are fixed or something in the UI changes.

## TODO

* add versioning

* add functionality for the engine to detect checkmate and end the game (WIP)

* fix this weird bug that only happens with black Pawns (DONE)

  <img src="http://i.imgur.com/cVGe6Sd.png" alt="step1" width = "257" height = "341"/> <img src="http://i.imgur.com/mkwlxOY.png" alt="step1" width = "257" height = "341"/>

* prevent the King from moving to a square that might threaten him (WIP)

* program a computer(AI) player (WIP)

* make a test GUI chess game using ChessLib

* split all the command line specific functions (such as printBoard) from the main chess engine and rewrite the Makefile accordingly

* port the main engine to other languages

### License stuff

![GPLv3 logo](http://www.gnu.org/graphics/gplv3-127x51.png)

See COPYING for more information.

Copyright © 2015 G.K. <cyberchiller@gmail.com>
