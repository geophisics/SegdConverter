#include "segdtestdialog.h"
#include "ui_segdtestdialog.h"

SegdTestDialog::SegdTestDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SegdTestDialog)
{
    ui->setupUi(this);
}

SegdTestDialog::~SegdTestDialog()
{
    delete ui;
}
