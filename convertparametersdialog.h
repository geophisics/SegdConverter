#ifndef CONVERTPARAMETERSDIALOG_H
#define CONVERTPARAMETERSDIALOG_H

#include <QDialog>
#include <QSettings>
namespace Ui {
class ConvertParametersDialog;
}

class ConvertParametersDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConvertParametersDialog(QWidget *parent = 0);
    ~ConvertParametersDialog();

private:
    Ui::ConvertParametersDialog *ui;
    QSettings* settings;
    void readSettings();


private slots:
    void selectBackupFolderSlot();
    void selectExclusionFileSlot();
    void backupCheckBoxStateChangedSlot(int state);
    void exclusionCheckBoxStateChangedSlot(int state);
    void maxFilesCheckBoxStateChangedSlot(int state);
    void userMpFactorRadioButtonToogled(bool checked);
    void saveSettings();

};

#endif // CONVERTPARAMETERSDIALOG_H
