#include "TaskPlanManagerDialog.h"
#include "ui_TaskPlanManagerDialog.h"
#include <QMessageBox>
#include <QDateTime>
#include <QInputDialog>
#include <QLineEdit>
#include "TaskDialog.h"

/**
 * @brief 构造函数
 * @param parent 父窗口指针
 * @details 初始化UI，设置表格属性，连接信号槽
 */
TaskPlanManagerDialog::TaskPlanManagerDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TaskPlanManagerDialog)
    , m_selectedIndex(-1)
{
    ui->setupUi(this);

    // 设置表格为行选择模式
    ui->taskPlanTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->taskPlanTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->taskPlanTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // 连接按钮信号槽
    connect(ui->addButton, &QPushButton::clicked, this, &TaskPlanManagerDialog::onAddTaskPlan);
    connect(ui->editButton, &QPushButton::clicked, this, &TaskPlanManagerDialog::onEditTaskPlan);
    connect(ui->deleteButton, &QPushButton::clicked, this, &TaskPlanManagerDialog::onDeleteTaskPlan);
    connect(ui->selectButton, &QPushButton::clicked, this, &TaskPlanManagerDialog::onSelectButtonClicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(ui->closeButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(ui->taskPlanTable, &QTableWidget::cellClicked, this, &TaskPlanManagerDialog::onTableItemClicked);
}

/**
 * @brief 析构函数
 * @details 释放UI资源
 */
TaskPlanManagerDialog::~TaskPlanManagerDialog()
{
    delete ui;
}

/**
 * @brief 设置任务规划列表
 * @param taskPlans 任务规划信息列表
 * @details 将传入的任务规划列表保存到成员变量并更新表格显示
 */
void TaskPlanManagerDialog::setTaskPlans(const QList<TaskInfo>& taskPlans)
{
    m_taskPlans = taskPlans;
    updateTable();
}

/**
 * @brief 获取任务规划列表
 * @return 任务规划信息列表
 * @details 返回当前管理的所有任务规划
 */
QList<TaskInfo> TaskPlanManagerDialog::getTaskPlans() const
{
    return m_taskPlans;
}

/**
 * @brief 更新表格显示
 * @details 将m_taskPlans中的所有任务规划同步显示到表格控件
 */
void TaskPlanManagerDialog::updateTable()
{
    ui->taskPlanTable->setRowCount(m_taskPlans.size());

    for (int i = 0; i < m_taskPlans.size(); ++i) {
        loadTaskPlanToForm(m_taskPlans[i], i);
    }
}

/**
 * @brief 加载任务规划到表格行
 * @param taskPlan 任务规划信息
 * @param row 目标行号
 * @details 将单个任务规划的各项数据显示到表格指定行
 */
void TaskPlanManagerDialog::loadTaskPlanToForm(const TaskInfo& taskPlan, int row)
{
    // 序号（从1开始）
    ui->taskPlanTable->setItem(row, 0, new QTableWidgetItem(QString::number(row + 1)));
    // 方案名称
    ui->taskPlanTable->setItem(row, 1, new QTableWidgetItem(taskPlan.planName));
    // 协同规划名称
    ui->taskPlanTable->setItem(row, 2, new QTableWidgetItem(taskPlan.coordinationName));
    // 任务名称
    ui->taskPlanTable->setItem(row, 3, new QTableWidgetItem(taskPlan.taskName));
    // 创建时间（时间范围字符串）
    ui->taskPlanTable->setItem(row, 4, new QTableWidgetItem(taskPlan.getTimeRange()));
    // 任务类型
    ui->taskPlanTable->setItem(row, 5, new QTableWidgetItem(taskPlan.taskType));
}

/**
 * @brief 新增任务规划
 * @details 弹出TaskDialog对话框让用户输入任务信息，然后通过输入框获取方案名称和协同规划名称
 */
void TaskPlanManagerDialog::onAddTaskPlan()
{
    // 弹出任务对话框
    TaskDialog dialog(this);
    dialog.setWindowTitle("新增任务规划");

    if (dialog.exec() == QDialog::Accepted) {
        TaskInfo newTask = dialog.getTaskInfo();

        // 获取输入的方案名称
        bool ok;
        QString planName = QInputDialog::getText(this, "新增任务规划", "方案名称:", QLineEdit::Normal, newTask.planName, &ok);
        if (!ok) return;
        newTask.planName = planName;

        // 获取输入的协同规划名称
        QString coordinationName = QInputDialog::getText(this, "新增任务规划", "协同规划名称:", QLineEdit::Normal, newTask.coordinationName, &ok);
        if (!ok) return;
        newTask.coordinationName = coordinationName;

        // 添加到列表并更新表格
        m_taskPlans.append(newTask);
        updateTable();
    }
}

/**
 * @brief 编辑任务规划
 * @details 弹出TaskDialog对话框编辑选中任务规划的详细信息
 */
void TaskPlanManagerDialog::onEditTaskPlan()
{
    int currentRow = ui->taskPlanTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "提示", "请先选择要编辑的任务规划");
        return;
    }

    if (currentRow >= m_taskPlans.size()) {
        QMessageBox::warning(this, "提示", "数据错误");
        return;
    }

    // 弹出任务对话框并加载当前任务信息
    TaskDialog dialog(this);
    dialog.setWindowTitle("编辑任务规划");
    dialog.setTaskInfo(m_taskPlans[currentRow]);

    if (dialog.exec() == QDialog::Accepted) {
        TaskInfo updatedTask = dialog.getTaskInfo();

        // 更新方案名称
        bool ok;
        QString planName = QInputDialog::getText(this, "编辑任务规划", "方案名称:", QLineEdit::Normal, updatedTask.planName, &ok);
        if (ok) updatedTask.planName = planName;

        // 更新协同规划名称
        QString coordinationName = QInputDialog::getText(this, "编辑任务规划", "协同规划名称:", QLineEdit::Normal, updatedTask.coordinationName, &ok);
        if (ok) updatedTask.coordinationName = coordinationName;

        // 更新列表并刷新表格
        m_taskPlans[currentRow] = updatedTask;
        updateTable();
    }
}

/**
 * @brief 删除任务规划
 * @details 删除选中的任务规划，需要用户二次确认
 */
void TaskPlanManagerDialog::onDeleteTaskPlan()
{
    int currentRow = ui->taskPlanTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "提示", "请先选择要删除的任务规划");
        return;
    }

    if (currentRow >= m_taskPlans.size()) {
        return;
    }

    // 弹出确认对话框
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "确认删除", "确定要删除选中的任务规划吗?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        // 从列表中移除并刷新表格
        m_taskPlans.removeAt(currentRow);
        updateTable();
    }
}

/**
 * @brief 表格项点击处理
 * @param row 点击的行号
 * @param column 点击的列号
 * @details 记录当前选中的行索引
 */
void TaskPlanManagerDialog::onTableItemClicked(int row, int column)
{
    Q_UNUSED(column);
    m_selectedIndex = row;
}

/**
 * @brief 选择按钮处理
 * @details 选中当前任务规划，发送taskPlanSelected信号，关闭对话框
 */
void TaskPlanManagerDialog::onSelectButtonClicked()
{
    int currentRow = ui->taskPlanTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "提示", "请先选择任务规划");
        return;
    }

    if (currentRow >= m_taskPlans.size()) {
        QMessageBox::warning(this, "提示", "数据错误");
        return;
    }

    // 记录选中索引
    m_selectedIndex = currentRow;

    // 发送任务规划选中信号
    emit taskPlanSelected(m_taskPlans[currentRow]);

    // 关闭对话框
    accept();
}
