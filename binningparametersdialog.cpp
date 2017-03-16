#include "binningparametersdialog.h"
#include "ui_binningparametersdialog.h"
#include "mydoublevalidator.h"
#include "limits.h"
#include "math.h"
#include <QDir>
#include <QMessageBox>
BinningParametersDialog::BinningParametersDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BinningParametersDialog)
{
    ui->setupUi(this);
    settings = new QSettings(QCoreApplication::applicationDirPath()+QDir::separator()+"config.ini",QSettings::IniFormat,this);
    ui->x0LineEdit->setValidator(new MyDoubleValidator(0,HUGE_VAL,2,ui->x0LineEdit));
    ui->y0LineEdit->setValidator(new MyDoubleValidator(0,HUGE_VAL,2,ui->y0LineEdit));
    ui->dXLineEdit->setValidator(new MyDoubleValidator(0,1000,2,ui->dXLineEdit));
    ui->dYLineEdit->setValidator(new MyDoubleValidator(0,1000,2,ui->dYLineEdit));
    ui->i0LineEdit->setValidator(new QIntValidator(1,10001,ui->i0LineEdit));
    ui->j0LineEdit->setValidator(new QIntValidator(1,10001,ui->j0LineEdit));
    ui->azXLineEdit->setValidator(new MyDoubleValidator(-6.28318531,6.28318531,8,ui->azXLineEdit));
    ui->azYLineEdit->setValidator(new MyDoubleValidator(-6.28318531,6.28318531,8,ui->azYLineEdit));
    readSettings();
}

BinningParametersDialog::~BinningParametersDialog()
{
    delete ui;
}
//сохраняем параметры
void BinningParametersDialog::saveSettings()
{
    double dX, dY;
    QString str = ui->dXLineEdit->text().replace(locale().decimalPoint(),'.');
    dX=str.toDouble();
    str = ui->dYLineEdit->text().replace(locale().decimalPoint(),'.');
    dY=str.toDouble();
    if (dX==0 || dY==0)
    {
        QMessageBox::warning(0, "Параметры бина", "Задайте корректные параметры бина");
        return;
    }
    settings->beginGroup("/BinningSettings");
    settings->setValue("/Xo",ui->x0LineEdit->text().replace(locale().decimalPoint(),'.'));
    settings->setValue("/Yo",ui->y0LineEdit->text().replace(locale().decimalPoint(),'.'));
    settings->setValue("/dX",ui->dXLineEdit->text().replace(locale().decimalPoint(),'.'));
    settings->setValue("/dY",ui->dYLineEdit->text().replace(locale().decimalPoint(),'.'));
    settings->setValue("/Io",ui->i0LineEdit->text());
    settings->setValue("/Jo",ui->j0LineEdit->text());
    settings->setValue("/azX",ui->azXLineEdit->text().replace(locale().decimalPoint(),'.'));
    settings->setValue("/azY",ui->azYLineEdit->text().replace(locale().decimalPoint(),'.'));
    settings->endGroup();
    accept();
}
//загружаем параметры
void BinningParametersDialog::readSettings()
{
    settings->beginGroup("/BinningSettings");
    ui->x0LineEdit->setText(settings->value("/Xo","0").toString().replace('.',locale().decimalPoint()));
    ui->y0LineEdit->setText(settings->value("/Yo","0").toString().replace('.',locale().decimalPoint()));
    ui->dXLineEdit->setText(settings->value("/dX","25").toString().replace('.',locale().decimalPoint()));
    ui->dYLineEdit->setText(settings->value("/dY","25").toString().replace('.',locale().decimalPoint()));
    ui->i0LineEdit->setText(settings->value("/Io","1001").toString());
    ui->j0LineEdit->setText(settings->value("/Jo","1001").toString());
    ui->azXLineEdit->setText(settings->value("/azX","0").toString().replace('.',locale().decimalPoint()));
    ui->azYLineEdit->setText(settings->value("/azY","0").toString().replace('.',locale().decimalPoint()));
    settings->endGroup();
}
