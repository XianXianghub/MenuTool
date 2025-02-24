#ifndef MESSAGEDIALOG_H
#define MESSAGEDIALOG_H

#include <QDialog>
#include <QString>

class QLabel;
class QPushButton;

class MessageDialog : public QDialog {
    Q_OBJECT

public:
    enum MessageType {
        Success,
        Failure
    };

    static MessageDialog* getInstance(QWidget *parent = nullptr); // 获取单例

    void showMessage(const QString &message, MessageType type); // 更新并显示消息
    bool isSuccess = false;
public slots:
    void myButtonClick(bool checked);
private:
    explicit MessageDialog(QWidget *parent = nullptr); // 私有构造函数
    ~MessageDialog(); // 析构函数
    void closeEvent(QCloseEvent *event);

    static MessageDialog *instance; // 单例对象
    QLabel *label;
    QPushButton *cancelButton;
};

#endif // MESSAGEDIALOG_H
