#include "segdconverterwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SegdConverterWindow w;
    w.show();

    return a.exec();
}
