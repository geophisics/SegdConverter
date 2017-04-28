#include "auxviewdialog.h"
#include "ui_auxviewdialog.h"
#include "attributewindow.h"


QT_CHARTS_USE_NAMESPACE

/*AuxViewDialog::AuxViewDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AuxViewDialog)
{
    ui->setupUi(this);
    settings = new QSettings(QCoreApplication::applicationDirPath()+QDir::separator()+"config.ini",QSettings::IniFormat,this);
    readSettings();
    //звук предупреждения
    alert = new QSound(":/sound/whistle8.wav",this);
    alert->setLoops(QSound::Infinite);
    //
    firstChart = new QChart();
    firstChart->legend()->setVisible(false);
    secondChart = new QChart();
    secondChart->legend()->setVisible(false);
    thirdChart = new QChart();
    thirdChart->legend()->setVisible(false);
    //
    firstSeries = new QLineSeries;
    secondSeries = new QLineSeries;
    thirdSeries = new QLineSeries;
    countedUpholeSeries = new QLineSeries;
    headerUpholeSeries = new QLineSeries;
    //
    firstChart->addSeries(firstSeries);
    secondChart->addSeries(secondSeries);
    thirdChart->addSeries(thirdSeries);
    thirdChart->addSeries(countedUpholeSeries);
    thirdChart->addSeries(headerUpholeSeries);


    firstChart->createDefaultAxes();
    secondChart->createDefaultAxes();
    thirdChart->createDefaultAxes();
    //firstChart->setMargins(QMargins(0,0,0,0));
    firstChart->axisX()->setVisible(false);
    firstChart->axisY()->setReverse(true);
    thirdChart->axisY()->setReverse(true);
    firstView = new QChartView(firstChart);
    firstView->setFrameStyle(QFrame::NoFrame);
    firstView->setRubberBand(QChartView::NoRubberBand);
    firstView->setRenderHint(QPainter::Antialiasing,true);


    ui->horizontalLayout->addWidget(firstView,0);
    firstView = new QChartView(secondChart);
    firstView->setFrameStyle(QFrame::NoFrame);
    firstView->setRubberBand(QChartView::NoRubberBand);
    firstView->setRenderHint(QPainter::Antialiasing,true);
    ui->horizontalLayout->addWidget(firstView,0);
    firstView = new QChartView(thirdChart);
    firstView->setRubberBand(QChartView::NoRubberBand);
    firstView->setFrameStyle(QFrame::NoFrame);
    firstView->setRenderHint(QPainter::Antialiasing,true);
    ui->horizontalLayout->addWidget(firstView,0);

    firstView->setFrameStyle(QFrame::NoFrame);

    ui->horizontalLayout->setContentsMargins(0,0,0,0);
    ui->horizontalLayout->setSpacing(0);

    connect(ui->prevPushButton,SIGNAL(clicked(bool)),this,SLOT(previousButtonClicked()));
    connect(ui->nextPushButton,SIGNAL(clicked(bool)),this,SLOT(nextButtonClicked()));
    //ui->horizontalLayout->setStretch(1,0);
}*/


AuxViewDialog::AuxViewDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AuxViewDialog)
{
    ui->setupUi(this);
    settings = new QSettings(QCoreApplication::applicationDirPath()+QDir::separator()+"config.ini",QSettings::IniFormat,this);
    readSettings();
    //звук предупреждения
    alert = new QSound(":/sound/whistle8.wav",this);
    alert->setLoops(QSound::Infinite);
    //-------------------------------------------
    QChartView *p_chartView;
    QChart *p_chart;
    for (int i=0; i<3; i++)
    {
        p_chart = new QChart();
        p_chart->legend()->setVisible(false);
        p_chart->addSeries(new QLineSeries);
        if (i==2)
        {
            p_chart->addSeries(new QLineSeries);
            p_chart->addSeries(new QLineSeries);
        }
        p_chart->createDefaultAxes();
        p_chart->axisY()->setReverse(true);
        p_chartView = new QChartView(p_chart,this);
        p_chartView->setRubberBand(QChartView::NoRubberBand);
        p_chartView->setRenderHint(QPainter::Antialiasing,true);
        p_chartView->setFrameStyle(QFrame::NoFrame);
        ui->horizontalLayout->addWidget(p_chartView);
    }
    connect(ui->prevPushButton,SIGNAL(clicked(bool)),this,SLOT(previousButtonClicked()));
    connect(ui->nextPushButton,SIGNAL(clicked(bool)),this,SLOT(nextButtonClicked()));
}
AuxViewDialog::~AuxViewDialog()
{
    saveSettings();
    delete ui;
}

void AuxViewDialog::readSettings()
{
    settings->beginGroup("/VibAuxSettings");
    minFreq = settings->value("/MinFreq",7).toUInt();
    maxFreq = settings->value("MaxFreq",100).toUInt();
    maxTimeAkf = settings->value("/PeakTime",500).toUInt();
    frqLvl = settings->value("/FreqLvl",50).toInt();
    settings->endGroup();
    settings->beginGroup("/AuxViewSettings");
    ui->correctAuxThemeComboBox->setCurrentIndex(settings->value("/CorrectAuxTheme",0).toInt());
    ui->incorrectAuxThemeComboBox->setCurrentIndex(settings->value("/IncorrectAuxTheme",2).toInt());
    ui->soundCheckBox->setChecked(settings->value("/SoundAlert",false).toBool());
    settings->endGroup();
}
void AuxViewDialog::saveSettings()
{
    settings->beginGroup("/AuxViewSettings");
    settings->setValue("/CorrectAuxTheme",ui->correctAuxThemeComboBox->currentIndex());
    settings->setValue("/IncorrectAuxTheme",ui->incorrectAuxThemeComboBox->currentIndex());
    settings->setValue("/SoundAlert",ui->soundCheckBox->isChecked());
    settings->endGroup();
}
void AuxViewDialog::receiveVectors(QVector<QPointF> *trace, const bool &traceStatus, QVector<QPointF> *spectrum, const bool &spectrumStatus, const int &ffid)
{
    AuxData data;
    data.auxData.append(trace);
    data.auxData.append(spectrum);
    data.checkData.append(traceStatus);
    data.checkData.append(spectrumStatus);
    auxIterator = auxes.insert(ffid,data);
    showAuxes();
}
void AuxViewDialog::receiveExplAuxes(const int &ffid, QVector<QPointF> *timeBreakTrace, const bool &timeBreakStatus, QVector<QPointF> *confirmedTimeBreakTrace, const bool &confirmedTimeBreakStatus, QVector<QPointF> *upholeTimeTrace, const bool &uphleTimeStatus)
{
    AuxData data;
    data.auxData.append(timeBreakTrace);
    data.auxData.append(confirmedTimeBreakTrace);
    data.auxData.append(upholeTimeTrace);
    data.checkData.append(timeBreakStatus);
    data.checkData.append(confirmedTimeBreakStatus);
    data.checkData.append(uphleTimeStatus);
    data.countedUphole = 800.0 + 10.0;
    data.headerUphole = 800.0 + 20.0;
    auxIterator = auxes.insert(ffid,data);
    showAuxes();
}

void AuxViewDialog::receiveExplAuxes(const int &ffid,
                                     QVector<QPointF> *timeBreakTrace, const bool &timeBreakStatus,
                                     QVector<QPointF> *confirmedTimeBreakTrace, const bool &confirmedTimeBreakStatus,
                                     QVector<QPointF> *upholeTimeTrace,const bool &upholeTimeStatus, const float &countUphole, const float &headUphole)
{
    AuxData data;
    data.auxData.append(timeBreakTrace);
    data.auxData.append(confirmedTimeBreakTrace);
    data.auxData.append(upholeTimeTrace);
    data.checkData.append(timeBreakStatus);
    data.checkData.append(confirmedTimeBreakStatus);
    data.checkData.append(upholeTimeStatus);
    data.countedUphole = 800.0 +countUphole;
    data.headerUphole = 800.0 + headUphole;
    auxIterator = auxes.insert(ffid,data);
    showAuxes();
}
void AuxViewDialog::showAuxes()
{
    AuxData data = auxIterator.value();
    QChartView *p_chartView;
    QLineSeries *p_series;
    QRectF brect;
    float xR;
    if (data.auxData.count()==2)
    {
        ui->horizontalLayout->itemAt(2)->widget()->setVisible(false);
        ui->horizontalLayout->setStretch(1,1);
        for (int i=0 ; i<2 ;i++ )
        {
            p_chartView=qobject_cast<QChartView*>(ui->horizontalLayout->itemAt(i)->widget());
            if (data.checkData.at(i))
            {
                p_chartView->chart()->setTheme((QChart::ChartTheme)ui->correctAuxThemeComboBox->currentIndex());
                alert->stop();
            }
            else
            {
                p_chartView->chart()->setTheme((QChart::ChartTheme)ui->incorrectAuxThemeComboBox->currentIndex());
                if (ui->soundCheckBox->isChecked())
                {
                    alert->play();
                }
            }
            p_series =qobject_cast<QLineSeries*>(p_chartView->chart()->series().first());
            p_series->replace(*data.auxData.at(i));
            brect = QPolygonF(*data.auxData.at(i)).boundingRect();
            xR = brect.right()>brect.left() ? brect.right() : brect.left();
            switch (i) {
            case 0:
                p_chartView->chart()->setTitle(QString("AКФ файла №%1").arg(auxIterator.key()));
                p_chartView->chart()->axisX()->setRange(-1.1*xR,1.1*xR);
                p_chartView->chart()->axisY()->setRange(brect.top(),brect.bottom());
                break;
            case 1:
                p_chartView->chart()->setTitle(QString("Спектр АКФ файла №%1").arg(auxIterator.key()));
                p_chartView->chart()->axisX()->setRange(brect.left(),brect.right());
                p_chartView->chart()->axisY()->setRange(ceil(brect.top()/25-1)*25,brect.bottom());
                p_chartView->chart()->axisY()->setReverse(false);
                break;
            default:
                break;
            }
        }
        /*firstChart->setTitle(QString("AКФ файла №%1").arg(auxIterator.key()));
        secondChart->setTitle(QString("Спектр АКФ файла №%1").arg(auxIterator.key()));
        firstSeries->replace(*data.auxData.at(0));
        secondSeries->replace(*data.auxData.at(1));
        if (data.checkData.at(0))
        {
            firstChart->setTheme((QChart::ChartTheme)ui->correctAuxThemeComboBox->currentIndex());
        }
        else
        {
            firstChart->setTheme((QChart::ChartTheme)ui->incorrectAuxThemeComboBox->currentIndex());
            if (ui->soundCheckBox->isChecked())
            {
                alert->play();
            }
        }
        if (data.checkData.at(1))
        {
            secondChart->setTheme((QChart::ChartTheme)ui->correctAuxThemeComboBox->currentIndex());
        }
        else
        {
            secondChart->setTheme((QChart::ChartTheme)ui->incorrectAuxThemeComboBox->currentIndex());
            if (ui->soundCheckBox->isChecked())
            {
                alert->play();
            }
        }
        //масштабируем шкалы
        QRectF brect = QPolygonF(*data.auxData.at(1)).boundingRect();
        //график спектра
        int maxAxisValue = ceil(brect.bottom());
        int minAxisValue = ceil(brect.top()/25-1)*25;
        QValueAxis *axis = qobject_cast<QValueAxis*>(secondChart->axisY());
        axis->setRange(minAxisValue,maxAxisValue);
        axis->setTickCount((maxAxisValue-minAxisValue)/25+1);
        secondChart->axisY()->setReverse(false);
        secondChart->axisX()->setRange(brect.left(),brect.right());
        //--------------------------------
        //график трассы
        brect = QPolygonF(*data.auxData.at(0)).boundingRect();
        firstChart->axisY()->setRange(brect.top(),brect.bottom());
        firstChart->axisX()->setRange(brect.right()*-1.1,brect.right()*1.1);*/

    }
    else
    {
        ui->horizontalLayout->itemAt(2)->widget()->setVisible(true);
        ui->horizontalLayout->setStretch(1,0);
        for (int i=0; i<3; i++)
        {
            p_chartView=qobject_cast<QChartView*>(ui->horizontalLayout->itemAt(i)->widget());
            if (data.checkData.at(i))
            {
                p_chartView->chart()->setTheme((QChart::ChartTheme)ui->correctAuxThemeComboBox->currentIndex());
                alert->stop();
            }
            else
            {
                p_chartView->chart()->setTheme((QChart::ChartTheme)ui->incorrectAuxThemeComboBox->currentIndex());
                if (ui->soundCheckBox->isChecked())
                {
                    alert->play();
                }
            }
            p_series =qobject_cast<QLineSeries*>(p_chartView->chart()->series().first());
            p_series->replace(*data.auxData.at(i));
            brect = QPolygonF(*data.auxData.at(i)).boundingRect();
            p_chartView->chart()->axisY()->setRange(brect.top(),brect.bottom());
            xR = brect.right()>brect.left() ? brect.right() : brect.left();
            p_chartView->chart()->axisX()->setRange(-1.1*xR,1.1*xR);
            switch (i) {
            case 0:
                p_chartView->chart()->setTitle(QString("Time Break FFID #%1").arg(auxIterator.key()));
                break;
            case 1:
                p_chartView->chart()->setTitle(QString("Confirmed Time Break FFID#%1").arg(auxIterator.key()));
                p_chartView->chart()->axisY()->setReverse(true);
                break;
            case 2:
                p_chartView->chart()->setTitle(QString("Uphole Time FFID#%1").arg(auxIterator.key()));
                p_series =qobject_cast<QLineSeries*>(p_chartView->chart()->series().at(1));
                p_series->clear();
                p_series->append(-0.5*xR,data.countedUphole);
                p_series->append( 0.5*xR,data.countedUphole);
                p_series =qobject_cast<QLineSeries*>(p_chartView->chart()->series().at(2));
                p_series->clear();
                p_series->append(-0.5*xR,data.headerUphole);
                p_series->append( 0.5*xR,data.headerUphole);
                break;
            default:
                break;
            }
        }
    }
}

void AuxViewDialog::previousButtonClicked()
{
    if (auxes.isEmpty())
    {
        return;
    }
    if (auxIterator == auxes.begin())
    {
        auxIterator = auxes.end();

    }

    auxIterator--;


    showAuxes();
}

void AuxViewDialog::nextButtonClicked()
{
    if (auxes.isEmpty())
    {
        return;
    }
    auxIterator++;
    if (auxIterator ==auxes.end())
    {
        auxIterator = auxes.begin();
    }
    showAuxes();
}


/*void AuxViewDialog::receiveVectors(QVector<QPointF> *trace, const bool &traceStatus, QVector<QPointF> *spectrum, const bool &spectrumStatus, const int &ffid)
{
    ui->horizontalLayout->itemAt(2)->widget()->setVisible(false);
    ui->horizontalLayout->setStretch(1,1);
    firstChart->setTitle(QString("AКФ файла №%1").arg(ffid));
    secondChart->setTitle(QString("Спектр АКФ файла №%1").arg(ffid));

    firstSeries->replace(*trace);
    secondSeries->replace(*spectrum);
    if ((!traceStatus || !spectrumStatus) && ui->soundCheckBox->isChecked())
    {
        this->show();
        alert->play();
    }
    else
    {
        alert->stop();
    }
    if (!traceStatus)
    {
        firstChart->setTheme((QChart::ChartTheme)ui->incorrectAuxThemeComboBox->currentIndex());//       QChart::ChartThemeDark);
    }
    else
    {
        firstChart->setTheme((QChart::ChartTheme)ui->correctAuxThemeComboBox->currentIndex());
    }
    if (!spectrumStatus)
    {
        secondChart->setTheme((QChart::ChartTheme)ui->incorrectAuxThemeComboBox->currentIndex());
    }
    else
    {
        secondChart->setTheme((QChart::ChartTheme)ui->correctAuxThemeComboBox->currentIndex());
    }
    //масштабируем шкалы
    QRectF brect = QPolygonF(*spectrum).boundingRect();
    //график спектра
    int maxAxisValue = ceil(brect.bottom());
    int minAxisValue = ceil(brect.top()/25-1)*25;
    QValueAxis *axis = qobject_cast<QValueAxis*>(secondChart->axisY());
    axis->setRange(minAxisValue,maxAxisValue);
    axis->setTickCount((maxAxisValue-minAxisValue)/25+1);
    secondChart->axisX()->setRange(brect.left(),brect.right());
    //--------------------------------
    //график трассы
    brect = QPolygonF(*trace).boundingRect();
    firstChart->axisY()->setRange(brect.top(),brect.bottom());
    firstChart->axisX()->setRange(brect.right()*-1.1,brect.right()*1.1);
    //---------------------------------
    delete trace;
    delete spectrum;
}*/

/*void AuxViewDialog::receiveExplAuxes(QVector<QPointF> *timeBreakTrace, const bool &timeBreakStatus, QVector<QPointF> *confirmedTimeBreakTrace, const bool &confirmedTimeBreakStatus, QVector<QPointF> *upholeTimeTrace, const bool &uphleTimeStatus)
{
    ui->horizontalLayout->itemAt(2)->widget()->setVisible(true);
    ui->horizontalLayout->setStretch(1,0);
    firstChart->setTitle("Time Break");
    secondChart->setTitle("Confirmed Time Break");
    thirdChart->setTitle("Uphole Time");
    firstSeries->replace(*timeBreakTrace);
    secondSeries->replace(*confirmedTimeBreakTrace);
    thirdSeries->replace(*upholeTimeTrace);
    if (!(timeBreakStatus && confirmedTimeBreakStatus && uphleTimeStatus) && ui->soundCheckBox->isChecked())
    {
        this->show();
        alert->play();
    }
    else
    {
        alert->stop();
    }
    if (timeBreakStatus)
    {
        firstChart->setTheme((QChart::ChartTheme)ui->correctAuxThemeComboBox->currentIndex());
    }
    else
    {
        firstChart->setTheme((QChart::ChartTheme)ui->incorrectAuxThemeComboBox->currentIndex());
    }
    if (confirmedTimeBreakStatus)
    {
        secondChart->setTheme((QChart::ChartTheme)ui->correctAuxThemeComboBox->currentIndex());
    }
    else
    {
        secondChart->setTheme((QChart::ChartTheme)ui->incorrectAuxThemeComboBox->currentIndex());
    }
    if (uphleTimeStatus)
    {
        thirdChart->setTheme((QChart::ChartTheme)ui->correctAuxThemeComboBox->currentIndex());
    }
    else
    {
        thirdChart->setTheme((QChart::ChartTheme)ui->incorrectAuxThemeComboBox->currentIndex());
    }
    QRectF brect = QPolygonF(*timeBreakTrace).boundingRect();
    firstChart->axisY()->setRange(brect.top(),brect.bottom());
    firstChart->axisX()->setRange(brect.right()*-1.1,brect.right()*1.1);
    brect = QPolygonF(*confirmedTimeBreakTrace).boundingRect();
    secondChart->axisY()->setRange(brect.top(),brect.bottom());
    secondChart->axisX()->setRange(brect.right()*-1.1,brect.right()*1.1);
    secondChart->axisY()->setReverse(true);
    brect = QPolygonF(*upholeTimeTrace).boundingRect();
    thirdChart->axisY()->setRange(brect.top(),brect.bottom());
    thirdChart->axisX()->setRange(brect.right()*-1.1,brect.right()*1.1);
    thirdChart->axisY()->setReverse(true);
    delete confirmedTimeBreakTrace;
    delete timeBreakTrace;
    delete upholeTimeTrace;
}*/

bool AuxViewDialog::showAuxesByFfid(const int &fileNum)
{
    auxIterator = auxes.find(fileNum);
    if (auxIterator==auxes.end()) {
        return false;
    }
    else {
        showAuxes();
    }
    return true;
}

void AuxViewDialog::clearData()
{
    foreach (AuxData data, auxes) {
        qDeleteAll(data.auxData);
    }
    auxes.clear();
    this->hide();
    auxIterator = auxes.begin();
}

AuxData::~AuxData()
{

}
