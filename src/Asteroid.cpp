#include "Asteroid.h"



C_Asteroid::C_Asteroid(int x, int y)
{
	position.x = x;
	position.y = y;
	collisionBox.x = x;
	collisionBox.y = y;
	collisionBox.w = ASTEROID_SIZE;
	collisionBox.h = ASTEROID_SIZE;

	asteroidSpeed = ASTEROID_SPEED;

}


