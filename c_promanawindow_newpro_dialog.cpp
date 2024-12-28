#include "c_promanawindow_newpro_dialog.h"
#include "ui_c_promanawindow_newpro_dialog.h"

#include <QStandardItemModel>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QCheckBox>
#include <QLabel>
#include <QScrollArea>
#include <QGridLayout>

C_ProManaWindow_newPro_Dialog::C_ProManaWindow_newPro_Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::C_ProManaWindow_newPro_Dialog)
{
    ui->setupUi(this);
    createCheckBoxesFromDatabase();

    // 连接按钮的点击信号
    connect(ui->pushButton_Create_Pro, &QPushButton::clicked, this, &C_ProManaWindow_newPro_Dialog::onCreateProButtonClicked);

}

C_ProManaWindow_newPro_Dialog::~C_ProManaWindow_newPro_Dialog()
{
    delete ui;
}

void C_ProManaWindow_newPro_Dialog::createCheckBoxesFromDatabase() {
    // 创建数据库连接
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName("QT");
    db.setUserName("mysql-user");
    db.setPassword("kjhl");
    db.setPort(63306);
    db.setHostName("111.120.13.93");

    if (!db.open()) {
        QMessageBox::critical(this, "Database Error", db.lastError().text());
        return;
    }

    // 查询数据
    QSqlQuery query("SELECT module_name FROM ProductRequirements");

    // 创建用于显示复选框的控件
    QWidget *scrollAreaWidgetContents = new QWidget();
    QGridLayout *layout = new QGridLayout(scrollAreaWidgetContents);

    int row = 0;
    int column = 0;

    // 用于存放选中标签的布局
    QVBoxLayout *labelLayout = new QVBoxLayout();

    // 逐行读取数据
    while (query.next()) {
        QString checkboxName = query.value(0).toString(); // 获取列值
        QCheckBox *checkBox = new QCheckBox(checkboxName); // 创建复选框

        // 连接复选框的信号
        connect(checkBox, &QCheckBox::toggled, this, [this, checkboxName, labelLayout](bool checked) {
            if (checked) {
                QLabel *label = new QLabel(checkboxName);
                labelLayout->addWidget(label); // 将标签添加到标签布局中
            } else {
                // 如果取消选中，移除标签
                for (int i = 0; i < labelLayout->count(); ++i) {
                    QLabel *existingLabel = qobject_cast<QLabel*>(labelLayout->itemAt(i)->widget());
                    if (existingLabel && existingLabel->text() == checkboxName) {
                        labelLayout->removeWidget(existingLabel);
                        delete existingLabel; // 释放内存
                        break; // 找到后退出循环
                    }
                }
            }
            // 更新 scrollArea_Checked 的内容
            updateCheckedScrollArea(labelLayout);

            // 更新 label_Num 显示的数量
            ui->label_Num->setText(QString::number(labelLayout->count()));

        });

        layout->addWidget(checkBox, row, column); // 添加复选框到布局中

        // 更新行和列
        column++;
        if (column >= 3) { // 每行显示3个复选框
            column = 0;
            row++;
        }
    }

    // 设置 scrollArea 的内容
    ui->scrollArea_Template->setWidget(scrollAreaWidgetContents);
    ui->scrollArea_Template->setWidgetResizable(true); // 使内容区域可调整大小
}


void C_ProManaWindow_newPro_Dialog::updateCheckedScrollArea(QVBoxLayout *labelLayout) {
    // 创建一个新的 QWidget 用于显示选中的标签
    QWidget *checkedWidget = new QWidget();
    QGridLayout *checkedLayout = new QGridLayout(checkedWidget); // 使用 QGridLayout

    int row = 0;
    int column = 0;

    // 将选中的标签添加到 checkedLayout
    for (int i = 0; i < labelLayout->count(); ++i) {
        QLabel *label = qobject_cast<QLabel*>(labelLayout->itemAt(i)->widget());
        if (label) {
            // 创建一个新的 QLabel，并设置文本
            QLabel *newLabel = new QLabel(label->text());
            checkedLayout->addWidget(newLabel, row, column); // 将新标签添加到 checkedLayout

            // 更新行和列
            column++;
            if (column >= 5) { // 每行显示5个标签
                column = 0;
                row++;
            }
        }
    }

    // 设置 scrollArea_Checked 的内容
    ui->scrollArea_Checked->setWidget(checkedWidget);
    ui->scrollArea_Checked->setWidgetResizable(true);
}


void C_ProManaWindow_newPro_Dialog::onCreateProButtonClicked() {
    QString projectNumber = ui->lineEdit_ProCharNum->text();
    QString projectManager = ui->lineEdit_17->text(); // 假设项目经理的输入框是 lineEdit_17
    QString creatorName = "NULL"; // 这里假设创建人信息暂时为 NULL，您可以根据实际情况修改

    if (projectNumber.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "项目编号不能为空，请填写项目编号。");
        return;
    }

    if (projectManager.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "项目经理不能为空，请填写项目经理。");
        return;
    }


    // 创建数据库连接
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName("All");
    db.setUserName("mysql-user");
    db.setPassword("kjhl");
    db.setPort(63306);
    db.setHostName("111.120.13.93");

    if (!db.open()) {
        QMessageBox::critical(this, "Database Error", db.lastError().text());
        return;
    }

    // 检查项目编号是否唯一
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT COUNT(*) FROM `project_total` WHERE `pro_charNum` = :projectNumber");
    checkQuery.bindValue(":projectNumber", projectNumber);

    if (!checkQuery.exec()) {
        QMessageBox::critical(this, "Database Error", checkQuery.lastError().text());
        return;
    }

    checkQuery.next();
    if (checkQuery.value(0).toInt() > 0) {
        QMessageBox::warning(this, "输入错误", "项目编号已存在，请修改项目编号。");
        return;
    }


    // 检查 scrollArea_Checked 是否有内容
    QLayout *layout = ui->scrollArea_Checked->widget()->layout();
    if (layout == nullptr || layout->count() == 0) {
        QMessageBox::warning(this, "输入错误", "请至少选择一个模块。");
        return;
    }

    // 获取选中的模块名称
    QStringList selectedModules;
    //QLayout *layout = ui->scrollArea_Checked->widget()->layout();
    for (int i = 0; i < layout->count(); ++i) {
        QLabel *label = qobject_cast<QLabel*>(layout->itemAt(i)->widget());
        if (label) {
            selectedModules << label->text();
            qDebug()<<label->text();
        }
    }
    // 插入新项目
    QSqlQuery insertQuery;
    insertQuery.prepare("INSERT INTO `project_total` (`pro_charNum`, `pro_name`, `creat_name`, `pro_manager`) VALUES (:projectNumber, :projectName, :creatorName, :projectManager)");
    insertQuery.bindValue(":projectNumber", projectNumber);
    insertQuery.bindValue(":projectName", ui->lineEdit_8->text()); // 假设项目名称的输入框是 lineEdit_8
    insertQuery.bindValue(":creatorName", creatorName); // 创建人
    insertQuery.bindValue(":projectManager", projectManager);

    if (insertQuery.exec()) {
        QMessageBox::information(this, "成功", "项目已成功创建。");
    } else {
        QMessageBox::critical(this, "数据库错误", insertQuery.lastError().text());
    }


    // 创建新数据库实例，名称为项目编号
    QSqlQuery createDbQuery;
    QString dbName = projectNumber; // 使用项目编号作为数据库名称
    createDbQuery.prepare("CREATE DATABASE IF NOT EXISTS `" + dbName + "`");

    if (!createDbQuery.exec()) {
        QMessageBox::critical(this, "数据库错误", createDbQuery.lastError().text());
        db.close(); // 关闭数据库连接
        return;
    }

    createProjectTable(dbName);
    createModuleParameterTable(dbName,selectedModules);
    createScheduleTable(dbName);
    createDailyReportTable(dbName);
    createUserFeedbackFormTable(dbName);
    createRequirementTable(dbName);
    // 关闭数据库连接
    db.close();

}
// 创建项目表
void C_ProManaWindow_newPro_Dialog::createProjectTable(const QString &dbName) {
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName(dbName);
    db.setUserName("mysql-user");
    db.setPassword("kjhl");
    db.setPort(63306);
    db.setHostName("111.120.13.93");

    // 尝试打开数据库连接
    if (!db.open()) return;

    QSqlQuery query;
    // 创建项目表
    query.exec("CREATE TABLE IF NOT EXISTS Project_Table ("
               "ID INT AUTO_INCREMENT PRIMARY KEY, "
               "Project_Name VARCHAR(255), "
               "Customer_Name VARCHAR(255), "
               "Sales_Name VARCHAR(255), "
               "Pre_sales_Name VARCHAR(255), "
               "Business_Name VARCHAR(255), "
               "Project_Manager VARCHAR(255), "
               "Start_Time DATE, "
               "Module_Name TEXT, "
               "Requirement_Quantity INT DEFAULT 0)");

    db.close(); // 关闭数据库连接
}

// 创建模块参数表
void C_ProManaWindow_newPro_Dialog::createModuleParameterTable(const QString &dbName, const QStringList &selectedModules) {
    // 连接到当前数据库
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName(dbName);
    db.setUserName("mysql-user");
    db.setPassword("kjhl");
    db.setPort(63306);
    db.setHostName("111.120.13.93");

    // 尝试打开数据库连接
    if (!db.open()) {
        QMessageBox::critical(this, "Database Error", db.lastError().text());
        return;
    }

    // 创建模块参数表
    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS Module_Parameter_Table ("
               "id INT AUTO_INCREMENT PRIMARY KEY, "
               "product_category VARCHAR(255), "
               "module_name VARCHAR(255), "
               "technical_specifications VARCHAR(255), "
               "mandatory_flag VARCHAR(255), "
               "remarks TEXT, "
               "suggested_tender_name VARCHAR(255), "
               "created_at DATETIME, "
               "updated_at DATETIME)");

    // 连接到 QT 数据库
    QSqlDatabase qtDb = QSqlDatabase::addDatabase("QMYSQL", "QT_DB_CONNECTION");
    qtDb.setDatabaseName("QT");
    qtDb.setUserName("mysql-user");
    qtDb.setPassword("kjhl");
    qtDb.setPort(63306);
    qtDb.setHostName("111.120.13.93");

    // 尝试打开 QT 数据库连接
    if (!qtDb.open()) {
        QMessageBox::critical(this, "Database Error", qtDb.lastError().text());
        return;
    }


    // 遍历选中的模块名称
    for (const QString &moduleName : selectedModules) {
        // 查询对应的模块参数
        QSqlQuery selectQuery;
        selectQuery.prepare("SELECT product_category, module_name, technical_specifications, mandatory_flag, remarks, suggested_tender_name, created_at, updated_at "
                            "FROM QT.ProductRequirements WHERE module_name = :moduleName");
        selectQuery.bindValue(":moduleName", moduleName);

        if (selectQuery.exec()) {
            while (selectQuery.next()) {
                // 获取字段值
                QString productCategory = selectQuery.value("product_category").toString();
                QString technicalSpecifications = selectQuery.value("technical_specifications").toString();
                QString mandatoryFlag = selectQuery.value("mandatory_flag").toString();
                QString remarks = selectQuery.value("remarks").toString();
                QString suggestedTenderName = selectQuery.value("suggested_tender_name").toString();
                QDateTime createdAt = selectQuery.value("created_at").toDateTime();
                QDateTime updatedAt = selectQuery.value("updated_at").toDateTime();

                // 输出模块信息
                qDebug() << "Module Name:" << moduleName;
                qDebug() << "Product Category:" << productCategory;
                qDebug() << "Technical Specifications:" << technicalSpecifications;
                qDebug() << "Mandatory Flag:" << mandatoryFlag;
                qDebug() << "Remarks:" << remarks;
                qDebug() << "Suggested Tender Name:" << suggestedTenderName;
                qDebug() << "Created At:" << createdAt;
                qDebug() << "Updated At:" << updatedAt;

                // 插入数据到 Module_Parameter_Table
                QSqlQuery insertQuery;
                insertQuery.prepare("INSERT INTO Module_Parameter_Table (product_category, module_name, technical_specifications, mandatory_flag, remarks, suggested_tender_name, created_at, updated_at) "
                                    "VALUES (:productCategory, :moduleName, :technicalSpecifications, :mandatoryFlag, :remarks, :suggestedTenderName, :createdAt, :updatedAt)");
                insertQuery.bindValue(":productCategory", productCategory);
                insertQuery.bindValue(":moduleName", moduleName);
                insertQuery.bindValue(":technicalSpecifications", technicalSpecifications);
                insertQuery.bindValue(":mandatoryFlag", mandatoryFlag);
                insertQuery.bindValue(":remarks", remarks);
                insertQuery.bindValue(":suggestedTenderName", suggestedTenderName);
                insertQuery.bindValue(":createdAt", createdAt);
                insertQuery.bindValue(":updatedAt", updatedAt);

                // 执行插入查询
                if (!insertQuery.exec()) {
                    QMessageBox::critical(this, "Database Error", insertQuery.lastError().text());
                }
            }
        } else {
            QMessageBox::critical(this, "Database Error", selectQuery.lastError().text());
        }
    }
    // 关闭数据库连接
    db.close();
    qtDb.close();
}

// 创建计划表
void C_ProManaWindow_newPro_Dialog::createScheduleTable(const QString &dbName) {
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName(dbName);
    db.setUserName("mysql-user");
    db.setPassword("kjhl");
    db.setPort(63306);
    db.setHostName("111.120.13.93");

    // 尝试打开数据库连接
    if (!db.open()) return;

    QSqlQuery query;
    // 创建计划表
    query.exec("CREATE TABLE IF NOT EXISTS Schedule_Table ("
               "ID INT AUTO_INCREMENT PRIMARY KEY, "
               "module_name VARCHAR(255), "
               "sub_task TEXT, "
               "responsible_party VARCHAR(255), "
               "day1 INT DEFAULT 0, "
               "day2 INT DEFAULT 0, "
               "day3 INT DEFAULT 0, "
               "day4 INT DEFAULT 0, "
               "day5 INT DEFAULT 0, "
               "day6 INT DEFAULT 0, "
               "day7 INT DEFAULT 0, "
               "day8 INT DEFAULT 0, "
               "day9 INT DEFAULT 0, "
               "day10 INT DEFAULT 0, "
               "day11 INT DEFAULT 0, "
               "day12 INT DEFAULT 0, "
               "day13 INT DEFAULT 0, "
               "day14 INT DEFAULT 0, "
               "day15 INT DEFAULT 0, "
               "day16 INT DEFAULT 0, "
               "day17 INT DEFAULT 0, "
               "day18 INT DEFAULT 0, "
               "day19 INT DEFAULT 0, "
               "day20 INT DEFAULT 0, "
               "day21 INT DEFAULT 0, "
               "day22 INT DEFAULT 0, "
               "day23 INT DEFAULT 0, "
               "day24 INT DEFAULT 0, "
               "day25 INT DEFAULT 0, "
               "day26 INT DEFAULT 0, "
               "day27 INT DEFAULT 0, "
               "day28 INT DEFAULT 0, "
               "day29 INT DEFAULT 0, "
               "day30 INT DEFAULT 0, "
               "day31 INT DEFAULT 0, "
               "day32 INT DEFAULT 0, "
               "day33 INT DEFAULT 0, "
               "day34 INT DEFAULT 0, "
               "day35 INT DEFAULT 0, "
               "day36 INT DEFAULT 0, "
               "day37 INT DEFAULT 0, "
               "day38 INT DEFAULT 0, "
               "day39 INT DEFAULT 0, "
               "day40 INT DEFAULT 0, "
               "day41 INT DEFAULT 0, "
               "day42 INT DEFAULT 0, "
               "day43 INT DEFAULT 0, "
               "day44 INT DEFAULT 0, "
               "day45 INT DEFAULT 0, "
               "day46 INT DEFAULT 0, "
               "day47 INT DEFAULT 0, "
               "day48 INT DEFAULT 0, "
               "day49 INT DEFAULT 0, "
               "day50 INT DEFAULT 0, "
               "day51 INT DEFAULT 0, "
               "day52 INT DEFAULT 0, "
               "day53 INT DEFAULT 0, "
               "day54 INT DEFAULT 0, "
               "day55 INT DEFAULT 0, "
               "day56 INT DEFAULT 0, "
               "day57 INT DEFAULT 0, "
               "day58 INT DEFAULT 0, "
               "day59 INT DEFAULT 0, "
               "day60 INT DEFAULT 0, "
               "day61 INT DEFAULT 0, "
               "day62 INT DEFAULT 0, "
               "day63 INT DEFAULT 0, "
               "day64 INT DEFAULT 0, "
               "day65 INT DEFAULT 0, "
               "day66 INT DEFAULT 0, "
               "day67 INT DEFAULT 0, "
               "day68 INT DEFAULT 0, "
               "day69 INT DEFAULT 0, "
               "day70 INT DEFAULT 0, "
               "day71 INT DEFAULT 0, "
               "day72 INT DEFAULT 0, "
               "day73 INT DEFAULT 0, "
               "day74 INT DEFAULT 0, "
               "day75 INT DEFAULT 0, "
               "day76 INT DEFAULT 0, "
               "day77 INT DEFAULT 0, "
               "day78 INT DEFAULT 0, "
               "day79 INT DEFAULT 0, "
               "day80 INT DEFAULT 0, "
               "day81 INT DEFAULT 0, "
               "day82 INT DEFAULT 0, "
               "day83 INT DEFAULT 0, "
               "day84 INT DEFAULT 0, "
               "day85 INT DEFAULT 0, "
               "day86 INT DEFAULT 0, "
               "day87 INT DEFAULT 0, "
               "day88 INT DEFAULT 0, "
               "day89 INT DEFAULT 0, "
               "day90 INT DEFAULT 0)");

    db.close(); // 关闭数据库连接
}

// 创建日报表
void C_ProManaWindow_newPro_Dialog::createDailyReportTable(const QString &dbName) {
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName(dbName);
    db.setUserName("mysql-user");
    db.setPassword("kjhl");
    db.setPort(63306);
    db.setHostName("111.120.13.93");

    // 尝试打开数据库连接
    if (!db.open()) return;

    QSqlQuery query;
    // 创建日报表
    query.exec("CREATE TABLE IF NOT EXISTS Daily_Report_Table ("
               "Daily_Report_Title VARCHAR(255), "
               "Work_Content TEXT, "
               "Module_Name VARCHAR(255), "
               "Task_Name VARCHAR(255))");

    db.close(); // 关闭数据库连接
}

// 创建用户反馈表
void C_ProManaWindow_newPro_Dialog::createUserFeedbackFormTable(const QString &dbName) {
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName(dbName);
    db.setUserName("mysql-user");
    db.setPassword("kjhl");
    db.setPort(63306);
    db.setHostName("111.120.13.93");

    // 尝试打开数据库连接
    if (!db.open()) return;

    QSqlQuery query;
    // 创建用户反馈表
    query.exec("CREATE TABLE IF NOT EXISTS User_Feedback_Form_Table ("
               "Module_Name VARCHAR(255), "
               "Feedback_Content TEXT, "
               "Feedback_Time DATETIME DEFAULT CURRENT_TIMESTAMP)");

    db.close(); // 关闭数据库连接
}

// 创建需求表
void C_ProManaWindow_newPro_Dialog::createRequirementTable(const QString &dbName) {
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName(dbName);
    db.setUserName("mysql-user");
    db.setPassword("kjhl");
    db.setPort(63306);
    db.setHostName("111.120.13.93");

    // 尝试打开数据库连接
    if (!db.open()) return;

    QSqlQuery query;
    // 创建需求表
    query.exec("CREATE TABLE IF NOT EXISTS Requirement_Table ("
               "ID INT AUTO_INCREMENT PRIMARY KEY, "
               "Customer_Name VARCHAR(255), "
               "Module_Name VARCHAR(255), "
               "Requirement_Summary TEXT, "
               "Requirement_Details TEXT, "
               "Priority INT, "
               "Priority_Category VARCHAR(255), "
               "Product_Design_Manager VARCHAR(255), "
               "Design_Start_Time DATE, "
               "Design_End_Time DATE, "
               "UI_Designer VARCHAR(255), "
               "UI_Start_Time DATE, "
               "UI_End_Time DATE, "
               "Development_Engineer VARCHAR(255), "
               "Development_Start_Time DATE, "
               "Development_End_Time DATE, "
               "Testing_Engineer VARCHAR(255), "
               "Testing_Start_Time DATE, "
               "Testing_End_Time DATE, "
               "Deployment_Time DATE, "
               "Customer_Feedback TEXT, "
               "Others TEXT)");

    db.close(); // 关闭数据库连接
}


/*

// 获取选中的模块名
QString C_ProManaWindow_newPro_Dialog::getCheckedModuleNames() {
    QString moduleNames;
    // 遍历已选模块的布局
    for (int i = 0; i < ui->scrollArea_Checked->widget()->layout()->count(); ++i) {
        QLabel *label = qobject_cast<QLabel*>(ui->scrollArea_Checked->widget()->layout()->itemAt(i)->widget());
        if (label) {
            moduleNames += label->text() + "\n"; // 以换行符分隔
        }
    }
    return moduleNames.trimmed(); // 去掉最后的换行符
}

*/
