//
// Created by admin on "2026.04.08 T 14:58:56".
//

#ifndef ESTIMATESITUATION_H
#define ESTIMATESITUATION_H

#include <QWidget>
#include <type_traits>
#include "EstimateSituationStruct.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class EstimateSituation;
}
QT_END_NAMESPACE

class RZSourceRadiation;
class SpectrumAnalysis;
class ThreatAssessment;
class FirepowerControl;

/**
 * @brief 态势估计主窗口
 * @details 主应用窗口，管理所有子窗口的创建、布局和数据互通。
 */
class EstimateSituation : public QWidget
{
    Q_OBJECT

public:
    explicit EstimateSituation(QWidget *parent = nullptr);
    ~EstimateSituation() override;

public:
    /**
     * @brief 添加辐射源数据（模板接口）
     * @tparam T 数据类型：RadarSource / RadioSource / RadarJammerSource / RadioJammerSource
     * @param data 待添加的数据对象
     * @details 同步分发到所有子窗口。
     */
    template <typename T>
    void addData(const T &data)
    {
        addDataImpl(data);
    }

    /**
     * @brief 更新辐射源数据（模板接口）
     * @tparam T 数据类型：RadarSource / RadioSource / RadarJammerSource / RadioJammerSource
     * @param data 待更新的数据对象（按 name 匹配）
     * @details 同步分发到所有子窗口。
     */
    template <typename T>
    void updateData(const T &data)
    {
        updateDataImpl(data);
    }

    /**
     * @brief 删除辐射源数据（模板接口）
     * @tparam T 数据类型：RadarSource / RadioSource / RadarJammerSource / RadioJammerSource
     * @param name 待删除目标名称（按 name 匹配）
     * @details 同步分发到所有子窗口。
     */
    template <typename T>
    void deleteData(const QString &name)
    {
        deleteDataImpl<T>(name);
    }

private:
    // 初始化参数（预留扩展）
    void initParams();
    // 初始化对象（子窗口、布局）
    void initObject();
    // 关联信号与槽函数
    void initConnect();

private:
    Ui::EstimateSituation *ui;

    // 子窗口指针
    RZSourceRadiation *m_rzSourceRadiation;
    SpectrumAnalysis *m_spectrumAnalysis;
    ThreatAssessment *m_threatAssessment;
    FirepowerControl *m_firepowerControl;


private:
    // 类型化增删改实现：由模板公共接口分发调用
    // --- Add ---
    void addDataImpl(const RadarSource &data);
    void addDataImpl(const RadioSource &data);
    void addDataImpl(const RadarJammerSource &data);
    void addDataImpl(const RadioJammerSource &data);
    void addDataImpl(const FirepowerItem &data);
    void addDataImpl(const SituationControlData &data);

    // --- Update ---
    void updateDataImpl(const RadarSource &data);
    void updateDataImpl(const RadioSource &data);
    void updateDataImpl(const RadarJammerSource &data);
    void updateDataImpl(const RadioJammerSource &data);
    void updateDataImpl(const FirepowerItem &data);
    void updateDataImpl(const SituationControlData &data);

    // --- Delete ---
    void deleteRadarDataByName(const QString &name);
    void deleteRadioDataByName(const QString &name);
    void deleteRadarJammerDataByName(const QString &name);
    void deleteRadioJammerDataByName(const QString &name);
    void deleteFirepowerDataByName(const QString &name);
    void deleteControlDataByType(const QString &type);

    /**
     * @brief 删除实现分发（模板内部函数）
     * @tparam T 目标数据类型
     * @param name 待删除目标名称
     */
    template <typename T>
    void deleteDataImpl(const QString &name)
    {
        if constexpr (std::is_same_v<T, RadarSource>)
        {
            deleteRadarDataByName(name);
        }
        else if constexpr (std::is_same_v<T, RadioSource>)
        {
            deleteRadioDataByName(name);
        }
        else if constexpr (std::is_same_v<T, RadarJammerSource>)
        {
            deleteRadarJammerDataByName(name);
        }
        else if constexpr (std::is_same_v<T, RadioJammerSource>)
        {
            deleteRadioJammerDataByName(name);
        }
        else if constexpr (std::is_same_v<T, FirepowerItem>)
        {
            deleteFirepowerDataByName(name);
        }
        else if constexpr (std::is_same_v<T, SituationControlData>)
        {
            deleteControlDataByType(name);
        }
        else
        {
            static_assert(std::is_same_v<T, void>, "Unsupported type for deleteData<T>()");
        }
    }
};

#endif // ESTIMATESITUATION_H
