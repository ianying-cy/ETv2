#include "b_main_window.h"
#include "ui_b_main_window.h"

B_MainWindow::B_MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::B_MainWindow)
{
    ui->setupUi(this);
}

B_MainWindow::~B_MainWindow()
{
    delete ui;
}
