#ifndef SEGDCONVERTERWINDOW_H
#define SEGDCONVERTERWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QThread>
#include <Workers/segyworker.h>
#include <Workers/cstworker.h>
#include <auxviewdialog.h>
#include <testviewdialog.h>
#include <Models/attributesmodel.h>
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
    QPointer<TestViewDialog> testViewDialog;
    AttributesModel *attr_model;
    AttributesSortFilterProxyModel *attr_sortFilterModel;

    TestMap tMap;
    volatile bool running;


    QSignalMapper *mapper;

signals:
    void stopSignal();
protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void openConvertParametersDialog();
    void openBinningParametersDialog();
    void openWindowsParametersDialog();
    void openTableViewParametersDialog();
    void eraseCountedAttributes();
    void selectSpsFile(bool checked);
    void selectRpsFile(bool checked);
    void selectXpsFile(bool checked);
    void openDataSlot();
    void saveDataSlot();
    void saveAttributesFileSlot();
    void saveAsAttributesSlot();
    void runActionSlot();
    void disableStop(bool disable);
    //void stopActionSlot();
    void convertingEnded();
    void openAuxParametersDialog(bool checked);
    void openTestParametersDialog(const bool &checked);
    void recieveInfoMessage(const QString &message,const QColor &color);

    void firstFilterComboItemChanged(const int &i);
    void secondFilterComboItemChanged(const int &i);
    void thirdFilterComboItemChanged(const int &i);

    void receiveSomeError(const QString &err);
    void aboutQtSlot();
    void aboutSlot();
    void resetTableViewPositions();

    void slot1(const int &i);
    void setColumnsForFiltering();

    void firstRangeChanged();
    void secondRangeChanged();
    void thirdRangeChanged();
    void filtersEnabled(const bool &b);
    void enableSorting(const bool &b);


    void tableViewItemDoubleClicked(QModelIndex index);

private:
    void runSegy();
    void runCst();
    void startThread(BaseWorker *worker);
    void setViewAuxesDialog(BaseWorker *worker);
    void setTestViewDialog(BaseWorker *worker);
    void saveAttributes(const QString &path);
    void setValidators();
    void filtersConnecting();
};

#endif // SEGDCONVERTERWINDOW_H
