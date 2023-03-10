#include "../includes/Equilattechess.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <string>

Board::Board(sf::RenderWindow *window, Game *currentGame, int *state)
{
    this->_window = window;
    this->_state = state;
    this->_currentGame = currentGame;

    // Loading the fonts
    if (!this->_fontGeneral.loadFromFile(AKIRA_FONT_PATH)) {
        printf("Could not load font");
        exit(-1);
    }

    // Creating the exit button
    this->_exitButton.setFont(this->_fontGeneral);
    this->_exitButton.setString(EXIT_TEXT);
    this->_exitButton.setCharacterSize(MENU_TEXT_SIZE);
    this->_exitButton.setFillColor(sf::Color::White);
    this->_exitButton.setPosition(this->_window->getSize().x - this->_exitButton.getGlobalBounds().width * 2, 40);

    // current player turn display
    this->_currentPlayerText.setFont(this->_fontGeneral);
    this->_currentPlayerText.setCharacterSize(CURR_PLAYER_TEXT_SIZE);
    this->_currentPlayerText.setFillColor(sf::Color::Red);
    this->_currentPlayerText.setPosition(this->_window->getSize().x - 400, 150);

    // Captured pieces text
    this->_capturedPiecesText.setFont(this->_fontGeneral);
    this->_capturedPiecesText.setString(CAPTURED_PIECES_TEXT);
    this->_capturedPiecesText.setCharacterSize(CURR_PLAYER_TEXT_SIZE);
    this->_capturedPiecesText.setFillColor(sf::Color::White);
    this->_capturedPiecesText.setPosition(this->_window->getSize().x - 400, 200);

    // Captured area
    this->_capturedArea = sf::RectangleShape(sf::Vector2f(WINDOW_WIDTH - 1000, 360));
    this->_capturedArea.setFillColor(sf::Color(255, 248, 220, 255));
    this->_capturedArea.setPosition(975, 240);

    // Winner text
    this->_winnerText.setFont(this->_fontGeneral);
    this->_winnerText.setString(WINNER_TEXT);
    this->_winnerText.setCharacterSize(50);
    this->_winnerText.setFillColor(sf::Color::Green);
    this->_winnerText.setPosition(this->_window->getSize().x / 2 - this->_winnerText.getGlobalBounds().width / 2, this->_window->getSize().y / 2 - 300);

    // Indexes text
    for (int i = 0; i < 16; i++) {
        sf::Text indexText;
        char index[2] = {'\0', '\0'};

        indexText.setFont(this->_fontGeneral);
        indexText.setCharacterSize(CURR_PLAYER_TEXT_SIZE);
        indexText.setFillColor(sf::Color::Black);
        // Number indexes
        if (i < 8) {
            indexText.setPosition(5, i * 120 + 5);
            index[0] = '8' - i;
            indexText.setString(index);
        }
        else if (i >= 8) {
            indexText.setPosition((i - 8) * 120 + 100, 940);
            index[0] = 'A' + (i - 8);
            indexText.setString(index);
        }
        this->_indexes[i] = indexText;
    }

    // Creating the ChessBoard squares
    bool colorToggle = true;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            sf::RectangleShape square(sf::Vector2f(120, 120));
            square.setFillColor(colorToggle == true ? sf::Color(255, 248, 220, 255) : sf::Color(165,42,42,255));
            square.setPosition(j * 120, i * 120);
            this->_chessSquares[i][j] = square;
            colorToggle = !colorToggle;
        }
        colorToggle = !colorToggle;
    }
};

void Board::drawBoard()
{
    IPiece *piece;
    char index[2] = {'\0', '\0'};

    // We draw EXIT button player turn display, the main board and all the pieces
    this->_window->draw(this->_exitButton);
    this->_currentPlayerText.setString(this->_currentGame->getCurrentPlayer() == WHITE ? CURRENT_PLAYER_TEXT_WHITE : CURRENT_PLAYER_TEXT_BLACK);
    this->_window->draw(this->_currentPlayerText);
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            piece = this->_currentGame->getPieceOfPos({.x = i, .y = j});
            this->_window->draw(this->_chessSquares[i][j]);
            if (piece != nullptr) {
                sf::Sprite sprite;
                sprite.setTexture(*piece->getTexture());
                sprite.setPosition(j * 120 + 33, i * 120 + (piece->getName() == Pawn ? 20 : 10));
                this->_window->draw(sprite);
            }
        }
    }

    // If there is a selected Piece we draw square around it and possible moves on board
    if ((piece = this->_currentGame->getSelectedPiece()) != nullptr) {
        sf::RectangleShape selectSquare(sf::Vector2f(110, 110));
        boardPos piecePos = piece->getPosition();
        std::vector<boardPos> possibleMoves = piece->getPossibleMoves();

        selectSquare.setFillColor(sf::Color(255,255,255,0));
        selectSquare.setOutlineColor(sf::Color(104,165,42));
        selectSquare.setOutlineThickness(5);
        selectSquare.setPosition(piecePos.y * 120 + 5, piecePos.x * 120 + 5);
        this->_window->draw(selectSquare);

        for (boardPos currPos: possibleMoves)
        {
            // We do not draw moves where the King would be in check
            if (this->_currentGame->checkNextKingSituation(piece, currPos) != NORMAL) {
                continue;
            }
            sf::RectangleShape possibleMoveSquare(sf::Vector2f(110, 110));
            
            possibleMoveSquare.setFillColor(sf::Color(255, 255, 255, 0));
            possibleMoveSquare.setOutlineColor(sf::Color(42,165,165));
            possibleMoveSquare.setOutlineThickness(5);
            possibleMoveSquare.setPosition(currPos.y * 120 + 5, currPos.x * 120 + 5);
            this->_window->draw(possibleMoveSquare);
        }
    }

    this->drawCapturedPieces();
    this->drawIndexes();

    this->checkClickEvents();
}

void Board::drawCapturedPieces()
{
    char widthIndex = 0;
    char heightIndex = 0;

    this->_window->draw(this->_capturedPiecesText);
    this->_window->draw(this->_capturedArea);

    for (IPiece *piece : this->_currentGame->getCapturedPieces(WHITE)) {
        if (widthIndex > 9) {
            widthIndex = 0;
            heightIndex++;
        }
        sf::Sprite sprite;
        sprite.setTexture(*piece->getTexture());
        sprite.scale(sf::Vector2f(0.5, 0.5));
        sprite.setPosition(widthIndex++ * 40 + (this->_window->getSize().x - 420), heightIndex * 60 + 300 - sprite.getGlobalBounds().height);
        this->_window->draw(sprite);
    }

    widthIndex = 0;
    heightIndex = 0;
    for (IPiece *piece : this->_currentGame->getCapturedPieces(BLACK)) {
        if (widthIndex > 9) {
            widthIndex = 0;
            heightIndex++;
        }
        sf::Sprite sprite;
        sprite.setTexture(*piece->getTexture());
        sprite.scale(sf::Vector2f(0.5, 0.5));
        sprite.setPosition(widthIndex++ * 40 + (this->_window->getSize().x - 420), heightIndex * 60 + 450 - sprite.getGlobalBounds().height);
        this->_window->draw(sprite);
    }
}

void Board::drawIndexes()
{
    for (int i = 0; i < 16; i++) {
        this->_window->draw(this->_indexes[i]);
    }
}

void Board::checkClickEvents()
{
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        // Quick sleep to prevent annoying multiple clicking
        usleep(100000);

        sf::Vector2i clicPosition = sf::Mouse::getPosition(*this->_window);
        sf::Vector2f exitButtonPosition = this->_exitButton.getPosition();
        sf::FloatRect exitButtonSize = this->_exitButton.getGlobalBounds();
        sf::Vector2f checkedPiecePos;
        sf::FloatRect checkedPosSize;
        IPiece *clickedPiece;

        // We clicked EXIT
        if ((clicPosition.x >= exitButtonPosition.x && clicPosition.x <= (exitButtonPosition.x + exitButtonSize.width * 2))
        && (clicPosition.y >= exitButtonPosition.y && clicPosition.y <= (exitButtonPosition.y + exitButtonSize.height * 2)))
        {
            *this->_state = EXIT_STATE;
        }

        // If there is no selected piece
        if ((clickedPiece = this->_currentGame->getSelectedPiece()) == nullptr) {
        //Check for a piece click
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    checkedPiecePos = this->_chessSquares[i][j].getPosition();
                    checkedPosSize = this->_chessSquares[i][j].getGlobalBounds();
                    if ((clicPosition.x > checkedPiecePos.x && clicPosition.x < (checkedPiecePos.x + checkedPosSize.width))
                    && (clicPosition.y > checkedPiecePos.y && clicPosition.y < (checkedPiecePos.y + checkedPosSize.height)))
                    {
                        // We clicked on a piece
                        clickedPiece = this->_currentGame->getPieceOfPos({.x = i, .y = j});
                        // If this is the right player then we can select
                        if (clickedPiece == nullptr ||
                        this->_currentGame->getCurrentPlayer() == clickedPiece->getColor()) {
                            this->_currentGame->setSelectedPiece(clickedPiece);
                            usleep(100000);
                        }
                    }
                }
            }
        }
        // Check for click on possible moves (unselect piece if outside of possible moves)
        else {
            std::vector<boardPos> possibleMoves = clickedPiece->getPossibleMoves();

            // We check if clic happened in any of the possible moves area
            for (boardPos currPos : possibleMoves) {
                // Tricky reversed indexes (sub optimal for understanding but I have time constraint)
                if ((clicPosition.x > (currPos.y * 120) && clicPosition.x < (currPos.y * 120 + 120))
                && (clicPosition.y > (currPos.x * 120) && clicPosition.y < (currPos.x * 120 + 120)))
                {
                    // Clic happened on a possible move, so we move the piece
                    // We can only move the piece if it does not put the King in Check
                    if (this->_currentGame->checkNextKingSituation(clickedPiece, currPos) == NORMAL) {
                        clickedPiece->move(currPos, false);
                        if (clickedPiece->getName() == Pawn &&
                            ((clickedPiece->getColor() == WHITE && currPos.x == 0) ||
                            (clickedPiece->getColor() == BLACK && currPos.x == 7))) {
                                PieceName chosenPromotion = this->promotionMenu(clickedPiece->getColor());
                                this->_currentGame->promotePawn(clickedPiece, chosenPromotion);
                        }
                        this->_currentGame->finishTurn();
                        this->_currentGame->resetEnPassant();
                        if (this->_currentGame->checkMateChecker(this->_currentGame->getCurrentPlayer()) == CHECKMATE) {
                            this->_currentGame->finishGame();
                        }
                    }
                }
            }
            this->_currentGame->setSelectedPiece(nullptr);
        }
    }
}

PieceName Board::promotionMenu(ColorName promotedColor)
{
    // display menu for piece promotion selection
    sf::RectangleShape menuArea(sf::Vector2f(600, 300));
    sf::Text           menuText;

    sf::Texture        pawnTexture;
    sf::Texture        bishopTexture;
    sf::Texture        knightTexture;
    sf::Texture        queenTexture;
    sf::Texture        rookTexture;

    sf::Sprite         pawnSprite;
    sf::Sprite         bishopSprite;
    sf::Sprite         knightSprite;
    sf::Sprite         queenSprite;
    sf::Sprite         rookSprite;

    menuArea.setFillColor(sf::Color::White);
    menuArea.setOutlineColor(sf::Color::Black);
    menuArea.setOutlineThickness(20);
    menuArea.setPosition(this->_window->getSize().x / 2 - 300, this->_window->getSize().y / 2 - 150);
    
    menuText.setFont(this->_fontGeneral);
    menuText.setCharacterSize(CURR_PLAYER_TEXT_SIZE);
    menuText.setString(PROMOTION_MENU_TEXT);
    menuText.setPosition(menuArea.getPosition().x + 30, menuArea.getPosition().y + 20);
    menuText.setFillColor(sf::Color::Black);

    pawnTexture.loadFromFile(promotedColor == WHITE ? WHITE_PAWN_IMG : BLACK_PAWN_IMG);
    bishopTexture.loadFromFile(promotedColor == WHITE ? WHITE_BISHOP_IMG : BLACK_BISHOP_IMG);
    knightTexture.loadFromFile(promotedColor == WHITE ? WHITE_KNIGHT_IMG : BLACK_KNIGHT_IMG);
    queenTexture.loadFromFile(promotedColor == WHITE ? WHITE_QUEEN_IMG : BLACK_QUEEN_IMG);
    rookTexture.loadFromFile(promotedColor == WHITE ? WHITE_ROOK_IMG : BLACK_ROOK_IMG);

    pawnSprite.setTexture(pawnTexture);
    pawnSprite.setPosition(menuArea.getPosition().x + 50, menuArea.getPosition().y + 120);

    bishopSprite.setTexture(bishopTexture);
    bishopSprite.setPosition(pawnSprite.getGlobalBounds().width + pawnSprite.getPosition().x + 40, menuArea.getPosition().y + 120);

    knightSprite.setTexture(knightTexture);
    knightSprite.setPosition(bishopSprite.getGlobalBounds().width + bishopSprite.getPosition().x + 40, menuArea.getPosition().y + 120);

    queenSprite.setTexture(queenTexture);
    queenSprite.setPosition(knightSprite.getGlobalBounds().width + knightSprite.getPosition().x + 40, menuArea.getPosition().y + 120);

    rookSprite.setTexture(rookTexture);
    rookSprite.setPosition(queenSprite.getGlobalBounds().width + queenSprite.getPosition().x + 40, menuArea.getPosition().y + 120);

    while (true)
    {
        this->_window->draw(menuArea);
        this->_window->draw(menuText);
        this->_window->draw(pawnSprite);
        this->_window->draw(bishopSprite);
        this->_window->draw(knightSprite);
        this->_window->draw(queenSprite);
        this->_window->draw(rookSprite);

        this->_window->display();

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            // Quick sleep to prevent annoying multiple clicking
            usleep(100000);

            sf::Vector2i clicPosition = sf::Mouse::getPosition(*this->_window);
            sf::Vector2f exitButtonPosition = this->_exitButton.getPosition();
            sf::FloatRect exitButtonSize = this->_exitButton.getGlobalBounds();

            // We clicked EXIT
            if ((clicPosition.x >= exitButtonPosition.x && clicPosition.x <= (exitButtonPosition.x + exitButtonSize.width * 2))
            && (clicPosition.y >= exitButtonPosition.y && clicPosition.y <= (exitButtonPosition.y + exitButtonSize.height * 2)))
            {
                *this->_state = EXIT_STATE;
                return Pawn;
            }

            // We clicked on PAWN
            if ((clicPosition.x > pawnSprite.getPosition().x && clicPosition.x < (pawnSprite.getPosition().x + pawnSprite.getGlobalBounds().width))
            && (clicPosition.y > pawnSprite.getPosition().y && clicPosition.y < (pawnSprite.getPosition().y + pawnSprite.getGlobalBounds().height))) {
                return Pawn;
            }
            // We clicked BISHOP
            else if ((clicPosition.x > bishopSprite.getPosition().x && clicPosition.x < (bishopSprite.getPosition().x + bishopSprite.getGlobalBounds().width))
            && (clicPosition.y > bishopSprite.getPosition().y && clicPosition.y < (bishopSprite.getPosition().y + bishopSprite.getGlobalBounds().height))) {
                return Bishop;
            }
            // We clicked KNIGHT
            else if ((clicPosition.x > knightSprite.getPosition().x && clicPosition.x < (knightSprite.getPosition().x + knightSprite.getGlobalBounds().width))
            && (clicPosition.y > knightSprite.getPosition().y && clicPosition.y < (knightSprite.getPosition().y + knightSprite.getGlobalBounds().height))) {
                return Knight;
            }
            // We clicked QUEEN
            else if ((clicPosition.x > queenSprite.getPosition().x && clicPosition.x < (queenSprite.getPosition().x + queenSprite.getGlobalBounds().width))
            && (clicPosition.y > queenSprite.getPosition().y && clicPosition.y < (queenSprite.getPosition().y + queenSprite.getGlobalBounds().height))) {
                return Queen;
            }
            // We clicked ROOK
            else if ((clicPosition.x > rookSprite.getPosition().x && clicPosition.x < (rookSprite.getPosition().x + rookSprite.getGlobalBounds().width))
            && (clicPosition.y > rookSprite.getPosition().y && clicPosition.y < (rookSprite.getPosition().y + rookSprite.getGlobalBounds().height))) {
                return Rook;
            }
        }
    }
}

void Board::displayWinner(ColorName winner)
{
    sf::Text           playerText;
    sf::RectangleShape winnerArea(sf::Vector2f(WINDOW_WIDTH - 40, WINDOW_HEIGHT - 40));

    winnerArea.setFillColor(sf::Color(165,42,42,255));
    winnerArea.setOutlineColor(sf::Color(104,165,42));
    winnerArea.setOutlineThickness(30);
    winnerArea.setPosition(20, 20);

    playerText.setFont(this->_fontGeneral);
    playerText.setCharacterSize(100);
    playerText.setFillColor(winner == WHITE ? sf::Color::White : sf::Color::Black);
    playerText.setString(winner == WHITE ? "WHITE" : "BLACK");
    playerText.setPosition(this->_window->getSize().x / 2 - playerText.getGlobalBounds().width / 2, this->_winnerText.getPosition().y + 200);
    
    this->_window->draw(winnerArea);
    this->_window->draw(this->_winnerText);
    this->_window->draw(playerText);
    this->_window->draw(this->_exitButton);

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        sf::Vector2i clicPosition = sf::Mouse::getPosition(*this->_window);
        sf::Vector2f exitButtonPosition = this->_exitButton.getPosition();
        sf::FloatRect exitButtonSize = this->_exitButton.getGlobalBounds();

        // We clicked EXIT
        if ((clicPosition.x >= exitButtonPosition.x && clicPosition.x <= (exitButtonPosition.x + exitButtonSize.width * 2))
        && (clicPosition.y >= exitButtonPosition.y && clicPosition.y <= (exitButtonPosition.y + exitButtonSize.height * 2)))
        {
            *this->_state = EXIT_STATE;
        }
    }
}