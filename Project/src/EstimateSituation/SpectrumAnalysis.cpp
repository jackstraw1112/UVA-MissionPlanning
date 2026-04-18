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
#include <QtMath>

#include <algorithm>
#include <limits>
#include <utility>
#include <vector>

namespace
{
    constexpr qreal kXOffset = 15.0;
    constexpr qreal kYOffset = 15.0;
    constexpr qreal kWidthMargin = 30.0;
    constexpr qreal kHeightMargin = 90.0;
    constexpr qreal kBarHeightRatio = 0.7;
    constexpr qreal kArrowSize = 6.0;
    constexpr qreal kBandLabelOffset = 20.0;
    constexpr qreal kFreqLabelOffset = 20.0;
    constexpr int kBandLabelFontSize = 8;
    constexpr int kFreqLabelFontSize = 7;
}

/**
 * @brief 频谱图表控件构造函数
 * @param parent 父窗口指针
 * @details 初始化频谱图表场景、渲染提示和鼠标追踪
 */
SpectrumChart::SpectrumChart(QWidget *parent)
    : QGraphicsView(parent)
{
    m_scene = new QGraphicsScene(this);
    setScene(m_scene);
    setRenderHint(QPainter::Antialiasing);
    setAlignment(Qt::AlignLeft | Qt::AlignBottom);
    setMouseTracking(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setDragMode(QGraphicsView::NoDrag);

    m_hoveredIndex = -1;
    m_tooltipItem = nullptr;
    m_tooltipBg = nullptr;

    m_bandLabelFont.setPointSize(kBandLabelFontSize);
    m_freqLabelFont.setPointSize(kFreqLabelFontSize);

    initScene();
}

/**
 * @brief 频谱图表控件析构函数
 * @details 清理场景资源并删除场景对象
 */
SpectrumChart::~SpectrumChart()
{
    clearScene();
    delete m_scene;
}

/**
 * @brief 初始化频谱图表场景
 * @details 清空现有场景并设置背景色
 */
void SpectrumChart::initScene()
{
    clearScene();
    m_scene->setBackgroundBrush(QColor(240, 240, 240));
}

/**
 * @brief 清空频谱图表场景
 * @details 清空场景内容并重置悬停状态和提示框
 */
void SpectrumChart::clearScene()
{
    m_scene->clear();
    m_hoveredIndex = -1;
    m_tooltipItem = nullptr;
    m_tooltipBg = nullptr;
}

/**
 * @brief 设置频谱图表数据
 * @param radarSource 雷达性能参数列表
 * @param radarColor 雷达颜色
 * @details 保存数据并绘制柱状图
 */
void SpectrumChart::setData(const QVector<RadarPerformancePara> &radarSource, const QColor &radarColor)
{
    m_radarSource = radarSource;
    m_radarColor = radarColor;
    drawBarChart();
}

/**
 * @brief 计算频率范围
 * @param frequencyStr 频率字符串（支持波段名称或数值范围）
 * @return 频率范围（MHz）
 * @details 支持波段名称映射（Ku/S/C/X波段）和正则匹配数值范围
 */
QPair<double, double> SpectrumChart::calculateFrequencyRange(const QString &frequencyStr)
{
    // 1. 波段名称 → 频率范围（MHz）【统一配置，一目了然】
    static const QVector<std::pair<QString, QPair<double, double>>> bandMap = {
        {QStringLiteral("Ku波段"),  {12000.0, 18000.0}},
        {QStringLiteral("S/C波段"),  {2000.0,  4000.0}},
        {QStringLiteral("S波段"),    {2000.0,  3500.0}},
        {QStringLiteral("C波段"),    {3500.0,  6500.0}},
        {QStringLiteral("X波段"),    {8000.0, 12000.0}},
    };

    // 2. 匹配波段名称
    for (const auto &band : bandMap) {
        if (frequencyStr.contains(band.first)) {
            return band.second;
        }
    }

    // 3. 正则匹配格式：2 ~ 4 GHz 或 2000 ~ 4000 MHz
    static const QRegularExpression regex(R"(([0-9.]+)\s*~\s*([0-9.]+)\s*(GHz|MHz))");
    const QRegularExpressionMatch match = regex.match(frequencyStr);

    if (match.hasMatch()) {
        bool okStart, okEnd;
        double start = match.captured(1).toDouble(&okStart);
        double end   = match.captured(2).toDouble(&okEnd);
        QString unit = match.captured(3);

        // 增加安全判断，防止转换失败
        if (!okStart || !okEnd || start > end) {
            return {0.0, 100.0};
        }

        // 单位转换 GHz → MHz
        if (unit == QStringLiteral("GHz")) {
            start *= 1000;
            end *= 1000;
        }

        return {start, end};
    }

    // 4. 默认值
    return {0.0, 100.0};
}
/*
QPair<double, double> SpectrumChart::calculateFrequencyRange(const QString &frequencyStr)
{
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

    return qMakePair(0.0, 100.0);
}
*/

/**
 * @brief 绘制频谱柱状图
 * @details 合并重叠频率范围并绘制双坐标轴频谱图
 */
void SpectrumChart::drawBarChart()
{
    clearScene();

    if (m_radarSource.isEmpty())
    {
        return;
    }

    QVector<RadarPerformancePara> allRangeInfos;

    for (const auto &radar : m_radarSource)
    {
        QString freqStr = QString("%1~%2GHz").arg(radar.freqMin).arg(radar.freqMax);
        RadarPerformancePara info;
        info.frequencyStr = freqStr;
        info.freqRange = calculateFrequencyRange(freqStr);
        info.barColor = m_radarColor;
        info.signalCount = 1;
        allRangeInfos.append(info);
    }

    QVector<RadarPerformancePara> mergedInfos;
    // 合并重叠的频率范围，只显示一个包含所有重叠范围的柱状图
    for (const auto &info : allRangeInfos)
    {
        bool merged = false;
        for (auto &existing : mergedInfos)
        {
            const bool overlaps = !(info.freqRange.second < existing.freqRange.first || info.freqRange.first > existing.freqRange.second);
            if (overlaps)
            {
                existing.freqRange.first = qMin(existing.freqRange.first, info.freqRange.first);
                existing.freqRange.second = qMax(existing.freqRange.second, info.freqRange.second);
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

    // 设置固定频率范围
    m_minFreq = 0.0;
    m_maxFreq = 20000.0; // 20 GHz

    Q_ASSERT(m_minFreq < m_maxFreq);

    qreal chartWidth = width() - kWidthMargin;
    qreal chartHeight = height() - kHeightMargin;
    qreal halfChartHeight = chartHeight / 2;
    qreal barHeight = halfChartHeight * kBarHeightRatio;
    qreal xOffset = kXOffset;
    qreal yOffset = kYOffset;

    double freqRange = m_maxFreq - m_minFreq;
    double scaleFactor = chartWidth / freqRange;

    // 绘制中间部分的柱状图（0~2000MHz）
    qreal middleBarY = yOffset + (halfChartHeight - barHeight);
    for (auto &info : mergedInfos)
    {
        if (info.freqRange.first < 2000) // 频率范围与0~2000MHz有交集
        {
            // 计算中间部分的起始和结束频率
            double startFreq = info.freqRange.first;
            double endFreq = qMin(info.freqRange.second, 2000.0);

            // 根据中间坐标轴刻度计算柱状图位置
            double middleChartWidth = chartWidth;
            double middleScaleFactor = middleChartWidth / 2000.0; // 0~2000MHz对应整个中间坐标轴宽度

            qreal barStart = xOffset + (startFreq) * middleScaleFactor;
            qreal barEnd = xOffset + (endFreq) * middleScaleFactor;
            qreal barWidth = barEnd - barStart;

            if (barWidth > 0)
            {
                QRectF barRect(barStart, middleBarY, barWidth, barHeight);
                QPen barPen(Qt::NoPen);  // 移除边框
                QGraphicsRectItem *barItem = m_scene->addRect(barRect, barPen, QBrush(info.barColor));
                barItem->setOpacity(0.7);  // 调整透明度，确保重叠时颜色不加深

                info.barRect = barRect;
            }
        }
    }

    // 绘制下面部分的柱状图（2000~20000MHz）
    qreal bottomBarY = yOffset + halfChartHeight + (halfChartHeight - barHeight);
    for (auto &info : mergedInfos)
    {
        if (info.freqRange.second > 2000) // 频率范围与2000~20000MHz有交集
        {
            // 计算底部部分的起始和结束频率
            double startFreq = qMax(info.freqRange.first, 2000.0);
            double endFreq = info.freqRange.second;

            // 根据底部坐标轴刻度计算柱状图位置
            double bottomChartWidth = chartWidth;
            double bottomScaleFactor = bottomChartWidth / (20000.0 - 2000.0); // 2000~20000MHz对应整个底部坐标轴宽度

            qreal barStart = xOffset + (startFreq - 2000.0) * bottomScaleFactor;
            qreal barEnd = xOffset + (endFreq - 2000.0) * bottomScaleFactor;
            qreal barWidth = barEnd - barStart;

            if (barWidth > 0)
            {
                QRectF barRect(barStart, bottomBarY, barWidth, barHeight);
                QPen barPen(Qt::NoPen);  // 移除边框
                QGraphicsRectItem *barItem = m_scene->addRect(barRect, barPen, QBrush(info.barColor));
                barItem->setOpacity(0.7);  // 调整透明度，确保重叠时颜色不加深

                info.barRect = barRect;
            }
        }
    }

    m_rangeInfos = mergedInfos;

    QPen axisPen(Qt::black, 1);

    // 绘制中间坐标轴（适应窗口大小，居中对齐）
    qreal middleAxisY = yOffset + halfChartHeight - barHeight / 2;
    m_scene->addLine(xOffset, middleAxisY, xOffset + chartWidth, middleAxisY, axisPen);

    // 绘制下面的坐标轴
    qreal bottomAxisY = yOffset + chartHeight;
    m_scene->addLine(xOffset, bottomAxisY, xOffset + chartWidth, bottomAxisY, axisPen);

    // 绘制下面坐标轴的箭头
    qreal arrowSize = kArrowSize;
    QPolygonF bottomArrow;
    bottomArrow << QPointF(xOffset + chartWidth, bottomAxisY)
                << QPointF(xOffset + chartWidth - arrowSize, bottomAxisY - arrowSize / 2)
                << QPointF(xOffset + chartWidth - arrowSize, bottomAxisY + arrowSize / 2);
    m_scene->addPolygon(bottomArrow, axisPen, QBrush(Qt::black));

    // 中间坐标轴的刻度（从中间坐标轴开始到结束均匀分配，5个标签）
    int middleTickCount = 5; // 5个刻度点，包括起点和终点
    double middleTickInterval = chartWidth / (middleTickCount - 1);
    for (int i = 0; i < middleTickCount; ++i)
    {
        double tickX = xOffset + i * middleTickInterval;

        // 计算对应的频率值（0~2000 MHz）
        double freq = (tickX - xOffset) * (2000.0 / chartWidth);

        // 绘制中间坐标轴刻度线
        QPen tickPen(Qt::black, 1);
        m_scene->addLine(tickX, middleAxisY, tickX, middleAxisY + 5, tickPen);

        // 中间坐标轴标签
        QString labelText = QStringLiteral("%1").arg(freq, 0, 'f', 0);
        QGraphicsTextItem *middleTextItem = m_scene->addText(labelText, m_freqLabelFont);
        middleTextItem->setDefaultTextColor(Qt::black);
        qreal textWidth = middleTextItem->boundingRect().width();
        qreal textX = tickX - textWidth / 2;
        qreal middleLabelY = middleAxisY + 15;
        middleTextItem->setPos(textX, middleLabelY - middleTextItem->boundingRect().height() / 2);
    }

    // 底部坐标轴的刻度（2000~20000 MHz，均匀分配）
    int bottomTickCount = 5; // 5个刻度点，包括起点和终点，与中间坐标轴一致
    double bottomTickInterval = chartWidth / (bottomTickCount - 1);
    for (int i = 0; i < bottomTickCount; ++i)
    {
        double tickX = xOffset + i * bottomTickInterval;

        // 计算对应的频率值（2000~20000 MHz）
        double freq = 2000.0 + (tickX - xOffset) * ((20000.0 - 2000.0) / chartWidth);

        // 绘制下边坐标轴刻度线
        QPen tickPen(Qt::black, 1);
        m_scene->addLine(tickX, bottomAxisY, tickX, bottomAxisY - 5, tickPen);

        // 下边坐标轴标签
        QString labelText = QStringLiteral("%1").arg(freq / 1000.0, 0, 'f', 0);
        QGraphicsTextItem *bottomTextItem = m_scene->addText(labelText, m_freqLabelFont);
        bottomTextItem->setDefaultTextColor(Qt::black);
        qreal textWidth = bottomTextItem->boundingRect().width();
        qreal textX = tickX - textWidth / 2;
        qreal bottomLabelY = bottomAxisY + 15;
        bottomTextItem->setPos(textX, bottomLabelY - bottomTextItem->boundingRect().height() / 2);
    }
}

/**
 * @brief 根据频率获取频段名称
 * @param freq 频率（MHz）
 * @return 频段名称
 * @details 根据频率阈值返回对应频段名称
 */
QString SpectrumChart::getBandName(double freq)
{
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

    for (const auto &pair : bandMap)
    {
        if (freq < pair.first)
        {
            return pair.second;
        }
    }

    return QStringLiteral("毫米波频段");
}

/**
 * @brief 鼠标移动事件处理
 * @param event 鼠标事件
 * @details 根据鼠标位置计算频率并显示提示框
 */
void SpectrumChart::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);

    QPointF scenePos = mapToScene(event->pos());

    qreal chartWidth = width() - kWidthMargin;
    qreal chartHeight = height() - kHeightMargin;
    qreal halfChartHeight = chartHeight / 2;
    qreal barHeight = halfChartHeight * kBarHeightRatio;
    qreal xOffset = kXOffset;
    qreal yOffset = kYOffset;

    qreal middleAxisY = yOffset + halfChartHeight - barHeight / 2;
    qreal bottomAxisY = yOffset + chartHeight;

    double mouseFreq = -1.0;

    if (scenePos.x() >= xOffset && scenePos.x() <= xOffset + chartWidth) {
        if (scenePos.y() >= yOffset && scenePos.y() <= middleAxisY) {
            double middleScaleFactor = chartWidth / 2000.0;
            mouseFreq = (scenePos.x() - xOffset) / middleScaleFactor;
        } else if (scenePos.y() > middleAxisY && scenePos.y() <= bottomAxisY) {
            double bottomScaleFactor = chartWidth / (20000.0 - 2000.0);
            mouseFreq = 2000.0 + (scenePos.x() - xOffset) / bottomScaleFactor;
        }
    }

    if (mouseFreq < 0.0) {
        QToolTip::hideText();
        return;
    }

    QVector<int> coveringIndices;
    for (int i = 0; i < m_radarSource.size(); ++i) {
        double fMin = m_radarSource[i].freqMin * 1000.0;
        double fMax = m_radarSource[i].freqMax * 1000.0;
        if (mouseFreq >= fMin && mouseFreq <= fMax) {
            coveringIndices.append(i);
        }
    }

    if (coveringIndices.isEmpty()) {
        QToolTip::hideText();
        return;
    }

    QVector<double> boundaries;
    for (int idx : coveringIndices) {
        boundaries.append(m_radarSource[idx].freqMin * 1000.0);
        boundaries.append(m_radarSource[idx].freqMax * 1000.0);
    }
    std::sort(boundaries.begin(), boundaries.end());
    boundaries.erase(std::unique(boundaries.begin(), boundaries.end()), boundaries.end());

    double segStart = boundaries.first();
    double segEnd = boundaries.last();
    for (int i = 0; i < boundaries.size() - 1; ++i) {
        if (mouseFreq >= boundaries[i] && mouseFreq <= boundaries[i + 1]) {
            segStart = boundaries[i];
            segEnd = boundaries[i + 1];
            break;
        }
    }

    int signalCount = 0;
    for (int idx : coveringIndices) {
        double fMin = m_radarSource[idx].freqMin * 1000.0;
        double fMax = m_radarSource[idx].freqMax * 1000.0;
        if (fMin <= segStart && fMax >= segEnd) {
            ++signalCount;
        }
    }

    auto formatFreq = [](double mhz) -> QString {
        if (mhz >= 1000.0) {
            return QStringLiteral("%1GHz").arg(mhz / 1000.0, 0, 'f', 1);
        }
        return QStringLiteral("%1MHz").arg(mhz, 0, 'f', 0);
    };

    QString freqDisplay = formatFreq(segStart) + QStringLiteral("~") + formatFreq(segEnd);
    double centerFreq = (segStart + segEnd) / 2;
    QString bandName = getBandName(centerFreq);

    QString tooltip = QString("%1\n").arg(bandName);
    tooltip += QString("频率范围: %1\n").arg(freqDisplay);
    tooltip += QString("信号数量: %1").arg(signalCount);

    QToolTip::showText(event->globalPos(), tooltip, this);
}

/**
 * @brief 鼠标离开事件处理
 * @param event 事件对象
 * @details 隐藏提示框
 */
void SpectrumChart::leaveEvent(QEvent *event)
{
    QGraphicsView::leaveEvent(event);
    QToolTip::hideText();
}

/**
 * @brief 窗口大小改变事件处理
 * @param event 窗口大小事件
 * @details 窗口大小变化时重新绘制图表
 */
void SpectrumChart::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    // 窗口大小变化时重新绘制图表
    if (!m_radarSource.isEmpty())
    {
        drawBarChart();
    }
}

/**
 * @brief 设置字体
 * @param font 字体对象
 * @details 设置频谱图表标签字体大小
 */
void SpectrumChart::setFont(const QFont &font)
{
    QGraphicsView::setFont(font);
    m_bandLabelFont = font;
    m_bandLabelFont.setPointSize(10);
    m_freqLabelFont = font;
    m_freqLabelFont.setPointSize(9);
}

// ==============================================================================
// SpectrumAnalysis 类实现
// ==============================================================================

/**
 * @brief 频谱分析窗口构造函数
 * @param parent 父窗口指针
 * @details 初始化UI、参数、对象和信号槽连接
 */
SpectrumAnalysis::SpectrumAnalysis(QWidget *parent)
    : QWidget(parent), ui(new Ui::SpectrumAnalysis)
{
    ui->setupUi(this);
    initPara();
    initClass();
    signalAndSlot();
}

/**
 * @brief 频谱分析窗口析构函数
 * @details 释放UI资源
 */
SpectrumAnalysis::~SpectrumAnalysis()
{
    delete ui;
}

/**
 * @brief 初始化参数
 * @details 预留扩展
 */
void SpectrumAnalysis::initPara()
{
}

/**
 * @brief 初始化对象
 * @details 初始化表格视图属性、数据模型和测试数据
 */
void SpectrumAnalysis::initClass()
{
    initTableViewAttr();
    initDataModel();
    generateTestData();
    displayData();
}

/**
 * @brief 关联信号与槽函数
 * @details 预留扩展
 */
void SpectrumAnalysis::signalAndSlot()
{
}

/**
 * @brief 生成测试数据
 * @details 初始化雷达辐射源测试数据
 */
void SpectrumAnalysis::generateTestData()
{
    m_radarSource.clear();

    const auto addRadar = [this](const QString &name, const QString &type, int presetIndex, const QString &scanMode)
    {
        RadarPerformancePara radar;
        radar = ProjectPublicInterface::radarInputFromPresetIndex(presetIndex);
        radar.name = name;
        radar.deviceType = type;
        radar.scanMode = scanMode;
        m_radarSource.append(radar);
    };

    addRadar(QStringLiteral("AN/MPQ-53 相控阵雷达"), QStringLiteral("MPQ-53 (PAC-2 火控)"), 2, QStringLiteral("电子扫描"));
    addRadar(QStringLiteral("爱国者 MPQ-65"), QStringLiteral("MPQ-65 (PAC-3 火控)"), 1, QStringLiteral("相控阵扫描"));
    addRadar(QStringLiteral("P-18 预警雷达"), QStringLiteral("SPS-48E"), 5, QStringLiteral("6rpm"));
    addRadar(QStringLiteral("MPQ-64 哨兵雷达"), QStringLiteral("TPS-75"), 7, QStringLiteral("旋转扫描"));
    addRadar(QStringLiteral("远程预警 FPS-117"), QStringLiteral("FPS-117"), 6, QStringLiteral("机械扫描"));
}

/**
 * @brief 初始化表格视图属性
 * @details 设置频谱图表颜色和几何位置
 */
void SpectrumAnalysis::initTableViewAttr()
{
    m_radarColor = QColor(Qt::blue);

    m_spectrumChart = new SpectrumChart(this);
    m_spectrumChart->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->gridLayout->addWidget(m_spectrumChart);
}

/**
 * @brief 初始化数据模型
 * @details 预留扩展
 */
void SpectrumAnalysis::initDataModel()
{
}

/**
 * @brief 显示数据
 * @details 刷新频谱图表
 */
void SpectrumAnalysis::displayData()
{
    refreshChart();
}

/**
 * @brief 添加雷达数据
 * @param data 雷达性能参数
 * @details 添加数据到列表并刷新图表
 */
void SpectrumAnalysis::addData(const RadarPerformancePara &data)
{
    m_radarSource.append(data);
    refreshChart();
}

/**
 * @brief 添加雷达数据（带颜色）
 * @param data 雷达性能参数
 * @param color 雷达颜色
 * @details 设置颜色并添加数据到列表刷新图表
 */
void SpectrumAnalysis::addData(const RadarPerformancePara &data, const QColor &color)
{
    m_radarColor = color;
    m_radarSource.append(data);
    refreshChart();
}

/**
 * @brief 更新雷达数据
 * @param data 雷达性能参数
 * @details 根据雷达名称查找并更新数据
 */
void SpectrumAnalysis::updateData(const RadarPerformancePara &data)
{
    int index = findIndexByName(m_radarSource, data.name);
    if (index >= 0)
    {
        m_radarSource[index] = data;
        refreshChart();
    }
}

/**
 * @brief 更新雷达数据（带颜色）
 * @param data 雷达性能参数
 * @param color 雷达颜色
 * @details 设置颜色并更新数据
 */
void SpectrumAnalysis::updateData(const RadarPerformancePara &data, const QColor &color)
{
    m_radarColor = color;
    int index = findIndexByName(m_radarSource, data.name);
    if (index >= 0)
    {
        m_radarSource[index] = data;
        refreshChart();
    }
}

/**
 * @brief 删除雷达数据
 * @param name 雷达名称
 * @details 根据名称删除数据
 */
void SpectrumAnalysis::deleteData(const QString &name)
{
    int index = findIndexByName(m_radarSource, name);
    if (index >= 0)
    {
        m_radarSource.removeAt(index);
        refreshChart();
    }
}

/**
 * @brief 清空缓存数据
 * @details 清空雷达数据列表并刷新图表
 */
void SpectrumAnalysis::clearCacheData()
{
    m_radarSource.clear();
    refreshChart();
}

/**
 * @brief 刷新图表
 * @details 更新频谱图表数据
 */
void SpectrumAnalysis::refreshChart()
{
    if (m_spectrumChart)
    {
        m_spectrumChart->setData(m_radarSource, m_radarColor);
    }
}

/**
 * @brief 窗口大小改变事件处理
 * @param event 窗口大小事件
 * @details 调整频谱图表大小
 */
void SpectrumAnalysis::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    if (m_spectrumChart && !m_radarSource.isEmpty())
    {
        m_spectrumChart->drawBarChart();
    }
}

/**
 * @brief 鼠标移动事件处理
 * @param event 鼠标事件
 * @details 预留扩展
 */
void SpectrumAnalysis::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);
}

/**
 * @brief 鼠标离开事件处理
 * @param event 事件对象
 * @details 预留扩展
 */
void SpectrumAnalysis::leaveEvent(QEvent *event)
{
    QWidget::leaveEvent(event);
}

/**
 * @brief 设置字体
 * @param font 字体对象
 * @details 设置窗口字体并同步到频谱图表
 */
void SpectrumAnalysis::setFont(const QFont &font)
{
    QWidget::setFont(font);
    if (m_spectrumChart) {
        m_spectrumChart->setFont(font);
    }
}

/**
 * @brief 雷达数据变更处理槽函数
 * @param data 雷达性能参数
 * @details 根据雷达名称查找并更新或添加数据
 */
void SpectrumAnalysis::onRadarDataChanged(const RadarPerformancePara &data)
{
    int index = findIndexByName(m_radarSource, data.name);
    if (index >= 0) {
        updateData(data);
    } else {
        addData(data);
    }
}

void SpectrumAnalysis::onRadarDataDeleted(const QString &name)
{
    int index = findIndexByName(m_radarSource, name);
    if (index >= 0) {
        m_radarSource.removeAt(index);
        refreshChart();
    }
}
