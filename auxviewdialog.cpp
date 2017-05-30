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
    auxWidget = new AuxesWidget(this);
    connect(ui->correctAuxThemeComboBox,SIGNAL(currentIndexChanged(int)),auxWidget,SLOT(setGoodTheme(int)));
    connect(ui->incorrectAuxThemeComboBox,SIGNAL(currentIndexChanged(int)),auxWidget,SLOT(setBadTheme(int)));

    readSettings();
    QFont font = ui->headerTvLabel->font();
    font.setPixelSize(18);
    font.setBold(true);
    font.setItalic(true);
    ui->headerTvLabel->setFont(font);
    ui->countTvLabel->setFont(font);
    alarmSound = new QSound("");
    setAlarmSound(ui->alarmSoundComboBox->currentIndex(),false);
    ui->gridLayout->addWidget(auxWidget,1,0,1,1);
    ui->gridLayout->setRowStretch(1,1);
    auxWidget->setVisible(false);
    connect(ui->prevPushButton,SIGNAL(clicked(bool)),this,SLOT(previousButtonClicked()));
    connect(ui->nextPushButton,SIGNAL(clicked(bool)),this,SLOT(nextButtonClicked()));
    connect(ui->alarmSoundComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(setAlarmSound(int)));

}
AuxViewDialog::~AuxViewDialog()
{
    saveSettings();
    delete ui;
}

void AuxViewDialog::readSettings()
{

    settings->beginGroup("/AuxViewSettings");
    ui->correctAuxThemeComboBox->setCurrentIndex(settings->value("/CorrectAuxTheme",0).toInt());
    ui->incorrectAuxThemeComboBox->setCurrentIndex(settings->value("/IncorrectAuxTheme",2).toInt());
    ui->alarmSoundComboBox->setCurrentIndex(settings->value("/AlarmSound",0).toInt());
    int size = settings->beginReadArray("ranges");
    QStringList ranges;
    ranges<<"tbRange"<<"confirmedTBrange"<<"upholeRange"<<"akfRange"<<"spectrumRange";
    QString name;
    QMap<QString,AxisRange> rangesMap;
    for (int i=0; i<size;++i)
    {
        settings->setArrayIndex(i);
        name = settings->value("RangeName","").toString();
        if (ranges.contains(name))
        {
            rangesMap.insert(name,AxisRange(settings->value("MinValue",0.0).toFloat(),settings->value("MaxValue",1000.0).toFloat()));
        }
    }
    auxWidget->setRanges(rangesMap);
    settings->endArray();
    settings->endGroup();
    settings->beginGroup("/VibAuxSettings");
    auxWidget->setFrqLvl(settings->value("/FreqLvl",50).toInt());
    settings->endGroup();
    settings->beginGroup("/ExplAuxSettings");
    auxWidget->setTbOffset(settings->value("/ConfirmedTimeBreakTvOffset",350).toInt());
    auxWidget->setTvOffset(settings->value("/UpholeTimeOffset",800).toInt());

    settings->endGroup();
}
void AuxViewDialog::saveSettings()
{
    settings->beginGroup("/AuxViewSettings");
    settings->setValue("/CorrectAuxTheme",ui->correctAuxThemeComboBox->currentIndex());
    settings->setValue("/IncorrectAuxTheme",ui->incorrectAuxThemeComboBox->currentIndex());
    settings->setValue("/AlarmSound",ui->alarmSoundComboBox->currentIndex());
    QMap<QString, AxisRange > ranges = auxWidget->getRanges();
    settings->beginWriteArray("ranges");
    for (int i=0;i<ranges.count();++i)
    {
        settings->setArrayIndex(i);
        settings->setValue("RangeName",ranges.keys().at(i));
        qDebug()<<"Range Name"<<ranges.keys().at(i)
               <<" Min"<<ranges.value(ranges.keys().at(i)).minValue
               <<" Max"<<ranges.value(ranges.keys().at(i)).maxValue;
        settings->setValue("MinValue",ranges.value(ranges.keys().at(i)).minValue);
        settings->setValue("MaxValue",ranges.value(ranges.keys().at(i)).maxValue);
    }
    settings->endArray();
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
    if (!(traceStatus || spectrumStatus))
    {
        alarmSound->play();
    }
    ui->headerTvFrame->setVisible(false);
    ui->countedTvFrame->setVisible(false);
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
    data.countedUphole = countUphole;
    data.headerUphole = headUphole;
    auxIterator = auxes.insert(ffid,data);
    ui->headerTvLabel->setText(QString::number(auxIterator.value().headerUphole,'f',1));
    ui->countTvLabel->setText(QString::number(auxIterator.value().countedUphole,'f',1));
    if (!(timeBreakStatus || confirmedTimeBreakStatus))
    {
        alarmSound->play();
    }
    ui->headerTvFrame->setVisible(true);
    ui->countedTvFrame->setVisible(true);
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
    ui->headerTvLabel->setText(QString::number(auxIterator.value().headerUphole,'f',1));
    ui->countTvLabel->setText(QString::number(auxIterator.value().countedUphole,'f',1));
    if (auxIterator.value().auxData.count()==2)
    {
        ui->headerTvFrame->setVisible(false);
        ui->countedTvFrame->setVisible(false);
    }
    else
    {
        ui->headerTvFrame->setVisible(true);
        ui->countedTvFrame->setVisible(true);
    }
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
    ui->headerTvLabel->setText(QString::number(auxIterator.value().headerUphole,'f',1));
    ui->countTvLabel->setText(QString::number(auxIterator.value().countedUphole,'f',1));
    if (auxIterator.value().auxData.count()==2)
    {
        ui->headerTvFrame->setVisible(false);
        ui->countedTvFrame->setVisible(false);
    }
    else
    {
        ui->headerTvFrame->setVisible(true);
        ui->countedTvFrame->setVisible(true);
    }
    auxWidget->paintAuxes(auxIterator.key(), auxIterator.value());
}

void AuxViewDialog::setAlarmSound(const int &i, const bool &b)
{
    delete alarmSound;
    switch (i) {
    case 1:
        alarmSound=new QSound(":/sound/sounds/ambulance.wav");
        break;
    case 2:
        alarmSound=new QSound(":/sound/sounds/bus.wav");
        break;
    case 3:
        alarmSound=new QSound(":/sound/sounds/taxi.wav");
        break;
    case 4:
        alarmSound=new QSound(":/sound/sounds/fire.wav");
        break;
    case 5:
        alarmSound=new QSound(":/sound/sounds/vuvuzela.wav");
        break;
    case 6:
        alarmSound=new QSound(":/sound/sounds/door.wav");
        break;
    case 7:
        alarmSound=new QSound(":/sound/sounds/clock.wav");
        break;
    default:
        alarmSound=new QSound("");
        break;
    }
    if (b) alarmSound->play();
    return;
}

bool AuxViewDialog::showAuxesByFfid(const int &fileNum)
{
    auxIterator = auxes.find(fileNum);
    if (auxIterator==auxes.end()) {
        return false;
    }
    else {
        auxWidget->paintAuxes(auxIterator.key(), auxIterator.value());
        if (auxIterator.value().auxData.count()==2)
        {
            ui->headerTvFrame->setVisible(false);
            ui->countedTvFrame->setVisible(false);
        }
        else
        {
            ui->headerTvFrame->setVisible(true);
            ui->countedTvFrame->setVisible(true);
        }
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
    ranges.insert("tbRange",AxisRange(0.0,1000.0));
    ranges.insert("confirmedTBrange",AxisRange(0.0,1000.0));
    ranges.insert("upholeRange",AxisRange(0.0,1000.0));
    ranges.insert("akfRange",AxisRange(0.0,1000.0));
    ranges.insert("spectrumRange",AxisRange(-200,0.0));
    goodTheme = QChart::ChartThemeLight;
    badTheme = QChart::ChartThemeDark;
    viewLayout = new QGridLayout;
    viewLayout->setHorizontalSpacing(0);
    AuxChartView *p_chartView;
    AuxChart *p_chart;
    QLineSeries *p_series;
    for (int i=0; i<3; i++)
    {
        p_chart = new AuxChart;
        p_chart->setMargins(QMargins(0,15,0,0));
        p_chart->legend()->setVisible(false);
        p_series = new QLineSeries(p_chart);
        p_chart->addSeries(p_series);
        p_chart->setTheme(goodTheme);

        if (i>0)
        {
            p_series = new QLineSeries(p_chart);
            p_series->setPen(QPen(QBrush(Qt::red),2.0));
            p_chart->addSeries(p_series);
        }
        if (i>1)
        {
            p_series = new QLineSeries(p_chart);
            p_series->setPen(QPen(QBrush(Qt::darkCyan),2.0));
            p_chart->addSeries(p_series);
        }
        p_chart->createDefaultAxes();
        p_chart->axisY()->setReverse(true);
        p_chart->axisX()->setTitleText("Амплитуда");
        p_chart->axisY()->setTitleText("Время, мс");
        p_chart->setAnimationOptions(QChart::NoAnimation);
        p_chartView = new AuxChartView(p_chart);
        p_chartView->setBackgroundBrush(QBrush(Qt::green,Qt::SolidPattern));
        p_chartView->setRenderHint(QPainter::Antialiasing);
        connect(p_chart,SIGNAL(sendNewRange(float,float,QString)),this,SLOT(setRange(float,float,QString)));
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
    themeChanged();
}

void AuxesWidget::setBadTheme(const int &i)
{
    badTheme = static_cast<QChart::ChartTheme>(i);
    themeChanged();
}

void AuxesWidget::setRange(const float &min, const float &max,const QString &chartName)
{
    if (chartName=="Отметка момента.")
    {
        ranges.insert("tbRange",AxisRange(min,max));

        //tBRange = qMakePair(min,max);
        return;
    }
    if (chartName=="Подтвержденная отметка момента.")
    {
        ranges.insert("confirmedTBrange",AxisRange(min,max));
        return;
    }
    if (chartName=="Вертикальное время.")
    {
        ranges.insert("upholeRange",AxisRange(min,max));
        return;
    }
    if (chartName=="АКФ.")
    {
        ranges.insert("akfRange",AxisRange(min,max));
        return;
    }
    if (chartName=="Спектр АКФ.")
    {
        ranges.insert("spectrumRange", AxisRange(min,max));
        return;
    }
}

void AuxesWidget::setRanges(QMap<QString, AxisRange> r)
{
    QMap<QString, AxisRange>::const_iterator i = r.begin();
    while (i!=r.end())
    {
        ranges.insert(i.key(),i.value());
        i++;
    }
}
QMap<QString,AxisRange> AuxesWidget::getRanges()
{
    return ranges;
}


void AuxesWidget::setFrqLvl(const int &i)
{
    frqLvl=i;
}

void AuxesWidget::setTbOffset(const int &i)
{
    tbOffset = i;
}

void AuxesWidget::setTvOffset(const int &i)
{
    tvOffset = i;
}

int AuxesWidget::getFrqLvl()
{
    return frqLvl;
}

void AuxesWidget::themeChanged()
{
    AuxChartView *p_chartView;
    for (int i=0; i<3; i++)
    {
        p_chartView=qobject_cast<AuxChartView*>(viewLayout->itemAt(i)->widget());
        if (p_chartView->backgroundBrush().color()==Qt::red)
        {
            p_chartView->chart()->setTheme(badTheme);
        }
        else
        {
            p_chartView->chart()->setTheme(goodTheme);
            p_chartView->setBackgroundBrush(p_chartView->chart()->backgroundBrush());
        }
     }
}


void AuxesWidget::recieveCoordinates(const QString &coordinates)
{
    coordinatesLabel->setText(coordinates);
}

void AuxesWidget::paintAuxes(const int &ffid, const AuxData &data)
{
    if (!this->isVisible())
    {
        this->setVisible(true);
    }
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
            auxChart = static_cast<AuxChart*>(p_chartView->chart());
            if (data.checkData.at(i))
            {
                p_chartView->chart()->setTheme(goodTheme);

            }
            else
            {
                p_chartView->chart()->setTheme(badTheme);
            }
            p_series =qobject_cast<QLineSeries*>(p_chartView->chart()->series().first());
            p_series->replace(*data.auxData.at(i));
            brect = QPolygonF(*data.auxData.at(i)).boundingRect();
            xR = brect.right()>brect.left() ? brect.right() : brect.left();
            switch (i) {
            case 0:
                auxChart->setTitle(QString("Файл №%1. АКФ.").arg(ffid));
                auxChart->setYRange(ranges.value("akfRange"));
                auxChart->axisX()->setRange(-1.1*xR,1.1*xR);
                //auxChart->axisY()->setRange(brect.top(),brect.bottom());
                break;
            case 1:
                auxChart->axisY()->setReverse(false);
                auxChart->axisY()->setTitleText("Амплитуда, дБ");
                auxChart->axisX()->setTitleText("Частота, Гц");
                auxChart->setTitle(QString("Файл №%1. Спектр АКФ.").arg(ffid));
                auxChart->axisX()->setRange(brect.left(),brect.right());

                auxChart->setYRange(ranges.value("spectrumRange"));
                //auxChart->axisY()->setRange(ceil(brect.top()/25-1)*25,brect.bottom());
                p_series =qobject_cast<QLineSeries*>(auxChart->series().at(1));
                p_series->clear();
                p_series->setPen(QPen(QBrush(Qt::red),2.0));
                p_series->append(0.0,frqLvl);
                p_series->append(xR,frqLvl);
                qDebug()<<p_series->pointsVector().at(0);
                qDebug()<<p_series->pointsVector().at(1);
                break;
            default:
                break;
            }
            auxChart->SetYRangeLimits(brect.top(),brect.bottom());
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
            p_series->replace(*data.auxData.at(i));
            brect = QPolygonF(*data.auxData.at(i)).boundingRect();

            xR = brect.right()>abs(brect.left()) ? brect.right() :abs(brect.left());
            p_chartView->chart()->axisX()->setRange(-1.1*xR,1.1*xR);
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
            switch (i) {
            case 0:
                auxChart->setTitle(QString("Файл №%1. Отметка момента.").arg(ffid));
                auxChart->setYRange(ranges.value("tbRange"));
                break;
            case 1:
                auxChart->setTitle(QString("Файл №%1. Подтвержденная отметка момента.").arg(ffid));
                auxChart->axisY()->setTitleText("Время, мс");
                auxChart->axisX()->setTitleText("Амплитуда");
                auxChart->setYRange(ranges.value("confirmedTBrange"));
                auxChart->axisY()->setReverse(true);
                p_series =qobject_cast<QLineSeries*>(p_chartView->chart()->series().at(1));
                p_series->clear();
                p_series->append(-0.5*xR,data.headerUphole+tbOffset);
                p_series->append( 0.5*xR,data.headerUphole+tbOffset);
                p_series->setPen(QPen(QBrush(Qt::red),2.0));
                break;
            case 2:
                auxChart->setTitle(QString("Файл №%1. Вертикальное время.").arg(ffid));
                auxChart->setYRange(ranges.value("upholeRange"));
                p_series =qobject_cast<QLineSeries*>(p_chartView->chart()->series().at(1));
                p_series->clear();
                p_series->append(-0.5*xR,data.headerUphole+tvOffset);
                p_series->append( 0.5*xR,data.headerUphole+tvOffset);
                p_series->setPen(QPen(QBrush(Qt::red),2.0));
                p_series =qobject_cast<QLineSeries*>(p_chartView->chart()->series().at(2));
                p_series->clear();
                p_series->append(-0.5*xR,data.countedUphole+tvOffset);
                p_series->append( 0.5*xR,data.countedUphole+tvOffset);
                p_series->setPen(QPen(QBrush(Qt::darkCyan),2.0));
                break;
            default:
                break;
            }
            auxChart->SetYRangeLimits(brect.top(),brect.bottom());
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

    qreal time;
    if (yAxis->isReverse())
    {
        if (mousePressed)
        {
            qreal yNew = event->pos().y();
            yNew<yOld ? auxChart->yScrollUp() : auxChart->yScrollDown();
            yOld=yNew;
        }
        time = yAxis->max()+yAxis->min()-this->chart()->mapToValue(event->pos()).y();
        emit sendCoordinates(auxChart->getCoordinatesForTime(time));
    }
    else
    {
        if (mousePressed)
        {
            qreal yNew = event->pos().y();
            yNew<yOld ? auxChart->yScrollDown() : auxChart->yScrollUp();
            yOld=yNew;
        }
        time = this->chart()->mapToValue(event->pos()).x();
        emit sendCoordinates(auxChart->getLevelForFreq(time));
    }
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
        if (auxChart->axisY()->isReverse()) {
            auxChart->yScrollDown();
        }
        else
        {
            auxChart->yScrollUp();
        }
        break;
    case Qt::Key_Down:
        if (auxChart->axisY()->isReverse()){
            auxChart->yScrollUp();
        }
        else {
            auxChart->yScrollDown();
        }
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

    yRangeLimits= qMakePair(0.0,1000.0);
}

AuxChart::~AuxChart()
{

}
void AuxChart::setYRange(const AxisRange &range)
{
    QValueAxis *yAxis = qobject_cast<QValueAxis*>(this->axisY());
    yAxis->setRange(range.minValue,range.maxValue);
    scrollFactor = (yAxis->max()-yAxis->min())/20.0;
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
    scrollFactor = (yAxis->max()-yAxis->min())/20.0;
    emit sendNewRange(yAxis->min(),yAxis->max(),this->title().mid(this->title().indexOf('.')+2));
}

void AuxChart::yZoomOut()
{
    QValueAxis *yAxis = qobject_cast<QValueAxis*>(this->axisY());
    qreal center = (yAxis->max() + yAxis->min())/2.0;
    qreal newLength = (yAxis->max()-yAxis->min());
    qreal newMin = center - newLength > yRangeLimits.first ? center-newLength : yRangeLimits.first;
    qreal newMax = center + newLength < yRangeLimits.second ? center + newLength : yRangeLimits.second;
    yAxis->setRange(newMin,newMax);
    scrollFactor = (yAxis->max()-yAxis->min())/20.0;
    emit sendNewRange(yAxis->min(),yAxis->max(),this->title().mid(this->title().indexOf('.')+2));
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
    emit sendNewRange(yAxis->min(),yAxis->max(),this->title().mid(this->title().indexOf('.')+2));
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
    emit sendNewRange(yAxis->min(),yAxis->max(),this->title().mid(this->title().indexOf('.')+2));
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
    emit sendNewRange(yAxis->min(),yAxis->max(),this->title().mid(this->title().indexOf('.')+2));
    return;
}
QString AuxChart::getCoordinatesForTime(const float &t) const
{

    QXYSeries *series = qobject_cast<QXYSeries*>(this->series().first());
    QVector<QPointF> seriesData = series->pointsVector();
    //float sInt = 1.0;
    QString chartName = this->title().mid(this->title().indexOf('.')+1);
    if (!seriesData.isEmpty() && t > yRangeLimits.first && t < yRangeLimits.second)
    {
        int i=0;
        float sInt = (seriesData.at(1).y()-seriesData.at(0).y())/2;
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

QString AuxChart::getLevelForFreq(const float &f) const
{
    QXYSeries *series = qobject_cast<QXYSeries*>(this->series().first());
    QVector<QPointF> seriesData = series->pointsVector();
    QString chartName = this->title().mid(this->title().indexOf('.')+1);
    if (!seriesData.isEmpty() && f > seriesData.first().x()&& f < seriesData.last().x())
    {
        int i=0;
        float sInt = (seriesData.at(1).x()-seriesData.at(0).x())/2;
        QPointF point = seriesData.at(i);
        while (fabs(point.x()-f) > sInt)
        {
            i++;
            point = seriesData.at(i);
        }
        return  QString("%1 A=%2 F=%3").arg(chartName).arg(point.y()).arg(point.x());
    }
    return chartName;
}
