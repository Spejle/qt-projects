#ifndef NEWGAMEDIALOG_H
#define NEWGAMEDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QSpinBox>
#include "types.h"

struct GameSettings {
    GameMode mode = GameMode::PlayerVsPlayer;
    PieceColor humanColor = PieceColor::White;
    AIDifficulty difficulty = AIDifficulty::Medium;
    BoardTheme theme = BoardTheme::TournamentGreen;
    int timerMinutes = 0; // 0 = unlimited
};

class NewGameDialog : public QDialog {
    Q_OBJECT

public:
    explicit NewGameDialog(const GameSettings &initial, QWidget *parent = nullptr);

    GameSettings settings() const;

private slots:
    void onModeChanged();

private:
    QComboBox *m_modeCombo;
    QButtonGroup *m_colorGroup;
    QRadioButton *m_whiteRadio;
    QRadioButton *m_blackRadio;
    QRadioButton *m_randomRadio;
    QWidget *m_colorWidget;

    QComboBox *m_diffCombo;
    QWidget *m_diffWidget;

    QComboBox *m_themeCombo;
    QComboBox *m_timeCombo;
};

#endif // NEWGAMEDIALOG_H
