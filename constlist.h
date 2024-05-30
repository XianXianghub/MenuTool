#ifndef CONSTLIST
#define CONSTLIST
#include <QString>
#include <QFileInfo>
#include <QFile>
#include <qDebug>
#include <QIODevice>
#include <QDir>

static QString XSHELL_IP_INDEX         = "xshell";
static QString VSCODE_IP_INDEX         = "vscode";

static bool FileExist(QString fullFileName)
{
    QFile file(fullFileName);

    if(file.exists())
    {
        return true;
    }
    else
    {
        return false;
    }
}

static bool DirExist(QString fullPath)
{
    QDir dir(fullPath);
    if(dir.exists())
    {
        //存在当前文件夹
        return true;
    }
    else
    {
        return false;
    }
}

#endif // CONSTLIST
