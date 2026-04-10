#include "TaskDialog.h"
#include "ui_TaskDialog.h"
#include "StructData.h"
#include <QTime>

/**
 * @brief 构造函数（新增模式）
 * @param parent 父窗口指针
 * @details 初始化对话框，设置editIndex为-1表示新增模式
 */
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

/**
 * @brief 构造函数（编辑模式）
 * @param editIndex 要编辑的任务索引
 * @param taskInfo 要编辑的任务信息
 * @param parent 父窗口指针
 * @details 初始化对话框，加载任务信息到表单
 */
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

/**
 * @brief 析构函数
 * @details 释放UI资源
 */
TaskDialog::~TaskDialog()
{
    delete ui;
}

/**
 * @brief 设置任务信息到表单
 * @param taskInfo 任务信息结构体
 * @details 将任务信息填充到各个表单控件中
 */
void TaskDialog::setTaskInfo(const TaskInfo &taskInfo)
{
    // 设置任务名称
    ui->taskNameEdit->setText(taskInfo.taskName);

    // 设置任务类型
    int taskTypeIndex = ui->taskTypeCombo->findText(taskInfo.taskType);
    if (taskTypeIndex != -1) {
        ui->taskTypeCombo->setCurrentIndex(taskTypeIndex);
    }

    // 设置目标类型
    int targetTypeIndex = ui->targetTypeCombo->findText(taskInfo.targetType);
    if (targetTypeIndex != -1) {
        ui->targetTypeCombo->setCurrentIndex(targetTypeIndex);
    }

    // 设置目标名称
    int targetNameIndex = ui->targetNameCombo->findText(taskInfo.taskTarget);
    if (targetNameIndex != -1) {
        ui->targetNameCombo->setCurrentIndex(targetNameIndex);
    }

    // 设置分配的无人机
    ui->uavsEdit->setText(taskInfo.allocatedUAVs);

    // 设置开始和结束时间
    ui->startTimeEdit->setTime(taskInfo.startTime);
    ui->endTimeEdit->setTime(taskInfo.endTime);
}

/**
 * @brief 获取任务信息
 * @return 任务信息结构体
 * @details 从表单控件中提取用户输入的任务信息
 */
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

/**
 * @brief 获取编辑索引
 * @return 编辑索引，-1表示新增模式
 */
int TaskDialog::getEditIndex() const
{
    return editIndex;
}

/**
 * @brief 确定按钮处理
 * @details 验证表单数据，根据editIndex发送相应信号并关闭对话框
 */
void TaskDialog::onOkButtonClicked()
{
    // 验证任务名称
    if (ui->taskNameEdit->text().isEmpty()) {
        return;
    }

    // 验证时间范围
    if (ui->startTimeEdit->time() >= ui->endTimeEdit->time()) {
        return;
    }

    // 获取任务信息
    TaskInfo taskInfo = getTaskInfo();

    // 根据编辑索引发送不同的信号
    if (editIndex == -1) {
        // 发送任务添加信号
        emit taskAdded(taskInfo);
    } else {
        // 发送任务更新信号
        emit taskUpdated(editIndex, taskInfo);
    }

    // 接受并关闭对话框
    accept();
}

/**
 * @brief 取消按钮处理
 * @details 关闭对话框，不保存数据
 */
void TaskDialog::onCancelButtonClicked()
{
    reject();
}

/**
 * @brief 关闭按钮处理
 * @details 关闭对话框
 */
void TaskDialog::on_closeButton_clicked()
{
    reject();
}
