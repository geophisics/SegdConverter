#include "auxviewdialog.h"
#include "ui_auxviewdialog.h"
#include "attributewindow.h"
#include <QGraphicsSceneResizeEvent>


QT_CHARTS_USE_NAMESPACE

AuxViewDialog::AuxViewDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AuxViewDialog)
{
    ui->setupUi(this);
    settings = new QSettings(QCoreApplication::applicationDirPath()+QDir::separator()+"config.ini",QSettings::IniFormat,this);
    readSettings();
    QFont font = ui->headerTvLabel->font();
    font.setPixelSize(18);
    font.setBold(true);
    font.setItalic(true);
    ui->headerTvLabel->setFont(font);
    ui->countTvLabel->setFont(font);

    //ui->alarmSoundComboBox->add
    //звук предупреждения
    alert = new QSound();//(":/sound/whistle8.wav",this);
    //alert->setLoops(QSound::Infinite);
    //-------------------------------------------
    auxWidget = new AuxesWidget(this);
    ui->gridLayout->addWidget(auxWidget,1,0,1,1);
    ui->gridLayout->setRowStretch(1,1);
    this->grabGesture(Qt::PanGesture);
    this->grabGesture(Qt::PinchGesture);
    connect(ui->prevPushButton,SIGNAL(clicked(bool)),this,SLOT(previousButtonClicked()));
    connect(ui->nextPushButton,SIGNAL(clicked(bool)),this,SLOT(nextButtonClicked()));
    connect(ui->correctAuxThemeComboBox,SIGNAL(currentIndexChanged(int)),auxWidget,SLOT(setGoodTheme(int)));
    connect(ui->incorrectAuxThemeComboBox,SIGNAL(currentIndexChanged(int)),auxWidget,SLOT(setBadTheme(int)));
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
    //ui->soundCheckBox->setChecked(settings->value("/SoundAlert",false).toBool());
    settings->endGroup();
}
void AuxViewDialog::saveSettings()
{
    settings->beginGroup("/AuxViewSettings");
    settings->setValue("/CorrectAuxTheme",ui->correctAuxThemeComboBox->currentIndex());
    settings->setValue("/IncorrectAuxTheme",ui->incorrectAuxThemeComboBox->currentIndex());
    //settings->setValue("/SoundAlert",ui->soundCheckBox->isChecked());
    settings->endGroup();
}
void AuxViewDialog::receiveVibAuxes(QVector<QPointF> *trace, const bool &traceStatus, QVector<QPointF> *spectrum, const bool &spectrumStatus, const int &ffid)
{
    AuxData data;
    data.auxData.append(trace);
    data.auxData.append(spectrum);
    data.checkData.append(traceStatus);
    data.checkData.append(spectrumStatus);
    auxIterator = auxes.insert(ffid,data);
    auxWidget->paintAuxes(auxIterator.key(), auxIterator.value());
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
    ui->headerTvLabel->setText(QString::number(auxIterator.value().headerUphole-800.0,'f',1));
    ui->countTvLabel->setText(QString::number(auxIterator.value().countedUphole-800.0,'f',1));
    auxWidget->paintAuxes(auxIterator.key(), auxIterator.value());
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
    ui->headerTvLabel->setText(QString::number(auxIterator.value().headerUphole-800.0,'f',1));
    ui->countTvLabel->setText(QString::number(auxIterator.value().countedUphole-800.0,'f',1));
    auxWidget->paintAuxes(auxIterator.key(), auxIterator.value());
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
    ui->headerTvLabel->setText(QString::number(auxIterator.value().headerUphole-800.0,'f',1));
    ui->countTvLabel->setText(QString::number(auxIterator.value().countedUphole-800.0,'f',1));
    auxWidget->paintAuxes(auxIterator.key(), auxIterator.value());
}

void AuxViewDialog::setAlarmSound(const int &i)
{

}

bool AuxViewDialog::showAuxesByFfid(const int &fileNum)
{
    auxIterator = auxes.find(fileNum);
    if (auxIterator==auxes.end()) {
        return false;
    }
    else {
        auxWidget->paintAuxes(auxIterator.key(), auxIterator.value());
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

AuxesWidget::AuxesWidget(QWidget *parent):QWidget(parent)
{
    goodTheme = QChart::ChartThemeLight;
    badTheme = QChart::ChartThemeDark;
    viewLayout = new QGridLayout;
    viewLayout->setHorizontalSpacing(0);
    AuxChartView *p_chartView;
    AuxChart *p_chart;
    for (int i=0; i<3; i++)
    {
        p_chart = new AuxChart;
        p_chart->setMargins(QMargins(0,15,0,0));
        p_chart->legend()->setVisible(false);
        p_chart->addSeries(new QLineSeries);
        p_chart->setTheme(goodTheme);
        if (i==2)
        {
            p_chart->addSeries(new QLineSeries);
            p_chart->addSeries(new QLineSeries);
            p_chart->setTheme(badTheme);
        }
        p_chart->createDefaultAxes();
        p_chart->axisY()->setReverse(true);
        p_chart->axisX()->setTitleText("Амплитуда");
        p_chart->axisY()->setTitleText("Время, мс");
        p_chart->setAnimationOptions(QChart::NoAnimation);
        p_chartView = new AuxChartView(p_chart);
        p_chartView->setBackgroundBrush(QBrush(Qt::green,Qt::SolidPattern));
        p_chartView->setRenderHint(QPainter::Antialiasing);
        connect(p_chartView,SIGNAL(sendCoordinates(QString)),this,SLOT(recieveCoordinates(QString)));
        viewLayout->addWidget(p_chartView,0,i,1,1);
    }
    viewLayout->setContentsMargins(3,0,3,0);
    statBar = new QStatusBar;
    coordinatesLabel = new QLabel("Координаты",statBar);
    statBar->addWidget(coordinatesLabel);
    statBar->setSizeGripEnabled(false);
    viewLayout->addWidget(statBar,1,0,3,1);
    this->setLayout(viewLayout);
}

AuxesWidget::~AuxesWidget()
{

}

void AuxesWidget::setGoodTheme(const int &i)
{
    goodTheme = static_cast<QChart::ChartTheme>(i);
}

void AuxesWidget::setBadTheme(const int &i)
{
    badTheme = static_cast<QChart::ChartTheme>(i);
}

void AuxesWidget::unselectChartViews()
{
    AuxChartView *p_chartView;
    for (int i=0; i<3; i++)
    {
        p_chartView=reinterpret_cast<AuxChartView*>(viewLayout->itemAt(i)->widget());
        p_chartView->setBackgroundBrush(QBrush(Qt::white,Qt::SolidPattern));
    }
}

void AuxesWidget::recieveCoordinates(const QString &coordinates)
{
    coordinatesLabel->setText(coordinates);
}

void AuxesWidget::paintAuxes(const int &ffid, const AuxData &data)
{
    QChartView *p_chartView;
    QLineSeries *p_series;
    AuxChart *auxChart;// = static_cast<AuxChart*>(this->chart());
    QRectF brect;
    float xR;
    if (data.auxData.count()==2)
    {
        viewLayout->itemAt(2)->widget()->setVisible(false);
        //viewLayout->setStretch(1,1);

        for (int i=0 ; i<2 ;i++ )
        {
            p_chartView=qobject_cast<QChartView*>(viewLayout->itemAt(i)->widget());
            if (data.checkData.at(i))
            {
                p_chartView->chart()->setTheme(goodTheme);
        //        alert->stop();
            }
            else
            {
                p_chartView->chart()->setTheme(badTheme);
            /*    if (ui->soundCheckBox->isChecked())
                {
                    alert->play();
                }*/
            }
            p_series =qobject_cast<QLineSeries*>(p_chartView->chart()->series().first());
            p_series->replace(*data.auxData.at(i));
            brect = QPolygonF(*data.auxData.at(i)).boundingRect();
            xR = brect.right()>brect.left() ? brect.right() : brect.left();
            switch (i) {
            case 0:
                p_chartView->chart()->setTitle(QString("AКФ файла №%1").arg(ffid));
                p_chartView->chart()->axisX()->setRange(-1.1*xR,1.1*xR);
                p_chartView->chart()->axisY()->setRange(brect.top(),brect.bottom());
                break;
            case 1:
                p_chartView->chart()->setTitle(QString("Спектр АКФ файла №%1").arg(ffid));
                p_chartView->chart()->axisX()->setRange(brect.left(),brect.right());
                p_chartView->chart()->axisY()->setRange(ceil(brect.top()/25-1)*25,brect.bottom());
                p_chartView->chart()->axisY()->setReverse(false);
                break;
            default:
                break;
            }
        }
    }
    else
    {
        viewLayout->itemAt(2)->widget()->setVisible(true);
        //viewLayout->setStretch(1,0);
        for (int i=0; i<3; i++)
        {
            p_chartView=qobject_cast<QChartView*>(viewLayout->itemAt(i)->widget());
            auxChart = static_cast<AuxChart*>(p_chartView->chart());
            p_series = qobject_cast<QLineSeries*>(auxChart->series().first());
            if (data.checkData.at(i))
            {
               auxChart->setTheme(goodTheme);
               p_chartView->setBackgroundBrush(auxChart->backgroundBrush());
            }
            else
            {
                auxChart->setTheme(badTheme);
                p_chartView->setBackgroundBrush(QBrush(Qt::red,Qt::SolidPattern));
            }
            p_series->replace(*data.auxData.at(i));
            brect = QPolygonF(*data.auxData.at(i)).boundingRect();
            auxChart->SetYRangeLimits(brect.top(),brect.bottom());
            xR = brect.right()>abs(brect.left()) ? brect.right() :abs(brect.left());
            p_chartView->chart()->axisX()->setRange(-1.1*xR,1.1*xR);
            switch (i) {
            case 0:
                p_chartView->chart()->setTitle(QString("Файл №%1. Отметка момента.").arg(ffid));
                break;
            case 1:
                p_chartView->chart()->setTitle(QString("Файл №%1. Подтвержденная отметка момента.").arg(ffid));
                p_chartView->chart()->axisY()->setReverse(true);
                break;
            case 2:
                p_chartView->chart()->setTitle(QString("Файл №%1. Вертикальное время.").arg(ffid));
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



AuxChartView::AuxChartView(QChart *chart, QWidget *parent):
    QChartView(chart,parent)
{
    mousePressed = false;
}

AuxChartView::AuxChartView(QWidget *parent):
    QChartView(parent)
{

}

void AuxChartView::mousePressEvent(QMouseEvent *event)
{
    if (event->button()==Qt::LeftButton)
    {
        mousePressed = true;
        this->setCursor(Qt::OpenHandCursor);
        yOld = event->pos().y();
    }
    QChartView::mousePressEvent(event);
}

void AuxChartView::mouseMoveEvent(QMouseEvent *event)
{
    AuxChart *auxChart = static_cast<AuxChart*>(this->chart());
    QValueAxis *yAxis = qobject_cast<QValueAxis*>(auxChart->axisY());
    if (mousePressed)
    {
        qreal yNew = event->pos().y();
        yNew<yOld ? auxChart->yScrollUp() : auxChart->yScrollDown();
        yOld=yNew;
    }
    qreal time = yAxis->max()+yAxis->min()-this->chart()->mapToValue(event->pos()).y();
    emit sendCoordinates(auxChart->getCoordinatesForTime(time));
    QChartView::mouseMoveEvent(event);
}

void AuxChartView::mouseReleaseEvent(QMouseEvent *event)
{
    mousePressed = false;
    this->setCursor(Qt::ArrowCursor);
    QChartView::mouseReleaseEvent(event);
}
void AuxChartView::keyPressEvent(QKeyEvent *event)
{
    AuxChart *auxChart = static_cast<AuxChart*>(this->chart());
    switch (event->key()) {
    case Qt::Key_Plus:
        auxChart->yZoomIn();
        break;
    case Qt::Key_Minus:
        auxChart->yZoomOut();
        break;
    case Qt::Key_Up:
        auxChart->yScrollDown();
        break;
    case Qt::Key_Down:
        auxChart->yScrollUp();
        break;
    default:
        QGraphicsView::keyPressEvent(event);
        break;
    }
}

void AuxChartView::wheelEvent(QWheelEvent *event)
{
    AuxChart *auxChart = static_cast<AuxChart*>(this->chart());
    if (event->angleDelta().y()>0)
    {
        auxChart->yZoomIn();
    }
    else
    {
        auxChart->yZoomOut();
    }
}

void AuxChartView::resizeEvent(QResizeEvent *event)
{
    if (scene()) {
        scene()->setSceneRect(QRect(QPoint(0, 0), event->size()));
        QValueAxis *axis = qobject_cast<QValueAxis*>(this->chart()->axisY());
        axis->setTickCount(event->size().height()/50);
        axis =qobject_cast<QValueAxis*>(this->chart()->axisX());
        int newTickCount = event->size().width()/100;
        if (newTickCount%2==0)
        {
            newTickCount++;
        }
        axis->setTickCount(newTickCount);
        chart()->resize(event->size());
    }
    QGraphicsView::resizeEvent(event);
}

AuxChart::AuxChart(QGraphicsItem *parent, Qt::WindowFlags wFlags)
    : QChart(QChart::ChartTypeCartesian, parent, wFlags)
{
    grabGesture(Qt::PanGesture);
    grabGesture(Qt::PinchGesture);
    yRangeLimits= qMakePair(0.0,1000.0);
}

AuxChart::~AuxChart()
{

}

void AuxChart::SetYRangeLimits(const float &min, const float &max)
{
    QValueAxis *yAxis = qobject_cast<QValueAxis*>(this->axisY());
    if (yAxis->min()<min || yAxis->max()>max)
    {
        yAxis->setRange(min,max);
    }
    yRangeLimits = qMakePair(min,max);
}

void AuxChart::yZoomIn()
{
    QValueAxis *yAxis = qobject_cast<QValueAxis*>(this->axisY());
    qreal center = (yAxis->max() + yAxis->min())/2.0;
    qreal newLength = (yAxis->max()-yAxis->min())/2.0;
    yAxis->setMin(center-newLength/2.0);
    yAxis->setMax(center+newLength/2.0);
    scrollFactor = (yAxis->max()-yAxis->min())/50.0;
}

void AuxChart::yZoomOut()
{
    QValueAxis *yAxis = qobject_cast<QValueAxis*>(this->axisY());
    qreal center = (yAxis->max() + yAxis->min())/2.0;
    qreal newLength = (yAxis->max()-yAxis->min());
    qreal newMin = center - newLength > yRangeLimits.first ? center-newLength : yRangeLimits.first;
    qreal newMax = center + newLength < yRangeLimits.second ? center + newLength : yRangeLimits.second;
    yAxis->setRange(newMin,newMax);
    scrollFactor = (yAxis->max()-yAxis->min())/50.0;
}

void AuxChart::yScroll( qreal dy)
{
    QValueAxis *yAxis = qobject_cast<QValueAxis*>(this->axisY());
    qreal newMin = yAxis->min()+dy;
    qreal newMax;
    if (newMin < yRangeLimits.first)
    {
        dy =  yRangeLimits.first - yAxis->min();
        newMin = yRangeLimits.first;
        newMax = yAxis->max()+dy;
        yAxis->setRange(newMin,newMax);
        return;
    }
    newMax = yAxis->max()+dy;
    if (newMax > yRangeLimits.second)
    {
        dy = yRangeLimits.second-yAxis->max();
        newMin =yAxis->min()+dy;
        newMax = yRangeLimits.second;
        yAxis->setRange(newMin,newMax);
        return;
    }
    yAxis->setRange(newMin,newMax);
    return;
}


void AuxChart::yScrollUp()
{
    QValueAxis *yAxis = qobject_cast<QValueAxis*>(this->axisY());
    qreal dy=scrollFactor;
    qreal newMin = yAxis->min()+dy;
    qreal newMax;
    if (newMin < yRangeLimits.first)
    {
        dy =  yRangeLimits.first - yAxis->min();
        newMin = yRangeLimits.first;
        newMax = yAxis->max()+dy;
        yAxis->setRange(newMin,newMax);
        return;
    }
    newMax = yAxis->max()+dy;
    if (newMax > yRangeLimits.second)
    {
        dy = yRangeLimits.second-yAxis->max();
        newMin =yAxis->min()+dy;
        newMax = yRangeLimits.second;
        yAxis->setRange(newMin,newMax);
        return;
    }
    yAxis->setRange(newMin,newMax);
    return;
}

void AuxChart::yScrollDown()
{
    QValueAxis *yAxis = qobject_cast<QValueAxis*>(this->axisY());
    qreal dy=-1.0*scrollFactor;
    qreal newMin = yAxis->min()+dy;
    qreal newMax;
    if (newMin < yRangeLimits.first)
    {
        dy =  yRangeLimits.first - yAxis->min();
        newMin = yRangeLimits.first;
        newMax = yAxis->max()+dy;
        yAxis->setRange(newMin,newMax);
        return;
    }
    newMax = yAxis->max()+dy;
    if (newMax > yRangeLimits.second)
    {
        dy = yRangeLimits.second-yAxis->max();
        newMin =yAxis->min()+dy;
        newMax = yRangeLimits.second;
        yAxis->setRange(newMin,newMax);
        return;
    }
    yAxis->setRange(newMin,newMax);
    return;
}
QString AuxChart::getCoordinatesForTime(const float &t) const
{

    QXYSeries *series = qobject_cast<QXYSeries*>(this->series().first());
    QVector<QPointF> seriesData = series->pointsVector();
    float sInt = 1.0;
    QString chartName = this->title().mid(this->title().indexOf('.')+1);
    if (!seriesData.isEmpty() && t > yRangeLimits.first && t < yRangeLimits.second)
    {
        int i=0;
        QPointF point = seriesData.at(i);
        while (fabs(point.y()-t) > sInt)
        {
            i++;
            point = seriesData.at(i);
        }
        return  QString("%1 T=%2 A=%3").arg(chartName).arg(point.y()).arg(point.x());
    }
    return chartName;
}
