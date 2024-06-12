#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCoreApplication>
#include <QClipboard>
#include <QMenu>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include <QFile>
#include <QDir>
#include <QKeySequence>
#include <QDebug>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    tool(new ToolUtils(QApplication::clipboard())),  // 获取系统剪贴板指针
    logger(new Logger()),
    logDialog(new LogDialog()),
    mSSHClientManager(new SSHClientManager(logger))
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon("./pkq.ico"));

    // 创建托盘图标和菜单
    createTrayIcon();

    // 设置全局快捷键
    QxtGlobalShortcut *shortcut = new QxtGlobalShortcut(this);
    if (shortcut->setShortcut(QKeySequence("F1"))) {
        connect(shortcut, &QxtGlobalShortcut::activated, this, &MainWindow::hotkeyPress);
    } else {
        logger->log("快捷键已占用");
        QMessageBox::information(this, "Title", "快捷键已占用", QMessageBox::Yes);
    }

    // 日志消息连接到日志对话框
    connect(logger, &Logger::newLogMessage, logDialog, &LogDialog::appendLog);

    // SSH 客户端管理器
    connect(mSSHClientManager, &SSHClientManager::forwardSSHData, this, &MainWindow::handleForwardedSSHData);

    // 加载 SSH 配置并启动客户端
    QString configPath = QCoreApplication::applicationDirPath() + "/remoteConfig.xml";
    mSSHClientManager->loadConfigsAndStartClients(configPath);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete logger; // 清理 Logger
}

void MainWindow::handleForwardedSSHData(const QString &data)
{
    qDebug() << "Received SSH data in MainWindow:" << data;
}

void MainWindow::createTrayMenu()
{
    getDateAction = new QAction("time", this);
    getCommentAction = new QAction("Comment", this);
    getMacAction = new QAction("mac", this);
    getSerialAction = new QAction("serial", this);
    w2lAction = new QAction("w2l", this);
    restartSshAction = new QAction("ssh", this);
    openLogAction = new QAction("log", this);
    quitAction = new QAction("Quit(&Q)", this);

    connect(getDateAction, &QAction::triggered, this, &MainWindow::getDateTime);
    connect(w2lAction, &QAction::triggered, this, &MainWindow::getW2L);
    connect(restartSshAction, &QAction::triggered, this, &MainWindow::restartSsh);
    connect(openLogAction, &QAction::triggered, this, &MainWindow::openLog);
    connect(quitAction, &QAction::triggered, this, &MainWindow::quit);
    connect(getCommentAction, &QAction::triggered, this, &MainWindow::getComment);

    myMenu = new QMenu();
    myMenu->addAction(getCommentAction);
    myMenu->addAction(getDateAction);
    myMenu->addAction(w2lAction);
    myMenu->addAction(restartSshAction);
    myMenu->addAction(openLogAction);
    myMenu->addSeparator();
    myMenu->addAction(quitAction);

    QFile qss("stylesheet.qss");
    if (qss.open(QFile::ReadOnly)) {
        QString style = QLatin1String(qss.readAll());
        myMenu->setStyleSheet(style);
        qss.close();
    }
}

void MainWindow::createTrayIcon()
{
    createTrayMenu();

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        return;
    }

    myTrayIcon = new QSystemTrayIcon(this);
    myTrayIcon->setIcon(QIcon(":/image/pkq.ico"));
    setWindowIcon(QIcon(":/image/pkq.ico"));
    myTrayIcon->setToolTip("V1.0");
    myTrayIcon->showMessage("tips", "SystemTray", QSystemTrayIcon::Information, 500);
    myTrayIcon->setContextMenu(myMenu);
    myTrayIcon->show();
    connect(myTrayIcon, &QSystemTrayIcon::activated, this, &MainWindow::iconActivated);
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    isQuit = true;
    switch(reason) {
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
    if (myTrayIcon->isVisible()) {
        myTrayIcon->showMessage("tips", "SystemTray", QSystemTrayIcon::Information, 500);
        hide(); // 最小化
        event->ignore();
    } else {
        event->accept();
    }
}

void MainWindow::getDateTime()
{
    QString time = tool->getDatetime();
    tool->setClip(time);
}

void MainWindow::getComment()
{
    QString time = tool->getDatetime();
    QString str = "/* xiangsq " + time + " */";
    tool->setClip(str);
}

void MainWindow::getW2L()
{
    QString originalText = tool->getfromClip().trimmed(); // 获取剪贴板上文本信息
    QString path = QDir::fromNativeSeparators(originalText);
    tool->setClip(path);
}

void MainWindow::quit()
{
    if (isQuit) {
        qApp->quit();
    } else {
        myMenu->hide();
    }
}

void MainWindow::restartSsh()
{
    mSSHClientManager->restartAllSSHClients();
}

void MainWindow::openLog()
{
    logDialog->show();
}

void MainWindow::hotkeyPress()
{
    isQuit = false;
    myMenu->exec(QCursor::pos());
}
