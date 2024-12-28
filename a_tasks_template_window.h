#ifndef A_TASKS_TEMPLATE_WINDOW_H
#define A_TASKS_TEMPLATE_WINDOW_H

#include "ui_a_tasks_template_window.h"
#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsView>
#include <QMenu>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QRegularExpression>
#include <QGraphicsSceneContextMenuEvent>
#include <QContextMenuEvent>
#include <QInputDialog>
#include <QDebug>


// 前向声明
class TargetTaskItem; // 在这里前向声明 TargetTaskItem

//使用下面的目的是方便统一修改画图的高和宽；

static int global_variablesRectHeight = 40;                     //长方形的高
static int global_variablesRectWidth = 120;                     //长方形的宽
static int global_spacing =2;
static int global_variablesSquareHeight = 40;                   //正方形的高
static int global_variablesSquareWidth = 20;                    //正方形的宽
static int global_InsertDatabaseId = 0;                         // 在CustomRectItem 类中，当插入了一行数据库后记录ID 方便给TargetTaskItem类使用。
static int global_RecordX = 0;                                  // 记录画的图形画的的位置
static int global_RecordY = 0;                                  // 记录画的图形画的的位置
static int var_global_DayCountInDatabase = 0;                   // 记录最大天数

static int global_variablesStartX = 50;

static int global_var_StartX_NumberSquares= 539;                //正方形的时间的X坐标

namespace Ui {
class A_tasks_template_Window; // 前向声明用户界面类
}

// ++++++++++++++++++++++++++++++++++++  ModuleNameItem 类  ++++++++++++++++++++++++++++++++++++
class ModuleNameItem : public QObject, public QGraphicsRectItem {
    Q_OBJECT

public:
    // 构造函数，接收矩形区域和父项
    ModuleNameItem(const QRectF &rect, QGraphicsItem *parent = nullptr)
        : QGraphicsRectItem(rect, parent) {
        setBrush(Qt::cyan); // 设置填充颜色为青色
        setFlag(QGraphicsItem::ItemIsSelectable); // 允许该项被选中

        textItem = new QGraphicsTextItem(this); // 创建文本项，作为当前类的子项
        textItem->setDefaultTextColor(Qt::black); // 设置文本颜色为黑色
        QFont font("Arial", 14, QFont::Bold); // 设置字体为 Arial，大小为 14，粗体
        textItem->setFont(font); // 应用字体设置

        // 设置文本项位置为矩形中心
        updateTextPosition(); // 更新文本位置，使其居中
    }

    // 设置模块名称文本
    void setModuleNameText(const QString &text) {
        textItem->setPlainText(text); // 设置文本内容
        updateTextPosition(); // 更新文本位置
    }

protected:
    // 重写鼠标双击事件
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override {

        bool ok;

        // 弹出输入对话框，让用户输入模块名称
        QString text = QInputDialog::getText(nullptr, "输入模块名称", "模块名称:", QLineEdit::Normal, "", &ok);
        if (ok && !text.isEmpty()) { // 如果用户点击 OK 并且输入不为空
            setToolTip(text); // 设置工具提示为输入的文本
            editTaskInDatabase(global_InsertDatabaseId,text); // 调用编辑数据库的方法
            emit textEdited(text); // 发出信号
            setModuleNameText(text); // 更新模块名称文本

        }
        QGraphicsRectItem::mouseDoubleClickEvent(event); // 调用基类处理
    }

    // 重写右键菜单事件
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override {
        QMenu menu; // 创建右键菜单
        QAction *viewDetailsAction = menu.addAction("查看详情"); // 添加查看详情选项
        QAction *selectedAction = menu.exec(event->screenPos()); // 显示菜单并获取用户选择
        if (selectedAction == viewDetailsAction) {
            viewDetails(); // 调用查看详情方法
        }
    }



private:
    QGraphicsTextItem *textItem; // 文本项指针，用于显示模块名称

    QString requestTaskName() {

        QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
        db.setHostName("111.120.13.93");
        db.setPort(63306);
        db.setUserName("mysql-user");
        db.setPassword("kjhl");
        db.setDatabaseName("QT");

        if (!db.open()) {
            QMessageBox::critical(nullptr, "Database Error", db.lastError().text());
            return QString(); // 返回空字符串表示失败
        }

        // 查询数据库中的某列数据
        QVector<QString> taskNames;
        QSqlQuery query("SELECT module_name FROM ProductRequirements"); // 替换为你的表名
        if (!query.exec()) {
            QMessageBox::critical(nullptr, "Query Error", query.lastError().text());
            return QString();
        }

        while (query.next()) {
            taskNames.append(query.value(0).toString());
        }
        qDebug()<<taskNames;
        if (taskNames.isEmpty()) {
            QMessageBox::information(nullptr, "信息", "没有可用的任务名称。");
            return QString(); // 返回空字符串表示没有可用的任务名称
        }

        // 获取用户输入或选择
        bool ok;
        QString inputText = QInputDialog::getItem(nullptr, "输入任务名称", "任务名称:", taskNames.toList(), 0, false, &ok);
        if (ok && !inputText.isEmpty()) {
            return inputText; // 返回用户选择的任务名称
        }

        // 如果用户没有选择，允许用户输入自定义名称
        inputText = QInputDialog::getText(nullptr, "输入任务名称", "任务名称:", QLineEdit::Normal, "", &ok);
        if (ok && !inputText.isEmpty()) {
            return inputText; // 返回用户自定义的任务名称
        }

        return QString(); // 返回空字符串表示没有输入
    }


    // 更新文本位置，使其居中
    void updateTextPosition() {
        // 计算文本的 X 坐标，使其水平居中
        qreal x = global_variablesStartX+10; // 加30的目的是对齐
        // 计算文本的 Y 坐标，使其垂直居中
        qreal y = (rect().height() - textItem->boundingRect().height()) / 2+global_RecordY;
        textItem->setPos(x, y); // 设置文本位置
    }
private slots:

    void viewDetails() {
        QMessageBox::information(nullptr, "模块详情", "模块的详细信息。"); // 弹出信息框显示模块的详细信息
    }


    void editTaskInDatabase(int taskId, const QString &mainTask) {

        QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
        db.setDatabaseName("QT");
        db.setUserName("mysql-user");
        db.setPassword("kjhl");
        db.setPort(63306);
        db.setHostName("111.120.13.93");

        // 尝试打开数据库连接
        if (!db.open()) {
            QMessageBox::critical(nullptr, "数据库错误", db.lastError().text());
            return; // 退出函数
        }

        // 准备 SQL 查询，构建更新语句
        QString sql = "UPDATE tasks SET main_task = ? WHERE id = ?";
        QSqlQuery query; // 创建一个 SQL 查询对象
        query.prepare(sql); // 准备 SQL 语句

        // 绑定值到占位符
        query.addBindValue(mainTask); // main_task 的新值
        query.addBindValue(taskId);    // 要更新的任务 ID

        // 输出调试信息
        qDebug() << "Updating task ID:" << taskId << "with main task:" << mainTask;

        // 执行更新操作
        if (!query.exec()) {
            // 如果执行失败，弹出错误消息框
            QMessageBox::critical(nullptr, "更新错误", query.lastError().text());
        } else {
            // 如果成功，弹出成功消息框
            QMessageBox::information(nullptr, "成功", "任务已成功更新到数据库中。");
        }

        db.close(); // 关闭数据库连接
    }
private:
    int insertedId; // 用于存储最后插入行的 ID
    QString mainTaskValue; // 存储主任务的值

signals:
    void textEdited(const QString &newText); // 添加信号
};


// ++++++++++++++++++++++++++++++++++++  TaskNameItem 类  ++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++  TaskNameItem 类  ++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++  TaskNameItem 类  ++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++  TaskNameItem 类  ++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++  TaskNameItem 类  ++++++++++++++++++++++++++++++++++++


class TaskNameItem : public QObject, public QGraphicsRectItem {
    Q_OBJECT

public:
    TaskNameItem(const QRectF &rect, QGraphicsItem *parent = nullptr)
        : QGraphicsRectItem(rect, parent) {
        setBrush(Qt::cyan); // 设置填充颜色
        textItem = new QGraphicsTextItem(this);
        textItem->setDefaultTextColor(Qt::black);
        QFont font("Arial", 14, QFont::Bold);
        textItem->setFont(font);
        updateTextPosition();
    }

    void setTaskNameText(const QString &text) {
        textItem->setPlainText(text);
        textItem->setTextWidth(rect().width()); // 设置文本宽度限制

        // 设置文本高度限制
        qreal maxHeight = rect().height(); // 获取矩形的高度
        qreal textHeight = textItem->boundingRect().height(); // 获取文本的高度
        if (textHeight > maxHeight) {
            // 如果文本高度超过矩形高度，则裁剪文本
            QString truncatedText = truncateText(text, maxHeight);
            textItem->setPlainText(truncatedText);
        }

        updateTextPosition();
    }

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override {
        bool ok;
        QString text = QInputDialog::getText(nullptr, "输入任务名称", "任务名称:", QLineEdit::Normal, "", &ok);
        if (ok && !text.isEmpty()) {
            setToolTip(text);
            emit textEdited(text); // 发出信号
            setTaskNameText(text);
        }
        QGraphicsRectItem::mouseDoubleClickEvent(event);
    }

    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override {
        QMenu menu;
        QAction *viewDetailsAction = menu.addAction("查看详情");
        QAction *selectedAction = menu.exec(event->screenPos());
        if (selectedAction == viewDetailsAction) {
            viewDetails();
        }
    }

private slots:
    void viewDetails() {

        // 获取文本项的内容
        QString taskName = textItem->toPlainText(); // 获取文本内容

        // 使用 HTML 标签来设置文本内容的左对齐
        QString htmlContent = "<div style='text-align: left;'>" + taskName + "</div>";

        // 弹出信息框显示任务详情，设置文本格式为 RichText
        QMessageBox msgBox;
        msgBox.setWindowTitle("任务详情");
        msgBox.setTextFormat(Qt::RichText);
        msgBox.setText(htmlContent); // 设置文本内容
        msgBox.exec(); // 显示信息框
    }
signals:
    void textEdited(const QString &newText); // 添加信号
private:
    QGraphicsTextItem *textItem;
    void updateTextPosition() {
        // 使用矩形的 X 和 Y 坐标来更新文本项的位置
        qreal x = rect().x(); // 文本左对齐
        qreal y = rect().y() + (rect().height() - textItem->boundingRect().height()) / 2; // 垂直居中
        textItem->setPos(x, y); // 设置文本位置
    }


    QString truncateText(const QString &text, qreal maxHeight) {
        // 逐渐裁剪文本直到它的高度符合限制
        QString truncatedText = text;
        while (textItem->boundingRect().height() > maxHeight && !truncatedText.isEmpty()) {
            truncatedText.chop(1); // 去掉最后一个字符
            textItem->setPlainText(truncatedText + "..."); // 添加省略号
        }
        return truncatedText;
    }

};

// ++++++++++++++++++++++++++++++++++++  RespNameItem 类  ++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++  RespNameItem 类  ++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++  RespNameItem 类  ++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++  RespNameItem 类  ++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++  RespNameItem 类  ++++++++++++++++++++++++++++++++++++

class RespNameItem : public QObject, public QGraphicsRectItem {
    Q_OBJECT

public:
    // 构造函数，设置矩形区域和父项
    RespNameItem(const QRectF &rect, QGraphicsItem *parent = nullptr)
        : QGraphicsRectItem(rect, parent) {
        setBrush(Qt::green); // 设置填充颜色为浅绿色
        textItem = new QGraphicsTextItem(this); // 创建文本项
        textItem->setDefaultTextColor(Qt::black); // 设置文本颜色为黑色
        QFont font("Arial", 14, QFont::Bold); // 设置字体
        textItem->setFont(font);
        updateTextPosition(); // 更新文本位置
    }

    // 设置响应者名称文本
    void setRespNameText(const QString &text) {
        textItem->setPlainText(text); // 设置文本内容
        updateTextPosition(); // 更新文本位置
    }
signals:
    void textEdited(const QString &newText); // 添加信号
protected:
    // 重写鼠标双击事件
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override {
        bool ok;
        // 弹出输入对话框，让用户输入响应者名称
        QString text = QInputDialog::getText(nullptr, "输入响应者名称", "响应者名称:", QLineEdit::Normal, "", &ok);
        if (ok && !text.isEmpty()) {
            setToolTip(text); // 设置工具提示
            emit textEdited(text); // 发出信号
            setRespNameText(text); // 更新响应者名称文本
        }
        QGraphicsRectItem::mouseDoubleClickEvent(event); // 调用基类处理
    }

    // 重写右键菜单事件
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override {
        QMenu menu; // 创建右键菜单
        QAction *viewDetailsAction = menu.addAction("查看详情"); // 添加查看详情选项
        QAction *selectedAction = menu.exec(event->screenPos()); // 显示菜单并获取用户选择
        if (selectedAction == viewDetailsAction) {
            viewDetails(); // 调用查看详情方法
        }
    }

private slots:
    // 查看详情的槽函数
    void viewDetails() {
        QMessageBox::information(nullptr, "响应者详情", "响应者的详细信息。"); // 弹出信息框显示详情
    }

private:
    QGraphicsTextItem *textItem; // 文本项指针
    // 更新文本位置，使其居中

    void updateTextPosition() {
        // 使用矩形的 X 和 Y 坐标来更新文本项的位置
        qreal x = rect().x() + (rect().width() - textItem->boundingRect().width()) / 2; // 水平居中
        qreal y = rect().y() + (rect().height() - textItem->boundingRect().height()) / 2; // 垂直居中
        textItem->setPos(x, y); // 设置文本位置
    }
};


// ++++++++++++++++++++++++++++++++++++  TaskMarkingColorItem 类  ++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++  TaskMarkingColorItem 类  ++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++  TaskMarkingColorItem 类  ++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++  TaskMarkingColorItem 类  ++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++  TaskMarkingColorItem 类  ++++++++++++++++++++++++++++++++++++




class TaskMarkingColorItem : public QGraphicsRectItem {
public:
    TaskMarkingColorItem(const QRectF &rect, int itemId, const QString &fieldToUpdate, QGraphicsItem *parent = nullptr)
        : QGraphicsRectItem(rect, parent), id(itemId), field(fieldToUpdate), isFilled(false) {
        setBrush(Qt::white); // 初始填充颜色为白色
    }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override {
        toggleFill(); // 切换填充状态
        updateDatabase(); // 更新数据库
        QGraphicsRectItem::mousePressEvent(event); // 调用基类处理
    }

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override {
        if (event->buttons() & Qt::LeftButton) { // 检查左键是否按下
            // 这里可以选择是否在拖动时也进行填充状态的切换
        }
        QGraphicsRectItem::mouseMoveEvent(event); // 调用基类处理
    }

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override {
        // 检查从按下到释放过程中，鼠标覆盖的所有项
        QList<QGraphicsItem*> items = this->scene()->items(event->scenePos());
        for (QGraphicsItem* item : items) {
            // 检查是否为 TaskMarkingColorItem
            TaskMarkingColorItem* markingItem = dynamic_cast<TaskMarkingColorItem*>(item);
            if (markingItem) {
                markingItem->toggleFill(); // 切换填充状态
                markingItem->updateDatabase(); // 更新数据库
            }
        }
        QGraphicsRectItem::mouseReleaseEvent(event); // 调用基类处理
    }

public:
    void toggleFill() {
        isFilled = !isFilled; // 切换填充状态
        setBrush(isFilled ? Qt::green : Qt::white); // 根据状态设置填充颜色
    }

    void updateDatabase() {
        QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
        db.setDatabaseName("QT");
        db.setUserName("mysql-user");
        db.setPassword("kjhl");
        db.setPort(63306);
        db.setHostName("111.120.13.93");

        if (!db.open()) {
            QMessageBox::critical(nullptr, "数据库错误", db.lastError().text());
            return;
        }

        QSqlQuery query;
        query.prepare(QString("UPDATE tasks SET %1 = ? WHERE id = ?").arg(field)); // 使用参数化查询
        query.addBindValue(isFilled ? 1 : 0); // 1 表示填充，0 表示未填充
        query.addBindValue(id); // 绑定 ID

        if (!query.exec()) {
            QMessageBox::critical(nullptr, "更新错误", query.lastError().text());
        }

        db.close();
    }

private:
    int id; // 记录 ID
    QString field; // 要更新的字段
    bool isFilled; // 记录当前填充状态
};


class MovableItemGroup : public QGraphicsItemGroup {
public:
    MovableItemGroup() {
        setFlag(QGraphicsItem::ItemIsMovable); // 设置组可移动
    }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override {
        QGraphicsItemGroup::mousePressEvent(event); // 调用基类处理
        // 记录初始位置
        initialPos = event->scenePos();
    }

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override {
        // 计算移动的偏移量
        QPointF offset = event->scenePos() - initialPos;
        moveBy(offset.x(), offset.y()); // 移动组
        initialPos = event->scenePos(); // 更新初始位置
        QGraphicsItemGroup::mouseMoveEvent(event); // 调用基类处理
    }

private:
    QPointF initialPos; // 记录初始位置
};





// ++++++++++++++++++++++++++++++++++++  TargetTaskItem 类  ++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++  TargetTaskItem 类  ++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++  TargetTaskItem 类  ++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++  TargetTaskItem 类  ++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++  TargetTaskItem 类  ++++++++++++++++++++++++++++++++++++




class TargetTaskItem : public QObject, public QGraphicsRectItem {
    Q_OBJECT // 添加 Q_OBJECT 宏以支持信号和槽机制

public:
    // 构造函数，初始化矩形项
    TargetTaskItem(const QRectF &rect, QGraphicsItem *parent = nullptr)
        : QGraphicsRectItem(rect, parent),isExpanded(false) {
        setBrush(Qt::yellow); // 设置矩形的填充颜色为黄色
        setFlag(QGraphicsItem::ItemIsSelectable); // 允许该项被选中

        // 初始化文本项
        textItem = new QGraphicsTextItem(this); // 创建文本项并将其设置为当前项的子项
        textItem->setDefaultTextColor(Qt::black);

        // 设置字体、大小和粗细
        QFont font("Arial", 18, QFont::Bold);
        textItem->setFont(font); // 应用字体

        // 设置文本项位置为矩形中心
        updateTextPosition(); // 初始化文本位置
    }


public:
    // 用于设置主任务文本
    void setMainTaskText(const QString &text) {
        textItem->setPlainText(text); // 更新文本内容
        updateTextPosition(); // 更新文本位置
    }

    // 记录插入的 ID
    void recordInsertedId(int id) {
        insertedIds.append(id);
    }


private:
    QGraphicsTextItem *textItem;

    // 更新文本项的位置，使其居中
    void updateTextPosition() {
        int spacing =14;
        qreal x = ((rect().width() - textItem->boundingRect().width()) /2)+spacing+global_variablesStartX; // 水平居中
        qreal y = (rect().height() - textItem->boundingRect().height()) / 2 + global_RecordY; // 垂直居中并加上 global_RecordY
        textItem->setPos(x, y); // 设置位置
    }


protected:
    // 鼠标双击事件处理
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override {
        bool ok;
        // 弹出输入对话框，获取用户输入的目标名称
        QString text = QInputDialog::getText(nullptr, "输入目标", "目标名称:", QLineEdit::Normal, "", &ok);
        if (ok && !text.isEmpty()) { // 如果用户确认并且输入不为空
            setToolTip(text); // 设置工具提示为输入的目标名称

            int currentId = insertedIds.last(); // 获取当前任务 ID
            editTaskInDatabase(global_InsertDatabaseId,text); // 调用编辑数据库的方法
            setMainTaskText(text); // 更新模块名称文本

        }
        QGraphicsRectItem::mouseDoubleClickEvent(event); // 调用父类的事件处理
        // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        //
        // QGraphicsRectItem::mouseDoubleClickEvent(event); 这一行代码的作用是调用父类 QGraphicsRectItem 的 mouseDoubleClickEvent 方法，
        // 以确保在处理双击事件时，父类的默认行为也能得到执行。
        // 具体作用
        //    1、继承性：在 Qt 的事件处理机制中，子类可以重写父类的事件处理方法，以实现特定的功能。在这个例子中，TargetTaskItem 继承自 QGraphicsRectItem，
        //      并重写了 mouseDoubleClickEvent 方法。当用户在 TargetTaskItem 上双击时，会触发这个事件。

        //    2、默认行为：调用 QGraphicsRectItem 的 mouseDoubleClickEvent 方法可以保证父类的默认行为得到执行。这可能包括一些基本的视觉反馈或状态更新，
        //       例如选中状态的切换、焦点的设置等。这样可以确保在自定义行为（如弹出输入对话框）之外，父类的功能也能正常工作。

        //    3、避免功能丢失：如果重写事件处理方法时不调用父类的实现，可能会导致一些默认行为被完全忽略。
        //       例如，QGraphicsRectItem 可能会有一些内置的功能（例如选中、拖动等），如果不调用父类的方法，这些功能可能会失效。

        // 结论
        //  在重写事件处理方法时，通常建议在自定义逻辑的最后调用父类的实现，以确保父类的行为被保留，避免潜在的功能丢失。这是一种良好的编程实践，特别是在使用 Qt 这样的框架时。
        // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    }


    // 右键菜单事件处理
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override {
        QMenu menu; // 创建上下文菜单
        // 添加菜单项
        QAction *addTargetAction = menu.addAction("添加目标");
        QAction *addTasksAction = menu.addAction("添加任务");
        QAction *deleteTargetAction = menu.addAction("删除目标");
        QAction *deleteTaskAction = menu.addAction("删除任务");
        QAction *viewDetailsAction = menu.addAction("查看详情");



        // 根据当前状态添加菜单项
        if (isExpanded) {
            QAction *collapseAction = menu.addAction("折叠"); // 添加折叠选项
            connect(collapseAction, &QAction::triggered, this, &TargetTaskItem::collapse); // 连接信号与槽
        } else {
            QAction *expandAction = menu.addAction("展开"); // 添加展开选项
            connect(expandAction, &QAction::triggered, this, &TargetTaskItem::expand); // 连接信号与槽
        }
        // 显示菜单并获取用户选择的操作
        QAction *selectedAction = menu.exec(event->screenPos());//显示菜单并获取用户选择


        // 根据选择的操作调用相应的槽函数
        if (selectedAction == addTargetAction) {
            addTarget(); // 添加目标
        } else if (selectedAction == addTasksAction) {
            addTasks(); // 添加任务
        } else if (selectedAction == deleteTargetAction) {
            deleteTarget(); // 删除目标
        }else if (selectedAction == deleteTaskAction) {
            deleteTask(); // 删除任务
        }else if (selectedAction == viewDetailsAction) {
            viewDetails(); // 查看目标详情
        }


    }


private slots:
    // 添加目标的槽函数
    void addTarget() {
        QMessageBox::information(nullptr, "添加目标", "添加目标的功能尚未实现。");

        if (insertOneRowTaskIntoDatabase()) { // 如果插入数据库成功

            // 获取当前 CustomRectItem 的位置和宽度
            QRectF rect = this->rect(); // 获取 CustomRectItem 的矩形
            qreal x = global_variablesStartX; // X 坐标
            qreal y = global_RecordY+45; // Y 坐标，向下偏移20像素

            // 计算 TargetTaskItem 的宽度
            QStringList labels = { "模块名称", "主任务", "子任务", "负责人" };
            const int rectWidth = 120; // 每个矩形的宽度
            const int spacing = 1; // 矩形之间的间距
            qreal targetWidth = labels.size() * (rectWidth + spacing) - spacing; // 计算总宽度

            // 弹出输入对话框，获取用户输入的主任务名称
            bool ok;
            QString mainTaskName = QInputDialog::getText(nullptr, "输入主任务", "主任务名称:", QLineEdit::Normal, "", &ok);
            if (ok && !mainTaskName.isEmpty()) { // 如果用户确认并且输入不为空
                // 更新数据库中的 mainTask 字段
                if (updateMainTaskInDatabase(mainTaskName, global_InsertDatabaseId)) {
                    // 创建 TargetTaskItem
                    TargetTaskItem *targetItem = new TargetTaskItem(QRectF(x, y, targetWidth, 40)); // 40 是高度
                    this->scene()->addItem(targetItem); // 将 TargetTaskItem 添加到场景中

                    // 更新 global_RecordY 后面文本时要用到

                    global_RecordY = y; // 存储当前 Y 轴位置

                    // 设置主任务文本
                    targetItem->setMainTaskText(mainTaskName); // 使用用户输入的主任务名称

                    // 更新场景的边界
                    QRectF sceneRect = this->scene()->sceneRect(); // 获取当前场景的矩形
                    sceneRect.setBottom(std::max(sceneRect.bottom(), y + global_variablesRectHeight)); // 更新底部边界
                    this->scene()->setSceneRect(sceneRect); // 设置新的场景矩形

                } else {
                    QMessageBox::warning(nullptr, "更新失败", "主任务名称更新数据库失败。");
                }
            } else {
                QMessageBox::warning(nullptr, "输入错误", "主任务名称不能为空。");
            }
        } else {
            // 插入失败，您可以在这里处理失败的情况
            QMessageBox::warning(nullptr, "插入失败", "任务插入数据库失败，无法创建新的任务项。");
        }
    }

    // 添加任务的槽函数

    void addTasks() {
        // 向数据库中插入一行；
        if (insertOneRowTaskIntoDatabase()) { // 如果插入数据库成功
            // 获取当前 Y 坐标
            qreal y = global_RecordY + 40; // 假设高度为 40

            // 创建三个自定义项
            ModuleNameItem *moduleItem = new ModuleNameItem(QRectF(global_variablesStartX, y, 120, 40));
            TaskNameItem *taskItem = new TaskNameItem(QRectF(global_variablesStartX + 120, y, 240, 40));
            RespNameItem *respItem = new RespNameItem(QRectF(global_variablesStartX + 360, y, 123, 40));

            // 连接信号与槽以编辑文本项时更新数据库
            // 连接信号与槽
            int currentId = insertedIds.last(); // 获取当前插入的任务 ID
            connect(moduleItem, &ModuleNameItem::textEdited, this, [this, currentId](const QString &newText) {
                editTaskText(currentId, newText, QString(), QString()); // 更新模块名称
            });
            connect(taskItem, &TaskNameItem::textEdited, this, [this, currentId](const QString &newText) {
                editTaskText(currentId, QString(), newText, QString()); // 更新任务名称
            });
            connect(respItem, &RespNameItem::textEdited, this, [this, currentId](const QString &newText) {
                editTaskText(currentId, QString(), QString(), newText); // 更新负责人名称
            });

            // 将它们添加到场景中
            this->scene()->addItem(moduleItem);
            this->scene()->addItem(taskItem);
            this->scene()->addItem(respItem);

            // 创建 TaskMarkingColorItem 的数量等于 var_global_DayCountInDatabase
            for (int i = 0; i < var_global_DayCountInDatabase; ++i) {
                // 计算每个 TaskMarkingColorItem 的 X 坐标
                qreal markingX = global_var_StartX_NumberSquares + (i * (global_variablesSquareWidth + global_spacing)); // 根据索引计算 X 坐标

                // 创建 TaskMarkingColorItem，假设 ID 为 currentId，字段为 "day" + (i + 1)
                TaskMarkingColorItem *markingItem = new TaskMarkingColorItem(QRectF(markingX, y, global_variablesSquareWidth, global_variablesSquareHeight), currentId, QString("day%1").arg(i + 1));
                this->scene()->addItem(markingItem); // 将 TaskMarkingColorItem 添加到场景中
            }

            // 更新场景的边界
            QRectF sceneRect = this->scene()->sceneRect(); // 获取当前场景的矩形
            sceneRect.setBottom(std::max(sceneRect.bottom(), y + global_variablesRectHeight)); // 更新底部边界
            this->scene()->setSceneRect(sceneRect); // 设置新的场景矩形

            // 确保文本项可见并更新位置
            moduleItem->setVisible(true);
            taskItem->setVisible(true);
            respItem->setVisible(true);

            // 更新 global_RecordY 后面文本时要用到
            global_RecordY = y; // 存储当前 Y 轴位置
        } else {
            // 插入失败，您可以在这里处理失败的情况
            QMessageBox::warning(nullptr, "插入失败", "任务插入数据库失败，无法创建新的任务项。");
        }
    }



    // 删除目标的槽函数
    void deleteTarget() {
        // 弹出确认对话框，询问用户是否确定删除
        if (QMessageBox::question(nullptr, "确认删除", "你确定要删除这个目标吗？") == QMessageBox::Yes) {
            scene()->removeItem(this); // 从场景中移除该项
            delete this; // 删除该项
        }
    }

    // 删除任务的槽函数
    void deleteTask() {
        if (QMessageBox::question(nullptr, "确认删除任务", "你确定要删除这个任务吗？") == QMessageBox::Yes) {
            removeTaskFromDatabase(); // 调用删除任务的方法

        }
    }

    // 查看目标详情的槽函数
    void viewDetails() {
        QMessageBox::information(nullptr, "目标详情", "目标的详细信息。");
    }



    // 将任务更新到数据库的方法
    void editTaskInDatabase(int taskId, const QString &mainTask) {
        // 创建一个 MySQL 数据库连接
        QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
        db.setDatabaseName("QT"); // 设置数据库名称
        db.setUserName("mysql-user"); // 设置数据库用户名
        db.setPassword("kjhl"); // 设置数据库密码
        db.setPort(63306); // 设置数据库端口
        db.setHostName("111.120.13.93"); // 设置数据库主机名

        // 尝试打开数据库连接
        if (!db.open()) {
            QMessageBox::critical(nullptr, "数据库错误", db.lastError().text());
            return; // 退出函数
        }

        // 准备 SQL 查询，构建更新语句
        QString sql = "UPDATE tasks SET main_task = ? WHERE id = ?";
        QSqlQuery query; // 创建一个 SQL 查询对象
        query.prepare(sql); // 准备 SQL 语句

        // 绑定值到占位符
        query.addBindValue(mainTask); // main_task 的新值
        query.addBindValue(taskId);    // 要更新的任务 ID

        // 输出调试信息
        qDebug() << "Updating task ID:" << taskId << "with main task:" << mainTask;

        // 执行更新操作
        if (!query.exec()) {

            QMessageBox::critical(nullptr, "更新错误", query.lastError().text());
        } else {

            QMessageBox::information(nullptr, "成功", "任务已成功更新到数据库中。");
        }

        db.close(); // 关闭数据库连接
    }


    // 将任务从数据库中删除的方法
    void removeTaskFromDatabase() {
        QMessageBox::information(nullptr, "删除任务", "删除任务的功能尚未实现。");
    }

    // 展开方法
    void expand() {
        isExpanded = true; // 设置状态为展开
        QMessageBox::information(nullptr, "展开功能", "展开功能还未实现，后期计划就是 点击了展开显示目标的任务。");
        // 创建并显示三个自定义项
        // addTask(); // 添加任务
        // update(); // 更新视图
    }

    // 折叠方法
    void collapse() {
        isExpanded = false; // 设置状态为折叠
        QMessageBox::information(nullptr, "折叠功能", "功能功能还未实现，后期计划就是 点击了折叠隐藏目标的任务，下面的依次往前移动。");
        // 隐藏所有自定义项
        // for (QGraphicsItem *item : childItems()) {
        //     item->hide(); // 隐藏子项
        // }
        // update(); // 更新视图
    }

    // 添加任务的方法
    void addTask() {




    }
    bool insertOneRowTaskIntoDatabase() {
        // 创建一个 MySQL 数据库连接
        QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
        db.setDatabaseName("QT"); // 设置数据库名称
        db.setUserName("mysql-user"); // 设置数据库用户名
        db.setPassword("kjhl"); // 设置数据库密码
        db.setPort(63306); // 设置数据库端口
        db.setHostName("111.120.13.93"); // 设置数据库主机名

        // 尝试打开数据库连接
        if (!db.open()) {
            // 如果连接失败，弹出错误消息框
            QMessageBox::critical(nullptr, "数据库错误", db.lastError().text());
            return false; // 返回 false 表示失败
        }

        // 查询任务表中的日期列数量
        QSqlQuery countQuery("SELECT COUNT(*) FROM information_schema.columns "
                             "WHERE table_name = 'tasks' AND column_name LIKE 'day%'");
        int dayCount = 0; // 初始化 dayCount

        // 执行查询并获取结果
        if (countQuery.next()) {
            dayCount = countQuery.value(0).toInt(); // 获取列数量
            var_global_DayCountInDatabase = dayCount; // 存到全局变量里去，创建任务画天数时用到。
        } else {
            QMessageBox::critical(nullptr, "查询错误", countQuery.lastError().text());
            db.close();
            return false;
        }

        // 创建动态的列名和占位符
        QStringList dayColumns;
        QStringList dayPlaceholders;

        // 为每个 day 列添加列名和占位符（?）
        for (int i = 1; i <= dayCount; ++i) {
            dayColumns << QString("day%1").arg(i); // 添加列名 day1, day2, ..., dayN
            dayPlaceholders << "?"; // 添加一个占位符
        }

        // 准备 SQL 查询，构建插入语句
        QString sql = QString("INSERT INTO tasks (module_name, main_task, sub_task, responsible_party, %1) "
                              "VALUES (?, ?, ?, ?, %2)")
                          .arg(dayColumns.join(", ")) // 动态列名
                          .arg(dayPlaceholders.join(", ")); // 动态占位符

        QSqlQuery query; // 创建一个 SQL 查询对象
        query.prepare(sql); // 准备 SQL 语句

        // 绑定值到占位符
        query.addBindValue("默认模块名称"); // module_name 的值
        query.addBindValue("请修改"); // main_task 的值
        query.addBindValue("默认子任务"); // sub_task 的值
        query.addBindValue("默认责任人"); // responsible_party 的值

        // 绑定每一天的值（这里假设为 0）
        for (int i = 0; i < dayCount; ++i) {
            query.addBindValue(0); // 绑定每个 day 列的值
        }

        // 执行插入操作
        if (!query.exec()) {
            // 如果执行失败，弹出错误消息框
            QMessageBox::critical(nullptr, "插入错误", query.lastError().text());
            db.close(); // 关闭数据库连接
            return false; // 返回 false 表示失败
        } else {
            // 如果成功，获取插入行的 ID
            int insertedId = query.lastInsertId().toInt(); // 获取并记录插入行的 ID
            recordInsertedId(insertedId); // 记录插入的 ID
            QMessageBox::information(nullptr, "成功", "任务已成功插入到数据库中。");
            db.close(); // 关闭数据库连接
            return true; // 返回 true 表示成功
        }
    }
private:
    bool isExpanded; // 状态标志，指示当前是展开还是折叠
    QList<int> insertedIds; // 存储插入的任务 ID 列表
    int insertedId; // 用于存储最后插入行的 ID
    QString mainTaskValue; // 存储主任务的值

private:
    bool updateMainTaskInDatabase(const QString &mainTask, int taskId) {

        QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
        db.setDatabaseName("QT");
        db.setUserName("mysql-user");
        db.setPassword("kjhl");
        db.setPort(63306);
        db.setHostName("111.120.13.93");

        // 尝试打开数据库连接
        if (!db.open()) {
            // 如果连接失败，弹出错误消息框
            QMessageBox::critical(nullptr, "数据库错误", db.lastError().text());
            return false; // 返回 false 表示失败
        }

        // 使用参数化查询更新 mainTask 字段
        QSqlQuery query;
        query.prepare("UPDATE tasks SET main_task = ? WHERE id = ?");
        query.addBindValue(mainTask); // 绑定主任务名称
        query.addBindValue(taskId); // 绑定任务 ID

        // 执行更新操作
        if (!query.exec()) {

            QMessageBox::critical(nullptr, "更新错误", query.lastError().text());
            return false; // 返回 false 表示失败
        }

        db.close();
        return true;
    }

    // 编辑任务的文本项时使用 ID
    void editTaskText(int taskId, const QString &moduleName, const QString &taskName, const QString &respName) {
        // 创建一个 MySQL 数据库连接
        QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
        db.setDatabaseName("QT"); // 设置数据库名称
        db.setUserName("mysql-user"); // 设置数据库用户名
        db.setPassword("kjhl"); // 设置数据库密码
        db.setPort(63306); // 设置数据库端口
        db.setHostName("111.120.13.93"); // 设置数据库主机名

        // 尝试打开数据库连接
        if (!db.open()) {
            QMessageBox::critical(nullptr, "数据库错误", db.lastError().text());
            return; // 退出函数
        }

        // 准备 SQL 查询，构建更新语句
        QString sql = "UPDATE tasks SET ";
        QStringList updates; // 用于存储更新字段的列表

        // 根据提供的参数构建更新语句
        if (!moduleName.isEmpty()) {
            updates.append("module_name = ?");
        }
        if (!taskName.isEmpty()) {
            updates.append("main_task = ?");
        }
        if (!respName.isEmpty()) {
            updates.append("responsible_party = ?");
        }

        // 将更新字段连接成字符串
        sql += updates.join(", ") + " WHERE id = ?"; // 追加条件

        QSqlQuery query; // 创建一个 SQL 查询对象
        query.prepare(sql); // 准备 SQL 语句

        // 绑定值到占位符
        int paramIndex = 0;
        if (!moduleName.isEmpty()) {
            query.addBindValue(moduleName); // 绑定模块名称
            paramIndex++;
        }
        if (!taskName.isEmpty()) {
            query.addBindValue(taskName); // 绑定任务名称
            paramIndex++;
        }
        if (!respName.isEmpty()) {
            query.addBindValue(respName); // 绑定负责人名称
            paramIndex++;
        }

        query.addBindValue(taskId); // 绑定任务 ID

        // 输出调试信息
        qDebug() << "Updating task ID:" << taskId << "with module name:" << moduleName << ", task name:" << taskName << ", responsible party:" << respName;

        // 执行更新操作
        if (!query.exec()) {
            // 如果执行失败，弹出错误消息框
            QMessageBox::critical(nullptr, "更新错误", query.lastError().text());
        } else {
            // 如果成功，弹出成功消息框
            QMessageBox::information(nullptr, "成功", "任务已成功更新到数据库中。");
        }

        db.close(); // 关闭数据库连接
    }

};



// ++++++++++++++++++++++++++++++++++++  A_tasks_template_Window 类  ++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++  A_tasks_template_Window 类  ++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++  A_tasks_template_Window 类  ++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++  A_tasks_template_Window 类  ++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++  A_tasks_template_Window 类  ++++++++++++++++++++++++++++++++++++



// A_tasks_template_Window 类继承自 QMainWindow
class A_tasks_template_Window : public QMainWindow {
    Q_OBJECT // Qt 的元对象系统需要这个宏

public:
    explicit A_tasks_template_Window(QWidget *parent = nullptr); // 构造函数
    ~A_tasks_template_Window(); // 析构函数
private:
    Ui::A_tasks_template_Window *ui; // 用户界面指针



    // 以下是自己定义的
public:
    void init_Gante(); // 初始化甘特第一列方法

    void add_Square_Number(); // 新增方法，添加新的正方形

    void remove_Square_Number();

    int getMysqlColumnCount(const QString &tableName);

    void fetchModuleNameCount();

    void fetchModuleNameFromDatabase();// 从数据库中获取模块名称

    void change_data(int value);// 更新数据库中的数据

    void Adding_data(const QString &data);// 添加数据到数据库

private:

    static int numberOfSquares; // 静态变量，用于存储当前正方形的数量
    static int lastTextX; // 静态变量，存储上一个文本的 X 坐标
    static int lastStringWidth; // 静态变量，存储上一个字符串的宽度
    QString std_module_name; // 用于存储模块名称
    QList<QGraphicsTextItem*> numberItems; // 存储数字文本项的列表





};



// ++++++++++++++++++++++++++++++++++++  CustomTitleRectItem  类  ++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++  CustomTitleRectItem  类  ++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++  CustomTitleRectItem  类  ++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++    ++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++    ++++++++++++++++++++++++++++++++++++


class CustomTitleRectItem : public QObject, public QGraphicsRectItem {
    Q_OBJECT
public:
    CustomTitleRectItem(const QRectF &rect, QGraphicsItem *parent = nullptr)
        : QGraphicsRectItem(rect, parent), canAddTarget(true), insertedId(-1) { // 初始化 canAddTarget是一个布尔变量，用于跟踪是否允许在上下文菜单中添加“增加初始目标”选项。它的主要作用是在用户成功插入一个新任务后，阻止再次添加同样的菜单项，从而避免重复操作。
        setBrush(Qt::green); // 设置矩形的填充颜色为绿色
    }

protected:

    // 重写上下文菜单事件
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override {
        QMenu menu; // 创建上下文菜单

        // 根据 canAddTarget 的值决定是否添加“增加初始目标”选项
        if (canAddTarget) {
            QAction *newTargetAction = menu.addAction("增加初始目标");
            connect(newTargetAction, &QAction::triggered, this, &CustomTitleRectItem::AddInit_FristTarget);
        }

        // 添加“展开”和“折叠”选项
        QAction *expandAllTarget = menu.addAction("展开所有目标");
        connect(expandAllTarget, &QAction::triggered, this, &CustomTitleRectItem::expandTarget);

        QAction *collapseAllTarget = menu.addAction("折叠所有目标");
        connect(collapseAllTarget, &QAction::triggered, this, &CustomTitleRectItem::collapseTarget);

        // 获取视图并计算全局位置
        QGraphicsView *view = dynamic_cast<QGraphicsView*>(this->scene()->views().at(0));
        if (view) {
            QPoint globalPos = view->mapToGlobal(event->scenePos().toPoint()); // 转换为全局坐标
            menu.exec(globalPos); // 显示菜单
        }
    }

private slots:
    // 这个注释不能删除， 只要添加了这个会编译会报错“error: Class declaration lacks Q_OBJECT macro.”
    //    解决办法是添加Q_OBJECT这个宏，同时这个类必须增加继承public QObject

    /*
     * 第一步：计算画TargetTaskItem图的位置x y
     * 第二步：调用QInputDialog输入文本
     *          如果输入的文本不是空的
     *                   1、创建 TargetTaskItem
     *                   2、更新 Y 轴坐标
     *                   3、更新 canAddTarget 为 false 右键菜单就不显示了
     *
     *                   4、ID传给函数updateMainTaskInDatabase 更新数据库内容，
     *
     *                   5、调用TargetTaskItem的方法 更新文本内容
     *
     *          如果为空给出提示要求输入
     *
     *
     *
    */
    void AddInit_FristTarget() {

        if (insertOneRowTaskIntoDatabase()) { // 如果插入数据库成功

            // 获取当前 CustomRectItem 的位置和宽度
            QRectF rect = this->rect(); // 获取 CustomRectItem 的矩形
            qreal x = global_variablesStartX; // X 坐标
            qreal y = rect.y() + rect.height() + 20; // Y 坐标，向下偏移20像素

            // 计算 TargetTaskItem 的宽度
            QStringList labels = { "模块名称", "主任务", "子任务", "负责人" };
            const int rectWidth = 120; // 每个矩形的宽度
            const int spacing = 1; // 矩形之间的间距
            qreal targetWidth = labels.size() * (rectWidth + spacing) - spacing; // 计算总宽度

            // 弹出输入对话框，获取用户输入的主任务名称
            bool ok;
            QString mainTaskName = QInputDialog::getText(nullptr, "输入主任务", "主任务名称:", QLineEdit::Normal, "", &ok);
            if (ok && !mainTaskName.isEmpty()) { // 如果用户确认并且输入不为空
                // 更新数据库中的 mainTask 字段
                if (updateMainTaskInDatabase(mainTaskName, global_InsertDatabaseId)) {
                    // 创建 TargetTaskItem
                    TargetTaskItem *targetItem = new TargetTaskItem(QRectF(x, y, targetWidth, 40)); // 40 是高度
                    this->scene()->addItem(targetItem); // 将 TargetTaskItem 添加到场景中

                    global_RecordY = y; // 存储当前 Y 轴位置 当执行下面的函数的时候 方便文本放到正确的位置

                    // 设置主任务文本
                    targetItem->setMainTaskText(mainTaskName); // 使用用户输入的主任务名称

                    // 更新 global_RecordY 后面文本时要用到

                    global_RecordY = y; // 存储当前 Y 轴位置
                    // 设置 canAddTarget 为 false，表示已经添加了目标
                    canAddTarget = false;
                } else {
                    QMessageBox::warning(nullptr, "更新失败", "主任务名称更新数据库失败。");
                }
            } else {
                QMessageBox::warning(nullptr, "输入错误", "主任务名称不能为空。");
            }
        } else {
            // 插入失败，您可以在这里处理失败的情况
            QMessageBox::warning(nullptr, "插入失败", "任务插入数据库失败，无法创建新的任务项。");
        }
    }


    // 添加展开目标的槽函数
    void expandTarget() {
        QMessageBox::information(nullptr, "展开所有目标", "展开所有目标的功能暂未实现");
    }

    // 添加折叠目标的槽函数
    void collapseTarget() {
        QMessageBox::information(nullptr, "折叠所有目标", "折叠所有目标的功能暂未实现");
    }

private:

    //往数据库中插入一行，插入之前先去判断了day1-N 有多少个哥，然后再去插入。同时记录插入的ID

    bool insertOneRowTaskIntoDatabase() {

        QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
        db.setDatabaseName("QT");
        db.setUserName("mysql-user");
        db.setPassword("kjhl");
        db.setPort(63306);
        db.setHostName("111.120.13.93");


        if (!db.open()) {
            QMessageBox::critical(nullptr, "数据库错误", db.lastError().text());
            return false;
        }

        // 查询数据库任务表中的日期列数量
        QSqlQuery countQuery("SELECT COUNT(*) FROM information_schema.columns "
                             "WHERE table_name = 'tasks' AND column_name LIKE 'day%'");
        int dayCount = 0; // 初始化 dayCount

        // 执行查询并获取结果
        if (countQuery.next()) {
            dayCount = countQuery.value(0).toInt(); // 获取列数量

        } else {
            QMessageBox::critical(nullptr, "查询错误", countQuery.lastError().text());
            db.close(); // 关闭数据库连接
            return false; // 返回 false 表示失败
        }

        qDebug() << "day------->" << dayCount;

        // 创建动态的列名和占位符
        QStringList dayColumns;
        QStringList dayPlaceholders;

        // 为每个 day 列添加列名和占位符（?）
        for (int i = 1; i <= dayCount; ++i) {
            dayColumns << QString("day%1").arg(i); // 添加列名 day1, day2, ..., dayN
            dayPlaceholders << "?"; // 添加一个占位符
        }

        // 准备 SQL 查询，构建插入语句
        QString sql = QString("INSERT INTO tasks (module_name, main_task, sub_task, responsible_party, %1) "
                              "VALUES (?, ?, ?, ?, %2)")
                          .arg(dayColumns.join(", ")) // 动态列名
                          .arg(dayPlaceholders.join(", ")); // 动态占位符

        QSqlQuery query; // 创建一个 SQL 查询对象
        query.prepare(sql); // 准备 SQL 语句

        // 绑定值到占位符
        query.addBindValue("这个是任务，不用修改"); // module_name 的值
        query.addBindValue("请修改"); // main_task 的值
        query.addBindValue("这个是任务，不用修改"); // sub_task 的值
        query.addBindValue("这个是任务，不用修改"); // responsible_party 的值

        // 绑定每一天的值（这里假设为 0）
        for (int i = 0; i < dayCount; ++i) {
            query.addBindValue(0); // 绑定每个 day 列的值
        }

        // 执行插入操作
        if (!query.exec()) {
            // 如果执行失败，弹出错误消息框
            QMessageBox::critical(nullptr, "插入错误", query.lastError().text());
            return false; // 返回 false 表示失败
        } else {
            // 如果成功，弹出成功消息框
            // 获取插入行的 ID
            insertedId = query.lastInsertId().toInt(); // 记录插入行的 ID
            global_InsertDatabaseId = insertedId;
            mainTaskValue = "请修改"; // 保存 main_task 的值
            qDebug()<<"插入数据库ID为"<<global_InsertDatabaseId;
            QMessageBox::information(nullptr, "成功", "任务已成功插入到数据库中。");
            return true; // 返回 true 表示成功
        }

        db.close(); // 关闭数据库连接
    }



    bool updateMainTaskInDatabase(const QString &mainTask, int taskId) {

        QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
        db.setDatabaseName("QT");
        db.setUserName("mysql-user");
        db.setPassword("kjhl");
        db.setPort(63306);
        db.setHostName("111.120.13.93");

        // 尝试打开数据库连接
        if (!db.open()) {
            // 如果连接失败，弹出错误消息框
            QMessageBox::critical(nullptr, "数据库错误", db.lastError().text());
            return false; // 返回 false 表示失败
        }

        // 使用参数化查询更新 mainTask 字段
        QSqlQuery query;
        query.prepare("UPDATE tasks SET main_task = ? WHERE id = ?");
        query.addBindValue(mainTask); // 绑定主任务名称
        query.addBindValue(taskId); // 绑定任务 ID

        // 执行更新操作
        if (!query.exec()) {

            QMessageBox::critical(nullptr, "更新错误", query.lastError().text());
            return false; // 返回 false 表示失败
        }

        db.close();
        return true;
    }


private:
    bool canAddTarget; // 用于跟踪是否可以添加目标
    int insertedId; // 用于存储最后插入行的 ID
    QString mainTaskValue; // 存储主任务的值
};



// ++++++++++++++++++++++++++++++++++++  NumberSquareItem 类  ++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++  NumberSquareItem 类  ++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++  NumberSquareItem 类  ++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++  NumberSquareItem 类  ++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++  NumberSquareItem 类  ++++++++++++++++++++++++++++++++++++

// NumberSquareItem 类继承自 QGraphicsRectItem，用于表示带有数字的正方形项  在 NumberSquareItem 类中添加删除天数的菜单项和方法
class NumberSquareItem : public QGraphicsRectItem {
public:
    // 构造函数，接收数字、矩形和父窗口指针
    NumberSquareItem(int number, const QRectF &rect, A_tasks_template_Window *parentWindow, QGraphicsItem *parent = nullptr)
        : QGraphicsRectItem(rect, parent), number(number), parentWindow(parentWindow) {
        setBrush(Qt::white); // 设置矩形的填充颜色为白色
    }
    // 返回存储的数字
    int getNumber() const {
        return number; // 返回存储的数字
    }

protected:

    // 重写上下文菜单事件
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override {
        QMenu menu; // 创建上下文菜单
        QAction *increaseDaysAction = menu.addAction("增加天数"); // 添加“增加天数”选项
        QAction *deleteDaysAction = menu.addAction("删除天数"); // 添加“删除天数”选项

        // 获取全局位置
        QPoint globalPos = event->screenPos(); // 获取鼠标在屏幕上的位置

        // 计算菜单的显示位置，调整为右侧
        int menuWidth = menu.sizeHint().width();
        int menuHeight = menu.sizeHint().height();

        // 设置菜单显示在鼠标右侧
        QPoint adjustedPos(globalPos.x() + 25, globalPos.y()); // 向右偏移25个像素

        // 显示菜单并获取用户选择
        QAction *selectedAction = menu.exec(adjustedPos);

        // 如果选择了“增加天数”，调用相应的方法
        if (selectedAction == increaseDaysAction) {
            add_Day(); // 调用增加天数的方法
            if (parentWindow) {
                parentWindow->add_Square_Number(); // 调用父窗口的方法以增加新方块
            }
        }
        // 如果选择了“删除天数”，调用相应的方法
        else if (selectedAction == deleteDaysAction) {
            remove_Day(); // 调用删除天数的方法
            if (parentWindow) {
                parentWindow->remove_Square_Number(); // 调用父窗口的方法以删除方块
            }
        }
    }

private:
    int number; // 存储正方形的数字
    A_tasks_template_Window *parentWindow; // 保存父窗口的指针

    // 增加天数的方法
    void add_Day() {
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
        query.exec("SHOW COLUMNS FROM tasks"); // 获取 tasks 表的列信息

        int maxNumber = 0; // 用于存储当前最大天数

        // 遍历列信息，查找最大天数
        while (query.next()) {
            QString columnName = query.value(0).toString(); // 获取列名
            QRegularExpression regex("day(\\d+)"); // 正则表达式匹配天数列
            QRegularExpressionMatch match = regex.match(columnName);
            if (match.hasMatch()) {
                int num = match.captured(1).toInt(); // 获取捕获的数字
                if (num > maxNumber) {
                    maxNumber = num; // 更新最大天数
                }
            }
        }

        // 创建新的天数列名
        QString newColumnName = QString("day%1").arg(maxNumber + 1);
        QString sql = QString("ALTER TABLE tasks ADD COLUMN %1 INT DEFAULT 0").arg(newColumnName); // 创建添加新列的 SQL 语句

        // 执行 SQL 语句
        if (!query.exec(sql)) {
            QMessageBox::critical(nullptr, "SQL Error", query.lastError().text()); // 如果失败，弹出错误信息
        } else {
            QMessageBox::information(nullptr, "Success", QString("成功增加列 %1").arg(newColumnName)); // 成功添加列，弹出信息框
        }

        db.close(); // 关闭数据库连接
    }

    // 删除天数的方法
    void remove_Day() {
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

        // 获取当前最大天数列名
        QString lastColumnName;
        query.exec("SHOW COLUMNS FROM tasks"); // 获取 tasks 表的列信息
        while (query.next()) {
            QString columnName = query.value(0).toString(); // 获取列名
            QRegularExpression regex("day(\\d+)"); // 正则表达式匹配天数列
            QRegularExpressionMatch match = regex.match(columnName);
            if (match.hasMatch()) {
                lastColumnName = columnName; // 更新最后一列名
            }
        }

        // 删除最后一列
        if (!lastColumnName.isEmpty()) {
            QString sql = QString("ALTER TABLE tasks DROP COLUMN %1").arg(lastColumnName); // 创建删除列的 SQL 语句
            if (!query.exec(sql)) {
                QMessageBox::critical(nullptr, "SQL Error", query.lastError().text()); // 如果失败，弹出错误信息
            } else {
                QMessageBox::information(nullptr, "Success", QString("成功删除列 %1").arg(lastColumnName)); // 成功删除列，弹出信息框
            }
        }

        db.close(); // 关闭数据库连接
    }
};




#endif // A_TASKS_TEMPLATE_WINDOW_H
