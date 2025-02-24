#ifndef CLIENTCONFIG_H
#define CLIENTCONFIG_H

#include <QString>
#include <QDomDocument>
#include <QFile>
#include <qDebug>

// 定义结构体，用于存储解析后的配置
struct ConfigData {
    QString mqttHost;
    int mqttPort;
    QString mqttUsername;
    QString mqttPassword;
    bool all;
    QString project;
    bool isShowFailed;
};

class ClientConfig
{
public:
    ClientConfig();

    // 解析 XML 文件并返回 ConfigData 结构体
    ConfigData parse(const QString &filePath);

private:
    // 辅助函数，用于解析 XML
    ConfigData parseXml(const QDomElement &root);
};

#endif // CLIENTCONFIG_H
