#include "a_all_template_window.h"
#include "ui_a_all_template_window.h"

A_All_template_Window::A_All_template_Window(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::A_All_template_Window)
{
    ui->setupUi(this);
}

A_All_template_Window::~A_All_template_Window()
{
    delete ui;
}
