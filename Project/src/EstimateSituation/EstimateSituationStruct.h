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
 * 对应网页中"雷达"标签页表格的字段
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
 * 对应网页中"电台"标签页表格的字段
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
 * 对应网页中"通信对抗"标签页表格的字段
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
 * 对应网页中"雷达对抗"标签页表格的字段
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
 * @details 存储频率范围字符串、数值范围、柱状图矩形区域和颜色，用于频谱分析图表绘制和鼠标悬浮检测。
 */
struct FrequencyRangeInfo
{
    QString frequencyStr;           // 频率范围字符串
    QPair<double, double> range;    // 频率范围数值（MHz）
    QRectF rect;                    // 柱状图矩形区域
    QColor color;                   // 柱状图颜色
};

// ============================================================================
// 威胁评估 — 枚举与常量
// ============================================================================

/// 防御等级，与界面 combo 索引一一对应
enum class DefenseLevel
{
    None   = 0,    // 无防护 — 独立雷达站，周边无防空火力
    Weak   = 1,    // 弱防护 — 仅近程/点防御武器
    Medium = 2,    // 中防护 — 有导弹级配置但未就绪
    Strong = 3     // 强防护 — 配套导弹级防空且已就绪
};

/// 辐射模式，与界面 combo 索引一一对应
enum class RadiationMode
{
    FullPower = 0, // 全功率辐射
    Normal     = 1, // 正常功率辐射
    LPI        = 2, // LPI 低截获概率模式
    Blink      = 3, // 闪避照射
    Off        = 4  // 关机
};

/// JWS01 导引头频段常量
namespace GuideHeadBand
{
    static constexpr double kFreqMin = 0.5;   // 覆盖下限 GHz
    static constexpr double kFreqMax = 18.0;  // 覆盖上限 GHz
    static constexpr double kOptMin  = 1.0;   // 最优灵敏度区间下限 GHz
    static constexpr double kOptMax  = 12.0;  // 最优灵敏度区间上限 GHz
}

// ============================================================================
// 威胁评估 — 参数与结果结构体
// ============================================================================

// 雷达性能参数区间
struct RadarPerformancePara
{
    double freqMin = 0.0;       // 工作频率区间下限（GHz）
    double freqMax = 0.0;       // 工作频率区间上限（GHz）
    double pwMin = 0.0;         // 脉冲宽度区间下限（μs）
    double pwMax = 0.0;         // 脉冲宽度区间上限（μs）
    double prfMin = 0.0;        // 脉冲重复频率区间下限（Hz）
    double prfMax = 0.0;        // 脉冲重复频率区间上限（Hz）
    double detectRange = 0.0;   // 探测距离（km），<=0 表示无数据（不做距离修正）
};

// 雷达典型参数（用户手动填写的代表值）
struct RadarTypicalPara
{
    bool valid = false;     // 三个代表值是否已有效填充
    double freq = 0.0;      // 频率代表值（GHz）
    double pw = 0.0;        // 脉宽代表值（μs）
    double prf = 0.0;       // PRF 代表值（Hz）
};

// 雷达实时参数（态势推演中动态变化的部分）
struct RadarRealWrokPara
{
    int situationDefenseIndex = -1;     // 阵地火力 combo 索引；-1 表示未设置
    int situationRadModeIndex = -1;     // 辐射模式 combo 索引；-1 表示未设置
    double workPower     = 0.0;         // 辐射功率
    double defaultPower  = 0.0;         // 额定功率
};

/**
 * @brief 威胁因子结构体（独立顶层）
 *
 * 存储完整的两层因子计算结果：
 * - 内层：F1 子因子（频率/PRF/脉宽 → f1Raw → F1），始终计算
 * - 外层：F2 阵地火力、F3 信号截获可行性，有数据时计算
 * - 加权：动态归一化权重、最终分数 S、等级映射
 *
 * 与 RadarThreatAssessResult 分离的原因：
 * - Result 定位为"表单一行的轻量持久化数据"（代表值 + 分数 + 错误信息）
 * - Factors 定位为"计算过程的完整明细"（排序、图表、详情面板消费）
 * - 批量计算时 Factors 可作为独立中间变量传递，无需携带 errorMsg 等冗余字段
 */
struct RadarThreatFactors
{
    // ===== 内层：F1 子因子（始终计算） =====
    double freqFactor = 0.0;     // 频率威胁倾向 ∈ [0,1]
    double prfFactor  = 0.0;     // PRF 威胁倾向 ∈ [0,1]
    double pwFactor   = 0.0;     // 脉宽威胁倾向 ∈ [0,1]

    // ===== 内层：F1 合成 =====
    double f1Raw      = 0.0;     // 距离修正前合成值 ∈ [0,1]
    double rangeMod   = 1.0;     // 探测范围修正系数
    double F1         = 0.0;     // 最终雷达威胁度 ∈ [0,1]

    // ===== 外层：F2 阵地火力（可选） =====
    bool   hasF2      = false;   // 是否有阵地火力数据
    double F2         = 0.0;     // 阵地火力强度 ∈ [0,1]

    // ===== 外层：F3 信号截获可行性（可选） =====
    bool   hasF3      = false;   // 是否有信号截获数据
    double f3M        = 0.0;     // 频段匹配度 M ∈ [0,1]
    double f3R        = 0.0;     // 辐射状态系数 R ∈ [0,1]
    double F3         = 0.0;     // 截获可行性 = M × R ∈ [0,1]
    bool   f3Excluded = false;   // 硬约束标记（关机或频段不匹配）
    QString f3ExcludeReason;     // 排除原因文案

    // ===== 外层：动态权重与结果 =====
    double W1         = 1.0;     // F1 实际权重（动态归一化后）
    double W2         = 0.0;     // F2 实际权重
    double W3         = 0.0;     // F3 实际权重
    double S          = 0.0;     // 威胁分数 ∈ [0,1]
    int    level      = 1;       // 威胁等级 L1~L5
};

/**
 * @brief 单源一次评估的轻量数值结果
 *
 * 定位：表单直接消费的最小数据集（代表值 + 最终分数 + 等级 + 错误信息）。
 * 完整因子明细通过 calculateThreatResult 的输出参数 outFactors 获取，
 * 在业务层由 RadarThreatAssessRecord 分别持有。
 */
struct RadarThreatAssessResult
{
    bool valid = false;             // 本次评估是否成功
    double threatLevel = 0.0;       // 最终威胁分数 S ∈ [0,1]（兼容旧接口）
    int threatGrade = 1;            // 威胁等级 L1~L5
    double freq = 0.0;              // 频率代表值（GHz）
    double pw = 0.0;                // 脉宽代表值（μs）
    double prf = 0.0;               // PRF 代表值（Hz）
    QString errorMsg;               // 评估失败或校验不通过时的说明文案
};

/**
 * @brief 单部雷达辐射源在业务与界面中的完整持久化单元
 *
 * 分别持有轻量结果与完整因子，各自服务不同消费场景：
 * - result：表格列展示（分数、等级文案、代表值）
 * - factors：排序逻辑、图表绘制、详情面板展开
 */
struct RadarThreatAssessRecord
{
    QString equipID;                // 设备ID
    QString entityName;             // 装备名称
    QString typeName;               // 雷达型号

    RadarPerformancePara perfPara;  // 雷达性能参数
    RadarRealWrokPara workPara;     // 雷达实时参数
    RadarTypicalPara typicalPara;   // 雷达典型参数
    RadarThreatFactors factors;     // 完整因子明细（排序/图表/详情消费）
    RadarThreatAssessResult result; // 轻量评估结果（表格直接消费）

    /**
     * @brief 从分数值映射到五档等级文案
     * @param score 威胁分数 S ∈ [0,1]
     * @return "紧急" / "高" / "中" / "低" / "排除"
     */
    static QString threatLevelFromF1(double score)
    {
        if (!std::isfinite(score))
        {
            return QStringLiteral("—");
        }
        if (score >= 0.75) return QStringLiteral("紧急");
        if (score >= 0.55) return QStringLiteral("高");
        if (score >= 0.35) return QStringLiteral("中");
        if (score >= 0.15) return QStringLiteral("低");
        return QStringLiteral("排除");
    }

    /**
     * @brief 实例方法：返回本记录的威胁等级文案
     * @param score 威胁分数
     */
    QString threatLevelText(double score) const
    {
        return threatLevelFromF1(score);
    }
};

// ============================================================================
// 项目公共接口
// ============================================================================
class ProjectPublicInterface
{
public:
    ProjectPublicInterface() = default;
    virtual ~ProjectPublicInterface() = default;

    // ── 通用工具函数 ──
    static QString f1TableCellText(double f1)
    {
        if (!std::isfinite(f1))
        {
            return QStringLiteral("—");
        }
        return QString::number(f1, 'f', 4);
    }

    static QString itemText(const QTableWidget *table, int row, int col)
    {
        const QTableWidgetItem *cellItem = table->item(row, col);
        return cellItem ? cellItem->text().trimmed() : QString();
    }

    static bool parseDouble(const QString &text, double *outValue)
    {
        bool ok = false;
        const double parsed = text.toDouble(&ok);
        if (!ok || outValue == nullptr)
        {
            return false;
        }
        *outValue = parsed;
        return true;
    }

    /**
     * @brief 辐射模式乘子（旧版兼容接口）
     * @note 新版威胁计算使用 calculateThreatResult 内部的辐射状态系数 R，
     *       此函数仅供非核心展示场景使用。
     */
    static double radiationModeMultiplier(int comboIndex)
    {
        switch (comboIndex)
        {
            case 0: return 1.0;   // 全功率
            case 1: return 1.0;   // 正常
            case 2: return 0.85;  // LPI
            case 3: return 0.75;  // 闪避
            case 4: return 0.0;   // 关机
            default: return 1.0;
        }
    }

    static double clamp01(double value)
    {
        if (value < 0.0) return 0.0;
        if (value > 1.0) return 1.0;
        return value;
    }

    static double geometricMean(double intervalLow, double intervalHigh)
    {
        if (intervalLow <= 0.0 || intervalHigh <= 0.0)
        {
            return 0.0;
        }
        return std::sqrt(intervalLow * intervalHigh);
    }

    // ── 内层子因子计算函数 ──

    /**
     * @brief 频率威胁倾向 f_freq
     * @param freq 频率代表值
     * @return ∈ [0,1]，X 波段(8-12GHz)为峰值区，UHF/HF 为低值区
     */
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

    /**
     * @brief 脉宽威胁倾向 f_pw（对数映射）
     * @param pw 脉宽代表值（μs）
     * @return ∈ [0,1]，脉宽越窄（精确引导）威胁越高
     */
    static double pulseWidthThreatFactor(double pw)
    {
        if (pw <= 0.0) return 0.0;
        const double logTau = std::log(pw) / std::log(10.0);
        return clamp01(1.0 - (logTau + 2.0) / 4.0);
    }

    /**
     * @brief PRF 威胁倾向 f_prf（对数映射）
     * @param prf PRF 代表值
     * @return ∈ [0,1]，重频越高（跟踪能力越强）威胁越高
     */
    static double prfThreatFactor(double prf)
    {
        if (prf <= 0.0) return 0.0;
        const double logP = std::log(prf) / std::log(10.0);
        return clamp01((logP - 1.5) / 3.5);
    }

    /**
     * @brief 探测范围距离修正系数
     * @param km  探测距离，<=0 表示无数据
     * @param f1  当前 f1Raw 值（修正幅度与威胁度挂钩）
     * @return 修正系数，远距(>=200km)增强、近距(<=50km)衰减、无数据返回1.0
     */
    static double rangeModifier(double km, double f1)
    {
        if (km <= 0.0)  return 1.0;
        if (km >= 200.0) return 1.0 + 0.08 * f1;
        if (km > 50.0)  return 1.0;
        return 1.0 - 0.10 * f1;
    }

    // ── 外层因子计算函数 ──

    /**
     * @brief 频段匹配度 M(f)
     * @param freq 频率代表值
     * @return ∈ [0,1]，0 表示硬约束（超出导引头覆盖范围）
     */
    static double frequencyMatchFactor(double freq)
    {
        if (freq < GuideHeadBand::kFreqMin || freq > GuideHeadBand::kFreqMax)
        {
            return 0.0;
        }
        if (freq >= GuideHeadBand::kOptMin && freq <= GuideHeadBand::kOptMax)
        {
            return 1.0;
        }
        if (freq < GuideHeadBand::kOptMin)
        {
            return 0.3 + 0.7 * (freq - GuideHeadBand::kFreqMin)
                   / (GuideHeadBand::kOptMin - GuideHeadBand::kFreqMin);
        }
        return 0.3 + 0.7 * (GuideHeadBand::kFreqMax - freq)
               / (GuideHeadBand::kFreqMax - GuideHeadBand::kOptMax);
    }

    /**
     * @brief 辐射状态系数 R
     * @param mode       辐射模式
     * @param powerRatio 功率比（辐射功率/额定功率），<0 表示无功率数据
     * @return ∈ [0,1]，0 表示硬约束（关机）
     */
    static double radiationStateFactor(RadiationMode mode, double powerRatio)
    {
        switch (mode)
        {
            case RadiationMode::Off:
                return 0.0;
            case RadiationMode::Blink:
                return 0.25;
            case RadiationMode::LPI:
                return 0.40 + 0.60 * (powerRatio >= 0.0 ? powerRatio : 0.3);
            case RadiationMode::Normal:
                return 0.70 + 0.30 * (powerRatio >= 0.0 ? powerRatio : 0.7);
            case RadiationMode::FullPower:
                return 1.0;
            default:
                return 0.0;
        }
    }

    /**
     * @brief 阵地火力查表
     * @param defenseComboIndex 界面 combo 索引
     * @return ∈ [0,1]
     */
    static double defenseLevelFactor(int defenseComboIndex)
    {
        switch (static_cast<DefenseLevel>(defenseComboIndex))
        {
            case DefenseLevel::Strong: return 1.00;
            case DefenseLevel::Medium: return 0.65;
            case DefenseLevel::Weak:   return 0.35;
            case DefenseLevel::None:   return 0.00;
            default:                   return 0.00;
        }
    }

    /**
     * @brief 等级映射
     * @param S           威胁分数
     * @param f3Excluded  F3 硬约束标记
     * @return 等级 L1~L5
     */
    static int mapScoreToLevel(double S, bool f3Excluded)
    {
        if (f3Excluded) return 1;
        if (S >= 0.75) return 5;
        if (S >= 0.55) return 4;
        if (S >= 0.35) return 3;
        if (S >= 0.15) return 2;
        return 1;
    }

    // ── 校验函数 ──

    /**
     * @brief 校验雷达性能参数合法性
     * @note 探测距离 <= 0 不作为校验失败条件，视为"无数据"
     */
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
        return true;
    }

    // ========================================================================
    // 核心计算函数
    // ========================================================================

    /**
     * @brief 计算单部雷达辐射源的威胁评估结果（完整版）
     *
     * ━━━ 算法流程 ━━━
     *
     * 内层（始终执行，回答"这个雷达有多危险"）：
     *   1. 校验频率/脉宽/PRF 区间合法性
     *   2. 确定代表值：typicalPara.valid 时使用典型值，否则取区间几何均值
     *   3. 由代表值计算三项子因子：
     *      - freqFactor：分段线性，X 波段(8-12GHz)为峰值区
     *      - pwFactor：对数映射，脉宽越窄威胁越高
     *      - prfFactor：对数映射，重频越高威胁越高
     *   4. 加权合成 f1Raw = 0.40×freq + 0.30×prf + 0.30×pw
     *   5. 距离修正：远距(>=200km)增强、近距(<=50km)衰减、无数据不修正
     *   6. F1 = clamp(f1Raw × rangeMod)
     *
     * 外层（有数据时执行）：
     *   7. F2：阵地火力查表（defenseIndex >= 0 时计算）
     *   8. F3：频段匹配 M(f) × 辐射状态 R(mode, ratio)（radModeIndex >= 0 时计算）
     *      - M=0 或 R=0 触发硬约束标记
     *   9. 动态权重归一化：
     *      - 基础权重 W1=0.50, W2=0.25, W3=0.25
     *      - 缺失因子不参与，剩余权重按比例重新分配
     *  10. 加权求和 S = W1×F1 + W2×F2 + W3×F3
     *  11. 等级映射：硬约束→L1，否则按分数区间映射 L1~L5
     *
     * @param performance  雷达性能参数区间
     * @param typicalPara  用户指定的典型参数（valid 时覆盖几何均值）
     * @param workPara     雷达实时参数（防御等级/辐射模式/功率）
     * @param[out] outFactors  输出：完整因子计算结果（与 Result 分离，可独立传递）
     * @return 轻量评估结果（代表值 + 最终分数 + 等级 + 错误信息）
     */
    static RadarThreatAssessResult calculateThreatResult(RadarThreatAssessRecord &record)
    {
        const auto &performance = record.perfPara;
        const auto &typicalPara = record.typicalPara;
        const auto &workPara = record.workPara;
        auto &outFactors = record.factors;

        // ── STEP 1：校验 ──
        RadarThreatAssessResult result;
        if (!validateRadarInput(performance, &result.errorMsg))
        {
            return result;
        }

        // ── STEP 2：代表值 ──
        if (typicalPara.valid)
        {
            result.freq = typicalPara.freq;
            result.pw = typicalPara.pw;
            result.prf = typicalPara.prf;
        }
        else
        {
            result.freq = geometricMean(performance.freqMin, performance.freqMax);
            result.pw = geometricMean(performance.pwMin, performance.pwMax);
            result.prf = geometricMean(performance.prfMin, performance.prfMax);
        }

        // ── STEP 3：内层子因子 ──
        outFactors.freqFactor = frequencyThreatFactor(result.freq);
        outFactors.prfFactor = prfThreatFactor(result.prf);
        outFactors.pwFactor = pulseWidthThreatFactor(result.pw);

        // ── STEP 4：F1 合成 ──
        constexpr double kW_Freq = 0.40;
        constexpr double kW_Prf = 0.30;
        constexpr double kW_Pw = 0.30;
        outFactors.f1Raw = kW_Freq * outFactors.freqFactor
                + kW_Prf * outFactors.prfFactor
                + kW_Pw * outFactors.pwFactor;

        // ── STEP 5：距离修正 ──
        outFactors.rangeMod = rangeModifier(performance.detectRange, outFactors.f1Raw);
        outFactors.F1 = clamp01(outFactors.f1Raw * outFactors.rangeMod);

        // ── STEP 6：F2 阵地火力（可选） ──
        if (workPara.situationDefenseIndex >= 0)
        {
            outFactors.hasF2 = true;
            outFactors.F2 = defenseLevelFactor(workPara.situationDefenseIndex);
        }

        // ── STEP 7：F3 信号截获可行性（可选） ──
        if (workPara.situationRadModeIndex >= 0)
        {
            outFactors.hasF3 = true;
            const auto mode = static_cast<RadiationMode>(workPara.situationRadModeIndex);

            // 7a. 频段匹配度 M(f)
            outFactors.f3M = frequencyMatchFactor(result.freq);
            if (outFactors.f3M == 0.0)
            {
                outFactors.f3ExcludeReason = (result.freq < GuideHeadBand::kFreqMin)
                        ? QStringLiteral("频段不匹配：低于导引头下限 0.5 GHz")
                        : QStringLiteral("频段不匹配：高于导引头上限 18 GHz");
            }

            // 7b. 辐射状态系数 R（功率比 < 0 表示无功率数据，用模式默认值）
            outFactors.f3R = radiationStateFactor(mode,
                                         (workPara.defaultPower > 0.0 && workPara.workPower > 0.0)
                                         ? std::min(workPara.workPower / workPara.defaultPower, 1.0)
                                         : -1.0);

            if (outFactors.f3R == 0.0 && outFactors.f3ExcludeReason.isEmpty())
            {
                outFactors.f3ExcludeReason = QStringLiteral("雷达关机：无电磁辐射");
            }

            // 7c. 合成 F3 = M × R
            outFactors.F3 = outFactors.f3M * outFactors.f3R;
            outFactors.f3Excluded = (outFactors.f3M == 0.0 || outFactors.f3R == 0.0);
        }

        // ── STEP 8：动态权重归一化 ──
        constexpr double kBaseW1 = 0.50;
        constexpr double kBaseW2 = 0.25;
        constexpr double kBaseW3 = 0.25;

        double wSum = kBaseW1;
        if (outFactors.hasF2)
            wSum += kBaseW2;
        if (outFactors.hasF3)
            wSum += kBaseW3;

        outFactors.W1 = kBaseW1 / wSum;
        outFactors.W2 = outFactors.hasF2 ? kBaseW2 / wSum : 0.0;
        outFactors.W3 = outFactors.hasF3 ? kBaseW3 / wSum : 0.0;

        // ── STEP 9：加权求和 ──
        outFactors.S = outFactors.W1 * outFactors.F1;
        if (outFactors.hasF2)
            outFactors.S += outFactors.W2 * outFactors.F2;
        if (outFactors.hasF3)
            outFactors.S += outFactors.W3 * outFactors.F3;

        // ── STEP 10：等级映射 ──
        outFactors.level = mapScoreToLevel(outFactors.S, outFactors.f3Excluded);

        // ── 写入轻量结果 ──
        result.threatLevel = outFactors.S;
        result.threatGrade = outFactors.level;
        result.valid = true;

        return result;
    }

    // ── 其他辅助函数 ──

    static RadarThreatAssessResult evaluate(const RadarPerformancePara &input)
    {
        RadarThreatAssessResult result;
        if (!validateRadarInput(input, &result.errorMsg))
        {
            return result;
        }
        result.freq = geometricMean(input.freqMin, input.freqMax);
        result.pw = geometricMean(input.pwMin, input.pwMax);
        result.prf = geometricMean(input.prfMin, input.prfMax);
        result.valid = true;
        return result;
    }

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
        const Preset &entry = presetTable[presetIndex - 1];
        performance.freqMin = performance.freqMax = entry.freqGhz;
        performance.pwMin = performance.pwMax = entry.pulseWidthUs;
        performance.prfMin = performance.prfMax = entry.prfHz;
        performance.detectRange = entry.rangeKm;
        return performance;
    }
};