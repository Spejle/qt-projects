#include "mainwindow.h"
#include "piecerenderer.h"
#include <QApplication>
#include <QMenuBar>
#include <QMenu>
#include <QToolBar>
#include <QStatusBar>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>
#include <QClipboard>
#include <QGroupBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_game(new ChessGame(this))
    , m_ai(new ChessAI(this))
    , m_antigravity(new AntigravityEngine(this))
    , m_claudeHaiku(new ClaudeHaikuEngine(this))
    , m_clockTimer(new QTimer(this))
{
    setWindowTitle(tr("Chess Qt"));
    resize(1020, 740);
    setMinimumSize(800, 580);

    setupUi();
    createMenus();

    connect(m_game, &ChessGame::moveMade, this, &MainWindow::onMoveMade);
    connect(m_game, &ChessGame::boardReset, this, &MainWindow::onBoardReset);
    connect(m_game, &ChessGame::gameStatusChanged, this, &MainWindow::onGameStatusChanged);
    connect(m_ai, &ChessAI::bestMoveReady, this, &MainWindow::onAiMoveReady);
    connect(m_antigravity, &AntigravityEngine::moveReady, this, &MainWindow::onAntigravityMoveReady);
    connect(m_claudeHaiku, &ClaudeHaikuEngine::moveReady, this, &MainWindow::onClaudeHaikuMoveReady);

    connect(m_clockTimer, &QTimer::timeout, this, &MainWindow::onClockTick);

    // Initial setup
    m_settings.mode = GameMode::PlayerVsPlayer;
    m_settings.humanColor = PieceColor::White;
    m_settings.difficulty = AIDifficulty::Medium;
    m_settings.theme = BoardTheme::TournamentGreen;
    m_settings.timerMinutes = 0;

    m_boardWidget->setTheme(m_settings.theme);
    m_boardWidget->setGameMode(m_settings.mode);
    m_boardWidget->setHumanColor(m_settings.humanColor);

    updateStatusBanner();
    updateMoveHistoryTable();
    updateCapturedPiecesDisplay();
    updateClocksDisplay();
}

void MainWindow::setupUi() {
    QWidget *central = new QWidget(this);
    QHBoxLayout *rootLayout = new QHBoxLayout(central);
    rootLayout->setContentsMargins(16, 16, 16, 16);
    rootLayout->setSpacing(20);

    // Left: Chess Board Widget
    m_boardWidget = new ChessBoardWidget(m_game, central);
    rootLayout->addWidget(m_boardWidget, 3);

    // Right: Sidebar / Game Controls Panel
    QWidget *sidePanel = new QWidget(central);
    sidePanel->setFixedWidth(320);
    QVBoxLayout *sideLayout = new QVBoxLayout(sidePanel);
    sideLayout->setContentsMargins(0, 0, 0, 0);
    sideLayout->setSpacing(12);

    // 1. Top Player Card (Black by default)
    QGroupBox *topBox = new QGroupBox(tr("Opponent"), sidePanel);
    QVBoxLayout *topLayout = new QVBoxLayout(topBox);
    topLayout->setContentsMargins(10, 8, 10, 8);
    topLayout->setSpacing(4);

    QHBoxLayout *topHeader = new QHBoxLayout();
    m_topPlayerName = new QLabel(tr("Black"), topBox);
    QFont nameFont = m_topPlayerName->font();
    nameFont.setBold(true);
    m_topPlayerName->setFont(nameFont);

    m_topPlayerClock = new QLabel(tr("--:--"), topBox);
    QFont clockFont("Monospace", 12, QFont::Bold);
    m_topPlayerClock->setFont(clockFont);
    m_topPlayerClock->setStyleSheet("color: #0969DA; background: #E7F3FF; padding: 2px 6px; border-radius: 4px;");

    topHeader->addWidget(m_topPlayerName);
    topHeader->addStretch();
    topHeader->addWidget(m_topPlayerClock);
    topLayout->addLayout(topHeader);

    QHBoxLayout *topCapLayout = new QHBoxLayout();
    m_topCapturedLabel = new QLabel(topBox);
    m_topCapturedLabel->setMinimumHeight(24);
    m_topAdvantageLabel = new QLabel(topBox);
    m_topAdvantageLabel->setStyleSheet("color: #2DA44E; font-weight: bold;");
    topCapLayout->addWidget(m_topCapturedLabel);
    topCapLayout->addStretch();
    topCapLayout->addWidget(m_topAdvantageLabel);
    topLayout->addLayout(topCapLayout);

    sideLayout->addWidget(topBox);

    // 2. Status Banner
    QWidget *statusCard = new QWidget(sidePanel);
    statusCard->setStyleSheet("background: #F6F8FA; border: 1px solid #D0D7DE; border-radius: 6px; padding: 6px;");
    QHBoxLayout *statusCardLayout = new QHBoxLayout(statusCard);
    statusCardLayout->setContentsMargins(8, 4, 8, 4);

    m_statusBadge = new QLabel(statusCard);
    m_statusBadge->setFixedSize(14, 14);
    m_statusBadge->setStyleSheet("background: #FFFFFF; border: 2px solid #57606A; border-radius: 7px;");

    m_statusLabel = new QLabel(tr("White's Turn"), statusCard);
    QFont stFont = m_statusLabel->font();
    stFont.setBold(true);
    stFont.setPointSize(10);
    m_statusLabel->setFont(stFont);

    statusCardLayout->addWidget(m_statusBadge);
    statusCardLayout->addWidget(m_statusLabel);
    statusCardLayout->addStretch();
    sideLayout->addWidget(statusCard);

    // Commentary Card (Antigravity's Thoughts)
    QWidget *commentaryCard = new QWidget(sidePanel);
    commentaryCard->setStyleSheet("background: #F0F6FF; border: 1px solid #C8E1FF; border-radius: 6px; padding: 6px;");
    QVBoxLayout *commLayout = new QVBoxLayout(commentaryCard);
    commLayout->setContentsMargins(6, 4, 6, 4);
    m_commentaryLabel = new QLabel(tr("💬 Ready to play!"), commentaryCard);
    m_commentaryLabel->setWordWrap(true);
    QFont commFont = m_commentaryLabel->font();
    commFont.setPointSize(9);
    commFont.setItalic(true);
    m_commentaryLabel->setFont(commFont);
    m_commentaryLabel->setStyleSheet("color: #0969DA;");
    commLayout->addWidget(m_commentaryLabel);
    sideLayout->addWidget(commentaryCard);

    // 3. Move History Table
    m_historyTable = new QTableWidget(0, 3, sidePanel);
    m_historyTable->setHorizontalHeaderLabels({tr("#"), tr("White"), tr("Black")});
    m_historyTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    m_historyTable->setColumnWidth(0, 36);
    m_historyTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_historyTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_historyTable->verticalHeader()->setVisible(false);
    m_historyTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_historyTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_historyTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_historyTable->setStyleSheet(
        "QTableWidget { background: #FFFFFF; border: 1px solid #D0D7DE; border-radius: 6px; }"
        "QHeaderView::section { background: #F6F8FA; border: none; font-weight: bold; padding: 4px; }"
    );
    sideLayout->addWidget(m_historyTable, 1);

    // 4. Bottom Player Card (White by default)
    QGroupBox *botBox = new QGroupBox(tr("Player"), sidePanel);
    QVBoxLayout *botLayout = new QVBoxLayout(botBox);
    botLayout->setContentsMargins(10, 8, 10, 8);
    botLayout->setSpacing(4);

    QHBoxLayout *botHeader = new QHBoxLayout();
    m_bottomPlayerName = new QLabel(tr("White"), botBox);
    m_bottomPlayerName->setFont(nameFont);

    m_bottomPlayerClock = new QLabel(tr("--:--"), botBox);
    m_bottomPlayerClock->setFont(clockFont);
    m_bottomPlayerClock->setStyleSheet("color: #0969DA; background: #E7F3FF; padding: 2px 6px; border-radius: 4px;");

    botHeader->addWidget(m_bottomPlayerName);
    botHeader->addStretch();
    botHeader->addWidget(m_bottomPlayerClock);
    botLayout->addLayout(botHeader);

    QHBoxLayout *botCapLayout = new QHBoxLayout();
    m_bottomCapturedLabel = new QLabel(botBox);
    m_bottomCapturedLabel->setMinimumHeight(24);
    m_bottomAdvantageLabel = new QLabel(botBox);
    m_bottomAdvantageLabel->setStyleSheet("color: #2DA44E; font-weight: bold;");
    botCapLayout->addWidget(m_bottomCapturedLabel);
    botCapLayout->addStretch();
    botCapLayout->addWidget(m_bottomAdvantageLabel);
    botLayout->addLayout(botCapLayout);

    sideLayout->addWidget(botBox);

    // 5. Action Buttons Grid
    QHBoxLayout *row1 = new QHBoxLayout();
    m_newGameBtn = new QPushButton(tr("New Game"), sidePanel);
    m_undoBtn = new QPushButton(tr("Undo"), sidePanel);
    m_flipBtn = new QPushButton(tr("Flip Board"), sidePanel);
    row1->addWidget(m_newGameBtn);
    row1->addWidget(m_undoBtn);
    row1->addWidget(m_flipBtn);
    sideLayout->addLayout(row1);

    QHBoxLayout *row2 = new QHBoxLayout();
    m_resignBtn = new QPushButton(tr("Resign"), sidePanel);
    QPushButton *pgnBtn = new QPushButton(tr("Export PGN"), sidePanel);
    row2->addWidget(m_resignBtn);
    row2->addWidget(pgnBtn);
    sideLayout->addLayout(row2);

    // Button Styling
    QString btnStyle =
        "QPushButton { background: #FFFFFF; border: 1px solid #D0D7DE; border-radius: 6px; padding: 6px 12px; font-weight: 500; }"
        "QPushButton:hover { background: #F3F4F6; border-color: #1A7F37; }"
        "QPushButton:pressed { background: #E5E7EB; }";
    m_newGameBtn->setStyleSheet("QPushButton { background: #2DA44E; color: white; border: 1px solid #2C974B; border-radius: 6px; padding: 6px 12px; font-weight: bold; }"
                                "QPushButton:hover { background: #2C974B; }"
                                "QPushButton:pressed { background: #298E46; }");
    m_undoBtn->setStyleSheet(btnStyle);
    m_flipBtn->setStyleSheet(btnStyle);
    m_resignBtn->setStyleSheet("QPushButton { background: #FFFFFF; color: #CF222E; border: 1px solid #D0D7DE; border-radius: 6px; padding: 6px 12px; font-weight: 500; }"
                               "QPushButton:hover { background: #FFEBE9; border-color: #CF222E; }");
    pgnBtn->setStyleSheet(btnStyle);

    // Connect Actions
    connect(m_newGameBtn, &QPushButton::clicked, this, &MainWindow::onNewGameClicked);
    connect(m_undoBtn, &QPushButton::clicked, this, &MainWindow::onUndoClicked);
    connect(m_flipBtn, &QPushButton::clicked, this, &MainWindow::onFlipClicked);
    connect(m_resignBtn, &QPushButton::clicked, this, &MainWindow::onResignClicked);
    connect(pgnBtn, &QPushButton::clicked, this, &MainWindow::onExportPgnClicked);

    rootLayout->addWidget(sidePanel);
    setCentralWidget(central);
}

void MainWindow::createMenus() {
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QAction *newAct = fileMenu->addAction(tr("&New Game..."), QKeySequence::New, this, &MainWindow::onNewGameClicked);
    QAction *pgnAct = fileMenu->addAction(tr("&Export PGN..."), QKeySequence::Save, this, &MainWindow::onExportPgnClicked);
    fileMenu->addSeparator();
    QAction *quitAct = fileMenu->addAction(tr("&Quit"), QKeySequence::Quit, qApp, &QApplication::quit);

    Q_UNUSED(newAct);
    Q_UNUSED(pgnAct);
    Q_UNUSED(quitAct);

    QMenu *gameMenu = menuBar()->addMenu(tr("&Game"));
    gameMenu->addAction(tr("&Undo Move"), QKeySequence::Undo, this, &MainWindow::onUndoClicked);
    gameMenu->addAction(tr("&Flip Board"), QKeySequence(Qt::Key_F), this, &MainWindow::onFlipClicked);
    gameMenu->addAction(tr("&Resign"), this, &MainWindow::onResignClicked);

    QMenu *viewMenu = menuBar()->addMenu(tr("&Theme"));
    auto addThemeAction = [this, viewMenu](const QString &name, BoardTheme theme) {
        QAction *act = viewMenu->addAction(name);
        connect(act, &QAction::triggered, this, [this, theme]() {
            m_settings.theme = theme;
            m_boardWidget->setTheme(theme);
        });
    };
    addThemeAction(tr("Tournament Green"), BoardTheme::TournamentGreen);
    addThemeAction(tr("Classic Wood"), BoardTheme::ClassicWood);
    addThemeAction(tr("Modern Slate"), BoardTheme::ModernSlate);
    addThemeAction(tr("Dark Glass"), BoardTheme::DarkGlass);

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(tr("&About Chess Qt"), this, [this]() {
        QMessageBox::about(this, tr("About Chess Qt"),
            tr("<h3>Chess Qt</h3>"
               "<p>A modern, full-featured Chess application written in C++ with Qt 6.</p>"
               "<p><b>Features:</b></p>"
               "<ul>"
               "<li>Complete chess rule engine (Castling, En Passant, Pawn Promotion, 50-move rule, Insufficient material).</li>"
               "<li>Minimax AI with Alpha-Beta pruning and Piece-Square evaluation.</li>"
               "<li>High-DPI vector piece graphics & customizable themes.</li>"
               "<li>Algebraic notation move history, clocks, and PGN export.</li>"
               "</ul>"));
    });
}

void MainWindow::onNewGameClicked() {
    NewGameDialog dlg(m_settings, this);
    if (dlg.exec() == QDialog::Accepted) {
        m_settings = dlg.settings();
        m_game->reset();

        m_boardWidget->setTheme(m_settings.theme);
        m_boardWidget->setGameMode(m_settings.mode);
        m_boardWidget->setHumanColor(m_settings.humanColor);

        bool isAiMode = (m_settings.mode == GameMode::PlayerVsAI ||
                         m_settings.mode == GameMode::PlayerVsAntigravity ||
                         m_settings.mode == GameMode::PlayerVsClaudeHaiku);
        bool flip = (isAiMode && m_settings.humanColor == PieceColor::Black);
        m_boardWidget->setFlipped(flip);

        // Clocks
        if (m_settings.timerMinutes > 0) {
            m_whiteSeconds = m_settings.timerMinutes * 60;
            m_blackSeconds = m_settings.timerMinutes * 60;
            startClocks();
        } else {
            stopClocks();
            m_topPlayerClock->setText(tr("--:--"));
            m_bottomPlayerClock->setText(tr("--:--"));
        }

        // Names
        if (m_settings.mode == GameMode::PlayerVsClaudeHaiku) {
            if (m_settings.humanColor == PieceColor::White) {
                m_bottomPlayerName->setText(tr("You (White)"));
                m_topPlayerName->setText(tr("Claude Haiku ⚡ (High Effort)"));
            } else {
                m_bottomPlayerName->setText(tr("Claude Haiku ⚡ (High Effort)"));
                m_topPlayerName->setText(tr("You (Black)"));
            }
            m_commentaryLabel->setText(tr("💬 Claude Haiku: High effort mode engaged. Make your opening move!"));
        } else if (m_settings.mode == GameMode::PlayerVsAntigravity) {
            if (m_settings.humanColor == PieceColor::White) {
                m_bottomPlayerName->setText(tr("You (White)"));
                m_topPlayerName->setText(tr("Antigravity 🚀 (AI)"));
            } else {
                m_bottomPlayerName->setText(tr("Antigravity 🚀 (AI)"));
                m_topPlayerName->setText(tr("You (Black)"));
            }
            m_commentaryLabel->setText(tr("💬 Antigravity: Game started! I'm ready. Make your opening move."));
        } else if (m_settings.mode == GameMode::PlayerVsAI) {
            if (m_settings.humanColor == PieceColor::White) {
                m_bottomPlayerName->setText(tr("You (White)"));
                m_topPlayerName->setText(tr("Computer (AI)"));
            } else {
                m_bottomPlayerName->setText(tr("Computer (AI)"));
                m_topPlayerName->setText(tr("You (Black)"));
            }
            m_commentaryLabel->setText(tr("💬 Ready to play against local AI engine."));
        } else {
            m_bottomPlayerName->setText(tr("White"));
            m_topPlayerName->setText(tr("Black"));
            m_commentaryLabel->setText(tr("💬 Pass & Play mode: White to move."));
        }

        updateStatusBanner();
        updateMoveHistoryTable();
        updateCapturedPiecesDisplay();

        // If AI is White, AI moves first
        if (m_settings.humanColor == PieceColor::Black) {
            if (m_settings.mode == GameMode::PlayerVsClaudeHaiku) {
                m_boardWidget->setInteractive(false);
                m_statusLabel->setText(tr("Claude Haiku is thinking (High Effort)... ⚡"));
                m_commentaryLabel->setText(tr("💬 Claude Haiku: Analyzing opening lines with maximum depth..."));
                m_claudeHaiku->requestMove(m_game, PieceColor::White);
            } else if (m_settings.mode == GameMode::PlayerVsAI) {
                m_boardWidget->setInteractive(false);
                m_statusLabel->setText(tr("Computer is thinking..."));
                m_ai->findBestMoveAsync(m_game, PieceColor::White, m_settings.difficulty);
            } else if (m_settings.mode == GameMode::PlayerVsAntigravity) {
                m_boardWidget->setInteractive(false);
                m_statusLabel->setText(tr("Antigravity is thinking... 🚀"));
                m_commentaryLabel->setText(tr("💬 Antigravity: Analyzing opening choices as White..."));
                m_antigravity->requestMove(m_game, PieceColor::White);
            }
        }
    }
}

void MainWindow::onUndoClicked() {
    if (m_settings.mode == GameMode::PlayerVsAI ||
        m_settings.mode == GameMode::PlayerVsAntigravity ||
        m_settings.mode == GameMode::PlayerVsClaudeHaiku) {
        // In AI mode, undo 2 moves (AI move and player move)
        m_game->undoMove();
        m_game->undoMove();
    } else {
        m_game->undoMove();
    }
    m_boardWidget->setInteractive(true);
}

void MainWindow::onFlipClicked() {
    m_boardWidget->toggleFlip();
}

void MainWindow::onResignClicked() {
    if (m_game->status() != GameStatus::Active && m_game->status() != GameStatus::Check) return;

    QString player = (m_game->currentTurn() == PieceColor::White) ? tr("White") : tr("Black");
    auto reply = QMessageBox::question(this, tr("Resign"),
        tr("Are you sure %1 wants to resign?").arg(player),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        m_game->setStatus(GameStatus::Resignation);
        stopClocks();
        updateStatusBanner();
    }
}

void MainWindow::onExportPgnClicked() {
    QString pgn = m_game->exportPGN();
    QClipboard *cb = QApplication::clipboard();
    cb->setText(pgn);

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save PGN File"),
        "game.pgn", tr("Portable Game Notation (*.pgn)"));
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << pgn;
            file.close();
            QMessageBox::information(this, tr("Export Successful"),
                tr("PGN saved to %1 and copied to clipboard!").arg(fileName));
            return;
        }
    }
    QMessageBox::information(this, tr("PGN Copied"),
        tr("Game PGN has been copied to your clipboard."));
}

void MainWindow::onThemeChanged(int index) {
    BoardTheme t = static_cast<BoardTheme>(index);
    m_settings.theme = t;
    m_boardWidget->setTheme(t);
}

void MainWindow::onMoveMade(const Move &) {
    updateStatusBanner();
    updateMoveHistoryTable();
    updateCapturedPiecesDisplay();

    // Check if AI should respond
    if ((m_game->status() == GameStatus::Active || m_game->status() == GameStatus::Check)) {
        PieceColor opp = opponentColor(m_settings.humanColor);
        if (m_game->currentTurn() == opp) {
            if (m_settings.mode == GameMode::PlayerVsClaudeHaiku) {
                m_boardWidget->setInteractive(false);
                m_statusLabel->setText(tr("Claude Haiku is thinking (High Effort)... ⚡"));
                m_commentaryLabel->setText(tr("💬 Claude Haiku: Calculating optimal strategic moves with deep reasoning..."));
                m_claudeHaiku->requestMove(m_game, opp);
            } else if (m_settings.mode == GameMode::PlayerVsAI) {
                m_boardWidget->setInteractive(false);
                m_statusLabel->setText(tr("Computer is thinking..."));
                m_ai->findBestMoveAsync(m_game, opp, m_settings.difficulty);
            } else if (m_settings.mode == GameMode::PlayerVsAntigravity) {
                m_boardWidget->setInteractive(false);
                m_statusLabel->setText(tr("Antigravity is thinking... 🚀"));
                m_commentaryLabel->setText(tr("💬 Antigravity: Calculating candidate moves..."));
                m_antigravity->requestMove(m_game, opp);
            }
        } else {
            m_boardWidget->setInteractive(true);
        }
    }
}

void MainWindow::onAiMoveReady(const Move &move) {
    if (move.from.isValid() && move.to.isValid()) {
        Move m = move;
        m_game->makeMove(m);
    }
    m_boardWidget->setInteractive(true);
    updateStatusBanner();
}

void MainWindow::onAntigravityMoveReady(const Move &move, const QString &commentary) {
    if (move.from.isValid() && move.to.isValid()) {
        Move m = move;
        m.commentary = commentary;
        m_game->makeMove(m);
        m_commentaryLabel->setText(commentary.isEmpty()
            ? tr("💬 Antigravity: Played %1.").arg(m.sanNotation)
            : QString("💬 %1").arg(commentary));
    }
    m_boardWidget->setInteractive(true);
    updateStatusBanner();
}

void MainWindow::onClaudeHaikuMoveReady(const Move &move, const QString &commentary) {
    if (move.from.isValid() && move.to.isValid()) {
        Move m = move;
        m.commentary = commentary;
        m_game->makeMove(m);
        m_commentaryLabel->setText(commentary.isEmpty()
            ? tr("💬 Claude Haiku: Played %1.").arg(m.sanNotation)
            : QString("💬 %1").arg(commentary));
    }
    m_boardWidget->setInteractive(true);
    updateStatusBanner();
}

void MainWindow::onBoardReset() {
    updateStatusBanner();
    updateMoveHistoryTable();
    updateCapturedPiecesDisplay();
}

void MainWindow::onGameStatusChanged(GameStatus) {
    updateStatusBanner();
}

void MainWindow::updateStatusBanner() {
    GameStatus st = m_game->status();
    PieceColor turn = m_game->currentTurn();

    if (turn == PieceColor::White) {
        m_statusBadge->setStyleSheet("background: #FFFFFF; border: 2px solid #57606A; border-radius: 7px;");
    } else {
        m_statusBadge->setStyleSheet("background: #24292F; border: 2px solid #24292F; border-radius: 7px;");
    }

    switch (st) {
        case GameStatus::Active:
            m_statusLabel->setText(turn == PieceColor::White ? tr("White's Turn") : tr("Black's Turn"));
            break;
        case GameStatus::Check:
            m_statusLabel->setText(turn == PieceColor::White ? tr("White is in Check!") : tr("Black is in Check!"));
            break;
        case GameStatus::Checkmate:
            stopClocks();
            m_statusLabel->setText(turn == PieceColor::White ? tr("Checkmate! Black wins!") : tr("Checkmate! White wins!"));
            QMessageBox::information(this, tr("Game Over"), m_statusLabel->text());
            break;
        case GameStatus::Stalemate:
            stopClocks();
            m_statusLabel->setText(tr("Draw by Stalemate!"));
            QMessageBox::information(this, tr("Game Over"), m_statusLabel->text());
            break;
        case GameStatus::DrawFiftyMoves:
            stopClocks();
            m_statusLabel->setText(tr("Draw by 50-move rule!"));
            break;
        case GameStatus::DrawInsufficientMaterial:
            stopClocks();
            m_statusLabel->setText(tr("Draw by insufficient material!"));
            break;
        case GameStatus::Resignation:
            stopClocks();
            m_statusLabel->setText(turn == PieceColor::White ? tr("White resigned. Black wins!") : tr("Black resigned. White wins!"));
            break;
    }
}

void MainWindow::updateMoveHistoryTable() {
    const auto &hist = m_game->moveHistory();
    m_historyTable->setRowCount(0);

    int row = 0;
    for (int i = 0; i < hist.size(); ++i) {
        if (i % 2 == 0) {
            row = m_historyTable->rowCount();
            m_historyTable->insertRow(row);
            int moveNum = (i / 2) + 1;
            m_historyTable->setItem(row, 0, new QTableWidgetItem(QString::number(moveNum) + "."));
            m_historyTable->setItem(row, 1, new QTableWidgetItem(hist[i].sanNotation));
        } else {
            m_historyTable->setItem(row, 2, new QTableWidgetItem(hist[i].sanNotation));
        }
    }
    m_historyTable->scrollToBottom();
}

void MainWindow::updateCapturedPiecesDisplay() {
    auto buildCapturedIcons = [](const QVector<Piece> &captured) -> QString {
        QString s;
        for (const auto &p : captured) {
            switch (p.type) {
                case PieceType::Pawn:   s += "♙"; break;
                case PieceType::Knight: s += "♘"; break;
                case PieceType::Bishop: s += "♗"; break;
                case PieceType::Rook:   s += "♖"; break;
                case PieceType::Queen:  s += "♕"; break;
                default: break;
            }
        }
        return s;
    };

    // Captured by White (Black pieces taken)
    QString capByWhite = buildCapturedIcons(m_game->capturedByWhite());
    // Captured by Black (White pieces taken)
    QString capByBlack = buildCapturedIcons(m_game->capturedByBlack());

    int wAdv = m_game->materialAdvantage(PieceColor::White);
    int bAdv = m_game->materialAdvantage(PieceColor::Black);

    m_bottomCapturedLabel->setText(capByWhite);
    m_topCapturedLabel->setText(capByBlack);

    m_bottomAdvantageLabel->setText(wAdv > 0 ? QString("+%1").arg(wAdv) : "");
    m_topAdvantageLabel->setText(bAdv > 0 ? QString("+%1").arg(bAdv) : "");
}

void MainWindow::startClocks() {
    m_clockRunning = true;
    m_clockTimer->start(1000);
    updateClocksDisplay();
}

void MainWindow::stopClocks() {
    m_clockRunning = false;
    m_clockTimer->stop();
}

void MainWindow::onClockTick() {
    if (!m_clockRunning) return;

    if (m_game->currentTurn() == PieceColor::White) {
        if (m_whiteSeconds > 0) {
            m_whiteSeconds--;
            if (m_whiteSeconds == 0) {
                stopClocks();
                m_statusLabel->setText(tr("White ran out of time! Black wins!"));
                QMessageBox::information(this, tr("Time Out"), m_statusLabel->text());
            }
        }
    } else {
        if (m_blackSeconds > 0) {
            m_blackSeconds--;
            if (m_blackSeconds == 0) {
                stopClocks();
                m_statusLabel->setText(tr("Black ran out of time! White wins!"));
                QMessageBox::information(this, tr("Time Out"), m_statusLabel->text());
            }
        }
    }
    updateClocksDisplay();
}

void MainWindow::updateClocksDisplay() {
    if (m_settings.timerMinutes <= 0) return;

    auto formatTime = [](int totalSecs) -> QString {
        int m = totalSecs / 60;
        int s = totalSecs % 60;
        return QString("%1:%2").arg(m, 2, 10, QChar('0')).arg(s, 2, 10, QChar('0'));
    };

    m_bottomPlayerClock->setText(formatTime(m_whiteSeconds));
    m_topPlayerClock->setText(formatTime(m_blackSeconds));
}
