# QTableView – teorie pro začátečníky

Vychází ze stylu tvých projektů `table` a `listView` (QStandardItemModel, `ui->`, prefix `m` u členů, sloty `onXxxClicked`).

## 1. Model / View
- **View** (`QTableView`) jen zobrazuje. Sám data nedrží.
- **Model** (`QStandardItemModel`) drží data. Pohled se k němu připojí přes `setModel()`.
- Jeden model může zobrazovat víc pohledů najednou (`QTableView`, `QListView`...).

```cpp
mModelStandardItem = new QStandardItemModel(this);   // this = rodič, o smazání se postará Qt
ui->tableView->setModel(mModelStandardItem);
```

V `.h`: `#include <QStandardItemModel>` a člen `QStandardItemModel *mModelStandardItem = nullptr;`

## 2. Položky (QStandardItem)
Každá buňka tabulky je jedna `QStandardItem`.

```cpp
QStandardItem *item = new QStandardItem("text");   // text v buňce
item->setText("jiný text");
item->setData(42, Qt::UserRole);                   // vlastní skrytá data
int v = item->data(Qt::UserRole).toInt();
item->setCheckable(true);                          // zaškrtávátko
item->setCheckState(Qt::Checked);
item->setEditable(false);                          // buňku nejde editovat
item->setTextAlignment(Qt::AlignCenter);
item->setBackground(QBrush(Qt::yellow));
item->setForeground(QBrush(Qt::red));
```

**Role**: jedna buňka může mít víc údajů podle role.
| Role | K čemu |
|---|---|
| `Qt::DisplayRole` | zobrazený text (to dělá `setText`) |
| `Qt::UserRole` | vlastní data (číslo, ID...) |
| `Qt::BackgroundRole` / `ForegroundRole` | barva pozadí / textu |
| `Qt::CheckStateRole` | stav zaškrtnutí |

> Vlastní data si ukládej do `UserRole`, text odvozuj z nich (viz `updateItemText` v projektu `table`).

## 3. Plnění modelu

```cpp
// po řádcích
for (int r = 0; r < 3; ++r) {
    QList<QStandardItem*> row;
    for (int c = 0; c < 4; ++c)
        row.append(new QStandardItem(QString::number(r * 4 + c)));
    mModelStandardItem->appendRow(row);
}

// nebo po buňkách
mModelStandardItem->setRowCount(3);
mModelStandardItem->setColumnCount(4);
mModelStandardItem->setItem(1, 2, new QStandardItem("X"));   // (řádek, sloupec, položka)
```

Hlavičky:
```cpp
mModelStandardItem->setHorizontalHeaderLabels({"Jméno", "Věk", "Město"});
mModelStandardItem->setVerticalHeaderLabels({"1", "2", "3"});
```

## 4. Čtení a mazání
```cpp
mModelStandardItem->rowCount();
mModelStandardItem->columnCount();
QStandardItem *item = mModelStandardItem->item(r, c);          // může být nullptr!
mModelStandardItem->removeRow(r);
mModelStandardItem->insertRow(r, row);                         // row = QList<QStandardItem*>
mModelStandardItem->clear();                                   // smaže vše včetně hlaviček
mModelStandardItem->removeRows(0, mModelStandardItem->rowCount());  // smaže jen řádky
```

## 5. QModelIndex
Adresa buňky: `index.row()`, `index.column()`, `index.isValid()`.

```cpp
QStandardItem *item = mModelStandardItem->itemFromIndex(index);
QModelIndex idx = mModelStandardItem->index(1, 2);
```

## 6. Signály a výběr

```cpp
connect(ui->tableView, &QTableView::clicked, this, &MainWindow::onTableViewClicked);
connect(ui->tableView, &QTableView::doubleClicked, this, &MainWindow::onTableViewDoubleClicked);
```
Slot: `void onTableViewClicked(const QModelIndex &index);` (deklarace v `public slots:`).

Aktuální výběr:
```cpp
QModelIndex cur = ui->tableView->currentIndex();
QModelIndexList rows = ui->tableView->selectionModel()->selectedRows();
```
Výběr po celých řádcích:
```cpp
ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
```

Signál změny dat v modelu (reaguje na editaci uživatelem i kódem):
```cpp
connect(mModelStandardItem, &QStandardItemModel::itemChanged, this, &MainWindow::onItemChanged);
// void onItemChanged(QStandardItem *item);
```
⚠ Když v `itemChanged` sám měníš položku, vzniká nekonečná smyčka – použij `QSignalBlocker` nebo příznak.

## 7. Vzhled a chování pohledu

```cpp
#include <QHeaderView>

ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);   // sloupce vyplní šířku
ui->tableView->horizontalHeader()->setVisible(false);       // skrýt záhlaví
ui->tableView->verticalHeader()->setVisible(false);
ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);   // zákaz editace
ui->tableView->setAlternatingRowColors(true);
ui->tableView->setSortingEnabled(true);                     // klik na hlavičku řadí
ui->tableView->setColumnWidth(0, 120);
ui->tableView->setShowGrid(false);
```

## 8. Časté chyby
1. Zapomenutý `setModel()` → tabulka je prázdná.
2. `item(r, c)` vrací `nullptr`, když buňka nebyla vytvořena → pád. Před použitím zkontroluj.
3. `new QStandardItem` nemažeš, model je vlastní. Do dvou modelů tu samou položku nedávej.
4. `clear()` smaže i hlavičky – po něm je nastav znovu.
5. Řazení (`setSortingEnabled`) s textem řadí abecedně ("10" < "9"). Číslo ulož přes `setData(10, Qt::DisplayRole)`.
6. Zapomenutý `#include <QHeaderView>` při práci s hlavičkami.
7. Index se po smazání řádku posouvá → mazat odzadu.
