#include "toolutils.h"


ToolUtils::ToolUtils(QClipboard *clip)
{
    this->clipboard = clip;
}

QString ToolUtils::TrasAmlMac(QString serial)
{
    if(serial.isEmpty()) return "";
    QString mac = serial.right(12);
    QString section1 = "echo 1 > /sys/class/unifykeys/attach;echo \"usid\" > /sys/class/unifykeys/name;echo \"";
    QString section2 = "\" > /sys/class/unifykeys/write;echo \"mac\" > /sys/class/unifykeys/name;echo \"";
    QString section3 = "\" > /sys/class/unifykeys/write";
    for(int i=2; i < 17 ;i=i+3 ){
        mac = mac.insert(i,":");
        qDebug()<<"mac=="<<mac<<"i="<<i;
    }
     qDebug()<<"mac111=="<<mac;
     QString cmd = section1+serial+section2+mac+section3;
     return cmd;
}


QString ToolUtils::getDatetime()
{
    QString strBuffer;
    QDateTime time;
    time = QDateTime::currentDateTime();
    strBuffer = time.toString("yyyy-MM-dd_hh-mm-ss");
    qDebug()<<"strBuffer=="<<strBuffer;
    return strBuffer;
}

QString ToolUtils::getfromClip()
{
    QString originalText  = clipboard->text().trimmed();            //获取剪贴板上文本信息
    return originalText;
}

void ToolUtils::setClip(QString str)
{
    clipboard->setText(str);
}


