//
// Created by admin on "2026.04.08 T 14:58:56".
//

#include "EstimateSituation.h"
#include "ui_EstimateSituation.h"
#include "RZSourceRadiation.h"
#include "SpectrumAnalysis.h"
#include "ThreatAssessment.h"
#include "FirepowerControl.h"
#include "SituationControl.h"
#include <QVBoxLayout>
#include <QSplitter>
#include <QScrollArea>

EstimateSituation::EstimateSituation(QWidget *parent)
    : QWidget(parent), ui(new Ui::EstimateSituation)
    , m_rzSourceRadiation(nullptr)
    , m_spectrumAnalysis(nullptr)
    , m_threatAssessment(nullptr)
    , m_firepowerControl(nullptr)
    , m_situationControl(nullptr)
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
    m_situationControl = new SituationControl(this);

    // 设置每个模块的最小高度
    m_rzSourceRadiation->setMinimumHeight(400);
    m_spectrumAnalysis->setMinimumHeight(500);
    m_threatAssessment->setMinimumHeight(400);
    m_firepowerControl->setMinimumHeight(400);
    m_situationControl->setMinimumHeight(300);

    // 创建垂直分割器
    QSplitter *verticalSplitter = new QSplitter(Qt::Vertical);
    verticalSplitter->addWidget(m_rzSourceRadiation);
    verticalSplitter->addWidget(m_spectrumAnalysis);
    verticalSplitter->addWidget(m_firepowerControl);
    verticalSplitter->addWidget(m_situationControl);
    verticalSplitter->addWidget(m_threatAssessment);
    verticalSplitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 设置初始分割器大小
    QList<int> sizes;
    sizes << 400 << 500 << 400 << 400 << 300;
    verticalSplitter->setSizes(sizes);

    // 创建滚动区域
    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(verticalSplitter);
    scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 设置主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(scrollArea);
}

void EstimateSituation::initConnect()
{
    connect(m_situationControl, &SituationControl::controlStateChanged,
            this, [this](const QString &type, bool enabled)
            {
                Q_UNUSED(type)
                Q_UNUSED(enabled)
            });
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
    if (m_situationControl) m_situationControl->addData(data);
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
    if (m_situationControl) m_situationControl->updateData(data);
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
    if (m_situationControl) m_situationControl->deleteData<SituationControlData>(type);
}
