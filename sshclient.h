#ifndef SSHCLIENT_H
#define SSHCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <libssh2.h>

class SSHClient : public QObject
{
    Q_OBJECT

public:
    SSHClient(const QString &hostname, int port, const QString &username, const QString &password, QObject *parent);
    ~SSHClient();
    void connectToHost();

private slots:
    void onConnected();
    void onReadyRead();
    void onDisconnected();

private:
    QTcpSocket socket;
    QString hostname;
    int port;
    QString username;
    QString password;
    LIBSSH2_SESSION *session;
    LIBSSH2_LISTENER *listener1;
    LIBSSH2_LISTENER *listener2;
};

#endif // SSHCLIENT_H