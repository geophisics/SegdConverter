#ifndef BINNINGPARAMETERSDIALOG_H
#define BINNINGPARAMETERSDIALOG_H

#include <QDialog>
#include <QSettings>
namespace Ui {
class BinningParametersDialog;
}

class BinningParametersDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BinningParametersDialog(QWidget *parent = 0);
    ~BinningParametersDialog();

private:
    Ui::BinningParametersDialog *ui;
    QSettings *settings;
    void readSettings();
private slots:
    void saveSettings();

};

#endif // BINNINGPARAMETERSDIALOG_H
