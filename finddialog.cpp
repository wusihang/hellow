//在finddialog.h本来也是可以通过包含QtGui即可，而不用使用前置声明，
//但是在一个头文件中包含一个大的头文件不是一个好的编程风格
#include<QtWidgets>
#include "finddialog.h"

FindDialog::FindDialog(QWidget *parent)
    :QDialog(parent)  //这里把parent传递给了基类的构造函数
{
    //tr是把它们翻译成其他语言的标记， &表示快捷键，Alt+W即可获得焦点
    label = new QLabel(tr("Find &what:"));
    lineEdit = new QLineEdit;
    label->setBuddy(lineEdit); // setBuddy相当于将两个控件绑在一起，当做一个控件
    caseCheckBox = new QCheckBox(tr("Match &case"));
    backwardCheckBox = new  QCheckBox(tr("Search &backward"));
    findButton = new QPushButton(tr("&Find"));
    findButton->setDefault(true);
    findButton->setEnabled(false);
    closeButton = new QPushButton(tr("Close"));
    //QObject是FindDialog的父对象之一，所以可以省略QObject
    QObject::connect(lineEdit,SIGNAL(textChanged(const QString &)),this,SLOT(enableFindButton(const QString&)));
    QObject::connect(findButton,SIGNAL(clicked()),this,SLOT(findClicked()));
    QObject::connect(closeButton,SIGNAL(clicked()),this,SLOT(close()));

    //以下为布局代码
     QHBoxLayout *topLeftLayout = new QHBoxLayout;
     topLeftLayout->addWidget(label);
     topLeftLayout->addWidget(lineEdit);

     QVBoxLayout *leftLayout = new QVBoxLayout;
     leftLayout->addLayout(topLeftLayout);
     leftLayout->addWidget(caseCheckBox);
     leftLayout->addWidget(backwardCheckBox);

     QVBoxLayout *rightLayout = new QVBoxLayout;
     rightLayout->addWidget(findButton);
     rightLayout->addWidget(closeButton);
     rightLayout->addStretch(); // 添加分隔符

     QHBoxLayout *mainLayout = new QHBoxLayout;
     mainLayout->addLayout(leftLayout);
     mainLayout->addLayout(rightLayout);
     setLayout(mainLayout);
     setWindowTitle(tr("Find"));
     //QWidget::sizeHint可以返回一个窗口部件的理想大小
     setFixedHeight(sizeHint().height());
}
//构造函数中，窗口部件和布局都是使用new，要写一个能调用delete的析构函数，但这不是必须的，因为Qt会在删除父对象时自动删除所属所有子对象

void FindDialog::findClicked()
{
    QString text = lineEdit->text();
    Qt::CaseSensitivity cs = caseCheckBox->isChecked()?Qt::CaseSensitive:Qt::CaseInsensitive;
    if(backwardCheckBox->isChecked()){
        emit findPrevious(text,cs);
    }else{
        emit findNext(text,cs);
    }
}

void FindDialog::enableFindButton(const QString &text){
    findButton->setEnabled(!text.isEmpty());
}
