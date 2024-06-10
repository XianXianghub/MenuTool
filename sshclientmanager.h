#ifndef SSH_CLIENT_MANAGER_H
#define SSH_CLIENT_MANAGER_H

#include <QString>
#include <QThread>
#include <QMap>
#include "SSHClient.h"
#include "sshconfig.h"

class SSHClientManager : public QObject {
    Q_OBJECT

public:
    SSHClientManager(Logger *logger) : logger(logger) {}

    void startSSHClient(const SshConfig &config);
    void restartAllSSHClients();
    void restartSSHClientWithConfig(const QString &name);
    void loadConfigsAndStartClients(const QString &configFilePath);
signals:
    void forwardSSHData(const QString &data);
private:
    QMap<QString, QPair<SSHClient*, QThread*>> clients;
    Logger *logger;
    SSHConfigParser parser;
    QString configFilePath;

    SshConfig getConfigByName(const QString &name) const;
};

#endif // SSH_CLIENT_MANAGER_H




