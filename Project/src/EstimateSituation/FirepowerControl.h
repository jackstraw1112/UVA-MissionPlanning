//
// Created by admin on "2026.04.09 T 19:19:20".
//

#ifndef FIREPOWERCONTROL_H
#define FIREPOWERCONTROL_H

#include <QWidget>
#include <QVector>
#include <type_traits>
#include "EstimateSituationStruct.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class FirepowerControl;
}
QT_END_NAMESPACE

/**
 * @brief 防空火力控制窗口
 * @details 管理防空火力设备的显示和控制，包括设备状态、参数和拦截概率等
 */
class FirepowerControl : public QWidget
{
    Q_OBJECT

public:
    explicit FirepowerControl(QWidget *parent = nullptr);
    ~FirepowerControl() override;

public:
    /**
     * @brief 添加防空火力数据（模板接口）
     * @tparam T 数据类型：FirepowerItem
     * @param data 待添加的数据对象
     * @details 调用后会同步更新缓存与界面显示。
     */
    template <typename T>
    void addData(const T &data)
    {
        addDataImpl(data);
    }

    /**
     * @brief 更新防空火力数据（模板接口）
     * @tparam T 数据类型：FirepowerItem
     * @param data 待更新的数据对象（按 name 匹配）
     * @details 若未找到同名数据，则按新增处理。
     */
    template <typename T>
    void updateData(const T &data)
    {
        updateDataImpl(data);
    }

    /**
     * @brief 删除防空火力数据（模板接口）
     * @tparam T 数据类型：FirepowerItem
     * @param name 待删除目标名称（按 name 匹配）
     * @details 调用后会同步删除缓存和界面中的对应项。
     */
    template <typename T>
    void deleteData(const QString &name)
    {
        deleteDataImpl<T>(name);
    }

private:
    // 初始化参数（预留扩展）
    void initParams();
    // 初始化对象（数据、视图）
    void initObject();
    // 关联信号与槽函数
    void initConnect();

    /**
     * @brief 生成测试数据
     * @details 生成防空火力测试数据
     */
    void generateTestData();

    /**
     * @brief 显示数据
     * @details 将防空火力数据显示到界面中
     */
    void displayData();

    /**
     * @brief 添加设备项
     * @details 根据设备数据创建并添加设备项到界面
     * @param item 设备数据
     */
    void addFirepowerItem(const FirepowerItem &item);

private:
    Ui::FirepowerControl *ui;
    QVector<FirepowerItem> m_firepowerData;

private:
    // 类型化增删改实现：由模板公共接口分发调用
    // --- Add ---
    void addDataImpl(const FirepowerItem &data);

    // --- Update ---
    void updateDataImpl(const FirepowerItem &data);

    // --- Delete by name ---
    void deleteFirepowerDataByName(const QString &name);

    /**
     * @brief 删除实现分发（模板内部函数）
     * @tparam T 目标数据类型
     * @param name 待删除目标名称
     * @details 通过 if constexpr 在编译期分发到对应删除函数。
     */
    template <typename T>
    void deleteDataImpl(const QString &name)
    {
        if constexpr (std::is_same_v<T, FirepowerItem>)
        {
            deleteFirepowerDataByName(name);
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

#endif // FIREPOWERCONTROL_H
