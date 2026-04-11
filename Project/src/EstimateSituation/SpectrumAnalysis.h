//
// Created by admin on "2026.04.10 T 19:19:20".
//

#ifndef SPECTRUMANALYSIS_H
#define SPECTRUMANALYSIS_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QVector>
#include <type_traits>
#include "EstimateSituationStruct.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class SpectrumAnalysis;
}
QT_END_NAMESPACE

/**
 * @brief 频谱分析图表
 * @details 绘制频谱分析图表，显示不同频段的能量分布
 */
class SpectrumChart : public QGraphicsView
{
    Q_OBJECT

public:
    explicit SpectrumChart(QWidget *parent = nullptr);
    void updateSpectrumData();
    void generateTestData();
    
    // 设置辐射源数据
    void setRadiationData(const QVector<RadarSource> &radarSource, 
                         const QVector<RadioSource> &radioSource, 
                         const QVector<RadarJammerSource> &radarJammerSource, 
                         const QVector<RadioJammerSource> &radioJammerSource);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    /**
     * @brief 频谱频段结构
     */
    struct SpectrumBand
    {
        QString name;       // 频段名称
        QString range;      // 频率范围
        QColor color;       // 显示颜色
        int height;         // 高度（能量占比）
        int signalCount;    // 信号数量
        QRectF rect;        // 矩形区域
        QGraphicsRectItem *rectItem;  // 矩形项
        QGraphicsTextItem *labelItem; // 标签项
    };

    QGraphicsScene* m_scene;                // 场景
    QVector<SpectrumBand> m_spectrumData;   // 频谱数据
    int m_hoveredIndex;                     // 悬停索引
    QGraphicsTextItem* m_tooltipItem;       // 工具提示文本
    QGraphicsRectItem* m_tooltipBg;         // 工具提示背景
    
    // 辐射源数据
    QVector<RadarSource> m_radarSource;              // 雷达辐射源数据
    QVector<RadioSource> m_radioSource;              // 电台辐射源数据
    QVector<RadarJammerSource> m_radarJammerSource;  // 雷达干扰辐射源数据
    QVector<RadioJammerSource> m_radioJammerSource;  // 通信干扰辐射源数据
    
    double m_totalRange;                    // 总频率范围

    /**
     * @brief 创建频谱条
     */
    void createSpectrumBars();
    /**
     * @brief 更新工具提示
     */
    void updateTooltip();
    
    /**
     * @brief 生成频谱数据
     */
    void generateSpectrumData();
    
    /**
     * @brief 统计各频段的信号数量
     */
    void countSignalsInBands(QVector<SpectrumBand> &bands);
    
    /**
     * @brief 更新频段的信号数量
     */
    void updateBandSignalCount(QVector<SpectrumBand> &bands, const QString &frequencyStr);
};

/**
 * @brief 频谱分析窗口
 * @details 显示频谱分析图表，支持实时频谱数据展示
 */
class SpectrumAnalysis : public QWidget
{
    Q_OBJECT

public:
    explicit SpectrumAnalysis(QWidget *parent = nullptr);
    ~SpectrumAnalysis() override;

public:
    /**
     * @brief 添加辐射源数据（模板接口）
     * @tparam T 辐射源类型：RadarSource / RadioSource / RadarJammerSource / RadioJammerSource
     * @param data 待添加的数据对象
     * @details 调用后会同步更新对应缓存。
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
     * @details 调用后会同步删除对应缓存中的记录。
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
    // 初始化样式表
    void initStyles();
    // 关联信号与槽函数
    void initConnect();

    /**
     * @brief 生成测试数据
     * @details 用户生成临时数据
     */
    void generateTestData();

private:
    Ui::SpectrumAnalysis *ui;               // UI对象
    SpectrumChart* m_spectrumChart;         // 频谱图表

    // 雷达辐射源数据
    QVector<RadarSource> m_radarSource;
    // 电台辐射源数据
    QVector<RadioSource> m_radioSource;
    // 雷达干扰辐射源数据
    QVector<RadarJammerSource> m_radarJammerSource;
    // 通信干扰辐射源数据
    QVector<RadioJammerSource> m_radioJammerSource;

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

#endif // SPECTRUMANALYSIS_H
