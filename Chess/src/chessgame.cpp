#include "chessgame.h"
#include <QDateTime>
#include <algorithm>

ChessGame::ChessGame(QObject *parent)
    : QObject(parent)
{
    reset();
}

void ChessGame::reset() {
    setupInitialPosition();
    m_turn = PieceColor::White;
    m_status = GameStatus::Active;

    m_whiteKingMoved = false;
    m_whiteKingsideRookMoved = false;
    m_whiteQueensideRookMoved = false;

    m_blackKingMoved = false;
    m_blackKingsideRookMoved = false;
    m_blackQueensideRookMoved = false;

    m_enPassantSquare = {-1, -1};
    m_halfMoveClock = 0;
    m_fullMoveNumber = 1;

    m_lastFrom = {-1, -1};
    m_lastTo = {-1, -1};

    m_history.clear();
    m_capturedByWhite.clear();
    m_capturedByBlack.clear();

    emit boardReset();
    emit gameStatusChanged(m_status);
}

void ChessGame::setupInitialPosition() {
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            m_board[r][c] = {PieceType::None, PieceColor::None};
        }
    }

    // Black main pieces (row 0)
    m_board[0][0] = {PieceType::Rook, PieceColor::Black};
    m_board[0][1] = {PieceType::Knight, PieceColor::Black};
    m_board[0][2] = {PieceType::Bishop, PieceColor::Black};
    m_board[0][3] = {PieceType::Queen, PieceColor::Black};
    m_board[0][4] = {PieceType::King, PieceColor::Black};
    m_board[0][5] = {PieceType::Bishop, PieceColor::Black};
    m_board[0][6] = {PieceType::Knight, PieceColor::Black};
    m_board[0][7] = {PieceType::Rook, PieceColor::Black};

    // Black pawns (row 1)
    for (int c = 0; c < 8; ++c) {
        m_board[1][c] = {PieceType::Pawn, PieceColor::Black};
    }

    // White pawns (row 6)
    for (int c = 0; c < 8; ++c) {
        m_board[6][c] = {PieceType::Pawn, PieceColor::White};
    }

    // White main pieces (row 7)
    m_board[7][0] = {PieceType::Rook, PieceColor::White};
    m_board[7][1] = {PieceType::Knight, PieceColor::White};
    m_board[7][2] = {PieceType::Bishop, PieceColor::White};
    m_board[7][3] = {PieceType::Queen, PieceColor::White};
    m_board[7][4] = {PieceType::King, PieceColor::White};
    m_board[7][5] = {PieceType::Bishop, PieceColor::White};
    m_board[7][6] = {PieceType::Knight, PieceColor::White};
    m_board[7][7] = {PieceType::Rook, PieceColor::White};
}

Piece ChessGame::pieceAt(int r, int c) const {
    if (r < 0 || r >= 8 || c < 0 || c >= 8) {
        return {PieceType::None, PieceColor::None};
    }
    return m_board[r][c];
}

Piece ChessGame::pieceAt(const BoardPos &pos) const {
    return pieceAt(pos.row, pos.col);
}

void ChessGame::setPieceAt(const BoardPos &pos, const Piece &piece) {
    if (pos.isValid()) {
        m_board[pos.row][pos.col] = piece;
    }
}

BoardPos ChessGame::kingPosition(PieceColor color) const {
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            if (m_board[r][c].type == PieceType::King && m_board[r][c].color == color) {
                return {r, c};
            }
        }
    }
    return {-1, -1};
}

bool ChessGame::isSquareAttacked(const BoardPos &pos, PieceColor attackingColor) const {
    if (!pos.isValid()) return false;

    // 1. Pawn attacks
    int pawnDir = (attackingColor == PieceColor::White) ? 1 : -1;
    int pawnRow = pos.row + pawnDir;
    if (pawnRow >= 0 && pawnRow < 8) {
        if (pos.col - 1 >= 0) {
            const Piece &p = m_board[pawnRow][pos.col - 1];
            if (p.type == PieceType::Pawn && p.color == attackingColor) return true;
        }
        if (pos.col + 1 < 8) {
            const Piece &p = m_board[pawnRow][pos.col + 1];
            if (p.type == PieceType::Pawn && p.color == attackingColor) return true;
        }
    }

    // 2. Knight attacks
    static const int kMoves[8][2] = {
        {-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
        {1, -2},  {1, 2},  {2, -1},  {2, 1}
    };
    for (const auto &km : kMoves) {
        int r = pos.row + km[0];
        int c = pos.col + km[1];
        if (r >= 0 && r < 8 && c >= 0 && c < 8) {
            const Piece &p = m_board[r][c];
            if (p.type == PieceType::Knight && p.color == attackingColor) return true;
        }
    }

    // 3. Bishop and Queen (diagonals)
    static const int diagDirs[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
    for (const auto &d : diagDirs) {
        int r = pos.row + d[0];
        int c = pos.col + d[1];
        while (r >= 0 && r < 8 && c >= 0 && c < 8) {
            const Piece &p = m_board[r][c];
            if (!p.isEmpty()) {
                if (p.color == attackingColor && (p.type == PieceType::Bishop || p.type == PieceType::Queen)) {
                    return true;
                }
                break;
            }
            r += d[0];
            c += d[1];
        }
    }

    // 4. Rook and Queen (orthogonals)
    static const int orthoDirs[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    for (const auto &d : orthoDirs) {
        int r = pos.row + d[0];
        int c = pos.col + d[1];
        while (r >= 0 && r < 8 && c >= 0 && c < 8) {
            const Piece &p = m_board[r][c];
            if (!p.isEmpty()) {
                if (p.color == attackingColor && (p.type == PieceType::Rook || p.type == PieceType::Queen)) {
                    return true;
                }
                break;
            }
            r += d[0];
            c += d[1];
        }
    }

    // 5. King attacks (adjacent squares)
    static const int kingDirs[8][2] = {
        {-1, -1}, {-1, 0}, {-1, 1},
        {0, -1},           {0, 1},
        {1, -1},  {1, 0},  {1, 1}
    };
    for (const auto &d : kingDirs) {
        int r = pos.row + d[0];
        int c = pos.col + d[1];
        if (r >= 0 && r < 8 && c >= 0 && c < 8) {
            const Piece &p = m_board[r][c];
            if (p.type == PieceType::King && p.color == attackingColor) return true;
        }
    }

    return false;
}

bool ChessGame::isKingInCheck(PieceColor color) const {
    BoardPos kPos = kingPosition(color);
    if (!kPos.isValid()) return false;
    return isSquareAttacked(kPos, opponentColor(color));
}

void ChessGame::generatePawnMoves(const BoardPos &pos, PieceColor color, QVector<Move> &moves) const {
    int forward = (color == PieceColor::White) ? -1 : 1;
    int startRow = (color == PieceColor::White) ? 6 : 1;
    int promoRow = (color == PieceColor::White) ? 0 : 7;

    // Single step forward
    int nextRow = pos.row + forward;
    if (nextRow >= 0 && nextRow < 8 && m_board[nextRow][pos.col].isEmpty()) {
        if (nextRow == promoRow) {
            // Promotions
            PieceType pTypes[] = {PieceType::Queen, PieceType::Rook, PieceType::Bishop, PieceType::Knight};
            for (auto pt : pTypes) {
                Move m;
                m.from = pos;
                m.to = {nextRow, pos.col};
                m.movedPiece = m_board[pos.row][pos.col];
                m.promotion = pt;
                moves.append(m);
            }
        } else {
            Move m;
            m.from = pos;
            m.to = {nextRow, pos.col};
            m.movedPiece = m_board[pos.row][pos.col];
            moves.append(m);

            // Double step forward from initial rank
            if (pos.row == startRow) {
                int doubleRow = pos.row + 2 * forward;
                if (m_board[doubleRow][pos.col].isEmpty()) {
                    Move m2;
                    m2.from = pos;
                    m2.to = {doubleRow, pos.col};
                    m2.movedPiece = m_board[pos.row][pos.col];
                    m2.isTwoSquarePawnMove = true;
                    moves.append(m2);
                }
            }
        }
    }

    // Diagonal captures
    int captureCols[2] = {pos.col - 1, pos.col + 1};
    for (int c : captureCols) {
        if (c >= 0 && c < 8 && nextRow >= 0 && nextRow < 8) {
            const Piece &target = m_board[nextRow][c];
            if (!target.isEmpty() && target.color == opponentColor(color)) {
                if (nextRow == promoRow) {
                    PieceType pTypes[] = {PieceType::Queen, PieceType::Rook, PieceType::Bishop, PieceType::Knight};
                    for (auto pt : pTypes) {
                        Move m;
                        m.from = pos;
                        m.to = {nextRow, c};
                        m.movedPiece = m_board[pos.row][pos.col];
                        m.capturedPiece = target;
                        m.promotion = pt;
                        moves.append(m);
                    }
                } else {
                    Move m;
                    m.from = pos;
                    m.to = {nextRow, c};
                    m.movedPiece = m_board[pos.row][pos.col];
                    m.capturedPiece = target;
                    moves.append(m);
                }
            } else if (target.isEmpty() && m_enPassantSquare == BoardPos(nextRow, c)) {
                // En Passant
                Move m;
                m.from = pos;
                m.to = {nextRow, c};
                m.movedPiece = m_board[pos.row][pos.col];
                m.capturedPiece = m_board[pos.row][c];
                m.isEnPassant = true;
                moves.append(m);
            }
        }
    }
}

void ChessGame::generateKnightMoves(const BoardPos &pos, PieceColor color, QVector<Move> &moves) const {
    static const int kMoves[8][2] = {
        {-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
        {1, -2},  {1, 2},  {2, -1},  {2, 1}
    };
    for (const auto &km : kMoves) {
        int r = pos.row + km[0];
        int c = pos.col + km[1];
        if (r >= 0 && r < 8 && c >= 0 && c < 8) {
            const Piece &p = m_board[r][c];
            if (p.isEmpty() || p.color == opponentColor(color)) {
                Move m;
                m.from = pos;
                m.to = {r, c};
                m.movedPiece = m_board[pos.row][pos.col];
                m.capturedPiece = p;
                moves.append(m);
            }
        }
    }
}

void ChessGame::generateBishopMoves(const BoardPos &pos, PieceColor color, QVector<Move> &moves) const {
    static const int diagDirs[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
    for (const auto &d : diagDirs) {
        int r = pos.row + d[0];
        int c = pos.col + d[1];
        while (r >= 0 && r < 8 && c >= 0 && c < 8) {
            const Piece &p = m_board[r][c];
            if (p.isEmpty()) {
                Move m;
                m.from = pos;
                m.to = {r, c};
                m.movedPiece = m_board[pos.row][pos.col];
                moves.append(m);
            } else {
                if (p.color == opponentColor(color)) {
                    Move m;
                    m.from = pos;
                    m.to = {r, c};
                    m.movedPiece = m_board[pos.row][pos.col];
                    m.capturedPiece = p;
                    moves.append(m);
                }
                break;
            }
            r += d[0];
            c += d[1];
        }
    }
}

void ChessGame::generateRookMoves(const BoardPos &pos, PieceColor color, QVector<Move> &moves) const {
    static const int orthoDirs[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    for (const auto &d : orthoDirs) {
        int r = pos.row + d[0];
        int c = pos.col + d[1];
        while (r >= 0 && r < 8 && c >= 0 && c < 8) {
            const Piece &p = m_board[r][c];
            if (p.isEmpty()) {
                Move m;
                m.from = pos;
                m.to = {r, c};
                m.movedPiece = m_board[pos.row][pos.col];
                moves.append(m);
            } else {
                if (p.color == opponentColor(color)) {
                    Move m;
                    m.from = pos;
                    m.to = {r, c};
                    m.movedPiece = m_board[pos.row][pos.col];
                    m.capturedPiece = p;
                    moves.append(m);
                }
                break;
            }
            r += d[0];
            c += d[1];
        }
    }
}

void ChessGame::generateQueenMoves(const BoardPos &pos, PieceColor color, QVector<Move> &moves) const {
    generateBishopMoves(pos, color, moves);
    generateRookMoves(pos, color, moves);
}

void ChessGame::generateKingMoves(const BoardPos &pos, PieceColor color, QVector<Move> &moves) const {
    static const int kingDirs[8][2] = {
        {-1, -1}, {-1, 0}, {-1, 1},
        {0, -1},           {0, 1},
        {1, -1},  {1, 0},  {1, 1}
    };
    for (const auto &d : kingDirs) {
        int r = pos.row + d[0];
        int c = pos.col + d[1];
        if (r >= 0 && r < 8 && c >= 0 && c < 8) {
            const Piece &p = m_board[r][c];
            if (p.isEmpty() || p.color == opponentColor(color)) {
                Move m;
                m.from = pos;
                m.to = {r, c};
                m.movedPiece = m_board[pos.row][pos.col];
                m.capturedPiece = p;
                moves.append(m);
            }
        }
    }

    generateCastlingMoves(color, moves);
}

void ChessGame::generateCastlingMoves(PieceColor color, QVector<Move> &moves) const {
    int r = (color == PieceColor::White) ? 7 : 0;
    bool kingMoved = (color == PieceColor::White) ? m_whiteKingMoved : m_blackKingMoved;
    bool ksRookMoved = (color == PieceColor::White) ? m_whiteKingsideRookMoved : m_blackKingsideRookMoved;
    bool qsRookMoved = (color == PieceColor::White) ? m_whiteQueensideRookMoved : m_blackQueensideRookMoved;

    if (kingMoved) return;

    PieceColor opp = opponentColor(color);

    // Cannot castle out of check
    if (isSquareAttacked({r, 4}, opp)) return;

    // Kingside castle
    if (!ksRookMoved) {
        const Piece &rook = m_board[r][7];
        if (rook.type == PieceType::Rook && rook.color == color) {
            if (m_board[r][5].isEmpty() && m_board[r][6].isEmpty()) {
                if (!isSquareAttacked({r, 5}, opp) && !isSquareAttacked({r, 6}, opp)) {
                    Move m;
                    m.from = {r, 4};
                    m.to = {r, 6};
                    m.movedPiece = m_board[r][4];
                    m.isCastling = true;
                    moves.append(m);
                }
            }
        }
    }

    // Queenside castle
    if (!qsRookMoved) {
        const Piece &rook = m_board[r][0];
        if (rook.type == PieceType::Rook && rook.color == color) {
            if (m_board[r][1].isEmpty() && m_board[r][2].isEmpty() && m_board[r][3].isEmpty()) {
                if (!isSquareAttacked({r, 3}, opp) && !isSquareAttacked({r, 2}, opp)) {
                    Move m;
                    m.from = {r, 4};
                    m.to = {r, 2};
                    m.movedPiece = m_board[r][4];
                    m.isCastling = true;
                    moves.append(m);
                }
            }
        }
    }
}

QVector<Move> ChessGame::generatePseudoLegalMoves(PieceColor color) const {
    QVector<Move> moves;
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            const Piece &p = m_board[r][c];
            if (p.color == color) {
                BoardPos pos(r, c);
                switch (p.type) {
                    case PieceType::Pawn:
                        generatePawnMoves(pos, color, moves);
                        break;
                    case PieceType::Knight:
                        generateKnightMoves(pos, color, moves);
                        break;
                    case PieceType::Bishop:
                        generateBishopMoves(pos, color, moves);
                        break;
                    case PieceType::Rook:
                        generateRookMoves(pos, color, moves);
                        break;
                    case PieceType::Queen:
                        generateQueenMoves(pos, color, moves);
                        break;
                    case PieceType::King:
                        generateKingMoves(pos, color, moves);
                        break;
                    default:
                        break;
                }
            }
        }
    }
    return moves;
}

void ChessGame::applyRawMove(const Move &move) {
    m_board[move.to.row][move.to.col] = move.movedPiece;
    m_board[move.from.row][move.from.col] = {PieceType::None, PieceColor::None};

    if (move.promotion != PieceType::None) {
        m_board[move.to.row][move.to.col] = {move.promotion, move.movedPiece.color};
    }

    if (move.isEnPassant) {
        m_board[move.from.row][move.to.col] = {PieceType::None, PieceColor::None};
    }

    if (move.isCastling) {
        if (move.to.col == 6) { // Kingside
            m_board[move.to.row][5] = m_board[move.to.row][7];
            m_board[move.to.row][7] = {PieceType::None, PieceColor::None};
        } else if (move.to.col == 2) { // Queenside
            m_board[move.to.row][3] = m_board[move.to.row][0];
            m_board[move.to.row][0] = {PieceType::None, PieceColor::None};
        }
    }
}

void ChessGame::revertRawMove(const Move &move) {
    m_board[move.from.row][move.from.col] = move.movedPiece;
    m_board[move.to.row][move.to.col] = move.capturedPiece;

    if (move.isEnPassant) {
        m_board[move.to.row][move.to.col] = {PieceType::None, PieceColor::None};
        m_board[move.from.row][move.to.col] = move.capturedPiece;
    }

    if (move.isCastling) {
        if (move.to.col == 6) {
            m_board[move.to.row][7] = m_board[move.to.row][5];
            m_board[move.to.row][5] = {PieceType::None, PieceColor::None};
        } else if (move.to.col == 2) {
            m_board[move.to.row][0] = m_board[move.to.row][3];
            m_board[move.to.row][3] = {PieceType::None, PieceColor::None};
        }
    }
}

QVector<Move> ChessGame::generateLegalMoves(PieceColor color) {
    QVector<Move> pseudo = generatePseudoLegalMoves(color);
    QVector<Move> legal;

    for (const auto &m : pseudo) {
        applyRawMove(m);
        bool inCheck = isKingInCheck(color);
        revertRawMove(m);

        if (!inCheck) {
            legal.append(m);
        }
    }

    return legal;
}

QVector<Move> ChessGame::legalMovesForSquare(const BoardPos &pos) {
    QVector<Move> allLegal = generateLegalMoves(m_turn);
    QVector<Move> filtered;
    for (const auto &m : allLegal) {
        if (m.from == pos) {
            filtered.append(m);
        }
    }
    return filtered;
}

bool ChessGame::makeMove(Move &move) {
    // Record current state in move for undo
    move.prevWhiteKingMoved = m_whiteKingMoved;
    move.prevWhiteKingsideRookMoved = m_whiteKingsideRookMoved;
    move.prevWhiteQueensideRookMoved = m_whiteQueensideRookMoved;

    move.prevBlackKingMoved = m_blackKingMoved;
    move.prevBlackKingsideRookMoved = m_blackKingsideRookMoved;
    move.prevBlackQueensideRookMoved = m_blackQueensideRookMoved;

    move.prevEnPassantSquare = m_enPassantSquare;
    move.prevHalfMoveClock = m_halfMoveClock;

    QVector<Move> legalBefore = generateLegalMoves(m_turn);

    // Apply the move on the board
    applyRawMove(move);

    // Update castling rights
    if (move.movedPiece.type == PieceType::King) {
        if (move.movedPiece.color == PieceColor::White) m_whiteKingMoved = true;
        else m_blackKingMoved = true;
    }
    if (move.from == BoardPos(7, 7)) m_whiteKingsideRookMoved = true;
    if (move.from == BoardPos(7, 0)) m_whiteQueensideRookMoved = true;
    if (move.from == BoardPos(0, 7)) m_blackKingsideRookMoved = true;
    if (move.from == BoardPos(0, 0)) m_blackQueensideRookMoved = true;

    if (move.to == BoardPos(7, 7)) m_whiteKingsideRookMoved = true;
    if (move.to == BoardPos(7, 0)) m_whiteQueensideRookMoved = true;
    if (move.to == BoardPos(0, 7)) m_blackKingsideRookMoved = true;
    if (move.to == BoardPos(0, 0)) m_blackQueensideRookMoved = true;

    // Update en-passant square
    if (move.isTwoSquarePawnMove) {
        m_enPassantSquare = BoardPos((move.from.row + move.to.row) / 2, move.from.col);
    } else {
        m_enPassantSquare = {-1, -1};
    }

    // 50-move clock
    if (move.movedPiece.type == PieceType::Pawn || !move.capturedPiece.isEmpty()) {
        m_halfMoveClock = 0;
    } else {
        m_halfMoveClock++;
    }

    // Track captured pieces
    if (!move.capturedPiece.isEmpty()) {
        if (move.movedPiece.color == PieceColor::White) {
            m_capturedByWhite.append(move.capturedPiece);
        } else {
            m_capturedByBlack.append(move.capturedPiece);
        }
    }

    // Switch turn
    m_turn = opponentColor(m_turn);
    if (m_turn == PieceColor::White) {
        m_fullMoveNumber++;
    }

    m_lastFrom = move.from;
    m_lastTo = move.to;

    // Update status
    updateGameStatus();

    // Compute standard algebraic notation (SAN)
    move.sanNotation = computeSAN(move, legalBefore);

    m_history.append(move);

    emit moveMade(move);
    emit gameStatusChanged(m_status);

    return true;
}

bool ChessGame::undoMove() {
    if (m_history.isEmpty()) return false;

    Move lastMove = m_history.takeLast();

    // Switch turn back
    m_turn = lastMove.movedPiece.color;
    if (m_turn == PieceColor::Black) {
        m_fullMoveNumber--;
    }

    // Revert captured piece tracking
    if (!lastMove.capturedPiece.isEmpty()) {
        if (m_turn == PieceColor::White && !m_capturedByWhite.isEmpty()) {
            m_capturedByWhite.removeLast();
        } else if (m_turn == PieceColor::Black && !m_capturedByBlack.isEmpty()) {
            m_capturedByBlack.removeLast();
        }
    }

    revertRawMove(lastMove);

    // Restore state flags
    m_whiteKingMoved = lastMove.prevWhiteKingMoved;
    m_whiteKingsideRookMoved = lastMove.prevWhiteKingsideRookMoved;
    m_whiteQueensideRookMoved = lastMove.prevWhiteQueensideRookMoved;

    m_blackKingMoved = lastMove.prevBlackKingMoved;
    m_blackKingsideRookMoved = lastMove.prevBlackKingsideRookMoved;
    m_blackQueensideRookMoved = lastMove.prevBlackQueensideRookMoved;

    m_enPassantSquare = lastMove.prevEnPassantSquare;
    m_halfMoveClock = lastMove.prevHalfMoveClock;

    if (!m_history.isEmpty()) {
        m_lastFrom = m_history.last().from;
        m_lastTo = m_history.last().to;
    } else {
        m_lastFrom = {-1, -1};
        m_lastTo = {-1, -1};
    }

    updateGameStatus();

    emit boardReset();
    emit gameStatusChanged(m_status);

    return true;
}

void ChessGame::updateGameStatus() {
    QVector<Move> oppLegal = generateLegalMoves(m_turn);
    bool inCheck = isKingInCheck(m_turn);

    if (oppLegal.isEmpty()) {
        if (inCheck) {
            m_status = GameStatus::Checkmate;
        } else {
            m_status = GameStatus::Stalemate;
        }
    } else if (inCheck) {
        m_status = GameStatus::Check;
    } else if (m_halfMoveClock >= 100) {
        m_status = GameStatus::DrawFiftyMoves;
    } else if (checkInsufficientMaterial()) {
        m_status = GameStatus::DrawInsufficientMaterial;
    } else {
        m_status = GameStatus::Active;
    }
}

bool ChessGame::checkInsufficientMaterial() const {
    int wPawns = 0, bPawns = 0;
    int wRooks = 0, bRooks = 0;
    int wQueens = 0, bQueens = 0;
    int wKnights = 0, bKnights = 0;
    int wBishops = 0, bBishops = 0;
    int wBishopSquareColor = -1;
    int bBishopSquareColor = -1;

    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            const Piece &p = m_board[r][c];
            if (p.isEmpty() || p.type == PieceType::King) continue;

            if (p.color == PieceColor::White) {
                if (p.type == PieceType::Pawn) wPawns++;
                else if (p.type == PieceType::Rook) wRooks++;
                else if (p.type == PieceType::Queen) wQueens++;
                else if (p.type == PieceType::Knight) wKnights++;
                else if (p.type == PieceType::Bishop) {
                    wBishops++;
                    wBishopSquareColor = (r + c) % 2;
                }
            } else {
                if (p.type == PieceType::Pawn) bPawns++;
                else if (p.type == PieceType::Rook) bRooks++;
                else if (p.type == PieceType::Queen) bQueens++;
                else if (p.type == PieceType::Knight) bKnights++;
                else if (p.type == PieceType::Bishop) {
                    bBishops++;
                    bBishopSquareColor = (r + c) % 2;
                }
            }
        }
    }

    if (wPawns > 0 || bPawns > 0 || wRooks > 0 || bRooks > 0 || wQueens > 0 || bQueens > 0) {
        return false;
    }

    // King vs King
    if (wKnights == 0 && wBishops == 0 && bKnights == 0 && bBishops == 0) return true;

    // King + Bishop vs King OR King + Knight vs King
    if ((wKnights + wBishops == 1 && bKnights + bBishops == 0) ||
        (bKnights + bBishops == 1 && wKnights + wBishops == 0)) {
        return true;
    }

    // King + Bishop vs King + Bishop (same color square)
    if (wKnights == 0 && bKnights == 0 && wBishops == 1 && bBishops == 1) {
        if (wBishopSquareColor == bBishopSquareColor) {
            return true;
        }
    }

    return false;
}

QString ChessGame::computeSAN(const Move &move, const QVector<Move> &legalMovesBefore) {
    if (move.isCastling) {
        QString san = (move.to.col == 6) ? "O-O" : "O-O-O";
        if (m_status == GameStatus::Checkmate) san += "#";
        else if (m_status == GameStatus::Check) san += "+";
        return san;
    }

    QString san;
    if (move.movedPiece.type == PieceType::Pawn) {
        if (!move.capturedPiece.isEmpty() || move.isEnPassant) {
            char fileFrom = 'a' + move.from.col;
            san = QString(fileFrom) + "x" + move.to.toAlgebraic();
        } else {
            san = move.to.toAlgebraic();
        }

        if (move.promotion != PieceType::None) {
            san += "=";
            switch (move.promotion) {
                case PieceType::Queen:  san += "Q"; break;
                case PieceType::Rook:   san += "R"; break;
                case PieceType::Bishop: san += "B"; break;
                case PieceType::Knight: san += "N"; break;
                default: break;
            }
        }
    } else {
        char pieceChar = ' ';
        switch (move.movedPiece.type) {
            case PieceType::Knight: pieceChar = 'N'; break;
            case PieceType::Bishop: pieceChar = 'B'; break;
            case PieceType::Rook:   pieceChar = 'R'; break;
            case PieceType::Queen:  pieceChar = 'Q'; break;
            case PieceType::King:   pieceChar = 'K'; break;
            default: break;
        }
        san = QString(pieceChar);

        // Disambiguation
        bool needFile = false;
        bool needRank = false;
        for (const auto &m : legalMovesBefore) {
            if (m.from != move.from && m.to == move.to && m.movedPiece.type == move.movedPiece.type) {
                if (m.from.col != move.from.col) {
                    needFile = true;
                } else if (m.from.row != move.from.row) {
                    needRank = true;
                } else {
                    needFile = true;
                    needRank = true;
                }
            }
        }

        if (needFile) san += QString(static_cast<char>('a' + move.from.col));
        if (needRank) san += QString::number(8 - move.from.row);

        if (!move.capturedPiece.isEmpty()) {
            san += "x";
        }
        san += move.to.toAlgebraic();
    }

    if (m_status == GameStatus::Checkmate) san += "#";
    else if (m_status == GameStatus::Check) san += "+";

    return san;
}

int ChessGame::materialAdvantage(PieceColor color) const {
    int wScore = 0;
    int bScore = 0;
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            const Piece &p = m_board[r][c];
            if (p.isEmpty() || p.type == PieceType::King) continue;
            if (p.color == PieceColor::White) wScore += p.value();
            else bScore += p.value();
        }
    }
    int diff = (color == PieceColor::White) ? (wScore - bScore) : (bScore - wScore);
    return diff / 100; // In pawn units
}

QString ChessGame::exportPGN() const {
    QString pgn;
    pgn += "[Event \"Qt Chess Game\"]\n";
    pgn += "[Site \"Localhost\"]\n";
    pgn += QString("[Date \"%1\"]\n").arg(QDate::currentDate().toString("yyyy.MM.dd"));
    pgn += "[Round \"1\"]\n";
    pgn += "[White \"Player 1\"]\n";
    pgn += "[Black \"Player 2\"]\n";

    QString resultStr = "*";
    if (m_status == GameStatus::Checkmate) {
        resultStr = (m_turn == PieceColor::Black) ? "1-0" : "0-1";
    } else if (m_status == GameStatus::Stalemate ||
               m_status == GameStatus::DrawFiftyMoves ||
               m_status == GameStatus::DrawInsufficientMaterial) {
        resultStr = "1/2-1/2";
    }
    pgn += QString("[Result \"%1\"]\n\n").arg(resultStr);

    int moveNum = 1;
    for (int i = 0; i < m_history.size(); ++i) {
        if (i % 2 == 0) {
            pgn += QString("%1. ").arg(moveNum);
        }
        pgn += m_history[i].sanNotation + " ";
        if (i % 2 == 1) {
            moveNum++;
            if (moveNum % 6 == 0) pgn += "\n";
        }
    }
    pgn += resultStr + "\n";
    return pgn;
}

QString ChessGame::exportFEN() const {
    QString fen;

    // 1. Piece placement (ranks 8 down to 1, i.e. row 0 to 7)
    for (int r = 0; r < 8; ++r) {
        int emptyCount = 0;
        for (int c = 0; c < 8; ++c) {
            const Piece &p = m_board[r][c];
            if (p.isEmpty()) {
                emptyCount++;
            } else {
                if (emptyCount > 0) {
                    fen += QString::number(emptyCount);
                    emptyCount = 0;
                }
                char ch = '?';
                switch (p.type) {
                    case PieceType::Pawn:   ch = 'P'; break;
                    case PieceType::Knight: ch = 'N'; break;
                    case PieceType::Bishop: ch = 'B'; break;
                    case PieceType::Rook:   ch = 'R'; break;
                    case PieceType::Queen:  ch = 'Q'; break;
                    case PieceType::King:   ch = 'K'; break;
                    default: break;
                }
                if (p.color == PieceColor::Black) {
                    ch = static_cast<char>(std::tolower(ch));
                }
                fen += QChar(ch);
            }
        }
        if (emptyCount > 0) {
            fen += QString::number(emptyCount);
        }
        if (r < 7) {
            fen += '/';
        }
    }

    // 2. Active turn
    fen += (m_turn == PieceColor::White) ? " w " : " b ";

    // 3. Castling availability
    QString castling;
    if (!m_whiteKingMoved) {
        if (!m_whiteKingsideRookMoved) castling += "K";
        if (!m_whiteQueensideRookMoved) castling += "Q";
    }
    if (!m_blackKingMoved) {
        if (!m_blackKingsideRookMoved) castling += "k";
        if (!m_blackQueensideRookMoved) castling += "q";
    }
    if (castling.isEmpty()) castling = "-";
    fen += castling + " ";

    // 4. En passant target
    if (m_enPassantSquare.isValid()) {
        fen += m_enPassantSquare.toAlgebraic() + " ";
    } else {
        fen += "- ";
    }

    // 5. Halfmove clock & Fullmove number
    fen += QString("%1 %2").arg(m_halfMoveClock).arg(m_fullMoveNumber);

    return fen;
}

QString ChessGame::lastMoveNotation() const {
    if (m_history.isEmpty()) return QStringLiteral("");
    return m_history.last().sanNotation;
}
