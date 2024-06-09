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
    // 定义一个信号，用于发送从 SSH 连接中收到的数据
    void sshDataReceived(const QString &data);
private:
    void handleForwardedConnection(LIBSSH2_CHANNEL *channel);
    void handleChannel2(LIBSSH2_LISTENER *listener2);
    void proccessData(QString data);
    QTcpSocket *socket;

    SshConfig config;

    LIBSSH2_SESSION *session;
    LIBSSH2_LISTENER *listener1;
    LIBSSH2_LISTENER *listener2;
    Logger *logger; // Add a Logger instance

};
class SSHClientManager : public QObject
{
    Q_OBJECT

public:
    SSHClientManager(Logger *logger)
        : logger(logger) {}

    void startSSHClient(const SshConfig &config)
    {
        SSHClient *client = new SSHClient(logger);
        QThread *thread = new QThread();

        client->moveToThread(thread);

        connect(thread, &QThread::started, [client, config]() {
            client->connectToHost(config);
        });
        connect(client, &SSHClient::sshDataReceived, this, &SSHClientManager::forwardSSHData);

        connect(thread, &QThread::finished, thread, &QThread::deleteLater);

        thread->start();
    }
signals:
    // 定义信号，用于转发从 SSHClient 接收到的数据
    void forwardSSHData(const QString &data);
private:
    Logger *logger;
};

#endif // SSHCLIENT_H
