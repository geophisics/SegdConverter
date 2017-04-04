#ifndef BASEWORKER_H
#define BASEWORKER_H

#include <QObject>

#include <SUB/point.h>
#include <SUB/exclusion.h>
#include <attributewindow.h>
#include <QtCharts/QLineSeries>
#include <SUB/general.h>
#include <QSettings>
#include <QFile>
#include <QTextStream>
#include <QFileSystemWatcher>

typedef QMap<QString,QString> Paths;


class SegdTrace;
class SegdFile;
struct TimeBreakSettings
{
    bool check;
    uint traceNb;
    uint nbOfDiscret;
    uint maxTime;
    float maxAmpl;
    TimeBreakSettings();
};

struct AkfSettings
{
    AkfSettings();
    quint8 traceNb;
    int frqLvl;
    uint maxTime,minFrq,maxFrq;
    double maxAmpl;

};

class BaseWorker : public QObject
{
    Q_OBJECT

public:
    enum exclusionType {mesaExcl, txtExcl, noExcl};
    enum readMode{fromDir,fromFile};
    enum writeAuxesMode {write,writeInNewFile,noWrite};
    explicit BaseWorker(CountedAttributes *attr);

protected:


    Paths paths;
    QString segdPath;
    QString outPath;
    QString outAuxesPath;
    QString attrFilePath;
    //QString spsPath;
    //QString rpsPath;
    //QString xpsPath;



    bool SercelMpFactor;
    //readMode rMode;
    bool mode; // true - Dir, false - File
    bool writeMutedChannels;
    bool writeMissedChannels;
    bool analysisAuxes;
    AkfSettings akf;
    TimeBreakSettings timeBreak, confirmedTimeBreak, upholeTime;
    float userMpFactorValue;
    bool useExternalXps;
    //параметры трасс для расчета атрибутов
    bool notUseMutedTraces;
    bool badTests;
    float minAmpl;
    //-----------------------------------

    //параметры анализа тестов
    bool checkTests;
    int testsPercent;
    //--------------------------
    bool backup;
    bool limitMaxFiles;
    int maxFilesValue;
    int waitingTime;
    bool *run;
    QString BackupFolder;
    QMap<QString, Point> pp;
    QMap<QString, Point> pv;

   // bool useExclusions; // используем или нет экслюзивные зоны
    exclusionType exType;
    writeAuxesMode auxMode;

    QVector<Exclusion*> exclusions;
    QVector<int> exclPoints;
    QSettings *settings;
    QVector<AttributeWindow> windows;
    QVector<Relation> relations;
    uint energyMaxFreq, energyMinFreq, widthLvl;
    bool widthByEnergy,rmsSpectrum;

    QFile *logFile;
    QTextStream *logStream;

    //QVector <QVariant> *ffidAttributes;


    CountedAttributes* attributes;
    AttributesFromFile fileAttributes;

public:
    void setSegdPath(const QString &path);
    void setOutPath(const QString &path);
    void setAttrFilePath(const QString &path);
    void setXpsPath(const QString &path);

    void setMode (const bool &md);
    void setUseExternalSps(const bool &use);
    void setUseExternalRps(const bool &use);
    void setUseExternalXps(const bool &use);
    void readRps(const QString &path);
    void readSps(const QString &path);
    void readSettings();

protected:


    void setExclusions(const QString &exclFileName);
    void setReceiversInExclusions(const QString &exclFileName);
    void createFileForMissedTraces();

    //функции расчета атрибутов
    void countAttriburesInWindow(QVector<QVector<float> > &traces, const int &winNb, const int &sInt, const int &ffid, QMap<QString,float> *ampls); //
    void countRelations(QMap<QString,float> ampls);
    float getAbsAvg(QVector<QVector<float> > &tracesData); //абсолютная амплитуда
    float getRms(QVector <QVector<float> > &tracesData); //среднеквадратическая амплитуда
    double countFreq(QVector<QVector<float> > &tracesData, const int &sRate); // доминантная частота в окне
    double countFreqByTrace(QVector<float> &trace, const int &sRate); // доминантная частота одной трассы

    std::vector<double> getSpectrum(QVector<QVector<float> > tracesData); // спектр в окне
    std::vector<double> getSpectrumByTrace(std::vector<float> traceData);
    std::vector<double> getSpectrumByTrace(std::vector<float> traceData, const std::size_t &SIZE);// спектр одной трассы
    int maxSizeOfVectors(QVector<QVector<float> > tracesData);

    float getEnergy(std::vector<double> spectrum, const float &frqStep); // энергия спектра
    int getWidth(std::vector<double> spectrum); // ширина спектра относительно максимаьной амплитуды спектра

    // отправляем данные служебных трасс
    void chekingAuxData(SegdFile *segd);
    void checkingTests(SegdFile *segd);

    QVector<float> getSpectrumDb(std::vector<float> traceData);

    bool checkTimeBreak(QVector<float> traceData, const int &sInt);
    bool checkConfirmedTimeBreak (QVector<float> traceData, const int &sInt);

    QQueue<QString>* findTemplates(const int &ffid);

signals:
    void finished();
    void sendInfoMessage(QString, QColor);
    void sendSomeError(QString);
    void badSegdFile(QString);
    void sendAkf(QVector<float>*, int, int);
    void sendVibroAux(QVector<float>*,QVector<float>*,int,int);
    void sendSeries(QtCharts::QLineSeries *trace, QtCharts::QLineSeries *spectrum);
    void sendVectors(QVector<QPointF> *trace,bool,QVector<QPointF> *spectrum,bool,int);
    void sendExplAuxes(QVector<QPointF>* , bool,QVector<QPointF>* , bool,QVector<QPointF>* , bool );
    void attributesCounted();

public slots:
    void stopRunning();
};



#endif // BASEWORKER_H
