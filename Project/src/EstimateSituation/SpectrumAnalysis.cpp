//
// Created by admin on "2026.04.10 T 19:19:20".
//

// You may need to build the project (run Qt uic code generator) to get "ui_SpectrumAnalysis.h" resolved

#include "SpectrumAnalysis.h"
#include "ui_SpectrumAnalysis.h"
#include <QPainter>
#include <QMouseEvent>
#include <QLinearGradient>
#include <QGraphicsDropShadowEffect>
#include <QFrame>

// ==============================================================================
// SpectrumChart 类实现
// ==============================================================================

SpectrumChart::SpectrumChart(QWidget *parent)
    : QGraphicsView(parent)
      , m_scene(new QGraphicsScene(this))
      , m_hoveredIndex(-1)
      , m_tooltipItem(nullptr)
      , m_tooltipBg(nullptr)
{
    setScene(m_scene);
    setRenderHint(QPainter::Antialiasing);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameStyle(QFrame::NoFrame);

    setMinimumHeight(220);
    setMouseTracking(true);

    // 初始化为空场景，后续会根据窗口大小和数据动态创建
}

void SpectrumChart::generateTestData()
{
    // 重新生成测试数据前先清空容器，避免重复追加
    m_spectrumData.clear();

    // ---------- 1. HF 频段 ----------
    SpectrumBand hfBand;
    hfBand.name = QString::fromUtf8("HF");
    hfBand.range = QString::fromUtf8("3-30MHz");
    hfBand.color = QColor(Qt::blue);
    hfBand.height = 25;
    hfBand.signalCount = 2;
    hfBand.rect = QRectF();
    hfBand.rectItem = nullptr;
    hfBand.labelItem = nullptr;
    m_spectrumData.append(hfBand);

    // ---------- 2. VHF 频段 ----------
    SpectrumBand vhfBand;
    vhfBand.name = QString::fromUtf8("VHF");
    vhfBand.range = QString::fromUtf8("30-300MHz");
    vhfBand.color = QColor(Qt::red);
    vhfBand.height = 45;
    vhfBand.signalCount = 5;
    vhfBand.rect = QRectF();
    vhfBand.rectItem = nullptr;
    vhfBand.labelItem = nullptr;
    m_spectrumData.append(vhfBand);

    // ---------- 3. UHF 频段 ----------
    SpectrumBand uhfBand;
    uhfBand.name = QString::fromUtf8("UHF");
    uhfBand.range = QString::fromUtf8("300-1000MHz");
    uhfBand.color = QColor(Qt::yellow);
    uhfBand.height = 60;
    uhfBand.signalCount = 8;
    uhfBand.rect = QRectF();
    uhfBand.rectItem = nullptr;
    uhfBand.labelItem = nullptr;
    m_spectrumData.append(uhfBand);

    // ---------- 4. L 频段 ----------
    SpectrumBand lBand;
    lBand.name = QString::fromUtf8("L");
    lBand.range = QString::fromUtf8("1-2GHz");
    lBand.color = QColor(Qt::green);
    lBand.height = 40;
    lBand.signalCount = 4;
    lBand.rect = QRectF();
    lBand.rectItem = nullptr;
    lBand.labelItem = nullptr;
    m_spectrumData.append(lBand);

    // ---------- 5. S 频段 ----------
    SpectrumBand sBand;
    sBand.name = QString::fromUtf8("S");
    sBand.range = QString::fromUtf8("2-4GHz");
    sBand.color = QColor(Qt::cyan);
    sBand.height = 75;
    sBand.signalCount = 9;
    sBand.rect = QRectF();
    sBand.rectItem = nullptr;
    sBand.labelItem = nullptr;
    m_spectrumData.append(sBand);

    // ---------- 6. C 频段 ----------
    SpectrumBand cBand;
    cBand.name = QString::fromUtf8("C");
    cBand.range = QString::fromUtf8("4-8GHz");
    cBand.color = QColor(Qt::magenta);
    cBand.height = 55;
    cBand.signalCount = 6;
    cBand.rect = QRectF();
    cBand.rectItem = nullptr;
    cBand.labelItem = nullptr;
    m_spectrumData.append(cBand);

    // ---------- 7. X 频段 ----------
    SpectrumBand xBand;
    xBand.name = QString::fromUtf8("X");
    xBand.range = QString::fromUtf8("8-12GHz");
    xBand.color = QColor(Qt::red);
    xBand.height = 85;
    xBand.signalCount = 11;
    xBand.rect = QRectF();
    xBand.rectItem = nullptr;
    xBand.labelItem = nullptr;
    m_spectrumData.append(xBand);

    // ---------- 8. Ku 频段 ----------
    SpectrumBand kuBand;
    kuBand.name = QString::fromUtf8("Ku");
    kuBand.range = QString::fromUtf8("12-18GHz");
    kuBand.color = QColor(Qt::darkGreen);
    kuBand.height = 50;
    kuBand.signalCount = 5;
    kuBand.rect = QRectF();
    kuBand.rectItem = nullptr;
    kuBand.labelItem = nullptr;
    m_spectrumData.append(kuBand);

    // ---------- 9. K 频段 ----------
    SpectrumBand kBand;
    kBand.name = QString::fromUtf8("K");
    kBand.range = QString::fromUtf8("18-27GHz");
    kBand.color = QColor(Qt::darkYellow);
    kBand.height = 30;
    kBand.signalCount = 3;
    kBand.rect = QRectF();
    kBand.rectItem = nullptr;
    kBand.labelItem = nullptr;
    m_spectrumData.append(kBand);

    // ---------- 10. Ka 频段 ----------
    SpectrumBand kaBand;
    kaBand.name = QString::fromUtf8("Ka");
    kaBand.range = QString::fromUtf8("27-40GHz");
    kaBand.color = QColor(Qt::blue);
    kaBand.height = 20;
    kaBand.signalCount = 2;
    kaBand.rect = QRectF();
    kaBand.rectItem = nullptr;
    kaBand.labelItem = nullptr;
    m_spectrumData.append(kaBand);

    // 生成数据后创建频谱条
    createSpectrumBars();
}

void SpectrumChart::createSpectrumBars()
{
    m_scene->clear();

    // 使用窗口的实际大小
    int sceneWidth = width();
    int sceneHeight = height() - 40;
    m_scene->setSceneRect(0, 0, sceneWidth, sceneHeight);

    QGraphicsRectItem *bgRect = m_scene->addRect(0, 0, sceneWidth, sceneHeight);
    bgRect->setBrush(QBrush(Qt::lightGray));
    bgRect->setPen(Qt::NoPen);

    int barCount = m_spectrumData.size();
    if (barCount == 0) {
        // 如果没有数据，只绘制背景
        return;
    }
    float barWidth = (sceneWidth - 100) / barCount;
    float chartHeight = sceneHeight - 40;

    for (int i = 0; i < barCount; ++i)
    {
        SpectrumBand &band = m_spectrumData[i];

        float x = 50 + i * barWidth;
        float barHeight = chartHeight * band.height / 100.0;
        float y = sceneHeight - 30 - barHeight;

        band.rect = QRectF(x, y, barWidth - 10, barHeight);

        band.rectItem = m_scene->addRect(band.rect, QPen(band.color, 1));
        band.rectItem->setBrush(QBrush(band.color));

        band.labelItem = m_scene->addText(band.name);
        band.labelItem->setPos(x + (barWidth - 10) / 2 - band.labelItem->boundingRect().width() / 2,
                               sceneHeight - 25);

        band.rectItem->setAcceptHoverEvents(true);
        band.rectItem->setData(0, i);
    }

    m_tooltipItem = m_scene->addText("");
    m_tooltipItem->setZValue(100);
    m_tooltipItem->hide();

    m_tooltipBg = m_scene->addRect(QRectF(), QPen(Qt::black, 1),
                                QBrush(Qt::white));
    m_tooltipBg->setZValue(99);
    m_tooltipBg->hide();
}

void SpectrumChart::updateSpectrumData()
{
    createSpectrumBars();
}

void SpectrumChart::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    
    // 当窗口大小改变时，重新创建频谱条以适应新的窗口大小
    if (!m_spectrumData.isEmpty()) {
        createSpectrumBars();
    }
    
    fitInView(m_scene->sceneRect(), Qt::KeepAspectRatioByExpanding);
}

void SpectrumChart::mouseMoveEvent(QMouseEvent *event)
{
    QPointF scenePos = mapToScene(event->pos());
    QGraphicsItem *item = m_scene->itemAt(scenePos, QTransform());

    if (item && item->data(0).toInt() >= 0)
    {
        int index = item->data(0).toInt();
        if (index < m_spectrumData.size())
        {
            m_hoveredIndex = index;
            const SpectrumBand &band = m_spectrumData[index];

            QString text = QString("%1频段\n频率范围: %2\n信号数量: %3个\n能量占比: %4%")
                          .arg(band.name)
                          .arg(band.range)
                          .arg(band.signalCount)
                          .arg(band.height);
            m_tooltipItem->setPlainText(text);

            QRectF textRect = m_tooltipItem->boundingRect();
            QRectF bgRect = textRect.adjusted(-5, -5, 5, 5);

            QPointF tooltipPos = band.rect.topRight() + QPointF(10, -bgRect.height() / 2);
            if (tooltipPos.x() + bgRect.width() > m_scene->width())
            {
                tooltipPos.setX(band.rect.left() - bgRect.width() - 10);
            }
            if (tooltipPos.y() < 0)
            {
                tooltipPos.setY(0);
            }
            if (tooltipPos.y() + bgRect.height() > m_scene->height())
            {
                tooltipPos.setY(m_scene->height() - bgRect.height());
            }

            m_tooltipBg->setRect(tooltipPos.x(), tooltipPos.y(), bgRect.width(), bgRect.height());
            m_tooltipItem->setPos(tooltipPos.x() + 5, tooltipPos.y() + 5);

            m_tooltipBg->show();
            m_tooltipItem->show();

            return;
        }
    }

    m_hoveredIndex = -1;
    if (m_tooltipItem)
        m_tooltipItem->hide();
    if (m_tooltipBg)
        m_tooltipBg->hide();

    QGraphicsView::mouseMoveEvent(event);
}

void SpectrumChart::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    m_hoveredIndex = -1;
    if (m_tooltipItem)
        m_tooltipItem->hide();
    if (m_tooltipBg)
        m_tooltipBg->hide();

    QGraphicsView::leaveEvent(event);
}

void SpectrumChart::updateTooltip()
{
    // 实现工具提示更新逻辑
}

// ==============================================================================
// SpectrumAnalysis 类实现
// ==============================================================================
SpectrumAnalysis::SpectrumAnalysis(QWidget *parent)
    : QWidget(parent), ui(new Ui::SpectrumAnalysis)
{
    ui->setupUi(this);

    // 初始化参数
    initParams();

    // 初始化对象
    initObject();

    // 初始化连接
    initConnect();
}

SpectrumAnalysis::~SpectrumAnalysis()
{
    delete ui;
}

void SpectrumAnalysis::initParams()
{
    // 初始化参数
}

void SpectrumAnalysis::initObject()
{
    // 生成测试数据
    generateTestData();

    // 创建频谱图表
    m_spectrumChart = new SpectrumChart();
    m_spectrumChart->setMinimumHeight(220);

    // 生成测试数据
    m_spectrumChart->generateTestData();

    // 初始化样式表
    initStyles();

    // 添加到布局
    ui->gridLayout->addWidget(m_spectrumChart);
}

void SpectrumAnalysis::initStyles()
{
    // 初始化样式表
    setStyleSheet(
        "QWidget {"
        "    background-color: #f0f0f0;"
        "    font-family: Arial;"
        "}"
        "QLabel#titleLabel {"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "    color: #333333;"
        "}"
    );

    // 为频谱图表设置样式
    m_spectrumChart->setStyleSheet(
        "QGraphicsView {"
        "    background-color: white;"
        "    border: 1px solid #cccccc;"
        "    border-radius: 4px;"
        "}"
    );
}

void SpectrumAnalysis::initConnect()
{
    // 初始化信号连接
}

void SpectrumAnalysis::generateTestData()
{
    // 重新生成测试数据前先清空容器，避免重复追加
    m_radarSource.clear();
    m_radioSource.clear();
    m_radarJammerSource.clear();
    m_radioJammerSource.clear();

    // ---------- 1. 雷达数据 (Radar) ----------
    RadarSource radar1;
    radar1.name = QString::fromUtf8("AN/MPQ-53 相控阵雷达");
    radar1.frequency = QString::fromUtf8("5.2~6.1GHz");
    radar1.prf = QString::fromUtf8("200~500Hz");
    radar1.pulseWidth = QString::fromUtf8("0.5~25μs");
    radar1.scanMode = QString::fromUtf8("电子扫描");
    radar1.threatLevel = QString::fromUtf8("高");
    radar1.deviceType = QString::fromUtf8("高功率火控雷达");
    m_radarSource.append(radar1);

    RadarSource radar2;
    radar2.name = QString::fromUtf8("P-18 预警雷达");
    radar2.frequency = QString::fromUtf8("150~170MHz");
    radar2.prf = QString::fromUtf8("300Hz");
    radar2.pulseWidth = QString::fromUtf8("8μs");
    radar2.scanMode = QString::fromUtf8("6rpm");
    radar2.threatLevel = QString::fromUtf8("中");
    radar2.deviceType = QString::fromUtf8("米波远程警戒");
    m_radarSource.append(radar2);

    RadarSource radar3;
    radar3.name = QString::fromUtf8("MPQ-64 哨兵雷达");
    radar3.frequency = QString::fromUtf8("8~12GHz");
    radar3.prf = QString::fromUtf8("可变");
    radar3.pulseWidth = QString::fromUtf8("-");
    radar3.scanMode = QString::fromUtf8("旋转扫描");
    radar3.threatLevel = QString::fromUtf8("高");
    radar3.deviceType = QString::fromUtf8("低空补盲雷达");
    m_radarSource.append(radar3);

    // ---------- 2. 通信电台数据 (Communication) ----------
    RadioSource radio1;
    radio1.name = QString::fromUtf8("Link-16 战术数据链");
    radio1.frequency = QString::fromUtf8("960~1215MHz");
    radio1.modulation = QString::fromUtf8("MSK/扩频");
    radio1.codeRate = QString::fromUtf8("1.0Mbps");
    radio1.powerOrWaveform = QString::fromUtf8("JTIDS");
    radio1.threatLevel = QString::fromUtf8("高");
    radio1.deviceType = QString::fromUtf8("关键指控链路");
    m_radioSource.append(radio1);

    RadioSource radio2;
    radio2.name = QString::fromUtf8("VHF 战术电台");
    radio2.frequency = QString::fromUtf8("30~88MHz");
    radio2.modulation = QString::fromUtf8("FM/跳频");
    radio2.codeRate = QString::fromUtf8("-");
    radio2.powerOrWaveform = QString::fromUtf8("25W");
    radio2.threatLevel = QString::fromUtf8("中");
    radio2.deviceType = QString::fromUtf8("班组级通信");
    m_radioSource.append(radio2);

    RadioSource radio3;
    radio3.name = QString::fromUtf8("卫星通信终端");
    radio3.frequency = QString::fromUtf8("Ku波段");
    radio3.modulation = QString::fromUtf8("QPSK");
    radio3.codeRate = QString::fromUtf8("5Msps");
    radio3.powerOrWaveform = QString::fromUtf8("同步卫星");
    radio3.threatLevel = QString::fromUtf8("中");
    radio3.deviceType = QString::fromUtf8("远程指挥链路");
    m_radioSource.append(radio3);

    // ---------- 3. 雷达对抗设备 (Radar Jammer) ----------
    RadarJammerSource radarJammer1;
    radarJammer1.name = QString::fromUtf8("SPECTRAL 侦察干扰吊舱");
    radarJammer1.jammingType = QString::fromUtf8("距离门拖引/速度欺骗");
    radarJammer1.workingBand = QString::fromUtf8("2~18GHz");
    radarJammer1.technology = QString::fromUtf8("DRFM转发");
    radarJammer1.threatLevel = QString::fromUtf8("高");
    radarJammer1.deviceType = QString::fromUtf8("先进数字射频存储");
    m_radarJammerSource.append(radarJammer1);

    RadarJammerSource radarJammer2;
    radarJammer2.name = QString::fromUtf8("Pelena-1 地面干扰站");
    radarJammer2.jammingType = QString::fromUtf8("噪声压制/假目标");
    radarJammer2.workingBand = QString::fromUtf8("8~12GHz");
    radarJammer2.technology = QString::fromUtf8("模拟转发");
    radarJammer2.threatLevel = QString::fromUtf8("高");
    radarJammer2.deviceType = QString::fromUtf8("火控雷达对抗");
    m_radarJammerSource.append(radarJammer2);

    RadarJammerSource radarJammer3;
    radarJammer3.name = QString::fromUtf8("战术侦察/干扰模块");
    radarJammer3.jammingType = QString::fromUtf8("测频/测向+间歇采样");
    radarJammer3.workingBand = QString::fromUtf8("S/C波段");
    radarJammer3.technology = QString::fromUtf8("数字接收");
    radarJammer3.threatLevel = QString::fromUtf8("中");
    radarJammer3.deviceType = QString::fromUtf8("小型化电子攻击");
    m_radarJammerSource.append(radarJammer3);

    // ---------- 4. 通信对抗设备 (Comm Jammer) ----------
    RadioJammerSource radioJammer1;
    radioJammer1.name = QString::fromUtf8("R-330Zh 通信干扰系统");
    radioJammer1.jammingStyle = QString::fromUtf8("噪声调频/梳状谱");
    radioJammer1.coverageBand = QString::fromUtf8("20~100MHz");
    radioJammer1.erp = QString::fromUtf8("1kW");
    radioJammer1.threatLevel = QString::fromUtf8("高");
    radioJammer1.deviceType = QString::fromUtf8("大功率宽带压制");
    m_radioJammerSource.append(radioJammer1);

    RadioJammerSource radioJammer2;
    radioJammer2.name = QString::fromUtf8("便携式通信干扰机");
    radioJammer2.jammingStyle = QString::fromUtf8("单音/扫频");
    radioJammer2.coverageBand = QString::fromUtf8("400~470MHz");
    radioJammer2.erp = QString::fromUtf8("50W");
    radioJammer2.threatLevel = QString::fromUtf8("低");
    radioJammer2.deviceType = QString::fromUtf8("近距离战术干扰");
    m_radioJammerSource.append(radioJammer2);

    RadioJammerSource radioJammer3;
    radioJammer3.name = QString::fromUtf8("车载智能干扰站");
    radioJammer3.jammingStyle = QString::fromUtf8("协议伪造/随机脉冲");
    radioJammer3.coverageBand = QString::fromUtf8("225~400MHz");
    radioJammer3.erp = QString::fromUtf8("200W");
    radioJammer3.threatLevel = QString::fromUtf8("中");
    radioJammer3.deviceType = QString::fromUtf8("自适应干扰");
    m_radioJammerSource.append(radioJammer3);
}

/**
 * @brief 添加雷达辐射源数据
 * @param data 雷达辐射源对象
 * @details 追加到雷达缓存。
 */
void SpectrumAnalysis::addDataImpl(const RadarSource &data)
{
    m_radarSource.append(data);
    if (m_spectrumChart)
    {
        m_spectrumChart->updateSpectrumData();
    }
}

/**
 * @brief 添加电台辐射源数据
 * @param data 电台辐射源对象
 * @details 追加到电台缓存并更新图表。
 */
void SpectrumAnalysis::addDataImpl(const RadioSource &data)
{
    m_radioSource.append(data);
    if (m_spectrumChart)
    {
        m_spectrumChart->updateSpectrumData();
    }
}

/**
 * @brief 添加雷达干扰辐射源数据
 * @param data 雷达干扰辐射源对象
 * @details 追加到雷达干扰缓存并更新图表。
 */
void SpectrumAnalysis::addDataImpl(const RadarJammerSource &data)
{
    m_radarJammerSource.append(data);
    if (m_spectrumChart)
    {
        m_spectrumChart->updateSpectrumData();
    }
}

/**
 * @brief 添加通信干扰辐射源数据
 * @param data 通信干扰辐射源对象
 * @details 追加到通信干扰缓存并更新图表。
 */
void SpectrumAnalysis::addDataImpl(const RadioJammerSource &data)
{
    m_radioJammerSource.append(data);
    if (m_spectrumChart)
    {
        m_spectrumChart->updateSpectrumData();
    }
}

/**
 * @brief 更新雷达辐射源数据
 * @param data 雷达辐射源对象（按 name 匹配）
 * @details 若未找到同名记录则转为新增。
 */
void SpectrumAnalysis::updateDataImpl(const RadarSource &data)
{
    const int row = findIndexByName(m_radarSource, data.name);
    if (row < 0)
    {
        addDataImpl(data);
        return;
    }
    m_radarSource[row] = data;
    if (m_spectrumChart)
    {
        m_spectrumChart->updateSpectrumData();
    }
}

/**
 * @brief 更新电台辐射源数据
 * @param data 电台辐射源对象（按 name 匹配）
 * @details 若未找到同名记录则转为新增。
 */
void SpectrumAnalysis::updateDataImpl(const RadioSource &data)
{
    const int row = findIndexByName(m_radioSource, data.name);
    if (row < 0)
    {
        addDataImpl(data);
        return;
    }
    m_radioSource[row] = data;
    if (m_spectrumChart)
    {
        m_spectrumChart->updateSpectrumData();
    }
}

/**
 * @brief 更新雷达干扰辐射源数据
 * @param data 雷达干扰辐射源对象（按 name 匹配）
 * @details 若未找到同名记录则转为新增。
 */
void SpectrumAnalysis::updateDataImpl(const RadarJammerSource &data)
{
    const int row = findIndexByName(m_radarJammerSource, data.name);
    if (row < 0)
    {
        addDataImpl(data);
        return;
    }
    m_radarJammerSource[row] = data;
    if (m_spectrumChart)
    {
        m_spectrumChart->updateSpectrumData();
    }
}

/**
 * @brief 更新通信干扰辐射源数据
 * @param data 通信干扰辐射源对象（按 name 匹配）
 * @details 若未找到同名记录则转为新增。
 */
void SpectrumAnalysis::updateDataImpl(const RadioJammerSource &data)
{
    const int row = findIndexByName(m_radioJammerSource, data.name);
    if (row < 0)
    {
        addDataImpl(data);
        return;
    }
    m_radioJammerSource[row] = data;
    if (m_spectrumChart)
    {
        m_spectrumChart->updateSpectrumData();
    }
}

/**
 * @brief 删除雷达辐射源数据
 * @param name 目标名称
 * @details 从雷达缓存中同步删除同名记录。
 */
void SpectrumAnalysis::deleteRadarDataByName(const QString &name)
{
    const int row = findIndexByName(m_radarSource, name);
    if (row < 0)
    {
        return;
    }
    m_radarSource.removeAt(row);
    if (m_spectrumChart)
    {
        m_spectrumChart->updateSpectrumData();
    }
}

/**
 * @brief 删除电台辐射源数据
 * @param name 目标名称
 * @details 从电台缓存中同步删除同名记录并更新图表。
 */
void SpectrumAnalysis::deleteRadioDataByName(const QString &name)
{
    const int row = findIndexByName(m_radioSource, name);
    if (row < 0)
    {
        return;
    }
    m_radioSource.removeAt(row);
    if (m_spectrumChart)
    {
        m_spectrumChart->updateSpectrumData();
    }
}

/**
 * @brief 删除雷达干扰辐射源数据
 * @param name 目标名称
 * @details 从雷达干扰缓存中同步删除同名记录并更新图表。
 */
void SpectrumAnalysis::deleteRadarJammerDataByName(const QString &name)
{
    const int row = findIndexByName(m_radarJammerSource, name);
    if (row < 0)
    {
        return;
    }
    m_radarJammerSource.removeAt(row);
    if (m_spectrumChart)
    {
        m_spectrumChart->updateSpectrumData();
    }
}

/**
 * @brief 删除通信干扰辐射源数据
 * @param name 目标名称
 * @details 从通信干扰缓存中同步删除同名记录并更新图表。
 */
void SpectrumAnalysis::deleteRadioJammerDataByName(const QString &name)
{
    const int row = findIndexByName(m_radioJammerSource, name);
    if (row < 0)
    {
        return;
    }
    m_radioJammerSource.removeAt(row);
    if (m_spectrumChart)
    {
        m_spectrumChart->updateSpectrumData();
    }
}
