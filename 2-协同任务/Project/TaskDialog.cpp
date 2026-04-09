#include "TaskDialog.h"
#include "ui_TaskDialog.h"
#include "StructData.h"
#include <QTime>

TaskDialog::TaskDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::TaskDialog), editIndex(-1) // -1表示添加新任务
{
    ui->setupUi(this);

    // 设置对话框属性
    setModal(true);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    // 连接信号和槽
    connect(ui->okButton, &QPushButton::clicked, this, &TaskDialog::onOkButtonClicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &TaskDialog::onCancelButtonClicked);
    connect(ui->closeButton, &QPushButton::clicked, this, &TaskDialog::on_closeButton_clicked);
}

TaskDialog::TaskDialog(int editIndex, const TaskInfo& taskInfo, QWidget *parent)
    : QDialog(parent), ui(new Ui::TaskDialog), editIndex(editIndex)
{
    ui->setupUi(this);

    // 设置对话框属性
    setModal(true);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    // 连接信号和槽
    connect(ui->okButton, &QPushButton::clicked, this, &TaskDialog::onOkButtonClicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &TaskDialog::onCancelButtonClicked);
    connect(ui->closeButton, &QPushButton::clicked, this, &TaskDialog::on_closeButton_clicked);
    
    // 设置表单数据
    setTaskInfo(taskInfo);
}

TaskDialog::~TaskDialog()
{
    delete ui;
}



// 使用结构体设置表单数据（直接操作控件，不依赖辅助方法）
void TaskDialog::setTaskInfo(const TaskInfo &taskInfo)
{
    ui->taskNameEdit->setText(taskInfo.taskName);
    
    int taskTypeIndex = ui->taskTypeCombo->findText(taskInfo.taskType);
    if (taskTypeIndex != -1) {
        ui->taskTypeCombo->setCurrentIndex(taskTypeIndex);
    }
    
    int targetTypeIndex = ui->targetTypeCombo->findText(taskInfo.targetType);
    if (targetTypeIndex != -1) {
        ui->targetTypeCombo->setCurrentIndex(targetTypeIndex);
    }
    
    int targetNameIndex = ui->targetNameCombo->findText(taskInfo.taskTarget);
    if (targetNameIndex != -1) {
        ui->targetNameCombo->setCurrentIndex(targetNameIndex);
    }
    
    ui->uavsEdit->setText(taskInfo.allocatedUAVs);
    ui->startTimeEdit->setTime(taskInfo.startTime);
    ui->endTimeEdit->setTime(taskInfo.endTime);
}

// 获取完整的任务信息结构体（直接操作控件，不依赖辅助方法）
TaskInfo TaskDialog::getTaskInfo() const
{
    TaskInfo taskInfo;
    taskInfo.taskName = ui->taskNameEdit->text();
    taskInfo.taskType = ui->taskTypeCombo->currentText();
    taskInfo.targetType = ui->targetTypeCombo->currentText();
    taskInfo.taskTarget = ui->targetNameCombo->currentText();
    taskInfo.allocatedUAVs = ui->uavsEdit->text();
    taskInfo.startTime = ui->startTimeEdit->time();
    taskInfo.endTime = ui->endTimeEdit->time();
    return taskInfo;
}

// 获取编辑索引
int TaskDialog::getEditIndex() const
{
    return editIndex;
}


// 槽函数
void TaskDialog::onOkButtonClicked()
{
    // 验证表单数据
    if (ui->taskNameEdit->text().isEmpty()) {
        // 可以添加错误提示
        return;
    }

    // 验证时间范围
    if (ui->startTimeEdit->time() >= ui->endTimeEdit->time()) {
        // 可以添加错误提示
        return;
    }

    // 获取任务信息
    TaskInfo taskInfo = getTaskInfo();
    
    // 根据编辑索引发送不同的信号
    if (editIndex == -1) {
        // 发出任务添加信号
        emit taskAdded(taskInfo);
    } else {
        // 发出任务更新信号
        emit taskUpdated(editIndex, taskInfo);
    }
    
    accept(); // 接受对话框
}

void TaskDialog::onCancelButtonClicked()
{
    reject(); // 拒绝对话框
}

void TaskDialog::on_closeButton_clicked()
{
    reject(); // 拒绝对话框
}