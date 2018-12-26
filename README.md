# Warcraft III: The Frozen Throne GAME TRAINER
## Note
This repository just for education. Application is working, but unfortunately it works with particular version of game and I don't remember this version and which patches were applied. I believe it was 1.24a or something like that. This app won't work with another version of game, because app using few absolute addresses of process, which are different for each version of application.

## Overview
This trainer will increase points of following abilities: Strength and Agility; Intelligence isn't supported.

Steps to use:
* Run game
* Run trainer
* Start any map
* Buy any item which increase Strength or Agility
* Drop item
* Press NumPad5
* Take item
* You should hear beep signal from motherboard when desire memory address captured

Next steps may require to drop and take item to take effect (increase points of attributes):
* NumPad1 - add current value of points to Strength points
* NumPad2 - add current value of points to Agility points
* NumPad7 - increase current value of points by 1
* NumPad8 - decrease current value of points by 1
* NumPad6 - inverse value stored in target address during NumPad1 and NumPad2 actions. For internal technical purposes
* NumPad6 - close trainer
