#include "TaskPlanManagerDialog.h"
#include "ui_TaskPlanManagerDialog.h"
#include <QMessageBox>
#include <QDateTime>
#include <QInputDialog>
#include "TaskDialog.h"

TaskPlanManagerDialog::TaskPlanManagerDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TaskPlanManagerDialog)
    , m_selectedIndex(-1)
{
    ui->setupUi(this);

    ui->taskPlanTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->taskPlanTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->taskPlanTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(ui->addButton, &QPushButton::clicked, this, &TaskPlanManagerDialog::onAddTaskPlan);
    connect(ui->editButton, &QPushButton::clicked, this, &TaskPlanManagerDialog::onEditTaskPlan);
    connect(ui->deleteButton, &QPushButton::clicked, this, &TaskPlanManagerDialog::onDeleteTaskPlan);
    connect(ui->selectButton, &QPushButton::clicked, this, &TaskPlanManagerDialog::onSelectButtonClicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(ui->closeButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(ui->taskPlanTable, &QTableWidget::cellClicked, this, &TaskPlanManagerDialog::onTableItemClicked);
}

TaskPlanManagerDialog::~TaskPlanManagerDialog()
{
    delete ui;
}

void TaskPlanManagerDialog::setTaskPlans(const QList<TaskInfo>& taskPlans)
{
    m_taskPlans = taskPlans;
    updateTable();
}

QList<TaskInfo> TaskPlanManagerDialog::getTaskPlans() const
{
    return m_taskPlans;
}

void TaskPlanManagerDialog::updateTable()
{
    ui->taskPlanTable->setRowCount(m_taskPlans.size());

    for (int i = 0; i < m_taskPlans.size(); ++i) {
        loadTaskPlanToForm(m_taskPlans[i], i);
    }
}

void TaskPlanManagerDialog::loadTaskPlanToForm(const TaskInfo& taskPlan, int row)
{
    ui->taskPlanTable->setItem(row, 0, new QTableWidgetItem(QString::number(row + 1)));
    ui->taskPlanTable->setItem(row, 1, new QTableWidgetItem(taskPlan.planName));
    ui->taskPlanTable->setItem(row, 2, new QTableWidgetItem(taskPlan.coordinationName));
    ui->taskPlanTable->setItem(row, 3, new QTableWidgetItem(taskPlan.taskName));
    ui->taskPlanTable->setItem(row, 4, new QTableWidgetItem(taskPlan.getTimeRange()));
    ui->taskPlanTable->setItem(row, 5, new QTableWidgetItem(taskPlan.taskType));
}

void TaskPlanManagerDialog::onAddTaskPlan()
{
    TaskDialog dialog(this);
    dialog.setWindowTitle("新增任务规划");

    if (dialog.exec() == QDialog::Accepted) {
        TaskInfo newTask = dialog.getTaskInfo();
        newTask.planName = ui->taskPlanTable->item(0, 1) ? ui->taskPlanTable->item(0, 1)->text() : "";
        newTask.coordinationName = ui->taskPlanTable->item(0, 2) ? ui->taskPlanTable->item(0, 2)->text() : "";

        bool ok;
        QString planName = QInputDialog::getText(this, "新增任务规划", "方案名称:", QLineEdit::Normal, newTask.planName, &ok);
        if (!ok) return;
        newTask.planName = planName;

        QString coordinationName = QInputDialog::getText(this, "新增任务规划", "协同规划名称:", QLineEdit::Normal, newTask.coordinationName, &ok);
        if (!ok) return;
        newTask.coordinationName = coordinationName;

        m_taskPlans.append(newTask);
        updateTable();
    }
}

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

    TaskDialog dialog(this);
    dialog.setWindowTitle("编辑任务规划");
    dialog.setTaskInfo(m_taskPlans[currentRow]);

    if (dialog.exec() == QDialog::Accepted) {
        TaskInfo updatedTask = dialog.getTaskInfo();

        bool ok;
        QString planName = QInputDialog::getText(this, "编辑任务规划", "方案名称:", QLineEdit::Normal, updatedTask.planName, &ok);
        if (ok) updatedTask.planName = planName;

        QString coordinationName = QInputDialog::getText(this, "编辑任务规划", "协同规划名称:", QLineEdit::Normal, updatedTask.coordinationName, &ok);
        if (ok) updatedTask.coordinationName = coordinationName;

        m_taskPlans[currentRow] = updatedTask;
        updateTable();
    }
}

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

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "确认删除", "确定要删除选中的任务规划吗?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        m_taskPlans.removeAt(currentRow);
        updateTable();
    }
}

void TaskPlanManagerDialog::onTableItemClicked(int row, int column)
{
    Q_UNUSED(column);
    m_selectedIndex = row;
}

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

    m_selectedIndex = currentRow;
    emit taskPlanSelected(m_taskPlans[currentRow]);
    accept();
}
