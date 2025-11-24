#include "historymodel.h"

HistoryModel::HistoryModel(QObject *parent)
    : QAbstractTableModel(parent)
{}

QVariant HistoryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
        return QVariant();
    static const QStringList headers = {
        "DateTime", "Url", "Load sec", "Status"
    };

    return (section >= 0 && section < headers.size()) ? headers[section] : QVariant();
}

int HistoryModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    // if (parent.isValid())
    //     return 0;
    return dataMatrix.size();
}

int HistoryModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    // if (parent.isValid())
    //     return 0;

    return COLUMN_COUNT;
}

// bool HistoryModel::hasChildren(const QModelIndex &parent) const
// {
//     // FIXME: Implement me!
// }

// bool HistoryModel::canFetchMore(const QModelIndex &parent) const
// {
//     // FIXME: Implement me!
//     return false;
// }

// void HistoryModel::fetchMore(const QModelIndex &parent)
// {
//     // FIXME: Implement me!
// }

QVariant HistoryModel::data(const QModelIndex &index, int role) const
{
    if ((role == Qt::TextAlignmentRole) &&
        ((index.column() >= HistoryColumn::LoadSec))){
        return Qt::AlignCenter;
    }
    if (!index.isValid()|| role != Qt::DisplayRole)
        return QVariant();

    return dataMatrix[index.row()][index.column()];
    // FIXME: Implement me!
    // return QVariant();
}

void HistoryModel::addEntry(const QString &dateTime,
              const QString &url,
              const QString &loadSec,
              const QString &status) {
    beginInsertRows(QModelIndex(), dataMatrix.size(), dataMatrix.size());
    dataMatrix.append({dateTime, url, loadSec, status});
    endInsertRows();
}

void HistoryModel::clear() {
    beginResetModel();
    dataMatrix.clear();
    endResetModel();
}
