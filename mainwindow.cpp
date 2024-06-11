#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <QCoreApplication>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon("./pkq.ico"));
    CreatTrayIcon();
    QClipboard *clipboard = QApplication::clipboard();   //获取系统剪贴板指针
    tool= new ToolUtils(clipboard);
    logger = new Logger();
    logDialog = new LogDialog();
    QxtGlobalShortcut *shortcut = new QxtGlobalShortcut(this);
    if(shortcut->setShortcut(QKeySequence("F1")))
    {
        connect(shortcut, SIGNAL(activated()), this, SLOT(hotkey_press_action()));
    }
    else
    {
        logger->log("快捷键已占用");
        QMessageBox::information(NULL, "Title", "1111快捷键已占用", QMessageBox::Yes, QMessageBox::Yes);
    }

   connect(logger, &Logger::newLogMessage, logDialog, &LogDialog::appendLog);

   mSSHClientManager = new SSHClientManager(logger);
   connect(mSSHClientManager, &SSHClientManager::forwardSSHData, this, &MainWindow::handleForwardedSSHData);

   QString configPath = QCoreApplication::applicationDirPath() + "/remoteConfig.xml";
   mSSHClientManager->loadConfigsAndStartClients(configPath);
}
void MainWindow::handleForwardedSSHData(const QString &data)
{
    qDebug() << "Received SSH data in MainWindow:" << data;
}

MainWindow::~MainWindow()
{
    delete ui;
    delete logger; // Clean up Logger
}

void MainWindow::CreatTrayMenu()
{
    getDateAction = new QAction("time", this);
    getCommentAction = new QAction("Comment", this);
    getMacAction = new QAction("mac", this);
    getSerialAction = new QAction("serial", this);
    w2lAction = new QAction("w2l", this);
    restartSshAction = new QAction("restart ssh", this);
    opennlogAction  = new QAction("log", this);
    quitAction = new QAction("Quit(&Q)", this);

    this->connect(getDateAction, SIGNAL(triggered()), this, SLOT(get_datetime_action()));
    this->connect(w2lAction, SIGNAL(triggered()), this, SLOT(get_w2l_action()));
    this->connect(restartSshAction, SIGNAL(triggered()), this, SLOT(restartSshSlot()));
    this->connect(opennlogAction, SIGNAL(triggered()), this, SLOT(openlogSlot()));

    this->connect(quitAction, SIGNAL(triggered()), this, SLOT(quit_action()));
    this->connect(getCommentAction, SIGNAL(triggered()), this, SLOT(get_CommentAction()));

    myMenu = new QMenu((QWidget*)QApplication::desktop());
    myMenu->addAction(getCommentAction);
    myMenu->addAction(getDateAction);
    myMenu->addAction(w2lAction);
    myMenu->addAction(restartSshAction);
    myMenu->addAction(opennlogAction);

    myMenu->addSeparator();
    myMenu->addAction(quitAction);

    QFile qss("stylesheet.qss");
    if (qss.open(QFile::ReadOnly))
    {
        QString style = QLatin1String(qss.readAll());
        myMenu->setStyleSheet(style);
        qss.close();
    }
}

void MainWindow::CreatTrayIcon()
{
    CreatTrayMenu();

    if (!QSystemTrayIcon::isSystemTrayAvailable())
    {
        return;
    }

    myTrayIcon = new QSystemTrayIcon(this);
    myTrayIcon->setIcon(QIcon(":/image/pkq.ico"));
    setWindowIcon(QIcon(":/image/pkq.ico"));
    myTrayIcon->setToolTip("V1.0");
    myTrayIcon->showMessage("tips", "SystemTray", QSystemTrayIcon::Information, 500);
    myTrayIcon->setContextMenu(myMenu);
    myTrayIcon->show();
    this->connect(myTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
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
        myTrayIcon->showMessage("tips", "SystemTray", QSystemTrayIcon::Information, 500);
        break;
    default:
        break;
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (myTrayIcon->isVisible())
    {
        myTrayIcon->showMessage("tips", "SystemTray", QSystemTrayIcon::Information, 500);
        hide();     //最小化
        event->ignore();
    }
    else
    {
        event->accept();
    }
}

void MainWindow::get_datetime_action()
{
    QString time = tool->getDatetime();
    tool->setClip(time);
}

void MainWindow::get_CommentAction()
{
    QString time = tool->getDatetime();
    QString str = "/* xiangsq " + time + " */";
    tool->setClip(str);
}

void MainWindow::get_w2l_action()
{
    QString originalText = tool->getfromClip().trimmed(); //获取剪贴板上文本信息
    QString path = QDir::fromNativeSeparators(originalText);
    tool->setClip(path);
}

void MainWindow::quit_action()
{
    if(isQuit) qApp->quit();
    else myMenu->hide();
}

void MainWindow::restartSshSlot()
{
//    mSSHClientManager->restartSSHClientWithConfig("90");
//    mSSHClientManager->restartSSHClientWithConfig("87");
//    mSSHClientManager->restartSSHClientWithConfig("80");
    mSSHClientManager->restartAllSSHClients();
}

void MainWindow::openlogSlot()
{
 logDialog->show();
}

void MainWindow::hotkey_press_action()
{
    isQuit = false;
    myMenu->exec(QCursor::pos());
}

void MainWindow::on_pushButton_clicked()
{
    logger->log("on_pushButton_clicked");
}


