#ifndef WINDOWSPARAMETERSDIALOG_H
#define WINDOWSPARAMETERSDIALOG_H

#include <QDialog>
#include <QSettings>
class QTableWidgetItem;
//class QSignalMapper;
namespace Ui {
class WindowsParametersDialog;
}

class WindowsParametersDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WindowsParametersDialog(QWidget *parent = 0);
    ~WindowsParametersDialog();

private:
    Ui::WindowsParametersDialog *ui;
    QSettings *settings;
    //QSignalMapper *mapper;
    void addRow();
    void addRatioRow();
    void readSettings();
private slots:
    void saveSettings();
    void nbWindowsSpinBoxValueChanged(int val);
    void countAmplCheckBoxStateChanged(int state);
    void addRatioSlot();
    void displayMenu(QPoint pos);
    void minFrqValueChanged(int val);
    void maxFrqValueChanged (int val);
    //void windowsTableWidgetItemChanged(int row,int column);
};

#endif // WINDOWSPARAMETERSDIALOG_H
