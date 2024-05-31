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

#include <QTcpserver>
#include <QTcpSocket>
#include <QNetworkInterface>
#include <QString>
#include <QHash>
#include "qmqtt.h"
using namespace QMQTT;

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
    QAction *getMacAction;
    QAction *getSerialAction;
    QAction *quitAction;
    QAction *w2lAction;
    ToolUtils *tool;
protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void get_datetime_action();
    void get_w2l_action();
    void quit_action();
    void hotkey_press_action();

    void on_pushButton_clicked();

    void server_New_Connect();

    void Read_Data();

    void disConnected();


    void doConnected();  //MQTT 连接成功
       void doDisconnected();//MQTT连接断开
       void doDataReceived(QMQTT::Message);//MQTT收到数据

private:
    Ui::MainWindow *ui;
    bool isQuit = false;
    QTcpSocket *socket;
     QTcpServer *server;
     QMQTT::Client *client; //MQTT客户端指针

    QHash<QString , QTcpSocket*> qhash;

    QList<QTcpSocket*> clientSocket;
    QString xshell_ip = "";
    QString vscode_ip = "";

};

#endif // MAINWINDOW_H
