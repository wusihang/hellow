#ifndef FINDDIALOG_H
#define FINDDIALOG_H
#include<QDialog>
//前置声明，告知类的存在，而不用提供类定义的所有细节
class QCheckBox;
class QLabel;
class QLineEdit;
class QPushButton;

class FindDialog:public QDialog
{
    Q_OBJECT   //这个宏对于所有定义了信号和槽的类都是必须存在的，相当于接口继承
public:
    FindDialog(QWidget *parent=0);

    //signals关键字实际上是一个宏定义，C++预处理器会在预处理时将它转化为标准C++代码
signals:
    //Qt::CaseSensitivity 是一个枚举，有Qt::CaseSensitive 和 Qt:CaseInsensitive两个值
    void findNext(const QString &str,Qt::CaseSensitivity cs);
    void findPrevious(const QString &str,Qt::CaseSensitivity cs);

    //关键字slots和signals一样，也是一个宏定义
private slots:
    void findClicked();
    void enableFindButton(const QString &text);
    //这些私有变量都使用的前置声明，这是可行的，因为他们都是指针，而且没有必要再头文件就去访问他们，编译程序无须这些类的完整定义。
    //没有包含类相关头文件，而是使用前置声明会让类的编译更快一些
private:
    QLabel *label;
    QLineEdit *lineEdit;
    QCheckBox *caseCheckBox;
    QCheckBox *backwardCheckBox;
    QPushButton *findButton;
    QPushButton *closeButton;
};

#endif // FINDDIALOG_H
