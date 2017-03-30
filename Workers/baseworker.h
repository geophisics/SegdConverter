#ifndef BASEWORKER_H
#define BASEWORKER_H

#include <QObject>
#include <QtXlsx>
#include <xlsxformat.h>
#include <SUB/point.h>
#include <SUB/exclusion.h>
#include <attributewindow.h>
#include <QtCharts/QLineSeries>
#include <QPair>
#include <SUB/general.h>

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

class BaseWorker : public QObject
{
    Q_OBJECT

public:
    enum exclusionType {mesaExcl, txtExcl};
    explicit BaseWorker(QObject *parent = 0);
    explicit BaseWorker(CountedAttributes *attr,QObject *parent=Q_NULLPTR);

protected:
    QString segdPath;
    QString outPath;
    QString outAuxesPath;
    QString attrFilePath;
    QString spsPath;
    QString rpsPath;
    QString xpsPath;
    QXlsx::Document xlsx;
    QXlsx::Format xlsxFormat; //формат ячейки в таблице xlsx

    bool SercelMpFactor;
    bool mode; // true - Dir, false - File
    //bool noMpFactor;
    bool writeMutedChannels;
    bool writeMissedChannels;
    bool analysisAuxes;
    quint8 akfTraceNb;
    int akfFrqLvl;
    uint akfMaxTime,akfMinFrq,akfMaxFrq;
    double akfMaxAmpl;

    TimeBreakSettings timeBreak, confirmedTimeBreak, upholeTime;

    float userMpFactorValue;
    bool writeAuxes;
    bool writeAuxesNewFile;
    bool useExternalSps;
    bool useExternalRps;
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
    QStringList relations;
    QMap<QString, Point*> pp;
    QMap<QString, Point*> pv;

    bool useExclusions; // используем или нет экслюзивные зоны
    exclusionType exType;

    QVector<Exclusion*> exclusions;
    QVector<int> exclPoints;
    QSettings *settings;
    QList<AttributeWindow*> windows;
    uint energyMaxFreq, energyMinFreq, widthLvl;
    bool widthByEnergy,rmsSpectrum;

    QFile *logFile;
    QTextStream *logStream;

    //QVector <QVariant> *ffidAttributes;


    CountedAttributes* attributes;
    AttributesFromFile fileAttributes;

    int currentRow; //текущая ячейка в таблице xlsx
    int currentColumn; //текущий столбец в таблице xlsx




public:
    void setSegdPath(const QString &path);
    void setOutPath(const QString &path);
    void setAttrFilePath(const QString &path);
    void setRpsPath(const QString &path);
    void setSpsPath(const QString &path);
    void setXpsPath(const QString &path);

    void setMode (const bool &md);
    void setUseExternalSps(const bool &use);
    void setUseExternalRps(const bool &use);
    void setUseExternalXps(const bool &use);
    bool readRps(const QString &path);
    bool readSps(const QString &path);
    void readSettings();

protected:


    void writeXlsxHeaders();
    void saveXlsxFile();
    void setExclusions(const QString &exclFileName);
    void setReceiversInExclusions(const QString &exclFileName);
    void createFileForMissedTraces();

    //функции расчета атрибутов
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
    void sendSegdData(FfidData);
    void sendSeisAttributes(SeisAttributes*, int);
    void sendRelation(QString realtionName, float relationValue, bool correct);
    void sendAkf(QVector<float>*, int, int);
    void sendVibroAux(QVector<float>*,QVector<float>*,int,int);
    void sendSeries(QtCharts::QLineSeries *trace, QtCharts::QLineSeries *spectrum);
    void sendVectors(QVector<QPointF> *trace,bool,QVector<QPointF> *spectrum,bool,int);
    void sendExplAuxes(QVector<QPointF>* , bool,QVector<QPointF>* , bool,QVector<QPointF>* , bool );
    void sendAuxStatus(bool);
    void sendTestStatus(float,QColor);
    void sendSegdAttributes(QVector<QVariant> *data);
    void fileConverted();

public slots:
    void stopRunning();
};



#endif // BASEWORKER_H
