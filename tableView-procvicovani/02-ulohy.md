# QTableView – úlohy na procvičení

Pro každou úlohu vytvoř nový projekt *Qt Widgets Application* s `QTableView` (`tableView`), případně dalšími prvky z Designeru. Řešení jsou v `03-reseni.md` – nejdřív zkus sám.
Obtížnost: ⭐ lehká, ⭐⭐ střední, ⭐⭐⭐ těžší.

## Část A – základy

### Úloha 1 ⭐ Násobilka
Naplň tabulku 10×10 malou násobilkou. Skryj obě záhlaví, sloupce nech roztáhnout na celou šířku, editace zakázána.

### Úloha 2 ⭐ Hlavičky
Vytvoř tabulku 4 řádky × 3 sloupce se sloupci `Jméno`, `Věk`, `Město` a ručně vyplň 4 osoby (`appendRow`).

### Úloha 3 ⭐ Čtení buňky
Po kliknutí na buňku zobraz ve `labelInfo` text `Řádek: X, sloupec: Y, hodnota: Z`.

### Úloha 4 ⭐ Zarovnání a barvy
V tabulce z úlohy 1 zarovnej text na střed. Čísla dělitelná 5 vybarvi žlutým pozadím, druhé mocniny (1, 4, 9...) červeným textem.

### Úloha 5 ⭐ Střídavé řádky a řazení
Tabulka z úlohy 2: zapni střídavé barvy řádků, výběr po celých řádcích a řazení kliknutím na hlavičku. Ověř, že se věk řadí správně jako číslo (viz chyba č. 5 v teorii).

## Část B – přidávání a mazání

### Úloha 6 ⭐⭐ Přidání řádku
Pod tabulku dej tři `lineEdit` (jméno, věk, město) a tlačítko `Přidat`. Po stisku se přidá řádek. Prázdné jméno nebo neplatný věk (`toInt` s `bool ok`) řádek nepřidá a ukáže chybu v `labelInfo`.

### Úloha 7 ⭐⭐ Smazání řádku
Přidej tlačítko `Smazat vybraný`. Smaže řádek aktuální buňky. Když nic není vybráno, tlačítko je neaktivní (`setEnabled`).

### Úloha 8 ⭐⭐ Smazat vše + počítadlo
Tlačítko `Vymazat vše` (zachovej hlavičky!) a `labelInfo` s textem `Počet řádků: N`, který se aktualizuje po každé změně.

### Úloha 9 ⭐⭐ Hromadné mazání
Povol výběr více řádků (`ExtendedSelection`) a tlačítko smaže všechny vybrané. Pozor na posun indexů.

## Část C – data a interakce

### Úloha 10 ⭐⭐ Počítadlo kliků
Tabulka 3×4 jako v projektu `table`: každá buňka uchovává číslo v `UserRole`, klik ho zvýší o 1. Přidej tlačítko `Reset`, které všechny hodnoty nuluje.

### Úloha 11 ⭐⭐ Součty
Tabulka 5×5 s náhodnými čísly 1–9 (`QRandomGenerator`). Pod ní tři labely: součet vybrané buňky řádku, součet sloupce a součet celé tabulky. Aktualizuj po kliknutí na buňku.

### Úloha 12 ⭐⭐ Zaškrtávací tabulka
Seznam 6 úkolů ve sloupci 0 se zaškrtávátkem (`setCheckable`), sloupec 1 s prioritou. Label zobrazí názvy zaškrtnutých úkolů (podobně jako v projektu `listView`).

### Úloha 13 ⭐⭐ Editace
Povol editaci dvojklikem. Sloupec `Věk` se po úpravě zkontroluje (`itemChanged`): neplatná hodnota se vrátí na 0 a zobrazí chybu. Nezacyklit se (`QSignalBlocker`).

### Úloha 14 ⭐⭐ Filtr podle comboBoxu
Do `comboBox` dej města z tabulky. Po výběru se skryjí řádky s jiným městem (`setRowHidden`). Položka `Všechna` zobrazí vše.

## Část D – větší projekty

### Úloha 15 ⭐⭐⭐ Kalkulačka mezd
Tabulka `Jméno | Hodiny | Sazba | Hrubá mzda`. Uživatel upravuje hodiny a sazbu, hrubá mzda (nelze editovat) se přepočte sama. Dole label s celkovým součtem všech mezd.

### Úloha 16 ⭐⭐⭐ Piškvorky 3×3
Tabulka 3×3 bez hlaviček, kliknutím se střídavě vkládá `X` a `O` do prázdných buněk. Detekuj výhru (řádek, sloupec, úhlopříčka) a remízu, tlačítko `Nová hra`.

### Úloha 17 ⭐⭐⭐ Seznam zboží s cenou
Sloupce `Zboží | Množství | Cena/ks | Celkem`. Tlačítka pro přidání a smazání, součet v labelu, comboBox s měnou (CZK, EUR, USD) přepočítá zobrazené ceny pevným kurzem.

### Úloha 18 ⭐⭐⭐ Uložení do souboru
K úloze 17 přidej `Uložit` a `Načíst` do CSV (`QFile`, `QTextStream`, `QFileDialog`). Při načtení nejdřív model vyčisti a nastav hlavičky.

## Kontrolní otázky
1. Jaký je rozdíl mezi modelem a pohledem? Proč se data nedržejí v `QTableView`?
2. K čemu slouží `Qt::UserRole` a proč se hodí ukládat v něm číslo?
3. Co vrátí `item(r, c)` pro neexistující buňku a jak se tomu bránit?
4. Čím se liší `clear()` od `removeRows(0, rowCount())`?
5. Proč se při mazání více řádků maže odzadu?
6. Jak zabráníš nekonečné smyčce při změně položky ve slotu `itemChanged`?
7. Jak zakážeš editaci celé tabulky a jak jen jedné buňky?
8. Jak zjistíš, na kterém řádku je uživatel, a jaké jsou jeho vybrané řádky?
