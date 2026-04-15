//
// Created by admin on "2026.04.09 T 19:19:20".
//

// You may need to build the project (run Qt uic code generator) to get "ui_RZSourceRadiation.h" resolved

#include "RZSourceRadiation.h"
#include "ui_RZSourceRadiation.h"
#include <QAbstractItemView>
#include <QHeaderView>
#include <QPushButton>
#include <QStyledItemDelegate>


namespace
{
    /**
     *Qt 表格 / 列表控件的自定义委托（Delegate），专门用来统一控制单元格的显示样式。
     *QStyledItemDelegate：Qt 里专门管 “控件项怎么显示” 的基类。
     * @brief 表格显示委托
     * @details 统一设置单元格文本"居中 + 大写显示"。
     * Qt 表格 / 列表控件的自定义委托（Delegate），专门用来统一控制单元格的显示样式。
     */
    class UpperCenterTextDelegate final : public QStyledItemDelegate
    {
    public:
        explicit UpperCenterTextDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent)
        {
        }

        void initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const override
        {
            QStyledItemDelegate::initStyleOption(option, index);
            option->displayAlignment = Qt::AlignCenter;
        }

        QString displayText(const QVariant &value, const QLocale &locale) const override
        {
            return QStyledItemDelegate::displayText(value, locale).toUpper();
        }
    };

    template <typename T>
    int findIndexByName(const QVector<T> &container, const QString &name)
    {
        for (int i = 0; i < container.size(); ++i)
        {
            if (container.at(i).name == name)
            {
                return i;
            }
        }
        return -1;
    }

}

RZSourceRadiation::RZSourceRadiation(QWidget *parent)
    : QWidget(parent), ui(new Ui::RZSourceRadiation)
{
    ui->setupUi(this);

    // 初始化参数
    initParams();

    // 初始化对象
    initObject();

    // 关联信号与槽函数
    initConnect();
}

RZSourceRadiation::~RZSourceRadiation()
{
    delete ui;
}

void RZSourceRadiation::initParams()
{
}

void RZSourceRadiation::initObject()
{
    // 生成测试数据
    generateTestData();

    // 初始化表格属性
    initTableAttr();

    // 初始化数据模型
    initTableModel();

    // 显示辐射源数据
    displayData();


}

void RZSourceRadiation::initConnect()
{
    // 四个类型按钮复用同一槽函数，根据 sender() 判断切换模型
    connect(ui->btnRadar, &QPushButton::clicked, this, &RZSourceRadiation::onShowTableData);
    /*connect(ui->btnRadio, &QPushButton::clicked, this, &RZSourceRadiation::onShowTableData);
    connect(ui->btnRadioJam, &QPushButton::clicked, this, &RZSourceRadiation::onShowTableData);
    connect(ui->btnRadarJam, &QPushButton::clicked, this, &RZSourceRadiation::onShowTableData);*/

    // 雷达开关
    /*connect(ui->radarSwitch, &QCheckBox::toggled, this, &RZSourceRadiation::onRadarSwitchChanged);
    // 电台开关
    connect(ui->radioSwitch, &QCheckBox::toggled, this, &RZSourceRadiation::onRadioSwitchChanged);
    // 通信对抗开关
    connect(ui->commJamSwitch, &QCheckBox::toggled, this, &RZSourceRadiation::onCommJamSwitchChanged);
    // 雷达对抗开关
    connect(ui->radarJamSwitch, &QCheckBox::toggled, this, &RZSourceRadiation::onRadarJamSwitchChanged);
    // 防控火力显示开关
    connect(ui->defenseFireDisplaySwitch, &QCheckBox::toggled, this, &RZSourceRadiation::onDefenseFireDisplaySwitchChanged);*/
}

void RZSourceRadiation::generateTestData()
{
    m_radarSources.clear();
    m_radioSource.clear();
    m_radarJammerSource.clear();
    m_RadioJammerSource.clear();

    auto addRadar = [this](const QString &id, const QString &name, const QString &type,
                           int presetIndex, const QString &scanMode, const QString &deviceType)
    {
        RadarThreatAssessRecord record;
        record.equipID = id;
        record.entityName = name;
        record.typeName = type;
        record.perfPara = ProjectPublicInterface::radarInputFromPresetIndex(presetIndex);
        record.perfPara.name = name;
        record.perfPara.scanMode = scanMode;
        record.perfPara.deviceType = deviceType;
        record.result = ProjectPublicInterface::evaluate(record.perfPara);
        record.typicalPara.freq = record.result.freq;
        record.typicalPara.pw = record.result.pw;
        record.typicalPara.prf = record.result.prf;
        record.typicalPara.valid = true;
        record.workPara.situationDefenseIndex = -1;
        record.workPara.situationRadModeIndex = -1;
        m_radarSources.push_back(record);
    };

    addRadar(QStringLiteral("RAD-001"), QStringLiteral("AN/MPQ-53 相控阵雷达"), QStringLiteral("MPQ-53 (PAC-2 火控)"), 2,
             QStringLiteral("电子扫描"), QStringLiteral("高功率火控雷达"));
    addRadar(QStringLiteral("RAD-002"), QStringLiteral("P-18 预警雷达"), QStringLiteral("SPS-48E"), 5,
             QStringLiteral("6rpm"), QStringLiteral("米波远程警戒"));
    addRadar(QStringLiteral("RAD-003"), QStringLiteral("MPQ-64 哨兵雷达"), QStringLiteral("TPS-75"), 7,
             QStringLiteral("旋转扫描"), QStringLiteral("低空补盲雷达"));
}

void RZSourceRadiation::initTableAttr()
{
    // 交互行为
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);               // 不可编辑
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);             // 单行选中
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);              // 全行选中
    ui->tableView->setItemDelegate(new UpperCenterTextDelegate(ui->tableView));      // 内容居中并大写显示
    ui->tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);         // 表头居中
    ui->tableView->setAlternatingRowColors(true);                                    // 奇偶行显示
    ui->tableView->setStyleSheet("QTableView{alternate-background-color:#F5F7FA;}"); // 间隔颜色
    ui->tableView->setSortingEnabled(false);                                         // 关闭自动排序
 // 调整表格列宽
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
    // 字体设置
    QFont headerFont = ui->tableView->horizontalHeader()->font();
    headerFont.setPointSize(11);
    ui->tableView->horizontalHeader()->setFont(headerFont);                          // 表头字体大小

    QFont indexFont = ui->tableView->verticalHeader()->font();
    indexFont.setPointSize(11);
    ui->tableView->verticalHeader()->setFont(indexFont);                             // 序号列字体大小

    QFont cellFont = ui->tableView->font();
    cellFont.setPointSize(10);
    ui->tableView->setFont(cellFont);                                                // 表格内容字体大小

    // 列宽策略：初始化后可手动拖动调整
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tableView->horizontalHeader()->setStretchLastSection(false);

    // 行高按内容自适应
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void RZSourceRadiation::initTableModel()
{
    // 清理旧模型并重新创建，防止重复初始化造成内存泄漏
    qDeleteAll(m_mapModel);
    m_mapModel.clear();

    // 初始模型
    auto createModel = [this](const QStringList &headers) -> QStandardItemModel *
    {
        auto *model = new QStandardItemModel(this);
        model->setColumnCount(headers.size());
        model->setHorizontalHeaderLabels(headers);
        return model;
    };

    // 1) 雷达
    auto *radarModel = createModel(QStringList{
        QString::fromUtf8("名称"),
        QString::fromUtf8("频率"),
        QString::fromUtf8("脉冲重频"),
        QString::fromUtf8("脉宽"),
        QString::fromUtf8("扫描方式"),
        /*QString::fromUtf8("威胁等级"),*/
        /*QString::fromUtf8("设备类型")*/
    });
    m_mapModel.insert(QString::fromUtf8("雷达"), radarModel);

    /*// 2) 电台
    auto *radioModel = createModel(QStringList{
        QString::fromUtf8("名称"),
        QString::fromUtf8("频率"),
        QString::fromUtf8("调制方式"),
        QString::fromUtf8("码速率"),
        QString::fromUtf8("功率/波形"),
        QString::fromUtf8("威胁等级"),
        QString::fromUtf8("设备类型")
    });*/
    /*m_mapModel.insert(QString::fromUtf8("电台"), radioModel);

    // 3) 雷达干扰
    auto *radarJamModel = createModel(QStringList{
        QString::fromUtf8("名称"),
        QString::fromUtf8("干扰类型"),
        QString::fromUtf8("工作频段"),
        QString::fromUtf8("技术体制"),
        QString::fromUtf8("威胁等级"),
        QString::fromUtf8("设备类型")
    });*/
    /*m_mapModel.insert(QString::fromUtf8("雷达干扰"), radarJamModel);

    // 4) 通信干扰
    auto *radioJamModel = createModel(QStringList{
        QString::fromUtf8("名称"),
        QString::fromUtf8("干扰样式"),
        QString::fromUtf8("覆盖频段"),
        QString::fromUtf8("ERP"),
        QString::fromUtf8("威胁等级"),
        QString::fromUtf8("设备类型")
    });
    m_mapModel.insert(QString::fromUtf8("通信干扰"), radioJamModel);
    */
    // 默认显示"雷达"模型
    ui->tableView->setModel(radarModel);
}

void RZSourceRadiation::displayData()
{
    // 清空表格
    for (auto *model : m_mapModel)
    {
        if (model == nullptr)
        {
            continue;
        }
        model->removeRows(0, model->rowCount());
    }

    // 将四类测试数据写入对应模型
    for (const auto &item : m_radarSources)
    {
        displayData(item);
    }
    /*for (const auto &item : m_radioSource)
    {
        displayData(item);
    }
    for (const auto &item : m_radarJammerSource)
    {
        displayData(item);
    }
    for (const auto &item : m_RadioJammerSource)
    {
        displayData(item);
    }*/
}

void RZSourceRadiation::displayData(const RadarThreatAssessRecord &data, int row)
{
    const RadarPerformancePara &perf = data.perfPara;
    auto formatFreqRange = [](double minVal, double maxVal) -> QString
    {
        if (minVal <= 0.0 && maxVal <= 0.0) return QStringLiteral("-");
        if (qFuzzyCompare(minVal, maxVal)) return QString::number(minVal, 'f', 1) + QStringLiteral("GHz");
        return QString::number(minVal, 'f', 1) + QStringLiteral("~") + QString::number(maxVal, 'f', 1) + QStringLiteral("GHz");
    };
    auto formatPrfRange = [](double minVal, double maxVal) -> QString
    {
        if (minVal <= 0.0 && maxVal <= 0.0) return QStringLiteral("-");
        if (qFuzzyCompare(minVal, maxVal)) return QString::number(minVal, 'f', 0) + QStringLiteral("Hz");
        return QString::number(minVal, 'f', 0) + QStringLiteral("~") + QString::number(maxVal, 'f', 0) + QStringLiteral("Hz");
    };
    auto formatPwRange = [](double minVal, double maxVal) -> QString
    {
        if (minVal <= 0.0 && maxVal <= 0.0) return QStringLiteral("-");
        if (qFuzzyCompare(minVal, maxVal)) return QString::number(minVal, 'f', 1) + QStringLiteral("μs");
        return QString::number(minVal, 'f', 1) + QStringLiteral("~") + QString::number(maxVal, 'f', 1) + QStringLiteral("μs");
    };

    writeModelRow(m_mapModel.value(QString::fromUtf8("雷达"), nullptr),
                  QStringList{perf.name,
                              formatFreqRange(perf.freqMin, perf.freqMax),
                              formatPrfRange(perf.prfMin, perf.prfMax),
                              formatPwRange(perf.pwMin, perf.pwMax),
                              perf.scanMode},
                  row);
}

void RZSourceRadiation::displayData(const RadioSource &data, int row)
{
    // 按电台字段顺序写入模型
    writeModelRow(m_mapModel.value(QString::fromUtf8("电台"), nullptr),
                  QStringList{data.name,
                              data.frequency,
                              data.modulation,
                              data.codeRate,
                              data.powerOrWaveform,
                              data.threatLevel,
                              data.deviceType},
                  row);
}

void RZSourceRadiation::displayData(const RadarJammerSource &data, int row)
{
    // 按雷达干扰字段顺序写入模型
    writeModelRow(m_mapModel.value(QString::fromUtf8("雷达干扰"), nullptr),
                  QStringList{data.name,
                              data.jammingType,
                              data.workingBand,
                              data.technology,
                              data.threatLevel,
                              data.deviceType},
                  row);
}

void RZSourceRadiation::displayData(const RadioJammerSource &data, int row)
{
    // 按通信干扰字段顺序写入模型
    writeModelRow(m_mapModel.value(QString::fromUtf8("通信干扰"), nullptr),
                  QStringList{data.name,
                              data.jammingStyle,
                              data.coverageBand,
                              data.erp,
                              data.threatLevel,
                              data.deviceType},
                  row);
}

void RZSourceRadiation::writeModelRow(QStandardItemModel *model, const QStringList &columns, int row)
{
    // 模型未初始化时直接返回，避免空指针访问
    if (model == nullptr)
    {
        return;
    }

    // row < 0 表示追加；否则按指定行写入
    const int targetRow = (row < 0) ? model->rowCount() : row;
    if (targetRow >= model->rowCount())
    {
        model->setRowCount(targetRow + 1);
    }

    // 将每列内容写入对应单元格
    for (int col = 0; col < columns.size(); ++col)
    {
        model->setItem(targetRow, col, new QStandardItem(columns.at(col)));
    }
}

void RZSourceRadiation::onShowTableData()
{
    // 通过 sender() 获取当前点击按钮
    const auto *btn = qobject_cast<QPushButton *>(sender());
    if (btn == nullptr)
    {
        return;
    }

    // 根据按钮映射到模型 key
    QString modelKey;
    if (btn == ui->btnRadar)
    {
        modelKey = QString::fromUtf8("雷达");
    }
    /*else if (btn == ui->btnRadio)
    {
        modelKey = QString::fromUtf8("电台");
    }
    else if (btn == ui->btnRadarJam)
    {
        modelKey = QString::fromUtf8("雷达干扰");
    }
    else if (btn == ui->btnRadioJam)
    {
        modelKey = QString::fromUtf8("通信干扰");
    }*/
    else
    {
        return;
    }

    // 切换模型并执行一次内容自适应
    auto *model = m_mapModel.value(modelKey, nullptr);
    if (model == nullptr)
    {
        return;
    }

    ui->tableView->setModel(model);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
}

/**
 * @brief 添加雷达辐射源数据
 * @param data 雷达辐射源对象
 * @details 追加到雷达缓存并写入雷达模型。
 */
void RZSourceRadiation::addDataImpl(const RadarThreatAssessRecord &data)
{
    m_radarSources.append(data);
    displayData(data);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
    emit radarDataChanged();
}

/**
 * @brief 添加电台辐射源数据
 * @param data 电台辐射源对象
 * @details 追加到电台缓存并写入电台模型。
 */
void RZSourceRadiation::addDataImpl(const RadioSource &data)
{
    m_radioSource.append(data);
    displayData(data);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
}

/**
 * @brief 添加雷达干扰辐射源数据
 * @param data 雷达干扰辐射源对象
 * @details 追加到雷达干扰缓存并写入雷达干扰模型。
 */
void RZSourceRadiation::addDataImpl(const RadarJammerSource &data)
{
    m_radarJammerSource.append(data);
    displayData(data);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
}

/**
 * @brief 添加通信干扰辐射源数据
 * @param data 通信干扰辐射源对象
 * @details 追加到通信干扰缓存并写入通信干扰模型。
 */
void RZSourceRadiation::addDataImpl(const RadioJammerSource &data)
{
    m_RadioJammerSource.append(data);
    displayData(data);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
}

/**
 * @brief 更新雷达辐射源数据
 * @param data 雷达辐射源对象（按 name 匹配）
 * @details 若未找到同名记录则转为新增。
 */
void RZSourceRadiation::updateDataImpl(const RadarThreatAssessRecord &data)
{
    int row = -1;
    for (int i = 0; i < m_radarSources.size(); ++i)
    {
        if (m_radarSources.at(i).perfPara.name == data.perfPara.name)
        {
            row = i;
            break;
        }
    }
    if (row < 0)
    {
        addDataImpl(data);
        return;
    }
    m_radarSources[row] = data;
    displayData(data, row);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
    emit radarDataChanged();
}

/**
 * @brief 更新电台辐射源数据
 * @param data 电台辐射源对象（按 name 匹配）
 * @details 若未找到同名记录则转为新增。
* 这是一个更新表格数据的核心方法：根据名字，找到就更新，没找到就新增
 */
void RZSourceRadiation::updateDataImpl(const RadioSource &data)
{
    const int row = findIndexByName(m_radioSource, data.name);
    if (row < 0)
    {
        addDataImpl(data);
        return;
    }
    m_radioSource[row] = data;
    displayData(data, row);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
}

/**
 * @brief 更新雷达干扰辐射源数据
 * @param data 雷达干扰辐射源对象（按 name 匹配）
 * @details 若未找到同名记录则转为新增。
 */
void RZSourceRadiation::updateDataImpl(const RadarJammerSource &data)
{
    const int row = findIndexByName(m_radarJammerSource, data.name);
    if (row < 0)
    {
        addDataImpl(data);
        return;
    }
    m_radarJammerSource[row] = data;
    displayData(data, row);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
}

/**
 * @brief 更新通信干扰辐射源数据
 * @param data 通信干扰辐射源对象（按 name 匹配）
 * @details 若未找到同名记录则转为新增。
 */
void RZSourceRadiation::updateDataImpl(const RadioJammerSource &data)
{
    const int row = findIndexByName(m_RadioJammerSource, data.name);
    if (row < 0)
    {
        addDataImpl(data);
        return;
    }
    m_RadioJammerSource[row] = data;
    displayData(data, row);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
}

/**
 * @brief 删除雷达辐射源数据
 * @param name 目标名称
 * @details 从雷达缓存和雷达模型中同步删除同名记录。
 */
void RZSourceRadiation::deleteRadarDataByName(const QString &name)
{
    int row = -1;
    for (int i = 0; i < m_radarSources.size(); ++i)
    {
        if (m_radarSources.at(i).perfPara.name == name)
        {
            row = i;
            break;
        }
    }
    if (row < 0)
    {
        return;
    }
    m_radarSources.removeAt(row);
    if (auto *model = m_mapModel.value(QString::fromUtf8("雷达"), nullptr); model != nullptr)
    {
        model->removeRow(row);
    }
    emit radarDataChanged();
}

/**
 * @brief 删除电台辐射源数据
 * @param name 目标名称
 * @details 从电台缓存和电台模型中同步删除同名记录。
 */
void RZSourceRadiation::deleteRadioDataByName(const QString &name)
{
    const int row = findIndexByName(m_radioSource, name);
    if (row < 0)
    {
        return;
    }
    m_radioSource.removeAt(row);
    if (auto *model = m_mapModel.value(QString::fromUtf8("电台"), nullptr); model != nullptr)
    {
        model->removeRow(row);
    }
}

/**
 * @brief 删除雷达干扰辐射源数据
 * @param name 目标名称
 * @details 从雷达干扰缓存和雷达干扰模型中同步删除同名记录。
 */
void RZSourceRadiation::deleteRadarJammerDataByName(const QString &name)
{
    const int row = findIndexByName(m_radarJammerSource, name);
    if (row < 0)
    {
        return;
    }
    m_radarJammerSource.removeAt(row);
    if (auto *model = m_mapModel.value(QString::fromUtf8("雷达干扰"), nullptr); model != nullptr)
    {
        model->removeRow(row);
    }
}

/**
 * @brief 删除通信干扰辐射源数据
 * @param name 目标名称
 * @details 从通信干扰缓存和通信干扰模型中同步删除同名记录。
 */
void RZSourceRadiation::deleteRadioJammerDataByName(const QString &name)
{
    const int row = findIndexByName(m_RadioJammerSource, name);
    if (row < 0)
    {
        return;
    }
    m_RadioJammerSource.removeAt(row);
    if (auto *model = m_mapModel.value(QString::fromUtf8("通信干扰"), nullptr); model != nullptr)
    {
        model->removeRow(row);
    }
}
// 态势控制相关实现
void RZSourceRadiation::addControlDataImpl(const SituationControlData &data)
{
    m_controlData.append(data);
}

void RZSourceRadiation::updateControlDataImpl(const SituationControlData &data)
{
    const int row = findIndexByType(m_controlData, data.type);
    if (row < 0)
    {
        addControlDataImpl(data);
        return;
    }
    m_controlData[row] = data;
}

void RZSourceRadiation::deleteControlDataByType(const QString &type)
{
    const int row = findIndexByType(m_controlData, type);
    if (row < 0)
    {
        return;
    }
    m_controlData.removeAt(row);
}

void RZSourceRadiation::onRadarSwitchChanged(bool checked)
{
    emit controlStateChanged("radar", checked);
    updateControlDataImpl(SituationControlData("radar", checked, QString::fromUtf8("雷达")));
}

void RZSourceRadiation::onRadioSwitchChanged(bool checked)
{
    emit controlStateChanged("radio", checked);
    updateControlDataImpl(SituationControlData("radio", checked, QString::fromUtf8("电台")));
}

void RZSourceRadiation::onCommJamSwitchChanged(bool checked)
{
    emit controlStateChanged("commJam", checked);
    updateControlDataImpl(SituationControlData("commJam", checked, QString::fromUtf8("通信对抗")));
}

void RZSourceRadiation::onRadarJamSwitchChanged(bool checked)
{
    emit controlStateChanged("radarJam", checked);
    updateControlDataImpl(SituationControlData("radarJam", checked, QString::fromUtf8("雷达对抗")));
}

void RZSourceRadiation::onDefenseFireDisplaySwitchChanged(bool checked)
{
    emit controlStateChanged("defenseFire", checked);
    updateControlDataImpl(SituationControlData("defenseFire", checked, QString::fromUtf8("防控火力")));
}

const QVector<RadarThreatAssessRecord> &RZSourceRadiation::radarSources() const
{
    return m_radarSources;
}

void RZSourceRadiation::syncRadarSources(const QVector<RadarThreatAssessRecord> &sources)
{
    m_radarSources = sources;
    displayData();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
}
