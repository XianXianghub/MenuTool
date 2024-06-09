#include "sshclient.h"
#include <QHostAddress>
#include <QDebug>
#include <winsock2.h>  // 添加此行
#include "toolutils.h"
#include <QNetworkProxy>
#include <QElapsedTimer>
#include <QThread>
#include "constlist.h"

#include <QProcess>


SSHClient::SSHClient(Logger *logger)
    : logger(logger)
{
    session = NULL;
    listener1 = NULL;
    listener2 = NULL;
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

void SSHClient::connectToHost(SshConfig config)
{
    qDebug() << "sshclient Name:" << config.name;
    qDebug() << "sshclient SSH Host:" << config.ssh_host;
    qDebug() << "sshclient SSH Port:" << config.ssh_port;
    qDebug() << "sshclient SSH User:" << config.ssh_user;
    qDebug() << "sshclient SSH Password:" << config.ssh_pwd;
    qDebug() << "sshclient ADB Remote Host:" << config.adb_remote_host;
    qDebug() << "sshclient ADB Remote Port:" << config.adb_remote_port;
    qDebug() << "sshclient CMD Local Host:" << config.cmd_local_host;
    qDebug() << "sshclient CMD Local Port:" << config.cmd_local_port;
    qDebug() << "--------------------------";
    this->config = config;
    socket = new QTcpSocket();
    socket->setProxy(QNetworkProxy::NoProxy);
    connect(socket, &QTcpSocket::connected, this, &SSHClient::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &SSHClient::onDisconnected);
    qDebug() << "connectToHost" << config.ssh_host << config.ssh_port;
    socket->connectToHost(QHostAddress(config.ssh_host), config.ssh_port);
    if (!socket->waitForConnected(3000)) {
        qDebug() << "connectToHost " << config.ssh_host << config.ssh_port << " Connection timed out.";
        // 处理连接超时的情况
    } else {
        qDebug() << "Connected!";
        // 连接成功后的处理
    }
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

    rc = libssh2_userauth_password(session, config.ssh_user.toStdString().c_str(), config.ssh_pwd.toStdString().c_str());
    if (rc) {
        qDebug() << "Authentication by password failed.";
        libssh2_session_disconnect(session, "Normal Shutdown, Thank you for playing");
        libssh2_session_free(session);
        return;
    }

    qDebug() << "Authentication succeeded.";

    listener1 = libssh2_channel_forward_listen_ex(session, config.adb_remote_host.toLatin1().constData(), config.adb_remote_port, NULL, 1);

    if (!listener1) {
        qDebug() << "Error setting up port forwarding for " << config.adb_remote_port << libssh2_session_last_error(session, NULL, NULL, 0);
        libssh2_session_disconnect(session, "Normal Shutdown, Thank you for playing");
        libssh2_session_free(session);
        return;
    }



    listener2 = libssh2_channel_forward_listen_ex(session, "127.0.0.1", 55590, NULL, 1);

    if (!listener2) {
        qDebug() << "Error setting up port forwarding for " << config.adb_remote_port << libssh2_session_last_error(session, NULL, NULL, 0);
        libssh2_session_disconnect(session, "Normal Shutdown, Thank you for playing");
        libssh2_session_free(session);
        return;
    }

    qDebug() << "Port forwarding setup successfully port=" << config.adb_remote_port;
    libssh2_session_set_blocking(session, 0);
    while (true) {
//        logger->log("libssh2_channel_forward_accept +++");
        LIBSSH2_CHANNEL *channel = libssh2_channel_forward_accept(listener1);
//        logger->log("libssh2_channel_forward_accept ---");
        if (channel) {
            handleForwardedConnection(channel);
        }else{
            handleChannel2(listener2);
            QThread::msleep(100);
        }
    }



}
void SSHClient::proccessData(QString data)
{
    logger->log("proccessData data==" + data);
    QStringList list = data.split("&");
    if (list.size() > 1)
    {
        logger->log("proccessData arg0==" + list[0]);
        if (QString::compare(list[0], "path") == 0)
        {
            logger->log("proccessData arg1==" + list[1]);
            if (DirExist(list[1]))
            {
                logger->log("DirExist true");
                QString strPicPath = list[1];
                strPicPath.replace("/", "\\");
                QProcess process;
                process.startDetached("explorer", QStringList() << QString("/root,") << QString("%1").arg(strPicPath));
            }
            else if (FileExist(list[1]))
            {
                QString strPicPath = list[1];
                strPicPath.replace("/", "\\");
                QProcess process;
                process.startDetached("explorer", QStringList() << QString("/select,") << QString("%1").arg(strPicPath));
            }
        }
        else if (QString::compare(list[0], "oa") == 0)
        {
            QString strPicPath = list[1];
            strPicPath.replace("/", "\\");
            QProcess process;
            process.startDetached("D:\\androidstudio\\bin\\studio64.exe", QStringList() << QString("%1").arg(strPicPath));
        }
        else if (QString::compare(list[0], "ov") == 0)
        {/*
            logger->log("proccessData vs==");
            if (!vscode_ip.isEmpty())
            {
                logger->log("proccessData isEmpty==");
                for (int i = 0; i < clientSocket.length(); ++i)
                {
                    QString IP_Port = tr("[%1:%2]:").arg(clientSocket[i]->peerAddress().toString().mid(7)).arg(clientSocket[i]->peerPort());
                    if (QString::compare(IP_Port, vscode_ip) == 0)
                    {
                        QByteArray ba = list[1].toUtf8();
                        clientSocket[i]->write(ba);
                    }
                }
            }*/
        }
        else if (QString::compare(list[0], "ob") == 0)
        {
            logger->log("proccessData list[1]==" + list[1] + " " + list[2]);
            QString strPicPath = list[1];
            strPicPath.replace("/", "\\");
            QString strPicPath2 = list[2];
            strPicPath2.replace("/", "\\");
            QProcess process;
            logger->log("proccessData strPicPath==" + strPicPath + "  strPicPath2==" + strPicPath2);
            process.startDetached("D:\\install\\Beyond Compare\\Beyond Compare\\Beyond Compare\\BCompare.exe", QStringList() << QString("%1").arg(strPicPath) << QString("%2").arg(strPicPath2));
        }
    }
}

void SSHClient::handleChannel2(LIBSSH2_LISTENER *listener2)
{
    LIBSSH2_CHANNEL *channel2 = libssh2_channel_forward_accept(listener2);
    if (channel2) {
        libssh2_channel_set_blocking(channel2, 1);
        char buffer[2 * 1024];
        ssize_t n = libssh2_channel_read(channel2, buffer, sizeof(buffer));
        if (n > 0) {
            logger->log("libssh2_channel_forward_accepthhhh  =" + QString::fromUtf8(buffer, n));
            proccessData(QString::fromUtf8(buffer, n));
        }
        libssh2_channel_set_blocking(channel2, 0);
        libssh2_channel_close(channel2);
        libssh2_channel_free(channel2);
    }
}


void SSHClient::handleForwardedConnection(LIBSSH2_CHANNEL *channel)
{
    QTcpSocket *localSocket = new QTcpSocket();
    localSocket->connectToHost(config.cmd_local_host, config.cmd_local_port);
    if (!localSocket->waitForConnected(3000)) {
        logger->log("Failed to connect to local port");
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
        while (true) {
            char buffer[1 * 1024];
            ssize_t n = libssh2_channel_read(channel, buffer, sizeof(buffer));
            if (n > 0) {
                qint64 written = localSocket->write(buffer, n);
                if (written == -1) {
                    logger->log("Failed to write data to local socket");
                    break;
                }
            } else {
                handleChannel2(listener2);
                QThread::msleep(3);
                break;
            }
        }

        if (localSocket->waitForReadyRead(10)) {
            QByteArray localData = localSocket->readAll();
            if (!localData.isEmpty()) {
                qint64 bytesRemaining = localData.size();
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
            }
        }
    }

    libssh2_channel_close(channel);
    libssh2_channel_free(channel);
    localSocket->close();
    delete localSocket;
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



