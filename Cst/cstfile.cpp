#include "cstfile.h"

CstFile::CstFile(QObject *parent) : QObject(parent)
{

}

CstFile::CstFile(SegdFile *segd, QObject *parent) : QObject(parent)
{
    settings = new QSettings(QCoreApplication::applicationDirPath()+QDir::separator()+"config.ini",QSettings::IniFormat,this);
    line = segd->getGeneralThree().getLineNumber();
    point =segd->getGeneralThree().getPointNumber();
    sampleRate = segd->getExtendedHeader().getSampleRate();
    length = segd->getGeneralTwo().getExtendedRecordLength();
    easting = segd->getExtendedHeader().getX();
    northing = segd->getExtendedHeader().getY();
    elevation =segd->getExtendedHeader().getZ();
    ffid = segd->getGeneralThree().getExtendedFileNumber();
    numOfSeis =segd->getExtendedHeader().getNumberOfSeis();
    numOfAuxes=segd->getExtendedHeader().getNumberOfAuxes();
    readSettings();
    QVector<SegdTrace*> segdTraces=segd->getTraces();
    QVector<SegdTrace*>::iterator segdTracesIterator=segdTraces.begin();
    for( ;segdTracesIterator!=segdTraces.end();++segdTracesIterator)
    {
        traces.append(new CstTrace(*segdTracesIterator,this));
    }
}


int CstFile::getLength()
{
    return length;
}
int CstFile::getFfid()
{
    return ffid;
}
int CstFile::getLine()
{
    return line;
}

int CstFile::getPoint()
{
    return point;
}

int CstFile::getSampleRate()
{
    return sampleRate;
}

double CstFile::getEasting()
{
    return easting;
}

double CstFile::getNorthing()
{
    return northing;
}
float CstFile::getElevation()
{
    return elevation;
}

void CstFile::readSettings()
{
    settings->beginGroup("/BinningSettings");
    xOrigin = settings->value("/Xo","0").toFloat();
    yOrigin = settings->value("/Yo","0").toFloat();
    dX = settings->value("/dX","25").toFloat();
    dY = settings->value("/dY","25").toFloat();
    i0 = settings->value("/Io","1001").toInt();
    j0= settings->value("/Jo","1001").toFloat();
    azX = settings->value("/azX","0").toFloat();
    azY = settings->value("/azY","0").toFloat();
    settings->endGroup();
}

float CstFile::getAzX()
{
    return azX;
}
float CstFile::getAzY()
{
    return azY;
}
float CstFile::getdX()
{
    return dX;
}
float CstFile::getdY()
{
    return dY;
}
int CstFile::getI0()
{
    return i0;
}
int CstFile::getJ0()
{
    return j0;
}
float CstFile::getXorigin()
{
    return xOrigin;
}
float CstFile::getYorigin()
{
    return yOrigin;
}

void CstFile::setReceiverCoordinats(QMap<QString, Point> coordinates)
{
    QVector<CstTrace*>::iterator traceIt = traces.begin();
    CstTrace *trace;
    QString linePoint;
    for (; traceIt!=traces.end();++traceIt)
    {
           trace = *traceIt;
           linePoint = QString::number(trace->getReceiverLinePoint());
           if (coordinates.contains(linePoint))
           {
               trace->setReceiverX(coordinates.value(linePoint).getX());
               trace->setReceiverY(coordinates.value(linePoint).getY());
               //trace->setReceiverZ(coordinates.value(linePoint)->getZ());
           }
    }
}
void CstFile::setReceiverCoordinats(QTextStream *logStr, QMap<QString, Point> coordinates)
{
    QVector<CstTrace*>::iterator traceIt = traces.begin();
    CstTrace ;
}

bool CstFile::setSourceCoordinats(QMap<QString, Point> coordinates)
{
    QVector<CstTrace*>::iterator traceIt = traces.begin();
    CstTrace *trace = *traceIt;
    QString linePoint = QString::number(line)+QString::number(point);
    if (coordinates.contains(linePoint))
    {
        for (; traceIt!=traces.end();++traceIt)
        {
            trace = *traceIt;
            trace->setSourceX(coordinates.value(linePoint).getX());
            trace->setSourceY(coordinates.value(linePoint).getY());
        }
        return true;
    }
    else {
        return false;
    }
}

void CstFile::setGeometry()
{

    QVector<CstTrace*>::iterator traceIt = traces.begin();
    CstTrace *trace;
    for (; traceIt!=traces.end();++traceIt)
    {
        trace = *traceIt;
        if (trace->getTraceNb()!=0 && trace->getReceiverLinePoint()!=0)
        {
            trace->countGeometry();
        }
    }

}
//записываем служебные каналы
void CstFile::writeAuxTraces(const QString &filePath)
{

        QFile fileOut;
        fileOut.setFileName(filePath);
        for (uint i=0; i<numOfAuxes;++i)
        {
            traces.value(i)->writeTrace(fileOut);
        }
        //fileOut.open(QIODevice::Append);
        /*QVector<CstTrace*>::iterator traceIt = traces.begin();
        CstTrace* trace;
        trace = *traceIt;
        while (trace->getTraceNb()==0) {
            trace->writeTrace(fileOut);
            traceIt++;
            trace = *traceIt;
        }*/
}
//записываем сейсмические каналы
void CstFile::writeTraces(const QString &filePath,const bool &muted)
{
    QFile fileOut;
    fileOut.setFileName(filePath);
    //fileOut.open(QIODevice::Append);
    QVector<CstTrace*>::iterator traceIt = traces.begin();
    CstTrace* trace;
    quint32 traceNum=1;
    for ( ;traceIt!=traces.end();++traceIt)
    {
        trace = *traceIt;
        if (trace->getTraceNb()!=0)
        {
            if (!muted)
            {
                if (trace->getTraceStatus()!=0)
                {
                    trace->writeTrace(fileOut);
                    traceNum++;
                }
            }
            else {
                trace->writeTrace(fileOut);
                traceNum++;
            }
        }
    }
}

void CstFile::writeTraces(const QString &filePath,const bool &muted, const bool &missed)
{
    QFile fileOut;
    fileOut.setFileName(filePath);
    QVector<CstTrace*>::iterator traceIt = traces.begin();
    CstTrace* trace;

    for ( ;traceIt!=traces.end();++traceIt)
    {
        trace = *traceIt;
        if (trace->getTraceNb()!=0)
        {
            if (muted && missed)
            {
                trace->writeTrace(fileOut);
            }
            if (muted && !missed && trace->getReceiverLinePoint()!=0)
            {
                trace->writeTrace(fileOut);
            }
            if (!muted && missed && trace->getTraceStatus()!=0)
            {
                trace->writeTrace(fileOut);
            }
            if (!muted && !missed && trace->getTraceStatus()!=0 && trace->getReceiverLinePoint()!=0)
            {
                trace->writeTrace(fileOut);
            }
        }
    }
}


//удаляем трассы, попавшие в эксклюзивные зоны
QStringList CstFile::deleteTracesInExclusions(QVector<Exclusion*> excl)
{
    QStringList result;
    QVector<CstTrace*>::iterator traceIterator = traces.begin();
    CstTrace *trace;
    for (; traceIterator!=traces.end();++traceIterator)
    {
        trace = *traceIterator;
        foreach (Exclusion *ex, excl) {
            if (ex->checkPoint(trace->getReceiverX(),trace->getReceiverY()))
            {

                result.append(QString("%1\t%2\t%3\t%4\n").arg(trace->getFfid()).arg(trace->getReceiverLinePoint()).arg(trace->getReceiverX()).arg(trace->getReceiverY()));
                traceIterator = traces.erase(traceIterator);
                traceIterator--;
                break;
            }
        }
     }
     return result;
}

//получаем трассы в заданном окне
QVector<QVector<float> > CstFile::getDataInWindow(const int &minOffset, const int &maxOffset, const int &minTime, const int &maxTime,const bool muted)
{
    QVector<CstTrace*>::iterator tracesIterator = traces.begin();
    int firstPos;
    int length;
    firstPos = minTime/(sampleRate/1000);
    length = (maxTime-minTime)/(sampleRate/1000);
    QVector< QVector<float> > result;
    CstTrace *trace;
    for (; tracesIterator!=traces.end();++tracesIterator)
    {
        trace = *tracesIterator;
        if (muted)
        {
            if (trace->getOffset() > minOffset && trace->getOffset() < maxOffset &&trace->getTraceNb()!=0 && trace->getTraceStatus()!=0)
            {
                result.append(trace->getData().mid(firstPos,length));
            }
        }
        else
        {
            if (trace->getOffset() > minOffset && trace->getOffset() < maxOffset &&trace->getTraceNb()!=0)
            {
                result.append(trace->getData().mid(firstPos,length));
            }
        }
    }
    return result;
}
//получаем трассы в заданном окне и пишем лог
QVector<QVector<float> > CstFile::getDataInWindow(QTextStream *logStr, const uint &minOffset, const uint &maxOffset, const uint &minTime, const uint &maxTime, const bool &muted, const bool &tests, const float &minA)
{
    QVector<CstTrace*>::iterator tracesIterator = traces.begin();
    int firstPos;
    int length;
    firstPos = minTime/(sampleRate/1000);
    length = (maxTime-minTime)/(sampleRate/1000);
    QVector< QVector<float> > result;
    CstTrace *trace;
    int nbOfMuted=0;
    int nbOfTest=0;
    int nbOfA =0;
    *logStr <<QString("Исключенные трассы: \n").toUtf8();
    *logStr <<QString("Пикет\t Причина \n").toUtf8();
    for (; tracesIterator!=traces.end();++tracesIterator)
    {
        trace = *tracesIterator;
        if (trace->getOffset()>=minOffset && trace->getOffset()<=maxOffset && trace->getTraceNb()!=0)
        {
            if (muted && trace->getTraceStatus()==0)
            {
                nbOfMuted++;
                *logStr << trace->getReceiverLinePoint()<<QString("\t Muted\n");

                continue;
            }
            if (tests && trace->getTestsStatus()==1)
            {
                nbOfTest++;
                *logStr << trace->getReceiverLinePoint()<<QString("\t Bad Tests\n");
                continue;
            }
            if (trace->getMaxAmpl()<minA)
            {
                *logStr << trace->getReceiverLinePoint()<<QString("\t Min Ampl\n");
                nbOfA++;
                continue;
            }
            result.append(trace->getData().mid(firstPos,length));
        }
    }
    *logStr<<QString("Muted: %1; BadTests: %2; MinA: %3; All Bad: %4; For Count: %5\n").arg(nbOfMuted).arg(nbOfTest).arg(nbOfA).arg(nbOfMuted+nbOfTest+nbOfA).arg(result.size());
    return result;
}

//получаем трассы в заданном окне с учетом эксклюзивных зон mesa и пишем лог
QVector<QVector<float> > CstFile::getDataInWindow(QTextStream *logStr, const uint &minOffset, const uint &maxOffset, const uint &minTime, const uint &maxTime, const bool &muted, const bool &tests, const float &minA, QVector<Exclusion *> exclVec)
{
    QVector<CstTrace*>::iterator tracesIterator = traces.begin();
    int firstPos;
    int length;
    firstPos = minTime/(sampleRate/1000);
    length = (maxTime-minTime)/(sampleRate/1000);
    QVector< QVector<float> > result;
    CstTrace *trace;
    int nbOfMuted=0;
    int nbOfTest=0;
    int nbOfA =0;
    int nbOfExcl=0;
    *logStr <<QString("Исключенные трассы: \n").toUtf8();
    *logStr <<QString("Пикет\t Причина \n").toUtf8();
    for (; tracesIterator!=traces.end();++tracesIterator)
    {
        trace = *tracesIterator;
        if (trace->getOffset()>=minOffset && trace->getOffset()<=maxOffset && trace->getTraceNb()!=0)
        {

            if (traceInExclusion(trace,exclVec))
            {
                nbOfExcl++;
                *logStr << trace->getReceiverLinePoint()<<QString("\t Exclusion\n");
                continue;
            }
            if (muted && trace->getTraceStatus()==0)
            {
                nbOfMuted++;
                *logStr << trace->getReceiverLinePoint()<<QString("\t Muted\n");

                continue;
            }
            if (tests && trace->getTestsStatus()==1)
            {
                nbOfTest++;
                *logStr << trace->getReceiverLinePoint()<<QString("\t Bad Tests\n");
                continue;
            }
            if (trace->getMaxAmpl()<minA)
            {
                *logStr << trace->getReceiverLinePoint()<<QString("\t Min Ampl\n");
                nbOfA++;
                continue;
            }
            result.append(trace->getData().mid(firstPos,length));
        }
    }
    *logStr<<QString("Muted: %1; Bad Tests: %2; Min Ampl: %3; Exclusion:%6 All Bad: %4; For Count: %5\n").arg(nbOfMuted).arg(nbOfTest).arg(nbOfA).arg(nbOfMuted+nbOfTest+nbOfA+nbOfExcl).arg(result.size()).arg(nbOfExcl);
    return result;
}

bool CstFile::traceInExclusion(CstTrace *tr, QVector<Exclusion *> excls)
{
    foreach (Exclusion *ex, excls) {
        if (ex->checkPoint(tr->getReceiverX(),tr->getReceiverY()))
        {
            return true;
        }
    }
    return false;
}
//получаем трассы в заданном окне с учетом списка исключаемых пикетов и пишем лог
QVector<QVector<float> >CstFile::getDataInWindow(QTextStream *logStr, const uint &minOffset, const uint &maxOffset, const uint &minTime, const uint &maxTime, const bool &muted, const bool &tests, const float &minA, QVector<int> exclReceivers)
{

    QVector<CstTrace*>::iterator tracesIterator = traces.begin();
    int firstPos;
    int length;
    firstPos = minTime/(sampleRate/1000);
    length = (maxTime-minTime)/(sampleRate/1000);
    QVector< QVector<float> > result;
    CstTrace *trace;
    int nbOfMuted=0;
    int nbOfTest=0;
    int nbOfA =0;
    int nbOfExcl=0;
    *logStr <<QString("Исключенные трассы: \n").toUtf8();
    *logStr <<QString("Пикет\t Причина \n").toUtf8();
    for (; tracesIterator!=traces.end();++tracesIterator)
    {
        trace = *tracesIterator;
        if (trace->getOffset()>=minOffset && trace->getOffset()<=maxOffset && trace->getTraceNb()!=0)
        {

            if (exclReceivers.indexOf(trace->getReceiverLinePoint())!=-1)
            {
                nbOfExcl++;
                *logStr << trace->getReceiverLinePoint()<<QString("\t Exclusion\n");
                continue;
            }
            if (muted && trace->getTraceStatus()==0)
            {
                nbOfMuted++;
                *logStr << trace->getReceiverLinePoint()<<QString("\t Muted\n");

                continue;
            }
            if (tests && trace->getTestsStatus()==1)
            {
                nbOfTest++;
                *logStr << trace->getReceiverLinePoint()<<QString("\t Bad Tests\n");
                continue;
            }
            if (trace->getMaxAmpl()<minA)
            {
                *logStr << trace->getReceiverLinePoint()<<QString("\t Min Ampl\n");
                nbOfA++;
                continue;
            }
            result.append(trace->getData().mid(firstPos,length));
        }
    }
    *logStr<<QString("Muted: %1; Bad Tests: %2; Min Ampl: %3; Exclusion:%6 All Bad: %4; For Count: %5\n").arg(nbOfMuted).arg(nbOfTest).arg(nbOfA).arg(nbOfMuted+nbOfTest+nbOfA+nbOfExcl).arg(result.size()).arg(nbOfExcl);
    return result;
}

bool CstFile::setTemplates(XFile *xps)
{
    if (xps->getTemplates()->last().lastChannel > numOfSeis)
    {
        return false;
    }
    else
    {
        uint currentTrace= numOfAuxes-1;
        int currentReceiver;
        int currentLine;
        Template templ;// = xps->getTemplates()->dequeue();
        for (currentTrace=0; currentTrace<numOfAuxes;++currentTrace)
        {
            traces.value(currentTrace)->setSourceLinePoint(xps->getLine()*10000+xps->getPoint());
        }
        while (!xps->getTemplates()->isEmpty()) {
            templ = xps->getTemplates()->dequeue();
            currentLine =templ.receiverLine;
            currentReceiver =templ.firstReceiver;
            for(;currentTrace<templ.firstChannel+numOfAuxes-1;++currentTrace)
            {
                traces.value(currentTrace)->setSourceLinePoint(xps->getLine()*10000+xps->getPoint());
                traces.value(currentTrace)->setReceiverLinePoint(0);
                traces.value(currentTrace)->setReceiverX(0);
                traces.value(currentTrace)->setReceiverY(0);
            }
            for (;currentTrace<templ.lastChannel+numOfAuxes;++currentTrace)
            {
                traces.value(currentTrace)->setSourceLinePoint(xps->getLine()*10000+xps->getPoint());
                traces.value(currentTrace)->setReceiverLinePoint(currentLine*10000+currentReceiver);
                currentReceiver++;
            }
        }
        for(;currentTrace<numOfAuxes;++currentTrace)
        {
            traces.value(currentTrace)->setSourceLinePoint(xps->getLine()*10000+xps->getPoint());
            traces.value(currentTrace)->setReceiverLinePoint(0);
            traces.value(currentTrace)->setReceiverX(0);
            traces.value(currentTrace)->setReceiverY(0);
        }
        return true;
    }
}
