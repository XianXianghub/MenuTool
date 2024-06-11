// logger.cpp
#include "logger.h"
#include <QDateTime>
#include <QDir>
#include <QDebug>
#include <QCoreApplication>
#include <QMutex>
#include <QThread>

static QMutex logMutex;

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

    QString logFileName = logDirPath + "/log_" + QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss") + ".txt";
    logFile.setFileName(logFileName);
    qDebug() << "Log file name:" << logFileName;

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
    QMutexLocker locker(&logMutex);

    if (isWiteFile && !logFile.isOpen())
    {
        qDebug() << "Log file is not open!";
        return;
    }


    QString logMessage = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz") + " ["+QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))+ "] " + message;
    qDebug() << logMessage;
    emit newLogMessage(logMessage);  // Emit the signal with the new log message
    if(isWiteFile){
        QTextStream logStream(&logFile);
        logStream << logMessage << "\n";
        if (logStream.status() != QTextStream::Ok)
        {
            qDebug() << "Failed to write to log file:" << logStream.status();
        }
        logFile.flush();
    }
}
