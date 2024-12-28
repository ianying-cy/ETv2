#ifndef A_LOGIN_WINDOWS_H
#define A_LOGIN_WINDOWS_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class A_login_Windows;
}
QT_END_NAMESPACE

class A_login_Windows : public QMainWindow
{
    Q_OBJECT

public:
    A_login_Windows(QWidget *parent = nullptr);
    ~A_login_Windows();

private:
    Ui::A_login_Windows *ui;
};
#endif // A_LOGIN_WINDOWS_H
