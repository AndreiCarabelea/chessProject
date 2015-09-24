#include "retea.h"
#include "constante.h"
#include "inlines.h"
#include "stdlib.h"
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "math.h"
#include "retea.h"
#include "functii_tabla.h"

#include <stdlib.h>
#include <time.h>
/*this vector contains the same data*/
 
neural_n network_w;
neural_n network_b;

void net_hash(int  tabela[9][9], int *nh)
{
	/*
    int idx1, idx2;
    memset(nh,0,144*sizeof(int));
    for(int i=PA1;i<=RRN8;i++)
        for(int j=i+1;j<=RRN8;j++)
        {
            if(apara_piesa(i,j))
            {
               idx1=tip_piesa(i);
               idx2=tip_piesa(j);
               
			   //@todo intial was 0.1
               nh[idx1*12+idx2]+=1; 
            }
        }
	*/
}
    
float transfer_function(float x)
{
  return 1/(1+exp(-x));   
}

/*random number between -0.5 and 0.5*/
float rand_net()
{
    float res=(float)rand()/(float)RAND_MAX;
    return res-0.5f;    
}

float rand_bin()
{
    double res=(double)rand()/(double)RAND_MAX;
    res*=2;
    return (float)((int)res/2);    
}

/*compute exit of the network */
void net_init(neural_n& nw)
{     
  int i,j;
 
  for(i=0;i<NET_ENTRIES;i++)
  {
      nw.bias1[i]=rand_net();
      for(j=0;j<5;j++)
          nw.w12[i][j]=rand_net();
  }
 
  for(i=0;i<5;i++)
  {
      nw.bias2[i]=rand_net();
      for(j=0;j<20;j++)
          nw.w23[i][j]=rand_net();
  }
  
  for(i=0;i<20;i++)
  {
        nw.bias3[i]=rand_net();
        nw.w34[i][0]=rand_net();
  }
  
  nw.bias4[0]=rand_net();

}

float compute(int *nh, neural_n& nw)
{
    float y12[NET_ENTRIES];
    float y23[5];
    float y34[20];
    float y;
    
    int i,j;
    for(i=0; i<NET_ENTRIES; i++)
        y12[i]=transfer_function(nw.bias1[i]+nh[i]);
    
    for(i=0; i<5; i++)
    {
        float intrare_i=0;
        for(j=0;j<NET_ENTRIES;j++)
            intrare_i+=nw.w12[j][i]*y12[j];
        intrare_i+=nw.bias2[i];
        y23[i]=transfer_function(intrare_i);
        
    }
    
    for(i=0; i<20; i++)
    {
        float intrare_i=0;
        for(j=0;j<5;j++)
            intrare_i+=nw.w23[j][i]*y23[j];
        intrare_i+=nw.bias3[i];
        
        y34[i]=transfer_function(intrare_i);
    }
    
    float intrare_y=0;
    for(j=0;j<20;j++)
        intrare_y+=nw.w34[j][0]*y34[j];
    intrare_y+=nw.bias4[0];
    
    y=transfer_function(intrare_y);
    
    return y;
    
}


float net_out(int  tabela[9][9], neural_n& nw)
{
    int nh[NET_ENTRIES];
    
    net_hash(tabela, nh);
    return compute(nh,nw);
    
}
/* calculeaza suma erorilor de pe stratul de iesire al retelei, 
 * intoarce numarul de pozitii gasite daca numarul este insuficient, altfel numar de 
 * exemple folosite pentru antrenare/
 */
void to_tables(float *np, neural_n& nw)
{
    
    int i,j,inp=0;
           
    for(i=0;i<NET_ENTRIES;i++)
    {
        nw.bias1[i]=np[inp++];
        for(j=0;j<5;j++)
            nw.w12[i][j]=np[inp++];
    }

    for(i=0;i<5;i++)
    {  
        nw.bias2[i]=np[inp++];
        for(j=0;j<20;j++)
            nw.w23[i][j]=np[inp++];
    }
    
    for(i=0;i<20;i++)
    {
        nw.bias3[i]=np[inp++];
        for(j=0;j<1;j++)
            nw.w34[i][j]=np[inp++];
    }
    
    nw.bias4[0]=np[inp];
    
}

/*intrari training + intrari test*/

/*
int fitness_np(float *np,float &sq1, float &sq2, int min_depth, int color)
{    
    neural_n local_nw;
    
    
    to_tables(np, local_nw);
          
    float eroare_locala;
    
    sq1=sq2=0;
    
    
    int count=0;
    
             
    for(int i=0;i<MAX_KEYS;i++)
    {       
        if( (scoresCache[i].type == EXACT) && (scoresCache[i].depth  >= min_depth) && (scoresCache[i].depth != 100) )
			if (color == scoresCache[i].color)
                
        {   
            
            if(count < NET_PATTERNS) 
            {
                eroare_locala=fabs(scoresCache[i].scor - net_out(scoresCache[i].chessTable, local_nw));

                sq1+=eroare_locala;

                count ++;           
            }
            
            else if(count < 2*NET_PATTERNS)
            {
                eroare_locala=fabs(scoresCache[i].scor - net_out(scoresCache[i].chessTable, local_nw));

                sq2+=eroare_locala;

                count ++;
                
            }
                
        }
    }
 
    if(count < (2*NET_PATTERNS))    
        return -count; 
   
    else
        return count;
    
}
*/

void read_net_parameters()
{
    FILE *f;
    char line[80];
    
	if (fopen_s(&f, "D:\\net_param_w.txt", "r") || (f == NULL))
        return;
    
    int contor_param=0;
           
    while(contor_param < NET_PARAM)
    {
       network_w.net_p[contor_param++]=(float)atof(fgets(line,80,f));       
    }
    
    network_w.net_rank=atoi(fgets(line,80,f));
    
    to_tables(network_w.net_p, network_w);
    
	if (fopen_s(&f, "D:\\net_param_b.txt", "r") || (f == NULL))
        return;
    
    contor_param=0;
           
    while(contor_param < NET_PARAM)
    {
       network_b.net_p[contor_param++]=(float)atof(fgets(line,80,f));         
    }
    
    network_b.net_rank=atoi(fgets(line,80,f));
    
    to_tables(network_b.net_p,network_b);
    
   
    fclose(f);       
}

/* count decide daca am solutie finala(count > 0), sau daca am insuficiente pozitii */
void display_net_parameters(float np[], int rank, int color)
{
    FILE *f;
    if(color == WHITE)
        fopen_s(&f, "D:\\net_param_w.txt","w+");
    else
        fopen_s(&f, "D:\\net_param_b.txt","w+");
    
	if (f == NULL)
	{
		return;
	}
    
    for(int i=0;i< NET_PARAM;i++)
    {
        //fprintf(f,"%f\n",np[i]);
    }
    
    //fprintf(f,"%d\n",rank);
    fclose(f);       
} 
float distance_flies(float *fl1, float *fl2)
{
    float sq_error=0;
    
    for(int i=0; i<NET_PARAM ; i++)
        sq_error+=pow((fl1[i]-fl2[i]),2);
    
    sq_error/=NET_PARAM;
    
    return sqrt(sq_error);    
}

#if 0
float e0(float *fl,int desired_rank, int color)
{
    float sq1, sq2;
    int count;
    
    count=fitness_np(fl, sq1, sq2, desired_rank, color);
    
    float eroare_medie=(sq1+sq2)/count;
    return pow((1/(1-eroare_medie)),1);
            
}
#endif

float prob(float e1, float e2, float T)
{
  if(e2 < e1)
      return 1;
  
  float k=1/500;
  
  return exp((e1-e2)/(k*T));
    
}

#if 0
void get_net_tunned_k(int color)
{
    /*initializez o populatie cu 50 de membri */
    float flies[NET_POPULATION][NET_PARAM];
    neural_n local_nw;
    local_nw=(color == WHITE) ? network_w : network_b;
     
    /*copiez in membrul zero parametrii retelei*/
    memcpy(flies[0],local_nw.net_p,NET_PARAM*sizeof(float));
    
    /*determin rangul retelei dorite: minim net_rank */
    
    /*determin daca e posibil*/
    int count;
    int desired_rank=-1;
    
    float SQ = 0, sq1,sq2;
       
    
    count=fitness_np(flies[0], sq1, sq2, local_nw.net_rank+1, color);
        
    if(count > 0)
        desired_rank=local_nw.net_rank+1;
                                   
    if(desired_rank == -1)
    {
#if DEBUG == 1    
    cout <<"No better positions found: "<<-count<<" positions. "<<endl;
#endif
        return;
    }
     
    SQ=sq1+sq2;
    
   
    /*initializez si ceilalti membrii ai populatiei */
        
    float T=500;      
    float last_e=e0(flies[0], desired_rank,color);
    
      
    float last_fly[NET_PARAM];
    memcpy(last_fly,flies[0],NET_PARAM*sizeof(float));
    
    float current_fly[NET_PARAM];
    
#if DEBUG == 1    
    cout <<"e initial: "<<last_e<<endl;
#endif
    
    while(1)
    {
               
        for(int k=0;k<NET_PARAM;k++)
             current_fly[k]=last_fly[k]+rand_net()*0.02f;
              
        float current_e=e0(current_fly, desired_rank,color);
           
        if ( prob(last_e, current_e, T) > (rand_net()+0.5)) 
                 
        {
            
            T--;
        
            if(T == 0)
            {
              display_net_parameters(current_fly,desired_rank,color);
              break;
            }
            
            last_e=current_e;
#if DEBUG == 1 
        cout<<"E "<<last_e<<", T: "<<T<<endl;
 #endif
            memcpy(last_fly,current_fly,NET_PARAM*sizeof(float));                
        }
        
                  
    }
                        
}
#endif
