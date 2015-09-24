
#include "functii_tabla.h"

int MAX_DEPTH;
int SEARCH_LIMIT;
float param[PARAMETERS];
float tabela_valori_piese[NUMBER_OF_PIECES];

struct compareMoves
{
	bool operator()(const mutare& m1, const mutare& m2)
	{
		return (m1.scor <= m2.scor);
	}
};











