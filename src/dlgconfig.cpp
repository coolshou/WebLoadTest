#include "dlgconfig.h"
#include "ui_dlgconfig.h"

DlgConfig::DlgConfig(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgConfig)
{
    ui->setupUi(this);
    connect(this, &DlgConfig::accepted, this, &DlgConfig::onAccepted);
}

DlgConfig::~DlgConfig()
{
    delete ui;
}

void DlgConfig::setClearCatch(bool checked)
{
    ui->cbClearCatch->setChecked(checked);
}

void DlgConfig::setLoadTimeout(int timeiout)
{
    ui->sbLoadTimeout->setValue(timeiout);
}

void DlgConfig::setRandomLink(bool random)
{
    ui->cbRandomLink->setChecked(random);
}

void DlgConfig::setMaxLinks(int value)
{
    ui->sbMaxLinks->setValue(value);
}

void DlgConfig::changeEvent(QEvent *e)
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

void DlgConfig::onAccepted()
{
    emit sigClearCatch(ui->cbClearCatch->isChecked());
    emit sigLoadTimeout(ui->sbLoadTimeout->value());
    emit sigRandomLink(ui->cbRandomLink->isChecked());
}
