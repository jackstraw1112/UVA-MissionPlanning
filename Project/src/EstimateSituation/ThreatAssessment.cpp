//
// Created by admin on "2026.04.09 T 19:19:20".
//

#include "ThreatAssessment.h"
#include "ui_ThreatAssessment.h"
#include <QHeaderView>
#include <QAbstractItemView>
#include <QStyledItemDelegate>
#include <QFont>
#include <algorithm>
namespace
{
    class UpperCenterTextDelegate final : public QStyledItemDelegate
    {
    public:
        explicit UpperCenterTextDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent)
        {
        }

        void initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const override
        {
            QStyledItemDelegate::initStyleOption(option, index);
            // 移除居中对齐，由 writeModelRow 方法单独设置每列对齐方式
        }

        QString displayText(const QVariant &value, const QLocale &locale) const override
        {
            return QStyledItemDelegate::displayText(value, locale).toUpper();
        }
    };

    int getThreatLevelPriority(const QString &threatLevel)
    {
        if (threatLevel == QString::fromUtf8("高")) return 3;
        if (threatLevel == QString::fromUtf8("中")) return 2;
        if (threatLevel == QString::fromUtf8("低")) return 1;
        return 0;
    }
}

/**
 * @brief 构造函数
 * @param parent 父窗口指针
 */
ThreatAssessment::ThreatAssessment(QWidget *parent)
    : QWidget(parent), ui(new Ui::ThreatAssessment)
{
    ui->setupUi(this);

    // 初始化参数
    initParams();

    // 初始化对象
    initObject();

    // 关联信号与槽函数
    initConnect();
}

/**
 * @brief 析构函数
 */
ThreatAssessment::~ThreatAssessment()
{
    qDeleteAll(m_mapModel);
    m_mapModel.clear();
    delete ui;
}

/**
 * @brief 初始化参数
 * @details 预留扩展
 */
void ThreatAssessment::initParams()
{
}

/**
 * @brief 初始化对象
 * @details 数据、视图、模型
 */
void ThreatAssessment::initObject()
{
    // 生成测试数据
    generateTestData();

    // 初始化数据模型
    initDataModel();

    // 初始化表格属性
    initTableViewAttr();

    // 显示数据
    displayData();

    // 调整表格列宽和行高
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
}

/**
 * @brief 关联信号与槽函数
 */
void ThreatAssessment::initConnect()
{
}

/**
 * @brief 生成测试数据
 * @details 生成威胁评估测试数据
 */
void ThreatAssessment::generateTestData()
{
    // 清空数据列表
    m_radarSource.clear();
    m_radioSource.clear();
    m_radioJammerSource.clear();
    m_radarJammerSource.clear();

    // ---------- 1. 雷达数据 (Radar) ----------
    RadarSource radar1;
    radar1.name = QString::fromUtf8("AN/MPQ-53 相控阵雷达");
    radar1.threatLevel = QString::fromUtf8("高");
    m_radarSource.append(radar1);

    RadarSource radar2;
    radar2.name = QString::fromUtf8("P-18 预警雷达");
    radar2.threatLevel = QString::fromUtf8("中");
    m_radarSource.append(radar2);

    RadarSource radar3;
    radar3.name = QString::fromUtf8("MPQ-64 哨兵雷达");
    radar3.threatLevel = QString::fromUtf8("高");
    m_radarSource.append(radar3);

    // ---------- 2. 通信电台数据 (Communication) ----------
    RadioSource radio1;
    radio1.name = QString::fromUtf8("Link-16 战术数据链");
    radio1.threatLevel = QString::fromUtf8("高");
    m_radioSource.append(radio1);

    RadioSource radio2;
    radio2.name = QString::fromUtf8("VHF 战术电台");
    radio2.threatLevel = QString::fromUtf8("中");
    m_radioSource.append(radio2);

    RadioSource radio3;
    radio3.name = QString::fromUtf8("卫星通信终端");
    radio3.threatLevel = QString::fromUtf8("中");
    m_radioSource.append(radio3);

    // ---------- 3. 雷达对抗设备 (Radar Jammer) ----------
    RadarJammerSource radarJammer1;
    radarJammer1.name = QString::fromUtf8("SPECTRAL 侦察干扰吊舱");
    radarJammer1.threatLevel = QString::fromUtf8("高");
    m_radarJammerSource.append(radarJammer1);

    RadarJammerSource radarJammer2;
    radarJammer2.name = QString::fromUtf8("Pelena-1 地面干扰站");
    radarJammer2.threatLevel = QString::fromUtf8("高");
    m_radarJammerSource.append(radarJammer2);

    RadarJammerSource radarJammer3;
    radarJammer3.name = QString::fromUtf8("战术侦察/干扰模块");
    radarJammer3.threatLevel = QString::fromUtf8("中");
    m_radarJammerSource.append(radarJammer3);

    // ---------- 4. 通信对抗设备 (Comm Jammer) ----------
    RadioJammerSource radioJammer1;
    radioJammer1.name = QString::fromUtf8("R-330Zh 通信干扰系统");
    radioJammer1.threatLevel = QString::fromUtf8("高");
    m_radioJammerSource.append(radioJammer1);

    RadioJammerSource radioJammer2;
    radioJammer2.name = QString::fromUtf8("便携式通信干扰机");
    radioJammer2.threatLevel = QString::fromUtf8("低");
    m_radioJammerSource.append(radioJammer2);

    RadioJammerSource radioJammer3;
    radioJammer3.name = QString::fromUtf8("车载智能干扰站");
    radioJammer3.threatLevel = QString::fromUtf8("中");
    m_radioJammerSource.append(radioJammer3);

    // 排序数据
    sortData();
}

/**
 * @brief 初始化表格属性
 * @details 设置表格属性，如列数、列宽、行高、表头、数据样式等
 */
void ThreatAssessment::initTableViewAttr()
{
    if (!ui->tableView) {
        return;
    }

    // 交互行为
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);               // 不可编辑
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);             // 单行选中
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);              // 全行选中
    ui->tableView->setItemDelegate(new UpperCenterTextDelegate(ui->tableView));      // 内容处理
    ui->tableView->setAlternatingRowColors(true);                                    // 奇偶行显示
    ui->tableView->setStyleSheet("QTableView{alternate-background-color:#F5F7FA;}"); // 间隔颜色
    ui->tableView->setSortingEnabled(false);                                         // 关闭自动排序

    // 字体设置
    QFont cellFont = ui->tableView->font();
    cellFont.setPointSize(10);
    ui->tableView->setFont(cellFont);                                                // 表格内容字体大小

    // 表头设置
    QHeaderView *horizontalHeader = ui->tableView->horizontalHeader();
    if (horizontalHeader) {
        horizontalHeader->setDefaultAlignment(Qt::AlignCenter);                     // 表头居中
        
        QFont headerFont = horizontalHeader->font();
        headerFont.setPointSize(11);
        horizontalHeader->setFont(headerFont);                                      // 表头字体大小
        
        // 列宽策略：两列等宽
        horizontalHeader->setSectionResizeMode(0, QHeaderView::Stretch);
        horizontalHeader->setSectionResizeMode(1, QHeaderView::Stretch);
    }

    // 垂直表头设置
    QHeaderView *verticalHeader = ui->tableView->verticalHeader();
    if (verticalHeader) {
        QFont indexFont = verticalHeader->font();
        indexFont.setPointSize(11);
        verticalHeader->setFont(indexFont);                                         // 序号列字体大小
        
        // 行高按内容自适应
        verticalHeader->setSectionResizeMode(QHeaderView::ResizeToContents);
    }

    ui->tableView->setShowGrid(true);
}

/**
 * @brief 初始化数据模型
 * @details 创建并配置数据模型
 */
void ThreatAssessment::initDataModel()
{
    qDeleteAll(m_mapModel);
    m_mapModel.clear();

    auto *model = new QStandardItemModel(this);
    model->setColumnCount(2);
    model->setHorizontalHeaderLabels(QStringList{
        QString::fromUtf8("目标名称"),
        QString::fromUtf8("威胁等级")
    });

    m_mapModel.insert(QString::fromUtf8("all"), model);
    ui->tableView->setModel(model);
}

/**
 * @brief 显示数据
 * @details 将威胁数据显示到表格中，按威胁等级从高到低排序
 */
void ThreatAssessment::displayData()
{
    auto *model = m_mapModel.value(QString::fromUtf8("all"), nullptr);
    if (model != nullptr)
    {
        model->removeRows(0, model->rowCount());
    }

    int row = 0;
    for (const auto &item : m_unifiedThreatData)
    {
        writeModelRow(model, QStringList{item.name, item.threatLevel}, row++);
    }
}

/**
 * @brief 写入模型行
 */
void ThreatAssessment::writeModelRow(QStandardItemModel *model, const QStringList &columns, int row)
{
    if (model == nullptr)
    {
        return;
    }

    const int targetRow = (row < 0) ? model->rowCount() : row;
    if (targetRow >= model->rowCount())
    {
        model->setRowCount(targetRow + 1);
    }

    for (int col = 0; col < columns.size(); ++col)
    {
        QStandardItem *item = new QStandardItem(columns.at(col));
        if (col == 0)
        {
            // 第一列左对齐
            item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        }
        else if (col == 1)
        {
            // 第二列右对齐
            item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        }
        model->setItem(targetRow, col, item);
    }
}

/**
 * @brief 排序数据
 * @details 根据威胁等级排序所有数据，不区分数据类型
 */
void ThreatAssessment::sortData()
{
    m_unifiedThreatData.clear();

    for (const auto &item : m_radarSource)
    {
        int priority = getThreatLevelPriority(item.threatLevel);
        m_unifiedThreatData.append(UnifiedThreatItem(item.name, item.threatLevel, QString::fromUtf8("雷达"), priority));
    }

    for (const auto &item : m_radioSource)
    {
        int priority = getThreatLevelPriority(item.threatLevel);
        m_unifiedThreatData.append(UnifiedThreatItem(item.name, item.threatLevel, QString::fromUtf8("电台"), priority));
    }

    for (const auto &item : m_radarJammerSource)
    {
        int priority = getThreatLevelPriority(item.threatLevel);
        m_unifiedThreatData.append(UnifiedThreatItem(item.name, item.threatLevel, QString::fromUtf8("雷达对抗"), priority));
    }

    for (const auto &item : m_radioJammerSource)
    {
        int priority = getThreatLevelPriority(item.threatLevel);
        m_unifiedThreatData.append(UnifiedThreatItem(item.name, item.threatLevel, QString::fromUtf8("通信对抗"), priority));
    }

    std::sort(m_unifiedThreatData.begin(), m_unifiedThreatData.end(),
        [](const UnifiedThreatItem &a, const UnifiedThreatItem &b) {
            return a.priority > b.priority;
        });
}

/**
 * @brief 添加雷达威胁数据
 * @param data 雷达威胁对象
 * @details 追加到雷达缓存并更新显示。
 */
void ThreatAssessment::addDataImpl(const RadarSource &data)
{
    m_radarSource.append(data);
    sortData();
    displayData();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
}

/**
 * @brief 添加电台威胁数据
 * @param data 电台威胁对象
 * @details 追加到电台缓存并更新显示。
 */
void ThreatAssessment::addDataImpl(const RadioSource &data)
{
    m_radioSource.append(data);
    sortData();
    displayData();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
}

/**
 * @brief 添加雷达干扰威胁数据
 * @param data 雷达干扰威胁对象
 * @details 追加到雷达干扰缓存并更新显示。
 */
void ThreatAssessment::addDataImpl(const RadarJammerSource &data)
{
    m_radarJammerSource.append(data);
    sortData();
    displayData();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
}

/**
 * @brief 添加通信干扰威胁数据
 * @param data 通信干扰威胁对象
 * @details 追加到通信干扰缓存并更新显示。
 */
void ThreatAssessment::addDataImpl(const RadioJammerSource &data)
{
    m_radioJammerSource.append(data);
    sortData();
    displayData();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
}

/**
 * @brief 更新雷达威胁数据
 * @param data 雷达威胁对象（按 name 匹配）
 * @details 若未找到同名记录则转为新增。
 */
void ThreatAssessment::updateDataImpl(const RadarSource &data)
{
    const int row = findIndexByName(m_radarSource, data.name);
    if (row < 0)
    {
        addDataImpl(data);
        return;
    }
    m_radarSource[row] = data;
    sortData();
    displayData();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
}

/**
 * @brief 更新电台威胁数据
 * @param data 电台威胁对象（按 name 匹配）
 * @details 若未找到同名记录则转为新增。
 */
void ThreatAssessment::updateDataImpl(const RadioSource &data)
{
    const int row = findIndexByName(m_radioSource, data.name);
    if (row < 0)
    {
        addDataImpl(data);
        return;
    }
    m_radioSource[row] = data;
    sortData();
    displayData();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
}

/**
 * @brief 更新雷达干扰威胁数据
 * @param data 雷达干扰威胁对象（按 name 匹配）
 * @details 若未找到同名记录则转为新增。
 */
void ThreatAssessment::updateDataImpl(const RadarJammerSource &data)
{
    const int row = findIndexByName(m_radarJammerSource, data.name);
    if (row < 0)
    {
        addDataImpl(data);
        return;
    }
    m_radarJammerSource[row] = data;
    sortData();
    displayData();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
}

/**
 * @brief 更新通信干扰威胁数据
 * @param data 通信干扰威胁对象（按 name 匹配）
 * @details 若未找到同名记录则转为新增。
 */
void ThreatAssessment::updateDataImpl(const RadioJammerSource &data)
{
    const int row = findIndexByName(m_radioJammerSource, data.name);
    if (row < 0)
    {
        addDataImpl(data);
        return;
    }
    m_radioJammerSource[row] = data;
    sortData();
    displayData();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
}

/**
 * @brief 删除雷达威胁数据
 * @param name 目标名称
 * @details 从雷达缓存和模型中同步删除同名记录。
 */
void ThreatAssessment::deleteRadarDataByName(const QString &name)
{
    const int row = findIndexByName(m_radarSource, name);
    if (row < 0)
    {
        return;
    }
    m_radarSource.removeAt(row);
    sortData();
    displayData();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
}

/**
 * @brief 删除电台威胁数据
 * @param name 目标名称
 * @details 从电台缓存和模型中同步删除同名记录。
 */
void ThreatAssessment::deleteRadioDataByName(const QString &name)
{
    const int row = findIndexByName(m_radioSource, name);
    if (row < 0)
    {
        return;
    }
    m_radioSource.removeAt(row);
    sortData();
    displayData();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
}

/**
 * @brief 删除雷达干扰威胁数据
 * @param name 目标名称
 * @details 从雷达干扰缓存和模型中同步删除同名记录。
 */
void ThreatAssessment::deleteRadarJammerDataByName(const QString &name)
{
    const int row = findIndexByName(m_radarJammerSource, name);
    if (row < 0)
    {
        return;
    }
    m_radarJammerSource.removeAt(row);
    sortData();
    displayData();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
}

/**
 * @brief 删除通信干扰威胁数据
 * @param name 目标名称
 * @details 从通信干扰缓存和模型中同步删除同名记录。
 */
void ThreatAssessment::deleteRadioJammerDataByName(const QString &name)
{
    const int row = findIndexByName(m_radioJammerSource, name);
    if (row < 0)
    {
        return;
    }
    m_radioJammerSource.removeAt(row);
    sortData();
    displayData();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
}
