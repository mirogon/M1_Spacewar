#include "Game.h"



C_Game::C_Game():

	socket{ SDLNet_UDP_Open(0)},
	serverEndpoint(IPaddress())

{
	if (!socket)
	{
		throw SDLNet_GetError();
	}

	registerPacket = *SDLNet_AllocPacket(1);

	SDLNet_ResolveHost(&serverEndpoint, SERVER_ADRESS, SERVER_PORT);

	//Bind Server endpoint to channel 1
	SDLNet_UDP_Bind(socket, 1, &serverEndpoint);

	IPaddress ownIP = *SDLNet_UDP_GetPeerAddress(socket, -1);
	m1::SafeLog("OWN IP: ", ownIP.host, " PORT: ", ownIP.port, "\n");

	player.MoveDirect(WINDOW_SIZE / 2 - player.GetCollisionBox().w / 2, WINDOW_SIZE - player.GetCollisionBox().h);
	otherPlayer.MoveDirect(0, 0);
	score = 0;
	scoreFont = TTF_OpenFont(SCOREFONT_PATH, 20);
	scoreSurface = nullptr;
	scoreTexture = nullptr;

	menu_play_surface = TTF_RenderText_Solid(scoreFont, "Press 1 to play Singleplayer", SDL_Color{ 0xFF, 0xFF, 0xFF, 0x00 });
	if (menu_play_surface == nullptr)
	{
		throw SDL_GetError();
	}

	menu_play_multiplayer_surface = TTF_RenderText_Solid(scoreFont, "Press 2 to play Multiplayer", SDL_Color{ 0xFF, 0xFF, 0xFF, 0x00 });
	if (menu_play_multiplayer_surface == nullptr)
	{
		throw SDL_GetError();
	}

	menu_play_texture = SDL_CreateTextureFromSurface(_GetRenderer, menu_play_surface);
	menu_play_multiplayer_texture = SDL_CreateTextureFromSurface(_GetRenderer, menu_play_multiplayer_surface);


	remainingTime = 0;

	currentGameState = m1::GS_None;
	
	firstTime_Game_Play_Multiplayer = true;
	firstTime_Game_Play = true;


}

C_Game::~C_Game()
{
	SDL_DestroyTexture(scoreTexture);
	SDL_FreeSurface(scoreSurface);
	
	SDL_DestroyTexture(menu_play_texture);
	SDL_FreeSurface(menu_play_surface);

	SDL_DestroyTexture(menu_play_multiplayer_texture);
	SDL_FreeSurface(menu_play_multiplayer_surface);

	TTF_CloseFont(scoreFont);
	
	if (scoreFont != nullptr)
	{
		scoreFont = nullptr;
	}
}


void C_Game::Reset()
{
	thread_multiplayer_send.join();
	thread_multiplayer_receive.join();

	player.MoveDirect(WINDOW_SIZE / 2 - player.GetCollisionBox().w / 2, WINDOW_SIZE - player.GetCollisionBox().h);
	otherPlayer.MoveDirect(0, 0);
	score = 0;
	SyncScore();
	RenderScore();
	asteroids.clear();
	asteroids.shrink_to_fit();
	player.GetLaserShots().clear();
	player.GetLaserShots().shrink_to_fit();

	firstTime_Game_Play_Multiplayer = true;
	firstTime_Game_Play = true;
}

void C_Game::Game_Init_Multiplayer(const char* argv0)
{
	std::string firewallException = "FirewallException.exe ";
	firewallException += argv0;
	system(firewallException.c_str());

}

void C_Game::RegisterToServer()

{
	SDLNet_UDP_Send(socket, 1, &registerPacket);
}

Uint32 C_Game::GetStartTime()
{
	UDPpacket* startTimePacket = SDLNet_AllocPacket(sizeof(Uint32));
	startTimePacket->len = sizeof(Uint32);
	static Uint32 startTime;
	m1::Recv_UDP_Packet(socket, *startTimePacket);
	m1::SafeMemcpy(&startTime, startTimePacket->data, startTimePacket->len);

	SDLNet_FreePacket(startTimePacket);
	return startTime;
}

void C_Game::SpawnAsteroids()
{
	static Uint32 lastTime = m1::SafeGetTicks();

	if (m1::SafeGetTicks() - lastTime >= ASTEROID_SPAWN_LATENCY)
	{
		asteroids.push_back( C_Asteroid( _RandomNumer(0 - ASTEROID_SIZE / 2, WINDOW_SIZE - ASTEROID_SIZE / 2), -ASTEROID_SIZE ) );

		lastTime = m1::SafeGetTicks();
	}
}

void C_Game::DespawnAsteroids()
{
	for (int i = 0; i < asteroids.size(); ++i)
	{
		std::vector<C_Asteroid>::iterator iT = asteroids.begin() + i;

		if (iT->GetCollisionBox().y >= WINDOW_SIZE)
		{
			asteroids.erase(iT);
			asteroids.shrink_to_fit();
		}

	}
}

void C_Game::SyncScore()
{
	SDL_FreeSurface(scoreSurface);
	SDL_DestroyTexture(scoreTexture);

	std::string scoreString = "Score: " + m1::to_string(score);
	scoreSurface = TTF_RenderText_Solid(scoreFont, scoreString.c_str() , SCORE_COLOR);
	if (scoreSurface == nullptr)
	{
		std::cout << SDL_GetError() << std::endl;
	}

	scoreTexture = SDL_CreateTextureFromSurface(_GetRenderer, scoreSurface);

}

void C_Game::Thread_RenderWaitingScreen(bool& active)
{
	SDL_Surface* waitingSurface = TTF_RenderText_Solid(scoreFont, "Waiting for the other player", SDL_Color{ 0xFF,0xFF,0xFF,0x00 });
	if (waitingSurface == nullptr)
	{
		throw SDL_GetError();
	}

	SDL_Texture* waitingTexture = SDL_CreateTextureFromSurface(_GetRenderer, waitingSurface);

	while (active == true)
	{
		SDL_SetRenderDrawColor(_GetRenderer, 0x00, 0x00, 0x00, 0x00);
		SDL_RenderClear(_GetRenderer);
		SDL_RenderCopy(_GetRenderer, waitingTexture, NULL, new SDL_Rect{ WINDOW_SIZE / 2 - waitingSurface->w / 2, WINDOW_SIZE / 2 - waitingSurface->h / 2, waitingSurface->w, waitingSurface->h });
		SDL_RenderPresent(_GetRenderer);
	}

	SDL_DestroyTexture(waitingTexture);
	SDL_FreeSurface(waitingSurface);

}

void C_Game::ChangeGameState(m1::E_GameState newGameState)
{
	currentGameState = newGameState;
}
