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

}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::CreatTrayMenu()
{
    getDateAction = new QAction("time",this);
    getMacAction = new QAction("mac",this);
    getSerialAction = new QAction("serial",this);
    w2lAction = new QAction("w2l",this);
    quitAction = new QAction("Quit(&Q)",this);

    this->connect(getDateAction,SIGNAL(triggered()),this,SLOT(get_datetime_action()));
    this->connect(getMacAction,SIGNAL(triggered()),this,SLOT(get_mac_action()));
    this->connect(getSerialAction,SIGNAL(triggered()),this,SLOT(get_serial_action()));
    this->connect(w2lAction,SIGNAL(triggered()),this,SLOT(get_w2l_action()));
    this->connect(quitAction,SIGNAL(triggered()),qApp,SLOT(quit()));

    myMenu = new QMenu((QWidget*)QApplication::desktop());
  //  myMenu->setStyleSheet("QMenu::item{ padding:5px;}");
    myMenu->addAction(getDateAction);
    myMenu->addAction(getMacAction);
    myMenu->addAction(getSerialAction);
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

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason)
    {
    case QSystemTrayIcon::Trigger:

    case QSystemTrayIcon::DoubleClick:
        showNormal();
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


void MainWindow::get_mac_action()
{
      QString originalText  =  tool->getfromClip().trimmed();
      qDebug()<<"originalText.length()=="<<originalText.length();
      if(originalText.length() == 32){
          QString cmd = tool->TrasAmlMac(originalText);
          tool->setClip(cmd);
          myTrayIcon->showMessage("tips","已经获取到Mac",QSystemTrayIcon::Information,500);
      }
}

void MainWindow::get_datetime_action()
{
    QString time = tool->getDatetime();
    tool->setClip(time);
}

void MainWindow::get_serial_action()
{
    tool->setClip("005003000018146000010019F0FFFE74");
}

void MainWindow::get_w2l_action()
{
    QString originalText  =  tool->getfromClip().trimmed();            //获取剪贴板上文本信息
    QString path = QDir::fromNativeSeparators(originalText);
    tool->setClip(path);
}

void MainWindow::on_pushButton_clicked()
{
    Dialog d;
    d.show();
}
