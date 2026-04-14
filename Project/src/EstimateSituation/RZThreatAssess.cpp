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
    if (!result.valid)
    {
        return QStringLiteral("—");
    }
    if (!std::isfinite(f1))
    {
        return QStringLiteral("—");
    }

    // situationRadModeIndex == -1：未写入行态势时与批量默认一致，使用界面「正常」辐射项对应索引
    int radiationModeIndex = workPara.situationRadModeIndex;
    if (radiationModeIndex < 0)
    {
        radiationModeIndex = ThreatAssessUi::kDefaultRadiationMode;
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
    m_setThreatPanel = new SetRadarThreatAssess(this);
    m_setThreatPanel->setWindowTitle(QStringLiteral("辐射源信息与威胁评估"));
    m_setThreatPanel->setWindowFlag(Qt::Dialog, true);
    m_setThreatPanel->resize(980, 620);
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
    // 单元格双击
    connect(ui->tableThreatList, &QTableWidget::cellDoubleClicked, this, &RZThreatAssess::onTableDoubleClicked);
    // 接收评估结果
    connect(m_setThreatPanel, &SetRadarThreatAssess::sigEvaluteResult, this, &RZThreatAssess::onRecvAssessResult);
    // 删除选中行
    connect(ui->btnRemoveRows, &QPushButton::clicked, this, &RZThreatAssess::onRemoveSelectedRows);
    // 清空表格
    connect(ui->btnClearTable, &QPushButton::clicked, this, &RZThreatAssess::onClearTable);
    // 批量评估
    connect(ui->btnCalcAllRadarThreats, &QPushButton::clicked, this, &RZThreatAssess::onCalculateAllRadarThreats);
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

        newRecord.entityName = id;
        newRecord.typeName = type;
        newRecord.perfPara = ProjectPublicInterface::radarInputFromPresetIndex(presetIndex);
        newRecord.result = ProjectPublicInterface::evaluate(newRecord.perfPara);
        newRecord.typicalPara.freq = newRecord.result.freq;
        newRecord.typicalPara.pw = newRecord.result.pw;
        newRecord.typicalPara.prf = newRecord.result.prf;
        newRecord.typicalPara.valid = true;
        newRecord.workPara.situationDefenseIndex = -1;
        newRecord.workPara.situationRadModeIndex = -1;

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
    for (const auto &var : m_radarSources)
    {
        displayDataToTable(var);
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
        const RadarPerformancePara &perf = data.perfPara;
        pTable->setItem(row, eColRadarModel, new QTableWidgetItem(data.typeName));
        pTable->setItem(row, eColEquipmentEntity, new QTableWidgetItem(data.entityName));
        pTable->setItem(row, eColFreqMin, new QTableWidgetItem(QString::number(perf.freqMin, 'f', 4)));
        pTable->setItem(row, eColFreqMax, new QTableWidgetItem(QString::number(perf.freqMax, 'f', 4)));
        pTable->setItem(row, eColPwMin, new QTableWidgetItem(QString::number(perf.pwMin, 'f', 4)));
        pTable->setItem(row, eColPwMax, new QTableWidgetItem(QString::number(perf.pwMax, 'f', 4)));
        pTable->setItem(row, eColPrfMin, new QTableWidgetItem(QString::number(perf.prfMin, 'f', 2)));
        pTable->setItem(row, eColPrfMax, new QTableWidgetItem(QString::number(perf.prfMax, 'f', 2)));
        pTable->setItem(row, eColRangeKm, new QTableWidgetItem(QString::number(perf.detectRange, 'f', 2)));

        // 评估结果
        if (data.result.valid)
        {
            pTable->setItem(row, eColF1, new QTableWidgetItem(ProjectPublicInterface::f1TableCellText(m_radarfactor.f1Factor)));
            pTable->setItem(row, eColLevel, new QTableWidgetItem(data.threatLevelText(m_radarfactor.f1Factor)));
            pTable->setItem(row, eColNote, new QTableWidgetItem(QStringLiteral("有效")));
        }
        else
        {
            pTable->setItem(row, eColF1, new QTableWidgetItem(QStringLiteral("—")));
            pTable->setItem(row, eColLevel, new QTableWidgetItem(QStringLiteral("—")));
            pTable->setItem(row, eColNote, new QTableWidgetItem(data.result.errorMessage.isEmpty() ? QStringLiteral("无效") : data.result.errorMessage));
        }
    }
    else
    {
        if (pTable->rowCount() >= row)
        {
            return;
        }

        // 更新数据
        const RadarPerformancePara &perf = data.perfPara;
        pTable->item(row, eColRadarModel)->setText(data.typeName);
        pTable->item(row, eColEquipmentEntity)->setText(data.entityName);
        pTable->item(row, eColFreqMin)->setText(QString::number(perf.freqMin, 'f', 4));
        pTable->item(row, eColFreqMax)->setText(QString::number(perf.freqMax, 'f', 4));
        pTable->item(row, eColPwMin)->setText(QString::number(perf.pwMin, 'f', 4));
        pTable->item(row, eColPwMax)->setText(QString::number(perf.pwMax, 'f', 4));
        pTable->item(row, eColPrfMin)->setText(QString::number(perf.prfMin, 'f', 2));
        pTable->item(row, eColPrfMax)->setText(QString::number(perf.prfMax, 'f', 2));
        pTable->item(row, eColRangeKm)->setText(QString::number(perf.detectRange, 'f', 2));

        // 评估结果
        if (data.result.valid)
        {
            pTable->item(row, eColF1)->setText(ProjectPublicInterface::f1TableCellText(m_radarfactor.f1Factor));
            pTable->item(row, eColLevel)->setText(data.threatLevelText(m_radarfactor.f1Factor));
            pTable->item(row, eColNote)->setText(QStringLiteral("有效"));
        }
        else
        {
            pTable->item(row, eColF1)->setText(QStringLiteral("—"));
            pTable->item(row, eColLevel)->setText(QStringLiteral("—"));
            pTable->item(row, eColNote)->setText(data.result.errorMessage.isEmpty() ? QStringLiteral("无效") : data.result.errorMessage);
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
    if (refRecord.result.valid)
    {
        pTable->item(row, eColF1)->setText(ProjectPublicInterface::f1TableCellText(m_radarfactor.f1Factor));
        pTable->item(row, eColLevel)->setText(refRecord.threatLevelText(m_radarfactor.f1Factor));
        pTable->item(row, eColNote)->setText(QStringLiteral("有效"));
    }
    else
    {
        pTable->item(row, eColF1)->setText(QString("—"));
        pTable->item(row, eColLevel)->setText(QString("—"));
        pTable->item(row, eColNote)->setText(refRecord.result.errorMessage.isEmpty() ? QString("无效") : refRecord.result.errorMessage);
    }
}

void RZThreatAssess::assessRadarThreat()
{
    if (!m_radarSources.isEmpty())
    {
        // 用首行性能参数初始化全局「子因子与合成」
        m_radarfactor = factorsFromPerformance(m_radarSources[0].perfPara);

        // 显示 子因子与合成
        m_setThreatPanel->displayData(m_radarfactor);

        // 计算 评估结果
        for (int rowIndex = 0; rowIndex < m_radarSources.size(); ++rowIndex)
        {
            // 计算评估结果
            auto result = ProjectPublicInterface::calculateThreatResult(m_radarSources[rowIndex].perfPara,
                                                                      m_radarSources[rowIndex].typicalPara,
                                                                      m_radarfactor);
            // 缓存评估结果
            if (result.valid)
            {
                m_radarSources[rowIndex].result = result;
            }

            // 显示评估结果
            displayDataToTable(rowIndex);
        }
    }
    else
    {
        // 清空
        m_setThreatPanel->undisplayData();

        // 清空评估结果
        m_result = RadarThreatAssessResult();

        // 显示评估结果
        m_setThreatPanel->displayData(m_result);
    }
}

void RZThreatAssess::onTableDoubleClicked(int row, int column)
{
    Q_UNUSED(column);
    if (row < 0 || row >= m_radarSources.size())
    {
        return;
    }

    // 缓存行数
    m_selectRow = row;

    // 右侧子因子与合成为全局快照，须先写入 spin，底部 F1/有效威胁度与记录一致
    m_setThreatPanel->displayData(m_radarfactor);
    m_setThreatPanel->setDisplay(m_radarSources.at(row));
    m_result = m_radarSources.at(row).result;

    // 显示界面
    m_setThreatPanel->show();
    m_setThreatPanel->raise();
    m_setThreatPanel->activateWindow();
}

void RZThreatAssess::onRecvAssessResult()
{
    // 缓存数据
    m_radarfactor = m_setThreatPanel->m_radarfactor;
    m_radarSources[m_selectRow].workPara = m_setThreatPanel->m_workPara;
    m_radarSources[m_selectRow].typicalPara = m_setThreatPanel->m_typicalPara;
    m_radarSources[m_selectRow].result = m_setThreatPanel->m_result;

    // 更新界面数据
    displayDataToTable(m_radarSources[m_selectRow], m_selectRow);

    // 保存到数据库-待处理

    // 重置标志位
    m_selectRow = -1;
}

void RZThreatAssess::onRemoveSelectedRows()
{
    // 判断是否存在选中行
    auto const pTable = ui->tableThreatList;
    const auto selectIndex = pTable->selectionModel()->selectedRows();
    if (selectIndex.isEmpty())
    {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先选中要删除的行。"));
        return;
    }

    // 获取所有删除行的行号
    QList<int> selectRows;
    for (const auto &modelIndex : selectIndex)
    {
        selectRows.append(modelIndex.row());
    }

    // 自大到小删行，避免删除后行号整体前移导致错位
    std::sort(selectRows.begin(), selectRows.end(), [](int lhs, int rhs)
    {
        return lhs > rhs;
    });
    for (int rowIndex : selectRows)
    {
        // 删除显示
        pTable->removeRow(rowIndex);

        // 删除缓存
        if (rowIndex >= 0 && rowIndex < m_radarSources.size())
        {
            m_radarSources.removeAt(rowIndex);
        }

        // 删除数据库-待处理
    }
}

void RZThreatAssess::onClearTable()
{
    // 清空缓存
    m_radarSources.clear();

    // 清空表格
    ui->tableThreatList->setRowCount(0);

    // 清空子窗口
    m_setThreatPanel->close();
    m_setThreatPanel->undisplayData();
}

void RZThreatAssess::onCalculateAllRadarThreats()
{
    if (m_radarSources.isEmpty())
    {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("表格中无数据，请先加载作战方案。"));
        return;
    }

    int completedRowCount = 0;
    for (int row = 0; row < m_radarSources.size(); ++row)
    {
        // 默认参数保护
        protectRealWorkPara(row);

        // 获取参数
        auto &record = m_radarSources[row];

        // 参数检查
        QString validationError;
        if (!ProjectPublicInterface::validateRadarInput(record.perfPara, &validationError))
        {
            QMessageBox::warning(this, QStringLiteral("参数错误"), QStringLiteral("第 %1 行：%2").arg(row + 1).arg(validationError));
            continue;
        }

        const auto rowResult = ProjectPublicInterface::calculateThreatResult(record.perfPara, record.typicalPara, m_radarfactor);
        if (!rowResult.valid)
        {
            QMessageBox::warning(this, QStringLiteral("参数错误"),  QStringLiteral("第 %1 行：%2").arg(row + 1).arg(rowResult.errorMessage));
            continue;
        }

        // 缓存数据
        record.result = rowResult;
        // 显示结果
        displayDataToTable(row);

        ++completedRowCount;
    }

    QMessageBox::information(this, QStringLiteral("完成"),
                             QStringLiteral("已重算全部行（共 %1 行）。性能参数、代表值均来自缓存；"
                                     "子因子与合成由首行性能参数统一推导。威胁等级按「F1×辐射模式系数」映射。")
                             .arg(completedRowCount));
}

void RZThreatAssess::protectRealWorkPara(int row)
{
    if (row < 0 || row >= m_radarSources.size())
    {
        return;
    }

    // 批量重算前把「未单独设置」的行统一到界面默认 combo 索引
    auto &workPara = m_radarSources[row].workPara;
    if (workPara.situationDefenseIndex < 0)
    {
        workPara.situationDefenseIndex = ThreatAssessUi::kDefaultPositionDefense;
    }
    if (workPara.situationRadModeIndex < 0)
    {
        workPara.situationRadModeIndex = ThreatAssessUi::kDefaultRadiationMode;
    }
}