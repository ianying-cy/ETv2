#include "a_all_template_window.h"
#include "a_list_template_window.h"
#include "a_login_windows.h"
#include "b_main_window.h"
#include "c_promanawindow.h"
#include "c_promanawindow_newpro.h"
#include "a_tasks_template_window.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //A_login_Windows w;


    A_login_Windows *W = new A_login_Windows(nullptr);
   // W->setWindowFlags(W->windowFlags() | Qt::FramelessWindowHint);
    W->show();

    B_MainWindow m;
    m.show();
    C_ProManaWindow p;
    p.show();
    p.req_database_fn();
    A_All_template_Window all_Template;
    all_Template.show();
    A_List_template_Window l;
    l.show();
    l.setListTemplate();
    // C_ProManaWindow_newPro pn;
    // pn.show();
    A_tasks_template_Window *t = new A_tasks_template_Window();
    t->show();
    t->init_Gante();



    return a.exec();
}
