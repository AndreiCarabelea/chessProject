
#ifndef __SAH_H__
#define __SAH_H__
#include <set>
#include <tuple>
#include <utility>
using namespace std;
class Compute
{
public:

	Compute();
	void  move();
	void evalueaza_best(int color, float p_score, float delta, int key,
	int depth, mutare &best_move);
	float evalueaza_max(int depth, float minim, float maxim, mutare &best_move, set<int> s, int oponentsMoves);
	float evalueaza_min(int depth, float minim, float maxim, mutare &best_move, set<int> s, int oponentsMoves);
	inline int found_gen_configuration(int key);
	inline void memoreaza_gen(int key, const mutari&  m);
	float scor_mutare(register int id_piesa, register int startx, register int starty, register int stopx, register int stopy, int color,
	int depth, float minim, float maxim, set<int> ancestors, int mutari_alb, int mutari_negru, bool complexEvaluation=true);
	inline unsigned int update_key(int id_piesa, int startx, int starty, int stopx, int stopy, int color);
	void aplica_mutare(int id_piesa, int stopx, int stopy);
	inline void memoreaza_scor(int key, int depth, int type, float scor, int color);
	float scor_static(bool type, float mutari_alb, float mutari_negru, int color);
	int genereaza_mutari(mutari *p, int color);
	float calculeaza_scor(int color, int key, int depth, float minim, float maxim, set<int> s, int oponentMoves);
	float scor_p(register int id_piesa, bool type, int color);
	inline void retrag_mutare(int id_piesa_atacanta, int id_piesa_capturata, int startx, int starty, int stopx, int stopy);
	float nonRecursiveScore(int  key, bool complexEvaluation, float mutari_alb, float mutari_negru, int color);
	bool mutare_corecta(register int id_piesa, register int startx, register int starty, register int stopx, register int  stopy, int color);
	pair <float, float> getInterval(int depth, float value, float delta, mutare& best_move, set<int> ancestor, int oponentMoves, int color);
	pair <float, float> localizeInterval(int depth, float max, float min, mutare& best_move, set<int> ancestor, int oponentMoves, int color);

	
private:

	enum checkPin
	{
		BISHOP = 1,
		BISHOP_OR_ROOK,
		BISHOP_OR_ROOK_OR_QUEEN,
		PAWN,
		KNIGHT,
		KING
	};

	static const int mutat_size = NUMBER_OF_PIECES * sizeof(int);
	float number_of_moves = 0;

	int play_mode = HUMAN_COMPUTER;
	int player_color = WHITE;
	int color_to_move = WHITE;

	int chessTable[9][9];
	piesa pieceMapCoords[NUMBER_OF_PIECES];
	int mutat[NUMBER_OF_PIECES];//se foloseste pentru ture si regi doar

	unsigned long long table_state[50][9][9];
	unsigned long long color_state[2];
	unsigned long long xor_state;

	cache_s  scoresCache[MAX_KEYS];

	int QUEENS_W = 0;
	int QUEENS_B = 0;

	void engine_move(int startx, int starty, int stopx, int stopy);
	void mutare_normala(register int id_piesa, register int startx, register int starty, register int stopx, register int stopy);
	bool mutare_valida(register int id_piesa, register int startx, register int starty, register int stopx, register int  stopy, int color);
	void mutari_pion(register int id_piesa, mutari *p, int color);
	void mutari_rege(register int id_piesa, mutari *p, int color);
	void mutari_nebun(register int id_piesa, mutari *p, int color);
	void mutari_cal(register int id_piesa, mutari *p, int color);
	void mutari_tura(register int id_piesa, mutari *p, int color);
	void mutari_regina(register int id_piesa, mutari *p, int color);
	inline int muta(register int x, register int y);
	int found_configuration(int key, int color);
	inline int m_alb(int x, int y);
	inline int m_negru(int x, int y);
	bool mutare_valida_nebun(register int startx, register int starty, register int stopx, register int stopy, int color);
	bool mutare_valida_tura(register int startx, register int starty, register int stopx, register int stopy, int color);
	bool diff_color(int color, register int x, register int y, register int stopx, register int stopy);
	bool isInCheck(int color, int x = 0, int y = 0);
	void loadMove(mutari *m, register int id_piesa, register int stopx, register int stopy, int color);
	float binaryEval(int key, int color, float delta, int depth, float min, float max, set<int>);
	bool isInDanger(int color, int stopx, int stopy, Compute::checkPin cP);
	bool sameLine(int idPiesa, int x);
	bool sameColumn(int idPiesa, int y);
	bool sameDiagonal(int idPiesa, int x, int y);
	float punctaj_linie(int i);
	float punctaj_linii();

	void initializeTable();
	
};

#endif