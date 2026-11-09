#include <QTest>
#include "../src/chessgame.h"
#include "../src/chessai.h"
#include "../src/mainwindow.h"

class TestChess : public QObject {
    Q_OBJECT

private slots:
    void testInitialPosition();
    void testPawnMoveAndDoubleMove();
    void testFoolsMateCheckmate();
    void testEnPassant();
    void testCastling();
    void testUndo();
    void testAIResponse();
    void testRenderScreenshot();
};

void TestChess::testInitialPosition() {
    ChessGame game;
    QCOMPARE(game.currentTurn(), PieceColor::White);
    QCOMPARE(game.status(), GameStatus::Active);

    // Initial legal moves for White: 16 pawn moves + 4 knight moves = 20 moves
    QVector<Move> moves = game.generateLegalMoves(PieceColor::White);
    QCOMPARE(moves.size(), 20);
}

void TestChess::testPawnMoveAndDoubleMove() {
    ChessGame game;
    // e2 to e4
    BoardPos e2(6, 4);
    BoardPos e4(4, 4);
    auto legalE2 = game.legalMovesForSquare(e2);
    QCOMPARE(legalE2.size(), 2); // e3 and e4

    Move e2e4;
    for (const auto &m : legalE2) {
        if (m.to == e4) e2e4 = m;
    }
    QVERIFY(e2e4.from.isValid());
    QVERIFY(e2e4.isTwoSquarePawnMove);

    QVERIFY(game.makeMove(e2e4));
    QCOMPARE(game.currentTurn(), PieceColor::Black);
    QCOMPARE(game.pieceAt(e4).type, PieceType::Pawn);
    QCOMPARE(game.pieceAt(e2).type, PieceType::None);
}

void TestChess::testFoolsMateCheckmate() {
    ChessGame game;
    // 1. f3 e5 2. g4 Qh4# (Fool's Mate)
    // 1. f3 (f2-f3: (6, 5) to (5, 5))
    Move f3 = game.legalMovesForSquare({6, 5})[0];
    for (const auto &m : game.legalMovesForSquare({6, 5})) {
        if (m.to == BoardPos(5, 5)) f3 = m;
    }
    game.makeMove(f3);

    // 1... e5 (e7-e5: (1, 4) to (3, 4))
    Move e5;
    for (const auto &m : game.legalMovesForSquare({1, 4})) {
        if (m.to == BoardPos(3, 4)) e5 = m;
    }
    game.makeMove(e5);

    // 2. g4 (g2-g4: (6, 6) to (4, 6))
    Move g4;
    for (const auto &m : game.legalMovesForSquare({6, 6})) {
        if (m.to == BoardPos(4, 6)) g4 = m;
    }
    game.makeMove(g4);

    // 2... Qh4# ((0, 3) to (4, 7))
    Move qh4;
    for (const auto &m : game.legalMovesForSquare({0, 3})) {
        if (m.to == BoardPos(4, 7)) qh4 = m;
    }
    QVERIFY(qh4.from.isValid());
    game.makeMove(qh4);

    QCOMPARE(game.status(), GameStatus::Checkmate);
    QCOMPARE(game.generateLegalMoves(PieceColor::White).size(), 0);
}

void TestChess::testEnPassant() {
    ChessGame game;
    // 1. e4 a6 2. e5 d5 3. exd6 (en passant!)
    // 1. e4
    Move m1;
    for (const auto &m : game.legalMovesForSquare({6, 4})) if (m.to == BoardPos(4, 4)) m1 = m;
    game.makeMove(m1);

    // 1... a6
    Move m2;
    for (const auto &m : game.legalMovesForSquare({1, 0})) if (m.to == BoardPos(2, 0)) m2 = m;
    game.makeMove(m2);

    // 2. e5
    Move m3;
    for (const auto &m : game.legalMovesForSquare({4, 4})) if (m.to == BoardPos(3, 4)) m3 = m;
    game.makeMove(m3);

    // 2... d5 (two square pawn move by Black to (3, 3))
    Move m4;
    for (const auto &m : game.legalMovesForSquare({1, 3})) if (m.to == BoardPos(3, 3)) m4 = m;
    game.makeMove(m4);

    // 3. exd6 (White pawn at (3, 4) captures to (2, 3) en-passant)
    auto e5Moves = game.legalMovesForSquare({3, 4});
    Move epMove;
    for (const auto &m : e5Moves) {
        if (m.to == BoardPos(2, 3) && m.isEnPassant) {
            epMove = m;
        }
    }
    QVERIFY(epMove.from.isValid());
    QVERIFY(epMove.isEnPassant);

    game.makeMove(epMove);
    // Pawn at (3, 3) should be captured
    QCOMPARE(game.pieceAt(3, 3).type, PieceType::None);
    // White pawn at (2, 3)
    QCOMPARE(game.pieceAt(2, 3).type, PieceType::Pawn);
    QCOMPARE(game.capturedByWhite().size(), 1);
}

void TestChess::testCastling() {
    ChessGame game;
    // Clear path for White Kingside castle: 1. e4 e5 2. Nf3 Nc6 3. Bc4 Bc5 4. O-O
    Move m;
    // 1. e4
    for (const auto &cand : game.legalMovesForSquare({6, 4})) if (cand.to == BoardPos(4, 4)) m = cand;
    game.makeMove(m);

    // 1... e5
    for (const auto &cand : game.legalMovesForSquare({1, 4})) if (cand.to == BoardPos(3, 4)) m = cand;
    game.makeMove(m);

    // 2. Nf3
    for (const auto &cand : game.legalMovesForSquare({7, 6})) if (cand.to == BoardPos(5, 5)) m = cand;
    game.makeMove(m);

    // 2... Nc6
    for (const auto &cand : game.legalMovesForSquare({0, 1})) if (cand.to == BoardPos(2, 2)) m = cand;
    game.makeMove(m);

    // 3. Bc4
    for (const auto &cand : game.legalMovesForSquare({7, 5})) if (cand.to == BoardPos(4, 2)) m = cand;
    game.makeMove(m);

    // 3... Bc5
    for (const auto &cand : game.legalMovesForSquare({0, 5})) if (cand.to == BoardPos(3, 2)) m = cand;
    game.makeMove(m);

    // 4. White King castling move to (7, 6)
    auto kingMoves = game.legalMovesForSquare({7, 4});
    Move castleMove;
    for (const auto &cand : kingMoves) {
        if (cand.to == BoardPos(7, 6) && cand.isCastling) {
            castleMove = cand;
        }
    }
    QVERIFY(castleMove.from.isValid());
    QVERIFY(castleMove.isCastling);

    game.makeMove(castleMove);
    // King at (7, 6), Rook at (7, 5)
    QCOMPARE(game.pieceAt(7, 6).type, PieceType::King);
    QCOMPARE(game.pieceAt(7, 5).type, PieceType::Rook);
    QCOMPARE(game.pieceAt(7, 7).type, PieceType::None);
    QCOMPARE(game.pieceAt(7, 4).type, PieceType::None);
}

void TestChess::testUndo() {
    ChessGame game;
    Move m;
    for (const auto &cand : game.legalMovesForSquare({6, 4})) if (cand.to == BoardPos(4, 4)) m = cand;
    game.makeMove(m);

    QCOMPARE(game.pieceAt(4, 4).type, PieceType::Pawn);
    QCOMPARE(game.currentTurn(), PieceColor::Black);

    QVERIFY(game.undoMove());
    QCOMPARE(game.pieceAt(4, 4).type, PieceType::None);
    QCOMPARE(game.pieceAt(6, 4).type, PieceType::Pawn);
    QCOMPARE(game.currentTurn(), PieceColor::White);
}

void TestChess::testAIResponse() {
    ChessGame game;
    ChessAI ai;
    Move aiMove = ai.findBestMove(&game, PieceColor::White, AIDifficulty::Easy);
    QVERIFY(aiMove.from.isValid());
    QVERIFY(aiMove.to.isValid());
}

void TestChess::testRenderScreenshot() {
    MainWindow window;
    window.resize(960, 680);
    window.show();
    QPixmap pixmap(window.size());
    pixmap.fill(Qt::white);
    window.render(&pixmap);
    bool saved = pixmap.save("/home/spejle/Chess/Chess.png", "PNG");
    QVERIFY(saved);
}

QTEST_MAIN(TestChess)
#include "test_chess.moc"
