//
// Created by admin on "2026.04.11 T 13:41:18".
//

#include "RZThreatAssess.h"
#include "SetRadarThreatAssess.h"
#include "ui_RZThreatAssess.h"

#include <QAbstractItemView>
#include <QBrush>
#include <QColor>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QMessageBox>
#include <QSignalBlocker>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QtMath>

#include <algorithm>
#include <cmath>

// --- RadarThreatAssessmentRecord（表格列文案 → F1 分档 → 行威胁等级）---
QString RadarThreatAssessRecord::entityCellText() const
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

QString RadarThreatAssessRecord::threatLevelFromF1(double adjustedF1)
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

QString RadarThreatAssessRecord::threatLevelText(double f1) const
{
    // 无有效评估或非有限 F1：表格「威胁等级」列统一显示一字线「—」
    if (!evaluation.valid)
    {
        return QStringLiteral("—");
    }
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
    const double radiationMultiplier = ProjectPublicInterface::radiationModeMultiplier(radiationModeIndex);
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

    static RadarThreatFactors factorsFromPerformance(const RadarPerformancePara &performance)
    {
        RadarThreatFactors factors;
        factors.freqFactor = ProjectPublicInterface::frequencyThreatFactor(
                ProjectPublicInterface::geometricMean(performance.freqMin, performance.freqMax));
        factors.pwFactor = ProjectPublicInterface::pulseWidthThreatFactor(
                ProjectPublicInterface::geometricMean(performance.pwMin, performance.pwMax));
        factors.prfFactor = ProjectPublicInterface::prfThreatFactor(
                ProjectPublicInterface::geometricMean(performance.prfMin, performance.prfMax));
        factors.f1RawFactor = 0.40 * factors.freqFactor + 0.30 * factors.prfFactor + 0.30 * factors.pwFactor;
        factors.rangeModFactor = ProjectPublicInterface::rangeModifier(performance.detectRange, factors.f1RawFactor);
        factors.f1Factor = ProjectPublicInterface::clamp01(factors.f1RawFactor * factors.rangeModFactor);
        return factors;
    }

} // namespace

RZThreatAssess::RZThreatAssess(QWidget *parent)
    : QWidget(parent), ui(new Ui::RZThreatAssess)
{
    ui->setupUi(this);

    m_setThreatPanel = new SetRadarThreatAssess(this);
    m_setThreatPanel->setWindowTitle(QStringLiteral("辐射源信息与威胁评估"));
    m_setThreatPanel->setWindowFlag(Qt::Dialog, true);
    m_setThreatPanel->resize(980, 620);

    // 初始化参数
    initPara();

    // 初始化对象
    initClass();

    // 关联信号与槽函数
    signalAndSlot();
}

RZThreatAssess::~RZThreatAssess()
{
    delete ui;
}

void RZThreatAssess::initPara()
{
    m_setThreatPanel = nullptr;
}

void RZThreatAssess::initClass()
{
    // 初始化表格属性
    initTableAttr();
    // 初始化表头
    initTableHeader();

    // 生成测试数据
    generateTestData();
    // 显示数据到表格
    displayDataToTable();

    // 评估雷达威胁
    assessRadarThreat();
}

void RZThreatAssess::signalAndSlot()
{
    // 接收评估结果
    connect(m_setThreatPanel, &SetRadarThreatAssess::sigEvaluteResult, this, &RZThreatAssess::onSituationConfirm);

    // 表格当前行改变
    connect(ui->tableThreatList, &QTableWidget::currentCellChanged, this, &RZThreatAssess::onTableCurrentCellChanged);
    // 单元格双击
    connect(ui->tableThreatList, &QTableWidget::cellDoubleClicked, this, &RZThreatAssess::onThreatTableCellDoubleClicked);
    // 删除选中行
    connect(ui->btnRemoveRows, &QPushButton::clicked, this, &RZThreatAssess::onRemoveSelectedRows);
    // 清空表格
    connect(ui->btnClearTable, &QPushButton::clicked, this, &RZThreatAssess::onClearTable);
    // 批量评估
    connect(ui->btnCalcAllRadarThreats, &QPushButton::clicked, this, &RZThreatAssess::onCalcAllRadarThreats);
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
        RadarThreatAssessRecord newRecord;

        newRecord.id = id;
        newRecord.name = name;
        newRecord.type = type;
        newRecord.performance = ProjectPublicInterface::radarInputFromPresetIndex(presetIndex);
        newRecord.evaluation = ProjectPublicInterface::evaluate(newRecord.performance);
        newRecord.typicalPara.freq = newRecord.evaluation.freq;
        newRecord.typicalPara.pw = newRecord.evaluation.pw;
        newRecord.typicalPara.prf = newRecord.evaluation.prf;
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

void RZThreatAssess::displayDataToTable()
{
    // 表格数据清空
    auto const pTable = ui->tableThreatList;
    pTable->setRowCount(0);

    // 表格数据填充
    for (int rowIndex = 0; rowIndex < m_radarSources.size(); ++rowIndex)
    {
        displayDataToTable(m_radarSources.at(rowIndex), rowIndex);
    }
}

void RZThreatAssess::displayDataToTable(const RadarThreatAssessRecord &data, int row)
{
    // 获取表格对象
    auto const pTable = ui->tableThreatList;

    // 添加数据
    if (row == -1)
    {
        // 插入行
        row = pTable->rowCount();
        pTable->insertRow(row);

        // 显示数据
        const RadarPerformancePara &perf = data.performance;
        pTable->setItem(row, eColRadarModel, new QTableWidgetItem(data.type));
        pTable->setItem(row, eColEquipmentEntity, new QTableWidgetItem(data.entityCellText()));
        pTable->setItem(row, eColFreqMin, new QTableWidgetItem(QString::number(perf.freqMin, 'f', 4)));
        pTable->setItem(row, eColFreqMax, new QTableWidgetItem(QString::number(perf.freqMax, 'f', 4)));
        pTable->setItem(row, eColPwMin, new QTableWidgetItem(QString::number(perf.pwMin, 'f', 4)));
        pTable->setItem(row, eColPwMax, new QTableWidgetItem(QString::number(perf.pwMax, 'f', 4)));
        pTable->setItem(row, eColPrfMin, new QTableWidgetItem(QString::number(perf.prfMin, 'f', 2)));
        pTable->setItem(row, eColPrfMax, new QTableWidgetItem(QString::number(perf.prfMax, 'f', 2)));
        pTable->setItem(row, eColRangeKm, new QTableWidgetItem(QString::number(perf.detectRange, 'f', 2)));

        // 评估结果
        if (data.evaluation.valid)
        {
            pTable->setItem(row, eColF1, new QTableWidgetItem(ProjectPublicInterface::f1TableCellText(m_subfactor.f1Factor)));
            pTable->setItem(row, eColLevel, new QTableWidgetItem(data.threatLevelText(m_subfactor.f1Factor)));
            pTable->setItem(row, eColNote, new QTableWidgetItem(QStringLiteral("有效")));
        }
        else
        {
            pTable->setItem(row, eColF1, new QTableWidgetItem(QStringLiteral("—")));
            pTable->setItem(row, eColLevel, new QTableWidgetItem(QStringLiteral("—")));
            pTable->setItem(row, eColNote, new QTableWidgetItem(data.evaluation.errorMessage.isEmpty() ? QStringLiteral("无效") : data.evaluation.errorMessage));
        }
    }
    else
    {
        if (pTable->rowCount() >= row)
        {
            return;
        }

        // 更新数据
        const RadarPerformancePara &perf = data.performance;
        pTable->item(row, eColRadarModel)->setText(data.type);
        pTable->item(row, eColEquipmentEntity)->setText(data.entityCellText());
        pTable->item(row, eColFreqMin)->setText(QString::number(perf.freqMin, 'f', 4)));
        pTable->item(row, eColFreqMax)->setText(QString::number(perf.freqMax, 'f', 4));
        pTable->item(row, eColPwMin)->setText(QString::number(perf.pwMin, 'f', 4));
        pTable->item(row, eColPwMax)->setText(QString::number(perf.pwMax, 'f', 4));
        pTable->item(row, eColPrfMin)->setText(QString::number(perf.prfMin, 'f', 2));
        pTable->item(row, eColPrfMax)->setText(QString::number(perf.prfMax, 'f', 2));
        pTable->item(row, eColRangeKm)->setText(QString::number(perf.detectRange, 'f', 2));

        // 评估结果
        if (data.evaluation.valid)
        {
            pTable->item(row, eColF1)->setText(ProjectPublicInterface::f1TableCellText(m_subfactor.f1Factor));
            pTable->item(row, eColLevel)->setText(data.threatLevelText(m_subfactor.f1Factor));
            pTable->item(row, eColNote)->setText(QStringLiteral("有效"));
        }
        else
        {
            pTable->item(row, eColF1)->setText(QStringLiteral("—"));
            pTable->item(row, eColLevel)->setText(QStringLiteral("—"));
            pTable->item(row, eColNote)->setText(data.evaluation.errorMessage.isEmpty() ? QStringLiteral("无效") : data.evaluation.errorMessage);
        }
    }
}

void RZThreatAssess::displayDataToTable(int row)
{
    if (row < 0 || row >= ui->tableThreatList->rowCount() || row >= m_radarSources.size())
    {
        return;
    }

    // 显示对象与数据
    auto *const pTable = ui->tableThreatList;
    const auto &refRecord = m_radarSources.at(row);

    // 相识评估结果
    if (refRecord.evaluation.valid)
    {
        pTable->item(row, eColF1)->setText(ProjectPublicInterface::f1TableCellText(m_subfactor.f1Factor));
        pTable->item(row, eColLevel)->setText(refRecord.threatLevelText(m_subfactor.f1Factor));
        pTable->item(row, eColNote)->setText(QStringLiteral("有效"));
    }
    else
    {
        pTable->item(row, eColF1)->setText(QString("—"));
        pTable->item(row, eColLevel)->setText(QString("—"));
        pTable->item(row, eColNote)->setText(refRecord.evaluation.errorMessage.isEmpty() ? QString("无效") : refRecord.evaluation.errorMessage);
    }
}

void RZThreatAssess::assessRadarThreat()
{
    if (!m_radarSources.isEmpty())
    {
        // 用首行性能参数初始化全局「子因子与合成」
        m_subfactor = factorsFromPerformance(m_radarSources[0].performance);

        // 显示 子因子与合成
        m_setThreatPanel->displaySubfactor(m_subfactor);

        // 计算 评估结果
        for (int rowIndex = 0; rowIndex < m_radarSources.size(); ++rowIndex)
        {
            // 计算评估结果
            auto result = ProjectPublicInterface::calculateThreatResult(m_radarSources[rowIndex].performance,
                                                                      m_radarSources[rowIndex].typicalPara,
                                                                      m_subfactor);
            // 缓存评估结果
            if (result.valid)
            {
                m_radarSources[rowIndex].evaluation = result;
            }

            // 显示评估结果
            displayDataToTable(rowIndex);
        }
    }
    else
    {
        // 清空
        m_setThreatPanel->clearTargetEdits();

        // 清空评估结果
        m_lastResult = RadarThreatAssessResult();

        // 显示评估结果
        m_setThreatPanel->displayResult(m_lastResult);
    }
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
        m_setThreatPanel->clearTargetEdits();
        m_lastResult = RadarThreatAssessResult();
        m_setThreatPanel->displayResult(m_lastResult);
    }
}

void RZThreatAssess::onClearTable()
{
    m_radarSources.clear();
    ui->tableThreatList->setRowCount(0); // 不逐 removeRow，直接清空更高效
    m_setThreatPanel->clearTargetEdits();
    m_lastResult = RadarThreatAssessResult();
    m_setThreatPanel->displayResult(m_lastResult);
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
    if (ProjectPublicInterface::parseDouble(ProjectPublicInterface::itemText(threatTable, row, eColFreqMin), &parsedValue))
    {
        m_setThreatPanel->setFreqMin(parsedValue);
    }
    if (ProjectPublicInterface::parseDouble(ProjectPublicInterface::itemText(threatTable, row, eColFreqMax), &parsedValue))
    {
        m_setThreatPanel->setFreqMax(parsedValue);
    }
    if (ProjectPublicInterface::parseDouble(ProjectPublicInterface::itemText(threatTable, row, eColPwMin), &parsedValue))
    {
        m_setThreatPanel->setPwMin(parsedValue);
    }
    if (ProjectPublicInterface::parseDouble(ProjectPublicInterface::itemText(threatTable, row, eColPwMax), &parsedValue))
    {
        m_setThreatPanel->setPwMax(parsedValue);
    }
    if (ProjectPublicInterface::parseDouble(ProjectPublicInterface::itemText(threatTable, row, eColPrfMin), &parsedValue))
    {
        m_setThreatPanel->setPrfMin(parsedValue);
    }
    if (ProjectPublicInterface::parseDouble(ProjectPublicInterface::itemText(threatTable, row, eColPrfMax), &parsedValue))
    {
        m_setThreatPanel->setPrfMax(parsedValue);
    }
    if (ProjectPublicInterface::parseDouble(ProjectPublicInterface::itemText(threatTable, row, eColRangeKm), &parsedValue))
    {
        m_setThreatPanel->setRangeKm(parsedValue);
    }

    if (row < m_radarSources.size())
    {
        const RadarThreatAssessRecord &sourceRecord = m_radarSources.at(row);
        m_setThreatPanel->setEditRadarModel(sourceRecord.type);
        m_setThreatPanel->setEditEquipmentEntity(sourceRecord.entityCellText());
        // 代表值 spin：优先记录内 representative，否则用 evaluation 或区间几何均值
        m_setThreatPanel->syncRepresentativeCentersToUi(sourceRecord.typicalPara, m_setThreatPanel->readInputFromUi(), sourceRecord.evaluation);
        if (sourceRecord.situationDefenseIndex >= 0)
        {
            m_setThreatPanel->setDefenseComboIndex(sourceRecord.situationDefenseIndex);
        }
        else
        {
            m_setThreatPanel->setDefenseComboIndex(ThreatAssessUi::kDefaultPositionDefenseComboIndex);
        }
        if (sourceRecord.situationRadModeIndex >= 0)
        {
            m_setThreatPanel->setRadiationComboIndex(sourceRecord.situationRadModeIndex);
        }
        else
        {
            m_setThreatPanel->setRadiationComboIndex(ThreatAssessUi::kDefaultRadiationModeComboIndex);
        }
    }
    else
    {
        m_setThreatPanel->clearTargetEdits();
    }

    // 同步后按当前表单与全局子因子重算本行并刷新底部展示
    m_setThreatPanel->onCalculate();
}

void RZThreatAssess::onTableCurrentCellChanged(int currentRow, int, int, int)
{
    if (m_syncingEntityPick)
    {
        return;
    }
    if (currentRow < 0)
    {
        m_setThreatPanel->clearEquipmentEntityEdit();
        return;
    }
    // 评估弹窗未打开时仅切换表格选中行，避免在未查看评估界面时反复触发计算
    if (m_setThreatPanel == nullptr || !m_setThreatPanel->isVisible())
    {
        return;
    }
    loadFormFromTableRow(currentRow);
}

void RZThreatAssess::onThreatTableCellDoubleClicked(int row, int column)
{
    Q_UNUSED(column);
    if (row < 0 || row >= ui->tableThreatList->rowCount())
    {
        return;
    }
    m_setThreatPanel->show();
    m_setThreatPanel->raise();
    m_setThreatPanel->activateWindow();
    loadFormFromTableRow(row);
}

void RZThreatAssess::onCalcAllRadarThreats()
{
    if (m_radarSources.isEmpty())
    {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("表格中无数据，请先加载作战方案。"));
        return;
    }

    // 右侧六个 spin 作为全体行共用的子因子与合成快照
    m_subfactor = m_setThreatPanel->readSubfactorSynthesisFromUi();

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
        RadarThreatAssessResult rowResult = ProjectPublicInterface::calculateThreatResult(rowPerformance, m_radarSources[row].typicalPara,
                                                              m_subfactor);
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

void RZThreatAssess::applySubfactorSynthesisToAllRows()
{
    if (m_radarSources.isEmpty())
    {
        return;
    }
    m_subfactor = m_setThreatPanel->readSubfactorSynthesisFromUi();
    for (int row = 0; row < m_radarSources.size(); ++row)
    {
        RadarThreatAssessResult rowResult = ProjectPublicInterface::calculateThreatResult(m_radarSources[row].performance, m_radarSources[row].typicalPara,
                                                      m_subfactor);
        if (!rowResult.valid)
        {
            continue;
        }
        m_radarSources[row].evaluation = rowResult;
        displayDataToTable(row);
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
        for (const RadarThreatAssessRecord &sourceRecord : m_radarSources)
        {
            if (sourceRecord.evaluation.valid)
            {
                m_lastResult = sourceRecord.evaluation;
                break;
            }
        }
    }
    m_setThreatPanel->displayResult(m_lastResult);
}

// --- 槽：代表值或子因子与合成编辑 ---
void RZThreatAssess::onRepresentativeCentersEdited()
{
    if (m_setThreatPanel->isBlockingResultSpinSignals())
    {
        return;
    }
    const int row = ui->tableThreatList->currentRow();
    if (row < 0 || row >= m_radarSources.size())
    {
        return;
    }
    // 仅重算当前行，不改变其他行的 representative
    m_radarSources[row].typicalPara = m_setThreatPanel->readRepresentativeCentersFromUi();
    const RadarPerformancePara uiPerformance = m_setThreatPanel->readInputFromUi();
    m_subfactor = m_setThreatPanel->readSubfactorSynthesisFromUi();
    RadarThreatAssessResult newResult = ProjectPublicInterface::calculateThreatResult(uiPerformance, m_radarSources[row].typicalPara, m_subfactor);
    if (!newResult.valid)
    {
        return;
    }
    m_radarSources[row].evaluation = newResult;
    m_lastResult = newResult;
    displayDataToTable(row);
    m_setThreatPanel->displayResult(newResult);
}

void RZThreatAssess::onSubfactorSynthesisEdited()
{
    if (m_setThreatPanel->isBlockingResultSpinSignals())
    {
        return;
    }
    applySubfactorSynthesisToAllRows();
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
    if (!ProjectPublicInterface::parseDouble(ProjectPublicInterface::itemText(threatTable, row, eColFreqMin), &cellValue))
    {
        if (errorMessage)
        {
            *errorMessage = QStringLiteral("频率下限不是有效数字");
        }
        return false;
    }
    parsedPerformance.freqMin = cellValue;
    if (!ProjectPublicInterface::parseDouble(ProjectPublicInterface::itemText(threatTable, row, eColFreqMax), &cellValue))
    {
        if (errorMessage)
        {
            *errorMessage = QStringLiteral("频率上限不是有效数字");
        }
        return false;
    }
    parsedPerformance.freqMax = cellValue;
    if (!ProjectPublicInterface::parseDouble(ProjectPublicInterface::itemText(threatTable, row, eColPwMin), &cellValue))
    {
        if (errorMessage)
        {
            *errorMessage = QStringLiteral("脉宽下限不是有效数字");
        }
        return false;
    }
    parsedPerformance.pwMin = cellValue;
    if (!ProjectPublicInterface::parseDouble(ProjectPublicInterface::itemText(threatTable, row, eColPwMax), &cellValue))
    {
        if (errorMessage)
        {
            *errorMessage = QStringLiteral("脉宽上限不是有效数字");
        }
        return false;
    }
    parsedPerformance.pwMax = cellValue;
    if (!ProjectPublicInterface::parseDouble(ProjectPublicInterface::itemText(threatTable, row, eColPrfMin), &cellValue))
    {
        if (errorMessage)
        {
            *errorMessage = QStringLiteral("PRF_min 不是有效数字");
        }
        return false;
    }
    parsedPerformance.prfMin = cellValue;
    if (!ProjectPublicInterface::parseDouble(ProjectPublicInterface::itemText(threatTable, row, eColPrfMax), &cellValue))
    {
        if (errorMessage)
        {
            *errorMessage = QStringLiteral("PRF_max 不是有效数字");
        }
        return false;
    }
    parsedPerformance.prfMax = cellValue;
    if (!ProjectPublicInterface::parseDouble(ProjectPublicInterface::itemText(threatTable, row, eColRangeKm), &cellValue))
    {
        if (errorMessage)
        {
            *errorMessage = QStringLiteral("探测距离不是有效数字");
        }
        return false;
    }
    parsedPerformance.detectRange = cellValue;

    QString validationMessage;
    if (!ProjectPublicInterface::validateRadarInput(parsedPerformance, &validationMessage))
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

    RadarThreatAssessRecord updatedRecord;
    updatedRecord.id = equipmentId;
    updatedRecord.name = equipmentName;
    updatedRecord.performance = performance;
    const RadarThreatAssessRecord &previousRecord = m_radarSources.at(row);
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
        updatedRecord.evaluation = ProjectPublicInterface::calculateThreatResult(performance, updatedRecord.typicalPara, m_setThreatPanel->readSubfactorSynthesisFromUi());
    }
    m_radarSources[row] = updatedRecord;

    threatTable->setItem(row, eColRadarModel, createCenterItem(updatedRecord.type));
    threatTable->setItem(row, eColEquipmentEntity, createCenterItem(updatedRecord.entityCellText()));

    if (updatedRecord.evaluation.valid)
    {
        threatTable->setItem(row, eColF1, createCenterItem(ProjectPublicInterface::f1TableCellText(m_subfactor.f1Factor)));
        threatTable->setItem(row, eColLevel, createThreatLevelItem(updatedRecord.threatLevelText(m_subfactor.f1Factor)));
        threatTable->setItem(row, eColNote, createCenterItem(QStringLiteral("有效")));
    }
    else
    {
        threatTable->setItem(row, eColF1, createCenterItem(QStringLiteral("—")));
        threatTable->setItem(row, eColLevel, createThreatLevelItem(QStringLiteral("—")));
        threatTable->setItem(row, eColNote, createCenterItem(updatedRecord.evaluation.errorMessage));
    }
}

void RZThreatAssess::onSituationConfirm()
{
    const RadarPerformancePara uiPerformance = m_setThreatPanel->readInputFromUi();
    QString validationError;
    if (!ProjectPublicInterface::validateRadarInput(uiPerformance, &validationError))
    {
        return;
    }

    m_subfactor = m_setThreatPanel->readSubfactorSynthesisFromUi();
    const RadarTypicalPara representativeCenters = m_setThreatPanel->readRepresentativeCentersFromUi();
    const RadarThreatAssessResult assessmentResult =
            ProjectPublicInterface::calculateThreatResult(uiPerformance, representativeCenters, m_subfactor);
    m_lastResult = assessmentResult;

    const int currentRow = ui->tableThreatList->currentRow();
    if (currentRow >= 0 && currentRow < m_radarSources.size())
    {
        m_radarSources[currentRow].typicalPara = representativeCenters;
    }
    if (assessmentResult.valid && currentRow >= 0 && currentRow < m_radarSources.size())
    {
        m_radarSources[currentRow].situationDefenseIndex = m_setThreatPanel->defenseComboCurrentIndex();
        m_radarSources[currentRow].situationRadModeIndex = m_setThreatPanel->radiationComboCurrentIndex();
        applyEvaluationToRow(currentRow, uiPerformance, m_radarSources[currentRow].id, m_radarSources[currentRow].name,
                             &assessmentResult);
    }
    refreshSituationDisplay();
}

// --- 点击「确定」后：将面板态势写回当前行，更新摘要/有效威胁度，并刷新威胁等级列 ---
void RZThreatAssess::refreshSituationDisplay()
{
    m_setThreatPanel->refreshSituationSummaryEffectiveProgress(&m_lastResult);

    const int currentRow = ui->tableThreatList->currentRow();
    if (currentRow < 0 || currentRow >= m_radarSources.size())
    {
        return;
    }
    RadarThreatAssessRecord &mutableRecord = m_radarSources[currentRow];
    mutableRecord.situationDefenseIndex = m_setThreatPanel->defenseComboCurrentIndex();
    mutableRecord.situationRadModeIndex = m_setThreatPanel->radiationComboCurrentIndex();
    displayDataToTable(currentRow);
}