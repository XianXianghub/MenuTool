#ifndef SSHCLIENT_H
#define SSHCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <libssh2.h>
#include "logger.h"
#include "sshconfig.h"
#include <QThread>
#include <QElapsedTimer>  // 添加此行




class SSHClient : public QObject
{
    Q_OBJECT

public:
    SSHClient(Logger * logger);
    ~SSHClient();
    void connectToHost(SshConfig config);
     void stopLooping() { stopLoop = true; } // 添加一个新的槽函数用于停止循环

private slots:
    void onConnected();
    void onReadyRead();
    void onDisconnected();

signals:
    void startForward(); // 添加信号声明
    // 定义一个信号，用于发送从 SSH 连接中收到的数据
    void sshDataReceived(const QString &data);
private:
    void handleForwardedConnection(LIBSSH2_CHANNEL *channel);
    void handleChannel2(LIBSSH2_LISTENER *listener2);
    void proccessData(QString data);
    QTcpSocket *socket;
    bool stopLoop = false; // 新的成员变量，用于标识是否停止循环
    QElapsedTimer keepAliveTimer;  // 添加此行
    qint64 keepAliveInterval;      // 添加此行
    bool hasConncted = false;

    SshConfig config;

    LIBSSH2_SESSION *session;
    LIBSSH2_LISTENER *listener1;
    LIBSSH2_LISTENER *listener2;
    Logger *logger; // Add a Logger instance

};



#endif // SSHCLIENT_H
