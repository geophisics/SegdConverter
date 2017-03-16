#ifndef TABLEVIEWDIALOG_H
#define TABLEVIEWDIALOG_H

#include <QDialog>
class QSettings;
namespace Ui {
class TableViewDialog;
}

class TableViewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TableViewDialog(QWidget *parent = 0);
    ~TableViewDialog();

private:
    Ui::TableViewDialog *ui;
    QSettings *settings;
    QStringList readSettings();
    void setCheckedRows(QStringList rows);
private slots:
    void saveSettings(const QStringList checkedRows);
    void okClicked();
signals:
    sendTableColumns(QStringList list);
};

#endif // TABLEVIEWDIALOG_H
