#include "dlghistory.h"
#include "ui_dlghistory.h"

#include "src/historymodel.h"


DlgHistory::DlgHistory(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgHistory)
{
    ui->setupUi(this);
    mPlot = new QCustomPlot();
    ui->vlChart->addWidget(mPlot);
}

DlgHistory::~DlgHistory()
{
    delete ui;
}

void DlgHistory::setDataModel(HistoryModel *model)
{
    mModel = model;
    connect(ui->pbClear, &QPushButton::clicked, mModel, &HistoryModel::clear);
    ui->tableView->setModel(model);
    ui->tableView->setColumnWidth(HistoryColumn::DateTime, 150);
    ui->tableView->setColumnWidth(HistoryColumn::Url, 400);

}

void DlgHistory::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
