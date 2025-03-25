#include "messagedialog.h"
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QApplication>
#include <QCloseEvent>
#include <qDebug>
#include <QProcess>

MessageDialog* MessageDialog::instance = nullptr;

MessageDialog::MessageDialog(QWidget *parent) : QDialog(parent), label(nullptr), cancelButton(nullptr) {
     setWindowTitle("提示");

      label = new QLabel(this);
      cancelButton = new QPushButton("取消", this);
      connect(cancelButton, &QPushButton::clicked, this,  myButtonClick);

      QVBoxLayout *mainLayout = new QVBoxLayout(this);

      QHBoxLayout *labelLayout = new QHBoxLayout;
      labelLayout->addWidget(label);
      labelLayout->setAlignment(Qt::AlignCenter);

      mainLayout->addLayout(labelLayout);

      mainLayout->addWidget(cancelButton, 0, Qt::AlignBottom);

      setLayout(mainLayout);
      setFixedSize(300, 150);
}

MessageDialog::~MessageDialog() {
}

MessageDialog* MessageDialog::getInstance(QWidget *parent) {
    if (!instance) {
        instance = new MessageDialog(parent);
    }
    return instance;
}
void MessageDialog::closeEvent(QCloseEvent *event)
{
    event->ignore();
    this->hide();
}

void MessageDialog::showMessage(const QString &message, MessageType type) {
    setWindowTitle(type == Success ? "编译成功" : "编译失败");
    isSuccess = type == Success;
    label->setText(message);
    if(type == Success){
        cancelButton->setText("打开");
    }else{
        cancelButton->setText("取消");
    }
    if (type == Success) {
        label->setStyleSheet("color: green;");
    } else {
        label->setStyleSheet("color: red;");
    }

    show();
    raise();
    activateWindow();
}

void MessageDialog::myButtonClick(bool checked)
{
    if(!isSuccess){
        hide();
    }else{
        qDebug() << "Project strPicPath:"+label->text();

        QStringList list = label->text().split("\n");

        if(list.size() == 3){
            QStringList list2 = list[1].split(" ");
            if(list2.size() >= 2){
                if(QString::compare(list2[1], "编译成功") == 0){
                    QString strPicPath =  "\\\\192.168.101.87\\share\\daily_build\\"+list2[0];
                    qDebug() << "Project strPicPath:" << strPicPath;
                    QStringList list3 = list[2].split(":");
                    if(list3.size() >= 2){
                                strPicPath= strPicPath+"\\"+list3[1];
                                qDebug() << "Project strPicPath:" << strPicPath;
                                strPicPath.replace("/", "\\");
                                QProcess process;
                                process.startDetached("explorer", QStringList() << QString("/select,") << QString("%1").arg(strPicPath));
                    }
                    hide();
                }
            }
        }
    }
}
