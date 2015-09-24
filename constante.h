#ifndef _CONSTANTE_H
#define	_CONSTANTE_H

/* configurable settings */

#define  DEBUG            0
#define  MAX_KEYS         512
#define  USE_CACHE       1
#define ADJUST_DEPTH     1
#define SMART_PRUNING    1


/* end configurable settings */

#define  WHITE         0
#define  BLACK         1
#define  HUMAN_COMPUTER    1
#define  COMPUTER_COMPUTER 2
#define	 INVALID_SCORE    -1000
#define	 WIN_SCORE      2000
#define	 LOSS_SCORE     -2000
#define  MAX_NUMBER_OF_EXPANDED_MOVES 60


#define ROCADA  5

#define  PA1           1
#define  PA2           2
#define  PA3           3
#define  PA4           4
#define  PA5           5
#define  PA6           6
#define  PA7           7
#define  PA8           8
#define  CA1           9
#define  CA2           10
#define  NA1           11     //NA1 nebun alb care merge pe negru
#define  NA2           12     //NA2 nebun alb care merge pe alb
#define  TA1           13
#define  TA2           14
#define  RA            15    //rege
#define  RRA           16   //regina
#define  RRA1          17
#define  RRA2          18
#define  RRA3          19
#define  RRA4          20
#define  RRA5          21
#define  RRA6          22
#define  RRA7          23
#define  RRA8          24

#define  EMPTY_SQUARE  25

#define  PN1           26
#define  PN2           27
#define  PN3           28
#define  PN4           29
#define  PN5           30
#define  PN6           31
#define  PN7           32
#define  PN8           33
#define  CN1           34
#define  CN2           35
#define  NN1           36     //NN1 nebun negru care merge pe negru
#define  NN2           37     //NN2 nebun negru care merge pe alb
#define  TN1           38
#define  TN2           39
#define  RN            40    //rege
#define  RRN           41   //regina
#define  RRN1          42
#define  RRN2          43
#define  RRN3          44
#define  RRN4          45
#define  RRN5          46
#define  RRN6          47
#define  RRN7          48
#define  RRN8          49
#define  NET_PARAM        1010
#define  SCOR_MAT         23
#define  PARAMETERS       8
#define  NET_ENTRIES      144
#define  NUMBER_OF_PIECES 50 



#define  VERT   0
#define  ORIZ   1
#define  D1 2   //similar cu diagonala principala dintr-o matrice
#define  D2 3   //similar cu diagaonala secundara dintr-o matrice
#define  DIRECTIE_CAL 4 //directia de pe care ataca calul


//for hash coding
#define  _PA  0
#define  _CA  1
#define  _NA  2
#define  _TA  3
#define  _RA  4
#define  _RRA  5


#define  _PN  6
#define  _CALN  7
#define  _NN  8
#define  _TN  9
#define  _RN  10
#define  _RRN 11

#define _EMPTY 12

#define EXACT      1
#define LOW_BOUND  2
#define HIGH_BOUND 3


#endif	/* _CONSTANTE_H */













