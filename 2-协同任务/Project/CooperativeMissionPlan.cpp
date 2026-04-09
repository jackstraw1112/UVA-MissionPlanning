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
#include <QDebug>

CooperativeMissionPlan::CooperativeMissionPlan(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CooperativeMissionPlan)
    , selectedTaskIndex(0)
    , currentFilePath("")
{
    ui->setupUi(this);

    if (!DatabaseManager::instance().initDatabase()) {
        qWarning() << "数据库初始化失败:" << DatabaseManager::instance().getLastError();
    }

    clockTimer = new QTimer(this);
    connect(clockTimer, &QTimer::timeout, this, &CooperativeMissionPlan::updateClock);
    clockTimer->start(1000);
    updateClock();

    setupConnections();
    setupStyles();
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
    connect(ui->addTargetParamButton, &QPushButton::clicked, this, &CooperativeMissionPlan::onAddTargetParam);
    connect(ui->editTargetParamButton, &QPushButton::clicked, this, &CooperativeMissionPlan::onEditTargetParam);

    // Task plan manager button
    connect(ui->taskPlanManagerButton, &QPushButton::clicked, this, &CooperativeMissionPlan::onOpenTaskPlanManager);

    // Path table double click
    connect(ui->pathTable, &QTableWidget::cellDoubleClicked, [this](int row, int column) {
        Q_UNUSED(column);
        onShowPathDetail();
    });

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


    QStringList radartargetTable = {"目标编号", "频率范围(MHz)", "脉宽范围(μs)", "重复周期(ms)", "工作模式"};
    ui->radartargetTable->setColumnCount(5); // Ensure we have 5 columns
    ui->radartargetTable->setHorizontalHeaderLabels(radartargetTable); // Fix: use correct headers
    ui->radartargetTable->horizontalHeader()->setStretchLastSection(true);
    ui->radartargetTable->verticalHeader()->setVisible(false);
    ui->radartargetTable->horizontalHeader()->setVisible(true); // Ensure headers are visible
    ui->radartargetTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft); // Align header text to left
    ui->radartargetTable->horizontalHeader()->setMinimumHeight(24); // Ensure header has sufficient height
    ui->radartargetTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->radartargetTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->radartargetTable->setAlternatingRowColors(false);


    QStringList radiotargetTable = {"目标编号", "频率范围(MHz)", "调制方式", "信号带宽(KHz)", "发射功率(KW)"};
    ui->radiotargetTable->setColumnCount(5); // Ensure we have 5 columns
    ui->radiotargetTable->setHorizontalHeaderLabels(radiotargetTable); // Fix: use correct headers
    ui->radiotargetTable->horizontalHeader()->setStretchLastSection(true);
    ui->radiotargetTable->verticalHeader()->setVisible(false);
    ui->radiotargetTable->horizontalHeader()->setVisible(true); // Ensure headers are visible
    ui->radiotargetTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft); // Align header text to left
    ui->radiotargetTable->horizontalHeader()->setMinimumHeight(24); // Ensure header has sufficient height
    ui->radiotargetTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->radiotargetTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->radiotargetTable->setAlternatingRowColors(false);

    QStringList radiofighttargetTable = {"目标编号", "干扰频率(MHz)", "干扰样式", "干扰功率(KW)", "覆盖范围(km)"};
    ui->radiofighttargetTable->setColumnCount(5); // Ensure we have 5 columns
    ui->radiofighttargetTable->setHorizontalHeaderLabels(radiofighttargetTable); // Fix: use correct headers
    ui->radiofighttargetTable->horizontalHeader()->setStretchLastSection(true);
    ui->radiofighttargetTable->verticalHeader()->setVisible(false);
    ui->radiofighttargetTable->horizontalHeader()->setVisible(true); // Ensure headers are visible
    ui->radiofighttargetTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft); // Align header text to left
    ui->radiofighttargetTable->horizontalHeader()->setMinimumHeight(24); // Ensure header has sufficient height
    ui->radiofighttargetTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->radiofighttargetTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->radiofighttargetTable->setAlternatingRowColors(false);


    QStringList radarfighttargetTable = {"目标编号", "干扰频率(MHz)", "干扰类型", "干扰功率(KW)", "工作方向"};
    ui->radarfighttargetTable->setColumnCount(5); // Ensure we have 5 columns
    ui->radarfighttargetTable->setHorizontalHeaderLabels(radarfighttargetTable); // Fix: use correct headers
    ui->radarfighttargetTable->horizontalHeader()->setStretchLastSection(true);
    ui->radarfighttargetTable->verticalHeader()->setVisible(false);
    ui->radarfighttargetTable->horizontalHeader()->setVisible(true); // Ensure headers are visible
    ui->radarfighttargetTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft); // Align header text to left
    ui->radarfighttargetTable->horizontalHeader()->setMinimumHeight(24); // Ensure header has sufficient height
    ui->radarfighttargetTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->radarfighttargetTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->radarfighttargetTable->setAlternatingRowColors(false);

}




void CooperativeMissionPlan::onAddForce()
{
    AddForceDialog dialog(this);
    connect(&dialog, &AddForceDialog::forceAdded, this, &CooperativeMissionPlan::onForceAdded);
    dialog.exec();
}

void CooperativeMissionPlan::onForceAdded(const GroupInfo& groupInfo)
{
    groupList.append(groupInfo);
    
    DisplayForceTreeToData(groupList);
    
    showToast(QString("已添加编组：%1").arg(groupInfo.groupName));
}

void CooperativeMissionPlan::onDeleteForce()
{
    QTreeWidgetItem* selectedItem = ui->forceTree->currentItem();
    if (!selectedItem) {
        showToast("请先选中要删除的节点");
        return;
    }
    
    QVariant data = selectedItem->data(0, Qt::UserRole);
    if (!data.isValid()) {
        showToast("无法获取节点信息");
        return;
    }
    
    QString itemType = selectedItem->text(0);
    
    if (itemType.startsWith("●")) {
        // 删除无人机节点
        QPoint indices = data.value<QPoint>();
        int groupIndex = indices.x();
        int equipIndex = indices.y();
        int uavIndex = selectedItem->data(1, Qt::UserRole).toInt();
        
        if (groupIndex >= 0 && groupIndex < groupList.size() &&
            equipIndex >= 0 && equipIndex < groupList[groupIndex].equipmentList.size()) {
            QString uavName = selectedItem->text(0);
            groupList[groupIndex].equipmentList[equipIndex].uavList.removeAt(uavIndex);
            showToast(QString("已删除无人机：%1").arg(uavName));
        }
    } else if (itemType.startsWith("📂")) {
        // 删除装备型号节点
        QPoint indices = data.value<QPoint>();
        int groupIndex = indices.x();
        int equipIndex = indices.y();
        
        if (groupIndex >= 0 && groupIndex < groupList.size() &&
            equipIndex >= 0 && equipIndex < groupList[groupIndex].equipmentList.size()) {
            QString equipName = selectedItem->text(0);
            groupList[groupIndex].equipmentList.removeAt(equipIndex);
            showToast(QString("已删除装备：%1").arg(equipName));
        }
    } else {
        // 删除编组节点
        int groupIndex = data.toInt();
        if (groupIndex >= 0 && groupIndex < groupList.size()) {
            QString groupName = selectedItem->text(0);
            groupList.removeAt(groupIndex);
            showToast(QString("已删除编组：%1").arg(groupName));
        }
    }
    
    DisplayForceTreeToData(groupList);
}

void CooperativeMissionPlan::onPathTabChanged(int index)
{
    QPushButton* tabs[] = { ui->flightPathTab, ui->searchPathTab };
    for (int i = 0; i < 2; i++) {
        if (i == index) {
            tabs[i]->setStyleSheet("QPushButton {\nbackground: rgba(0,150,255,0.25);\nborder: 1px solid rgba(0,180,255,0.4);\nborder-bottom: none;\nborder-radius: 3px 3px 0 0;\ncolor: #8ad6ff;\nfont-size: 12px;\n}\nQPushButton:hover {\nbackground: rgba(0,160,255,0.3);\ncolor: #c8e8ff;\n}");
        } else {
            tabs[i]->setStyleSheet("QPushButton {\nbackground: rgba(0,80,160,0.15);\nborder: 1px solid rgba(0,120,200,0.3);\nborder-bottom: none;\nborder-radius: 3px 3px 0 0;\ncolor: #99ccee;\nfont-size: 12px;\n}\nQPushButton:hover {\nbackground: rgba(0,100,180,0.2);\ncolor: #c8e8ff;\n}");
        }
    }

    if (index == 0) {
        DisplayPathTableToData(cruisePathList);
    } else if (index == 1) {
        DisplayPathTableToData(searchPathList);
    }
}

void CooperativeMissionPlan::onTargetTabChanged(int index)
{
    QString tabNames[] = { "雷达", "电台", "通信对抗", "雷达对抗" };
}

void CooperativeMissionPlan::onGenerateSelectedPath()
{
    int currentRow = ui->pathTable->currentRow();
    if (currentRow < 0) {
        showToast("请先选中要生成路径的无人机");
        return;
    }

    QString selectedUAVName = ui->pathTable->item(currentRow, 0)->text();

    int generatedCount = 0;
    for (auto& path : cruisePathList) {
        if (path.uavName == selectedUAVName && path.status == "待生成") {
            path.status = "已生成";
            generatedCount++;
        }
    }

    for (auto& path : searchPathList) {
        if (path.uavName == selectedUAVName && path.status == "待生成") {
            path.status = "已生成";
            generatedCount++;
        }
    }

    DisplayPathTableToData(cruisePathList);

    if (generatedCount > 0) {
        showToast(QString("已为 %1 生成路径").arg(selectedUAVName));
    } else {
        showToast(QString("%1 的路径已生成").arg(selectedUAVName));
    }
}

void CooperativeMissionPlan::onGenerateAllPaths()
{
    int generatedCount = 0;

    for (auto& path : cruisePathList) {
        if (path.status == "待生成") {
            path.status = "已生成";
            generatedCount++;
        }
    }

    for (auto& path : searchPathList) {
        if (path.status == "待生成") {
            path.status = "已生成";
            generatedCount++;
        }
    }

    DisplayPathTableToData(cruisePathList);

    showToast(QString("所有路径生成完成，已生成 %1 条路径").arg(generatedCount));
}

void CooperativeMissionPlan::onShowPathDetail()
{
    int currentRow = ui->pathTable->currentRow();
    if (currentRow < 0) {
        showToast("请先选中要查看的无人机路径");
        return;
    }

    QString selectedUAVName = ui->pathTable->item(currentRow, 0)->text();
    QString relatedTask = ui->pathTable->item(currentRow, 1)->text();

    PathPlanning* selectedPath = nullptr;
    for (auto& path : cruisePathList) {
        if (path.uavName == selectedUAVName && path.relatedTask == relatedTask) {
            selectedPath = &path;
            break;
        }
    }

    if (!selectedPath) {
        for (auto& path : searchPathList) {
            if (path.uavName == selectedUAVName && path.relatedTask == relatedTask) {
                selectedPath = &path;
                break;
            }
        }
    }

    if (selectedPath) {
        PathDisplayDialog dialog(this);
        dialog.setPathData(*selectedPath);
        dialog.exec();
    } else {
        showToast("未找到对应的路径信息");
    }
}

void CooperativeMissionPlan::onAddTargetParam()
{
    TargetParamDialog dialog(this);
    dialog.setPlanInfo(currentPlanName, currentCoordinationName);
    connect(&dialog, &TargetParamDialog::radarTargetAdded, this, &CooperativeMissionPlan::onRadarTargetAdded);
    connect(&dialog, &TargetParamDialog::radioTargetAdded, this, &CooperativeMissionPlan::onRadioTargetAdded);
    connect(&dialog, &TargetParamDialog::commJammingAdded, this, &CooperativeMissionPlan::onCommJammingAdded);
    connect(&dialog, &TargetParamDialog::rcmJammingAdded, this, &CooperativeMissionPlan::onRcmJammingAdded);
    dialog.exec();
}

void CooperativeMissionPlan::onRadarTargetAdded(const RadarTargetParam& radar)
{
    radarTargetList.append(radar);
    DisplayRadarTargetTableToData(radarTargetList);
    showToast(QString("已添加雷达目标：%1").arg(radar.targetId));
}

void CooperativeMissionPlan::onRadioTargetAdded(const RadioTargetParam& radio)
{
    radioTargetList.append(radio);
    DisplayRadioTargetTableToData(radioTargetList);
    showToast(QString("已添加电台目标：%1").arg(radio.targetId));
}

void CooperativeMissionPlan::onCommJammingAdded(const CommJammingParam& comm)
{
    commJammingList.append(comm);
    DisplayCommJammingTableToData(commJammingList);
    showToast(QString("已添加通信对抗：%1").arg(comm.targetId));
}

void CooperativeMissionPlan::onRcmJammingAdded(const RcmJammingParam& rcm)
{
    rcmJammingList.append(rcm);
    DisplayRcmJammingTableToData(rcmJammingList);
    showToast(QString("已添加雷达对抗：%1").arg(rcm.targetId));
}

void CooperativeMissionPlan::onEditTargetParam()
{
    int currentTab = ui->tabWidget->currentIndex();
    int selectedRow = -1;
    QTableWidget* currentTable = nullptr;
    QString dialogTitle;

    switch (currentTab) {
    case 0:
        currentTable = ui->radartargetTable;
        dialogTitle = "编辑雷达目标参数";
        break;
    case 1:
        currentTable = ui->radiotargetTable;
        dialogTitle = "编辑电台目标参数";
        break;
    case 2:
        currentTable = ui->radiofighttargetTable;
        dialogTitle = "编辑通信对抗参数";
        break;
    case 3:
        currentTable = ui->radarfighttargetTable;
        dialogTitle = "编辑雷达对抗参数";
        break;
    default:
        showToast("未知的目标参数类型");
        return;
    }

    selectedRow = currentTable->currentRow();
    if (selectedRow < 0) {
        showToast("请先选中要编辑的参数");
        return;
    }

    TargetParamDialog dialog(this);
    dialog.setWindowTitle(dialogTitle);
    dialog.setPlanInfo(currentPlanName, currentCoordinationName);

    connect(&dialog, &TargetParamDialog::radarTargetUpdated, this, &CooperativeMissionPlan::onRadarTargetUpdated);
    connect(&dialog, &TargetParamDialog::radioTargetUpdated, this, &CooperativeMissionPlan::onRadioTargetUpdated);
    connect(&dialog, &TargetParamDialog::commJammingUpdated, this, &CooperativeMissionPlan::onCommJammingUpdated);
    connect(&dialog, &TargetParamDialog::rcmJammingUpdated, this, &CooperativeMissionPlan::onRcmJammingUpdated);

    switch (currentTab) {
    case 0: {
        RadarTargetParam radar = radarTargetList.at(selectedRow);
        dialog.setRadarTarget(radar);
        break;
    }
    case 1: {
        RadioTargetParam radio = radioTargetList.at(selectedRow);
        dialog.setRadioTarget(radio);
        break;
    }
    case 2: {
        CommJammingParam comm = commJammingList.at(selectedRow);
        dialog.setCommJamming(comm);
        break;
    }
    case 3: {
        RcmJammingParam rcm = rcmJammingList.at(selectedRow);
        dialog.setRcmJamming(rcm);
        break;
    }
    }

    dialog.exec();
}

void CooperativeMissionPlan::onRadarTargetUpdated(const RadarTargetParam& radar, int index)
{
    Q_UNUSED(index);
    int currentRow = ui->radartargetTable->currentRow();
    if (currentRow >= 0 && currentRow < radarTargetList.size()) {
        radarTargetList[currentRow] = radar;
        DisplayRadarTargetTableToData(radarTargetList);
        showToast(QString("已更新雷达目标：%1").arg(radar.targetId));
    }
}

void CooperativeMissionPlan::onRadioTargetUpdated(const RadioTargetParam& radio, int index)
{
    Q_UNUSED(index);
    int currentRow = ui->radiotargetTable->currentRow();
    if (currentRow >= 0 && currentRow < radioTargetList.size()) {
        radioTargetList[currentRow] = radio;
        DisplayRadioTargetTableToData(radioTargetList);
        showToast(QString("已更新电台目标：%1").arg(radio.targetId));
    }
}

void CooperativeMissionPlan::onCommJammingUpdated(const CommJammingParam& comm, int index)
{
    Q_UNUSED(index);
    int currentRow = ui->radiofighttargetTable->currentRow();
    if (currentRow >= 0 && currentRow < commJammingList.size()) {
        commJammingList[currentRow] = comm;
        DisplayCommJammingTableToData(commJammingList);
        showToast(QString("已更新通信对抗：%1").arg(comm.targetId));
    }
}

void CooperativeMissionPlan::onRcmJammingUpdated(const RcmJammingParam& rcm, int index)
{
    Q_UNUSED(index);
    int currentRow = ui->radarfighttargetTable->currentRow();
    if (currentRow >= 0 && currentRow < rcmJammingList.size()) {
        rcmJammingList[currentRow] = rcm;
        DisplayRcmJammingTableToData(rcmJammingList);
        showToast(QString("已更新雷达对抗：%1").arg(rcm.targetId));
    }
}

void CooperativeMissionPlan::onSaveAllocation()
{

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
    this->taskList = taskList;

    ui->taskTable->clearContents();
    ui->taskTable->setRowCount(0);

    for (const TaskInfo& task : taskList) {
        int row = ui->taskTable->rowCount();
        ui->taskTable->insertRow(row);

        ui->taskTable->setItem(row, 0, new QTableWidgetItem(task.planName));
        ui->taskTable->setItem(row, 1, new QTableWidgetItem(task.coordinationName));
        ui->taskTable->setItem(row, 2, new QTableWidgetItem(task.taskName));
        ui->taskTable->setItem(row, 3, new QTableWidgetItem(task.taskType));
        ui->taskTable->setItem(row, 4, new QTableWidgetItem(task.targetType));
        ui->taskTable->setItem(row, 5, new QTableWidgetItem(task.taskTarget));
        ui->taskTable->setItem(row, 6, new QTableWidgetItem(task.getTimeRange()));
        ui->taskTable->setItem(row, 7, new QTableWidgetItem(task.allocatedUAVs));
    }
}

// 显示兵力需求计算表格数据
void CooperativeMissionPlan::DisplayForceCalculationTableToData(const QList<ForceCalculation>& forceList)
{
    ui->forceCalculationTable->clearContents();
    ui->forceCalculationTable->setRowCount(0);

    for (const ForceCalculation& force : forceList) {
        int row = ui->forceCalculationTable->rowCount();
        ui->forceCalculationTable->insertRow(row);

        ui->forceCalculationTable->setItem(row, 0, new QTableWidgetItem(force.planName));
        ui->forceCalculationTable->setItem(row, 1, new QTableWidgetItem(force.coordinationName));
        ui->forceCalculationTable->setItem(row, 2, new QTableWidgetItem(force.taskName));
        ui->forceCalculationTable->setItem(row, 3, new QTableWidgetItem(force.taskTarget));
        ui->forceCalculationTable->setItem(row, 4, new QTableWidgetItem(force.threatLevel));
        ui->forceCalculationTable->setItem(row, 5, new QTableWidgetItem(force.priority));
        ui->forceCalculationTable->setItem(row, 6, new QTableWidgetItem(QString::number(force.calculatedCount)));
        ui->forceCalculationTable->setItem(row, 7, new QTableWidgetItem(QString::number(force.adjustedCount)));
    }
}

// 显示任务分配表格数据
void CooperativeMissionPlan::DisplayAllocationTableToData(const QList<TaskAllocation>& allocationList)
{
    ui->allocationTable->clearContents();
    ui->allocationTable->setRowCount(0);

    for (const TaskAllocation& allocation : allocationList) {
        int row = ui->allocationTable->rowCount();
        ui->allocationTable->insertRow(row);

        ui->allocationTable->setItem(row, 0, new QTableWidgetItem(allocation.planName));
        ui->allocationTable->setItem(row, 1, new QTableWidgetItem(allocation.coordinationName));
        ui->allocationTable->setItem(row, 2, new QTableWidgetItem(allocation.taskName));
        ui->allocationTable->setItem(row, 3, new QTableWidgetItem(allocation.targetType));
        ui->allocationTable->setItem(row, 4, new QTableWidgetItem(allocation.taskTarget));
        ui->allocationTable->setItem(row, 5, new QTableWidgetItem(allocation.threatLevel));
        ui->allocationTable->setItem(row, 6, new QTableWidgetItem(allocation.allocatedUAVs));
        ui->allocationTable->setItem(row, 7, new QTableWidgetItem(allocation.formation));
    }
}

// 显示路径规划表格数据
void CooperativeMissionPlan::DisplayPathTableToData(const QList<PathPlanning>& pathList)
{
    ui->pathTable->clearContents();
    ui->pathTable->setRowCount(0);

    for (const PathPlanning& path : pathList) {
        int row = ui->pathTable->rowCount();
        ui->pathTable->insertRow(row);

        ui->pathTable->setItem(row, 0, new QTableWidgetItem(path.planName));
        ui->pathTable->setItem(row, 1, new QTableWidgetItem(path.coordinationName));
        ui->pathTable->setItem(row, 2, new QTableWidgetItem(path.uavName));
        ui->pathTable->setItem(row, 3, new QTableWidgetItem(path.relatedTask));
        ui->pathTable->setItem(row, 4, new QTableWidgetItem(QString::number(path.pathPointCount)));
        ui->pathTable->setItem(row, 5, new QTableWidgetItem(path.status));
    }
}

void CooperativeMissionPlan::DisplayRadarTargetTableToData(const QList<RadarTargetParam>& radarList)
{
    ui->radartargetTable->clearContents();
    ui->radartargetTable->setRowCount(0);

    for (const RadarTargetParam& radar : radarList) {
        int row = ui->radartargetTable->rowCount();
        ui->radartargetTable->insertRow(row);

        ui->radartargetTable->setItem(row, 0, new QTableWidgetItem(radar.planName));
        ui->radartargetTable->setItem(row, 1, new QTableWidgetItem(radar.coordinationName));
        ui->radartargetTable->setItem(row, 2, new QTableWidgetItem(radar.targetId));
        ui->radartargetTable->setItem(row, 3, new QTableWidgetItem(radar.frequencyRange));
        ui->radartargetTable->setItem(row, 4, new QTableWidgetItem(radar.pulseWidthRange));
        ui->radartargetTable->setItem(row, 5, new QTableWidgetItem(radar.repetitionPeriod));
        ui->radartargetTable->setItem(row, 6, new QTableWidgetItem(radar.workingMode));
    }
}

void CooperativeMissionPlan::DisplayRadioTargetTableToData(const QList<RadioTargetParam>& radioList)
{
    ui->radiotargetTable->clearContents();
    ui->radiotargetTable->setRowCount(0);

    for (const RadioTargetParam& radio : radioList) {
        int row = ui->radiotargetTable->rowCount();
        ui->radiotargetTable->insertRow(row);

        ui->radiotargetTable->setItem(row, 0, new QTableWidgetItem(radio.planName));
        ui->radiotargetTable->setItem(row, 1, new QTableWidgetItem(radio.coordinationName));
        ui->radiotargetTable->setItem(row, 2, new QTableWidgetItem(radio.targetId));
        ui->radiotargetTable->setItem(row, 3, new QTableWidgetItem(radio.frequencyRange));
        ui->radiotargetTable->setItem(row, 4, new QTableWidgetItem(radio.modulationMode));
        ui->radiotargetTable->setItem(row, 5, new QTableWidgetItem(radio.signalBandwidth));
        ui->radiotargetTable->setItem(row, 6, new QTableWidgetItem(radio.transmitPower));
    }
}

void CooperativeMissionPlan::DisplayCommJammingTableToData(const QList<CommJammingParam>& commList)
{
    ui->radiofighttargetTable->clearContents();
    ui->radiofighttargetTable->setRowCount(0);

    for (const CommJammingParam& comm : commList) {
        int row = ui->radiofighttargetTable->rowCount();
        ui->radiofighttargetTable->insertRow(row);

        ui->radiofighttargetTable->setItem(row, 0, new QTableWidgetItem(comm.planName));
        ui->radiofighttargetTable->setItem(row, 1, new QTableWidgetItem(comm.coordinationName));
        ui->radiofighttargetTable->setItem(row, 2, new QTableWidgetItem(comm.targetId));
        ui->radiofighttargetTable->setItem(row, 3, new QTableWidgetItem(comm.jammingFrequency));
        ui->radiofighttargetTable->setItem(row, 4, new QTableWidgetItem(comm.jammingMode));
        ui->radiofighttargetTable->setItem(row, 5, new QTableWidgetItem(comm.jammingPower));
        ui->radiofighttargetTable->setItem(row, 6, new QTableWidgetItem(comm.coverageRange));
    }
}

void CooperativeMissionPlan::DisplayRcmJammingTableToData(const QList<RcmJammingParam>& rcmList)
{
    ui->radarfighttargetTable->clearContents();
    ui->radarfighttargetTable->setRowCount(0);

    for (const RcmJammingParam& rcm : rcmList) {
        int row = ui->radarfighttargetTable->rowCount();
        ui->radarfighttargetTable->insertRow(row);

        ui->radarfighttargetTable->setItem(row, 0, new QTableWidgetItem(rcm.planName));
        ui->radarfighttargetTable->setItem(row, 1, new QTableWidgetItem(rcm.coordinationName));
        ui->radarfighttargetTable->setItem(row, 2, new QTableWidgetItem(rcm.targetId));
        ui->radarfighttargetTable->setItem(row, 3, new QTableWidgetItem(rcm.jammingFrequency));
        ui->radarfighttargetTable->setItem(row, 4, new QTableWidgetItem(rcm.jammingMode));
        ui->radarfighttargetTable->setItem(row, 5, new QTableWidgetItem(rcm.jammingPower));
        ui->radarfighttargetTable->setItem(row, 6, new QTableWidgetItem(rcm.coverageRange));
    }
}



//显示兵力树资源
void CooperativeMissionPlan::DisplayForceTreeToData(const QList<GroupInfo>& groupList)
{
    // 清空现有内容
    ui->forceTree->clear();

    // 设置列宽
    ui->forceTree->setColumnWidth(0, 280);
    ui->forceTree->setColumnWidth(1, 80);

    // 遍历所有编组
    for (int groupIndex = 0; groupIndex < groupList.size(); ++groupIndex) {
        const auto& group = groupList[groupIndex];
        QTreeWidgetItem* groupItem = new QTreeWidgetItem(ui->forceTree);
        int available = group.getAvailableUAVCount();
        int total = group.getTotalUAVCount();
        groupItem->setText(0, group.groupName);
        groupItem->setText(1, QString("%1/%2").arg(available).arg(total));
        groupItem->setData(0, Qt::UserRole, groupIndex);
        groupItem->setExpanded(true);

        // 遍历该编组下的所有装备型号
        for (int equipIndex = 0; equipIndex < group.equipmentList.size(); ++equipIndex) {
            const auto& equipment = group.equipmentList[equipIndex];
            QTreeWidgetItem* equipmentItem = new QTreeWidgetItem(groupItem);
            int eqAvailable = 0;
            for (const auto& uav : equipment.uavList) {
                if (uav.status == "就绪") {
                    eqAvailable++;
                }
            }
            equipmentItem->setText(0, "📂 " + equipment.equipmentName);
            equipmentItem->setText(1, QString("%1/%2").arg(eqAvailable).arg(equipment.uavList.size()));
            equipmentItem->setData(0, Qt::UserRole, QVariant::fromValue(QPoint(groupIndex, equipIndex)));
            equipmentItem->setExpanded(true);

            // 遍历该装备型号下的所有无人机
            for (int uavIndex = 0; uavIndex < equipment.uavList.size(); ++uavIndex) {
                const auto& uav = equipment.uavList[uavIndex];
                QTreeWidgetItem* uavItem = new QTreeWidgetItem(equipmentItem);
                uavItem->setText(0, "● " + uav.uavName);
                uavItem->setText(1, uav.status);
                uavItem->setData(0, Qt::UserRole, QVariant::fromValue(QPoint(groupIndex, equipIndex)));
                uavItem->setData(1, Qt::UserRole, uavIndex);
            }
        }
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
    if (DatabaseManager::instance().saveAllData(
            taskList,
            forceList,
            groupList,
            allocationList,
            cruisePathList,
            searchPathList,
            radarTargetList,
            radioTargetList,
            commJammingList,
            rcmJammingList)) {
        showToast("数据已保存到数据库");
    } else {
        showToast("保存失败: " + DatabaseManager::instance().getLastError());
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
    int selectedRow = ui->taskTable->currentRow();

    if (selectedRow < 0) {
        showToast("请先选择一个任务");
        return;
    }

    if (selectedRow >= taskList.size()) {
        showToast("任务数据错误");
        return;
    }

    taskList.removeAt(selectedRow);

    DisplayTaskTableToData(taskList);

    showToast("任务已删除");
}

void CooperativeMissionPlan::onOpenTaskPlanManager()
{
    TaskPlanManagerDialog dialog(this);
    dialog.setTaskPlans(taskList);

    connect(&dialog, &TaskPlanManagerDialog::taskPlanSelected, this, &CooperativeMissionPlan::onTaskPlanSelected);

    if (dialog.exec() == QDialog::Accepted) {
        taskList = dialog.getTaskPlans();
        DisplayTaskTableToData(taskList);
    }
}

void CooperativeMissionPlan::onTaskPlanSelected(const TaskInfo& taskPlan)
{
    currentPlanName = taskPlan.planName;
    currentCoordinationName = taskPlan.coordinationName;

    int index = taskList.indexOf(taskPlan);
    if (index >= 0) {
        selectedTaskIndex = index;
        ui->taskTable->selectRow(index);
    }

    showToast(QString("已选择方案: %1 - %2").arg(taskPlan.planName).arg(taskPlan.taskName));
}

void CooperativeMissionPlan::onCalculateForce()
{
    // 清空现有兵力需求计算列表和编组信息
    forceList.clear();
    groupList.clear();
    
    // 首先计算所有任务的兵力需求
    int totalRequired = 0;
    QList<QString> strikeUAVs;   // 打击任务需要的无人机列表
    QList<QString> suppressUAVs;  // 压制任务需要的无人机列表
    
    for (const auto& task : taskList) {
        ForceCalculation force;
        force.taskName = task.taskName;
        force.taskTarget = task.taskTarget;
        
        // 根据任务类型计算兵力需求
        if (task.taskType == "打击") {
            force.threatLevel = "高";
            force.priority = "P1";
            force.calculatedCount = 2; // 打击任务需要2架无人机
            strikeUAVs.append("ARM-A" + QString::number(strikeUAVs.size() / 2 + 1));
            strikeUAVs.append("ARM-A" + QString::number(strikeUAVs.size() / 2 + 1));
        } else if (task.taskType == "压制") {
            force.threatLevel = "中";
            force.priority = "P2";
            force.calculatedCount = 1; // 压制任务需要1架无人机
            suppressUAVs.append("ARM-C" + QString::number(suppressUAVs.size() + 1));
        } else {
            force.threatLevel = "低";
            force.priority = "P3";
            force.calculatedCount = 1; // 其他任务需要1架无人机
            strikeUAVs.append("ARM-B" + QString::number(strikeUAVs.size() + 1));
        }
        
        force.adjustedCount = force.calculatedCount;
        forceList.append(force);
        totalRequired += force.adjustedCount;
    }
    
    // 统计打击编队数量（每2架无人机为一个打击编队）
    int strikeTeamCount = (strikeUAVs.size() + 1) / 2;
    if (strikeTeamCount == 0) strikeTeamCount = 1;
    
    // 统计压制编队数量（每架无人机为一个压制编队，但至少1个）
    int suppressTeamCount = suppressUAVs.size();
    if (suppressTeamCount == 0) suppressTeamCount = 1;
    
    // 动态创建打击编队
    for (int i = 0; i < strikeTeamCount; ++i) {
        GroupInfo strikeGroup;
        strikeGroup.groupName = QString("打击编队%1").arg(i + 1);
        
        EquipmentType strikeEquipment;
        strikeEquipment.equipmentName = "ARM-A 型反辐射无人机";
        
        int uavStartIdx = i * 2;
        int uavEndIdx = qMin(uavStartIdx + 2, strikeUAVs.size());
        
        for (int j = uavStartIdx; j < uavEndIdx; ++j) {
            UAVInfo uav;
            uav.uavName = strikeUAVs[j];
            uav.status = "待命";
            strikeEquipment.uavList.append(uav);
        }
        
        if (!strikeEquipment.uavList.isEmpty()) {
            strikeGroup.equipmentList.append(strikeEquipment);
        }
        
        if (!strikeGroup.equipmentList.isEmpty()) {
            groupList.append(strikeGroup);
        }
    }
    
    // 动态创建压制编队
    for (int i = 0; i < suppressTeamCount; ++i) {
        GroupInfo suppressGroup;
        suppressGroup.groupName = QString("压制编队%1").arg(i + 1);
        
        EquipmentType suppressEquipment;
        suppressEquipment.equipmentName = "ARM-C 型反辐射无人机";
        
        if (i < suppressUAVs.size()) {
            UAVInfo uav;
            uav.uavName = suppressUAVs[i];
            uav.status = "待命";
            suppressEquipment.uavList.append(uav);
        }
        
        if (!suppressEquipment.uavList.isEmpty()) {
            suppressGroup.equipmentList.append(suppressEquipment);
        }
        
        if (!suppressGroup.equipmentList.isEmpty()) {
            groupList.append(suppressGroup);
        }
    }
    
    // 如果没有任务，也创建默认的打击编队和压制编队
    if (groupList.isEmpty()) {
        GroupInfo defaultStrikeGroup;
        defaultStrikeGroup.groupName = "打击编队1";
        EquipmentType defaultEquipment;
        defaultEquipment.equipmentName = "ARM-A 型反辐射无人机";
        defaultStrikeGroup.equipmentList.append(defaultEquipment);
        groupList.append(defaultStrikeGroup);
        
        GroupInfo defaultSuppressGroup;
        defaultSuppressGroup.groupName = "压制编队1";
        EquipmentType defaultSuppressEquipment;
        defaultSuppressEquipment.equipmentName = "ARM-C 型反辐射无人机";
        defaultSuppressGroup.equipmentList.append(defaultSuppressEquipment);
        groupList.append(defaultSuppressGroup);
    }
    
    // 更新兵力计算表格显示
    DisplayForceCalculationTableToData(forceList);
    
    // 更新兵力资源管理树
    DisplayForceTreeToData(groupList);
    
    // 显示计算结果
    showToast(QString("兵力需求计算完成，合计 %1 架（%2个打击编队、%3个压制编队）")
              .arg(totalRequired).arg(strikeTeamCount).arg(suppressTeamCount));
}

void CooperativeMissionPlan::onAutoAllocate()
{
    allocationList.clear();
    cruisePathList.clear();
    searchPathList.clear();

    if (taskList.isEmpty()) {
        showToast("请先添加协同任务");
        return;
    }

    if (groupList.isEmpty()) {
        showToast("请先进行兵力计算");
        return;
    }

    QList<QString> usedUAVs;

    for (const auto& task : taskList) {
        TaskAllocation allocation;
        allocation.taskName = task.taskName;
        allocation.targetType = task.targetType;
        allocation.taskTarget = task.taskTarget;

        QString taskType = task.taskType;
        QString requiredPrefix;

        if (taskType == "打击") {
            allocation.threatLevel = "高";
            requiredPrefix = "打击编队";
        } else if (taskType == "压制") {
            allocation.threatLevel = "中";
            requiredPrefix = "压制编队";
        } else {
            allocation.threatLevel = "低";
            requiredPrefix = "打击编队";
        }

        QStringList allocatedUAVNames;
        QString formationName;

        for (const auto& group : groupList) {
            if (group.groupName.startsWith(requiredPrefix)) {
                bool hasAvailable = false;
                for (const auto& equip : group.equipmentList) {
                    for (const auto& uav : equip.uavList) {
                        if (uav.status != "已分配" && !usedUAVs.contains(uav.uavName)) {
                            hasAvailable = true;
                            break;
                        }
                    }
                    if (hasAvailable) break;
                }

                if (hasAvailable) {
                    formationName = group.groupName;

                    int uavNeeded = (taskType == "打击") ? 2 : 1;

                    for (const auto& equip : group.equipmentList) {
                        for (const auto& uav : equip.uavList) {
                            if (uavNeeded <= 0) break;
                            if (uav.status != "已分配" && !usedUAVs.contains(uav.uavName)) {
                                allocatedUAVNames.append(uav.uavName);
                                usedUAVs.append(uav.uavName);
                                uavNeeded--;
                            }
                        }
                        if (uavNeeded <= 0) break;
                    }
                    break;
                }
            }
        }

        allocation.allocatedUAVs = allocatedUAVNames.join(", ");
        allocation.formation = formationName;

        allocationList.append(allocation);

        for (const QString& uavName : allocatedUAVNames) {
            PathPlanning cruisePath;
            cruisePath.uavName = uavName;
            cruisePath.relatedTask = task.taskName;
            cruisePath.pathPointCount = 5;
            cruisePath.status = "待生成";
            for (int i = 0; i < 5; ++i) {
                PathPoint point;
                point.pointOrder = i + 1;
                point.latitude = 39.9 + (qrand() % 100) / 1000.0;
                point.longitude = 116.4 + (qrand() % 100) / 1000.0;
                point.altitude = 500 + (qrand() % 200);
                cruisePath.pathPoints.append(point);
            }
            cruisePathList.append(cruisePath);

            PathPlanning searchPath;
            searchPath.uavName = uavName;
            searchPath.relatedTask = task.taskName;
            searchPath.pathPointCount = 3;
            searchPath.status = "待生成";
            for (int i = 0; i < 3; ++i) {
                PathPoint point;
                point.pointOrder = i + 1;
                point.latitude = 39.9 + (qrand() % 100) / 1000.0;
                point.longitude = 116.4 + (qrand() % 100) / 1000.0;
                point.altitude = 300 + (qrand() % 150);
                searchPath.pathPoints.append(point);
            }
            searchPathList.append(searchPath);
        }
    }

    DisplayAllocationTableToData(allocationList);
    DisplayPathTableToData(cruisePathList);

    for (const QString& uavName : usedUAVs) {
        for (auto& group : groupList) {
            for (auto& equip : group.equipmentList) {
                for (auto& uav : equip.uavList) {
                    if (uav.uavName == uavName) {
                        uav.status = "已分配";
                    }
                }
            }
        }
    }

    DisplayForceTreeToData(groupList);

    showToast(QString("自动分配完成，已分配 %1 架无人机").arg(usedUAVs.size()));
}

void CooperativeMissionPlan::onGeneratePath()
{
    int generatedCount = 0;

    for (auto& path : cruisePathList) {
        if (path.status == "待生成") {
            path.status = "已生成";
            generatedCount++;
        }
    }

    for (auto& path : searchPathList) {
        if (path.status == "待生成") {
            path.status = "已生成";
            generatedCount++;
        }
    }

    DisplayPathTableToData(cruisePathList);

    showToast(QString("路径生成完成，已生成 %1 条路径").arg(generatedCount));
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

void CooperativeMissionPlan::onShutdown()
{
    // 关闭应用程序
    QApplication::quit();
}