#pragma once
#include "Laser.h"

class C_PlayerShip
{
	public:

		C_PlayerShip();
		~C_PlayerShip() = default;

		void MoveDirect(int x, int y);
		void Move(Uint32 deltaTime);
		void Render(SDL_Color renderColor = {0x00, 0x00, 0xFF, 0x00});


		bool Shoot();
		void HandleBullets(Uint32 deltaTime);
		void DeleteLaserShots();

		void KeepInScreen();

		SDL_Rect& GetCollisionBox()
		{
			return collisionBox;
		}

		std::vector<C_Laser>& GetLaserShots()
		{
			return laserShots;
		}

		//Multiplayer

		void RenderBulletsOnly(SDL_Color& renderColor = SDL_Color(OTHER_LASER_COLOR) )
		{
			for (int i = 0; i < laserShots.size(); i++)
			{
				laserShots.at(i).Render(renderColor);
			}
		}

		void InitLaserShots(m1::multiplayerData_Bullet& bulletData)
		{
			laserShots.clear();
			for (int i = 0; i < bulletData.bulletNum; i++)
			{
				laserShots.push_back( C_Laser( bulletData.bulletPositions[i].x, bulletData.bulletPositions[i].y ) );
			}
		}

	private:
		std::vector<C_Laser> laserShots;
		m1::Position position;
		SDL_Rect collisionBox;
		float shipSpeed;
};

inline void C_PlayerShip::Move(Uint32 deltaTime)
{
	static Uint32 lastTime = m1::SafeGetTicks();

	if (m1::KeyIsPressed(SDL_SCANCODE_W) == true || m1::KeyIsPressed(SDL_SCANCODE_UP) == true)
	{
		position.y -= deltaTime * shipSpeed;
	}

	if (m1::KeyIsPressed(SDL_SCANCODE_A) == true || m1::KeyIsPressed(SDL_SCANCODE_LEFT) == true)
	{
		position.x -= deltaTime * shipSpeed;
	}

	if (m1::KeyIsPressed(SDL_SCANCODE_S) == true || m1::KeyIsPressed(SDL_SCANCODE_DOWN) == true)
	{
		position.y += deltaTime * shipSpeed;
	}

	if (m1::KeyIsPressed(SDL_SCANCODE_D) == true || m1::KeyIsPressed(SDL_SCANCODE_RIGHT) == true)
	{
		position.x += deltaTime * shipSpeed;
	}

	collisionBox.x = position.x;
	collisionBox.y = position.y;

	lastTime = m1::SafeGetTicks();
}

inline void C_PlayerShip::MoveDirect(int x, int y)
{
	position.x = x;
	position.y = y;
	collisionBox.x = x;
	collisionBox.y = y;
}

inline void C_PlayerShip::Render(SDL_Color renderColor)
{
	SDL_SetRenderDrawColor(_GetRenderer, renderColor.r, renderColor.g, renderColor.b, renderColor.a);
	static SDL_Point triangle[4];

	triangle[0].x = 0 + position.x;
	triangle[0].y = PLAYERSHIP_SIZE + position.y;
	triangle[1].x = PLAYERSHIP_SIZE / 2 + position.x;
	triangle[1].y = 0 + position.y;
	triangle[2].x = PLAYERSHIP_SIZE + position.x;
	triangle[2].y = PLAYERSHIP_SIZE + position.y;
	triangle[3].x = 0 + position.x;
	triangle[3].y = PLAYERSHIP_SIZE + position.y;

	SDL_RenderDrawLines(_GetRenderer, triangle, 4);
}

inline bool C_PlayerShip::Shoot()
{
	static Uint32 lastTimeShot = m1::SafeGetTicks();

	if (m1::KeyIsPressed(SDL_SCANCODE_SPACE) == true)
	{	
		if (m1::SafeGetTicks() - lastTimeShot >= PLAYERSHIP_SHOOT_LATENCY)
		{
			laserShots.push_back(C_Laser(position.x + PLAYERSHIP_SIZE / 2, position.y));

			lastTimeShot = m1::SafeGetTicks();

			return true;

		}

	}
	return false;

}

inline void C_PlayerShip::HandleBullets(Uint32 deltaTime) 
{
	for (int i = 0; i < laserShots.size(); ++i)
	{
		laserShots.at(i).Move(deltaTime);
		laserShots.at(i).Render();
	}
}

inline void C_PlayerShip::DeleteLaserShots()
{
	for (int i = 0; i < laserShots.size(); ++i)
	{
		std::vector<C_Laser>::iterator iT = laserShots.begin() + i;

		if (iT->GetPosition().y < 0)
		{
			laserShots.erase(iT);
			laserShots.shrink_to_fit();
		}
	}
}

inline void C_PlayerShip::KeepInScreen()
{
	if (position.x < 0)
	{
		MoveDirect(0, position.y);
	}

	if (position.x > WINDOW_SIZE - PLAYERSHIP_SIZE)
	{
		MoveDirect(WINDOW_SIZE - PLAYERSHIP_SIZE, position.y);
	}

	if (position.y < 0)
	{
		MoveDirect(position.x, 0);
	}

	if (position.y > WINDOW_SIZE - PLAYERSHIP_SIZE)
	{
		MoveDirect(position.x, WINDOW_SIZE - PLAYERSHIP_SIZE);
	}


}

