#include "ChessPlayer.h"
#include "Chess\GameStatus.h"
#include "Chess\Gameplay.h"
#include "Chess\Board.h"
#include "Chess\Piece.h"

using namespace std;



void ChessPlayer::setupPlayers(ChessPlayer** playerWhite, ChessPlayer** playerBlack, Board* pBoard, GameStatus* pGameStatus, Gameplay* pGamePlay)
{
	*playerBlack = new ChessPlayer(pBoard, pGameStatus, pGamePlay, PieceColor::BLACK);
	(*playerBlack)->setAI();

	*playerWhite = new ChessPlayer(pBoard, pGameStatus, pGamePlay, PieceColor::WHITE);
	(*playerWhite)->setAI();

}

ChessPlayer::ChessPlayer(Board* pBoard, GameStatus* pGameStatus, Gameplay* pGamePlay, PieceColor colour)
{
	m_colour = colour;
	m_pBoard = pBoard;
	m_pGameStatus = pGameStatus;
	m_pGamePlay = pGamePlay;
	m_bAI = false;
}

unsigned int ChessPlayer::getAllLivePieces(vecPieces& vpieces)
{
	vpieces.clear();

	PieceInPosition pip;

	unsigned int count = 0;
	for (int i = m_pBoard->MIN_ROW_INDEX; i < m_pBoard->MAX_ROW_INDEX; i++)
	{
		for (int j = m_pBoard->MIN_COL_INDEX; j < m_pBoard->MAX_COL_INDEX; j++)
		{
			std::shared_ptr<Piece> pPiece = m_pBoard->getSquare(i, j)->getOccupyingPiece();

			if (pPiece == nullptr)
				continue;
			if (pPiece->getColor() != m_colour)
				continue;

			count++;
			pip.piece = pPiece;
			pip.row = i;
			pip.col = j;
			vpieces.emplace_back(pip);
		}
	}

	return count;
}

vector<std::shared_ptr<Move>> ChessPlayer::getValidMovesForPiece(PieceInPosition pip)
{
	return Gameplay::getValidMoves(m_pGameStatus, m_pBoard, pip.piece, pip.row, pip.col);
}

// chooseAIMove
// in this method - for an AI chess player - choose a move to make. This is called once per play. 
bool ChessPlayer::chooseAIMove(std::shared_ptr<Move>* moveToMake)
{
	vecPieces vPieces;
	unsigned int piecesAvailable = getAllLivePieces(vPieces);

	// BAD AI !! - for the first piece which can move, choose the first available move
	bool moveAvailable = false;
	int randomPiece;
	while (!moveAvailable)
	{
		randomPiece = rand() % vPieces.size(); // choose a random chess piece
		vector<std::shared_ptr<Move>> moves = getValidMovesForPiece(vPieces[randomPiece]); // get all the valid moves for this piece if any)
		if (moves.size() > 0) // if there is a valid move exit this loop - we have a candidate 
			moveAvailable = true;
	}

	// get all moves for the random piece chosen (yes there is some duplication here...)
	vector<std::shared_ptr<Move>> moves = getValidMovesForPiece(vPieces[randomPiece]);
	if (moves.size() > 0)
	{
		int field = moves.size();
		int randomMove = rand() % field; // for all the possible moves for that piece, choose a random one
		*moveToMake = moves[randomMove]; // store it in 'moveToMake' and return
		return true;
	}

	return false; // if there are no moves to make return false
}

//vector<std::shared_ptr<Move>> ChessPlayer::MiniMax(int depth)
//{
//	/*vecPieces vPieces;
//	unsigned int piecesAvailable = getAllLivePieces(vPieces);
//	vector<std::shared_ptr<Move>> moves = vector<std::shared_ptr<Move>>();
//	for (int i = 0; i < vPieces.size(); i++)
//	{
//		vector<std::shared_ptr<Move>> pieceMoves = getValidMovesForPiece(vPieces[i]);
//		for (std::shared_ptr<Move> m : pieceMoves)
//		{
//			moves.push_back(m);
//		}
//	}
//	return MaxMove(moves, 0, 5);*/
//}
//
//vector<std::shared_ptr<Move>> ChessPlayer::MinMove(vector<std::shared_ptr<Move>> moves, int depth, int depthLimit)
//{
//	/*if (depth >= depthLimit)
//	{
//		return moves;
//	}
//
//	for (std::shared_ptr<Move> m : moves)
//	{
//
//	}
//
//	return vector<std::shared_ptr<Move>>();*/
//}
//
//vector<std::shared_ptr<Move>> ChessPlayer::MaxMove(vector<std::shared_ptr<Move>> moves, int depth, int depthLimit)
//{
//	/*if (depth >= depthLimit)
//	{
//		return moves;
//	}
//
//	for (std::shared_ptr<Move> m : moves)
//	{
//
//	}
//
//	return vector<std::shared_ptr<Move>>();*/
//}

int ChessPlayer::MiniMax(PieceInPosition piece, int depth, int alpha, int beta, bool maximisingPlayer)
{
	if (depth <= 0)
	{
		// return evaluation of piece
	}

	// find valid moves for the current piece
	vector<std::shared_ptr<Move>> pieceMoves = getValidMovesForPiece(piece);
	// convert moves into pieceinposition
	vector<PieceInPosition> movedPieces = vector<PieceInPosition>();
	for (std::shared_ptr<Move> m : pieceMoves)
	{
		PieceInPosition p = piece;
		p.row = m.get()->getDestinationPosition().first;
		p.col = m.get()->getDestinationPosition().second;
		movedPieces.push_back(p);
	}

	if (maximisingPlayer)
	{
		int maxEval = -INFINITY;
		for (PieceInPosition p : movedPieces)
		{
			int eval = MiniMax(p, depth - 1, alpha, beta, false);
			maxEval = max(maxEval, eval);
			alpha = max(alpha, eval);
			if (beta <= alpha)
				break;
		}
		return maxEval;
	}
	else
	{
		int minEval = INFINITY;
		for (PieceInPosition p : movedPieces)
		{
			int eval = MiniMax(p, depth - 1, alpha, beta, true);
			minEval = min(minEval, eval);
			beta = min(beta, eval);
			if (beta <= alpha)
				break;
		}
		return minEval;
	}
}

vector<shared_ptr<Piece>> ChessPlayer::GetOpponentPieces()
{
	const int height = m_pBoard->HEIGHT;
	const int width = m_pBoard->WIDTH;
	PieceColor oppositeColour = m_colour == PieceColor::BLACK ? PieceColor::WHITE : PieceColor::BLACK;
	vector<shared_ptr<Piece>> opponentPieces = vector<shared_ptr<Piece>>();
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			std::shared_ptr<Piece> p = m_pBoard->getSquare(i, j)->getOccupyingPiece();
			if (p.get()->getColor() == oppositeColour)
			{
				opponentPieces.push_back(p);
			}
		}
	}
	return opponentPieces;
}
