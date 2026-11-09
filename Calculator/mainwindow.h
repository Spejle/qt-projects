#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

public slots:
    void on0();
    void on1();
    void on2();
    void on3();
    void on4();
    void on5();
    void on6();
    void on7();
    void on8();
    void on9();

    void onPlus();
    void onMinus();
    void onDivide();
    void onMultiply();
    void onPower();
    void onEqual();
    void onComma();
    void onDelete();
    void onClear();

private:
    Ui::MainWindow *ui;
    void insertToDisplay(QString string);
    double calculate(double number1, char ope, double number2);
};
#endif // MAINWINDOW_H
