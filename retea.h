/* 
 * File:   retea.h
 * Author: andrei
 *
 * Created on 11 iunie 2011, 21:42
 */

#ifndef _RETEA_H
#define	_RETEA_H

#include "constante.h"


void net_init();
void read_net_parameters();
/*nh este un vector cu 69 de pozitii*/
float net_out(int  tabela[9][9], int color);
void get_net_tunned_k(int color);

typedef struct 

{
    float net_p[NET_PARAM];

    float w12[NET_ENTRIES][5];
    float w23[5][20];
    float w34[20][1];
    float bias1[NET_ENTRIES]; 
    float bias2[5];
    float bias3[20];
    float bias4[1];

    int net_rank;
} neural_n;


#endif	/* _RETEA_H */

