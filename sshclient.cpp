#include "sshclient.h"
#include <QHostAddress>
#include <QDebug>
#include <winsock2.h>  // 添加此行
#include "toolutils.h"
#include <QNetworkProxy>
#include <QElapsedTimer>
#include <QThread>


SSHClient::SSHClient(const QString &hostname, int port, const QString &username, const QString &password, Logger *logger, QObject *parent)
    : QObject(parent), hostname(hostname), port(port), username(username), password(password),logger(logger)
{
    session = NULL;
    listener1 = NULL;
    listener2 = NULL;
    socket =  new QTcpSocket();


    connect(socket, &QTcpSocket::connected, this, &SSHClient::onConnected);
    connect(socket, &QTcpSocket::readyRead, this, &SSHClient::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &SSHClient::onDisconnected);

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

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
    WSACleanup();
}

void SSHClient::connectToHost()
{
//    QNetworkProxy proxy;
//        proxy.setType(QNetworkProxy::NoProxy);
//        QNetworkProxy::setApplicationProxy(proxy);
      qDebug() << "connectToHost"<<hostname<<port;
    socket->connectToHost(QHostAddress(hostname), port);
}

void SSHClient::onConnected()
{
    logger->log("Connected to host");
    session = libssh2_session_init();
    if (!session) {
        qDebug() << "Failed to create session";
        return;
    }

    int sock = socket->socketDescriptor();
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

    int localport = 55597;

    listener1 = libssh2_channel_forward_listen_ex(session, "127.0.0.1", localport, NULL, 1);

    if (!listener1) {
        qDebug() << "Error setting up port forwarding for 55578:" << libssh2_session_last_error(session, NULL, NULL, 0);
        libssh2_session_disconnect(session, "Normal Shutdown, Thank you for playing");
        libssh2_session_free(session);
        return;
    }


    qDebug() << "Port forwarding setup successfully port="<<localport;


    while (true) {
            LIBSSH2_CHANNEL *channel = libssh2_channel_forward_accept(listener1);
            if (channel) {
                handleForwardedConnection(channel);
            }
        }
}
void SSHClient::handleForwardedConnection(LIBSSH2_CHANNEL *channel)
{

    QTcpSocket *localSocket = new QTcpSocket();
    localSocket->connectToHost("127.0.0.1", 5037);
    if (!localSocket->waitForConnected(3000)) {
        logger->log("Failed to connect to local port 5037");
        libssh2_channel_free(channel);
        delete localSocket;
        return;
    }

    libssh2_channel_set_blocking(channel, 0);


    while (true) {

        if (libssh2_channel_eof(channel)) {
            logger->log("Channel EOF detected");
            break;
        }
        if (!localSocket->waitForConnected(3000)) {
            logger->log("localSocket disconnect");
            break;
        }

        bool done = false;
        while(true){
            char buffer[1 * 1024];
            ssize_t n = libssh2_channel_read(channel, buffer, sizeof(buffer));
            if (n > 0) {
    //            QString bufferString = QString::fromUtf8(buffer, n);
    //            logger->log("Read from channel: " + bufferString);

                qint64 written = localSocket->write(buffer, n);
                if (written == -1) {
                    logger->log("Failed to write data to local socket");
                    break;
                }
//                logger->log("Read " + QString::number(n) + " bytes from channel.");
            } else {
                QThread::msleep(3);
                break;
            }
        }




        if (localSocket->waitForReadyRead(10)) {
//            logger->log("localSocket descriptor is set");
            QByteArray localData = localSocket->readAll();
            if (!localData.isEmpty()) {
                qint64 bytesRemaining = localData.size();
//                logger->log("libssh2_channel_write localData " + QString::number(bytesRemaining));

                const char *dataPtr = localData.constData();

                libssh2_channel_set_blocking(channel, 1);
                while (bytesRemaining > 0) {
                    ssize_t bytesToWrite = qMin<qint64>(bytesRemaining, 1 * 1024);
                    ssize_t nwritten = libssh2_channel_write(channel, dataPtr, bytesToWrite);
                    if (nwritten < 0) {
                        logger->log("Error writing to channel: " + QString::number(nwritten));
                        break;
                    }
                    dataPtr += nwritten;
                    bytesRemaining -= nwritten;
                }
                libssh2_channel_set_blocking(channel, 0);
//                logger->log("Finished writing data to channel");
            }
        }

    }

//    logger->log("libssh2_channel_close");
    libssh2_channel_close(channel);
    libssh2_channel_free(channel);
    localSocket->close();
    delete localSocket;
}


void SSHClient::waitForChannelRead(LIBSSH2_SESSION *session)
{
    fd_set fds;
    struct timeval timeout;

    int dir = libssh2_session_block_directions(session);

    FD_ZERO(&fds);

    int sock = socket->socketDescriptor();
    if (dir & LIBSSH2_SESSION_BLOCK_INBOUND) {
        FD_SET(sock, &fds);
    }

    timeout.tv_sec = 0;  // 1 秒超时
    timeout.tv_usec = 1000;

    int rc = select(sock + 1, &fds, NULL, NULL, &timeout);
//    if (rc <= 0) {
//        logger->log("Failed to wait for channel read readiness");
//    }
}

void SSHClient::onReadyRead()
{
    QByteArray data = socket->readAll();
    logger->log("Received data: " + data);
}

void SSHClient::onDisconnected()
{
    logger->log("Disconnected from host");

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
