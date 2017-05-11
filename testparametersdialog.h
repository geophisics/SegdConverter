#ifndef TESTPARAMETERSDIALOG_H
#define TESTPARAMETERSDIALOG_H

#include <QDialog>
#include <QSettings>
namespace Ui {
class TestParametersDialog;
}

class TestParametersDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TestParametersDialog(QSettings *p_settings, QWidget *parent = 0);
    ~TestParametersDialog();

private:
    Ui::TestParametersDialog *ui;
    QSettings *settings;
    void readSettings();
private slots:
    void saveSettings();
};

#endif // TESTPARAMETERSDIALOG_H
