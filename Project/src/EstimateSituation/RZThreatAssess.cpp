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

// 表格相关属性封装函数
namespace
{
    // 威胁列表列序
    enum EThreatTableCol
    {
        eColEquipID = 0,    // 设备ID
        eColEquipType,      // 雷达型号
        eColEquipEntity,    // 装备实体
        // eColFreqMin,          // 频率下限(GHz)
        // eColFreqMax,          // 频率上限(GHz)
        // eColPwMin,            // 脉宽下限(μs)
        // eColPwMax,            // 脉宽上限(μs)
        // eColPrfMin,           // PRF 下限(Hz)
        // eColPrfMax,           // PRF 上限(Hz)
        // eColRangeKm,          // 探测距离(千米)
        eColF1,             // 体制威胁指数 F1
        eColLevel,          // 威胁等级
        eColNote,           // 说明
        eColCount           // 列数（哨兵，非数据列）
    };

    // 创建居中表格项
    static QTableWidgetItem *createCenterItem(const QString &text)
    {
        auto *cellItem = new QTableWidgetItem(text);
        cellItem->setTextAlignment(Qt::AlignCenter);
        return cellItem;
    }

    /**
     * @brief 根据威胁等级文案设置单元格的背景色、前景色、字体与 tooltip
     *
     * 五档等级与 RadarThreatAssessRecord::threatLevelFromF1 一一对应：
     *   极高 (F1 ≥ 0.8) → 深红底白字，加粗     | 高危火控/精确制导雷达
     *   高   (F1 ≥ 0.6) → 橙色底白字，加粗      | 中远程跟踪/火控雷达
     *   中   (F1 ≥ 0.4) → 琥珀底深字            | 中程搜索/引导雷达
     *   低   (F1 ≥ 0.2) → 浅绿底深字            | 远程预警/监视雷达
     *   极低 (F1 <  0.2) → 浅灰底深灰字          | 气象/导航等民用类雷达
     *
     * 占位符 "—" 或空文本恢复默认配色（无色无 tooltip）。
     */
    static void setItemBackground(QTableWidgetItem *item)
    {
        if (!item)
        {
            return;
        }

        const QString level = item->text();

        // 占位或无等级：恢复默认配色并清除 tooltip
        if (level == QStringLiteral("—") || level.isEmpty())
        {
            item->setBackground(QBrush());
            item->setForeground(QBrush());
            item->setToolTip(QString());
            QFont font = item->font();
            font.setBold(false);
            item->setFont(font);
            return;
        }

        // 五档配色/tooltip 查找表（等级名、背景色、前景色、是否加粗、tooltip）
        struct LevelStyle
        {
            const char16_t *name;
            QColor background;
            QColor foreground;
            bool bold;
            const char16_t *tooltip;
        };

        static const LevelStyle kStyles[] = {
            { u"极高", QColor(0xc6, 0x28, 0x28), QColor(Qt::white), true,
              u"极高威胁（F1 ≥ 0.8）：高危火控/精确制导雷达，需立即规避或实施电子攻击" },
            { u"高",   QColor(0xef, 0x6c, 0x00), QColor(Qt::white), true,
              u"高威胁（F1 ≥ 0.6）：中远程跟踪/火控雷达，建议优先压制" },
            { u"中",   QColor(0xf9, 0xa8, 0x25), QColor(0x1a, 0x1a, 0x1a), false,
              u"中等威胁（F1 ≥ 0.4）：中程搜索/引导雷达，需持续监视" },
            { u"低",   QColor(0x7c, 0xb3, 0x42), QColor(0x1a, 0x1a, 0x1a), false,
              u"低威胁（F1 ≥ 0.2）：远程预警/监视雷达，常规关注" },
            { u"极低", QColor(0xec, 0xef, 0xf1), QColor(0x37, 0x47, 0x4f), false,
              u"极低威胁（F1 < 0.2）：气象/导航等民用类雷达，可忽略" },
        };

        // 匹配等级并应用样式
        for (const auto &style : kStyles)
        {
            if (level == QString::fromUtf16(style.name))
            {
                item->setBackground(style.background);
                item->setForeground(style.foreground);
                item->setToolTip(QString::fromUtf16(style.tooltip));
                item->setTextAlignment(Qt::AlignCenter);

                QFont font = item->font();
                font.setBold(style.bold);
                item->setFont(font);
                return;
            }
        }

        // 未知等级文案：恢复默认配色
        item->setBackground(QBrush());
        item->setForeground(QBrush());
        item->setToolTip(QString());
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
        factors.f1Raw = 0.40 * factors.freqFactor + 0.30 * factors.prfFactor + 0.30 * factors.pwFactor;
        factors.rangeMod = ProjectPublicInterface::rangeModifier(performance.detectRange, factors.f1Raw);
        factors.F1 = ProjectPublicInterface::clamp01(factors.f1Raw * factors.rangeMod);
        return factors;
    }

    // 居中显示
    static void setTableItemCenter(QTableWidget *pTable)
    {
        for (int row = 0; row < pTable->rowCount(); ++row)
        {
            for (int col = 0; col < pTable->columnCount(); ++col)
            {
                auto item = pTable->item(row, col);
                if (item)
                {
                    item->setTextAlignment(Qt::AlignCenter);
                }
            }
        }
    }
} // namespace

RZThreatAssess::RZThreatAssess(QWidget *parent)
    : QWidget(parent), ui(new Ui::RZThreatAssess)
{
    ui->setupUi(this);

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
    // 控件管理
    widgetManage();

    // 生成测试数据
    generateTestData();
    // 评估雷达威胁
    assessRadarThreat();
    // 显示数据到表格
    displayDataToTable();
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
            QString::fromUtf8("设备ID"),
            QString::fromUtf8("雷达型号"),
            QString::fromUtf8("装备实体"),
            // QString::fromUtf8("频率下限(GHz)"),
            // QString::fromUtf8("频率上限(GHz)"),
            // QString::fromUtf8("脉宽下限(μs)"),
            // QString::fromUtf8("脉宽上限(μs)"),
            // QString::fromUtf8("PRF下限(Hz)"),
            // QString::fromUtf8("PRF上限(Hz)"),
            // QString::fromUtf8("探测距离(千米)"),
            QString::fromUtf8("体制威胁指数F1"),
            QString::fromUtf8("威胁等级"),
            QString::fromUtf8("说明"),
    };
    ui->tableThreatList->setHorizontalHeaderLabels(headers);

    // 隐藏第一列
    ui->tableThreatList->setColumnHidden(0, true);
}

void RZThreatAssess::widgetManage()
{
    // 隐藏删除与清空按钮
    ui->btnRemoveRows->hide();
    ui->btnClearTable->hide();
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

void RZThreatAssess::assessRadarThreat()
{
    if (!m_radarSources.isEmpty())
    {
        // 计算 评估结果
        for (int rowIndex = 0; rowIndex < m_radarSources.size(); ++rowIndex)
        {
            // 计算评估结果
            auto result = ProjectPublicInterface::calculateThreatResult(m_radarSources[rowIndex]);
            // 缓存评估结果
            if (result.valid)
            {
                m_radarSources[rowIndex].result = result;
            }
        }
    }
}

void RZThreatAssess::assessRadarThreatSort()
{
    // 对威胁等级进行排序
    std::sort(m_radarSources.begin(), m_radarSources.end(),
              [](const RadarThreatAssessRecord &a, const RadarThreatAssessRecord &b)
              {
                  return a.result.threatLevel < b.result.threatLevel;
              });
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

void RZThreatAssess::displayDataToTable(const RadarThreatAssessRecord &record, int row)
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
        const RadarPerformancePara &perf = record.perfPara;
        pTable->setItem(row, eColEquipID, new QTableWidgetItem(record.equipID));
        pTable->setItem(row, eColEquipType, new QTableWidgetItem(record.typeName));
        pTable->setItem(row, eColEquipEntity, new QTableWidgetItem(record.entityName));
        // pTable->setItem(row, eColFreqMin, new QTableWidgetItem(QString::number(perf.freqMin, 'f', 4)));
        // pTable->setItem(row, eColFreqMax, new QTableWidgetItem(QString::number(perf.freqMax, 'f', 4)));
        // pTable->setItem(row, eColPwMin, new QTableWidgetItem(QString::number(perf.pwMin, 'f', 4)));
        // pTable->setItem(row, eColPwMax, new QTableWidgetItem(QString::number(perf.pwMax, 'f', 4)));
        // pTable->setItem(row, eColPrfMin, new QTableWidgetItem(QString::number(perf.prfMin, 'f', 2)));
        // pTable->setItem(row, eColPrfMax, new QTableWidgetItem(QString::number(perf.prfMax, 'f', 2)));
        // pTable->setItem(row, eColRangeKm, new QTableWidgetItem(QString::number(perf.detectRange, 'f', 2)));

        // 评估结果
        if (record.result.valid)
        {
            pTable->setItem(row, eColF1, createCenterItem(ProjectPublicInterface::f1TableCellText(record.factors.F1)));
            pTable->setItem(row, eColLevel, createThreatLevelItem(record.threatLevelText(record.factors.F1)));
            pTable->setItem(row, eColNote, createCenterItem(QStringLiteral("有效")));
        }
        else
        {
            pTable->setItem(row, eColF1, createCenterItem(QStringLiteral("—")));
            pTable->setItem(row, eColLevel, createThreatLevelItem(QStringLiteral("—")));
            pTable->setItem(row, eColNote, createCenterItem(record.result.errorMsg.isEmpty() ? QStringLiteral("无效") : record.result.errorMsg));
        }
    }
    else
    {
        if (pTable->rowCount() >= row)
        {
            return;
        }

        // 更新数据
        const RadarPerformancePara &perf = record.perfPara;
        pTable->item(row, eColEquipID)->setText(record.equipID);
        pTable->item(row, eColEquipType)->setText(record.typeName);
        pTable->item(row, eColEquipEntity)->setText(record.entityName);
        // pTable->item(row, eColFreqMin)->setText(QString::number(perf.freqMin, 'f', 4));
        // pTable->item(row, eColFreqMax)->setText(QString::number(perf.freqMax, 'f', 4));
        // pTable->item(row, eColPwMin)->setText(QString::number(perf.pwMin, 'f', 4));
        // pTable->item(row, eColPwMax)->setText(QString::number(perf.pwMax, 'f', 4));
        // pTable->item(row, eColPrfMin)->setText(QString::number(perf.prfMin, 'f', 2));
        // pTable->item(row, eColPrfMax)->setText(QString::number(perf.prfMax, 'f', 2));
        // pTable->item(row, eColRangeKm)->setText(QString::number(perf.detectRange, 'f', 2));

        // 评估结果
        if (record.result.valid)
        {
            pTable->item(row, eColF1)->setText(ProjectPublicInterface::f1TableCellText(record.factors.F1));
            pTable->item(row, eColLevel)->setText(record.threatLevelText(record.factors.F1));
            setItemBackground(pTable->item(row, eColLevel));
            pTable->item(row, eColNote)->setText(QStringLiteral("有效"));
        }
        else
        {
            pTable->item(row, eColF1)->setText(QStringLiteral("—"));
            pTable->item(row, eColLevel)->setText(QStringLiteral("—"));
            setItemBackground(pTable->item(row, eColLevel));
            pTable->item(row, eColNote)->setText(record.result.errorMsg.isEmpty() ? QStringLiteral("无效") : record.result.errorMsg);
        }
    }

    // 居中显示
    setTableItemCenter(pTable);
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

    // 显示评估结果
    if (refRecord.result.valid)
    {
        pTable->item(row, eColF1)->setText(ProjectPublicInterface::f1TableCellText(refRecord.factors.F1));
        pTable->item(row, eColLevel)->setText(refRecord.threatLevelText(refRecord.factors.F1));
        setItemBackground(pTable->item(row, eColLevel));
        pTable->item(row, eColNote)->setText(QStringLiteral("有效"));
    }
    else
    {
        pTable->item(row, eColF1)->setText(QString("—"));
        pTable->item(row, eColLevel)->setText(QString("—"));
        setItemBackground(pTable->item(row, eColLevel));
        pTable->item(row, eColNote)->setText(refRecord.result.errorMsg.isEmpty() ? QString("无效") : refRecord.result.errorMsg);
    }

    // 居中显示
    setTableItemCenter(pTable);
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
    m_setThreatPanel->displayData(m_radarSources.at(row).factors);
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
    m_radarSources[m_selectRow] = m_setThreatPanel->m_record;

    // 保存到数据库-待处理

    // 评估雷达威胁排序
    assessRadarThreatSort();

    // 重新显示
    displayDataToTable();

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

    // 清空数据库-待处理

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

        const auto rowResult = ProjectPublicInterface::calculateThreatResult(record);
        if (!rowResult.valid)
        {
            QMessageBox::warning(this, QStringLiteral("参数错误"),  QStringLiteral("第 %1 行：%2").arg(row + 1).arg(rowResult.errorMsg));
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

// 公共接口实现
void RZThreatAssess::addRadarData(const RadarPerformancePara &data)
{
    // 查找是否存在同名雷达
    for (int i = 0; i < m_radarSources.size(); ++i) {
        if (m_radarSources[i].entityName == data.name) {
            // 更新现有雷达
            updateRadarData(data);
            return;
        }
    }
    
    // 创建新的雷达评估记录
    RadarThreatAssessRecord record;
    record.entityName = data.name;
    record.typeName = data.deviceType;
    record.perfPara = data;
    
    // 计算威胁评估结果
    record.result = ProjectPublicInterface::calculateThreatResult(record);
    
    // 添加到缓存和界面
    m_radarSources.append(record);
    displayDataToTable(record);
    
    // 发送更新信号
    emit radarDataUpdated(data);
}

void RZThreatAssess::updateRadarData(const RadarPerformancePara &data)
{
    // 查找同名雷达
    for (int i = 0; i < m_radarSources.size(); ++i) {
        if (m_radarSources[i].entityName == data.name) {
            // 更新性能参数
            m_radarSources[i].perfPara = data;
            m_radarSources[i].typeName = data.deviceType;
            
            // 重新计算威胁评估结果
            m_radarSources[i].result = ProjectPublicInterface::calculateThreatResult(m_radarSources[i]);
            
            // 更新界面
            displayDataToTable(m_radarSources[i], i);
            
            // 发送更新信号
            emit radarDataUpdated(data);
            return;
        }
    }
    
    // 如果不存在，添加新雷达
    addRadarData(data);
}

void RZThreatAssess::deleteRadarData(const QString &name)
{
    // 查找并删除同名雷达
    for (int i = 0; i < m_radarSources.size(); ++i) {
        if (m_radarSources[i].entityName == name) {
            m_radarSources.removeAt(i);
            ui->tableThreatList->removeRow(i);
            
            // 发送删除信号
            emit radarDataRemoved(name);
            return;
        }
    }
}

// 与 RZSourceRadiation 数据互通的槽函数
void RZThreatAssess::onRadarDataChanged(const RadarPerformancePara &data)
{
    // 调用公共接口添加或更新雷达数据
    addRadarData(data);
}

void RZThreatAssess::onRadarDataDeleted(const QString &name)
{
    // 调用公共接口删除雷达数据
    deleteRadarData(name);
}
