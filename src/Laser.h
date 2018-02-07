#pragma once
#include "Asteroid.h"
class C_Laser
{

	public:

		C_Laser() = delete;
		C_Laser(int x, int y);
		~C_Laser() = default;
		void Move(Uint32 deltaTime);
		void Render(SDL_Color& renderColor = SDL_Color{ LASER_COLOR.r, LASER_COLOR.g, LASER_COLOR.b, LASER_COLOR.a } );

		const m1::Position& GetPosition()
		{
			return position;
		}

		const SDL_Rect& GetCollisionBox()
		{
			return collisionBox;
		}

	private:

		int laserSize;
		SDL_Rect collisionBox;
		m1::Position position;
};

inline void C_Laser::Move(Uint32 deltaTime)
{
	
	position.y -= deltaTime * LASER_SPEED;
	collisionBox.y = position.y;

}

inline void C_Laser::Render(SDL_Color& renderColor)
{
	SDL_SetRenderDrawColor(_GetRenderer, renderColor.r, renderColor.g, renderColor.b, renderColor.a);
		
	for (int ix = 0; ix < laserSize; ++ix)
	{
		for (int iy = 0; iy < laserSize; ++iy)
		{
			SDL_RenderDrawPoint(_GetRenderer, position.x + ix, position.y + iy);
		}
	}

}

namespace m1
{
	struct multiplayerData_Bullet
	{
		multiplayerData_Bullet() :
			ID{ 1 },
			bulletNum{ 0 },
			bulletPositions{ { 0,0 } }
		{
		}

		void Init(std::vector<C_Laser>& lasers)
		{
			bulletNum = lasers.size();
			if (bulletNum > 64)
			{
				bulletNum = 64;
			}

			for (int i = 0; i < bulletNum; i++)
			{
				bulletPositions[i].x = lasers.at(i).GetPosition().x;
				bulletPositions[i].y = lasers.at(i).GetPosition().y;
			}
		}


		const Uint8 ID;
		uint8_t bulletNum;
		Sint16_Position bulletPositions[64];

	};
}