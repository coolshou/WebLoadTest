#ifndef DLGCONFIG_H
#define DLGCONFIG_H

#include <QDialog>

namespace Ui {
class DlgConfig;
}

class DlgConfig : public QDialog
{
    Q_OBJECT

public:
    explicit DlgConfig(QWidget *parent = nullptr);
    ~DlgConfig();
    void setClearCatch(bool checked);
    void setLoadTimeout(int timeiout);
    void setRandomLink(bool random);
    void setMaxLinks(int value);
signals:
    void sigClearCatch(bool checked);
    void sigLoadTimeout(int timeiout);
    void sigRandomLink(bool random);
    void sigMaxLinks(int value);
protected:
    void changeEvent(QEvent *e);
private slots:
    void onAccepted();
private:
    Ui::DlgConfig *ui;
};

#endif // DLGCONFIG_H
