#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <QObject>
#include <QtMqtt/qmqttclient.h>
#include <QDateTime>
#include "ClientConfig.h"


class MqttClient : public QObject
{
    Q_OBJECT

public:
    explicit MqttClient(QObject *parent = nullptr);

    void connectToBroker();
    ConfigData getConfigData();

    void disconnectFromBroker();
    void publishMessage(const QString &topic, const QString &message);
    void subscribeToTopic(const QString &topic);
    ConfigData  mConfigData;

signals:
    void logMessage(const QString &message); // 日志消息信号
    void connectionStateChanged(bool connected); // 连接状态变化信号

public slots:
    void updateLogStateChange();
    void onMessageReceived(const QByteArray &message, const QMqttTopicName &topic);
    void onPingResponseReceived();
    void connectClicked();
    void disconnectClicked();

private:
    QMqttClient *m_client;
};

#endif // MQTTCLIENT_H
