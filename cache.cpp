#include "cache.h"
#include "inlines.h"
#include "constante.h"

//64 * 13
//numaratoarea incepe de (1,1) la (8,8)

extern int chessTable[9][9];
extern piesa pieceMapCoords[50];
extern int mutat[NUMBER_OF_PIECES];
extern int QUEENS_W,QUEENS_B;

cache_s  scoresCache[MAX_KEYS];
cache_s  v_cache_aux[MAX_KEYS];











