#ifndef C_PROMANAWINDOW_NEWPRO_DIALOG_H
#define C_PROMANAWINDOW_NEWPRO_DIALOG_H

#include "QtSql/qsqldatabase.h"
#include <QDialog>
#include <QVBoxLayout>
namespace Ui {
class C_ProManaWindow_newPro_Dialog;
}

class C_ProManaWindow_newPro_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit C_ProManaWindow_newPro_Dialog(QWidget *parent = nullptr);
    ~C_ProManaWindow_newPro_Dialog();

private:
    Ui::C_ProManaWindow_newPro_Dialog *ui;

    // 以下是自己写的
public:

    void createCheckBoxesFromDatabase();
    void arrangeLabels(QVBoxLayout *labelLayout);
    void updateCheckedScrollArea(QVBoxLayout *labelLayout);
    void onCreateProButtonClicked();
    bool createDatabase(const QString &dbName);
    QString getCheckedModuleNames();

    void createProjectTable(const QString &dbName);
    void createModuleParameterTable(const QString &dbName, const QStringList &selectedModules);
    void createScheduleTable(const QString &dbName);
    void createDailyReportTable(const QString &dbName);
    void createUserFeedbackFormTable(const QString &dbNameb);
    void createRequirementTable(const QString &dbName);
};

#endif // C_PROMANAWINDOW_NEWPRO_DIALOG_H
