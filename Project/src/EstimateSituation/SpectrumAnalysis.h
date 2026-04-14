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
#include <QObject>
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
 * @details 基于 QGraphicsView 绘制频谱分析柱状图，显示不同频段的频率范围和信号数量。
 *          支持鼠标悬停显示频段详细信息（频段名称、频率范围、信号数量）。
 *          频谱范围覆盖 HF/VHF/UHF/L/S/C/X/Ku/K/Ka 等频段。
 */
class SpectrumChart : public QGraphicsView
{
    Q_OBJECT

public:
    explicit SpectrumChart(QWidget *parent = nullptr);
    ~SpectrumChart() override;

    /**
     * @brief 设置辐射源数据并重绘柱状图
     * @param radarSource 雷达辐射源数据
     * @param radioSource 电台辐射源数据
     * @param radarJammerSource 雷达干扰辐射源数据
     * @param radioJammerSource 通信干扰辐射源数据
     * @param radarColor 雷达辐射源颜色
     * @param radioColor 电台辐射源颜色
     * @param radarJammerColor 雷达干扰辐射源颜色
     * @param radioJammerColor 通信干扰辐射源颜色
     */
    void setData(const QVector<RadarSource> &radarSource,
                 const QVector<RadioSource> &radioSource,
                 const QVector<RadarJammerSource> &radarJammerSource,
                 const QVector<RadioJammerSource> &radioJammerSource,
                 const QColor &radarColor = QColor(Qt::red),
                 const QColor &radioColor = QColor(Qt::red),
                 const QColor &radarJammerColor = QColor(Qt::red),
                 const QColor &radioJammerColor = QColor(Qt::red));

    /**
     * @brief 设置辐射源数据并重绘柱状图（默认颜色版本）
     * @param radarSource 雷达辐射源数据
     * @param radioSource 电台辐射源数据
     * @param radarJammerSource 雷达干扰辐射源数据
     * @param radioJammerSource 通信干扰辐射源数据
     */
    void setData(const QVector<RadarSource> &radarSource,
                 const QVector<RadioSource> &radioSource,
                 const QVector<RadarJammerSource> &radarJammerSource,
                 const QVector<RadioJammerSource> &radioJammerSource);

    /**
     * @brief 绘制柱状图
     * @details 收集所有辐射源的频率范围，按频率比例绘制红色半透明柱状图，
     *          柱状图宽度由频率带宽决定，支持重叠显示。
     */
    void drawBarChart();

    /**
     * @brief 计算频率范围的起始和结束值（MHz）
     * @param frequencyStr 频率字符串，如 "5.2~6.1GHz"、"150~170MHz" 或 "Ku波段"
     * @return 频率范围的起始和结束值（MHz），解析失败返回 (0, 100)
     */
    QPair<double, double> calculateFrequencyRange(const QString &frequencyStr);

    /**
     * @brief 计算频率带宽（MHz）
     * @param frequencyStr 频率字符串
     * @return 带宽值（MHz）
     */
    double calculateBandwidth(const QString &frequencyStr);

private:
    // 图形场景
    QGraphicsScene *m_scene;                // 图形场景
    int m_hoveredIndex;                     // 当前悬停的频段索引，-1 表示无悬停
    QGraphicsTextItem *m_tooltipItem;       // 工具提示文本
    QGraphicsRectItem *m_tooltipBg;         // 工具提示背景

    // 辐射源数据缓存
    QVector<RadarSource> m_radarSource;              // 雷达辐射源数据
    QVector<RadioSource> m_radioSource;              // 电台辐射源数据
    QVector<RadarJammerSource> m_radarJammerSource;  // 雷达干扰辐射源数据
    QVector<RadioJammerSource> m_radioJammerSource;  // 通信干扰辐射源数据

    // 频率范围信息，用于鼠标悬浮检测
    QVector<FrequencyRangeInfo> m_rangeInfos;

    double m_totalRange;                    // 总频率范围（MHz），用于按比例分配频段宽度

    // 颜色配置
    QColor m_radarColor;            // 雷达辐射源颜色
    QColor m_radioColor;                   // 电台辐射源颜色
    QColor m_radarJammerColor;             // 雷达干扰辐射源颜色
    QColor m_radioJammerColor;             // 通信干扰辐射源颜色

    /**
     * @brief 初始化场景
     * @details 清空场景并设置浅灰色背景。
     */
    void initScene();

    /**
     * @brief 清理场景
     * @details 清空频谱数据、移除所有图形项并重置悬停状态。
     */
    void clearScene();

    /**
     * @brief 计算重叠数量
     * @param targetRange 目标频率范围
     * @return 与目标范围存在重叠的频率范围数量
     */
    int calculateOverlapCount(const QPair<double, double> &targetRange);

    /**
     * @brief 计算频率范围的最小值和最大值
     * @return 频率范围的最小值和最大值（MHz），无数据时返回 (0, 1000)
     */
    QPair<double, double> getFrequencyRangeMinMax();

    /**
     * @brief 根据频率获取频段名称
     * @param freq 频率值（MHz）
     * @return 频段名称，如 "HF频段"、"VHF频段"、"Ku频段" 等
     * @details 频段划分标准：
     *          HF  < 30MHz,  VHF < 300MHz, UHF < 1000MHz,
     *          L   < 2000MHz, S   < 4000MHz, C   < 8000MHz,
     *          X   < 12000MHz, Ku  < 18000MHz, K  < 26500MHz,
     *          Ka  < 40000MHz, 其余为毫米波频段
     */
    QString getBandName(double freq);

    /**
     * @brief 构建频率范围显示文本
     * @param frequencyStr 频率字符串，如 "5.2~6.1GHz" 或 "Ku波段"
     * @return 用于悬浮提示的频率范围文本
     * @details 波段名称自动转换为具体频率范围，数值型频率范围直接返回原字符串。
     */
    QString buildFrequencyDisplay(const QString &frequencyStr);

    /**
     * @brief 鼠标移动事件
     * @param event 鼠标事件
     * @details 根据鼠标位置换算对应频率，查找包含该频率的所有频率范围，
     *          选取最窄（最具体）的范围作为目标，显示悬浮提示。
     */
    void mouseMoveEvent(QMouseEvent *event) override;

    /**
     * @brief 鼠标离开事件
     * @param event 事件对象
     * @details 鼠标离开图表区域时隐藏悬浮提示。
     */
    void leaveEvent(QEvent *event) override;
};

/**
 * @brief 频谱分析窗口
 * @details 封装频谱分析图表，提供辐射源数据的增删改接口，
 *          数据变更后自动同步更新频谱图表显示。
 */
class SpectrumAnalysis : public QWidget
{
    Q_OBJECT

public:
    explicit SpectrumAnalysis(QWidget *parent = nullptr);
    ~SpectrumAnalysis() override;

    /**
     * @brief 添加雷达辐射源数据
     * @param data 雷达辐射源对象
     * @details 追加到雷达缓存并刷新频谱图表。
     */
    void addData(const RadarSource &data);

    /**
     * @brief 添加雷达辐射源数据
     * @param data 雷达辐射源对象
     * @param color 雷达辐射源颜色
     * @details 追加到雷达缓存并刷新频谱图表。
     */
    void addData(const RadarSource &data, const QColor &color);

    /**
     * @brief 添加电台辐射源数据
     * @param data 电台辐射源对象
     * @details 追加到电台缓存并刷新频谱图表。
     */
    void addData(const RadioSource &data);

    /**
     * @brief 添加电台辐射源数据
     * @param data 电台辐射源对象
     * @param color 电台辐射源颜色
     * @details 追加到电台缓存并刷新频谱图表。
     */
    void addData(const RadioSource &data, const QColor &color);

    /**
     * @brief 添加雷达干扰辐射源数据
     * @param data 雷达干扰辐射源对象
     * @details 追加到雷达干扰缓存并刷新频谱图表。
     */
    void addData(const RadarJammerSource &data);

    /**
     * @brief 添加雷达干扰辐射源数据
     * @param data 雷达干扰辐射源对象
     * @param color 雷达干扰辐射源颜色
     * @details 追加到雷达干扰缓存并刷新频谱图表。
     */
    void addData(const RadarJammerSource &data, const QColor &color);

    /**
     * @brief 添加通信干扰辐射源数据
     * @param data 通信干扰辐射源对象
     * @details 追加到通信干扰缓存并刷新频谱图表。
     */
    void addData(const RadioJammerSource &data);

    /**
     * @brief 添加通信干扰辐射源数据
     * @param data 通信干扰辐射源对象
     * @param color 通信干扰辐射源颜色
     * @details 追加到通信干扰缓存并刷新频谱图表。
     */
    void addData(const RadioJammerSource &data, const QColor &color);

    /**
     * @brief 更新雷达辐射源数据
     * @param data 雷达辐射源对象（按 name 匹配）
     * @details 若未找到同名记录则不操作。
     */
    void updateData(const RadarSource &data);

    /**
     * @brief 更新雷达辐射源数据
     * @param data 雷达辐射源对象（按 name 匹配）
     * @param color 雷达辐射源颜色
     * @details 若未找到同名记录则不操作。
     */
    void updateData(const RadarSource &data, const QColor &color);

    /**
     * @brief 更新电台辐射源数据
     * @param data 电台辐射源对象（按 name 匹配）
     * @details 若未找到同名记录则不操作。
     */
    void updateData(const RadioSource &data);

    /**
     * @brief 更新电台辐射源数据
     * @param data 电台辐射源对象（按 name 匹配）
     * @param color 电台辐射源颜色
     * @details 若未找到同名记录则不操作。
     */
    void updateData(const RadioSource &data, const QColor &color);

    /**
     * @brief 更新雷达干扰辐射源数据
     * @param data 雷达干扰辐射源对象（按 name 匹配）
     * @details 若未找到同名记录则不操作。
     */
    void updateData(const RadarJammerSource &data);

    /**
     * @brief 更新雷达干扰辐射源数据
     * @param data 雷达干扰辐射源对象（按 name 匹配）
     * @param color 雷达干扰辐射源颜色
     * @details 若未找到同名记录则不操作。
     */
    void updateData(const RadarJammerSource &data, const QColor &color);

    /**
     * @brief 更新通信干扰辐射源数据
     * @param data 通信干扰辐射源对象（按 name 匹配）
     * @details 若未找到同名记录则不操作。
     */
    void updateData(const RadioJammerSource &data);

    /**
     * @brief 更新通信干扰辐射源数据
     * @param data 通信干扰辐射源对象（按 name 匹配）
     * @param color 通信干扰辐射源颜色
     * @details 若未找到同名记录则不操作。
     */
    void updateData(const RadioJammerSource &data, const QColor &color);

    /**
     * @brief 删除指定类型和名称的辐射源数据
     * @tparam T 数据类型（RadarSource / RadioSource / RadarJammerSource / RadioJammerSource）
     * @param name 目标名称
     * @details 根据类型在对应容器中查找并删除，删除后刷新频谱图表。
     */
    template <typename T>
    void deleteData(const QString &name);

protected:
    /**
     * @brief 窗口大小改变事件
     * @param event 大小改变事件
     * @details 调整频谱图表尺寸并重绘。
     */
    void resizeEvent(QResizeEvent *event) override;

    /**
     * @brief 鼠标移动事件
     * @param event 鼠标事件
     */
    void mouseMoveEvent(QMouseEvent *event) override;

    /**
     * @brief 鼠标离开事件
     * @param event 事件对象
     */
    void leaveEvent(QEvent *event) override;

private:
    // 初始化参数（预留扩展）
    void initParams();
    // 初始化对象（数据、视图、模型）
    void initObject();
    // 关联信号与槽函数
    void initConnect();

    /**
     * @brief 生成测试数据
     * @details 生成雷达、电台、雷达对抗、通信对抗四类辐射源测试数据，
     *          生成前先清空容器，避免重复追加。
     */
    void generateTestData();

    /**
     * @brief 初始化表格属性
     * @details 设置表格属性，如列数、列宽、行高、表头、数据样式等
     */
    void initTableViewAttr();

    /**
     * @brief 初始化数据模型
     * @details 预留扩展
     */
    void initDataModel();

    /**
     * @brief 显示数据
     * @details 预留扩展
     */
    void displayData();

private:
    Ui::SpectrumAnalysis *ui;               // UI对象
    SpectrumChart *m_spectrumChart;          // 频谱图表

    // 辐射源数据缓存
    QVector<RadarSource> m_radarSource;             // 雷达辐射源数据
    QVector<RadioSource> m_radioSource;             // 电台辐射源数据
    QVector<RadarJammerSource> m_radarJammerSource; // 雷达干扰辐射源数据
    QVector<RadioJammerSource> m_RadioJammerSource; // 通信干扰辐射源数据

    // 颜色配置
    QColor m_radarColor;                   // 雷达辐射源颜色
    QColor m_radioColor;                   // 电台辐射源颜色
    QColor m_radarJammerColor;             // 雷达干扰辐射源颜色
    QColor m_radioJammerColor;             // 通信干扰辐射源颜色

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
