#ifndef CELL_H
#define CELL_H
#include<QTableWidgetItem>
//这个类没有实现信号和槽，所以没有使用Q_OBJECT宏，QTableWidget也只是一个普通C++类，未实现信号
//要实现信号的功能，可以在使用这个Item的窗口部件中去实现，或者多继承QObject
class Cell:public QTableWidgetItem{
public:
    Cell();
    QTableWidgetItem *clone() const;
    void setData(int role,const QVariant &value);
    QVariant data(int role) const;
    void setFormula(const QString &formula);
    QString formula() const;
    void setDirty();
private :
    QVariant value() const;
    QVariant evalExpression(const QString &str,int &pos) const;
    QVariant evalTerm(const QString &str,int &pos) const;
    QVariant evalFactor(const QString &str,int &pos) const;
    //声明为mutable后，就可以在const函数中修改，否则会报错
    mutable QVariant cachedValue;
    mutable bool cacheIsDirty;
};

#endif // CELL_H
