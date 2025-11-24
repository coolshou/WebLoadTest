#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDirIterator>
#include <QTimer>
#include <qtwebenginecoreglobal.h>
#include <QMessageBox>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mDlgConfig = new DlgConfig();
    connect(mDlgConfig, &DlgConfig::sigClearCatch, this, &MainWindow::setClearCatch);
    mDlgHistory = new DlgHistory();
    mHistoryModel = new HistoryModel();
    mDlgHistory->setDataModel(mHistoryModel);
    // mDlgHistory->addData(0,5); //test
    /* test data
    QVector<double> keys, values;
    keys   << 1 << 2 << 3 << 4 << 5;
    values << 5 << 7 << 3 << 9 << 6;
    mDlgHistory->addDatas(keys, values);
    */

    QString settingfilename = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)
                              + QDir::separator() + "webload.ini";
    mSetting = new QSettings(settingfilename, QSettings::IniFormat);
    loadSetting();
    timer = new QElapsedTimer();
    mWeb = new QWebEngineView(this);
    profile = new QWebEngineProfile("webload", this);
    cachePath = profile->cachePath();
    clearCatch();
    // qDebug() << "cachePath: " << cachePath << " size:" << getFolderSize(cachePath);
    customPage = new QWebEnginePage(profile, this);
    mWeb->setPage(customPage);
    // connect(profile, &QWebEngineProfile::);
    ui->vlWeb->addWidget(mWeb);
    connect(mWeb, &QWebEngineView::loadProgress, this, &MainWindow::onLoadProgress);
    connect(mWeb, &QWebEngineView::loadStarted, this, &MainWindow::onLoadStarted);
    connect(mWeb, &QWebEngineView::loadFinished, this, &MainWindow::onLoadFinished);
    connect(mWeb, &QWebEngineView::renderProcessTerminated, this, &MainWindow::onRenderProcessTerminated);
    connect(ui->pbLoad, &QPushButton::clicked, this, &MainWindow::onLoadClick);
    connect(ui->pbHistory, &QPushButton::clicked, this, &MainWindow::onHistoryClick);
    connect(ui->actionConfig, &QAction::triggered, this, &MainWindow::onConfigClick);
    connect(ui->actionClearCatch, &QAction::triggered, this, &MainWindow::onClearCatchClick);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onAboutClick);
    connect(this, &MainWindow::addHistoryEntry, mHistoryModel, &HistoryModel::addEntry);
    connect(this, &MainWindow::addBarChartData, mDlgHistory, &DlgHistory::addData);
    connect(this, &MainWindow::addErrorBarData, mDlgHistory, &DlgHistory::addErrorData);
    connect(this, &MainWindow::sigClose, mDlgHistory, &DlgHistory::close);
    connect(this, &MainWindow::sigClose, mDlgConfig, &DlgConfig::close);

}

MainWindow::~MainWindow()
{
    delete customPage;
    delete mWeb;
    delete ui;
}

void MainWindow::startTest()
{
    // TODO: random url?
    QString url = ui->leUrl->text();
    if (url.isEmpty()){
        ui->leUrl->setFocus();
        return;
    }
    // qDebug() << "initCatchSize:" << initCatchSize;
    if (bClearCatch){
        clearCatch();
        int cachesize = getFolderSize(cachePath);
        if (cachesize> initCatchSize){
            //
            // qDebug()<< "have catch:" << cachesize;
            // return;
        }
    }
    // mWeb->load(QUrl(url));
    // delay 1 sec for catch cleanup? after 6.7 should use signal clearHttpCacheCompleted()
    QTimer::singleShot(1000, this, [this, url](){
        mWeb->load(QUrl(url));
    });
}

void MainWindow::updateStatus(QString msg, int showsec)
{
    ui->statusbar->showMessage(msg, showsec*1000);
}

void MainWindow::clearCatch()
{
    profile->clearHttpCache();
    // how to known catch is cleared??
    // QTimer::singleShot(1000, this, [this](){
    //     initCatchSize = getFolderSize(cachePath);
    //     qDebug() << "clearCatch cachePath: " << cachePath << " size:" << initCatchSize;
    // });

}

void MainWindow::loadSetting()
{
    mSetting->beginGroup("main");
    bClearCatch = mSetting->value("ClearCatch", true).toBool();
    ui->sbMaxTimes->setValue(mSetting->value("MaxTimes", 1).toInt());
    ui->leUrl->setText(mSetting->value("Url", "https://doc.qt.io/").toString());
    mSetting->endGroup();
}

void MainWindow::saveSetting()
{
    mSetting->beginGroup("main");
    mSetting->setValue("ClearCatch", bClearCatch);
    mSetting->setValue("MaxTimes", ui->sbMaxTimes->value());
    mSetting->setValue("Url", ui->leUrl->text());
    mSetting->endGroup();

    mSetting->sync();

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)
    saveSetting();
    emit sigClose();
}

void MainWindow::onLoadProgress(int progress)
{
    ui->progressBar->setValue(progress);
}

void MainWindow::onLoadStarted()
{
    ui->progressBar->setValue(0);
    qint64 ms =  QDateTime::currentMSecsSinceEpoch();
    mStartTime = QDateTime::fromMSecsSinceEpoch(ms);
    timer->start();
    ui->pbLoad->setText("Stop");
}

void MainWindow::onLoadFinished(bool ok)
{
    loadtimes++;
    ui->pbLoad->setText("Load");
    ui->pbLoad->setChecked(false);
    qint64 elapsed_ms = timer->elapsed();
    double elapsed_val= elapsed_ms/1000.0;
    QString elapsed = QString::number(elapsed_val, 'f', 3);
    QString status="";
    if (ok){
        status = "OK";
        emit addBarChartData(mStartTime.toSecsSinceEpoch(), elapsed_val);
    }else {
        status = "ERROR";
        //TODO: show to get error string of web?
        qDebug() << "load error: title:" << customPage->title();
        emit addErrorBarData(mStartTime.toSecsSinceEpoch(), 60);
    }
    emit addHistoryEntry(mStartTime.toString("yyyy-MM-dd HH:mm:ss.zzz"),
                         mWeb->url().toString(), elapsed, status);

    updateStatus("Elapsed time(sec):" + elapsed);
    if (loadtimes < maxTime){
        startTest();
    }
}

void MainWindow::onRenderProcessTerminated(QWebEnginePage::RenderProcessTerminationStatus terminationStatus, int exitCode)
{
    qDebug() << "onRenderProcessTerminated:" << terminationStatus << " exitCode:" << exitCode;
}

void MainWindow::onLoadClick(bool checked)
{
    if (checked){
        loadtimes = 0;
        maxTime = ui->sbMaxTimes->value();
        if (mDlgHistory){
            mDlgHistory->setStartTime(mStartTime.toMSecsSinceEpoch()/1000.0);
        }
        startTest();
    }else{
        maxTime = 0;
    }
}

void MainWindow::onHistoryClick(bool checked)
{
    Q_UNUSED(checked)
    if (mDlgHistory){
        mDlgHistory->exec();
    }
}

void MainWindow::onClearCatchClick(bool checked)
{
    Q_UNUSED(checked)
    clearCatch();
}

void MainWindow::onAboutClick(bool checked)
{
    Q_UNUSED(checked)
    QString msg = QString("Chromium Version: %1\nQWebEngine Version: %2")
                      .arg(qWebEngineChromiumVersion(),
                           qWebEngineVersion());
    QMessageBox::information(this, "About",
                             msg);
}

void MainWindow::setClearCatch(bool checked)
{
    bClearCatch = checked;
}

void MainWindow::onConfigClick(bool checked)
{
    Q_UNUSED(checked)
    if (mDlgConfig){
        mDlgConfig->setClearCatch(bClearCatch);
        mDlgConfig->show();
    }
}
/**
 * @brief Calculates the total size of a directory and its contents recursively.
 * @param directoryPath The path to the folder.
 * @return The total size in bytes (qint64).
 */
qint64 MainWindow::getFolderSize(const QString &directoryPath)
{
    qint64 totalSize = 0;

    // Set up the iterator to walk the directory structure recursively.
    QDirIterator it(directoryPath,
                    QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot,
                    QDirIterator::Subdirectories);

    while (it.hasNext()) {
        it.next();
        QFileInfo info = it.fileInfo();

        if (info.isFile()) {
            // Sum the size of files.
            totalSize += info.size();
        }
        // Note: The size of a directory entry itself (info.isDir()) is usually
        // very small or 0, and not relevant to the folder's "content size."
    }

    return totalSize;
}
