#ifndef HISTORYMODEL_H
#define HISTORYMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QString>

enum HistoryColumn {
    DateTime = 0,
    Url,
    LoadSec,
    Status,
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

    // Fetch data dynamically:
    // bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;
    // bool canFetchMore(const QModelIndex &parent) const override;
    // void fetchMore(const QModelIndex &parent) override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    //
    void addEntry(const QString &dateTime, const QString &url, const QString &loadSec, const QString &status);
    void clear();
private:
    QVector<QVector<QString>> dataMatrix;
};

#endif // HISTORYMODEL_H
