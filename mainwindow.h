#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
class QAction;
class QLabel;
class FindDialog;
class Spreadsheet;
class MainWindow : public QMainWindow
{
    //要提供自己的信号和槽的时候才需要这个定义
    Q_OBJECT
public :
    MainWindow();
protected:
    //closeEvent是QWidget类中的一个虚函数，当用户关闭窗口时，这个函数被调用
    //此处重新实现
    void closeEvent(QCloseEvent *event);

private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void find();
    void goToCell();
    void sort();
    void about();
    void openRecentFile();
    void updateStatusBar();
    void spreadsheetModified();

  private:
    void createActionns();
    void createMenus();
    void createContextMenu();
    void createToolBars();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    bool okToContinue();
    bool loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    void updateRecentFileActions();
    QString strippedName(const QString &fullFileName);

    Spreadsheet *spreadsheet;
    FindDialog *findDialog;
    QLabel *locationLabel;
    QLabel *formulaLabel;
    QStringList recentFiles;
    QString curFile;
    enum {MAX_RECENT_FILES = 5};
    QAction *recentFileActions[MAX_RECENT_FILES];
    //间隔指针，用于隐藏和显示recent open files
    QAction *separatorAction;

    //头部菜单
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *toolsMenu;
    QMenu *optionsMenu;
    QMenu *helpMenu;

    //子菜单
    QMenu *selectSubMenu;

    QToolBar *fileToolBar;
    QToolBar *editToolBar;

    //文件菜单相关动作
    QAction *newAction;
    QAction *openAction;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction *closeAction;
    QAction *exitAction;

    //编辑菜单相关动作
    QAction *cutAction;
    QAction *pasteAction;
    QAction *copyAction;
    QAction *deleteAction;
    QAction *selectRowAction;
    QAction *selectColumnAction;
    QAction *selectAllAction;
    QAction *findAction;
    QAction *gotoCellAction;

    //Tools菜单相关动作
    QAction *recalculateAction;
    QAction *sortAction;

    //选项菜单相关动作
    QAction *showGridAction;
    QAction *autoRecalculateAction;

    //help菜单相关动作
    QAction *aboutAction;
    QAction *aboutQtAction;
};
#endif // MAINWINDOW_H
