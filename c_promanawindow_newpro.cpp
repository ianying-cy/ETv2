#include "c_promanawindow_newpro.h"
#include "ui_c_promanawindow_newpro.h"

#include <QStandardItemModel>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QCheckBox>



C_ProManaWindow_newPro::C_ProManaWindow_newPro(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::C_ProManaWindow_newPro)
{
    ui->setupUi(this);

    // 以下是自己写的

    createCheckBoxesFromDatabase();
}

C_ProManaWindow_newPro::~C_ProManaWindow_newPro()
{
    delete ui;
}


void C_ProManaWindow_newPro::createCheckBoxesFromDatabase(){
    // 以下是自己写的
    // 创建数据库连接
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL"); // 使用 SQLite 数据库

    db.setDatabaseName("QT");
    db.setUserName("mysql-user");
    db.setPassword("kjhl");
    db.setPort(63306);
    db.setDatabaseName("QT");
    db.setHostName("111.120.13.93");

    bool connect_sql_ok = db.open();
    if (connect_sql_ok){// QMessageBox::information(0,"infor","link success");
    }else{QMessageBox::information(0,"infor","link failed");
    }

    if (!db.open()) {
        QMessageBox::critical(this, "Database Error", db.lastError().text());
        return;
    }
    // 查询数据
    QSqlQuery query("SELECT module_name FROM ProductRequirements"); // 替换为实际的表名和列名


    // 获取 QScrollArea 控件

    QWidget *scrollAreaWidgetContents = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(scrollAreaWidgetContents);


    // 变量用于跟踪当前的行和列
    int row = 0;
    int col = 0;

    // 逐行读取数据
    while (query.next()) {
        QString checkboxName = query.value(0).toString(); // 获取列值
        QCheckBox *checkBox = new QCheckBox(checkboxName); // 创建复选框

        layout->addWidget(checkBox);
            // 更新列和行的索引
            col++;
        if (col >= 3) { // 假设每行最多显示 3 个复选框
            col = 0; // 列索引重置
            row++; // 行索引增加
        }
    }



    // 设置 scrollArea 的内容

    ui->scrollArea->setWidget(scrollAreaWidgetContents);
    ui->scrollArea->setWidgetResizable(true); // 使内容区域可调整大小
}
