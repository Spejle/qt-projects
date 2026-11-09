#include "newgamedialog.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QGroupBox>
#include <QRandomGenerator>

NewGameDialog::NewGameDialog(const GameSettings &initial, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Start New Game"));
    setMinimumWidth(360);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(16);

    QFormLayout *form = new QFormLayout();
    form->setLabelAlignment(Qt::AlignLeft);
    form->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    form->setSpacing(12);

    // Mode
    m_modeCombo = new QComboBox(this);
    m_modeCombo->addItem(tr("Player vs Player (Pass & Play)"), static_cast<int>(GameMode::PlayerVsPlayer));
    m_modeCombo->addItem(tr("Player vs Claude Haiku (High Effort) ⚡"), static_cast<int>(GameMode::PlayerVsClaudeHaiku));
    m_modeCombo->addItem(tr("Player vs Antigravity (AI Assistant) 🚀"), static_cast<int>(GameMode::PlayerVsAntigravity));
    m_modeCombo->addItem(tr("Player vs Computer (Local Minimax)"), static_cast<int>(GameMode::PlayerVsAI));
    if (initial.mode == GameMode::PlayerVsClaudeHaiku) {
        m_modeCombo->setCurrentIndex(1);
    } else if (initial.mode == GameMode::PlayerVsAntigravity) {
        m_modeCombo->setCurrentIndex(2);
    } else if (initial.mode == GameMode::PlayerVsAI) {
        m_modeCombo->setCurrentIndex(3);
    } else {
        m_modeCombo->setCurrentIndex(0);
    }
    form->addRow(tr("Game Mode:"), m_modeCombo);

    // Color selection
    m_colorWidget = new QWidget(this);
    QHBoxLayout *colorLayout = new QHBoxLayout(m_colorWidget);
    colorLayout->setContentsMargins(0, 0, 0, 0);

    m_colorGroup = new QButtonGroup(this);
    m_whiteRadio = new QRadioButton(tr("White"), m_colorWidget);
    m_blackRadio = new QRadioButton(tr("Black"), m_colorWidget);
    m_randomRadio = new QRadioButton(tr("Random"), m_colorWidget);
    m_whiteRadio->setChecked(true);

    m_colorGroup->addButton(m_whiteRadio, 1);
    m_colorGroup->addButton(m_blackRadio, 2);
    m_colorGroup->addButton(m_randomRadio, 3);

    colorLayout->addWidget(m_whiteRadio);
    colorLayout->addWidget(m_blackRadio);
    colorLayout->addWidget(m_randomRadio);
    form->addRow(tr("Your Pieces:"), m_colorWidget);

    // AI Difficulty
    m_diffWidget = new QWidget(this);
    QHBoxLayout *diffLayout = new QHBoxLayout(m_diffWidget);
    diffLayout->setContentsMargins(0, 0, 0, 0);
    m_diffCombo = new QComboBox(m_diffWidget);
    m_diffCombo->addItem(tr("Easy (Casual)"), static_cast<int>(AIDifficulty::Easy));
    m_diffCombo->addItem(tr("Medium (Club)"), static_cast<int>(AIDifficulty::Medium));
    m_diffCombo->addItem(tr("Hard (Master)"), static_cast<int>(AIDifficulty::Hard));
    m_diffCombo->setCurrentIndex(static_cast<int>(initial.difficulty));
    diffLayout->addWidget(m_diffCombo);
    form->addRow(tr("AI Difficulty:"), m_diffWidget);

    // Theme
    m_themeCombo = new QComboBox(this);
    m_themeCombo->addItem(tr("Tournament Green"), static_cast<int>(BoardTheme::TournamentGreen));
    m_themeCombo->addItem(tr("Classic Wood"), static_cast<int>(BoardTheme::ClassicWood));
    m_themeCombo->addItem(tr("Modern Slate"), static_cast<int>(BoardTheme::ModernSlate));
    m_themeCombo->addItem(tr("Dark Glass"), static_cast<int>(BoardTheme::DarkGlass));
    m_themeCombo->setCurrentIndex(static_cast<int>(initial.theme));
    form->addRow(tr("Board Theme:"), m_themeCombo);

    // Time Control
    m_timeCombo = new QComboBox(this);
    m_timeCombo->addItem(tr("Unlimited"), 0);
    m_timeCombo->addItem(tr("Rapid (10 min)"), 10);
    m_timeCombo->addItem(tr("Blitz (5 min)"), 5);
    m_timeCombo->addItem(tr("Bullet (3 min)"), 3);
    for (int i = 0; i < m_timeCombo->count(); ++i) {
        if (m_timeCombo->itemData(i).toInt() == initial.timerMinutes) {
            m_timeCombo->setCurrentIndex(i);
            break;
        }
    }
    form->addRow(tr("Time Control:"), m_timeCombo);

    mainLayout->addLayout(form);

    // Dialog buttons
    QDialogButtonBox *box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    box->button(QDialogButtonBox::Ok)->setText(tr("Start Game"));
    connect(box, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(box, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(box);

    connect(m_modeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &NewGameDialog::onModeChanged);
    onModeChanged();
}

void NewGameDialog::onModeChanged() {
    GameMode mode = static_cast<GameMode>(m_modeCombo->currentData().toInt());
    bool isAI = (mode == GameMode::PlayerVsAI ||
                 mode == GameMode::PlayerVsAntigravity ||
                 mode == GameMode::PlayerVsClaudeHaiku);
    m_colorWidget->setEnabled(isAI);
    m_diffWidget->setEnabled(mode == GameMode::PlayerVsAI);
}

GameSettings NewGameDialog::settings() const {
    GameSettings s;
    s.mode = static_cast<GameMode>(m_modeCombo->currentData().toInt());

    if (m_whiteRadio->isChecked()) {
        s.humanColor = PieceColor::White;
    } else if (m_blackRadio->isChecked()) {
        s.humanColor = PieceColor::Black;
    } else {
        s.humanColor = (QRandomGenerator::global()->bounded(2) == 0) ? PieceColor::White : PieceColor::Black;
    }

    s.difficulty = static_cast<AIDifficulty>(m_diffCombo->currentData().toInt());
    s.theme = static_cast<BoardTheme>(m_themeCombo->currentData().toInt());
    s.timerMinutes = m_timeCombo->currentData().toInt();
    return s;
}
