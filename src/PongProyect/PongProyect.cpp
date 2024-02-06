#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <string>
#include <vector>
#include "PongProyect.h"

using namespace std;

const int WIDTH = 1280;
const int HEIGHT = 720;
const int NORMAL_FONT_SIZE = 32;
const int TITLE_FONT_SIZE = 128;
const int CREDIT_FONT_SIZE = 64;
const int SIZE_BALL = 16;
const float PI = 3.14159265;
const int BALL_SPEED = 10;
const int GAME_DURATION = 120;

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Color color;
TTF_Font* font;
float counterFrame, lastFrame, FPS, timerFPS, lastTime;
bool gameRunning = true, myTurn = true, replay = true, curvedBall = true;
double Time;
SDL_Event event;

SDL_Rect paddleL, paddleR, ball, scoreboard, timer, startMenu, result, startAgain, title, subtitle, name;
float ballVelX, ballVelY;
int paddleLVelY = 5;
int paddleRVelY = 4;
string score,timerText,startmenuText;
int scoreL, scoreR;
double relation, bounce, normalize, distanceMidX, distanceMidY, normDistMidX, normDistMidY;

//-----------------------------------------------------//

void write(string text, int size, int x, int y, SDL_Rect obj){
    TTF_SetFontSize(font, size);
    SDL_Surface* surface;
    SDL_Texture* texture;
    const char* t = text.c_str();
    surface = TTF_RenderText_Solid(font, t, color);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    obj.w = surface->w;
    obj.h = surface->h;
    obj.x = x - obj.w/2;
    obj.y = y - obj.h/2;
    SDL_FreeSurface(surface);
    SDL_RenderCopy(renderer, texture, NULL, &obj);
    SDL_DestroyTexture(texture);
}

//-----------------------------------------------------//

void postImage(string image, SDL_Rect obj) {
    SDL_Surface* surface;
    SDL_Texture* texture;
    const char* t = image.c_str();
    surface = IMG_Load(t);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    obj.w = surface->w;
    obj.h = surface->h;

    SDL_FreeSurface(surface);
    SDL_RenderCopy(renderer, texture, NULL, &obj);
    SDL_DestroyTexture(texture);
}

//-----------------------------------------------------//

void quitGame() {
    const Uint8* keyStates = SDL_GetKeyboardState(NULL);
    if (event.type == SDL_QUIT) {
        gameRunning = false;
        replay = false;
    }
    if (keyStates[SDL_SCANCODE_ESCAPE]) {
        gameRunning = false;
        replay = false;
    }
}

//-----------------------------------------------------//

void pauseUntilKey() {
    while (SDL_WaitEvent(&event)) {
        quitGame();
        if (event.type == SDL_KEYUP){
            break;
        }
    }
}

//-----------------------------------------------------//

void movimientoPaddle(int velPaddle, SDL_Rect &paddle) {
   if (paddle.y >= 0 && paddle.y <= HEIGHT - paddle.h) {
        paddle.y += velPaddle;
   }
   if (paddle.y < 0)
   {
       paddle.y = 0;
   }
   if (paddle.y > HEIGHT - paddle.h)
   {
       paddle.y = HEIGHT - paddle.h;
   }
}

//-----------------------------------------------------//

void initializeEngine() {
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    IMG_Init(IMG_INIT_PNG);
    font = TTF_OpenFont("upheavtt.ttf",NORMAL_FONT_SIZE);

    if (font == NULL) {
        cout << "la font no pudo cargarse correctamente" << endl;
    }

}

//-----------------------------------------------------// 

void gameLoop(SDL_Renderer* renderer) {
    serve();
    while (gameRunning) {
        lastFrame = SDL_GetTicks();

        if (lastFrame >= lastTime + 1000) {
            lastTime = lastFrame;
            FPS = counterFrame;
            counterFrame = 0;
        }

        input();
        update();
        render();
    }
}

//-----------------------------------------------------//

void results() {
    if ((int)Time <= 0 && scoreL < scoreR) {
        write("You lose :(", CREDIT_FONT_SIZE, (WIDTH / 2), (HEIGHT / 2), result);
        write("Press any key to start again or ESC to close", NORMAL_FONT_SIZE, (WIDTH / 2), (HEIGHT / 2)+CREDIT_FONT_SIZE, startAgain);
        SDL_RenderPresent(renderer);
        gameRunning = false;
        pauseUntilKey();
    }

    if ((int)Time <= 0 && scoreR < scoreL) {
        write("You Win :D", CREDIT_FONT_SIZE, (WIDTH / 2), (HEIGHT / 2), result);
        write("Press any key to start again or ESC to close", NORMAL_FONT_SIZE, (WIDTH / 2), (HEIGHT / 2) + CREDIT_FONT_SIZE, startAgain);
        SDL_RenderPresent(renderer);
        gameRunning = false;
        pauseUntilKey();
    }

    if ((int)Time <= 0 && scoreR == scoreL) {
        write("Draw -_-", CREDIT_FONT_SIZE, (WIDTH / 2), (HEIGHT / 2), result);
        write("Press any key to start again or ESC to close", NORMAL_FONT_SIZE, (WIDTH / 2), (HEIGHT / 2) + CREDIT_FONT_SIZE, startAgain);
        SDL_RenderPresent(renderer);
        gameRunning = false;
        pauseUntilKey();
    }
}

//-----------------------------------------------------//

void pantallaInicio() {
    
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
   
    write("Press any key to continue...", CREDIT_FONT_SIZE, (WIDTH / 2), (HEIGHT / 2), subtitle);

    SDL_RenderPresent(renderer);

    pauseUntilKey();

    gameRunning = true;
    replay = true;

    write("PONG", TITLE_FONT_SIZE, (WIDTH / 2), TITLE_FONT_SIZE, title);
    write("By Alan Luca Codini", NORMAL_FONT_SIZE, (WIDTH*3/4), HEIGHT - (NORMAL_FONT_SIZE/2)-10, name);
    write("ESC to close", NORMAL_FONT_SIZE, (WIDTH * 1 / 4), HEIGHT - (NORMAL_FONT_SIZE / 2) - 10, name);

    SDL_RenderPresent(renderer);

    pauseUntilKey();

}

//-----------------------------------------------------//

void render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    counterFrame++;
    timerFPS = SDL_GetTicks() - lastFrame;

    if (timerFPS < (1000 / 60)) {
        SDL_Delay((1000 / 60) - timerFPS);
    }

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
    
    postImage("Paddle.png", paddleL);
    postImage("Paddle.png", paddleR);
    postImage("Ball.png", ball);
    write(score, NORMAL_FONT_SIZE, (WIDTH / 2), NORMAL_FONT_SIZE, scoreboard);
    write(to_string((int)(Time/1000)), NORMAL_FONT_SIZE, (WIDTH / 2) , HEIGHT - (NORMAL_FONT_SIZE), timer);

    results();
    
    SDL_RenderPresent(renderer);
}

//-----------------------------------------------------//

void update() {
    if (SDL_HasIntersection(&paddleL, &ball)) {
        relation = (paddleL.y + (paddleL.h / 2)) - (ball.y + (ball.h / 2));
        normalize = relation / (paddleL.h / 2);
        bounce = normalize * (5*PI/16);
        float sen = SDL_sin(bounce)+0.01;
        float con = SDL_cos(bounce)+0.01;
        ballVelY =  -sen * BALL_SPEED;
        ballVelX = con * BALL_SPEED;
    }

    if (SDL_HasIntersection(&paddleR, &ball)) {
        relation = (paddleR.y + (paddleR.h / 2)) - (ball.y + (ball.h / 2));
        normalize = relation / (paddleR.h / 2);
        bounce = normalize * (5*PI /16);
        float sen = SDL_sin(bounce) + 0.01;
        float con = SDL_cos(bounce) + 0.01;
        ballVelY = -sen * BALL_SPEED;
        ballVelX = con * -BALL_SPEED;
    }

    if ((ball.y + (ball.h / 2)) < (paddleR.y + (paddleR.h / 2)) && (paddleR.y > 0 || paddleR.y < HEIGHT - paddleR.h)) {
        paddleR.y -= paddleRVelY;
    }

    if ((ball.y + (ball.h / 2)) > (paddleR.y + (paddleR.h / 2)) && (paddleR.y > 0 || paddleR.y < HEIGHT - paddleR.h)) {
        paddleR.y += paddleRVelY;
    }

    if (paddleR.y < 0)
    {
        paddleR.y = 0;
    }

    if (paddleR.y > HEIGHT - paddleR.h)
    {
        paddleR.y = HEIGHT - paddleR.h;
    }

    if (ball.x <= 0) {
        scoreR += 1;
        myTurn = false;
        serve();
    }

    if (ball.x >= WIDTH - ball.w) {
        scoreL += 1;
        myTurn = true;
        serve();
    }

    if (ball.y <= 0 || ball.y >= HEIGHT - ball.h) {
        ballVelY = -ballVelY;
    }

    if (curvedBall) {
        distanceMidX = (WIDTH / 2) - ball.x;
        distanceMidY = (HEIGHT / 2) - ball.y;
        normDistMidX = distanceMidX / sqrt(((distanceMidX * distanceMidX) + (distanceMidY * distanceMidY)));
        normDistMidY = sqrt(((distanceMidX * distanceMidX) + (distanceMidY * distanceMidY))) / ((distanceMidX * distanceMidX) + (distanceMidY * distanceMidY));

        if (distanceMidY < 0) {
            ballVelY -= normDistMidY*50;
        }
        if (distanceMidY > 0) {
            ballVelY += normDistMidY*50;
        }

    }

    Time -= 1000/60;
    ball.x += ballVelX;
    ball.y += ballVelY;
    score = to_string(scoreL) + " - " + to_string(scoreR);
}

//-----------------------------------------------------//

void input() {
    const Uint8* keyStates = SDL_GetKeyboardState(NULL);
    
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            gameRunning = false;
            replay = false;
        }
    }

    if (keyStates[SDL_SCANCODE_ESCAPE]) {
            gameRunning = false;
            replay = false;
    }
    if (keyStates[SDL_SCANCODE_W]) {
            movimientoPaddle(-paddleLVelY, paddleL);
    }
    if (keyStates[SDL_SCANCODE_S]) {
            movimientoPaddle(paddleLVelY, paddleL);
    }
}

//-----------------------------------------------------//

void serve(){
    paddleL.y = (HEIGHT / 2) - (paddleL.h / 2);
    paddleR.y = (HEIGHT / 2) - (paddleR.h / 2);
    if (myTurn){
        ball.x = (WIDTH / 2) - ball.w;
        ball.y = (HEIGHT / 2) - ball.h;
        ballVelX = -7;
        ballVelY = 0;
    }
    else    {
        ball.x = (WIDTH / 2) - ball.w;
        ball.y = (HEIGHT / 2) - ball.h;
        ballVelX = 7;
        ballVelY = 0;
    }
}

//-----------------------------------------------------//

int main(int argc, char* args[])
{
    initializeEngine();
    
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        cout << "Failed at SDL_Init()" << endl;
    }

    if (SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer) < 0)
    {
        cout << "Failed at SDL_CreateWindowAndRenderer()" << endl;
    }

    while (replay) {
        color.r = color.g = color.b = 255;
        scoreL = scoreR = 0;
        score = to_string(scoreL) + " - " + to_string(scoreR);
        paddleL.x = 32; paddleL.h = HEIGHT / 4;
        paddleL.y = (HEIGHT / 2) - (paddleL.h / 2); paddleL.w = paddleL.h / 10;
        paddleR = paddleL;
        paddleR.x = WIDTH - 32 - paddleR.w;
        ball.w = ball.h = SIZE_BALL;
        ball.x = (WIDTH / 2) - ball.w;
        ball.y = (HEIGHT / 2) - ball.h;
        lastTime = 0;
        Time = (GAME_DURATION + 1) * 1000;
        gameRunning = true;


        pantallaInicio();
        gameLoop(renderer);
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);


    return 0;
}