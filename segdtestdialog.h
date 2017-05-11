#ifndef SEGDTESTDIALOG_H
#define SEGDTESTDIALOG_H

#include <QDialog>

namespace Ui {
class SegdTestDialog;
}

class SegdTestDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SegdTestDialog(QWidget *parent = 0);
    ~SegdTestDialog();

private:
    Ui::SegdTestDialog *ui;
};

#endif // SEGDTESTDIALOG_H
