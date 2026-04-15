//
// Created by admin on "2026.04.10 T 19:19:20".
//

#include "SpectrumAnalysis.h"
#include "ui_SpectrumAnalysis.h"

#include <QFrame>
#include <QFont>
#include <QMouseEvent>
#include <QPainter>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QStandardItemModel>
#include <QToolTip>

#include <limits>
#include <utility>
#include <vector>

// ==============================================================================
// SpectrumChart 类实现
// ==============================================================================

namespace
{
    constexpr qreal kXOffset = 20.0;
    constexpr qreal kYOffset = 20.0;
    constexpr qreal kWidthMargin = 40.0;
    constexpr qreal kHeightMargin = 110.0;
    constexpr qreal kBarHeightRatio = 0.7;
    constexpr qreal kArrowSize = 8.0;
    constexpr qreal kBandLabelOffset = 25.0;
    constexpr qreal kFreqLabelOffset = 25.0;
    constexpr int kBandLabelFontSize = 10;
    constexpr int kFreqLabelFontSize = 9;
}

SpectrumChart::SpectrumChart(QWidget *parent)
    : QGraphicsView(parent)
{
    m_scene = new QGraphicsScene(this);
    setScene(m_scene);
    setRenderHint(QPainter::Antialiasing);
    setAlignment(Qt::AlignLeft | Qt::AlignBottom);
    setMouseTracking(true);

    m_hoveredIndex = -1;
    m_tooltipItem = nullptr;
    m_tooltipBg = nullptr;


    initScene();
}

SpectrumChart::~SpectrumChart()
{
    clearScene();
    delete m_scene;
}

void SpectrumChart::initScene()
{
    clearScene();
    m_scene->setBackgroundBrush(QColor(240, 240, 240));
}

void SpectrumChart::clearScene()
{
    m_scene->clear();
    m_hoveredIndex = -1;
    m_tooltipItem = nullptr;
    m_tooltipBg = nullptr;
}

void SpectrumChart::setData(const QVector<RadarSource> &radarSource,
                            const QVector<RadioSource> &radioSource,
                            const QVector<RadarJammerSource> &radarJammerSource,
                            const QVector<RadioJammerSource> &radioJammerSource,
                            const QColor &radarColor,
                            const QColor &radioColor,
                            const QColor &radarJammerColor,
                            const QColor &radioJammerColor)
{
    m_radarSource = radarSource;
    m_radioSource = radioSource;
    m_radarJammerSource = radarJammerSource;
    m_radioJammerSource = radioJammerSource;
    m_radarColor = radarColor;
    m_radioColor = radioColor;
    m_radarJammerColor = radarJammerColor;
    m_radioJammerColor = radioJammerColor;
    drawBarChart();
}

void SpectrumChart::setData(const QVector<RadarSource> &radarSource,
                             const QVector<RadioSource> &radioSource,
                             const QVector<RadarJammerSource> &radarJammerSource,
                             const QVector<RadioJammerSource> &radioJammerSource)
{
    setData(radarSource, radioSource, radarJammerSource, radioJammerSource,
            QColor(Qt::green), QColor(Qt::green), QColor(Qt::green), QColor(Qt::green));
}

QPair<double, double> SpectrumChart::calculateFrequencyRange(const QString &frequencyStr)
{
    // 处理常见波段名称
    if (frequencyStr.contains(QStringLiteral("Ku波段")))
    {
        return qMakePair(12000.0, 18000.0);
    }
    else if (frequencyStr.contains(QStringLiteral("S/C波段")))
    {
        return qMakePair(2000.0, 4000.0);
    }
    else if (frequencyStr.contains(QStringLiteral("S波段")))
    {
        return qMakePair(2000.0, 3500.0);
    }
    else if (frequencyStr.contains(QStringLiteral("C波段")))
    {
        return qMakePair(3500.0, 6500.0);
    }
    else if (frequencyStr.contains(QStringLiteral("X波段")))
    {
        return qMakePair(8000.0, 12000.0);
    }

    // 处理频率范围格式，如 "5.2~6.1GHz", "150~170MHz"
    QRegularExpression regex(QStringLiteral("([0-9.]+)\\s*~\\s*([0-9.]+)\\s*(GHz|MHz)"));
    QRegularExpressionMatch match = regex.match(frequencyStr);
    if (match.hasMatch())
    {
        double start = match.captured(1).toDouble();
        double end = match.captured(2).toDouble();
        QString unit = match.captured(3);

        if (unit == QStringLiteral("GHz"))
        {
            start *= 1000;
            end *= 1000;
        }
        return qMakePair(start, end);
    }

    // 默认范围
    return qMakePair(0.0, 100.0);
}

void SpectrumChart::drawBarChart()
{
    clearScene();

    // 计算总数据量，无数据时直接返回
    int totalCount = m_radarSource.size() + m_radioSource.size()
                     + m_radarJammerSource.size() + m_radioJammerSource.size();
    if (totalCount == 0)
    {
        return;
    }

    // 收集所有频率范围（暂不合并）
    QVector<FrequencyRangeInfo> allRangeInfos;

    // ---------- 1. 雷达数据 (Radar) ----------
    for (const auto &radar : m_radarSource)
    {
        FrequencyRangeInfo info;
        info.frequencyStr = radar.frequency;
        info.range = calculateFrequencyRange(radar.frequency);
        info.color = m_radarColor;
        info.signalCount = 1;
        allRangeInfos.append(info);
    }

    // ---------- 2. 通信电台数据 (Communication) ----------
    for (const auto &radio : m_radioSource)
    {
        FrequencyRangeInfo info;
        info.frequencyStr = radio.frequency;
        info.range = calculateFrequencyRange(radio.frequency);
        info.color = m_radioColor;
        info.signalCount = 1;
        allRangeInfos.append(info);
    }

    // ---------- 3. 雷达对抗设备 (Radar Jammer) ----------
    for (const auto &radarJammer : m_radarJammerSource)
    {
        FrequencyRangeInfo info;
        info.frequencyStr = radarJammer.workingBand;
        info.range = calculateFrequencyRange(radarJammer.workingBand);
        info.color = m_radarJammerColor;
        info.signalCount = 1;
        allRangeInfos.append(info);
    }

    // ---------- 4. 通信对抗设备 (Comm Jammer) ----------
    for (const auto &radioJammer : m_radioJammerSource)
    {
        FrequencyRangeInfo info;
        info.frequencyStr = radioJammer.coverageBand;
        info.range = calculateFrequencyRange(radioJammer.coverageBand);
        info.color = m_radioJammerColor;
        info.signalCount = 1;
        allRangeInfos.append(info);
    }

    // ---------- 5. 频率范围合并：重叠时保留较大范围，累加信号数量 ----------
    QVector<FrequencyRangeInfo> mergedInfos;
    for (const auto &info : allRangeInfos)
    {
        bool merged = false;
        for (auto &existing : mergedInfos)
        {
            // 检查是否重叠或包含
            const bool overlaps = !(info.range.second < existing.range.first || info.range.first > existing.range.second);
            if (overlaps)
            {
                // 合并为更大的范围
                existing.range.first = qMin(existing.range.first, info.range.first);
                existing.range.second = qMax(existing.range.second, info.range.second);
                // 累加信号数量
                existing.signalCount += info.signalCount;
                merged = true;
                break;
            }
        }
        if (!merged)
        {
            mergedInfos.append(info);
        }
    }

    // 计算频率范围的最小值和最大值（使用合并后的数据）
    double minFreq = std::numeric_limits<double>::max();
    double maxFreq = std::numeric_limits<double>::min();
    for (const auto &info : mergedInfos)
    {
        minFreq = qMin(minFreq, info.range.first);
        maxFreq = qMax(maxFreq, info.range.second);
    }

    // 确保有有效的频率范围
    if (minFreq >= maxFreq)
    {
        minFreq = 0.0;
        maxFreq = 1000.0;
    }

    // 图表布局参数（底部增加更多空间以显示频段标签和频率标签）
    qreal chartWidth = width() - kWidthMargin;
    qreal chartHeight = height() - kHeightMargin;
    qreal barHeight = chartHeight * kBarHeightRatio;
    qreal xOffset = kXOffset;
    qreal yOffset = kYOffset;

    // 计算频率到像素的缩放比例
    double freqRange = maxFreq - minFreq;
    double scaleFactor = chartWidth / freqRange;

    // 绘制所有柱状图（使用合并后的数据）
    for (auto &info : mergedInfos)
    {
        qreal barStart = xOffset + (info.range.first - minFreq) * scaleFactor;
        qreal barWidth = (info.range.second - info.range.first) * scaleFactor;

        QRectF barRect(barStart, yOffset + (chartHeight - barHeight), barWidth, barHeight);
        QPen barPen(info.color, 2);
        QGraphicsRectItem *barItem = m_scene->addRect(barRect, barPen, QBrush(info.color));
        barItem->setOpacity(0.7);

        // 保存矩形区域，用于鼠标悬浮检测
        info.rect = barRect;
    }

    // 存储合并后的频率范围信息，用于鼠标悬浮检测
    m_rangeInfos = mergedInfos;

    // 绘制X轴
    QPen axisPen(Qt::black, 1);
    m_scene->addLine(xOffset, yOffset + chartHeight,
                     xOffset + chartWidth, yOffset + chartHeight, axisPen);

    // 绘制X轴末尾向右的三角形箭头
    qreal arrowSize = kArrowSize;
    QPolygonF arrow;
    arrow << QPointF(xOffset + chartWidth, yOffset + chartHeight)
          << QPointF(xOffset + chartWidth - arrowSize, yOffset + chartHeight - arrowSize / 2)
          << QPointF(xOffset + chartWidth - arrowSize, yOffset + chartHeight + arrowSize / 2);
    m_scene->addPolygon(arrow, axisPen, QBrush(Qt::black));

    // 绘制频段标签（在X轴下方）
    qreal bandLabelY = yOffset + chartHeight + 25;
    QFont bandLabelFont;
    bandLabelFont.setPointSize(10);

    // 绘制具体频率刻度标签（在频段标签下方）
    qreal freqLabelY = bandLabelY + 25;
    QFont freqLabelFont;
    freqLabelFont.setPointSize(9);

    // 频率刻度表（按从小到大排列）
    static const QList<double> freqTicks = {0, 30, 300, 1000, 2000, 4000, 8000, 12000, 18000, 26500, 40000};

    // 绘制每个频率刻度和标签
    for (double freq : freqTicks)
    {
        // 检查频率是否在图表范围内
        if (freq < minFreq || freq > maxFreq)
        {
            continue;
        }

        // 计算频率在图表中的位置
        double tickX = xOffset + (freq - minFreq) * scaleFactor;

        // 绘制频率标签
        QString labelText;
        if (freq >= 1000)
        {
            labelText = QStringLiteral("%1").arg(freq / 1000.0, 0, 'f', 0) + QStringLiteral("G");
        }
        else
        {
            labelText = QStringLiteral("%1").arg(freq, 0, 'f', 0) + QStringLiteral("M");
        }

        QGraphicsTextItem *textItem = m_scene->addText(labelText, freqLabelFont);
        textItem->setDefaultTextColor(Qt::black);
        qreal textWidth = textItem->boundingRect().width();
        qreal textX = tickX - textWidth / 2;
        textItem->setPos(textX, freqLabelY - textItem->boundingRect().height() / 2);
    }
}

QString SpectrumChart::getBandName(double freq)
{
    // 频率阈值 + 频段名称（按从小到大排列）
    static const QList<std::pair<double, QString>> bandMap = {
        {30,    QStringLiteral("HF频段")},
        {300,   QStringLiteral("VHF频段")},
        {1000,  QStringLiteral("UHF频段")},
        {2000,  QStringLiteral("L频段")},
        {4000,  QStringLiteral("S频段")},
        {8000,  QStringLiteral("C频段")},
        {12000, QStringLiteral("X频段")},
        {18000, QStringLiteral("Ku频段")},
        {26500, QStringLiteral("K频段")},
        {40000, QStringLiteral("Ka频段")}
    };

    // 匹配频段
    for (const auto &pair : bandMap)
    {
        if (freq < pair.first)
        {
            return pair.second;
        }
    }

    // 超过40000 返回毫米波频段
    return QStringLiteral("毫米波频段");
}

void SpectrumChart::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);

    // 转换鼠标坐标到图表坐标系
    QPointF scenePos = mapToScene(event->pos());

    // 图表布局参数
    qreal chartWidth = width() - 40;
    qreal chartHeight = height() - 110;
    qreal xOffset = 20;
    qreal yOffset = 20;

    // 计算频率范围的最小值和最大值
    double minFreq = std::numeric_limits<double>::max();
    double maxFreq = std::numeric_limits<double>::min();
    for (const auto &info : m_rangeInfos)
    {
        minFreq = qMin(minFreq, info.range.first);
        maxFreq = qMax(maxFreq, info.range.second);
    }
    if (minFreq >= maxFreq)
    {
        minFreq = 0.0;
        maxFreq = 1000.0;
    }

    // 计算频率到像素的缩放比例
    double freqRange = maxFreq - minFreq;
    double scaleFactor = chartWidth / freqRange;

    // 计算鼠标位置对应的频率
    double mouseFreq = minFreq + (scenePos.x() - xOffset) / scaleFactor;

    // 找到包含当前鼠标频率的频率范围（合并后的）
    for (const auto &info : m_rangeInfos)
    {
        if (mouseFreq >= info.range.first && mouseFreq <= info.range.second)
        {
            // 构建频率范围显示文本
            QString freqDisplay = QStringLiteral("%1~%2MHz").arg(info.range.first).arg(info.range.second);

            // 确定频段名称
            double centerFreq = (info.range.first + info.range.second) / 2;
            QString bandName = getBandName(centerFreq);

            // 显示悬浮提示（包含信号数量）
            QString tooltip = QString("%1\n频率范围: %2\n信号数量: %3")
                                  .arg(bandName).arg(freqDisplay).arg(info.signalCount);
            QToolTip::showText(event->globalPos(), tooltip, this);
            return;
        }
    }

    // 鼠标不在任何频率范围上，隐藏提示
    QToolTip::hideText();
}

void SpectrumChart::leaveEvent(QEvent *event)
{
    QGraphicsView::leaveEvent(event);
    QToolTip::hideText();
}

// ==============================================================================
// SpectrumAnalysis 类实现
// ==============================================================================

SpectrumAnalysis::SpectrumAnalysis(QWidget *parent)
    : QWidget(parent), ui(new Ui::SpectrumAnalysis)
{
    ui->setupUi(this);

    // 初始化参数
    initPara();

    // 初始化对象
    initClass();

    // 关联信号与槽函数
    signalAndSlot();
}

SpectrumAnalysis::~SpectrumAnalysis()
{
    delete ui;
}

void SpectrumAnalysis::initPara()
{
}

void SpectrumAnalysis::initClass()
{
    // 初始化颜色
    initTableViewAttr();

    // 初始化数据模型
    initDataModel();

    // 生成测试数据
    generateTestData();

    // 显示频谱源数据
    displayData();
}

void SpectrumAnalysis::signalAndSlot()
{
}

void SpectrumAnalysis::generateTestData()
{
    // 清空容器，避免重复追加
    m_radarSource.clear();
    m_radioSource.clear();
    m_radarJammerSource.clear();
    m_RadioJammerSource.clear();

    // ---------- 1. 雷达数据 (Radar) ----------
    RadarSource radar1;
    radar1.frequency = QStringLiteral("5.2~6.1GHz");
    m_radarSource.append(radar1);

    RadarSource radar2;
    radar2.frequency = QStringLiteral("150~170MHz");
    m_radarSource.append(radar2);

    RadarSource radar3;
    radar3.frequency = QStringLiteral("8~12GHz");
    m_radarSource.append(radar3);
}

void SpectrumAnalysis::initTableViewAttr()
{
    // 初始化默认颜色（全部绿色）
    m_radarColor = QColor(Qt::blue);
    m_radioColor = QColor(Qt::blue);
    m_radarJammerColor = QColor(Qt::blue);
    m_radioJammerColor = QColor(Qt::blue);

    // 初始化频谱图表
    m_spectrumChart = new SpectrumChart(this);
    m_spectrumChart->setGeometry(10, 10, width() - 20, height() - 20);
}

void SpectrumAnalysis::initDataModel()
{
}

void SpectrumAnalysis::displayData()
{
    // 刷新频谱图表
    refreshChart();
}

// ==============================================================================
// 缓存数据管理
// ==============================================================================

void SpectrumAnalysis::addData(const RadarSource &data)
{
    m_radarSource.append(data);
    refreshChart();
}

void SpectrumAnalysis::addData(const RadarSource &data, const QColor &color)
{
    m_radarColor = color;
    m_radarSource.append(data);
    refreshChart();
}

void SpectrumAnalysis::addData(const RadioSource &data)
{
    m_radioSource.append(data);
    refreshChart();
}

void SpectrumAnalysis::addData(const RadioSource &data, const QColor &color)
{
    m_radioColor = color;
    m_radioSource.append(data);
    refreshChart();
}

void SpectrumAnalysis::addData(const RadarJammerSource &data)
{
    m_radarJammerSource.append(data);
    refreshChart();
}

void SpectrumAnalysis::addData(const RadarJammerSource &data, const QColor &color)
{
    m_radarJammerColor = color;
    m_radarJammerSource.append(data);
    refreshChart();
}

void SpectrumAnalysis::addData(const RadioJammerSource &data)
{
    m_RadioJammerSource.append(data);
    refreshChart();
}

void SpectrumAnalysis::addData(const RadioJammerSource &data, const QColor &color)
{
    m_radioJammerColor = color;
    m_RadioJammerSource.append(data);
    refreshChart();
}

void SpectrumAnalysis::updateData(const RadarSource &data)
{
    int index = findIndexByName(m_radarSource, data.name);
    if (index >= 0)
    {
        m_radarSource[index] = data;
        refreshChart();
    }
}

void SpectrumAnalysis::updateData(const RadarSource &data, const QColor &color)
{
    m_radarColor = color;
    int index = findIndexByName(m_radarSource, data.name);
    if (index >= 0)
    {
        m_radarSource[index] = data;
        refreshChart();
    }
}

void SpectrumAnalysis::updateData(const RadioSource &data)
{
    int index = findIndexByName(m_radioSource, data.name);
    if (index >= 0)
    {
        m_radioSource[index] = data;
        refreshChart();
    }
}

void SpectrumAnalysis::updateData(const RadioSource &data, const QColor &color)
{
    m_radioColor = color;
    int index = findIndexByName(m_radioSource, data.name);
    if (index >= 0)
    {
        m_radioSource[index] = data;
        refreshChart();
    }
}

void SpectrumAnalysis::updateData(const RadarJammerSource &data)
{
    int index = findIndexByName(m_radarJammerSource, data.name);
    if (index >= 0)
    {
        m_radarJammerSource[index] = data;
        refreshChart();
    }
}

void SpectrumAnalysis::updateData(const RadarJammerSource &data, const QColor &color)
{
    m_radarJammerColor = color;
    int index = findIndexByName(m_radarJammerSource, data.name);
    if (index >= 0)
    {
        m_radarJammerSource[index] = data;
        refreshChart();
    }
}

void SpectrumAnalysis::updateData(const RadioJammerSource &data)
{
    int index = findIndexByName(m_RadioJammerSource, data.name);
    if (index >= 0)
    {
        m_RadioJammerSource[index] = data;
        refreshChart();
    }
}

void SpectrumAnalysis::updateData(const RadioJammerSource &data, const QColor &color)
{
    m_radioJammerColor = color;
    int index = findIndexByName(m_RadioJammerSource, data.name);
    if (index >= 0)
    {
        m_RadioJammerSource[index] = data;
        refreshChart();
    }
}

void SpectrumAnalysis::clearCacheData()
{
    // 清空所有缓存数据
    m_radarSource.clear();
    m_radioSource.clear();
    m_radarJammerSource.clear();
    m_RadioJammerSource.clear();

    // 刷新频谱图表
    refreshChart();
}

void SpectrumAnalysis::refreshChart()
{
    // 从缓存数据刷新频谱图表
    if (m_spectrumChart)
    {
        m_spectrumChart->setData(m_radarSource, m_radioSource, m_radarJammerSource, m_RadioJammerSource,
                                 m_radarColor, m_radioColor, m_radarJammerColor, m_radioJammerColor);
    }

}

// ==============================================================================
// 事件处理
// ==============================================================================

void SpectrumAnalysis::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    // 调整频谱图表尺寸
    if (m_spectrumChart)
    {
        m_spectrumChart->setGeometry(10, 10, width() - 20, height() - 20);
    }
}

void SpectrumAnalysis::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);
}

void SpectrumAnalysis::leaveEvent(QEvent *event)
{
    QWidget::leaveEvent(event);
}
