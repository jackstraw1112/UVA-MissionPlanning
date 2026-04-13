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

QT_BEGIN_NAMESPACE
namespace Ui
{
    class RZThreatAssessment;
}
QT_END_NAMESPACE

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

    RadarPerformancePara performance;     // 雷达性能参数区间
    RadarTypicalPara typicalPara;         // 频率/脉宽/PRF 代表值（可独立于区间编辑）
    RadarThreatAssessResult evaluation;   // 最近一次威胁评估数值结果
    
    int situationDefenseIndex = -1;       // 阵地火力 combo 索引；-1 未设则用批量默认
    int situationRadModeIndex = -1;       // 辐射模式 combo 索引；-1 未设则用批量默认

    static QString threatLevelFromF1(double adjustedF1)  // [0,1] 威胁度→五档等级；adjustedF1 含辐射模式等
    {
        double f = adjustedF1;
        if (f < 0.0)
            f = 0.0;
        if (f > 1.0)
            f = 1.0;
        if (f >= 0.8)
            return QStringLiteral("极高");
        if (f >= 0.6)
            return QStringLiteral("高");
        if (f >= 0.4)
            return QStringLiteral("中");
        if (f >= 0.2)
            return QStringLiteral("低");
        return QStringLiteral("极低");
    }

    QString threatLevelText() const;   // 按 evaluation.f1 与辐射模式系数得等级（表格列）
    QString entityCellText() const;  // 「装备实体」列：名称（id），缺一则只显示另一侧
};

/**
 * @brief 雷达威胁评估窗口
 * @details 由性能区间与代表值计算体制威胁指数 F1；表格行与 RadarThreatAssessmentRecord 一一对应，右侧子因子与合成为全局共享。
 */
class RZThreatAssessment : public QWidget
{
    Q_OBJECT

public:
    explicit RZThreatAssessment(QWidget *parent = nullptr);
    ~RZThreatAssessment() override;

    // 根据性能区间按技术方案计算全套中间量与 F1
    static RadarThreatAssessResult evaluate(const RadarPerformancePara &input);

    // 校验 performance 区间是否满足评估前提（正数、min≤max 等）
    static bool validateRadarInput(const RadarPerformancePara &input, QString *errorMessage);

    // 与界面 combo 默认项一致：批量计算时未单独设置态势所用的「中防护」索引
    static int defaultDefenseIndex();

    // 与界面 combo 默认项一致：批量计算时未单独设置时所用的「正常」辐射模式索引（供 RadarThreatAssessmentRecord::threatLevelText 等调用）
    static int defaultRadiationModeIndex();

    // 辐射模式对威胁度的乘性修正；索引与 comboRadiationMode 顺序一致（如关机为 0）
    static double radiationModeMultiplier(int comboIndex);

    // 当前表格中全部雷达辐射源（行号与 vector 下标一致）
    const QVector<RadarThreatAssessmentRecord> &threatRecords() const { return m_radarSources; }

    // 加载内置作战方案：由 generateTestData 填充 m_radarSources 并刷新表格，首行 evaluate 初始化全局子因子与合成
    void loadBattlePlanRadiationSources();

private:
    // 初始化参数
    void initParams();
    // 初始化对象
    void initObject();
    // 关联信号与槽函数
    void initConnect();

    /**
     * @brief 初始化表格属性
     * @details 设置表格属性，如列数、列宽、行高、表头、数据样式等
     */
    void initTableViewAttr();

    /**
     * @brief 生成测试数据
     * @details 用户生成临时数据
     */
    void generateTestData();

private slots:
    // 「计算威胁评估」：读左侧性能与右侧代表值/子因子，更新当前行 evaluation 与表格
    void onCalculateClicked();

    // 态势或功率文案变化：刷新摘要、进度条、有效威胁度，并写回当前行态势索引与表格等级列
    void refreshSituationDisplay();

    // 右侧三个代表值 spin 变化：更新当前行 representative 并重算 evaluation
    void onRepresentativeCentersEdited();

    // 右侧子因子与合成 spin 变化：用当前 GlobalThreatFactors 重算所有行
    void onSubfactorSynthesisEdited();

    void onRemoveSelectedRows();
    void onClearTable();

    // 按表格各行单元格解析 performance，用全局子因子批量 buildThreatResult
    void onCalcAllRadarThreats();

    // 表格当前行变化：同步左侧与右侧代表值并触发一次 onCalculateClicked
    void onTableCurrentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);

private:
    /**
     * @brief 将 m_radarSources 全部刷新到表格（先清空表格再逐行写入）
     */
    void displayDataToTable();

    /**
     * @brief 将单条雷达威胁评估记录显示到表格
     * @param data 雷达威胁评估记录
     * @param row 表格行号；为 -1 时在末尾追加新行并 push_back 到 m_radarSources
     */
    void displayDataToTable(const RadarThreatAssessmentRecord &data, int row = -1);

    // 从第 row 行单元格文本解析出 RadarThreatInput；id/name 来自 m_radarSources[row]
    bool parseInputFromTableRow(int row, RadarPerformancePara *outInput, QString *outId, QString *outName,
                                QString *errorMessage) const;

    // 用新的 performance 与可选预计算 evaluation 写回 m_radarSources[row] 并刷新该行结果列
    void applyEvaluationToRow(int row, const RadarPerformancePara &in, const QString &id, const QString &name,
                              const RadarThreatAssessResult *precomputed = nullptr);

    // 方案中 radarPresetIndex（1..9）对应的默认单值区间 performance
    static RadarPerformancePara radarInputFromPresetIndex(int presetIndex);

    // 用表格第 row 行刷新左侧性能 spin、目标只读行、态势 combo、右侧代表值 spin，并调用 onCalculateClicked
    void loadFormFromTableRow(int row);

    // 批量计算前：若该行态势索引仍为 -1，则填入默认防护与默认辐射模式
    void ensureSituationDefaultsOnRecord(int row);

    // 从左侧只读性能 spin 读取当前显示区间
    RadarPerformancePara readInputFromUi() const;

    // 从右侧三个代表值 spin 读取
    RadarTypicalPara readRepresentativeCentersFromUi() const;

    // 从右侧「子因子与合成」六个 spin 读取
    GlobalThreatFactors readSubfactorSynthesisFromUi() const;

    // 将全局子因子写回右侧 spin（会暂时阻塞 valueChanged）
    void syncSubfactorSynthesisToUi(const GlobalThreatFactors &s);

    // 切换表格行时，用该行 representative 或几何均值等填充右侧三个代表值 spin
    void syncRepresentativeCentersToUi(const RadarTypicalPara &rep, const RadarPerformancePara &in,
                                       const RadarThreatAssessResult &fallbackResult);

    // 在 performance 合法前提下，合并代表值中心与子因子拷贝为一条 RadarThreatResult（供表格与图示）
    static RadarThreatAssessResult buildThreatResult(const RadarPerformancePara &in, const RadarTypicalPara &rep,
                                               const GlobalThreatFactors &synth);

    // 读取 UI 子因子后对每一行 buildThreatResult，刷新 F1/等级/说明列与当前展示
    void applySubfactorSynthesisToAllRows();

    // 将 m_radarSources[row].evaluation 写到表格结果三列
    void pushCurrentRowResultToTable(int row);

    // 根据一次评估结果刷新底部态势摘要、F1 进度条、有效威胁度（F1×辐射模式系数）
    void displayResult(const RadarThreatAssessResult &r);

    static double geometricMean(double a, double b);
    static double clamp01(double v);
    static double frequencyThreatFactor(double fGhz);
    static double pulseWidthThreatFactor(double tauUs);
    static double prfThreatFactor(double prfHz);
    static double rangeModifier(double rangeKm, double f1Raw);

    Ui::RZThreatAssessment *ui;

    // 当前上下文最后一次有效评估结果（底部态势区展示用）
    RadarThreatAssessResult m_lastResult;

    // 表格行持久数据（威胁评估用雷达源）
    QVector<RadarThreatAssessmentRecord> m_radarSources;

    // 程序化选中行时置位，抑制 currentCellChanged 与 loadFormFromTableRow 重入
    bool m_syncingEntityPick = false;

    // setValue 同步右侧 spin 时置位，避免 valueChanged 连锁触发
    bool m_blockResultSpinSignals = false;

    // 右侧「子因子与合成」全局共享数值（与各 buildThreatResult 一致）
    GlobalThreatFactors m_subfactorSynthesis;
};

#endif // RZTHREATASSESSMENT_H
