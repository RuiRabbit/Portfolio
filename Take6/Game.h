#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"

struct _Player;

void CardShuffle();

void GameInit(int num, int difficulty, int totalcard, int speed);

void Game(SDL_Renderer *renderer);

void GamePresent(SDL_Renderer *renderer);

int LastCardPlaceinRow(int row);

int rowofcattles(int row);

void sort(int a[], int n);

void PlaceCard(SDL_Renderer *renderer);

void ChooseRow(int id, SDL_Renderer *renderer);

void GameAI_rnd(int id);
int GameAI_rnd_row(int id);

void GameAI_1(int id);
int GameAI_1_row(int id);

void GameAI_2(int id);
int GameAI_2_row(int id);

void GamePresent(SDL_Renderer *renderer);

void Loadtexture(SDL_Renderer *renderer);
void LoadCard(SDL_Renderer *renderer);
void destroyallsource();