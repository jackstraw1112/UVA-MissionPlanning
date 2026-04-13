//
// Created by admin on "2026.04.08 T 14:58:56".
//

#include "EstimateSituation.h"
#include "ui_EstimateSituation.h"
#include "RZSourceRadiation.h"
#include "SpectrumAnalysis.h"
#include "ThreatAssessment.h"
#include "FirepowerControl.h"
#include <QVBoxLayout>
#include <QSplitter>
#include <QScrollArea>

EstimateSituation::EstimateSituation(QWidget *parent)
    : QWidget(parent), ui(new Ui::EstimateSituation)
    , m_rzSourceRadiation(nullptr)
    , m_spectrumAnalysis(nullptr)
    , m_threatAssessment(nullptr)
    , m_firepowerControl(nullptr)
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
}

void EstimateSituation::initObject()
{
    m_rzSourceRadiation = new RZSourceRadiation(this);
    m_spectrumAnalysis = new SpectrumAnalysis(this);
    m_threatAssessment = new ThreatAssessment(this);
    m_firepowerControl = new FirepowerControl(this);

    // 设置每个模块的最小大小
    m_rzSourceRadiation->setMinimumHeight(400);
    m_rzSourceRadiation->setMinimumWidth(400);
    m_spectrumAnalysis->setMinimumHeight(400);
    m_spectrumAnalysis->setMinimumWidth(400);
    m_threatAssessment->setMinimumHeight(400);
    m_firepowerControl->setMinimumHeight(400);

    // 左侧：辐射源列表与频谱分析竖直布局
    QSplitter *leftSplitter = new QSplitter(Qt::Vertical);
    leftSplitter->addWidget(m_rzSourceRadiation);
    leftSplitter->addWidget(m_spectrumAnalysis);
    leftSplitter->setStretchFactor(0, 1);
    leftSplitter->setStretchFactor(1, 1);
    QList<int> leftSizes;
    leftSizes << 400 << 400;
    leftSplitter->setSizes(leftSizes);

    // 右侧：防空火力与威胁评估竖直布局
    QSplitter *rightSplitter = new QSplitter(Qt::Vertical);
    rightSplitter->addWidget(m_firepowerControl);
    rightSplitter->addWidget(m_threatAssessment);
    rightSplitter->setStretchFactor(0, 1);
    rightSplitter->setStretchFactor(1, 1);
    QList<int> rightSizes;
    rightSizes << 400 << 400;
    rightSplitter->setSizes(rightSizes);

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

// ==============================================================================
// Add 实现：分发到所有子窗口
// ==============================================================================

void EstimateSituation::addDataImpl(const RadarSource &data)
{
    if (m_rzSourceRadiation) m_rzSourceRadiation->addData(data);
    if (m_spectrumAnalysis) m_spectrumAnalysis->addData(data);
    if (m_threatAssessment) m_threatAssessment->addData(data);
}

void EstimateSituation::addDataImpl(const RadioSource &data)
{
    if (m_rzSourceRadiation) m_rzSourceRadiation->addData(data);
    if (m_spectrumAnalysis) m_spectrumAnalysis->addData(data);
    if (m_threatAssessment) m_threatAssessment->addData(data);
}

void EstimateSituation::addDataImpl(const RadarJammerSource &data)
{
    if (m_rzSourceRadiation) m_rzSourceRadiation->addData(data);
    if (m_spectrumAnalysis) m_spectrumAnalysis->addData(data);
    if (m_threatAssessment) m_threatAssessment->addData(data);
}

void EstimateSituation::addDataImpl(const RadioJammerSource &data)
{
    if (m_rzSourceRadiation) m_rzSourceRadiation->addData(data);
    if (m_spectrumAnalysis) m_spectrumAnalysis->addData(data);
    if (m_threatAssessment) m_threatAssessment->addData(data);
}

void EstimateSituation::addDataImpl(const FirepowerItem &data)
{
    if (m_firepowerControl) m_firepowerControl->addData(data);
}

void EstimateSituation::addDataImpl(const SituationControlData &data)
{
    if (m_rzSourceRadiation) m_rzSourceRadiation->addData(data);
}

// ==============================================================================
// Update 实现：分发到所有子窗口
// ==============================================================================

void EstimateSituation::updateDataImpl(const RadarSource &data)
{
    if (m_rzSourceRadiation) m_rzSourceRadiation->updateData(data);
    if (m_spectrumAnalysis) m_spectrumAnalysis->updateData(data);
    if (m_threatAssessment) m_threatAssessment->updateData(data);
}

void EstimateSituation::updateDataImpl(const RadioSource &data)
{
    if (m_rzSourceRadiation) m_rzSourceRadiation->updateData(data);
    if (m_spectrumAnalysis) m_spectrumAnalysis->updateData(data);
    if (m_threatAssessment) m_threatAssessment->updateData(data);
}

void EstimateSituation::updateDataImpl(const RadarJammerSource &data)
{
    if (m_rzSourceRadiation) m_rzSourceRadiation->updateData(data);
    if (m_spectrumAnalysis) m_spectrumAnalysis->updateData(data);
    if (m_threatAssessment) m_threatAssessment->updateData(data);
}

void EstimateSituation::updateDataImpl(const RadioJammerSource &data)
{
    if (m_rzSourceRadiation) m_rzSourceRadiation->updateData(data);
    if (m_spectrumAnalysis) m_spectrumAnalysis->updateData(data);
    if (m_threatAssessment) m_threatAssessment->updateData(data);
}

void EstimateSituation::updateDataImpl(const FirepowerItem &data)
{
    if (m_firepowerControl) m_firepowerControl->updateData(data);
}

void EstimateSituation::updateDataImpl(const SituationControlData &data)
{
    if (m_rzSourceRadiation) m_rzSourceRadiation->updateData(data);
}

// ==============================================================================
// Delete 实现：分发到所有子窗口
// ==============================================================================

void EstimateSituation::deleteRadarDataByName(const QString &name)
{
    if (m_rzSourceRadiation) m_rzSourceRadiation->deleteData<RadarSource>(name);
    if (m_spectrumAnalysis) m_spectrumAnalysis->deleteData<RadarSource>(name);
    if (m_threatAssessment) m_threatAssessment->deleteData<RadarSource>(name);
}

void EstimateSituation::deleteRadioDataByName(const QString &name)
{
    if (m_rzSourceRadiation) m_rzSourceRadiation->deleteData<RadioSource>(name);
    if (m_spectrumAnalysis) m_spectrumAnalysis->deleteData<RadioSource>(name);
    if (m_threatAssessment) m_threatAssessment->deleteData<RadioSource>(name);
}

void EstimateSituation::deleteRadarJammerDataByName(const QString &name)
{
    if (m_rzSourceRadiation) m_rzSourceRadiation->deleteData<RadarJammerSource>(name);
    if (m_spectrumAnalysis) m_spectrumAnalysis->deleteData<RadarJammerSource>(name);
    if (m_threatAssessment) m_threatAssessment->deleteData<RadarJammerSource>(name);
}

void EstimateSituation::deleteRadioJammerDataByName(const QString &name)
{
    if (m_rzSourceRadiation) m_rzSourceRadiation->deleteData<RadioJammerSource>(name);
    if (m_spectrumAnalysis) m_spectrumAnalysis->deleteData<RadioJammerSource>(name);
    if (m_threatAssessment) m_threatAssessment->deleteData<RadioJammerSource>(name);
}

void EstimateSituation::deleteFirepowerDataByName(const QString &name)
{
    if (m_firepowerControl) m_firepowerControl->deleteData<FirepowerItem>(name);
}

void EstimateSituation::deleteControlDataByType(const QString &type)
{
    if (m_rzSourceRadiation) m_rzSourceRadiation->deleteData<SituationControlData>(type);
}
