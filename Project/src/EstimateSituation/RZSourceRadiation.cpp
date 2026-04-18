//
// Created by admin on "2026.04.09 T 19:19:20".
//

// You may need to build the project (run Qt uic code generator) to get "ui_RZSourceRadiation.h" resolved

#include "RZSourceRadiation.h"
#include "ui_RZSourceRadiation.h"
#include "EstimateSituationStruct.h"
#include <QAbstractItemView>
#include <QHeaderView>
#include <QPushButton>
#include <QStyledItemDelegate>
#include <QMenu>
#include <QInputDialog>
#include <QMessageBox>



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

/**
 * @brief 雷达辐射源窗口构造函数
 * @param parent 父窗口指针
 * @details 初始化UI、参数、对象和信号槽连接
 */
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

/**
 * @brief 雷达辐射源窗口析构函数
 * @details 释放UI资源
 */
RZSourceRadiation::~RZSourceRadiation()
{
    delete ui;
}

/**
 * @brief 初始化参数
 * @details 预留扩展
 */
void RZSourceRadiation::initParams()
{
}

/**
 * @brief 初始化对象
 * @details 生成测试数据、初始化表格属性和数据模型
 */
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

/**
 * @brief 关联信号与槽函数
 * @details 关联按钮点击、表格右键菜单等信号槽
 */
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
    
    // 表格右键菜单
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView, &QTableView::customContextMenuRequested, this, [this](const QPoint &pos) {
        QMenu menu(this);
        QAction *addAction = menu.addAction("添加雷达");
        QAction *editAction = menu.addAction("编辑雷达");
        QAction *deleteAction = menu.addAction("删除雷达");
        
        QModelIndex index = ui->tableView->indexAt(pos);
        QModelIndexList selectedIndexes = ui->tableView->selectionModel()->selectedRows();
        
        // 编辑操作只在选中单行时可用
        editAction->setEnabled(index.isValid() && selectedIndexes.size() == 1);
        // 删除操作在选中至少一行时可用
        deleteAction->setEnabled(!selectedIndexes.isEmpty());
        
        QAction *selectedAction = menu.exec(ui->tableView->viewport()->mapToGlobal(pos));
        if (selectedAction == addAction) {
            onAddRadar();
        } else if (selectedAction == editAction && index.isValid()) {
            onEditRadar(index.row());
        } else if (selectedAction == deleteAction && !selectedIndexes.isEmpty()) {
            // 处理多选删除
            if (selectedIndexes.size() == 1) {
                onDeleteRadar(selectedIndexes.first().row());
            } else {
                // 多选删除确认
                if (QMessageBox::question(this, "删除雷达", QString("确定要删除选中的 %1 个雷达吗？").arg(selectedIndexes.size())) == QMessageBox::Yes) {
                    // 按行号降序删除，避免索引混乱
                    QList<int> rows;
                    for (const QModelIndex &idx : selectedIndexes) {
                        rows.append(idx.row());
                    }
                    std::sort(rows.begin(), rows.end(), std::greater<int>());
                    
                    for (int row : rows) {
                        QString name = m_radarSource[row].name;
                        m_radarSource.removeAt(row);
                        if (auto *model = m_mapModel.value(QString::fromUtf8("雷达"), nullptr); model != nullptr) {
                            model->removeRow(row);
                        }
                        emit radarDataDeleted(name);
                    }
                }
            }
        }
    });
    

}

/**
 * @brief 生成测试数据
 * @details 初始化雷达辐射源测试数据
 */
void RZSourceRadiation::generateTestData()
{
    // 重新生成测试数据前先清空容器，避免重复追加
    m_radarSource.clear();


    // ---------- 1. 雷达数据 (Radar) ----------
    // 与 RZThreatAssess 统一使用相同的预设数据
    const auto addRadar = [this](const QString &id, const QString &name, const QString &type, int presetIndex, const QString &scanMode)
    {
        RadarPerformancePara radar;
        radar = ProjectPublicInterface::radarInputFromPresetIndex(presetIndex);
        radar.name = name;
        radar.deviceType = type;
        radar.scanMode = scanMode;
        radar.equipID = id;
        // radar.entityName = name;
        // radar.typeName = type;
    radar.entityName = id;
    radar.typeName = type;
        m_radarSource.append(radar);
    };

    // 雷达辐射源（与 RZThreatAssess 中的预设一致）
    addRadar(QStringLiteral("RAD-001"), QStringLiteral("AN/MPQ-53 相控阵雷达"), QStringLiteral("MPQ-53 (PAC-2 火控)"), 2, QStringLiteral("电子扫描"));
    addRadar(QStringLiteral("RAD-002"), QStringLiteral("爱国者 MPQ-65"), QStringLiteral("MPQ-65 (PAC-3 火控)"), 1, QStringLiteral("相控阵扫描"));
    addRadar(QStringLiteral("RAD-003"), QStringLiteral("P-18 预警雷达"), QStringLiteral("SPS-48E"), 5, QStringLiteral("6rpm"));
    addRadar(QStringLiteral("RAD-004"), QStringLiteral("MPQ-64 哨兵雷达"), QStringLiteral("TPS-75"), 7, QStringLiteral("旋转扫描"));
    addRadar(QStringLiteral("RAD-005"), QStringLiteral("远程预警 FPS-117"), QStringLiteral("FPS-117"), 6, QStringLiteral("机械扫描"));
}

/**
 * @brief 初始化表格属性
 * @details 设置表格交互行为、字体、列宽策略等
 */
void RZSourceRadiation::initTableAttr()
{
    // 交互行为
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);               // 不可编辑
    ui->tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);           // 多行选中，支持Ctrl键多选
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


    ui->btnRadio->setVisible(false); // 电台
    ui->btnCommJam->setVisible(false); // 通信对抗
    ui->btnRadarJam->setVisible(false); // 雷达对抗
}

/**
 * @brief 初始化数据模型
 * @details 创建雷达表格模型并设置表头
 */
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


    ui->tableView->setModel(radarModel);
}

/**
 * @brief 显示所有辐射源数据
 * @details 清空并重新显示所有雷达数据
 */
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

}

/**
 * @brief 显示单条辐射源数据
 * @param data 雷达性能参数
 * @param row 行号，-1表示追加
 * @details 将雷达数据写入表格指定行
 */
void RZSourceRadiation::displayData(const RadarPerformancePara &data, int row)
{
    // 按雷达字段顺序写入模型
    writeModelRow(m_mapModel.value(QString::fromUtf8("雷达"), nullptr),
                  QStringList{data.name,
                              QString("%1~%2GHz").arg(data.freqMin).arg(data.freqMax),
                              QString("%1~%2Hz").arg(data.prfMin).arg(data.prfMax),
                              QString("%1~%2μs").arg(data.pwMin).arg(data.pwMax),
                              data.scanMode,
                              /*data.threatLevel,*/
                              /*data.deviceType*/
                               },
                  row);
}

/**
 * @brief 写入模型行数据
 * @param model 数据模型指针
 * @param columns 列数据列表
 * @param row 行号，-1表示追加
 * @details 将数据写入模型的指定行
 */
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

/**
 * @brief 显示表格数据槽函数
 * @details 根据点击的按钮切换显示对应的数据模型
 */
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
 * @brief 添加雷达数据
 * @param data 雷达性能参数
 * @details 调用addDataImpl添加数据
 */
void RZSourceRadiation::addData(const RadarPerformancePara &data)
{
    addDataImpl(data);
}

/**
 * @brief 添加控制数据
 * @param data 态势控制数据
 * @details 调用addControlDataImpl添加数据
 */
void RZSourceRadiation::addData(const SituationControlData &data)
{
    addControlDataImpl(data);
}

/**
 * @brief 更新雷达数据
 * @param data 雷达性能参数
 * @details 调用updateDataImpl更新数据
 */
void RZSourceRadiation::updateData(const RadarPerformancePara &data)
{
    updateDataImpl(data);
}

/**
 * @brief 更新控制数据
 * @param data 态势控制数据
 * @details 调用updateControlDataImpl更新数据
 */
void RZSourceRadiation::updateData(const SituationControlData &data)
{
    updateControlDataImpl(data);
}

/**
 * @brief 删除数据
 * @param name 名称
 * @details 调用deleteDataByName删除数据
 */
void RZSourceRadiation::deleteData(const QString &name)
{
    deleteDataByName(name);
}

/**
 * @brief 添加雷达数据实现
 * @param data 雷达性能参数
 * @details 添加数据到列表并更新显示
 */
void RZSourceRadiation::addDataImpl(const RadarPerformancePara &data)
{
    m_radarSource.append(data);
    displayData(data);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
}

/**
 * @brief 更新雷达数据实现
 * @param data 雷达性能参数
 * @details 根据雷达名称查找并更新数据
 */
void RZSourceRadiation::updateDataImpl(const RadarPerformancePara &data)
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
 * @brief 添加控制数据实现
 * @param data 态势控制数据
 * @details 添加到控制数据列表
 */
void RZSourceRadiation::addControlDataImpl(const SituationControlData &data)
{
    m_controlData.append(data);
}

/**
 * @brief 更新控制数据实现
 * @param data 态势控制数据
 * @details 根据类型查找并更新数据
 */
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

/**
 * @brief 根据名称删除数据
 * @param name 雷达名称
 * @details 从列表和模型中删除指定雷达
 */
void RZSourceRadiation::deleteDataByName(const QString &name)
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
 * @brief 根据类型删除控制数据
 * @param type 数据类型
 * @details 从控制数据列表中删除
 */
void RZSourceRadiation::deleteControlDataByType(const QString &type)
{
    const int row = findIndexByType(m_controlData, type);
    if (row < 0)
    {
        return;
    }
    m_controlData.removeAt(row);
}

/**
 * @brief 添加雷达槽函数
 * @details 弹出对话框获取雷达信息并添加
 */
void RZSourceRadiation::onAddRadar()
{
    QString name = QInputDialog::getText(this, "添加雷达", "雷达名称:");
    if (name.isEmpty()) return;

    QString frequency = QInputDialog::getText(this, "添加雷达", "工作频率 (如 5.2~6.1GHz):");
    QString prf = QInputDialog::getText(this, "添加雷达", "脉冲重频 (如 200~500Hz):");
    QString pulseWidth = QInputDialog::getText(this, "添加雷达", "脉宽 (如 0.5~25μs):");
    QString scanMode = QInputDialog::getText(this, "添加雷达", "扫描方式:");
    QString threatLevel = QInputDialog::getText(this, "添加雷达", "威胁等级 (高/中/低):");
    QString deviceType = QInputDialog::getText(this, "添加雷达", "设备类型:");

    RadarPerformancePara radar;
    radar.name = name;
    radar.scanMode = scanMode;
    radar.threatLevel = threatLevel;
    radar.deviceType = deviceType;

    if (frequency.contains("~")) {
        QStringList parts = frequency.split("~");
        if (parts.size() == 2) {
            radar.freqMin = parts[0].replace("GHz", "").toDouble();
            radar.freqMax = parts[1].replace("GHz", "").toDouble();
        }
    } else {
        radar.freqMin = radar.freqMax = frequency.replace("GHz", "").toDouble();
    }

    if (prf.contains("~")) {
        QStringList parts = prf.split("~");
        if (parts.size() == 2) {
            radar.prfMin = parts[0].replace("Hz", "").toDouble();
            radar.prfMax = parts[1].replace("Hz", "").toDouble();
        }
    } else {
        radar.prfMin = radar.prfMax = prf.replace("Hz", "").toDouble();
    }

    if (pulseWidth.contains("~")) {
        QStringList parts = pulseWidth.split("~");
        if (parts.size() == 2) {
            radar.pwMin = parts[0].replace("μs", "").toDouble();
            radar.pwMax = parts[1].replace("μs", "").toDouble();
        }
    } else {
        radar.pwMin = radar.pwMax = pulseWidth.replace("μs", "").toDouble();
    }

    addDataImpl(radar);
}

/**
 * @brief 编辑雷达槽函数
 * @param row 待编辑的行号
 * @details 弹出对话框修改雷达信息并更新显示
 */
void RZSourceRadiation::onEditRadar(int row)
{
    if (row < 0 || row >= m_radarSource.size()) return;

    RadarPerformancePara &radar = m_radarSource[row];

    QString name = QInputDialog::getText(this, "编辑雷达", "雷达名称:", QLineEdit::Normal, radar.name);
    if (name.isEmpty()) return;

    QString frequency = QInputDialog::getText(this, "编辑雷达", "工作频率 (如 5.2~6.1GHz):", QLineEdit::Normal, QString("%1~%2GHz").arg(radar.freqMin).arg(radar.freqMax));
    QString prf = QInputDialog::getText(this, "编辑雷达", "脉冲重频 (如 200~500Hz):", QLineEdit::Normal, QString("%1~%2Hz").arg(radar.prfMin).arg(radar.prfMax));
    QString pulseWidth = QInputDialog::getText(this, "编辑雷达", "脉宽 (如 0.5~25μs):", QLineEdit::Normal, QString("%1~%2μs").arg(radar.pwMin).arg(radar.pwMax));
    QString scanMode = QInputDialog::getText(this, "编辑雷达", "扫描方式:", QLineEdit::Normal, radar.scanMode);

    if (frequency.contains("~")) {
        QStringList parts = frequency.split("~");
        if (parts.size() == 2) {
            radar.freqMin = parts[0].replace("GHz", "").toDouble();
            radar.freqMax = parts[1].replace("GHz", "").toDouble();
        }
    } else {
        radar.freqMin = radar.freqMax = frequency.replace("GHz", "").toDouble();
    }

    if (prf.contains("~")) {
        QStringList parts = prf.split("~");
        if (parts.size() == 2) {
            radar.prfMin = parts[0].replace("Hz", "").toDouble();
            radar.prfMax = parts[1].replace("Hz", "").toDouble();
        }
    } else {
        radar.prfMin = radar.prfMax = prf.replace("Hz", "").toDouble();
    }

    if (pulseWidth.contains("~")) {
        QStringList parts = pulseWidth.split("~");
        if (parts.size() == 2) {
            radar.pwMin = parts[0].replace("μs", "").toDouble();
            radar.pwMax = parts[1].replace("μs", "").toDouble();
        }
    } else {
        radar.pwMin = radar.pwMax = pulseWidth.replace("μs", "").toDouble();
    }

    radar.name = name;
    radar.scanMode = scanMode;

    displayData(radar, row);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();

    emit radarDataChanged(radar);
}

/**
 * @brief 删除雷达槽函数
 * @param row 待删除的行号
 * @details 弹出确认对话框，删除雷达数据并发送删除信号
 */
void RZSourceRadiation::onDeleteRadar(int row)
{
    if (row < 0 || row >= m_radarSource.size()) return;

    QString name = m_radarSource[row].name;
    if (QMessageBox::question(this, "删除雷达", QString("确定要删除雷达 %1 吗？").arg(name)) == QMessageBox::Yes) {
        m_radarSource.removeAt(row);
        if (auto *model = m_mapModel.value(QString::fromUtf8("雷达"), nullptr); model != nullptr) {
            model->removeRow(row);
        }

        emit radarDataDeleted(name);
    }
}

/**
 * @brief 雷达数据更新处理槽函数
 * @param data 雷达性能参数
 * @details 查找并更新数据，如不存在则添加
 */
void RZSourceRadiation::onRadarDataUpdated(const RadarPerformancePara &data)
{
    int row = findIndexByName(m_radarSource, data.name);

    if (row < 0) {
        addDataImpl(data);
    } else {
        m_radarSource[row] = data;
        displayData(data, row);
        ui->tableView->resizeColumnsToContents();
        ui->tableView->resizeRowsToContents();
    }
}

/**
 * @brief 雷达数据删除处理槽函数
 * @param name 雷达名称
 * @details 调用deleteDataByName删除数据
 */
void RZSourceRadiation::onRadarDataRemoved(const QString &name)
{
    deleteDataByName(name);
}



