#include "ThreatAssessment.h"
#include "ui_ThreatAssessment.h"
#include <QHeaderView>
#include <QAbstractItemView>
#include <QBrush>
#include <QColor>
#include <QFont>
#include <Qt>
#include <algorithm>

namespace
{
    /**
     * @brief 根据名称查找索引
     * @tparam T 数据类型
     * @param container 数据容器
     * @param name 目标名称
     * @return 找到的索引，未找到返回 -1
     */
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
    /**
     * @brief 获取威胁等级的优先级值
     * @param threatLevel 威胁等级字符串
     * @return 优先级值，数字越大优先级越高
     */
    int getThreatLevelPriority(const QString &threatLevel)
    {
        if (threatLevel == "高") return 3;
        if (threatLevel == "中") return 2;
        if (threatLevel == "低") return 1;
        return 0;
    }
    /**
     * @brief 通用排序函数
     * @tparam T 数据类型
     * @param a 第一个元素
     * @param b 第二个元素
     * @return a是否应该排在b前面
     */
    template <typename T>
    bool sortByThreatLevel(const T &a, const T &b)
    {
        return getThreatLevelPriority(a.threatLevel) > getThreatLevelPriority(b.threatLevel);
    }

}

/**
 * @brief 构造函数
 * @param parent 父窗口指针
 */
ThreatAssessment::ThreatAssessment(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ThreatAssessment)
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

    // 初始化表格属性
    initTableViewAttr();

    // 初始化数据模型
    initDataModel();

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
    // 无需按钮连接，仅添加数据
}

/**
 * @brief 生成测试数据
 * @details 生成威胁评估测试数据
 */
void ThreatAssessment::generateTestData()
{
    // 清空数据列表
    m_radarSources.clear();
    m_radioSources.clear();
    m_radioJammerSources.clear();
    m_radarJammerSources.clear();

    // ---------- 1. 雷达数据 (Radar) ----------
    RadarSource radar1;
    radar1.name = QString::fromUtf8("AN/MPQ-53 相控阵雷达");
    radar1.threatLevel = QString::fromUtf8("高");
    m_radarSources.append(radar1);

    RadarSource radar2;
    radar2.name = QString::fromUtf8("P-18 预警雷达");
    radar2.threatLevel = QString::fromUtf8("中");
    m_radarSources.append(radar2);

    RadarSource radar3;
    radar3.name = QString::fromUtf8("MPQ-64 哨兵雷达");
    radar3.threatLevel = QString::fromUtf8("高");
    m_radarSources.append(radar3);

    // ---------- 2. 通信电台数据 (Communication) ----------
    RadioSource radio1;
    radio1.name = QString::fromUtf8("Link-16 战术数据链");
    radio1.threatLevel = QString::fromUtf8("高");
    m_radioSources.append(radio1);

    RadioSource radio2;
    radio2.name = QString::fromUtf8("VHF 战术电台");
    radio2.threatLevel = QString::fromUtf8("中");
    m_radioSources.append(radio2);

    RadioSource radio3;
    radio3.name = QString::fromUtf8("卫星通信终端");
    radio3.threatLevel = QString::fromUtf8("中");
    m_radioSources.append(radio3);

    // ---------- 3. 雷达对抗设备 (Radar Jammer) ----------
    RadarJammerSource radarJammer1;
    radarJammer1.name = QString::fromUtf8("SPECTRAL 侦察干扰吊舱");
    radarJammer1.threatLevel = QString::fromUtf8("高");
    m_radarJammerSources.append(radarJammer1);

    RadarJammerSource radarJammer2;
    radarJammer2.name = QString::fromUtf8("Pelena-1 地面干扰站");
    radarJammer2.threatLevel = QString::fromUtf8("高");
    m_radarJammerSources.append(radarJammer2);

    RadarJammerSource radarJammer3;
    radarJammer3.name = QString::fromUtf8("战术侦察/干扰模块");
    radarJammer3.threatLevel = QString::fromUtf8("中");
    m_radarJammerSources.append(radarJammer3);

    // ---------- 4. 通信对抗设备 (Comm Jammer) ----------
    RadioJammerSource radioJammer1;
    radioJammer1.name = QString::fromUtf8("R-330Zh 通信干扰系统");
    radioJammer1.threatLevel = QString::fromUtf8("高");
    m_radioJammerSources.append(radioJammer1);

    RadioJammerSource radioJammer2;
    radioJammer2.name = QString::fromUtf8("便携式通信干扰机");
    radioJammer2.threatLevel = QString::fromUtf8("低");
    m_radioJammerSources.append(radioJammer2);

    RadioJammerSource radioJammer3;
    radioJammer3.name = QString::fromUtf8("车载智能干扰站");
    radioJammer3.threatLevel = QString::fromUtf8("中");
    m_radioJammerSources.append(radioJammer3);

    // 排序数据
    sortData();
}

/**
 * @brief 初始化表格属性
 * @details 设置表格属性，如列数、列宽、行高、表头、数据样式等
 */
void ThreatAssessment::initTableViewAttr()
{
    // 交互行为
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);               // 不可编辑
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);             // 单行选中
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);              // 全行选中
    ui->tableView->setSortingEnabled(false);                                         // 关闭自动排序

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
    ui->tableView->horizontalHeader()->setVisible(false);
    // 行高按内容自适应
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    // 显示网格线
    ui->tableView->setShowGrid(true);
}

/**
 * @brief 初始化数据模型
 * @details 创建并配置数据模型
 */
void ThreatAssessment::initDataModel()
{
    // 清理旧模型并重新创建，防止重复初始化造成内存泄漏
    qDeleteAll(m_mapModel);
    m_mapModel.clear();

    // 创建单个模型用于显示所有数据
    auto *model = new QStandardItemModel(this);
    model->setColumnCount(2);

    // 将模型添加到映射中并设置为当前显示模型
    m_mapModel.insert(QString::fromUtf8("all"), model);
    ui->tableView->setModel(model);
}

/**
 * @brief 显示数据
 * @details 将威胁数据显示到表格中，按威胁等级从高到低排序
 */
void ThreatAssessment::displayData()
{
    // 清空表格
    auto *model = m_mapModel.value(QString::fromUtf8("all"), nullptr);
    if (model != nullptr)
    {
        model->removeRows(0, model->rowCount());
    }
    // 从第一行开始显式写入数据
    int row = 0;
    // 按统一排序后的数据显示所有测试数据
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
        QStandardItem *item = new QStandardItem(columns.at(col));
        // 第二列（威胁等级）右对齐
        if (col == 1)
        {
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
    // 清空统一数据列表
    m_unifiedThreatData.clear();

    // 将雷达数据添加到统一列表
    for (const auto &item : m_radarSources)
    {
        int priority = getThreatLevelPriority(item.threatLevel);
        m_unifiedThreatData.append(UnifiedThreatItem(item.name, item.threatLevel, QString::fromUtf8("雷达"), priority));
    }

    // 将通信电台数据添加到统一列表
    for (const auto &item : m_radioSources)
    {
        int priority = getThreatLevelPriority(item.threatLevel);
        m_unifiedThreatData.append(UnifiedThreatItem(item.name, item.threatLevel, QString::fromUtf8("电台"), priority));
    }

    // 将雷达对抗设备数据添加到统一列表
    for (const auto &item : m_radarJammerSources)
    {
        int priority = getThreatLevelPriority(item.threatLevel);
        m_unifiedThreatData.append(UnifiedThreatItem(item.name, item.threatLevel, QString::fromUtf8("雷达对抗"), priority));
    }

    // 将通信对抗设备数据添加到统一列表
    for (const auto &item : m_radioJammerSources)
    {
        int priority = getThreatLevelPriority(item.threatLevel);
        m_unifiedThreatData.append(UnifiedThreatItem(item.name, item.threatLevel, QString::fromUtf8("通信对抗"), priority));
    }

    // 按威胁等级优先级从高到低排序
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
    m_radarSources.append(data);
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
    m_radioSources.append(data);
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
    m_radarJammerSources.append(data);
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
    m_radioJammerSources.append(data);
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
    const int row = ::findIndexByName(m_radarSources, data.name);
    if (row < 0) {
        addDataImpl(data);
        return;
    }
    m_radarSources[row] = data;
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
    const int row = ::findIndexByName(m_radioSources, data.name);
    if (row < 0) {
        addDataImpl(data);
        return;
    }
    m_radioSources[row] = data;
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
    const int row = ::findIndexByName(m_radarJammerSources, data.name);
    if (row < 0) {
        addDataImpl(data);
        return;
    }
    m_radarJammerSources[row] = data;
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
    const int row = ::findIndexByName(m_radioJammerSources, data.name);
    if (row < 0) {
        addDataImpl(data);
        return;
    }
    m_radioJammerSources[row] = data;
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
    const int row = ::findIndexByName(m_radarSources, name);
    if (row >= 0) {
        m_radarSources.remove(row);
        sortData();
        displayData();
        ui->tableView->resizeColumnsToContents();
        ui->tableView->resizeRowsToContents();
    }
}

/**
 * @brief 删除电台威胁数据
 * @param name 目标名称
 * @details 从电台缓存和模型中同步删除同名记录。
 */
void ThreatAssessment::deleteRadioDataByName(const QString &name)
{
    const int row = ::findIndexByName(m_radioSources, name);
    if (row >= 0) {
        m_radioSources.remove(row);
        sortData();
        displayData();
        ui->tableView->resizeColumnsToContents();
        ui->tableView->resizeRowsToContents();
    }
}

/**
 * @brief 删除雷达干扰威胁数据
 * @param name 目标名称
 * @details 从雷达干扰缓存和模型中同步删除同名记录。
 */
void ThreatAssessment::deleteRadarJammerDataByName(const QString &name)
{
    const int row = ::findIndexByName(m_radarJammerSources, name);
    if (row >= 0) {
        m_radarJammerSources.remove(row);
        sortData();
        displayData();
        ui->tableView->resizeColumnsToContents();
        ui->tableView->resizeRowsToContents();
    }
}

/**
 * @brief 删除通信干扰威胁数据
 * @param name 目标名称
 * @details 从通信干扰缓存和模型中同步删除同名记录。
 */
void ThreatAssessment::deleteRadioJammerDataByName(const QString &name)
{
    const int row = ::findIndexByName(m_radioJammerSources, name);
    if (row >= 0) {
        m_radioJammerSources.remove(row);
        sortData();
        displayData();
        ui->tableView->resizeColumnsToContents();
        ui->tableView->resizeRowsToContents();
    }
}