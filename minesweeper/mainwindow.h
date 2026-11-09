#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:

private:

    Ui::MainWindow *ui;
    // model se kterym pracujeme v view
    QStandardItemModel *mModelStandardItem;

    void createMines();
    void createNumbers();
    int countMineNumbers(int x, int y);
    bool checkValidity( int x, int y);
};
#endif // MAINWINDOW_H
