#include "mqttclient.h"
#include <QMessageBox>
#include <QThread>
#include <QCoreApplication>

MqttClient::MqttClient(QObject *parent)
    : QObject(parent)
{
    m_client = new QMqttClient(this);

    connect(m_client, &QMqttClient::stateChanged, this, &MqttClient::updateLogStateChange);
    connect(m_client, &QMqttClient::messageReceived, this, &MqttClient::onMessageReceived);
    connect(m_client, &QMqttClient::pingResponseReceived, this, &MqttClient::onPingResponseReceived);
}

void MqttClient::connectToBroker()
{

    QString filePath = QCoreApplication::applicationDirPath() + "/client.xml";

    ClientConfig clientConfig;
    mConfigData = clientConfig.parse(filePath);
    m_client->setHostname(mConfigData.mqttHost);
    m_client->setPort(mConfigData.mqttPort);
    m_client->setUsername(mConfigData.mqttUsername);
    m_client->setPassword(mConfigData.mqttPassword);
    m_client->connectToHost();
}

ConfigData MqttClient::getConfigData()
{
    return mConfigData;
}

void MqttClient::disconnectFromBroker()
{
    m_client->disconnectFromHost();
}

void MqttClient::publishMessage(const QString &topic, const QString &message)
{
    if (m_client->publish(topic, message.toUtf8()) == -1) {
        emit logMessage(QDateTime::currentDateTime().toString("yyyy-MM-dd hh-mm-ss") + ": Error - Could not publish message");
    }
}

void MqttClient::subscribeToTopic(const QString &topic)
{
    auto subscription = m_client->subscribe(topic);
    if (!subscription) {
        emit logMessage(QDateTime::currentDateTime().toString("yyyy-MM-dd hh-mm-ss") + ": Error - Could not subscribe to topic");
    }
}

void MqttClient::updateLogStateChange()
{
    QString state;
    switch (m_client->state()) {
    case QMqttClient::Disconnected:
        state = "Disconnected";
        emit connectionStateChanged(false);
        break;
    case QMqttClient::Connecting:
        state = "Connecting";
        break;
    case QMqttClient::Connected:
        state = "Connected";
        emit connectionStateChanged(true);
        break;
    default:
        state = "Unknown";
        break;
    }
    emit logMessage(QDateTime::currentDateTime().toString("yyyy-MM-dd hh-mm-ss") + ": State Change - " + state);
}

void MqttClient::onMessageReceived(const QByteArray &message, const QMqttTopicName &topic)
{
//    emit logMessage( QDateTime::currentDateTime().toString("yyyy-MM-dd hh-mm-ss") + ": Received Topic: " + topic.name() + " Message: " + message);
    if(QString::compare("hello world", message) == 0) return;
    emit logMessage(message);
}

void MqttClient::onPingResponseReceived()
{
//    emit logMessage(QDateTime::currentDateTime().toString("yyyy-MM-dd hh-mm-ss") + ": Ping Response Received");
}

void MqttClient::connectClicked()
{
    connectToBroker();
}

void MqttClient::disconnectClicked()
{
    m_client->disconnectFromHost();

}
