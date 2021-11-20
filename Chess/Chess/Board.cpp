#include "Board.h"

Board::Board()
{
}

Board::~Board()
{
}

Board::Board(const Board& board)
{
	for(int row= MIN_ROW_INDEX; row< MAX_ROW_INDEX; row++)
	{
		for(int col= MIN_COL_INDEX; col< MAX_COL_INDEX; col++)
		{
			this->squares[row][col] = board.squares[row][col];
		}
	}
}

Square* Board::getSquare(int row, int col)
{
	return &squares[row][col];
}

Board* Board::hardCopy()
{
	Board* pBoard = new Board();

	for (int row = MIN_ROW_INDEX; row < MAX_ROW_INDEX; row++)
	{
		for (int col = MIN_COL_INDEX; col < MAX_COL_INDEX; col++)
		{
			Square temp = this->squares[row][col].hardCopy();
			if (temp.getOccupyingPiece())
			{
				pBoard->squares[row][col] = temp;
			}
		}
	}

	return pBoard;
}

// TODO: change this so it evaluates board after x moves. currently just evaluates current board
// possibly pass in a list of moves and then apply them?
int Board::evaluateBoard(PieceColor colour, std::vector<Move> moves)
{
	// save the squares of the current board
	Square newBoard[WIDTH][HEIGHT];
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			newBoard[i][j] = squares[i][j];
		}
	}

	// simulate what the board would be like after the moves
	for (Move m : moves)
	{
		//std::shared_ptr<Piece> p = m.getMovedPiece();
		if (m.getType() == MoveType::NORMAL)
		{
			newBoard[m.getOriginPosition().first][m.getOriginPosition().second].removeOccupyingPiece();
			newBoard[m.getDestinationPosition().first][m.getDestinationPosition().second].occupySquare(m.getMovedPiece());
		}
		else if (m.getType() == MoveType::CAPTURE)
		{
			newBoard[m.getOriginPosition().first][m.getOriginPosition().second].removeOccupyingPiece();
			newBoard[m.getDestinationPosition().first][m.getDestinationPosition().second].removeOccupyingPiece();
			newBoard[m.getDestinationPosition().first][m.getDestinationPosition().second].occupySquare(m.getMovedPiece());
		}
	}

	std::vector<std::shared_ptr<Piece>> whitePieces = std::vector<std::shared_ptr<Piece>>();
	std::vector<std::shared_ptr<Piece>> blackPieces = std::vector<std::shared_ptr<Piece>>();

	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			std::shared_ptr<Piece> p = newBoard[i][j].getOccupyingPiece();
			if (p.get() != nullptr)
			{
				if (p.get()->getColor() == PieceColor::WHITE)
				{
					whitePieces.push_back(p);
				}
				else
				{
					blackPieces.push_back(p);
				}
			}
		}
	}

	int whiteEval = 0;
	for (std::shared_ptr<Piece> p : whitePieces)
	{
		whiteEval += (int)p.get()->getType();
	}
	int blackEval = 0;
	for (std::shared_ptr<Piece> p : blackPieces)
	{
		blackEval += (int)p.get()->getType();
	}

	int eval = 0;
	if (colour == PieceColor::WHITE)
		eval = whiteEval - blackEval;
	else
		eval = blackEval - whiteEval;

	return eval;
}
