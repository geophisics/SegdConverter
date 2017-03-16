#include "auxviewdialog.h"
#include "ui_auxviewdialog.h"
#include "attributewindow.h"


QT_CHARTS_USE_NAMESPACE

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

    firstChart->addSeries(firstSeries);
    secondChart->addSeries(secondSeries);
    thirdChart->addSeries(thirdSeries);
    firstChart->createDefaultAxes();
    secondChart->createDefaultAxes();
    thirdChart->createDefaultAxes();
    firstChart->setMargins(QMargins(0,0,0,0));
    firstChart->axisX()->setVisible(false);
    firstChart->axisY()->setReverse(true);
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
    //ui->horizontalLayout->setStretch(1,0);
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

    //secondChart->axisX()->setRange(0,250.0);
    //secondChart->axisY()->setRange(-100,0);
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
}

void AuxViewDialog::receiveExplAuxes(QVector<QPointF> *timeBreakTrace, const bool &timeBreakStatus, QVector<QPointF> *confirmedTimeBreakTrace, const bool &confirmedTimeBreakStatus, QVector<QPointF> *upholeTimeTrace, const bool &uphleTimeStatus)
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
}
