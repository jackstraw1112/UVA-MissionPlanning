#include "SituationAnalysis.h"
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QFrame>
#include <QPainter>
#include <QMouseEvent>
#include <QToolTip>
#include <QDebug>
#include <QLinearGradient>
#include <QFontMetrics>
#include <QGraphicsDropShadowEffect>
#include <QScrollBar>

// 频谱分析图实现 - 基于QGraphicsView
SpectrumChart::SpectrumChart(QWidget *parent)
    : QGraphicsView(parent)
    , scene(new QGraphicsScene(this))
    , hoveredIndex(-1)
    , showTooltip(false)
    , tooltipItem(nullptr)
{
    setScene(scene);
    setRenderHint(QPainter::Antialiasing);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setFrameStyle(QFrame::NoFrame);
    
    // 初始化频谱数据（与HTML页面相同）
    spectrumData = {
        {"HF", "3-30MHz", QColor("#8B5CF6"), 25, 2, QRectF(), nullptr, nullptr},
        {"VHF", "30-300MHz", QColor("#EC4899"), 45, 5, QRectF(), nullptr, nullptr},
        {"UHF", "300-1000MHz", QColor("#F59E0B"), 60, 8, QRectF(), nullptr, nullptr},
        {"L", "1-2GHz", QColor("#10B981"), 40, 4, QRectF(), nullptr, nullptr},
        {"S", "2-4GHz", QColor("#3B82F6"), 75, 9, QRectF(), nullptr, nullptr},
        {"C", "4-8GHz", QColor("#6366F1"), 55, 6, QRectF(), nullptr, nullptr},
        {"X", "8-12GHz", QColor("#EF4444"), 85, 11, QRectF(), nullptr, nullptr},
        {"Ku", "12-18GHz", QColor("#14B8A6"), 50, 5, QRectF(), nullptr, nullptr},
        {"K", "18-27GHz", QColor("#F97316"), 30, 3, QRectF(), nullptr, nullptr},
        {"Ka", "27-40GHz", QColor("#8B5CF6"), 20, 2, QRectF(), nullptr, nullptr}
    };
    
    setMinimumHeight(220);
    setMouseTracking(true);
    
    // 创建频谱条
    createSpectrumBars();
}

void SpectrumChart::createSpectrumBars()
{
    scene->clear();
    
    // 设置场景大小（与HTML一致）
    int sceneWidth = 800;
    int sceneHeight = 220 - 40; // 220px高度减去padding
    scene->setSceneRect(0, 0, sceneWidth, sceneHeight);
    
    // 绘制背景渐变 - 与HTML相同 linear-gradient(0deg, rgba(20, 40, 60, 0.5) 0%, transparent 100%)
    QGraphicsRectItem* bgRect = scene->addRect(0, 0, sceneWidth, sceneHeight);
    QLinearGradient bgGradient(0, sceneHeight, 0, 0);
    bgGradient.setColorAt(0, QColor(20, 40, 60, 128));
    bgGradient.setColorAt(1, QColor(0, 0, 0, 0));
    bgRect->setBrush(bgGradient);
    bgRect->setPen(Qt::NoPen);
    
    // 绘制频谱条
    int barCount = spectrumData.size();
    float barWidth = (sceneWidth - 100) / barCount;
    float chartHeight = sceneHeight - 40;
    
    for (int i = 0; i < barCount; ++i) {
        SpectrumBand &band = spectrumData[i];
        
        float x = 50 + i * barWidth;
        float barHeight = chartHeight * band.height / 100.0;
        float y = sceneHeight - 30 - barHeight;
        
        band.rect = QRectF(x, y, barWidth - 10, barHeight);
        
        // 创建频谱条
        band.rectItem = scene->addRect(band.rect, QPen(band.color.darker(200), 1));
        
        // 设置渐变填充
        QLinearGradient barGradient(band.rect.topLeft(), band.rect.bottomLeft());
        barGradient.setColorAt(0, band.color.lighter(150));
        barGradient.setColorAt(1, band.color.darker(150));
        band.rectItem->setBrush(barGradient);
        
        // 添加阴影效果
        QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect;
        shadow->setBlurRadius(10);
        shadow->setColor(band.color);
        shadow->setOffset(0, 0);
        band.rectItem->setGraphicsEffect(shadow);
        
        // 添加频率标签
        band.labelItem = scene->addText(band.name, QFont("Microsoft YaHei", 9));
        band.labelItem->setDefaultTextColor(QColor("#8E9CB9"));
        band.labelItem->setPos(x + (barWidth - 10) / 2 - band.labelItem->boundingRect().width() / 2, 
                              sceneHeight - 25);
        
        // 设置可悬停
        band.rectItem->setAcceptHoverEvents(true);
        band.rectItem->setData(0, i); // 存储索引
    }
    
    // 创建工具提示项
    tooltipItem = scene->addText("", QFont("Microsoft YaHei", 10));
    tooltipItem->setDefaultTextColor(QColor("#5BC0FF"));
    tooltipItem->setZValue(100);
    tooltipItem->hide();
    
    QGraphicsRectItem* tooltipBg = scene->addRect(QRectF(), QPen(QColor(91, 192, 255, 128), 1), 
                                                 QBrush(QColor(10, 20, 40, 242)));
    tooltipBg->setZValue(99);
    tooltipBg->hide();
    tooltipBg->setData(0, -1); // 标记为工具提示背景
}

void SpectrumChart::updateSpectrumData()
{
    createSpectrumBars();
}

void SpectrumChart::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    fitInView(scene->sceneRect(), Qt::KeepAspectRatioByExpanding);
}

void SpectrumChart::mouseMoveEvent(QMouseEvent *event)
{
    QPointF scenePos = mapToScene(event->pos());
    QGraphicsItem* item = scene->itemAt(scenePos, QTransform());
    
    if (item && item->data(0).toInt() >= 0) {
        int index = item->data(0).toInt();
        if (index < spectrumData.size()) {
            hoveredIndex = index;
            const SpectrumBand &band = spectrumData[index];
            
            // 更新工具提示
            tooltipText = QString("频段: %1\n范围: %2\n信号数: %3")
                         .arg(band.name)
                         .arg(band.range)
                         .arg(band.signalCount);
            
            updateTooltip();
            return;
        }
    }
    
    hoveredIndex = -1;
    showTooltip = false;
    if (tooltipItem) tooltipItem->hide();
    
    QGraphicsView::mouseMoveEvent(event);
}

void SpectrumChart::updateTooltip()
{
    if (hoveredIndex >= 0 && hoveredIndex < spectrumData.size()) {
        const SpectrumBand &band = spectrumData[hoveredIndex];
        
        tooltipItem->setPlainText(tooltipText);
        
        // 计算工具提示位置
        QRectF tooltipRect = tooltipItem->boundingRect();
        QPointF tooltipPos = band.rect.topRight() + QPointF(10, 0);
        
        // 确保工具提示在场景内
        if (tooltipPos.x() + tooltipRect.width() > scene->width()) {
            tooltipPos.setX(scene->width() - tooltipRect.width() - 10);
        }
        if (tooltipPos.y() + tooltipRect.height() > scene->height()) {
            tooltipPos.setY(scene->height() - tooltipRect.height() - 10);
        }
        
        tooltipItem->setPos(tooltipPos);
        tooltipItem->show();
        showTooltip = true;
    }
}

void SpectrumChart::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    hoveredIndex = -1;
    showTooltip = false;
    if (tooltipItem) tooltipItem->hide();
    
    QGraphicsView::leaveEvent(event);
}

SituationAnalysis::SituationAnalysis(QWidget *parent)
    : QMainWindow(parent)
    , headerWidget(nullptr)
    , titleLabel(nullptr)
    , totalTargetsLabel(nullptr)
    , highThreatLabel(nullptr)
    , firepowerCountLabel(nullptr)
    , currentTimeLabel(nullptr)
    , leftPanel(nullptr)
    , radarSourceGroup(nullptr)
    , radarSourceTabs(nullptr)
    , radarSourceTable(nullptr)
    , commSourceTable(nullptr)
    , commJammerTable(nullptr)
    , radarJammerTable(nullptr)
    , scrollArea(nullptr)
    , scrollWidget(nullptr)
    , spectrumWidget(nullptr)
    , threatWidget(nullptr)
    , firepowerWidget(nullptr)
    , situationControlGroup(nullptr)
    , spectrumGroup(nullptr)
    , threatGroup(nullptr)
    , firepowerGroup(nullptr)
    , showRadarCheck(nullptr)
    , showCommCheck(nullptr)
    , showCommJammerCheck(nullptr)
    , showRadarJammerCheck(nullptr)
    , showFirepowerCheck(nullptr)
    , systemTimer(nullptr)
{
    // 设置UI
    setupUI();
    
    // 初始化系统定时器
    systemTimer = new QTimer(this);
    connect(systemTimer, &QTimer::timeout, this, &SituationAnalysis::updateSystemTime);
    systemTimer->start(1000); // 每秒更新一次
    
    // 初始化数据
    createRadarSourceData();
    createThreatData();
    createFirepowerData();
    
    // 连接信号槽（确保UI组件已初始化）
    if (radarSourceTabs) {
        connect(radarSourceTabs, &QTabWidget::currentChanged, this, &SituationAnalysis::onRadarTabChanged);
    }
    if (showRadarCheck) {
        connect(showRadarCheck, &QCheckBox::toggled, this, &SituationAnalysis::onToggleRadar);
    }
    if (showCommCheck) {
        connect(showCommCheck, &QCheckBox::toggled, this, &SituationAnalysis::onToggleComm);
    }
    if (showCommJammerCheck) {
        connect(showCommJammerCheck, &QCheckBox::toggled, this, &SituationAnalysis::onToggleCommJammer);
    }
    if (showRadarJammerCheck) {
        connect(showRadarJammerCheck, &QCheckBox::toggled, this, &SituationAnalysis::onToggleRadarJammer);
    }
    if (showFirepowerCheck) {
        connect(showFirepowerCheck, &QCheckBox::toggled, this, &SituationAnalysis::onToggleFirepower);
    }
    
    updateSystemTime();
}

SituationAnalysis::~SituationAnalysis()
{
}

void SituationAnalysis::setupUI()
{
    // 设置窗口标题和大小
    setWindowTitle("反辐射无人机仿真系统 - 态势分析");
    resize(1600, 900);
    
    // 设置深蓝色背景
    setStyleSheet(
        "QMainWindow {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #0a0f1e, stop:1 #03060c);"
        "    color: #eef2ff;"
        "    font-family: 'Inter', 'Microsoft YaHei', sans-serif;"
        "}"
        "QWidget {"
        "    background: rgba(10, 15, 30, 200);"
        "    color: #eef2ff;"
        "}"
    );
    
    // 创建滚动区域
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setFrameStyle(QFrame::NoFrame);
    
    // 创建滚动内容窗口
    scrollWidget = new QWidget();
    scrollArea->setWidget(scrollWidget);
    
    // 设置中央窗口部件为滚动区域
    setCentralWidget(scrollArea);
    
    // 创建主垂直布局
    QVBoxLayout *mainLayout = new QVBoxLayout(scrollWidget);
    mainLayout->setSpacing(16);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    
    // 设置头部
    setupHeader();
    mainLayout->addWidget(headerWidget);
    
    // 创建左右两列布局
    QHBoxLayout *contentLayout = new QHBoxLayout();
    contentLayout->setSpacing(16);
    
    // 左侧面板
    QWidget *leftPanelWidget = new QWidget();
    leftPanelWidget->setStyleSheet("QWidget { background: transparent; }");
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanelWidget);
    leftLayout->setSpacing(16);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    
    // 设置辐射源列表
    setupRadarSourceList();
    leftLayout->addWidget(radarSourceGroup, 1);
    
    // 设置态势控制模块
    setupSituationControl();
    leftLayout->addWidget(situationControlGroup, 0);
    
    // 右侧面板
    QWidget *rightPanelWidget = new QWidget();
    rightPanelWidget->setStyleSheet("QWidget { background: transparent; }");
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanelWidget);
    rightLayout->setSpacing(16);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    
    // 设置频谱分析图
    setupSpectrumAnalysis();
    rightLayout->addWidget(spectrumGroup, 0);
    
    // 设置火力控制模块
    setupFirepowerControl();
    rightLayout->addWidget(firepowerGroup, 0);
    
    // 设置威胁评估模块
    setupThreatAssessment();
    rightLayout->addWidget(threatGroup, 1);
    
    // 将左右面板添加到内容布局
    contentLayout->addWidget(leftPanelWidget, 1);
    contentLayout->addWidget(rightPanelWidget, 1);
    
    mainLayout->addLayout(contentLayout, 1);
    
    // 设置滚动区域的最小高度
    scrollWidget->setMinimumHeight(1200);
}

void SituationAnalysis::setupHeader()
{
    headerWidget = new QWidget();
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(16, 16, 16, 16);
    
    // 标题
    titleLabel = new QLabel("反辐射无人机仿真系统");
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #eef2ff;");
    
    // 统计信息
    QWidget *statsWidget = new QWidget();
    QHBoxLayout *statsLayout = new QHBoxLayout(statsWidget);
    statsLayout->setSpacing(20);
    
    // 总目标
    QWidget *totalTargetsWidget = new QWidget();
    QVBoxLayout *totalTargetsLayout = new QVBoxLayout(totalTargetsWidget);
    totalTargetsLabel = new QLabel("12");
    totalTargetsLabel->setAlignment(Qt::AlignCenter);
    totalTargetsLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #5BC0FF;");
    QLabel *totalTargetsDesc = new QLabel("总目标");
    totalTargetsDesc->setAlignment(Qt::AlignCenter);
    totalTargetsDesc->setStyleSheet("font-size: 10px; color: #8E9CB9;");
    totalTargetsLayout->addWidget(totalTargetsLabel);
    totalTargetsLayout->addWidget(totalTargetsDesc);
    
    // 高威胁
    QWidget *highThreatWidget = new QWidget();
    QVBoxLayout *highThreatLayout = new QVBoxLayout(highThreatWidget);
    highThreatLabel = new QLabel("4");
    highThreatLabel->setAlignment(Qt::AlignCenter);
    highThreatLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #5BC0FF;");
    QLabel *highThreatDesc = new QLabel("高威胁");
    highThreatDesc->setAlignment(Qt::AlignCenter);
    highThreatDesc->setStyleSheet("font-size: 10px; color: #8E9CB9;");
    highThreatLayout->addWidget(highThreatLabel);
    highThreatLayout->addWidget(highThreatDesc);
    
    // 防控火力
    QWidget *firepowerCountWidget = new QWidget();
    QVBoxLayout *firepowerCountLayout = new QVBoxLayout(firepowerCountWidget);
    firepowerCountLabel = new QLabel("5");
    firepowerCountLabel->setAlignment(Qt::AlignCenter);
    firepowerCountLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #5BC0FF;");
    QLabel *firepowerCountDesc = new QLabel("防控火力");
    firepowerCountDesc->setAlignment(Qt::AlignCenter);
    firepowerCountDesc->setStyleSheet("font-size: 10px; color: #8E9CB9;");
    firepowerCountLayout->addWidget(firepowerCountLabel);
    firepowerCountLayout->addWidget(firepowerCountDesc);
    
    // 系统时间
    QWidget *currentTimeWidget = new QWidget();
    QVBoxLayout *currentTimeLayout = new QVBoxLayout(currentTimeWidget);
    currentTimeLabel = new QLabel("--:--:--");
    currentTimeLabel->setAlignment(Qt::AlignCenter);
    currentTimeLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #5BC0FF;");
    QLabel *currentTimeDesc = new QLabel("系统时间");
    currentTimeDesc->setAlignment(Qt::AlignCenter);
    currentTimeDesc->setStyleSheet("font-size: 10px; color: #8E9CB9;");
    currentTimeLayout->addWidget(currentTimeLabel);
    currentTimeLayout->addWidget(currentTimeDesc);
    
    statsLayout->addWidget(totalTargetsWidget);
    statsLayout->addWidget(highThreatWidget);
    statsLayout->addWidget(firepowerCountWidget);
    statsLayout->addWidget(currentTimeWidget);
    
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(statsWidget);
    
    // 设置头部样式
    headerWidget->setStyleSheet(
        "QWidget {"
        "    background: rgba(12, 20, 35, 200);"
        "    border-radius: 16px;"
        "    border: 1px solid rgba(72, 187, 255, 80);"
        "}"
    );
}

void SituationAnalysis::setupRadarSourceList()
{
    // 创建辐射源列表的GroupBox
    radarSourceGroup = new QGroupBox();
    radarSourceGroup->setStyleSheet(
        "QGroupBox {"
        "    background: rgba(8, 16, 30, 190);"
        "    border: 1px solid rgba(72, 187, 255, 77);"
        "    border-radius: 16px;"
        "    margin-top: 10px;"
        "    padding-top: 0px;"
        "}"
        "QGroupBox::title {"
        "    subcontrol-origin: margin;"
        "    subcontrol-position: top left;"
        "    padding: 0px;"
        "    background: transparent;"
        "}"
    );
    
    QVBoxLayout *radarSourceGroupLayout = new QVBoxLayout(radarSourceGroup);
    radarSourceGroupLayout->setContentsMargins(0, 0, 0, 0);
    radarSourceGroupLayout->setSpacing(0);
    
    // 创建卡片头部（模拟HTML中的.card-header）
    QWidget *cardHeaderWidget = new QWidget();
    cardHeaderWidget->setStyleSheet(
        "QWidget {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 rgba(20, 40, 65, 230), stop:1 rgba(10, 22, 45, 180));"
        "    border-bottom: 1px solid rgba(72, 187, 255, 77);"
        "    border-top-left-radius: 16px;"
        "    border-top-right-radius: 16px;"
        "}"
    );
    
    QHBoxLayout *cardHeaderLayout = new QHBoxLayout(cardHeaderWidget);
    cardHeaderLayout->setContentsMargins(12, 16, 12, 16);
    cardHeaderLayout->setSpacing(10);
    
    QLabel *iconLabel = new QLabel("📡");
    iconLabel->setStyleSheet("font-size: 18px; color: #5BC0FF;");
    
    QLabel *titleLabel = new QLabel("辐射源列表");
    titleLabel->setStyleSheet(
        "QLabel {"
        "    color: #eef2ff;"
        "    font-weight: 600;"
        "    font-size: 16px;"
        "}"
    );
    
    QLabel *countLabel = new QLabel("12个目标");
    countLabel->setStyleSheet(
        "QLabel {"
        "    background: rgba(0, 0, 0, 77);"
        "    padding: 4px 10px;"
        "    border-radius: 20px;"
        "    font-size: 11px;"
        "    color: #5BC0FF;"
        "}"
    );
    
    cardHeaderLayout->addWidget(iconLabel);
    cardHeaderLayout->addWidget(titleLabel);
    cardHeaderLayout->addStretch();
    cardHeaderLayout->addWidget(countLabel);
    
    radarSourceGroupLayout->addWidget(cardHeaderWidget);
    
    // 创建辐射源列表表格
    radarSourceTable = new QTableWidget();
    radarSourceTable->setColumnCount(5);
    radarSourceTable->setHorizontalHeaderLabels(QStringList() << "名称" << "类型" << "频率" << "功率" << "威胁等级");
    radarSourceTable->horizontalHeader()->setStretchLastSection(true);
    
    commSourceTable = new QTableWidget();
    commSourceTable->setColumnCount(5);
    commSourceTable->setHorizontalHeaderLabels(QStringList() << "名称" << "类型" << "频率" << "功率" << "威胁等级");
    commSourceTable->horizontalHeader()->setStretchLastSection(true);
    
    commJammerTable = new QTableWidget();
    commJammerTable->setColumnCount(5);
    commJammerTable->setHorizontalHeaderLabels(QStringList() << "名称" << "类型" << "频率" << "功率" << "威胁等级");
    commJammerTable->horizontalHeader()->setStretchLastSection(true);
    
    radarJammerTable = new QTableWidget();
    radarJammerTable->setColumnCount(5);
    radarJammerTable->setHorizontalHeaderLabels(QStringList() << "名称" << "类型" << "频率" << "功率" << "威胁等级");
    radarJammerTable->horizontalHeader()->setStretchLastSection(true);
    
    // 创建标签页
    radarSourceTabs = new QTabWidget();
    radarSourceTabs->addTab(radarSourceTable, "雷达");
    radarSourceTabs->addTab(commSourceTable, "电台");
    radarSourceTabs->addTab(commJammerTable, "通信对抗");
    radarSourceTabs->addTab(radarJammerTable, "雷达对抗");
    
    // 设置卡片样式
    radarSourceTabs->setStyleSheet(
        "QTabWidget::pane {"
        "    background: transparent;"
        "    border: none;"
        "    border-radius: 0px 0px 16px 16px;"
        "}"
        "QTabBar::tab {"
        "    background: rgba(30, 50, 70, 128);"
        "    color: #bbd9ff;"
        "    padding: 8px 10px;"
        "    margin: 6px 3px;"
        "    border-radius: 8px;"
        "    font-size: 12px;"
        "    font-weight: 500;"
        "}"
        "QTabBar::tab:selected {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #2c577c, stop:1 #1a3a55);"
        "    color: #fff;"
        "    box-shadow: 0 2px 8px rgba(91, 192, 255, 77);"
        "}"
        "QTabBar::tab:hover {"
        "    background: rgba(50, 80, 110, 153);"
        "}"
    );
    
    radarSourceGroupLayout->addWidget(radarSourceTabs);
}

void SituationAnalysis::setupSpectrumAnalysis()
{
    // 创建频谱分析的GroupBox
    spectrumGroup = new QGroupBox();
    spectrumGroup->setStyleSheet(
        "QGroupBox {"
        "    background: rgba(8, 16, 30, 190);"
        "    border: 1px solid rgba(72, 187, 255, 77);"
        "    border-radius: 16px;"
        "    margin-top: 10px;"
        "    padding-top: 0px;"
        "}"
        "QGroupBox::title {"
        "    subcontrol-origin: margin;"
        "    subcontrol-position: top left;"
        "    padding: 0px;"
        "    background: transparent;"
        "}"
    );
    
    QVBoxLayout *spectrumLayout = new QVBoxLayout(spectrumGroup);
    spectrumLayout->setContentsMargins(0, 0, 0, 0);
    spectrumLayout->setSpacing(0);
    
    // 创建卡片头部（模拟HTML中的.card-header）
    QWidget *cardHeaderWidget = new QWidget();
    cardHeaderWidget->setStyleSheet(
        "QWidget {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 rgba(20, 40, 65, 230), stop:1 rgba(10, 22, 45, 180));"
        "    border-bottom: 1px solid rgba(72, 187, 255, 77);"
        "    border-top-left-radius: 16px;"
        "    border-top-right-radius: 16px;"
        "}"
    );
    
    QHBoxLayout *cardHeaderLayout = new QHBoxLayout(cardHeaderWidget);
    cardHeaderLayout->setContentsMargins(12, 16, 12, 16);
    cardHeaderLayout->setSpacing(10);
    
    // 图标
    QLabel *iconLabel = new QLabel("🌊");
    iconLabel->setStyleSheet("font-size: 18px; color: #5BC0FF;");
    
    // 标题
    QLabel *titleLabel = new QLabel("频谱分析");
    titleLabel->setStyleSheet(
        "QLabel {"
        "    color: #eef2ff;"
        "    font-weight: 600;"
        "    font-size: 16px;"
        "}"
    );
    
    // 实时频谱标签
    QLabel *countLabel = new QLabel("实时频谱");
    countLabel->setStyleSheet(
        "QLabel {"
        "    background: rgba(0, 0, 0, 77);"
        "    padding: 4px 10px;"
        "    border-radius: 20px;"
        "    font-size: 11px;"
        "    color: #5BC0FF;"
        "}"
    );
    
    cardHeaderLayout->addWidget(iconLabel);
    cardHeaderLayout->addWidget(titleLabel);
    cardHeaderLayout->addStretch();
    cardHeaderLayout->addWidget(countLabel);
    
    spectrumLayout->addWidget(cardHeaderWidget);
    
    // 创建频谱容器（模拟HTML中的.spectrum-container）
    QWidget *spectrumContainerWidget = new QWidget();
    spectrumContainerWidget->setStyleSheet(
        "QWidget {"
        "    background: rgba(5, 10, 20, 204);"
        "    padding: 16px;"
        "    border-radius: 0px 0px 16px 16px;"
        "}"
    );
    
    QVBoxLayout *spectrumContainerLayout = new QVBoxLayout(spectrumContainerWidget);
    spectrumContainerLayout->setContentsMargins(16, 16, 16, 16);
    spectrumContainerLayout->setSpacing(0);
    
    // 频谱分析组件 - 使用基于QGraphicsView的频谱图表
    spectrumWidget = new SpectrumChart();
    spectrumWidget->setMinimumHeight(220);
    
    // 设置频谱分析图的样式（与HTML页面相同）
    spectrumWidget->setStyleSheet(
        "QGraphicsView {"
        "    background: transparent;"
        "    border: none;"
        "}"
    );
    
    spectrumContainerLayout->addWidget(spectrumWidget);
    spectrumLayout->addWidget(spectrumContainerWidget);
}

void SituationAnalysis::setupThreatAssessment()
{
    // 创建威胁评估的GroupBox
    threatGroup = new QGroupBox();
    threatGroup->setStyleSheet(
        "QGroupBox {"
        "    background: rgba(8, 16, 30, 190);"
        "    border: 1px solid rgba(72, 187, 255, 77);"
        "    border-radius: 16px;"
        "    margin-top: 10px;"
        "    padding-top: 0px;"
        "}"
        "QGroupBox::title {"
        "    subcontrol-origin: margin;"
        "    subcontrol-position: top left;"
        "    padding: 0px;"
        "    background: transparent;"
        "}"
    );
    
    QVBoxLayout *threatGroupLayout = new QVBoxLayout(threatGroup);
    threatGroupLayout->setContentsMargins(0, 0, 0, 0);
    threatGroupLayout->setSpacing(0);
    
    // 创建卡片头部（模拟HTML中的.card-header）
    QWidget *cardHeaderWidget = new QWidget();
    cardHeaderWidget->setStyleSheet(
        "QWidget {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 rgba(20, 40, 65, 230), stop:1 rgba(10, 22, 45, 180));"
        "    border-bottom: 1px solid rgba(72, 187, 255, 77);"
        "    border-top-left-radius: 16px;"
        "    border-top-right-radius: 16px;"
        "}"
    );
    
    QHBoxLayout *cardHeaderLayout = new QHBoxLayout(cardHeaderWidget);
    cardHeaderLayout->setContentsMargins(12, 16, 12, 16);
    cardHeaderLayout->setSpacing(10);
    
    // 图标
    QLabel *iconLabel = new QLabel("⚠️");
    iconLabel->setStyleSheet("font-size: 18px; color: #FFD966;");
    
    // 标题
    QLabel *titleLabel = new QLabel("威胁评估");
    titleLabel->setStyleSheet(
        "QLabel {"
        "    color: #eef2ff;"
        "    font-weight: 600;"
        "    font-size: 16px;"
        "}"
    );
    
    // 优先级排序标签
    QLabel *countLabel = new QLabel("优先级排序");
    countLabel->setStyleSheet(
        "QLabel {"
        "    background: rgba(0, 0, 0, 77);"
        "    padding: 4px 10px;"
        "    border-radius: 20px;"
        "    font-size: 11px;"
        "    color: #5BC0FF;"
        "}"
    );
    
    cardHeaderLayout->addWidget(iconLabel);
    cardHeaderLayout->addWidget(titleLabel);
    cardHeaderLayout->addStretch();
    cardHeaderLayout->addWidget(countLabel);
    
    threatGroupLayout->addWidget(cardHeaderWidget);
    
    // 威胁评估组件
    threatWidget = new QWidget();
    threatWidget->setMinimumHeight(300);
    threatWidget->setStyleSheet("QWidget { background: transparent; }");
    
    QVBoxLayout *mainLayout = new QVBoxLayout(threatWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // 威胁规则区域 - 4个规则项，2x2网格
    threatRulesWidget = new QWidget();
    threatRulesWidget->setStyleSheet(
        "QWidget {"
        "    background: rgba(0, 0, 0, 50);"
        "    padding: 12px;"
        "}"
    );
    
    QGridLayout *rulesLayout = new QGridLayout(threatRulesWidget);
    rulesLayout->setSpacing(8);
    rulesLayout->setContentsMargins(12, 12, 12, 12);
    
    // 创建规则项的辅助函数
    auto createRuleItem = [](const QString& icon, const QString& iconColor, const QString& name, const QString& value) -> QWidget* {
        QWidget *ruleWidget = new QWidget();
        ruleWidget->setStyleSheet(
            "QWidget {"
            "    background: rgba(20, 40, 60, 130);"
            "    border-radius: 8px;"
            "    padding: 8px 10px;"
            "}"
        );
        
        QHBoxLayout *ruleLayout = new QHBoxLayout(ruleWidget);
        ruleLayout->setContentsMargins(0, 0, 0, 0);
        ruleLayout->setSpacing(8);
        
        QLabel *iconLabel = new QLabel(icon);
        iconLabel->setStyleSheet(QString("font-size: 16px; color: %1;").arg(iconColor));
        
        QWidget *infoWidget = new QWidget();
        QVBoxLayout *infoLayout = new QVBoxLayout(infoWidget);
        infoLayout->setContentsMargins(0, 0, 0, 0);
        infoLayout->setSpacing(2);
        
        QLabel *nameLabel = new QLabel(name);
        nameLabel->setStyleSheet("font-size: 11px; font-weight: 500; color: #eef2ff;");
        
        QLabel *valueLabel = new QLabel(value);
        valueLabel->setStyleSheet("font-size: 10px; color: #8E9CB9;");
        
        infoLayout->addWidget(nameLabel);
        infoLayout->addWidget(valueLabel);
        
        ruleLayout->addWidget(iconLabel);
        ruleLayout->addWidget(infoWidget);
        
        return ruleWidget;
    };
    
    // 添加4个规则项
    rulesLayout->addWidget(createRuleItem("🎯", "#ff8a7a", "威胁等级", "高/中/低"), 0, 0);
    rulesLayout->addWidget(createRuleItem("🔢", "#5bc0ff", "打击优先级", "1-12排序"), 0, 1);
    rulesLayout->addWidget(createRuleItem("⏱️", "#f3a683", "响应时间", "≤30s"), 1, 0);
    rulesLayout->addWidget(createRuleItem("🎯", "#2ecc71", "打击精度", "CEP≤10m"), 1, 1);
    
    mainLayout->addWidget(threatRulesWidget);
    
    // 威胁列表区域
    threatListWidget = new QWidget();
    threatListWidget->setStyleSheet("QWidget { background: transparent; }");
    
    QVBoxLayout *listLayout = new QVBoxLayout(threatListWidget);
    listLayout->setContentsMargins(0, 0, 0, 0);
    listLayout->setSpacing(0);
    
    QScrollArea *listScrollArea = new QScrollArea();
    listScrollArea->setWidgetResizable(true);
    listScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    listScrollArea->setFrameStyle(QFrame::NoFrame);
    listScrollArea->setStyleSheet(
        "QScrollArea {"
        "    background: transparent;"
        "    border: none;"
        "}"
    );
    
    QWidget *listContent = new QWidget();
    listContent->setStyleSheet("QWidget { background: transparent; }");
    QVBoxLayout *listContentLayout = new QVBoxLayout(listContent);
    listContentLayout->setContentsMargins(0, 0, 0, 0);
    listContentLayout->setSpacing(0);
    listContentLayout->addStretch();
    
    listScrollArea->setWidget(listContent);
    listLayout->addWidget(listScrollArea);
    
    mainLayout->addWidget(threatListWidget, 1);
    
    threatGroupLayout->addWidget(threatWidget);
}

void SituationAnalysis::setupFirepowerControl()
{
    // 创建防控火力的GroupBox
    firepowerGroup = new QGroupBox();
    firepowerGroup->setStyleSheet(
        "QGroupBox {"
        "    background: rgba(8, 16, 30, 190);"
        "    border: 1px solid rgba(72, 187, 255, 77);"
        "    border-radius: 16px;"
        "    margin-top: 10px;"
        "    padding-top: 0px;"
        "}"
        "QGroupBox::title {"
        "    subcontrol-origin: margin;"
        "    subcontrol-position: top left;"
        "    padding: 0px;"
        "    background: transparent;"
        "}"
    );
    
    QVBoxLayout *firepowerGroupLayout = new QVBoxLayout(firepowerGroup);
    firepowerGroupLayout->setContentsMargins(0, 0, 0, 0);
    firepowerGroupLayout->setSpacing(0);
    
    // 创建卡片头部（模拟HTML中的.card-header）
    QWidget *cardHeaderWidget = new QWidget();
    cardHeaderWidget->setStyleSheet(
        "QWidget {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 rgba(20, 40, 65, 230), stop:1 rgba(10, 22, 45, 180));"
        "    border-bottom: 1px solid rgba(72, 187, 255, 77);"
        "    border-top-left-radius: 16px;"
        "    border-top-right-radius: 16px;"
        "}"
    );
    
    QHBoxLayout *cardHeaderLayout = new QHBoxLayout(cardHeaderWidget);
    cardHeaderLayout->setContentsMargins(12, 16, 12, 16);
    cardHeaderLayout->setSpacing(10);
    
    // 图标
    QLabel *iconLabel = new QLabel("🎯");
    iconLabel->setStyleSheet("font-size: 18px; color: #2ecc71;");
    
    // 标题
    QLabel *titleLabel = new QLabel("防控火力");
    titleLabel->setStyleSheet(
        "QLabel {"
        "    color: #eef2ff;"
        "    font-weight: 600;"
        "    font-size: 16px;"
        "}"
    );
    
    // 5个单位标签
    QLabel *countLabel = new QLabel("5个单位");
    countLabel->setStyleSheet(
        "QLabel {"
        "    background: rgba(0, 0, 0, 77);"
        "    padding: 4px 10px;"
        "    border-radius: 20px;"
        "    font-size: 11px;"
        "    color: #5BC0FF;"
        "}"
    );
    
    cardHeaderLayout->addWidget(iconLabel);
    cardHeaderLayout->addWidget(titleLabel);
    cardHeaderLayout->addStretch();
    cardHeaderLayout->addWidget(countLabel);
    
    firepowerGroupLayout->addWidget(cardHeaderWidget);
    
    // 防控火力组件
    firepowerWidget = new QWidget();
    firepowerWidget->setMinimumHeight(200);
    firepowerWidget->setStyleSheet(
        "QWidget {"
        "    background: transparent;"
        "    border-radius: 0px 0px 16px 16px;"
        "}"
    );
    
    // 创建5个火力单位
    QVBoxLayout *layout = new QVBoxLayout(firepowerWidget);
    layout->setSpacing(8);
    layout->setContentsMargins(12, 12, 12, 12);
    
    struct FirepowerUnit {
        QString name;
        QString type;
        QString status;
        QString icon;
        QString color;
    };
    
    QList<FirepowerUnit> firepowerUnits = {
        {"HQ-9 防空导弹", "sam", "ready", "🎯", "#2ecc71"},
        {"HQ-16 中程导弹", "sam", "ready", "🎯", "#2ecc71"},
        {"PGZ-09 自行高炮", "aaa", "ready", "🔫", "#2ecc71"},
        {"HQ-7 近程导弹", "sam", "busy", "🎯", "#FFD966"},
        {"PGZ-95 弹炮系统", "aaa", "ready", "🔫", "#2ecc71"}
    };
    
    for (const auto& unit : firepowerUnits) {
        QWidget *unitWidget = new QWidget();
        unitWidget->setStyleSheet(
            "QWidget {"
            "    background: rgba(22, 45, 70, 180);"
            "    border: 1px solid rgba(72, 187, 255, 50);"
            "    border-radius: 8px;"
            "    padding: 8px;"
            "}"
        );
        
        QHBoxLayout *unitLayout = new QHBoxLayout(unitWidget);
        unitLayout->setContentsMargins(8, 8, 8, 8);
        unitLayout->setSpacing(12);
        
        QLabel *iconLabel = new QLabel(unit.icon);
        iconLabel->setStyleSheet(QString("font-size: 20px; color: %1;").arg(unit.color));
        
        QWidget *infoWidget = new QWidget();
        QVBoxLayout *infoLayout = new QVBoxLayout(infoWidget);
        infoLayout->setContentsMargins(0, 0, 0, 0);
        infoLayout->setSpacing(2);
        
        QLabel *nameLabel = new QLabel(unit.name);
        nameLabel->setStyleSheet("font-size: 13px; font-weight: 600; color: #eef2ff;");
        
        QLabel *typeLabel = new QLabel(unit.type);
        typeLabel->setStyleSheet("font-size: 10px; color: #8E9CB9;");
        
        infoLayout->addWidget(nameLabel);
        infoLayout->addWidget(typeLabel);
        
        QString statusColor = unit.status == "ready" ? "#2ecc71" : "#FFD966";
        QLabel *statusLabel = new QLabel(unit.status == "ready" ? "待机" : "执行中");
        statusLabel->setStyleSheet(QString(
            "QLabel {"
            "    padding: 4px 10px;"
            "    border-radius: 10px;"
            "    background: %1;"
            "    color: #0a0f1e;"
            "    font-size: 11px;"
            "    font-weight: 600;"
            "}"
        ).arg(statusColor));
        
        unitLayout->addWidget(iconLabel);
        unitLayout->addWidget(infoWidget, 1);
        unitLayout->addWidget(statusLabel);
        
        layout->addWidget(unitWidget);
    }
    
    firepowerGroupLayout->addWidget(firepowerWidget);
}

void SituationAnalysis::setupSituationControl()
{
    // 态势控制组件 - 与HTML文件相同的布局和样式
    situationControlGroup = new QGroupBox();
    situationControlGroup->setStyleSheet(
        "QGroupBox {"
        "    background: rgba(8, 16, 30, 190);"
        "    border: 1px solid rgba(72, 187, 255, 77);"
        "    border-radius: 16px;"
        "    margin-top: 10px;"
        "    padding-top: 0px;"
        "}"
        "QGroupBox::title {"
        "    subcontrol-origin: margin;"
        "    subcontrol-position: top left;"
        "    padding: 0px;"
        "    background: transparent;"
        "}"
    );
    
    QVBoxLayout *mainLayout = new QVBoxLayout(situationControlGroup);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // 创建卡片头部（模拟HTML中的.card-header）
    QWidget *cardHeaderWidget = new QWidget();
    cardHeaderWidget->setStyleSheet(
        "QWidget {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 rgba(20, 40, 65, 230), stop:1 rgba(10, 22, 45, 180));"
        "    border-bottom: 1px solid rgba(72, 187, 255, 77);"
        "    border-top-left-radius: 16px;"
        "    border-top-right-radius: 16px;"
        "}"
    );
    
    QHBoxLayout *cardHeaderLayout = new QHBoxLayout(cardHeaderWidget);
    cardHeaderLayout->setContentsMargins(12, 16, 12, 16);
    cardHeaderLayout->setSpacing(10);
    
    QLabel *iconLabel = new QLabel("⚙️");
    iconLabel->setStyleSheet("font-size: 18px; color: #f3a683;");
    
    QLabel *titleLabel = new QLabel("态势控制");
    titleLabel->setStyleSheet(
        "QLabel {"
        "    color: #eef2ff;"
        "    font-weight: 600;"
        "    font-size: 16px;"
        "}"
    );
    
    QLabel *countLabel = new QLabel("威力范围");
    countLabel->setStyleSheet(
        "QLabel {"
        "    background: rgba(0, 0, 0, 77);"
        "    padding: 4px 10px;"
        "    border-radius: 20px;"
        "    font-size: 11px;"
        "    color: #5BC0FF;"
        "}"
    );
    
    cardHeaderLayout->addWidget(iconLabel);
    cardHeaderLayout->addWidget(titleLabel);
    cardHeaderLayout->addStretch();
    cardHeaderLayout->addWidget(countLabel);
    
    mainLayout->addWidget(cardHeaderWidget);
    
    // 辐射源威力范围控制组
    QWidget *rangeGroup = new QWidget();
    rangeGroup->setStyleSheet(
        "QWidget {"
        "    background: transparent;"
        "    border-bottom: 1px solid rgba(72, 120, 160, 0.2);"
        "    padding: 12px;"
        "}"
    );
    QVBoxLayout *rangeLayout = new QVBoxLayout(rangeGroup);
    rangeLayout->setSpacing(10);
    rangeLayout->setContentsMargins(0, 0, 0, 0);
    
    // 辐射源威力范围标题
    QLabel *rangeTitle = new QLabel("📡 辐射源威力范围");
    rangeTitle->setStyleSheet("font-size: 11px; color: #8E9CB9;");
    rangeLayout->addWidget(rangeTitle);
    
    // 创建切换开关行
    auto createToggleRow = [](const QString& label, const QString& icon, const QString& color, QCheckBox*& checkbox) -> QWidget* {
        QWidget *rowWidget = new QWidget();
        QHBoxLayout *rowLayout = new QHBoxLayout(rowWidget);
        rowLayout->setContentsMargins(0, 0, 0, 0);
        rowLayout->setSpacing(6);
        
        QLabel *iconLabel = new QLabel(icon);
        iconLabel->setStyleSheet(QString("font-size: 12px; color: %1;").arg(color));
        
        QLabel *labelWidget = new QLabel(label);
        labelWidget->setStyleSheet("font-size: 12px; color: #eef2ff;");
        
        checkbox = new QCheckBox();
        checkbox->setChecked(true);
        checkbox->setStyleSheet(
            "QCheckBox {"
            "    spacing: 0;"
            "    background: transparent;"
            "}"
            "QCheckBox::indicator {"
            "    width: 40px;"
            "    height: 20px;"
            "}"
            "QCheckBox::indicator:unchecked {"
            "    border: 1px solid rgba(72, 120, 160, 0.5);"
            "    border-radius: 10px;"
            "    background: rgba(30, 50, 70, 0.8);"
            "}"
            "QCheckBox::indicator:checked {"
            "    border: 1px solid rgba(91, 192, 255, 0.5);"
            "    border-radius: 10px;"
            "    background: rgba(91, 192, 255, 0.5);"
            "}"
            "QCheckBox::indicator:unchecked:hover {"
            "    background: rgba(50, 80, 110, 0.6);"
            "}"
            "QCheckBox::indicator:checked:hover {"
            "    background: rgba(91, 192, 255, 0.7);"
            "}"
        );
        
        rowLayout->addWidget(iconLabel);
        rowLayout->addWidget(labelWidget);
        rowLayout->addStretch();
        rowLayout->addWidget(checkbox);
        
        return rowWidget;
    };
    
    // 创建各个切换开关行
    rangeLayout->addWidget(createToggleRow("雷达", "🛰️", "#ff8a7a", showRadarCheck));
    rangeLayout->addWidget(createToggleRow("电台", "📡", "#5bc0ff", showCommCheck));
    rangeLayout->addWidget(createToggleRow("通信对抗", "📶", "#f3a683", showCommJammerCheck));
    rangeLayout->addWidget(createToggleRow("雷达对抗", "🛡️", "#c68eff", showRadarJammerCheck));
    
    mainLayout->addWidget(rangeGroup);
    
    // 防控火力控制组
    QWidget *firepowerGroup = new QWidget();
    firepowerGroup->setStyleSheet(
        "QWidget {"
        "    background: transparent;"
        "    padding: 12px;"
        "}"
    );
    QVBoxLayout *firepowerLayout = new QVBoxLayout(firepowerGroup);
    firepowerLayout->setSpacing(10);
    firepowerLayout->setContentsMargins(0, 0, 0, 0);
    
    // 防控火力标题
    QLabel *firepowerTitle = new QLabel("🎯 防控火力");
    firepowerTitle->setStyleSheet("font-size: 11px; color: #8E9CB9;");
    firepowerLayout->addWidget(firepowerTitle);
    
    // 防控火力切换开关
    firepowerLayout->addWidget(createToggleRow("防控火力显示", "🎯", "#2ecc71", showFirepowerCheck));
    
    mainLayout->addWidget(firepowerGroup);
}

void SituationAnalysis::createRadarSourceData()
{
    // 安全检查
    if (!radarSourceTable) {
        qWarning() << "radarSourceTable is null, skipping data creation";
        return;
    }
    
    // 创建雷达源数据
    radarSourceTable->setRowCount(6);
    
    // 雷达数据
    QStringList radarData = {
        "预警雷达-01", "预警雷达", "3.5GHz", "100kW", "高",
        "火控雷达-02", "火控雷达", "9.5GHz", "50kW", "高",
        "搜索雷达-03", "搜索雷达", "2.8GHz", "80kW", "中",
        "导航雷达-04", "导航雷达", "1.2GHz", "30kW", "低",
        "气象雷达-05", "气象雷达", "5.6GHz", "20kW", "低",
        "跟踪雷达-06", "跟踪雷达", "10.5GHz", "60kW", "高"
    };
    
    for (int i = 0; i < 6; ++i) {
        for (int j = 0; j < 5; ++j) {
            QTableWidgetItem *item = new QTableWidgetItem(radarData[i * 5 + j]);
            radarSourceTable->setItem(i, j, item);
            
            // 设置威胁等级颜色
            if (j == 4) {
                if (radarData[i * 5 + j] == "高") {
                    item->setBackground(QColor(255, 90, 70, 80));
                    item->setForeground(QColor(255, 138, 122));
                } else if (radarData[i * 5 + j] == "中") {
                    item->setBackground(QColor(255, 180, 60, 80));
                    item->setForeground(QColor(255, 217, 102));
                } else {
                    item->setBackground(QColor(100, 200, 130, 80));
                    item->setForeground(QColor(111, 207, 151));
                }
            }
        }
    }
    
    // 其他表格的示例数据...
}

void SituationAnalysis::createThreatData()
{
    // 安全检查
    if (!threatListWidget) {
        qWarning() << "threatListWidget is null, skipping threat data creation";
        return;
    }
    
    // 找到威胁列表的滚动区域内容
    QScrollArea *listScrollArea = threatListWidget->findChild<QScrollArea*>();
    if (!listScrollArea) {
        qWarning() << "listScrollArea not found";
        return;
    }
    
    QWidget *listContent = listScrollArea->widget();
    if (!listContent) {
        qWarning() << "listContent not found";
        return;
    }
    
    QVBoxLayout *listContentLayout = qobject_cast<QVBoxLayout*>(listContent->layout());
    if (!listContentLayout) {
        qWarning() << "listContentLayout not found";
        return;
    }
    
    // 清除现有内容（除了最后的stretch）
    while (listContentLayout->count() > 1) {
        QLayoutItem *item = listContentLayout->takeAt(0);
        if (item && item->widget()) {
            delete item->widget();
        }
        delete item;
    }
    
    // 威胁数据（与HTML中的12个目标一致）
    struct ThreatItem {
        QString name;
        QString type;
        QString threat;
        QString icon;
        QString iconColor;
    };
    
    QList<ThreatItem> threatItems = {
        {"AN/MPQ-53 相控阵雷达", "radar", "高", "🛰️", "#ff8a7a"},
        {"MPQ-64 哨兵雷达", "radar", "高", "🛰️", "#ff8a7a"},
        {"Link-16 战术数据链", "communication", "高", "📡", "#5bc0ff"},
        {"SPECTRAL 侦察干扰吊舱", "radar_jammer", "高", "🛡️", "#c68eff"},
        {"Pelena-1 地面干扰站", "radar_jammer", "高", "🛡️", "#c68eff"},
        {"R-330Zh 通信干扰系统", "comm_jammer", "高", "📶", "#f3a683"},
        {"P-18 预警雷达", "radar", "中", "🛰️", "#ff8a7a"},
        {"VHF 战术电台", "communication", "中", "📡", "#5bc0ff"},
        {"卫星通信终端", "communication", "中", "📡", "#5bc0ff"},
        {"车载智能干扰站", "comm_jammer", "中", "📶", "#f3a683"},
        {"战术侦察/干扰模块", "radar_jammer", "中", "🛡️", "#c68eff"},
        {"便携式通信干扰机", "comm_jammer", "低", "📶", "#f3a683"}
    };
    
    // 创建威胁项的辅助函数
    auto createThreatItem = [](const ThreatItem& item, int rank) -> QWidget* {
        QWidget *threatWidget = new QWidget();
        threatWidget->setStyleSheet(
            "QWidget {"
            "    background: transparent;"
            "    border-bottom: 1px solid rgba(72, 120, 160, 38);"
            "    padding: 10px 12px;"
            "}"
            "QWidget:hover {"
            "    background: rgba(30, 70, 110, 77);"
            "}"
        );
        
        QHBoxLayout *threatLayout = new QHBoxLayout(threatWidget);
        threatLayout->setContentsMargins(0, 0, 0, 0);
        threatLayout->setSpacing(10);
        
        // 优先级圆形
        QLabel *rankLabel = new QLabel(QString::number(rank + 1));
        QString rankClass = rank < 1 ? "rank-1" : rank < 4 ? "rank-2" : "rank-3";
        QString rankBgColor = rank < 1 ? "qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #ff5e4a, stop:1 #ff8a7a)" :
                              rank < 4 ? "qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #f5b042, stop:1 #FFD966)" :
                              "qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #2ecc71, stop:1 #6fcf97)";
        QString rankTextColor = rank < 4 ? (rank < 1 ? "#fff" : "#333") : "#fff";
        
        rankLabel->setStyleSheet(QString(
            "QLabel {"
            "    width: 28px;"
            "    height: 28px;"
            "    border-radius: 14px;"
            "    background: %1;"
            "    color: %2;"
            "    font-weight: 700;"
            "    font-size: 13px;"
            "    qproperty-alignment: AlignCenter;"
            "}"
        ).arg(rankBgColor).arg(rankTextColor));
        rankLabel->setFixedSize(28, 28);
        rankLabel->setAlignment(Qt::AlignCenter);
        
        // 威胁名称和类型
        QWidget *nameWidget = new QWidget();
        QVBoxLayout *nameLayout = new QVBoxLayout(nameWidget);
        nameLayout->setContentsMargins(0, 0, 0, 0);
        nameLayout->setSpacing(2);
        
        QWidget *nameRowWidget = new QWidget();
        QHBoxLayout *nameRowLayout = new QHBoxLayout(nameRowWidget);
        nameRowLayout->setContentsMargins(0, 0, 0, 0);
        nameRowLayout->setSpacing(6);
        
        QLabel *iconLabel = new QLabel(item.icon);
        iconLabel->setStyleSheet(QString("font-size: 13px; color: %1;").arg(item.iconColor));
        
        QLabel *nameLabel = new QLabel(item.name);
        nameLabel->setStyleSheet("font-size: 14px; color: #eef2ff;");
        
        nameRowLayout->addWidget(iconLabel);
        nameRowLayout->addWidget(nameLabel);
        
        QLabel *typeLabel = new QLabel(item.type);
        typeLabel->setStyleSheet("font-size: 10px; color: #8E9CB9;");
        
        nameLayout->addWidget(nameRowWidget);
        nameLayout->addWidget(typeLabel);
        
        // 威胁等级标签
        QString threatColor = item.threat == "高" ? "rgba(255, 90, 70, 77)" :
                              item.threat == "中" ? "rgba(255, 180, 60, 77)" : "rgba(100, 200, 130, 77)";
        QString threatTextColor = item.threat == "高" ? "#ff8a7a" :
                                  item.threat == "中" ? "#FFD966" : "#6fcf97";
        QString threatBorderColor = item.threat == "高" ? "rgba(255, 90, 70, 128)" :
                                     item.threat == "中" ? "rgba(255, 180, 60, 128)" : "rgba(100, 200, 130, 128)";
        
        QLabel *threatLabel = new QLabel(item.threat);
        threatLabel->setStyleSheet(QString(
            "QLabel {"
            "    padding: 4px 10px;"
            "    border-radius: 12px;"
            "    background: %1;"
            "    color: %2;"
            "    border: 1px solid %3;"
            "    font-size: 11px;"
            "    font-weight: 600;"
            "}"
        ).arg(threatColor).arg(threatTextColor).arg(threatBorderColor));
        
        threatLayout->addWidget(rankLabel);
        threatLayout->addWidget(nameWidget, 1);
        threatLayout->addWidget(threatLabel);
        
        return threatWidget;
    };
    
    // 添加所有威胁项
    for (int i = 0; i < threatItems.size(); ++i) {
        listContentLayout->insertWidget(i, createThreatItem(threatItems[i], i));
    }
}

void SituationAnalysis::createFirepowerData()
{
    // 创建防控火力数据
    // 这里可以添加防控火力的具体实现
}

void SituationAnalysis::updateSystemTime()
{
    QDateTime currentTime = QDateTime::currentDateTime();
    currentTimeLabel->setText(currentTime.toString("hh:mm:ss"));
}

void SituationAnalysis::onRadarTabChanged(int index)
{
    // 标签页切换处理
    qDebug() << "切换到标签页:" << index;
}

void SituationAnalysis::onToggleRadar(bool checked)
{
    qDebug() << "雷达显示:" << checked;
}

void SituationAnalysis::onToggleComm(bool checked)
{
    qDebug() << "电台显示:" << checked;
}

void SituationAnalysis::onToggleCommJammer(bool checked)
{
    qDebug() << "通信对抗显示:" << checked;
}

void SituationAnalysis::onToggleRadarJammer(bool checked)
{
    qDebug() << "雷达对抗显示:" << checked;
}

void SituationAnalysis::onToggleFirepower(bool checked)
{
    qDebug() << "防控火力显示:" << checked;
}