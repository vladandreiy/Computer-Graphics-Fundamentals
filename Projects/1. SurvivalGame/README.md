## Gameplay
The objective of the game is to get the biggest score possible by shooting the enemies before the player runs out of health. Enemies spawn randomly on the map, can move through walls/obstacle and follow the player on the map. The player cannot move out of bounds or through the walls. After 10 kills, the player progresses to another wave, where enemies spawn and move faster. Occasionally, a healthpack will show up randomly on the map. When the player dies, the score (the number of kills) is displayed in the console, and the player is given a chance to retry to get a bigger score.


## Implemented Functionalities
* Drawing player, enemies, map, healthpacks with basic geometry
* Player movement
* Player orientation
* Collisions (player - enemy, projectile - enemy, player - map, etc.)
* Score and Healthbar HUD and Functionality
* Enemies movement to follow the player
* Projectiles to match the player's orientation have a limited range
* Wave design gameplay - enemies move and spawn faster based on the current wave (3 waves)
