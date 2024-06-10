#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <QDir>
#include "toolutils.h"
#include <QDebug>
#include "dialog.h"
#include "qxtglobalshortcut.h"
#include <QMessageBox>
#include <QScreen>
#include "constlist.h"
#include "logger.h"
#include "sshclient.h"
#include <QTcpserver>
#include <QTcpSocket>
#include <QNetworkInterface>
#include <QString>
#include <QHash>
#include <QProcess>
#include "SshTunnelConfigDialog.h"
#include "sshclientmanager.h"
#include "sshconfig.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void CreatTrayMenu();
    void CreatTrayIcon();
    void proccessData(QString data);

    QSystemTrayIcon *myTrayIcon;

    QMenu *myMenu;

    QAction *getDateAction;
    QAction *getCommentAction;
    QAction *getMacAction;
    QAction *openSshConfigAction;

    QAction *getSerialAction;
    QAction *quitAction;
    QAction *w2lAction;
    ToolUtils *tool;

protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void get_datetime_action();
    void get_CommentAction();
    void get_w2l_action();
    void quit_action();
    void openSshConfigPage();

    void hotkey_press_action();

    void on_pushButton_clicked();

    void server_New_Connect();

    void Read_Data();

    void disConnected();
    void handleForwardedSSHData(const QString &data);

private:
    Ui::MainWindow *ui;
    bool isQuit = false;
    QTcpSocket *mQsocket;
    QTcpServer *server;
    Logger *logger;
    QTcpSocket *SShsocket;
    QHash<QString , QTcpSocket*> qhash;
     SSHClientManager *mSSHClientManager;

    QList<QTcpSocket*> clientSocket;
    QString xshell_ip = "";
    QString vscode_ip = "";

};

#endif // MAINWINDOW_H
