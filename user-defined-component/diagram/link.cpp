#include "link.h"
#include "node.h"
#include<QPen>

Link::Link(Node* fromNode, Node* toNode)
{
    myFromNode = fromNode;
    myToNode = toNode;
    myFromNode->addLink(this);
    myToNode->addLink(this);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setZValue(-1);  //每个item都有x,y坐标和一个 z值（表示前后位置),因为是要画线，z值设-1就会让线条位于节点下方

    setColor(Qt::darkRed);
    trackNodes(); // 设置线的端点
}

Link::~Link(){
    myFromNode->removeLink(this);
    myToNode->removeLink(this);
}

void Link::setColor(const QColor &color){
    setPen(QPen(color,1.0));
}

void Link::trackNodes()
{
    setLine(QLineF(myFromNode->pos(),myToNode->pos()));
    //QGraphicItem::pos函数返回项相对于场景的位置，或者相对于父项的位置
}

QColor Link::color() const{
    return pen().color();
}


