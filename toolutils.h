#ifndef TOOLUTILS_H
#define TOOLUTILS_H

#endif // TOOLUTILS_H

#include <QString>
#include <QDebug>
#include <QDate>
#include <QDateTime>
#include <QClipboard>

class ToolUtils{

public:
    ToolUtils(QClipboard *clip);
    QString TrasAmlMac(QString serial);
    QString getDatetime();
    QString getfromClip();
    void setClip(QString str);
private:
QClipboard *clipboard ;
};
