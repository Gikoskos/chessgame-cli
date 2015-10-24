# chessgame-cli
## Overview

chessgame-cli is an incomplete chess game implementation in C. Incomplete because it doesn't support en passant
and checkmate doesn't work sometimes.

The thought process behind this project was for me to implement a chess game for the first time, without seeing
code from other projects or how implementations of chess programs are generally done.

If you find any bugs or encounter problematic behavior, either do a PR or file a bug report. To clear the screen
I'm using the system() function so make sure first noone has changed the 'clear' program in Unix or 'cls' in Windows
to something that can harm your computer since this game uses it a lot.

## How to play

You type the letter of the piece you want to move as you see it on the board and 
the coordinates of the square you want to move to (for example if you want to move Pawn to square 
A3 you type Pa3 or pa3 or PA3 or pA3), and the game will find the piece you want to move
and move it by itself.

If a King is in check you can't do moves that don't get King out of check, just like regular chess.

At the debug version you will see possible moves the King can make when he's threatened, or
when an enemy piece is threatening a square surrounding the King. This is mainly for debugging the check and
checkmate features.

A log file which records all the moves that took place, is created during the game.

With the Linux version if you type 'pieces' or 'PIECES' you will get the Unicode chess characters 
and if you type 'letters' or 'LETTERS' it reverts back to the capital letter representation.

## Building

If you're compiling on Windows, try using Mingw-w64 to install gcc and dmake (or make). It will probably won't compile as well with MSVC.

To compile and run on Windows with dmake (or make):

    dmake dll & dmake exe & chessgame-cli.exe
    
or if you're using Powershell:

    dmake dll;dmake exe;./chessgame-cli.exe

To compile and run on Linux:

    make && make run

To play against the computer (chooses only random moves for now) run:

    ./build/chessgame-cli -c

### License stuff

MIT License

See LICENSE for more information.
