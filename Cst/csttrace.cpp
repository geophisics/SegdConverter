#include "csttrace.h"
#include "cstfile.h"
#include "math.h"
CstTrace::CstTrace(QObject *parent) : QObject(parent)
{

}
CstTrace::CstTrace(SegdTrace *segdTr, QObject *parent):QObject(parent)
{
    CstFile *cst = qobject_cast<CstFile*>(parent);
    //headers
    headers.resize(64);
    headers.fill(0);//= QVector(63);
    headers[0]=cst->getLength(); // время последнего ненулевого дискрета!!
    headers[1]=segdTr->getTraceHeader()->getExtendedFileNum();
    headers[8]=cst->getSampleRate();
    headers[9]=segdTr->getExtensionOne()->getSamplesPerTrace()*cst->getSampleRate()/1000;
    if (segdTr->getTraceHeader()->getTraceEdit()==2)
    {
        headers[10]=0;
    }
    else
    {
        headers[10]=3145735; //для [3,7] в заголовке !!!!
    }
    headers[13]=-1;
    headers[15]=7;
    if (segdTr->getExtensionSeven()->getChannelType()==9)
    {
        headers[16]=0;
    }
    else
    {
        headers[16]=segdTr->getTraceHeader()->getTraceNumber();
    }
    headers[17]=segdTr->getExtensionOne()->getReceiverLineNum()*10000+segdTr->getExtensionOne()->getReceiverPointNum();//QString(QString::number(segdTr->getExtensionOne()->getReceiverLineNum())+QString::number(segdTr->getExtensionOne()->getReceiverPointNum())).toInt();
    headers[20]=cst->getLine()*10000+cst->getPoint();//QString(QString::number(cst->getLine())+QString::number(cst->getPoint())).toInt();
    //------------------------------------------------
    // если плохие тесты Resistance или Tilt (Res = 9999.0; Tilt =100.0), то deviceTraceIdentifier =1 иначе 0
    if (segdTr->getExtensionThree()->getResistanceValue()==9999.0 || segdTr->getExtensionThree()->getTiltValue()==100.0)
    {
        headers[58]=1;
    }
    else
    {
        headers[58]=0;
    }
    //--------------------------------------------------


    headers[59]=(int)round(segdTr->getExtensionTwo()->getX());
    headers[60]=(int)round(segdTr->getExtensionTwo()->getY());
    headers[61]=(int)round(cst->getEasting());
    headers[62]=(int)round(cst->getNorthing());
    trace = QVector<float>(segdTr->getTraceData());
    maxAmpl =0.0;
    for (int i=0;i<trace.size();++i)
    {
        if (fabs(trace.value(i))>maxAmpl)
        {
            maxAmpl = trace.value(i);
        }
    }

}

double CstTrace::xRotating(const float azX, const float azY)
{
    return headers.value(41)*cos(azX)-headers.value(42)*sin(azY);
}
double CstTrace::yRotating(const float azX, const float azY)
{
    return headers.value(41)*sin(azX)+headers.value(42)*cos(azY);
}

double CstTrace::xRotating(const double x, const double y, const float azX, const float azY)
{
    return x*cos(azX)-y*sin(azY);
}
double CstTrace::yRotating(const double x, const double y, const float azX, const float azY)
{
    return x*sin(azX)-y*cos(azY);
}


//получаем линию/пикет приемника
int CstTrace::getReceiverLinePoint()
{
    return headers.value(17);
}

//получаем порядковый номер трассы
int CstTrace::getTraceNb()
{
    return headers.value(16);
}
//получаем статус трассы
int CstTrace::getTraceStatus()
{
    return headers.value(10);
}
//устанавливаем абсциссу X приемника

void CstTrace::setReceiverX(const uint &val)
{
    headers.replace(59,val);
}
//устанавливаем ординату Y приемника
void CstTrace::setReceiverY(const uint &val)
{
    headers.replace(60,val);
}
//устанавливаем абсциссу X источника
void CstTrace::setSourceX(const uint &val)
{
    headers.replace(61,val);
}
//устанавливаем ординату Y источника
void CstTrace::setSourceY(const uint &val)
{
    headers.replace(62,val);
}
//устанавливаем линию пикет источника
void CstTrace::setSourceLinePoint(const uint &lp)
{
    headers.replace(20,lp);
}

//устанавливаем линию пикет приемника
void CstTrace::setReceiverLinePoint(const uint &lp)
{
    headers.replace(17,lp);
}
//получаем абсциссу X приемника
int CstTrace::getReceiverX()
{
    return headers.value(59);
}
//получаем ординату Y приемника
int CstTrace::getReceiverY()
{
    return headers.value(60);
}
//получаем номер файла
int CstTrace::getFfid()
{
    return headers.value(1);
}
//получаем удаление
int CstTrace::getOffset()
{
    return headers.value(19);
}
int CstTrace::getTestsStatus()
{
    return headers.value(58);
}
QVector<float> CstTrace::getData()
{
    return trace;
}
//максимальное значение для трассы
float CstTrace::getMaxAmpl()
{
    return maxAmpl;
}
//cчитаем геометрию для трассы
void CstTrace::countGeometry()
{
    CstFile *cst = qobject_cast<CstFile*>(this->parent());
    int xDif = headers.value(61)-headers.value(59);  //sourceX-receiverX;
    xDif = pow(xDif,2);
    int yDif =headers.value(62)-headers.value(60);
    yDif = pow(yDif,2);
    headers[19]=sqrt(xDif + yDif);
    headers[42]=(headers.value(61)+headers.value(59))/2;
    headers[43]=(headers.value(62)+headers.value(60))/2;
    //offset = sqrt(xDif + yDif);
    float xOrigin = cst->getXorigin();
    float yOrigin = cst->getYorigin();
    float dX = cst->getdX();
    float dY = cst->getdY();
    int i0 = cst->getI0();
    int j0= cst->getJ0();
    float azX = cst->getAzX();
    float azY = cst->getAzY();
    azX = M_PI_2 - azX;
    azY = M_PI_2 - azY;
    headers[18] =roundl((((headers.value(42)-xOrigin)/sin(azX) - (headers.value(43)-yOrigin)/cos(azX))/(sin(azY)/sin(azX)-cos(azY)/cos(azX)))/dY) + i0;
    headers[3] =roundl((((headers.value(42)-xOrigin)/sin(azY) - (headers.value(43)-yOrigin)/cos(azY))/(sin(azX)/sin(azY)-cos(azX)/cos(azY)))/dX) +j0;
    headers[40] = (headers.value(3)-j0)*dX*sin(azX)+(headers.value(18)-i0)*dY*sin(azY)+xOrigin;
    headers[41] = (headers.value(3)-j0)*dX*cos(azX)+(headers.value(18)-i0)*dY*cos(azY)+yOrigin;
}
//записываем трассу
void CstTrace::writeTrace(QFile &fileOut)
{

    fileOut.open(QIODevice::Append);
    QDataStream dStream(&fileOut);
    dStream.setFloatingPointPrecision(QDataStream::SinglePrecision);
    int i=0;
    for ( ; i<13;++i)
    {
        dStream << headers.value(i);

    }
    dStream << maxAmpl;
    i++;
    for (; i<headers.count();++i)
    {
        dStream << headers.value(i);

    }
    i=0;

    for (; i<trace.count();++i)
    {
        dStream<<trace.value(i);

    }
    i=0;
    fileOut.close();
    fileOut.open(QIODevice::Append);

    int size = (64+trace.count())*4;
    if (size % 512 != 0)
    {
       int numOfBlocks = size / 512 + 1;
       quint8 zero =0;
       //int empty = numOfBlocks*512 - size;
       for ( ; i<  numOfBlocks*512 - size; ++i)
       {
           dStream <<zero;

       }
    }

    fileOut.close();
}
