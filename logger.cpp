#include "Logger.h"
#include <QCoreApplication>

Logger::Logger()
{
    createLogFile();
}

Logger::~Logger()
{
    if (logStream) {
        delete logStream;
    }
    if (logFile) {
        logFile->close();
        delete logFile;
    }
}

void Logger::createLogFile()
{
    QString logDirPath = QCoreApplication::applicationDirPath() + "/logs";
    QDir logDir(logDirPath);
    if (!logDir.exists()) {
        logDir.mkpath(logDirPath);
    }

    QString logFileName = logDirPath + "/log_" + QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss") + ".txt";
    logFile = new QFile(logFileName);
    if (logFile->open(QIODevice::WriteOnly | QIODevice::Text)) {
        logStream = new QTextStream(logFile);
    }
}

void Logger::log(const QString &message)
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString logMessage = QString("[%1] %2").arg(timestamp).arg(message);
    if (logStream) {
        *logStream << logMessage << "\n";
        logStream->flush();
    }
    qDebug() << logMessage;
}
