//
// Created by admin on "2026.04.09 T 19:19:20".
//

#ifndef SITUATIONCONTROL_H
#define SITUATIONCONTROL_H

#include <QWidget>
#include <QVector>
#include <type_traits>
#include "AnimatedSwitch.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class SituationControl;
}
QT_END_NAMESPACE

/**
 * @brief 态势控制数据结构体
 * @details 存储态势控制相关的配置数据
 */
struct SituationControlData {
    QString type;       // 控制类型（radar/radio/commJam/radarJam/defenseFire）
    bool enabled;       // 是否启用
    QString description; // 描述信息

    /**
     * @brief 默认构造函数
     */
    SituationControlData() = default;

    /**
     * @brief 构造函数
     * @param t 控制类型
     * @param e 是否启用
     * @param desc 描述信息
     */
    SituationControlData(const QString &t, bool e, const QString &desc)
        : type(t), enabled(e), description(desc) {}
};

/**
 * @brief 态势控制窗口
 * @details 管理态势显示相关的控制选项，如辐射源威力范围显示、防控火力显示等
 */
class SituationControl : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口指针
     */
    explicit SituationControl(QWidget *parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~SituationControl() override;

private:
    // 初始化参数（预留扩展）
    void initParams();
    // 初始化对象（数据、视图）
    void initObject();
    // 关联信号与槽函数
    void initConnect();

public:
    /**
     * @brief 添加态势控制数据（模板接口）
     * @tparam T 数据类型：SituationControlData
     * @param data 待添加的数据对象
     * @details 调用后会同步更新缓存。
     */
    template <typename T>
    void addData(const T &data)
    {
        addDataImpl(data);
    }

    /**
     * @brief 更新态势控制数据（模板接口）
     * @tparam T 数据类型：SituationControlData
     * @param data 待更新的数据对象（按 type 匹配）
     * @details 若未找到同类型数据，则按新增处理。
     */
    template <typename T>
    void updateData(const T &data)
    {
        updateDataImpl(data);
    }

    /**
     * @brief 删除态势控制数据（模板接口）
     * @tparam T 数据类型：SituationControlData
     * @param type 待删除目标类型
     * @details 调用后会同步删除缓存中的对应项。
     */
    template <typename T>
    void deleteData(const QString &type)
    {
        deleteDataImpl<T>(type);
    }

signals:
    /**
     * @brief 控制状态变更信号
     * @param type 控制类型
     * @param enabled 是否启用
     */
    void controlStateChanged(const QString &type, bool enabled);

private:
    Ui::SituationControl *ui; ///< UI对象指针
    QVector<SituationControlData> m_controlData; ///< 态势控制数据列表

private:
    // 类型化增删改实现：由模板公共接口分发调用
    // --- Add ---
    void addDataImpl(const SituationControlData &data);

    // --- Update ---
    void updateDataImpl(const SituationControlData &data);

    // --- Delete by type ---
    void deleteControlDataByType(const QString &type);

    /**
     * @brief 删除实现分发（模板内部函数）
     * @tparam T 目标数据类型
     * @param type 待删除目标类型
     * @details 通过 if constexpr 在编译期分发到对应删除函数。
     */
    template <typename T>
    void deleteDataImpl(const QString &type)
    {
        if constexpr (std::is_same_v<T, SituationControlData>)
        {
            deleteControlDataByType(type);
        }
        else
        {
            static_assert(std::is_same_v<T, void>, "Unsupported type for deleteData<T>()");
        }
    }

    /**
     * @brief 根据类型查找索引
     * @param container 数据容器
     * @param type 目标类型
     * @return 找到的索引，未找到返回 -1
     */
    int findIndexByType(const QVector<SituationControlData> &container, const QString &type);

private slots:
    /**
     * @brief 雷达开关状态变更
     * @param checked 是否启用
     */
    void onRadarSwitchChanged(bool checked);

    /**
     * @brief 电台开关状态变更
     * @param checked 是否启用
     */
    void onRadioSwitchChanged(bool checked);

    /**
     * @brief 通信对抗开关状态变更
     * @param checked 是否启用
     */
    void onCommJamSwitchChanged(bool checked);

    /**
     * @brief 雷达对抗开关状态变更
     * @param checked 是否启用
     */
    void onRadarJamSwitchChanged(bool checked);

    /**
     * @brief 防控火力显示开关状态变更
     * @param checked 是否启用
     */
    void onDefenseFireDisplaySwitchChanged(bool checked);
};

#endif // SITUATIONCONTROL_H
