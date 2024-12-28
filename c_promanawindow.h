#ifndef C_PROMANAWINDOW_H
#define C_PROMANAWINDOW_H

#include <QMainWindow>

namespace Ui {
class C_ProManaWindow;
}

class C_ProManaWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit C_ProManaWindow(QWidget *parent = nullptr);
    ~C_ProManaWindow();



public://我自己定义的
    void req_database_fn();// 请求数据库数据
    void click_newButton();// 点击了新建按钮
    void createCheckBoxesFromDatabase(QDialog *parent);
private:
    Ui::C_ProManaWindow *ui;
};

#endif // C_PROMANAWINDOW_H
