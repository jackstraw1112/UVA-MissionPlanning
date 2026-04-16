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

#include <limits>
#include <utility>
#include <vector>

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

void SpectrumChart::setData(const QVector<RadarPerformancePara> &radarSource, const QColor &radarColor)
{
    m_radarSource = radarSource;
    m_radarColor = radarColor;
    drawBarChart();
}

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

    // 动态计算最大频率范围，确保频谱图不重叠
    m_maxFreq = 0.0;
    for (const auto &info : mergedInfos)
    {
        if (info.freqRange.second > m_maxFreq)
        {
            m_maxFreq = info.freqRange.second;
        }
    }
    // 确保最小频率为 0，最大频率至少为 1000 MHz
    m_minFreq = 0.0;
    m_maxFreq = qMax(m_maxFreq, 1000.0);
    // 向上取整到最近的 1000 MHz
    m_maxFreq = qCeil(m_maxFreq / 1000.0) * 1000.0;
    
    Q_ASSERT(m_minFreq < m_maxFreq);

    qreal chartWidth = width() - kWidthMargin;
    qreal chartHeight = height() - kHeightMargin;
    qreal barHeight = chartHeight * kBarHeightRatio;
    qreal xOffset = kXOffset;
    qreal yOffset = kYOffset;

    double freqRange = m_maxFreq - m_minFreq;
    double scaleFactor = chartWidth / freqRange;

    for (auto &info : mergedInfos)
    {
        qreal barStart = xOffset + (info.freqRange.first - m_minFreq) * scaleFactor;
        qreal barWidth = (info.freqRange.second - info.freqRange.first) * scaleFactor;

        QRectF barRect(barStart, yOffset + (chartHeight - barHeight), barWidth, barHeight);
        QPen barPen(Qt::NoPen);  // 移除边框
        QGraphicsRectItem *barItem = m_scene->addRect(barRect, barPen, QBrush(info.barColor));
        barItem->setOpacity(0.7);  // 调整透明度，确保重叠时颜色不加深

        info.barRect = barRect;
    }

    m_rangeInfos = mergedInfos;

    QPen axisPen(Qt::black, 1);
    m_scene->addLine(xOffset, yOffset + chartHeight,
                     xOffset + chartWidth, yOffset + chartHeight, axisPen);

    qreal arrowSize = kArrowSize;
    QPolygonF arrow;
    arrow << QPointF(xOffset + chartWidth, yOffset + chartHeight)
          << QPointF(xOffset + chartWidth - arrowSize, yOffset + chartHeight - arrowSize / 2)
          << QPointF(xOffset + chartWidth - arrowSize, yOffset + chartHeight + arrowSize / 2);
    m_scene->addPolygon(arrow, axisPen, QBrush(Qt::black));

    qreal bandLabelY = yOffset + chartHeight + 25;
    QFont bandLabelFont;
    bandLabelFont.setPointSize(10);

    qreal freqLabelY = bandLabelY + 25;
    QFont freqLabelFont;
    freqLabelFont.setPointSize(9);

    // 动态生成刻度标签（优化版）

    // 标准频率刻度点（按业务常用值）
    QList<double> freqTicks;
    int tickCount = 6; // 想要几个刻度
    double step = m_maxFreq / (tickCount - 1);

    for (int i = 0; i < tickCount; ++i) {
        freqTicks << i * step;
    }
    for (double freq : freqTicks)
    {
        if (freq < m_minFreq || freq > m_maxFreq)
        {
            continue;
        }

        double tickX = xOffset + (freq - m_minFreq) * scaleFactor;

        // 绘制刻度线
        QPen tickPen(Qt::black, 1);
        m_scene->addLine(tickX, yOffset + chartHeight, tickX, yOffset + chartHeight + 5, tickPen);

        QString labelText;
        if (freq >= 1000)
        {
            labelText = QStringLiteral("%1G").arg(freq / 1000.0, 0, 'f', 0);
        }
        else
        {
            labelText = QStringLiteral("%1").arg(freq, 0, 'f', 0);
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

void SpectrumChart::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);

    QPointF scenePos = mapToScene(event->pos());

    qreal chartWidth = width() - 40;
    qreal chartHeight = height() - 110;
    qreal xOffset = 20;
    qreal yOffset = 20;

    double freqRange = m_maxFreq - m_minFreq;
    double scaleFactor = chartWidth / freqRange;

    double mouseFreq = m_minFreq + (scenePos.x() - xOffset) / scaleFactor;

    // 查找所有与当前鼠标位置重叠的雷达数据
    QVector<RadarPerformancePara> overlappingRadars;
    for (const auto &radar : m_radarSource)
    {
        double radarFreqMin = radar.freqMin * 1000; // 转换为 MHz
        double radarFreqMax = radar.freqMax * 1000;
        if (mouseFreq >= radarFreqMin && mouseFreq <= radarFreqMax)
        {
            overlappingRadars.append(radar);
        }
    }

    if (!overlappingRadars.isEmpty())
    {
        if (overlappingRadars.size() == 1)
        {
            // 只有一个雷达，显示该雷达的信息
            const auto &radar = overlappingRadars[0];
            double radarFreqMin = radar.freqMin * 1000;
            double radarFreqMax = radar.freqMax * 1000;
            QString freqDisplay = QStringLiteral("%1~%2MHz").arg(radarFreqMin).arg(radarFreqMax);

            double centerFreq = (radarFreqMin + radarFreqMax) / 2;
            QString bandName = getBandName(centerFreq);

            QString tooltip = QString("%1\n频率范围: %2\n信号数量: 1").arg(bandName).arg(freqDisplay);
            
            QToolTip::showText(event->globalPos(), tooltip, this);
            return;
        }
        else
        {
            // 多个雷达重叠，计算重叠范围
            double overlapMin = overlappingRadars[0].freqMin * 1000;
            double overlapMax = overlappingRadars[0].freqMax * 1000;
            for (const auto &radar : overlappingRadars)
            {
                double radarFreqMin = radar.freqMin * 1000;
                double radarFreqMax = radar.freqMax * 1000;
                overlapMin = qMax(overlapMin, radarFreqMin);
                overlapMax = qMin(overlapMax, radarFreqMax);
            }

            QString freqDisplay = QStringLiteral("%1~%2MHz").arg(overlapMin).arg(overlapMax);

            double centerFreq = (overlapMin + overlapMax) / 2;
            QString bandName = getBandName(centerFreq);

            QString tooltip = QString("%1\n重叠频率范围: %2\n信号数量: %3").arg(bandName).arg(freqDisplay).arg(overlappingRadars.size());
            
            QToolTip::showText(event->globalPos(), tooltip, this);
            return;
        }
    }

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
    initPara();
    initClass();
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
    initTableViewAttr();
    initDataModel();
    generateTestData();
    displayData();
}

void SpectrumAnalysis::signalAndSlot()
{
}

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

void SpectrumAnalysis::initTableViewAttr()
{
    m_radarColor = QColor(Qt::blue);

    m_spectrumChart = new SpectrumChart(this);
    m_spectrumChart->setGeometry(10, 10, width() - 20, height() - 20);
}

void SpectrumAnalysis::initDataModel()
{
}

void SpectrumAnalysis::displayData()
{
    refreshChart();
}

void SpectrumAnalysis::addData(const RadarPerformancePara &data)
{
    m_radarSource.append(data);
    refreshChart();
}

void SpectrumAnalysis::addData(const RadarPerformancePara &data, const QColor &color)
{
    m_radarColor = color;
    m_radarSource.append(data);
    refreshChart();
}

void SpectrumAnalysis::updateData(const RadarPerformancePara &data)
{
    int index = findIndexByName(m_radarSource, data.name);
    if (index >= 0)
    {
        m_radarSource[index] = data;
        refreshChart();
    }
}

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

void SpectrumAnalysis::deleteData(const QString &name)
{
    int index = findIndexByName(m_radarSource, name);
    if (index >= 0)
    {
        m_radarSource.removeAt(index);
        refreshChart();
    }
}

void SpectrumAnalysis::clearCacheData()
{
    m_radarSource.clear();
    refreshChart();
}

void SpectrumAnalysis::refreshChart()
{
    if (m_spectrumChart)
    {
        m_spectrumChart->setData(m_radarSource, m_radarColor);
    }
}

void SpectrumAnalysis::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

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
