#pragma once
#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>

using namespace std;

void initializeEngine();
void update();
void render();
void input();
void gameLoop(SDL_Renderer* renderer);
void write(string text, int size, int x, int y, SDL_Rect obj);
void serve();
void movimientoPaddle(int velPaddle, SDL_Rect &paddle);
void pauseUntilKey();
void results();
void quitGame();
void pantallaInicio();
void postImage(string image, SDL_Rect obj);