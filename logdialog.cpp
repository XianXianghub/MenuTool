#include "logdialog.h"
#include <QCloseEvent>  // 添加这一行
#include <QDebug>  // 添加这一行

LogDialog::LogDialog()
    : logTextEdit(new QTextEdit(this)), clearButton(new QPushButton("Clear", this))
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    logTextEdit->setReadOnly(true);
    layout->addWidget(logTextEdit);
    layout->addWidget(clearButton);
    setLayout(layout);
    setWindowTitle("Log Viewer");
    resize(600, 500);

    connect(clearButton, &QPushButton::clicked, this, &LogDialog::clearLog);
}

void LogDialog::appendLog(const QString &log)
{
    logTextEdit->append(log);
    QStringList logLines = logTextEdit->toPlainText().split("\n");
    if (logLines.size() > 1000) {
        clearLog();
    }
}

void LogDialog::clearLog()
{
    logTextEdit->clear();
}

void LogDialog::setButtonText(const QString &title)
{
    clearButton->setText(title);
}

void LogDialog::closeEvent(QCloseEvent *event)
{
    event->ignore();  // 忽略关闭事件
    this->hide();     // 仅隐藏对话框
}
