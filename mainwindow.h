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

    void get_mac_action();
    void get_datetime_action();
    void get_serial_action();
    void get_w2l_action();
    void quit_action();
    void hotkey_press_action();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    bool isQuit = false;
};

#endif // MAINWINDOW_H
