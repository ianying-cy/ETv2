#include "c_promanawindow.h"
#include "ui_c_promanawindow.h"
#include "c_promanawindow_newpro_dialog.h"
#include "ui_c_promanawindow_newpro_dialog.h"
#include <QStandardItemModel>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QPushButton>
#include <QComboBox>
#include <QDialog>
#include <QMessageBox>
#include <QCheckBox>
#include <QGridLayout>
#include <QHBoxLayout>



C_ProManaWindow::C_ProManaWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::C_ProManaWindow)
{
    ui->setupUi(this);


    QObject::connect(ui->new_pushButton, &QPushButton::clicked, [=]() {
        qDebug("点击了新建按钮按钮");
        click_newButton();
    });
}

C_ProManaWindow::~C_ProManaWindow()
{
    delete ui;
}
void C_ProManaWindow::req_database_fn(){

    //第一步：定义模型
    QStandardItemModel *theModel;

    QSqlQueryModel *sqlQueryModel;
    QItemSelectionModel *theSelection;

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
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


    sqlQueryModel = new QSqlQueryModel(this);
    //theSelection = new QItemSelectionModel(sqlQueryModel);//初始化数据模型
    sqlQueryModel->setQuery("SELECT * FROM Project");
    sqlQueryModel->setHeaderData(0, Qt::Horizontal, QStringLiteral("项目ID"));
    sqlQueryModel->setHeaderData(1, Qt::Horizontal, QStringLiteral("项目名称"));
    sqlQueryModel->setHeaderData(2, Qt::Horizontal, QStringLiteral("项目类型"));
    sqlQueryModel->setHeaderData(3, Qt::Horizontal, QStringLiteral("项目所属区域"));
    sqlQueryModel->setHeaderData(4, Qt::Horizontal, QStringLiteral("项目位置"));
    sqlQueryModel->setHeaderData(5, Qt::Horizontal, QStringLiteral("销售（销售存在多人情况）"));
    sqlQueryModel->setHeaderData(6, Qt::Horizontal, QStringLiteral("售前"));
    sqlQueryModel->setHeaderData(7, Qt::Horizontal, QStringLiteral("渠道名称"));
    sqlQueryModel->setHeaderData(8, Qt::Horizontal, QStringLiteral("进厂身份"));
    sqlQueryModel->setHeaderData(9, Qt::Horizontal, QStringLiteral("工前协调会时间"));
    sqlQueryModel->setHeaderData(10, Qt::Horizontal, QStringLiteral("进厂时间"));
    sqlQueryModel->setHeaderData(11, Qt::Horizontal, QStringLiteral("暂停时间"));
    sqlQueryModel->setHeaderData(12, Qt::Horizontal, QStringLiteral("暂停原因"));
    sqlQueryModel->setHeaderData(13, Qt::Horizontal, QStringLiteral("计划完成时间"));
    sqlQueryModel->setHeaderData(14, Qt::Horizontal, QStringLiteral("实际完成时间"));
    sqlQueryModel->setHeaderData(15, Qt::Horizontal, QStringLiteral("当前进展天数"));
    sqlQueryModel->setHeaderData(16, Qt::Horizontal, QStringLiteral("计划验收时间"));
    sqlQueryModel->setHeaderData(17, Qt::Horizontal, QStringLiteral("实际验收时间"));
    sqlQueryModel->setHeaderData(18, Qt::Horizontal, QStringLiteral("停工时间"));
    sqlQueryModel->setHeaderData(19, Qt::Horizontal, QStringLiteral("模块要求"));
    sqlQueryModel->setHeaderData(20, Qt::Horizontal, QStringLiteral("是否定制化"));
    sqlQueryModel->setHeaderData(21, Qt::Horizontal, QStringLiteral("超期说明"));
    sqlQueryModel->setHeaderData(22, Qt::Horizontal, QStringLiteral("要求说明"));
    sqlQueryModel->setHeaderData(23, Qt::Horizontal, QStringLiteral("现场查看说明"));
    sqlQueryModel->setHeaderData(24, Qt::Horizontal, QStringLiteral("需求描述"));
    sqlQueryModel->setHeaderData(25, Qt::Horizontal, QStringLiteral("期望部署时间"));
    sqlQueryModel->setHeaderData(26, Qt::Horizontal, QStringLiteral("产品"));
    sqlQueryModel->setHeaderData(27, Qt::Horizontal, QStringLiteral("UI"));
    sqlQueryModel->setHeaderData(28, Qt::Horizontal, QStringLiteral("研发"));
    sqlQueryModel->setHeaderData(29, Qt::Horizontal, QStringLiteral("售后"));
    sqlQueryModel->setHeaderData(30, Qt::Horizontal, QStringLiteral("测试"));
    sqlQueryModel->setHeaderData(31, Qt::Horizontal, QStringLiteral("发版时间"));
    sqlQueryModel->setHeaderData(32, Qt::Horizontal, QStringLiteral("需求处理后反馈"));


    // 将模型设置到表视图
    ui->pro_tableView->setModel(sqlQueryModel);



    // 隐藏最左边的列排序
    ui->pro_tableView->verticalHeader()->hide();
    ui->pro_tableView->horizontalHeader()->setHighlightSections(true);


    QComboBox *tempComboBox = new QComboBox(this);
    tempComboBox->addItem("工程");
    tempComboBox->addItem("测试");
    //pro_winTableView->setIndexWidget(theModel->index(1,2),tempComboBox);
    ui->pro_tableView->setIndexWidget(sqlQueryModel->index(2,2),tempComboBox);
}
void C_ProManaWindow::click_newButton(){
    // 创建一个对话框

    C_ProManaWindow_newPro_Dialog newProWindow;

    newProWindow.exec();


}
