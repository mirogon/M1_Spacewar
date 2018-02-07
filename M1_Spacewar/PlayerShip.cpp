#include "PlayerShip.h"

C_PlayerShip::C_PlayerShip()
{
	position.x = 0;
	position.y = 0;
	collisionBox.x = 0;
	collisionBox.y = 0;
	collisionBox.w = PLAYERSHIP_SIZE;
	collisionBox.h = PLAYERSHIP_SIZE;
	shipSpeed = PLAYERSHIP_SPEED;
}

