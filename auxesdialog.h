#ifndef AUXESDIALOG_H
#define AUXESDIALOG_H

#include <QDialog>
#include <QSettings>
namespace Ui {
class AuxesDialog;
}

class AuxesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AuxesDialog(QWidget *parent = 0);
    ~AuxesDialog();

private:
    Ui::AuxesDialog *ui;

    void readSettings();

    void setTimeBreakSettings(QVector<float> traceData, const int &sInt);
    void setConfirmedTimeBreakSettings(QVector<float> traceData, const int &sInt);

    QSettings *settings;
private slots:
    void openVibroSegdSlot();
    void openExplSegdSlot();
    void writeSettings();
    void getSpectrum(std::vector<float> vec,const int &sampleRate,const int &dump);
};

#endif // AUXESDIALOG_H
