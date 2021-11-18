#pragma once
#include "Chess\PieceColor.h"
#include "Chess\PieceType.h"
#include <vector>
#include <memory>
#include "Chess/Move.h"

using namespace std;

class Piece;
class Board;
class GameStatus;
class Gameplay;
class Move;

struct PieceInPosition
{
	std::shared_ptr<Piece> piece;
	int col;
	int row;
};

typedef vector<PieceInPosition> vecPieces;

class ChessPlayer
{
public:
	static void		setupPlayers(ChessPlayer** playerWhite, ChessPlayer** playerBlack, Board* pBoard, GameStatus* pGameStatus, Gameplay* pGamePlay);
	ChessPlayer(Board* pBoard, GameStatus* pGameStatus, Gameplay* pGamePlay, PieceColor colour);

	void			setAI() { m_bAI = true; }
	bool			isAI() { return m_bAI; }
	unsigned int	getAllLivePieces(vecPieces& vpieces);
	vector<std::shared_ptr<Move>>	getValidMovesForPiece(PieceInPosition pip);
	bool			chooseAIMove(std::shared_ptr<Move>* moveToMake);

protected:
	PieceColor		getColour() { return m_colour; }

private:
	PieceColor	m_colour;
	Board*		m_pBoard;
	GameStatus* m_pGameStatus;
	Gameplay*	m_pGamePlay;
	bool		m_bAI;

	int MiniMax(PieceInPosition piece, int depth, int alpha, int beta, bool maximisingPlayer);

	/*vector<std::shared_ptr<Move>> MiniMax(int depth);
	vector<std::shared_ptr<Move>> MinMove(vector<std::shared_ptr<Move>> moves, int depth, int depthLimit);
	vector<std::shared_ptr<Move>> MaxMove(vector<std::shared_ptr<Move>> moves, int depth, int depthLimit);*/
	vector<shared_ptr<Piece>> GetOpponentPieces();
};

