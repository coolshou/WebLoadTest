#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDirIterator>
#include <QTimer>
#include <qtwebenginecoreglobal.h>
#include <QMessageBox>
#include <QRandomGenerator>

#include "versions.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->cbConutinus, &QCheckBox::toggled, this, &MainWindow::onConutinusClick);
    // connect(ui->sbMaxTimes, &QSpinBox::valueChanged, this, &MainWindow::onValueChanged);
    mDlgConfig = new DlgConfig();
    connect(mDlgConfig, &DlgConfig::sigClearCatch, this, &MainWindow::setClearCatch);
    connect(mDlgConfig, &DlgConfig::sigLoadTimeout, this, &MainWindow::setLoadTimeout);
    connect(mDlgConfig, &DlgConfig::sigRandomLink, this, &MainWindow::setRandomLink);
    connect(mDlgConfig, &DlgConfig::sigMaxLinks, this, &MainWindow::setMaxLinks);
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
    customPage = new QWebEnginePage(profile, this);
    mWeb->setPage(customPage);
    ui->vlWeb->addWidget(mWeb);
    ui->vlWeb->setStretch(1,1);
    connect(mWeb, &QWebEngineView::loadProgress, this, &MainWindow::onLoadProgress);
    connect(mWeb, &QWebEngineView::loadStarted, this, &MainWindow::onLoadStarted);
    connect(mWeb, &QWebEngineView::loadFinished, this, &MainWindow::onLoadFinished);
    connect(mWeb, &QWebEngineView::renderProcessTerminated, this, &MainWindow::onRenderProcessTerminated);
    connect(ui->pbLoad, &QPushButton::clicked, this, &MainWindow::onLoadClick);
    connect(ui->pbHistory, &QPushButton::clicked, this, &MainWindow::onHistoryClick);
    connect(ui->pbStop, &QPushButton::clicked, this, &MainWindow::onStopClick);
    connect(ui->actionConfig, &QAction::triggered, this, &MainWindow::onConfigClick);
    connect(ui->actionClearCatch, &QAction::triggered, this, &MainWindow::onClearCatchClick);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onAboutClick);
    connect(this, &MainWindow::addHistoryEntry, mHistoryModel, &HistoryModel::addEntry);
    connect(this, &MainWindow::addBarChartData, mDlgHistory, &DlgHistory::addData);
    connect(this, &MainWindow::addErrorBarData, mDlgHistory, &DlgHistory::addErrorData);
    connect(this, &MainWindow::sigClose, mDlgHistory, &DlgHistory::close);
    connect(this, &MainWindow::sigClose, mDlgConfig, &DlgConfig::close);
    timeouttimer= new QTimer();
    connect(timeouttimer, &QTimer::timeout, this, &MainWindow::onLoadTimeout);
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
    QString url ="";
    if (mRandomLink && (mCurrentLinks<=mMaxLinks)){
        url = mRandomLinkUrl;
        //only use RandomLinkUrl once
        setRandomLinkUrl("");
        qDebug() << "mCurrentLinks:" << QString::number(mCurrentLinks)
                 << " url:" << url;
    }
    if (url.isEmpty()){
        url = ui->leUrl->text();
        mCurrentLinks = 0;
        if (url.isEmpty()){
            ui->leUrl->setFocus();
            return;
        }
    }

    // qDebug() << "initCatchSize:" << initCatchSize;
    if (bClearCatch){
        clearCatch();
        qint64 cachesize = getFolderSize(cachePath);
        if (cachesize> initCatchSize){
            //
            // qDebug()<< "TODO: have catch:" << cachesize;
            // return;
        }
    }
    // delay 1 sec for catch cleanup? after 6.7 should use signal clearHttpCacheCompleted()
    timeouttimer->start(loadTimeout*1000);
    QTimer::singleShot(1000, this, [this, url](){
        // mWeb->load(QUrl(url));
        mWeb->setUrl(QUrl(url));
    });
}

void MainWindow::updateStatus(QString msg, int showsec)
{
    ui->statusbar->showMessage(msg, showsec*1000);
}

void MainWindow::clearCatch()
{
    profile->clearHttpCache();
}

void MainWindow::loadSetting()
{
    mSetting->beginGroup("main");
    bClearCatch = mSetting->value("ClearCatch", true).toBool();
    maxTime = mSetting->value("MaxTimes", 1).toInt();
    ui->sbMaxTimes->setValue(maxTime);
    ui->leUrl->setText(mSetting->value("Url", "https://doc.qt.io/").toString());
    loadTimeout = mSetting->value("loadTimeout", 30).toInt();
    mConutinus =  mSetting->value("loadConutinus", false).toBool();
    ui->cbConutinus->setChecked(mConutinus);
    mRandomLink =  mSetting->value("loadRandomLink", false).toBool();
    mMaxLinks =  mSetting->value("MaxLinks", 1).toInt();
    mSetting->endGroup();
}

void MainWindow::saveSetting()
{
    mSetting->beginGroup("main");
    mSetting->setValue("ClearCatch", bClearCatch);
    mSetting->setValue("MaxTimes", ui->sbMaxTimes->value());
    mSetting->setValue("Url", ui->leUrl->text());
    mSetting->setValue("loadTimeout", loadTimeout);
    mSetting->setValue("loadConutinus", mConutinus);
    mSetting->setValue("loadRandomLink", mRandomLink);
    mSetting->setValue("MaxLinks", mMaxLinks);
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
    updateStartBtn(false);
}

void MainWindow::onLoadFinished(bool ok)
{
    loadtimes++;
    updateStartBtn(true);
    qint64 elapsed_ms = timer->elapsed();
    double elapsed_val= elapsed_ms/1000.0;
    QString elapsed = QString::number(elapsed_val, 'f', 3);
    QString status="";
    if (ok){
        // customPage->
        status = "OK";
        emit addBarChartData(mStartTime.toSecsSinceEpoch(), elapsed_val);
        if (mRandomLink){
            QString script = "Array.from(document.querySelectorAll('a')).map(a => a.href).join('\\n')";
            mWeb->page()->runJavaScript(script, [this](const QVariant& result) {
                if (result.isValid()) {
                    QString linkUrls = result.toString();
                    // Convert the single string into a list of separate links
                    QStringList linksList = linkUrls.split('\n', Qt::SkipEmptyParts);
                    QString link = this->getRandomLink(linksList);
                    this->setRandomLinkUrl(link);
                }else{
                    qWarning() << "runJavaScript returned an invalid result.";
                }
            });
        }
    }else {
        status = "ERROR";
        //TODO: show to get error string of web?
        qDebug() << "load error: title:" << customPage->title();
        emit addErrorBarData(mStartTime.toSecsSinceEpoch(), 60);
    }
    emit addHistoryEntry(mStartTime.toString("yyyy-MM-dd HH:mm:ss.zzz"),
                         mWeb->url().toString(), elapsed, status);

    updateStatus(QString("(%1/%2)Elapsed time(sec): %3").arg(QString::number(loadtimes),
                                                             QString::number(maxTime),
                                                             elapsed));
    if (((loadtimes < maxTime)|mConutinus) & !mStop){
        startTest();
    }else{
        timeouttimer->stop();
    }
}

void MainWindow::onLoadTimeout()
{
    emit addHistoryEntry(mStartTime.toString("yyyy-MM-dd HH:mm:ss.zzz"),
                         mWeb->url().toString(),
                         QString::number(loadTimeout), "TIMEOUT");
    emit addErrorBarData(mStartTime.toSecsSinceEpoch(), 60);
    if (((loadtimes < maxTime)|mConutinus) & !mStop){
        startTest();
    }else{
        timeouttimer->stop();
    }
}

void MainWindow::onRenderProcessTerminated(QWebEnginePage::RenderProcessTerminationStatus terminationStatus, int exitCode)
{
    qDebug() << "onRenderProcessTerminated:" << terminationStatus << " exitCode:" << exitCode;
}

void MainWindow::onLoadClick(bool checked)
{
    Q_UNUSED(checked)
    loadtimes = 0;
    mCurrentLinks = 0;
    maxTime = ui->sbMaxTimes->value();
    if (mDlgHistory){
        mDlgHistory->setStartTime(mStartTime.toMSecsSinceEpoch()/1000.0);
    }
    ui->pbStop->setEnabled(true);
    startTest();

}

void MainWindow::onHistoryClick(bool checked)
{
    Q_UNUSED(checked)
    if (mDlgHistory){
        mDlgHistory->exec();
    }
}

void MainWindow::onStopClick(bool checked)
{
    Q_UNUSED(checked)
    mStop = true;
    mWeb->stop();
    if (timeouttimer->isActive()){
        timeouttimer->stop();
    }
}
void MainWindow::onConutinusClick(bool checked)
{
    ui->wTimes->setEnabled(!checked);
    mConutinus = checked;
}

void MainWindow::onClearCatchClick(bool checked)
{
    Q_UNUSED(checked)
    clearCatch();
}

void MainWindow::onAboutClick(bool checked)
{
    Q_UNUSED(checked)
    QString msg = QString("%1: %2\n  Chromium Version: %3\n  QWebEngine Version: %4")
                      .arg(WEBLOADTEST, WEBLOADTEST_VERSION,
                           qWebEngineChromiumVersion(),
                           qWebEngineVersion());
    msg.append("\n\nAuther: jimmy");
    QMessageBox::information(this, "About", msg);
}

void MainWindow::setClearCatch(bool checked)
{
    bClearCatch = checked;
}

void MainWindow::setLoadTimeout(int timeout)
{
    loadTimeout = timeout;
}

void MainWindow::setRandomLink(bool random)
{
    mRandomLink = random;
}

void MainWindow::setMaxLinks(int value)
{
    mMaxLinks = value;
}

void MainWindow::onConfigClick(bool checked)
{
    Q_UNUSED(checked)
    if (mDlgConfig){
        mDlgConfig->setClearCatch(bClearCatch);
        mDlgConfig->setRandomLink(mRandomLink);
        mDlgConfig->setMaxLinks(mMaxLinks);
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

void MainWindow::updateStartBtn(bool start)
{
    ui->pbLoad->setEnabled(start);
    // ui->pbStop->setEnabled(!start);
}

QString MainWindow::getRandomLink(QStringList& linksList)
{
    // 1. Check if the list is empty
    if (linksList.isEmpty()) {
        qWarning() << "Error: The list is empty.";
        return QString(); // Return an empty string
    }

    // 2. Determine the range
    int size = linksList.size();

    // 3. Generate a random index
    // QRandomGenerator::global()->bounded(size) generates a random integer
    // in the range [0, size - 1].
    int randomIndex = QRandomGenerator::global()->bounded(size);

    // 4. check if url is start with http:// or https://
    QString url = linksList.at(randomIndex);
    if ((!url.startsWith("http://"))&&(!url.startsWith("https://"))){
        linksList.removeAt(randomIndex);
        size = linksList.size();
        randomIndex = QRandomGenerator::global()->bounded(size);
        url = linksList.at(randomIndex);
    }
    // 5. Return the string at the random index
    return url;
}

void MainWindow::setRandomLinkUrl(QString value)
{
    qDebug() << "setRandomLinkUrl: " << value;
    if (!value.isEmpty()){
        mCurrentLinks++;
        mRandomLinkUrl = value;
    }
}
