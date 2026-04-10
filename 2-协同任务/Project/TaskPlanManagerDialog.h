#ifndef TASKPLANMANAGERDIALOG_H
#define TASKPLANMANAGERDIALOG_H

#include <QDialog>
#include <QList>
#include "StructData.h"

namespace Ui {
class TaskPlanManagerDialog;
}

/**
 * @brief 任务规划管理对话框类
 * @details 用于管理任务规划的增删改查操作，支持方案名称、协同规划名称等字段的管理
 */
class TaskPlanManagerDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口指针，默认为nullptr
     */
    explicit TaskPlanManagerDialog(QWidget *parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~TaskPlanManagerDialog();

    /**
     * @brief 设置任务规划列表
     * @param taskPlans 任务规划信息列表
     */
    void setTaskPlans(const QList<TaskInfo>& taskPlans);

    /**
     * @brief 获取任务规划列表
     * @return 任务规划信息列表
     */
    QList<TaskInfo> getTaskPlans() const;

signals:
    /**
     * @brief 任务规划选择信号
     * @param taskPlan 被选中的任务规划信息
     */
    void taskPlanSelected(const TaskInfo& taskPlan);

private slots:
    /**
     * @brief 新增任务规划槽函数
     * @details 弹出新增对话框，输入方案名称和协同规划名称后添加到列表
     */
    void onAddTaskPlan();

    /**
     * @brief 编辑任务规划槽函数
     * @details 弹出编辑对话框，修改选中任务规划的信息
     */
    void onEditTaskPlan();

    /**
     * @brief 删除任务规划槽函数
     * @details 删除选中的任务规划，需要用户确认
     */
    void onDeleteTaskPlan();

    /**
     * @brief 表格项点击槽函数
     * @param row 点击的行号
     * @param column 点击的列号
     */
    void onTableItemClicked(int row, int column);

    /**
     * @brief 选择按钮槽函数
     * @details 选中当前任务规划并关闭对话框，发送taskPlanSelected信号
     */
    void onSelectButtonClicked();

private:
    Ui::TaskPlanManagerDialog *ui;      // UI界面指针
    QList<TaskInfo> m_taskPlans;        // 任务规划列表
    int m_selectedIndex;                // 当前选中索引

    /**
     * @brief 更新表格显示
     * @details 将m_taskPlans中的数据同步到表格控件中
     */
    void updateTable();

    /**
     * @brief 加载任务规划到表格行
     * @param taskPlan 任务规划信息
     * @param row 目标行号
     */
    void loadTaskPlanToForm(const TaskInfo& taskPlan, int row);
};

#endif
