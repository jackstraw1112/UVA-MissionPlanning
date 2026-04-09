#ifndef TASKPLANMANAGERDIALOG_H
#define TASKPLANMANAGERDIALOG_H

#include <QDialog>
#include <QList>
#include "StructData.h"

namespace Ui {
class TaskPlanManagerDialog;
}

class TaskPlanManagerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TaskPlanManagerDialog(QWidget *parent = nullptr);
    ~TaskPlanManagerDialog();

    void setTaskPlans(const QList<TaskInfo>& taskPlans);
    QList<TaskInfo> getTaskPlans() const;

signals:
    void taskPlanSelected(const TaskInfo& taskPlan);

private slots:
    void onAddTaskPlan();
    void onEditTaskPlan();
    void onDeleteTaskPlan();
    void onTableItemClicked(int row, int column);
    void onSelectButtonClicked();

private:
    Ui::TaskPlanManagerDialog *ui;
    QList<TaskInfo> m_taskPlans;
    int m_selectedIndex;

    void updateTable();
    void loadTaskPlanToForm(const TaskInfo& taskPlan, int row);
};

#endif
