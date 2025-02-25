#include "mainwindow.h"
#include <QApplication>
#include <QSharedMemory>
#include <QMessageBox>
#include <QWidget>


#include <QString>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // 创建一个唯一的标识符
    QSharedMemory sharedMemory("UniqueAppId");

    // 尝试附加到共享内存段
    if (sharedMemory.attach()) {
        // 如果附加成功，说明已经有一个实例在运行
        QMessageBox::warning(nullptr, "Warning", "An instance of this application is already running.");
        return 0;
    }

    // 创建共享内存段
    if (!sharedMemory.create(1)) {
        QMessageBox::critical(nullptr, "Error", "Unable to create shared memory segment.");
        return 1;
    }


    MainWindow w;
    w.setWindowIcon(QIcon(":/image/pkq.ico"));
    return a.exec();
}
