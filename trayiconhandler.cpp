#include "trayiconhandler.h"
#include <QDebug>

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
    trayIcon->showMessage(title, message, QSystemTrayIcon::Information, 3000);
}

void TrayIconHandler::onMessageClicked()
{
    // 打印消息内容
    qDebug() << "Tray message was clicked!";
    qDebug() << "Title:" << lastMessageTitle;
    qDebug() << "Content:" << lastMessageContent;
}
