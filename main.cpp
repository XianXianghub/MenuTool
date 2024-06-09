#include "mainwindow.h"
#include <QApplication>
#include <QNetworkProxy>
#include "sshconfig.h"

#include <QString>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowIcon(QIcon(":/image/pkq.ico"));
//    QNetworkProxy proxy;
//        proxy.setType(QNetworkProxy::NoProxy);
//        QNetworkProxy::setApplicationProxy(proxy);
   // w.show();
    return a.exec();
}
