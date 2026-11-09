import sys

from PySide6.QtCore import QAbstractTableModel, Qt, Signal
from PySide6.QtWidgets import (
    QApplication,
    QHeaderView,
    QLabel,
    QTableView,
    QVBoxLayout,
    QWidget,
)

ROWS = 5
COLS = 6
CYCLE = ["", "X", "O"]  # klik: prázdné -> X -> O -> prázdné


class TicTacModel(QAbstractTableModel):
    changed = Signal()

    def __init__(self):
        super().__init__()
        self._data = [[""] * COLS for _ in range(ROWS)]

    def rowCount(self, parent=None):
        return ROWS

    def columnCount(self, parent=None):
        return COLS

    def data(self, index, role=Qt.DisplayRole):
        if not index.isValid():
            return None
        value = self._data[index.row()][index.column()]
        if role == Qt.DisplayRole:
            return value
        if role == Qt.TextAlignmentRole:
            return Qt.AlignCenter
        return None

    def flags(self, index):
        return Qt.ItemIsEnabled | Qt.ItemIsSelectable

    def toggle(self, index):
        r, c = index.row(), index.column()
        current = self._data[r][c]
        self._data[r][c] = CYCLE[(CYCLE.index(current) + 1) % len(CYCLE)]
        self.dataChanged.emit(index, index)
        self.changed.emit()

    def row_count_x(self, row):
        # kolečka (O) i prázdná pole se nepočítají, jen křížky
        return self._data[row].count("X")

    def total_x(self):
        return sum(self.row_count_x(r) for r in range(ROWS))


class Window(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Tabulka 5x6 - křížky")

        self.model = TicTacModel()
        self.view = QTableView()
        self.view.setModel(self.model)
        self.view.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)
        self.view.verticalHeader().setSectionResizeMode(QHeaderView.Stretch)
        self.view.setStyleSheet("QTableView { font-size: 28px; }")
        self.view.clicked.connect(self.model.toggle)

        self.label = QLabel()
        self.label.setStyleSheet("font-size: 16px;")

        layout = QVBoxLayout(self)
        layout.addWidget(self.view)
        layout.addWidget(self.label)

        self.model.changed.connect(self.update_label)
        self.update_label()
        self.resize(640, 480)

    def update_label(self):
        parts = [f"Řádek {r + 1}: {self.model.row_count_x(r)}" for r in range(ROWS)]
        self.label.setText(" | ".join(parts) + f"   —   Celkem: {self.model.total_x()}")


if __name__ == "__main__":
    app = QApplication(sys.argv)
    w = Window()
    w.show()
    sys.exit(app.exec())
