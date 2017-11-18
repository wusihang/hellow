#include<QtWidgets>
#include "mainwindow.h"
#include "gotocelldialog.h"
#include "sortdialog.h"
#include "finddialog.h"
#include "spreadsheet.h"

MainWindow::MainWindow()
{
    spreadsheet = new Spreadsheet;
    setCentralWidget(spreadsheet);
    createActionns();
    createMenus();
    createContextMenu();
    createToolBars();
    createStatusBar();

    readSettings();
    findDialog=0;
    setWindowIcon(QIcon(":/images/icon.jpg"));
    //默认最大化显示
    showMaximized();
    setCurrentFile("");
    //设置窗口关闭时删除
    setAttribute(Qt::WA_DeleteOnClose);
}

void MainWindow::createActionns()
{
    //动作new，一个加速键，一个图标，一个快捷键，一个状态提升
    newAction = new QAction(tr("&New"),this);
    newAction->setIcon(QIcon(":/images/new.jpg"));
    //QKeySequence提供的枚举值快捷键Crtl + N
    newAction->setShortcut(QKeySequence::New);
    newAction->setStatusTip(tr("Create a new spreadsheet file"));
    //这个Action的triggered信号连接到主窗口的私有槽
    connect(newAction,SIGNAL(triggered()),this,SLOT(newFile()));

    //open
    openAction = new QAction(tr("&Open"),this);
    openAction->setIcon(QIcon(":/images/open.jpg"));
    openAction->setShortcut(QKeySequence::Open);
    openAction->setStatusTip(tr("Open an exist spreadsheet file"));
    connect(openAction,SIGNAL(triggered()),this,SLOT(open()));
    //save
    saveAction = new QAction(tr("&Save"),this);
    saveAction->setIcon(QIcon(":/images/save.jpg"));
    saveAction->setShortcut(QKeySequence::Save);
    saveAction->setStatusTip(tr("save a spreadsheet file"));
    connect(saveAction,SIGNAL(triggered()),this,SLOT(save()));

    //saveAs
    saveAsAction = new QAction(tr("&SaveAs"),this);
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    saveAsAction->setStatusTip(tr("saveAs a new spreadsheet file"));
    connect(saveAsAction,SIGNAL(triggered()),this,SLOT(saveAs()));

//     recent open files
    for(int i=0;i<MAX_RECENT_FILES;i++){
        recentFileActions[i] = new QAction(this);
        recentFileActions[i]->setVisible(false);
        connect(recentFileActions[i],SIGNAL(triggered()),this,SLOT(openRecentFile()));
    }

    closeAction = new QAction(tr("&Close"),this);
    closeAction->setShortcut(QKeySequence::Close);
    closeAction->setStatusTip("Close this window");
    connect(closeAction,SIGNAL(triggered()),this,SLOT(close()));

    //退出
    exitAction = new QAction(tr("E&xit"),this);
    //由于没有标准的退出快捷键，故而明确指定键
    exitAction->setShortcut(QKeySequence::Quit);
    exitAction->setStatusTip("Exit the application");
    connect(exitAction,SIGNAL(triggered()),qApp,SLOT(closeAllWindows()));

    //剪切
    cutAction = new QAction(tr("&Cut"),this);
    cutAction->setIcon(QIcon(":/images/cut.jpg"));
    cutAction->setShortcut(QKeySequence::Cut);
    cutAction->setStatusTip(tr("cut sheet"));
    connect(cutAction,SIGNAL(triggered()),spreadsheet,SLOT(cut()));

    //粘贴
    pasteAction = new QAction(tr("&Paste"),this);
    pasteAction->setIcon(QIcon(":/images/paste.jpg"));
    pasteAction->setShortcut(QKeySequence::Paste);
    pasteAction->setStatusTip(tr("paste sheet"));
    connect(pasteAction,SIGNAL(triggered()),spreadsheet,SLOT(paste()));

    //复制
    copyAction = new QAction(tr("&Copy"),this);
    copyAction->setIcon(QIcon(":/images/copy.jpg"));
    copyAction->setShortcut(QKeySequence::Copy);
    copyAction->setStatusTip(tr("copy sheet"));
    connect(copyAction,SIGNAL(triggered()),spreadsheet,SLOT(copy()));

    //删除
    deleteAction = new QAction(tr("&Delete"),this);
    deleteAction->setShortcut(QKeySequence::Delete);
    deleteAction->setStatusTip(tr("delete sheet"));
    connect(deleteAction,SIGNAL(triggered()),spreadsheet,SLOT(del()));

    //选择行
    selectRowAction = new QAction(tr("Select&Row"),this);
    selectRowAction->setStatusTip(tr("Select a row of the spreadsheet"));

    //选择列
    selectColumnAction = new QAction(tr("Select&Column"),this);
    selectColumnAction->setStatusTip(tr("Select a column of the spreadsheet"));

    //全选事件
    selectAllAction = new QAction(tr("&All"),this);
    selectAllAction->setShortcut(QKeySequence::SelectAll);
    selectAllAction->setStatusTip(tr("Select all the cells in the spreadsheet"));
    //槽selectAll是由QTableWidget的父类之一QAbstractItemView实现，故而不需要亲自实现
    connect(selectAllAction,SIGNAL(triggered()),spreadsheet,SLOT(selectAll()));

    findAction = new QAction(tr("&Find"),this);
    findAction->setIcon(QIcon(":/images/find.jpg"));
    findAction->setShortcut(QKeySequence::Find);
    findAction->setStatusTip(tr("find a keyword"));
    connect(findAction,SIGNAL(triggered()),this,SLOT(find()));

    gotoCellAction = new QAction(tr("&GoToCell"),this);
    gotoCellAction->setIcon(QIcon(":/images/goto.jpg"));
    gotoCellAction->setShortcut(tr("Crtl+G"));
    gotoCellAction->setStatusTip(tr("go to cell"));
    connect(gotoCellAction,SIGNAL(triggered()),this,SLOT(goToCell()));

    recalculateAction = new QAction(tr("&Recalculate"),this);
    recalculateAction->setStatusTip(tr("recalculating"));
    connect(recalculateAction,SIGNAL(triggered()),spreadsheet,SLOT(recalculate()));

    sortAction = new QAction(tr("&Sort"),this);
    sortAction->setStatusTip(tr("sort"));
    connect(sortAction,SIGNAL(triggered()),this,SLOT(sort()));

    //show grid 菜单
    showGridAction = new QAction(tr("&Show Grid"),this);
    showGridAction->setCheckable(true); //要使用toggled信号，这个属性必须设置为true
    showGridAction->setChecked(spreadsheet->showGrid());//spreadsheet->showGrid()
    showGridAction->setStatusTip(tr("Show or hide the spreadsheet's grid"));
    //setShowGrid槽是继承自QTableWidget
    connect(showGridAction,SIGNAL(toggled(bool)),spreadsheet,SLOT(setShowGrid(bool)));

    autoRecalculateAction = new QAction(tr("&Auto-Recalculate"),this);
    autoRecalculateAction->setCheckable(true); //要使用toggled信号，这个属性必须设置为true
    autoRecalculateAction->setChecked(true);
    autoRecalculateAction->setStatusTip(tr("auto recalculate or not"));
    connect(autoRecalculateAction,SIGNAL(toggled(bool)),spreadsheet,SLOT(setAutoRecalculate(bool)));

    aboutAction = new QAction(tr("&About"),this);
    aboutAction->setStatusTip(tr("show infomation about this programs"));
    connect(aboutAction,SIGNAL(triggered()),this,SLOT(about()));

    aboutQtAction = new QAction(tr("About &Qt"),this);
    aboutQtAction->setStatusTip(tr("Show the Qt library's About box"));
//    通过访问qApp全局变量，我们可以使用QApplication的aboutQt()槽
    connect(aboutQtAction,SIGNAL(triggered()),qApp,SLOT(aboutQt()));
}

void MainWindow::createMenus()
{
    //文件菜单，QMainWindow::menuBar方法会在首次调用时返回一个QMenuBar指针
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    //添加一条线
    separatorAction = fileMenu->addSeparator();
    for(int i =0;i<MAX_RECENT_FILES;i++){
        fileMenu->addAction(recentFileActions[i]);
    }
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    //编辑菜单
    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(cutAction);
    editMenu->addAction(copyAction);
    editMenu->addAction(pasteAction);
    editMenu->addAction(deleteAction);
    selectSubMenu = editMenu->addMenu(tr("&Select"));
    selectSubMenu->addAction(selectRowAction);
    selectSubMenu->addAction(selectColumnAction);
    selectSubMenu->addAction(selectAllAction);
    editMenu->addSeparator();
    editMenu->addAction(findAction);
    editMenu->addAction(gotoCellAction);

    //工具菜单
    toolsMenu = menuBar()->addMenu(tr("&Tools"));
    toolsMenu->addAction(recalculateAction);
    toolsMenu->addAction(sortAction);

    //options菜单
    optionsMenu = menuBar()->addMenu(tr("&Options"));
    optionsMenu->addAction(showGridAction);
    optionsMenu->addAction(autoRecalculateAction);

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(aboutQtAction);

}

void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("&File"));
    fileToolBar->addAction(newAction);
    fileToolBar->addAction(openAction);
    fileToolBar->addAction(saveAction);

    editToolBar = addToolBar(tr("&Edit"));
    editToolBar->addAction(cutAction);
    editToolBar->addAction(copyAction);
    editToolBar->addAction(pasteAction);
    editToolBar->addSeparator();
    editToolBar->addAction(findAction);
    editToolBar->addAction(gotoCellAction);
}

void MainWindow::createContextMenu(){
    spreadsheet->addAction(cutAction);
    spreadsheet->addAction(copyAction);
    spreadsheet->addAction(pasteAction);
    //任何一个Qt窗口部件都有一个QAction列表与之对应，这里右键点击即可查看菜单
    spreadsheet->setContextMenuPolicy(Qt::ActionsContextMenu);
}

void MainWindow::createStatusBar()
{
    locationLabel = new QLabel(" W999 ");
    locationLabel->setAlignment(Qt::AlignCenter);
    //sizeHint提供理想大小
    locationLabel->setMinimumSize(locationLabel->sizeHint());

    formulaLabel = new QLabel;
    formulaLabel->setIndent(3);
    //QMainWindow::statusBar函数返回一个指向状态栏的指针
    statusBar()->addWidget(locationLabel);
    //第二个参数为伸展因子，默认为0，这里设置为1的意思是将右边多余的空间都分配给formulaLabel
    statusBar()->addWidget(formulaLabel,1);
    //spreadsheet的格子变化时，更新主窗口的状态栏
    connect(spreadsheet,SIGNAL(currentCellChanged(int,int,int,int)),this,SLOT(updateStatusBar()));
    connect(spreadsheet,SIGNAL(modified()),this,SLOT(spreadsheetModified()));
    updateStatusBar();
}

void MainWindow::updateStatusBar()
{
    locationLabel->setText(spreadsheet->currentLocation());
    formulaLabel->setText(spreadsheet->currentFormula());
}

void MainWindow::spreadsheetModified()
{
    setWindowModified(true);
    updateStatusBar();
}

void MainWindow::newFile()
{
    MainWindow *mainWin = new MainWindow;
    mainWin->show();
//    if(okToContinue()){
//        spreadsheet->clear();
//        setCurrentFile("");
//    }
}

bool MainWindow::okToContinue(){
    if(isWindowModified()){
        int r = QMessageBox::warning(this,
                                     tr("Spreadsheet"),
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

void MainWindow::open()
{
    if(okToContinue()){
        QString fileName = QFileDialog::getOpenFileName(this,tr("Open Spreadsheet"),".",tr("Spreadsheet files (*.sp)"));
        if(!fileName.isEmpty()){
            loadFile(fileName);
        }
    }
}

bool MainWindow::loadFile(const QString &fileName)
{
    if(!spreadsheet->readFile(fileName)){
        statusBar()->showMessage(tr("loading cancelled"),2000);
        return false;
    }
    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File Loaded"),2000);
    return true;
}

bool MainWindow::save()
{
    if(curFile.isEmpty()){
        return saveAs();
    }else{
       return saveFile(curFile);
    }
}

bool MainWindow::saveFile(const QString &fileName){
    if(!spreadsheet->writeFile(fileName)){
        statusBar()->showMessage(tr("Saving cancelled"),2000);
        return false;
    }
    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"),2000);
    return true;
}

bool MainWindow::saveAs()
{
    //如果存在重复的文件，会询问用户，可以通过传入QFileDialog::DontConfirmOverwrite来改变这个行为
    QString fileName = QFileDialog::getSaveFileName(this,tr("Save Spreadsheet"),".",tr("Spreadsheet files (*.sp)"));
    if(fileName.isEmpty()){
        return false;
    }
    return saveFile(fileName);
}

//重新实现该事件可以截获close槽
void MainWindow::closeEvent(QCloseEvent *event){
    if(okToContinue()){
        writeSettings();
        event->accept();
    }else{
        event->ignore();
    }
}

void MainWindow::setCurrentFile(const QString &fileName){
    curFile = fileName;
    setWindowModified(false);
    QString shownName = tr("Untitled");
    if(!curFile.isEmpty()){
        shownName = strippedName(curFile);
        recentFiles.removeAll(curFile);
        recentFiles.prepend(curFile);
        foreach(QWidget *win,QApplication::topLevelWidgets()){
            if(MainWindow *mainWin = qobject_cast<MainWindow *>(win)){
                mainWin->updateRecentFileActions();
            }
        }

        updateRecentFileActions();
    }
    setWindowTitle(tr("%1[*] - %2").arg(shownName).arg(tr("Spreadsheet")));
}

//移除文件路径
QString MainWindow::strippedName(const QString &fullFileName){
    return QFileInfo(fullFileName).fileName();
}

void MainWindow::updateRecentFileActions(){
    QMutableStringListIterator i(recentFiles);
    while(i.hasNext()){
        if(!QFile::exists(i.next())){
            i.remove();
        }
    }
    for(int j=0;j<MAX_RECENT_FILES;j++){
        if(j<recentFiles.count()){
            QString text = tr("&%1 %2").arg(j+1).arg(strippedName(recentFiles[j]));
            recentFileActions[j]->setText(text);
            recentFileActions[j]->setData(recentFiles[j]);
            recentFileActions[j]->setVisible(true);
        }else{
            recentFileActions[j]->setVisible(false);
        }
    }
    separatorAction->setVisible(!recentFiles.isEmpty());
}

void MainWindow::openRecentFile(){
    if(okToContinue()){
        //sender可以查出是哪个特有动作调用了这个槽，qobject_cast<T>()，如果转化不成功，返回0，否则返回对应QObject子类的指针
        QAction *action = qobject_cast<QAction *>(sender());
        if(action){
            loadFile(action->data().toString());
        }
    }
}

void MainWindow::find(){
    if(!findDialog){
        findDialog = new FindDialog(this);
        connect(findDialog,SIGNAL(findNext(const QString&,Qt::CaseSensitivity)),spreadsheet,SLOT(findNext(QString,Qt::CaseSensitivity)));
        connect(findDialog,SIGNAL(findPrevious(QString,Qt::CaseSensitivity)),spreadsheet,SLOT(findPrevious(QString,Qt::CaseSensitivity)));
    }
    findDialog->show();
    //raise和activateWindow是在对话框已经打开的情况下才有意义
    findDialog->raise();
    findDialog->activateWindow();
}

void MainWindow::goToCell()
{
    GoToCellDialog *dialog = new GoToCellDialog(this);
    //QDialog::exec可以返回一个true值(QDialog::Accepted),否则返回QDialog::rejected
    if(dialog->exec()){
        QString str = dialog->lineEdit->text().toUpper();
        spreadsheet->setCurrentCell(str.mid(1).toInt()-1,str[0].unicode()-'A');
    }
    //注意，这里需要删除
    delete dialog;
}

void MainWindow::sort()
{
    SortDialog dialog(this);
    QTableWidgetSelectionRange range = spreadsheet->selectedRange();
    dialog.setColumnRange('A'+range.leftColumn(),'A'+range.rightColumn());
    if(dialog.exec()){
        SpreadsheetCompare compare;
        //这不是一个好的实践，因为如果SortDialog改变了，这里的代码也需要改变
        //较好的实践是，让Dialog提供一个方法，该方法返回一个Compare对象
        compare.keys[0] = dialog.primaryCombo->currentIndex();
        compare.keys[1] = dialog.secondaryCombo->currentIndex()-1;
        compare.keys[2] = dialog.tertiaryCombo->currentIndex()-1;
        compare.ascending[0]=(dialog.primaryOrder->currentIndex()==0);
        compare.ascending[1]=(dialog.secondaryOrder->currentIndex()==0);
        compare.ascending[2]=(dialog.tertiaryOrder->currentIndex()==0);
        spreadsheet->sort(compare);
    }
}

void MainWindow::about()
{
    QMessageBox::about(this,tr("About Spreadsheet"),tr("<h2>Spreadsheet 1.0</h2>"
                                                      "<p>Copyright &copy; 2017 Software Inc."
                                                      "<p>Spreadsheet is a small application that"
                                                      "demonstrates QAction,QMainWindow,QMenuBar,"
                                                      "QStatusBar,QTableWidget,QToolBar,and many other"
                                                      "Qt classes."));
}


void MainWindow::writeSettings()
{
    QSettings settings("SoftWare Inc.","Spreadsheet");
    settings.setValue("geometry",saveGeometry());
    settings.setValue("recentFiles",recentFiles);
    settings.setValue("showGrid",showGridAction->isChecked());
    settings.setValue("autoRecalc",autoRecalculateAction->isChecked());
}

void MainWindow::readSettings()
{
     QSettings settings("SoftWare Inc.","Spreadsheet");
     restoreGeometry(settings.value("geometry").toByteArray());
     recentFiles = settings.value("recentFiles").toStringList();
     updateRecentFileActions();
     bool showGrid = settings.value("showGrid",true).toBool();
     showGridAction->setChecked(showGrid);
     bool autoRecalc = settings.value("autoRecalc",true).toBool();
     autoRecalculateAction->setChecked(autoRecalc);

}
