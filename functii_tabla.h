#ifndef _FUNCTII_TABLA_H
#define _FUNCTII_TABLA_H
#include <string.h>
#include <iostream>

using namespace std;
#include "constante.h"

typedef unsigned char uchar;

typedef struct
{
	int x,y;
}piesa;


typedef struct
{
    uchar id_piesa;
    uchar  stopx;
    uchar stopy;
    float scor;
    
}mutare; 

typedef struct
{
    uchar numar_mutari;
    mutare pieceMapCoords[MAX_NUMBER_OF_EXPANDED_MOVES];
}mutari;
//high level


typedef struct
{   
    unsigned long long table_state[50][9][9];
    unsigned long long color_state[2];
    unsigned long long xor_state; //intial position
    
} param_s;

typedef struct
{
  int type;  
  int depth;
  float scor; 
  unsigned long long xor_key;

  //the color that just moved
  int color;
  int chessTable[9][9];
  int mutat[NUMBER_OF_PIECES];


}cache_s;

struct cache_s_large : public cache_s
{
	int key;
};

#endif
