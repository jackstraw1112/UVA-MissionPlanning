//
// Created by admin on "2026.04.11 T 13:41:18".
//

#include "RZThreatAssessment.h"
#include "ui_RZThreatAssessment.h"

#include <QAbstractItemView>
#include <QBrush>
#include <QColor>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QMessageBox>
#include <QSignalBlocker>
#include <QTableWidgetItem>
#include <QtMath>

#include <algorithm>

// 实现说明：evaluate() 按技术方案由 performance 区间算子因子与 F1；交互路径上多用 buildThreatResult() 将
// 「表格/左侧区间」、各行 representative 与全局 GlobalThreatFactors 组合，写入 RadarThreatAssessmentRecord::evaluation。
namespace
{
    /// 与 initTableViewAttr 表头顺序严格一致
    enum ThreatTableCol
    {
        ColRadarModel = 0,
        ColEquipmentEntity,
        ColFreqMin,
        ColFreqMax,
        ColPwMin,
        ColPwMax,
        ColPrfMin,
        ColPrfMax,
        ColRangeKm,
        ColF1,
        ColLevel,
        ColNote,
        ColCount
    };

    // 表格项：只读、居中；威胁等级列额外着色（与 threatLevelFromF1 五档文案一致）
    static QTableWidgetItem *makeReadOnlyItem(const QString &text)
    {
        auto *it = new QTableWidgetItem(text);
        // 禁止就地编辑，保留可选中/可用，文本居中
        it->setFlags((it->flags() | Qt::ItemIsSelectable | Qt::ItemIsEnabled) & ~Qt::ItemIsEditable);
        it->setTextAlignment(Qt::AlignCenter);
        return it;
    }

    static void applyThreatLevelAppearance(QTableWidgetItem *item)
    {
        if (!item)
            return;
        const QString t = item->text();
        // 占位或无等级：恢复默认配色
        if (t == QStringLiteral("—") || t.isEmpty())
        {
            item->setBackground(QBrush());
            item->setForeground(QBrush());
            return;
        }
        QColor bg;
        QColor fg;
        // 五档等级 → 背景/前景色（与 threatLevelFromF1 文案严格一致）
        if (t == QStringLiteral("极高"))
        {
            bg = QColor(0xc6, 0x28, 0x28);
            fg = Qt::white;
        }
        else if (t == QStringLiteral("高"))
        {
            bg = QColor(0xef, 0x6c, 0x00);
            fg = Qt::white;
        }
        else if (t == QStringLiteral("中"))
        {
            bg = QColor(0xf9, 0xa8, 0x25);
            fg = QColor(0x1a, 0x1a, 0x1a);
        }
        else if (t == QStringLiteral("低"))
        {
            bg = QColor(0x7c, 0xb3, 0x42);
            fg = QColor(0x1a, 0x1a, 0x1a);
        }
        else if (t == QStringLiteral("极低"))
        {
            bg = QColor(0xec, 0xef, 0xf1);
            fg = QColor(0x37, 0x47, 0x4f);
        }
        else
        {
            item->setBackground(QBrush());
            item->setForeground(QBrush());
            return;
        }
        item->setBackground(bg);
        item->setForeground(fg);
    }

    static QTableWidgetItem *makeThreatLevelItem(const QString &text)
    {
        QTableWidgetItem *it = makeReadOnlyItem(text);
        applyThreatLevelAppearance(it);
        return it;
    }

    static QString radarPresetDisplayName(int presetIndex)
    {
        switch (presetIndex)
        {
            case 1:
                return QStringLiteral("MPQ-65 (PAC-3 火控)");
            case 2:
                return QStringLiteral("MPQ-53 (PAC-2 火控)");
            case 3:
                return QStringLiteral("MPN-45 (天弓火控)");
            case 4:
                return QStringLiteral("HR-2000S");
            case 5:
                return QStringLiteral("SPS-48E");
            case 6:
                return QStringLiteral("FPS-117");
            case 7:
                return QStringLiteral("TPS-75");
            case 8:
                return QStringLiteral("JYL-1 (天波)");
            case 9:
                return QStringLiteral("近程高炮火控");
            default:
                return QString();
        }
    }

    static QString itemText(const QTableWidget *table, int row, int col)
    {
        const QTableWidgetItem *it = table->item(row, col);
        return it ? it->text().trimmed() : QString();
    }

    static bool parseDouble(const QString &s, double *out)
    {
        bool ok = false;
        const double v = s.toDouble(&ok); // 表格文本可能含空格，调用方已 trim
        if (!ok || out == nullptr)
            return false;
        *out = v;
        return true;
    }

} // namespace

// --- RadarThreatAssessmentRecord ---
QString RadarThreatAssessmentRecord::entityCellText() const
{
    // 缺省一侧时避免「（）」空壳
    if (name.isEmpty())
        return id;
    if (id.isEmpty())
        return name;
    return QStringLiteral("%1（%2）").arg(name, id);
}

QString RadarThreatAssessmentRecord::threatLevelText() const
{
    if (!evaluation.valid)
        return QStringLiteral("—");
    // 未单独设辐射模式时用界面默认项对应的乘子
    int modeIdx = situationRadModeIndex;
    if (modeIdx < 0)
        modeIdx = RZThreatAssessment::defaultRadiationModeIndex();
    const double mul = RZThreatAssessment::radiationModeMultiplier(modeIdx);
    const double adjusted = qBound(0.0, evaluation.f1 * mul, 1.0);
    return threatLevelFromF1(adjusted);
}

RZThreatAssessment::RZThreatAssessment(QWidget *parent)
    : QWidget(parent), ui(new Ui::RZThreatAssessment)
{
    ui->setupUi(this);

    // 初始化参数
    initParams();

    // 初始化对象
    initObject();

    // 关联信号与槽函数
    initConnect();
}

RZThreatAssessment::~RZThreatAssessment()
{
    delete ui;
}


void RZThreatAssessment::initParams()
{
}

void RZThreatAssessment::initObject()
{
    // 初始化表格属性
    initTableViewAttr();

    // 加载内置作战方案并填充雷达行
    loadBattlePlanRadiationSources();
}

void RZThreatAssessment::initConnect()
{
    // 主操作与表格
    connect(ui->btnCalculate, &QPushButton::clicked, this, &RZThreatAssessment::onCalculateClicked);
    connect(ui->tableThreatList, &QTableWidget::currentCellChanged, this, &RZThreatAssessment::onTableCurrentCellChanged);

    // 态势 combo：摘要、有效威胁度、当前行等级列
    connect(ui->comboRadiationMode, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &RZThreatAssessment::refreshSituationDisplay);
    connect(ui->comboPositionDefense, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &RZThreatAssessment::refreshSituationDisplay);
    connect(ui->btnRemoveRows, &QPushButton::clicked, this, &RZThreatAssessment::onRemoveSelectedRows);
    connect(ui->btnClearTable, &QPushButton::clicked, this, &RZThreatAssessment::onClearTable);
    connect(ui->btnCalcAllRadarThreats, &QPushButton::clicked, this, &RZThreatAssessment::onCalcAllRadarThreats);

    // 右侧三个代表值：只驱动当前行
    connect(ui->spinResultFreqCenter, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &RZThreatAssessment::onRepresentativeCentersEdited);
    connect(ui->spinResultPwCenter, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &RZThreatAssessment::onRepresentativeCentersEdited);
    connect(ui->spinResultPrfCenter, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &RZThreatAssessment::onRepresentativeCentersEdited);

    // 子因子与合成：驱动全部行批量重算
    connect(ui->spinResultFFreq, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &RZThreatAssessment::onSubfactorSynthesisEdited);
    connect(ui->spinResultFPrf, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &RZThreatAssessment::onSubfactorSynthesisEdited);
    connect(ui->spinResultFPw, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &RZThreatAssessment::onSubfactorSynthesisEdited);
    connect(ui->spinResultF1Raw, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &RZThreatAssessment::onSubfactorSynthesisEdited);
    connect(ui->spinResultRangeMod, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &RZThreatAssessment::onSubfactorSynthesisEdited);
    connect(ui->spinResultF1, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &RZThreatAssessment::onSubfactorSynthesisEdited);
}

void RZThreatAssessment::onRemoveSelectedRows()
{
    QTableWidget *t = ui->tableThreatList;
    const QModelIndexList sel = t->selectionModel()->selectedRows();
    if (sel.isEmpty())
    {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先选中要删除的行。"));
        return;
    }

    QList<int> rows;
    for (const QModelIndex &idx : sel)
        rows.append(idx.row());
    // 自大到小删行，避免删除后行号整体前移导致错位
    std::sort(rows.begin(), rows.end(), [](int a, int b)
    {
        return a > b;
    });

    for (int r : rows)
    {
        t->removeRow(r);
        if (r >= 0 && r < m_radarSources.size())
            m_radarSources.removeAt(r);
    }

    if (!m_radarSources.isEmpty())
    {
        // 选中第 0 行会触发 currentCellChanged，用标志抑制重入
        m_syncingEntityPick = true;
        ui->tableThreatList->selectRow(0);
        m_syncingEntityPick = false;
        loadFormFromTableRow(0);
    }
    else
    {
        ui->editRadarModel->clear();
        ui->editEquipmentEntity->clear();
        m_lastResult = RadarThreatAssessResult();
        displayResult(m_lastResult);
    }
}

void RZThreatAssessment::onClearTable()
{
    m_radarSources.clear();
    ui->tableThreatList->setRowCount(0); // 不逐 removeRow，直接清空更高效
    ui->editEquipmentEntity->clear();
    ui->editRadarModel->clear();
    m_lastResult = RadarThreatAssessResult();
    displayResult(m_lastResult);
}

// --- 态势默认值与辐射模式系数（与 combo 项顺序绑定） ---
int RZThreatAssessment::defaultDefenseIndex()
{
    return 1; // 与 comboPositionDefense「中防护」默认项下标一致
}

int RZThreatAssessment::defaultRadiationModeIndex()
{
    return 1; // 与 comboRadiationMode「正常」默认项下标一致
}

double RZThreatAssessment::radiationModeMultiplier(int comboIndex)
{
    // 与 ui 中 comboRadiationMode 条目顺序一致；关机等待定为 0 威胁乘子
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

// --- 预设雷达体制 → 默认 RadarThreatInput ---

RadarPerformancePara RZThreatAssessment::radarInputFromPresetIndex(int presetIndex)
{
    RadarPerformancePara in;
    // 演示用：每种预设对应一组单值区间（上下限相同）
    struct Preset
    {
        double f;
        double pw;
        double prf;
        double r;
    };
    static const Preset table[] = {
            {5.5, 0.7, 15000.0, 150.0},
            {5.5, 1.0, 10000.0, 120.0},
            {4.5, 0.8, 12000.0, 150.0},
            {3.0, 2.0, 5000.0, 250.0},
            {4.5, 1.5, 8000.0, 370.0},
            {1.3, 50.0, 300.0, 470.0},
            {3.1, 10.0, 600.0, 440.0},
            {0.02, 200.0, 50.0, 1000.0},
            {15.0, 0.05, 50000.0, 18.0},
    };
    static const int kPresetCount = static_cast<int>(sizeof(table) / sizeof(table[0]));
    if (presetIndex < 1 || presetIndex > kPresetCount)
        return in;
    const Preset &p = table[presetIndex - 1]; // 预设编号 1..kPresetCount
    in.freqMinGhz = in.freqMaxGhz = p.f;
    in.pulseWidthMinUs = in.pulseWidthMaxUs = p.pw;
    in.prfMinHz = in.prfMaxHz = p.prf;
    in.detectRangeKm = p.r;
    return in;
}

// --- 当前行 ↔ 左侧表单 / 右侧代表值 ---
void RZThreatAssessment::loadFormFromTableRow(int row)
{
    if (row < 0 || row >= ui->tableThreatList->rowCount())
        return;

    const QTableWidget *t = ui->tableThreatList;
    // 性能区间：从表格单元格解析后写入左侧 spin（若解析失败则保留 spin 原值）
    double v = 0.0;
    if (parseDouble(itemText(t, row, ColFreqMin), &v))
        ui->spinFreqMin->setValue(v);
    if (parseDouble(itemText(t, row, ColFreqMax), &v))
        ui->spinFreqMax->setValue(v);
    if (parseDouble(itemText(t, row, ColPwMin), &v))
        ui->spinPwMin->setValue(v);
    if (parseDouble(itemText(t, row, ColPwMax), &v))
        ui->spinPwMax->setValue(v);
    if (parseDouble(itemText(t, row, ColPrfMin), &v))
        ui->spinPrfMin->setValue(v);
    if (parseDouble(itemText(t, row, ColPrfMax), &v))
        ui->spinPrfMax->setValue(v);
    if (parseDouble(itemText(t, row, ColRangeKm), &v))
        ui->spinRangeKm->setValue(v);

    if (row < m_radarSources.size())
    {
        const RadarThreatAssessmentRecord &rec = m_radarSources.at(row);
        ui->editRadarModel->setText(rec.type);
        ui->editEquipmentEntity->setText(rec.entityCellText());
        // 代表值 spin：优先记录内 representative，否则用 evaluation 或区间几何均值
        syncRepresentativeCentersToUi(rec.typicalPara, readInputFromUi(), rec.evaluation);
        if (rec.situationDefenseIndex >= 0)
            ui->comboPositionDefense->setCurrentIndex(rec.situationDefenseIndex);
        else
            ui->comboPositionDefense->setCurrentIndex(defaultDefenseIndex());
        if (rec.situationRadModeIndex >= 0)
            ui->comboRadiationMode->setCurrentIndex(rec.situationRadModeIndex);
        else
            ui->comboRadiationMode->setCurrentIndex(defaultRadiationModeIndex());
    }
    else
    {
        ui->editRadarModel->clear();
        ui->editEquipmentEntity->clear();
    }

    // 同步后按当前表单与全局子因子重算本行并刷新底部展示
    onCalculateClicked();
}

void RZThreatAssessment::onTableCurrentCellChanged(int currentRow, int, int, int)
{
    if (m_syncingEntityPick)
        return;
    if (currentRow < 0)
    {
        ui->editEquipmentEntity->clear();
        return;
    }
    loadFormFromTableRow(currentRow);
}

void RZThreatAssessment::displayDataToTable()
{
    QTableWidget *t = ui->tableThreatList;
    t->setRowCount(0);
    for (int i = 0; i < m_radarSources.size(); ++i)
        displayDataToTable(m_radarSources.at(i), i);
}

// --- 作战方案演示数据：m_radarSources 与表格同步，首行结果初始化全局子因子 ---
void RZThreatAssessment::loadBattlePlanRadiationSources()
{
    // 生成测试数据
    generateTestData();

    // 清空表格并按 m_radarSources 逐行显示
    displayDataToTable();

    if (!m_radarSources.isEmpty())
    {
        // 用首行 evaluate 的子因子快照初始化全局「子因子与合成」
        const RadarThreatAssessResult &r0 = m_radarSources[0].evaluation;
        m_subfactorSynthesis.fFreq = r0.fFreq;
        m_subfactorSynthesis.fPrf = r0.fPrf;
        m_subfactorSynthesis.fPw = r0.fPw;
        m_subfactorSynthesis.f1Raw = r0.f1Raw;
        m_subfactorSynthesis.rangeMod = r0.rangeMod;
        m_subfactorSynthesis.f1 = r0.f1;
        syncSubfactorSynthesisToUi(m_subfactorSynthesis);

        // 各行在统一全局子因子下重建 evaluation，保证表格 F1 与右侧 spin 一致
        for (int i = 0; i < m_radarSources.size(); ++i)
        {
            RadarThreatAssessResult r = buildThreatResult(m_radarSources[i].performance, m_radarSources[i].typicalPara,
                                                          m_subfactorSynthesis);
            if (r.valid)
            {
                m_radarSources[i].evaluation = r;
            }
            pushCurrentRowResultToTable(i);
        }
        ui->tableThreatList->selectRow(0);
    }
    else
    {
        ui->editRadarModel->clear();
        ui->editEquipmentEntity->clear();
        m_lastResult = RadarThreatAssessResult();
        displayResult(m_lastResult);
    }
}

void RZThreatAssessment::onCalcAllRadarThreats()
{
    if (m_radarSources.isEmpty())
    {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("表格中无数据，请先加载作战方案。"));
        return;
    }

    // 右侧六个 spin 作为全体行共用的子因子与合成快照
    m_subfactorSynthesis = readSubfactorSynthesisFromUi();

    int done = 0;
    for (int row = 0; row < m_radarSources.size(); ++row)
    {
        ensureSituationDefaultsOnRecord(row);
        RadarPerformancePara in;
        QString id;
        QString name;
        QString err;
        if (!parseInputFromTableRow(row, &in, &id, &name, &err))
        {
            QMessageBox::warning(this, QStringLiteral("参数错误"),
                                 QStringLiteral("第 %1 行：%2").arg(row + 1).arg(err));
            continue;
        }
        // 代表值取各行已存 representative，不读当前 UI 行
        RadarThreatAssessResult r = buildThreatResult(in, m_radarSources[row].typicalPara, m_subfactorSynthesis);
        if (!r.valid)
        {
            QMessageBox::warning(this, QStringLiteral("参数错误"),
                                 QStringLiteral("第 %1 行：%2").arg(row + 1).arg(r.errorMessage));
            continue;
        }
        applyEvaluationToRow(row, in, id, name, &r);
        ++done;
    }

    QMessageBox::information(
            this, QStringLiteral("完成"),
            QStringLiteral("已重算全部行（共 %1 行）。代表值按各行已存数据；子因子与合成使用当前右侧全局数值（全体一致）。"
                    "威胁等级按「F1×辐射模式系数」映射。")
            .arg(done));
}

// --- 威胁度分段函数与 F1 管线（与详细技术方案一致） ---
double RZThreatAssessment::geometricMean(double a, double b)
{
    // 区间上下限的几何中心；非正区间不参与开方
    if (a <= 0.0 || b <= 0.0)
        return 0.0;
    return qSqrt(a * b);
}

double RZThreatAssessment::clamp01(double v)
{
    if (v < 0.0)
        return 0.0;
    if (v > 1.0)
        return 1.0;
    return v;
}

double RZThreatAssessment::frequencyThreatFactor(double fGhz)
{
    const double f = fGhz;
    // 分段线性：低频弱威胁，中高频升高，极高频略回落（与方案曲线一致）
    if (f <= 0.1)
        return 0.05;
    if (f <= 1.0)
        return 0.15;
    if (f <= 4.0)
        return 0.15 + 0.30 * (f - 1.0) / 3.0;
    if (f <= 8.0)
        return 0.45 + 0.25 * (f - 4.0) / 4.0;
    if (f <= 12.0)
        return 0.70 + 0.20 * (f - 8.0) / 4.0;
    if (f <= 18.0)
        return 0.90 - 0.30 * (f - 12.0) / 6.0;
    return 0.40;
}

double RZThreatAssessment::pulseWidthThreatFactor(double tauUs)
{
    if (tauUs <= 0.0)
        return 0.0;

    // 对数域线性再压到 [0,1]
    const double logTau = qLn(tauUs) / qLn(10.0);
    const double raw = 1.0 - (logTau + 2.0) / 4.0;
    return clamp01(raw);
}

double RZThreatAssessment::prfThreatFactor(double prfHz)
{
    if (prfHz <= 0.0)
        return 0.0;
    const double logP = qLn(prfHz) / qLn(10.0);
    const double raw = (logP - 1.5) / 3.5;
    return clamp01(raw);
}

double RZThreatAssessment::rangeModifier(double rangeKm, double f1Raw)
{
    // 远距略放大、中距不变、近距略压制（与 f1Raw 耦合）
    if (rangeKm >= 200.0)
        return 1.0 + 0.08 * f1Raw;
    if (rangeKm > 50.0)
        return 1.0;
    return 1.0 - 0.10 * f1Raw;
}

bool RZThreatAssessment::validateRadarInput(const RadarPerformancePara &input, QString *errorMessage)
{
    // 各区间为正当、min≤max；探测距离单独要求 >0
    if (input.freqMinGhz <= 0.0 || input.freqMaxGhz <= 0.0
        || input.freqMinGhz > input.freqMaxGhz)
    {
        if (errorMessage)
            *errorMessage = QStringLiteral("频率范围无效：请填写正数且最小值不大于最大值（单值时请将上下限设为相同）。");
        return false;
    }
    if (input.pulseWidthMinUs <= 0.0 || input.pulseWidthMaxUs <= 0.0
        || input.pulseWidthMinUs > input.pulseWidthMaxUs)
    {
        if (errorMessage)
            *errorMessage = QStringLiteral("脉宽范围无效。");
        return false;
    }
    if (input.prfMinHz <= 0.0 || input.prfMaxHz <= 0.0 || input.prfMinHz > input.prfMaxHz)
    {
        if (errorMessage)
            *errorMessage = QStringLiteral("PRF 范围无效。");
        return false;
    }
    if (input.detectRangeKm <= 0.0)
    {
        if (errorMessage)
            *errorMessage = QStringLiteral("探测距离须为大于 0 的数值（km）。");
        return false;
    }
    return true;
}

/// 不重新计算子因子：把 UI 上的全局因子与代表值中心直接写入结果（供人工微调演示）
RadarThreatAssessResult RZThreatAssessment::buildThreatResult(const RadarPerformancePara &in,
                                                              const RadarTypicalPara &rep,
                                                              const GlobalThreatFactors &g)
{
    RadarThreatAssessResult out;
    if (!validateRadarInput(in, &out.errorMessage))
        return out;

    // 代表值优先用户编辑；否则用区间几何均值
    if (rep.valid)
    {
        out.freqCenterGhz = rep.freqCenterGhz;
        out.pulseWidthCenterUs = rep.pulseWidthCenterUs;
        out.prfCenterHz = rep.prfCenterHz;
    }
    else
    {
        out.freqCenterGhz = geometricMean(in.freqMinGhz, in.freqMaxGhz);
        out.pulseWidthCenterUs = geometricMean(in.pulseWidthMinUs, in.pulseWidthMaxUs);
        out.prfCenterHz = geometricMean(in.prfMinHz, in.prfMaxHz);
    }

    // 子因子与 F1 直接取自全局 g（不重新跑 frequencyThreatFactor 等）
    out.fFreq = g.fFreq;
    out.fPrf = g.fPrf;
    out.fPw = g.fPw;
    out.f1Raw = g.f1Raw;
    out.rangeMod = g.rangeMod;
    out.f1 = clamp01(g.f1);

    out.valid = true;
    return out;
}

/// 由输入区间自动算子因子、加权 f1Raw、距离修正与 F1（无 UI 侧全局因子）
RadarThreatAssessResult RZThreatAssessment::evaluate(const RadarPerformancePara &input)
{
    RadarThreatAssessResult out;
    if (!validateRadarInput(input, &out.errorMessage))
        return out;

    out.freqCenterGhz = geometricMean(input.freqMinGhz, input.freqMaxGhz);
    out.pulseWidthCenterUs = geometricMean(input.pulseWidthMinUs, input.pulseWidthMaxUs);
    out.prfCenterHz = geometricMean(input.prfMinHz, input.prfMaxHz);

    // 由代表中心计算三个子因子
    out.fFreq = frequencyThreatFactor(out.freqCenterGhz);
    out.fPw = pulseWidthThreatFactor(out.pulseWidthCenterUs);
    out.fPrf = prfThreatFactor(out.prfCenterHz);

    // 加权合成 → 距离修正 → 压到 [0,1] 得 F1
    out.f1Raw = 0.40 * out.fFreq + 0.30 * out.fPrf + 0.30 * out.fPw;
    out.rangeMod = rangeModifier(input.detectRangeKm, out.f1Raw);
    out.f1 = clamp01(out.f1Raw * out.rangeMod);

    out.valid = true;
    return out;
}

// --- 右侧「评估结果」控件读写 ---

RadarTypicalPara RZThreatAssessment::readRepresentativeCentersFromUi() const
{
    RadarTypicalPara r;
    // 从右侧「评估结果」区三个代表值 spin 读取
    r.freqCenterGhz = ui->spinResultFreqCenter->value();
    r.pulseWidthCenterUs = ui->spinResultPwCenter->value();
    r.prfCenterHz = ui->spinResultPrfCenter->value();
    r.valid = true;
    return r;
}

GlobalThreatFactors RZThreatAssessment::readSubfactorSynthesisFromUi() const
{
    GlobalThreatFactors g;
    // 子因子与合成共六个 spin
    g.fFreq = ui->spinResultFFreq->value();
    g.fPrf = ui->spinResultFPrf->value();
    g.fPw = ui->spinResultFPw->value();
    g.f1Raw = ui->spinResultF1Raw->value();
    g.rangeMod = ui->spinResultRangeMod->value();
    g.f1 = ui->spinResultF1->value();
    return g;
}

void RZThreatAssessment::syncSubfactorSynthesisToUi(const GlobalThreatFactors &g)
{
    m_subfactorSynthesis = g;
    // 避免 setValue 触发 valueChanged → 递归重算
    m_blockResultSpinSignals = true;
    ui->spinResultFFreq->setValue(g.fFreq);
    ui->spinResultFPrf->setValue(g.fPrf);
    ui->spinResultFPw->setValue(g.fPw);
    ui->spinResultF1Raw->setValue(g.f1Raw);
    ui->spinResultRangeMod->setValue(g.rangeMod);
    ui->spinResultF1->setValue(g.f1);
    m_blockResultSpinSignals = false;
}

void RZThreatAssessment::syncRepresentativeCentersToUi(const RadarTypicalPara &rep, const RadarPerformancePara &in,
                                                       const RadarThreatAssessResult &fallbackResult)
{
    m_blockResultSpinSignals = true;
    // 优先级：已存代表值 > 上次评估中心 > 区间几何均值 > 清零
    if (rep.valid)
    {
        ui->spinResultFreqCenter->setValue(rep.freqCenterGhz);
        ui->spinResultPwCenter->setValue(rep.pulseWidthCenterUs);
        ui->spinResultPrfCenter->setValue(rep.prfCenterHz);
    }
    else if (fallbackResult.valid)
    {
        ui->spinResultFreqCenter->setValue(fallbackResult.freqCenterGhz);
        ui->spinResultPwCenter->setValue(fallbackResult.pulseWidthCenterUs);
        ui->spinResultPrfCenter->setValue(fallbackResult.prfCenterHz);
    }
    else if (validateRadarInput(in, nullptr))
    {
        ui->spinResultFreqCenter->setValue(geometricMean(in.freqMinGhz, in.freqMaxGhz));
        ui->spinResultPwCenter->setValue(geometricMean(in.pulseWidthMinUs, in.pulseWidthMaxUs));
        ui->spinResultPrfCenter->setValue(geometricMean(in.prfMinHz, in.prfMaxHz));
    }
    else
    {
        ui->spinResultFreqCenter->setValue(0.0);
        ui->spinResultPwCenter->setValue(0.0);
        ui->spinResultPrfCenter->setValue(0.0);
    }
    m_blockResultSpinSignals = false;
}

// --- 将 m_radarSources[row].evaluation 反映到表格 ---

void RZThreatAssessment::pushCurrentRowResultToTable(int row)
{
    if (row < 0 || row >= ui->tableThreatList->rowCount() || row >= m_radarSources.size())
        return;
    QTableWidget *tw = ui->tableThreatList;
    const RadarThreatAssessmentRecord &rec = m_radarSources.at(row);
    // F1 / 威胁等级 / 说明三列与 evaluation 及辐射模式一致
    if (rec.evaluation.valid)
    {
        tw->setItem(row, ColF1, makeReadOnlyItem(QString::number(rec.evaluation.f1, 'f', 4)));
        tw->setItem(row, ColLevel, makeThreatLevelItem(rec.threatLevelText()));
        tw->setItem(row, ColNote, makeReadOnlyItem(QStringLiteral("有效")));
    }
    else
    {
        tw->setItem(row, ColF1, makeReadOnlyItem(QString()));
        tw->setItem(row, ColLevel, makeThreatLevelItem(QStringLiteral("—")));
        tw->setItem(row, ColNote,
                    makeReadOnlyItem(rec.evaluation.errorMessage.isEmpty()
                            ? QStringLiteral("无效")
                            : rec.evaluation.errorMessage));
    }
}

void RZThreatAssessment::applySubfactorSynthesisToAllRows()
{
    if (m_radarSources.isEmpty())
        return;
    m_subfactorSynthesis = readSubfactorSynthesisFromUi();
    for (int row = 0; row < m_radarSources.size(); ++row)
    {
        RadarThreatAssessResult r = buildThreatResult(m_radarSources[row].performance, m_radarSources[row].typicalPara,
                                                      m_subfactorSynthesis);
        if (!r.valid)
            continue;
        m_radarSources[row].evaluation = r;
        pushCurrentRowResultToTable(row);
    }
    const int cr = ui->tableThreatList->currentRow();
    // 底部展示：优先当前行结果，否则取首个有效行
    if (cr >= 0 && cr < m_radarSources.size() && m_radarSources[cr].evaluation.valid)
        m_lastResult = m_radarSources[cr].evaluation;
    else
    {
        for (const RadarThreatAssessmentRecord &rec : m_radarSources)
        {
            if (rec.evaluation.valid)
            {
                m_lastResult = rec.evaluation;
                break;
            }
        }
    }
    displayResult(m_lastResult);
}

// --- 槽：代表值或子因子与合成编辑 ---

void RZThreatAssessment::onRepresentativeCentersEdited()
{
    if (m_blockResultSpinSignals)
        return;
    const int row = ui->tableThreatList->currentRow();
    if (row < 0 || row >= m_radarSources.size())
        return;
    // 仅重算当前行，不改变其他行的 representative
    m_radarSources[row].typicalPara = readRepresentativeCentersFromUi();
    const RadarPerformancePara in = readInputFromUi();
    m_subfactorSynthesis = readSubfactorSynthesisFromUi();
    RadarThreatAssessResult r = buildThreatResult(in, m_radarSources[row].typicalPara, m_subfactorSynthesis);
    if (!r.valid)
        return;
    m_radarSources[row].evaluation = r;
    m_lastResult = r;
    pushCurrentRowResultToTable(row);
    displayResult(r);
}

void RZThreatAssessment::onSubfactorSynthesisEdited()
{
    if (m_blockResultSpinSignals)
        return;
    applySubfactorSynthesisToAllRows();
}

void RZThreatAssessment::initTableViewAttr()
{
    auto pTable = ui->tableThreatList;
    pTable->setColumnCount(ColCount);
    // 列顺序与匿名命名空间 ThreatTableCol 枚举一致
    const QStringList headers = {
            QStringLiteral("雷达型号"),
            QStringLiteral("装备实体"),
            QStringLiteral("频率下限(GHz)"),
            QStringLiteral("频率上限(GHz)"),
            QStringLiteral("脉宽下限(μs)"),
            QStringLiteral("脉宽上限(μs)"),
            QStringLiteral("PRF下限(Hz)"),
            QStringLiteral("PRF上限(Hz)"),
            QStringLiteral("探测距离(千米)"),
            QStringLiteral("体制威胁指数F1"),
            QStringLiteral("威胁等级"),
            QStringLiteral("说明"),
    };
    pTable->setHorizontalHeaderLabels(headers);
    pTable->horizontalHeader()->setStretchLastSection(true);
    pTable->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    pTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void RZThreatAssessment::generateTestData()
{
    // 清空数据
    m_radarSources.clear();

    const auto addRadar = [this](const QString &id, const QString &name, int presetIndex)
    {
        RadarThreatAssessmentRecord rec;
        rec.id = id;
        rec.name = name;
        rec.type = radarPresetDisplayName(presetIndex);
        rec.performance = radarInputFromPresetIndex(presetIndex);
        rec.evaluation = evaluate(rec.performance);
        rec.typicalPara.freqCenterGhz = rec.evaluation.freqCenterGhz;
        rec.typicalPara.pulseWidthCenterUs = rec.evaluation.pulseWidthCenterUs;
        rec.typicalPara.prfCenterHz = rec.evaluation.prfCenterHz;
        rec.typicalPara.valid = true;
        rec.situationDefenseIndex = -1;
        rec.situationRadModeIndex = -1;
        m_radarSources.push_back(rec);
    };

    // 雷达辐射源
    addRadar(QStringLiteral("RAD-001"), QStringLiteral("AN/MPQ-53 相控阵雷达"), 2);
    addRadar(QStringLiteral("RAD-002"), QStringLiteral("爱国者 MPQ-65"), 1);
    addRadar(QStringLiteral("RAD-003"), QStringLiteral("P-18 预警雷达"), 5);
    addRadar(QStringLiteral("RAD-004"), QStringLiteral("MPQ-64 哨兵雷达"), 7);
    addRadar(QStringLiteral("RAD-005"), QStringLiteral("远程预警 FPS-117"), 6);
}

void RZThreatAssessment::displayDataToTable(const RadarThreatAssessmentRecord &data, int row)
{
    QTableWidget *t = ui->tableThreatList;
    int targetRow = 0;

    // row < 0：追加新行（与 RZSourceRadiation::writeModelRow 语义一致）
    if (row < 0)
    {
        targetRow = t->rowCount();
        t->insertRow(targetRow);
        m_radarSources.push_back(data);
    }
    else
    {
        targetRow = row;
        while (t->rowCount() <= targetRow)
            t->insertRow(t->rowCount());
        while (m_radarSources.size() <= targetRow)
            m_radarSources.push_back(RadarThreatAssessmentRecord());
        m_radarSources[targetRow] = data;
    }

    const RadarPerformancePara &in = data.performance;

    // 性能列来自 data.performance；结果列来自 data.evaluation
    t->setItem(targetRow, ColRadarModel, makeReadOnlyItem(data.type));
    t->setItem(targetRow, ColEquipmentEntity, makeReadOnlyItem(data.entityCellText()));
    t->setItem(targetRow, ColFreqMin, makeReadOnlyItem(QString::number(in.freqMinGhz, 'f', 4)));
    t->setItem(targetRow, ColFreqMax, makeReadOnlyItem(QString::number(in.freqMaxGhz, 'f', 4)));
    t->setItem(targetRow, ColPwMin, makeReadOnlyItem(QString::number(in.pulseWidthMinUs, 'f', 4)));
    t->setItem(targetRow, ColPwMax, makeReadOnlyItem(QString::number(in.pulseWidthMaxUs, 'f', 4)));
    t->setItem(targetRow, ColPrfMin, makeReadOnlyItem(QString::number(in.prfMinHz, 'f', 2)));
    t->setItem(targetRow, ColPrfMax, makeReadOnlyItem(QString::number(in.prfMaxHz, 'f', 2)));
    t->setItem(targetRow, ColRangeKm, makeReadOnlyItem(QString::number(in.detectRangeKm, 'f', 2)));

    if (data.evaluation.valid)
    {
        t->setItem(targetRow, ColF1, makeReadOnlyItem(QString::number(data.evaluation.f1, 'f', 4)));
        t->setItem(targetRow, ColLevel, makeThreatLevelItem(data.threatLevelText()));
        t->setItem(targetRow, ColNote, makeReadOnlyItem(QStringLiteral("有效")));
    }
    else
    {
        t->setItem(targetRow, ColF1, makeReadOnlyItem(QString()));
        t->setItem(targetRow, ColLevel, makeThreatLevelItem(QStringLiteral("—")));
        t->setItem(targetRow, ColNote,
                    makeReadOnlyItem(data.evaluation.errorMessage.isEmpty()
                            ? QStringLiteral("无效")
                            : data.evaluation.errorMessage));
    }
}

// --- 从表格文本解析输入（批量重算 / 校验用） ---
bool RZThreatAssessment::parseInputFromTableRow(int row, RadarPerformancePara *outInput, QString *outId,
                                                QString *outName, QString *errorMessage) const
{
    const QTableWidget *t = ui->tableThreatList;
    if (row < 0 || row >= t->rowCount())
    {
        if (errorMessage)
            *errorMessage = QStringLiteral("行号无效");
        return false;
    }

    if (row >= 0 && row < m_radarSources.size())
    {
        if (outId)
            *outId = m_radarSources.at(row).id;
        if (outName)
            *outName = m_radarSources.at(row).name;
    }
    else
    {
        if (outId)
            *outId = QString();
        if (outName)
            *outName = QString();
    }

    RadarPerformancePara in;
    double v = 0.0;
    // 逐列解析数值，再交给 validateRadarInput 做区间合法性检查
    if (!parseDouble(itemText(t, row, ColFreqMin), &v))
    {
        if (errorMessage)
            *errorMessage = QStringLiteral("频率下限不是有效数字");
        return false;
    }
    in.freqMinGhz = v;
    if (!parseDouble(itemText(t, row, ColFreqMax), &v))
    {
        if (errorMessage)
            *errorMessage = QStringLiteral("频率上限不是有效数字");
        return false;
    }
    in.freqMaxGhz = v;
    if (!parseDouble(itemText(t, row, ColPwMin), &v))
    {
        if (errorMessage)
            *errorMessage = QStringLiteral("脉宽下限不是有效数字");
        return false;
    }
    in.pulseWidthMinUs = v;
    if (!parseDouble(itemText(t, row, ColPwMax), &v))
    {
        if (errorMessage)
            *errorMessage = QStringLiteral("脉宽上限不是有效数字");
        return false;
    }
    in.pulseWidthMaxUs = v;
    if (!parseDouble(itemText(t, row, ColPrfMin), &v))
    {
        if (errorMessage)
            *errorMessage = QStringLiteral("PRF_min 不是有效数字");
        return false;
    }
    in.prfMinHz = v;
    if (!parseDouble(itemText(t, row, ColPrfMax), &v))
    {
        if (errorMessage)
            *errorMessage = QStringLiteral("PRF_max 不是有效数字");
        return false;
    }
    in.prfMaxHz = v;
    if (!parseDouble(itemText(t, row, ColRangeKm), &v))
    {
        if (errorMessage)
            *errorMessage = QStringLiteral("探测距离不是有效数字");
        return false;
    }
    in.detectRangeKm = v;

    QString verr;
    if (!validateRadarInput(in, &verr))
    {
        if (errorMessage)
            *errorMessage = verr;
        return false;
    }
    if (outInput)
        *outInput = in;
    return true;
}

void RZThreatAssessment::ensureSituationDefaultsOnRecord(int row)
{
    if (row < 0 || row >= m_radarSources.size())
        return;
    // 批量重算前把「未单独设置」的行统一到界面默认 combo 索引
    if (m_radarSources[row].situationDefenseIndex < 0)
        m_radarSources[row].situationDefenseIndex = defaultDefenseIndex();
    if (m_radarSources[row].situationRadModeIndex < 0)
        m_radarSources[row].situationRadModeIndex = defaultRadiationModeIndex();
}

void RZThreatAssessment::applyEvaluationToRow(int row, const RadarPerformancePara &in, const QString &id,
                                              const QString &name, const RadarThreatAssessResult *precomputed)
{
    QTableWidget *t = ui->tableThreatList;
    if (row < 0 || row >= t->rowCount() || row >= m_radarSources.size())
        return;

    RadarThreatAssessmentRecord rec;
    rec.id = id;
    rec.name = name;
    rec.performance = in;
    const RadarThreatAssessmentRecord &prev = m_radarSources.at(row);
    // 型号、代表值、态势索引沿用行内原记录
    rec.type = prev.type;
    rec.typicalPara = prev.typicalPara;
    rec.situationDefenseIndex = prev.situationDefenseIndex;
    rec.situationRadModeIndex = prev.situationRadModeIndex;
    if (precomputed)
        rec.evaluation = *precomputed;
    else
        rec.evaluation = buildThreatResult(in, rec.typicalPara, readSubfactorSynthesisFromUi());
    m_radarSources[row] = rec;

    t->setItem(row, ColRadarModel, makeReadOnlyItem(rec.type));
    t->setItem(row, ColEquipmentEntity, makeReadOnlyItem(rec.entityCellText()));

    if (rec.evaluation.valid)
    {
        t->setItem(row, ColF1, makeReadOnlyItem(QString::number(rec.evaluation.f1, 'f', 4)));
        t->setItem(row, ColLevel, makeThreatLevelItem(rec.threatLevelText()));
        t->setItem(row, ColNote, makeReadOnlyItem(QStringLiteral("有效")));
    }
    else
    {
        t->setItem(row, ColF1, makeReadOnlyItem(QString()));
        t->setItem(row, ColLevel, makeThreatLevelItem(QStringLiteral("—")));
        t->setItem(row, ColNote, makeReadOnlyItem(rec.evaluation.errorMessage));
    }
}

// --- 左侧性能参数 spin（与当前表格行同步，只读时仍可读 value） ---
RadarPerformancePara RZThreatAssessment::readInputFromUi() const
{
    RadarPerformancePara in;
    // 左侧性能参数区（与当前选中行对应）
    in.freqMinGhz = ui->spinFreqMin->value();
    in.freqMaxGhz = ui->spinFreqMax->value();
    in.pulseWidthMinUs = ui->spinPwMin->value();
    in.pulseWidthMaxUs = ui->spinPwMax->value();
    in.prfMinHz = ui->spinPrfMin->value();
    in.prfMaxHz = ui->spinPrfMax->value();
    in.detectRangeKm = ui->spinRangeKm->value();
    return in;
}

// --- 底部态势摘要、F1 进度条、有效威胁度（F1×辐射模式系数） ---
void RZThreatAssessment::displayResult(const RadarThreatAssessResult &r)
{
    if (!r.valid)
    {
        ui->lblSituationSummary->clear();
        ui->progressF1->setValue(0);
        ui->lblEffectiveVal->setText(QStringLiteral("—"));
        return;
    }

    // 摘要中的功率为展示/记录项，不参与 F1 公式
    QString radKw;
    QString ratedKw;
    if (ui->spinRadPowerKw->value() > 0.0)
        radKw = QString::number(ui->spinRadPowerKw->value(), 'f', 2);
    else
        radKw = QStringLiteral("未填");
    if (ui->spinRatedPowerKw->value() > 0.0)
        ratedKw = QString::number(ui->spinRatedPowerKw->value(), 'f', 2);
    else
        ratedKw = QStringLiteral("未填");
    ui->lblSituationSummary->setText(
            QStringLiteral("阵地火力防护：%1 | 当前辐射功率：%2 kW | 额定功率：%3 kW（记录项；未录入时批量计算按默认态势：中防护、正常辐射模式）")
            .arg(ui->comboPositionDefense->currentText(), radKw, ratedKw));

    const int pct = static_cast<int>(qRound(r.f1 * 100.0));
    ui->progressF1->setValue(qBound(0, pct, 100));

    int modeIdx = ui->comboRadiationMode->currentIndex();
    if (modeIdx < 0)
        modeIdx = defaultRadiationModeIndex();
    const double radMul = radiationModeMultiplier(modeIdx);
    // 有效威胁度：F1 × 当前辐射模式系数
    const double effective = clamp01(r.f1 * radMul);
    ui->lblEffectiveVal->setText(QString::number(effective, 'f', 4));
}

// --- 「计算威胁评估」：写回当前行态势索引与评估结果 ---
void RZThreatAssessment::onCalculateClicked()
{
    const RadarPerformancePara in = readInputFromUi();
    QString verr;
    if (!validateRadarInput(in, &verr))
    {
        m_lastResult = RadarThreatAssessResult();
        m_lastResult.errorMessage = verr;
        QMessageBox::warning(this, QStringLiteral("参数错误"), verr);
        displayResult(m_lastResult);
        return;
    }

    m_subfactorSynthesis = readSubfactorSynthesisFromUi();
    const int row = ui->tableThreatList->currentRow();
    RadarTypicalPara rep = readRepresentativeCentersFromUi();
    if (row >= 0 && row < m_radarSources.size())
        m_radarSources[row].typicalPara = rep;

    // 左侧区间 + 右侧代表值 + 全局子因子 → 当前行 evaluation
    RadarThreatAssessResult r = buildThreatResult(in, rep, m_subfactorSynthesis);
    if (!r.valid)
    {
        m_lastResult = RadarThreatAssessResult();
        QMessageBox::warning(this, QStringLiteral("参数错误"), r.errorMessage);
        displayResult(m_lastResult);
        return;
    }
    m_lastResult = r;
    displayResult(r);

    if (row >= 0 && row < m_radarSources.size())
    {
        // 持久化当前 combo 选择，供 threatLevelText 与批量默认逻辑使用
        m_radarSources[row].situationDefenseIndex = ui->comboPositionDefense->currentIndex();
        m_radarSources[row].situationRadModeIndex = ui->comboRadiationMode->currentIndex();
        applyEvaluationToRow(row, in, m_radarSources[row].id, m_radarSources[row].name, &r);
    }
}

// --- 态势下拉变化：更新摘要、有效威胁度，并刷新当前行威胁等级列 ---
void RZThreatAssessment::refreshSituationDisplay()
{
    // 与 displayResult 中摘要文案保持一致
    QString radKw;
    QString ratedKw;
    if (ui->spinRadPowerKw->value() > 0.0)
        radKw = QString::number(ui->spinRadPowerKw->value(), 'f', 2);
    else
        radKw = QStringLiteral("未填");
    if (ui->spinRatedPowerKw->value() > 0.0)
        ratedKw = QString::number(ui->spinRatedPowerKw->value(), 'f', 2);
    else
        ratedKw = QStringLiteral("未填");
    ui->lblSituationSummary->setText(
            QStringLiteral("阵地火力防护：%1 | 当前辐射功率：%2 kW | 额定功率：%3 kW（记录项；未录入时批量计算按默认态势：中防护、正常辐射模式）")
            .arg(ui->comboPositionDefense->currentText(), radKw, ratedKw));

    // 有效威胁度直接跟右侧 F1 spin 与辐射模式联动（便于微调时即时反馈）
    const double f1 = ui->spinResultF1->value();
    int modeIdx = ui->comboRadiationMode->currentIndex();
    if (modeIdx < 0)
        modeIdx = defaultRadiationModeIndex();
    const double radMul = radiationModeMultiplier(modeIdx);
    ui->lblEffectiveVal->setText(QString::number(clamp01(f1 * radMul), 'f', 4));
    ui->progressF1->setValue(qBound(0, static_cast<int>(qRound(f1 * 100.0)), 100));

    if (m_lastResult.valid)
        m_lastResult.f1 = f1;

    const int row = ui->tableThreatList->currentRow();
    if (row < 0 || row >= m_radarSources.size())
        return;
    RadarThreatAssessmentRecord &rec = m_radarSources[row];
    rec.situationDefenseIndex = ui->comboPositionDefense->currentIndex();
    rec.situationRadModeIndex = ui->comboRadiationMode->currentIndex();
    rec.evaluation.f1 = f1;
    // 辐射模式变化时重刷威胁等级列颜色/文案
    pushCurrentRowResultToTable(row);
}