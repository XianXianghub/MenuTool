#include "MessageDialog.h"
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QApplication>
#include <QCloseEvent>

MessageDialog* MessageDialog::instance = nullptr;

MessageDialog::MessageDialog(QWidget *parent) : QDialog(parent), label(nullptr), cancelButton(nullptr) {
     setWindowTitle("提示");

      label = new QLabel(this);
      cancelButton = new QPushButton("取消", this);
      connect(cancelButton, &QPushButton::clicked, this, &QDialog::hide);

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
    label->setText(message);

    if (type == Success) {
        label->setStyleSheet("color: green;");
    } else {
        label->setStyleSheet("color: red;");
    }

    show();
    raise();
    activateWindow();
}
