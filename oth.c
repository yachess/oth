/***
 *
 * oth.c - Othello 
 * simple othello game (Reversi)
 * utilizing bitboards
 *
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>

const char side[2][12]={"Black","White"};
const int HUMAN=0;
const int RANDOM=1;
const int AI=2;
const int player_type[2]={RANDOM,AI};

const int WEIGHTS[64]={
  120,-20,2,1,1,2,-20,120,
  -20,-10,1,1,1,1,-10,-20,
  2,1,1,1,1,1,1,2,
  1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,
  2,1,1,1,1,1,1,2,
  -20,-10,1,1,1,1,-10,-20,
  120,-20,2,1,1,2,-20,120,
};

typedef struct {
  long bb[2];
  int last_mv;
  char t;
  int ply;
} Board;

void print_board(Board board){
  int i,j;
  printf("  a b c d e f g h\n");
  for (i=0;i<8;i++) {
    printf("%d",i+1);
    for (j=0;j<8;j++) {
      int sq = i*8+j;
      if (board.bb[0] & 1L << sq)
        if (board.last_mv == sq)
          printf(">X");
        else
          printf(" X");
      else if (board.bb[1] & 1L << sq)
        if (board.last_mv == sq)
          printf(">O");
        else
          printf(" O");
      else
        printf(" .");
    }
    printf("\n");
  }
}

int eval(Board *b){
  int val=0;
  if (b->ply > 50 || b->ply < 15) { 
    /* in endgame is it meaningless to weight squares. thus only count stones */
    for (int i=0;i<64;i++)
      if (b->bb[0] & 1L<<i)
        val-=1;
      else if (b->bb[1] & 1L<<i)
        val+=1;
  } /* middle game */
  else {
    for (int i=0;i<64;i++)
      if (b->bb[0] & 1L<<i)
        val-=WEIGHTS[i];
      else if (b->bb[1] & 1L<<i)
        val+=WEIGHTS[i];
  }
  return val;
}

int count(Board *b,char clr){
  return __builtin_popcountl(b->bb[clr]);
}

void put(Board *b, char sq, char color){
  b->bb[color] |= 1L << sq;
}

void flip(Board *b, char sq){
  b->bb[0] ^= 1L << sq;
  b->bb[1] ^= 1L << sq;
}

int nota_to_sq(char* nota){
  return ((int)(nota[1]-'1')*8 + nota[0]-'a');
}

int sq_to_nota(char sq,char* nota){
  nota[0]='a'+(sq%8);
  nota[1]='1'+(sq/8);
  nota[2]=0;
  return 0;
}

int avail_moves(Board *b,  char *moves){
  char size=0;
  int k,delta,tsq;

  long alloc = b->bb[0] | b->bb[1];

  for (int sq=0;sq<64;sq++){
    if (alloc & 1L << sq)
      continue;   
    k = 1;
    delta = -9;   /*  NW  */
    
    tsq = sq+delta*k;
    while (tsq%8 < sq%8 && tsq >=0 && (b->bb[b->t ^ 1] & 1L << tsq))
      tsq = sq + delta * ++k;
    if ( k>1 && tsq%8 < sq%8 && tsq >= 0 &&
        (b->bb[b->t] & 1L << tsq)){
      moves[size+1]=sq;
      size++;
      continue;
    } 
    k = 1;
    delta = -8;   /*  N  */
    tsq = sq+delta*k;
    while ( tsq >=0 &&(b->bb[b->t ^ 1] & 1L << tsq))
      tsq = sq + delta * ++k;
    if ( k>1 && tsq >= 0 &&(b->bb[b->t] & 1L << tsq)){
      moves[size+1]=sq;
      size++;
      continue;
    } 
    k = 1;
    delta = -7;   /*  NE  */
    tsq = sq+delta*k;
    while ( tsq%8 > sq%8 && tsq >=0 &&(b->bb[b->t ^ 1] & 1L << tsq))
      tsq = sq + delta * ++k;
    if ( k>1 && tsq%8 > sq%8 && tsq >=0 &&(b->bb[b->t] & 1L << tsq)){
      moves[size+1]=sq;
      size++;
      continue;
    } 
    k = 1;
    delta = -1;   /*  W  */
    tsq = sq+delta*k;
    while ( tsq%8 < sq%8 && tsq >=0 &&(b->bb[b->t ^ 1] & 1L << tsq))
      tsq = sq + delta * ++k;
    if ( k>1 && tsq%8 < sq%8 && tsq >=0 &&(b->bb[b->t] & 1L << tsq)){
      moves[size+1]=sq;
      size++;
      continue;
    } 
    k = 1;
    delta = 1;   /*  E  */
    tsq = sq+delta*k;
    while ( tsq%8 > sq%8 && tsq < 64 &&(b->bb[b->t ^ 1] & 1L << tsq))
      tsq = sq + delta * ++k;
    if ( k>1 && tsq%8 > sq%8 && tsq < 64 &&(b->bb[b->t] & 1L << tsq)){
      moves[size+1]=sq;
      size++;
      continue;
    } 
    k = 1;
    delta = 7;   /*  SW  */
    tsq = sq+delta*k;
    while ( tsq%8 < sq%8 && tsq < 64 && (b->bb[b->t ^ 1] & 1L << tsq))
      tsq = sq + delta * ++k;
    if ( k>1 && tsq%8 < sq%8 && tsq < 64 && (b->bb[b->t] & 1L << tsq)){
      moves[size+1]=sq;
      size++;
      continue;
    } 
    k=1;
    delta = 8;   /*  S  */
    tsq = sq+delta*k;
    while ( tsq < 64 &&(b->bb[b->t ^ 1] & 1L << tsq))
      tsq = sq + delta * ++k;
    if ( k>1 && tsq < 64 &&(b->bb[b->t] & 1L << tsq)){
      moves[size+1]=sq;
      size++;
      continue;
    } 
    k=1;
    delta = 9;   /*  SE  */
    tsq = sq+delta*k;
    while ( tsq%8 > sq%8 && tsq < 64 &&(b->bb[b->t ^ 1] & 1L << tsq))
      tsq = sq + delta * ++k;
    if ( k>1 && tsq%8 > sq%8 && tsq < 64 &&(b->bb[b->t] & 1L << tsq)){
      moves[size+1]=sq;
      size++;
      continue;
    } 
  }
  moves[0] = size;
}
        

int in_moves(char moves[],char mv){
  int i;
  for (i=0;i<moves[0];i++){
    if (mv == moves[i+1])
      return 1;
  }
  return 0;
}

int make_move(Board *b,char sq){
  int k = 1;
  int delta = -9;   /*  NW  */
  int tsq = sq+delta*k;
  char clr = b->t;

  if (sq != -1){
    while (tsq%8 < sq%8 && tsq >=0 &&
        (b->bb[clr ^ 1] & 1L << tsq))
      tsq = sq+delta*++k;
    if ( k>1 && tsq%8 < sq%8 && tsq >= 0 &&
        (b->bb[clr] & 1L << tsq))
      for (int q=1;q<k;q++)
        flip(b, sq+delta*q);
    k = 1;
    delta = -8;   /*  N  */
    tsq = sq+delta*k;
    while ( tsq >=0 &&(b->bb[clr ^ 1] & 1L << tsq))
      tsq = sq+delta* ++k;
    if ( k>1 && tsq >= 0 &&(b->bb[clr] & 1L << tsq))
      for (int q=1;q<k;q++)
        flip(b, sq+delta*q);
    k = 1;
    delta = -7;   /*  NE  */
    tsq = sq+delta*k;
    while ( tsq%8 > sq%8 && tsq >=0 &&(b->bb[clr ^ 1] & 1L << tsq))
      tsq = sq+delta* ++k;
    if ( k>1 && tsq%8 > sq%8 && tsq >=0 &&(b->bb[clr] & 1L << tsq))
      for (int q=1;q<k;q++)
        flip(b, sq+delta*q);
    k = 1;
    delta = -1;   /*  W  */
    tsq = sq+delta*k;
    while ( tsq%8 < sq%8 && tsq >=0 &&(b->bb[clr ^ 1] & 1L << tsq))
      tsq = sq+delta* ++k;
    if ( k>1 && tsq%8 < sq%8 && tsq >=0 &&(b->bb[clr] & 1L << tsq))
      for (int q=1;q<k;q++)
        flip(b, sq+delta*q);
    k = 1;
    delta = 1;   /*  E  */
    tsq = sq+delta*k;
    while ( tsq%8 > sq%8 && tsq < 64 &&(b->bb[clr ^ 1] & 1L << tsq))
      tsq = sq+delta* ++k;
    if ( k>1 && tsq%8 > sq%8 && tsq < 64 &&(b->bb[clr] & 1L << tsq))
      for (int q=1;q<k;q++)
        flip(b, sq+delta*q);
    k = 1;
    delta = 7;   /*  SW  */
    tsq = sq+delta*k;
    while ( tsq%8 < sq%8 && tsq < 64 &&(b->bb[clr ^ 1] & 1L << tsq))
      tsq = sq+delta* ++k;
    if ( k>1 && tsq%8 < sq%8 && tsq < 64 &&(b->bb[clr] & 1L << tsq))
      for (int q=1;q<k;q++)
        flip(b, sq+delta*q);
    k=1;
    delta = 8;   /*  S  */
    tsq = sq+delta*k;
    while ( tsq < 64 &&(b->bb[clr ^ 1] & 1L << tsq))
      tsq = sq+delta* ++k;
    if ( k>1 && tsq < 64 &&(b->bb[clr] & 1L << tsq))
      for (int q=1;q<k;q++)
        flip(b, sq+delta*q);
    k=1;
    delta = 9;   /*  SE  */
    tsq = sq+delta*k;
    while ( tsq%8 > sq%8 && tsq < 64 &&(b->bb[clr ^ 1] & 1L << tsq))
      tsq = sq+delta* ++k;
    if ( k>1 && tsq%8 > sq%8 && tsq < 64 &&(b->bb[clr] & 1L << tsq))
      for (int q=1;q<k;q++)
        flip(b, sq+delta*q);

    put(b,sq,clr);
  }

  b->t ^= 1;
  b->last_mv = sq;
  b->ply ++;
}

void print_moves(char moves[]){
  char nota[3];
  printf("size:%d\t",moves[0]);
  if (moves[0]>0){
    for (int i=0;i<moves[0];i++){
      sq_to_nota(moves[i+1],nota);
      printf("%s:",nota);
    }
    printf("\n");
  }
}

/***
 *
 * ai_player
 *
 ***/

int minimax(Board *b, int v, int depth, char* mv){
  int max=INT_MIN;
  int min=INT_MAX;
  char idx=-1;
  char moves[64];

  avail_moves(b, moves);

//if (b->ply>1 && b->ply<15) {  /* In opening, only mobility matters */
//  return moves[0]*(b->t==0?-1:1);
//}
  int myval=eval(b);
  if (depth == 0)
    return myval+moves[0]*5*(b->t==0?-1:1); 
  if (moves[0]==0)
    return myval;

  for (int i=0;i<moves[0];i++){
    Board b2 = *b;
    char mv2;
    make_move(&b2, moves[i+1]);
    int val =minimax(&b2, v, depth-1, &mv2);

    if(b->t==0) {     /* mini ... */
      if (myval-14>val){ /* killer move */
        *mv = moves[i+1];
        return val;
      }

      if (min > val){
        idx = i;
        min = val;
        *mv = moves[i+1];
      } 

    }
    else {            /* ... max */
      if (myval+14<val){  /* killer move */
        *mv = moves[i+1];
        return val;
      }
      if (max < val){
        idx = i;
        max = val;
        *mv = moves[i+1];
      }
    }
  }

  if (b->t==0) return min;
  return max;
}


char get_ai_move(Board *b,char moves[],char type){
  char mv;
  if (type==RANDOM) {
    srand(time(NULL));
    int r = rand() % moves[0];
    return moves[r+1];
  }
  else {
    int depth = 8;
    if (b->ply > 50)
      depth = 14;
    int v = minimax(b,0,depth,&mv);
    return mv;
  }
}


int main(int argc ,char** argv){
  char moves[64];

  Board b= { 0L, 0L };
  b.t=0;
  b.ply=0;

  put(&b,3*8+3,1);  /* white */
  put(&b,4*8+4,1);
  put(&b,3*8+4,0);  /* black */
  put(&b,4*8+3,0);

  print_board(b);

  char nota[3];

  int mv = nota_to_sq(nota);
  b.t=0;
  avail_moves(&b,moves);
  print_moves(moves);

  while (~(b.bb[0] | b.bb[1]) != 0L) {

    if (player_type[b.t]==HUMAN){
      printf("Enter Your Move:");
      scanf("%s",nota);
      mv = nota_to_sq(nota);
    } else {
      mv = get_ai_move(&b,moves,player_type[b.t]);
    }
    if (in_moves(&moves[0],mv)){
        make_move(&b,mv);
        print_board(b);
        avail_moves(&b,moves);
        print_moves(moves);
        if (moves[0]==0){
          printf("%s has no move.Passing.\n",side[b.t]);
          make_move(&b,-1);
          avail_moves(&b,moves);
          if (moves[0]==0)
            break;
        }
    }
    else
      printf("Wrong move\n");
  }
  /*judge*/
  int blk = count(&b,0);  
  int wht = count(&b,1); 
  if (blk>wht)
    printf("0-1\n");
  else if (blk==wht)
    printf("1/2-1/2\n");
  else
    printf("1-0\n");

  return 0;
}
