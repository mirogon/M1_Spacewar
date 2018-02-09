#pragma once
#include "PlayerShip.h"

class C_Game
{
	public:
		
		C_Game();
		~C_Game();

		//Game
		//Game_Menu
		m1::E_GameState Game_Menu();
		//Game_Play
		bool Game_Play(Uint32 deltaTime);
		//Game_Over is called, when the player died
		void Game_Over();
		//Reset everything to start a new game
		void Reset();

		void ChangeGameState(m1::E_GameState newGameState);

		//MULTIPLAYER
		//Initialize everything needed for the Multiplayer
		void Game_Init_Multiplayer(const char* argv0);
		//Game_Play_Multiplayer
		m1::E_GameState Game_Play_Multiplayer(Uint32 deltaTime);

		bool Multiplayer_WinningScreen();
		bool Multiplayer_LosingScreen();

	private:

		//Game
		//The player
		C_PlayerShip player;
		//Asteroids
		std::vector<C_Asteroid> asteroids;

		//The score used in Singleplayer and Multiplayer
		int32_t score;
	
		//The font which is used to display the score and other text
		TTF_Font* scoreFont;
		SDL_Surface* scoreSurface;
		SDL_Texture* scoreTexture;

		//Game menu
		SDL_Surface* menu_play_surface;
		SDL_Texture* menu_play_texture;

		SDL_Surface* menu_play_multiplayer_surface;
		SDL_Texture* menu_play_multiplayer_texture;
	
		//firstTime variables
		bool firstTime_Game_Play_Multiplayer;
		bool firstTime_Game_Play;

		m1::C_Timer mainTimer;

		//MULTIPLAYER
		C_PlayerShip otherPlayer;

		UDPsocket socket;
		IPaddress serverEndpoint;
		UDPpacket registerPacket;

		boost::thread thread_multiplayer_send;

		boost::thread thread_multiplayer_receive;

		uint32_t remainingTime;

		m1::E_GameState currentGameState;

		//FUNCTIONS
		void SpawnAsteroids();
		void DespawnAsteroids();
		void SyncScore();
		void MoveAsteroids(Uint32 deltaTime);
		void RenderAsteroids();
		void AsteroidLaserCollisionHandling();
		bool AsteroidPlayerCollisionHandling();
		void RenderScore();

		//MULTIPLAYER
		//Send register packet to server
		void RegisterToServer();
		//Create and render the waiting screen while waiting for the server/other client
		void Thread_RenderWaitingScreen(bool& active);
		//Receive the start time from the game server
		Uint32 GetStartTime();
		//Handle all incoming packets from the server
		void ReceiveAndHandleServerPackets();
		//Send everything to the server via thread
		void MultiplayerThread_Send();
		//Receive all incoming packets ( just calls ReceiveAndHandleServerPackets )
		void MultiplayerThread_Receive();

		void RenderRemainingTime();
	
};

inline m1::E_GameState C_Game::Game_Menu()
{

	SDL_SetRenderDrawColor(_GetRenderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderClear(_GetRenderer);

	SDL_RenderCopy(_GetRenderer, menu_play_texture, NULL, new SDL_Rect{ WINDOW_SIZE / 2 - menu_play_surface->w / 2, WINDOW_SIZE / 2 - menu_play_surface->h / 2 - 50, menu_play_surface->w, menu_play_surface->h });

	SDL_RenderCopy(_GetRenderer, menu_play_multiplayer_texture, NULL, new SDL_Rect{ WINDOW_SIZE / 2 - menu_play_multiplayer_surface->w / 2, WINDOW_SIZE / 2 - menu_play_multiplayer_surface->h / 2 + 50, menu_play_multiplayer_surface->w, menu_play_multiplayer_surface->h });

	if (m1::KeyIsPressed(SDL_SCANCODE_1) == true)
	{
		return m1::GS_Game_Play;
	}

	else if (m1::KeyIsPressed(SDL_SCANCODE_2) == true)
	{
		return m1::GS_Game_Play_Multiplayer;
	}

	else 
	{
		return m1::GS_Game_Menu;
	}

}

inline bool C_Game::Game_Play(Uint32 deltaTime)
{


	if (firstTime_Game_Play == true)
	{
		SyncScore();
		firstTime_Game_Play = false;
	}

	player.Move(deltaTime);
	player.KeepInScreen();
	player.Render();
	player.Shoot();
	player.HandleBullets(deltaTime);
	player.DeleteLaserShots();

	SpawnAsteroids();
	MoveAsteroids(deltaTime);
	AsteroidLaserCollisionHandling();
	RenderAsteroids();

	if ( AsteroidPlayerCollisionHandling() == true)
	{
		return true;
	}

	DespawnAsteroids();

	static Uint32 lTS = m1::SafeGetTicks();

	if (m1::SafeGetTicks() - lTS >= 1000)
	{
		score += 10;
		SyncScore();
		lTS = m1::SafeGetTicks();
	}

	RenderScore();

	return false;
}

inline m1::E_GameState C_Game::Game_Play_Multiplayer(Uint32 deltaTime)
{
	//m1::SageLog("MP 1\n");
	static Uint32 startTime;

	if (firstTime_Game_Play_Multiplayer == true)
	{
		bool waitingScreenActive = true;
		RegisterToServer();
		std::cout << "Sent local endpoint to Server" << std::endl;

		boost::thread thread_waitingScreen([&, this] {this->Thread_RenderWaitingScreen(waitingScreenActive); });

		startTime = GetStartTime();
		
		std::cout << "Received start time from Server: " <<startTime<< std::endl;
		
		waitingScreenActive = false;
		thread_waitingScreen.join();
		
		Uint32 waitTime = m1::SafeGetTicks();

		SDL_Surface* countSurface = TTF_RenderText_Solid(scoreFont, m1::to_string(startTime).c_str(), SDL_Color{ 0xFF,0xFF,0xFF,0x00 });
		if (countSurface == nullptr)
		{
			throw SDL_GetError();
		}
		SDL_Texture* countTexture = SDL_CreateTextureFromSurface(_GetRenderer, countSurface);

		while (m1::SafeGetTicks() <= waitTime + startTime)
		{
			countSurface = TTF_RenderText_Solid(scoreFont, m1::to_string(int( ( ( waitTime + startTime ) - m1::SafeGetTicks() +1000 ) / 1000 )).c_str(), SDL_Color{ 0xFF,0xFF,0xFF,0x00 });
			countTexture = SDL_CreateTextureFromSurface(_GetRenderer, countSurface);

			SDL_SetRenderDrawColor(_GetRenderer, 0x00, 0x00, 0x00, 0x00);
			SDL_RenderClear(_GetRenderer);
			SDL_RenderCopy(_GetRenderer, countTexture, NULL, new SDL_Rect{ WINDOW_SIZE / 2 - countSurface->w / 2, WINDOW_SIZE / 2 - countSurface->h / 2, countSurface->w, countSurface->h });
			SDL_RenderPresent(_GetRenderer);
			
		}
		SDL_DestroyTexture(countTexture);
		SDL_FreeSurface(countSurface);

		thread_multiplayer_send = boost::thread([this] {this->MultiplayerThread_Send(); });
		thread_multiplayer_receive = boost::thread([this] {this->MultiplayerThread_Receive(); });

		SyncScore();

		firstTime_Game_Play_Multiplayer = false;
	}

	//m1::SageLog("MP 2\n");
	mutex_player.lock();
	player.Move(deltaTime);
	player.KeepInScreen();
	player.Render();
	player.Shoot();
	player.HandleBullets(deltaTime);
	player.DeleteLaserShots();
	mutex_player.unlock();

	//m1::SageLog("MP 3\n");
	mutex_otherPlayer.lock();
	otherPlayer.Render({0xFF, 0xFF, 0x00, 0x00});
	otherPlayer.RenderBulletsOnly();
	mutex_otherPlayer.unlock();

	mutex_asteroids.lock();
	MoveAsteroids(deltaTime);
	RenderAsteroids();
	mutex_asteroids.unlock();

	//m1::SageLog("MP 4\n");
	static Uint32 lTS = m1::SafeGetTicks();

	SyncScore();
	RenderScore();

	RenderRemainingTime();

	//m1::SageLog("MP 6\n");

	return currentGameState;

}

inline void C_Game::Game_Over()
{
	SDL_SetRenderDrawColor(_GetRenderer, 0x7F, 0x00, 0x00, 0xFF);
	SDL_RenderClear(_GetRenderer);

	player.Render();
	
	RenderAsteroids(); 

	for(std::vector<C_Laser>::iterator i = player.GetLaserShots().begin(); i != player.GetLaserShots().end(); ++i)
	{
		i->Render();
	}

	RenderScore();

}

inline void C_Game::MoveAsteroids(Uint32 deltaTime)
{
	for (std::vector<C_Asteroid>::iterator i = asteroids.begin(); i != asteroids.end(); ++i)
	{
		i->Move(deltaTime);
	}
}

inline void C_Game::RenderAsteroids()
{
	for (std::vector<C_Asteroid>::iterator i = asteroids.begin(); i != asteroids.end(); ++i)
	{
		i->Render();
	}
}



inline void C_Game::AsteroidLaserCollisionHandling()
{
	for (int i = 0; i < asteroids.size(); ++i)
	{
		std::vector<C_Asteroid>::iterator asteroidIterator = asteroids.begin() + i;

		for (int i2 = 0; i2 < player.GetLaserShots().size(); ++i2)
		{
			std::vector<C_Laser>::iterator laserIT = player.GetLaserShots().begin() + i2;

			if (m1::CollisionDetection(asteroidIterator->GetCollisionBox(), laserIT->GetCollisionBox()) != m1::E_CollisionDirection::None)
			{
				score += 25;
				SyncScore();
				asteroids.erase(asteroidIterator);
				asteroids.shrink_to_fit();
				player.GetLaserShots().erase(laserIT);
				player.GetLaserShots().shrink_to_fit();
				break;
			}
		}
	}
}

inline bool C_Game::AsteroidPlayerCollisionHandling()
{
	for (std::vector<C_Asteroid>::iterator aIT = asteroids.begin(); aIT != asteroids.end();++aIT)
	{
		if (m1::CollisionDetection(player.GetCollisionBox(), aIT->GetCollisionBox()) != m1::E_CollisionDirection::None)
		{
			return true;
		}
	}

	return false;

}

inline void C_Game::RenderScore()
{
	SDL_RenderCopy(_GetRenderer, scoreTexture, NULL, new SDL_Rect{ WINDOW_SIZE / 2 - (scoreSurface->w / 2) ,0, scoreSurface->w, scoreSurface->h });

	//SDL_RenderPresent(_GetRenderer);

}

inline void C_Game::ReceiveAndHandleServerPackets()
{
	//m1::SageLog("Receive 1\n");

	//m1::SafeLog("Trying to receive a package\n");
	static UDPpacket packet = *SDLNet_AllocPacket(500);
	packet.len = 500;

	static uint8_t ID = 0;

	static m1::multiplayerData_Player cachePlayerData;
	static m1::multiplayerData_Bullet cacheBulletData;
	static m1::multiplayerData_Asteroid cacheAsteroidData;
	static m1::multiplayerData_GameInfo cacheGameInfo;

	static uint64_t startTime = mainTimer.GetTimeSinceStart_microseconds();
	static uint32_t sleepTime = 0;

	while (currentGameState == m1::E_GameState::GS_Game_Play_Multiplayer)
	{

		startTime = mainTimer.GetTimeSinceStart_microseconds();
		mutex_socket.lock();
		short rc = SDLNet_UDP_Recv(socket, &packet);
		mutex_socket.unlock();

		if (rc == 1)
		{
			m1::SafeMemcpy(&ID, packet.data, sizeof(uint8_t));

			switch (ID)
			{

			case 0:
				//m1::SageLog("Receive 3\n");
				m1::SafeMemcpy(&cachePlayerData, packet.data, sizeof(m1::multiplayerData_Player));

				mutex_otherPlayer.lock();
				otherPlayer.MoveDirect(cachePlayerData.xPos, cachePlayerData.yPos);
				mutex_otherPlayer.unlock();
				break;
			case 1:
				m1::SafeMemcpy(&cacheBulletData, packet.data, sizeof(m1::multiplayerData_Bullet));
				mutex_otherPlayer.lock();
				otherPlayer.InitLaserShots(cacheBulletData);
				mutex_otherPlayer.unlock();
				break;
			case 2:
				m1::SafeMemcpy(&cacheAsteroidData, packet.data, sizeof(m1::multiplayerData_Asteroid));
				mutex_asteroids.lock();
				asteroids.clear();
				asteroids.shrink_to_fit();
				for(int i = 0; i < cacheAsteroidData.asteroidNum; i++)
				{
					asteroids.push_back(C_Asteroid(cacheAsteroidData.asteroidPositions[i].x, cacheAsteroidData.asteroidPositions[i].y ));
				}
				mutex_asteroids.unlock();
				break;
			case 3:
				m1::SafeMemcpy(&cacheGameInfo, packet.data, sizeof(m1::multiplayerData_GameInfo));
				score = cacheGameInfo.score;
				remainingTime = cacheGameInfo.remainingTime;
				if (remainingTime <= 0)
				{
					//gameOver = true;
					if (cacheGameInfo.won == true)
					{
						currentGameState = m1::GS_Multiplayer_WinningScreen;
					}

					else
					{
						currentGameState = m1::GS_Multiplayer_LosingScreen;
					}

				}
				break;
			}



		}

		else if (rc == 0)
		{
		}

		else if (rc == -1)
		{
			throw SDLNet_GetError();
		}

		sleepTime = 2000 - (mainTimer.GetTimeSinceStart_microseconds() - startTime);
		if (sleepTime > 0 && sleepTime <= 1)
		{
			//m1::SafeLog("SleepTime: ", sleepTime, "\n");
			std::this_thread::sleep_for(std::chrono::microseconds(sleepTime));
		}

	}

}

inline void C_Game::MultiplayerThread_Receive()
{
	//Receive other player data from the server and save it
	ReceiveAndHandleServerPackets();


}


inline void C_Game::MultiplayerThread_Send()
{
	UDPpacket playerDataPacket = *SDLNet_AllocPacket(sizeof(m1::multiplayerData_Player));
	playerDataPacket.len = sizeof(m1::multiplayerData_Player);

	UDPpacket bulletDataPacket = *SDLNet_AllocPacket(sizeof(m1::multiplayerData_Bullet));
	bulletDataPacket.len = sizeof(m1::multiplayerData_Bullet);

	m1::multiplayerData_Player sendPlayerData;
	m1::multiplayerData_Bullet bulletData;

	uint64_t startTime = mainTimer.GetTimeSinceStart_microseconds();
	int32_t sleepTime = 0;

	while (currentGameState == m1::GS_Game_Play_Multiplayer )
	{
		
		startTime = mainTimer.GetTimeSinceStart_microseconds();
		//m1::SageLog("Send 3\n");

		//Send this playerdata to the server

		mutex_player.lock();
		sendPlayerData.xPos = player.GetCollisionBox().x;
		sendPlayerData.yPos = player.GetCollisionBox().y;
		mutex_player.unlock();

		//m1::SageLog("Send 3.1\n");
		m1::SafeMemcpy(playerDataPacket.data, &sendPlayerData, sizeof(m1::multiplayerData_Player));
		//m1::SageLog("Send 3.2\n");

		mutex_socket.lock();
		SDLNet_UDP_Send(socket, 1, &playerDataPacket);
		mutex_socket.unlock();

		//Send bullets to server
		mutex_player.lock();
		bulletData.Init(player.GetLaserShots());
		mutex_player.unlock();

		m1::SafeMemcpy(bulletDataPacket.data, &bulletData, sizeof(bulletData));

		SDLNet_UDP_Send(socket, 1, &bulletDataPacket);

		//m1::SageLog("Send 4\n");
		sleepTime = CLIENT_TICKRATE_DELAY - (mainTimer.GetTimeSinceStart_microseconds() - startTime);
		//m1::SafeLog("SleepTime: ", sleepTime, "\n");
		if (sleepTime > 0 && sleepTime <= CLIENT_TICKRATE_DELAY)
		{
			std::this_thread::sleep_for( std::chrono::microseconds(sleepTime) );
		}
		
	
		
	}

}

inline void C_Game::RenderRemainingTime()
{
	static std::string timeString;

	timeString = "Remaining time: " + m1::to_string((remainingTime + 1000) / 1000);

	SDL_Surface* timeSurface = TTF_RenderText_Solid(scoreFont, timeString.c_str() , SDL_Color{ 0xFF,0xFF,0xFF,0x00 });
	if (timeSurface == nullptr)
	{
		//throw SDL_GetError();
	}
	SDL_Texture* timeTexture = SDL_CreateTextureFromSurface(_GetRenderer, timeSurface);

	SDL_RenderCopy(_GetRenderer, timeTexture, NULL, new SDL_Rect{0, 0 , timeSurface->w, timeSurface->h });
	
	SDL_DestroyTexture(timeTexture);
	SDL_FreeSurface(timeSurface);
}

inline bool C_Game::Multiplayer_WinningScreen()
{
	static std::string winningString;
	winningString = "You won with a score of " + m1::to_string(score);

	SDL_SetRenderDrawColor(_GetRenderer, 0x00, 0xFF, 0x00, 0x00);
	SDL_RenderClear(_GetRenderer);

	SDL_Surface* winningSurface = TTF_RenderText_Solid(scoreFont, winningString.c_str(), SDL_Color{ 0x00, 0x00, 0x00, 0x00 });
	SDL_Texture* winningTexture = SDL_CreateTextureFromSurface(_GetRenderer, winningSurface);

	SDL_RenderCopy(_GetRenderer, winningTexture, NULL, new SDL_Rect{ WINDOW_SIZE / 2 - winningSurface->w / 2, WINDOW_SIZE / 2 - winningSurface->h / 2, winningSurface->w, winningSurface->h });

	SDL_DestroyTexture(winningTexture);
	SDL_FreeSurface(winningSurface);

	return false;

}
inline bool C_Game::Multiplayer_LosingScreen()
{
	static std::string losingString;
	losingString = "You lost with a score of " + m1::to_string(score);

	SDL_SetRenderDrawColor(_GetRenderer, 0xFF, 0x00, 0x00, 0x00);
	SDL_RenderClear(_GetRenderer);


	SDL_Surface* losingSurface = TTF_RenderText_Solid(scoreFont, losingString.c_str(), SDL_Color{ 0x00, 0x00, 0x00, 0x00 });
	SDL_Texture* losingTexture = SDL_CreateTextureFromSurface(_GetRenderer, losingSurface);

	SDL_RenderCopy(_GetRenderer, losingTexture, NULL, new SDL_Rect{ WINDOW_SIZE / 2 - losingSurface->w / 2, WINDOW_SIZE / 2 - losingSurface->h / 2, losingSurface->w, losingSurface->h });

	SDL_DestroyTexture(losingTexture);
	SDL_FreeSurface(losingSurface);

	return false;

}
