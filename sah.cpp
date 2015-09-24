
#include "constante.h"
#include "functii_tabla.h"
#include "functii_invatare.h"
#include "cache.h"
#include "inlines.h"
#include "retea.h"
#include "sah.h"

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <cstdarg>
#include <chrono>
#include <algorithm>
#include <windows.h>
#include <thread>
#include <ctype.h>
#include <algorithm>
#include <vector>

using namespace std;
using namespace std::chrono;

extern float param[PARAMETERS];
extern int MAX_DEPTH, SEARCH_LIMIT;
extern float tabela_valori_piese[NUMBER_OF_PIECES];

char words[10][80];
char input[80];

float INITIAL_TIME = 300;
float available_time = INITIAL_TIME;


cache_s_large temp_p[1000];

void memoreaza_pozitie(float scor, int step, int actual_depth, int key)
{        
	temp_p[step].depth=actual_depth;
	temp_p[step].type=EXACT;
	temp_p[step].scor=scor;
	temp_p[step].key=key;
}

int CountWords(char *inp) 
{
	int n=0,nwrd=0,l=0;

	for (n=0;n<(int)strlen(inp);n++) {
		if (inp[n]!=' ' && inp[n]!='\n' && l==0) {nwrd++;l++;}
		if (inp[n]==' ') l=0;
	}
	return nwrd;
}


void GetWords(char *inp) 
{
	int n=-1,l=0,wno=0;
	int nwrd=CountWords(inp);

	for (wno=0;wno<nwrd;wno++) {
		memset(words[wno],0,sizeof(words[wno]));
		while (inp[++n]==' ');
		while (inp[n]!=' ' && inp[n]!='\n') {
			words[wno][l]=inp[n];
			l++;
			n++;
		}
		l=0;
	}
}


float sum(vector<float> v, int toIndex)
{
	float res = 0;
	for (int i = 0; i <= toIndex; i++)
	{
		res += v[i];
	}

	return res;
}


Compute::Compute()
{
	initializeTable();
}

//juctorul de culoare color tocmai a facut mutarea
float Compute::nonRecursiveScore(int key, bool complexEvaluation, float mutari_alb, float mutari_negru, int color)
{
	if (!complexEvaluation && isInCheck(color))
	{
		return INVALID_SCORE;
	}

#if USE_CACHE == 1
	if (scoresCache[key].type == EXACT)
	{
		if (found_configuration(key, color)) {
			return scoresCache[key].scor;
		}
	}
#endif

	return scor_static(complexEvaluation, mutari_alb, mutari_negru, color);
}

//key este asociata pozitiei care a rezultat in urma mutarii negrului.
float Compute::evalueaza_max(int depth, float minim, float maxim, mutare &best_move, set<int> ancestors, int oponentMoves)
{
	float maxim_local = LOSS_SCORE - 1;
	int indexBestMove;

	mutari* m = new mutari;
	genereaza_mutari(m, WHITE);
	int mutari_evaluate = m->numar_mutari;
	if (!mutari_evaluate)
	{
		return LOSS_SCORE;
	}


	float scor_m = 1;
	int countGoodMoves = 0;
	indexBestMove = mutari_evaluate - 1;
	for ( int i = mutari_evaluate - 1; i >= 0; i--)
	{
		int id_piesa = m->pieceMapCoords[i].id_piesa;
		int startx = pieceMapCoords[id_piesa].x;
		int starty = pieceMapCoords[id_piesa].y;
		int stopx = m->pieceMapCoords[i].stopx;
		int stopy = m->pieceMapCoords[i].stopy;

#if SMART_PRUNING == 1
		if (countGoodMoves > param[6])
		{
			break;
		}
#endif

		float max2 = max(maxim_local, maxim);
		scor_m = scor_mutare(id_piesa, startx, starty, stopx, stopy, WHITE, depth + 1, minim, max2, ancestors, mutari_evaluate, oponentMoves);
		

		if (scor_m >= maxim_local)
		{
			maxim_local = scor_m;
			indexBestMove = i;
			countGoodMoves++;
		}

		if (maxim_local > minim)
		{
			//we don't need the best move because this is not an exact move
			best_move.scor = maxim_local;
			return maxim_local;
		}
	}

	best_move.id_piesa = m->pieceMapCoords[indexBestMove].id_piesa;
	best_move.scor = maxim_local;
	best_move.stopx = m->pieceMapCoords[indexBestMove].stopx;
	best_move.stopy = m->pieceMapCoords[indexBestMove].stopy;

	delete m;

	return maxim_local;
}

//key este asociata pozitiei care a rezultat in urma mutarii albului.
float Compute::evalueaza_min(int depth, float minim, float maxim, mutare &best_move, set<int> ancestors, int oponentMoves)
{
	float minim_local = WIN_SCORE + 1;
	int indexBestMove;

	mutari* m = new mutari;

	//mutarile posibile ale adversarului
	genereaza_mutari(m, BLACK);
	int mutari_evaluate = m->numar_mutari;
	if (!mutari_evaluate)
	{
		return WIN_SCORE;
	}

	float scor_m = 0;
	int countGoodMoves = 0;
	indexBestMove = 0;
	for ( int i = 0; i < mutari_evaluate; i++) 
	{
		int id_piesa = m->pieceMapCoords[i].id_piesa;
		int startx = pieceMapCoords[id_piesa].x;
		int starty = pieceMapCoords[id_piesa].y;
		int stopx = m->pieceMapCoords[i].stopx;
		int stopy = m->pieceMapCoords[i].stopy;

#if SMART_PRUNING == 1
		if (countGoodMoves > param[6])
		{
			break;
		}
#endif // SMART_PRUNING

		float min2 = min(minim, minim_local);
		scor_m = scor_mutare(id_piesa, startx, starty, stopx, stopy, BLACK, depth + 1, min2, maxim, ancestors, oponentMoves, mutari_evaluate);
		

		if (scor_m <= minim_local)
		{
			minim_local = scor_m;
			indexBestMove = i;
			countGoodMoves++;
		}

		if (minim_local < maxim)
		{
			//we don't need the best move because this is not an exact move
			best_move.scor = minim_local;
			return minim_local;
		}
	}

	best_move.id_piesa = m->pieceMapCoords[indexBestMove].id_piesa;
	best_move.scor = minim_local;
	best_move.stopx = m->pieceMapCoords[indexBestMove].stopx;
	best_move.stopy = m->pieceMapCoords[indexBestMove].stopy;

	delete m;

	return minim_local;

}

//jucatorul de culoare color tocmai a facut mutarea.
float Compute::calculeaza_scor(int color, int key, int depth, float minim, float maxim, set<int> ancestors, int oponentMoves)
{
	float scor = -1;
	int actual_depth = (depth < MAX_DEPTH) ? (MAX_DEPTH - depth) : 0;

#if USE_CACHE == 1
	if (found_configuration(key, color) && (scoresCache[key].depth >= actual_depth)) //configuratia a fost gasita
	{
		switch (scoresCache[key].type)
		{
		case EXACT:
			scor = scoresCache[key].scor;
			break;
		case LOW_BOUND:
			if (scoresCache[key].scor > minim)
			{
				scor = scoresCache[key].scor;
			}
			break;
		case HIGH_BOUND:
			if (scoresCache[key].scor < maxim)
			{
				scor = scoresCache[key].scor;
			}
			break;
		default:
			break;
		}
	}

	//score not found 
	if (scor == -1)
	{
	
#endif
		mutare best_move;
		if (color == WHITE)
		{
			scor = evalueaza_min(depth, minim, maxim, best_move, ancestors, oponentMoves);
		}
		else
		{
			scor = evalueaza_max(depth, minim, maxim, best_move, ancestors, oponentMoves);
		}
#if USE_CACHE == 1
		int type;
		if (scor > minim)
			type = LOW_BOUND;
		else if (scor < maxim)
			type = HIGH_BOUND;
		else
			type = EXACT;


		if ((actual_depth > scoresCache[key].depth) || ((actual_depth >= scoresCache[key].depth) && (type == EXACT)))
		{ 
			memoreaza_scor(key, actual_depth, type, scor, color);
		}
	}
#endif
	return scor;
}

//jucatorul de culoare color face mutarea indicata de parametrii 
float Compute::scor_mutare( int id_piesa,  int startx,  int starty,  int stopx,  int stopy, int color,
	int depth, float minim, float maxim, set<int> ancestors, int mutari_alb, int mutari_negru, bool complexEvaluation)

{	//au passant,promovare,avans o patratica
	float scor = -1;
	int mutat_s[NUMBER_OF_PIECES];
	int  key;
	int oponentMoves;

	//check if it is a king capture
	if (color == WHITE)
	{
		oponentMoves = mutari_negru;
	}
	else
	{
		oponentMoves = mutari_alb;
	}

	if (pion(id_piesa))
	{
		//promotion
		if ((stopy == 1) || (stopy == 8)) 
		{
			//salvez piesa de la destinatie;
			int piesa_destinatie = chessTable[stopx][stopy];
			memcpy(mutat_s, mutat, Compute::mutat_size);

			
			key = update_key(id_piesa, startx, starty, stopx, stopy, color);
			update_key(id_piesa, startx, starty, stopx, stopy, color);
			
			
			aplica_mutare(id_piesa, stopx, stopy);
			
			if ((ancestors.find(key) != ancestors.end()) || (depth == SEARCH_LIMIT))
			{
				scor = nonRecursiveScore(key, complexEvaluation, (float)mutari_alb, (float)mutari_negru, color);                           
#if USE_CACHE == 1				
				if ((scoresCache[key].depth == 0) && (complexEvaluation == true))
				{
					memoreaza_scor(key, 0, EXACT, scor, color);
				}
#endif

			}


			else
			{
				scor = calculeaza_scor(color, key, depth, minim, maxim, ancestors, oponentMoves);
				ancestors.insert(key);
			}

			//sfarsit calcul scor 

			
			memcpy(mutat, mutat_s, Compute::mutat_size);

			//retrag mutare
			//distrug referinta in v a damei
			pieceMapCoords[chessTable[stopx][stopy]].x = false;
			//la destinatie retaurez piesa care era inainte
			chessTable[stopx][stopy] = piesa_destinatie;
			if (piesa_destinatie != EMPTY_SQUARE)
			{
				pieceMapCoords[piesa_destinatie].x = stopx;
				pieceMapCoords[piesa_destinatie].y = stopy;
			}
			//la sursa pun la loc pionul

			chessTable[startx][starty] = id_piesa;
			pieceMapCoords[id_piesa].x = startx;
			pieceMapCoords[id_piesa].y = starty;

			if (piesa_alba(id_piesa))
				QUEENS_W--;
			else
				QUEENS_B--;
			

			return scor;
		}

	}

	//rocade
	if (rege(id_piesa))
	{
		if (abs(startx - stopx) == 2)
		{
			//salvez coordonate initiale rege
			int y_rege = starty;

			//salvez coordonate initiale tura
			int x_tura;
			if (stopx > startx)
				x_tura = 8;
			else
				x_tura = 1;
			//am salvat coordonate initiale rege si tura,in caz de rocada sunt pe acceasi linie
			int id_tura = chessTable[x_tura][y_rege];
			//deci coordonatele pieselor sunt x_rege,y_rege,x_tura,y_tura
			//fac rocada
			memcpy(mutat_s, mutat, Compute::mutat_size);

			
			key = update_key(id_piesa, startx, starty, stopx, stopy, color);
			update_key(id_piesa, startx, starty, stopx, stopy, color);
			
			
			aplica_mutare(id_piesa, stopx, stopy);
			
			if ((ancestors.find(key) != ancestors.end()) || (depth == SEARCH_LIMIT))
			{
				scor = nonRecursiveScore(key, complexEvaluation, (float)mutari_alb, (float)mutari_negru, color);                
				if ((scoresCache[key].depth == 0) && (complexEvaluation == true))
					memoreaza_scor(key, 0, EXACT, scor, color); 
			}

			else
			{
				scor = calculeaza_scor(color, key, depth, minim, maxim, ancestors, oponentMoves);
				ancestors.insert(key);
			}

			//sfarsit calcul scor
			
			memcpy(mutat, mutat_s, Compute::mutat_size);
			if (stopx > startx)
			{
				retrag_mutare(id_piesa, EMPTY_SQUARE, 5, y_rege, 7, y_rege);
				retrag_mutare(id_tura, EMPTY_SQUARE, x_tura, y_rege, 6, y_rege);
			}
			else
			{
				retrag_mutare(id_piesa, EMPTY_SQUARE, 5, y_rege, 3, y_rege);
				retrag_mutare(id_tura, EMPTY_SQUARE, x_tura, y_rege, 4, y_rege);
			}
			

			return scor;
		}
	}

	//mai intai am tratat mutarile execptie care nu se supun regulilor generale

	int id_piesa_capturata = chessTable[stopx][stopy];

	memcpy(mutat_s, mutat, Compute::mutat_size);

	int captura = 0;
	if (id_piesa_capturata != EMPTY_SQUARE)
		captura = 1;

	
	key = update_key(id_piesa, startx, starty, stopx, stopy, color);
	update_key(id_piesa, startx, starty, stopx, stopy, color);
	
	
	aplica_mutare(id_piesa, stopx, stopy);

	if (((depth < MAX_DEPTH) || (captura && (depth < SEARCH_LIMIT))) && ancestors.find(key) == ancestors.end())
	{
		scor = calculeaza_scor(color, key, depth, minim, maxim, ancestors, oponentMoves);
		ancestors.insert(key);
	}
	else
	{
		scor = nonRecursiveScore(key, complexEvaluation,  (float)mutari_alb, (float)mutari_negru, color);
#if USE_CACHE == 1
		if ((scoresCache[key].depth == 0) && (complexEvaluation == true))
			memoreaza_scor(key, 0, EXACT, scor, color);
#endif
	}

	//sfarsit calcul scor
	memcpy(mutat, mutat_s, Compute::mutat_size);
	retrag_mutare(id_piesa, id_piesa_capturata, startx, starty, stopx, stopy);
	
	return scor;
}

//startx,starty de unde pleaca
bool Compute::diff_color(int color,  int x,  int y,  int stopx,  int stopy)
{
	if ((x != stopx) || (y != stopy))
		return false;


	int id_stop = chessTable[stopx][stopy];

	if (chessTable[stopx][stopy] == EMPTY_SQUARE)
		return true;


	//a iesit la destinatie
	if (color == WHITE)
	{
		if (piesa_neagra(id_stop))
			return true;
	}

	else
	{
		if (piesa_alba(id_stop))
			return true;
	}

	return false;

}

bool Compute::mutare_valida_nebun( int startx,  int starty,  int stopx,  int stopy, int color)
{
	int v1 = abs(startx - stopx);
	int v2 = abs(starty - stopy);

	if (v1 != v2)
		return false;

	//diagonala 1
	if (startx < stopx)
	{
		if (starty < stopy)
		{
			int x1 = startx + 1;
			int y1 = starty + 1;

			while (muta(x1, y1) && (x1 < stopx) && (y1 < stopy))
			{
				x1++;
				y1++;
			}

			return diff_color(color, x1, y1, stopx, stopy);
		}

		if (starty > stopy)
		{

			int x1 = startx + 1;
			int y1 = starty - 1;

			while (muta(x1, y1) && (x1 <stopx) && (y1>stopy))
			{
				x1++;
				y1--;
			}
			return diff_color(color, x1, y1, stopx, stopy);

		}
	}

	if (startx > stopx)
	{
		if (starty > stopy)
		{
			int x1 = startx - 1;
			int y1 = starty - 1;

			while (muta(x1, y1) && (x1 > stopx) && (y1 > stopy))
			{
				x1--;
				y1--;
			}
			return diff_color(color, x1, y1, stopx, stopy);

		}

		if (starty < stopy)
		{
			int x1 = startx - 1;
			int y1 = starty + 1;

			while (muta(x1, y1) && (x1 > stopx) && (y1 < stopy))
			{
				x1--;
				y1++;
			}
			return diff_color(color, x1, y1, stopx, stopy);

		}
	}

	return false;

}

//piesa este implicit valida,la fel ca si pozitia de start si stop
bool Compute::mutare_valida_tura( int startx,  int starty,  int stopx,  int stopy, int color)
{
	int v1 = (startx - stopx);
	int v2 = (starty - stopy);

	if (v1&&v2)
		return false;

	if (!v2)
	{
		if (startx < stopx)
		{
			int x1 = startx + 1;
			int y1 = starty;
			while (muta(x1, y1) && (x1 < stopx))
			{
				x1++;
			}
			return diff_color(color, x1, y1, stopx, stopy);
		}

		if (startx > stopx)
		{
			int x1 = startx - 1;
			int y1 = starty;
			while (muta(x1, y1) && (x1 > stopx))
			{
				x1--;
			}
			return diff_color(color, x1, y1, stopx, stopy);
		}
	}

	if (!v1)
	{
		if (starty < stopy)
		{
			int x1 = startx;
			int y1 = starty + 1;

			while (muta(x1, y1) && (y1 < stopy))
			{
				y1++;
			}
			return diff_color(color, x1, y1, stopx, stopy);
		}

		if (starty > stopy)
		{
			int x1 = startx;
			int y1 = starty - 1;

			while (muta(x1, y1) && (y1 > stopy))
			{
				y1--;
			}
			return diff_color(color, x1, y1, stopx, stopy);
		}
	}

	return false;

}

void Compute::engine_move(int startx, int starty, int stopx, int stopy)
{
	char m[5];
	m[0] = startx + 96;
	m[1] = starty + 48;
	m[2] = stopx + 96;
	m[3] = stopy + 48;
	m[4] = '\0';

	fprintf(stdout, "move %4s\n", m);
	fflush(stdout);

}

void Compute::mutare_normala( int id_piesa,  int startx,  int starty,  int stopx,  int stopy)
{
	int piesa_capturata = chessTable[stopx][stopy];
	chessTable[stopx][stopy] = id_piesa;
	chessTable[startx][starty] = EMPTY_SQUARE;
	pieceMapCoords[id_piesa].x = stopx;
	pieceMapCoords[id_piesa].y = stopy;
	if (piesa_capturata != EMPTY_SQUARE)
	pieceMapCoords[piesa_capturata].x = false;

}

bool Compute::mutare_valida( int id_piesa,  int startx,  int starty,  int stopx,  int  stopy, int color)
{

	if ((stopx < 1) || (stopx > 8) || (stopy < 1) || (stopy > 8))
		return false;

	if ((startx == stopx) && (starty == stopy))
		return false;


	if (pion(id_piesa))
	{
		//avans
		int diff = abs(startx - stopx);

		if (!diff)
		{
			if (color == WHITE)
			{
				//validare avans doua patretele
				if ((starty == 2) && ((stopy - starty) == 2) && (chessTable[startx][starty + 1] == EMPTY_SQUARE) && \
					(chessTable[startx][starty + 2] == EMPTY_SQUARE))
					return true;

				//validare avans o patratica
				if (((stopy - starty) == 1) && (chessTable[startx][starty + 1] == EMPTY_SQUARE))
					return true;
			}

			else
			{
				if ((starty == 7) && ((stopy - starty) == -2) && (chessTable[startx][starty - 1] == EMPTY_SQUARE) && \
					(chessTable[startx][starty - 2] == EMPTY_SQUARE))
					return true;

				if (((stopy - starty) == -1) && (chessTable[startx][stopy] == EMPTY_SQUARE))
					return true;
			}

		}
		//captura
		else if (diff == 1)
		{
			if (color == WHITE)
			{
				if ((stopy - starty) == 1)
				{
					if (piesa_neagra(chessTable[stopx][stopy]))
						return true;
				}
			}
			else
			{
				if ((stopy - starty) == -1)
				{
					if (piesa_alba(chessTable[stopx][stopy]))
						return true;

				}
			}
		}//sfarsit if diff == 1

	} //sfarsit pion

	else if (!rege(id_piesa))
	{
		if (color == WHITE)
		{
			if (m_alb(stopx, stopy))
				return true;
		}
		else if (m_negru(stopx, stopy))
			return true;
	}


	//validez mutare regi
	else
	{
		//mutare cu o patratica
		int diff_x = abs(startx - stopx);
		int diff_y = abs(starty - stopy);

		//mutare cu o patratica
		if ((diff_x <= 1) && (diff_y <= 1))
		{
			if (color == WHITE)
			{
				if (m_alb(stopx, stopy))
					return true;
			}
			else
			{
				if (m_negru(stopx, stopy))
					return true;
			}
		}

		if (diff_x == 2)
		{
			if (color == WHITE)
			{
				if ((startx == 5) && (starty == 1) && (stopy == 1))  {
					if (stopx > startx)
					{
						if ((chessTable[startx + 1][starty] == EMPTY_SQUARE) && (chessTable[startx + 2][starty] == EMPTY_SQUARE) && 
							!mutat[TA2] && !mutat[RA] && (pieceMapCoords[TA2].x == 8) && (pieceMapCoords[TA2].y == 1))
						{
							if (!isInCheck(WHITE) && !isInCheck(WHITE, pieceMapCoords[RA].x + 1) &&
								!isInCheck(WHITE, pieceMapCoords[RA].x + 2))
							{
								return true;
							}
						}
					}
					else
					{
						if ((chessTable[startx - 1][starty] == EMPTY_SQUARE) && (chessTable[startx - 2][starty] == EMPTY_SQUARE) &&
							 (chessTable[startx - 3][starty] == EMPTY_SQUARE) &&
							!mutat[TA1] && !mutat[RA] && (pieceMapCoords[TA1].x == 1) && (pieceMapCoords[TA1].y == 1))
						{
							if (!isInCheck(WHITE) && !isInCheck(WHITE, pieceMapCoords[RA].x - 1)
								&& !isInCheck(WHITE, pieceMapCoords[RA].x - 2))
							{
								return true;
							}

						}
					}
				}
			}
			else
			{
				if ((startx == 5) && (starty == 8) && (stopy == 8)) {
					if (stopx > startx)
					{
						if ((chessTable[startx + 1][starty] == EMPTY_SQUARE) && (chessTable[startx + 2][starty] == EMPTY_SQUARE) &&
							!mutat[TN1] && !mutat[RN] && (pieceMapCoords[TN1].x == 8) && (pieceMapCoords[TA2].y == 8))
						{
							if (!isInCheck(BLACK) && !isInCheck(BLACK, pieceMapCoords[RN].x + 1) &&
								!isInCheck(BLACK, pieceMapCoords[RN].x + 2))
							{
								return true;
							}
						}
					}
					else
					{
						if ((chessTable[startx - 1][starty] == EMPTY_SQUARE) && (chessTable[startx - 2][starty] == EMPTY_SQUARE) &&
							(chessTable[startx - 3][starty] == EMPTY_SQUARE) &&
							!mutat[TN2] && !mutat[RN] && (pieceMapCoords[TN2].x == 1) && (pieceMapCoords[TN2].y == 8))
						{
							if (!isInCheck(BLACK) && !isInCheck(BLACK, pieceMapCoords[RN].x - 1)
								&& !isInCheck(BLACK, pieceMapCoords[RN].x - 2))
							{
								return true;
							}

						}
					}
				}
			}
		}
	}


	return false;

}

bool Compute::mutare_corecta( int id_piesa,  int startx,  int starty,  int stopx,  int  stopy, int color)
{

	if (pion(id_piesa) || rege(id_piesa))
		return mutare_valida(id_piesa, startx, starty, stopx, stopy, color);

	if (!mutare_valida(id_piesa, startx, starty, stopx, stopy, color))
		return false;

	if (cal(id_piesa))
	{
		int v1 = abs(startx - stopx);
		if (v1 < 3)
		{
			int v2 = abs(starty - stopy);
			if (v1 && v2 && ((v1 + v2) == 3))
			{
				return true;
			}
		}
	}

	else if (nebun(id_piesa)) //nebunii de negru
	{
		return mutare_valida_nebun(startx, starty, stopx, stopy, color);
	}

	else if (tura(id_piesa))
	{
		return mutare_valida_tura(startx, starty, stopx, stopy, color);
	}

	else if (regina(id_piesa))
	{
		return (mutare_valida_nebun(startx, starty, stopx, stopy, color) || mutare_valida_tura(startx, starty, stopx, stopy, color));
	}

	return false;

}

void Compute::aplica_mutare(int id_piesa, int stopx, int stopy)
{
	int startx = pieceMapCoords[id_piesa].x;
	int starty = pieceMapCoords[id_piesa].y;

	mutat[id_piesa] = true;

#ifdef DEBUG
	//problema_mutare(id_piesa,startx,starty,stopx,stopy,true);
#endif

	if (pion(id_piesa))
	{
		//promovare
		if ((stopy == 1) || (stopy == 8))
		{
			mutare_normala(id_piesa, startx, starty, stopx, stopy);

			int id_dama;
			if (piesa_alba(id_piesa))
			{
				id_dama = RRA1 + QUEENS_W;
				QUEENS_W++;
			}
			else
			{
				id_dama = RRN1 + QUEENS_B;
				QUEENS_B++;
			}
			//inlocuiesc pion cu dama
			pieceMapCoords[id_piesa].x = false;
			chessTable[stopx][stopy] = id_dama;
			pieceMapCoords[id_dama].x = stopx;
			pieceMapCoords[id_dama].y = stopy;


			return;
		}

		if (startx == stopx)
		{
			mutare_normala(id_piesa, startx, starty, stopx, stopy);
			return;
		}

		//au passant
		if ((stopx != startx) && (chessTable[stopx][stopy] == EMPTY_SQUARE))
		{

			int ppy = 0, ppx = stopx;//ppy initializat la 0
			if (piesa_alba(id_piesa))
				ppy = stopy - 1;
			else
				ppy = stopy + 1;

			mutare_normala(id_piesa, startx, starty, stopx, stopy);
			int aux = chessTable[ppx][ppy];
			chessTable[ppx][ppy] = EMPTY_SQUARE;
			pieceMapCoords[aux].x = false;
			return;
		}

	}

	if (id_piesa == RA)
	{
		//rocade
		if (abs(stopx - startx) == 2)
		{
			mutat[RA] = ROCADA;
			mutare_normala(id_piesa, startx, starty, stopx, stopy);

			if (stopx > startx)  //rocada la dreapta
			{
				mutare_normala(TA2, 8, 1, 6, 1);
				mutat[TA2] = true;
				return;
			}
			else    //rocada la stanga
			{
				mutare_normala(TA1, 1, 1, 4, 1);
				mutat[TA1] = true;
				return;
			}

		}
	}

	if (id_piesa == RN)
	{
		//rocade
		if (abs(stopx - startx) == 2)
		{
			mutat[RN] = ROCADA;
			mutare_normala(id_piesa, startx, starty, stopx, stopy);

			if (stopx > startx)  //rocada la dreapta
			{
				mutare_normala(TN1, 8, 8, 6, 8);
				mutat[TN1] = true;
				return;
			}
			else    //rocada la stanga
			{
				mutare_normala(TN2, 1, 8, 4, 8);
				mutat[TN2] = true;
				return;
			}
		}
	}

	mutare_normala(id_piesa, startx, starty, stopx, stopy);

}


//check if the king on new position is on check
//default new position is current position
bool Compute::isInCheck(int color, int dx, int dy)
{
	int x, y, id_piesa;

	//get ccordinates of the king
	if (color == WHITE)
	{
		id_piesa = RA;
		x = pieceMapCoords[RA].x + dx;
		y = pieceMapCoords[RA].y + dy;
	}
	else
	{
		id_piesa = RN;
		x = pieceMapCoords[RN].x + dx;
		y = pieceMapCoords[RN].y + dy;
	}

	if (isInDanger(color, x, y, PAWN) || isInDanger(color, x, y, KNIGHT) || isInDanger(color, x, y , KING))
	{
		return true;
	}

	if (isInDanger(color, x, y, BISHOP_OR_ROOK_OR_QUEEN))
	{
		int aux = chessTable[x][y];
		chessTable[x][y] = EMPTY_SQUARE;

		if (color == WHITE)
		{
			for (int i = RRN + QUEENS_B; i > CN2; i--)
			{
				//pentru a evita recurenta nu consider regele
				if ((pieceMapCoords[i].x == false) || (i == RN))
					continue;

				if (mutare_corecta(i, pieceMapCoords[i].x, pieceMapCoords[i].y, x, y, BLACK))
				{
					chessTable[x][y] = aux;
					return true;
				}
			}
		}
		else
		{
			for (int i = RRA + QUEENS_W; i > CA2; i--)
			{
				if ((pieceMapCoords[i].x == false) || (i == RA))
					continue;

				if (mutare_corecta(i, pieceMapCoords[i].x, pieceMapCoords[i].y, x, y, WHITE))
				{
					chessTable[x][y] = aux;
					return true;
				}
			}

		}

		chessTable[x][y] = aux;
    }

	return false;
}

//stiu cu siguranta ca mutarea e valida cand functia este apelata
void Compute::loadMove(mutari *m,  int id_piesa,  int stopx,  int stopy, int color)
{
	std::set<int> emptySet;
	if (m->numar_mutari < (MAX_NUMBER_OF_EXPANDED_MOVES - 1))
	{
		std::set<int> emptySet;
		float scor = scor_mutare(id_piesa, pieceMapCoords[id_piesa].x, pieceMapCoords[id_piesa].y, stopx, stopy, color, SEARCH_LIMIT, 0, 0, emptySet, 1, 1, false);

		if (scor != INVALID_SCORE)
		{
			mutare m1;
			m1.id_piesa = id_piesa;
			m1.scor = scor;
			m1.stopx = stopx;
			m1.stopy = stopy;

			m->pieceMapCoords[m->numar_mutari++] = m1;
		}
	}
	
}

/* color e culoarea piesei care muta */

void Compute::mutari_pion( int id_piesa, mutari *p, int color)
{

	if (!pieceMapCoords[id_piesa].x)
		return;

	int x = pieceMapCoords[id_piesa].x;
	int y = pieceMapCoords[id_piesa].y;

if (color == WHITE)
{
	if (mutare_valida(id_piesa, x, y, x + 1, y + 1, WHITE))
	{
		loadMove(p, id_piesa, x + 1, y + 1, WHITE);
	}

	if (mutare_valida(id_piesa, x, y, x - 1, y + 1, WHITE))
	{
		loadMove(p, id_piesa, x - 1, y + 1, WHITE);
	}

	if (mutare_valida(id_piesa, x, y, x, y + 1, WHITE))
	{
		loadMove(p, id_piesa, x, y + 1, WHITE);
	}

	if (mutare_valida(id_piesa, x, y, x, y + 2, WHITE))
	{
		loadMove(p, id_piesa, x, y + 2, WHITE);
	}
}

else
{
	if (mutare_valida(id_piesa, x, y, x + 1, y - 1, BLACK))
	{
		loadMove(p, id_piesa, x + 1, y - 1, BLACK);
	}

	if (mutare_valida(id_piesa, x, y, x - 1, y - 1, BLACK))
	{
		loadMove(p, id_piesa, x - 1, y - 1, BLACK);
	}

	if (mutare_valida(id_piesa, x, y, x, y - 1, BLACK))
	{
		loadMove(p, id_piesa, x, y - 1, BLACK);
	}

	if (mutare_valida(id_piesa, x, y, x, y - 2, BLACK))
	{
		loadMove(p, id_piesa, x, y - 2, BLACK);
	}
}
}

void Compute::mutari_rege( int id_piesa, mutari *p, int color)
{

	int i, j;
	int x = pieceMapCoords[id_piesa].x;
	int y = pieceMapCoords[id_piesa].y;
	int x1 = x - 1;
	int y1 = y - 1;

	for (i = x1; i < x1 + 3; i++)
	{
		for (j = y1; j < y1 + 3; j++)
		{
			if ((i == x) && (j == y))
				continue;

			if (mutare_valida(id_piesa, x, y, i, j, color))
			{
				loadMove(p, id_piesa, i, j, color);
			}
		}
	}

	if (mutare_valida(id_piesa, x, y, x - 2, y, color))
	{
		loadMove(p, id_piesa, x - 2, y, color);
	}

	if (mutare_valida(id_piesa, x, y, x + 2, y, color))
	{
		loadMove(p, id_piesa, x + 2, y, color);
	}

}

bool Compute::isInDanger(int color, int stopx, int stopy, checkPin cP)
{
	int id_nebun1, id_nebun2, tura1, tura2, regina1, regina2, regina3, id_cal1, id_cal2, id_rege;
	if (color == WHITE)
	{
		if ((stopy < 7) && (cP == PAWN))
		{
			if ((stopx > 1) && pion_negru(chessTable[stopx - 1][stopy + 1]))
			{
				return true;
			}

			if ((stopx < 8) && pion_negru(chessTable[stopx + 1][stopy + 1]))
			{
				return true;
			}
		}

		id_nebun1 = NN1;
		id_nebun2 = NN2;
		id_cal1 = CN1;
		id_cal2 = CN2;
		tura1 = TN1;
		tura2 = TN2;
		regina1 = RRN;
		regina2 = RRN1;
		regina3 = RRN1;
		id_rege = RN;

		
	}
	else
	{

		if ((stopy > 2) && (cP == PAWN))
		{
			if ((stopx > 1) && pion_alb(chessTable[stopx - 1][stopy - 1]))
			{
				return true;
			}

			if ((stopx < 8) && pion_alb(chessTable[stopx + 1][stopy - 1]))
			{
				return true;
			}
		}

		id_nebun1 = NA1;
		id_nebun2 = NA2;
		id_cal1 = CA1;
		id_cal2 = CA2;
		tura1 = TA1;
		tura2 = TA2;
		regina1 = RRA;
		regina2 = RRA1;
		regina3 = RRA2;
		id_rege = RA;

	}

	if (cP == BISHOP)
	{
		return sameDiagonal(id_nebun1, stopx, stopy) || sameDiagonal(id_nebun2, stopx, stopy);
	}

	else if (cP == KING)
	{
		return ( (abs(stopx - pieceMapCoords[id_rege].x) <= 1) &&
				 (abs(stopy - pieceMapCoords[id_rege].y) <= 1)   );
	}

	else if (cP == KNIGHT)
	{
		int dx1 = abs(pieceMapCoords[id_cal1].x - stopx);
		int dy1 = abs(pieceMapCoords[id_cal1].y - stopy);
		
		int dx2 = abs(pieceMapCoords[id_cal2].x - stopx);
		int dy2 = abs(pieceMapCoords[id_cal2].y - stopy);

		if ((abs(dx1 - dy1) == 1) && (max(dx1, dy1) < 3) && (min(dx1, dy1) > 0))
			return true;

		if ((abs(dx2 - dy2) == 1) && (max(dx2, dy2) < 3) && (min(dx2, dy2) > 0))
			return true;
	
	}

	else if (cP == BISHOP_OR_ROOK)
	{

		return (sameDiagonal(id_nebun1, stopx, stopy) || sameDiagonal(id_nebun2, stopx, stopy) ||
			sameLine(tura1, stopy) || sameColumn(tura1, stopx) ||
			sameLine(tura2, stopy) || sameColumn(tura2, stopx));
	}

	else if (cP == BISHOP_OR_ROOK_OR_QUEEN)
	{

		return (sameDiagonal(id_nebun1, stopx, stopy) || sameDiagonal(id_nebun2, stopx, stopy) ||
			sameLine(tura1, stopy) || sameColumn(tura1, stopx) ||
			sameLine(tura2, stopy) || sameColumn(tura2, stopx) ||
			sameLine(regina1, stopy) || sameColumn(regina1, stopx) || sameDiagonal(regina1, stopx, stopy) ||
			sameLine(regina2, stopy) || sameColumn(regina2, stopx) || sameDiagonal(regina2, stopx, stopy) ||
			sameLine(regina3, stopy) || sameColumn(regina3, stopx) || sameDiagonal(regina3, stopx, stopy));
	}

	return false;
}

bool Compute::sameLine(int idPiesa, int y)
{
	return (y == pieceMapCoords[idPiesa].y);
}

bool Compute::sameColumn(int idPiesa, int x)
{
	return (x == pieceMapCoords[idPiesa].x);
}

bool Compute::sameDiagonal(int idPiesa, int x, int y)
{
	if (pieceMapCoords[idPiesa].x && pieceMapCoords[idPiesa].y)
	{
		return abs(x - pieceMapCoords[idPiesa].x) == abs(y - pieceMapCoords[idPiesa].y);
	}
	
	return false;

}


void Compute::mutari_tura( int id_piesa, mutari* p, int color)
{
	if (!pieceMapCoords[id_piesa].x)
		return;

	int x = pieceMapCoords[id_piesa].x;
	int y = pieceMapCoords[id_piesa].y;
	int x1 = x + 1;
	int y1 = y;

	while (muta(x1, y1))
	{
		loadMove(p, id_piesa, x1, y1, color);
		x1++;
	}

	if (mutare_valida(id_piesa, x, y, x1, y1, color))
	{
		loadMove(p, id_piesa, x1, y1, color);
	}

	x1 = x - 1;
	y1 = y;

	while (muta(x1, y1))
	{
		loadMove(p, id_piesa, x1, y1, color);
		x1--;
	}

	if (mutare_valida(id_piesa, x, y, x1, y1, color))
	{
		loadMove(p, id_piesa, x1, y1, color);
	}

	x1 = x;
	y1 = y - 1;

	while (muta(x1, y1))
	{
		loadMove(p, id_piesa, x1, y1, color);
		y1--;
	}

	if (mutare_valida(id_piesa, x, y, x1, y1, color))
	{
		loadMove(p, id_piesa, x1, y1, color);
	}

	x1 = x;
	y1 = y + 1;

	while (muta(x1, y1))
	{
		loadMove(p, id_piesa, x1, y1, color);
		y1++;
	}

	if (mutare_valida(id_piesa, x, y, x1, y1, color))
	{
		loadMove(p, id_piesa, x1, y1, color);
	}
}

void Compute::mutari_nebun( int id_piesa, mutari *p, int color)
{
	if (!pieceMapCoords[id_piesa].x)
		return;

	int x = pieceMapCoords[id_piesa].x;
	int y = pieceMapCoords[id_piesa].y;
	int x1 = x + 1;
	int y1 = y + 1;

	while (muta(x1, y1))
	{
		loadMove(p, id_piesa, x1, y1, color);
		x1++;
		y1++;
	}

	if (mutare_valida(id_piesa, x, y, x1, y1, color))
	{
		loadMove(p, id_piesa, x1, y1, color);
	}

	x1 = x + 1;
	y1 = y - 1;

	while (muta(x1, y1))
	{
		loadMove(p, id_piesa, x1, y1, color);
		x1++;
		y1--;
	}

	if (mutare_valida(id_piesa, x, y, x1, y1, color))
	{
		loadMove(p, id_piesa, x1, y1, color);
	}

	x1 = x - 1;
	y1 = y + 1;

	while (muta(x1, y1))
	{
		loadMove(p, id_piesa, x1, y1, color);
		x1--;
		y1++;
	}

	if (mutare_valida(id_piesa, x, y, x1, y1, color))
	{
		loadMove(p, id_piesa, x1, y1, color);
	}

	x1 = x - 1;
	y1 = y - 1;

	while (muta(x1, y1))
	{
		loadMove(p, id_piesa, x1, y1, color);
		x1--;
		y1--;
	}

	if (mutare_valida(id_piesa, x, y, x1, y1, color))
	{
		loadMove(p, id_piesa, x1, y1, color);
	}
}

void Compute::mutari_cal( int id_piesa, mutari *p, int color)
{
	if (!pieceMapCoords[id_piesa].x)
		return;
	int x = pieceMapCoords[id_piesa].x;
	int y = pieceMapCoords[id_piesa].y;

	if (mutare_valida(id_piesa, x, y, x + 2, y + 1, color))
	{
		loadMove(p, id_piesa, x + 2, y + 1, color);
	}
	if (mutare_valida(id_piesa, x, y, x + 1, y + 2, color))
	{
		loadMove(p, id_piesa, x + 1, y + 2, color);
	}

	if (mutare_valida(id_piesa, x, y, x - 2, y + 1, color))
	{
		loadMove(p, id_piesa, x - 2, y + 1, color);
	}
	if (mutare_valida(id_piesa, x, y, x - 1, y + 2, color))
	{
		loadMove(p, id_piesa, x - 1, y + 2, color);
	}
	if (mutare_valida(id_piesa, x, y, x + 2, y - 1, color))
	{
		loadMove(p, id_piesa, x + 2, y - 1, color);
	}
	if (mutare_valida(id_piesa, x, y, x + 1, y - 2, color))
	{
		loadMove(p, id_piesa, x + 1, y - 2, color);
	}
	if (mutare_valida(id_piesa, x, y, x - 2, y - 1, color))
	{
		loadMove(p, id_piesa, x - 2, y - 1, color);
	}
	if (mutare_valida(id_piesa, x, y, x - 1, y - 2, color))
	{
		loadMove(p, id_piesa, x - 1, y - 2, color);
	}
}

void Compute::mutari_regina( int id_piesa, mutari *p, int color)
{
	mutari_nebun(id_piesa, p, color);
	mutari_tura(id_piesa, p, color);
}

//color este culoarea celui care a mutat
float Compute::scor_p( int id_piesa, bool type, int colorOfLastMove) //color culoarea piesei care tocmai a mutat
{
	float scor_local = valoare(id_piesa);

	int start_x = pieceMapCoords[id_piesa].x;
	int start_y = pieceMapCoords[id_piesa].y;
	
	//penalty for double pawns in complex evaluation
	if (type == true)
	{
		if (pion_alb(id_piesa))
		{
			for (int i = start_y + 1; i <= 8; i++)
			{
				if (pion_alb(chessTable[start_x][i]))
				{
					scor_local -= param[0];
					break;
				}
			}
		}

		else if (pion_negru(id_piesa))
		{
			for (int i = start_y - 1; i >= 1; i--)
			{
				if (pion_negru(chessTable[start_x][i]))
				{
					scor_local -= param[0];
				}
			}
		}
	}

	//bonification for king castle in both evaluation types
	if (rege(id_piesa))
	{
		if (mutat[id_piesa] == ROCADA)
		{
			scor_local += param[1];
		}
	}

	//this is valid in both evaluation types
	
	int color = piesa_alba(id_piesa) ? WHITE : BLACK;
	
	if (color == colorOfLastMove)
	{
		if (isInDanger(color, start_x, start_y, PAWN) || isInDanger(color, start_x, start_y, KNIGHT) ||
			isInDanger(color, start_x, start_y, KING) ||
			isInDanger(color, start_x, start_y, BISHOP_OR_ROOK_OR_QUEEN))
		{
			return scor_local * param[4];
		}
	}

	return scor_local;

}

float Compute::punctaj_linie(int linie)
{
	float piese_albe = 0;
	float piese_negre = 0;

	for (int i = 1; i <= 8; i++)
	{
		if (piesa_alba(chessTable[i][linie]))
		{
			piese_albe++;
		}
		else if (piesa_neagra(chessTable[i][linie]))
		{
			piese_negre++;
		}
	}

	if (piese_albe == piese_negre)
	{
		return 0;
	}

	else if (piese_albe > piese_negre)
	{
		return piese_albe / (piese_albe + piese_negre);
	}
	else
	{
		return -piese_negre / (piese_albe + piese_negre);
	}
}

float Compute::punctaj_linii()
{
	float result = 0;
	for (int i = 1; i <= 8; i++)
	{
		result += punctaj_linie(i);
	}
	return result;
}

//color ste culoarea piesei care tocami a mutat
//culoarea este importanta pentru indentificarea situatiilor de remiza,pozitie incorecta sau situatii de mat.
float Compute::scor_static(bool type, float mutari_alb, float mutari_negru, int color)
{
	float sca = 0;
	

	for (int i = PA1; i <= RRA + QUEENS_W; i++)
	{
		if (!pieceMapCoords[i].x)
			continue;
		else //o piesa alba existenta
			sca += scor_p(i, type, color);
	}

	float scb = 0;

	for (int i = PN1; i <= RRN + QUEENS_B; i++)
	{
		if (!pieceMapCoords[i].x)
			continue;
		else //o piesa neagra existenta               
			scb += scor_p(i, type, color);
	}

	
	float bonusMutari = param[2] * ((float)mutari_alb / ((float)mutari_negru + (float)mutari_alb) - 0.5);
	sca += bonusMutari;

	if (type == true)
	{
		float bonusAgresivitate = param[3] * punctaj_linii() / 8;
		sca += bonusAgresivitate;
	}
	
	
	return (sca - scb);
}

struct compareMoves
{
	bool operator()(mutare& m1, mutare& m2)
	{
		return (m1.scor <= m2.scor);
	}
};

int Compute::genereaza_mutari(mutari *p, int color)
{
	p->numar_mutari = 0;
	 int i;
	
	if (color == WHITE)
	{
		//the king moves are probable the worst
		mutari_rege(RA, p, color);

		//the queen move is always bad especially in the beginning
		mutari_regina(RRA, p, color);

		for (i = RRA1; i < RRA1 + QUEENS_W; i++)
		{
			mutari_regina(i, p, color);
		}

		for (i = TA1; i <= TA2; i++)
		{
			mutari_tura(i, p, color);
		}

		for (i = PA1; i <= PA8; i++)
		{
			mutari_pion(i, p, color);
		}

		for (i = NA1; i <= NA2; i++)
		{
			mutari_nebun(i, p, color);
		}

		for (i = CA1; i <= CA2; i++) //cal 1 si cal 2
		{
			mutari_cal(i, p, color);
		}

	}

	if (color == BLACK)
	{

		//the king moves are probable the worst
		mutari_rege(RN, p, color);

		//the queen move is always bad especially in the beginning
		mutari_regina(RRN, p, color);

		for (i = RRN1; i < RRN1 + QUEENS_B; i++)
		{
			mutari_regina(i, p, color);
		}

		for (i = TN1; i <= TN2; i++)
		{
			mutari_tura(i, p, color);
		}
		
		for (i = PN1; i <= PN8; i++)
		{
			mutari_pion(i, p, color);
		}

		for (i = NN1; i <= NN2; i++)
		{
			mutari_nebun(i, p, color);
		}

		for (i = CN1; i <= CN2; i++) //cal 1 si cal 2
		{
			mutari_cal(i, p, color);
		}
	}

	std::sort(p->pieceMapCoords, p->pieceMapCoords + p->numar_mutari, compareMoves());

	return true;
}

int Compute::m_alb(int x, int y)
{
	return ((chessTable[x][y] == EMPTY_SQUARE) || piesa_neagra(chessTable[x][y]));
}

int Compute::m_negru(int x, int y)
{
	return ((chessTable[x][y] == EMPTY_SQUARE) || piesa_alba(chessTable[x][y]));

}

int Compute::muta( int x,  int y)
{
	return ((chessTable[x][y] == EMPTY_SQUARE) && (x > 0) && (x < 9) && (y > 0) && (y < 9));
}

void Compute::retrag_mutare(int id_piesa_atacanta, int id_piesa_capturata, int startx, int starty, int stopx, int stopy)
{
	chessTable[startx][starty] = id_piesa_atacanta;
	chessTable[stopx][stopy] = id_piesa_capturata;
	pieceMapCoords[id_piesa_atacanta].x = startx;
	pieceMapCoords[id_piesa_atacanta].y = starty;

	if (id_piesa_capturata != EMPTY_SQUARE)
	{
		pieceMapCoords[id_piesa_capturata].x = stopx;
		pieceMapCoords[id_piesa_capturata].y = stopy;
	}

}

//depth is the number of plies to be expanded 
void Compute::memoreaza_scor(int key,int depth, int type, float scor, int color)
{
	scoresCache[key].depth = depth;
	scoresCache[key].type = type;
	scoresCache[key].scor = scor;
	scoresCache[key].xor_key = xor_state;
	scoresCache[key].color = color;
	memcpy(scoresCache[key].chessTable, chessTable, sizeof(chessTable));
	memcpy(scoresCache[key].mutat, mutat, sizeof(mutat));
	
}


int Compute::found_configuration(int key, int color)
{
	return (scoresCache[key].scor) && (xor_state == scoresCache[key].xor_key) &&
		(scoresCache[key].color == color) && !memcmp(scoresCache[key].mutat, mutat, mutat_size) &&
		!memcmp(scoresCache[key].chessTable, chessTable, sizeof(chessTable));
}


unsigned int Compute::update_key(int id_piesa, int startx, int starty, 
								 int stopx, int stopy, int color)
{

	if (rege(id_piesa))
	{
		if ((startx - stopx) == -2)
		{
			xor_state ^= table_state[id_piesa][5][stopy];
			xor_state ^= table_state[id_piesa][7][stopy];

if (stopy > 1)
{
	xor_state ^= table_state[TN1][8][stopy];
	xor_state ^= table_state[TN1][6][stopy];
}
else
{
	xor_state ^= table_state[TA2][8][stopy];
	xor_state ^= table_state[TA2][6][stopy];
}

		}
		//face rocada la stanga
		else if ((startx - stopx) == 2)
		{

			xor_state ^= table_state[id_piesa][5][stopy];
			xor_state ^= table_state[id_piesa][3][stopy];

			if (stopy > 1)
			{
				xor_state ^= table_state[TN2][1][stopy];
				xor_state ^= table_state[TN2][4][stopy];
			}
			else
			{
				xor_state ^= table_state[TA1][1][stopy];
				xor_state ^= table_state[TA1][4][stopy];

			}

		}

		else
		{

			xor_state ^= table_state[id_piesa][startx][starty];
			xor_state ^= table_state[id_piesa][stopx][stopy];

			if (chessTable[stopx][stopy] != EMPTY_SQUARE)
				xor_state ^= table_state[chessTable[stopx][stopy]][stopx][stopy];

		}

	}
	else if (pion(id_piesa))
	{
		xor_state ^= table_state[id_piesa][startx][starty];

		if ((stopy > 1) && (stopy < 8))
			xor_state ^= table_state[id_piesa][stopx][stopy];
		else if (pion_alb(id_piesa))
			xor_state ^= table_state[RRA1 + QUEENS_W][stopx][stopy];
		else
			xor_state ^= table_state[RRN1 + QUEENS_B][stopx][stopy];

		if (abs(startx - stopx) == abs(starty - stopy))
		{
			if (chessTable[stopx][stopy] == EMPTY_SQUARE)
				xor_state ^= table_state[chessTable[stopx][starty]][stopx][starty];
			else
				xor_state ^= table_state[chessTable[stopx][stopy]][stopx][starty];

		}

	}
	else
	{
		xor_state ^= table_state[id_piesa][startx][starty];
		xor_state ^= table_state[id_piesa][stopx][stopy];

		if (chessTable[stopx][stopy] != EMPTY_SQUARE)
			xor_state ^= table_state[chessTable[stopx][stopy]][stopx][stopy];
	}

	xor_state ^= color_state[color];

	return (xor_state & (MAX_KEYS - 1));

}

//return an ordered pair.
pair <float, float> Compute::getInterval(int depth, float value, float delta, mutare& best_move, set<int> ancestor, int oponentMoves, int color)
{
	float v1, v2, v3, v4;
	bool larger;
	
	if (color == WHITE)
	{
		if (evalueaza_max(depth, value, value, best_move, ancestor, oponentMoves) > value)
		{
			larger = true;
		}
		else
		{
			larger = false;
		}
	}
	else
	{
		if(evalueaza_min(depth, value, value, best_move, ancestor, oponentMoves) > value)
		{
			larger = true;
		}
		else
		{
			larger = false;
		}
	}

	beginCompute:
	if (larger)
	{
		v1 = value + delta;
		v2 = value + 2 * delta;
		v3 = value + 4 * delta;
		v4 = value + 8 * delta;

	}
	else
	{
		v1 = value - delta;
		v2 = value - 2 * delta;
		v3 = value - 4 * delta;
		v4 = value - 8 * delta;
	}

	float res1, res2, res3, res4;
	
	if (color == WHITE)
	{
		std::thread t1([this, &res1, depth, v1, ancestor, oponentMoves](){
			mutare best_move;
			Compute c1(*this);
			res1 = c1.evalueaza_max(depth, v1, v1, best_move, ancestor, oponentMoves);
		});

		std::thread t2([this, &res2, depth, v2, ancestor, oponentMoves](){
			mutare best_move;
			Compute c2(*this);
			res2 = c2.evalueaza_max(depth, v2, v2, best_move, ancestor, oponentMoves);
		});

		std::thread t3([this, &res3, depth, v3, ancestor, oponentMoves](){
			mutare best_move;
			Compute c3(*this);
			res3 = c3.evalueaza_max(depth, v3, v3, best_move, ancestor, oponentMoves);
		});

		std::thread t4([this, &res4, depth, v4, ancestor, oponentMoves](){
			Compute c4(*this);
			mutare best_move;
			res4 = c4.evalueaza_max(depth, v4, v4, best_move, ancestor, oponentMoves);
		});

		t1.join();
		t2.join();
		t3.join();
		t4.join();
	}

	else
	{
		std::thread t1([this, &res1, depth, v1, ancestor, oponentMoves](){
			mutare best_move;
			Compute c1(*this);
			res1 = c1.evalueaza_min(depth, v1, v1, best_move, ancestor, oponentMoves);
		});

		std::thread t2([this, &res2, depth, v2, ancestor, oponentMoves](){
			mutare best_move;
			Compute c2(*this);
			res2 = c2.evalueaza_min(depth, v2, v2, best_move, ancestor, oponentMoves);
		});

		std::thread t3([this, &res3, depth, v3, ancestor, oponentMoves](){
			mutare best_move;
			Compute c3(*this);
			res3 = c3.evalueaza_min(depth, v3, v3, best_move, ancestor, oponentMoves);
		});

		std::thread t4([this, &res4, depth, v4, ancestor, oponentMoves](){
			mutare best_move;
			Compute c4(*this);
			res4 = c4.evalueaza_min(depth, v4, v4, best_move, ancestor, oponentMoves);
		});

		t1.join();
		t2.join();
		t3.join();
		t4.join();

	}

	if (larger)
	{
		if (res1 < v1)
		{
			pair<float, float> p(value, v1);
			return p;
		}
		else if (res2 < v2)
		{
			pair<float, float> p(v1, v2);
			return p;
		}
		else if (res3 < v3)
		{
			pair<float, float> p(v2, v3);
			return p;
		}
		else if (res4 < v4)
		{
			pair<float, float> p(v3, v4);
			return p;
		}
		else
		{
			value = v4;
			delta = 8 * delta;
			goto beginCompute;
		}
	}
	
	else
	{
		if (res1 > v1)
		{
			pair<float, float> p(v1, value);
			return p;
		}

		else if (res2 > v2)
		{
			pair<float, float> p(v2, v1);
			return p;
		}

		else if (res3 > v3)
		{
			pair<float, float> p(v3, v2);
			return p;
		}

		else if (res4 > v4)
		{
			pair<float, float> p(v4, v3);
			return p;
		}
		else
		{
			value = v4;
			delta = 8 * delta;
			goto beginCompute;
		}

	}

}

//return an ordered interval
pair <float, float> Compute::localizeInterval(int depth, float max, float min, mutare& best_move, set<int> ancestor, int oponentMoves, int color)
{
#if 0 
	float v1, v2, v3, v4;
	
	Compute c1(*this);
	Compute c2(*this);
	Compute c3(*this);
	Compute c4(*this);
	
	float delta = fabs(min - max);
	
	v1 = max + delta / 5;
	v2 = max + 2* delta / 5;
	v3 = max + 3 * delta / 5;
	v4 = max + 4 * delta / 5;

	float res1, res2, res3, res4;

	if (color == WHITE)
	{
		std::thread t1([&res1, &c1, depth, v1, ancestor, oponentMoves](){
			mutare best_move;
			res1 = c1.evalueaza_max(depth, v1, v1, best_move, ancestor, oponentMoves);
		});

		std::thread t2([&res2, &c2, depth, v2, ancestor, oponentMoves](){
			mutare best_move;
			res2 = c2.evalueaza_max(depth, v2, v2, best_move, ancestor, oponentMoves);
		});

		std::thread t3([&res3, &c3, depth, v3, ancestor, oponentMoves](){
			mutare best_move;
			res3 = c3.evalueaza_max(depth, v3, v3, best_move, ancestor, oponentMoves);
		});

		std::thread t4([&res4, &c4, depth, v4, ancestor, oponentMoves](){
			mutare best_move;
			res4 = c4.evalueaza_max(depth, v4, v4, best_move, ancestor, oponentMoves);
		});

		t1.join();
		t2.join();
		t3.join();
		t4.join();
	}

	else
	{
		std::thread t1([&res1, &c1, depth, v1, ancestor, oponentMoves](){
			mutare best_move;
			res1 = c1.evalueaza_min(depth, v1, v1, best_move, ancestor, oponentMoves);
		});

		std::thread t2([&res2, &c2, depth, v2, ancestor, oponentMoves](){
			mutare best_move;
			res2 = c2.evalueaza_min(depth, v2, v2, best_move, ancestor, oponentMoves);
		});

		std::thread t3([&res3, &c3, depth, v3, ancestor, oponentMoves](){
			mutare best_move;
			res3 = c3.evalueaza_min(depth, v3, v3, best_move, ancestor, oponentMoves);
		});

		std::thread t4([&res4, &c4, depth, v4, ancestor, oponentMoves](){
			mutare best_move;
			res4 = c4.evalueaza_min(depth, v4, v4, best_move, ancestor, oponentMoves);
		});

		t1.join();
		t2.join();
		t3.join();
		t4.join();
	}

	if (res1 < v1)
	{
		
		pair<float, float> p(max, v1);
		return p;
		
	}
	
	else if (res2 < v2)
	{
		pair<float, float> p(v1, v2);
		return p;	
	}
	
	else if (res3 < v3)
	{
		pair<float, float> p(v2, v3);
		return p;	
	}

	else if (res4 < v4)
	{
		pair<float, float> p(v3, v4);
		return p;
	}
	
	else
	{
		pair<float, float> p(v4, min);
		return p;	
	}
#endif
	std::pair<float, float> p(0, 0);
	return p;

}

//color is the color of the player who is going to move
void Compute::evalueaza_best(int color, float p_score, float delta, int key,
							 int depth, mutare &best_move)
{
	set<int>  ancestors;
	ancestors.insert(key);
	
	mutari m;
	genereaza_mutari(&m, 1-color);

	pair <float, float> vpair;

	if (delta > 0)
	{
		vpair = getInterval(depth, p_score, delta, best_move, ancestors, m.numar_mutari, color);
	
		if (color == WHITE)
		{
			evalueaza_max(depth, vpair.second, vpair.first, best_move, ancestors, m.numar_mutari);
		}

		else
		{	
			evalueaza_min(depth, vpair.second, vpair.first, best_move, ancestors, m.numar_mutari);	
		}
		
	}
	else
	{
		if (color == WHITE)
		{
			evalueaza_max(depth, WIN_SCORE + 1, LOSS_SCORE - 1, best_move, ancestors, m.numar_mutari);
		}
		else
		{
			evalueaza_min(depth, WIN_SCORE + 1, LOSS_SCORE - 1, best_move, ancestors, m.numar_mutari);
		}
		
	}

}


void Compute::initializeTable()
{
	//initializare pioni
	int i, j, k, l;
	
	for (i = 1; i <= 49; i++)
		for (j = 1; j <= 8; j++)
			for (k = 1; k <= 8; k++)
			{
		for (l = 0; l < 64; l++)
			if (rand() % 2)
				table_state[i][j][k] |= (unsigned long long)(1) << l;
			}

	for (i = 0; i < 2; i++)
		for (j = 0; j < 64; j++)
			if (rand() % 2)
				color_state[i] |= (unsigned long long)(1) << j;


	MAX_DEPTH =  4;
	SEARCH_LIMIT = 6;
	

	QUEENS_W = 0;
	QUEENS_B = 0;

	for (i = 0; i < NUMBER_OF_PIECES; i++)
		mutat[i] = false;

	for (i = 1; i <= 8; i++)
		for (j = 1; j <= 8; j++)
		{
		chessTable[i][j] = EMPTY_SQUARE;
		}

	memset(mutat, 0, sizeof(int) * NUMBER_OF_PIECES);

	for (i = PA1; i <= PA8; i++)
	{
		chessTable[1 + i - PA1][2] = i;
		pieceMapCoords[i].x = 1 + i - PA1;
		pieceMapCoords[i].y = 2;

		chessTable[8 - i + PA1][7] = PN1 + i - PA1;
		pieceMapCoords[PN1 + i - PA1].x = 8 - i + PA1;
		pieceMapCoords[PN1 + i - PA1].y = 7;
	}

	//cai
	chessTable[2][1] = CA1;
	pieceMapCoords[CA1].x = 2;
	pieceMapCoords[CA1].y = 1;
	chessTable[7][8] = CN1;
	pieceMapCoords[CN1].x = 7;
	pieceMapCoords[CN1].y = 8;

	chessTable[7][1] = CA2;
	pieceMapCoords[CA2].x = 7;
	pieceMapCoords[CA2].y = 1;
	chessTable[2][8] = CN2;
	pieceMapCoords[CN2].x = 2;
	pieceMapCoords[CN2].y = 8;

	//nebuni

	chessTable[3][1] = NA1;
	pieceMapCoords[NA1].x = 3;
	pieceMapCoords[NA1].y = 1;
	chessTable[6][8] = NN1;
	pieceMapCoords[NN1].x = 6;
	pieceMapCoords[NN1].y = 8;

	chessTable[6][1] = NA2;
	pieceMapCoords[NA2].x = 6;
	pieceMapCoords[NA2].y = 1;
	chessTable[3][8] = NN2;
	pieceMapCoords[NN2].x = 3;
	pieceMapCoords[NN2].y = 8;

	//ture

	chessTable[1][1] = TA1;
	pieceMapCoords[TA1].x = 1;
	pieceMapCoords[TA1].y = 1;
	chessTable[8][8] = TN1;
	pieceMapCoords[TN1].x = 8;
	pieceMapCoords[TN1].y = 8;
	chessTable[8][1] = TA2;
	pieceMapCoords[TA2].x = 8;
	pieceMapCoords[TA2].y = 1;
	chessTable[1][8] = TN2;
	pieceMapCoords[TN2].x = 1;
	pieceMapCoords[TN2].y = 8;

	//regi
	chessTable[5][1] = RA;
	pieceMapCoords[RA].x = 5;
	pieceMapCoords[RA].y = 1;
	chessTable[5][8] = RN;
	pieceMapCoords[RN].x = 5;
	pieceMapCoords[RN].y = 8;

	//regine
	chessTable[4][1] = RRA;
	pieceMapCoords[RRA].x = 4;
	pieceMapCoords[RRA].y = 1;
	chessTable[4][8] = RRN;
	pieceMapCoords[RRN].x = 4;
	pieceMapCoords[RRN].y = 8;

	xor_state = 0;
	for (i = 1; i <= 8; i++)
		for (j = 1; j <= 8; j++)
			if (chessTable[i][j] != EMPTY_SQUARE)
				xor_state ^= table_state[chessTable[i][j]][i][j];

	//since white move first assume that previous move was done by black.
	xor_state ^= color_state[BLACK];

	number_of_moves = 0;
	color_to_move = WHITE;
	player_color = WHITE;

#if USE_CACHE == 1
	memset(scoresCache, 0, sizeof(scoresCache));
#endif

}

void  Compute::move()
{
	//last_depth maximum number of levels in tree
	if ((color_to_move == player_color) && (play_mode == HUMAN_COMPUTER))
	{
		while (true)
		{
			//input is single threaded, it is ok to use globals.
			fflush(stdin);
			Sleep(20);
			fgets(input, 80, stdin);
			GetWords(input);

			if (!strcmp(words[0], "quit"))
			{
				exit(0);
			}

			else if (!strcmp(words[0], "time"))
			{
				available_time = atof(words[1]) / 100;				
			}

			else if (!strcmp(words[0], "new") || !strcmp(words[0], "result"))
			{
				initializeTable();
				return;
			}

			else if (!strcmp(words[0], "black"))
			{
				player_color = WHITE;
				color_to_move = WHITE;
				return;
			}
			else if (!strcmp(words[0], "white"))
			{
				player_color = BLACK;
				color_to_move = WHITE;
				return;
			}

			else if (isalpha(words[0][0]) && isdigit(words[0][1]))
			{
				int startx = words[0][0] - 96;
				int starty = words[0][1] - 48;
				int stopx = words[0][2] - 96;
				int stopy = words[0][3] - 48;
				int id_piesa = chessTable[startx][starty];

				if (mutare_corecta(id_piesa, startx, starty, stopx, stopy, color_to_move))
				{
					update_key(id_piesa, startx, starty, stopx, stopy, color_to_move);
					aplica_mutare(id_piesa, stopx, stopy);
					break;
				}
				
			}
		}
	}

	else
	{
		//asignez cate un scor la fiecare mutare posibila
		number_of_moves++;
		high_resolution_clock::time_point start_time = high_resolution_clock::now(), stop_time;


		float current_move_time = 0;
		float remaining_moves = (50 - number_of_moves);
		mutare best_move;
		best_move.scor = -1;

		//from opponent point of view
		//key includes who is going to move from this position 
		int  key = xor_state&(MAX_KEYS - 1);

		//in intial position delta is not so large
		static float delta = 0.05;
		static float p_score = 0;

		//fprintf(stdout, "MAX_DEPTH %d SEARCH_LIMIT %d available_time %2f remaining_moves %f number_of_moves %d\n", MAX_DEPTH, SEARCH_LIMIT, available_time, remaining_moves, number_of_moves);
		vector<float> evalTimes(10);
		evalTimes[0] = 0;
		
		for (int i = MAX_DEPTH - 1; ; i--)
		{
			
			evalueaza_best(color_to_move, p_score, delta, key, i, best_move);
			duration<double> time_span = duration_cast<duration<double>>(high_resolution_clock::now() - start_time);
			int currentIndex = MAX_DEPTH - i;
			evalTimes[currentIndex] = (float)time_span.count() - sum(evalTimes, currentIndex - 1);
			float nEvalTime;
			if (currentIndex > 1)
			{
				nEvalTime = (evalTimes[currentIndex] * evalTimes[currentIndex]) / evalTimes[currentIndex - 1];
			}
			else
			{
				nEvalTime = 8 * evalTimes[currentIndex];
			}

			float eEvalTime = sum(evalTimes, currentIndex) + nEvalTime;
			if ( ( ((available_time - eEvalTime) / remaining_moves) < (1.2 * eEvalTime) ) || (remaining_moves <= 0))
			{
				//at least ply 2
				if (currentIndex >= 2)
				{
					break;
				}
			}
			delta = fabs(best_move.scor - p_score);
			p_score = best_move.scor;
		}

		available_time-= sum(evalTimes, evalTimes.size() - 1);
		
		if ((color_to_move == WHITE && best_move.scor != LOSS_SCORE) ||
			(color_to_move == BLACK && best_move.scor != WIN_SCORE)
			)
		{
			int id_piesa = best_move.id_piesa;
			int startx = pieceMapCoords[id_piesa].x;
			int starty = pieceMapCoords[id_piesa].y;
			int stopx = best_move.stopx;
			int stopy = best_move.stopy;

			update_key(id_piesa, startx, starty, stopx, stopy, color_to_move);
			aplica_mutare(id_piesa, stopx, stopy);
			engine_move(startx, starty, stopx, stopy);
		}
		else
		{
			fprintf(stdout, "resign\n");
			fflush(stdout);
		}
		
			
	} //end engine to move

	
	color_to_move = 1 - color_to_move;
}

void blockInput()
{
	//to allow profiler to atach to process
	cout << "Block input" << endl;
	char c;
	cin >> c;
}


int main()

{
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stdin, NULL, _IONBF, 0); //turn off buffering

	for (int i = 1; i<50; i++)
	{
		if (pion(i))
			tabela_valori_piese[i] = 1;

		else if (cal(i) || nebun(i))
			tabela_valori_piese[i] = 3;

		else if (tura(i))
			tabela_valori_piese[i] = 5;

		else if (regina(i))
			tabela_valori_piese[i] = 9.3;

		else
			tabela_valori_piese[i] = 0;

	}
	

	read_eval_parameters();	
	Compute mainChessBoard;

	
	while (true)
	{
		mainChessBoard.move();
	}

	return 1;
}
