#ifndef LOGGER_H
#define LOGGER_H

#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDir>
#include <QDebug>

class Logger
{
public:
    Logger();
    ~Logger();

    void log(const QString &message);

private:
    QFile *logFile;
    QTextStream *logStream;
    void createLogFile();
};

#endif // LOGGER_H
