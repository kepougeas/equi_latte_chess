#pragma once

#include "Equilattechess.hpp"
#include "IPiece.hpp"

#define BLACK_KING_IMG "assets/black_king.png"
#define WHITE_KING_IMG "assets/white_king.png"

class King : public IPiece
{
public:
    King (Game*, ColorName, boardPos);
    virtual ~King();
    virtual std::vector<boardPos> getPossibleMoves();
    virtual bool move(boardPos);
protected:
    Game     *_currentGame;
};