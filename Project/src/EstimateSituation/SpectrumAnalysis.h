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

class SpectrumChart : public QGraphicsView
{
    Q_OBJECT

public:
    explicit SpectrumChart(QWidget *parent = nullptr);
    ~SpectrumChart() override;

    void setData(const QVector<RadarPerformancePara> &radarSource, const QColor &radarColor = QColor(Qt::green));

    void drawBarChart();

    QPair<double, double> calculateFrequencyRange(const QString &frequencyStr);

private:
    QGraphicsScene *m_scene;
    int m_hoveredIndex;
    QGraphicsTextItem *m_tooltipItem;
    QGraphicsRectItem *m_tooltipBg;

    QVector<RadarPerformancePara> m_radarSource;

    QVector<RadarPerformancePara> m_rangeInfos;

    // 颜色配置
    QColor m_radarColor;
    
    // 动态频率范围
    double m_minFreq = 0.0;
    double m_maxFreq = 18000.0;

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

    void addData(const RadarPerformancePara &data);
    void addData(const RadarPerformancePara &data, const QColor &color);

    void updateData(const RadarPerformancePara &data);
    void updateData(const RadarPerformancePara &data, const QColor &color);

    void deleteData(const QString &name);

    void clearCacheData();

    void refreshChart();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    void initPara();
    void initClass();
    void signalAndSlot();

    void generateTestData();
    void initTableViewAttr();
    void initDataModel();
    void displayData();

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

public slots:
    void onRadarDataChanged(const RadarPerformancePara &data);
    void onRadarDataDeleted(const QString &name);

signals:
    void radarDataUpdated(const RadarPerformancePara &data);
    void radarDataRemoved(const QString &name);

private:
    Ui::SpectrumAnalysis *ui;
    SpectrumChart *m_spectrumChart;

    QVector<RadarPerformancePara> m_radarSource;

    QColor m_radarColor;
};

#endif // SPECTRUMANALYSIS_H
