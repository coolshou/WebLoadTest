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
    void setDataModel(HistoryModel *model);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::DlgHistory *ui;
    HistoryModel *mModel;
    QCustomPlot *mPlot;
};

#endif // DLGHISTORY_H
