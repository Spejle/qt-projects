#ifndef CHESSGAME_H
#define CHESSGAME_H

#include <QObject>
#include <QVector>
#include "types.h"

class ChessGame : public QObject {
    Q_OBJECT

public:
    explicit ChessGame(QObject *parent = nullptr);

    void reset();

    Piece pieceAt(int r, int c) const;
    Piece pieceAt(const BoardPos &pos) const;
    void setPieceAt(const BoardPos &pos, const Piece &piece);

    PieceColor currentTurn() const { return m_turn; }
    GameStatus status() const { return m_status; }
    void setStatus(GameStatus st) { m_status = st; }

    BoardPos lastMoveFrom() const { return m_lastFrom; }
    BoardPos lastMoveTo() const { return m_lastTo; }

    BoardPos kingPosition(PieceColor color) const;
    bool isKingInCheck(PieceColor color) const;
    bool isSquareAttacked(const BoardPos &pos, PieceColor attackingColor) const;

    QVector<Move> generateLegalMoves(PieceColor color);
    QVector<Move> legalMovesForSquare(const BoardPos &pos);

    bool makeMove(Move &move);
    bool undoMove();

    const QVector<Move>& moveHistory() const { return m_history; }
    const QVector<Piece>& capturedByWhite() const { return m_capturedByWhite; }
    const QVector<Piece>& capturedByBlack() const { return m_capturedByBlack; }

    int materialAdvantage(PieceColor color) const;

    QString exportPGN() const;
    QString exportFEN() const;
    QString lastMoveNotation() const;

signals:
    void moveMade(const Move &move);
    void boardReset();
    void gameStatusChanged(GameStatus status);

private:
    void setupInitialPosition();
    QVector<Move> generatePseudoLegalMoves(PieceColor color) const;
    void generatePawnMoves(const BoardPos &pos, PieceColor color, QVector<Move> &moves) const;
    void generateKnightMoves(const BoardPos &pos, PieceColor color, QVector<Move> &moves) const;
    void generateBishopMoves(const BoardPos &pos, PieceColor color, QVector<Move> &moves) const;
    void generateRookMoves(const BoardPos &pos, PieceColor color, QVector<Move> &moves) const;
    void generateQueenMoves(const BoardPos &pos, PieceColor color, QVector<Move> &moves) const;
    void generateKingMoves(const BoardPos &pos, PieceColor color, QVector<Move> &moves) const;
    void generateCastlingMoves(PieceColor color, QVector<Move> &moves) const;

    void applyRawMove(const Move &move);
    void revertRawMove(const Move &move);

    void updateGameStatus();
    bool checkInsufficientMaterial() const;
    QString computeSAN(const Move &move, const QVector<Move> &legalMovesBefore);

    Piece m_board[8][8];
    PieceColor m_turn = PieceColor::White;
    GameStatus m_status = GameStatus::Active;

    // Castling rights
    bool m_whiteKingMoved = false;
    bool m_whiteKingsideRookMoved = false;
    bool m_whiteQueensideRookMoved = false;

    bool m_blackKingMoved = false;
    bool m_blackKingsideRookMoved = false;
    bool m_blackQueensideRookMoved = false;

    BoardPos m_enPassantSquare = {-1, -1};
    int m_halfMoveClock = 0;
    int m_fullMoveNumber = 1;

    BoardPos m_lastFrom = {-1, -1};
    BoardPos m_lastTo = {-1, -1};

    QVector<Move> m_history;
    QVector<Piece> m_capturedByWhite;
    QVector<Piece> m_capturedByBlack;
};

#endif // CHESSGAME_H
