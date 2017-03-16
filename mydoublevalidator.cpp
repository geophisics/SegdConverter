#include "mydoublevalidator.h"

MyDoubleValidator::MyDoubleValidator(QObject * parent):QDoubleValidator(parent)
{

}
MyDoubleValidator::MyDoubleValidator(double bottom, double top, int decimals, QObject *parent):
    QDoubleValidator(bottom,top,decimals,parent)
{

}
QValidator::State MyDoubleValidator::validate(QString &s,int &i) const
{
    Q_UNUSED(i);
    if (s.isEmpty() || (s == "-" && bottom()<0)) {
        return QValidator::Intermediate;
    }

    QChar decimalPoint = locale().decimalPoint();
    //QChar decimalPoint = '.';
    if(s.indexOf(decimalPoint) != -1) {
        int charsAfterPoint = s.length() - s.indexOf(decimalPoint) - 1;

        if (charsAfterPoint > decimals()) {
            return QValidator::Invalid;
        }
    }

    bool ok;
    double d = locale().toDouble(s, &ok);

    if (ok && d >= bottom() && d <= top()) {
        return QValidator::Acceptable;
    } else {
        return QValidator::Invalid;
    }
}

