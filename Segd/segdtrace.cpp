#include "segdtrace.h"
#include "segdfile.h"


SegdTrace::SegdTrace(QObject *parent): QObject(parent)
{

}
SegdTrace::SegdTrace(const QByteArray &arrIn, QObject *parent) : QObject(parent)
{
    SegdFile *segd = qobject_cast<SegdFile*>(parent);
    generalTraceHeader=new TraceHeader(arrIn.mid(0,20));
    extHeader1=new TraceExtensionOne(arrIn.mid(20,32));
    extHeader2=new TraceExtensionTwo(arrIn.mid(52,32));
    extHeader3=new TraceExtensionThree(arrIn.mid(84,32));
    extHeader4=new TraceExtensionFour(arrIn.mid(116,32));
    extHeader5=new TraceExtensionFive(arrIn.mid(148,32));
    extHeader6=new TraceExtensionSix(arrIn.mid(180,32));
    extHeader7=new TraceExtensionSeven(arrIn.mid(212,32));
    int traces=extHeader1->samplesPerTrace;
    float mpFactor = this->extHeader7->getSampleToMvFactor();
    if (mpFactor==0)
    {
        mpFactor = segd->getChannelSetHeader(this->generalTraceHeader->getChannelSetNumber()-1)->getDescaleMultiplier();
    }
    traceData = new QVector<float>;
    float traceValue;
    //QByteArray tmpArr = QByteArray(arrIn.mid(244,4));
    QByteArray *tmpArr =new QByteArray(arrIn.mid(244));
    QDataStream ds(tmpArr,QIODevice::ReadOnly);
    //QDataStream *ds = new QDataStream(tmpArr);
    ds.setFloatingPointPrecision(QDataStream::SinglePrecision);
    for (int i=0;i<traces;i++)
    {
        //tmpArr = new QByteArray(arrIn.mid(244+i*4,4));
        //tmpArr->append(arrIn.mid(244+i*4,4));
        //tmpArr->replace(0,4,arrIn.mid(244+i*4,4))
        ds>>traceValue;
        traceData->append(traceValue*mpFactor);
        //delete tmpArr;
        //tmpArr->clear();
        //tmpArr.replace(0,4,arrIn.mid(244+i*4,4));
        //ds->resetStatus();
        //delete ds;
        //ds = new QDataStream(tmpArr);
        //ds->setFloatingPointPrecision(QDataStream::SinglePrecision);

        //traceData->append(arrIn.mid(244+i*4,4).toFloat()*mpFactor);
        //traceData->append((SegdFile::toFloat(arrIn.mid(244+i*4,4)))*mpFactor);
    }
    tmpArr->clear();
    delete tmpArr;
    //delete ds;
}



SegdTrace::SegdTrace(const QByteArray &arrIn,const float &mpFactor, QObject *parent) : QObject(parent)
{
    //SegdFile *segd = qobject_cast<SegdFile*>(parent);
    generalTraceHeader=new TraceHeader(arrIn.mid(0,20));
    extHeader1=new TraceExtensionOne(arrIn.mid(20,32));
    extHeader2=new TraceExtensionTwo(arrIn.mid(52,32));
    extHeader3=new TraceExtensionThree(arrIn.mid(84,32));
    extHeader4=new TraceExtensionFour(arrIn.mid(116,32));
    extHeader5=new TraceExtensionFive(arrIn.mid(148,32));
    extHeader6=new TraceExtensionSix(arrIn.mid(180,32));
    extHeader7=new TraceExtensionSeven(arrIn.mid(212,32));
    int traces=extHeader1->samplesPerTrace;
    traceData = new QVector<float>;
    float traceValue;
    QByteArray *tmpArr =new QByteArray(arrIn.mid(244));
    QDataStream ds(tmpArr,QIODevice::ReadOnly);
    ds.setFloatingPointPrecision(QDataStream::SinglePrecision);
    for (int i=0;i<traces;i++)
    {

        ds>>traceValue;
        traceData->append(traceValue*mpFactor);
    }
    tmpArr->clear();
    delete tmpArr;
}
SegdTrace::~SegdTrace()
{
    delete generalTraceHeader;
    delete extHeader1;
    delete extHeader2;
    delete extHeader3;
    delete extHeader4;
    delete extHeader5;
    delete extHeader6;
    delete extHeader7;
    delete traceData;
    //traceData->clear();

}
TraceExtensionOne* SegdTrace::getExtensionOne()
{
    return extHeader1;
}

TraceExtensionThree* SegdTrace::getExtensionThree()
{
    return extHeader3;
}
TraceExtensionFive* SegdTrace::getExtensionFive()
{
    return extHeader5;
}
quint8 SegdTrace::getChannelType()
{
    return extHeader7->getChannelType();
}
TraceExtensionTwo* SegdTrace::getExtensionTwo()
{
    return extHeader2;
}
TraceExtensionSeven* SegdTrace::getExtensionSeven()
{
    return extHeader7;
}
QString SegdTrace::getTests()
{
    QString out;
    out = QString::number(generalTraceHeader->getExtendedFileNum())+"\t"+QString::number(extHeader1->getReceiverLineNum())+
            "\t"+QString::number(extHeader1->getReceiverPointNum())+"\t"+QString::number(extHeader3->getResistanceValue())+
            "\t"+QString::number(extHeader3->getResistanceError())+"\t"+QString::number(extHeader3->getTiltValue())+
            "\t"+QString::number(extHeader3->getTiltError())+"\t"+QString::number(extHeader5->getLeakageValue())+
            "\t"+QString::number(extHeader5->getLeakageError())+"\t"+QString::number(extHeader2->getX())+"\t"+QString::number(extHeader2->getY())+
            "\t"+QString::number(extHeader2->getZ());
    return out;
}
TraceHeader* SegdTrace::getTraceHeader()
{
    return generalTraceHeader;
}

QVector<float> SegdTrace::getTraceData()
{
    return *traceData;
}

float SegdTrace::getMaxA()
{
    return *std::max_element(traceData->begin(),traceData->end());
}
