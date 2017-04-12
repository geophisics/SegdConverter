#ifndef TABLEVIEWDIALOG_H
#define TABLEVIEWDIALOG_H

#include <QDialog>
namespace Ui {
class TableViewDialog;
}

class TableViewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TableViewDialog(QWidget *parent = 0);
    TableViewDialog(QStringList *rows, QSet<int> *checked, QWidget *parent=0);
    ~TableViewDialog();

private:
    Ui::TableViewDialog *ui;
    QSet<int> *checkedRows;
private slots:
    void okClicked();
    void selectAllSlot();
    void diselectAllSlot();

};

#endif // TABLEVIEWDIALOG_H
