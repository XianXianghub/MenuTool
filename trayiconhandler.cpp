#include "trayiconhandler.h"
#include <QDebug>
#include <QProcess>

TrayIconHandler::TrayIconHandler( QSystemTrayIcon *trayIcon, QObject *parent)
    : QObject(parent)
{
   this->trayIcon = trayIcon;
  connect(trayIcon, &QSystemTrayIcon::messageClicked, this, &TrayIconHandler::onMessageClicked);
}

void TrayIconHandler::showMessage(const QString &title, const QString &message)
{
    // 存储消息标题和内容
    lastMessageTitle = title;
    lastMessageContent = message;

    // 显示托盘消息
    trayIcon->showMessage(title, message, QSystemTrayIcon::Information, 5000);
}

void TrayIconHandler::onMessageClicked()
{

    // 打印消息内容
    qDebug() << "Tray message was clicked!";
    qDebug() << "Title:" << lastMessageTitle;
    qDebug() << "Content:" << lastMessageContent;
    QStringList list = lastMessageContent.split("\n");

    if(list.size() == 3){
        QStringList list2 = list[1].split(" ");
        if(list2.size() >= 2){
            if(QString::compare(list2[1], "编译成功") == 0){
                QString strPicPath =  "\\\\192.168.101.80\\share\\daily_build\\"+list2[0];
                qDebug() << "Project strPicPath:" << strPicPath;
                strPicPath.replace("/", "\\");
                QProcess process;
                process.startDetached("explorer", QStringList() << QString("/root,") << QString("%1").arg(strPicPath));

            }
        }
    }



}
