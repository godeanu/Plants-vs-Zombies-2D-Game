# Plants-vs-Zombies-2D-Game
OpenGL C++ project inspired by the Plants vs Zombies game.

## Description

This project is a 2D game made using OpenGL and C++, inspired by Plants vs Zombies. Instead of plants we use rhombuses and instead of zombies we use hexagons.

Each rhombus has a color and a cost (can only be placed in a square if the player has enough money for it). The money system works by picking up stars that fall at random times on the ground. The hexagons also have colors and a certain one can only be killed by a rhombus with the same color. The rhombuses shoot bullets (stars) of their own color. A rhombus dies after being touched by three hexagons. Also, a hexagon dies after being shot by three bullets.

Behind the plants there is red area which we must protect. If it is reached by a hexagon, we lose a life out of three. When we lose all of them the game ends in a defeat.

More not-so-important information: The game has multiple animations and effects (zoomed-out rhombuses when the plant dies and zooomed-out stars when losing a life, rotating bullets, rotating enemies, etc)

For the framework used, check this [repository](https://github.com/UPB-Graphics/gfx-framework).
