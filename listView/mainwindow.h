#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QStringListModel>
#include <QMainWindow>
#include <QStandardItemModel>
#include <Qt>

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
    void onListViewItemClicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
    QStringListModel *mModelStringList = nullptr;
    QStandardItemModel *mModelStandardItem = nullptr;
};
#endif // MAINWINDOW_H
