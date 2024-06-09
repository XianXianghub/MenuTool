#ifndef SSHCLIENT_H
#define SSHCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <libssh2.h>
#include "logger.h"
#include "sshconfig.h"


#include <QThread>
class SSHClient : public QObject
{
    Q_OBJECT

public:
    SSHClient(Logger * logger);
    ~SSHClient();
    void connectToHost(SshConfig config);

private slots:
    void onConnected();
    void onReadyRead();
    void onDisconnected();
signals:
    void startForward(); // 添加信号声明

private:
    void handleForwardedConnection(LIBSSH2_CHANNEL *channel);
    QTcpSocket *socket;

    SshConfig config;

    LIBSSH2_SESSION *session;
    LIBSSH2_LISTENER *listener1;
    LIBSSH2_LISTENER *listener2;
    Logger *logger; // Add a Logger instance

};

#endif // SSHCLIENT_H
