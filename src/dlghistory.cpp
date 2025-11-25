#include "dlghistory.h"
#include "ui_dlghistory.h"

#include "src/historymodel.h"


DlgHistory::DlgHistory(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgHistory)
{
    ui->setupUi(this);
    initMenu();
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView, &QTreeView::customContextMenuRequested, this, &DlgHistory::onTableContextMenu);
    mPlot = new QCustomPlot();
    mPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                           QCP::iSelectLegend | QCP::iSelectPlottables);
    QSharedPointer<QCPAxisTickerDateTime> timeTicker(new QCPAxisTickerDateTime);
    timeTicker->setDateTimeFormat("hh:mm:ss");
    mPlot->xAxis->setTicker(timeTicker);
    mPlot->xAxis->setLabel("Time(Sec)");
    mPlot->yAxis->setLabel("Load Time(Sec)");
    mPlot->yAxis->setRange(0, 30);
    mBars = new QCPBars(mPlot->xAxis, mPlot->yAxis);
    // Set bar style
    mBars->setWidth(0.6);                  // width of bars
    mBars->setPen(Qt::NoPen);              // no outline
    mBars->setBrush(QColor(40, 110, 255)); // fill color
    mBarsError = new QCPBars(mPlot->xAxis, mPlot->yAxis);
    mBarsError->setWidth(0.6);                  // width of bars
    mBarsError->setPen(Qt::NoPen);              // no outline
    mBarsError->setBrush(QColor("red")); // fill color

    ui->vlChart->addWidget(mPlot);

    connect(ui->pbClear, &QPushButton::clicked, this, &DlgHistory::clear);
}

DlgHistory::~DlgHistory()
{
    delete ui;
}

void DlgHistory::onTableContextMenu(QPoint pos)
{
    if (mModel){
        if (mModel->rowCount()>0){
            mCopyAction->setEnabled(true);
        }else {
            mCopyAction->setEnabled(false);
        }
    }
    mTableMenu->popup(ui->tableView->mapToGlobal(pos));
}

void DlgHistory::onCopyAction(bool checked)
{
    Q_UNUSED(checked)
    qDebug() << "TODO: onCopyAction";
}

void DlgHistory::initMenu()
{
    mTableMenu=new QMenu();
    mCopyAction = new QAction("Copy(TODO)");
    connect(mCopyAction, &QAction::triggered, this, &DlgHistory::onCopyAction);
    mTableMenu->addAction(mCopyAction);
}

void DlgHistory::setStartTime(double starttime)
{
    mPlot->xAxis->setRange(starttime, starttime+30);
}

void DlgHistory::setDataModel(HistoryModel *model)
{
    mModel = model;
    ui->tableView->setModel(model);
    ui->tableView->setColumnWidth(HistoryColumn::ID, 50);
    ui->tableView->setColumnWidth(HistoryColumn::DateTime, 150);
    ui->tableView->setColumnWidth(HistoryColumn::Url, 200);

}

void DlgHistory::addData(double x, double y)
{
    mBars->addData(x, y);
    // Rescale axes to fit all data
    // mPlot->rescaleAxes();
    mPlot->xAxis->rescale();

    // Redraw
    mPlot->replot();
}

void DlgHistory::addErrorData(double x, double y)
{
    if (mBarsError) {
        mBarsError->addData(x, y);
    }
    mPlot->xAxis->rescale();
    mPlot->replot();
}

void DlgHistory::addDatas(QVector<double> keys, QVector<double> values)
{
    double maxT = *std::max_element(keys.begin(), keys.end());// x: max time
    mPlot->xAxis->setRange(0, maxT);
    mBars->addData(keys, values);
    // Rescale axes to fit all data
    mPlot->rescaleAxes();
    // Redraw
    mPlot->replot();

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

void DlgHistory::clear()
{
    if (mModel){
        mModel->clear();
    }
    if (mBars){
        QCPBarsDataContainer* dataContainer =mBars->data().data();
        if (dataContainer){
            dataContainer->clear();
        }
    }
    if (mBarsError){
        QCPBarsDataContainer* errordataContainer = mBarsError->data().data();
        if (errordataContainer){
            errordataContainer->clear();
        }
    }
    mPlot->replot();
}
