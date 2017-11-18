#include <QApplication>
#include<QtWidgets>
#include "mainwindow.h"
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QSplashScreen *splash = new QSplashScreen;
    splash->setPixmap(QPixmap(":/images/icon.jpg"));
    splash->show();
    Qt::Alignment topRight = Qt::AlignRight|Qt::AlignTop;
    splash->showMessage(QObject::tr("Setting up the main window..."),topRight,Qt::white);
    MainWindow *window = new MainWindow;
    window->show();
    splash->finish(window);
    delete splash;
    return app.exec();
}
