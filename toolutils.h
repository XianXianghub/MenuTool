#ifndef TOOLUTILS_H
#define TOOLUTILS_H

#endif // TOOLUTILS_H

#include <QString>
#include <QDebug>
#include <QDate>
#include <QDateTime>
#include <QClipboard>


#define OPENDIR 0;
#define TRANS_MSG_TO_VS 1;


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
