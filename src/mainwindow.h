#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWebEngineWidgets>
#include <QWebEngineProfile>
#include <QWebEnginePage>
#include <QElapsedTimer>
#include <QSettings>

#include <QDateTime>

#include "src/dlgconfig.h"
#include "src/dlghistory.h"
#include "src/historymodel.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void startTest();
    void updateStatus(QString msg, int showsec=0);
    void clearCatch();
    void loadSetting();
    void saveSetting();
    QString getRandomLink(const QStringList &linksList);
    void setRandomLinkUrl(QString value);
signals:
    void sigClose();
    void addHistoryEntry(const QString &dateTime, const QString &url,
                         const QString &loadSec, const QString &status,
                         const QString &comment="");
    void addBarChartData(double x, double y);
    void addErrorBarData(double x, double y);
protected:
    void closeEvent(QCloseEvent *event)override;
private slots:
    void onLoadProgress(int progress);
    void onLoadStarted();
    void onLoadFinished(bool ok);
    void onLoadTimeout();
    void onRenderProcessTerminated(QWebEnginePage::RenderProcessTerminationStatus terminationStatus,
                                   int exitCode);
    void onLoadClick(bool checked);
    void onHistoryClick(bool checked);
    void onStopClick(bool checked);
    void onConutinusClick(bool checked);
    void onConfigClick(bool checked);
    void onClearCatchClick(bool checked);
    void onAboutClick(bool checked);
    void setClearCatch(bool checked);
    void setLoadTimeout(int timeout);
    void setRandomLink(bool random);
    void setMaxLinks(int value);

private:
    qint64 getFolderSize(const QString& directoryPath);
    void updateStartBtn(bool start);
private:
    Ui::MainWindow *ui;
    QSettings *mSetting;
    QWebEngineView *mWeb;
    QWebEngineProfile *profile;
    QString cachePath;
    int initCatchSize;
    QWebEnginePage *customPage;
    QElapsedTimer *timer;
    bool bClearCatch;
    QDateTime mStartTime;
    int loadtimes;
    int maxTime;
    bool mConutinus;
    DlgConfig *mDlgConfig;
    HistoryModel *mHistoryModel;
    DlgHistory *mDlgHistory;
    int loadTimeout;
    QTimer *timeouttimer;
    bool mRandomLink;
    int mCurrentLinks;
    int mMaxLinks;
    QString mRandomLinkUrl;
    bool mStop=false;
};
#endif // MAINWINDOW_H
