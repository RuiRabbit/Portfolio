#include "Game.h"

struct _Player{
    int32_t id;
    SDL_Color color;
    int32_t totalcard;
    int32_t card[10];
    int32_t cattle;
} player[10];//at most ten player

uint8_t playercolor[11][3] = {
    {0, 229, 238},
    {102, 205, 170},
    {106, 90, 205},
    {30, 144, 255},
    {69, 139, 0},
    {255, 215, 0},
    {255, 106, 106},
    {139, 69, 19},
    {178, 34, 34},
    {255, 20, 147},
    {255, 255, 255}
};//the represent color for all player

int GameTable[4][5];
// 0 = no card
// 1 ~ 104 = numbers of card

struct _GameSetting{
    int totalplayer;
    int difficulty;
    int totalcard;
    int speed;
} GameSetting;

struct _Card{
    int playerid;
    int cardid;
} buffer[10];
//list the buffer of every player's card

void (*ai[5])(int) = {GameAI_rnd, GameAI_1, GameAI_2};
int (*ai_row[5])(int) = {GameAI_rnd_row, GameAI_1_row, GameAI_2_row};


SDL_Texture *card[110];
SDL_Texture *background;
SDL_Texture *wood;
SDL_Texture *pickcard;
SDL_Texture *pickrow;
SDL_Texture *wait;
SDL_Texture *gameover;
SDL_Texture *yourcardtxt;
SDL_Texture *arrow;
SDL_Texture *scoreboardtxt;
int statment = 0;// 0 pickcard 1 pickrow 2 wait

SDL_Rect mycard[10];
SDL_Rect buffercard[10];
SDL_Rect tablecard[4][5];
SDL_Rect button[4];
SDL_Rect signplace;
SDL_Rect line[3];
SDL_Rect colorblock[10];
SDL_Rect yourcard;
SDL_Rect arrowplace[4];
SDL_Rect scoreboardtitle;

TTF_Font *font60;
TTF_Font *font24;

//shuffle card
void CardShuffle(){
    bool used[104] = {false};
    int card_gived = 0;
    srand(time(NULL));
    while(card_gived < GameSetting.totalplayer * 10){
        int now = rand() % GameSetting.totalcard;
        while(used[now]){
            now = rand() % GameSetting.totalcard;
        }
        player[card_gived / 10].card[card_gived % 10] = now + 1;
        used[now] = true;
        card_gived++;
    }
    for(int i = 0; i < 4; i++){
        int now = rand() % GameSetting.totalcard;
        while(used[now]){
            now = rand() % GameSetting.totalcard;
        }
        GameTable[i][0] = now + 1;
        used[now] = true;
        card_gived++;
        if(card_gived >= GameSetting.totalcard)
            break;
    }
    for(int i = 0; i < GameSetting.totalplayer; i++){
        sort(player[i].card, 10);
    }
    // for(int i = 0; i < GameSetting.totalplayer; i++){
    //     printf("Player %d :", i);
    //     for(int j = 0; j < 10; j++){
    //         printf(" %d", player[i].card[j]);
    //     }
    //     printf("\n");
    // }
    // printf("GameTable 0 0 -> %d\n", GameTable[0][0]);
    return;
}

// init the game
void GameInit(int totalplayer, int difficulty, int totalcard, int speed){
    GameSetting.totalplayer = totalplayer;
    GameSetting.difficulty = difficulty;
    if(speed == 0){
        GameSetting.speed = 700;
    }
    else if(speed == 1){
        GameSetting.speed = 1000;
    }
    else{
        GameSetting.speed = 1300;
    }
    statment = 0;
    if(totalcard){
        GameSetting.totalcard = totalplayer * 10 + 4;
    }
    else{
        GameSetting.totalcard = 104;
    }
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 5; j++){
            GameTable[i][j] = 0;
        }
    }
    for(int i = 0; i < totalplayer; i++){
        player[i].id = i;
        player[i].color = (SDL_Color){playercolor[i][0], playercolor[i][1], playercolor[i][2], 255};
        player[i].totalcard = 10;
        player[i].cattle = 0;
    }

    for(int i = 0; i < 10; i++){
        buffer[i].cardid = 0;
        buffer[i].playerid = 0;
    }
    for(int i = 0; i < 10; i++){
        mycard[i].h = 106;
        mycard[i].w = 76;
        mycard[i].y = 25;
        mycard[i].x = 40 + i * 110;
    }
    line[0].h = 10;
    line[0].w = 1440;
    line[0].x = 0;
    line[0].y = 150;

    line[1].h = 10;
    line[1].w = 1440;
    line[1].x = 0;
    line[1].y = 730;

    line[2].h = 580;
    line[2].w = 10;
    line[2].x = 1030;
    line[2].y = 150;

    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 5; j++){
            tablecard[i][j].h = 106;
            tablecard[i][j].w = 76;
            tablecard[i][j].x = 40 + j * 110;
            tablecard[i][j].y = 180 + i * 140;
        }
        arrowplace[i].h = 106;
        arrowplace[i].w = 76;
        arrowplace[i].x = 590;
        arrowplace[i].y = 180 + i * 140;
    }

    for(int i = 0; i < 10; i++){
        buffercard[i].h = 106;
        buffercard[i].w = 76;
        buffercard[i].x = 40 + i * 110;
        buffercard[i].y = 760;

        colorblock[i].h = 10;
        colorblock[i].w = 56;
        colorblock[i].y = 876;
        colorblock[i].x = buffercard[i].x + 10;
    }
    signplace.h = 300;
    signplace.w = 204;
    signplace.x = 800;
    signplace.y = 400;

    yourcard.x = 1140;
    yourcard.y = 40;

    scoreboardtitle.x = 1085;
    scoreboardtitle.y = 180;
    CardShuffle(totalplayer);
}

// select sort
void sort(int a[], int n){
    for(int i = 0; i < n; i++){
        for(int j = i + 1; j < n; j++){
            if(a[i] > a[j]){
                int tmp = a[i];
                a[i] = a[j];
                a[j] = tmp;
            }
        }
    }
}

// find the last card index in row
int LastCardPlaceinRow(int row){
    if(GameTable[row][0] == 0)
        return -1;
    for(int i = 0; i < 5; i++){
        if(GameTable[row][i] == 0)
            return i - 1;
    }
    return 4;
}

// cmp for _Card
int cmp(const void *a, const void *b)
{
    struct _Card c = *(struct _Card *)a;
    struct _Card d = *(struct _Card *)b;
    return c.cardid - d.cardid;
}

// swap _Card
void swap(struct _Card *a, struct _Card *b){
    struct _Card tmp = *a;
    *a = *b;
    *b = tmp;
    return;
}

// correspond the cardnum to cattle
int cattlecount(int cardnum){
    if(cardnum == 0)
        return 0;
    if(cardnum == 55)
        return 7;
    if(cardnum % 11 == 0)
        return 5;
    if(cardnum % 10 == 0)
        return 3;
    if(cardnum % 5 == 0)
        return 2;
    return 1;
}

// place card stage anime
void PlaceCard(SDL_Renderer *renderer){
    int endofrow[4] = {0};
    for(int i = 1; i <= GameSetting.totalplayer; i++){
        qsort((void *)buffer, GameSetting.totalplayer - i + 1, sizeof(buffer[0]), cmp);
        GamePresent(renderer);
        SDL_Delay(GameSetting.speed);
        printf("Player %d play (%d) to the table.\n", buffer[0].playerid, buffer[0].cardid);
        for(int j = 0; j < 4; j++){
            endofrow[j] = GameTable[j][LastCardPlaceinRow(j)];
        }
        int minv = 2e9;
        int placed = -1;
        for(int j = 0; j < 4; j++){
            if(endofrow[j] < buffer[0].cardid && buffer[0].cardid - endofrow[j] < minv){
                minv = buffer[0].cardid - endofrow[j];
                placed = j;
            }
        }
        if(placed == -1){
            ChooseRow(buffer[0].playerid, renderer);
        }
        else if(LastCardPlaceinRow(placed) != 4){
            GameTable[placed][LastCardPlaceinRow(placed) + 1] = buffer[0].cardid;
        }
        else{
            printf("Player %d has to take row %d.\n", buffer[0].playerid, placed);
            printf("Player %d get %d cattles\n", buffer[0].playerid, rowofcattles(placed));
            for(int j = 0; j < 5; j++){
                player[buffer[0].playerid].cattle += cattlecount(GameTable[placed][j]);
                GameTable[placed][j] = 0;
            }
            GameTable[placed][0] = buffer[0].cardid;
        }
        buffer[0].cardid = 0;
        buffer[0].playerid = 0;
        swap(&buffer[0], &buffer[GameSetting.totalplayer - i]);
    }
    GamePresent(renderer);
}

// check if user left click the area
bool inrect(SDL_Event event, SDL_Rect rect){
    int x = event.button.x;
    int y = event.button.y;
    return (event.button.button == SDL_BUTTON_LEFT, x > rect.x && y > rect.y && x < rect.x + rect.w && y < rect.y + rect.h);
}

// return the num of cattles in this row
int rowofcattles(int row){
    int cattle = 0;
    for(int i = 0; i <= LastCardPlaceinRow(row); i++){
        cattle += cattlecount(GameTable[row][i]);
    }
    return cattle;
}

// pick row
void ChooseRow(int id, SDL_Renderer *renderer){
    int row;
    int (*pick)(int) = ai_row[GameSetting.difficulty];
    if(id != 0){
        row = pick(id);
    }
    else{
        statment = 1;
        while(statment == 1){
            SDL_Event event;
            GamePresent(renderer);
            while(SDL_PollEvent(&event)){
                switch(event.type){
                    case SDL_MOUSEBUTTONDOWN:
                        for(int i = 0; i < 4; i++){
                            if(inrect(event, arrowplace[i])){
                                row = i;
                                statment = 2;
                                break;
                            }
                        }
                        break;

                    default:
                        break;
                }
            }
        }
    }
    printf("Player %d choose to take row %d.\n", id, row);
    printf("Player %d get %d cattles\n", id, rowofcattles(row));
    for(int i = 0; i < 5; i++){
        player[id].cattle += cattlecount(GameTable[row][i]);
        GameTable[row][i] = 0;
    }
    GameTable[row][0] = buffer[0].cardid;
}

// pick card & waiting
void Game(SDL_Renderer *renderer){
    bool quit = false;
    while (!quit){
        SDL_Event event;
        GamePresent(renderer);
        if(player[0].totalcard > 0)
            printf("\nRound %d\n", 10 - player[0].totalcard + 1);
        SDL_FlushEvent(SDL_MOUSEBUTTONDOWN);
        while(statment == 0){
            // printf("Running statment 0\n");
            while(SDL_PollEvent(&event)){
                switch(event.type){
                    case SDL_QUIT:
                        quit = true;
                        statment == 3;
                        break;
                    case SDL_MOUSEBUTTONDOWN:
                        for(int i = 0; i < player[0].totalcard; i++){
                            if(inrect(event, mycard[i])){
                                buffer[0].cardid = player[0].card[i];
                                buffer[0].playerid = 0;
                                player[0].totalcard--;
                                statment = 2;
                                for(int j = i; j < player[0].totalcard; j++){
                                    int tmp = player[0].card[j];
                                    player[0].card[j] = player[0].card[j + 1];
                                    player[0].card[j + 1] = tmp;
                                }
                                printf("Player 0 pick (%d) to play.\n", buffer[0].cardid);
                                // printf("choose successful\n");
                                break;
                            }
                        }
                        break;

                    default:
                        break;
                }
                // printf("buffer : %d\n", buffer[0].cardid);
                if(quit){
                    statment = 3;
                    break;
                }
                if(buffer[0].cardid != 0)
                    break;
            }
            // printf("OUT\n");
        }
        while(statment == 2){
            void (*pick)(int) = ai[GameSetting.difficulty];
            for(int i = 1; i < GameSetting.totalplayer; i++){
                pick(i);
            }
            PlaceCard(renderer);
            // printf("Running statment 2\n");
            statment = 0;
        }
        if(quit)
            printf("Quit!!!\n");
    }
    
}

// level 0 AI
void GameAI_rnd(int id){
    int choice = rand() % player[id].totalcard;
    buffer[id].cardid = player[id].card[choice];
    buffer[id].playerid = id;
    for(int i = choice; i < player[id].totalcard - 1; i++){
        int tmp = player[id].card[i];
        player[id].card[i] = player[id].card[i + 1];
        player[id].card[i + 1] = tmp;
    }
    player[id].totalcard--;
}
int GameAI_rnd_row(int id){
    int choice = rand() % 4;
    return choice;
}

// level 1 AI
void GameAI_1(int id){
    int choice = -1;
    int minv = 2e9;
    for(int i = 0; i < player[id].totalcard; i++){
        int endofrow[4] = {0};
        for(int j = 0; j < 4; j++){
            endofrow[j] = GameTable[j][LastCardPlaceinRow(j)];
        }
        int placed = -1;
        for(int j = 0; j < 4; j++){
            if(endofrow[j] < player[id].card[i] && player[id].card[i] - endofrow[j] < minv && LastCardPlaceinRow(j) != 4){
                minv = player[id].card[id] - endofrow[j];
                placed = j;
            }
        }
        if(placed != -1){
            choice = i;
        }
    }
    if(choice == -1)
        choice = rand() % player[id].totalcard;
    buffer[id].cardid = player[id].card[choice];
    buffer[id].playerid = id;
    for(int i = choice; i < player[id].totalcard - 1; i++){
        int tmp = player[id].card[i];
        player[id].card[i] = player[id].card[i + 1];
        player[id].card[i + 1] = tmp;
    }
    player[id].totalcard--;
}
int GameAI_1_row(int id){
    int minv = 2e9;
    int choice = -1;
    for(int i = 0; i < 4; i++){
        if(rowofcattles(i) < minv){
            choice = i;
            minv = rowofcattles(i);
        }
    }
    return choice;
}

// level 2 AI
void GameAI_2(int id){
    int choice = -1;
    for(int i = player[id].totalcard - 1; i >= 0; i--){
        int endofrow[4] = {0};
        for(int j = 0; j < 4; j++){
            endofrow[j] = GameTable[j][LastCardPlaceinRow(j)];
        }
        int placed = -1;
        for(int j = 0; j < 4; j++){
            if(endofrow[j] < player[id].card[i] && LastCardPlaceinRow(j) != 4){
                placed = j;
            }
        }
        if(placed != -1){
            choice = i;
        }
    }
    if(choice == -1)
        choice = rand() % player[id].totalcard;
    buffer[id].cardid = player[id].card[choice];
    buffer[id].playerid = id;
    for(int i = choice; i < player[id].totalcard - 1; i++){
        int tmp = player[id].card[i];
        player[id].card[i] = player[id].card[i + 1];
        player[id].card[i + 1] = tmp;
    }
    player[id].totalcard--;
}
int GameAI_2_row(int id){
    int minv = 2e9;
    int maxv = 0;
    int choice = -1;
    for(int i = 0; i < 4; i++){
        if(rowofcattles(i) < minv){
            choice = i;
            minv = rowofcattles(i);
            maxv = GameTable[i][LastCardPlaceinRow(i)];
        }
        else if(rowofcattles(i) == minv && GameTable[i][LastCardPlaceinRow(i)] > maxv){
            choice = i;
            minv = rowofcattles(i);
            maxv = GameTable[i][LastCardPlaceinRow(i)];
        }
    }
    return choice;
}

// a cmp to compare player's rank
int rankcmp(const void *a, const void *b)
{
    int c = *(int *)a;
    int d = *(int *)b;
    return player[c].cattle - player[d].cattle;
}

// show table to user
void GamePresent(SDL_Renderer *renderer){
    SDL_RenderCopy(renderer, background, NULL, NULL);
    SDL_Texture *sign;
    switch (statment){
        case 0:
            sign = pickcard;
            break;
        case 1:
            sign = pickrow;
            for(int i = 0; i < 4; i++){
                SDL_RenderCopy(renderer, arrow, NULL, &arrowplace[i]);
            }
            break;
        case 2:
            sign = wait;
            break;
        
        default:
            break;
    }
    if(player[0].totalcard <= 0)
        sign = gameover;

    SDL_RenderCopy(renderer, sign, NULL, &signplace);
    for(int i = 0; i < player[0].totalcard; i++){
        SDL_RenderCopy(renderer, card[player[0].card[i]], NULL, &mycard[i]);
    }
    SDL_RenderCopy(renderer, wood, &line[0], &line[0]);
    SDL_RenderCopy(renderer, wood, &line[1], &line[1]);
    SDL_RenderCopy(renderer, wood, &line[2], &line[2]);
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 5; j++){
            if(GameTable[i][j] != 0){
                SDL_RenderCopy(renderer, card[GameTable[i][j]], NULL, &tablecard[i][j]);
            }
        }
    }
    for(int i = 0; i < 10; i++){
        if(buffer[i].cardid != 0){
            int id = buffer[i].playerid;
            // printf("id = %d\n", id);
            SDL_SetRenderDrawColor(renderer, player[id].color.r, player[id].color.g, player[id].color.b, player[id].color.a);
            SDL_RenderFillRect(renderer, &colorblock[i]);
            // printf("Fill Rect\n");
            // printf("Card = %d\n", buffer[i].cardid);
            // printf("i = %d\n", i);
            SDL_RenderCopy(renderer, card[buffer[i].cardid], NULL, &buffercard[i]);
            // printf("Buffer render\n");
        }
    }
    // printf("%d\n", __LINE__);
    SDL_RenderCopy(renderer, yourcardtxt, NULL, &yourcard);
    SDL_RenderCopy(renderer, scoreboardtxt, NULL, &scoreboardtitle);

    int rank[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    qsort(rank, GameSetting.totalplayer, sizeof(int), rankcmp);
    for(int i = 0; i < GameSetting.totalplayer; i++){
        SDL_Surface *tmp = NULL;
        SDL_Rect rect;
        SDL_Texture *txt;
        char data[100];
        sprintf(data, "Player %d  : %3d", rank[i], player[rank[i]].cattle);
        tmp = TTF_RenderText_Solid(font24, data, player[rank[i]].color);
        txt = SDL_CreateTextureFromSurface(renderer, tmp);
        rect.h = tmp->h;
        rect.w = tmp->w;
        SDL_FreeSurface(tmp);
        rect.x = 1170;
        rect.y = 260 + i * 45;
        SDL_RenderCopy(renderer, txt, NULL, &rect);
        SDL_DestroyTexture(txt);
    }
    SDL_RenderPresent(renderer);
}

// load all source texture
void Loadtexture(SDL_Renderer *renderer){
    SDL_Surface *tmp = NULL;
    tmp = IMG_Load("source/table.png");
    background = SDL_CreateTextureFromSurface(renderer, tmp);
    SDL_FreeSurface(tmp);

    tmp = IMG_Load("source/wood.png");
    wood = SDL_CreateTextureFromSurface(renderer, tmp);
    SDL_FreeSurface(tmp);

    tmp = IMG_Load("source/pickcard_sign.png");
    pickcard = SDL_CreateTextureFromSurface(renderer, tmp);
    SDL_FreeSurface(tmp);

    tmp = IMG_Load("source/pickrow_sign.png");
    pickrow = SDL_CreateTextureFromSurface(renderer, tmp);
    SDL_FreeSurface(tmp);

    tmp = IMG_Load("source/waiting_sign.png");
    wait = SDL_CreateTextureFromSurface(renderer, tmp);
    SDL_FreeSurface(tmp);

    tmp = IMG_Load("source/gameover_sign.png");
    gameover = SDL_CreateTextureFromSurface(renderer, tmp);
    SDL_FreeSurface(tmp);

    font60 = TTF_OpenFont("source/ERASDEMI.TTF", 60);
    font24 = TTF_OpenFont("source/ERASDEMI.TTF", 24);
    tmp = TTF_RenderText_Solid( font60, "Your Card", (SDL_Color){255, 255, 255, 255});
    yourcardtxt = SDL_CreateTextureFromSurface(renderer, tmp);
    yourcard.h = tmp->h;
    yourcard.w = tmp->w;
    SDL_FreeSurface(tmp);
    
    tmp = TTF_RenderText_Solid( font60, "ScoreBoard", (SDL_Color){255, 255, 255, 255});
    scoreboardtxt = SDL_CreateTextureFromSurface(renderer, tmp);
    scoreboardtitle.h = tmp->h;
    scoreboardtitle.w = tmp->w;
    SDL_FreeSurface(tmp);

    tmp = IMG_Load("source/arrow.png");
    arrow = SDL_CreateTextureFromSurface(renderer, tmp);
    SDL_FreeSurface(tmp);
}

// load all card texture
void LoadCard(SDL_Renderer *renderer){
    for(int i = 1; i <= 104; i++){
        SDL_Surface *tmp = NULL;
        char filename[50];
        sprintf(filename, "source/Card/PNG/Card_%d.png", i);
        tmp = IMG_Load(filename);
        card[i] = SDL_CreateTextureFromSurface(renderer, tmp);
        SDL_FreeSurface(tmp);
    }
}

// free all source
void destroyallsource(){
    SDL_DestroyTexture(background);
    SDL_DestroyTexture(wood);
    SDL_DestroyTexture(pickcard);
    SDL_DestroyTexture(pickrow);
    SDL_DestroyTexture(wait);
    SDL_DestroyTexture(gameover);
    SDL_DestroyTexture(arrow);
    for(int i = 1; i <= 104; i++){
        SDL_DestroyTexture(card[i]);
    }
    SDL_DestroyTexture(yourcardtxt);
    TTF_CloseFont(font60);
    TTF_CloseFont(font24);
}