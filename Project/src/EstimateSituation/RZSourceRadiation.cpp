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
    initTableViewAttr();

    // 初始化数据模型
    initDataModel();

    // 显示辐射源数据
    displayData();

    // 调整表格列宽
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();

    // 生成态势控制测试数据
    m_controlData.clear();
    m_controlData.append(SituationControlData("radar", ui->radarSwitch->isChecked(), QString::fromUtf8("雷达")));
    m_controlData.append(SituationControlData("radio", ui->radioSwitch->isChecked(), QString::fromUtf8("电台")));
    m_controlData.append(SituationControlData("commJam", ui->commJamSwitch->isChecked(), QString::fromUtf8("通信对抗")));
    m_controlData.append(SituationControlData("radarJam", ui->radarJamSwitch->isChecked(), QString::fromUtf8("雷达对抗")));
    m_controlData.append(SituationControlData("defenseFire", ui->defenseFireDisplaySwitch->isChecked(), QString::fromUtf8("防控火力")));
}

void RZSourceRadiation::initConnect()
{
    // 四个类型按钮复用同一槽函数，根据 sender() 判断切换模型
    connect(ui->btnRadar, &QPushButton::clicked, this, &RZSourceRadiation::onShowTableData);
    connect(ui->btnRadio, &QPushButton::clicked, this, &RZSourceRadiation::onShowTableData);
    connect(ui->btnRadioJam, &QPushButton::clicked, this, &RZSourceRadiation::onShowTableData);
    connect(ui->btnRadarJam, &QPushButton::clicked, this, &RZSourceRadiation::onShowTableData);

    // 雷达开关
    connect(ui->radarSwitch, &QCheckBox::toggled, this, &RZSourceRadiation::onRadarSwitchChanged);
    // 电台开关
    connect(ui->radioSwitch, &QCheckBox::toggled, this, &RZSourceRadiation::onRadioSwitchChanged);
    // 通信对抗开关
    connect(ui->commJamSwitch, &QCheckBox::toggled, this, &RZSourceRadiation::onCommJamSwitchChanged);
    // 雷达对抗开关
    connect(ui->radarJamSwitch, &QCheckBox::toggled, this, &RZSourceRadiation::onRadarJamSwitchChanged);
    // 防控火力显示开关
    connect(ui->defenseFireDisplaySwitch, &QCheckBox::toggled, this, &RZSourceRadiation::onDefenseFireDisplaySwitchChanged);
}

void RZSourceRadiation::generateTestData()
{
    // 重新生成测试数据前先清空容器，避免重复追加
    m_radarSource.clear();
    m_radioSource.clear();
    m_radarJammerSource.clear();
    m_RadioJammerSource.clear();

    // ---------- 1. 雷达数据 (Radar) ----------
    RadarSource radar1;
    radar1.name = QString::fromUtf8("AN/MPQ-53 相控阵雷达");
    radar1.frequency = QString::fromUtf8("5.2~6.1GHz");
    radar1.prf = QString::fromUtf8("200~500Hz");
    radar1.pulseWidth = QString::fromUtf8("0.5~25μs");
    radar1.scanMode = QString::fromUtf8("电子扫描");
    radar1.threatLevel = QString::fromUtf8("高");
    radar1.deviceType = QString::fromUtf8("高功率火控雷达");
    m_radarSource.append(radar1);

    RadarSource radar2;
    radar2.name = QString::fromUtf8("P-18 预警雷达");
    radar2.frequency = QString::fromUtf8("150~170MHz");
    radar2.prf = QString::fromUtf8("300Hz");
    radar2.pulseWidth = QString::fromUtf8("8μs");
    radar2.scanMode = QString::fromUtf8("6rpm");
    radar2.threatLevel = QString::fromUtf8("中");
    radar2.deviceType = QString::fromUtf8("米波远程警戒");
    m_radarSource.append(radar2);

    RadarSource radar3;
    radar3.name = QString::fromUtf8("MPQ-64 哨兵雷达");
    radar3.frequency = QString::fromUtf8("8~12GHz");
    radar3.prf = QString::fromUtf8("可变");
    radar3.pulseWidth = QString::fromUtf8("-");
    radar3.scanMode = QString::fromUtf8("旋转扫描");
    radar3.threatLevel = QString::fromUtf8("高");
    radar3.deviceType = QString::fromUtf8("低空补盲雷达");
    m_radarSource.append(radar3);

    // ---------- 2. 通信电台数据 (Communication) ----------
    RadioSource radio1;
    radio1.name = QString::fromUtf8("Link-16 战术数据链");
    radio1.frequency = QString::fromUtf8("960~1215MHz");
    radio1.modulation = QString::fromUtf8("MSK/扩频");
    radio1.codeRate = QString::fromUtf8("1.0Mbps");
    radio1.powerOrWaveform = QString::fromUtf8("JTIDS");
    radio1.threatLevel = QString::fromUtf8("高");
    radio1.deviceType = QString::fromUtf8("关键指控链路");
    m_radioSource.append(radio1);

    RadioSource radio2;
    radio2.name = QString::fromUtf8("VHF 战术电台");
    radio2.frequency = QString::fromUtf8("30~88MHz");
    radio2.modulation = QString::fromUtf8("FM/跳频");
    radio2.codeRate = QString::fromUtf8("-");
    radio2.powerOrWaveform = QString::fromUtf8("25W");
    radio2.threatLevel = QString::fromUtf8("中");
    radio2.deviceType = QString::fromUtf8("班组级通信");
    m_radioSource.append(radio2);

    RadioSource radio3;
    radio3.name = QString::fromUtf8("卫星通信终端");
    radio3.frequency = QString::fromUtf8("Ku波段");
    radio3.modulation = QString::fromUtf8("QPSK");
    radio3.codeRate = QString::fromUtf8("5Msps");
    radio3.powerOrWaveform = QString::fromUtf8("同步卫星");
    radio3.threatLevel = QString::fromUtf8("中");
    radio3.deviceType = QString::fromUtf8("远程指挥链路");
    m_radioSource.append(radio3);

    // ---------- 3. 雷达对抗设备 (Radar Jammer) ----------
    RadarJammerSource radarJammer1;
    radarJammer1.name = QString::fromUtf8("SPECTRAL 侦察干扰吊舱");
    radarJammer1.jammingType = QString::fromUtf8("距离门拖引/速度欺骗");
    radarJammer1.workingBand = QString::fromUtf8("2~18GHz");
    radarJammer1.technology = QString::fromUtf8("DRFM转发");
    radarJammer1.threatLevel = QString::fromUtf8("高");
    radarJammer1.deviceType = QString::fromUtf8("先进数字射频存储");
    m_radarJammerSource.append(radarJammer1);

    RadarJammerSource radarJammer2;
    radarJammer2.name = QString::fromUtf8("Pelena-1 地面干扰站");
    radarJammer2.jammingType = QString::fromUtf8("噪声压制/假目标");
    radarJammer2.workingBand = QString::fromUtf8("8~12GHz");
    radarJammer2.technology = QString::fromUtf8("模拟转发");
    radarJammer2.threatLevel = QString::fromUtf8("高");
    radarJammer2.deviceType = QString::fromUtf8("火控雷达对抗");
    m_radarJammerSource.append(radarJammer2);

    RadarJammerSource radarJammer3;
    radarJammer3.name = QString::fromUtf8("战术侦察/干扰模块");
    radarJammer3.jammingType = QString::fromUtf8("测频/测向+间歇采样");
    radarJammer3.workingBand = QString::fromUtf8("S/C波段");
    radarJammer3.technology = QString::fromUtf8("数字接收");
    radarJammer3.threatLevel = QString::fromUtf8("中");
    radarJammer3.deviceType = QString::fromUtf8("小型化电子攻击");
    m_radarJammerSource.append(radarJammer3);

    // ---------- 4. 通信对抗设备 (Comm Jammer) ----------
    RadioJammerSource radioJammer1;
    radioJammer1.name = QString::fromUtf8("R-330Zh 通信干扰系统");
    radioJammer1.jammingStyle = QString::fromUtf8("噪声调频/梳状谱");
    radioJammer1.coverageBand = QString::fromUtf8("20~100MHz");
    radioJammer1.erp = QString::fromUtf8("1kW");
    radioJammer1.threatLevel = QString::fromUtf8("高");
    radioJammer1.deviceType = QString::fromUtf8("大功率宽带压制");
    m_RadioJammerSource.append(radioJammer1);

    RadioJammerSource radioJammer2;
    radioJammer2.name = QString::fromUtf8("便携式通信干扰机");
    radioJammer2.jammingStyle = QString::fromUtf8("单音/扫频");
    radioJammer2.coverageBand = QString::fromUtf8("400~470MHz");
    radioJammer2.erp = QString::fromUtf8("50W");
    radioJammer2.threatLevel = QString::fromUtf8("低");
    radioJammer2.deviceType = QString::fromUtf8("近距离战术干扰");
    m_RadioJammerSource.append(radioJammer2);

    RadioJammerSource radioJammer3;
    radioJammer3.name = QString::fromUtf8("车载智能干扰站");
    radioJammer3.jammingStyle = QString::fromUtf8("协议伪造/随机脉冲");
    radioJammer3.coverageBand = QString::fromUtf8("225~400MHz");
    radioJammer3.erp = QString::fromUtf8("200W");
    radioJammer3.threatLevel = QString::fromUtf8("中");
    radioJammer3.deviceType = QString::fromUtf8("自适应干扰");
    m_RadioJammerSource.append(radioJammer3);
}

void RZSourceRadiation::initTableViewAttr()
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

void RZSourceRadiation::initDataModel()
{
    // 清理旧模型并重新创建，防止重复初始化造成内存泄漏
    qDeleteAll(m_mapModel);
    m_mapModel.clear();

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
        QString::fromUtf8("威胁等级"),
        QString::fromUtf8("设备类型")
    });
    m_mapModel.insert(QString::fromUtf8("雷达"), radarModel);

    // 2) 电台
    auto *radioModel = createModel(QStringList{
        QString::fromUtf8("名称"),
        QString::fromUtf8("频率"),
        QString::fromUtf8("调制方式"),
        QString::fromUtf8("码速率"),
        QString::fromUtf8("功率/波形"),
        QString::fromUtf8("威胁等级"),
        QString::fromUtf8("设备类型")
    });
    m_mapModel.insert(QString::fromUtf8("电台"), radioModel);

    // 3) 雷达干扰
    auto *radarJamModel = createModel(QStringList{
        QString::fromUtf8("名称"),
        QString::fromUtf8("干扰类型"),
        QString::fromUtf8("工作频段"),
        QString::fromUtf8("技术体制"),
        QString::fromUtf8("威胁等级"),
        QString::fromUtf8("设备类型")
    });
    m_mapModel.insert(QString::fromUtf8("雷达干扰"), radarJamModel);

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

    // 默认显示"雷达"模型（此时仅有表头）
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
    for (const auto &item : m_radarSource)
    {
        displayData(item);
    }
    for (const auto &item : m_radioSource)
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
    }
}

void RZSourceRadiation::displayData(const RadarSource &data, int row)
{
    // 按雷达字段顺序写入模型
    writeModelRow(m_mapModel.value(QString::fromUtf8("雷达"), nullptr),
                  QStringList{data.name,
                              data.frequency,
                              data.prf,
                              data.pulseWidth,
                              data.scanMode,
                              data.threatLevel,
                              data.deviceType},
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
    else if (btn == ui->btnRadio)
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
    }
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
void RZSourceRadiation::addDataImpl(const RadarSource &data)
{
    m_radarSource.append(data);
    displayData(data);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
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
void RZSourceRadiation::updateDataImpl(const RadarSource &data)
{
    const int row = findIndexByName(m_radarSource, data.name);
    if (row < 0)
    {
        addDataImpl(data);
        return;
    }
    m_radarSource[row] = data;
    displayData(data, row);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
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
    const int row = findIndexByName(m_radarSource, name);
    if (row < 0)
    {
        return;
    }
    m_radarSource.removeAt(row);
    if (auto *model = m_mapModel.value(QString::fromUtf8("雷达"), nullptr); model != nullptr)
    {
        model->removeRow(row);
    }
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
