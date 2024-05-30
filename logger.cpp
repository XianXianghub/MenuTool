#include "logger.h"
#include <QDateTime>
#include <QDir>
#include <QDebug>
#include <QCoreApplication>


Logger::Logger()
{
    QString logDirPath = QCoreApplication::applicationDirPath() + "/logs";
    qDebug() << "Log directory path:" << logDirPath;
    QDir logDir(logDirPath);

    if (!logDir.exists())
    {
        bool dirCreated = logDir.mkpath(logDirPath);
        qDebug() << "Log directory created:" << dirCreated;
        if (!dirCreated)
        {
            qDebug() << "Failed to create log directory:" << logDirPath;
            return;
        }
    }

    // 创建日志文件名
    QString logFileName = logDirPath + "/log_" + QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss") + ".txt";
    logFile.setFileName(logFileName);
    qDebug() << "Log file name:" << logFileName;

    // 尝试打开日志文件
    bool fileOpened = logFile.open(QIODevice::WriteOnly | QIODevice::Text);
    qDebug() << "Log file opened:" << fileOpened;
    if (!fileOpened)
    {
        qDebug() << "Failed to open log file:" << logFileName;
    }
}

Logger::~Logger()
{
    if (logFile.isOpen())
    {
        logFile.close();
    }
}

void Logger::log(const QString &message)
{
    if (!logFile.isOpen())
    {
        qDebug() << "Log file is not open!";
        return;
    }

    QTextStream logStream(&logFile);
    QString logMessage = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz") + " - " + message + "\n";
    logStream << logMessage;
    qDebug() << logMessage;
    if (logStream.status() != QTextStream::Ok)
    {
        qDebug() << "Failed to write to log file:" << logStream.status();
    }

    logFile.flush();  // Ensure the buffer is written to the file
}
