#include <QtWidgets>
#include "iconeditor.h"

IconEditor::IconEditor(QWidget *parent)
    :QWidget(parent)
{
    setAttribute(Qt::WA_StaticContents);
    setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    curColor = Qt::black;
    zoom = 8;
    image = QImage(16,16,QImage::Format_ARGB32);
    image.fill(qRgba(0,0,0,0));
}


QSize IconEditor::sizeHint() const
{
    QSize size = zoom*image.size();
    //如果缩放因子超过3，那么需要在每个方向上额外添加一个网格线
    if(zoom>=3){
        size+=QSize(1,1);
    }
    return size;
}

void IconEditor::setPenColor(const QColor &newColor){
    curColor = newColor;
}

void IconEditor::setIconImage(const QImage &newImage){
    if(newImage!=image){
        //如果传入的图片不是当前编辑的图片，那么将图片转化为一个带alpha的32位图像
        image=newImage.convertToFormat(QImage::Format_ARGB32);
        //设置完image后，使用新图像强制重绘窗口
        update();
        //告诉包含这个窗口的任意布局，这个窗口部件的大小已经发生改变。
        updateGeometry();
    }
}

void IconEditor::setZoomFactor(int newZoom)
{
    if(newZoom<1){
        newZoom=1;
    }
    if(newZoom!=zoom){
        zoom=newZoom;
        update();
        updateGeometry();
    }
}

void IconEditor::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    //如果缩放超过3，那么需要展示网格线
    if(zoom>=3){
        painter.setPen(palette().foreground().color());
        for(int i=0;i<=image.width();i++){
            painter.drawLine(zoom*i,0,zoom*i,zoom*image.height());
        }
        for(int j=0;j<=image.height();j++){
            painter.drawLine(0,zoom*j,zoom*image.width(),zoom*j);
        }
    }
    for(int i=0;i<image.width();i++){
        for(int j=0;j<image.height();j++){
            QRect rect = pixelRect(i,j);
            if(!event->region().intersected(rect).isEmpty()){
                QColor color = QColor::fromRgba(image.pixel(i,j));
                if(color.alpha()<255){
                    painter.fillRect(rect,Qt::white);
                }
                painter.fillRect(rect,color);
            }
        }
    }
}

QRect IconEditor::pixelRect(int i,int j) const
{
    if(zoom>=3){
        return QRect(zoom*i+1,zoom*j+1,zoom-1,zoom-1);
    }else{
        return QRect(zoom*i,zoom*j,zoom,zoom);
    }
}

void IconEditor::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton){
        setImagePixel(event->pos(),true);
    }else if(event->button()==Qt::RightButton){
        setImagePixel(event->pos(),false);
    }
}

void IconEditor::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons()&Qt::LeftButton){
        setImagePixel(event->pos(),true);
    }else if(event->buttons()&Qt::RightButton){
        setImagePixel(event->pos(),false);
    }
}

void IconEditor::setImagePixel(const QPoint &pos,bool opaque)
{
    int i = pos.x()/zoom;
    int j = pos.y()/zoom;
    if(image.rect().contains(i,j)){
        if(opaque){
            image.setPixel(i,j,penColor().rgba());
        }else{
             image.setPixel(i,j,qRgba(0,0,0,0));
        }
        update(pixelRect(i,j));
    }
}


