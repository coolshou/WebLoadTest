#include "historymodel.h"

HistoryModel::HistoryModel(QObject *parent)
    : QAbstractTableModel(parent)
{}

QVariant HistoryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
        return QVariant();
    static const QStringList headers = {
        "ID", "DateTime", "Load sec", "Status", "Url", "Comment"
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

QVariant HistoryModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::TextAlignmentRole){
        if ((index.column() == HistoryColumn::LoadSec)||
         (index.column() == HistoryColumn::ID)){
            return Qt::AlignCenter;
        }else if (index.column() < HistoryColumn::Status){
            return Qt::AlignVCenter;
        }
    }
    if (!index.isValid()|| role != Qt::DisplayRole)
        return QVariant();

    return dataMatrix[index.row()][index.column()];
}

void HistoryModel::addEntry(const QString &dateTime,
              const QString &url,
              const QString &loadSec,
              const QString &status,
              const QString &comment) {
    int idx = dataMatrix.size();
    beginInsertRows(QModelIndex(), idx, idx);
    dataMatrix.append({QString::number(idx), dateTime, loadSec, status, url, comment});
    endInsertRows();
}

void HistoryModel::clear() {
    beginResetModel();
    dataMatrix.clear();
    endResetModel();
}
