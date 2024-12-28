#ifndef A_ALL_TEMPLATE_WINDOW_H
#define A_ALL_TEMPLATE_WINDOW_H

#include <QMainWindow>

namespace Ui {
class A_All_template_Window;
}

class A_All_template_Window : public QMainWindow
{
    Q_OBJECT

public:
    explicit A_All_template_Window(QWidget *parent = nullptr);
    ~A_All_template_Window();

private:
    Ui::A_All_template_Window *ui;
};

#endif // A_ALL_TEMPLATE_WINDOW_H
