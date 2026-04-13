//
// Created by admin on "2026.04.11 T 13:41:18".
//
// 雷达威胁评估界面：表格行与 QVector<RadarThreatAssessmentRecord> 一一对应；右侧「子因子与合成」为全局共享的
// GlobalThreatFactors。若后续扩展非雷达辐射源，可新增与 RadarThreatAssessmentRecord 平行的结构体并在界面层分表或分栏承载。
//

#ifndef RZTHREATASSESSMENT_H
#define RZTHREATASSESSMENT_H

#include <QString>
#include <QVector>
#include <QWidget>

#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class RZThreatAssess;
}
QT_END_NAMESPACE

class ThreatAssessFormBridge;

// 雷达性能参数区间
struct RadarPerformancePara
{
    double freqMinGhz = 0.0;            // 工作频率区间下限（GHz）
    double freqMaxGhz = 0.0;            // 工作频率区间上限（GHz）
    double pulseWidthMinUs = 0.0;       // 脉冲宽度区间下限（μs）
    double pulseWidthMaxUs = 0.0;       // 脉冲宽度区间上限（μs）
    double prfMinHz = 0.0;              // 脉冲重复频率区间下限（Hz）
    double prfMaxHz = 0.0;              // 脉冲重复频率区间上限（Hz）
    double detectRangeKm = 0.0;         // 探测距离（km），参与距离修正系数
};

// 单源一次评估的数值结果
struct RadarThreatAssessResult
{
    bool valid = false;                 // 本次评估是否成功（失败时 errorMessage 有效）
    QString errorMessage;               // 评估失败或校验不通过时的说明文案

    double freqCenterGhz = 0.0;         // 频率代表值 / 几何中心（GHz）
    double pulseWidthCenterUs = 0.0;    // 脉宽代表值 / 几何中心（μs）
    double prfCenterHz = 0.0;           // PRF 代表值 / 几何中心（Hz）

    double fFreq = 0.0;                 // 频率子因子
    double fPrf = 0.0;                  // PRF 子因子
    double fPw = 0.0;                   // 脉宽子因子
    double f1Raw = 0.0;                 // 距离修正前加权合成威胁指数
    double rangeMod = 1.0;              // 探测距离修正系数（乘在合成结果上）
    double f1 = 0.0;                    // 体制威胁指数 F1（已含距离修正）
};

// 雷达典型参数
struct RadarTypicalPara
{
    double freqCenterGhz = 0.0;         // 频率代表值（GHz）
    double pulseWidthCenterUs = 0.0;    // 脉宽代表值（μs）
    double prfCenterHz = 0.0;           // PRF 代表值（Hz）
    bool valid = false;                 // 三个代表值是否已有效填充
};

// 子因子与合成
struct GlobalThreatFactors
{
    double fFreq = 0.0;    // 全局频率子因子（与界面 spin 一致）
    double fPrf = 0.0;     // 全局 PRF 子因子
    double fPw = 0.0;      // 全局脉宽子因子
    double f1Raw = 0.0;    // 全局距离修正前合成 f1Raw
    double rangeMod = 1.0; // 全局距离修正系数 rangeMod
    double f1 = 0.0;       // 全局合成威胁指数 F1（写入各行前的共享快照）
};

/**
 * @brief 单部雷达辐射源在业务与界面中的完整持久化单元
 * @details 对应表格一行及左侧表单上下文；含性能区间、代表值、评估结果与态势 combo 索引。
 */
struct RadarThreatAssessmentRecord
{
    QString id;                           // 装备ID（装备名称）
    QString name;                         // 雷达名称
    QString type;                         // 雷达型号

    RadarPerformancePara performance;       // 雷达性能参数区间
    RadarTypicalPara typicalPara;         // 频率/脉宽/PRF 代表值
    RadarThreatAssessResult evaluation;   // 最近一次威胁评估数值结果

    int situationDefenseIndex = -1;       // 阵地火力 combo 索引；-1 未设则用批量默认
    int situationRadModeIndex = -1;       // 辐射模式 combo 索引；-1 未设则用批量默认

    // 「装备实体」列：名称（id），缺一则只显示另一侧
    QString entityCellText() const;

    // [0,1] 威胁度→五档文案；供 threatLevelText 与外部展示复用
    static QString threatLevelFromF1(double adjustedF1);

    // 按 evaluation.f1 与辐射模式乘子得五档等级（表格「威胁等级」列；与底部有效威胁度 F1×系数一致）
    QString threatLevelText() const;
};

/**
 * @brief 雷达威胁评估窗口
 * @details 由性能区间与代表值计算体制威胁指数 F1；表格行与 RadarThreatAssessmentRecord 一一对应，右侧子因子与合成为全局共享。
 *
 * 成员函数在头文件中的顺序与典型调用链一致：
 * 构造 → init* → 表格与演示数据装载 → 评估静态管道 → 左侧/右侧 UI 与表格行同步 → 槽函数（用户操作）。
 */
class RZThreatAssess : public QWidget
{
    Q_OBJECT

public:
    explicit RZThreatAssess(QWidget *parent = nullptr);
    ~RZThreatAssess() override;

    static double radiationModeMultiplier(int comboIndex);

    /** 供 ThreatAssessFormBridge（horizontalLayout_3）与评估管道共用 */
    static double geometricMean(double a, double b);
    static bool validateRadarInput(const RadarPerformancePara &input, QString *errorMessage);

private:
    // 初始化参数
    void initParams();
    // 初始化对象
    void initObject();
    // 关联信号与槽函数
    void initConnect();

    // 表格属性初始化
    void initTableAttr();
    // 表格表头初始化
    void initTableHeader();
    // 生成测试数据
    void generateTestData();
    // 显示数据到表格
    void displayDataToTable();
    // 显示数据到表格（单行）
    void displayDataToTable(const RadarThreatAssessmentRecord &data, int row = -1);
    // 从作战计划加载辐射源
    void loadBattlePlanRadiationSources();

    //  评估静态管道
    static double clamp01(double v);
    // 评估单源
    static double frequencyThreatFactor(double fGhz);
    // 评估单源
    static double pulseWidthThreatFactor(double tauUs);
    // 评估单源
    static double prfThreatFactor(double prfHz);
    // 评估单源
    static double rangeModifier(double rangeKm, double f1Raw);
    // 评估单源
    static RadarThreatAssessResult evaluate(const RadarPerformancePara &input);
    // 评估单源
    static RadarThreatAssessResult buildThreatResult(const RadarPerformancePara &in, const RadarTypicalPara &rep,
                                                     const GlobalThreatFactors &synth);
    // 获取预设输入
    static RadarPerformancePara radarInputFromPresetIndex(int presetIndex);

    void syncSubfactorSynthesisToUi(const GlobalThreatFactors &s);

    // --- 表格行 ↔ m_radarSources（批量重算、写回结果列）---
    void loadFormFromTableRow(int row);
    bool parseInputFromTableRow(int row, RadarPerformancePara *outInput, QString *outId, QString *outName,
                                QString *errorMessage) const;
    void ensureSituationDefaultsOnRecord(int row);
    void applyEvaluationToRow(int row, const RadarPerformancePara &in, const QString &id, const QString &name,
                              const RadarThreatAssessResult *precomputed = nullptr);
    void pushCurrentRowResultToTable(int row);
    void applySubfactorSynthesisToAllRows();

    const QVector<RadarThreatAssessmentRecord> &threatRecords() const { return m_radarSources; }

private slots:
    // --- 槽：用户操作（内部多调用 loadFormFromTableRow / onCalculateClicked / displayResult 等）---
    void onTableCurrentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);
    void onCalculateClicked();
    void refreshSituationDisplay();
    void onRepresentativeCentersEdited();
    void onSubfactorSynthesisEdited();
    void onRemoveSelectedRows();
    void onClearTable();
    void onCalcAllRadarThreats();

private:
    Ui::RZThreatAssess *ui;

    RadarThreatAssessResult m_lastResult;
    QVector<RadarThreatAssessmentRecord> m_radarSources;
    bool m_syncingEntityPick = false;
    GlobalThreatFactors m_subfactorSynthesis;

    std::unique_ptr<ThreatAssessFormBridge> m_form;
};

#endif // RZTHREATASSESSMENT_H
