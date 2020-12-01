#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "windows.h"
#define min(a,b) (a<b?a:b)
#define max(a,b) (a>b?a:b)

#include "AI.h"
#include "board.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <math.h>
#include <string.h>

#define HSF "highscores.txt"
#define FILENAME "../Files/in.in"
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

int mouseX=0,mouseY=0;
int uni=0,corn=0;
int marginX=SCREEN_WIDTH/6;
int marginY=SCREEN_HEIGHT/6;
int p1=0, p2=3,player=1;

int xCells[9]={334,403,471,537,606,674,741,810,878};
int yCells[9]={53,122,189,259,325,393,465,530,597};
SDL_Surface* textSurface;
SDL_Texture* textTexture;
SDL_Texture* gotoMenuTexture;
SDL_Rect gotoMenuBox={20,400,250,250};

typedef struct Entry {
    int turns;
    time_t date;
    char name[25];
} Entry;

Entry entry[10];

void
loadHighScores(void)
{
    FILE *f = fopen(HSF, "r+");
    fread(entry, sizeof(Entry), 10, f);
    fclose(f);
    return;
}

void
saveHighScores(void)
{
    FILE *f = fopen(HSF, "w");
    fwrite(entry, sizeof(Entry), 10, f);
    fclose(f);
    return;
}

void
writeDef(void)
{
    FILE *f = fopen(HSF, "w");
    time_t t = time(NULL);
    struct tm *tmp;
    for (int i = 0; i < 10; ++i) {
        entry[i].turns = 100;
        tmp = localtime(&t);
        entry[i].date = mktime(tmp);
        strcpy(entry[i].name, "HODJORBEGE        ");
    }
    fwrite(entry, sizeof(Entry), 10, f);
    fclose(f);
    return;
}

void
printHighScores(void)
{
    char *date;
    loadHighScores();
    for (int i = 9; i >= 0; --i) {
        date = ctime(&(entry[i].date));
        date[strlen(date) - 1] = 0;
        printf("%2d. %d %s %s|\n", 10-i, entry[i].turns, date, entry[i].name);
    }
}

void
addEntry(int turns, time_t date, char *name)
{
    int i = 1;
    if (turns > entry[0].turns)
        return;
    while(turns < entry[i].turns && i < 10) {
        entry[i - 1] = entry[i];
        ++i;
    }
    entry[i - 1].turns = turns;
    entry[i - 1].date = date;
    strcpy(entry[i - 1].name, name);
    saveHighScores();
}

TTF_Font *font;

void paintBoard(Board *b,SDL_Renderer* renderer,SDL_Texture* xTexture,SDL_Texture* oTexture,SDL_Texture* boardTexture,SDL_Texture* es,SDL_Event* e){
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor( renderer, 0xe9, 0xc2, 0x89, 0xFF );
    SDL_Rect boardBox={(SCREEN_WIDTH-SCREEN_HEIGHT)/2,0,SCREEN_HEIGHT,SCREEN_HEIGHT};
    SDL_RenderCopy( renderer, boardTexture,NULL,&boardBox);

    SDL_RenderCopy(renderer,gotoMenuTexture,NULL,&gotoMenuBox);


    for(int i=0;i<9;++i){
        for(int j=0;j<9;++j){
            if(b->lastMove/9==j && b->lastMove%9==i){
                SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0x44 );
                SDL_Rect cellBox={xCells[i],yCells[j],68,68};
                if(b->bigTile[i/3+j/3*3]){
                    cellBox.x=xCells[i/3*3];
                    cellBox.y=yCells[j/3*3];
                    cellBox.w*=3;
                    cellBox.h*=3;
                }
                SDL_RenderFillRect(renderer,&cellBox);
                SDL_SetRenderDrawColor( renderer, 0xe9, 0xc2, 0x89, 0xFF );
            }
            if((b->bigToPlay==i/3+j/3*3)&&(!b->win)){
                SDL_SetRenderDrawColor( renderer, 0x33, 0x99, 0x66, 0x11 );
                SDL_Rect cellBox={xCells[i/3*3],yCells[j/3*3],68*3,68*3};
                SDL_RenderFillRect(renderer,&cellBox);
                SDL_SetRenderDrawColor( renderer, 0xe9, 0xc2, 0x89, 0xFF );
//                SDL_Rect cellBox={xCells[i/3*3],yCells[j/3*3],68*3,68*3};
//                SDL_RenderCopy(renderer,es,NULL,&cellBox);
            }
            if(b->bigTile[i/3+j/3*3]){
                SDL_Rect cellBox={xCells[i/3*3],yCells[j/3*3],68*3,68*3};
                SDL_RenderCopy( renderer, (b->bigTile[i/3+j/3*3]!=3?(b->bigTile[i/3+j/3*3]==1?xTexture:(b->bigTile[i/3+j/3*3]==2?oTexture:NULL)):(b->lastMove==1?xTexture:oTexture)),NULL,&cellBox);
            }
            else{
                SDL_Rect cellBox={xCells[i],yCells[j],68,68};
                SDL_RenderCopy( renderer, (b->tiles[j][i]==1?xTexture:(b->tiles[j][i]==2?oTexture:NULL)),NULL,&cellBox);
            }

            SDL_Rect cellBox={xCells[i/3*3]+i/3*3,yCells[j/3*3],68*3,68*3};
            SDL_RenderCopy(renderer,es,NULL,&cellBox);
        }
    }
    if(b->win){
        SDL_Color textColor = { 11, 11, 11 };
        //font=TTF_OpenFont("Font.ttf", 16);

        textSurface = TTF_RenderText_Solid(font , "Pobedio je", textColor);
        textTexture = SDL_CreateTextureFromSurface(renderer,textSurface);

        SDL_Rect textBox = {1000,50,240,60};
        SDL_Rect cellBox={1050,140,170,170};
        SDL_RenderCopy(renderer,textTexture,NULL,&textBox);
        SDL_RenderCopy(renderer,(b->win==1?xTexture:oTexture),NULL,&cellBox);

        SDL_DestroyTexture(textTexture);
        SDL_FreeSurface( textSurface );


    }
    else{
        SDL_Color textColor = { 11, 11, 11 };

        //font=TTF_OpenFont("Font.ttf", 16);

        textSurface = TTF_RenderText_Solid(font , "Na potezu:", textColor);
        textTexture = SDL_CreateTextureFromSurface(renderer,textSurface);

        SDL_Rect textBox = {1000,50,200,50};
        SDL_Rect cellBox={1050-10*sin((double)clock()/200),120-10*sin((double)clock()/200),120+20*sin((double)clock()/200),120+20*sin((double)clock()/200)};
        SDL_RenderCopy(renderer,textTexture,NULL,&textBox);
        SDL_RenderCopy(renderer,(b->playerOnTurn==1?xTexture:oTexture),NULL,&cellBox);

        SDL_DestroyTexture(textTexture);
        SDL_FreeSurface( textSurface );


        if(p1!=0 && p2!=0){
            return;
        };

        textSurface = TTF_RenderText_Solid(font , "Backspace = undo", textColor);
        textTexture = SDL_CreateTextureFromSurface(renderer,textSurface);
        textBox.y = 600;
        textBox.w=280;
        textBox.h=50;
        SDL_RenderCopy(renderer,textTexture,NULL,&textBox);
        SDL_DestroyTexture(textTexture);
        SDL_FreeSurface( textSurface );

        textSurface = TTF_RenderText_Solid(font , "H = help", textColor);
        textTexture = SDL_CreateTextureFromSurface(renderer,textSurface);
        textBox.y = 630;
        textBox.w=180;
        textBox.h=50;
        SDL_RenderCopy(renderer,textTexture,NULL,&textBox);

        SDL_DestroyTexture(textTexture);
        SDL_FreeSurface( textSurface );
    }
    //ne updatuj!
}

int distInt(int xSred,int ySred, int x, int y){
    return sqrt((x-xSred)*(x-xSred)+(y-ySred)*(y-ySred));
}

int distance(SDL_Rect r, int x, int y){
    int xSred=r.x+r.w/2;
    int ySred=r.y+r.h/2;
    return sqrt((x-xSred)*(x-xSred)+(y-ySred)*(y-ySred));
}

int inRect(SDL_Rect r, int x, int y){
    return (x>r.x && x<r.x+r.w && y>r.y && y<r.y+r.h);
}

static void writeMove(int player, int move)
{
    FILE *f=fopen(FILENAME, "w+");
    fprintf(f, "%d %d", move%9+1, move/9+1);
    fclose(f);
}

// mozda bi mogla vecina ovoga da se potrpa u neki update() i setup()
int
main(int argc, char *argv[])
{
    Board *board;
    Board backup;
    int cX=6, cY=5, autoRestart=0;
    double time1=2.5, time2=2.5;
    char ch;
    FILE *f;
    clock_t start;
    MCTree *mct1=NULL, *mct2=NULL;
    TTF_Init();
    font=TTF_OpenFont("PermanentMarker.ttf", 72);
    srand(time(NULL));


    //writeDef();
    time_t t;
    t = time(0);
    loadHighScores();
    //writeDef();
    //addEntry(30, t, "asd");
    printHighScores();
    saveHighScores();

    SDL_Window* window = NULL;
    SDL_Surface* screenSurface = NULL;
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow( "Ultimate Tic Tac Toe", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    screenSurface=SDL_GetWindowSurface(window);
    SDL_Renderer *gRenderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawBlendMode(gRenderer,SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );


    SDL_Event e; //Bog i batina

    board=calloc(1, sizeof(Board));
    resetBoard(board);
    initHeurVal();

    int mouseInBackButton=0;

    int quit=0,inGame=0,inMenu=1,inOptions=0,inHS=0;
    int played=0;
    int i=0;

    SDL_Surface *startImage= IMG_Load("Start.png");
    SDL_Surface *startImageRed= IMG_Load("StartOver.png");
    SDL_Surface *optionsImage= IMG_Load("Options.png");
    SDL_Surface *sijalicaImage= IMG_Load("Sijalica.png");
    SDL_Surface *optionsBackImage=IMG_Load("OptionsBack.png");
    SDL_Surface *optionsWallImage=IMG_Load("OptionsWall.jpg");
    SDL_Surface *humanImage=IMG_Load("HumanPlayer.png");
    SDL_Surface *p1Image=IMG_Load("p1.png");
    SDL_Surface *p2Image=IMG_Load("p2.png");
    SDL_Surface *easyImage=IMG_Load("easy.png");
    SDL_Surface *medImage=IMG_Load("Medium.png");
    SDL_Surface *hardImage=IMG_Load("hard.png");
    SDL_Surface *txtImage=IMG_Load("txt.png");
    SDL_Surface *unicornImage=IMG_Load("Unicorn.png");
    SDL_Surface *xImage=IMG_Load("X.png");
    SDL_Surface *oImage=IMG_Load("O.png");
    SDL_Surface *boardImage=IMG_Load("Board.png");
    SDL_Surface *emptySquareImage=IMG_Load("EmptySquare.png");
    SDL_Surface *gotoMenuImage=IMG_Load("MenuIcon.png");
    SDL_Surface *hsImage=IMG_Load("HighScores.png");

    SDL_Texture *hsTexture=SDL_CreateTextureFromSurface(gRenderer,hsImage);
    gotoMenuTexture=SDL_CreateTextureFromSurface(gRenderer,gotoMenuImage);
    SDL_Texture *emptySquareTexture=SDL_CreateTextureFromSurface(gRenderer,emptySquareImage);
    SDL_Texture *boardTexture=SDL_CreateTextureFromSurface(gRenderer,boardImage);
    SDL_Texture *xTexture=SDL_CreateTextureFromSurface(gRenderer,xImage);
    SDL_Texture *oTexture=SDL_CreateTextureFromSurface(gRenderer,oImage);
    SDL_Texture *unicornTexture=SDL_CreateTextureFromSurface(gRenderer,unicornImage);
    SDL_Texture *easyTexture=SDL_CreateTextureFromSurface(gRenderer,easyImage);
    SDL_Texture *medTexture=SDL_CreateTextureFromSurface(gRenderer,medImage);
    SDL_Texture *hardTexture=SDL_CreateTextureFromSurface(gRenderer,hardImage);
    SDL_Texture *txtTexture=SDL_CreateTextureFromSurface(gRenderer,txtImage);
    SDL_Texture *p2Texture=SDL_CreateTextureFromSurface(gRenderer,p2Image);
    SDL_Texture *p1Texture=SDL_CreateTextureFromSurface(gRenderer,p1Image);
    SDL_Texture *humanTexture=SDL_CreateTextureFromSurface(gRenderer,humanImage);
    SDL_Texture *optionsWallTexture=SDL_CreateTextureFromSurface(gRenderer,optionsWallImage);
    SDL_Texture *optionsBackTexture=SDL_CreateTextureFromSurface(gRenderer,optionsBackImage);
    SDL_Texture *startTexture=SDL_CreateTextureFromSurface(gRenderer,startImage);
    SDL_Texture *startOverTexture=SDL_CreateTextureFromSurface(gRenderer,startImageRed);
    SDL_Texture *optionsTexture=SDL_CreateTextureFromSurface(gRenderer,optionsImage);
    SDL_Texture *sijalicaTexture=SDL_CreateTextureFromSurface(gRenderer,sijalicaImage);


    while(!quit){

        SDL_RenderClear(gRenderer);

        if(inMenu){
            int dist=sqrt((mouseX-marginX/2-160)*(mouseX-marginX/2-160)+(mouseY-marginY/2-160)*(mouseY-marginY/2-160));
            //SDL_SetRenderDrawColor( gRenderer, 120+(0xFF-0xFF*dist/1000)/2, 120+(0xFF-0xFF*dist/1000)/2, 30, 0xFF );
            SDL_SetRenderDrawColor( gRenderer, (dist>160?0xEE:0xFF), (dist>160?0xEE:dist), (dist>160?0xEE:dist), 0xFF );
            //printf("%i\n",dist);
            SDL_Rect startButtonBox={marginX/2+(dist<160?rand()%((170-dist)/5)-5:0),marginY/2+(dist<160?rand()%((170-dist)/5)-5:0),320,320};
            SDL_Rect optionsBox={650,350,554,430};
            SDL_Rect sijalicaBox={optionsBox.x+246,optionsBox.y+100,30,50};
            int koef=0;
            i=(i+1)%(5*256);

            SDL_Rect hsBox={900-sin((double)clock()/150)*10,100-sin((double)clock()/150)*10,100+2*sin((double)clock()/150)*10,100+2*sin((double)clock()/150)*10};

            if(distance(hsBox,mouseX,mouseY)>60){ hsBox.x=900;hsBox.y=100;hsBox.h=100;hsBox.w=100;};

            SDL_SetTextureAlphaMod(optionsTexture,(dist<160?0:255));
            SDL_SetTextureAlphaMod(hsTexture,(dist<160?0:255));
            SDL_SetTextureAlphaMod(sijalicaTexture,((mouseX>optionsBox.x+30)&&mouseX<(optionsBox.x+optionsBox.w-30)&&mouseY>(optionsBox.y+150)&&mouseY<(optionsBox.y+optionsBox.h-30)?255:0));
            SDL_SetTextureAlphaMod((dist<160?startOverTexture:startTexture), max(0,0xFF-0xFF*dist/500));
            SDL_RenderCopy(gRenderer,hsTexture,NULL,&hsBox);
            SDL_RenderCopy( gRenderer, optionsTexture,NULL,&optionsBox);
            SDL_RenderCopy( gRenderer, (dist<160?startOverTexture:startTexture),NULL,&startButtonBox);
            SDL_RenderCopy( gRenderer, sijalicaTexture,NULL,&sijalicaBox);


            if(corn){
                SDL_Rect uniBox={mouseX,mouseY,50,50};
                SDL_ShowCursor(SDL_DISABLE);
                SDL_RenderCopy(gRenderer,unicornTexture,NULL,&uniBox);
            }
            else{
                SDL_ShowCursor(SDL_ENABLE);
            }
            SDL_RenderPresent( gRenderer );
            i=(i+1)%(5*256);

            while( SDL_PollEvent( &e ) != 0 ) {
                if( e.type == SDL_QUIT ){
                    quit = 1;
                }
                if(e.type==SDL_KEYDOWN){
                    if(e.key.keysym.sym==SDLK_u){
                        uni=1;
                    }
                    if(uni && e.key.keysym.sym==SDLK_c){
                        corn=1-corn;
                        //printf("Unicorn");
                    }
                }
                if(e.type==SDL_KEYUP){
                    uni=0;
                }
                else if(e.type==SDL_MOUSEMOTION){
                    mouseX=e.motion.x;
                    mouseY=e.motion.y;
                }
                else if(e.type==SDL_MOUSEBUTTONDOWN){
                    if(distance(hsBox,mouseX,mouseY)<=60){
                        inHS=1;
                        inMenu=0;
                    }

                    if(dist<=160){
                        //printf("START!\n");
                        start=clock();
                        inMenu=0;
                        inGame=1;
                        mouseInBackButton=0;
                        i=0;
                        int wannaLoad=0;
                        while(!wannaLoad){
                            SDL_RenderClear(gRenderer);

                            SDL_Color textColor = { 11, 11, 11 };
                            textSurface = TTF_RenderText_Solid(font , "Pocni novu igru", textColor);
                            textTexture = SDL_CreateTextureFromSurface(gRenderer,textSurface);

                            SDL_Rect textBox = {300,100,700,200};
                            SDL_RenderCopy(gRenderer,textTexture,NULL,&textBox);

                            SDL_FreeSurface(textSurface);
                            SDL_DestroyTexture(textTexture);

                            textSurface = TTF_RenderText_Solid(font , "Ucitaj staru igru", textColor);
                            textTexture = SDL_CreateTextureFromSurface(gRenderer,textSurface);

                            SDL_Rect textBox2 = {400,400,700,200};
                            SDL_RenderCopy(gRenderer,textTexture,NULL,&textBox2);

                            SDL_FreeSurface(textSurface);
                            SDL_DestroyTexture(textTexture);
                            if(corn){
                                SDL_Rect uniBox={mouseX,mouseY,50,50};
                                SDL_ShowCursor(SDL_DISABLE);
                                SDL_RenderCopy(gRenderer,unicornTexture,NULL,&uniBox);
                            }
                            else{
                                SDL_ShowCursor(SDL_ENABLE);
                            }
                            SDL_RenderPresent(gRenderer);

                            while( SDL_PollEvent( &e ) != 0 ){
                                if(e.type==SDL_QUIT){
                                    quit=1;
                                }
                                if(e.type==SDL_MOUSEBUTTONDOWN){
                                    if(inRect(textBox,mouseX,mouseY)){
                                        wannaLoad=-1;
                                        resetBoard(board);
                                        backup=*board;
                                        saveBoard(board);
                                    }
                                    if(inRect(textBox2,mouseX,mouseY)){
                                        wannaLoad=1;
                                        loadBoard(board);
                                        player=board->bigTile;
                                    }
                                }
                                if(e.type==SDL_KEYDOWN){
                                    if(e.key.keysym.sym==SDLK_u){
                                        uni=1;
                                    }
                                    if(uni && e.key.keysym.sym==SDLK_c){
                                        corn=1-corn;
                                        //printf("Unicorn");
                                    }
                                }
                                if(e.type==SDL_KEYUP){
                                    uni=0;
                                }
                                if(e.type==SDL_MOUSEMOTION){
                                    mouseX=e.motion.x;
                                    mouseY=e.motion.y;
                                }
                            }
                        }

                        player = board->playerOnTurn;
                    }
                    //{650,350,554,430}
                    else if((mouseX>optionsBox.x+30)&&mouseX<(optionsBox.x+optionsBox.w-30)&&mouseY>(optionsBox.y+150)&&mouseY<(optionsBox.y+optionsBox.h-30)){
                        //printf("OPTIONS!\n");
                        inMenu=0;
                        inOptions=1;
                        mouseInBackButton=0;
                        i=0;
                    }
                }
            }
        }

        else if (inGame) {
            //if (CLI) cliDrawBoard(board);
            /*else */
            while( SDL_PollEvent( &e ) != 0 ){
                if( e.type == SDL_QUIT ){
                    quit = 1;
                }
                if(e.type==SDL_KEYDOWN){
                    if(e.key.keysym.sym==SDLK_u){
                        uni=1;
                    }
                    if(uni && e.key.keysym.sym==SDLK_c){
                        corn=1-corn;
                        //printf("Unicorn");
                    }
                }
                if(e.type==SDL_KEYUP){
                    uni=0;
                }
                else if(e.type==SDL_MOUSEMOTION){
                    mouseX=e.motion.x;
                    mouseY=e.motion.y;
                }
            }

            //system("cls");
            paintBoard(board,gRenderer,xTexture,oTexture,boardTexture,emptySquareTexture,&e);
            SDL_RenderPresent( gRenderer );
            //drawBoard(board);

            // odigravanje poteza
            int tileX=-1,tileY=-1;
            int h=1;
            if(p1!=0 && p2!=0){
                SDL_ShowCursor(SDL_ENABLE);
            }
            if (player==1) {
                switch (p1) {
                case 0:
                    //if (CLI)
                        //cliPlayerMove(board, 1, &cY, &cX);

                    while(!played){
                        paintBoard(board,gRenderer,xTexture,oTexture,boardTexture,emptySquareTexture,&e);
                        if(corn){
                            SDL_Rect uniBox={mouseX,mouseY,50,50};
                            SDL_ShowCursor(SDL_DISABLE);
                            SDL_RenderCopy(gRenderer,unicornTexture,NULL,&uniBox);
                        }
                        else{
                            SDL_ShowCursor(SDL_ENABLE);
                        }

                        SDL_RenderPresent( gRenderer );

                        while(SDL_PollEvent(&e)){
                            if(e.type==SDL_KEYDOWN){
                                if(e.key.keysym.sym==SDLK_u){
                                    uni=1;
                                }
                                if(uni && e.key.keysym.sym==SDLK_c){
                                    corn=1-corn;
                                    //printf("Unicorn");
                                }
                                if(e.key.keysym.sym==SDLK_BACKSPACE){
                                    *board=backup;
                                    paintBoard(board,gRenderer,xTexture,oTexture,boardTexture,emptySquareTexture,&e);
                                }
                                if(h && e.key.keysym.sym==SDLK_h){
                                    getHint(board);
                                    SDL_SetRenderDrawColor( gRenderer, 0x33, 0x33, 0x33, 0x66 );
                                    SDL_Rect cellBox={xCells[board->hint%9],yCells[board->hint/9],68,68};
                                    SDL_RenderFillRect(gRenderer,&cellBox);
                                    SDL_RenderPresent( gRenderer );
                                    h=0;
                                    SDL_Delay(500);
                                    SDL_SetRenderDrawColor( gRenderer, 0xe9, 0xc2, 0x89, 0xFF );
                                }
                            }
                            if(e.type==SDL_KEYUP){
                                uni=0;
                                h=1;
                            }
                            if( e.type == SDL_QUIT ){
                                quit = 1;
                                played=99;
                            }
                            else if(e.type==SDL_MOUSEMOTION){
                                mouseX=e.motion.x;
                                mouseY=e.motion.y;
                            }
                            if(e.type==SDL_MOUSEBUTTONDOWN){
                                if(inRect(gotoMenuBox,mouseX,mouseY)){
                                    inGame=0;
                                    inMenu=1;
                                    backup=*board;
                                    saveBoard(board);
                                    played=-1;
                                    break;
                                }
                                for(int i=0;i<9;++i){
                                    if(mouseX>xCells[i]){
                                        tileX=i;
                                    }
                                    if(mouseY>yCells[i]){
                                        tileY=i;
                                    }
                                }
                                if(mouseX>xCells[8]+70 || mouseY>yCells[8]+70){
                                    tileX=-1;
                                    tileY=-1;
                                }
                                if(tileX!=-1 && tileY!=-1){
                                    int xTp=board->bigToPlay%3;
                                    int yTp=board->bigToPlay/3;
                                    if((board->bigToPlay==-1)||(tileX/3==xTp && tileY/3==yTp)){
                                        if(board->tiles[tileY][tileX]==0){
                                            backup=*board;
                                            doMove(board,tileY,tileX);
                                            saveBoard(board);
                                            played=1;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    played=0;

                    writeMove(player, board->lastMove);
                    break;
                case 1:
                    AIMinimaxMove(board, 1, time1, 4, 3+rand()%4);
                    writeMove(player, board->lastMove);
                    break;
                case 2:
                    AIMove1(board, 1, time1);
                    writeMove(player, board->lastMove);
                    break;
                case 3:
                    mct1=AIMonteCarloMove2(board, 1, mct1, time1);
                    writeMove(player, board->lastMove);
                    break;
                case 4:
                    AIMinimaxMove(board, 1, time1, 4, 1+rand()%2);
                    writeMove(player, board->lastMove);
                    break;
                case 5:
                    AIMoveFromFile(board, &player);
                    break;
                }
                player=3-player;
            } else if (player==2) {
                switch (p2) {
                case 0:
                    //if (CLI)
                        //cliPlayerMove(board, 2, &cY, &cX);
                    while(!played){
                        paintBoard(board,gRenderer,xTexture,oTexture,boardTexture,emptySquareTexture,&e);
                        if(corn){
                            SDL_Rect uniBox={mouseX,mouseY,50,50};
                            SDL_ShowCursor(SDL_DISABLE);
                            SDL_RenderCopy(gRenderer,unicornTexture,NULL,&uniBox);
                        }
                        else{
                            SDL_ShowCursor(SDL_ENABLE);
                        }

                        SDL_RenderPresent( gRenderer );

                        while(SDL_PollEvent(&e)){
                            if(e.type==SDL_KEYDOWN){
                                if(e.key.keysym.sym==SDLK_u){
                                    uni=1;
                                }
                                if(uni && e.key.keysym.sym==SDLK_c){
                                    corn=1-corn;
                                    //printf("Unicorn");
                                }
                                if(e.key.keysym.sym==SDLK_BACKSPACE){
                                    *board=backup;
                                    paintBoard(board,gRenderer,xTexture,oTexture,boardTexture,emptySquareTexture,&e);
                                }
                                if(h && e.key.keysym.sym==SDLK_h){
                                    getHint(board);
                                    SDL_SetRenderDrawColor( gRenderer, 0x33, 0x33, 0x33, 0x66 );
                                    SDL_Rect cellBox={xCells[board->hint%9],yCells[board->hint/9],68,68};
                                    SDL_RenderFillRect(gRenderer,&cellBox);
                                    SDL_RenderPresent( gRenderer );
                                    h=0;
                                    SDL_Delay(500);
                                    SDL_SetRenderDrawColor( gRenderer, 0xe9, 0xc2, 0x89, 0xFF );
                                }
                            }
                            if(e.type==SDL_KEYUP){
                                h=1;
                                uni=0;
                            }
                            if( e.type == SDL_QUIT ){
                                quit = 1;
                                played=99;
                            }
                            else if(e.type==SDL_MOUSEMOTION){
                                mouseX=e.motion.x;
                                mouseY=e.motion.y;
                            }
                            if(e.type==SDL_MOUSEBUTTONDOWN){
                                if(inRect(gotoMenuBox,mouseX,mouseY)){
                                    inGame=0;
                                    inMenu=1;
                                    backup=*board;
                                    saveBoard(board);
                                    played=-1;
                                    break;
                                }
                                for(int i=0;i<9;++i){
                                    if(mouseX>xCells[i]){
                                        tileX=i;
                                    }
                                    if(mouseY>yCells[i]){
                                        tileY=i;
                                    }
                                }
                                if(mouseX>xCells[8]+70 || mouseY>yCells[8]+70){
                                    tileX=-1;
                                    tileY=-1;
                                }
                                if(tileX!=-1 && tileY!=-1){
                                    int xTp=board->bigToPlay%3;
                                    int yTp=board->bigToPlay/3;
                                    if((board->bigToPlay==-1)||(tileX/3==xTp && tileY/3==yTp)){
                                        if(board->tiles[tileY][tileX]==0){
                                            backup=*board;
                                            doMove(board,tileY,tileX);
                                            saveBoard(board);
                                            played=1;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    played=0;

                    writeMove(player, board->lastMove);
                    break;

                case 1:
                    AIMinimaxMove(board, 2, time1, 4, 3+rand()%4);
                    writeMove(player, board->lastMove);
                    break;
                case 2:
                    AIMove1(board, 2, time2);
                    writeMove(player, board->lastMove);
                    break;
                case 3:
                    mct2=AIMonteCarloMove2(board, 2, mct2, time2);
                    writeMove(player, board->lastMove);
                    break;
                case 4:
                    AIMinimaxMove(board, 2, time2, 4, 1+rand()%2);
                    writeMove(player, board->lastMove);
                    break;
                case 5:
                    AIMoveFromFile(board, &player);
                    break;
                }
                player=3-player;
            }
            //if (CLI) mvprintw(8, 15, "%lf", 1.0*(clock()-start)/CLOCKS_PER_SEC);
            if (board->win) {

                paintBoard(board,gRenderer,xTexture,oTexture,boardTexture,emptySquareTexture,&e);
                SDL_RenderPresent(gRenderer);
                SDL_Delay(3000);

                if(((p1==0 && p2==3 &&board->win==1)||(p1==3 && p2==0 &&board->win==2))&&(board->turns<entry[0].turns)){
                    int entered=0;
                    char ime[30];
                    for(int asd=0;asd<30;++asd) ime[asd]='\0';
                    int imeIndex=0;
                    while(!entered){
                        SDL_RenderClear(gRenderer);
                        SDL_SetRenderDrawColor( gRenderer,100+rand()%150,100+rand()%150,100+rand()%150,200+rand()%50);
                        SDL_Color textColor ={11,11,11};
                        textSurface = TTF_RenderText_Solid(font , "New High Score! Enter your name:", textColor);
                        textTexture = SDL_CreateTextureFromSurface(gRenderer,textSurface);
                        SDL_Rect textBox = {200,100,900,150};
                        SDL_RenderCopy(gRenderer,textTexture,NULL,&textBox);
                        SDL_FreeSurface(textSurface);
                        SDL_DestroyTexture(textTexture);

                        textSurface = TTF_RenderText_Solid(font , ime, textColor);
                        textTexture = SDL_CreateTextureFromSurface(gRenderer,textSurface);
                        SDL_Rect textBoxIme = {200,400-2*imeIndex,70*imeIndex,150+4*imeIndex};
                        SDL_RenderCopy(gRenderer,textTexture,NULL,&textBoxIme);
                        SDL_RenderPresent(gRenderer);
                        SDL_FreeSurface(textSurface);
                        SDL_DestroyTexture(textTexture);

                        while(SDL_PollEvent(&e)>0){
                            if(e.type==SDL_KEYDOWN){
                                if(e.key.keysym.sym==SDLK_BACKSPACE && imeIndex>0){
                                    ime[--imeIndex]='\0';
                                }
                                if(e.key.keysym.sym>=SDLK_a && e.key.keysym.sym<=SDLK_z && imeIndex<14){
                                    ime[imeIndex++]='A'+e.key.keysym.sym-SDLK_a;
                                }
                                if(e.key.keysym.sym>=SDLK_0 && e.key.keysym.sym<=SDLK_9 && imeIndex<14){
                                    ime[imeIndex++]='0'+e.key.keysym.sym-SDLK_0;
                                }
                                if(imeIndex>0 && imeIndex<14){
                                    switch(e.key.keysym.sym){
                                    case SDLK_PERIOD:
                                        ime[imeIndex++]='.';
                                        break;
                                    case SDLK_SPACE:
                                        ime[imeIndex++]=' ';
                                        break;
                                    case SDLK_MINUS:
                                        ime[imeIndex++]='-';
                                        break;
                                    }
                                }
                                if((e.key.keysym.sym==SDLK_RETURN || e.key.keysym.sym==SDLK_RETURN)&&imeIndex>=3){
                                    entered=1;
                                }
                            }
                        }
                    }
                    for(int ww=0;ww<18;++ww) ime[ww]=(ime[ww]?ime[ww]:' ');
                    t = time(0);
                    addEntry(board->turns,t, ime);
                    printHighScores();
                    saveHighScores();
                    loadHighScores();
                }

                if (board->win==1)
                    ++(board->wins1);
                else if (board->win==2)
                    ++(board->wins2);
                else ++(board->draws);
                /*if (CLI) {
                    cliDrawBoard(board);
                    refresh();
                }
                if (!autoRestart && CLI) {
                    while ((ch=getch())!='r')
                        if (ch=='q') {
                            curs_set(1);
                            exit(0);
                        }
                }*/
                //system("cls");
                //drawBoard(board);


                while(SDL_PollEvent(&e));
                inMenu=1;
                inGame=0;
                f = fopen(SAVEFILE, "w");   //Samo brise bord
                fclose(f);
                resetBoard(board);
                mct1=deleteMCTree(mct1);
                mct2=deleteMCTree(mct2);


                //if (CLI) cX=6, cY=5;
            }
        }

        else if(inOptions){
            i=(i+1)%500;
            SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

            SDL_Rect wallBox={0,0,SCREEN_WIDTH,SCREEN_HEIGHT};
            SDL_Rect backRect={50-(mouseInBackButton?i/25:0),SCREEN_HEIGHT*3/4,200,200};
            mouseInBackButton=(mouseX>backRect.x && mouseX<backRect.x+backRect.w && mouseY>backRect.y && mouseY<backRect.y+backRect.h);
            SDL_Rect humanBox1={410,200,130,130};
            SDL_Rect humanBox2={610,200,130,130};
            SDL_Rect easyBox1={410,330,130,130};
            SDL_Rect easyBox2={610,330,130,130};
            SDL_Rect medBox1={410,460,130,130};
            SDL_Rect medBox2={610,460,130,130};
            SDL_Rect hardBox1={410,590,130,130};
            SDL_Rect hardBox2={610,590,130,130};
            SDL_Rect txtBox1={260,60,130,130};
            SDL_Rect txtBox2={760,60,130,130};

            SDL_Rect p1Box={400+(rand()%16-8)*max(0,200-distInt(475,125,mouseX,mouseY))/200,50+(rand()%16-8)*max(0,200-distInt(475,125,mouseX,mouseY))/200,150,150};
            SDL_Rect p2Box={600+(rand()%16-8)*max(0,200-distInt(675,125,mouseX,mouseY))/200,50+(rand()%10-5)*max(0,200-distInt(675,125,mouseX,mouseY))/200,150,150};




            SDL_RenderCopy(gRenderer, optionsWallTexture,NULL,&wallBox);
            SDL_RenderCopy(gRenderer,optionsBackTexture,NULL,&backRect);

            SDL_SetTextureAlphaMod(easyTexture,(p1==4?255:90));
            SDL_RenderCopy(gRenderer, easyTexture,NULL,&easyBox1);
            SDL_SetTextureAlphaMod(easyTexture,(p2==4?255:90));
            SDL_RenderCopy(gRenderer, easyTexture,NULL,&easyBox2);

            SDL_SetTextureAlphaMod(medTexture,(p1==1?255:90));
            SDL_RenderCopy(gRenderer, medTexture,NULL,&medBox1);
            SDL_SetTextureAlphaMod(medTexture,(p2==1?255:90));
            SDL_RenderCopy(gRenderer, medTexture,NULL,&medBox2);

            SDL_SetTextureAlphaMod(txtTexture,(p1==5?255:90));
            SDL_RenderCopy(gRenderer, txtTexture,NULL,&txtBox1);
            SDL_SetTextureAlphaMod(txtTexture,(p2==5?255:90));
            SDL_RenderCopy(gRenderer, txtTexture,NULL,&txtBox2);

            SDL_SetTextureAlphaMod(hardTexture,(p1==3?255:90));
            SDL_RenderCopy(gRenderer, hardTexture,NULL,&hardBox1);
            SDL_SetTextureAlphaMod(hardTexture,(p2==3?255:90));
            SDL_RenderCopy(gRenderer, hardTexture,NULL,&hardBox2);

            SDL_SetTextureAlphaMod(humanTexture,(p1==0?255:90));
            SDL_RenderCopy(gRenderer, humanTexture,NULL,&humanBox1);
            SDL_SetTextureAlphaMod(humanTexture,(p2==0?255:90));
            SDL_RenderCopy(gRenderer, humanTexture,NULL,&humanBox2);
            SDL_RenderCopy(gRenderer, p1Texture,NULL,&p1Box);
            SDL_RenderCopy(gRenderer, p2Texture,NULL,&p2Box);


            if(corn){
                if(unicornTexture==NULL){
                    //printf("nok\n");
                }
                SDL_Rect uniBox={mouseX,mouseY,50,50};
                SDL_ShowCursor(SDL_DISABLE);
                SDL_RenderCopy(gRenderer,unicornTexture,NULL,&uniBox);
            }
            else{
                SDL_ShowCursor(SDL_ENABLE);
            }
            SDL_RenderPresent( gRenderer );



            while( SDL_PollEvent( &e ) != 0 ) {
                if(e.type==SDL_KEYDOWN){
                    if(e.key.keysym.sym==SDLK_u){
                        uni=1;
                    }
                    if(uni && e.key.keysym.sym==SDLK_c){
                        corn=1-corn;
                        //printf("Unicorn");
                    }
                }
                if(e.type==SDL_KEYUP){
                    uni=0;
                }
                if(e.type==SDL_KEYDOWN){
                    if(e.key.keysym.sym>=SDLK_0 && e.key.keysym.sym<=SDLK_9){
                        time1=e.key.keysym.sym-SDLK_0;
                        time2=e.key.keysym.sym-SDLK_0;
                        if(time1==0){
                            time1=10;
                            time2=10;
                        }
                    }
                }

                if( e.type == SDL_QUIT ){
                    quit = 1;
                }
                else if(e.type==SDL_MOUSEMOTION){
                    mouseX=e.motion.x;
                    mouseY=e.motion.y;
                }
                else if(e.type==SDL_MOUSEBUTTONDOWN && mouseInBackButton){
                    inOptions=0;
                    inMenu=1;
                    i=0;
                    mouseInBackButton=0;
                }
                else if(e.type==SDL_MOUSEBUTTONDOWN){
                    if(inRect(humanBox1,mouseX,mouseY)){
                        p1=0;
                    }
                    if(inRect(humanBox2,mouseX,mouseY)){
                        p2=0;
                    }
                    if(inRect(easyBox1,mouseX,mouseY)){
                        p1=4;
                    }
                    if(inRect(easyBox2,mouseX,mouseY)){
                        p2=4;
                    }
                    if(inRect(medBox1,mouseX,mouseY)){
                        p1=1;
                    }
                    if(inRect(medBox2,mouseX,mouseY)){
                        p2=1;
                    }
                    if(inRect(hardBox1,mouseX,mouseY)){
                        p1=3;
                    }
                    if(inRect(hardBox2,mouseX,mouseY)){
                        p2=3;
                    }
                    if(inRect(txtBox1,mouseX,mouseY)){
                        if(p2!=5)
                            p1=5;
                    }
                    if(inRect(txtBox2,mouseX,mouseY)){
                        if(p1!=5)
                            p2=5;
                    }
                }
            }

        }

        if(inHS){
            SDL_SetRenderDrawColor( gRenderer, 128+127*sin((double)clock()/500), 128+127*sin((double)clock()/2000), 128-127*sin((double)clock()/500), 0xFF );
            SDL_Color textColor={11,11,11};
            for(int i=0;i<10;++i){
                char* displayThing=calloc(100,sizeof(char));
                itoa(10-i,displayThing,10);
                strcat(displayThing,".");
                if(i!=0) strcat(displayThing," ");
                strcat(displayThing,entry[i].name);
                char tmp[30];
                itoa(entry[i].turns,tmp,10);
                strcat(displayThing,tmp);
                strcat(displayThing,"   ");
                char* date;
                date=ctime(&(entry[i].date));
                char* date2=calloc(15,sizeof(char));
                strncpy(date2,date+4,7);
                strncpy(date2+7,date+20,5);
                strcat(displayThing,date2);
                //printf("%s",displayThing);
                SDL_Rect textBox={250,600-i*50,700,40};
                textSurface = TTF_RenderText_Solid(font , displayThing, textColor);
                textTexture = SDL_CreateTextureFromSurface(gRenderer,textSurface);

                SDL_RenderCopy(gRenderer,textTexture,NULL,&textBox);
                SDL_FreeSurface(textSurface);
                SDL_DestroyTexture(textTexture);

            }
            SDL_RenderPresent(gRenderer);

            while(SDL_PollEvent(&e)>0){
                if(e.type==SDL_MOUSEMOTION){
                    mouseX=e.motion.x;
                    mouseY=e.motion.y;
                }
                if(e.type==SDL_QUIT){
                    quit=1;
                }
                if(e.type==SDL_MOUSEBUTTONDOWN || e.type==SDL_KEYDOWN){
                    inMenu=1;
                    inHS=0;
                }
            }
        }

    }
}
