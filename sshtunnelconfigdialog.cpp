#include "SshTunnelConfigDialog.h"
#include "ui_SshTunnelConfigDialog.h"
#include <QDebug>

SshTunnelConfigDialog::SshTunnelConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SshTunnelConfigDialog),
    sshProcess(new QProcess(this))
{
    ui->setupUi(this);
    setupConnections();
}

SshTunnelConfigDialog::~SshTunnelConfigDialog()
{
    stopSshTunnel();
    delete ui;
}
void SshTunnelConfigDialog::closeEvent(QCloseEvent *event)
{
    // 忽略关闭事件
    event->ignore();
    // 隐藏对话框
    hide();
}
void SshTunnelConfigDialog::setupConnections()
{
    connect(ui->startButton, &QPushButton::clicked, this, &SshTunnelConfigDialog::startSshTunnel);
    connect(ui->restartButton, &QPushButton::clicked, this, &SshTunnelConfigDialog::restartSshTunnel);
    connect(sshProcess, &QProcess::readyReadStandardOutput, this, &SshTunnelConfigDialog::handleProcessOutput);
    connect(sshProcess, &QProcess::readyReadStandardError, this, &SshTunnelConfigDialog::handleProcessOutput);
//    connect(sshProcess, QOverload<QProcess::ProcessError>::of(&QProcess::errorOccurred), this, &SshTunnelConfigDialog::handleProcessError);
}

void SshTunnelConfigDialog::startSshTunnel()
{
    QString remoteHost = ui->remoteHostEdit->text();
    int remotePort = ui->remotePortEdit->text().toInt();
    int localPort = ui->localPortEdit->text().toInt();
    QString username = ui->usernameEdit->text();
    int sshPort = ui->sshPortEdit->text().toInt();
//    ssh -R :55527:localhost:9006 xiangsq@43.248.140.157 -p 16127

    remotePort = 55527;
localPort = 9006;
username = "xiangsq";
remoteHost = "43.248.140.157";
sshPort = 16127;

    QString command = QString("C:\\Windows\\System32\\OpenSSH\\ssh.exe  -o PasswordAuthentication=yes  -R %1:127.0.0.1:%2 %3@%4 -p %5")
                      .arg(remotePort)
                      .arg(localPort)
                      .arg(username)
                      .arg(remoteHost)
                      .arg(sshPort);


    QProcess process;
    process.startDetached(command);


    qDebug() << "Starting SSH tunnel with command:" << command;

    // Start the SSH process
//    sshProcess->start(command);
//    if (!sshProcess->waitForFinished())
//    {
//        qDebug() << "Failed to execute ssh command.";
//        qDebug() << "Error: " << sshProcess->errorString();
//    }

    // Enable the restart button after starting the process
    ui->restartButton->setEnabled(true);
}

void SshTunnelConfigDialog::restartSshTunnel()
{
    stopSshTunnel();
    startSshTunnel();
}

void SshTunnelConfigDialog::stopSshTunnel()
{
    if (sshProcess->state() != QProcess::NotRunning) {
        sshProcess->terminate();
        if (!sshProcess->waitForFinished(3000)) {
            sshProcess->kill();
        }
    }
}

void SshTunnelConfigDialog::handleProcessOutput()
{
    qDebug() << "handleProcessOutput";

    QByteArray output = sshProcess->readAllStandardOutput();
    QByteArray errorOutput = sshProcess->readAllStandardError();
    if (!output.isEmpty())
        qDebug() << "SSH Process Output:" << output;
    if (!errorOutput.isEmpty())
        qDebug() << "SSH Process Error Output:" << errorOutput;
}

void SshTunnelConfigDialog::handleProcessError(QProcess::ProcessError error)
{
    qDebug() << "SSH Process Error:" << error;
}
