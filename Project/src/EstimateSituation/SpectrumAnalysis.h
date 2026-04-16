//
// Created by admin on "2026.04.10 T 19:19:20".
//

#ifndef SPECTRUMANALYSIS_H
#define SPECTRUMANALYSIS_H

#include "EstimateSituationStruct.h"

#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QObject>
#include <QVector>
#include <QWidget>
#include <type_traits>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class SpectrumAnalysis;
}
QT_END_NAMESPACE

// 频谱分析图表
class SpectrumChart : public QGraphicsView
{
    Q_OBJECT

public:
    explicit SpectrumChart(QWidget *parent = nullptr);
    ~SpectrumChart() override;

    // 设置辐射源数据并重绘柱状图
    void setData(const QVector<RadarSource> &radarSource,
                 const QVector<RadioSource> &radioSource,
                 const QVector<RadarJammerSource> &radarJammerSource,
                 const QVector<RadioJammerSource> &radioJammerSource,
                 const QColor &radarColor = QColor(Qt::green),
                 const QColor &radioColor = QColor(Qt::green),
                 const QColor &radarJammerColor = QColor(Qt::green),
                 const QColor &radioJammerColor = QColor(Qt::green));

    // 设置辐射源数据并重绘柱状图（默认颜色版本）
    void setData(const QVector<RadarSource> &radarSource,
                 const QVector<RadioSource> &radioSource,
                 const QVector<RadarJammerSource> &radarJammerSource,
                 const QVector<RadioJammerSource> &radioJammerSource);

    // 绘制柱状图
    void drawBarChart();

    // 计算频率范围的起始和结束值（MHz）
    QPair<double, double> calculateFrequencyRange(const QString &frequencyStr);

private:
    QGraphicsScene *m_scene;
    int m_hoveredIndex;
    QGraphicsTextItem *m_tooltipItem;
    QGraphicsRectItem *m_tooltipBg;

    // 辐射源数据缓存
    QVector<RadarSource> m_radarSource;
    QVector<RadioSource> m_radioSource;
    QVector<RadarJammerSource> m_radarJammerSource;
    QVector<RadioJammerSource> m_radioJammerSource;

    // 频率范围信息
    QVector<FrequencyRangeInfo> m_rangeInfos;

    // 颜色配置
    QColor m_radarColor;
    QColor m_radioColor;
    QColor m_radarJammerColor;
    QColor m_radioJammerColor;

    // 初始化场景
    void initScene();
    // 清理场景
    void clearScene();

    // 根据频率获取频段名称
    QString getBandName(double freq);

    // 鼠标移动事件
    void mouseMoveEvent(QMouseEvent *event) override;
    // 鼠标离开事件
    void leaveEvent(QEvent *event) override;
};

// 频谱分析窗口
class SpectrumAnalysis : public QWidget
{
    Q_OBJECT

public:
    explicit SpectrumAnalysis(QWidget *parent = nullptr);
    ~SpectrumAnalysis() override;

    // 添加雷达辐射源数据
    void addData(const RadarSource &data);
    void addData(const RadarSource &data, const QColor &color);

    // 添加电台辐射源数据
    void addData(const RadioSource &data);
    void addData(const RadioSource &data, const QColor &color);

    // 添加雷达干扰辐射源数据
    void addData(const RadarJammerSource &data);
    void addData(const RadarJammerSource &data, const QColor &color);

    // 添加通信干扰辐射源数据
    void addData(const RadioJammerSource &data);
    void addData(const RadioJammerSource &data, const QColor &color);

    // 添加雷达性能参数数据
    void addData(const RadarPerformancePara &data);
    void addData(const RadarPerformancePara &data, const QColor &color);

    // 更新雷达辐射源数据（按 name 匹配）
    void updateData(const RadarSource &data);
    void updateData(const RadarSource &data, const QColor &color);

    // 更新电台辐射源数据（按 name 匹配）
    void updateData(const RadioSource &data);
    void updateData(const RadioSource &data, const QColor &color);

    // 更新雷达干扰辐射源数据（按 name 匹配）
    void updateData(const RadarJammerSource &data);
    void updateData(const RadarJammerSource &data, const QColor &color);

    // 更新通信干扰辐射源数据（按 name 匹配）
    void updateData(const RadioJammerSource &data);
    void updateData(const RadioJammerSource &data, const QColor &color);

    // 更新雷达性能参数数据（按 name 匹配）
    void updateData(const RadarPerformancePara &data);
    void updateData(const RadarPerformancePara &data, const QColor &color);

    // 删除指定类型和名称的辐射源数据
    template <typename T>
    void deleteData(const QString &name)
    {
        if constexpr (std::is_same_v<T, RadarSource>)
        {
            int index = findIndexByName(m_radarSource, name);
            if (index >= 0)
            {
                m_radarSource.removeAt(index);
                refreshChart();
            }
        }
        else if constexpr (std::is_same_v<T, RadioSource>)
        {
            int index = findIndexByName(m_radioSource, name);
            if (index >= 0)
            {
                m_radioSource.removeAt(index);
                refreshChart();
            }
        }
        else if constexpr (std::is_same_v<T, RadarJammerSource>)
        {
            int index = findIndexByName(m_radarJammerSource, name);
            if (index >= 0)
            {
                m_radarJammerSource.removeAt(index);
                refreshChart();
            }
        }
        else if constexpr (std::is_same_v<T, RadioJammerSource>)
        {
            int index = findIndexByName(m_RadioJammerSource, name);
            if (index >= 0)
            {
                m_RadioJammerSource.removeAt(index);
                refreshChart();
            }
        }
    }

    // 清空缓存数据
    void clearCacheData();

    // 刷新频谱图表
    void refreshChart();

protected:
    // 窗口大小改变事件
    void resizeEvent(QResizeEvent *event) override;
    // 鼠标移动事件
    void mouseMoveEvent(QMouseEvent *event) override;
    // 鼠标离开事件
    void leaveEvent(QEvent *event) override;

private:
    // 初始化参数
    void initPara();
    // 初始化对象
    void initClass();
    // 关联信号与槽函数
    void signalAndSlot();

    // 生成测试数据
    void generateTestData();
    // 初始化表格属性
    void initTableViewAttr();
    // 初始化数据模型
    void initDataModel();
    // 显示数据
    void displayData();

    // 根据名称查找索引
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

private:
    Ui::SpectrumAnalysis *ui;
    SpectrumChart *m_spectrumChart;

    // 辐射源数据缓存
    QVector<RadarSource> m_radarSource;
    QVector<RadioSource> m_radioSource;
    QVector<RadarJammerSource> m_radarJammerSource;
    QVector<RadioJammerSource> m_RadioJammerSource;

    // 颜色配置
    QColor m_radarColor;
    QColor m_radioColor;
    QColor m_radarJammerColor;
    QColor m_radioJammerColor;
};

#endif // SPECTRUMANALYSIS_H
