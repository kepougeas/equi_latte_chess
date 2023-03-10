#pragma once

#include <SFML/Graphics/Texture.hpp>
#include <string>
#include <vector>

struct boardPos {
    int x;
    int y;
};

enum PieceName {
    Pawn,
    Bishop,
    Knight,
    Rook,
    Queen,
    King
};

static const char *enumPieceName[] = {
    "Pawn",
    "Bishop",
    "Knight",
    "Rook",
    "Queen",
    "King"
};

enum ColorName {
    WHITE,
    BLACK
};

class   IPiece
{
public:
    ~IPiece() { delete this->_displayPiece; };

    virtual std::vector<boardPos> getPossibleMoves() = 0;
    virtual IPiece *move(boardPos, bool) = 0;

    boardPos getPosition() { return this->_position; };
    bool isFirstMove() { return this->_isFirstMove; };
    void setFirstMoveState(bool state) { this->_isFirstMove = state; };
    PieceName getName() { return this->_name; };
    ColorName getColor() { return this->_color; };
    void printPossibleMoves();
    void printPosition() { printf("Piece %s %s is at [X = %i ; Y = %i]\n", this->_color == WHITE ? "WHITE" : "BLACK", enumPieceName[this->_name], this->_position.x, this->_position.y); };
    sf::Texture *getTexture() { return this->_displayPiece; };
    bool getEnPassant() { return this->_enPassant; };
    void setEnPassant(bool val) { this->_enPassant = val; };
protected:
    PieceName        _name;
    boardPos         _position;
    boardPos         _initialPosition;
    ColorName        _color;
    sf::Texture      *_displayPiece;
    bool             _enPassant = false;
    bool             _isFirstMove = true;
};

inline bool operator==(const boardPos &pos1, const boardPos &pos2)
{
    return pos1.x == pos2.x && pos1.y == pos2.y;
}

inline void IPiece::printPossibleMoves()
{
    std::vector<boardPos> possibleMoves;

    possibleMoves = this->getPossibleMoves();
    printf("Piece %s can move to the following positions : \n", enumPieceName[this->_name]);

    for (boardPos pos : possibleMoves) {
        printf("[X = %i ; Y = %i]\n", pos.x, pos.y);
    }
}