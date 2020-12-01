#include <stdlib.h>

#include "board.h"
#include "cli.h"

void
cliPlayerMove(Board *b, int player, int *cY, int *cX) 
{
    int y, x, *moves, ch;
    if (player!=b->playerOnTurn) // ovo je vrv nepotrebno
        return;
    curs_set(1);
    wmove(stdscr, *cY, *cX);
    while (ch=getch()) {
        if (ch=='a' || ch==KEY_LEFT || ch=='h') {
            if (*cX==5 || *cX==10) *cX-=3;
            else --(*cX);
        }
        else if (ch=='w' || ch==KEY_UP || ch=='k') {
            if (*cY==4 || *cY==8) *cY-=2;
            else --(*cY);
        }
        else if (ch=='d' || ch==KEY_RIGHT || ch=='l') {
            if (*cX==2 || *cX==7) *cX+=3;
            else ++(*cX);
        }
        else if (ch=='s' || ch==KEY_DOWN || ch=='j') {
            if (*cY==2 || *cY==6) *cY+=2;
            else ++(*cY);
        }
        else if (ch=='A' || ch=='H')
            *cX-=5;
        else if (ch=='W' || ch=='K')
            *cY-=4;
        else if (ch=='D' || ch=='L')
            *cX+=5;
        else if (ch=='S' || ch=='J')  
            *cY+=4;
        else if (ch=='?') {
            getHint(b);
            cliDrawBoard(b);
        }
        else if (ch=='q')
            exit(0);
        else if (ch=='r') {
            b->win=3-player;
            curs_set(0);
            return;
        }
        else if (ch=='\n') {
            y=*cY, x=*cX;
            if (x>2) x-=2;
            if (x>7) x-=2;
            if (y>2) --y;
            if (y>6) --y;
            moves=allMoves(b);
            for (int i=1; i<moves[0]+1; ++i) {
                if(moves[i]==y*9+x) {
                    doMove(b, y, x);
                    curs_set(0);
                    b->hint=-1;
                    return;
                }
            }
            free(moves);
        }
        if (*cX<0) *cX=0;
        if (*cY<0) *cY=0;
        if (*cX>12) *cX=12;
        if (*cY>10) *cY=10;
        wmove(stdscr, *cY, *cX);
        refresh();
    }
    return;
}

void
cliDrawBoard(Board *b) 
{
    int *moves=allMoves(b), x, y;
    char ch;
    for (int i=0, y=0; y<11; ++i, ++y) {
        if (y==3 || y==7) {
            mvprintw(y, 0, "=============");
            ++y;
        }
        for (int j=0, x=0; x<13; ++j, ++x) {
            if (x==3 || x==8) {
                mvprintw(y, x, "||");
                x+=2;
            }
            if (b->bigTile[i/3*3+j/3]==1)
                mvprintw(y, x, "X");
            else if (b->bigTile[i/3*3+j/3]==2)
                mvprintw(y, x, "O");
            else if (b->tiles[i][j]==1)
                mvprintw(y, x, "X");
            else if (b->tiles[i][j]==2)
                mvprintw(y, x, "O");
            else if (b->tiles[i][j]==0)
                mvprintw(y, x, " ");
        }
    }
    if (moves) {
        for (int i=1; i<moves[0]+1; ++i) {
            y=moves[i]/9;
            y+=y/3;
            x=moves[i]%9;
            x+=x/3*2;
            mvaddch(y, x, '+');
        }
    }
    if (b->hint!=-1)
        mvaddch(b->hint/9+b->hint/9/3, b->hint%9+(b->hint%9)/3*2, '*');
    if (b->win==1)
        mvprintw(12, 0, "Pobedio je X");
    else if (b->win==2)
        mvprintw(12, 0, "Pobedio je O");
    else if (b->win==3)
        mvprintw(12, 0, "Nereseno je");
    else mvprintw(12, 0, "            ");
    if (b->win)
        mvprintw(13, 0, "Pritisnite r da igrate opet");
    else mvprintw(13, 0, "                           ");
    if (b->playerOnTurn==1) ch='X';
    else ch='O';
    mvprintw(2, 15, "Igrac na potezu: %c", ch);
    mvprintw(4, 15, "Pobede X: %d", b->wins1);
    mvprintw(5, 15, "Pobede O: %d", b->wins2);
    mvprintw(6, 15, "Nereseno: %d", b->draws);
    refresh();
    if (moves)
        free(moves);
}
