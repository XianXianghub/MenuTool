#ifndef CONFIGPARSER_H
#define CONFIGPARSER_H

#include <QString>
#include <QList>

// 配置数据结构
struct Config {
    QString name;
    QString ip;
    QString project;
};

class ConfigParser {
public:
    // 构造函数，传入文件路径
    explicit ConfigParser(const QString &filePath);

    // 解析配置文件，返回 Config 的列表
    QList<Config> parseConfigFile();

private:
    QString m_filePath; // 存储文件路径
};

#endif // CONFIGPARSER_H
