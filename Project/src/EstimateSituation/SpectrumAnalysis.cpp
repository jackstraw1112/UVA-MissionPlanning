//
// Created by admin on "2026.04.10 T 19:19:20".
//

// You may need to build the project (run Qt uic code generator) to get "ui_SpectrumAnalysis.h" resolved

#include "SpectrumAnalysis.h"
#include "ui_SpectrumAnalysis.h"
#include <QPainter>
#include <QMouseEvent>
#include <QStandardItemModel>
#include <QFrame>
#include <QFont>
#include <vector>
#include <utility>
#include <QToolTip>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <limits>

// ==============================================================================
// SpectrumChart 类实现
// ==============================================================================

/**
 * @brief 构造函数
 * @param parent 父窗口指针
 * @details 初始化图形场景、渲染参数、鼠标追踪及成员变量。
 */
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
    m_totalRange = 0;

    initScene();
}

/**
 * @brief 析构函数
 * @details 清理场景并释放图形场景资源。
 */
SpectrumChart::~SpectrumChart()
{
    clearScene();
    delete m_scene;
}

/**
 * @brief 初始化场景
 * @details 清空场景并设置浅灰色背景。
 */
void SpectrumChart::initScene()
{
    clearScene();
    m_scene->setBackgroundBrush(QColor(240, 240, 240));
}

/**
 * @brief 清理场景
 * @details 清空频谱数据、移除所有图形项并重置悬停状态。
 */
void SpectrumChart::clearScene()
{
    m_scene->clear();
    m_hoveredIndex = -1;
    m_tooltipItem = nullptr;
    m_tooltipBg = nullptr;
}

/**
 * @brief 设置辐射源数据并重绘柱状图
 * @param radarSource 雷达辐射源数据
 * @param radioSource 电台辐射源数据
 * @param radarJammerSource 雷达干扰辐射源数据
 * @param radioJammerSource 通信干扰辐射源数据
 */
void SpectrumChart::setData(const QVector<RadarSource> &radarSource,
                            const QVector<RadioSource> &radioSource,
                            const QVector<RadarJammerSource> &radarJammerSource,
                            const QVector<RadioJammerSource> &radioJammerSource)
{
    m_radarSource = radarSource;
    m_radioSource = radioSource;
    m_radarJammerSource = radarJammerSource;
    m_radioJammerSource = radioJammerSource;
    drawBarChart();
}

/**
 * @brief 计算频率范围的起始和结束值（MHz）
 * @param frequencyStr 频率字符串，如 "5.2~6.1GHz"、"150~170MHz" 或 "Ku波段"
 * @return 频率范围的起始和结束值（MHz），解析失败返回 (0, 100)
 * @details 优先匹配常见波段名称（Ku/S/C/X波段），再通过正则解析数值范围，
 *          GHz 自动转换为 MHz。
 */
QPair<double, double> SpectrumChart::calculateFrequencyRange(const QString &frequencyStr)
{
    // 处理常见波段名称
    if (frequencyStr.contains("Ku波段"))
    {
        return qMakePair(12000.0, 18000.0);
    }
    else if (frequencyStr.contains("S/C波段"))
    {
        return qMakePair(2000.0, 4000.0);
    }
    else if (frequencyStr.contains("S波段"))
    {
        return qMakePair(2000.0, 3500.0);
    }
    else if (frequencyStr.contains("C波段"))
    {
        return qMakePair(3500.0, 6500.0);
    }
    else if (frequencyStr.contains("X波段"))
    {
        return qMakePair(8000.0, 12000.0);
    }

    // 处理频率范围格式，如 "5.2~6.1GHz", "150~170MHz"
    QRegularExpression regex("([0-9.]+)\\s*~\\s*([0-9.]+)\\s*(GHz|MHz)");
    QRegularExpressionMatch match = regex.match(frequencyStr);
    if (match.hasMatch())
    {
        double start = match.captured(1).toDouble();
        double end = match.captured(2).toDouble();
        QString unit = match.captured(3);

        if (unit == "GHz")
        {
            start *= 1000;
            end *= 1000;
        }
        return qMakePair(start, end);
    }

    // 默认范围
    return qMakePair(0.0, 100.0);
}

/**
 * @brief 计算频率带宽（MHz）
 * @param frequencyStr 频率字符串
 * @return 带宽值（MHz）
 */
double SpectrumChart::calculateBandwidth(const QString &frequencyStr)
{
    QPair<double, double> range = calculateFrequencyRange(frequencyStr);
    return range.second - range.first;
}

/**
 * @brief 绘制柱状图
 * @details 收集所有辐射源的频率范围，按频率比例绘制红色半透明柱状图，
 *          柱状图宽度由频率带宽决定，支持重叠显示。底部保留X轴及刻度线，
 *          不显示Y轴、顶部标签和X轴频率文字。
 */
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

    // 定义统一的红色及边框画笔
    QColor barColor(255, 0, 0);
    QPen barPen(barColor, 2);

    // 收集所有频率范围和对应的频率字符串
    QVector<FrequencyRangeInfo> allRangeInfos;

    // ---------- 1. 雷达数据 (Radar) ----------
    for (const auto &radar : m_radarSource)
    {
        FrequencyRangeInfo info;
        info.frequencyStr = radar.frequency;
        info.range = calculateFrequencyRange(radar.frequency);
        allRangeInfos.append(info);
    }

    // ---------- 2. 通信电台数据 (Communication) ----------
    for (const auto &radio : m_radioSource)
    {
        FrequencyRangeInfo info;
        info.frequencyStr = radio.frequency;
        info.range = calculateFrequencyRange(radio.frequency);
        allRangeInfos.append(info);
    }

    // ---------- 3. 雷达对抗设备 (Radar Jammer) ----------
    for (const auto &radarJammer : m_radarJammerSource)
    {
        FrequencyRangeInfo info;
        info.frequencyStr = radarJammer.workingBand;
        info.range = calculateFrequencyRange(radarJammer.workingBand);
        allRangeInfos.append(info);
    }

    // ---------- 4. 通信对抗设备 (Comm Jammer) ----------
    for (const auto &radioJammer : m_radioJammerSource)
    {
        FrequencyRangeInfo info;
        info.frequencyStr = radioJammer.coverageBand;
        info.range = calculateFrequencyRange(radioJammer.coverageBand);
        allRangeInfos.append(info);
    }

    // 计算频率范围的最小值和最大值
    double minFreq = std::numeric_limits<double>::max();
    double maxFreq = std::numeric_limits<double>::min();
    for (const auto &info : allRangeInfos)
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

    // 图表布局参数
    qreal chartWidth = width() - 40;
    qreal chartHeight = height() - 40;
    qreal barHeight = chartHeight * 0.8;
    qreal xOffset = 20;
    qreal yOffset = 20;

    // 计算频率到像素的缩放比例
    double freqRange = maxFreq - minFreq;
    double scaleFactor = chartWidth / freqRange;

    // 绘制所有柱状图
    for (auto &info : allRangeInfos)
    {
        qreal barStart = xOffset + (info.range.first - minFreq) * scaleFactor;
        qreal barWidth = (info.range.second - info.range.first) * scaleFactor;

        QRectF barRect(barStart, yOffset + (chartHeight - barHeight), barWidth, barHeight);
        QGraphicsRectItem *barItem = m_scene->addRect(barRect, barPen, QBrush(barColor));
        barItem->setOpacity(0.7);

        // 保存矩形区域，用于鼠标悬浮检测
        info.rect = barRect;
    }

    // 存储所有频率范围信息，用于鼠标悬浮检测
    m_rangeInfos = allRangeInfos;

    // 绘制X轴
    QPen axisPen(Qt::black, 1);
    m_scene->addLine(xOffset, yOffset + chartHeight,
                     xOffset + chartWidth, yOffset + chartHeight, axisPen);

    // 绘制X轴末尾向右的三角形箭头
    qreal arrowSize = 8;
    QPolygonF arrow;
    arrow << QPointF(xOffset + chartWidth, yOffset + chartHeight) // 箭头尾部（X轴终点）
          << QPointF(xOffset + chartWidth - arrowSize, yOffset + chartHeight - arrowSize / 2) // 左上点
          << QPointF(xOffset + chartWidth - arrowSize, yOffset + chartHeight + arrowSize / 2); // 左下点
    QGraphicsPolygonItem *arrowItem = m_scene->addPolygon(arrow, axisPen, QBrush(Qt::black));

    // 添加X轴刻度线（5个刻度，不显示刻度标签）
    qreal axisStep = chartWidth / 5;
    for (int i = 0; i <= 5; ++i)
    {
        qreal tickX = xOffset + i * axisStep;
        m_scene->addLine(tickX, yOffset + chartHeight,
                         tickX, yOffset + chartHeight + 5, axisPen);
    }
}

/**
 * @brief 计算重叠数量
 * @param targetRange 目标频率范围
 * @return 与目标范围存在重叠的频率范围数量
 */
int SpectrumChart::calculateOverlapCount(const QPair<double, double> &targetRange)
{
    int overlapCount = 0;
    for (int i = 0; i < m_rangeInfos.size(); ++i)
    {
        const auto &info = m_rangeInfos.at(i);
        if (info.range.first < targetRange.second && info.range.second > targetRange.first)
        {
            overlapCount++;
        }
    }
    return overlapCount;
}

/**
 * @brief 计算频率范围的最小值和最大值
 * @return 频率范围的最小值和最大值（MHz），无数据时返回 (0, 1000)
 */
QPair<double, double> SpectrumChart::getFrequencyRangeMinMax()
{
    double minFreq = std::numeric_limits<double>::max();
    double maxFreq = std::numeric_limits<double>::min();

    for (int i = 0; i < m_rangeInfos.size(); ++i)
    {
        const auto &info = m_rangeInfos.at(i);
        minFreq = qMin(minFreq, info.range.first);
        maxFreq = qMax(maxFreq, info.range.second);
    }

    if (minFreq >= maxFreq)
    {
        minFreq = 0.0;
        maxFreq = 1000.0;
    }

    return qMakePair(minFreq, maxFreq);
}

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
QString SpectrumChart::getBandName(double freq)
{
    if (freq < 30)
    {
        return QString::fromUtf8("HF频段");
    }
    else if (freq < 300)
    {
        return QString::fromUtf8("VHF频段");
    }
    else if (freq < 1000)
    {
        return QString::fromUtf8("UHF频段");
    }
    else if (freq < 2000)
    {
        return QString::fromUtf8("L频段");
    }
    else if (freq < 4000)
    {
        return QString::fromUtf8("S频段");
    }
    else if (freq < 8000)
    {
        return QString::fromUtf8("C频段");
    }
    else if (freq < 12000)
    {
        return QString::fromUtf8("X频段");
    }
    else if (freq < 18000)
    {
        return QString::fromUtf8("Ku频段");
    }
    else if (freq < 26500)
    {
        return QString::fromUtf8("K频段");
    }
    else if (freq < 40000)
    {
        return QString::fromUtf8("Ka频段");
    }
    // else
    // {
    //     return QString::fromUtf8("毫米波频段");
    // }
}

/**
 * @brief 构建频率范围显示文本
 * @param frequencyStr 频率字符串，如 "5.2~6.1GHz" 或 "Ku波段"
 * @return 用于悬浮提示的频率范围文本
 * @details 波段名称自动转换为具体频率范围（如 "Ku波段" → "12000~18000MHz"），
 *          数值型频率范围直接返回原字符串。
 */
QString SpectrumChart::buildFrequencyDisplay(const QString &frequencyStr)
{
    QPair<double, double> range = calculateFrequencyRange(frequencyStr);
    if (frequencyStr.contains("波段"))
    {
        return QString("%1~%2MHz").arg(range.first).arg(range.second);
    }
    else
    {
        return frequencyStr;
    }
}

/**
 * @brief 鼠标移动事件
 * @param event 鼠标事件
 * @details 根据鼠标位置换算对应频率，查找包含该频率的所有频率范围，
 *          选取最窄（最具体）的范围作为目标，显示悬浮提示：
 *          频段名称、频率范围、信号数量。
 *          信号数量 = 包含当前鼠标频率的频率范围总数。
 */
void SpectrumChart::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);

    // 转换鼠标坐标到图表坐标系
    QPointF scenePos = mapToScene(event->pos());

    // 计算频率范围的最小值和最大值
    QPair<double, double> freqMinMax = getFrequencyRangeMinMax();
    double minFreq = freqMinMax.first;
    double maxFreq = freqMinMax.second;

    // 图表布局参数
    qreal chartWidth = width() - 40;
    qreal xOffset = 20;

    // 计算频率到像素的缩放比例
    double freqRange = maxFreq - minFreq;
    double scaleFactor = chartWidth / freqRange;

    // 计算鼠标位置对应的频率
    double mouseFreq = minFreq + (scenePos.x() - xOffset) / scaleFactor;

    // 找到所有包含当前鼠标频率的频率范围
    QVector<FrequencyRangeInfo> containingRanges;
    for (int i = 0; i < m_rangeInfos.size(); ++i)
    {
        const auto &info = m_rangeInfos.at(i);
        if (mouseFreq >= info.range.first && mouseFreq <= info.range.second)
        {
            containingRanges.append(info);
        }
    }

    if (!containingRanges.isEmpty())
    {
        // 找到最小的频率范围（最具体的）
        FrequencyRangeInfo targetRange = containingRanges[0];
        for (const auto &range : containingRanges)
        {
            double currentWidth = range.range.second - range.range.first;
            double targetWidth = targetRange.range.second - targetRange.range.first;
            if (currentWidth < targetWidth)
            {
                targetRange = range;
            }
        }

        // 计算信号数量（包含当前鼠标频率的频率范围数量）
        int signalCount = containingRanges.size();

        // 构建频率范围显示文本
        QString freqDisplay = buildFrequencyDisplay(targetRange.frequencyStr);

        // 确定频段名称
        QPair<double, double> range = calculateFrequencyRange(targetRange.frequencyStr);
        double centerFreq = (range.first + range.second) / 2;
        QString bandName = getBandName(centerFreq);

        // 显示悬浮提示
        QString tooltip = QString("%1\n频率范围: %2\n信号数量: %3")
                              .arg(bandName).arg(freqDisplay).arg(signalCount);
        QToolTip::showText(event->globalPos(), tooltip, this);
    }
    else
    {
        // 鼠标不在任何频率范围上，隐藏提示
        QToolTip::hideText();
    }
}

/**
 * @brief 鼠标离开事件
 * @param event 事件对象
 * @details 鼠标离开图表区域时隐藏悬浮提示。
 */
void SpectrumChart::leaveEvent(QEvent *event)
{
    QGraphicsView::leaveEvent(event);
    QToolTip::hideText();
}

// ==============================================================================
// SpectrumAnalysis 类实现
// ==============================================================================

/**
 * @brief 构造函数
 * @param parent 父窗口指针
 */
SpectrumAnalysis::SpectrumAnalysis(QWidget *parent)
    : QWidget(parent), ui(new Ui::SpectrumAnalysis)
{
    ui->setupUi(this);

    // 初始化参数
    initParams();

    // 初始化对象
    initObject();

    // 关联信号与槽函数
    initConnect();
}

/**
 * @brief 析构函数
 */
SpectrumAnalysis::~SpectrumAnalysis()
{
    delete ui;
}

/**
 * @brief 初始化参数
 * @details 预留扩展
 */
void SpectrumAnalysis::initParams()
{
}

/**
 * @brief 初始化对象
 * @details 生成测试数据、初始化表格属性、创建频谱图表、初始化数据模型并显示数据。
 */
void SpectrumAnalysis::initObject()
{
    // 生成测试数据
    generateTestData();

    // 初始化表格属性
    initTableViewAttr();

    // 初始化频谱图表
    m_spectrumChart = new SpectrumChart(this);
    m_spectrumChart->setGeometry(10, 10, width() - 20, height() - 20);
    m_spectrumChart->setData(m_radarSource, m_radioSource, m_radarJammerSource, m_RadioJammerSource);

    // 初始化数据模型
    initDataModel();

    // 显示频谱源数据
    displayData();
}

/**
 * @brief 关联信号与槽函数
 * @details 预留扩展
 */
void SpectrumAnalysis::initConnect()
{
}

/**
 * @brief 生成测试数据
 * @details 生成雷达、电台、雷达对抗、通信对抗四类辐射源测试数据，
 *          生成前先清空容器，避免重复追加。
 */
void SpectrumAnalysis::generateTestData()
{
    // 重新生成测试数据前先清空容器，避免重复追加
    m_radarSource.clear();
    m_radioSource.clear();
    m_radarJammerSource.clear();
    m_RadioJammerSource.clear();

    // ---------- 1. 雷达数据 (Radar) ----------
    RadarSource radar1;
    radar1.frequency = QString::fromUtf8("5.2~6.1GHz");
    m_radarSource.append(radar1);

    RadarSource radar2;
    radar2.frequency = QString::fromUtf8("150~170MHz");
    m_radarSource.append(radar2);

    RadarSource radar3;
    radar3.frequency = QString::fromUtf8("8~12GHz");
    m_radarSource.append(radar3);

    // ---------- 2. 通信电台数据 (Communication) ----------
    RadioSource radio1;
    radio1.frequency = QString::fromUtf8("960~1215MHz");
    m_radioSource.append(radio1);

    RadioSource radio2;
    radio2.frequency = QString::fromUtf8("30~88MHz");
    m_radioSource.append(radio2);

    RadioSource radio3;
    radio3.frequency = QString::fromUtf8("Ku波段");
    m_radioSource.append(radio3);

    // ---------- 3. 雷达对抗设备 (Radar Jammer) ----------
    RadarJammerSource radarJammer1;
    radarJammer1.workingBand = QString::fromUtf8("2~18GHz");
    m_radarJammerSource.append(radarJammer1);

    RadarJammerSource radarJammer2;
    radarJammer2.workingBand = QString::fromUtf8("8~12GHz");
    m_radarJammerSource.append(radarJammer2);

    RadarJammerSource radarJammer3;
    radarJammer3.workingBand = QString::fromUtf8("S/C波段");
    m_radarJammerSource.append(radarJammer3);

    // ---------- 4. 通信对抗设备 (Comm Jammer) ----------
    RadioJammerSource radioJammer1;
    radioJammer1.coverageBand = QString::fromUtf8("20~100MHz");
    m_RadioJammerSource.append(radioJammer1);

    RadioJammerSource radioJammer2;
    radioJammer2.coverageBand = QString::fromUtf8("400~470MHz");
    m_RadioJammerSource.append(radioJammer2);

    RadioJammerSource radioJammer3;
    radioJammer3.coverageBand = QString::fromUtf8("225~400MHz");
    m_RadioJammerSource.append(radioJammer3);
}

/**
 * @brief 初始化表格属性
 * @details 设置表格属性，如列数、列宽、行高、表头、数据样式等
 */
void SpectrumAnalysis::initTableViewAttr()
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
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
 * @details 预留扩展
 */
void SpectrumAnalysis::displayData()
{
}

/**
 * @brief 添加雷达辐射源数据
 * @param data 雷达辐射源对象
 * @details 追加到雷达缓存并刷新频谱图表。
 */
void SpectrumAnalysis::addData(const RadarSource &data)
{
    m_radarSource.append(data);
    if (m_spectrumChart)
    {
        m_spectrumChart->setData(m_radarSource, m_radioSource, m_radarJammerSource, m_RadioJammerSource);
    }
}

/**
 * @brief 添加电台辐射源数据
 * @param data 电台辐射源对象
 * @details 追加到电台缓存并刷新频谱图表。
 */
void SpectrumAnalysis::addData(const RadioSource &data)
{
    m_radioSource.append(data);
    if (m_spectrumChart)
    {
        m_spectrumChart->setData(m_radarSource, m_radioSource, m_radarJammerSource, m_RadioJammerSource);
    }
}

/**
 * @brief 添加雷达干扰辐射源数据
 * @param data 雷达干扰辐射源对象
 * @details 追加到雷达干扰缓存并刷新频谱图表。
 */
void SpectrumAnalysis::addData(const RadarJammerSource &data)
{
    m_radarJammerSource.append(data);
    if (m_spectrumChart)
    {
        m_spectrumChart->setData(m_radarSource, m_radioSource, m_radarJammerSource, m_RadioJammerSource);
    }
}

/**
 * @brief 添加通信干扰辐射源数据
 * @param data 通信干扰辐射源对象
 * @details 追加到通信干扰缓存并刷新频谱图表。
 */
void SpectrumAnalysis::addData(const RadioJammerSource &data)
{
    m_RadioJammerSource.append(data);
    if (m_spectrumChart)
    {
        m_spectrumChart->setData(m_radarSource, m_radioSource, m_radarJammerSource, m_RadioJammerSource);
    }
}

/**
 * @brief 更新雷达辐射源数据
 * @param data 雷达辐射源对象（按 name 匹配）
 * @details 若未找到同名记录则不操作。
 */
void SpectrumAnalysis::updateData(const RadarSource &data)
{
    int index = findIndexByName(m_radarSource, data.name);
    if (index != -1)
    {
        m_radarSource[index] = data;
        if (m_spectrumChart)
        {
            m_spectrumChart->setData(m_radarSource, m_radioSource, m_radarJammerSource, m_RadioJammerSource);
        }
    }
}

/**
 * @brief 更新电台辐射源数据
 * @param data 电台辐射源对象（按 name 匹配）
 * @details 若未找到同名记录则不操作。
 */
void SpectrumAnalysis::updateData(const RadioSource &data)
{
    int index = findIndexByName(m_radioSource, data.name);
    if (index != -1)
    {
        m_radioSource[index] = data;
        if (m_spectrumChart)
        {
            m_spectrumChart->setData(m_radarSource, m_radioSource, m_radarJammerSource, m_RadioJammerSource);
        }
    }
}

/**
 * @brief 更新雷达干扰辐射源数据
 * @param data 雷达干扰辐射源对象（按 name 匹配）
 * @details 若未找到同名记录则不操作。
 */
void SpectrumAnalysis::updateData(const RadarJammerSource &data)
{
    int index = findIndexByName(m_radarJammerSource, data.name);
    if (index != -1)
    {
        m_radarJammerSource[index] = data;
        if (m_spectrumChart)
        {
            m_spectrumChart->setData(m_radarSource, m_radioSource, m_radarJammerSource, m_RadioJammerSource);
        }
    }
}

/**
 * @brief 更新通信干扰辐射源数据
 * @param data 通信干扰辐射源对象（按 name 匹配）
 * @details 若未找到同名记录则不操作。
 */
void SpectrumAnalysis::updateData(const RadioJammerSource &data)
{
    int index = findIndexByName(m_RadioJammerSource, data.name);
    if (index != -1)
    {
        m_RadioJammerSource[index] = data;
        if (m_spectrumChart)
        {
            m_spectrumChart->setData(m_radarSource, m_radioSource, m_radarJammerSource, m_RadioJammerSource);
        }
    }
}

/**
 * @brief 删除指定类型和名称的辐射源数据
 * @tparam T 数据类型（RadarSource / RadioSource / RadarJammerSource / RadioJammerSource）
 * @param name 目标名称
 * @details 根据类型在对应容器中查找并删除，删除后刷新频谱图表。
 */
template <typename T>
void SpectrumAnalysis::deleteData(const QString &name)
{
    if constexpr (std::is_same_v<T, RadarSource>)
    {
        int index = findIndexByName(m_radarSource, name);
        if (index != -1)
        {
            m_radarSource.removeAt(index);
            if (m_spectrumChart)
            {
                m_spectrumChart->setData(m_radarSource, m_radioSource, m_radarJammerSource, m_RadioJammerSource);
            }
        }
    }
    else if constexpr (std::is_same_v<T, RadioSource>)
    {
        int index = findIndexByName(m_radioSource, name);
        if (index != -1)
        {
            m_radioSource.removeAt(index);
            if (m_spectrumChart)
            {
                m_spectrumChart->setData(m_radarSource, m_radioSource, m_radarJammerSource, m_RadioJammerSource);
            }
        }
    }
    else if constexpr (std::is_same_v<T, RadarJammerSource>)
    {
        int index = findIndexByName(m_radarJammerSource, name);
        if (index != -1)
        {
            m_radarJammerSource.removeAt(index);
            if (m_spectrumChart)
            {
                m_spectrumChart->setData(m_radarSource, m_radioSource, m_radarJammerSource, m_RadioJammerSource);
            }
        }
    }
    else if constexpr (std::is_same_v<T, RadioJammerSource>)
    {
        int index = findIndexByName(m_RadioJammerSource, name);
        if (index != -1)
        {
            m_RadioJammerSource.removeAt(index);
            if (m_spectrumChart)
            {
                m_spectrumChart->setData(m_radarSource, m_radioSource, m_radarJammerSource, m_RadioJammerSource);
            }
        }
    }
}

// 显式实例化模板
template void SpectrumAnalysis::deleteData<RadarSource>(const QString &name);
template void SpectrumAnalysis::deleteData<RadioSource>(const QString &name);
template void SpectrumAnalysis::deleteData<RadarJammerSource>(const QString &name);
template void SpectrumAnalysis::deleteData<RadioJammerSource>(const QString &name);

/**
 * @brief 窗口大小改变事件
 * @param event 大小改变事件
 * @details 调整频谱图表尺寸并重绘。
 */
void SpectrumAnalysis::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (m_spectrumChart)
    {
        m_spectrumChart->setGeometry(10, 10, width() - 20, height() - 20);
        m_spectrumChart->drawBarChart();
    }
}

/**
 * @brief 鼠标移动事件
 * @param event 鼠标事件
 */
void SpectrumAnalysis::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);
}

/**
 * @brief 鼠标离开事件
 * @param event 事件对象
 */
void SpectrumAnalysis::leaveEvent(QEvent *event)
{
    QWidget::leaveEvent(event);
}
