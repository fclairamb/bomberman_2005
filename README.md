# Bomberman student project of 2005

Backup of a bomberman game. I stumbled on it and decided to back it up on github before it disappears forever.

It was made in 2005 during my first year at [ECE](https://www.ece.fr/).

This game is quite important to me because both my girlfriend and my little sister have played with it intensively in the following years and they even added a few levels (that have been lost now).

This was a two-person project, I did most of the code (pretty much everything except the entrance menu), the NSIS installer and none of the assets (icons, images, sounds). 

The code is ugly but the game is fun and worked reliably on Windows XP (which is the only platform it was tested on).

This game relies as an interesting - but kind of crazy - idea: We are using pointers on each cell of the bomberman 2-dimentional table that, if they are below 100, are not pointing to anything but just describing a simple element to place there.

It's based on [Allegro](https://liballeg.org/) 4.2.
