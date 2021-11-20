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
Move ChessPlayer::chooseAIMove()
{
	vecPieces vPieces;
	unsigned int piecesAvailable = getAllLivePieces(vPieces);

	int bestScore = INT_MIN;
	Move bestMove = Move();
	for (PieceInPosition p : vPieces)
	{
		vector<std::shared_ptr<Move>> moves = getValidMovesForPiece(p);
		for (std::shared_ptr<Move> m : moves)
		{
			int move = MiniMax(*m.get(), 6, 0, 0, true);
			std::pair<int, int> originPos = m.get()->getOriginPosition();
			std::pair<int, int> destinationPos = m.get()->getDestinationPosition();
			if (originPos != std::pair<int, int>() && destinationPos != std::pair<int, int>())
			{
				if (move > bestScore)
				{
					bestScore = move;
					bestMove = *m.get();
				}
			}
		}
	}
	if (bestScore != INT_MIN)
	{
		std::pair<int, int> originPos = bestMove.getOriginPosition();
		std::pair<int, int> destinationPos = bestMove.getDestinationPosition();
		if (originPos != std::pair<int, int>() && destinationPos != std::pair<int, int>())
		{
			return bestMove;
		}
	}

	return Move(); // returning an empty Move will show that a move was not taken
	//return false; // if there are no moves to make return false
}

int ChessPlayer::MiniMax(Move m, int depth, int alpha, int beta, bool maximisingPlayer)
{
	if (depth <= 0)
	{
		// return evaluation of piece
		return m_pBoard->evaluateBoard(m_colour);
	}

	// find valid moves for the current piece
	PieceInPosition movedPiece = PieceInPosition();// = m.getMovedPiece().get();
	movedPiece.piece = m.getMovedPiece();
	movedPiece.row = m.getDestinationPosition().first;
	movedPiece.col = m.getDestinationPosition().second;
	vector<std::shared_ptr<Move>> pieceMoves = getValidMovesForPiece(movedPiece);

	if (maximisingPlayer)
	{
		int maxEval = INT_MIN;
		for (std::shared_ptr<Move> m : pieceMoves)
		{
			int move = MiniMax(*m.get(), depth - 1, alpha, beta, false);
			int eval = move;
			if (eval > maxEval)
			{
				maxEval = eval;
			}
			maxEval = max(maxEval, eval);
			alpha = max(alpha, eval);
			if (beta <= alpha)
				break;
		}
		return maxEval;
	}
	else
	{
		int minEval = INT_MAX;
		for (std::shared_ptr<Move> m : pieceMoves)
		{
			int move = MiniMax(*m.get(), depth - 1, alpha, beta, true);
			int eval = move;
			if (eval < minEval)
			{
				minEval = eval;
			}
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
