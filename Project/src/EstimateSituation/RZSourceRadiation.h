//
// Created by admin on "2026.04.09 T 19:19:20".
//

#ifndef RZSOURCERADIATION_H
#define RZSOURCERADIATION_H

#include <QWidget>
#include <QMap>
#include <QStandardItemModel>
#include <QVector>
#include <type_traits>
#include "EstimateSituationStruct.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class RZSourceRadiation;
}
QT_END_NAMESPACE

/**
 * @brief 辐射源列表窗口
 * @details 管理雷达、电台、雷达干扰、通信干扰四类数据的模型初始化、显示与切换，以及态势控制功能。
 */
class RZSourceRadiation : public QWidget
{
    Q_OBJECT

public:
    explicit RZSourceRadiation(QWidget *parent = nullptr);
    ~RZSourceRadiation() override;

private:
    // 初始化参数
    void initParams();
    // 初始化对象
    void initObject();
    // 关联信号与槽函数
    void initConnect();

    /**
     * @brief 生成测试数据
     * @details 用户生成临时数据
     */
    void generateTestData();

    /**
     * @brief 初始化表格属性
     * @details 设置表格属性，如列数、列宽、行高、表头、数据样式等
     */
    void initTableAttr();

    /**
     * @brief 初始化数据模型
     * @details 仅创建四类数据模型并初始化表头；不在此函数中写入行数据
     */
    void initTableModel();

    /**
     * @brief 显示表格数据
     * @details 用户点击不同辐射源，显示对应的数据
     */
    void onShowTableData();

    /**
     * @brief 显示辐射源数据
     * @details 辐射源数据显示
     */
    void displayData();

    /**
     * @brief 显示雷达辐射源数据
     * @param data 雷达辐射源数据
     * @param row  表格行数，如果为-1，则添加新的行
     */
    void displayData(const RadarPerformancePara &data, int row = -1);

    /**
     * @brief 显示电台辐射源数据
     * @param data 电台辐射源数据
     * @param row  表格行数，如果为-1，则添加新的行
     */
    void displayData(const RadioSource &data, int row = -1);

    /**
     * @brief 显示雷达干扰辐射源数据
     * @param data 雷达干扰辐射源数据
     * @param row  表格行数，如果为-1，则添加新的行
     */
    void displayData(const RadarJammerSource &data, int row = -1);

    /**
     * @brief 显示通信干扰辐射源数据
     * @param data 通信干扰辐射源数据
     * @param row  表格行数，如果为-1，则添加新的行
     */
    void displayData(const RadioJammerSource &data, int row = -1);

    /**
     * @brief 向指定模型写入一行数据
     * @param model 目标模型
     * @param columns 列数据
     * @param row 行号，-1 表示追加
     */
    void writeModelRow(QStandardItemModel *model, const QStringList &columns, int row = -1);

public:
    /**
     * @brief 添加数据（模板接口）
     * @tparam T 数据类型：RadarSource / RadioSource / RadarJammerSource / RadioJammerSource / SituationControlData
     * @param data 待添加的数据对象
     * @details 调用后会同步更新对应缓存与界面模型。
     */
    template <typename T>
    void addData(const T &data)
    {
        if constexpr (std::is_same_v<T, SituationControlData>)
        {
            addControlDataImpl(data);
        }
        else
        {
            addDataImpl(data);
        }
    }

    /**
     * @brief 更新数据（模板接口）
     * @tparam T 数据类型：RadarSource / RadioSource / RadarJammerSource / RadioJammerSource / SituationControlData
     * @param data 待更新的数据对象（按 name 或 type 匹配）
     * @details 若未找到同名或同类型数据，则按新增处理。
     */
    template <typename T>
    void updateData(const T &data)
    {
        if constexpr (std::is_same_v<T, SituationControlData>)
        {
            updateControlDataImpl(data);
        }
        else
        {
            updateDataImpl(data);
        }
    }

    /**
     * @brief 删除数据（模板接口）
     * @tparam T 数据类型：RadarSource / RadioSource / RadarJammerSource / RadioJammerSource / SituationControlData
     * @param nameOrType 待删除目标名称或类型（按 name 或 type 匹配）
     * @details 调用后会同步删除对应缓存和界面模型中的行。
     */
    template <typename T>
    void deleteData(const QString &nameOrType)
    {
        if constexpr (std::is_same_v<T, SituationControlData>)
        {
            deleteControlDataImpl<T>(nameOrType);
        }
        else
        {
            deleteDataImpl<T>(nameOrType);
        }
    }

signals:
    /**
     * @brief 控制状态变更信号
     * @param type 控制类型
     * @param enabled 是否启用
     */
    void controlStateChanged(const QString &type, bool enabled);
    
    /**
     * @brief 雷达数据变更信号
     * @param data 雷达性能参数
     */
    void radarDataChanged(const RadarPerformancePara &data);
    
    /**
     * @brief 雷达数据删除信号
     * @param name 雷达名称
     */
    void radarDataDeleted(const QString &name);
    
public slots:
    /**
     * @brief 接收来自威胁评估模块的雷达数据更新
     * @param data 雷达性能参数
     */
    void onRadarDataUpdated(const RadarPerformancePara &data);
    
    /**
     * @brief 接收来自威胁评估模块的雷达数据删除
     * @param name 雷达名称
     */
    void onRadarDataRemoved(const QString &name);
    
private slots:
    /**
     * @brief 添加雷达
     */
    void onAddRadar();
    
    /**
     * @brief 编辑雷达
     * @param row 行索引
     */
    void onEditRadar(int row);
    
    /**
     * @brief 删除雷达
     * @param row 行索引
     */
    void onDeleteRadar(int row);

private:
    Ui::RZSourceRadiation *ui;

    // 雷达辐射源数据
    QVector<RadarPerformancePara> m_radarSource;
    // 电台辐射源数据
    QVector<RadioSource> m_radioSource;
    // 雷达干扰辐射源数据
    QVector<RadarJammerSource> m_radarJammerSource;
    // 通信干扰辐射源数据
    QVector<RadioJammerSource> m_RadioJammerSource;

    // 数据模型映射：key 为类别名（雷达/电台/雷达干扰/通信干扰）
    QMap<QString, QStandardItemModel *> m_mapModel;

    // 态势控制数据
    QVector<SituationControlData> m_controlData;

private:
    // 态势控制开关槽函数
    void onRadarSwitchChanged(bool checked);
    void onRadioSwitchChanged(bool checked);
    void onCommJamSwitchChanged(bool checked);
    void onRadarJamSwitchChanged(bool checked);
    void onDefenseFireDisplaySwitchChanged(bool checked);

private:
    // 类型化增删改实现：由模板公共接口分发调用
    // --- Add ---
    void addDataImpl(const RadarPerformancePara &data);
    void addDataImpl(const RadarSource &data);
    void addDataImpl(const RadioSource &data);
    void addDataImpl(const RadarJammerSource &data);
    void addDataImpl(const RadioJammerSource &data);
    void addControlDataImpl(const SituationControlData &data);

    // --- Update ---
    void updateDataImpl(const RadarPerformancePara &data);
    void updateDataImpl(const RadarSource &data);
    void updateDataImpl(const RadioSource &data);
    void updateDataImpl(const RadarJammerSource &data);
    void updateDataImpl(const RadioJammerSource &data);
    void updateControlDataImpl(const SituationControlData &data);

    // --- Delete by name / type ---
    void deleteRadarDataByName(const QString &name);
    void deleteRadioDataByName(const QString &name);
    void deleteRadarJammerDataByName(const QString &name);
    void deleteRadioJammerDataByName(const QString &name);
    void deleteControlDataByType(const QString &type);

    /**
     * @brief 删除实现分发（模板内部函数）
     * @tparam T 目标辐射源类型
     * @param name 待删除目标名称
     * @details 通过 if constexpr 在编译期分发到对应删除函数。
     */
    template <typename T>
    void deleteDataImpl(const QString &name)
    {
        if constexpr (std::is_same_v<T, RadarPerformancePara> || std::is_same_v<T, RadarSource>)
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
            static_assert(false, "Unsupported type for deleteData<T>()");
        }
    }

    /**
     * @brief 删除态势控制实现分发（模板内部函数）
     * @tparam T 目标数据类型
     * @param type 待删除目标类型
     * @details 通过 if constexpr 在编译期分发到对应删除函数。
     */
    template <typename T>
    void deleteControlDataImpl(const QString &type)
    {
        if constexpr (std::is_same_v<T, SituationControlData>)
        {
            deleteControlDataByType(type);
        }
        else
        {
            static_assert(std::is_same_v<T, void>, "Unsupported type for deleteControlData<T>()");
        }
    }

    /**
     * @brief 根据类型查找索引
     * @tparam T 数据类型
     * @param container 数据容器
     * @param type 目标类型
     * @return 找到的索引，未找到返回 -1
     */
    template <typename T>
    int findIndexByType(const QVector<T> &container, const QString &type)
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
};

#endif //RZSOURCERADIATION_H
