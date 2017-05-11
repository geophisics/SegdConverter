#include "testparametersdialog.h"
#include "ui_testparametersdialog.h"

TestParametersDialog::TestParametersDialog(QSettings *p_settings, QWidget *parent):
    QDialog(parent),
    ui(new Ui::TestParametersDialog)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    settings = p_settings;
    readSettings();
    connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(saveSettings()));
}

TestParametersDialog::~TestParametersDialog()
{

    delete ui;
}

void TestParametersDialog::readSettings()
{
    settings->beginGroup("/TestSettings");
    ui->inRowSpinBox->setValue(settings->value("/MaxInRow",0).toInt());
    ui->inLineDoubleSpinBox->setValue(settings->value("/MaxInLine",0.0).toDouble());
    ui->allDoubleSpinBox->setValue(settings->value("/MaxAll").toDouble());
    settings->endGroup();
}

void TestParametersDialog::saveSettings()
{
    settings->beginGroup("/TestSettings");
    settings->setValue("/MaxInRow",ui->inRowSpinBox->value());
    settings->setValue("/MaxInLine",ui->inLineDoubleSpinBox->value());
    settings->setValue("/MaxAll",ui->allDoubleSpinBox->value());
    settings->endGroup();
    accept();
}
