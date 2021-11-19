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

int Board::evaluateBoard(PieceColor colour)
{
	std::vector<std::shared_ptr<Piece>> whitePieces = std::vector<std::shared_ptr<Piece>>();
	std::vector<std::shared_ptr<Piece>> blackPieces = std::vector<std::shared_ptr<Piece>>();

	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			std::shared_ptr<Piece> p = squares[i][j].getOccupyingPiece();
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
			else
			{
				printf("hello :)");
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
		eval = whiteEval;// -blackEval;
	else
		eval = blackEval;// -whiteEval;

	return eval;
}
