//
// Created by admin on "2026.04.09 T 19:19:20".
//

// You may need to build the project (run Qt uic code generator) to get "ui_SituationControl.h" resolved

#include "SituationControl.h"
#include "ui_SituationControl.h"

/**
 * @brief 构造函数
 * @param parent 父窗口指针
 */
SituationControl::SituationControl(QWidget *parent)
    : QWidget(parent), ui(new Ui::SituationControl)
{
    ui->setupUi(this);

    // 初始化参数
    initParams();

    // 初始化对象
    initObject();

    // 关联信号与槽函数
    initConnect();
}

/**
 * @brief 初始化参数
 * @details 预留扩展
 */
void SituationControl::initParams()
{
}

/**
 * @brief 初始化对象
 * @details 数据、视图
 */
void SituationControl::initObject()
{
    // 替换普通 QCheckBox 为带动画的 AnimatedSwitch
    // 雷达开关
    AnimatedSwitch *radarSwitch = new AnimatedSwitch();
    radarSwitch->setFixedSize(ui->radarSwitch->size());
    radarSwitch->setChecked(ui->radarSwitch->isChecked());
    ui->radarLayout->replaceWidget(ui->radarSwitch, radarSwitch);
    connect(radarSwitch, &AnimatedSwitch::toggled, this, &SituationControl::onRadarSwitchChanged);
    ui->radarSwitch->deleteLater();
    
    // 电台开关
    AnimatedSwitch *radioSwitch = new AnimatedSwitch();
    radioSwitch->setFixedSize(ui->radioSwitch->size());
    radioSwitch->setChecked(ui->radioSwitch->isChecked());
    ui->radioLayout->replaceWidget(ui->radioSwitch, radioSwitch);
    connect(radioSwitch, &AnimatedSwitch::toggled, this, &SituationControl::onRadioSwitchChanged);
    ui->radioSwitch->deleteLater();
    
    // 通信对抗开关
    AnimatedSwitch *commJamSwitch = new AnimatedSwitch();
    commJamSwitch->setFixedSize(ui->commJamSwitch->size());
    commJamSwitch->setChecked(ui->commJamSwitch->isChecked());
    ui->commJamLayout->replaceWidget(ui->commJamSwitch, commJamSwitch);
    connect(commJamSwitch, &AnimatedSwitch::toggled, this, &SituationControl::onCommJamSwitchChanged);
    ui->commJamSwitch->deleteLater();
    
    // 雷达对抗开关
    AnimatedSwitch *radarJamSwitch = new AnimatedSwitch();
    radarJamSwitch->setFixedSize(ui->radarJamSwitch->size());
    radarJamSwitch->setChecked(ui->radarJamSwitch->isChecked());
    ui->radarJamLayout->replaceWidget(ui->radarJamSwitch, radarJamSwitch);
    connect(radarJamSwitch, &AnimatedSwitch::toggled, this, &SituationControl::onRadarJamSwitchChanged);
    ui->radarJamSwitch->deleteLater();
    
    // 防控火力显示开关
    AnimatedSwitch *defenseFireDisplaySwitch = new AnimatedSwitch();
    defenseFireDisplaySwitch->setFixedSize(ui->defenseFireDisplaySwitch->size());
    defenseFireDisplaySwitch->setChecked(ui->defenseFireDisplaySwitch->isChecked());
    ui->defenseFireDisplayLayout->replaceWidget(ui->defenseFireDisplaySwitch, defenseFireDisplaySwitch);
    connect(defenseFireDisplaySwitch, &AnimatedSwitch::toggled, this, &SituationControl::onDefenseFireDisplaySwitchChanged);
    ui->defenseFireDisplaySwitch->deleteLater();

    // 缓存初始状态
    m_controlData.append(SituationControlData("radar", radarSwitch->isChecked(), "雷达"));
    m_controlData.append(SituationControlData("radio", radioSwitch->isChecked(), "电台"));
    m_controlData.append(SituationControlData("commJam", commJamSwitch->isChecked(), "通信对抗"));
    m_controlData.append(SituationControlData("radarJam", radarJamSwitch->isChecked(), "雷达对抗"));
    m_controlData.append(SituationControlData("defenseFire", defenseFireDisplaySwitch->isChecked(), "防控火力"));
}

/**
 * @brief 关联信号与槽函数
 */
void SituationControl::initConnect()
{
    // 注意：由于我们替换了原始的 QCheckBox，需要在创建 AnimatedSwitch 时直接连接信号
    // 这里在 initObject 函数中创建开关时已经连接了信号
    // 实际使用时，应该在创建每个 AnimatedSwitch 后立即连接其信号
}

/**
 * @brief 析构函数
 */
SituationControl::~SituationControl()
{
    delete ui;
}

/**
 * @brief 根据类型查找索引
 * @param container 数据容器
 * @param type 目标类型
 * @return 找到的索引，未找到返回 -1
 */
int SituationControl::findIndexByType(const QVector<SituationControlData> &container, const QString &type)
{
    for (int i = 0; i < container.size(); ++i)
    {
        if (container.at(i).type == type)
        {
            return i;
        }
    }
    return -1;
}

/**
 * @brief 添加态势控制数据
 * @param data 态势控制数据对象
 * @details 追加到缓存中。
 */
void SituationControl::addDataImpl(const SituationControlData &data)
{
    m_controlData.append(data);
}

/**
 * @brief 更新态势控制数据
 * @param data 态势控制数据对象（按 type 匹配）
 * @details 若未找到同类型记录则转为新增。
 */
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

/**
 * @brief 删除态势控制数据
 * @param type 目标类型
 * @details 从缓存中删除同类型记录。
 */
void SituationControl::deleteControlDataByType(const QString &type)
{
    const int row = findIndexByType(m_controlData, type);
    if (row >= 0)
    {
        m_controlData.remove(row);
    }
}

/**
 * @brief 雷达开关状态变更
 * @param checked 是否启用
 */
void SituationControl::onRadarSwitchChanged(bool checked)
{
    emit controlStateChanged("radar", checked);
    updateDataImpl(SituationControlData("radar", checked, "雷达"));
}

/**
 * @brief 电台开关状态变更
 * @param checked 是否启用
 */
void SituationControl::onRadioSwitchChanged(bool checked)
{
    emit controlStateChanged("radio", checked);
    updateDataImpl(SituationControlData("radio", checked, "电台"));
}

/**
 * @brief 通信对抗开关状态变更
 * @param checked 是否启用
 */
void SituationControl::onCommJamSwitchChanged(bool checked)
{
    emit controlStateChanged("commJam", checked);
    updateDataImpl(SituationControlData("commJam", checked, "通信对抗"));
}

/**
 * @brief 雷达对抗开关状态变更
 * @param checked 是否启用
 */
void SituationControl::onRadarJamSwitchChanged(bool checked)
{
    emit controlStateChanged("radarJam", checked);
    updateDataImpl(SituationControlData("radarJam", checked, "雷达对抗"));
}

/**
 * @brief 防控火力显示开关状态变更
 * @param checked 是否启用
 */
void SituationControl::onDefenseFireDisplaySwitchChanged(bool checked)
{
    emit controlStateChanged("defenseFire", checked);
    updateDataImpl(SituationControlData("defenseFire", checked, "防控火力"));
}
