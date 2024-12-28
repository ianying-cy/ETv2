
#include "a_tasks_template_window.h"
#include "ui_a_tasks_template_window.h"

// 定义静态变量
int A_tasks_template_Window::lastTextX = 0; // 上一个文本的 X 坐标
int A_tasks_template_Window::numberOfSquares = 90; // 当前正方形的数量
int A_tasks_template_Window::lastStringWidth = 0; // 上一个字符串的宽度
static int std_module_name_Count = 0;





A_tasks_template_Window::A_tasks_template_Window(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::A_tasks_template_Window) {
    ui->setupUi(this); // 设置用户界面
}

A_tasks_template_Window::~A_tasks_template_Window() {
    delete ui; // 清理用户界面
}



void A_tasks_template_Window::init_Gante() {
    const int rectWidth = global_variablesRectWidth; // 模块矩形的宽度
    const int rectHeight = global_variablesRectHeight; // 模块矩形的高度
    const int spacing = 1; // 矩形之间的间距
    const int offset = global_variablesStartX; // 左右边距
    const int squareOffset = 5; // 正方形的偏移量

    // 定义标签名称
    QStringList labels = { "模块名称", "主任务", "子任务", "负责人" };

    // 计算场景的宽度
    int sceneWidth = offset * 2; // 初始宽度包含左右边距
    sceneWidth += labels.size() * (rectWidth + spacing); // 加上标签的宽度和间距

    lastStringWidth = sceneWidth; // 存储当前场景的宽度，以便后续使用

    const int rectWidth_1 = 20; // 正方形的宽度
    const int spacing_1 = 2; // 正方形之间的间距

    // 计算场景的最终宽度

    numberOfSquares=getMysqlColumnCount("tasks")-5;
    sceneWidth += numberOfSquares * (rectWidth_1 + spacing_1) + squareOffset;

    // 创建图形场景
    QGraphicsScene *scene = new QGraphicsScene(0, 0, sceneWidth, 500);



    // 添加标签矩形和文本
    for (int i = 0; i < labels.size(); ++i) {
        int x = offset + i * (rectWidth + spacing); // 计算每个标签的 X 坐标
        CustomTitleRectItem *rect = new CustomTitleRectItem(QRectF(x, offset, rectWidth, rectHeight)); // 创建矩形
        scene->addItem(rect); // 将矩形添加到场景中
        QGraphicsTextItem *textItem = scene->addText(labels[i]); // 创建文本项
        textItem->setPos(x + 5, offset + 5); // 设置文本位置
    }

    // 计算正方形的起始位置
    int startX = offset + labels.size() * (rectWidth + spacing) + squareOffset;
    global_var_StartX_NumberSquares = startX; // 存到全局变量里，后面的X坐标TaskMarkingColorItem方便统一对齐
    qDebug()<<"-------global_var_StartX_NumberSquares-----"<<global_var_StartX_NumberSquares;

    // 添加正方形和数字
    for (int i = 1; i <= numberOfSquares; ++i) {
        int x = startX + (i - 1) * (rectWidth_1 + spacing_1); // 计算每个正方形的 X 坐标
        NumberSquareItem *square = new NumberSquareItem(i, QRectF(x, offset, rectWidth_1, rectHeight), this); // 创建正方形
        scene->addItem(square); // 将正方形添加到场景中

        // 创建数字文本项
        QGraphicsTextItem *numberItem = scene->addText(QString::number(i));
        int textX = x + (rectWidth_1 - numberItem->boundingRect().width()) / 2; // 计算文本的 X 坐标
        int textY = offset + (rectHeight - numberItem->boundingRect().height()) / 2; // 计算文本的 Y 坐标

        numberItem->setPos(textX, textY); // 设置文本位置
        lastTextX = textX; // 更新上一个文本的 X 坐标

        // 将数字文本项存储在列表中
        numberItems.append(numberItem); // 假设你在类中定义了 QList<QGraphicsTextItem*> numberItems;
    }


    // 将场景设置到图形视图中
    ui->graphicsView->setScene(scene);
}



void A_tasks_template_Window::add_Square_Number() {
    QGraphicsScene *scene = ui->graphicsView->scene(); // 获取当前场景
    if (!scene) {
        qDebug() << "Scene is not valid!"; // 检查场景是否有效
        return;
    }

    int newNumber = numberOfSquares + 1; // 新的正方形编号
    const int rectWidth = global_variablesSquareWidth; // 正方形的宽度
    const int rectHeight = global_variablesSquareHeight; // 正方形的高度
    const int spacing = global_spacing; // 正方形之间的间距

    // 计算新正方形的 X 坐标
    int startX = lastTextX + 23; // 基于上一个文本的位置进行偏移

    // 创建新的 NumberSquareItem
    NumberSquareItem *newSquare = new NumberSquareItem(newNumber, QRectF(startX, 50, rectWidth, rectHeight), this);
    scene->addItem(newSquare); // 将新正方形添加到场景中

    // 创建数字文本项
    QGraphicsTextItem *numberItem = scene->addText(QString::number(newNumber));
    int textX = startX + (rectWidth - numberItem->boundingRect().width()) / 2; // 计算文本的 X 坐标
    int textY = 50 + (rectHeight - numberItem->boundingRect().height()) / 2; // 计算文本的 Y 坐标
    numberItem->setPos(textX, textY); // 设置文本位置

    numberItems.append(numberItem); // 将新的数字文本项存储到列表中

    numberOfSquares++; // 更新当前正方形数量
    lastTextX = lastTextX + 23; // 更新 lastTextX 为新的正方形位置

    // 更新场景的大小
    const int offset = 50; // 左右边距
    int newSceneWidth = lastStringWidth + offset + numberOfSquares * (rectWidth + spacing); // 计算新的场景宽度
    scene->setSceneRect(0, 0, newSceneWidth, 500); // 更新场景的大小

    // 输出调试信息
    qDebug() << "Adding square number:" << newNumber << "at position:" << startX;
}



void A_tasks_template_Window::remove_Square_Number() { // 在 A_tasks_template_Window 类中添加删除方块的方法
    // 获取当前场景
    QGraphicsScene *scene = ui->graphicsView->scene(); // 获取当前场景
    if (!scene) {
        qDebug() << "Scene is not valid!"; // 检查场景是否有效
        return;
    }

    // 获取场景中的所有项目
    QList<QGraphicsItem*> items = scene->items();
    if (!items.isEmpty()) {
        int maxNumber = 0; // 用于存储最大的数字
        NumberSquareItem *squareToRemove = nullptr; // 用于存储要删除的正方形
        QGraphicsTextItem *textToRemove = nullptr; // 用于存储要删除的文本项

        // 遍历所有项目，找到最大的数字及其对应的正方形
        for (QGraphicsItem *item : items) {
            NumberSquareItem *squareItem = dynamic_cast<NumberSquareItem*>(item);
            if (squareItem) {
                int number = squareItem->getNumber(); // 获取正方形的数字
                if (number > maxNumber) {
                    maxNumber = number; // 更新最大数字
                    squareToRemove = squareItem; // 记录要删除的正方形
                }
            }
        }

        // 如果找到了要删除的正方形
        if (squareToRemove) {
            // 找到对应的数字文本项
            int index = squareToRemove->getNumber() - 1; // 获取正方形的数字
            if (index >= 0 && index < numberItems.size()) {
                textToRemove = numberItems[index]; // 获取对应的数字文本项
            }

            // 从场景中移除最大数字文本项
            if (textToRemove) {
                scene->removeItem(textToRemove); // 从场景中移除数字文本项
                delete textToRemove; // 删除数字文本项以释放内存
                numberItems.removeAt(index); // 从列表中移除文本项
            }

            // 从场景中移除该正方形
            scene->removeItem(squareToRemove); // 从场景中移除该正方形
            delete squareToRemove; // 删除项目以释放内存
            numberOfSquares--; // 更新当前正方形数量

            // 更新 lastTextX
            lastTextX = lastTextX - 23; // 这里可以根据实际情况进行调整

            // 更新场景的大小
            const int offset = 50; // 左右边距
            int newSceneWidth = lastStringWidth + offset + numberOfSquares * (20 + 2); // 计算新的场景宽度
            scene->setSceneRect(0, 0, newSceneWidth, 500); // 更新场景的大小

            // 更新视图
            ui->graphicsView->update(); // 强制更新视图

            // 输出调试信息
            qDebug() << "Removed square number:" << maxNumber << ", new count:" << numberOfSquares;
        } else {
            qDebug() << "No square found to remove.";
        }
    }
}



int A_tasks_template_Window::getMysqlColumnCount(const QString &tableName) {
    // 创建 MySQL 数据库连接
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName("QT");
    db.setUserName("mysql-user");
    db.setPassword("kjhl");
    db.setPort(63306);
    db.setHostName("111.120.13.93");

    // 尝试打开数据库连接
    if (!db.open()) {
        QMessageBox::critical(nullptr, "Database Error", db.lastError().text()); // 如果失败，弹出错误信息
        return -1; // 返回 -1 表示错误
    }

    // 创建查询对象
    QSqlQuery query;

    // 执行查询以获取列信息
    query.exec("SELECT * FROM " + tableName + " LIMIT 1"); // 获取表的第一行数据

    // 检查查询是否成功
    if (!query.isSelect() || !query.exec()) {
        QMessageBox::critical(nullptr, "Query Error", query.lastError().text()); // 如果失败，弹出错误信息
        return -1; // 返回 -1 表示错误
    }

    // 获取列数
    int columnCount = query.record().count(); // 获取列的数量

    // 关闭数据库连接
    db.close();

    return columnCount; // 返回列数
}



void A_tasks_template_Window::fetchModuleNameCount() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL"); // 创建 MySQL 数据库连接
    db.setDatabaseName("QT"); // 设置数据库名称
    db.setUserName("mysql-user"); // 设置用户名
    db.setPassword("kjhl"); // 设置密码
    db.setPort(63306); // 设置端口
    db.setHostName("111.120.13.93"); // 设置主机名

    // 尝试打开数据库连接
    if (!db.open()) {
        QMessageBox::critical(nullptr, "Database Error", db.lastError().text()); // 如果失败，弹出错误信息
        return;
    }

    QSqlQuery query; // 创建 SQL 查询对象
    query.exec("SELECT COUNT(module_name) FROM ProductRequirements"); // 执行查询获取 module_name 的数量

    // 检查查询结果
    if (query.next()) {
        std_module_name_Count = query.value(0).toInt(); // 获取数量并存储到全局静态变量
    }

    db.close(); // 关闭数据库连接

    // 输出调试信息
    qDebug() << "Fetched module name count:" << std_module_name_Count;
}

// 从数据库中获取模块名称
void A_tasks_template_Window::fetchModuleNameFromDatabase() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName("QT");
    db.setUserName("mysql-user");
    db.setPassword("kjhl");
    db.setPort(63306);
    db.setHostName("111.120.13.93");

    // 打开数据库连接
    if (!db.open()) {
        QMessageBox::critical(nullptr, "Database Error", db.lastError().text());
        return;
    }

    QSqlQuery query;
    query.exec("SELECT module_name FROM ProductRequirements");

    // 获取模块名称
    if (query.next()) {
        std_module_name = query.value(0).toString();
    }

    db.close(); // 关闭数据库连接
}

void A_tasks_template_Window::Adding_data(const QString &data) { // 添加数据到数据库
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName("QT");
    db.setUserName("mysql-user");
    db.setPassword("kjhl");
    db.setPort(63306);
    db.setHostName("111.120.13.93");

    // 打开数据库连接
    if (!db.open()) {
        QMessageBox::critical(nullptr, "Database Error", db.lastError().text());
        return;
    }

    QSqlQuery query;
    // 准备插入数据的 SQL 语句
    query.prepare("INSERT INTO Tasks (task_name, module_name) VALUES (:task_name, :module_name)");
    query.bindValue(":task_name", data);
    query.bindValue(":module_name", std_module_name);

    // 执行插入操作
    if (!query.exec()) {
        QMessageBox::critical(nullptr, "Database Error", query.lastError().text());
    }

    db.close(); // 关闭数据库连接
}

void A_tasks_template_Window::change_data(int value) { // 更新数据库中的数据
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName("QT");
    db.setUserName("mysql-user");
    db.setPassword("kjhl");
    db.setPort(63306);
    db.setHostName("111.120.13.93");

    // 打开数据库连接
    if (!db.open()) {
        QMessageBox::critical(nullptr, "Database Error", db.lastError().text());
        return;
    }

    QSqlQuery query;
    // 准备更新数据的 SQL 语句
    query.prepare("UPDATE Tasks SET status = :status WHERE id = :id");
    query.bindValue(":status", value);
    query.bindValue(":id", 1); // 假设我们更新 ID 为 1 的任务

    // 执行更新操作
    if (!query.exec()) {
        QMessageBox::critical(nullptr, "Database Error", query.lastError().text());
    }

    db.close(); // 关闭数据库连接
}
