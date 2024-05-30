#ifndef LOGGER_H
#define LOGGER_H

#include <QFile>
#include <QTextStream>
#include <QString>

class Logger
{
public:
    Logger();
    ~Logger();
    void log(const QString &message);

private:
    QFile logFile;
};

#endif // LOGGER_H
