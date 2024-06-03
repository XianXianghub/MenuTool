#include "mainwindow.h"
#include <QApplication>

#include <QString>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowIcon(QIcon(":/image/pkq.ico"));

   // w.show();
    return a.exec();
}
