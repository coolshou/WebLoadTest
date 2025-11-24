#ifndef DLGHISTORY_H
#define DLGHISTORY_H

#include <QDialog>
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

private:
    Ui::DlgHistory *ui;
    HistoryModel *mModel;
    QCustomPlot *mPlot;
    QCPBars *mBars;
    QCPBars *mBarsError;
};

#endif // DLGHISTORY_H
