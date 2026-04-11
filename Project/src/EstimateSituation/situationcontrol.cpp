//
// Created by admin on "2026.04.09 T 19:19:20".
//

// You may need to build the project (run Qt uic code generator) to get "ui_SituationControl.h" resolved

#include "SituationControl.h"
#include "ui_SituationControl.h"

SituationControl::SituationControl(QWidget *parent)
    : QWidget(parent), ui(new Ui::SituationControl)
    , m_radarSwitch(nullptr)
    , m_radioSwitch(nullptr)
    , m_commJamSwitch(nullptr)
    , m_radarJamSwitch(nullptr)
    , m_defenseFireSwitch(nullptr)
{
    ui->setupUi(this);

    // 初始化参数
    initParams();

    // 初始化对象
    initObject();

    // 关联信号与槽函数
    initConnect();
}

SituationControl::~SituationControl()
{
    delete ui;
}

void SituationControl::initParams()
{
}

void SituationControl::initObject()
{
    // 替换普通 QCheckBox 为带动画的 AnimatedSwitch
    // 雷达开关
    m_radarSwitch = new AnimatedSwitch();
    m_radarSwitch->setFixedSize(ui->radarSwitch->size());
    m_radarSwitch->setChecked(ui->radarSwitch->isChecked());
    ui->radarLayout->replaceWidget(ui->radarSwitch, m_radarSwitch);
    ui->radarSwitch->deleteLater();

    // 电台开关
    m_radioSwitch = new AnimatedSwitch();
    m_radioSwitch->setFixedSize(ui->radioSwitch->size());
    m_radioSwitch->setChecked(ui->radioSwitch->isChecked());
    ui->radioLayout->replaceWidget(ui->radioSwitch, m_radioSwitch);
    ui->radioSwitch->deleteLater();

    // 通信对抗开关
    m_commJamSwitch = new AnimatedSwitch();
    m_commJamSwitch->setFixedSize(ui->commJamSwitch->size());
    m_commJamSwitch->setChecked(ui->commJamSwitch->isChecked());
    ui->commJamLayout->replaceWidget(ui->commJamSwitch, m_commJamSwitch);
    ui->commJamSwitch->deleteLater();

    // 雷达对抗开关
    m_radarJamSwitch = new AnimatedSwitch();
    m_radarJamSwitch->setFixedSize(ui->radarJamSwitch->size());
    m_radarJamSwitch->setChecked(ui->radarJamSwitch->isChecked());
    ui->radarJamLayout->replaceWidget(ui->radarJamSwitch, m_radarJamSwitch);
    ui->radarJamSwitch->deleteLater();

    // 防控火力显示开关
    m_defenseFireSwitch = new AnimatedSwitch();
    m_defenseFireSwitch->setFixedSize(ui->defenseFireDisplaySwitch->size());
    m_defenseFireSwitch->setChecked(ui->defenseFireDisplaySwitch->isChecked());
    ui->defenseFireDisplayLayout->replaceWidget(ui->defenseFireDisplaySwitch, m_defenseFireSwitch);
    ui->defenseFireDisplaySwitch->deleteLater();

    // 生成测试数据
    generateTestData();
}

void SituationControl::initConnect()
{
    // 雷达开关
    connect(m_radarSwitch, &AnimatedSwitch::toggled, this, &SituationControl::onRadarSwitchChanged);
    // 电台开关
    connect(m_radioSwitch, &AnimatedSwitch::toggled, this, &SituationControl::onRadioSwitchChanged);
    // 通信对抗开关
    connect(m_commJamSwitch, &AnimatedSwitch::toggled, this, &SituationControl::onCommJamSwitchChanged);
    // 雷达对抗开关
    connect(m_radarJamSwitch, &AnimatedSwitch::toggled, this, &SituationControl::onRadarJamSwitchChanged);
    // 防控火力显示开关
    connect(m_defenseFireSwitch, &AnimatedSwitch::toggled, this, &SituationControl::onDefenseFireDisplaySwitchChanged);
}

void SituationControl::generateTestData()
{
    // 重新生成测试数据前先清空容器，避免重复追加
    m_controlData.clear();

    // 缓存初始状态
    m_controlData.append(SituationControlData("radar", m_radarSwitch->isChecked(), QString::fromUtf8("雷达")));
    m_controlData.append(SituationControlData("radio", m_radioSwitch->isChecked(), QString::fromUtf8("电台")));
    m_controlData.append(SituationControlData("commJam", m_commJamSwitch->isChecked(), QString::fromUtf8("通信对抗")));
    m_controlData.append(SituationControlData("radarJam", m_radarJamSwitch->isChecked(), QString::fromUtf8("雷达对抗")));
    m_controlData.append(SituationControlData("defenseFire", m_defenseFireSwitch->isChecked(), QString::fromUtf8("防控火力")));
}

void SituationControl::addDataImpl(const SituationControlData &data)
{
    m_controlData.append(data);
}

void SituationControl::updateDataImpl(const SituationControlData &data)
{
    const int row = findIndexByType(m_controlData, data.type);
    if (row < 0)
    {
        addDataImpl(data);
        return;
    }
    m_controlData[row] = data;
}

void SituationControl::deleteControlDataByType(const QString &type)
{
    const int row = findIndexByType(m_controlData, type);
    if (row < 0)
    {
        return;
    }
    m_controlData.removeAt(row);
}

void SituationControl::onRadarSwitchChanged(bool checked)
{
    emit controlStateChanged("radar", checked);
    updateDataImpl(SituationControlData("radar", checked, QString::fromUtf8("雷达")));
}

void SituationControl::onRadioSwitchChanged(bool checked)
{
    emit controlStateChanged("radio", checked);
    updateDataImpl(SituationControlData("radio", checked, QString::fromUtf8("电台")));
}

void SituationControl::onCommJamSwitchChanged(bool checked)
{
    emit controlStateChanged("commJam", checked);
    updateDataImpl(SituationControlData("commJam", checked, QString::fromUtf8("通信对抗")));
}

void SituationControl::onRadarJamSwitchChanged(bool checked)
{
    emit controlStateChanged("radarJam", checked);
    updateDataImpl(SituationControlData("radarJam", checked, QString::fromUtf8("雷达对抗")));
}

void SituationControl::onDefenseFireDisplaySwitchChanged(bool checked)
{
    emit controlStateChanged("defenseFire", checked);
    updateDataImpl(SituationControlData("defenseFire", checked, QString::fromUtf8("防控火力")));
}
