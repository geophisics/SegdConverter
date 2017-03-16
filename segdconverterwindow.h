#ifndef SEGDCONVERTERWINDOW_H
#define SEGDCONVERTERWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QThread>
#include <Workers/segyworker.h>
#include <auxviewdialog.h>
namespace Ui {
class SegdConverterWindow;
}

class SegdConverterWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SegdConverterWindow(QWidget *parent = 0);
    ~SegdConverterWindow();

private:

    Ui::SegdConverterWindow *ui;
    QSettings* settings;
    QString WorkDir;
    void readSettings();
    void saveSettings();
    void readConvertParamsSettings();
    QStringList AttributeColumns;
    QThread *p_myThread;
    bool online;
    //SegyWorker *p_segyWorker;
    QString spsFile, rpsFile, xpsFile;
    QPointer<AuxViewDialog> viewDialog;
signals:
    void stopSignal();
protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void openConvertParametersDialog();
    void openBinningParametersDialog();
    void openWindowsParametersDialog();
    void openTableViewParametersDialog();
    void selectSpsFile(bool checked);
    void selectRpsFile(bool checked);
    void selectXpsFile(bool checked);
    void openDataSlot();
    void saveDataSlot();
    void saveAttributesFileSlot();
    void runActionSlot();
    void disableStop(bool disable);
    //void stopActionSlot();
    void convertingEnded();
    void openAuxParametersDialog();
    void recieveInfoMessage(const QString &message,const QColor &color);
    void setColumnsForView (const QStringList &columns);
    void receiveFfidDataSlot (const FfidData &data);
    void receiveSeisAttributes(SeisAttributes *attr, const int &winNb);
    void receiveRelation(QString name, float value, bool correct);
    void receiveAuxStatus (const bool &status);
    void receiveTestStatus(const float &percent, const QColor &color);
    void receiveSomeError(const QString &err);
    void aboutQtSlot();
    void aboutSlot();
    //void slot1();

private:
    void runSegy();
    void runSegyOnline();
    void runCst();
    void runCstOnline();
    void setViewAuxesDialog(BaseWorker *worker);
};

#endif // SEGDCONVERTERWINDOW_H
