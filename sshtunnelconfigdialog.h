#ifndef SSHTUNNELCONFIGDIALOG_H
#define SSHTUNNELCONFIGDIALOG_H

#include <QDialog>
#include <QProcess>
#include <QCloseEvent>
namespace Ui {
class SshTunnelConfigDialog;
}

class SshTunnelConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SshTunnelConfigDialog(QWidget *parent);
    ~SshTunnelConfigDialog();

private slots:
    void startSshTunnel();
    void restartSshTunnel();
    void stopSshTunnel();
    void handleProcessOutput();
    void handleProcessError(QProcess::ProcessError error);
protected:
    void closeEvent(QCloseEvent *event) override;
private:
    Ui::SshTunnelConfigDialog *ui;
    QProcess *sshProcess;
    void setupConnections();
};

#endif // SSHTUNNELCONFIGDIALOG_H
