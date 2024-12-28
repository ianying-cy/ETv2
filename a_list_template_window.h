#ifndef A_LIST_TEMPLATE_WINDOW_H
#define A_LIST_TEMPLATE_WINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlTableModel>
namespace Ui {
class A_List_template_Window;
}

class A_List_template_Window : public QMainWindow
{
    Q_OBJECT

public:
    explicit A_List_template_Window(QWidget *parent = nullptr);
    ~A_List_template_Window();
public://自己定义的
    void setListTemplate();
    QSqlTableModel *model;//定义模型
    void submitChanges();//提交修改
    void showContextMenu(const QPoint &pos);//显示右键菜单
    void insertRow();//插入行；
    void deleteRow();//删除行；
    void refreshData();//刷新，重新读取数据库；
    void updatePrimaryKeys(QSqlTableModel* model);// 修改数据库主键

private:
    Ui::A_List_template_Window *ui;

};

#endif // A_LIST_TEMPLATE_WINDOW_H
