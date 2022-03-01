## Gameplay
The objective of the game is to get the end of the maze (marked as a yellow square) before the time runs out. There are enemies on the map, which will harden the player's progression through the maze, as you will have to shoot them or you will lose health and eventually die if you hit too many of them. Enemies spawn randomly on the map, move only in their designated space and have a cool animation when they die. When the player runs out of time or health, he is given a chance to retry to finish the maze. If the player manages to get out in time, the time remaining is displayed. 
If you are lost in the maze, the maze map can be displayed in the console by pressing 'H':
Maze Hint:
▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓
▓▓******▓▓**    ▓▓  ▓▓
▓▓  ▓▓▓▓▓▓  ▓▓▓▓▓▓  ▓▓
▓▓  ▓▓      ▓▓  ▓▓  ▓▓
▓▓  ▓▓**▓▓▓▓▓▓  ▓▓**▓▓
P **▓▓  ****▓▓**▓▓  ▓▓
▓▓  ▓▓  ▓▓▓▓▓▓**▓▓**▓▓
▓▓    **▓▓      ▓▓  ▓▓
▓▓**▓▓▓▓▓▓**▓▓▓▓▓▓  ▓▓
▓▓**    ****E       ▓▓
▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓
S = Starting Point
E = Maze End
P = Player Position
** = Enemy
▓▓ = Wall


## Implemented Functionalities
* Maze is Generated randomly each run
* Third Person and First Person Camera
* Animating and Changing Color of Enemies and Player in Vertex/ Fragment Shader
* Drawing player, enemies, map with basic geometry
* Player movement
* Player orientation
* Collisions (player - enemy, projectile - enemy, player - map, etc.)
* Time and Healthbar HUD and Functionality
* Enemies movement limited in their own space
* Projectiles to match the player's orientation have a limited range
