/* 
 * File:   inlines.h
 * Author: andrei
 *
 * Created on December 23, 2009, 12:26 AM
 */

#ifndef _INLINES_H
#define	_INLINES_H
#include "constante.h"
#include "functii_tabla.h"
#include "sah.h"

extern float tabela_valori_piese[NUMBER_OF_PIECES];

inline int piesa_alba( int id_piesa)
{
    return (id_piesa<EMPTY_SQUARE);
}


 inline int piesa_neagra( int id_piesa)
{
    return (id_piesa>EMPTY_SQUARE);
}

 
 inline int  pion(int id_piesa)
{
    return ( ((id_piesa >= PA1) && (id_piesa <= PA8)) || ( (id_piesa >= PN1) && (id_piesa <= PN8)));
        
}

inline int pion_alb(int id_piesa)
{
  return ( (id_piesa >= PA1) && (id_piesa <= PA8));
       
}

inline int pion_negru(int id_piesa)
{
  return ( (id_piesa >= PN1) && (id_piesa <= PN8));        
}

inline int  cal(int id_piesa)
{
    return ( (id_piesa == CA1) || (id_piesa == CA2) || (id_piesa == CN1) || (id_piesa == CN2));
        
}
inline int  nebun(int id_piesa)
{
    return ( (id_piesa == NA1) || (id_piesa == NA2) || (id_piesa == NN1) || (id_piesa == NN2));
        
}
inline int  tura(int id_piesa)
{
    return ( (id_piesa == TA1) || (id_piesa == TA2) || (id_piesa == TN1) || (id_piesa == TN2));        
}
inline int  regina(int id_piesa)
{
    return ( (id_piesa > RN) || ( (id_piesa > RA) && (id_piesa <= RRA8)) );        
}
inline int  rege(int id_piesa)
{
    return ( (id_piesa == RA) || (id_piesa == RN) ); 
        
}

inline float valoare( int id_piesa)
{	
    return tabela_valori_piese[id_piesa];       
}

inline int  tip_piesa(int id_piesa)
{
    
    if(piesa_alba(id_piesa))
    {
         if(pion(id_piesa))
            return _PA;
         
         if(cal(id_piesa))
            return _CA;
         
         if(nebun(id_piesa))
            return _NA;
         
         if(tura(id_piesa))
            return _TA;
         
         if(rege(id_piesa))
            return _RA;  
         
         if(regina(id_piesa))
            return _RRA;
    }

    else
        {
        if(pion(id_piesa))
            return _PN;
        if(cal(id_piesa))
            return _CALN;
        if(nebun(id_piesa))
            return _NN;
        if(tura(id_piesa))
            return _TN;
        if(rege(id_piesa))
            return _RN;
        if(regina(id_piesa))
            return _RRN;
        }

    return 0;
          
}

#endif	/* _INLINES_H */

