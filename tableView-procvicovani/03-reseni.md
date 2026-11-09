# Řešení úloh (nejdřív zkus sám!)

Všechna řešení počítají s `ui->tableView`, `QStandardItemModel *mModelStandardItem` a v `.h` s `#include <QStandardItemModel>`. V `.cpp` je potřeba `#include <QStandardItem>` a `<QHeaderView>`.

## Úloha 1
```cpp
mModelStandardItem = new QStandardItemModel(this);
for (int r = 1; r <= 10; ++r) {
    QList<QStandardItem*> row;
    for (int c = 1; c <= 10; ++c)
        row.append(new QStandardItem(QString::number(r * c)));
    mModelStandardItem->appendRow(row);
}
ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
ui->tableView->horizontalHeader()->setVisible(false);
ui->tableView->verticalHeader()->setVisible(false);
ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
ui->tableView->setModel(mModelStandardItem);
```

## Úloha 2
```cpp
mModelStandardItem->setHorizontalHeaderLabels({"Jméno", "Věk", "Město"});
auto addPerson = [this](const QString &name, int age, const QString &city) {
    QList<QStandardItem*> row;
    row << new QStandardItem(name);
    QStandardItem *ageItem = new QStandardItem;
    ageItem->setData(age, Qt::DisplayRole);      // číslo, ne text → správné řazení
    row << ageItem << new QStandardItem(city);
    mModelStandardItem->appendRow(row);
};
addPerson("Jana", 25, "Praha");
addPerson("Petr", 31, "Brno");
addPerson("Eva", 19, "Ostrava");
addPerson("Tomáš", 42, "Praha");
```

## Úloha 3
```cpp
// konstruktor
connect(ui->tableView, &QTableView::clicked, this, &MainWindow::onTableViewClicked);

void MainWindow::onTableViewClicked(const QModelIndex &index)
{
    QStandardItem *item = mModelStandardItem->itemFromIndex(index);
    ui->labelInfo->setText(QString("Řádek: %1, sloupec: %2, hodnota: %3")
                           .arg(index.row()).arg(index.column()).arg(item->text()));
}
```

## Úloha 4
```cpp
for (int r = 0; r < 10; ++r)
    for (int c = 0; c < 10; ++c) {
        QStandardItem *item = mModelStandardItem->item(r, c);
        int v = item->text().toInt();
        item->setTextAlignment(Qt::AlignCenter);
        if (v % 5 == 0)
            item->setBackground(QBrush(Qt::yellow));
        int s = qRound(std::sqrt(v));            // #include <cmath>
        if (s * s == v)
            item->setForeground(QBrush(Qt::red));
    }
```

## Úloha 5
```cpp
ui->tableView->setAlternatingRowColors(true);
ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
ui->tableView->setSortingEnabled(true);
```
Věk řadí správně díky `setData(age, Qt::DisplayRole)` z úlohy 2.

## Úloha 6
```cpp
void MainWindow::onPushButtonAddClicked()
{
    QString name = ui->lineEditName->text().trimmed();
    bool ok = false;
    int age = ui->lineEditAge->text().toInt(&ok);
    if (name.isEmpty() || !ok || age < 0) {
        ui->labelInfo->setText("Chyba: zadej jméno a platný věk");
        return;
    }
    QStandardItem *ageItem = new QStandardItem;
    ageItem->setData(age, Qt::DisplayRole);
    mModelStandardItem->appendRow({new QStandardItem(name), ageItem,
                                   new QStandardItem(ui->lineEditCity->text())});
    ui->labelInfo->setText("Přidáno");
}
```

## Úloha 7
```cpp
ui->pushButtonDelete->setEnabled(false);
connect(ui->tableView->selectionModel(), &QItemSelectionModel::currentChanged, this,
        [this](const QModelIndex &cur) { ui->pushButtonDelete->setEnabled(cur.isValid()); });

void MainWindow::onPushButtonDeleteClicked()
{
    QModelIndex cur = ui->tableView->currentIndex();
    if (cur.isValid())
        mModelStandardItem->removeRow(cur.row());
}
```
`selectionModel()` existuje až po `setModel()`, připoj tedy po něm.

## Úloha 8
```cpp
void MainWindow::updateCount()
{
    ui->labelInfo->setText("Počet řádků: " + QString::number(mModelStandardItem->rowCount()));
}

void MainWindow::onPushButtonClearClicked()
{
    mModelStandardItem->removeRows(0, mModelStandardItem->rowCount());  // hlavičky zůstanou
    updateCount();
}
```
`updateCount()` volej na konci přidání, mazání i vymazání.

## Úloha 9
```cpp
ui->tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

void MainWindow::onPushButtonDeleteClicked()
{
    QModelIndexList rows = ui->tableView->selectionModel()->selectedRows();
    QList<int> numbers;
    for (const QModelIndex &i : rows)
        numbers.append(i.row());
    std::sort(numbers.begin(), numbers.end(), std::greater<int>());   // #include <algorithm>, <functional>
    for (int r : numbers)
        mModelStandardItem->removeRow(r);
}
```

## Úloha 10
```cpp
// vytvoření: item->setData(0, Qt::UserRole); updateItemText(item);
void MainWindow::onTableViewClicked(const QModelIndex &index)
{
    QStandardItem *item = mModelStandardItem->itemFromIndex(index);
    item->setData(item->data(Qt::UserRole).toInt() + 1, Qt::UserRole);
    item->setText(QString::number(item->data(Qt::UserRole).toInt()));
}

void MainWindow::onPushButtonResetClicked()
{
    for (int r = 0; r < mModelStandardItem->rowCount(); ++r)
        for (int c = 0; c < mModelStandardItem->columnCount(); ++c) {
            QStandardItem *item = mModelStandardItem->item(r, c);
            item->setData(0, Qt::UserRole);
            item->setText("0");
        }
}
```

## Úloha 11
```cpp
// plnění (#include <QRandomGenerator>)
for (int r = 0; r < 5; ++r) {
    QList<QStandardItem*> row;
    for (int c = 0; c < 5; ++c)
        row.append(new QStandardItem(QString::number(QRandomGenerator::global()->bounded(1, 10))));
    mModelStandardItem->appendRow(row);
}

void MainWindow::onTableViewClicked(const QModelIndex &index)
{
    int rowSum = 0, colSum = 0, total = 0;
    for (int r = 0; r < mModelStandardItem->rowCount(); ++r)
        for (int c = 0; c < mModelStandardItem->columnCount(); ++c) {
            int v = mModelStandardItem->item(r, c)->text().toInt();
            total += v;
            if (r == index.row()) rowSum += v;
            if (c == index.column()) colSum += v;
        }
    ui->labelRow->setText("Řádek: " + QString::number(rowSum));
    ui->labelColumn->setText("Sloupec: " + QString::number(colSum));
    ui->labelTotal->setText("Celkem: " + QString::number(total));
}
```

## Úloha 12
```cpp
QStringList tasks = {"Úkol 1", "Úkol 2", "Úkol 3", "Úkol 4", "Úkol 5", "Úkol 6"};
for (int i = 0; i < tasks.size(); ++i) {
    QStandardItem *name = new QStandardItem(tasks[i]);
    name->setCheckable(true);
    name->setEditable(false);
    QStandardItem *prio = new QStandardItem(QString::number(i % 3 + 1));
    mModelStandardItem->appendRow({name, prio});
}
mModelStandardItem->setHorizontalHeaderLabels({"Úkol", "Priorita"});

void MainWindow::onTableViewClicked(const QModelIndex &)
{
    QString result;
    for (int r = 0; r < mModelStandardItem->rowCount(); ++r) {
        QStandardItem *item = mModelStandardItem->item(r, 0);
        if (item->checkState() == Qt::Checked)
            result += item->text() + " ";
    }
    ui->labelInfo->setText(result);
}
```

## Úloha 13
```cpp
ui->tableView->setEditTriggers(QAbstractItemView::DoubleClicked);
connect(mModelStandardItem, &QStandardItemModel::itemChanged, this, &MainWindow::onItemChanged);

void MainWindow::onItemChanged(QStandardItem *item)
{
    if (item->column() != 1)
        return;
    bool ok = false;
    int age = item->text().toInt(&ok);
    if (!ok || age < 0) {
        QSignalBlocker blocker(mModelStandardItem);   // žádná smyčka
        item->setData(0, Qt::DisplayRole);
        ui->labelInfo->setText("Neplatný věk, nastaveno 0");
    }
}
```

## Úloha 14
```cpp
// po naplnění: unikátní města
QSet<QString> cities;
for (int r = 0; r < mModelStandardItem->rowCount(); ++r)
    cities.insert(mModelStandardItem->item(r, 2)->text());
ui->comboBox->addItem("Všechna");
ui->comboBox->addItems(QStringList(cities.begin(), cities.end()));

connect(ui->comboBox, &QComboBox::currentTextChanged, this, &MainWindow::onComboBoxChanged);

void MainWindow::onComboBoxChanged(const QString &city)
{
    for (int r = 0; r < mModelStandardItem->rowCount(); ++r) {
        bool show = (city == "Všechna") || mModelStandardItem->item(r, 2)->text() == city;
        ui->tableView->setRowHidden(r, !show);
    }
}
```

## Úloha 15
```cpp
void MainWindow::recalcRow(int r)
{
    double hours = mModelStandardItem->item(r, 1)->text().toDouble();
    double rate  = mModelStandardItem->item(r, 2)->text().toDouble();
    QSignalBlocker blocker(mModelStandardItem);
    mModelStandardItem->item(r, 3)->setText(QString::number(hours * rate, 'f', 2));
}

void MainWindow::onItemChanged(QStandardItem *item)
{
    if (item->column() == 1 || item->column() == 2) {
        recalcRow(item->row());
        updateTotal();
    }
}

void MainWindow::updateTotal()
{
    double sum = 0;
    for (int r = 0; r < mModelStandardItem->rowCount(); ++r)
        sum += mModelStandardItem->item(r, 3)->text().toDouble();
    ui->labelInfo->setText("Celkem: " + QString::number(sum, 'f', 2));
}
```
Sloupec 3 při vytváření řádku: `item->setEditable(false)`.

## Úloha 16
```cpp
// člen: bool mPlayerX = true; bool mGameOver = false;
void MainWindow::onTableViewClicked(const QModelIndex &index)
{
    QStandardItem *item = mModelStandardItem->itemFromIndex(index);
    if (mGameOver || !item->text().isEmpty())
        return;
    item->setText(mPlayerX ? "X" : "O");
    item->setTextAlignment(Qt::AlignCenter);

    if (hasWon(item->text())) {
        ui->labelInfo->setText("Vyhrál " + item->text());
        mGameOver = true;
    } else if (isFull()) {
        ui->labelInfo->setText("Remíza");
        mGameOver = true;
    }
    mPlayerX = !mPlayerX;
}

bool MainWindow::hasWon(const QString &s)
{
    auto t = [&](int r, int c) { return mModelStandardItem->item(r, c)->text() == s; };
    for (int i = 0; i < 3; ++i)
        if ((t(i,0) && t(i,1) && t(i,2)) || (t(0,i) && t(1,i) && t(2,i)))
            return true;
    return (t(0,0) && t(1,1) && t(2,2)) || (t(0,2) && t(1,1) && t(2,0));
}

bool MainWindow::isFull()
{
    for (int r = 0; r < 3; ++r)
        for (int c = 0; c < 3; ++c)
            if (mModelStandardItem->item(r, c)->text().isEmpty())
                return false;
    return true;
}
```
Prázdné buňky vytvoř jako `new QStandardItem("")`. Nová hra je smyčka přes buňky: `setText("")`, `mPlayerX = true`, `mGameOver = false`. Výšku řádků nastav `verticalHeader()->setSectionResizeMode(QHeaderView::Stretch)`.

## Úloha 17 (kostra)
```cpp
// Cena/ks ukládáme v CZK do UserRole, zobrazujeme podle měny
double MainWindow::rate() const
{
    const QString cur = ui->comboBoxCurrency->currentText();
    if (cur == "EUR") return 1.0 / 25.0;
    if (cur == "USD") return 1.0 / 23.0;
    return 1.0;
}

void MainWindow::refreshRow(int r)
{
    QSignalBlocker blocker(mModelStandardItem);
    int qty = mModelStandardItem->item(r, 1)->text().toInt();
    double priceCzk = mModelStandardItem->item(r, 2)->data(Qt::UserRole).toDouble();
    mModelStandardItem->item(r, 2)->setText(QString::number(priceCzk * rate(), 'f', 2));
    mModelStandardItem->item(r, 3)->setText(QString::number(qty * priceCzk * rate(), 'f', 2));
}
```
Při změně comboBoxu projdi `refreshRow(r)` pro všechny řádky a přepočti součet.

## Úloha 18
```cpp
void MainWindow::onPushButtonSaveClicked()
{
    QString path = QFileDialog::getSaveFileName(this, "Uložit", "", "CSV (*.csv)");
    if (path.isEmpty()) return;
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    QTextStream out(&file);
    for (int r = 0; r < mModelStandardItem->rowCount(); ++r) {
        QStringList cells;
        for (int c = 0; c < mModelStandardItem->columnCount(); ++c)
            cells << mModelStandardItem->item(r, c)->text();
        out << cells.join(';') << '\n';
    }
}

void MainWindow::onPushButtonLoadClicked()
{
    QString path = QFileDialog::getOpenFileName(this, "Načíst", "", "CSV (*.csv)");
    if (path.isEmpty()) return;
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    mModelStandardItem->removeRows(0, mModelStandardItem->rowCount());
    QTextStream in(&file);
    while (!in.atEnd()) {
        QStringList cells = in.readLine().split(';');
        QList<QStandardItem*> row;
        for (const QString &s : cells)
            row.append(new QStandardItem(s));
        mModelStandardItem->appendRow(row);
    }
}
```
Includy: `<QFile> <QTextStream> <QFileDialog>`.

## Odpovědi na kontrolní otázky
1. Model drží data, pohled je jen zobrazuje; díky tomu lze jeden model zobrazit ve více pohledech a data jsou oddělená od UI.
2. `UserRole` je volná role pro vlastní data; číslo se nemusí pořád převádět z textu a text se z něj odvodí.
3. `nullptr`. Před použitím zkontroluj `if (item)`, nebo buňky vždy vytvoř.
4. `clear()` smaže i hlavičky a nastaví 0×0, `removeRows` smaže jen řádky.
5. Po smazání řádku se další posunou nahoru a indexy by neplatily.
6. `QSignalBlocker`, nebo příznak `mUpdating`.
7. Celou tabulku: `setEditTriggers(NoEditTriggers)`. Jednu buňku: `item->setEditable(false)`.
8. `currentIndex().row()` a `selectionModel()->selectedRows()`.
