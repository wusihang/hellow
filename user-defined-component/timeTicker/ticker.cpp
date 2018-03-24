#include <QtWidgets>
#include "ticker.h"

Ticker::Ticker(QWidget* parent)
    :QWidget(parent){
    offset=0;
    myTimerId=0;
}

void Ticker::setText(const QString& newText){
    myText = newText;
    update();  //强制刷新控件，重绘
    updateGeometry(); //通知窗口部件负责的任意布局管理器，提示窗口部件大小变更
}

QSize Ticker::sizeHint() const
{
    //根据字符来调整窗口部件大小
    return fontMetrics().size(0,text());
}

void Ticker::paintEvent(QPaintEvent* /*event*/){
    QPainter painter(this);
    int textWidth = fontMetrics().width(text());
    if(textWidth<1){
        return;
    }
    int x= -offset;
    while(x<textWidth){
        painter.drawText(x,0,textWidth,height(),Qt::AlignLeft|Qt::AlignVCenter,text());
        x += textWidth;
    }
}

void Ticker::showEvent(QShowEvent* /*event*/){
    myTimerId = startTimer(30);
}

void Ticker::timerEvent(QTimerEvent* event){
    if(event->timerId() == myTimerId){
        ++offset;
        if(offset >= fontMetrics().width(text())){
            offset = 0;
        }
        scroll(-1,0);
    }else
    {
        QWidget::timerEvent(event);
    }
}

void Ticker::hideEvent(QHideEvent* /*event*/){
    killTimer(myTimerId);
    myTimerId=0;
}
