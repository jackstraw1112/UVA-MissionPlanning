//
// Created by admin on "2026.04.09 T 19:19:20".
//

#ifndef THREATASSESSMENT_H
#define THREATASSESSMENT_H

#include <QWidget>
#include <QVector>
#include <QMap>
#include <QStandardItemModel>
#include <type_traits>
#include "EstimateSituationStruct.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class ThreatAssessment;
}
QT_END_NAMESPACE

/**
 * @brief 威胁评估窗口
 * @details 显示和管理威胁评估数据，包括目标名称、类型和威胁等级
 */
class ThreatAssessment : public QWidget
{
    Q_OBJECT

public:
    explicit ThreatAssessment(QWidget *parent = nullptr);
    ~ThreatAssessment() override;

public:
    /**
     * @brief 添加威胁数据（模板接口）
     * @tparam T 威胁数据类型：RadarSource / RadioSource / RadarJammerSource / RadioJammerSource
     * @param data 待添加的数据对象
     * @details 调用后会同步更新对应缓存与界面模型。
     */
    template <typename T>
    void addData(const T &data)
    {
        addDataImpl(data);
    }

    /**
     * @brief 更新威胁数据（模板接口）
     * @tparam T 威胁数据类型：RadarSource / RadioSource / RadarJammerSource / RadioJammerSource
     * @param data 待更新的数据对象（按 name 匹配）
     * @details 若未找到同名数据，则按新增处理。
     */
    template <typename T>
    void updateData(const T &data)
    {
        updateDataImpl(data);
    }

    /**
     * @brief 删除威胁数据（模板接口）
     * @tparam T 威胁数据类型：RadarSource / RadioSource / RadarJammerSource / RadioJammerSource
     * @param name 待删除目标名称（按 name 匹配）
     * @details 调用后会同步删除对应缓存和界面模型中的行。
     */
    template <typename T>
    void deleteData(const QString &name)
    {
        deleteDataImpl<T>(name);
    }

private:
    // 初始化参数（预留扩展）
    void initParams();
    // 初始化对象（数据、视图、模型）
    void initObject();
    // 关联信号与槽函数
    void initConnect();

    /**
     * @brief 生成测试数据
     * @details 生成威胁评估测试数据
     */
    void generateTestData();

    /**
     * @brief 初始化表格属性
     * @details 设置表格的交互行为、字体、颜色等属性
     */
    void initTableViewAttr();

    /**
     * @brief 初始化数据模型
     * @details 创建并配置数据模型
     */
    void initDataModel();

    /**
     * @brief 显示数据
     * @details 将威胁数据显示到表格中
     */
    void displayData();

    /**
     * @brief 排序数据
     * @details 根据威胁等级排序所有数据
     */
    void sortData();

    /**
     * @brief 写入模型行
     */
    void writeModelRow(QStandardItemModel *model, const QStringList &columns, int row = -1);

private:
    Ui::ThreatAssessment *ui;

    // 雷达辐射源数据
    QVector<RadarSource> m_radarSource;
    // 通信电台数据
    QVector<RadioSource> m_radioSource;
    // 雷达对抗设备数据
    QVector<RadarJammerSource> m_radarJammerSource;
    // 通信对抗设备数据
    QVector<RadioJammerSource> m_radioJammerSource;
    // 数据模型映射：key 为类别名（雷达/电台/雷达干扰/通信干扰）
    QMap<QString, QStandardItemModel *> m_mapModel;

    /**
     * @brief 统一威胁数据项结构体
     * @details 用于统一排序所有类型的威胁数据
     */
    struct UnifiedThreatItem
    {
        QString name;       // 目标名称
        QString threatLevel; // 威胁等级
        QString type;       // 数据类型（雷达/电台/雷达对抗/通信对抗）
        int priority;       // 威胁等级优先级（高=3, 中=2, 低=1）

        UnifiedThreatItem() = default;

        UnifiedThreatItem(const QString &n, const QString &t, const QString &type, int p)
            : name(n), threatLevel(t), type(type), priority(p) {}
    };

    QVector<UnifiedThreatItem> m_unifiedThreatData;

private:
    // 类型化增删改实现：由模板公共接口分发调用
    // --- Add ---
    void addDataImpl(const RadarSource &data);
    void addDataImpl(const RadioSource &data);
    void addDataImpl(const RadarJammerSource &data);
    void addDataImpl(const RadioJammerSource &data);

    // --- Update ---
    void updateDataImpl(const RadarSource &data);
    void updateDataImpl(const RadioSource &data);
    void updateDataImpl(const RadarJammerSource &data);
    void updateDataImpl(const RadioJammerSource &data);

    // --- Delete by name ---
    void deleteRadarDataByName(const QString &name);
    void deleteRadioDataByName(const QString &name);
    void deleteRadarJammerDataByName(const QString &name);
    void deleteRadioJammerDataByName(const QString &name);

    /**
     * @brief 删除实现分发（模板内部函数）
     * @tparam T 目标威胁数据类型
     * @param name 待删除目标名称
     * @details 通过 if constexpr 在编译期分发到对应删除函数。
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
        else
        {
            static_assert(std::is_same_v<T, void>, "Unsupported type for deleteData<T>()");
        }
    }

    /**
     * @brief 根据名称查找索引
     * @tparam T 数据类型
     * @param container 数据容器
     * @param name 目标名称
     * @return 找到的索引，未找到返回 -1
     */
    template <typename T>
    int findIndexByName(const QVector<T> &container, const QString &name)
    {
        for (int i = 0; i < container.size(); ++i)
        {
            if (container.at(i).name == name)
            {
                return i;
            }
        }
        return -1;
    }
};

#endif // THREATASSESSMENT_H
