#ifndef LOGDIALOG_H
#define LOGDIALOG_H

#include <QDialog>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QCloseEvent>

class LogDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LogDialog();
     void closeEvent(QCloseEvent *event);

public slots:
    void appendLog(const QString &log);
    void clearLog();
    void setButtonText(const QString &title);



private:
    QTextEdit *logTextEdit;
    QPushButton *clearButton;
};

#endif // LOGDIALOG_H
