#include "Game.h"

//#ifdef _DEBUG

#undef main

//#endif
void ReadConfigVariables();

int main(int argc, char* argv[])
{
	
	try
	{

		if (SDL_Init(SDL_INIT_VIDEO) == -1)
		{
			throw SDL_GetError();
		}
		if (SDLNet_Init() == -1)
		{
			throw SDLNet_GetError();
		}
		TTF_Init();

		ReadConfigVariables();

		bool quit = false;
		bool keyDown = false;

		SDL_Event ev;

		C_Game mainGame;

		mainGame.Game_Init_Multiplayer(argv[0]);

		m1::E_GameState gameState = m1::E_GameState::GS_Game_Menu;

		while (quit == false)
		{

			static int fps = 0;
			static Uint32 lastTime = m1::SafeGetTicks();

			while (SDL_PollEvent(&ev) != 0)
			{
				if (ev.type == SDL_QUIT)
				{
					quit = true;
				}

				if (ev.type == SDL_KEYDOWN)
				{
					if (ev.key.keysym.sym == SDLK_RETURN)
					{
						keyDown = true;
					}
				}

				else
				{
					keyDown = false;
				}

			}

			SDL_SetRenderDrawColor(_GetRenderer, 0x00, 0x00, 0x00, 0xFF);
			SDL_RenderClear(_GetRenderer);

			static Uint32 deltaTime = m1::SafeGetTicks();

			switch (gameState)
			{

			case m1::E_GameState::GS_Game_Menu:
				gameState = mainGame.Game_Menu();
				mainGame.ChangeGameState(gameState);
				break;

			case m1::E_GameState::GS_Game_Play:
				if (mainGame.Game_Play(m1::SafeGetTicks() - deltaTime) == true)
				{
					gameState = m1::E_GameState::GS_Game_Over;
					mainGame.ChangeGameState(gameState);
				}
				break;

			case m1::E_GameState::GS_Game_Play_Multiplayer:
				gameState = mainGame.Game_Play_Multiplayer(m1::SafeGetTicks() - deltaTime);
				mainGame.ChangeGameState(gameState);
				break;

			case m1::E_GameState::GS_Game_Over:
				mainGame.Game_Over();

				if (m1::KeyIsPressed(SDL_SCANCODE_RETURN) == true && gameState == m1::E_GameState::GS_Game_Over)
				{
					mainGame.Reset();
					gameState = m1::E_GameState::GS_Game_Play;
					mainGame.ChangeGameState(gameState);
				}
				break;

			case m1::E_GameState::GS_Multiplayer_WinningScreen:
				mainGame.Multiplayer_WinningScreen();
				if (m1::KeyIsPressed(SDL_SCANCODE_RETURN) == true )
				{
					mainGame.Reset();
					gameState = m1::GS_Game_Menu;
				}
				break;

			case m1::E_GameState::GS_Multiplayer_LosingScreen:
				mainGame.Multiplayer_LosingScreen();
				if (m1::KeyIsPressed(SDL_SCANCODE_RETURN) == true)
				{
					mainGame.Reset();
					gameState = m1::GS_Game_Menu;
				}
				break;

			}

			deltaTime = m1::SafeGetTicks();

			SDL_RenderPresent(_GetRenderer);

			if (m1::SafeGetTicks() - lastTime >= 1000)
			{
				//std::cout << "FPS: " << fps << std::endl;
				fps = 0;
				lastTime = m1::SafeGetTicks();
			}
			++fps;

		}

	}
	catch(std::exception& e)
	{
		std::cout << "Error with errorcode: " << e.what() << std::endl;
	}
	catch (const char* e)
	{
		std::cout << "Error with errorcode: " << e << std::endl;
	}

	TTF_Quit();
	SDLNet_Quit();
	SDL_Quit();

	system("pause");

	return 0;
}

void ReadConfigVariables()
{
	std::string sA;
	if (m1::ReadConfig("config.cfg", "SERVER_ADRESS", sA) == false)
	{
		m1::SafeLog("SERVER_ADRESS in config.cfg could not be located\n");
	}
	SERVER_ADRESS = const_cast<char*>(sA.c_str());

}