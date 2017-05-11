#include "testviewdialog.h"
#include "ui_testviewdialog.h"

TestViewDialog::TestViewDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TestViewDialog)
{
    ui->setupUi(this);
}

TestViewDialog::~TestViewDialog()
{
    delete ui;
}
