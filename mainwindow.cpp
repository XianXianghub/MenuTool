#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
     ui->setupUi(this);
     this->setWindowIcon(QIcon("./pkq.ico"));
     CreatTrayIcon();
     QClipboard *clipboard = QApplication::clipboard();   //获取系统剪贴板指针
     tool= new ToolUtils(clipboard);
     QxtGlobalShortcut *shortcut = new QxtGlobalShortcut(this);
     if(shortcut->setShortcut(QKeySequence("F1")))
     {

        connect(shortcut, SIGNAL(activated()), this, SLOT(hotkey_press_action()));

     }
     else
     {
         qDebug()<<"快捷键已占用";
         QMessageBox::information(NULL, "Title", "快捷键已占用", QMessageBox::Yes, QMessageBox::Yes);
     }
     socket = new QTcpSocket();
     server = new QTcpServer();
     connect(server, &QTcpServer::newConnection, this, &MainWindow::server_New_Connect);
     server->listen(QHostAddress::LocalHost, 5055 );

//     QProcess process;
////               QString filePath = ui->filePathLE->text();
////               filePath.replace("/", "\\"); // 只能识别 "\"
//        QString cmd = QString("explorer.exe /select,\"%1\"").arg("D:\adb\adb.exe");
//        qDebug() << cmd;
//        process.startDetached(cmd);

//        QString strPicPath = "X:\\test\\linux2window";
//        strPicPath.replace("/", "\\");    //win32下替换斜杠
//        QProcess process;
//        process.startDetached("explorer", QStringList() << QString("/root,") << QString("%1").arg(strPicPath));




}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::CreatTrayMenu()
{
    getDateAction = new QAction("time",this);
    getCommentAction = new QAction("Comment",this);
    getMacAction = new QAction("mac",this);
    getSerialAction = new QAction("serial",this);
    w2lAction = new QAction("w2l",this);
    quitAction = new QAction("Quit(&Q)",this);

    this->connect(getDateAction,SIGNAL(triggered()),this,SLOT(get_datetime_action()));
    this->connect(w2lAction,SIGNAL(triggered()),this,SLOT(get_w2l_action()));
    this->connect(quitAction,SIGNAL(triggered()),this,SLOT(quit_action()));
     this->connect(getCommentAction,SIGNAL(triggered()),this,SLOT(get_CommentAction()));

    myMenu = new QMenu((QWidget*)QApplication::desktop());
  //  myMenu->setStyleSheet("QMenu::item{ padding:5px;}");
     myMenu->addAction(getCommentAction);
    myMenu->addAction(getDateAction);
//    myMenu->addAction(getMacAction);
//    myMenu->addAction(getSerialAction);
    myMenu->addAction(w2lAction);
    myMenu->addSeparator();     //加入一个分离符
    myMenu->addAction(quitAction);
    QFile qss("stylesheet.qss");
    if(qss.open(QFile::ReadOnly))
    {
       QString style = QLatin1String(qss.readAll());
       myMenu->setStyleSheet(style);
       qss.close();
    }
}

void MainWindow::CreatTrayIcon()
{
    CreatTrayMenu();

    if (!QSystemTrayIcon::isSystemTrayAvailable())      //判断系统是否支持系统托盘图标
    {
        return;
    }

    myTrayIcon = new QSystemTrayIcon(this);
    myTrayIcon->setIcon(QIcon(":/image/pkq.ico"));   //设置图标图片
    setWindowIcon(QIcon(":/image/pkq.ico"));  //把图片设置到窗口上
    myTrayIcon->setToolTip("V1.0");    //托盘时，鼠标放上去的提示信息
    myTrayIcon->showMessage("tips","SystemTray",QSystemTrayIcon::Information,500);
    myTrayIcon->setContextMenu(myMenu);     //设置托盘上下文菜单
    myTrayIcon->show();
    this->connect(myTrayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
}

void MainWindow::proccessData(QString data)
{
       qDebug()<<"proccessData data=="<<data;
    QStringList list = data.split("&");
    if(list.size() > 1){
      qDebug()<<"proccessData arg0=="<<list[0];
      if(QString::compare(list[0], "path") == 0){
        qDebug()<<"proccessData arg1=="<<list[1];
        if(DirExist(list[1])){
             qDebug()<<"DirExist true";
               QString strPicPath = list[1];
               strPicPath.replace("/", "\\");    //win32下替换斜杠
               QProcess process;
               process.startDetached("explorer", QStringList() << QString("/root,") << QString("%1").arg(strPicPath));
        }else if(FileExist(list[1])){
               QString strPicPath = list[1];
               strPicPath.replace("/", "\\");    //win32下替换斜杠
               QProcess process;
               process.startDetached("explorer", QStringList() << QString("/select,") << QString("%1").arg(strPicPath));
        }
      }else if(QString::compare(list[0], "oa") == 0){
          QString strPicPath = list[1];
          strPicPath.replace("/", "\\");    //win32下替换斜杠
          QProcess process;
          process.startDetached("D:\\androidstudio\\bin\\studio64.exe", QStringList() << QString("%1").arg(strPicPath));
      }else if(QString::compare(list[0], "ov") == 0){
           qDebug()<<"proccessData vs==";
          if(!vscode_ip.isEmpty()){
                 qDebug()<<"proccessData isEmpty==";
              for (int i = 0; i < clientSocket.length(); ++i) {
                  QString IP_Port = tr("[%1:%2]:").arg(clientSocket[i]->peerAddress().toString().mid(7)).arg(clientSocket[i]->peerPort());
                  if(QString::compare(IP_Port, vscode_ip) == 0){
                   QByteArray ba = list[1].toUtf8();
                         clientSocket[i]->write(ba);
                  }
          }

      }
    }else if(QString::compare(list[0], "ob") == 0){
         qDebug()<<"proccessData  list[1]=="<< list[1]<< list[2];
         QString strPicPath = list[1];
         strPicPath.replace("/", "\\");    //win32下替换斜杠
         QString strPicPath2 = list[2];
         strPicPath2.replace("/", "\\");    //win32下替换斜杠
         QProcess process;
         qDebug()<<"proccessData strPicPath=="<<strPicPath<<"  strPicPath2=="<<strPicPath2;

         process.startDetached("D:\\install\\Beyond Compare\\Beyond Compare\\Beyond Compare\\BCompare.exe", QStringList() << QString("%1").arg(strPicPath)<< QString("%2").arg(strPicPath2));
     }
   }

}


void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    isQuit = true;
    switch(reason)
    {
    case QSystemTrayIcon::Trigger:

    case QSystemTrayIcon::DoubleClick:
        break;
    case QSystemTrayIcon::MiddleClick:
        myTrayIcon->showMessage("tips","SystemTray",QSystemTrayIcon::Information,500);
        break;

    default:
        break;
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (myTrayIcon->isVisible())
    {
        myTrayIcon->showMessage("tips","SystemTray",QSystemTrayIcon::Information,500);
        hide();     //最小化
        event->ignore();
    }
    else
        event->accept();
}



void MainWindow::get_datetime_action()
{
    QString time = tool->getDatetime();
    tool->setClip(time);
}

void MainWindow::get_CommentAction()
{
    QString time = tool->getDatetime();
    QString str = "/* xiangsq "+time+" */";
    tool->setClip(str);
}



void MainWindow::get_w2l_action()
{
    QString originalText  =  tool->getfromClip().trimmed();            //获取剪贴板上文本信息
    QString path = QDir::fromNativeSeparators(originalText);
    tool->setClip(path);
}

void MainWindow::quit_action()
{
    if(isQuit)qApp->quit();
    else myMenu->hide();
}

void MainWindow::hotkey_press_action()
{
  isQuit = false;
  myMenu->exec(QCursor::pos());
}

void MainWindow::on_pushButton_clicked()
{
    qDebug()<<"on_pushButton_clicked";
}

void MainWindow::server_New_Connect()
{
    // 获取客户端连接
    socket = server->nextPendingConnection();
    clientSocket.append(socket);
    connect(socket, SIGNAL(readyRead()), this, SLOT(Read_Data()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disConnected()));
}

void MainWindow::Read_Data()
{
    // 由于readyRead信号并未提供SocketDecriptor，所以需要遍历所有客户端
    for (int i = 0; i < clientSocket.length(); ++i) {
        // 读取缓冲区数据
        QByteArray buffer = clientSocket[i]->readAll();
        if(buffer.isEmpty()) {
            continue;
        }

        static QString IP_Port, IP_Port_Pre;
        IP_Port = tr("[%1:%2]:").arg(clientSocket[i]->peerAddress().toString().mid(7)).arg(clientSocket[i]->peerPort());
        QString data = buffer;

        if(QString::compare(data, XSHELL_IP_INDEX) == 0){
            xshell_ip = IP_Port;
            qhash.insert(XSHELL_IP_INDEX, clientSocket[i]);
            clientSocket[i]->write("ok");
             qDebug()<<"xhslle=="<<buffer;
        }else if(QString::compare(data, VSCODE_IP_INDEX) == 0){
            vscode_ip = IP_Port;
            qhash.insert(VSCODE_IP_INDEX, clientSocket[i]);
            clientSocket[i]->write("ok");
        }else{
            proccessData(data);
        }

        qDebug()<<"buffer=="<<buffer;
        qDebug()<<"IP_Port=="<<IP_Port;
    }
}

void MainWindow::disConnected()
{
      qDebug()<<"disConnected==";
    // 遍历寻找断开连接的是哪一个客户端
       for(int i = 0; i < clientSocket.length(); ++i) {
           if(clientSocket[i]->state() == QAbstractSocket::UnconnectedState)
           {
               QString ipport = tr("[%1:%2]:").arg(clientSocket[i]->peerAddress().toString().mid(7)).arg(clientSocket[i]->peerPort());
               if(QString::compare(ipport, xshell_ip) == 0){
                    xshell_ip = "";
               }else if(QString::compare(ipport, vscode_ip) == 0){
                    vscode_ip="";
               }
               // 删除存储在clientSocket列表中的客户端信息
               clientSocket[i]->destroyed();
               clientSocket.removeAt(i);
           }
       }
}
