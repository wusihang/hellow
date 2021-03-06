#include <user-defined-component/ovenTimer/oventimer.h>
#include <QtWidgets>
#include <cmath>
const double DegreesPerMinute = 7.0;
const double DegreesPerSecond = DegreesPerMinute / 60;
const int MaxMinutes = 45;
const int MaxSeconds = MaxMinutes * 60;
const int UpdateInterval = 5;

OvenTimer::OvenTimer(QWidget *parent)
    :QWidget(parent){
    finishTime = QDateTime::currentDateTime();
    updateTimer = new QTimer(this);
    //定时器的timeout信号触发后，ui该组件就刷新一次
    connect(updateTimer,SIGNAL(timeout()),this,SLOT(update()));
    finishTimer = new QTimer(this);
    finishTimer->setSingleShot(true);//只需要执行一次
    connect(finishTimer,SIGNAL(timeout()),this,SIGNAL(timeout()));
    connect(finishTimer,SIGNAL(timeout()),updateTimer,SLOT(stop()));//当定时器停止运行时停止窗口更新

    QFont font;
    font.setPointSize(8);
    setFont(font);
}

void OvenTimer::setDuration(int secs){
    secs = qBound(0,secs,MaxSeconds);   //qBound函数，如果secs<0，设置为0，如果secs>MaxSeconds，设置为MaxSeconds
    finishTime = QDateTime::currentDateTime().addSecs(secs);  //得到结束时间
    if(secs>0){
        updateTimer->start(UpdateInterval*1000); //更新周期设置为5s，每5s触发一次timeout信号
        finishTimer->start(secs*1000);
    }else{
        updateTimer->stop();
        finishTimer->stop();
    }
    update(); //用新的时间间隔重绘窗口
}

int OvenTimer::duration() const
{
    int secs = QDateTime::currentDateTime().secsTo(finishTime);
    if(secs<0){
        secs = 0;
    }
    return secs;
}

void OvenTimer::mousePressEvent(QMouseEvent *event){
    QPointF point = event->pos() - rect().center(); //获得鼠标点击点和组件中点之间的向量
    double theta = std::atan2(-point.x(),-point.y())*180.0 / M_PI; //计算夹角
    setDuration(duration() + int(theta / DegreesPerSecond));  //设置定时器，并更新UI
}

void OvenTimer::paintEvent(QPaintEvent* /*event*/){
    //直接绘制
//    QPainter painter(this);
//    painter.setRenderHint(QPainter::Antialiasing,true); //反走样
//    int side = qMin(width(),height());
//    painter.setViewport((width()-side)/2,(height()-side)/2,side,side);
//    painter.setWindow(-50,-50,100,100);
//    draw(&painter);
    //先绘制image，再贴图
    QImage image(size(),QImage::Format_ARGB32_Premultiplied);
    QPainter imagePainter(&image);
    int side = qMin(width(),height());
    imagePainter.setViewport((width()-side)/2,(height()-side)/2,side,side);  //视口为最大正方形
    imagePainter.setWindow(-50,-50,100,100);  //窗口为-50,-50  ~~   +50,+50
    imagePainter.initFrom(this);
    imagePainter.setRenderHint(QPainter::Antialiasing,true);
    imagePainter.eraseRect(rect());
    draw(&imagePainter);
    imagePainter.end();

    //最后将image贴到widget上
    QPainter widgetPainter(this);
    widgetPainter.drawImage(0,0,image);
}

void OvenTimer::draw(QPainter* painter){
    static const int triangle[3][2] = {{-2,-49},{+2,-49},{0,-47}};
    QPen thickPen(palette().foreground(),1.5);
    QPen thinPen(palette().foreground(),0.5);
    QColor niceBlue(150,150,200);
    painter->setPen(thinPen);
    painter->setBrush(palette().foreground());
    painter->drawPolygon(QPolygon(3,&triangle[0][0]));

    QConicalGradient coneGradient(0,0,-90.0);
    coneGradient.setColorAt(0.0,Qt::darkGray);
    coneGradient.setColorAt(0.2,niceBlue);
    coneGradient.setColorAt(0.5,Qt::white);
    coneGradient.setColorAt(1.0,Qt::darkGray);
    painter->setBrush(coneGradient);
    painter->drawEllipse(-46,-46,92,92);

    QRadialGradient haloGradient(0,0,20,0,0);
    coneGradient.setColorAt(0.0,Qt::lightGray);
    coneGradient.setColorAt(0.8,Qt::darkGray);
    coneGradient.setColorAt(0.9,Qt::white);
    coneGradient.setColorAt(1.0,Qt::black);
    painter->setBrush(haloGradient);
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(-20,-20,40,40);

    QLinearGradient knobGradient(-7,-25,7,-25);
    knobGradient.setColorAt(0.0,Qt::black);
    knobGradient.setColorAt(0.2,niceBlue);
    knobGradient.setColorAt(0.3,Qt::lightGray);
    knobGradient.setColorAt(0.8,Qt::white);
    knobGradient.setColorAt(1.0,Qt::black);
    painter->rotate(duration()*DegreesPerSecond);
    painter->setBrush(knobGradient);
    painter->setPen(thinPen);
    painter->drawRoundRect(-7,-25,14,50,99,49);

    for(int i=0;i<=MaxMinutes;i++){
        painter->save();
        painter->rotate(-i*DegreesPerMinute);
        if(i%5==0){
            painter->setPen(thickPen);
            painter->drawLine(0,-41,0,-44);
            painter->drawText(-15,-41,30,30,Qt::AlignHCenter|Qt::AlignTop,QString::number(i));
        }else{
            painter->setPen(thinPen);
            painter->drawLine(0,-42,0,-44);
        }
        painter->restore();
    }
}

