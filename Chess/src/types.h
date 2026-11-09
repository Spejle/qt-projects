#ifndef TYPES_H
#define TYPES_H

#include <QString>
#include <QMetaType>

enum class PieceColor {
    None = 0,
    White,
    Black
};

inline PieceColor opponentColor(PieceColor c) {
    if (c == PieceColor::White) return PieceColor::Black;
    if (c == PieceColor::Black) return PieceColor::White;
    return PieceColor::None;
}

enum class PieceType {
    None = 0,
    Pawn,
    Knight,
    Bishop,
    Rook,
    Queen,
    King
};

struct BoardPos {
    int row = -1;
    int col = -1;

    constexpr BoardPos() = default;
    constexpr BoardPos(int r, int c) : row(r), col(c) {}

    bool isValid() const {
        return row >= 0 && row < 8 && col >= 0 && col < 8;
    }

    bool operator==(const BoardPos &other) const {
        return row == other.row && col == other.col;
    }

    bool operator!=(const BoardPos &other) const {
        return !(*this == other);
    }

    QString toAlgebraic() const {
        if (!isValid()) return QStringLiteral("-");
        char fileChar = static_cast<char>('a' + col);
        int rankNum = 8 - row;
        return QString("%1%2").arg(fileChar).arg(rankNum);
    }

    static BoardPos fromAlgebraic(const QString &alg) {
        if (alg.length() < 2) return {-1, -1};
        int c = alg[0].toLatin1() - 'a';
        int r = 8 - (alg[1].toLatin1() - '0');
        BoardPos pos(r, c);
        return pos.isValid() ? pos : BoardPos(-1, -1);
    }
};

struct Piece {
    PieceType type = PieceType::None;
    PieceColor color = PieceColor::None;

    bool isEmpty() const {
        return type == PieceType::None || color == PieceColor::None;
    }

    bool operator==(const Piece &o) const {
        return type == o.type && color == o.color;
    }

    bool operator!=(const Piece &o) const {
        return !(*this == o);
    }

    int value() const {
        switch (type) {
            case PieceType::Pawn:   return 100;
            case PieceType::Knight: return 320;
            case PieceType::Bishop: return 330;
            case PieceType::Rook:   return 500;
            case PieceType::Queen:  return 900;
            case PieceType::King:   return 20000;
            default: return 0;
        }
    }
};

struct Move {
    BoardPos from;
    BoardPos to;
    Piece movedPiece;
    Piece capturedPiece;
    PieceType promotion = PieceType::None;

    bool isEnPassant = false;
    bool isCastling = false;
    bool isTwoSquarePawnMove = false;

    // State restoration for undo
    bool prevWhiteKingMoved = false;
    bool prevWhiteKingsideRookMoved = false;
    bool prevWhiteQueensideRookMoved = false;
    bool prevBlackKingMoved = false;
    bool prevBlackKingsideRookMoved = false;
    bool prevBlackQueensideRookMoved = false;
    BoardPos prevEnPassantSquare = {-1, -1};
    int prevHalfMoveClock = 0;

    QString sanNotation; // e.g. "e4", "Nf3", "O-O", "Qh5#"
    QString commentary;  // Antigravity AI reasoning/thoughts

    bool operator==(const Move &o) const {
        return from == o.from && to == o.to && promotion == o.promotion;
    }
};

enum class GameStatus {
    Active,
    Check,
    Checkmate,
    Stalemate,
    DrawInsufficientMaterial,
    DrawFiftyMoves,
    Resignation
};

enum class GameMode {
    PlayerVsPlayer,
    PlayerVsAI,
    PlayerVsClaudeHaiku,
    PlayerVsAntigravity
};

enum class AIDifficulty {
    Easy,
    Medium,
    Hard
};

enum class BoardTheme {
    ClassicWood,
    ModernSlate,
    TournamentGreen,
    DarkGlass
};

#endif // TYPES_H
