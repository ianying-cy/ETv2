#include "a_login_windows.h"
#include "ui_a_login_windows.h"

A_login_Windows::A_login_Windows(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::A_login_Windows)
{
    ui->setupUi(this);
}

A_login_Windows::~A_login_Windows()
{
    delete ui;
}
