#include "CooperativeMissionPlan.h"
#include "ui_CooperativeMissionPlan.h"
#include "StructData.h"
#include <QDateTime>
#include <QTimer>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QTreeWidgetItem>
#include <QHeaderView>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileDialog>

CooperativeMissionPlan::CooperativeMissionPlan(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CooperativeMissionPlan)
    , selectedTaskIndex(0)
    , currentFilePath("")
{
    ui->setupUi(this);
    
    clockTimer = new QTimer(this);
    connect(clockTimer, &QTimer::timeout, this, &CooperativeMissionPlan::updateClock);
    clockTimer->start(1000);
    updateClock();
    
    setupConnections();
    setupStyles();
    initializeTables();
    initializeTree();
}

CooperativeMissionPlan::~CooperativeMissionPlan()
{
    delete ui;
}

void CooperativeMissionPlan::updateClock()
{
    QDateTime currentTime = QDateTime::currentDateTime();
    ui->timeLabel->setText(currentTime.toString("yyyy-MM-dd hh:mm:ss")); // 使用timeLabel代替clockLabel和dateLabel
}

void CooperativeMissionPlan::setupConnections()
{
    // Top bar tab buttons
    connect(ui->dataManagementButton, &QPushButton::clicked, this, &CooperativeMissionPlan::onTabButtonClicked);
    connect(ui->cooperativePlanningButton, &QPushButton::clicked, this, &CooperativeMissionPlan::onTabButtonClicked);
    connect(ui->uavPlanningButton, &QPushButton::clicked, this, &CooperativeMissionPlan::onTabButtonClicked);
    connect(ui->situationAnalysisButton, &QPushButton::clicked, this, &CooperativeMissionPlan::onTabButtonClicked);
    connect(ui->situationDisplayButton, &QPushButton::clicked, this, &CooperativeMissionPlan::onTabButtonClicked);
    
    // Toolbar buttons
    connect(ui->newButton, &QPushButton::clicked, this, &CooperativeMissionPlan::onNewTask);
    connect(ui->openButton, &QPushButton::clicked, this, &CooperativeMissionPlan::onOpenTask);
    connect(ui->saveButton, &QPushButton::clicked, this, &CooperativeMissionPlan::onSaveTask);
    connect(ui->exportButton, &QPushButton::clicked, this, &CooperativeMissionPlan::onExport);
    connect(ui->shutdownButton, &QPushButton::clicked, this, &CooperativeMissionPlan::onShutdown);
    connect(ui->automaticAllocationButton, &QPushButton::clicked, this, &CooperativeMissionPlan::onAutoAllocate);
    connect(ui->planPathButton, &QPushButton::clicked, this, &CooperativeMissionPlan::onGeneratePath);
    connect(ui->settingsButton, &QPushButton::clicked, this, &CooperativeMissionPlan::onSettings);
    
    // Panel buttons
    connect(ui->editTaskButton, &QPushButton::clicked, this, &CooperativeMissionPlan::onEditTask);
    connect(ui->deleteTaskButton, &QPushButton::clicked, this, &CooperativeMissionPlan::onDeleteTask);
    connect(ui->calculateForceButton, &QPushButton::clicked, this, &CooperativeMissionPlan::onCalculateForce);
    connect(ui->calculateForceButton1, &QPushButton::clicked, this, &CooperativeMissionPlan::onCalculateForce);
    connect(ui->manualAdjustButton, &QPushButton::clicked, this, &CooperativeMissionPlan::onAutoAllocate);
    connect(ui->addForceButton, &QPushButton::clicked, this, &CooperativeMissionPlan::onAddForce);
    connect(ui->deleteForceButton, &QPushButton::clicked, this, &CooperativeMissionPlan::onDeleteForce);
    
    // Task management panel buttons
    connect(ui->newTaskButton, &QPushButton::clicked, this, &CooperativeMissionPlan::onNewTask);
    connect(ui->saveTaskButton, &QPushButton::clicked, this, &CooperativeMissionPlan::onSaveTask);
    
    // Task allocation panel buttons
    connect(ui->autoAllocationButton, &QPushButton::clicked, this, &CooperativeMissionPlan::onAutoAllocate);
    connect(ui->saveAllocationButton, &QPushButton::clicked, this, &CooperativeMissionPlan::onSaveAllocation);
    
    // Path planning buttons
    connect(ui->flightPathTab, &QPushButton::clicked, [this]() { onPathTabChanged(0); });
    connect(ui->searchPathTab, &QPushButton::clicked, [this]() { onPathTabChanged(1); });
    connect(ui->allTasksFilter, &QPushButton::clicked, this, [this]() { showToast("显示所有任务路径"); });
    connect(ui->selectedTasksFilter, &QPushButton::clicked, this, [this]() { showToast("显示选中任务路径"); });
    connect(ui->generateSelectedPathButton, &QPushButton::clicked, this, &CooperativeMissionPlan::onGenerateSelectedPath);
    connect(ui->generateAllPathsButton, &QPushButton::clicked, this, &CooperativeMissionPlan::onGenerateAllPaths);
    
    // Target parameter buttons
    connect(ui->radarTargetTab, &QPushButton::clicked, [this]() { onTargetTabChanged(0); });
    connect(ui->radioTargetTab, &QPushButton::clicked, [this]() { onTargetTabChanged(1); });
    connect(ui->commTargetTab, &QPushButton::clicked, [this]() { onTargetTabChanged(2); });
    connect(ui->rcmTargetTab, &QPushButton::clicked, [this]() { onTargetTabChanged(3); });
    connect(ui->addTargetParamButton, &QPushButton::clicked, this, &CooperativeMissionPlan::onAddTargetParam);
    connect(ui->editTargetParamButton, &QPushButton::clicked, this, &CooperativeMissionPlan::onEditTargetParam);
    
    // Table selection
    connect(ui->taskTable, &QTableWidget::cellClicked, [this](int row, int column) {
        Q_UNUSED(column);
        onTaskSelected(row);
    });
    
    connect(ui->shutdownButton, &QPushButton::clicked, this, []() {
        QApplication::quit();
    });
}

void CooperativeMissionPlan::setupStyles()
{
    // Set window icon and title
    setWindowTitle("无人机任务规划与效能评估系统");
    
    // Configure table headers
    QStringList taskHeaders = {"任务名称", "任务类型", "目标类型", "任务目标", "时间范围", "分配无人机"};
    ui->taskTable->setColumnCount(6); // Ensure we have 6 columns
    ui->taskTable->setHorizontalHeaderLabels(taskHeaders);
    ui->taskTable->horizontalHeader()->setStretchLastSection(false);
    ui->taskTable->verticalHeader()->setVisible(false);
    ui->taskTable->horizontalHeader()->setVisible(true); // Ensure headers are visible
    ui->taskTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft); // Align header text to left
    ui->taskTable->horizontalHeader()->setMinimumHeight(24); // Ensure header has sufficient height
    ui->taskTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->taskTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->taskTable->setAlternatingRowColors(false);

   
    
    QStringList forceCalcHeaders = {"任务名称", "任务目标", "威胁等级", "优先级", "计算数量", "调整数量"};
    ui->forceCalculationTable->setColumnCount(6); // Ensure we have 6 columns
    ui->forceCalculationTable->setHorizontalHeaderLabels(forceCalcHeaders);
    ui->forceCalculationTable->horizontalHeader()->setStretchLastSection(true);
    ui->forceCalculationTable->verticalHeader()->setVisible(false);
    ui->forceCalculationTable->horizontalHeader()->setVisible(true); // Ensure headers are visible
    ui->forceCalculationTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft); // Align header text to left
    ui->forceCalculationTable->horizontalHeader()->setMinimumHeight(24); // Ensure header has sufficient height
    ui->forceCalculationTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->forceCalculationTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->forceCalculationTable->setAlternatingRowColors(false);



    QStringList allocationHeaders = {"任务名称", "目标类型", "任务目标", "威胁等级", "分配无人机", "编队"};
    ui->allocationTable->setColumnCount(6); // Ensure we have 6 columns
    ui->allocationTable->setHorizontalHeaderLabels(allocationHeaders);
    ui->allocationTable->horizontalHeader()->setStretchLastSection(true);
    ui->allocationTable->verticalHeader()->setVisible(false);
    ui->allocationTable->horizontalHeader()->setVisible(true); // Ensure headers are visible
    ui->allocationTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft); // Align header text to left
    ui->allocationTable->horizontalHeader()->setMinimumHeight(24); // Ensure header has sufficient height
    ui->allocationTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->allocationTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->allocationTable->setAlternatingRowColors(false);


    QStringList pathHeaders = {"无人机名称", "关联任务", "路径点数", "状态"};
    ui->pathTable->setColumnCount(4); // Ensure we have 4 columns
    ui->pathTable->setHorizontalHeaderLabels(pathHeaders); // Fix: use correct headers
    ui->pathTable->horizontalHeader()->setStretchLastSection(true);
    ui->pathTable->verticalHeader()->setVisible(false);
    ui->pathTable->horizontalHeader()->setVisible(true); // Ensure headers are visible
    ui->pathTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft); // Align header text to left
    ui->pathTable->horizontalHeader()->setMinimumHeight(24); // Ensure header has sufficient height
    ui->pathTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->pathTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->pathTable->setAlternatingRowColors(false);


    QStringList targetHeaders = {"目标编号", "频率范围(MHz)", "脉宽范围(μs)", "重复周期(ms)", "工作模式"};
    ui->targetTable->setColumnCount(5); // Ensure we have 5 columns
    ui->targetTable->setHorizontalHeaderLabels(targetHeaders); // Fix: use correct headers
    ui->targetTable->horizontalHeader()->setStretchLastSection(true);
    ui->targetTable->verticalHeader()->setVisible(false);
    ui->targetTable->horizontalHeader()->setVisible(true); // Ensure headers are visible
    ui->targetTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft); // Align header text to left
    ui->targetTable->horizontalHeader()->setMinimumHeight(24); // Ensure header has sufficient height
    ui->targetTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->targetTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->targetTable->setAlternatingRowColors(false);

}

void CooperativeMissionPlan::initializeTables()
{
    
}

void CooperativeMissionPlan::initializeTree()
{
    
}

void CooperativeMissionPlan::updateForceTree()
{
    // 清空现有内容
    ui->forceTree->clear();
    
    // 设置列宽
    ui->forceTree->setColumnWidth(0, 280);
    ui->forceTree->setColumnWidth(1, 80);
    
    // 遍历所有编组
    for (const auto& group : groupList) {
        QTreeWidgetItem* groupItem = new QTreeWidgetItem(ui->forceTree);
        int available = group.getAvailableUAVCount();
        int total = group.getTotalUAVCount();
        groupItem->setText(0, "� " + group.groupName);
        groupItem->setText(1, QString("%1/%2").arg(available).arg(total));
        groupItem->setExpanded(true);
        
        // 遍历该编组下的所有装备型号
        for (const auto& equipment : group.equipmentList) {
            QTreeWidgetItem* equipmentItem = new QTreeWidgetItem(groupItem);
            int eqAvailable = 0;
            for (const auto& uav : equipment.uavList) {
                if (uav.status == "就绪") {
                    eqAvailable++;
                }
            }
            equipmentItem->setText(0, "📂 " + equipment.equipmentName);
            equipmentItem->setText(1, QString("%1/%2").arg(eqAvailable).arg(equipment.uavList.size()));
            equipmentItem->setExpanded(true);
            
            // 遍历该装备型号下的所有无人机
            for (const auto& uav : equipment.uavList) {
                QTreeWidgetItem* uavItem = new QTreeWidgetItem(equipmentItem);
                uavItem->setText(0, "● " + uav.uavName);
                uavItem->setText(1, uav.status);
            }
        }
    }
}

void CooperativeMissionPlan::onAddForce()
{
    showToast("打开添加兵力对话框");
}

void CooperativeMissionPlan::onDeleteForce()
{
    showToast("已删除选中编组");
}

void CooperativeMissionPlan::onPathTabChanged(int index)
{
    // 更新标签页样式
    QPushButton* tabs[] = { ui->flightPathTab, ui->searchPathTab };
    for (int i = 0; i < 2; i++) {
        if (i == index) {
            tabs[i]->setStyleSheet("QPushButton {\nbackground: rgba(0,150,255,0.25);\nborder: 1px solid rgba(0,180,255,0.4);\nborder-bottom: none;\nborder-radius: 3px 3px 0 0;\ncolor: #8ad6ff;\nfont-size: 12px;\n}\nQPushButton:hover {\nbackground: rgba(0,160,255,0.3);\ncolor: #c8e8ff;\n}");
        } else {
            tabs[i]->setStyleSheet("QPushButton {\nbackground: rgba(0,80,160,0.15);\nborder: 1px solid rgba(0,120,200,0.3);\nborder-bottom: none;\nborder-radius: 3px 3px 0 0;\ncolor: #99ccee;\nfont-size: 12px;\n}\nQPushButton:hover {\nbackground: rgba(0,100,180,0.2);\ncolor: #c8e8ff;\n}");
        }
    }
    
    showToast(index == 0 ? "切换到飞行路径" : "切换到搜索路径");
}

void CooperativeMissionPlan::onTargetTabChanged(int index)
{
    // 更新标签页样式
    QPushButton* tabs[] = { ui->radarTargetTab, ui->radioTargetTab, ui->commTargetTab, ui->rcmTargetTab };
    for (int i = 0; i < 4; i++) {
        if (i == index) {
            tabs[i]->setStyleSheet("QPushButton {\nbackground: rgba(0,150,255,0.25);\nborder: 1px solid rgba(0,180,255,0.4);\nborder-bottom: none;\nborder-radius: 3px 3px 0 0;\ncolor: #8ad6ff;\nfont-size: 12px;\n}\nQPushButton:hover {\nbackground: rgba(0,160,255,0.3);\ncolor: #c8e8ff;\n}");
        } else {
            tabs[i]->setStyleSheet("QPushButton {\nbackground: rgba(0,80,160,0.15);\nborder: 1px solid rgba(0,120,200,0.3);\nborder-bottom: none;\nborder-radius: 3px 3px 0 0;\ncolor: #99ccee;\nfont-size: 12px;\n}\nQPushButton:hover {\nbackground: rgba(0,100,180,0.2);\ncolor: #c8e8ff;\n}");
        }
    }
    
    QString tabNames[] = { "雷达", "电台", "通信对抗", "雷达对抗" };
    showToast(QString("切换到%1目标参数").arg(tabNames[index]));
}

void CooperativeMissionPlan::onGenerateSelectedPath()
{
    showToast("已为选中无人机生成路径");
}

void CooperativeMissionPlan::onGenerateAllPaths()
{
    showToast("正在生成所有路径...");
}

void CooperativeMissionPlan::onAddTargetParam()
{
    showToast("打开添加目标参数对话框");
}

void CooperativeMissionPlan::onEditTargetParam()
{
    showToast("打开目标参数编辑对话框");
}

void CooperativeMissionPlan::onSaveAllocation()
{
    showToast("任务分配已保存");
}

// 接收任务添加信号的槽函数
void CooperativeMissionPlan::onTaskAdded(const TaskInfo& taskInfo)
{
    // 将任务添加到内部数据列表
    taskList.append(taskInfo);
    
    // 更新表格显示
    DisplayTaskTableToData(taskList);
    
    // 显示提示信息
    showToast("协同任务已添加");
}

// 接收任务更新信号的槽函数
void CooperativeMissionPlan::onTaskUpdated(int index, const TaskInfo& taskInfo)
{
    // 检查索引是否有效
    if (index >= 0 && index < taskList.size()) {
        // 更新内部数据列表
        taskList[index] = taskInfo;
        
        // 更新表格显示
        DisplayTaskTableToData(taskList);
        
        // 显示提示信息
        showToast("协同任务已更新");
    }
}

// 显示任务表格数据
void CooperativeMissionPlan::DisplayTaskTableToData(const QList<TaskInfo>& taskList)
{
    // 更新内部数据列表
    this->taskList = taskList;
    
    // 清空表格
    ui->taskTable->clearContents();
    ui->taskTable->setRowCount(0);
    
    // 添加数据行
    for (const TaskInfo& task : taskList) {
        int row = ui->taskTable->rowCount();
        ui->taskTable->insertRow(row);
        
        ui->taskTable->setItem(row, 0, new QTableWidgetItem(task.taskName));
        ui->taskTable->setItem(row, 1, new QTableWidgetItem(task.taskType));
        ui->taskTable->setItem(row, 2, new QTableWidgetItem(task.targetType));
        ui->taskTable->setItem(row, 3, new QTableWidgetItem(task.taskTarget));
        ui->taskTable->setItem(row, 4, new QTableWidgetItem(task.getTimeRange()));
        ui->taskTable->setItem(row, 5, new QTableWidgetItem(task.allocatedUAVs));
    }
}

// 显示兵力需求计算表格数据
void CooperativeMissionPlan::DisplayForceCalculationTableToData(const QList<ForceCalculation>& forceList)
{
    // 清空表格
    ui->forceCalculationTable->clearContents();
    ui->forceCalculationTable->setRowCount(0);
    
    // 添加数据行
    for (const ForceCalculation& force : forceList) {
        int row = ui->forceCalculationTable->rowCount();
        ui->forceCalculationTable->insertRow(row);
        
        ui->forceCalculationTable->setItem(row, 0, new QTableWidgetItem(force.taskName));
        ui->forceCalculationTable->setItem(row, 1, new QTableWidgetItem(force.taskTarget));
        ui->forceCalculationTable->setItem(row, 2, new QTableWidgetItem(force.threatLevel));
        ui->forceCalculationTable->setItem(row, 3, new QTableWidgetItem(force.priority));
        ui->forceCalculationTable->setItem(row, 4, new QTableWidgetItem(QString::number(force.calculatedCount)));
        ui->forceCalculationTable->setItem(row, 5, new QTableWidgetItem(QString::number(force.adjustedCount)));
    }
}

// 显示任务分配表格数据
void CooperativeMissionPlan::DisplayAllocationTableToData(const QList<TaskAllocation>& allocationList)
{
    // 清空表格
    ui->allocationTable->clearContents();
    ui->allocationTable->setRowCount(0);
    
    // 添加数据行
    for (const TaskAllocation& allocation : allocationList) {
        int row = ui->allocationTable->rowCount();
        ui->allocationTable->insertRow(row);
        
        ui->allocationTable->setItem(row, 0, new QTableWidgetItem(allocation.taskName));
        ui->allocationTable->setItem(row, 1, new QTableWidgetItem(allocation.targetType));
        ui->allocationTable->setItem(row, 2, new QTableWidgetItem(allocation.taskTarget));
        ui->allocationTable->setItem(row, 3, new QTableWidgetItem(allocation.threatLevel));
        ui->allocationTable->setItem(row, 4, new QTableWidgetItem(allocation.allocatedUAVs));
        ui->allocationTable->setItem(row, 5, new QTableWidgetItem(allocation.formation));
    }
}

// 显示路径规划表格数据
void CooperativeMissionPlan::DisplayPathTableToData(const QList<PathPlanning>& pathList)
{
    // 清空表格
    ui->pathTable->clearContents();
    ui->pathTable->setRowCount(0);
    
    // 添加数据行
    for (const PathPlanning& path : pathList) {
        int row = ui->pathTable->rowCount();
        ui->pathTable->insertRow(row);
        
        ui->pathTable->setItem(row, 0, new QTableWidgetItem(path.uavName));
        ui->pathTable->setItem(row, 1, new QTableWidgetItem(path.relatedTask));
        ui->pathTable->setItem(row, 2, new QTableWidgetItem(QString::number(path.pathPointCount)));
        ui->pathTable->setItem(row, 3, new QTableWidgetItem(path.status));
    }
}

// 显示目标参数表格数据
void CooperativeMissionPlan::DisplayTargetTableToData(const QList<TargetParam>& targetList)
{
    // 清空表格
    ui->targetTable->clearContents();
    ui->targetTable->setRowCount(0);
    
    // 添加数据行
    for (const TargetParam& target : targetList) {
        int row = ui->targetTable->rowCount();
        ui->targetTable->insertRow(row);
        
        ui->targetTable->setItem(row, 0, new QTableWidgetItem(target.targetId));
        ui->targetTable->setItem(row, 1, new QTableWidgetItem(target.frequencyRange));
        ui->targetTable->setItem(row, 2, new QTableWidgetItem(target.pulseWidthRange));
        ui->targetTable->setItem(row, 3, new QTableWidgetItem(target.repetitionPeriod));
        ui->targetTable->setItem(row, 4, new QTableWidgetItem(target.workingMode));
    }
}

void CooperativeMissionPlan::showToast(const QString& message)
{
    QMessageBox::information(this, "提示", message);
}

// 保存任务到文件（辅助函数）
bool CooperativeMissionPlan::saveTasksToFile(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    
    QJsonObject rootObject;
    QJsonArray taskArray;
    
    // 遍历所有任务，转换为JSON对象
    for (const TaskInfo& task : taskList) {
        QJsonObject taskObject;
        taskObject["taskName"] = task.taskName;
        taskObject["taskType"] = task.taskType;
        taskObject["targetType"] = task.targetType;
        taskObject["taskTarget"] = task.taskTarget;
        taskObject["startTime"] = task.startTime.toString("HH:mm");
        taskObject["endTime"] = task.endTime.toString("HH:mm");
        taskObject["allocatedUAVs"] = task.allocatedUAVs;
        
        taskArray.append(taskObject);
    }
    
    rootObject["tasks"] = taskArray;
    
    // 生成JSON文档
    QJsonDocument doc(rootObject);
    
    // 写入文件
    if (file.write(doc.toJson()) == -1) {
        return false;
    }
    
    file.close();
    return true;
}

// 从文件加载任务（辅助函数）
bool CooperativeMissionPlan::loadTasksFromFile(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    
    // 读取文件内容
    QByteArray jsonData = file.readAll();
    file.close();
    
    // 解析JSON文档
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (doc.isNull()) {
        return false;
    }
    
    QJsonObject rootObject = doc.object();
    QJsonArray taskArray = rootObject["tasks"].toArray();
    
    // 清空当前任务列表
    taskList.clear();
    
    // 解析所有任务
    for (const QJsonValue& value : taskArray) {
        if (value.isObject()) {
            QJsonObject taskObject = value.toObject();
            TaskInfo task;
            task.taskName = taskObject["taskName"].toString();
            task.taskType = taskObject["taskType"].toString();
            task.targetType = taskObject["targetType"].toString();
            task.taskTarget = taskObject["taskTarget"].toString();
            task.startTime = QTime::fromString(taskObject["startTime"].toString(), "HH:mm");
            task.endTime = QTime::fromString(taskObject["endTime"].toString(), "HH:mm");
            task.allocatedUAVs = taskObject["allocatedUAVs"].toString();
            
            taskList.append(task);
        }
    }
    
    // 更新表格显示
    DisplayTaskTableToData(taskList);
    
    return true;
}

void CooperativeMissionPlan::onNewTask()
{
    TaskDialog dialog;
    
    // 连接任务添加信号到槽函数
    connect(&dialog, &TaskDialog::taskAdded, this, &CooperativeMissionPlan::onTaskAdded);
    
    // 显示对话框
    dialog.exec();
}

// 打开任务文件
void CooperativeMissionPlan::onOpenTask()
{
    // 打开文件对话框
    QString filePath = QFileDialog::getOpenFileName(
        this, "打开任务文件", "", "JSON文件 (*.json);;所有文件 (*.*)");
    
    if (!filePath.isEmpty()) {
        // 从文件加载任务
        if (loadTasksFromFile(filePath)) {
            // 更新当前文件路径
            currentFilePath = filePath;
            showToast("任务文件已打开");
        } else {
            showToast("打开文件失败");
        }
    }
}

// 保存任务到文件
void CooperativeMissionPlan::onSaveTask()
{
    // 如果没有当前文件路径，则调用另存为
    if (currentFilePath.isEmpty()) {
        onSaveAs();
    } else {
        // 保存到当前文件
        if (saveTasksToFile(currentFilePath)) {
            showToast("任务已保存");
        } else {
            showToast("保存失败");
        }
    }
}

// 另存为新文件
void CooperativeMissionPlan::onSaveAs()
{
    // 打开文件对话框
    QString filePath = QFileDialog::getSaveFileName(
        this, "保存任务文件", "", "JSON文件 (*.json);;所有文件 (*.*)");
    
    if (!filePath.isEmpty()) {
        // 保存到选择的文件
        if (saveTasksToFile(filePath)) {
            // 更新当前文件路径
            currentFilePath = filePath;
            showToast("任务已另存为新文件");
        } else {
            showToast("保存失败");
        }
    }
}

void CooperativeMissionPlan::onExport()
{
    showToast("正在导出...");
}

void CooperativeMissionPlan::onEditTask()
{
    // 获取当前选中的行
    int selectedRow = ui->taskTable->currentRow();
    
    // 检查是否有选中的行
    if (selectedRow < 0) {
        showToast("请先选择一个任务");
        return;
    }
    
    // 检查索引是否有效
    if (selectedRow >= taskList.size()) {
        showToast("任务数据错误");
        return;
    }
    
    // 获取选中的任务信息
    TaskInfo taskInfo = taskList[selectedRow];
    
    // 创建编辑任务对话框
    TaskDialog dialog(selectedRow, taskInfo, this);
    
    // 连接任务更新信号到槽函数
    connect(&dialog, &TaskDialog::taskUpdated, this, &CooperativeMissionPlan::onTaskUpdated);
    
    // 显示对话框
    dialog.exec();
}

void CooperativeMissionPlan::onDeleteTask()
{
    // 获取当前选中的行
    int selectedRow = ui->taskTable->currentRow();
    
    // 检查是否有选中的行
    if (selectedRow < 0) {
        showToast("请先选择一个任务");
        return;
    }
    
    // 检查索引是否有效
    if (selectedRow >= taskList.size()) {
        showToast("任务数据错误");
        return;
    }
    
    // 从内部数据列表中删除任务
    taskList.removeAt(selectedRow);
    
    // 更新表格显示
    DisplayTaskTableToData(taskList);
    
    // 显示提示信息
    showToast("任务已删除");
}

void CooperativeMissionPlan::onCalculateForce()
{
    // 清空现有兵力需求计算列表
    forceList.clear();
    
    // 计算每个任务的兵力需求
    int totalRequired = 0;
    for (const auto& task : taskList) {
        ForceCalculation force;
        force.taskName = task.taskName;
        force.taskTarget = task.taskTarget;
        
        // 根据任务类型和目标类型计算兵力需求
        // 这里使用简化的计算逻辑，实际应用中可能需要更复杂的算法
        if (task.taskType == "打击") {
            force.threatLevel = "高";
            force.priority = "P1";
            force.calculatedCount = 2; // 打击任务需要2架无人机
        } else if (task.taskType == "压制") {
            force.threatLevel = "中";
            force.priority = "P2";
            force.calculatedCount = 1; // 压制任务需要1架无人机
        } else {
            force.threatLevel = "低";
            force.priority = "P3";
            force.calculatedCount = 1; // 其他任务需要1架无人机
        }
        
        // 调整数量（这里简单设为计算数量）
        force.adjustedCount = force.calculatedCount;
        
        forceList.append(force);
        totalRequired += force.adjustedCount;
    }
    
    // 更新兵力计算表格显示
    DisplayForceCalculationTableToData(forceList);
    
    // 更新兵力资源管理树
    // 这里简单地更新树的显示，实际应用中可能需要根据计算结果调整无人机状态
    updateForceTree();
    
    // 显示计算结果
    showToast(QString("兵力需求计算完成，合计 %1 架").arg(totalRequired));
}

void CooperativeMissionPlan::onAutoAllocate()
{
    showToast("自动分配完成，已优化兵力分配方案");
}

void CooperativeMissionPlan::onGeneratePath()
{
    showToast("路径生成中...");
}

void CooperativeMissionPlan::onBindParameters()
{
    showToast("目标参数已装载");
}

void CooperativeMissionPlan::onSettings()
{
    showToast("打开设置");
}

void CooperativeMissionPlan::onTaskSelected(int index)
{
    selectedTaskIndex = index;
    showToast(QString("选中任务 %1").arg(index + 1));
}



void CooperativeMissionPlan::onSpinValueChanged(int id, int delta)
{
    Q_UNUSED(id);
    Q_UNUSED(delta);
    // Implementation for spin controls
}

void CooperativeMissionPlan::onTreeItemToggled(const QString& path, bool expanded)
{
    Q_UNUSED(path);
    showToast(expanded ? "展开节点" : "折叠节点");
}

void CooperativeMissionPlan::onTabButtonClicked()
{
    // 获取发送信号的按钮
    QPushButton* senderButton = qobject_cast<QPushButton*>(sender());
    if (!senderButton) return;
    
    // 重置所有按钮样式
    QPushButton* buttons[] = {
        ui->dataManagementButton,
        ui->cooperativePlanningButton,
        ui->uavPlanningButton,
        ui->situationAnalysisButton,
        ui->situationDisplayButton
    };
    
    QString normalStyle = "QPushButton { padding: 0 18px; height: 40px; color: #8ab8d8; background: transparent; border: none; border-right: 1px solid rgba(0,140,220,0.2); } QPushButton:hover { color: #00cfff; background: rgba(0,150,255,0.08); } QPushButton:pressed { color: #00cfff; background: rgba(0,130,220,0.15); }";
    QString activeStyle = "QPushButton { padding: 0 18px; height: 40px; color: #e8f4ff; background: rgba(0,130,220,0.2); border: none; border-right: 1px solid rgba(0,140,220,0.2); } QPushButton:hover { color: #00cfff; background: rgba(0,150,255,0.08); } QPushButton:pressed { color: #00cfff; background: rgba(0,130,220,0.15); }";
    QString lastNormalStyle = "QPushButton { padding: 0 18px; height: 40px; color: #8ab8d8; background: transparent; border: none; } QPushButton:hover { color: #00cfff; background: rgba(0,150,255,0.08); } QPushButton:pressed { color: #00cfff; background: rgba(0,130,220,0.15); }";
    QString lastActiveStyle = "QPushButton { padding: 0 18px; height: 40px; color: #e8f4ff; background: rgba(0,130,220,0.2); border: none; } QPushButton:hover { color: #00cfff; background: rgba(0,150,255,0.08); } QPushButton:pressed { color: #00cfff; background: rgba(0,130,220,0.15); }";
    
    for (int i = 0; i < 5; ++i) {
        if (buttons[i] == senderButton) {
            // 设置活动按钮样式
            if (i == 4) {
                buttons[i]->setStyleSheet(lastActiveStyle);
            } else {
                buttons[i]->setStyleSheet(activeStyle);
            }
        } else {
            // 设置普通按钮样式
            if (i == 4) {
                buttons[i]->setStyleSheet(lastNormalStyle);
            } else {
                buttons[i]->setStyleSheet(normalStyle);
            }
        }
    }
    
    // 在这里添加页面切换逻辑
    showToast(QString("切换到%1页面").arg(senderButton->text()));
}

void CooperativeMissionPlan::onShutdown()
{
    // 关闭应用程序
    QApplication::quit();
}