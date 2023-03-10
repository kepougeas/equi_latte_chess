#include "../../includes/Equilattechess.hpp"
#include <vector>

Knight::Knight(Game *currentGame, ColorName color, boardPos position)
{
    this->_currentGame = currentGame;
    this->_color = color;
    this->_position = position;
    this->_initialPosition = position;
    this->_name = PieceName::Knight;
    this->_displayPiece = new sf::Texture();
    this->_displayPiece->setSmooth(true);
    if (!this->_displayPiece->loadFromFile(color == WHITE ? WHITE_KNIGHT_IMG : BLACK_KNIGHT_IMG)) {
        printf("Error loading image for knight\n");
        exit(1);
    }
}

Knight::~Knight()
{}

std::vector<boardPos> Knight::getPossibleMoves()
{
    std::vector<boardPos> possibleMoves;
    boardPos checkedPos = this->_position;

    // Knights can jump over pieces and do an L shaped move
    // Basically 8 moves to check
    checkedPos.x += 1;
    checkedPos.y -= 2;
    if (this->_currentGame->isPositionFree(checkedPos, this->_color)) {
        possibleMoves.push_back(checkedPos);
    }
    checkedPos = this->_position;
    checkedPos.x -= 1;
    checkedPos.y -= 2;
    if (this->_currentGame->isPositionFree(checkedPos, this->_color)) {
        possibleMoves.push_back(checkedPos);
    }

    checkedPos = this->_position;
    checkedPos.x -= 2;
    checkedPos.y -= 1;
    if (this->_currentGame->isPositionFree(checkedPos, this->_color)) {
        possibleMoves.push_back(checkedPos);
    }
    checkedPos = this->_position;
    checkedPos.x -= 2;
    checkedPos.y += 1;
    if (this->_currentGame->isPositionFree(checkedPos, this->_color)) {
        possibleMoves.push_back(checkedPos);
    }

    checkedPos = this->_position;
    checkedPos.x -= 1;
    checkedPos.y += 2;
    if (this->_currentGame->isPositionFree(checkedPos, this->_color)) {
        possibleMoves.push_back(checkedPos);
    }
    checkedPos = this->_position;
    checkedPos.x += 1;
    checkedPos.y += 2;
    if (this->_currentGame->isPositionFree(checkedPos, this->_color)) {
        possibleMoves.push_back(checkedPos);
    }

    checkedPos = this->_position;
    checkedPos.x += 2;
    checkedPos.y -= 1;
    if (this->_currentGame->isPositionFree(checkedPos, this->_color)) {
        possibleMoves.push_back(checkedPos);
    }
    checkedPos = this->_position;
    checkedPos.x += 2;
    checkedPos.y += 1;
    if (this->_currentGame->isPositionFree(checkedPos, this->_color)) {
        possibleMoves.push_back(checkedPos);
    }

    return possibleMoves;
}

IPiece *Knight::move(boardPos destPos, bool simulated)
{
    IPiece *destPiece = this->_currentGame->getPieceOfPos(destPos);

    // Piece is attacking opponent
    if (destPiece && destPiece->getColor() != this->_color) {
        if (!simulated) {
            this->_currentGame->addCapturedPiece(destPiece);
        }
        this->_currentGame->removePiece(destPiece);
    }

    this->_position = destPos;
    this->_isFirstMove = simulated ? this->_isFirstMove : false;

    return destPiece;
}