#include "node.h"
#include "link.h"
#include<QFontMetricsF>
#include<QStyleOptionGraphicsItem>
#include<QPen>
#include<QPainter>
#include<QInputDialog>
#include<QGraphicsSceneMouseEvent>
Node::Node(){
    myTextColor = Qt::darkGreen;
    myOutlineColor = Qt::darkBlue;
    myBackgroundColor = Qt::white;
    setFlags(ItemIsMovable | ItemIsSelectable);
}

Node::~Node(){
    foreach (Link* link, myLinks){
        delete link;
    }
}

void Node::setText(const QString& text){
    prepareGeometryChange();
    myText = text;
    update();
}

void Node::setTextColor(const QColor& color){
    myTextColor = color;
    update();
}
void Node::setOutlineColor(const QColor& color){
    myOutlineColor = color;
    update();
}
void Node::setBackgroundColor(const QColor& color){
    myBackgroundColor = color;
    update();
}

void Node::addLink(Link* link){
    myLinks.insert(link);
}

void Node::removeLink(Link* link){
    myLinks.remove(link);
}

QRectF Node::outlineRect() const{
    const int padding = 8;
    QFontMetricsF metrics(qApp->font());
    QRectF rect = metrics.boundingRect(myText);
    rect.adjust(-padding,-padding,+padding,+padding);
    rect.translate(-rect.center());
    return rect;
}

QRectF Node::boundingRect() const{
    const int margin = 1;
    return outlineRect().adjusted(-margin,-margin,+margin,+margin);
}

QPainterPath Node::shape() const
{
    QRectF rect = outlineRect();
    QPainterPath path;
    path.addRoundedRect(rect,roundness(rect.width()),roundness(rect.height()));
    return path;
}

int Node::roundness(double size) const{
    const int diameter = 12;
    return 100 * diameter / int(size);
}

void Node::paint(QPainter *painter,const QStyleOptionGraphicsItem *option,QWidget* /*widget*/){
    QPen pen(myOutlineColor);
    if(option->state&QStyle::State_Selected){
        pen.setStyle(Qt::DotLine);
        pen.setWidth(2);
    }
    painter->setPen(pen);
    painter->setBrush(myBackgroundColor);
    QRectF rect = outlineRect();
    painter->drawRoundedRect(rect,roundness(rect.width()),roundness(rect.height()));
    painter->setPen(myTextColor);
    painter->drawText(rect,Qt::AlignCenter,myText);
}

QVariant Node::itemchange(GraphicsItemChange change, const QVariant& value){
    if(change == ItemPositionHasChanged){
        foreach(Link* link,myLinks){
            link->trackNodes();
        }
    }
    return QGraphicsItem::itemChange(change,value);
}

void Node::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event){
    QString text = QInputDialog::getText(event->widget(),tr("Edit Text"),tr("Enter new text:"),QLineEdit::Normal,myText);
    if(!text.isEmpty()){
        setText(text);
    }
}

