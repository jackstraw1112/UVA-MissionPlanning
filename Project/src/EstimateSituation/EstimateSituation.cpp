//
// Created by admin on "2026.04.08 T 14:58:56".
//

#include "EstimateSituation.h"
#include "ui_EstimateSituation.h"
#include "RZSourceRadiation.h"
#include "SpectrumAnalysis.h"
#include "FirepowerControl.h"
#include "RZThreatAssess.h"
#include <QVBoxLayout>
#include <QSplitter>
#include <QScrollArea>

EstimateSituation::EstimateSituation(QWidget *parent)
    : QWidget(parent), ui(new Ui::EstimateSituation)
    , m_rzSourceRadiation(nullptr)
    , m_spectrumAnalysis(nullptr)
    , m_firepowerControl(nullptr)
    , m_rzThreatAssess(nullptr)
{
    ui->setupUi(this);

    initParams();
    initObject();
    initConnect();
}

EstimateSituation::~EstimateSituation()
{
    delete ui;
}

void EstimateSituation::initParams()
{
    setWindowTitle(QString::fromUtf8("态势估计系统"));
    resize(1000, 800);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    // 设置全局字体
    QFont font;
    font.setFamily("Microsoft YaHei");
    font.setPointSize(10);
    setFont(font);
}

void EstimateSituation::initObject()
{
    m_rzSourceRadiation = new RZSourceRadiation(this);
    m_spectrumAnalysis = new SpectrumAnalysis(this);
    m_spectrumAnalysis->setFont(font());

    m_firepowerControl = new FirepowerControl(this);
    m_rzThreatAssess = new RZThreatAssess(this);

    m_rzSourceRadiation->setMinimumHeight(400);
    m_rzSourceRadiation->setMinimumWidth(400);
    m_spectrumAnalysis->setMinimumHeight(400);
    m_spectrumAnalysis->setMinimumWidth(400);

    m_firepowerControl->setMinimumHeight(400);
    m_rzThreatAssess->setMinimumHeight(400);

    QSplitter *leftSplitter = new QSplitter(Qt::Vertical);
    leftSplitter->addWidget(m_rzSourceRadiation);
    leftSplitter->addWidget(m_spectrumAnalysis);
    leftSplitter->setStretchFactor(0, 1);
    leftSplitter->setStretchFactor(1, 1);
    QList<int> leftSizes;
    leftSizes << 400 << 400;
    leftSplitter->setSizes(leftSizes);

    QSplitter *rightSplitter = new QSplitter(Qt::Vertical);
    rightSplitter->addWidget(m_firepowerControl);
    rightSplitter->addWidget(m_rzThreatAssess);
    rightSplitter->setStretchFactor(0, 1);
    rightSplitter->setStretchFactor(1, 1);
    QList<int> rightSizes;
    rightSizes << 400 << 400;
    rightSplitter->setSizes(rightSizes);
    
    // ── 数据流向设计 ──
    // RZSourceRadiation（辐射源列表）为数据主源，用户在此增/删/改雷达
    // RZThreatAssess（威胁评估）为评估模块，可修改评估参数后回写
    // SpectrumAnalysis（频谱图）为被动显示模块，仅接收数据刷新图表，不回传信号
    //
    // 信号流向：
    //   RZSourceRadiation ──→ RZThreatAssess  （增/删/改同步到评估）
    //   RZSourceRadiation ──→ SpectrumAnalysis （增/删/改同步到频谱图）
    //   RZThreatAssess    ──→ RZSourceRadiation （评估结果回写到辐射源列表）
    //   RZThreatAssess    ──→ SpectrumAnalysis  （评估结果同步到频谱图）
    
    // RZSourceRadiation → RZThreatAssess
    connect(m_rzSourceRadiation, &RZSourceRadiation::radarDataChanged, m_rzThreatAssess, &RZThreatAssess::onRadarDataChanged);
    connect(m_rzSourceRadiation, &RZSourceRadiation::radarDataDeleted, m_rzThreatAssess, &RZThreatAssess::onRadarDataDeleted);
    
    // RZThreatAssess → RZSourceRadiation（评估结果回写）
    connect(m_rzThreatAssess, &RZThreatAssess::radarDataUpdated, m_rzSourceRadiation, &RZSourceRadiation::onRadarDataUpdated);
    connect(m_rzThreatAssess, &RZThreatAssess::radarDataRemoved, m_rzSourceRadiation, &RZSourceRadiation::onRadarDataRemoved);

    // RZSourceRadiation → SpectrumAnalysis
    connect(m_rzSourceRadiation, &RZSourceRadiation::radarDataChanged, m_spectrumAnalysis, &SpectrumAnalysis::onRadarDataChanged);
    connect(m_rzSourceRadiation, &RZSourceRadiation::radarDataDeleted, m_spectrumAnalysis, &SpectrumAnalysis::onRadarDataDeleted);

    // RZThreatAssess → SpectrumAnalysis
    connect(m_rzThreatAssess, &RZThreatAssess::radarDataUpdated, m_spectrumAnalysis, &SpectrumAnalysis::onRadarDataChanged);
    connect(m_rzThreatAssess, &RZThreatAssess::radarDataRemoved, m_spectrumAnalysis, &SpectrumAnalysis::onRadarDataDeleted);

    // 整体水平布局：左侧与右侧水平排列
    QSplitter *mainSplitter = new QSplitter(Qt::Horizontal);
    mainSplitter->addWidget(leftSplitter);
    mainSplitter->addWidget(rightSplitter);
    mainSplitter->setStretchFactor(0, 1);
    mainSplitter->setStretchFactor(1, 1);
    QList<int> mainSizes;
    mainSizes << 600 << 600;
    mainSplitter->setSizes(mainSizes);
    mainSplitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 设置主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(mainSplitter);
}

void EstimateSituation::initConnect()
{
}
// Add 实现：分发到所有子窗口
void EstimateSituation::addDataImpl(const RadarPerformancePara &data)
{
    if (m_rzSourceRadiation) m_rzSourceRadiation->addData(data);
    if (m_spectrumAnalysis) m_spectrumAnalysis->addData(data);
    if (m_rzThreatAssess) m_rzThreatAssess->addRadarData(data);
}

void EstimateSituation::addDataImpl(const FirepowerItem &data)
{
    if (m_firepowerControl) m_firepowerControl->addData(data);
}

void EstimateSituation::addDataImpl(const SituationControlData &data)
{
    if (m_rzSourceRadiation) m_rzSourceRadiation->addData(data);
}

void EstimateSituation::updateDataImpl(const RadarPerformancePara &data)
{
    if (m_rzSourceRadiation) m_rzSourceRadiation->updateData(data);
    if (m_spectrumAnalysis) m_spectrumAnalysis->updateData(data);
    if (m_rzThreatAssess) m_rzThreatAssess->updateRadarData(data);
}

void EstimateSituation::updateDataImpl(const FirepowerItem &data)
{
    if (m_firepowerControl) m_firepowerControl->updateData(data);
}

void EstimateSituation::updateDataImpl(const SituationControlData &data)
{
    if (m_rzSourceRadiation) m_rzSourceRadiation->updateData(data);
}

void EstimateSituation::deleteDataImpl(const QString &name)
{
    if (m_rzSourceRadiation) m_rzSourceRadiation->deleteData(name);
    if (m_spectrumAnalysis) m_spectrumAnalysis->deleteData(name);
    if (m_rzThreatAssess) m_rzThreatAssess->deleteRadarData(name);
}
