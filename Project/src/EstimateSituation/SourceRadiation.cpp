//
// Created by admin on "2026.04.08 T 15:03:39".
//


// You may need to build the project (run Qt uic code generator) to get "ui_SourceRadiation.h" resolved

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
#include <QObject>
#include <QIcon>

// 自定义代理用于绘制圆角背景的威胁等级
class ThreatLevelDelegate : public QStyledItemDelegate {
public:
    explicit ThreatLevelDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}
    
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        QString value = index.data().toString();
        if (value.isEmpty()) return;
        
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        
        // 根据内容确定颜色
        QColor bgColor;
        if (value == "高") {
            bgColor = QColor(139, 0, 0, 180);
        } else if (value == "中") {
            bgColor = QColor(218, 165, 32, 180);
        } else {
            bgColor = QColor(100, 100, 100, 180);
        }
        
        // 绘制圆角矩形背景
        QRect rect = option.rect;
        QRect ellipseRect(rect.x() + 5, rect.y() + (rect.height() - 20) / 2, rect.width() - 10, 20);
        painter->setBrush(bgColor);
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(ellipseRect);
        
        // 绘制文字
        painter->setPen(QColor(255, 255, 255));
        QFont font = option.font;
        font.setPointSize(10);
        painter->setFont(font);
        painter->drawText(ellipseRect, Qt::AlignCenter, value);
        
        painter->restore();
    }
};

SourceRadiation::SourceRadiation(QWidget *parent)
    : QWidget(parent), ui(new Ui::SourceRadiation)
{
    ui->setupUi(this);
    
    // 初始化雷达数据（默认显示）
    initRadarData();

    // 设置列标题样式
    QHeaderView *headerView = ui->tableView->horizontalHeader();
    headerView->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    headerView->setStyleSheet("QHeaderView::section { background-color: #0f3460; color: #ffffff; padding: 8px; border: none; border-bottom: 1px solid #1e2a4a; font-size: 20px; }");

    // 设置表格属性
    ui->tableView->setShowGrid(false);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    // 为威胁等级列设置自定义代理
    ThreatLevelDelegate *delegate = new ThreatLevelDelegate(this);
    ui->tableView->setItemDelegateForColumn(5, delegate);
    
    // 设置按钮图标大小
    ui->radarButton->setIconSize(QSize(24, 24));
    ui->radioButton->setIconSize(QSize(24, 24));
    ui->comJamButton->setIconSize(QSize(24, 24));
    ui->radarJamButton->setIconSize(QSize(24, 24));
    
    // 连接按钮点击信号
    connect(ui->radarButton, &QPushButton::clicked, this, &SourceRadiation::initRadarData);
    connect(ui->radioButton, &QPushButton::clicked, this, &SourceRadiation::initRadioData);
    connect(ui->comJamButton, &QPushButton::clicked, this, &SourceRadiation::initComJamData);
    connect(ui->radarJamButton, &QPushButton::clicked, this, &SourceRadiation::initRadarJamData);
}

SourceRadiation::~SourceRadiation()
{
    delete ui;
}

// 初始化雷达数据
void SourceRadiation::initRadarData()
{
    QStandardItemModel *model = new QStandardItemModel(3, 6, this);
    
    // 第一行：AN/MPQ-53 相控阵雷达
    QStandardItem *item00 = new QStandardItem("  AN/MPQ-53 相控阵雷达");
    item00->setIcon(QIcon(":/image/radar.png"));
    model->setItem(0, 0, item00);
    model->setItem(0, 1, new QStandardItem("5.2-6.1GHz"));
    model->setItem(0, 2, new QStandardItem("200-500Hz"));
    model->setItem(0, 3, new QStandardItem("0.5-25μs"));
    model->setItem(0, 4, new QStandardItem("电子扫描"));
    model->setItem(0, 5, new QStandardItem("高"));

    // 第二行：P-18 预警雷达
    QStandardItem *item10 = new QStandardItem("  P-18 预警雷达");
    item10->setIcon(QIcon(":/image/radar.png"));
    model->setItem(1, 0, item10);
    model->setItem(1, 1, new QStandardItem("150-170MHz"));
    model->setItem(1, 2, new QStandardItem("300Hz"));
    model->setItem(1, 3, new QStandardItem("8μs"));
    model->setItem(1, 4, new QStandardItem("6rpm"));
    model->setItem(1, 5, new QStandardItem("中"));

    // 第三行：MPQ-64 哨兵雷达
    QStandardItem *item20 = new QStandardItem("  MPQ-64 哨兵雷达");
    item20->setIcon(QIcon(":/image/radar.png"));
    model->setItem(2, 0, item20);
    model->setItem(2, 1, new QStandardItem("8-12GHz"));
    model->setItem(2, 2, new QStandardItem("可变"));
    model->setItem(2, 3, new QStandardItem("-"));
    model->setItem(2, 4, new QStandardItem("旋转扫描"));
    model->setItem(2, 5, new QStandardItem("高"));

    // 设置列标题
    model->setHeaderData(0, Qt::Horizontal, "目标名称");
    model->setHeaderData(1, Qt::Horizontal, "工作频率");
    model->setHeaderData(2, Qt::Horizontal, "脉冲重频");
    model->setHeaderData(3, Qt::Horizontal, "脉宽");
    model->setHeaderData(4, Qt::Horizontal, "扫描方式");
    model->setHeaderData(5, Qt::Horizontal, "威胁等级");

    // 设置表格模型
    ui->tableView->setModel(model);

    // 隐藏行号
    ui->tableView->verticalHeader()->setVisible(false);

    // 调整列宽
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setColumnWidth(0, 470);
    ui->tableView->setColumnWidth(1, 320);
    ui->tableView->setColumnWidth(2, 240);
    ui->tableView->setColumnWidth(3, 240);
    ui->tableView->setColumnWidth(4, 200);
    ui->tableView->setColumnWidth(5, 166);
}

// 初始化电台数据
void SourceRadiation::initRadioData()
{
    QStandardItemModel *model = new QStandardItemModel(3, 6, this);
    
    // 第一行：Link-16 战术数据链
    QStandardItem *item00 = new QStandardItem("  Link-16 战术数据链");
    item00->setIcon(QIcon(":/image/link.png"));
    model->setItem(0, 0, item00);
    model->setItem(0, 1, new QStandardItem("960-1215MHz"));
    model->setItem(0, 2, new QStandardItem("MSK/扩频"));
    model->setItem(0, 3, new QStandardItem("1.0Mbps"));
    model->setItem(0, 4, new QStandardItem("JTIDS"));
    model->setItem(0, 5, new QStandardItem("高"));

    // 第二行：VHF 战术电台
    QStandardItem *item10 = new QStandardItem("  VHF 战术电台");
    item10->setIcon(QIcon(":/image/link.png"));
    model->setItem(1, 0, item10);
    model->setItem(1, 1, new QStandardItem("30-88MHz"));
    model->setItem(1, 2, new QStandardItem("FM/跳频"));
    model->setItem(1, 3, new QStandardItem("-"));
    model->setItem(1, 4, new QStandardItem("25W"));
    model->setItem(1, 5, new QStandardItem("中"));

    // 第三行：卫星通信终端
    QStandardItem *item20 = new QStandardItem("  卫星通信终端");
    item20->setIcon(QIcon(":/image/link.png"));
    model->setItem(2, 0, item20);
    model->setItem(2, 1, new QStandardItem("Ku 波段"));
    model->setItem(2, 2, new QStandardItem("QPSK"));
    model->setItem(2, 3, new QStandardItem("5Msps"));
    model->setItem(2, 4, new QStandardItem("undefined"));
    model->setItem(2, 5, new QStandardItem("中"));

    // 设置列标题
    model->setHeaderData(0, Qt::Horizontal, "目标名称");
    model->setHeaderData(1, Qt::Horizontal, "工作频率");
    model->setHeaderData(2, Qt::Horizontal, "调制方式");
    model->setHeaderData(3, Qt::Horizontal, "码速率");
    model->setHeaderData(4, Qt::Horizontal, "功率/波形");
    model->setHeaderData(5, Qt::Horizontal, "威胁等级");

    // 设置表格模型
    ui->tableView->setModel(model);

    // 隐藏行号
    ui->tableView->verticalHeader()->setVisible(false);

    // 调整列宽
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setColumnWidth(0, 470);
    ui->tableView->setColumnWidth(1, 320);
    ui->tableView->setColumnWidth(2, 240);
    ui->tableView->setColumnWidth(3, 240);
    ui->tableView->setColumnWidth(4, 200);
    ui->tableView->setColumnWidth(5, 166);
}

// 初始化通信对抗数据
void SourceRadiation::initComJamData()
{
    QStandardItemModel *model = new QStandardItemModel(3, 6, this);
    
    // 第一行：R-3302h 通信干扰系统
    QStandardItem *item00 = new QStandardItem("  R-3302h 通信干扰系统");
    item00->setIcon(QIcon(":/image/countermeasures.png"));
    model->setItem(0, 0, item00);
    model->setItem(0, 1, new QStandardItem("噪声调频/梳状谱"));
    model->setItem(0, 2, new QStandardItem("20-100MHz"));
    model->setItem(0, 3, new QStandardItem("1kW"));
    model->setItem(0, 4, new QStandardItem("大功率宽带压制"));
    model->setItem(0, 5, new QStandardItem("高"));

    // 第二行：便携式通信干扰机
    QStandardItem *item10 = new QStandardItem("  便携式通信干扰机");
    item10->setIcon(QIcon(":/image/countermeasures.png"));
    model->setItem(1, 0, item10);
    model->setItem(1, 1, new QStandardItem("单音/扫频"));
    model->setItem(1, 2, new QStandardItem("400-470MHz"));
    model->setItem(1, 3, new QStandardItem("50W"));
    model->setItem(1, 4, new QStandardItem("近距离战术干扰"));
    model->setItem(1, 5, new QStandardItem("低"));

    // 第三行：车载智能干扰站
    QStandardItem *item20 = new QStandardItem("  车载智能干扰站");
    item20->setIcon(QIcon(":/image/countermeasures.png"));
    model->setItem(2, 0, item20);
    model->setItem(2, 1, new QStandardItem("协议伪造/随机脉冲"));
    model->setItem(2, 2, new QStandardItem("225-400MHz"));
    model->setItem(2, 3, new QStandardItem("200W"));
    model->setItem(2, 4, new QStandardItem("自适应干扰"));
    model->setItem(2, 5, new QStandardItem("中"));

    // 设置列标题
    model->setHeaderData(0, Qt::Horizontal, "目标名称");
    model->setHeaderData(1, Qt::Horizontal, "干扰样式");
    model->setHeaderData(2, Qt::Horizontal, "覆盖频段");
    model->setHeaderData(3, Qt::Horizontal, "有效辐射功率");
    model->setHeaderData(4, Qt::Horizontal, "类型");
    model->setHeaderData(5, Qt::Horizontal, "威胁等级");

    // 设置表格模型
    ui->tableView->setModel(model);

    // 隐藏行号
    ui->tableView->verticalHeader()->setVisible(false);

    // 调整列宽
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setColumnWidth(0, 470);
    ui->tableView->setColumnWidth(1, 320);
    ui->tableView->setColumnWidth(2, 240);
    ui->tableView->setColumnWidth(3, 240);
    ui->tableView->setColumnWidth(4, 200);
    ui->tableView->setColumnWidth(5, 166);
}

// 初始化雷达对抗数据
void SourceRadiation::initRadarJamData()
{
    QStandardItemModel *model = new QStandardItemModel(3, 7, this);
    
    // 第一行：SPECTRAL 侦察干扰吊舱
    QStandardItem *item00 = new QStandardItem("  SPECTRAL 侦察干扰吊舱");
    item00->setIcon(QIcon(":/image/countermeasures.png"));
    model->setItem(0, 0, item00);
    model->setItem(0, 1, new QStandardItem("距离门拖引/速度欺骗"));
    model->setItem(0, 2, new QStandardItem("2-18GHz"));
    model->setItem(0, 3, new QStandardItem("DRFM 转发"));
    model->setItem(0, 4, new QStandardItem("先进数字射频存储"));
    model->setItem(0, 5, new QStandardItem("高"));
    model->setItem(0, 6, new QStandardItem("先进数字射频存储"));

    // 第二行：Pelena-1 地面干扰站
    QStandardItem *item10 = new QStandardItem("  Pelena-1 地面干扰站");
    item10->setIcon(QIcon(":/image/countermeasures.png"));
    model->setItem(1, 0, item10);
    model->setItem(1, 1, new QStandardItem("噪声压制/假目标"));
    model->setItem(1, 2, new QStandardItem("8-12GHz"));
    model->setItem(1, 3, new QStandardItem("模拟转发"));
    model->setItem(1, 4, new QStandardItem("火控雷达对抗"));
    model->setItem(1, 5, new QStandardItem("高"));
    model->setItem(1, 6, new QStandardItem("火控雷达对抗"));

    // 第三行：战术侦察/干扰模块
    QStandardItem *item20 = new QStandardItem("  战术侦察/干扰模块");
    item20->setIcon(QIcon(":/image/countermeasures.png"));
    model->setItem(2, 0, item20);
    model->setItem(2, 1, new QStandardItem("测频/测向 + 间歇采样"));
    model->setItem(2, 2, new QStandardItem("S/C 波段"));
    model->setItem(2, 3, new QStandardItem("数字接收"));
    model->setItem(2, 4, new QStandardItem("小型化电子攻击"));
    model->setItem(2, 5, new QStandardItem("中"));
    model->setItem(2, 6, new QStandardItem("小型化电子攻击"));

    // 设置列标题
    model->setHeaderData(0, Qt::Horizontal, "目标名称");
    model->setHeaderData(1, Qt::Horizontal, "对抗类型");
    model->setHeaderData(2, Qt::Horizontal, "工作频段");
    model->setHeaderData(3, Qt::Horizontal, "技术体制");
    model->setHeaderData(4, Qt::Horizontal, "威胁等级");
    model->setHeaderData(5, Qt::Horizontal, "类型");

    // 设置表格模型
    ui->tableView->setModel(model);

    // 隐藏行号
    ui->tableView->verticalHeader()->setVisible(false);

    // 调整列宽
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setColumnWidth(0, 400);
    ui->tableView->setColumnWidth(1, 280);
    ui->tableView->setColumnWidth(2, 200);
    ui->tableView->setColumnWidth(3, 200);
    ui->tableView->setColumnWidth(4, 150);
    ui->tableView->setColumnWidth(5, 200);
}
