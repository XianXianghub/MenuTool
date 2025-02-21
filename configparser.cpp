#include "ConfigParser.h"
#include <QFile>
#include <QDomDocument>
#include <QDebug>

ConfigParser::ConfigParser(const QString &filePath)
    : m_filePath(filePath)
{
}

QList<Config> ConfigParser::parseConfigFile() {
    QList<Config> configList;

    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open file:" << m_filePath;
        return configList;
    }

    QDomDocument doc;
    if (!doc.setContent(&file)) {
        qWarning() << "Failed to parse XML content!";
        file.close();
        return configList;
    }

    file.close();

    // 获取 <clientconfig> 节点
    QDomElement root = doc.documentElement();
    if (root.tagName() != "clientconfig") {
        qWarning() << "Root element is not <clientconfig>";
        return configList;
    }

    // 遍历所有 <config> 节点
    QDomNodeList configNodes = root.elementsByTagName("config");
    for (int i = 0; i < configNodes.count(); ++i) {
        QDomNode node = configNodes.at(i);
        if (node.isElement()) {
            QDomElement configElement = node.toElement();

            Config config;
            // 获取 <name>、<ip> 和 <project> 的内容
            config.name = configElement.firstChildElement("name").text();
            config.ip = configElement.firstChildElement("ip").text();
            config.project = configElement.firstChildElement("project").text();

            // 将配置添加到列表中
            configList.append(config);
        }
    }

    return configList;
}
