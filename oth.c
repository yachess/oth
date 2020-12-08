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

const char side[2][12]={"Black","White"};

typedef struct {
  long bb[2];
} Board;

void print_board(Board board){
  int i,j;
  printf("  a b c d e f g h\n");
  for (i=0;i<8;i++) {
    printf("%d",i+1);
    for (j=0;j<8;j++) {
      if (board.bb[0] & 1L << (i*8+j))
        printf(" X");
      else if (board.bb[1] & 1L << (i*8+j))
        printf(" O");
      else
        printf(" .");
    }
    printf("\n");
  }
}

int eval(Board *b){
  int val=0;
  for (int i=0;i<64;i++)
    if (b->bb[0] & 1L<<i)
      val++;
    else if (b->bb[1] & 1L<<i)
      val--;
  return val;
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

int avail_moves(Board *b, char clr, char *moves){
  char size=0;
  int k,delta,tsq;

  long alloc = b->bb[0] | b->bb[1];

  for (int sq=0;sq<64;sq++){
    if (alloc & 1L << sq)
      continue;   
    k = 1;
    delta = -9;   /*  NW  */
    
    tsq = sq+delta*k;
    while (tsq%8 < sq%8 && tsq >=0 && (b->bb[clr ^ 1] & 1L << tsq))
      tsq = sq + delta * ++k;
    if ( k>1 && tsq%8 < sq%8 && tsq >= 0 &&
        (b->bb[clr] & 1L << tsq)){
      moves[size+1]=sq;
      size++;
      continue;
    } 
    k = 1;
    delta = -8;   /*  N  */
    tsq = sq+delta*k;
    while ( tsq >=0 &&(b->bb[clr ^ 1] & 1L << tsq))
      tsq = sq + delta * ++k;
    if ( k>1 && tsq >= 0 &&(b->bb[clr] & 1L << tsq)){
      moves[size+1]=sq;
      size++;
      continue;
    } 
    k = 1;
    delta = -7;   /*  NE  */
    tsq = sq+delta*k;
    while ( tsq%8 > sq%8 && tsq >=0 &&(b->bb[clr ^ 1] & 1L << tsq))
      tsq = sq + delta * ++k;
    if ( k>1 && tsq%8 > sq%8 && tsq >=0 &&(b->bb[clr] & 1L << tsq)){
      moves[size+1]=sq;
      size++;
      continue;
    } 
    k = 1;
    delta = -1;   /*  W  */
    tsq = sq+delta*k;
    while ( tsq%8 < sq%8 && tsq >=0 &&(b->bb[clr ^ 1] & 1L << tsq))
      tsq = sq + delta * ++k;
    if ( k>1 && tsq%8 < sq%8 && tsq >=0 &&(b->bb[clr] & 1L << tsq)){
      moves[size+1]=sq;
      size++;
      continue;
    } 
    k = 1;
    delta = 1;   /*  E  */
    tsq = sq+delta*k;
    while ( tsq%8 > sq%8 && tsq < 64 &&(b->bb[clr ^ 1] & 1L << tsq))
      tsq = sq + delta * ++k;
    if ( k>1 && tsq%8 > sq%8 && tsq < 64 &&(b->bb[clr] & 1L << tsq)){
      moves[size+1]=sq;
      size++;
      continue;
    } 
    k = 1;
    delta = 7;   /*  SW  */
    tsq = sq+delta*k;
    while ( tsq%8 < sq%8 && tsq < 64 && (b->bb[clr ^ 1] & 1L << tsq))
      tsq = sq + delta * ++k;
    if ( k>1 && tsq%8 < sq%8 && tsq < 64 && (b->bb[clr] & 1L << tsq)){
      moves[size+1]=sq;
      size++;
      continue;
    } 
    k=1;
    delta = 8;   /*  S  */
    tsq = sq+delta*k;
    while ( tsq < 64 &&(b->bb[clr ^ 1] & 1L << tsq))
      tsq = sq + delta * ++k;
    if ( k>1 && tsq < 64 &&(b->bb[clr] & 1L << tsq)){
      moves[size+1]=sq;
      size++;
      continue;
    } 
    k=1;
    delta = 9;   /*  SE  */
    tsq = sq+delta*k;
    while ( tsq%8 > sq%8 && tsq < 64 &&(b->bb[clr ^ 1] & 1L << tsq))
      tsq = sq + delta * ++k;
    if ( k>1 && tsq%8 > sq%8 && tsq < 64 &&(b->bb[clr] & 1L << tsq)){
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

int make_move(Board *b,char sq,int clr){
  int k = 1;
  int delta = -9;   /*  NW  */
  int tsq = sq+delta*k;
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

int main(int argc ,char** argv){
  char moves[64];

  Board b= { 0L, 0L };

  put(&b,3*8+3,1);  /* white */
  put(&b,4*8+4,1);
  put(&b,3*8+4,0);  /* black */
  put(&b,4*8+3,0);

  print_board(b);

  avail_moves(&b,0,moves);
  printf("avail Moves:%d\n", moves[0]);

  char nota[3];

  int mv = nota_to_sq(nota);
  char t=0;
  avail_moves(&b,t,moves);
  if (moves[0]>0){
    for (int i=0;i<moves[0];i++){
      sq_to_nota(moves[i+1],nota);
      printf("%s:",nota);
    }
    printf("\n");
  }

  while (~(b.bb[0] | b.bb[1]) != 0L) {
    printf("Enter your move:");
    scanf("%s",nota);
    int mv = nota_to_sq(nota);
    avail_moves(&b,t,moves);
    if (in_moves(&moves[0],mv)){
        make_move(&b,mv,t);
        print_board(b);
        t ^= 1;
        /* print available moves */ 
        printf("Legal moves:");
        avail_moves(&b,t,moves);
        if (moves[0]>0){
          for (int i=0;i<moves[0];i++){
            sq_to_nota(moves[i+1],nota);
            printf("%s:",nota);
          }
          printf("\n");
        }else{
          printf("%s has no move.Passing.\n",side[t]);
          t ^= 1;
        }
    }
    else
      printf("Wrong move\n");
  }
  return 0;
}
