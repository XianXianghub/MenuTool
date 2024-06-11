// logger.h
#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QFile>
#include <QTextStream>

class Logger : public QObject
{
    Q_OBJECT

public:
    Logger();
    ~Logger();
    void log(const QString &message);

signals:
    void newLogMessage(const QString &message);

private:
    QFile logFile;
    bool isWiteFile = false;
};

#endif // LOGGER_H
