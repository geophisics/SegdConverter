#include "segdfile.h"
#include "math.h"
SegdFile::SegdFile(QObject *parent) : QObject(parent)
{

}
SegdFile::SegdFile(const QString &path, QObject *parent):QObject(parent)
{
    correct = true;
    QFile fileIn(path); //читаем файл
    if (!fileIn.open(QIODevice::ReadOnly)) //пробуем открыть файл, в случае неудачи посылаем сигнал о некорректности файла
    {
        emit incorrectFile();
        correct =false;
        return;
    }

    QByteArray *rowSegd =new QByteArray(fileIn.readAll()); //считываем весь файл
    fileIn.close();
    //if (rowSegd.size()<2656) //если размер считанного файла меньше чем размер заголовка, то посылаем сигнал о некорректности файла
    //{
    //    emit incorrectFile();
    //    return;
    //}
    if (fileIn.size()<2656)
    {
        emit incorrectFile();
        correct = false;
        return;
    }
    //QByteArray rowSegd = fileIn.read(2656);
    int numOfTraces = toUint(rowSegd->mid(616,4)); // количество трасс в файле
    int samplesPerTrace = toUint(rowSegd->mid(640,4)); // количество отсчетов на одну трассу
    int sizeOfTrace=244+samplesPerTrace*4;
    int correctSize = 2656+ sizeOfTrace*numOfTraces; //рассчетный размер файла
    if (correctSize!=fileIn.size()) //если фактический размер файла не соответсвует рассчетному то посылаем сигнал о некорректности файла
    {
        emit incorrectFile();
        correct = false;
        return;
    }
    // заполнение данных файла
    genHeader1 = new GeneralOne(rowSegd->mid(0,32));
    genHeader2 = new GeneralTwo(rowSegd->mid(32,32));
    genHeader3 = new GeneralThree(rowSegd->mid(64,32));
    for (int i = 0; i<16;i++)
    {
        channelHeaders[i]=new ChannelSetHeader(rowSegd->mid(96+i*32,32));
    }
    extHeader=new ExtendedHeader(rowSegd->mid(608,1024));
    externalHeader=new QString(rowSegd->mid(1632,1024));

    for (int i=0; i<numOfTraces;i++)
    {
        //fileIn.seek(2656+sizeOfTrace*i);
        //rowSegd=fileIn.read(sizeOfTrace);
        tracesOfSegd.append(new SegdTrace(rowSegd->mid(2656+sizeOfTrace*i,sizeOfTrace),this));
    }
    rowSegd->clear();
    delete(rowSegd);
}

SegdFile::SegdFile(const QString &path, const float &mpFactor, QObject *parent):QObject(parent)
{
    correct = true;
    QFile fileIn(path); //читаем файл
    if (!fileIn.open(QIODevice::ReadOnly)) //пробуем открыть файл, в случае неудачи посылаем сигнал о некорректности файла
    {
        emit incorrectFile();
        correct = false;
        return;
    }

    QByteArray rowSegd = fileIn.readAll(); //считываем весь файл
    fileIn.close();
    //if (rowSegd.size()<2656) //если размер считанного файла меньше чем размер заголовка, то посылаем сигнал о некорректности файла
    //{
    //    emit incorrectFile();
    //    return;
    //}
    if (fileIn.size()<2656)
    {
        emit incorrectFile();
        correct = false;
        return;
    }
    //QByteArray rowSegd = fileIn.read(2656);
    int numOfTraces = toUint(rowSegd.mid(616,4)); // количество трасс в файле
    int samplesPerTrace = toUint(rowSegd.mid(640,4)); // количество отсчетов на одну трассу
    int sizeOfTrace=244+samplesPerTrace*4;
    int correctSize = 2656+ sizeOfTrace*numOfTraces; //рассчетный размер файла
    if (correctSize!=fileIn.size()) //если фактический размер файла не соответсвует рассчетному то посылаем сигнал о некорректности файла
    {
        emit incorrectFile();
        correct = false;
        return;
    }
    // заполнение данных файла
    genHeader1 = new GeneralOne(rowSegd.mid(0,32));
    genHeader2 = new GeneralTwo(rowSegd.mid(32,32));
    genHeader3 = new GeneralThree(rowSegd.mid(64,32));
    for (int i = 0; i<16;i++)
    {
        channelHeaders[i]=new ChannelSetHeader(rowSegd.mid(96+i*32,32));
    }
    extHeader=new ExtendedHeader(rowSegd.mid(608,1024));
    externalHeader=new QString(rowSegd.mid(1632,1024));

    for (int i=0; i<numOfTraces;i++)
    {
        //fileIn.seek(2656+sizeOfTrace*i);
        //rowSegd=fileIn.read(sizeOfTrace);
        tracesOfSegd.append(new SegdTrace(rowSegd.mid(2656+sizeOfTrace*i,sizeOfTrace),mpFactor,this));
    }
    rowSegd.clear();
}

SegdFile::SegdFile(const QString &path, const bool &onlyAuxes, QObject *parent):QObject(parent)
{
    correct = true;
    QFile fileIn(path); //читаем файл
    if (!fileIn.open(QIODevice::ReadOnly)) //пробуем открыть файл, в случае неудачи посылаем сигнал о некорректности файла
    {
        emit incorrectFile();
        correct =false;
        return;
    }

    QByteArray *rowSegd =new QByteArray(fileIn.readAll()); //считываем весь файл
    fileIn.close();
    //if (rowSegd.size()<2656) //если размер считанного файла меньше чем размер заголовка, то посылаем сигнал о некорректности файла
    //{
    //    emit incorrectFile();
    //    return;
    //}
    if (fileIn.size()<2656)
    {
        emit incorrectFile();
        correct = false;
        return;
    }
    //QByteArray rowSegd = fileIn.read(2656);
    int numOfTraces = toUint(rowSegd->mid(616,4)); // количество трасс в файле
    int samplesPerTrace = toUint(rowSegd->mid(640,4)); // количество отсчетов на одну трассу
    int sizeOfTrace=244+samplesPerTrace*4;
    int correctSize = 2656+ sizeOfTrace*numOfTraces; //рассчетный размер файла
    if (correctSize!=fileIn.size()) //если фактический размер файла не соответсвует рассчетному то посылаем сигнал о некорректности файла
    {
        emit incorrectFile();
        correct = false;
        return;
    }
    // заполнение данных файла
    genHeader1 = new GeneralOne(rowSegd->mid(0,32));
    genHeader2 = new GeneralTwo(rowSegd->mid(32,32));
    genHeader3 = new GeneralThree(rowSegd->mid(64,32));
    for (int i = 0; i<16;i++)
    {
        channelHeaders[i]=new ChannelSetHeader(rowSegd->mid(96+i*32,32));
    }
    extHeader=new ExtendedHeader(rowSegd->mid(608,1024));
    externalHeader=new QString(rowSegd->mid(1632,1024));
    if (onlyAuxes)
    {
        for (int i=0; i<extHeader->getNumberOfAuxes();i++)
        {
            //fileIn.seek(2656+sizeOfTrace*i);
            //rowSegd=fileIn.read(sizeOfTrace);
            tracesOfSegd.append(new SegdTrace(rowSegd->mid(2656+sizeOfTrace*i,sizeOfTrace),this));
        }
    }
    else
    {
        for (int i=0; i<numOfTraces;i++)
        {
            //fileIn.seek(2656+sizeOfTrace*i);
            //rowSegd=fileIn.read(sizeOfTrace);
            tracesOfSegd.append(new SegdTrace(rowSegd->mid(2656+sizeOfTrace*i,sizeOfTrace),this));
        }
    }
    rowSegd->clear();
    delete(rowSegd);
}

SegdFile::~SegdFile()
{
    delete genHeader1;
    delete genHeader2;
    delete genHeader3;
    for (int i=0;i<16;i++)
    {
        delete channelHeaders[i];
    }

    delete extHeader;
    delete externalHeader;
    qDeleteAll(tracesOfSegd);
    tracesOfSegd.clear();
}
// преобразование массива байт в число
uint SegdFile::toUint(const QByteArray &arr)
{
    QString str=QString(arr.toHex());
    bool ok;
    uint i = str.toUInt(&ok,16);
    return i;
}
// преобразование массива байт в число (BCD)
uint SegdFile::BCDtoUint(const QByteArray &arr)
{
    uint result =0;
    QString str =QString(arr.toHex());
    bool ok;
    result = str.toInt(&ok,10);
    return result;
}
//преобразование массива байт во float
float SegdFile::toFloat(const QByteArray &arr)
{
    float result;
    QByteArray tmpArr=arr;
    QDataStream ds(&tmpArr,QIODevice::ReadOnly);
    ds.setFloatingPointPrecision(QDataStream::SinglePrecision);
    ds>>result;
    return result;
}
//преобразование массива байт в double
double SegdFile::toDouble(const QByteArray &arr)
{
    double result;
    QByteArray tmpArr=arr;
    QDataStream ds(&tmpArr,QIODevice::ReadOnly);
    ds.setFloatingPointPrecision(QDataStream::DoublePrecision);
    ds>>result;
    return result;
}
//преобразование первых четырех бит в число (двоично-десятичное)

quint8 SegdFile::BCDtoUintFirstFourBit(const QByteArray &arr)
{
    //quint8 result =(((arr[0]&0xF0)>>4)*10+(arr[0]&0x0F));
    // return (((arr[0]&0xF0)>>4)*10+(arr[0]&0x0F))/10;
    //return result/10;
    return (quint8)arr[0]/16;
}

// преобразование массива без первых четырех бит в целое число (двоично десятичное)
quint16 SegdFile::BCDtoUintWithoutFirstFourBit(const QByteArray &arr)
{
    quint16 result =0;
    quint8 powOftwo=-1;
    for (int i=0;i<arr.count();i++)
    {
        result = result*100;
        result =result + (((arr[i]&0xF0)>>4)*10+(arr[i]&0x0F));
        powOftwo+=2;
    }

    return result%(int)pow(10,powOftwo);
}
// преобразование массива байтов в число (для номера линии и пикета ПВ)
float SegdFile::toBinAsFloat(const QByteArray &arrIn)
{
    QString forResult;
    bool ok;
    uint floor;
    uint ceil;
    if (arrIn.size()==2)
    {
    QByteArray arr1 =arrIn.toHex();
    floor = arr1.left(arr1.size()-2).toInt(&ok,10);
    ceil = arr1.right(2).toInt(&ok,10);
    forResult = QString("%1.%2").arg(floor).arg(ceil);
    return forResult.toFloat(&ok);
    }
    else {
        QByteArray arr1 =arrIn;
        arr1.resize(3);
        floor = toUint(arr1);
        arr1=arrIn.right(2);
        ceil = toUint(arr1);
        forResult = QString("%1.%2").arg(floor).arg(ceil);
        return forResult.toFloat(&ok);

    }
}
//преобразование в беззнаковое 8 байтовое целое число
quint64 SegdFile::toUint64(const QByteArray &arr)
{
    quint64 result;
    QByteArray tmpArr=arr;
    QDataStream ds(&tmpArr,QIODevice::ReadOnly);
    ds>>result;
    return result;
}
//получаем вектор с номерами трасс
QVector <int> SegdFile::getVectorOfTraces()
{
    QVector<int> result;
    QVector<SegdTrace*>::iterator it1 = tracesOfSegd.begin();
    SegdTrace* oneTrace;
    for ( ; it1!=tracesOfSegd.end(); it1++)
    {
        oneTrace=*it1;
        result.append(oneTrace->getExtensionOne()->getLinePointNum());
    }
    return result;
}

//отдаем трассу с заданным номером
SegdTrace* SegdFile::getTrace(const int &traceNum)
{
    return tracesOfSegd.value(traceNum);
}

// отдаем расширенный заголовок Extended Header
ExtendedHeader SegdFile::getExtendedHeader()
{
    return *extHeader;
}
GeneralThree SegdFile::getGeneralThree()
{
    return *genHeader3;
}

GeneralTwo SegdFile::getGeneralTwo()
{
    return *genHeader2;
}
//формируем QStringList с результатами тестов
QStringList SegdFile::getHeaderTests()
{
    QStringList result;
    QVector<SegdTrace*>::iterator itTrace = tracesOfSegd.begin();
    SegdTrace* trace;
    for (; itTrace!=tracesOfSegd.end();++itTrace)
    {
        trace = *itTrace;
        if (trace->getChannelType()==1)
        {
            result.append(trace->getTests() + "\t"+QString::number(extHeader->getUpholeTime())+ "\t"+QString::number(extHeader->getX())+
                          "\t"+QString::number(extHeader->getY())+ "\t"+QString::number(extHeader->getZ())+"\n");
        }
    }
    return result;
}

//проверяем тесты
void SegdFile::analyzingTestsHeaders(const int &badTraces, const int &badTracesPercent, const int &badLinesPercent)
{
    QList<int> lps = getLPs();
    //qsort(lps);
    QList<int>::iterator itSet = lps.begin();
    QVector<SegdTrace*> tracesPerLine;
    int badLines=0;
    for (; itSet!=lps.end();++itSet)
    {
        tracesPerLine=getLineTraces(*itSet);
        if (!checkLine(tracesPerLine,badTraces,badTracesPercent))
        {
            badLines++;
        }
    }
    float badLinesPerc = badLines*100.0/lps.count();
    if (badLinesPerc>badLinesPercent)
    {
        emit badFfidDetected(QString ("Файл не прошел проверку. Процент забракованных линий - %1%").arg(badLinesPerc));
    }
    else if(badLinesPerc>0)
        {
            emit normalFfidDetected(QString ("Файл прошел проверку. КК 0,9. Процент забракованных линий - %1%").arg(badLinesPerc));
        }
        else
        {
            emit goodFfidDetected(QString ("Файл прошел проверку. КК 1,0. Процент забракованных линий - %1%").arg(badLinesPerc));
        }
}

float SegdFile::checkTests()
{
    QVector<SegdTrace*>::iterator itTrace = tracesOfSegd.begin();
    SegdTrace* trace;
    int incorrectTraces=0;
    for (; itTrace!=tracesOfSegd.end();++itTrace)
    {
        trace = *itTrace;
        if (trace->getChannelType()!=1)
        {
            continue;
        }
        //qDebug()<<trace->getExtensionThree()->getResistanceValue();
        if (trace->getExtensionThree()->getResistanceError()==1 || std::isnan(trace->getExtensionThree()->getResistanceValue()))
        {
            incorrectTraces++;
            continue;
        }
        if (trace->getExtensionThree()->getTiltError()==1 || std::isnan(trace->getExtensionThree()->getTiltValue()))
        {
            incorrectTraces++;
            continue;
        }
        if (trace->getExtensionFive()->getLeakageError()==1 || std::isnan(trace->getExtensionFive()->getLeakageValue()))
        {
            incorrectTraces++;
            continue;
        }
    }
    return 100.0*incorrectTraces/this->getExtendedHeader().getNumberOfSeis();
}

QList<int> SegdFile::getLPs()
{
    QList<int> result;
    QVector<SegdTrace*>::iterator itTrace =tracesOfSegd.begin();
    SegdTrace* trace;
    for (; itTrace!=tracesOfSegd.end();++itTrace)
    {
        trace = *itTrace;
        if (trace->getChannelType()==1 && !result.contains(trace->getExtensionOne()->getReceiverLineNum()))
        {
            result.append(trace->getExtensionOne()->getReceiverLineNum());
        }
    }
    qSort(result);
    return result;
}

bool SegdFile::checkLine(QVector<SegdTrace*> tracesOnLine, const int badPodryad, const int badPercent)
{
    QVector<SegdTrace*>::iterator itTrace =tracesOnLine.begin();
    SegdTrace* trace;
    int badTracesPodryad=0;
    int badTracesVsego=0;
    bool result = true;
    for (; itTrace!=tracesOnLine.end();++itTrace)
    {
        trace = *itTrace;
        if (trace->getExtensionThree()->getResistanceError()==1 || trace->getExtensionThree()->getTiltError()==1 || trace->getExtensionFive()->getLeakageError()==1)
        {
            badTracesPodryad++;
            badTracesVsego++;
            if (trace->getExtensionThree()->getResistanceError()==1)
            {
                emit badChannelDetected(QString("Пикет %1. Ошибка Resistance. Resistance=%2").arg(trace->getExtensionOne()->getLinePointNum()).arg(trace->getExtensionThree()->getResistanceValue()));
            }
            if (trace->getExtensionThree()->getTiltError()==1)
            {
                emit badChannelDetected(QString("Пикет %1. Ошибка Tilt. Tilt=%2").arg(trace->getExtensionOne()->getLinePointNum()).arg(trace->getExtensionThree()->getTiltValue()));
            }
            if (trace->getExtensionFive()->getLeakageError()==1)
            {
                emit badChannelDetected(QString("Пикет %1. Ошибка Leakage. Leakage=%2").arg(trace->getExtensionOne()->getLinePointNum()).arg(trace->getExtensionFive()->getLeakageValue()));
            }
        }
        else
        {
            badTracesPodryad=0;
        }
        if (badTracesPodryad>badPodryad)
        {
            result =false;
            //emit badLineDetected(QString::number(trace->getExtensionOne().getReceiverLineNum()));
        }
    }
    if (!result)
    {
       emit badLineDetected(QString("Линия %1. Превышено максимально-допустимое количество неисправных каналов подряд").arg(trace->getExtensionOne()->getReceiverLineNum()));
    }
    float percentOfBad = badTracesVsego*100.0/tracesOnLine.count();
    if (percentOfBad > badPercent)
    {
        result = false;
        emit badLineDetected(QString("Линия %1. Превышен максимально-допустимый процент неисправных каналов на линии (%2%)").arg(trace->getExtensionOne()->getReceiverLineNum()).arg(percentOfBad));
    }
    if (result)
    {
        emit goodLineDetected(QString("Линия %1 успешно прошла проверку. Процент неисправных каналов на линии - %2%").arg(trace->getExtensionOne()->getReceiverLineNum()).arg(percentOfBad));
    }
    return result;
}
QVector<SegdTrace*> SegdFile::getLineTraces(const uint &lineNb)
{
   QVector<SegdTrace*>::iterator itTrace= tracesOfSegd.begin();
   QVector<SegdTrace*> result;
   SegdTrace* trace;
   for (; itTrace!=tracesOfSegd.end();++itTrace)
   {
       trace = *itTrace;
       if (trace->getExtensionOne()->getReceiverLineNum()==lineNb)
       {
           result.append(trace);
       }
   }
   return result;
}

bool SegdFile::getFileState()
{
    return correct;
}

GeneralOne SegdFile::getGeneralOne()
{
    return *genHeader1;
}
ChannelSetHeader *SegdFile::getChannelSetHeader(const int &numHeader)
{
    return channelHeaders[numHeader];
}
QVector<SegdTrace*> SegdFile::getTraces()
{
    return tracesOfSegd;
}
QStringList SegdFile::getXFile()
{
    QStringList xFile;
    QList<int> lps = getLPs();
    QList<int>::iterator itList = lps.begin();
    QVector<SegdTrace*> tracesPerLine;
    //int badLines=0;
    for (; itList!=lps.end();++itList)
    {
        tracesPerLine=getLineTraces(*itList);
        xFile.append(getXLine(tracesPerLine));
        //if (!checkLine(tracesPerLine,badTraces,badTracesPercent))
        //{
        //    badLines++;
        //}
    }
    return xFile;
}

QStringList SegdFile::getXLine(QVector<SegdTrace*> traces)
{
    QStringList out;
    QString str="X";
    str=str.leftJustified(7,' ');
    QString tmp=QString::number(getGeneralThree().getExtendedFileNumber()).rightJustified(4,' ');
    str.append(tmp);
    tmp = QString::number(getExtendedHeader().getTapeNumber()).leftJustified(6,' ');
    str.replace(1,6,tmp);
    str=str.leftJustified(13,' ');
    tmp=QString::number(getGeneralThree().getLineNumber()).leftJustified(16,' ');
    str.append(tmp);
    tmp=QString::number(getGeneralThree().getPointNumber()).rightJustified(8,' ');
    str.append(tmp);
    str = str.leftJustified(80,' ');
    str.replace(11,2,"11");
    str.replace(79,1,'1');
    str.replace(37,1,'1');
    tmp=QString::number(traces.first()->getExtensionOne()->getReceiverLineNum()).leftJustified(16,' ');
    str.replace(47,16,tmp);
    tmp=QString::number(traces.first()->getTraceHeader()->getTraceNumber()).rightJustified(4,' ');
    str.replace(38,4,tmp);
    tmp=QString::number(traces.first()->getExtensionOne()->getReceiverPointNum()).rightJustified(8,' ');
    str.replace(63,8,tmp);

    for (int i = 0; i < traces.count()-1; ++i) {
        if (traces.value(i+1)->getExtensionOne()->getReceiverPointNum()-traces.value(i)->getExtensionOne()->getReceiverPointNum()>1)
        {
            tmp = QString::number(traces.value(i)->getTraceHeader()->getTraceNumber()).rightJustified(4,' ');
            str.replace(42,4,tmp);
            tmp=QString::number(traces.value(i)->getExtensionOne()->getReceiverPointNum()).rightJustified(8,' ');
            str.replace(71,8,tmp);

            out.append(str);

            tmp=QString::number(traces.value(i+1)->getTraceHeader()->getTraceNumber()).rightJustified(4,' ');
            str.replace(38,4,tmp);
            tmp=QString::number(traces.value(i+1)->getExtensionOne()->getReceiverPointNum()).rightJustified(8,' ');
            str.replace(63,8,tmp);

        }
    }

    tmp=QString::number(traces.last()->getTraceHeader()->getTraceNumber()).rightJustified(4,' ');
    str.replace(42,4,tmp);
    tmp=QString::number(traces.last()->getExtensionOne()->getReceiverPointNum()).rightJustified(8,' ');
    str.replace(71,8,tmp);
    out.append(str);
    return out;
}



void SegdFile::deleteUnworkedTraces()
{
    QVector<SegdTrace*>::iterator traceIt = tracesOfSegd.begin();
    SegdTrace* tr;
    for (; traceIt!=tracesOfSegd.end();++traceIt)
    {
        tr=*traceIt;
        if (tr->getExtensionThree()->getResistanceValue()==9999.0)
        {
            traceIt=tracesOfSegd.erase(traceIt);
            //qDebug()<<QString("Udalena Trassa %1 Resistance = 9999.0").arg(tr->getExtensionOne().getLinePointNum());
            --traceIt;
        }
    }
}

void SegdFile::deleteMuteTraces()
{
    QVector<SegdTrace*>::iterator traceIt = tracesOfSegd.begin();
    SegdTrace* tr;
    for (; traceIt!=tracesOfSegd.end();++traceIt)
    {
        tr=*traceIt;
        if (tr->getTraceHeader()->getTraceEdit()==2)
        {
            traceIt=tracesOfSegd.erase(traceIt);
            //qDebug()<<QString("Udalena Trassa %1  (trace intentionally zeroed)").arg(tr->getExtensionOne().getLinePointNum());
            --traceIt;
        }
    }
}
void SegdFile::deleteMuteTraces(const QString &logFile)
{
    QVector<SegdTrace*>::iterator traceIt = tracesOfSegd.begin();
    SegdTrace* tr;
    QFile lFile;
    lFile.setFileName(logFile);
    lFile.open(QIODevice::Append|QIODevice::Text);
    QTextStream out(&lFile);
    for (; traceIt!=tracesOfSegd.end();++traceIt)
    {
        tr=*traceIt;
        if (tr->getTraceHeader()->getTraceEdit()==2)
        {
            out<<QString::number(this->getGeneralThree().getExtendedFileNumber())<<"\t"<<QString::number(this->getGeneralThree().getLineNumber())<<"\t"
              <<QString::number(this->getGeneralThree().getPointNumber())<<"\t"<<QString::number(tr->getExtensionOne()->getReceiverLineNum())<<"\t"
             <<QString::number(tr->getExtensionOne()->getReceiverPointNum())<<"\t"<<tr->getTraceHeader()->getTraceNumber()<<"\t"<<"Trace intentionally zeroed"<<"\n";
            traceIt=tracesOfSegd.erase(traceIt);
            //qDebug()<<QString("Udalena Trassa %1  (trace intentionally zeroed)").arg(tr->getExtensionOne().getLinePointNum());
            --traceIt;
        }
    }
    lFile.close();
}
void SegdFile::deleteUnworkedTraces(const QString &logFile)
{
    QVector<SegdTrace*>::iterator traceIt = tracesOfSegd.begin();
    SegdTrace* tr;
    QFile lFile;
    lFile.setFileName(logFile);
    lFile.open(QIODevice::Append|QIODevice::Text);
    QTextStream out(&lFile);
    //out << "FFID\t" << "Source Line\t" <<"Source Point\t"<<"Receiver Line\t"<<"Reseiver Poing\t"<<"Reason\t";
    //logFile.close();

    for (; traceIt!=tracesOfSegd.end();++traceIt)
    {
        tr=*traceIt;
        if (tr->getExtensionThree()->getResistanceValue()==9999.0)
        {
            out<<QString::number(this->getGeneralThree().getExtendedFileNumber())<<"\t"<<QString::number(this->getGeneralThree().getLineNumber())<<"\t"
              <<QString::number(this->getGeneralThree().getPointNumber())<<"\t"<<QString::number(tr->getExtensionOne()->getReceiverLineNum())<<"\t"
             <<QString::number(tr->getExtensionOne()->getReceiverPointNum())<<"\t"<<tr->getTraceHeader()->getTraceNumber()<<"\t"<<"Incorrect Tests"<<"\n";
            traceIt=tracesOfSegd.erase(traceIt);
            //qDebug()<<QString("Udalena Trassa %1 Resistance = 9999.0").arg(tr->getExtensionOne().getLinePointNum());
            --traceIt;
        }
    }
    lFile.close();
}