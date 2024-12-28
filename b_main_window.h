#ifndef B_MAIN_WINDOW_H
#define B_MAIN_WINDOW_H

#include <QMainWindow>

namespace Ui {
class B_MainWindow;
}

class B_MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit B_MainWindow(QWidget *parent = nullptr);
    ~B_MainWindow();

private:
    Ui::B_MainWindow *ui;
};

#endif // B_MAIN_WINDOW_H
