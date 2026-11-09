#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QComboBox>
#include <QTimer>
#include <QElapsedTimer>
#include "types.h"
#include "chessgame.h"
#include "chessboardwidget.h"
#include "chessai.h"
#include "antigravityengine.h"
#include "claudehaikuengine.h"
#include "newgamedialog.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void onNewGameClicked();
    void onUndoClicked();
    void onFlipClicked();
    void onResignClicked();
    void onExportPgnClicked();
    void onThemeChanged(int index);

    void onMoveMade(const Move &move);
    void onBoardReset();
    void onGameStatusChanged(GameStatus status);
    void onAiMoveReady(const Move &move);
    void onAntigravityMoveReady(const Move &move, const QString &commentary);
    void onClaudeHaikuMoveReady(const Move &move, const QString &commentary);

    void onClockTick();

private:
    void setupUi();
    void createMenus();
    void updateStatusBanner();
    void updateMoveHistoryTable();
    void updateCapturedPiecesDisplay();
    void updateClocksDisplay();

    void startClocks();
    void stopClocks();

    ChessGame *m_game;
    ChessAI *m_ai;
    AntigravityEngine *m_antigravity;
    ClaudeHaikuEngine *m_claudeHaiku;
    ChessBoardWidget *m_boardWidget;

    GameSettings m_settings;

    // UI Widgets
    QLabel *m_statusLabel;
    QLabel *m_statusBadge;
    QLabel *m_commentaryLabel;

    QLabel *m_topPlayerName;
    QLabel *m_topPlayerClock;
    QLabel *m_topCapturedLabel;
    QLabel *m_topAdvantageLabel;

    QLabel *m_bottomPlayerName;
    QLabel *m_bottomPlayerClock;
    QLabel *m_bottomCapturedLabel;
    QLabel *m_bottomAdvantageLabel;

    QTableWidget *m_historyTable;
    QPushButton *m_undoBtn;
    QPushButton *m_flipBtn;
    QPushButton *m_newGameBtn;
    QPushButton *m_resignBtn;
    QComboBox *m_themeCombo;

    // Timers
    QTimer *m_clockTimer;
    int m_whiteSeconds = 0;
    int m_blackSeconds = 0;
    bool m_clockRunning = false;
};

#endif // MAINWINDOW_H
