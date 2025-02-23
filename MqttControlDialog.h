#ifndef MQTTCONTROLDIALOG_H
#define MQTTCONTROLDIALOG_H

#include <QDialog>
#include <QTextEdit>
#include <QPushButton>
#include <QLineEdit>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCloseEvent>
#include <QDebug>

class MqttControlDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MqttControlDialog(QWidget *parent = nullptr);

    void appendLog(const QString &log);
    void clearLog();
    void setButtonText(const QString &title);

    QString getHost() const;
    int getPort() const;
    bool isConnected() const;
    void updateConnectState(bool connected);

signals:
    void connectClicked();
    void disconnectClicked();
    void publishClicked(const QString &topic, const QString &message);
    void subscribeClicked(const QString &topic);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onConnectButtonClicked();
private:
    QTextEdit *logTextEdit;
    QPushButton *clearButton;
    QPushButton *connectButton;
    bool connected;
};

#endif // MQTTCONTROLDIALOG_H
