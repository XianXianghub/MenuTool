#include "ClientConfig.h"

ClientConfig::ClientConfig()
{
}

ConfigData ClientConfig::parse(const QString &filePath)
{
    ConfigData config;

    // 打开文件
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file:" << filePath;
        return config; // 如果文件打开失败，返回空的 ConfigData
    }

    // 读取文件内容
    QDomDocument doc;
    if (!doc.setContent(&file)) {
        qWarning() << "Failed to parse XML content from file:" << filePath;
        file.close();
        return config; // 如果解析失败，返回空的 ConfigData
    }
    file.close();

    // 获取根元素
    QDomElement root = doc.documentElement();
    return parseXml(root); // 解析 XML 并返回 ConfigData
}

ConfigData ClientConfig::parseXml(const QDomElement &root)
{
    ConfigData config;

    // 遍历子节点
    QDomNodeList nodes = root.childNodes();
    for (int i = 0; i < nodes.count(); ++i) {
        QDomNode node = nodes.at(i);
        if (node.isElement()) {
            QDomElement element = node.toElement();
            if (element.tagName() == "mqtt_host") {
                config.mqttHost = element.text();
            } else if (element.tagName() == "mqtt_port") {
                config.mqttPort = element.text().toInt();
            } else if (element.tagName() == "mqtt_username") {
                config.mqttUsername = element.text();
            } else if (element.tagName() == "mqtt_password") {
                config.mqttPassword = element.text();
            } else if (element.tagName() == "all") {
                config.all = (element.text() == "true");
            } else if (element.tagName() == "project") {
                config.project = element.text();
            }else if (element.tagName() == "isShowFailed") {
                config.isShowFailed = (element.text() == "true");
            }
        }
    }

    return config;
}
