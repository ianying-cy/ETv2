#include "a_list_template_window.h"
#include "ui_a_list_template_window.h"
#include <QStandardItemModel>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QPushButton>
#include <QComboBox>
#include <QDialog>
#include <QMessageBox>
#include <QMenu>
#include <QAction>
#include <QModelIndex>
#include <QDebug>
#include <QModelIndex>
#include <QDateTime>

A_List_template_Window::A_List_template_Window(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::A_List_template_Window)
{
    ui->setupUi(this);
}

A_List_template_Window::~A_List_template_Window()
{
    delete ui;
}

void A_List_template_Window::setListTemplate(){
    // 创建数据库连接
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");

    db.setHostName("111.120.13.93");
    db.setPort(63306);

    db.setUserName("mysql-user");
    db.setPassword("kjhl");

    db.setDatabaseName("QT");


    bool connect_sql_ok = db.open();
    if (connect_sql_ok){// QMessageBox::information(0,"infor","link success");
    }else{QMessageBox::information(0,"infor","link failed");
    }

    if (!db.open()) {
        QMessageBox::critical(nullptr, "Database Error", db.lastError().text());
        return;
    }



    // 创建模型
    model = new QSqlTableModel(this, db);
    model->setTable("ProductRequirements");
    model->select(); // 加载数据
    ui->tableView->setModel(model);
    ui->tableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);

    // 点击提交按钮
    connect(ui->submit_pushButton,&QPushButton::clicked,this,&A_List_template_Window::submitChanges);


    // 右键菜单
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView, &QTableView::customContextMenuRequested, this, &A_List_template_Window::showContextMenu);

}
void A_List_template_Window::submitChanges(){
    if (model->submitAll()) {
        QMessageBox::information(this, "Success", "Changes submitted successfully!");
    } else {
        QMessageBox::critical(this, "Error", model->lastError().text());
    }
}


void A_List_template_Window::showContextMenu(const QPoint &pos) {
    QMenu contextMenu(tr("Context Menu"), this);

    QAction *insertAction = new QAction("当前行插入", this);
    connect(insertAction, &QAction::triggered, this, &A_List_template_Window::insertRow);
    contextMenu.addAction(insertAction);

    QAction *deleteAction = new QAction("删除当前行", this);
    connect(deleteAction, &QAction::triggered, this, &A_List_template_Window::deleteRow);
    contextMenu.addAction(deleteAction);

    QAction *refreshAction = new QAction("刷新整张表", this);
    connect(refreshAction, &QAction::triggered, this, &A_List_template_Window::refreshData);
    contextMenu.addAction(refreshAction);


    contextMenu.exec(ui->tableView->mapToGlobal(pos));
}


//下面代码使用ID排序有问题

//下面代码使用ID排序有问题
void A_List_template_Window::insertRow() {
    QModelIndex currentIndex = ui->tableView->currentIndex();
    if (!currentIndex.isValid()) {
        return; // 没有选中行
    }

    QString originalTableName = "ProductRequirements";
    QString newTableName = originalTableName + "_" + QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");

    QSqlQuery query;
    QString copyTableQuery = QString("CREATE TABLE %1 AS SELECT * FROM %2").arg(newTableName).arg(originalTableName);
    if (!query.exec(copyTableQuery)) {
        QMessageBox::critical(this, "Error", query.lastError().text());
        return;
    }

    int row = currentIndex.row();
    int needUpdataId = row + 1;
    qDebug()<<"初始 needUpadataId ="<<needUpdataId;

    // 更新当前行后面的ID 从10000开始
    for (int i = 10000; i < (10000+model->rowCount()); ++i) {
        model->setData(model->index(needUpdataId,0),i);
        needUpdataId++;
        qDebug()<<"needUpadataId ="<<needUpdataId;
        qDebug()<<"index ---="<<model->index(needUpdataId,0);
        model->submitAll();
    }
    // 更新完成后读取数据库
    model->submitAll();
    model->select(); // 重新读取数据

    // 更新后面的行的 ID：将ID从10000改回来的同时插入行下面的行自动完成加一。
    qDebug()<<"将ID改回来 row ="<<row;
    int r = row+2;
    for (int i = r-1; i < model->rowCount(); ++i) {
        model->setData(model->index(i, 0), r+1);
        r++;
        model->submitAll();
        qDebug()<<"index="<<model->index(i,0);
        qDebug()<<"i="<<i;
        qDebug()<<"执行了吗"<<r;
    }
    model->submitAll();
    model->select(); // 重新读取数据
    qDebug()<<"row----------------------------------- ="<<row;
    model->insertRow(row + 1);

    // // 更新新行的 ID
    model->setData(model->index(row + 1, 0), row + 2); // 假设 ID 在第 0 列

    // // 复制当前行的内容到新行
    for (int col = 1; col < model->columnCount(); ++col) { // 从第 1 列开始复制
        model->setData(model->index(row + 1, col), model->data(model->index(row, col)));
    }


    //提交更改
    if (!model->submitAll()) {
        QMessageBox::critical(this, "Error", model->lastError().text());
    }


    model->setSort(model->fieldIndex("id"), Qt::AscendingOrder);
    model->select(); // 重新加载数据以应用排序

    // 更新完成后读取数据库
    model->select(); // 重新读取数据
}



/*以下代码是直接插入
void A_List_template_Window::insertRow() {
    QModelIndex currentIndex = ui->tableView->currentIndex();
    if (!currentIndex.isValid()) {
        return; // 没有选中行
    }

    QString originalTableName = "ProductRequirements";
    QString newTableName = originalTableName + "_" + QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");

    QSqlQuery query;
    QString copyTableQuery = QString("CREATE TABLE %1 AS SELECT * FROM %2").arg(newTableName).arg(originalTableName);
    if (!query.exec(copyTableQuery)) {
        QMessageBox::critical(this, "Error", query.lastError().text());
        return;
    }

    int row = currentIndex.row();

    model->insertRow(row + 1);

    // // 更新新行的 ID
    model->setData(model->index(row + 1, 0), row + 2); // 假设 ID 在第 0 列

    // // 复制当前行的内容到新行
    for (int col = 2; col < model->columnCount(); ++col) { // 从第 1 列开始复制
        model->setData(model->index(row + 1, col), model->data(model->index(row, col)));
    }


    //提交更改
    if (!model->submitAll()) {
        QMessageBox::critical(this, "Error", model->lastError().text());
    }


    model->setSort(model->fieldIndex("id"), Qt::AscendingOrder);

    // 更新完成后读取数据库
    model->select(); // 重新读取数据
}
*/
void A_List_template_Window::deleteRow() {
    QModelIndex currentIndex = ui->tableView->currentIndex();
    if (!currentIndex.isValid()) {
        return; // 没有选中行
    }

    model->removeRow(currentIndex.row());
}
void A_List_template_Window::refreshData(){
    model->setSort(model->fieldIndex("id"), Qt::AscendingOrder);
    model->select(); // 重新加载数据以应用排序
    model->select(); // 重新读取数据
}


