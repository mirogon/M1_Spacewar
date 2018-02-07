#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_net.h>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <array>
#include <thread>
#include <boost\thread.hpp>
#include <M1Random.h>


//GENERAL
extern const int WINDOW_SIZE;
extern const char* WINDOWNAME;
extern const char* SCOREFONT_PATH;
extern const SDL_Color SCORE_COLOR;

//MULTIPLAYER
extern const Uint16 SERVER_PORT;
extern const char* SERVER_ADRESS;
extern const short CLIENT_TICKRATE;
extern const short CLIENT_TICKRATE_DELAY;
extern const SDL_Color OTHER_LASER_COLOR;

extern boost::mutex mutex_player;
extern boost::mutex mutex_otherPlayer;
extern boost::mutex mutex_asteroids;
extern boost::mutex mutex_iostream;
extern boost::mutex mutex_SDL_GetTicks;
extern boost::mutex mutex_socket; 
extern boost::mutex mutex_memcpy;

//Player
extern const float PLAYERSHIP_SPEED;
extern const int PLAYERSHIP_SIZE;
extern const float PLAYERSHIP_SHOOT_LATENCY;

//Laser
extern const int LASER_SIZE;
extern const float LASER_SPEED;
extern const SDL_Color LASER_COLOR;


//Asteroid

extern const float ASTEROID_SPEED;
extern const int ASTEROID_SIZE;
extern const float ASTEROID_SPAWN_LATENCY;

namespace m1 {

	template<typename T>
	inline void SafeLog(const T& s)
	{
		mutex_iostream.lock();
		std::cout << s;
		mutex_iostream.unlock();
	}

	template<typename FIRST, typename... REST>
	inline void SafeLog(const FIRST& first, const REST&... rest)
	{
		SafeLog(first);
		SafeLog(rest...);
	}

	inline Uint32 SafeGetTicks()
	{
		Uint32 ticks;

		mutex_SDL_GetTicks.lock();
		ticks = SDL_GetTicks();
		mutex_SDL_GetTicks.unlock();

		return ticks;

	}

	inline void SafeMemcpy(void* dst, const void* src, size_t size)
	{
		mutex_memcpy.lock();
		memcpy(dst, src, size);
		mutex_memcpy.unlock();
	}
	

	struct Sint16_Position
	{
		Sint16_Position() : x{ 0 }, y{ 0 } {}
		Sint16_Position(Sint16 x_, Sint16 y_) : x{ x_ }, y{ y_ } {}

		Sint16 x;
		Sint16 y;
	};

	struct double_Position
	{
		double_Position() : x{ 0 }, y{ 0 } {}

		double_Position(double x_, double y_) : x{ x_ }, y{ y_ } {}

		double x;
		double y;
	};

	struct multiplayerData_Player
	{
		multiplayerData_Player() :
			ID{ 0 },
			xPos{ 0 },
			yPos{ 0 }

		{
		}
		const Uint8 ID;
		Sint16 xPos;
		Sint16 yPos;
	};


	//multiplayer_Bullet declared in Laser.h


	struct multiplayerData_Asteroid
	{

		multiplayerData_Asteroid() :
			ID{ 2 },
			asteroidPositions{ { 0,0 } }
		{

		}

		void Init(std::vector<m1::Sint16_Position>& positions)
		{
			asteroidNum = positions.size();
			if (asteroidNum > 32)
			{
				asteroidNum = 32;
			}

			for (int i = 0; i < asteroidNum; i++)
			{
				asteroidPositions[i].x = positions.at(i).x;
				asteroidPositions[i].y = positions.at(i).y;
			}
		}

		const Uint8 ID;
		uint8_t asteroidNum;
		Sint16_Position asteroidPositions[32];
	};
	
	struct multiplayerData_GameInfo
	{
		multiplayerData_GameInfo() : ID{ 3 }, score{ 0 }, otherScore{ 0 }, remainingTime{ 0 }, won { false } {}
		const Uint8 ID;
		int32_t score;
		int32_t otherScore;
		uint32_t remainingTime;
		bool won;
	};


	typedef IPaddress server_endpoint;

	inline void Recv_UDP_Packet(UDPsocket& socket, UDPpacket& packet)
	{

		short rc = SDLNet_UDP_Recv(socket, &packet);
		if (rc == 1)
		{
			return;
		}

		if (rc == 0)
		{
			Recv_UDP_Packet(socket, packet);
		}

		else if (rc == -1)
		{
			throw SDLNet_GetError();
		}

	}

	enum E_CollisionDirection { Left = 0, Right = 1, Bot = 2, Top = 3, None = 4 };

	enum E_GameState { GS_None = 0, GS_Game_Play = 1, GS_Game_Over = 2, GS_Game_Play_Multiplayer = 3, GS_Game_Menu = 4, GS_Multiplayer_WinningScreen = 5, GS_Multiplayer_LosingScreen = 6 };

	struct Position
	{
		Position(): x{0}, y{0} {}

		double x;
		double y;
	};

	struct double_Rect {

		double x;
		double y;
		int w;
		int h;

		double_Rect() : x(0), y(0), w(0), h(0)
		{}

		double_Rect(double x_, double y_, int w_, int h_) : x{ x_ }, y{ y_ }, w{ w_ }, h{ h_ }
		{}

		static int GetCenterX(const double_Rect& a)
		{
			return a.x + a.w / 2;
		}
		static int GetCenterY(const double_Rect& a)
		{
			return a.y + a.h / 2;
		}
	};

	//CHECK IF THE KEY WITH THE SCANCODE scancode IS PRESSED
	inline bool KeyIsPressed(SDL_Scancode scancode)
	{
		static const Uint8* keyStatus = SDL_GetKeyboardState(NULL);
		keyStatus = SDL_GetKeyboardState(NULL);

		if (keyStatus[scancode] == 1)
		{
			return true;
		}

		else
		{
			return false;
		}

	}


	inline m1::E_CollisionDirection CollisionDetection(const SDL_Rect& pCollisionBox, const SDL_Rect& collisionBox)
	{
		float w = 0.5 * (pCollisionBox.w + collisionBox.w);
		float h = 0.5 * (pCollisionBox.h + collisionBox.h);
		float dx = (pCollisionBox.x + pCollisionBox.w / 2) - (collisionBox.x + collisionBox.w / 2);
		float dy = (pCollisionBox.y + pCollisionBox.h / 2) - (collisionBox.y + collisionBox.h / 2);

		if (abs(dx) <= w && abs(dy) <= h)
		{
			/* collision! */
			float wy = w * dy;
			float hx = h * dx;

			if (wy > hx)
			{
				if (wy > -hx)
				{
					return Bot;
				}

				else
				{
					return Left;
				}
			}

			else
			{
				if (wy > -hx)
				{
					return Right;
				}

				else
				{
					return Top;
				}

			}

		}

		else return None;

	}

	//CAST EVERYTHING TO A STRING
	template <typename T>
	inline std::string to_string(T value)
	{
		std::ostringstream os;
		os << value;
		return os.str();
	}

};








