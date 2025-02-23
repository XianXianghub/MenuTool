#ifndef TRAYICONHANDLER_H
#define TRAYICONHANDLER_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QApplication>

class TrayIconHandler : public QObject
{
    Q_OBJECT

public:
    explicit TrayIconHandler( QSystemTrayIcon *trayIcon, QObject *parent = nullptr);
    void showMessage(const QString &title, const QString &message);

private slots:
    void onMessageClicked();

private:
    QSystemTrayIcon *trayIcon; // 系统托盘图标

    QString lastMessageTitle;  // 最后显示的消息标题
    QString lastMessageContent; // 最后显示的消息内容
};

#endif // TRAYICONHANDLER_H
