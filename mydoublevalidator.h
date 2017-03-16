#ifndef MYDOUBLEVALIDATOR_H
#define MYDOUBLEVALIDATOR_H
#include <QDoubleValidator>

class MyDoubleValidator : public QDoubleValidator
{
public:
    MyDoubleValidator(QObject *parent);
    MyDoubleValidator(double bottom,double top,int decimals, QObject *parent);
    QValidator::State validate(QString &, int &) const;
};

#endif // MYDOUBLEVALIDATOR_H
