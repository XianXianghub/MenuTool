#include "sshclient.h"
#include <QHostAddress>
#include <QDebug>


SSHClient::SSHClient(const QString &hostname, int port, const QString &username, const QString &password, QObject *parent)
    : QObject(parent), hostname(hostname), port(port), username(username), password(password)
{
    session = NULL;
    listener1 = NULL;
    listener2 = NULL;
    socket =  new QTcpSocket();



    connect(socket, &QTcpSocket::connected, this, &SSHClient::onConnected);
    connect(socket, &QTcpSocket::readyRead, this, &SSHClient::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &SSHClient::onDisconnected);
}

SSHClient::SSHClient()
{
    hostname = "43.248.140.157";
    port = 16127;
    username =  "xiangsq";
    password = "xiangsq";;
    socket =  new QTcpSocket();



    connect(socket, &QTcpSocket::connected, this, &SSHClient::onConnected);
    connect(socket, &QTcpSocket::readyRead, this, &SSHClient::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &SSHClient::onDisconnected);

//            SSHClient *mSShclient = new SSHClient("43.248.140.157",16127, "xiangsq","xiangsq");
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
      qDebug() << "connectToHost";
    socket->connectToHost(QHostAddress(hostname), port);
}

void SSHClient::onConnected()
{
    qDebug() << "Connected to host";
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


    // 设置反向端口转发
    listener1 = libssh2_channel_forward_listen_ex(session, "127.0.0.1", 55590, NULL, 1);

    if (!listener1) {
        qDebug() << "Error setting up port forwarding for 55578:" << libssh2_session_last_error(session, NULL, NULL, 0);
        libssh2_session_disconnect(session, "Normal Shutdown, Thank you for playing");
        libssh2_session_free(session);
        return;
    }


    qDebug() << "Port forwarding setup successfully.";

    while (true) {
            LIBSSH2_CHANNEL *channel = libssh2_channel_forward_accept(listener1);
            if (channel) {
                handleForwardedConnection(channel);
            }
        }
}

void SSHClient::handleForwardedConnection(LIBSSH2_CHANNEL *channel)
{
    qDebug() << "Handling forwarded connection";

    QTcpSocket localSocket;
    localSocket.connectToHost("127.0.0.1", 5037);
    if (!localSocket.waitForConnected()) {
        qDebug() << "Failed to connect to local port 5037";
        libssh2_channel_free(channel);
        return;
    }

    qDebug() << "Connected to local port 5037";

    while (true) {
        qDebug() << "libssh2_channel_eof";

        if (libssh2_channel_eof(channel)) {
            qDebug() << "Channel EOF detected";
            break;
        }
        qDebug() << "libssh2_channel_read";

        char buffer[1024];
        ssize_t n = libssh2_channel_read(channel, buffer, sizeof(buffer));
        if (n > 0) {
            qDebug() << "Read" << n << "bytes from channel:" << QByteArray(buffer, n);
            qint64 written = localSocket.write(buffer, n);
            if (written != n) {
                qDebug() << "Failed to write all data to local socket";
                break;
            }
        } else if (n < 0) {
            qDebug() << "Error reading from channel:" << n;
            break;
        }
       qDebug() << "waitForReadyRead";
        // 等待本地 socket 有数据可读
        if (!localSocket.waitForReadyRead(3000)) {  // 等待 3 秒钟
            qDebug() << "Failed to wait for local socket to be ready";
            break;
        }

        // 读取本地 socket 的数据
        QByteArray localData;
        while (localSocket.bytesAvailable() > 0) {
            localData.append(localSocket.readAll());
            // 在一定时间内等待数据到达
            if (!localSocket.waitForReadyRead(1000)) {  // 等待 1 秒钟
                break;
            }
        }

        // 检查是否成功读取到数据
        if (localData.isEmpty()) {
            qDebug() << "Failed to read data from local socket";
            break;
        }

        qDebug() << "Read" << localData.size() << "bytes from local socket:" << localData;

        // 将读取到的数据写入 SSH 通道
        ssize_t nwritten = libssh2_channel_write(channel, localData.constData(), localData.size());
        if (nwritten < 0) {
            qDebug() << "Error writing to channel:" << nwritten;
            break;
        }

        // 打印写入的数据
        qDebug() << "Written" << nwritten << "bytes to channel:" << localData;
    }

    qDebug() << "Closing channel";
    libssh2_channel_close(channel);
    libssh2_channel_free(channel);
}


void SSHClient::onReadyRead()
{
    QByteArray data = socket->readAll();
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
