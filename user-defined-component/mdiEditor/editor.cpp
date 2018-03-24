#include <QtWidgets>
#include "editor.h"

Editor::Editor(QWidget *parent)
    :QTextEdit(parent)
{
    action = new QAction(this);
    action->setCheckable(true);

    connect(action,SIGNAL(triggered()),this,SLOT(show()));
    connect(action,SIGNAL(triggered()),this,SLOT(setFocus()));

    isUntitled = true;
    connect(document(),SIGNAL(contentsChanged()),this,SLOT(documentWasModified()));

    setWindowIcon(QPixmap(":/images/document.jpg"));
    setWindowTitle("[*]");
    //如果主调用代码在栈内存中分配，有可能引起程序关闭时崩溃
    setAttribute(Qt::WA_DeleteOnClose);
}

void Editor::newFile()
{
    static int documentNumber = 1;
    curFile = tr("document%1.txt").arg(documentNumber);
    setWindowTitle(curFile+"[*]");
    action->setText(curFile);
    isUntitled = true;
    ++documentNumber;
}

Editor *Editor::open(QWidget *parent)
{
    QString fileName = QFileDialog::getOpenFileName(parent,tr("Open"),".");
    return fileName.isEmpty()?0:openFile(fileName,parent);
}

Editor *Editor::openFile(const QString &fileName,QWidget *parent)
{
    Editor *editor = new Editor(parent);
    if(editor->readFile(fileName))
    {
        editor->setCurrentFile(fileName);
        return editor;
    }else{
        delete editor;
        return 0;
    }
}

bool Editor::save()
{
    if(isUntitled){
        return saveAs();
    }else{
        return saveFile(curFile);
    }
}

bool Editor::saveAs()
{
    //如果存在重复的文件，会询问用户，可以通过传入QFileDialog::DontConfirmOverwrite来改变这个行为
    QString fileName = QFileDialog::getSaveFileName(this,tr("Save File"),".",tr("save files (*.txt)"));
    if(fileName.isEmpty()){
        return false;
    }
    return saveFile(fileName);
}

void Editor::closeEvent(QCloseEvent *event)
{
    if(okToContinue()){
        event->accept();
    }else{
        event->ignore();
    }
}

bool Editor::okToContinue()
{
    if(isWindowModified()){
        int r = QMessageBox::warning(this,
                                     tr("Editor"),
                                     tr("the document has been modified.\n"
                                        "Do you want to save your changes?"),
                                     QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
        if(r==QMessageBox::Yes){
            return save();
        }else if(r==QMessageBox::Cancel){
            return false;
        }
    }
    return true;
}

void Editor::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    isUntitled = false;
    action->setText(strippedName(curFile));
    document()->setModified(false);
    setWindowTitle(strippedName(curFile)+"[*]");
    setWindowModified(false);
}


QSize Editor::sizeHint() const
{
    return QSize(72*fontMetrics().width('x'),25*fontMetrics().lineSpacing());
}


void Editor::documentWasModified()
{
    setWindowModified(true);
}

QString Editor::strippedName(const QString &fullFileName)
{
    //移除文件路径
    return QFileInfo(fullFileName).fileName();
}

bool Editor::saveFile(const QString &fileName)
{
    if(writeFile(fileName)){
        setCurrentFile(fileName);
        return true;
    }
    return false;
}

bool Editor::writeFile(const QString &fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly)){
        QMessageBox::warning(this,tr("Editor"),tr("cannot write file %1:\n%2.").arg(file.fileName()).arg(file.errorString()));
        return false;
    }
    //QTextStream是文本模式，QDataStream是二进制模式
    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << toPlainText();
    QApplication::restoreOverrideCursor();
    return true;
}

bool Editor::readFile(const QString &fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this,tr("Editor"),tr("cannot read file %1:\n%2.").arg(file.fileName()).arg(file.errorString()));
        return false;
    }
    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    setPlainText(in.readAll());
    QApplication::restoreOverrideCursor();
    return true;
}

