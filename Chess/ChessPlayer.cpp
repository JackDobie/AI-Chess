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

	ScoredMove* bestMove = nullptr;
	for (PieceInPosition p : vPieces)
	{
		ScoredMove* m = MiniMax(&p, nullptr, 6, 0, 0, true);
		if (bestMove == nullptr)
			bestMove = m;
		else
		{
			if (m->score > bestMove->score)
			{
				bestMove = m;
			}
		}
	}

	return false; // if there are no moves to make return false
}

ScoredMove* ChessPlayer::MiniMax(PieceInPosition* piece, Move* m, int depth, int alpha, int beta, bool maximisingPlayer)
{
	if (depth <= 0)
	{
		// return evaluation of piece
	}

	// find valid moves for the current piece
	vector<std::shared_ptr<Move>> pieceMoves = getValidMovesForPiece(*piece);

	ScoredMove* bestMove = new ScoredMove();
	bestMove->piece = piece;
	bestMove->move = m;
	if (maximisingPlayer)
	{
		int maxEval = -INFINITY;
		for (std::shared_ptr<Move> m : pieceMoves)
		{
			// create a pieceinposition from move and pass both into minimax
			PieceInPosition* p = piece;
			p->row = m.get()->getDestinationPosition().first;
			p->col = m.get()->getDestinationPosition().second;
			ScoredMove* move = MiniMax(p, m.get(), depth - 1, alpha, beta, false);
			int eval = move->score;
			if (eval > maxEval)
			{
				bestMove->score = move->score;
				if (bestMove->move == nullptr) bestMove->move = move->move;
				maxEval = eval;
			}
			maxEval = max(maxEval, eval);
			alpha = max(alpha, eval);
			if (beta <= alpha)
				break;
		}
		return bestMove;
	}
	else
	{
		int minEval = INFINITY;
		for (std::shared_ptr<Move> m : pieceMoves)
		{
			PieceInPosition* p = piece;
			p->row = m.get()->getDestinationPosition().first;
			p->col = m.get()->getDestinationPosition().second;
			ScoredMove* move = MiniMax(p, m.get(), depth - 1, alpha, beta, true);
			int eval = move->score;
			if (eval < minEval)
			{
				bestMove->score = move->score;
				if (bestMove->move == nullptr) bestMove->move = move->move;
				minEval = eval;
			}
			minEval = min(minEval, eval);
			beta = min(beta, eval);
			if (beta <= alpha)
				break;
		}
		return bestMove;
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
