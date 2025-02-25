#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <QDir>
#include "toolutils.h"
#include "qxtglobalshortcut.h"
#include "logger.h"
#include "sshclient.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QNetworkInterface>
#include <QHash>
#include <QProcess>
#include "sshclientmanager.h"
#include "logdialog.h"
#include "messagedialog.h"
#include <QDateTime>
#include "trayiconhandler.h"

#include "mqttclient.h"
#include "MqttControlDialog.h"


#define IS_MASTER  0

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
     Ui::MainWindow *ui;
    void createTrayMenu();
    void createTrayIcon();

    QSystemTrayIcon *myTrayIcon;
    QMenu *myMenu;
    QAction *getDateAction;
    QAction *getCommentAction;
    QAction *getMacAction;
    QAction *restartSshAction;
    QAction *openLogAction;
    QAction *getSerialAction;
    QAction *quitAction;
    QAction *w2lAction;
    QAction *compilationAction;

    ToolUtils *tool;
    Logger *logger;
    SSHClientManager *mSSHClientManager;
    LogDialog *logDialog;
    MqttControlDialog *mqttControlDialog;
    bool isQuit = false;
    TrayIconHandler *trayHandler;
    MqttClient *m_mqttClient;

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void getDateTime();
    void getComment();
    void getW2L();
    void exuteString();
    void quit();
    void restartSsh();
    void openLog();
    void hotkeyPress();
    void handleForwardedSSHData(const QString &data);
    void handleMqttData(const QString &data);



};

#endif // MAINWINDOW_H
