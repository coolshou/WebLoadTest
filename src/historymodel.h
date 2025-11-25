#ifndef HISTORYMODEL_H
#define HISTORYMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QString>

enum HistoryColumn {
    ID = 0,
    DateTime,
    LoadSec,
    Status,
    Url,
    COLUMN_COUNT
};

class HistoryModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit HistoryModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    //
    void addEntry(const QString &dateTime, const QString &url,
                  const QString &loadSec, const QString &status,
                  const QString &comment="");
    void clear();
private:
    QVector<QVector<QString>> dataMatrix;
};

#endif // HISTORYMODEL_H
