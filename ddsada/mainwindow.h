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
    void onTableViewItemClicked(const QModelIndex &index);
    void onTableViewDoubleClicked(const QModelIndex &index);
    void onHeaderClicked(int logicalIndex);

    void onCurrencyChange();

private:
    Ui::MainWindow *ui;
    //model se kterym pracujeme v view
    QStandardItemModel *mModelStandardItem;
};
#endif // MAINWINDOW_H
