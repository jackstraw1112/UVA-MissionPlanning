//
// Created by admin on "2026.04.09 T 19:10:36".
//

#pragma once

#include <QString>
#include <QPair>
#include <QRectF>

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
