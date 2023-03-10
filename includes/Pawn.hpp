#pragma once

#include "Equilattechess.hpp"
#include "IPiece.hpp"

#define BLACK_PAWN_IMG "assets/black_pawn.png"
#define WHITE_PAWN_IMG "assets/white_pawn.png"

class Pawn : public IPiece
{
public:
    Pawn (Game*, ColorName, boardPos);
    virtual ~Pawn();
    virtual std::vector<boardPos> getPossibleMoves();
    virtual IPiece *move(boardPos, bool);
protected:
    Game     *_currentGame;
};