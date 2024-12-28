#ifndef C_PROMANAWINDOW_NEWPRO_H
#define C_PROMANAWINDOW_NEWPRO_H

#include <QMainWindow>

namespace Ui {
class C_ProManaWindow_newPro;
}

class C_ProManaWindow_newPro : public QMainWindow
{
    Q_OBJECT
public:
    explicit C_ProManaWindow_newPro(QWidget *parent = nullptr);
    ~C_ProManaWindow_newPro();

private:
    Ui::C_ProManaWindow_newPro *ui;

// 以下是自己写的
public:

    void createCheckBoxesFromDatabase();
};

#endif // C_PROMANAWINDOW_NEWPRO_H
