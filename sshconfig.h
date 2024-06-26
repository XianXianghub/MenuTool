#ifndef SSH_CONFIG_H
#define SSH_CONFIG_H

#include <QString>
#include <QDomDocument>
#include <QFile>
#include <QList>
#include <QDebug>
#include <QMetaType>

class SshConfig {
public:
    QString name;
    QString ssh_host;
    int ssh_port;
    QString ssh_user;
    QString ssh_pwd;
    QString adb_remote_host;
    int adb_remote_port;
    QString cmd_remote_host;
    int cmd_remote_port;
    QString adb_local_host; // 添加adb_local_host
    int adb_local_port; // 添加adb_local_port

    SshConfig() : ssh_port(0), adb_remote_port(0), cmd_remote_port(0), adb_local_port(0) {} // 初始化adb_local_port
};

Q_DECLARE_METATYPE(SshConfig)

class SSHConfigParser {
public:
    QList<SshConfig> configs;

    bool loadFromFile(const QString &filePath) {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "Cannot open file for reading:" << filePath;
            return false;
        }

        QDomDocument doc;
        if (!doc.setContent(&file)) {
            qDebug() << "Failed to parse XML content.";
            file.close();
            return false;
        }
        file.close();

        QDomElement root = doc.documentElement();
        QDomNodeList configList = root.elementsByTagName("config");

        for (int i = 0; i < configList.count(); ++i) {
            QDomNode configNode = configList.at(i);
            if (configNode.isElement()) {
                QDomElement configElement = configNode.toElement();
                SshConfig config;
                config.name = configElement.elementsByTagName("name").at(0).toElement().text();
                config.ssh_host = configElement.elementsByTagName("ssh_host").at(0).toElement().text();
                config.ssh_port = configElement.elementsByTagName("ssh_port").at(0).toElement().text().toInt();
                config.ssh_user = configElement.elementsByTagName("ssh_user").at(0).toElement().text();
                config.ssh_pwd = configElement.elementsByTagName("ssh_pwd").at(0).toElement().text();
                config.adb_remote_host = configElement.elementsByTagName("adb_remote_host").at(0).toElement().text();
                config.adb_remote_port = configElement.elementsByTagName("adb_remote_port").at(0).toElement().text().toInt();
                config.cmd_remote_host = configElement.elementsByTagName("cmd_remote_host").at(0).toElement().text();
                config.cmd_remote_port = configElement.elementsByTagName("cmd_remote_port").at(0).toElement().text().toInt();
                config.adb_local_host = configElement.elementsByTagName("adb_local_host").at(0).toElement().text(); // 解析adb_local_host
                config.adb_local_port = configElement.elementsByTagName("adb_local_port").at(0).toElement().text().toInt(); // 解析adb_local_port

                configs.append(config);
            }
        }
        return true;
    }
};
#endif // SSH_CONFIG_H
