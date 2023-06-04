#ifndef XMLUTILS_H
#define XMLUTILS_H

#include <QtXml>
#include <QStringList>
#include <QHash>
#include "constlist.h"

#include <QString>
class XmlUtils
{
public:
    XmlUtils(QString path);
    ~XmlUtils();
    void WriteXml();
    QString ReadXml(QString key);
    void AddXml();
    void RemoveXml();
    void UpdateXml(QString key, QString val);
    QHash<QString, QString> getConfigMap();
    void InitData();
private:
 QString path;
 QHash<QString, QString> map;
};

#endif // XMLUTILS_H
