#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "Game.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <string.h>

void LoadBackground(SDL_Renderer *renderer);
void LoadCard(SDL_Renderer *renderer);
void destroyalltexture();

int main(){

    printf("Please enter how many enemies you want(1 ~ 9)?\n");
    int totalplayer = 9;
    scanf("%d", &totalplayer);
    totalplayer++;
    printf("Please set the difficulty(0 ~ 2).\n");
    int difficulty = 0;
    scanf("%d", &difficulty);
    printf("Do you want to use all card(0) or only 1 ~ %d card(1)?\n", totalplayer * 10 + 4);
    int choice = 0;
    scanf("%d", &choice);
    printf("Please choose the game speed(0 ~ 2)\n");
    int speed = 0;
    scanf("%d", &speed);

    GameInit(totalplayer, difficulty, choice, speed);
    printf("Init completed\n");
    printf("-----------------------\n");
    printf("Game log :\n");
    SDL_Window *window;
    SDL_Renderer *renderer;
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
        SDL_GetError();
        return 0;
    }
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
    window = SDL_CreateWindow(
        "Take6",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        1440,
        900,
        SDL_WINDOW_SHOWN
    );
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    Loadtexture(renderer);
    LoadCard(renderer);
    GamePresent(renderer);

    Game(renderer);

    destroyallsource();
	SDL_DestroyRenderer(renderer); 
	SDL_DestroyWindow(window); 
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0;
}
