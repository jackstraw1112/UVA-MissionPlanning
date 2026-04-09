//
// Created by admin on "2026.04.09 T 19:19:20".
//

#ifndef RZSOURCERADIATION_H
#define RZSOURCERADIATION_H

#include <QWidget>
#include <QMap>
#include <QStandardItemModel>
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
 * @details 管理雷达、电台、雷达干扰、通信干扰四类数据的模型初始化、显示与切换。
 */
class RZSourceRadiation : public QWidget
{
    Q_OBJECT

public:
    explicit RZSourceRadiation(QWidget *parent = nullptr);
    ~RZSourceRadiation() override;

private:
    // 初始化参数（预留扩展）
    void initParams();
    // 初始化对象（数据、视图、模型）
    void initObject();
    // 关联信号与槽函数（按钮点击 -> 模型切换）
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
    void initTableViewAttr();

    /**
     * @brief 初始化数据模型
     * @details 仅创建四类数据模型并初始化表头；不在此函数中写入行数据
     */
    void initDataModel();

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
    void displayData(const RadarSource &data, int row = -1);

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
     * @brief 添加辐射源数据（模板接口）
     * @tparam T 辐射源类型：RadarSource / RadioSource / RadarJammerSource / RadioJammerSource
     * @param data 待添加的数据对象
     * @details 调用后会同步更新对应缓存与界面模型。
     */
    template <typename T>
    void addData(const T &data)
    {
        addDataImpl(data);
    }

    /**
     * @brief 更新辐射源数据（模板接口）
     * @tparam T 辐射源类型：RadarSource / RadioSource / RadarJammerSource / RadioJammerSource
     * @param data 待更新的数据对象（按 name 匹配）
     * @details 若未找到同名数据，则按新增处理。
     */
    template <typename T>
    void updateData(const T &data)
    {
        updateDataImpl(data);
    }

    /**
     * @brief 删除辐射源数据（模板接口）
     * @tparam T 辐射源类型：RadarSource / RadioSource / RadarJammerSource / RadioJammerSource
     * @param name 待删除目标名称（按 name 匹配）
     * @details 调用后会同步删除对应缓存和界面模型中的行。
     */
    template <typename T>
    void deleteData(const QString &name)
    {
        deleteDataImpl<T>(name);
    }

private:
    Ui::RZSourceRadiation *ui;

    // 雷达辐射源数据
    QVector<RadarSource> m_radarSource;
    // 电台辐射源数据
    QVector<RadioSource> m_radioSource;
    // 雷达干扰辐射源数据
    QVector<RadarJammerSource> m_radarJammerSource;
    // 通信干扰辐射源数据
    QVector<RadioJammerSource> m_RadioJammerSource;

    // 数据模型映射：key 为类别名（雷达/电台/雷达干扰/通信干扰）
    QMap<QString, QStandardItemModel *> m_mapModel;

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
     * @tparam T 目标辐射源类型
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
};

#endif //RZSOURCERADIATION_H
