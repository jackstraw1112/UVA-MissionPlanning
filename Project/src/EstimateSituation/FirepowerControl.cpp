//
// Created by admin on "2026.04.09 T 19:19:20".
//

#include "FirepowerControl.h"
#include "ui_FirepowerControl.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QLabel>
#include <QProgressBar>

/**
 * @brief 构造函数
 * @param parent 父窗口指针
 */
FirepowerControl::FirepowerControl(QWidget *parent)
    : QWidget(parent), ui(new Ui::FirepowerControl)
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
 * @brief 初始化参数
 * @details 预留扩展
 */
void FirepowerControl::initParams()
{
}

/**
 * @brief 初始化对象
 * @details 数据、视图
 */
void FirepowerControl::initObject()
{
    // 生成测试数据
    generateTestData();

    // 显示数据
    displayData();
}

/**
 * @brief 关联信号与槽函数
 */
void FirepowerControl::initConnect()
{
}

/**
 * @brief 析构函数
 */
FirepowerControl::~FirepowerControl()
{
    delete ui;
}

/**
 * @brief 生成测试数据
 * @details 生成防空火力测试数据
 */
void FirepowerControl::generateTestData()
{
    // 清空数据列表
    m_firepowerData.clear();

    // ---------- 1. 红旗-16 防空系统 ----------
    FirepowerItem firepower1;
    firepower1.name = QString::fromUtf8("红旗-16 防空系统");
    firepower1.status = QString::fromUtf8("active");
    firepower1.type = QString::fromUtf8("中程防空");
    firepower1.range = QString::fromUtf8("40km");
    firepower1.ammo = QString::fromUtf8("16枚");
    firepower1.response = QString::fromUtf8("8s");
    firepower1.intercept = QString::fromUtf8("95%");
    m_firepowerData.append(firepower1);

    // ---------- 2. PGZ-07 自行高炮 ----------
    FirepowerItem firepower2;
    firepower2.name = QString::fromUtf8("PGZ-07 自行高炮");
    firepower2.status = QString::fromUtf8("standby");
    firepower2.type = QString::fromUtf8("末端防御");
    firepower2.range = QString::fromUtf8("3km");
    firepower2.ammo = QString::fromUtf8("400发");
    firepower2.response = QString::fromUtf8("3s");
    firepower2.intercept = QString::fromUtf8("85%");
    m_firepowerData.append(firepower2);

    // ---------- 3. 道尔-M1 防空车 ----------
    FirepowerItem firepower3;
    firepower3.name = QString::fromUtf8("道尔-M1 防空车");
    firepower3.status = QString::fromUtf8("active");
    firepower3.type = QString::fromUtf8("低空防御");
    firepower3.range = QString::fromUtf8("15km");
    firepower3.ammo = QString::fromUtf8("8枚");
    firepower3.response = QString::fromUtf8("5s");
    firepower3.intercept = QString::fromUtf8("90%");
    m_firepowerData.append(firepower3);

    // ---------- 4. 便携式防空导弹 ----------
    FirepowerItem firepower4;
    firepower4.name = QString::fromUtf8("便携式防空导弹");
    firepower4.status = QString::fromUtf8("standby");
    firepower4.type = QString::fromUtf8("单兵武器");
    firepower4.range = QString::fromUtf8("5km");
    firepower4.ammo = QString::fromUtf8("2枚");
    firepower4.response = QString::fromUtf8("10s");
    firepower4.intercept = QString::fromUtf8("80%");
    m_firepowerData.append(firepower4);

    // ---------- 5. 电子对抗系统 ----------
    FirepowerItem firepower5;
    firepower5.name = QString::fromUtf8("电子对抗系统");
    firepower5.status = QString::fromUtf8("disabled");
    firepower5.type = QString::fromUtf8("软杀伤");
    firepower5.range = QString::fromUtf8("20km");
    firepower5.ammo = QString::fromUtf8("压制/欺骗");
    firepower5.response = QString::fromUtf8("-");
    firepower5.intercept = QString::fromUtf8("-");
    m_firepowerData.append(firepower5);
}

/**
 * @brief 显示数据
 * @details 将防空火力数据显示到界面中
 */
void FirepowerControl::displayData()
{
    // 清空布局
    QLayoutItem *item;
    while ((item = ui->verticalLayout_3->takeAt(0)) != nullptr)
    {
        delete item->widget();
        delete item;
    }

    // 设置主布局间距，进一步缩小布局
    ui->verticalLayout_3->setSpacing(1);

    // 添加所有设备项
    for (const auto &firepower : m_firepowerData)
    {
        addFirepowerItem(firepower);
    }

    // // 更新单位数量标签
    // if (ui->unitCountLabel)
    // {
    //     ui->unitCountLabel->setText(QString::fromUtf8("%1个单位").arg(m_firepowerData.size()));
    // }
}

/**
 * @brief 添加设备项
 * @details 根据设备数据创建并添加设备项到界面
 * @param item 设备数据
 */
void FirepowerControl::addFirepowerItem(const FirepowerItem &item)
{
    // 创建设备项框架
    QFrame *firepowerItem = new QFrame;
    firepowerItem->setObjectName("firepowerItem");
    QVBoxLayout *itemLayout = new QVBoxLayout(firepowerItem);
    itemLayout->setContentsMargins(0, 0, 0, 0);
    itemLayout->setSpacing(1); // 进一步减小间距

    // 设备头部
    QWidget *headerWidget = new QWidget;
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(1); // 进一步减小间距

    // 设备名称
    QLabel *nameLabel = new QLabel(item.name);
    nameLabel->setProperty("name", "nameLabel");
    nameLabel->setMaximumWidth(150);
    QFont nameFont = nameLabel->font();
    nameFont.setPointSize(3); // 进一步缩小字体大小以显示更多模块
    nameLabel->setFont(nameFont);

    // 设备状态
    QLabel *statusLabel = new QLabel;
    statusLabel->setProperty("name", "statusLabel");
    statusLabel->setFixedWidth(60);
    QFont statusFont = statusLabel->font();
    statusFont.setPointSize(3); // 进一步缩小字体大小以显示更多模块
    statusLabel->setFont(statusFont);

    if (item.status == QString::fromUtf8("active"))
    {
        statusLabel->setText(QString::fromUtf8("在线"));
        statusLabel->setStyleSheet("QLabel { background-color: #28a745; color: white; border-radius: 4px; padding: 2px 8px; text-align: center; }");
    }
    else if (item.status == QString::fromUtf8("standby"))
    {
        statusLabel->setText(QString::fromUtf8("待机"));
        statusLabel->setStyleSheet("QLabel { background-color: #ffc107; color: black; border-radius: 4px; padding: 2px 8px; text-align: center; }");
    }
    else
    {
        statusLabel->setText(QString::fromUtf8("离线"));
        statusLabel->setStyleSheet("QLabel { background-color: #6c757d; color: white; border-radius: 4px; padding: 2px 8px; text-align: center; }");
    }

    headerLayout->addWidget(nameLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(statusLabel);

    itemLayout->addWidget(headerWidget);

    // 参数网格
    QGridLayout *paramLayout = new QGridLayout;
    paramLayout->setSpacing(1);
    paramLayout->setColumnStretch(0, 1);
    paramLayout->setColumnStretch(1, 1);

    // 类型
    QFrame *typeBox = new QFrame;
    QVBoxLayout *typeBoxLayout = new QVBoxLayout(typeBox);
    typeBoxLayout->setContentsMargins(0, 0, 0, 0);
    typeBoxLayout->setSpacing(1); // 减小间距
    QLabel *typeLabel = new QLabel(QString::fromUtf8("类型"));
    QFont labelFont = typeLabel->font();
    labelFont.setPointSize(3); // 进一步缩小字体大小以显示更多模块
    typeLabel->setFont(labelFont);
    QLabel *typeValue = new QLabel(item.type);
    typeValue->setProperty("name", "typeValue");
    QFont valueFont = typeValue->font();
    valueFont.setPointSize(3); // 进一步缩小字体大小以显示更多模块
    typeValue->setFont(valueFont);
    typeBoxLayout->addWidget(typeLabel);
    typeBoxLayout->addWidget(typeValue);

    // 射程
    QFrame *rangeBox = new QFrame;
    QVBoxLayout *rangeBoxLayout = new QVBoxLayout(rangeBox);
    rangeBoxLayout->setContentsMargins(0, 0, 0, 0);
    rangeBoxLayout->setSpacing(1); // 减小间距
    QLabel *rangeLabel = new QLabel(QString::fromUtf8("射程"));
    rangeLabel->setFont(labelFont);
    QLabel *rangeValue = new QLabel(item.range);
    rangeValue->setProperty("name", "rangeValue");
    rangeValue->setFont(valueFont);
    rangeBoxLayout->addWidget(rangeLabel);
    rangeBoxLayout->addWidget(rangeValue);

    // 弹药/模式
    QFrame *ammoBox = new QFrame;
    QVBoxLayout *ammoBoxLayout = new QVBoxLayout(ammoBox);
    ammoBoxLayout->setContentsMargins(0, 0, 0, 0);
    ammoBoxLayout->setSpacing(1); // 减小间距
    QLabel *ammoLabel = new QLabel(item.ammo.contains(QString::fromUtf8("枚")) ? QString::fromUtf8("弹药") : QString::fromUtf8("模式"));
    ammoLabel->setFont(labelFont);
    QLabel *ammoValue = new QLabel(item.ammo);
    ammoValue->setProperty("name", "ammoValue");
    ammoValue->setFont(valueFont);
    ammoBoxLayout->addWidget(ammoLabel);
    ammoBoxLayout->addWidget(ammoValue);

    // 响应
    QFrame *responseBox = new QFrame;
    QVBoxLayout *responseBoxLayout = new QVBoxLayout(responseBox);
    responseBoxLayout->setContentsMargins(0, 0, 0, 0);
    responseBoxLayout->setSpacing(1); // 减小间距
    QLabel *responseLabel = new QLabel(QString::fromUtf8("响应"));
    responseLabel->setFont(labelFont);
    QLabel *responseValue = new QLabel(item.response);
    responseValue->setProperty("name", "responseValue");
    responseValue->setFont(valueFont);
    responseBoxLayout->addWidget(responseLabel);
    responseBoxLayout->addWidget(responseValue);

    paramLayout->addWidget(typeBox, 0, 0);
    paramLayout->addWidget(rangeBox, 0, 1);
    paramLayout->addWidget(ammoBox, 1, 0);
    paramLayout->addWidget(responseBox, 1, 1);

    itemLayout->addLayout(paramLayout);

    // 拦截概率
    QFrame *rangeFrame = new QFrame;
    QVBoxLayout *rangeFrameLayout = new QVBoxLayout(rangeFrame);
    rangeFrameLayout->setContentsMargins(0, 0, 0, 0);
    rangeFrameLayout->setSpacing(0); // 最小化间距

    QLabel *rangeTitle = new QLabel(QString::fromUtf8("拦截概率"));
    rangeTitle->setProperty("name", "rangeTitle");
    rangeTitle->setFont(labelFont);

    if (item.intercept != QString::fromUtf8("-"))
    {
        QProgressBar *interceptBar = new QProgressBar;
        interceptBar->setProperty("name", "interceptBar");
        interceptBar->setMinimum(0);
        interceptBar->setMaximum(100);
        int percent = QString(item.intercept).replace(QString::fromUtf8("%"), QString()).toInt();
        interceptBar->setValue(percent);
        interceptBar->setStyleSheet(
            "QProgressBar { border: 1px solid #444; border-radius: 8px; background: #333; height: 1px; } "
            "QProgressBar::chunk { background: #28a745; border-radius: 8px; }"
        );
        interceptBar->setTextVisible(false);

        rangeFrameLayout->addWidget(rangeTitle);
        rangeFrameLayout->addWidget(interceptBar);
    }

    itemLayout->addWidget(rangeFrame);

    // 添加到布局
    ui->verticalLayout_3->addWidget(firepowerItem);
}

/**
 * @brief 添加防空火力数据
 * @param data 防空火力对象
 * @details 追加到防空火力缓存并更新显示。
 */
void FirepowerControl::addDataImpl(const FirepowerItem &data)
{
    m_firepowerData.append(data);
    displayData();
}

/**
 * @brief 更新防空火力数据
 * @param data 防空火力对象（按 name 匹配）
 * @details 若未找到同名记录则转为新增。
 */
void FirepowerControl::updateDataImpl(const FirepowerItem &data)
{
    const int row = findIndexByName(m_firepowerData, data.name);
    if (row < 0)
    {
        addDataImpl(data);
        return;
    }
    m_firepowerData[row] = data;
    displayData();
}

/**
 * @brief 删除防空火力数据
 * @param name 目标名称
 * @details 从防空火力缓存和界面中同步删除同名记录。
 */
void FirepowerControl::deleteFirepowerDataByName(const QString &name)
{
    const int row = findIndexByName(m_firepowerData, name);
    if (row < 0)
    {
        return;
    }
    m_firepowerData.removeAt(row);
    displayData();
}
