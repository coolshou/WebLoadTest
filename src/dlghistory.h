#ifndef DLGHISTORY_H
#define DLGHISTORY_H

#include <QDialog>
#include <QMenu>
#include <QAction>
#include "src/historymodel.h"
#include "lib/qcustomplot.h"

namespace Ui {
class DlgHistory;
}

class DlgHistory : public QDialog
{
    Q_OBJECT

public:
    explicit DlgHistory(QWidget *parent = nullptr);
    ~DlgHistory();
    void setStartTime(double starttime);
    void setDataModel(HistoryModel *model);
public slots:
    void addData(double x, double y);
    void addErrorData(double x, double y);
    void addDatas(QVector<double> keys, QVector<double> values);
protected:
    void changeEvent(QEvent *e);
private slots:
    void clear();
    void onTableContextMenu(QPoint pos);
    void onCopyAction(bool checked);
private:
    void initMenu();
private:
    Ui::DlgHistory *ui;
    HistoryModel *mModel;
    QCustomPlot *mPlot;
    QCPBars *mBars;
    QCPBars *mBarsError;
    QMenu *mTableMenu;
    QAction *mCopyAction;
};

#endif // DLGHISTORY_H
