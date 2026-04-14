//
// Created by admin on "2026.04.09 T 19:10:36".
//

#pragma once

#include <QString>
#include <QStringList>
#include <QPair>
#include <QRectF>
#include <QTableWidgetItem>
#include <cmath>

class QTableWidget;

/**
 * @brief 雷达辐射源结构体
 * 对应网页中“雷达”标签页表格的字段
 */
struct RadarSource
{
    QString name;           // 目标名称，如 "AN/MPQ-53 相控阵雷达"
    QString frequency;      // 工作频率，如 "5.2~6.1GHz"
    QString prf;            // 脉冲重频 (PRF)，如 "200~500Hz"
    QString pulseWidth;     // 脉宽 (PW)，如 "0.5~25μs"
    QString scanMode;       // 扫描方式，如 "电子扫描"
    QString threatLevel;    // 威胁等级：高 / 中 / 低
    QString deviceType;     // 设备类型子类（额外字段），如 "高功率火控雷达"
};

/**
 * @brief 通信电台结构体
 * 对应网页中“电台”标签页表格的字段
 */
struct RadioSource
{
    QString name;               // 目标名称，如 "Link-16 战术数据链"
    QString frequency;          // 工作频率，如 "960~1215MHz"
    QString modulation;         // 调制方式，如 "MSK/扩频"
    QString codeRate;           // 码速率，如 "1.0Mbps"
    QString powerOrWaveform;    // 功率或波形，如 "25W" 或 "JTIDS"
    QString threatLevel;        // 威胁等级：高 / 中 / 低
    QString deviceType;         // 设备类型子类（额外字段），如 "关键指控链路"
};

/**
 * @brief 通信对抗设备（通信干扰）结构体
 * 对应网页中“通信对抗”标签页表格的字段
 */
struct RadioJammerSource
{
    QString name;           // 目标名称，如 "R-330Zh 通信干扰系统"
    QString jammingStyle;   // 干扰样式，如 "噪声调频/梳状谱"
    QString coverageBand;   // 覆盖频段，如 "20~100MHz"
    QString erp;            // 有效辐射功率 (ERP)，如 "1kW"
    QString threatLevel;    // 威胁等级：高 / 中 / 低
    QString deviceType;     // 设备类型描述，如 "大功率宽带压制"
};

/**
 * @brief 雷达对抗设备（雷达干扰）结构体
 * 对应网页中“雷达对抗”标签页表格的字段
 */
struct RadarJammerSource
{
    QString name;           // 目标名称，如 "SPECTRAL 侦察干扰吊舱"
    QString jammingType;    // 对抗类型 / 干扰样式，如 "距离门拖引/速度欺骗"
    QString workingBand;    // 工作频段，如 "2~18GHz"
    QString technology;     // 技术体制，如 "DRFM转发"
    QString threatLevel;    // 威胁等级：高 / 中 / 低
    QString deviceType;     // 设备类型描述，如 "先进数字射频存储"
};

enum class SourceType
{
    Radar,
    Radio,
    CommJamming,
    RadarJamming
};

struct ColumnConfig
{
    QStringList headers;
};

// 辐射源数据结构体
struct SourceData
{
    QString name;        // 辐射源名称
    QString frequency;   // 频率范围
    QString repetition;  // 重复频率
    QString pulseWidth;  // 脉冲宽度
    QString scanMode;    // 扫描方式
    QString threatLevel; // 威胁等级
};

// 通用数据结构体（赋值为空，可通过接口传值）
struct RadarSourceGenericData
{
    QString name;       // 名称
    QStringList values; // 数据值列表
};

/**
 * @brief 防空火力设备结构体
 * @details 存储防空火力设备的基本信息和状态
 */
struct FirepowerItem
{
    QString name;       // 设备名称
    QString status;     // 状态（active/standby/disabled）
    QString type;       // 类型
    QString range;      // 射程
    QString ammo;       // 弹药/模式
    QString response;   // 响应时间
    QString intercept;  // 拦截概率

    FirepowerItem() = default;

    FirepowerItem(const QString &n, const QString &s, const QString &t, const QString &r, const QString &a, const QString &resp, const QString &inter)
        : name(n), status(s), type(t), range(r), ammo(a), response(resp), intercept(inter) {}
};

/**
 * @brief 态势控制数据结构体
 * @details 存储态势控制相关的配置数据
 */
struct SituationControlData
{
    QString type;       // 控制类型（radar/radio/commJam/radarJam/defenseFire）
    bool enabled;       // 是否启用
    QString description; // 描述信息

    SituationControlData() = default;

    SituationControlData(const QString &t, bool e, const QString &desc)
        : type(t), enabled(e), description(desc) {}
};

/**
 * @brief 统一威胁项结构体
 * @details 用于存储不同类型威胁的统一格式数据
 */
struct UnifiedThreatItem
{
    QString name;        // 威胁名称
    QString threatLevel; // 威胁等级
    QString type;        // 威胁类型
    int priority;        // 优先级（用于排序）

    UnifiedThreatItem() : priority(0) {}

    UnifiedThreatItem(const QString &n, const QString &tl, const QString &t, int p)
        : name(n), threatLevel(tl), type(t), priority(p) {}
};

/**
 * @brief 频率范围信息结构体
 * @details 存储频率范围字符串、数值范围和柱状图矩形区域，用于频谱分析图表绘制和鼠标悬浮检测。
 */
struct FrequencyRangeInfo
{
    QString frequencyStr;           // 频率范围字符串
    QPair<double, double> range;    // 频率范围数值（MHz）
    QRectF rect;                    // 柱状图矩形区域
};

// 威胁评估
#pragma region

// 雷达性能参数区间
struct RadarPerformancePara
{
    double freqMin = 0.0;       // 工作频率区间下限（GHz）
    double freqMax = 0.0;       // 工作频率区间上限（GHz）
    double pwMin = 0.0;         // 脉冲宽度区间下限（μs）
    double pwMax = 0.0;         // 脉冲宽度区间上限（μs）
    double prfMin = 0.0;        // 脉冲重复频率区间下限（Hz）
    double prfMax = 0.0;        // 脉冲重复频率区间上限（Hz）
    double detectRange = 0.0;   // 探测距离（km），参与距离修正系数
};

// 单源一次评估的数值结果
struct RadarThreatAssessResult
{
    bool valid = false;     // 本次评估是否成功（失败时 errorMessage 有效）
    QString errorMessage;   // 评估失败或校验不通过时的说明文案
    double freq = 0.0;      // 频率代表值 / 几何中心（GHz）
    double pw = 0.0;        // 脉宽代表值 / 几何中心（μs）
    double prf = 0.0;       // PRF 代表值 / 几何中心（Hz）
};

// 雷达实时参数
struct RadarRealWrokPara
{
    int situationDefenseIndex = -1;     // 阵地火力 combo 索引；-1 未设则用批量默认
    int situationRadModeIndex = -1;     // 辐射模式 combo 索引；-1 未设则用批量默认
    double workPower;                   // 辐射功率
    double defaultPower;                // 额定功率
};

// 雷达典型参数
struct RadarTypicalPara
{
    bool valid = false;     // 三个代表值是否已有效填充
    double freq = 0.0;      // 频率代表值（GHz）
    double pw = 0.0;        // 脉宽代表值（μs）
    double prf = 0.0;       // PRF 代表值（Hz）
};

// 子因子与合成
struct RadarThreatFactors
{
    double freqFactor = 0.0;     // 全局频率子因子（与界面 spin 一致）
    double prfFactor = 0.0;      // 全局 PRF 子因子
    double pwFactor = 0.0;       // 全局脉宽子因子
    double f1RawFactor = 0.0;    // 全局距离修正前合成 f1Raw
    double rangeModFactor = 1.0; // 全局距离修正系数 rangeMod
    double f1Factor = 0.0;       // 全局合成威胁指数 F1（写入各行前的共享快照）
};

/**
 * @brief 单部雷达辐射源在业务与界面中的完整持久化单元
 * @details 对应表格一行及左侧表单上下文；含性能区间、代表值、评估结果与态势 combo 索引。
 */
struct RadarThreatAssessRecord
{
    QString entityName;                 // 装备名称
    QString typeName;                   // 雷达型号

    RadarPerformancePara perfPara;        // 雷达性能参数
    RadarRealWrokPara workPara;           // 雷达实时参数
    RadarTypicalPara typicalPara;         // 雷达典型参数
    RadarThreatAssessResult result; // 雷达威胁评估结果

    // [0,1] 威胁度→五档文案；供 threatLevelText 与外部展示复用
    static QString threatLevelFromF1(double adjustedF1);

    // 按传入 F1 与辐射模式乘子得五档等级（表格「威胁等级」列；与底部有效威胁度 F1×系数一致）
    QString threatLevelText(double f1) const;
};
#pragma endregion

// 项目公共接口
class ProjectPublicInterface
{
public:
    ProjectPublicInterface() = default;
    virtual ~ProjectPublicInterface() = default;

    // 通用格式转换：F1 值 -> 表格展示文本
    static QString ProjectPublicInterface::f1TableCellText(double f1)
    {
        if (!std::isfinite(f1))
        {
            return QStringLiteral("—");
        }
        return QString::number(f1, 'f', 4);
    }

    // 通用取值转换：读取表格单元文本并去首尾空格
    static QString ProjectPublicInterface::itemText(const QTableWidget *table, int row, int col)
    {
        const QTableWidgetItem *cellItem = table->item(row, col);
        return cellItem ? cellItem->text().trimmed() : QString();
    }

    // 通用数值转换：字符串 -> double，成功返回 true
    static bool ProjectPublicInterface::parseDouble(const QString &text, double *outValue)
    {
        bool ok = false;
        const double parsed = text.toDouble(&ok); // 表格文本可能含空格，调用方已 trim
        if (!ok || outValue == nullptr)
        {
            return false;
        }
        *outValue = parsed;
        return true;
    }

    // 辐射模式乘子（用于计算有效威胁度 F1×系数）
    static double radiationModeMultiplier(int comboIndex)
    {
        switch (comboIndex)
        {
            case 0:
                return 1.0;
            case 1:
                return 1.0;
            case 2:
                return 0.85;
            case 3:
                return 0.75;
            case 4:
                return 0.0;
            default:
                return 1.0;
        }
    }

    // 威胁等级[0,1] threatLevel→五档文案
    static double clamp01(double value)
    {
        if (value < 0.0)
        {
            return 0.0;
        }
        if (value > 1.0)
        {
            return 1.0;
        }
        return value;
    }

    // 几何平均
    static double geometricMean(double intervalLow, double intervalHigh)
    {
        if (intervalLow <= 0.0 || intervalHigh <= 0.0)
        {
            return 0.0;
        }
        return std::sqrt(intervalLow * intervalHigh);
    }

    // 频率子因子
    static double frequencyThreatFactor(double freq)
    {
        if (freq <= 0.1) { return 0.05; }
        if (freq <= 1.0) { return 0.15; }
        if (freq <= 4.0) { return 0.15 + 0.30 * (freq - 1.0) / 3.0; }
        if (freq <= 8.0) { return 0.45 + 0.25 * (freq - 4.0) / 4.0; }
        if (freq <= 12.0) { return 0.70 + 0.20 * (freq - 8.0) / 4.0; }
        if (freq <= 18.0) { return 0.90 - 0.30 * (freq - 12.0) / 6.0; }
        return 0.40;
    }

    // 脉宽子因子
    static double pulseWidthThreatFactor(double pw)
    {
        if (pw <= 0.0)
        {
            return 0.0;
        }
        const double logTau = std::log(pw) / std::log(10.0);
        return clamp01(1.0 - (logTau + 2.0) / 4.0);
    }

    // 重频子因子
    static double prfThreatFactor(double prf)
    {
        if (prf <= 0.0)
        {
            return 0.0;
        }
        const double logP = std::log(prf) / std::log(10.0);
        return clamp01((logP - 1.5) / 3.5);
    }

    // 距离修正
    static double rangeModifier(double km, double f1)
    {
        if (km >= 200.0) { return 1.0 + 0.08 * f1; }
        if (km > 50.0) { return 1.0; }
        return 1.0 - 0.10 * f1;
    }

    // 评估结果
    static bool validateRadarInput(const RadarPerformancePara &input, QString *errorMessage)
    {
        if (input.freqMin <= 0.0 || input.freqMax <= 0.0 || input.freqMin > input.freqMax)
        {
            if (errorMessage) { *errorMessage = QStringLiteral("频率范围无效：请填写正数且最小值不大于最大值（单值时请将上下限设为相同）。"); }
            return false;
        }
        if (input.pwMin <= 0.0 || input.pwMax <= 0.0 || input.pwMin > input.pwMax)
        {
            if (errorMessage) { *errorMessage = QStringLiteral("脉宽范围无效。"); }
            return false;
        }
        if (input.prfMin <= 0.0 || input.prfMax <= 0.0 || input.prfMin > input.prfMax)
        {
            if (errorMessage) { *errorMessage = QStringLiteral("PRF 范围无效。"); }
            return false;
        }
        if (input.detectRange <= 0.0)
        {
            if (errorMessage) { *errorMessage = QStringLiteral("探测距离须为大于 0 的数值（km）。"); }
            return false;
        }
        return true;
    }

    // 构建评估结果
    static RadarThreatAssessResult calculateThreatResult(const RadarPerformancePara &performance,
                                                     const RadarTypicalPara &representative,
                                                     const RadarThreatFactors &globalFactors)
    {
        Q_UNUSED(globalFactors);
        RadarThreatAssessResult result;
        if (!validateRadarInput(performance, &result.errorMessage))
        {
            return result;
        }
        if (representative.valid)
        {
            result.freq = representative.freq;
            result.pw = representative.pw;
            result.prf = representative.prf;
        }
        else
        {
            result.freq = geometricMean(performance.freqMin, performance.freqMax);
            result.pw = geometricMean(performance.pwMin, performance.pwMax);
            result.prf = geometricMean(performance.prfMin, performance.prfMax);
        }
        result.valid = true;
        return result;
    }

    // 评估
    static RadarThreatAssessResult evaluate(const RadarPerformancePara &input)
    {
        RadarThreatAssessResult result;
        if (!validateRadarInput(input, &result.errorMessage))
        {
            return result;
        }
        result.freq = geometricMean(input.freqMin, input.freqMax);
        result.pw = geometricMean(input.pwMin, input.pwMax);
        result.prf = geometricMean(input.prfMin, input.prfMax);
        result.valid = true;
        return result;
    }

    // 从预设索引获取雷达性能参数
    static RadarPerformancePara radarInputFromPresetIndex(int presetIndex)
    {
        RadarPerformancePara performance;
        struct Preset { double freqGhz; double pulseWidthUs; double prfHz; double rangeKm; };
        static const Preset presetTable[] = {
                {5.5, 0.7, 15000.0, 150.0}, {5.5, 1.0, 10000.0, 120.0}, {4.5, 0.8, 12000.0, 150.0},
                {3.0, 2.0, 5000.0, 250.0}, {4.5, 1.5, 8000.0, 370.0}, {1.3, 50.0, 300.0, 470.0},
                {3.1, 10.0, 600.0, 440.0}, {0.02, 200.0, 50.0, 1000.0}, {15.0, 0.05, 50000.0, 18.0},
        };
        static const int kPresetCount = static_cast<int>(sizeof(presetTable) / sizeof(presetTable[0]));
        if (presetIndex < 1 || presetIndex > kPresetCount)
        {
            return performance;
        }
        const Preset &presetEntry = presetTable[presetIndex - 1];
        performance.freqMin = performance.freqMax = presetEntry.freqGhz;
        performance.pwMin = performance.pwMax = presetEntry.pulseWidthUs;
        performance.prfMin = performance.prfMax = presetEntry.prfHz;
        performance.detectRange = presetEntry.rangeKm;
        return performance;
    }
};