#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRandomGenerator>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , scoreLabel(nullptr)
    , timeLabel(nullptr)
    , mScore(0)
    , mTimeLeft(30)
    , mCurrentInterval(1000.0)
{
    setWindowTitle("Clicker");
    resize(600, 500);
    setMinimumSize(600,360);


    QWidget *central = new QWidget(this);
    setCentralWidget(central);


    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    scoreLabel = new QLabel("Score: 0", this);
    timeLabel = new QLabel("Time left: 30s", this);

    topBarWidget = new QWidget(this);
    QHBoxLayout *topBarLayout = new QHBoxLayout(topBarWidget);
    topBarLayout->addWidget(scoreLabel);
    topBarLayout->addStretch();
    topBarLayout->addWidget(timeLabel);
    mainLayout->addWidget(topBarWidget);




    gameAreaWidget = new QWidget(central);
    mainLayout->addWidget(gameAreaWidget, 1);
    targetButton = new QPushButton("Click me", gameAreaWidget);
    targetButton->resize(100,50);

    gameTimer = new QTimer(this);
    buttonClickTimer = new QTimer(this);

    connect(targetButton, &QPushButton::clicked, this, &MainWindow::onTargetClicker);
    connect(gameTimer, &QTimer::timeout, this, &MainWindow::onGameTimerTimeout);
    connect(buttonClickTimer, &QTimer::timeout, this, &MainWindow::onButtonTimerTimeout);
}

MainWindow::~MainWindow() = default;

void MainWindow::onTargetClicker()
{
    if(mScore == 0){
        mTimeLeft = 30;
        mCurrentInterval = 1000.0;

        gameTimer->start(1000);
        buttonClickTimer->start(static_cast<int>(mCurrentInterval));
    }
    ++ mScore;
    scoreLabel->setText("Score: " + QString::number(mScore));
    moveTargetToRandomPosition();
    mCurrentInterval *= 0.95;
    buttonClickTimer->setInterval(static_cast<int>(mCurrentInterval));
}

void MainWindow::onGameTimerTimeout()
{
    mTimeLeft--;
    timeLabel->setText("Time left: " + QString::number(mTimeLeft) + "s");

    if (mTimeLeft <= 0) {
        buttonClickTimer->stop();
        gameTimer->stop();
        targetButton->setEnabled(false);
    }
}

void MainWindow::onButtonTimerTimeout()
{
    moveTargetToRandomPosition();
}

void MainWindow::moveTargetToRandomPosition()
{
    int maxX = gameAreaWidget->width() - targetButton->width();
    int maxY = gameAreaWidget->height() - targetButton->height();

    int randomX = QRandomGenerator::global()->bounded(0, maxX);
    int randomY = QRandomGenerator::global()->bounded(0, maxY);

    targetButton->move(randomX, randomY);
}
