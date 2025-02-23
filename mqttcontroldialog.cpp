#include "mqttcontroldialog.h"

MqttControlDialog::MqttControlDialog(QWidget *parent)
    : QDialog(parent), logTextEdit(new QTextEdit(this)), clearButton(new QPushButton("Clear", this)),
      connectButton(new QPushButton("Connect", this)),
       connected(false)
{
    // 设置 UI 布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 添加日志显示区域
    logTextEdit->setReadOnly(true);
    mainLayout->addWidget(logTextEdit, 1);

    // 添加清除按钮
    mainLayout->addWidget(clearButton);


    // 添加连接按钮
    mainLayout->addWidget(connectButton);



    // 设置窗口标题和大小
    setWindowTitle("MQTT Control");
    resize(600, 500);



    // 连接按钮信号到槽函数
    connect(clearButton, &QPushButton::clicked, this, &MqttControlDialog::clearLog);
    connect(connectButton, &QPushButton::clicked, this, &MqttControlDialog::onConnectButtonClicked);
}
void MqttControlDialog::appendLog(const QString &log)
{
    logTextEdit->append(log);
    QStringList logLines = logTextEdit->toPlainText().split("\n");
    if (logLines.size() > 1000) {
        clearLog();
    }
}

void MqttControlDialog::clearLog()
{
    logTextEdit->clear();
}

void MqttControlDialog::setButtonText(const QString &title)
{
    clearButton->setText(title);
}

void MqttControlDialog::closeEvent(QCloseEvent *event)
{
    event->ignore();  // 忽略关闭事件
    this->hide();     // 仅隐藏对话框
}


bool MqttControlDialog::isConnected() const
{
    return connected;
}

void MqttControlDialog::onConnectButtonClicked()
{
    if (connected) {
        emit disconnectClicked();
        connectButton->setText("Connect");
        connected = false;
    } else {
        emit connectClicked();
        connectButton->setText("Disconnect");
        connected = true;
    }
}

void MqttControlDialog::updateConnectState(bool connected)
{
    if(connected){
        connectButton->setText("Disconnect");
        connected = true;
    }else{
        connectButton->setText("Connect");
        connected = false;
    }
}


