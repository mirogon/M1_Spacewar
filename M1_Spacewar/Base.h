#pragma once
#include "Global.h"

#define _GetWindow C_Base::GetInstance()->GetWindow()
#define _GetRenderer C_Base::GetInstance()->GetRenderer()
#define _RandomNumer C_Base::GetInstance()->GenerateRandomNumber

class C_Base
{

public:

	C_Base();
	~C_Base();

	static C_Base* GetInstance()
	{
		static C_Base singleton;

		return &singleton;
	}

	int GenerateRandomNumber(int min, int max)
	{
		return randomGenerator.randomNumber(min, max);
	}
	
	SDL_Window* GetWindow()
	{
		return mainWindow;
	}

	SDL_Renderer* GetRenderer()
	{
		return mainRenderer;
	}

private:

	SDL_Window* mainWindow;
	SDL_Renderer* mainRenderer;
	m1::C_Random randomGenerator;

};