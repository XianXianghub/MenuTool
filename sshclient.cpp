#include "sshclient.h"
#include <QHostAddress>
#include <QDebug>


SSHClient::SSHClient(const QString &hostname, int port, const QString &username, const QString &password, QObject *parent)
    : QObject(parent), hostname(hostname), port(port), username(username), password(password)
{
    session = NULL;
    listener1 = NULL;
    listener2 = NULL;
    connect(&socket, &QTcpSocket::connected, this, &SSHClient::onConnected);
    connect(&socket, &QTcpSocket::readyRead, this, &SSHClient::onReadyRead);
    connect(&socket, &QTcpSocket::disconnected, this, &SSHClient::onDisconnected);
}

SSHClient::~SSHClient()
{
    if (listener1) {
        libssh2_channel_forward_cancel(listener1);
    }
    if (listener2) {
        libssh2_channel_forward_cancel(listener2);
    }
    if (session) {
        libssh2_session_disconnect(session, "Normal Shutdown, Thank you for playing");
        libssh2_session_free(session);
    }
    libssh2_exit();
}

void SSHClient::connectToHost()
{
    socket.connectToHost(QHostAddress(hostname), port);
}

void SSHClient::onConnected()
{
    qDebug() << "Connected to host";
    session = libssh2_session_init();
    if (!session) {
        qDebug() << "Failed to create session";
        return;
    }

    int sock = socket.socketDescriptor();
    int rc = libssh2_session_handshake(session, sock);
    if (rc) {
        qDebug() << "Failure establishing SSH session:" << rc;
        return;
    }

    rc = libssh2_userauth_password(session, username.toStdString().c_str(), password.toStdString().c_str());
    if (rc) {
        qDebug() << "Authentication by password failed.";
        libssh2_session_disconnect(session, "Normal Shutdown, Thank you for playing");
        libssh2_session_free(session);
        return;
    }

    qDebug() << "Authentication succeeded.";


    // 设置反向端口转发
    listener1 = libssh2_channel_forward_listen_ex(session, hostname.toUtf8().constData(), port, NULL, 1);

    if (!listener1) {
        qDebug() << "Error setting up port forwarding for 55578:" << libssh2_session_last_error(session, NULL, NULL, 0);
        libssh2_session_disconnect(session, "Normal Shutdown, Thank you for playing");
        libssh2_session_free(session);
        return;
    }

//    listener2 = libssh2_channel_forward_listen_ex(session, 55579, "127.0.0.1", 9009, NULL, 1);
//    listener2 = libssh2_channel_forward_listen_ex(session, hostname.toUtf8().constData(), port, nullptr, 1);

//    if (!listener2) {
//        qDebug() << "Error setting up port forwarding for 55579:" << libssh2_session_last_error(session, NULL, NULL, 0);
//        libssh2_session_disconnect(session, "Normal Shutdown, Thank you for playing");
//        libssh2_session_free(session);
//        return;
//    }

    qDebug() << "Port forwarding setup successfully.";
}

void SSHClient::onReadyRead()
{
    QByteArray data = socket.readAll();
    qDebug() << "Received data:" << data;
}

void SSHClient::onDisconnected()
{
    qDebug() << "Disconnected from host";

    if (listener1) {
        libssh2_channel_forward_cancel(listener1);
    }
    if (listener2) {
        libssh2_channel_forward_cancel(listener2);
    }
    if (session) {
        libssh2_session_disconnect(session, "Normal Shutdown, Thank you for playing");
        libssh2_session_free(session);
    }
    libssh2_exit();
}
