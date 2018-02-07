#include "Laser.h"

C_Laser::C_Laser(int x, int y)
{
	position.x = x;
	position.y = y;

	laserSize = LASER_SIZE;

	collisionBox.x = x;
	collisionBox.y = y;
	collisionBox.w = LASER_SIZE;
	collisionBox.h = LASER_SIZE;

}

