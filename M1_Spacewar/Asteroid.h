#pragma once
#include "Base.h"

class C_Asteroid
{
	public:
		
		C_Asteroid() = delete;
		C_Asteroid(int x, int y);
		~C_Asteroid() = default;

		void Move(Uint32 deltaTime);
		void Render();

		SDL_Rect& GetCollisionBox()
		{
			return collisionBox;
		}

	private:

		double asteroidSpeed;
		m1::Position position;
		SDL_Rect collisionBox;

};

inline void C_Asteroid::Move(Uint32 deltaTime)
{
	position.y += deltaTime * asteroidSpeed;
	collisionBox.y = position.y;
}

inline void C_Asteroid::Render()
{
	SDL_SetRenderDrawColor(_GetRenderer, 127, 127, 127, 0xFF);

	SDL_Point hexagonPoints[7];
	hexagonPoints[0].x = ASTEROID_SIZE / 2 + position.x;
	hexagonPoints[0].y = 0 + position.y;
	hexagonPoints[1].x = 0 + position.x;
	hexagonPoints[1].y = 0.25 * ASTEROID_SIZE + position.y;
	hexagonPoints[2].x = 0 + position.x;
	hexagonPoints[2].y = 0.75 * ASTEROID_SIZE + position.y;
	hexagonPoints[3].x = ASTEROID_SIZE / 2 + position.x;
	hexagonPoints[3].y = ASTEROID_SIZE + position.y;
	hexagonPoints[4].x = ASTEROID_SIZE + position.x;
	hexagonPoints[4].y = 0.75 * ASTEROID_SIZE + position.y;
	hexagonPoints[5].x = ASTEROID_SIZE + position.x;
	hexagonPoints[5].y = 0.25 * ASTEROID_SIZE + position.y;
	hexagonPoints[6].x = ASTEROID_SIZE / 2 + position.x;
	hexagonPoints[6].y = 0 + position.y;

	SDL_RenderDrawLines(_GetRenderer, hexagonPoints, 7);
}