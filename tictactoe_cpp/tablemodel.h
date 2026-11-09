#pragma once

#include <QAbstractTableModel>
#include <QStringList>
#include <array>

class TicTacModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    static constexpr int Rows = 5;
    static constexpr int Cols = 6;

    explicit TicTacModel(QObject *parent = nullptr) : QAbstractTableModel(parent)
    {
        for (auto &row : m_data)
            row.fill(QString());
    }

    int rowCount(const QModelIndex & = {}) const override { return Rows; }
    int columnCount(const QModelIndex & = {}) const override { return Cols; }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
    {
        if (!index.isValid())
            return {};
        if (role == Qt::DisplayRole)
            return m_data[index.row()][index.column()];
        if (role == Qt::TextAlignmentRole)
            return int(Qt::AlignCenter);
        return {};
    }

    Qt::ItemFlags flags(const QModelIndex &) const override
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }

    // klik: prázdné -> X -> O -> prázdné
    void toggle(const QModelIndex &index)
    {
        if (!index.isValid())
            return;
        QString &cell = m_data[index.row()][index.column()];
        if (cell.isEmpty())
            cell = "X";
        else if (cell == "X")
            cell = "O";
        else
            cell.clear();
        emit dataChanged(index, index);
        emit changed();
    }

    // kolečka (O) a prázdná pole se nepočítají, jen křížky
    int rowCountX(int row) const
    {
        int n = 0;
        for (const QString &s : m_data[row])
            if (s == "X")
                ++n;
        return n;
    }

    int totalX() const
    {
        int n = 0;
        for (int r = 0; r < Rows; ++r)
            n += rowCountX(r);
        return n;
    }

signals:
    void changed();

private:
    std::array<std::array<QString, Cols>, Rows> m_data;
};
