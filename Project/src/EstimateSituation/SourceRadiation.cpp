//
// Created by admin on "2026.04.08 T 15:03:39".
//

#include "SourceRadiation.h"
#include "ui_SourceRadiation.h"
#include <QStandardItemModel>
#include <QBrush>
#include <QColor>
#include <QFont>
#include <QHeaderView>
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QStyledItemDelegate>
#include <QMenu>
#include <QAction>

class ThreatLevelDelegate : public QStyledItemDelegate
{
public:
    explicit ThreatLevelDelegate(QObject *parent = nullptr)
        : QStyledItemDelegate(parent)
    {
    }

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        QString value = index.data().toString();
        if (value.isEmpty())
        {
            QStyledItemDelegate::paint(painter, option, index);
            return;
        }

        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);

        // 根据威胁等级设置颜色
        QColor bgColor = getThreatLevelColor(value);

        // 绘制椭圆背景
        QRect rect = option.rect;
        int ellipseHeight = qMin(20, rect.height() - 4);
        QRect ellipseRect(rect.x() + 5, rect.y() + (rect.height() - ellipseHeight) / 2,
                          rect.width() - 10, ellipseHeight);

        painter->setBrush(bgColor);
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(ellipseRect);

        // 绘制文本
        painter->setPen(QColor(255, 255, 255));
        QFont font = option.font;
        font.setPointSize(10);
        painter->setFont(font);
        painter->drawText(ellipseRect, Qt::AlignCenter, value);

        painter->restore();
    }

private:
    // 获取威胁等级对应的颜色
    QColor getThreatLevelColor(const QString &level) const
    {
        if (level == "高")
        {
            return QColor(139, 0, 0, 180);
        }
        else if (level == "中")
        {
            return QColor(218, 165, 32, 180);
        }
        else
        {
            return QColor(100, 100, 100, 180);
        }
    }
};

SourceRadiation::SourceRadiation(QWidget *parent)
    : QWidget(parent), ui(new Ui::SourceRadiation), m_currentModel(nullptr)
{
    ui->setupUi(this);
    initUI();
    setupTable();
    loadData(SourceType::Radar);
}

SourceRadiation::~SourceRadiation()
{
    delete ui;
}

void SourceRadiation::addData(const RadarSourceGenericData &data)
{
    if (!m_currentModel)
    {
        return;
    }

    int row = m_currentModel->rowCount();
    m_currentModel->insertRow(row);

    // 设置名称
    m_currentModel->setItem(row, 0, new QStandardItem(data.name));

    // 设置其他数据值
    for (int i = 0; i < data.values.size() && i < 5; ++i)
    {
        m_currentModel->setItem(row, i + 1, new QStandardItem(data.values[i]));
    }
}

void SourceRadiation::addRadarData(const SourceData &data)
{
    m_radarData.append(data);
    if (m_currentModel)
    {
        addTableRow(m_currentModel, m_currentModel->rowCount(), data);
    }
}

void SourceRadiation::addRadioData(const SourceData &data)
{
    m_radioData.append(data);
    if (m_currentModel)
    {
        addTableRow(m_currentModel, m_currentModel->rowCount(), data);
    }
}

void SourceRadiation::addCommJammingData(const SourceData &data)
{
    m_commJamData.append(data);
    if (m_currentModel)
    {
        addTableRow(m_currentModel, m_currentModel->rowCount(), data);
    }
}

void SourceRadiation::addRadarJammingData(const SourceData &data)
{
    m_radarJamData.append(data);
    if (m_currentModel)
    {
        addTableRow(m_currentModel, m_currentModel->rowCount(), data);
    }
}

// 批量添加数据，提高性能
void SourceRadiation::addRadarDataBatch(const QList<SourceData> &dataList)
{
    m_radarData.append(dataList);
    if (m_currentModel)
    {
        // 批量插入，减少模型更新次数
        int startRow = m_currentModel->rowCount();
        for (int i = 0; i < dataList.size(); ++i)
        {
            addTableRow(m_currentModel, startRow + i, dataList[i]);
        }
    }
}

// 批量添加电台数据
void SourceRadiation::addRadioDataBatch(const QList<SourceData> &dataList)
{
    m_radioData.append(dataList);
    if (m_currentModel)
    {
        int startRow = m_currentModel->rowCount();
        for (int i = 0; i < dataList.size(); ++i)
        {
            addTableRow(m_currentModel, startRow + i, dataList[i]);
        }
    }
}

void SourceRadiation::removeData(int row)
{
    if (!m_currentModel || row < 0 || row >= m_currentModel->rowCount())
    {
        return;
    }

    m_currentModel->removeRow(row);
}

void SourceRadiation::updateData(int row, const SourceData &data)
{
    if (!m_currentModel || row < 0 || row >= m_currentModel->rowCount())
    {
        return;
    }

    m_currentModel->setItem(row, 0, new QStandardItem(data.name));
    m_currentModel->setItem(row, 1, new QStandardItem(data.frequency));
    m_currentModel->setItem(row, 2, new QStandardItem(data.repetition));
    m_currentModel->setItem(row, 3, new QStandardItem(data.pulseWidth));
    m_currentModel->setItem(row, 4, new QStandardItem(data.scanMode));
    m_currentModel->setItem(row, 5, new QStandardItem(data.threatLevel));
}

void SourceRadiation::onCustomContextMenuRequested(const QPoint &pos)
{
    QModelIndex index = ui->tableView->indexAt(pos);
    if (!index.isValid())
    {
        return;
    }

    QMenu contextMenu(this);

    // 添加编辑菜单项
    QAction *editAction = contextMenu.addAction("编辑");
    connect(editAction, &QAction::triggered, [this, index]()
    {
        // 启用编辑模式
        ui->tableView->edit(index);
    });

    // 添加删除菜单项
    QAction *deleteAction = contextMenu.addAction("删除");
    connect(deleteAction, &QAction::triggered, [this, index]()
    {
        removeData(index.row());
    });

    contextMenu.exec(ui->tableView->viewport()->mapToGlobal(pos));
}

void SourceRadiation::initUI()
{
    connect(ui->radarButton, &QPushButton::clicked, this, &SourceRadiation::onRadarClicked);
    connect(ui->radioButton, &QPushButton::clicked, this, &SourceRadiation::onRadioClicked);
    connect(ui->comJamButton, &QPushButton::clicked, this, &SourceRadiation::onCommJammingClicked);
    connect(ui->radarJamButton, &QPushButton::clicked, this, &SourceRadiation::onRadarJammingClicked);

    // 启用右键菜单
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView, &QTableView::customContextMenuRequested, this,
            &SourceRadiation::onCustomContextMenuRequested);
}

void SourceRadiation::setupTable()
{
    QHeaderView *headerView = ui->tableView->horizontalHeader();
    headerView->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    headerView->setSectionResizeMode(QHeaderView::ResizeToContents); // 自动调整列宽以适应内容

    ui->tableView->setShowGrid(false);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);
    ui->tableView->verticalHeader()->setVisible(false);

    ThreatLevelDelegate *delegate = new ThreatLevelDelegate(this);
    ui->tableView->setItemDelegateForColumn(5, delegate);
}

void SourceRadiation::loadData(SourceType type)
{
    if (m_currentModel)
    {
        m_currentModel->deleteLater();
    }

    ColumnConfig config = getColumnConfig(type);
    m_currentModel = new QStandardItemModel(0, config.headers.size(), this);

    for (int i = 0; i < config.headers.size(); ++i)
    {
        m_currentModel->setHeaderData(i, Qt::Horizontal, config.headers[i]);
    }

    switch (type)
    {
        case SourceType::Radar:
            fillRadarData(m_currentModel);
            break;
        case SourceType::Radio:
            fillRadioData(m_currentModel);
            break;
        case SourceType::CommJamming:
            fillCommJammingData(m_currentModel);
            break;
        case SourceType::RadarJamming:
            fillRadarJammingData(m_currentModel);
            break;
    }

    ui->tableView->setModel(m_currentModel);
    // 列宽会自动调整，不需要手动设置

    setActiveButton(type);
}

void SourceRadiation::setActiveButton(SourceType type)
{
    // 移除样式表设置，使用默认样式
    ui->radarButton->setDefault(type == SourceType::Radar);
    ui->radioButton->setDefault(type == SourceType::Radio);
    ui->comJamButton->setDefault(type == SourceType::CommJamming);
    ui->radarJamButton->setDefault(type == SourceType::RadarJamming);
}

ColumnConfig SourceRadiation::getColumnConfig(SourceType type) const
{
    ColumnConfig config;

    switch (type)
    {
        case SourceType::Radar:
            config.headers = QStringList({"目标名称", "工作频率", "脉冲重频", "脉宽", "扫描方式", "威胁等级"});
            break;
        case SourceType::Radio:
            config.headers = QStringList({"目标名称", "工作频率", "调制方式", "码速率", "功率/波形", "威胁等级"});
            break;
        case SourceType::CommJamming:
            config.headers = QStringList({"目标名称", "干扰样式", "覆盖频段", "有效辐射功率", "类型", "威胁等级"});
            break;
        case SourceType::RadarJamming:
            config.headers = QStringList({"目标名称", "对抗类型", "工作频段", "技术体制", "类型", "威胁等级"});
            break;
    }

    return config;
}

void SourceRadiation::fillRadarData(QStandardItemModel *model)
{
    // 缓存雷达数据
    m_radarData.clear();

    SourceData radar1 = {
            "AN/MPQ-53 相控阵雷达",
            "5.2-6.1GHz",
            "200-500Hz",
            "0.5-25μs",
            "电子扫描",
            "高"
    };
    m_radarData.append(radar1);

    SourceData radar2 = {
            "P-18 预警雷达",
            "150-170MHz",
            "300Hz",
            "8μs",
            "6rpm",
            "中"
    };
    m_radarData.append(radar2);

    SourceData radar3 = {
            "MPQ-64 哨兵雷达",
            "8-12GHz",
            "可变",
            "-",
            "旋转扫描",
            "高"
    };
    m_radarData.append(radar3);

    // 添加到模型中
    for (int i = 0; i < m_radarData.size(); ++i)
    {
        addTableRow(model, i, m_radarData[i]);
    }
}

void SourceRadiation::fillRadioData(QStandardItemModel *model)
{
    // 缓存电台数据
    m_radioData.clear();

    SourceData radio1 = {
            "Link-16 战术数据链",
            "960-1215MHz",
            "MSK/扩频",
            "1.0Mbps",
            "JTIDS",
            "高"
    };
    m_radioData.append(radio1);

    SourceData radio2 = {
            "VHF 战术电台",
            "30-88MHz",
            "FM/跳频",
            "-",
            "25W",
            "中"
    };
    m_radioData.append(radio2);

    SourceData radio3 = {
            "卫星通信终端",
            "Ku 波段",
            "QPSK",
            "5Msps",
            "undefined",
            "中"
    };
    m_radioData.append(radio3);

    // 添加到模型中
    for (int i = 0; i < m_radioData.size(); ++i)
    {
        addTableRow(model, i, m_radioData[i]);
    }
}

void SourceRadiation::fillCommJammingData(QStandardItemModel *model)
{
    // 缓存通信对抗数据
    m_commJamData.clear();

    SourceData commJam1 = {
            "R-3302h 通信干扰系统",
            "噪声调频/梳状谱",
            "20-100MHz",
            "1kW",
            "大功率宽带压制",
            "高"
    };
    m_commJamData.append(commJam1);

    SourceData commJam2 = {
            "便携式通信干扰机",
            "单音/扫频",
            "400-470MHz",
            "50W",
            "近距离战术干扰",
            "低"
    };
    m_commJamData.append(commJam2);

    SourceData commJam3 = {
            "车载智能干扰站",
            "协议伪造/随机脉冲",
            "225-400MHz",
            "200W",
            "自适应干扰",
            "中"
    };
    m_commJamData.append(commJam3);

    // 添加到模型中
    for (int i = 0; i < m_commJamData.size(); ++i)
    {
        addTableRow(model, i, m_commJamData[i]);
    }
}

void SourceRadiation::fillRadarJammingData(QStandardItemModel *model)
{
    // 缓存雷达对抗数据
    m_radarJamData.clear();

    SourceData radarJam1 = {
            "SPECTRAL 侦察干扰吊舱",
            "距离门拖引/速度欺骗",
            "2-18GHz",
            "DRFM 转发",
            "先进数字射频存储",
            "高"
    };
    m_radarJamData.append(radarJam1);

    SourceData radarJam2 = {
            "Pelena-1 地面干扰站",
            "噪声压制/假目标",
            "8-12GHz",
            "模拟转发",
            "火控雷达对抗",
            "高"
    };
    m_radarJamData.append(radarJam2);

    SourceData radarJam3 = {
            "战术侦察/干扰模块",
            "测频/测向 + 间歇采样",
            "S/C 波段",
            "数字接收",
            "小型化电子攻击",
            "中"
    };
    m_radarJamData.append(radarJam3);

    // 添加到模型中
    for (int i = 0; i < m_radarJamData.size(); ++i)
    {
        addTableRow(model, i, m_radarJamData[i]);
    }
}

void SourceRadiation::addTableRow(QStandardItemModel *model, int row, const SourceData &data)
{
    model->insertRow(row);
    model->setItem(row, 0, new QStandardItem(data.name));
    model->setItem(row, 1, new QStandardItem(data.frequency));
    model->setItem(row, 2, new QStandardItem(data.repetition));
    model->setItem(row, 3, new QStandardItem(data.pulseWidth));
    model->setItem(row, 4, new QStandardItem(data.scanMode));
    model->setItem(row, 5, new QStandardItem(data.threatLevel));
}

void SourceRadiation::onRadarClicked()
{
    loadData(SourceType::Radar);
}

void SourceRadiation::onRadioClicked()
{
    loadData(SourceType::Radio);
}

void SourceRadiation::onCommJammingClicked()
{
    loadData(SourceType::CommJamming);
}

void SourceRadiation::onRadarJammingClicked()
{
    loadData(SourceType::RadarJamming);
}
