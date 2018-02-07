#include "Global.h"

//GENERAL
const int WINDOW_SIZE = 720;
const char* WINDOWNAME = "M1 Spacewar";
extern const char* SCOREFONT_PATH = "./data/font/Arvo-Regular.ttf";
extern const SDL_Color SCORE_COLOR = SDL_Color{0xFF,0xFF,0xFF,0xFF};

//MULTIPLAYER
const Uint16 SERVER_PORT = 55555;
char* SERVER_ADRESS = "";
const short CLIENT_TICKRATE = 64;
const short CLIENT_TICKRATE_DELAY = 1000/CLIENT_TICKRATE;

const SDL_Color OTHER_LASER_COLOR = SDL_Color{ 0xFF, 0x80, 0x00, 0xFF };

boost::mutex mutex_player;
boost::mutex mutex_otherPlayer;
boost::mutex mutex_asteroids;
boost::mutex mutex_iostream;
boost::mutex mutex_SDL_GetTicks;
boost::mutex mutex_socket;
boost::mutex mutex_memcpy;

//Player
const float PLAYERSHIP_SPEED = 0.6f;
const int PLAYERSHIP_SIZE = 50; 
const float PLAYERSHIP_SHOOT_LATENCY = 300;

//Laser
const int LASER_SIZE = 2;
const float LASER_SPEED = 0.7f;
const SDL_Color LASER_COLOR = SDL_Color{ 0x00, 0xFF, 0x00, 0xFF };

//Asteroid
const float ASTEROID_SPEED = 0.4f;
const int ASTEROID_SIZE = 50;
const float ASTEROID_SPAWN_LATENCY = 750;