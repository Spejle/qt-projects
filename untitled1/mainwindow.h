#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QTimer>
#include <QLabel>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

public slots:
    void onTargetClicker();
    void onGameTimerTimeout();
    void onButtonTimerTimeout();

private:
    QWidget *topBarWidget;
    QWidget *gameAreaWidget;
    QPushButton *targetButton;

    QTimer *gameTimer;
    QTimer *buttonClickTimer;

    QLabel *scoreLabel;
    QLabel *timeLabel;

    int mScore;
    int mTimeLeft;
    double mCurrentInterval;

    void moveTargetToRandomPosition();

};
#endif // MAINWINDOW_H
