#ifndef NODE_H
#define NODE_H
#include<QGraphicsItem>
#include<QSet>
#include<QApplication>
class Link;
class QStyleOptionGraphicsItem;
class QGraphicsSceneMouseEvent;
class DiagramWindow;
class Node : public QGraphicsItem{
    Q_DECLARE_TR_FUNCTIONS(Node)
public:
    Node();
    ~Node();
    void setText(const QString& text);
    QString text() const{
        return myText;
    }
    void setTextColor(const QColor& color);
    QColor textColor() const{
        return myTextColor;
    }
    void setOutlineColor(const QColor& color);
    QColor outlineColor() const{
        return myOutlineColor;
    }
    void setBackgroundColor(const QColor& color);
    QColor backgroundColor() const{
        return myBackgroundColor;
    }

    void addLink(Link* link);
    void removeLink(Link* link);
    //创建QGraphicsItem子类时，要想自己绘图，一般是重新实现以下几个方法
    QRectF boundingRect() const;
    QPainterPath shape() const; //视图体系用外接矩形来考虑是否重绘，shape用来决定一个点是否在项内，是否两个项是重合的。
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    //为了更加方便，提供鼠标双击修改项的属性
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
    //当节点更新了，我们必须确保其与link做相应的更新，不使用mouseMove是因为，如果程序自动更新，mouseMove不会触发
    QVariant itemchange(GraphicsItemChange change, const QVariant& value);
private:
    QRectF outlineRect() const;
    int roundness(double size) const;
    QSet<Link *> myLinks;
    QString myText;
    QColor myTextColor;
    QColor myOutlineColor;
    QColor myBackgroundColor;
};

#endif // NODE_H
