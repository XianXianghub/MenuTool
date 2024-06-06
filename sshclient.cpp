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
//        qDebug() << "libssh2_channel_forward_accept 111";

            LIBSSH2_CHANNEL *channel = libssh2_channel_forward_accept(listener1);
//            qDebug() << "libssh2_channel_forward_accept 222";

            if (channel) {
//                qDebug() << "libssh2_channel_forward_accept 3333";
                handleForwardedConnection(channel);
//                qDebug() << "libssh2_channel_forward_accept 4444";
            }
        }
}
void SSHClient::handleForwardedConnection(LIBSSH2_CHANNEL *channel)
{
    QTcpSocket *localSocket = new QTcpSocket();
    if (localSocket->state() != QAbstractSocket::ConnectedState) {
        localSocket->connectToHost("127.0.0.1", 5037);
        logger->log("localSocket->connectToHost");
    }

    libssh2_channel_set_blocking(channel, 0);

    while (true) {
        if (!localSocket->waitForConnected()) {
            logger->log("Failed to connect to local port 5037");
            libssh2_channel_free(channel);
            return;
        }

        if (libssh2_channel_eof(channel)) {
            logger->log("Channel EOF detected");
            break;
        }

        logger->log("libssh2_channel_read");

        bool done = false;
        int cnt = 0;
        QElapsedTimer timer;
        QByteArray dataBuffer; // 存储从channel读取的数据

        while (!done) {
            char buffer[1*1024];
            ssize_t n = libssh2_channel_read(channel, buffer, sizeof(buffer));

            if (n > 0) {
                // 将数据追加到dataBuffer中
                dataBuffer.append(buffer, n);
                cnt += n;
                logger->log("Read " + QString::number(n) + " bytes from channel: ");
                logger->log("Read total " + QString::number(cnt) );
                timer.restart(); // 重新开始计时
            } else if (n == LIBSSH2_ERROR_EAGAIN) {
                // 检查是否超过500毫秒
                if (timer.isValid() && timer.elapsed() >= 200) {
                    logger->log("Timeout: No data received for 500ms");
                    done = true;
                }
            } else if (n == 0) {
                done = true;
            } else {
                logger->log("Error reading from channel: " + QString::number(n));
                break;
            }
        }

        // 将dataBuffer中的数据写入localSocket
        qint64 totalWritten = 0;
        while (totalWritten < dataBuffer.size()) {
            qint64 written = localSocket->write(dataBuffer.constData() + totalWritten, dataBuffer.size() - totalWritten);
            if (written == -1) {
                logger->log("Failed to write data to local socket");
                break;
            }
            totalWritten += written;
        }

        // 清空dataBuffer
        dataBuffer.clear();




        logger->log("waitForReadyRead +++");
        if (!localSocket->waitForReadyRead(3000)) {  // 等待 3 秒钟
            logger->log("Failed to wait for local socket to be ready");
            libssh2_channel_free(channel);
            return;
        }
        logger->log("waitForReadyRead ---");
        logger->log("localSocket readAll +++");
        QByteArray localData;
        while (localSocket->bytesAvailable() > 0) {
            localData.append(localSocket->readAll());
            if (!localSocket->waitForReadyRead(100)) {  // 等待 1 秒钟
                break;
            }
        }
        logger->log("localSocket readAll ---");

        // 检查是否成功读取到数据
         if (localData.isEmpty()) {
             logger->log("Failed to read data from local socket");
             break;
         }

        logger->log("Read " + QString::number(localData.size()) + " bytes from local socket: " + localData);

        // 将读取到的数据分段写入 SSH 通道
        qint64 bytesRemaining = localData.size();
        const char *dataPtr = localData.constData();

        while (bytesRemaining > 0) {
            ssize_t bytesToWrite = qMin<qint64>(bytesRemaining, 1024*1024);  // 每次写入最多 1024 字节
//            logger->log("dddd===" + QString::number(bytesToWrite));

            ssize_t nwritten = libssh2_channel_write(channel, dataPtr, bytesToWrite);
            if (nwritten < 0) {
                logger->log("Error writing to channel: " + QString::number(nwritten));
                libssh2_channel_free(channel);
                return;
            }

            dataPtr += nwritten;
            bytesRemaining -= nwritten;

//            logger->log("Wrote " + QString::number(nwritten) + " bytes to channel");
        }

        logger->log("Finished writing data to channel");
    }

    logger->log("libssh2_channel_close");
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
