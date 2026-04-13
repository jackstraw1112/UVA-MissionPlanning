//
// Created by admin on "2026.04.11 T 13:41:18".
//

#include "RZThreatAssess.h"
#include "SetThreatAssess.h"
#include "ui_RZThreatAssess.h"

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
#include <cmath>

// --- RadarThreatAssessmentRecord（表格列文案 → F1 分档 → 行威胁等级）---
QString RadarThreatAssessmentRecord::entityCellText() const
{
    // 缺省一侧时避免「（）」空壳
    if (name.isEmpty())
    {
        return id;
    }
    if (id.isEmpty())
    {
        return name;
    }
    return QStringLiteral("%1（%2）").arg(name, id);
}

QString RadarThreatAssessmentRecord::threatLevelFromF1(double adjustedF1)
{
    double f = adjustedF1;
    if (f < 0.0)
    {
        f = 0.0;
    }
    if (f > 1.0)
    {
        f = 1.0;
    }
    if (f >= 0.8)
    {
        return QStringLiteral("极高");
    }
    if (f >= 0.6)
    {
        return QStringLiteral("高");
    }
    if (f >= 0.4)
    {
        return QStringLiteral("中");
    }
    if (f >= 0.2)
    {
        return QStringLiteral("低");
    }
    return QStringLiteral("极低");
}

QString RadarThreatAssessmentRecord::threatLevelText() const
{
    // 无有效评估或非有限 F1：表格「威胁等级」列统一显示一字线「—」
    if (!evaluation.valid)
    {
        return QStringLiteral("—");
    }

    const double f1 = evaluation.f1;
    if (!std::isfinite(f1))
    {
        return QStringLiteral("—");
    }

    // situationRadModeIndex == -1：未写入行态势时与批量默认一致，使用界面「正常」辐射项对应索引
    int radiationModeIndex = situationRadModeIndex;
    if (radiationModeIndex < 0)
    {
        radiationModeIndex = ThreatAssessUi::kDefaultRadiationModeComboIndex;
    }

    // 与底部「有效威胁度」相同：F1 × 辐射模式乘子
    const double radiationMultiplier = ThreatAssessUi::radiationModeMultiplier(radiationModeIndex);
    const double adjustedF1 = qBound(0.0, f1 * radiationMultiplier, 1.0);
    return threatLevelFromF1(adjustedF1);
}

// 表格相关属性封装函数
namespace
{
    // 威胁列表列序
    enum EThreatTableCol
    {
        eColRadarModel = 0,   // 雷达型号
        eColEquipmentEntity,  // 装备实体
        eColFreqMin,          // 频率下限(GHz)
        eColFreqMax,          // 频率上限(GHz)
        eColPwMin,            // 脉宽下限(μs)
        eColPwMax,            // 脉宽上限(μs)
        eColPrfMin,           // PRF 下限(Hz)
        eColPrfMax,           // PRF 上限(Hz)
        eColRangeKm,          // 探测距离(千米)
        eColF1,               // 体制威胁指数 F1
        eColLevel,            // 威胁等级
        eColNote,             // 说明
        eColCount             // 列数（哨兵，非数据列）
    };

    // 创建居中表格项
    static QTableWidgetItem *createCenterItem(const QString &text)
    {
        auto *cellItem = new QTableWidgetItem(text);
        cellItem->setTextAlignment(Qt::AlignCenter);
        return cellItem;
    }

    // F1 列：非有限数值时与无结果一致，统一显示一字线「—」（U+2014）
    static QString f1TableCellText(double f1)
    {
        if (!std::isfinite(f1))
        {
            return QStringLiteral("—");
        }
        return QString::number(f1, 'f', 4);
    }

    // 设置威胁等级背景颜色
    static void setItemBackground(QTableWidgetItem *item)
    {
        if (!item)
        {
            return;
        }

        // 威胁等级
        const QString level = item->text();

        // 占位或无等级：恢复默认配色
        if (level == QStringLiteral("—") || level.isEmpty())
        {
            item->setBackground(QBrush());
            item->setForeground(QBrush());
            return;
        }

        // 背景/前景色
        QColor background;
        QColor foreground;
        if (level == QStringLiteral("极高"))
        {
            background = QColor(0xc6, 0x28, 0x28);
            foreground = Qt::white;
        }
        else if (level == QStringLiteral("高"))
        {
            background = QColor(0xef, 0x6c, 0x00);
            foreground = Qt::white;
        }
        else if (level == QStringLiteral("中"))
        {
            background = QColor(0xf9, 0xa8, 0x25);
            foreground = QColor(0x1a, 0x1a, 0x1a);
        }
        else if (level == QStringLiteral("低"))
        {
            background = QColor(0x7c, 0xb3, 0x42);
            foreground = QColor(0x1a, 0x1a, 0x1a);
        }
        else if (level == QStringLiteral("极低"))
        {
            background = QColor(0xec, 0xef, 0xf1);
            foreground = QColor(0x37, 0x47, 0x4f);
        }
        else
        {
            item->setBackground(QBrush());
            item->setForeground(QBrush());
            return;
        }

        item->setBackground(background);
        item->setForeground(foreground);
    }

    // 创建威胁等级表格项
    static QTableWidgetItem *createThreatLevelItem(const QString &text)
    {
        // 显示等级
        QTableWidgetItem *item = createCenterItem(text);
        // 设置背景色
        setItemBackground(item);
        return item;
    }

    // 获取表格列文本
    static QString itemText(const QTableWidget *table, int row, int col)
    {
        const QTableWidgetItem *cellItem = table->item(row, col);
        return cellItem ? cellItem->text().trimmed() : QString();
    }

    // 获取表格列双精度浮点数
    static bool parseDouble(const QString &text, double *outValue)
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

} // namespace

RZThreatAssess::RZThreatAssess(QWidget *parent)
    : QWidget(parent), ui(new Ui::RZThreatAssess), m_form(std::make_unique<ThreatAssessFormBridge>())
{
    ui->setupUi(this);
    m_form->attachToRZUi(ui);

    // 初始化参数
    initParams();

    // 初始化对象
    initObject();

    // 关联信号与槽函数
    initConnect();
}

RZThreatAssess::~RZThreatAssess()
{
    delete ui;
}

void RZThreatAssess::initParams()
{
}

void RZThreatAssess::initObject()
{
    // 初始化表格属性
    initTableAttr();

    // 初始化表头
    initTableHeader();

    // 生成测试数据
    generateTestData();

    // 显示数据到表格
    displayDataToTable();

    // 加载内置作战方案并填充雷达行
    loadBattlePlanRadiationSources();
}

void RZThreatAssess::initConnect()
{
    // 主操作与表格
    connect(m_form->btnCalculate(), &QPushButton::clicked, this, &RZThreatAssess::onCalculateClicked);
    connect(ui->tableThreatList, &QTableWidget::currentCellChanged, this, &RZThreatAssess::onTableCurrentCellChanged);

    // 态势 combo：摘要、有效威胁度、当前行等级列
    connect(m_form->comboRadiationMode(), QOverload<int>::of(&QComboBox::currentIndexChanged), this, &RZThreatAssess::refreshSituationDisplay);
    connect(m_form->comboPositionDefense(), QOverload<int>::of(&QComboBox::currentIndexChanged), this, &RZThreatAssess::refreshSituationDisplay);
    connect(ui->btnRemoveRows, &QPushButton::clicked, this, &RZThreatAssess::onRemoveSelectedRows);
    connect(ui->btnClearTable, &QPushButton::clicked, this, &RZThreatAssess::onClearTable);
    connect(ui->btnCalcAllRadarThreats, &QPushButton::clicked, this, &RZThreatAssess::onCalcAllRadarThreats);

    // 右侧三个代表值：只驱动当前行
    connect(m_form->spinResultFreqCenter(), QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &RZThreatAssess::onRepresentativeCentersEdited);
    connect(m_form->spinResultPwCenter(), QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &RZThreatAssess::onRepresentativeCentersEdited);
    connect(m_form->spinResultPrfCenter(), QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &RZThreatAssess::onRepresentativeCentersEdited);

    // 子因子与合成：驱动全部行批量重算
    connect(m_form->spinResultFFreq(), QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &RZThreatAssess::onSubfactorSynthesisEdited);
    connect(m_form->spinResultFPrf(), QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &RZThreatAssess::onSubfactorSynthesisEdited);
    connect(m_form->spinResultFPw(), QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &RZThreatAssess::onSubfactorSynthesisEdited);
    connect(m_form->spinResultF1Raw(), QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &RZThreatAssess::onSubfactorSynthesisEdited);
    connect(m_form->spinResultRangeMod(), QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &RZThreatAssess::onSubfactorSynthesisEdited);
    connect(m_form->spinResultF1(), QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &RZThreatAssess::onSubfactorSynthesisEdited);
}

void RZThreatAssess::initTableAttr()
{
    // 设置表格属性
    ui->tableThreatList->setEditTriggers(QAbstractItemView::NoEditTriggers);               // 不可编辑
    ui->tableThreatList->setSelectionMode(QAbstractItemView::SingleSelection);             // 单行选中
    ui->tableThreatList->setSelectionBehavior(QAbstractItemView::SelectRows);              // 全行选中
    ui->tableThreatList->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);         // 表头居中
    ui->tableThreatList->setAlternatingRowColors(true);                                    // 奇偶行显示
    ui->tableThreatList->setStyleSheet("QTableView{alternate-background-color:#F5F7FA;}"); // 间隔颜色
    ui->tableThreatList->setSortingEnabled(false);                                         // 关闭自动排序

    // 字体设置
    QFont headerFont = ui->tableThreatList->horizontalHeader()->font();
    headerFont.setPointSize(11);
    ui->tableThreatList->horizontalHeader()->setFont(headerFont);                          // 表头字体大小

    QFont indexFont = ui->tableThreatList->verticalHeader()->font();
    indexFont.setPointSize(11);
    ui->tableThreatList->verticalHeader()->setFont(indexFont);                             // 序号列字体大小

    QFont cellFont = ui->tableThreatList->font();
    cellFont.setPointSize(10);
    ui->tableThreatList->setFont(cellFont);                                                // 表格内容字体大小

    // 列宽策略：初始化后可手动拖动调整
    ui->tableThreatList->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tableThreatList->horizontalHeader()->setStretchLastSection(false);

    // 行高按内容自适应
    ui->tableThreatList->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void RZThreatAssess::initTableHeader()
{
    // 设置列数
    ui->tableThreatList->setColumnCount(eColCount);

    // 设置表头
    const QStringList headers = {
            QString::fromUtf8("雷达型号"),
            QString::fromUtf8("装备实体"),
            QString::fromUtf8("频率下限(GHz)"),
            QString::fromUtf8("频率上限(GHz)"),
            QString::fromUtf8("脉宽下限(μs)"),
            QString::fromUtf8("脉宽上限(μs)"),
            QString::fromUtf8("PRF下限(Hz)"),
            QString::fromUtf8("PRF上限(Hz)"),
            QString::fromUtf8("探测距离(千米)"),
            QString::fromUtf8("体制威胁指数F1"),
            QString::fromUtf8("威胁等级"),
            QString::fromUtf8("说明"),
    };
    ui->tableThreatList->setHorizontalHeaderLabels(headers);
}

void RZThreatAssess::generateTestData()
{
    // 清空数据
    m_radarSources.clear();

    // 添加雷达辐射源
    const auto addRadar = [this](const QString &id, const QString &name, const QString &type, int presetIndex)
    {
        RadarThreatAssessmentRecord newRecord;
        newRecord.id = id;
        newRecord.name = name;
        newRecord.type = type;
        newRecord.performance = radarInputFromPresetIndex(presetIndex);
        newRecord.evaluation = evaluate(newRecord.performance);
        newRecord.typicalPara.freqCenterGhz = newRecord.evaluation.freqCenterGhz;
        newRecord.typicalPara.pulseWidthCenterUs = newRecord.evaluation.pulseWidthCenterUs;
        newRecord.typicalPara.prfCenterHz = newRecord.evaluation.prfCenterHz;
        newRecord.typicalPara.valid = true;
        newRecord.situationDefenseIndex = -1;
        newRecord.situationRadModeIndex = -1;
        m_radarSources.push_back(newRecord);
    };

    // 雷达辐射源（type 与 radarInputFromPresetIndex 预设一致）
    addRadar(QStringLiteral("RAD-001"), QStringLiteral("AN/MPQ-53 相控阵雷达"), QStringLiteral("MPQ-53 (PAC-2 火控)"), 2);
    addRadar(QStringLiteral("RAD-002"), QStringLiteral("爱国者 MPQ-65"), QStringLiteral("MPQ-65 (PAC-3 火控)"), 1);
    addRadar(QStringLiteral("RAD-003"), QStringLiteral("P-18 预警雷达"), QStringLiteral("SPS-48E"), 5);
    addRadar(QStringLiteral("RAD-004"), QStringLiteral("MPQ-64 哨兵雷达"), QStringLiteral("TPS-75"), 7);
    addRadar(QStringLiteral("RAD-005"), QStringLiteral("远程预警 FPS-117"), QStringLiteral("FPS-117"), 6);
}

void RZThreatAssess::onRemoveSelectedRows()
{
    QTableWidget *const threatTable = ui->tableThreatList;
    const QModelIndexList selectedRowIndexes = threatTable->selectionModel()->selectedRows();
    if (selectedRowIndexes.isEmpty())
    {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先选中要删除的行。"));
        return;
    }

    // 获取所有删除行的行号
    QList<int> rowsToRemove;
    for (const QModelIndex &modelIndex : selectedRowIndexes)
    {
        rowsToRemove.append(modelIndex.row());
    }

    // 自大到小删行，避免删除后行号整体前移导致错位
    std::sort(rowsToRemove.begin(), rowsToRemove.end(), [](int lhs, int rhs)
    {
        return lhs > rhs;
    });
    for (int rowIndex : rowsToRemove)
    {
        threatTable->removeRow(rowIndex);
        if (rowIndex >= 0 && rowIndex < m_radarSources.size())
        {
            m_radarSources.removeAt(rowIndex);
        }
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
        m_form->clearTargetEdits();
        m_lastResult = RadarThreatAssessResult();
        m_form->displayResult(m_lastResult);
    }
}

void RZThreatAssess::onClearTable()
{
    m_radarSources.clear();
    ui->tableThreatList->setRowCount(0); // 不逐 removeRow，直接清空更高效
    m_form->clearTargetEdits();
    m_lastResult = RadarThreatAssessResult();
    m_form->displayResult(m_lastResult);
}

double RZThreatAssess::radiationModeMultiplier(int comboIndex)
{
    return ThreatAssessUi::radiationModeMultiplier(comboIndex);
}

RadarPerformancePara RZThreatAssess::radarInputFromPresetIndex(int presetIndex)
{
    RadarPerformancePara performance;
    // 演示用：每种预设对应一组单值区间（上下限相同）
    struct Preset
    {
        double freqGhz;
        double pulseWidthUs;
        double prfHz;
        double rangeKm;
    };
    static const Preset presetTable[] = {
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
    static const int kPresetCount = static_cast<int>(sizeof(presetTable) / sizeof(presetTable[0]));
    if (presetIndex < 1 || presetIndex > kPresetCount)
    {
        return performance;
    }
    const Preset &presetEntry = presetTable[presetIndex - 1]; // 预设编号 1..kPresetCount
    performance.freqMinGhz = performance.freqMaxGhz = presetEntry.freqGhz;
    performance.pulseWidthMinUs = performance.pulseWidthMaxUs = presetEntry.pulseWidthUs;
    performance.prfMinHz = performance.prfMaxHz = presetEntry.prfHz;
    performance.detectRangeKm = presetEntry.rangeKm;
    return performance;
}

void RZThreatAssess::loadFormFromTableRow(int row)
{
    if (row < 0 || row >= ui->tableThreatList->rowCount())
    {
        return;
    }

    const QTableWidget *threatTable = ui->tableThreatList;
    // 性能区间：从表格单元格解析后写入左侧 spin（若解析失败则保留 spin 原值）
    double parsedValue = 0.0;
    if (parseDouble(itemText(threatTable, row, eColFreqMin), &parsedValue))
    {
        m_form->setFreqMin(parsedValue);
    }
    if (parseDouble(itemText(threatTable, row, eColFreqMax), &parsedValue))
    {
        m_form->setFreqMax(parsedValue);
    }
    if (parseDouble(itemText(threatTable, row, eColPwMin), &parsedValue))
    {
        m_form->setPwMin(parsedValue);
    }
    if (parseDouble(itemText(threatTable, row, eColPwMax), &parsedValue))
    {
        m_form->setPwMax(parsedValue);
    }
    if (parseDouble(itemText(threatTable, row, eColPrfMin), &parsedValue))
    {
        m_form->setPrfMin(parsedValue);
    }
    if (parseDouble(itemText(threatTable, row, eColPrfMax), &parsedValue))
    {
        m_form->setPrfMax(parsedValue);
    }
    if (parseDouble(itemText(threatTable, row, eColRangeKm), &parsedValue))
    {
        m_form->setRangeKm(parsedValue);
    }

    if (row < m_radarSources.size())
    {
        const RadarThreatAssessmentRecord &sourceRecord = m_radarSources.at(row);
        m_form->setEditRadarModel(sourceRecord.type);
        m_form->setEditEquipmentEntity(sourceRecord.entityCellText());
        // 代表值 spin：优先记录内 representative，否则用 evaluation 或区间几何均值
        m_form->syncRepresentativeCentersToUi(sourceRecord.typicalPara, m_form->readInputFromUi(), sourceRecord.evaluation);
        if (sourceRecord.situationDefenseIndex >= 0)
        {
            m_form->setDefenseComboIndex(sourceRecord.situationDefenseIndex);
        }
        else
        {
            m_form->setDefenseComboIndex(ThreatAssessUi::kDefaultPositionDefenseComboIndex);
        }
        if (sourceRecord.situationRadModeIndex >= 0)
        {
            m_form->setRadiationComboIndex(sourceRecord.situationRadModeIndex);
        }
        else
        {
            m_form->setRadiationComboIndex(ThreatAssessUi::kDefaultRadiationModeComboIndex);
        }
    }
    else
    {
        m_form->clearTargetEdits();
    }

    // 同步后按当前表单与全局子因子重算本行并刷新底部展示
    onCalculateClicked();
}

void RZThreatAssess::onTableCurrentCellChanged(int currentRow, int, int, int)
{
    if (m_syncingEntityPick)
    {
        return;
    }
    if (currentRow < 0)
    {
        m_form->clearEquipmentEntityEdit();
        return;
    }
    loadFormFromTableRow(currentRow);
}

void RZThreatAssess::displayDataToTable()
{
    QTableWidget *const threatTable = ui->tableThreatList;
    threatTable->setRowCount(0);
    for (int rowIndex = 0; rowIndex < m_radarSources.size(); ++rowIndex)
    {
        displayDataToTable(m_radarSources.at(rowIndex), rowIndex);
    }
}

void RZThreatAssess::loadBattlePlanRadiationSources()
{
    if (!m_radarSources.isEmpty())
    {
        // 用首行 evaluate 的子因子快照初始化全局「子因子与合成」
        const RadarThreatAssessResult &firstRowEvaluation = m_radarSources[0].evaluation;
        m_subfactorSynthesis.fFreq = firstRowEvaluation.fFreq;
        m_subfactorSynthesis.fPrf = firstRowEvaluation.fPrf;
        m_subfactorSynthesis.fPw = firstRowEvaluation.fPw;
        m_subfactorSynthesis.f1Raw = firstRowEvaluation.f1Raw;
        m_subfactorSynthesis.rangeMod = firstRowEvaluation.rangeMod;
        m_subfactorSynthesis.f1 = firstRowEvaluation.f1;
        syncSubfactorSynthesisToUi(m_subfactorSynthesis);

        // 各行在统一全局子因子下重建 evaluation，保证表格 F1 与右侧 spin 一致
        for (int rowIndex = 0; rowIndex < m_radarSources.size(); ++rowIndex)
        {
            RadarThreatAssessResult rebuiltResult = buildThreatResult(m_radarSources[rowIndex].performance,
                                                                      m_radarSources[rowIndex].typicalPara,
                                                                      m_subfactorSynthesis);
            if (rebuiltResult.valid)
            {
                m_radarSources[rowIndex].evaluation = rebuiltResult;
            }
            pushCurrentRowResultToTable(rowIndex);
        }
        ui->tableThreatList->selectRow(0);
    }
    else
    {
        m_form->clearTargetEdits();
        m_lastResult = RadarThreatAssessResult();
        m_form->displayResult(m_lastResult);
    }
}

void RZThreatAssess::onCalcAllRadarThreats()
{
    if (m_radarSources.isEmpty())
    {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("表格中无数据，请先加载作战方案。"));
        return;
    }

    // 右侧六个 spin 作为全体行共用的子因子与合成快照
    m_subfactorSynthesis = m_form->readSubfactorSynthesisFromUi();

    int completedRowCount = 0;
    for (int row = 0; row < m_radarSources.size(); ++row)
    {
        ensureSituationDefaultsOnRecord(row);
        RadarPerformancePara rowPerformance;
        QString equipmentId;
        QString equipmentName;
        QString parseError;
        if (!parseInputFromTableRow(row, &rowPerformance, &equipmentId, &equipmentName, &parseError))
        {
            QMessageBox::warning(this, QStringLiteral("参数错误"),
                                 QStringLiteral("第 %1 行：%2").arg(row + 1).arg(parseError));
            continue;
        }
        // 代表值取各行已存 representative，不读当前 UI 行
        RadarThreatAssessResult rowResult = buildThreatResult(rowPerformance, m_radarSources[row].typicalPara,
                                                              m_subfactorSynthesis);
        if (!rowResult.valid)
        {
            QMessageBox::warning(this, QStringLiteral("参数错误"),
                                 QStringLiteral("第 %1 行：%2").arg(row + 1).arg(rowResult.errorMessage));
            continue;
        }
        applyEvaluationToRow(row, rowPerformance, equipmentId, equipmentName, &rowResult);
        ++completedRowCount;
    }

    QMessageBox::information(
            this, QStringLiteral("完成"),
            QStringLiteral("已重算全部行（共 %1 行）。代表值按各行已存数据；子因子与合成使用当前右侧全局数值（全体一致）。"
                    "威胁等级按「F1×辐射模式系数」映射。")
            .arg(completedRowCount));
}

double RZThreatAssess::geometricMean(double intervalLow, double intervalHigh)
{
    // 区间上下限的几何中心；非正区间不参与开方
    if (intervalLow <= 0.0 || intervalHigh <= 0.0)
    {
        return 0.0;
    }
    return qSqrt(intervalLow * intervalHigh);
}

double RZThreatAssess::clamp01(double value)
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

double RZThreatAssess::frequencyThreatFactor(double fGhz)
{
    const double f = fGhz;
    // 分段线性：低频弱威胁，中高频升高，极高频略回落（与方案曲线一致）
    if (f <= 0.1)
    {
        return 0.05;
    }
    if (f <= 1.0)
    {
        return 0.15;
    }
    if (f <= 4.0)
    {
        return 0.15 + 0.30 * (f - 1.0) / 3.0;
    }
    if (f <= 8.0)
    {
        return 0.45 + 0.25 * (f - 4.0) / 4.0;
    }
    if (f <= 12.0)
    {
        return 0.70 + 0.20 * (f - 8.0) / 4.0;
    }
    if (f <= 18.0)
    {
        return 0.90 - 0.30 * (f - 12.0) / 6.0;
    }
    return 0.40;
}

double RZThreatAssess::pulseWidthThreatFactor(double tauUs)
{
    if (tauUs <= 0.0)
    {
        return 0.0;
    }

    // 对数域线性再压到 [0,1]
    const double logTau = qLn(tauUs) / qLn(10.0);
    const double raw = 1.0 - (logTau + 2.0) / 4.0;
    return clamp01(raw);
}

double RZThreatAssess::prfThreatFactor(double prfHz)
{
    if (prfHz <= 0.0)
    {
        return 0.0;
    }
    const double logP = qLn(prfHz) / qLn(10.0);
    const double raw = (logP - 1.5) / 3.5;
    return clamp01(raw);
}

double RZThreatAssess::rangeModifier(double rangeKm, double f1Raw)
{
    // 远距略放大、中距不变、近距略压制（与 f1Raw 耦合）
    if (rangeKm >= 200.0)
    {
        return 1.0 + 0.08 * f1Raw;
    }
    if (rangeKm > 50.0)
    {
        return 1.0;
    }
    return 1.0 - 0.10 * f1Raw;
}

bool RZThreatAssess::validateRadarInput(const RadarPerformancePara &input, QString *errorMessage)
{
    // 各区间为正当、min≤max；探测距离单独要求 >0
    if (input.freqMinGhz <= 0.0 || input.freqMaxGhz <= 0.0
        || input.freqMinGhz > input.freqMaxGhz)
    {
        if (errorMessage)
        {
            *errorMessage = QStringLiteral("频率范围无效：请填写正数且最小值不大于最大值（单值时请将上下限设为相同）。");
        }
        return false;
    }
    if (input.pulseWidthMinUs <= 0.0 || input.pulseWidthMaxUs <= 0.0
        || input.pulseWidthMinUs > input.pulseWidthMaxUs)
    {
        if (errorMessage)
        {
            *errorMessage = QStringLiteral("脉宽范围无效。");
        }
        return false;
    }
    if (input.prfMinHz <= 0.0 || input.prfMaxHz <= 0.0 || input.prfMinHz > input.prfMaxHz)
    {
        if (errorMessage)
        {
            *errorMessage = QStringLiteral("PRF 范围无效。");
        }
        return false;
    }
    if (input.detectRangeKm <= 0.0)
    {
        if (errorMessage)
        {
            *errorMessage = QStringLiteral("探测距离须为大于 0 的数值（km）。");
        }
        return false;
    }
    return true;
}

// 不重新计算子因子：把 UI 上的全局因子与代表值中心直接写入结果（供人工微调演示）
RadarThreatAssessResult RZThreatAssess::buildThreatResult(const RadarPerformancePara &performance,
                                                              const RadarTypicalPara &representative,
                                                              const GlobalThreatFactors &globalFactors)
{
    RadarThreatAssessResult result;
    if (!validateRadarInput(performance, &result.errorMessage))
    {
        return result;
    }

    // 代表值优先用户编辑；否则用区间几何均值
    if (representative.valid)
    {
        result.freqCenterGhz = representative.freqCenterGhz;
        result.pulseWidthCenterUs = representative.pulseWidthCenterUs;
        result.prfCenterHz = representative.prfCenterHz;
    }
    else
    {
        result.freqCenterGhz = geometricMean(performance.freqMinGhz, performance.freqMaxGhz);
        result.pulseWidthCenterUs = geometricMean(performance.pulseWidthMinUs, performance.pulseWidthMaxUs);
        result.prfCenterHz = geometricMean(performance.prfMinHz, performance.prfMaxHz);
    }

    // 子因子与 F1 直接取自全局 globalFactors（不重新跑 frequencyThreatFactor 等）
    result.fFreq = globalFactors.fFreq;
    result.fPrf = globalFactors.fPrf;
    result.fPw = globalFactors.fPw;
    result.f1Raw = globalFactors.f1Raw;
    result.rangeMod = globalFactors.rangeMod;
    result.f1 = clamp01(globalFactors.f1);

    result.valid = true;
    return result;
}

// 由输入区间自动算子因子、加权 f1Raw、距离修正与 F1（无 UI 侧全局因子）
RadarThreatAssessResult RZThreatAssess::evaluate(const RadarPerformancePara &input)
{
    RadarThreatAssessResult result;
    if (!validateRadarInput(input, &result.errorMessage))
    {
        return result;
    }

    result.freqCenterGhz = geometricMean(input.freqMinGhz, input.freqMaxGhz);
    result.pulseWidthCenterUs = geometricMean(input.pulseWidthMinUs, input.pulseWidthMaxUs);
    result.prfCenterHz = geometricMean(input.prfMinHz, input.prfMaxHz);

    // 由代表中心计算三个子因子
    result.fFreq = frequencyThreatFactor(result.freqCenterGhz);
    result.fPw = pulseWidthThreatFactor(result.pulseWidthCenterUs);
    result.fPrf = prfThreatFactor(result.prfCenterHz);

    // 加权合成 → 距离修正 → 压到 [0,1] 得 F1
    result.f1Raw = 0.40 * result.fFreq + 0.30 * result.fPrf + 0.30 * result.fPw;
    result.rangeMod = rangeModifier(input.detectRangeKm, result.f1Raw);
    result.f1 = clamp01(result.f1Raw * result.rangeMod);

    result.valid = true;
    return result;
}

void RZThreatAssess::syncSubfactorSynthesisToUi(const GlobalThreatFactors &factors)
{
    m_subfactorSynthesis = factors;
    m_form->syncSubfactorSpinBoxes(factors);
}

// --- 将 m_radarSources[row].evaluation 反映到表格 ---
void RZThreatAssess::pushCurrentRowResultToTable(int row)
{
    if (row < 0 || row >= ui->tableThreatList->rowCount() || row >= m_radarSources.size())
    {
        return;
    }
    QTableWidget *const threatTable = ui->tableThreatList;
    const RadarThreatAssessmentRecord &rowRecord = m_radarSources.at(row);
    // F1 / 威胁等级 / 说明三列与 evaluation 及辐射模式一致
    if (rowRecord.evaluation.valid)
    {
        threatTable->setItem(row, eColF1, createCenterItem(f1TableCellText(rowRecord.evaluation.f1)));
        threatTable->setItem(row, eColLevel, createThreatLevelItem(rowRecord.threatLevelText()));
        threatTable->setItem(row, eColNote, createCenterItem(QStringLiteral("有效")));
    }
    else
    {
        threatTable->setItem(row, eColF1, createCenterItem(QStringLiteral("—")));
        threatTable->setItem(row, eColLevel, createThreatLevelItem(QStringLiteral("—")));
        threatTable->setItem(row, eColNote,
                    createCenterItem(rowRecord.evaluation.errorMessage.isEmpty()
                            ? QStringLiteral("无效")
                            : rowRecord.evaluation.errorMessage));
    }
}

void RZThreatAssess::applySubfactorSynthesisToAllRows()
{
    if (m_radarSources.isEmpty())
    {
        return;
    }
    m_subfactorSynthesis = m_form->readSubfactorSynthesisFromUi();
    for (int row = 0; row < m_radarSources.size(); ++row)
    {
        RadarThreatAssessResult rowResult = buildThreatResult(m_radarSources[row].performance, m_radarSources[row].typicalPara,
                                                      m_subfactorSynthesis);
        if (!rowResult.valid)
        {
            continue;
        }
        m_radarSources[row].evaluation = rowResult;
        pushCurrentRowResultToTable(row);
    }
    const int currentRowIndex = ui->tableThreatList->currentRow();
    // 底部展示：优先当前行结果，否则取首个有效行
    if (currentRowIndex >= 0 && currentRowIndex < m_radarSources.size()
        && m_radarSources[currentRowIndex].evaluation.valid)
    {
        m_lastResult = m_radarSources[currentRowIndex].evaluation;
    }
    else
    {
        for (const RadarThreatAssessmentRecord &sourceRecord : m_radarSources)
        {
            if (sourceRecord.evaluation.valid)
            {
                m_lastResult = sourceRecord.evaluation;
                break;
            }
        }
    }
    m_form->displayResult(m_lastResult);
}

// --- 槽：代表值或子因子与合成编辑 ---
void RZThreatAssess::onRepresentativeCentersEdited()
{
    if (m_form->isBlockingResultSpinSignals())
    {
        return;
    }
    const int row = ui->tableThreatList->currentRow();
    if (row < 0 || row >= m_radarSources.size())
    {
        return;
    }
    // 仅重算当前行，不改变其他行的 representative
    m_radarSources[row].typicalPara = m_form->readRepresentativeCentersFromUi();
    const RadarPerformancePara uiPerformance = m_form->readInputFromUi();
    m_subfactorSynthesis = m_form->readSubfactorSynthesisFromUi();
    RadarThreatAssessResult newResult = buildThreatResult(uiPerformance, m_radarSources[row].typicalPara, m_subfactorSynthesis);
    if (!newResult.valid)
    {
        return;
    }
    m_radarSources[row].evaluation = newResult;
    m_lastResult = newResult;
    pushCurrentRowResultToTable(row);
    m_form->displayResult(newResult);
}

void RZThreatAssess::onSubfactorSynthesisEdited()
{
    if (m_form->isBlockingResultSpinSignals())
    {
        return;
    }
    applySubfactorSynthesisToAllRows();
}

void RZThreatAssess::displayDataToTable(const RadarThreatAssessmentRecord &data, int row)
{
    QTableWidget *const threatTable = ui->tableThreatList;
    int targetRow = 0;

    // row < 0：追加新行（与 RZSourceRadiation::writeModelRow 语义一致）
    if (row < 0)
    {
        targetRow = threatTable->rowCount();
        threatTable->insertRow(targetRow);
        m_radarSources.push_back(data);
    }
    else
    {
        targetRow = row;
        while (threatTable->rowCount() <= targetRow)
        {
            threatTable->insertRow(threatTable->rowCount());
        }
        while (m_radarSources.size() <= targetRow)
        {
            m_radarSources.push_back(RadarThreatAssessmentRecord());
        }
        m_radarSources[targetRow] = data;
    }

    const RadarPerformancePara &performance = data.performance;

    // 性能列来自 data.performance；结果列来自 data.evaluation
    threatTable->setItem(targetRow, eColRadarModel, createCenterItem(data.type));
    threatTable->setItem(targetRow, eColEquipmentEntity, createCenterItem(data.entityCellText()));
    threatTable->setItem(targetRow, eColFreqMin, createCenterItem(QString::number(performance.freqMinGhz, 'f', 4)));
    threatTable->setItem(targetRow, eColFreqMax, createCenterItem(QString::number(performance.freqMaxGhz, 'f', 4)));
    threatTable->setItem(targetRow, eColPwMin, createCenterItem(QString::number(performance.pulseWidthMinUs, 'f', 4)));
    threatTable->setItem(targetRow, eColPwMax, createCenterItem(QString::number(performance.pulseWidthMaxUs, 'f', 4)));
    threatTable->setItem(targetRow, eColPrfMin, createCenterItem(QString::number(performance.prfMinHz, 'f', 2)));
    threatTable->setItem(targetRow, eColPrfMax, createCenterItem(QString::number(performance.prfMaxHz, 'f', 2)));
    threatTable->setItem(targetRow, eColRangeKm, createCenterItem(QString::number(performance.detectRangeKm, 'f', 2)));

    if (data.evaluation.valid)
    {
        threatTable->setItem(targetRow, eColF1, createCenterItem(f1TableCellText(data.evaluation.f1)));
        threatTable->setItem(targetRow, eColLevel, createThreatLevelItem(data.threatLevelText()));
        threatTable->setItem(targetRow, eColNote, createCenterItem(QStringLiteral("有效")));
    }
    else
    {
        threatTable->setItem(targetRow, eColF1, createCenterItem(QStringLiteral("—")));
        threatTable->setItem(targetRow, eColLevel, createThreatLevelItem(QStringLiteral("—")));
        threatTable->setItem(targetRow, eColNote,
                    createCenterItem(data.evaluation.errorMessage.isEmpty()
                            ? QStringLiteral("无效")
                            : data.evaluation.errorMessage));
    }
}

// --- 从表格文本解析输入（批量重算 / 校验用） ---
bool RZThreatAssess::parseInputFromTableRow(int row, RadarPerformancePara *outInput, QString *outId,
                                                QString *outName, QString *errorMessage) const
{
    const QTableWidget *threatTable = ui->tableThreatList;
    if (row < 0 || row >= threatTable->rowCount())
    {
        if (errorMessage)
        {
            *errorMessage = QStringLiteral("行号无效");
        }
        return false;
    }

    if (row >= 0 && row < m_radarSources.size())
    {
        if (outId)
        {
            *outId = m_radarSources.at(row).id;
        }
        if (outName)
        {
            *outName = m_radarSources.at(row).name;
        }
    }
    else
    {
        if (outId)
        {
            *outId = QString();
        }
        if (outName)
        {
            *outName = QString();
        }
    }

    RadarPerformancePara parsedPerformance;
    double cellValue = 0.0;
    // 逐列解析数值，再交给 validateRadarInput 做区间合法性检查
    if (!parseDouble(itemText(threatTable, row, eColFreqMin), &cellValue))
    {
        if (errorMessage)
        {
            *errorMessage = QStringLiteral("频率下限不是有效数字");
        }
        return false;
    }
    parsedPerformance.freqMinGhz = cellValue;
    if (!parseDouble(itemText(threatTable, row, eColFreqMax), &cellValue))
    {
        if (errorMessage)
        {
            *errorMessage = QStringLiteral("频率上限不是有效数字");
        }
        return false;
    }
    parsedPerformance.freqMaxGhz = cellValue;
    if (!parseDouble(itemText(threatTable, row, eColPwMin), &cellValue))
    {
        if (errorMessage)
        {
            *errorMessage = QStringLiteral("脉宽下限不是有效数字");
        }
        return false;
    }
    parsedPerformance.pulseWidthMinUs = cellValue;
    if (!parseDouble(itemText(threatTable, row, eColPwMax), &cellValue))
    {
        if (errorMessage)
        {
            *errorMessage = QStringLiteral("脉宽上限不是有效数字");
        }
        return false;
    }
    parsedPerformance.pulseWidthMaxUs = cellValue;
    if (!parseDouble(itemText(threatTable, row, eColPrfMin), &cellValue))
    {
        if (errorMessage)
        {
            *errorMessage = QStringLiteral("PRF_min 不是有效数字");
        }
        return false;
    }
    parsedPerformance.prfMinHz = cellValue;
    if (!parseDouble(itemText(threatTable, row, eColPrfMax), &cellValue))
    {
        if (errorMessage)
        {
            *errorMessage = QStringLiteral("PRF_max 不是有效数字");
        }
        return false;
    }
    parsedPerformance.prfMaxHz = cellValue;
    if (!parseDouble(itemText(threatTable, row, eColRangeKm), &cellValue))
    {
        if (errorMessage)
        {
            *errorMessage = QStringLiteral("探测距离不是有效数字");
        }
        return false;
    }
    parsedPerformance.detectRangeKm = cellValue;

    QString validationMessage;
    if (!validateRadarInput(parsedPerformance, &validationMessage))
    {
        if (errorMessage)
        {
            *errorMessage = validationMessage;
        }
        return false;
    }
    if (outInput)
    {
        *outInput = parsedPerformance;
    }
    return true;
}

void RZThreatAssess::ensureSituationDefaultsOnRecord(int row)
{
    if (row < 0 || row >= m_radarSources.size())
    {
        return;
    }
    // 批量重算前把「未单独设置」的行统一到界面默认 combo 索引
    if (m_radarSources[row].situationDefenseIndex < 0)
    {
        m_radarSources[row].situationDefenseIndex = ThreatAssessUi::kDefaultPositionDefenseComboIndex;
    }
    if (m_radarSources[row].situationRadModeIndex < 0)
    {
        m_radarSources[row].situationRadModeIndex = ThreatAssessUi::kDefaultRadiationModeComboIndex;
    }
}

void RZThreatAssess::applyEvaluationToRow(int row, const RadarPerformancePara &performance, const QString &equipmentId,
                                              const QString &equipmentName, const RadarThreatAssessResult *precomputed)
{
    QTableWidget *const threatTable = ui->tableThreatList;
    if (row < 0 || row >= threatTable->rowCount() || row >= m_radarSources.size())
    {
        return;
    }

    RadarThreatAssessmentRecord updatedRecord;
    updatedRecord.id = equipmentId;
    updatedRecord.name = equipmentName;
    updatedRecord.performance = performance;
    const RadarThreatAssessmentRecord &previousRecord = m_radarSources.at(row);
    // 型号、代表值、态势索引沿用行内原记录
    updatedRecord.type = previousRecord.type;
    updatedRecord.typicalPara = previousRecord.typicalPara;
    updatedRecord.situationDefenseIndex = previousRecord.situationDefenseIndex;
    updatedRecord.situationRadModeIndex = previousRecord.situationRadModeIndex;
    if (precomputed)
    {
        updatedRecord.evaluation = *precomputed;
    }
    else
    {
        updatedRecord.evaluation = buildThreatResult(performance, updatedRecord.typicalPara, m_form->readSubfactorSynthesisFromUi());
    }
    m_radarSources[row] = updatedRecord;

    threatTable->setItem(row, eColRadarModel, createCenterItem(updatedRecord.type));
    threatTable->setItem(row, eColEquipmentEntity, createCenterItem(updatedRecord.entityCellText()));

    if (updatedRecord.evaluation.valid)
    {
        threatTable->setItem(row, eColF1, createCenterItem(f1TableCellText(updatedRecord.evaluation.f1)));
        threatTable->setItem(row, eColLevel, createThreatLevelItem(updatedRecord.threatLevelText()));
        threatTable->setItem(row, eColNote, createCenterItem(QStringLiteral("有效")));
    }
    else
    {
        threatTable->setItem(row, eColF1, createCenterItem(QStringLiteral("—")));
        threatTable->setItem(row, eColLevel, createThreatLevelItem(QStringLiteral("—")));
        threatTable->setItem(row, eColNote, createCenterItem(updatedRecord.evaluation.errorMessage));
    }
}

// --- 「计算威胁评估」：写回当前行态势索引与评估结果 ---
void RZThreatAssess::onCalculateClicked()
{
    const RadarPerformancePara uiPerformance = m_form->readInputFromUi();
    QString validationError;
    if (!validateRadarInput(uiPerformance, &validationError))
    {
        m_lastResult = RadarThreatAssessResult();
        m_lastResult.errorMessage = validationError;
        QMessageBox::warning(this, QStringLiteral("参数错误"), validationError);
        m_form->displayResult(m_lastResult);
        return;
    }

    m_subfactorSynthesis = m_form->readSubfactorSynthesisFromUi();
    const int currentRow = ui->tableThreatList->currentRow();
    RadarTypicalPara representativeCenters = m_form->readRepresentativeCentersFromUi();
    if (currentRow >= 0 && currentRow < m_radarSources.size())
    {
        m_radarSources[currentRow].typicalPara = representativeCenters;
    }

    // 左侧区间 + 右侧代表值 + 全局子因子 → 当前行 evaluation
    RadarThreatAssessResult assessmentResult = buildThreatResult(uiPerformance, representativeCenters, m_subfactorSynthesis);
    if (!assessmentResult.valid)
    {
        m_lastResult = RadarThreatAssessResult();
        QMessageBox::warning(this, QStringLiteral("参数错误"), assessmentResult.errorMessage);
        m_form->displayResult(m_lastResult);
        return;
    }
    m_lastResult = assessmentResult;
    m_form->displayResult(assessmentResult);

    if (currentRow >= 0 && currentRow < m_radarSources.size())
    {
        // 持久化当前 combo 选择，供 threatLevelText 与批量默认逻辑使用
        m_radarSources[currentRow].situationDefenseIndex = m_form->defenseComboCurrentIndex();
        m_radarSources[currentRow].situationRadModeIndex = m_form->radiationComboCurrentIndex();
        applyEvaluationToRow(currentRow, uiPerformance, m_radarSources[currentRow].id, m_radarSources[currentRow].name,
                             &assessmentResult);
    }
}

// --- 态势下拉变化：更新摘要、有效威胁度，并刷新当前行威胁等级列 ---
void RZThreatAssess::refreshSituationDisplay()
{
    m_form->refreshSituationSummaryEffectiveProgress(&m_lastResult);

    const int currentRow = ui->tableThreatList->currentRow();
    if (currentRow < 0 || currentRow >= m_radarSources.size())
    {
        return;
    }
    RadarThreatAssessmentRecord &mutableRecord = m_radarSources[currentRow];
    mutableRecord.situationDefenseIndex = m_form->defenseComboCurrentIndex();
    mutableRecord.situationRadModeIndex = m_form->radiationComboCurrentIndex();
    mutableRecord.evaluation.f1 = m_form->spinResultF1()->value();
    // 辐射模式变化时重刷威胁等级列颜色/文案
    pushCurrentRowResultToTable(currentRow);
}