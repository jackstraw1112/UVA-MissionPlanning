#include "CooperativeMissionPlan.h"
#include "ui_CooperativeMissionPlan.h"
#include <QDateTime>
#include <QTimer>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QTreeWidgetItem>
#include <QHeaderView>

CooperativeMissionPlan::CooperativeMissionPlan(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CooperativeMissionPlan)
    , selectedTaskIndex(0)
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
    connect(ui->addTaskButton, &QPushButton::clicked, this, &CooperativeMissionPlan::onNewTask);
    connect(ui->editTaskButton, &QPushButton::clicked, this, &CooperativeMissionPlan::onEditTask);
    connect(ui->deleteTaskButton, &QPushButton::clicked, this, &CooperativeMissionPlan::onDeleteTask);
    connect(ui->calculateForceButton, &QPushButton::clicked, this, &CooperativeMissionPlan::onCalculateForce);
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
    ui->taskTable->setStyleSheet(
        "QTableWidget { background: rgba(8,24,48,0.92); border: none; }"
        "QTableWidget::item { color: #c8e8ff;background: rgba(255,255,255,0.02); border-bottom: 1px solid rgba(0,140,220,0.12); }"
        "QTableWidget::item:hover { background: rgba(0,180,255,0.08); }"
        "QTableWidget::item:selected { background: rgba(0,180,255,0.18); color: #e8f4ff; }"
        "QHeaderView::section { background: rgba(0,120,200,0.25); border-bottom: 1px solid rgba(0,160,255,0.35); border-right: 1px solid rgba(0,140,220,0.12); padding: 5px 8px; font-size: 11px; font-weight: 500; color: rgba(120,180,230,0.6); text-align: left; letter-spacing: 0.3px; white-space: nowrap; overflow: hidden; text-overflow: ellipsis; }"
        "QHeaderView::section:horizontal:last { border-right: none; }"
    );
   
    
    QStringList forceCalcHeaders = {"任务名称", "任务目标", "威胁等级", "优先级", "计算数量", "调整数量"};
    ui->forceCalculationTable->setColumnCount(6); // Ensure we have 6 columns
    ui->forceCalculationTable->setHorizontalHeaderLabels(forceCalcHeaders);
    ui->forceCalculationTable->horizontalHeader()->setStretchLastSection(true);
    ui->forceCalculationTable->verticalHeader()->setVisible(false);
    ui->forceCalculationTable->horizontalHeader()->setVisible(true); // Ensure headers are visible
    ui->forceCalculationTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft); // Align header text to left
    ui->forceCalculationTable->horizontalHeader()->setMinimumHeight(24); // Ensure header has sufficient height
    ui->forceCalculationTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->forceCalculationTable->setAlternatingRowColors(false);
    ui->forceCalculationTable->setStyleSheet(
        "QTableWidget { background: rgba(8,24,48,0.92); border: none; }" 
        "QTableWidget::item { color: #c8e8ff;background: rgba(255,255,255,0.02); border-bottom: 1px solid rgba(0,140,220,0.12); }" 
        "QTableWidget::item:hover { background: rgba(0,180,255,0.08); }" 
        "QTableWidget::item:selected { background: rgba(0,180,255,0.18); color: #e8f4ff; }" 
        "QHeaderView::section { background: rgba(0,120,200,0.25); border-bottom: 1px solid rgba(0,160,255,0.35); border-right: 1px solid rgba(0,140,220,0.12); padding: 5px 8px; font-size: 11px; font-weight: 500; color: rgba(120,180,230,0.6); text-align: left; letter-spacing: 0.3px; white-space: nowrap; overflow: hidden; text-overflow: ellipsis; }" 
        "QHeaderView::section:horizontal:last { border-right: none; }"
    );


    QStringList allocationHeaders = {"任务名称", "目标类型", "任务目标", "威胁等级", "分配无人机", "编队"};
    ui->allocationTable->setColumnCount(6); // Ensure we have 6 columns
    ui->allocationTable->setHorizontalHeaderLabels(allocationHeaders);
    ui->allocationTable->horizontalHeader()->setStretchLastSection(true);
    ui->allocationTable->verticalHeader()->setVisible(false);
    ui->allocationTable->horizontalHeader()->setVisible(true); // Ensure headers are visible
    ui->allocationTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft); // Align header text to left
    ui->allocationTable->horizontalHeader()->setMinimumHeight(24); // Ensure header has sufficient height
    ui->allocationTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->allocationTable->setAlternatingRowColors(false);
    ui->allocationTable->setStyleSheet(
        "QTableWidget { background: rgba(8,24,48,0.92); border: none; }" 
        "QTableWidget::item { color: #c8e8ff;background: rgba(255,255,255,0.02); border-bottom: 1px solid rgba(0,140,220,0.12); }" 
        "QTableWidget::item:hover { background: rgba(0,180,255,0.08); }" 
        "QTableWidget::item:selected { background: rgba(0,180,255,0.18); color: #e8f4ff; }" 
        "QHeaderView::section { background: rgba(0,120,200,0.25); border-bottom: 1px solid rgba(0,160,255,0.35); border-right: 1px solid rgba(0,140,220,0.12); padding: 5px 8px; font-size: 11px; font-weight: 500; color: rgba(120,180,230,0.6); text-align: left; letter-spacing: 0.3px; white-space: nowrap; overflow: hidden; text-overflow: ellipsis; }" 
        "QHeaderView::section:horizontal:last { border-right: none; }"
    );

    QStringList pathHeaders = {"无人机名称", "关联任务", "路径点数", "状态"};
    ui->pathTable->setColumnCount(4); // Ensure we have 4 columns
    ui->pathTable->setHorizontalHeaderLabels(pathHeaders); // Fix: use correct headers
    ui->pathTable->horizontalHeader()->setStretchLastSection(true);
    ui->pathTable->verticalHeader()->setVisible(false);
    ui->pathTable->horizontalHeader()->setVisible(true); // Ensure headers are visible
    ui->pathTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft); // Align header text to left
    ui->pathTable->horizontalHeader()->setMinimumHeight(24); // Ensure header has sufficient height
    ui->pathTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->pathTable->setAlternatingRowColors(false);
    ui->pathTable->setStyleSheet(
        "QTableWidget { background: rgba(8,24,48,0.92); border: none; }" 
         "QTableWidget::item { color: #c8e8ff;background: rgba(255,255,255,0.02); border-bottom: 1px solid rgba(0,140,220,0.12); }" 
        "QTableWidget::item:hover { background: rgba(0,180,255,0.08); }" 
        "QTableWidget::item:selected { background: rgba(0,180,255,0.18); color: #e8f4ff; }" 
        "QHeaderView::section { background: rgba(0,120,200,0.25); border-bottom: 1px solid rgba(0,160,255,0.35); border-right: 1px solid rgba(0,140,220,0.12); padding: 5px 8px; font-size: 11px; font-weight: 500; color: rgba(120,180,230,0.6); text-align: left; letter-spacing: 0.3px; white-space: nowrap; overflow: hidden; text-overflow: ellipsis; }" 
        "QHeaderView::section:horizontal:last { border-right: none; }"
    );

    QStringList targetHeaders = {"目标编号", "频率范围(MHz)", "脉宽范围(μs)", "重复周期(ms)", "工作模式"};
    ui->targetTable->setColumnCount(5); // Ensure we have 5 columns
    ui->targetTable->setHorizontalHeaderLabels(targetHeaders); // Fix: use correct headers
    ui->targetTable->horizontalHeader()->setStretchLastSection(true);
    ui->targetTable->verticalHeader()->setVisible(false);
    ui->targetTable->horizontalHeader()->setVisible(true); // Ensure headers are visible
    ui->targetTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft); // Align header text to left
    ui->targetTable->horizontalHeader()->setMinimumHeight(24); // Ensure header has sufficient height
    ui->targetTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->targetTable->setAlternatingRowColors(false);
    ui->targetTable->setStyleSheet(
        "QTableWidget { background: rgba(8,24,48,0.92); border: none; }" 
        "QTableWidget::item { color: #c8e8ff;background: rgba(255,255,255,0.02); border-bottom: 1px solid rgba(0,140,220,0.12); }" 
        "QTableWidget::item:hover { background: rgba(0,180,255,0.08); }" 
        "QTableWidget::item:selected { background: rgba(0,180,255,0.18); color: #e8f4ff; }" 
        "QHeaderView::section { background: rgba(0,120,200,0.25); border-bottom: 1px solid rgba(0,160,255,0.35); border-right: 1px solid rgba(0,140,220,0.12); padding: 5px 8px; font-size: 11px; font-weight: 500; color: rgba(120,180,230,0.6); text-align: left; letter-spacing: 0.3px; white-space: nowrap; overflow: hidden; text-overflow: ellipsis; }" 
        "QHeaderView::section:horizontal:last { border-right: none; }"
    );
}

void CooperativeMissionPlan::initializeTables()
{
    // Initialize task table with sample data
    QStringList taskData = {
        "打击雷达阵地α|打击|点目标|E-001|08:30~09:15|ARM-A1, ARM-A2",
        "压制通信干扰β|压制|点目标|E-002|08:45~09:30|ARM-B1",
        "摧毁预警阵地γ|打击|点目标|E-003|09:00~09:45|ARM-C1, ARM-C2",
        "压制北部干扰区|压制|区域目标|ZONE-A|09:00~10:30|ARM-D1, ARM-D2"
    };
    ui->taskTable->setRowCount(taskData.size());
    for (int row = 0; row < taskData.size(); ++row) {
        QStringList columns = taskData[row].split('|');
        for (int col = 0; col < columns.size(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(columns[col]);
            ui->taskTable->setItem(row, col, item);
        }
    }
    
    // Initialize force calculation table
    QStringList forceCalcData = {
        "打击雷达阵地α|E-001|高|P1|2|2",
        "压制通信干扰β|E-002|中|P2|1|1",
        "摧毁预警阵地γ|E-003|高|P1|2|2",
        "压制北部干扰区|ZONE-A|中|P2|2|2"
    };
    ui->forceCalculationTable->setRowCount(forceCalcData.size());
    for (int row = 0; row < forceCalcData.size(); ++row) {
        QStringList columns = forceCalcData[row].split('|');
        for (int col = 0; col < columns.size(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(columns[col]);
            ui->forceCalculationTable->setItem(row, col, item);
        }
    }
    
    // Initialize resource table
    QStringList resourceData = {
        "A-编组|ARM-A|3|2 就绪, 1 待命",
        "B-编组|ARM-B|3|2 就绪, 1 故障",
        "C-编组|ARM-C|2|2 就绪",
        "D-编组|ARM-D|2|2 就绪"
    };
    
    
    // Initialize allocation table
    QStringList allocationData = {
        "打击雷达阵地α|点目标|E-001|高|ARM-A1, ARM-A2|A-编组",
        "压制通信干扰β|点目标|E-002|中|ARM-B1|B-编组",
        "摧毁预警阵地γ|点目标|E-003|高|ARM-C1, ARM-C2|C-编组",
        "压制北部干扰区|区域目标|ZONE-A|中|ARM-D1, ARM-D2|D-编组"
    };
    ui->allocationTable->setRowCount(allocationData.size());
    for (int row = 0; row < allocationData.size(); ++row) {
        QStringList columns = allocationData[row].split('|');
        for (int col = 0; col < columns.size(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(columns[col]);
            ui->allocationTable->setItem(row, col, item);
        }
    }
}

void CooperativeMissionPlan::initializeTree()
{
    // 清空现有内容
    ui->forceTree->clear();
    
    // 设置列宽
    ui->forceTree->setColumnWidth(0, 280);
    ui->forceTree->setColumnWidth(1, 80);
    
    // 创建第1编组 — 打击队
    QTreeWidgetItem* group1 = new QTreeWidgetItem(ui->forceTree);
    group1->setText(0, "📁 第1编组 — 打击队");
    group1->setText(1, "4/6");
    group1->setExpanded(true);
    
    // ARM-A 型反辐射无人机
    QTreeWidgetItem* armAGroup = new QTreeWidgetItem(group1);
    armAGroup->setText(0, "📂 ARM-A 型反辐射无人机");
    armAGroup->setText(1, "2/3");
    armAGroup->setExpanded(true);
    
    // ARM-A1
    QTreeWidgetItem* armA1 = new QTreeWidgetItem(armAGroup);
    armA1->setText(0, "● ARM-A1");
    armA1->setText(1, "就绪");
    
    // ARM-A2
    QTreeWidgetItem* armA2 = new QTreeWidgetItem(armAGroup);
    armA2->setText(0, "● ARM-A2");
    armA2->setText(1, "就绪");
    
    // ARM-A3
    QTreeWidgetItem* armA3 = new QTreeWidgetItem(armAGroup);
    armA3->setText(0, "● ARM-A3");
    armA3->setText(1, "待命");
    
    // ARM-B 型反辐射无人机
    QTreeWidgetItem* armBGroup = new QTreeWidgetItem(group1);
    armBGroup->setText(0, "📂 ARM-B 型反辐射无人机");
    armBGroup->setText(1, "2/3");
    armBGroup->setExpanded(true);
    
    // ARM-B1
    QTreeWidgetItem* armB1 = new QTreeWidgetItem(armBGroup);
    armB1->setText(0, "● ARM-B1");
    armB1->setText(1, "就绪");
    
    // ARM-B2
    QTreeWidgetItem* armB2 = new QTreeWidgetItem(armBGroup);
    armB2->setText(0, "● ARM-B2");
    armB2->setText(1, "就绪");
    
    // ARM-B3
    QTreeWidgetItem* armB3 = new QTreeWidgetItem(armBGroup);
    armB3->setText(0, "● ARM-B3");
    armB3->setText(1, "故障");
    
    // 创建第2编组 — 压制队
    QTreeWidgetItem* group2 = new QTreeWidgetItem(ui->forceTree);
    group2->setText(0, "📁 第2编组 — 压制队");
    group2->setText(1, "3/6");
    group2->setExpanded(true);
    
    // ARM-C 型反辐射无人机
    QTreeWidgetItem* armCGroup = new QTreeWidgetItem(group2);
    armCGroup->setText(0, "📂 ARM-C 型反辐射无人机");
    armCGroup->setText(1, "2/3");
    armCGroup->setExpanded(true);
    
    // ARM-C1
    QTreeWidgetItem* armC1 = new QTreeWidgetItem(armCGroup);
    armC1->setText(0, "● ARM-C1");
    armC1->setText(1, "就绪");
    
    // ARM-C2
    QTreeWidgetItem* armC2 = new QTreeWidgetItem(armCGroup);
    armC2->setText(0, "● ARM-C2");
    armC2->setText(1, "就绪");
    
    // ARM-D 型反辐射无人机
    QTreeWidgetItem* armDGroup = new QTreeWidgetItem(group2);
    armDGroup->setText(0, "📂 ARM-D 型反辐射无人机");
    armDGroup->setText(1, "1/3");
    armDGroup->setExpanded(true);
    
    // ARM-D1
    QTreeWidgetItem* armD1 = new QTreeWidgetItem(armDGroup);
    armD1->setText(0, "● ARM-D1");
    armD1->setText(1, "就绪");
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
    showToast("分配已保存");
}

void CooperativeMissionPlan::showToast(const QString& message)
{
    QMessageBox::information(this, "提示", message);
}

void CooperativeMissionPlan::onNewTask()
{
    showToast("新建任务");
}

void CooperativeMissionPlan::onOpenTask()
{
    showToast("已打开任务文件");
}

void CooperativeMissionPlan::onSaveTask()
{
    showToast("任务已保存");
}

void CooperativeMissionPlan::onSaveAs()
{
    showToast("已另存为新文件");
}

void CooperativeMissionPlan::onExport()
{
    showToast("正在导出...");
}

void CooperativeMissionPlan::onEditTask()
{
    showToast("编辑任务");
}

void CooperativeMissionPlan::onDeleteTask()
{
    showToast("任务已删除");
}

void CooperativeMissionPlan::onCalculateForce()
{
    showToast("兵力需求计算完成，合计 7 架");
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